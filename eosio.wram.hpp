#pragma once

#include <eosio/eosio.hpp>
#include <eosio.token/eosio.token.hpp>
#include <eosio.system/eosio.system.hpp>

namespace eosio {
   /**
    * The `eosio.wram` contract is a contract that allows to wrap & unwrap system RAM at 1:1 using the `ramtransfer` method.
    */
   class [[eosio::contract("eosio.wram")]] wram : public contract {
      public:
         using contract::contract;

         /**
          * Send `RAM` tokens to receive system RAM `bytes` tokens and retire the tokens.
          */
         [[eosio::on_notify("*::transfer")]]
         void on_transfer(const name from, const name to, const asset quantity, const string memo);

         /**
          * Send system RAM `bytes` to contract to issue `RAM` tokens to sender.
          */
         [[eosio::on_notify("*::ramtransfer")]]
         void on_ramtransfer(const name from, const name to, const int64_t bytes, const string memo);
   };
} /// namespace eosio
