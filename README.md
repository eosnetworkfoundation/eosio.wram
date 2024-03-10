# 💾 `eosio.wram` Contract [![Bun Test](https://github.com/eosnetworkfoundation/eosio.wram/actions/workflows/test.yml/badge.svg)](https://github.com/eosnetworkfoundation/eosio.wram/actions/workflows/test.yml)

The `eosio.wram` contract is a contract that allows to wrap & unwrap system RAM at 1:1 using the `ramtransfer` or `buyram` & `buyrambytes` methods.

## Token Contract

- Contract: [`eosio.wram`](https://bloks.io/account/eosio.wram)
- Symbol: `RAM`
- Precision: `0`

## RAM Wrapper Contract

- Deployed under `eosio.wram` contract with `eosio@owner` permissions.

## Actions

## `wrap` & `unwrap`
- **Wrap:** Send or buy system RAM `bytes` to contract to issue `RAM` tokens to sender.
- **Unwrap:** Send `RAM` tokens to receive system RAM `bytes` tokens and retire the tokens.

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
✓ eosio.wram > eosio::init [12.37ms]
✓ eosio.wram > eosio.token::issue::EOS [55.90ms]
✓ eosio.wram > eosio.token::issue::RAM [7.57ms]
✓ eosio.wram > eosio::buyrambytes [16.45ms]
✓ eosio.wram > eosio::ramtransfer [11.20ms]
✓ eosio.wram > fake::init [11.56ms]
✓ eosio.wram > fake::buyrambytes [8.94ms]
✓ eosio.wram > fake.token::issue::RAM [30.78ms]
✓ eosio.wram > on_notify::ramtransfer - wrap RAM [14.24ms]
✓ eosio.wram > on_notify::buyrambytes - wrap RAM [16.17ms]
✓ eosio.wram > transfer - unwrap RAM [14.40ms]
✓ eosio.wram > transfer - RAM to another account [11.30ms]
✓ eosio.wram > transfer::error - cannot transfer to eosio.* accounts [64.28ms]
✓ eosio.wram > transfer::error - fake eosio.token RAM [9.47ms]
✓ eosio.wram > transfer::error - fake eosio system RAM bytes [9.42ms]

 15 pass
```