#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/singleton.hpp>
#include <eosio/asset.hpp>

#include <string>

using namespace eosio;

namespace eosiosystem {

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
    void buyram( const name& payer, const name& receiver, const asset& quant );

    /**
     * Buy a specific amount of ram bytes action. Increases receiver's ram in quantity of bytes provided.
     * An inline transfer from receiver to system contract of tokens will be executed.
     *
     * @param payer - the ram buyer,
     * @param receiver - the ram receiver,
     * @param bytes - the quantity of ram to buy specified in bytes.
     */
    [[eosio::action]]
    void buyrambytes( const name& payer, const name& receiver, uint32_t bytes );

    /**
     * Sell ram action, reduces quota by bytes and then performs an inline transfer of tokens
     * to receiver based upon the average purchase price of the original quota.
     *
     * @param account - the ram seller account,
     * @param bytes - the amount of ram to sell in bytes.
     */
    [[eosio::action]]
    void sellram( const name& account, int64_t bytes );

    /**
     * Transfer ram action, reduces sender's quota by bytes and increase receiver's quota by bytes.
     *
     * @param from - the ram sender account,
     * @param to - the ram receiver account,
     * @param bytes - the amount of ram to transfer in bytes,
     * @param memo - the memo string to accompany the transaction.
     */
    [[eosio::action]]
    void ramtransfer( const name& from, const name& to, int64_t bytes, const std::string& memo );

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
    void logbuyram( const name& payer, const name& receiver, const asset& quantity, int64_t bytes, int64_t ram_bytes );

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
};
}