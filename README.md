### `eosio.wram` Contract

The `eosio.wram` contract is a contract that allows to wrap & unwrap system RAM at 1:1 using the `ramtransfer` or `buyram` & `buyrambytes` methods.

## Token Contract

- Contract: `eosio.wrap`
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
✓ eosio.wram > eosio::init [14.94ms]
✓ eosio.wram > eosio.token::issue::EOS [57.22ms]
✓ eosio.wram > eosio.token::issue::RAM [11.80ms]
✓ eosio.wram > eosio::buyrambytes [12.59ms]
✓ eosio.wram > eosio::ramtransfer [12.22ms]
✓ eosio.wram > fake::init [11.29ms]
✓ eosio.wram > fake::buyrambytes [16.20ms]
✓ eosio.wram > fake.token::issue::RAM [37.68ms]
✓ eosio.wram > on_notify::ramtransfer - wrap RAM [16.50ms]
✓ eosio.wram > on_notify::buyrambytes - wrap RAM [15.54ms]
✓ eosio.wram > transfer - unwrap RAM [12.33ms]
✓ eosio.wram > transfer::error - fake eosio.token RAM [14.85ms]
✓ eosio.wram > transfer::error - fake eosio system RAM bytes [11.49ms]

 13 pass
```