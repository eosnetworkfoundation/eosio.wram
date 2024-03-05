#include "eosio.wram.hpp"

namespace eosio {

// @user
[[eosio::on_notify("*::transfer")]]
void wram::on_transfer(const name from, const name to, const asset quantity, const string memo)
{
   // ignore transfers not sent to this contract
   if (to != get_self()) { return; }

   // validate incoming token transfer
   check(get_first_receiver() == eosiosystem::system_contract::token_account, "Only the eosio.token contract may send tokens to this contract.");
   check(quantity.symbol == eosiosystem::system_contract::ram_symbol, "Only the system RAM token is accepted for transfers.");

   // burn RAM tokens
   eosio::token::retire_action retire_act{eosiosystem::system_contract::token_account, {get_self(), "active"_n}};
   retire_act.send(quantity, "unwrap ram");

   // ramtransfer to user
   eosiosystem::system_contract::ramtransfer_action ramtransfer_act{"eosio"_n, {get_self(), "active"_n}};
   ramtransfer_act.send(get_self(), from, quantity.amount, "unwrap ram");
}

// @user
[[eosio::on_notify("*::ramtransfer")]]
void wram::on_ramtransfer(const name from, const name to, const int64_t bytes, const string memo)
{
   // ignore transfers not sent to this contract
   if (to != get_self()) { return; }

   // validate incoming RAM transfer
   check(get_first_receiver() == "eosio"_n, "Only the eosio contract may send RAM bytes to this contract.");
   check(bytes > 0, "RAM transfer must be positive.");

   // issue RAM tokens
   const asset quantity{bytes, eosiosystem::system_contract::ram_symbol};
   eosio::token::issue_action issue_act{eosiosystem::system_contract::token_account, {get_self(), "active"_n}};
   issue_act.send(get_self(), quantity, "wrap ram");

   // transfer RAM tokens to user
   eosio::token::transfer_action transfer_act{eosiosystem::system_contract::token_account, {get_self(), "active"_n}};
   transfer_act.send(get_self(), from, quantity, "wrap ram");
}

} /// namespace eosio
