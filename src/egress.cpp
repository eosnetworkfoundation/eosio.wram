namespace eosio {
    [[eosio::action]]
    void wram::addegress( const set<name> accounts )
    {
        require_auth(get_self());

        egresslist _egresslist(get_self(), get_self().value);

        for (const name account : accounts) {
            auto itr = _egresslist.find(account.value);
            if (itr != _egresslist.end() ) continue; // skip if already exists
            _egresslist.emplace(get_self(), [&](auto& row) {
                row.account = account;
            });
        }
    }

    [[eosio::action]]
    void wram::removeegress( const set<name> accounts )
    {
        require_auth(get_self());

        egresslist _egresslist(get_self(), get_self().value);

        for (const name account : accounts) {
            auto itr = _egresslist.find(account.value);
            if (itr == _egresslist.end() ) continue; // skip if not exists
            _egresslist.erase(itr);
        }
    }

    // block transfers to any account in the egress list
    void wram::check_disable_transfer( const name receiver )
    {
        if (receiver == get_self()) { return; } // ignore self transfer (eosio.wram)

        egresslist _egresslist(get_self(), get_self().value);
        auto itr = _egresslist.find(receiver.value);
        check( itr == _egresslist.end(), "transfer disabled to account" );
    }
}