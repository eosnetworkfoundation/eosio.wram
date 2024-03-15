#include "eosio.wram.hpp"
#include "src/token.cpp"

namespace eosio {

void wram::unwrap_ram( const name to, const asset quantity )
{
   check( quantity.amount > 0, "quantity must be positive" ); // shouldn't be possible
   mirror_system_ram(); // update RAM supply

   // ramtransfer to user
   eosiosystem::system_contract::ramtransfer_action ramtransfer_act{"eosio"_n, {get_self(), "active"_n}};
   ramtransfer_act.send(get_self(), to, quantity.amount, "unwrap ram");
}

void wram::wrap_ram( const name to, const int64_t bytes )
{
   check( bytes > 0, "bytes must be positive" ); // shouldn't be possible
   mirror_system_ram(); // update RAM supply

   // transfer RAM tokens to user
   const asset quantity{bytes, RAM_SYMBOL};
   eosio::token::transfer_action transfer_act{get_self(), {get_self(), "active"_n}};
   transfer_act.send(get_self(), to, quantity, "wrap ram");
}

eosiosystem::eosio_global_state wram::get_global()
{
   eosiosystem::global_state_singleton _global( "eosio"_n, "eosio"_n.value );
   check( _global.exists(), "global state does not exist" );
   auto global = _global.get();
   return global;
}

void wram::mirror_system_ram()
{
   auto global = get_global();
   const int64_t total_ram_bytes_reserved = global.total_ram_bytes_reserved;
   const asset supply = eosio::token::get_supply(get_self(), RAM_SYMBOL.code());
   const int64_t delta = total_ram_bytes_reserved - supply.amount;

   // issue supply
   if (delta > 0) {
      eosio::token::issue_action issue_act{get_self(), {get_self(), "active"_n}};
      issue_act.send(get_self(), asset{delta, RAM_SYMBOL}, "mirror ram");

   // retire supply
   } else if (delta < 0) {
      eosio::token::retire_action retire_act{get_self(), {get_self(), "active"_n}};
      retire_act.send(asset{-delta, RAM_SYMBOL}, "mirror ram");
   }
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
   if ( memo == "ignore") { return; } // allow for internal RAM transfers
   wrap_ram(from, bytes);
}

// @user
[[eosio::on_notify("*::transfer")]]
void wram::on_transfer( const name from, const name to, const asset quantity, const string memo )
{
   // ignore transfers not sent to this contract
   if (to != get_self()) { return; }

   // validate incoming token transfer
   check(get_first_receiver() == get_self(), "Only the " + get_self().to_string() + " contract may send tokens to this contract.");
   check(quantity.symbol == RAM_SYMBOL, "Only the system " + RAM_SYMBOL.code().to_string() + " token is accepted for transfers.");
}

// block accidental transfers to eosio.* accounts
void wram::block_receiver( const name receiver )
{
   if ( receiver == get_self() ) { return; } // ignore self transfer (eosio.wram)
   check( receiver.prefix() != "eosio"_n, "cannot transfer to eosio.* accounts" );
}

} /// namespace eosio
