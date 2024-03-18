#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/singleton.hpp>
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
        reserve_ram(bytes);

        // log buy ram action
        system_contract::logbuyram_action logbuyram_act{get_self(), {get_self(), "active"_n}};
        logbuyram_act.send(payer, receiver, quant, bytes, 0);
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
        reserve_ram(bytes);

        // log buy ram action
        system_contract::logbuyram_action logbuyram_act{get_self(), {get_self(), "active"_n}};
        logbuyram_act.send(payer, receiver, asset{0, ramcore_symbol}, bytes, 0);
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
        reserve_ram(-bytes);
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

    /**
     * Logging for buyram & buyrambytes action
     *
     * @param payer - the ram buyer,
     * @param receiver - the ram receiver,
     * @param quantity - the quantity of tokens to buy ram with.
     * @param bytes - the quantity of ram to buy specified in bytes.
     * @param ram_bytes - the ram bytes held by receiver after the action.
     */
    [[eosio::action]]
    void logbuyram( const name& payer, const name& receiver, const asset& quantity, int64_t bytes, int64_t ram_bytes )
    {
        require_auth(get_self());
        require_recipient( receiver );
    }


    [[eosio::action]]
    void init()
    {
        // set ram market
        rammarket _rammarket(get_self(), get_self().value);
        auto itr = _rammarket.find(symbol("RAMCORE", 4).raw());

        if (itr == _rammarket.end()) {
            _rammarket.emplace(get_self(), [&](auto& m) {
                m.supply.amount = 100000000000000;
                m.supply.symbol = symbol("RAMCORE", 4);
                m.base.balance.amount = 129542469746;
                m.base.balance.symbol = symbol("RAM", 0);
                m.quote.balance.amount = 147223045946;
                m.quote.balance.symbol = symbol("EOS", 4);
            });
        }

        // set global state
        global_state_singleton _global(get_self(), get_self().value);
        eosio_global_state global;
        global.max_ram_size = 418945440768;
        global.total_ram_bytes_reserved = 321908101425;
        _global.set(global, get_self());
    }

    // action wrappers
    using sellram_action = eosio::action_wrapper<"sellram"_n, &system_contract::sellram>;
    using buyrambytes_action = eosio::action_wrapper<"buyrambytes"_n, &system_contract::buyrambytes>;
    using buyram_action = eosio::action_wrapper<"buyram"_n, &system_contract::buyram>;
    using ramtransfer_action = eosio::action_wrapper<"ramtransfer"_n, &system_contract::ramtransfer>;
    using logbuyram_action = eosio::action_wrapper<"logbuyram"_n, &system_contract::logbuyram>;

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

   // Defines new global state parameters.
   struct [[eosio::table("global"), eosio::contract("eosio.system")]] eosio_global_state {

      /**
       * The blockchain parameters
       * https://github.com/AntelopeIO/cdt/blob/26289ffcaa40f8531ddbeeb8c11697c3c6a70fac/libraries/eosiolib/contracts/eosio/privileged.hpp#L46C1-L167C6
       */

      /**
      * The maximum net usage in instructions for a block
      * @brief the maximum net usage in instructions for a block
      */
      uint64_t max_block_net_usage;

      /**
      * The target percent (1% == 100, 100%= 10,000) of maximum net usage; exceeding this triggers congestion handling
      * @brief The target percent (1% == 100, 100%= 10,000) of maximum net usage; exceeding this triggers congestion handling
      */
      uint32_t target_block_net_usage_pct;

      /**
      * The maximum objectively measured net usage that the chain will allow regardless of account limits
      * @brief The maximum objectively measured net usage that the chain will allow regardless of account limits
      */
      uint32_t max_transaction_net_usage;

      /**
       * The base amount of net usage billed for a transaction to cover incidentals
       */
      uint32_t base_per_transaction_net_usage;

      /**
       * The amount of net usage leeway available whilst executing a transaction (still checks against new limits without leeway at the end of the transaction)
       * @brief The amount of net usage leeway available whilst executing a transaction  (still checks against new limits without leeway at the end of the transaction)
       */
      uint32_t net_usage_leeway;

      /**
      * The numerator for the discount on net usage of context-free data
      * @brief The numerator for the discount on net usage of context-free data
      */
      uint32_t context_free_discount_net_usage_num;

      /**
      * The denominator for the discount on net usage of context-free data
      * @brief The denominator for the discount on net usage of context-free data
      */
      uint32_t context_free_discount_net_usage_den;

      /**
      * The maximum billable cpu usage (in microseconds) for a block
      * @brief The maximum billable cpu usage (in microseconds) for a block
      */
      uint32_t max_block_cpu_usage;

      /**
      * The target percent (1% == 100, 100%= 10,000) of maximum cpu usage; exceeding this triggers congestion handling
      * @brief The target percent (1% == 100, 100%= 10,000) of maximum cpu usage; exceeding this triggers congestion handling
      */
      uint32_t target_block_cpu_usage_pct;

      /**
      * The maximum billable cpu usage (in microseconds) that the chain will allow regardless of account limits
      * @brief The maximum billable cpu usage (in microseconds) that the chain will allow regardless of account limits
      */
      uint32_t max_transaction_cpu_usage;

      /**
      * The minimum billable cpu usage (in microseconds) that the chain requires
      * @brief The minimum billable cpu usage (in microseconds) that the chain requires
      */
      uint32_t min_transaction_cpu_usage;

      /**
       * Maximum lifetime of a transacton
       * @brief Maximum lifetime of a transacton
       */
      uint32_t max_transaction_lifetime;

      /**
      * The number of seconds after the time a deferred transaction can first execute until it expires
      * @brief the number of seconds after the time a deferred transaction can first execute until it expires
      */
      uint32_t deferred_trx_expiration_window;


      /**
      * The maximum number of seconds that can be imposed as a delay requirement by authorization checks
      * @brief The maximum number of seconds that can be imposed as a delay requirement by authorization checks
      */
      uint32_t max_transaction_delay;

      /**
       * Maximum size of inline action
       * @brief Maximum size of inline action
       */
      uint32_t max_inline_action_size;

      /**
       * Maximum depth of inline action
       * @brief Maximum depth of inline action
       */
      uint16_t max_inline_action_depth;

      /**
       * Maximum authority depth
       * @brief Maximum authority depth
       */
      uint16_t max_authority_depth;

      uint64_t free_ram()const { return max_ram_size - total_ram_bytes_reserved; }

      uint64_t             max_ram_size = 64ll*1024 * 1024 * 1024;
      uint64_t             total_ram_bytes_reserved = 0;
      int64_t              total_ram_stake = 0;

      block_timestamp      last_producer_schedule_update;
      time_point           last_pervote_bucket_fill;
      int64_t              pervote_bucket = 0;
      int64_t              perblock_bucket = 0;
      uint32_t             total_unpaid_blocks = 0; /// all blocks which have been produced but not paid
      int64_t              total_activated_stake = 0;
      time_point           thresh_activated_stake_time;
      uint16_t             last_producer_schedule_size = 0;
      double               total_producer_vote_weight = 0; /// the sum of all producer votes
      block_timestamp      last_name_close;
   };
   typedef eosio::singleton< "global"_n, eosio_global_state >   global_state_singleton;

    typedef eosio::multi_index< "userres"_n, user_resources >      user_resources_table;

    int64_t add_ram( const name& owner, int64_t bytes ) {
        require_recipient( owner );

        // update user resources
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

    void reserve_ram( int64_t bytes ) {
        // update global state
        global_state_singleton _global(get_self(), get_self().value);
        eosio_global_state global = _global.get();
        global.total_ram_bytes_reserved += bytes;
        _global.set(global, get_self());
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