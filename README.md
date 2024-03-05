### `eosio.wram` Contract

The `eosio.wram` contract is a contract that allows to wrap & unwrap system RAM at 1:1 using the `ramtransfer` method.

## Symbol & Contract

- symbol: `0,RAM`
- contract: `eosio.wram` & `eosio.token`
- name: `Wrapped RAM`

## Actions

## `wrap` & `unwrap`
- **Wrap:** Send system RAM `bytes` to contract to issue `RAM` tokens to sender.
- **Unwrap:** Send `RAM` tokens to receive system RAM `bytes` tokens and retire the tokens.

## Token Format

- Uses `eosio.token` system smart contract
- Uses native system contract `0,RAM` symbol.

## Build

```
$ cdt-cpp eosio.wram.cpp -I ./include
```

## Tests

```
$ npm test

> test
> bun test

bun test v1.0.26 (c75e768a)

eosio.wram.spec.ts:
✓ eosio.wram > eosio::init [18.69ms]
✓ eosio.wram > eosio.token::issue::EOS [61.51ms]
✓ eosio.wram > eosio.token::issue::RAM [10.93ms]
✓ eosio.wram > eosio::buyrambytes [13.67ms]
✓ eosio.wram > eosio::ramtransfer [12.24ms]
✓ eosio.wram > fake::init [21.58ms]
✓ eosio.wram > fake::buyrambytes [11.28ms]
✓ eosio.wram > fake.token::issue::RAM [32.20ms]
✓ eosio.wram > on_notify::ramtransfer [26.99ms]
✓ eosio.wram > transfer::error - fake eosio.token RAM [11.22ms]
✓ eosio.wram > transfer::error - fake eosio system RAM bytes [13.56ms]

 11 pass
```