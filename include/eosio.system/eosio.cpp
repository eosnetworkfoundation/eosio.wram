#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/asset.hpp>

#include <string>

using namespace eosio;

class [[eosio::contract("eosio")]] system_contract : public contract
{
public:
    using contract::contract;

    static constexpr symbol ramcore_symbol = symbol(symbol_code("RAMCORE"), 4);

    /**
     * Buy ram action, increases receiver's ram quota based upon current price and quantity of
     * tokens provided. An inline transfer from receiver to system contract of
     * tokens will be executed.
     *
     * @param payer - the ram buyer,
     * @param receiver - the ram receiver,
     * @param quant - the quantity of tokens to buy ram with.
     */
    [[eosio::action]]
    void buyram( const name& payer, const name& receiver, const asset& quant )
    {
        const int64_t bytes = bytes_cost_with_fee(quant);
        add_ram(receiver, bytes);
    }

    /**
     * Buy a specific amount of ram bytes action. Increases receiver's ram in quantity of bytes provided.
     * An inline transfer from receiver to system contract of tokens will be executed.
     *
     * @param payer - the ram buyer,
     * @param receiver - the ram receiver,
     * @param bytes - the quantity of ram to buy specified in bytes.
     */
    [[eosio::action]]
    void buyrambytes( const name& payer, const name& receiver, uint32_t bytes )
    {
        add_ram(receiver, bytes);
    }

    /**
     * Sell ram action, reduces quota by bytes and then performs an inline transfer of tokens
     * to receiver based upon the average purchase price of the original quota.
     *
     * @param account - the ram seller account,
     * @param bytes - the amount of ram to sell in bytes.
     */
    [[eosio::action]]
    void sellram( const name& account, int64_t bytes )
    {
        add_ram(account, -bytes);
    }

    /**
     * Transfer ram action, reduces sender's quota by bytes and increase receiver's quota by bytes.
     *
     * @param from - the ram sender account,
     * @param to - the ram receiver account,
     * @param bytes - the amount of ram to transfer in bytes,
     * @param memo - the memo string to accompany the transaction.
     */
    [[eosio::action]]
    void ramtransfer( const name& from, const name& to, int64_t bytes, const std::string& memo )
    {
        add_ram(from, -bytes);
        add_ram(to, bytes);
    }

    [[eosio::action]]
    void init()
    {
        rammarket _rammarket("eosio"_n, "eosio"_n.value);
        auto itr = _rammarket.find(symbol("RAMCORE", 4).raw());

        if (itr == _rammarket.end()) {
            _rammarket.emplace("eosio"_n, [&](auto& m) {
                m.supply.amount = 100000000000000;
                m.supply.symbol = symbol("RAMCORE", 4);
                m.base.balance.amount = 129542469746;
                m.base.balance.symbol = symbol("RAM", 0);
                m.quote.balance.amount = 147223045946;
                m.quote.balance.symbol = symbol("EOS", 4);
            });
        }
    }

    // action wrappers
    using sellram_action = eosio::action_wrapper<"sellram"_n, &system_contract::sellram>;
    using buyrambytes_action = eosio::action_wrapper<"buyrambytes"_n, &system_contract::buyrambytes>;
    using buyram_action = eosio::action_wrapper<"buyram"_n, &system_contract::buyram>;
    using ramtransfer_action = eosio::action_wrapper<"ramtransfer"_n, &system_contract::ramtransfer>;

    struct [[eosio::table, eosio::contract("eosio.system")]] exchange_state {
        asset    supply;
        struct connector {
            asset balance;
            double weight = .5;
        };
        connector base;
        connector quote;
        uint64_t primary_key()const { return supply.symbol.raw(); }
    };

    typedef eosio::multi_index< "rammarket"_n, exchange_state > rammarket;

    struct [[eosio::table, eosio::contract("eosio.system")]] user_resources {
        name          owner;
        asset         net_weight;
        asset         cpu_weight;
        int64_t       ram_bytes = 0;

        bool is_empty()const { return net_weight.amount == 0 && cpu_weight.amount == 0 && ram_bytes == 0; }
        uint64_t primary_key()const { return owner.value; }
    };

    typedef eosio::multi_index< "userres"_n, user_resources >      user_resources_table;

    int64_t add_ram( const name& owner, int64_t bytes ) {
        require_recipient( owner );
        user_resources_table _userres( get_self(), owner.value );
        auto res_itr = _userres.find( owner.value );

        if ( res_itr == _userres.end() ) {
            _userres.emplace( get_self(), [&]( auto& res ) {
                res.owner = owner;
                res.ram_bytes = bytes;
                res.net_weight = asset(0, symbol("EOS", 4));
                res.cpu_weight = asset(0, symbol("EOS", 4));
            });
            return bytes;
        } else {
            _userres.modify( res_itr, same_payer, [&]( auto& res ) {
                res.ram_bytes += bytes;
            });
        }
        return res_itr->ram_bytes;
    }

    int64_t get_bancor_output(int64_t inp_reserve, int64_t out_reserve, int64_t inp)
    {
        const double ib = inp_reserve;
        const double ob = out_reserve;
        const double in = inp;

        int64_t out = int64_t((in * ob) / (ib + in));

        if (out < 0)
            out = 0;

        return out;
    }

    int64_t bytes_cost_with_fee(const asset quantity)
    {
        name      system_account = "eosio"_n;
        rammarket _rammarket(system_account, system_account.value);

        const asset fee                = get_fee(quantity);
        const asset quantity_after_fee = quantity - fee;

        auto          itr         = _rammarket.find(system_contract::ramcore_symbol.raw());
        const int64_t ram_reserve = itr->base.balance.amount;
        const int64_t eos_reserve = itr->quote.balance.amount;
        const int64_t cost        = get_bancor_output(eos_reserve, ram_reserve, quantity_after_fee.amount);
        return cost;
    }

    asset get_fee(const asset quantity)
    {
        asset fee  = quantity;
        fee.amount = (fee.amount + 199) / 200; /// .5% fee (round up)
        return fee;
    }
};