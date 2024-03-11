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
✓ eosio.wram > eosio::init [16.52ms]
✓ eosio.wram > eosio.token::issue::EOS [60.84ms]
✓ eosio.wram > eosio.token::issue::WRAM [13.30ms]
✓ eosio.wram > eosio::buyrambytes [14.28ms]
✓ eosio.wram > eosio::ramtransfer [13.52ms]
✓ eosio.wram > fake::init [11.93ms]
✓ eosio.wram > fake::buyrambytes [13.37ms]
✓ eosio.wram > fake.token::issue::WRAM [34.72ms]
✓ eosio.wram > on_notify::ramtransfer - wrap RAM bytes [16.18ms]
✓ eosio.wram > on_notify::buyrambytes - wrap RAM bytes [17.23ms]
✓ eosio.wram > transfer - unwrap WRAM [14.58ms]
✓ eosio.wram > transfer - WRAM to another account [13.15ms]
✓ eosio.wram > transfer - ignore [15.75ms]
✓ eosio.wram > transfer::error - cannot transfer to eosio.* accounts [67.26ms]
✓ eosio.wram > transfer::error - fake eosio.token WRAM [22.25ms]
✓ eosio.wram > transfer::error - fake eosio system RAM bytes [15.44ms]

 16 pass
```