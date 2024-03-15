# 💾 `eosio.wram` Contract [![Bun Test](https://github.com/eosnetworkfoundation/eosio.wram/actions/workflows/test.yml/badge.svg)](https://github.com/eosnetworkfoundation/eosio.wram/actions/workflows/test.yml)

The `eosio.wram` contract is a contract that allows to wrap & unwrap system RAM bytes at 1:1 using the `ramtransfer` or `buyram` & `buyrambytes` actions.

## Token Contract

- Contract: [`eosio.wram`](https://bloks.io/account/eosio.wram)
- Symbol: `WRAM`
- Precision: `0`
- Name: `EOS Wrapped RAM`

## RAM Wrapper Contract

- Deployed under `eosio.wram` contract with `eosio@owner` permissions.

## Actions

## `wrap` & `unwrap`
- **Wrap:** Send or buy system RAM `bytes` to contract to issue `WRAM` tokens to sender.
- **Unwrap:** Send `WRAM` tokens to receive system RAM `bytes` tokens and retire the tokens.

## Notes

- Using `buyram` & `buyrambytes` to issue `WRAM` will still incur a 0.5% fee from the system.
- Using `ramtransfer` to issue `WRAM` will not incur any fee from the system.
- `eosio.*` system accounts are not allowed to receive `WRAM` tokens
  - prevents accidental transfers, which will result in loss of RAM.

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
✓ eosio.wram > eosio::init [15.94ms]
✓ eosio.wram > eosio.token::issue::EOS [60.18ms]
✓ eosio.wram > eosio.token::create::WRAM [15.35ms]
✓ eosio.wram > eosio::buyrambytes [15.70ms]
✓ eosio.wram > buyrambytes - mirror system RAM [15.11ms]
✓ eosio.wram > eosio::ramtransfer [14.78ms]
✓ eosio.wram > fake::init [10.25ms]
✓ eosio.wram > fake::buyrambytes [13.23ms]
✓ eosio.wram > fake.token::issue::WRAM [36.20ms]
✓ eosio.wram > on_notify::ramtransfer - wrap RAM bytes [15.50ms]
✓ eosio.wram > on_notify::buyrambytes - wrap RAM bytes [16.46ms]
✓ eosio.wram > transfer - unwrap WRAM [13.36ms]
✓ eosio.wram > transfer - WRAM to another account [15.32ms]
✓ eosio.wram > transfer - ignore [13.41ms]
✓ eosio.wram > transfer::error - cannot transfer to eosio.* accounts [66.78ms]
✓ eosio.wram > transfer::error - fake eosio.token WRAM [10.06ms]
✓ eosio.wram > transfer::error - fake eosio system RAM bytes [14.67ms]

 17 pass
 0 fail
 26 expect() calls
```