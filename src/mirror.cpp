namespace eosio {

eosiosystem::system_contract::eosio_global_state wram::get_global()
{
   eosiosystem::system_contract::global_state_singleton _global( "eosio"_n, "eosio"_n.value );
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

}