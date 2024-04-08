#include "eosio.wram.hpp"
#include "src/token.cpp"
#include "src/mirror.cpp"
#include "src/egress.cpp"

namespace eosio {

[[eosio::action]]
void wram::unwrap( const name owner, const int64_t bytes )
{
   transfer(owner, get_self(), asset{bytes, RAM_SYMBOL}, "unwrap ram");
}

void wram::unwrap_ram( const name to, const asset quantity )
{
   // update WRAM supply to reflect system RAM
   mirror_system_ram();

   // validate incoming token transfer
   check(quantity.symbol == RAM_SYMBOL, "Only the system " + RAM_SYMBOL.code().to_string() + " token is accepted for transfers.");

   // ramtransfer to user
   eosiosystem::system_contract::ramtransfer_action ramtransfer_act{"eosio"_n, {get_self(), "active"_n}};
   ramtransfer_act.send(get_self(), to, quantity.amount, "unwrap ram");
}

void wram::wrap_ram( const name to, const int64_t bytes )
{
   // update WRAM supply to reflect system RAM
   mirror_system_ram();

   // cannot have contract itself mint WRAM
   check(to != get_self(), "cannot wrap ram to self" );

   // transfer RAM tokens to user
   const asset quantity{bytes, RAM_SYMBOL};
   transfer_action transfer_act{get_self(), {get_self(), "active"_n}};
   transfer_act.send(get_self(), to, quantity, "wrap ram");
}

[[eosio::on_notify("eosio::logbuyram")]]
void wram::on_logbuyram( const name& payer, const name& receiver, const asset& quantity, int64_t bytes, int64_t ram_bytes )
{
   // ignore buy ram not sent to this contract
   if (receiver != get_self()) { return; }
   wrap_ram(payer, bytes);
}

// @user
[[eosio::on_notify("eosio::ramtransfer")]]
void wram::on_ramtransfer( const name from, const name to, const int64_t bytes, const string memo )
{
   // ignore transfers not sent to this contract
   if (to != get_self()) { return; }
   if (memo == "ignore") { return; } // allow for internal RAM transfers
   wrap_ram(from, bytes);
}

// @user
[[eosio::on_notify("*::transfer")]]
void wram::on_transfer( const name from, const name to, const asset quantity, const string memo )
{
   // ignore transfers not sent to this contract
   if (to != get_self()) { return; }

   // unwrap is triggered by internal transfer method
   check(false, "only " + get_self().to_string() + " token transfers are allowed");
}

} /// namespace eosio
