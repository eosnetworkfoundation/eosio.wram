# Litepaper: 💾 EOS Wrapped RAM (`eosio.wram`) [![Bun Test](https://github.com/eosnetworkfoundation/eosio.wram/actions/workflows/test.yml/badge.svg)](https://github.com/eosnetworkfoundation/eosio.wram/actions/workflows/test.yml)

## Overview

The `eosio.wram` contract is a pivotal development in the EOS ecosystem, designed to introduce a flexible mechanism for managing system RAM through a tokenization process. This light paper delves into the core functionalities, specifications, and operational nuances of the `eosio.wram` contract.

## Contract Specifications

- **Chain**: EOS Native
- **Contract Account**: `eosio.wram`
- **Token Symbol**: `WRAM`
- **Precision**: 0
- **Token Name**: EOS Wrapped RAM
- **Max Supply**: `418945440768` (418 GB)

## Security Audits

| Date | Security Auditor | URL |
| -----| --------------|------|
| April 9, 2024 | [BlockSec](https://blocksec.com/) | [blocksec_eos_wram_v1.1-signed.pdf](https://github.com/blocksecteam/audit-reports/blob/main/c%2B%2B/blocksec_eos_wram_v1.1-signed.pdf)

## Key Functionalities

### Wrapping and Unwrapping RAM

The `eosio.wram` contract introduces two primary actions: `wrap` and `unwrap`. These actions facilitate the conversion of system RAM bytes into `WRAM` tokens and vice versa, enabling a streamlined tokenization of RAM resources.

- **Wrap**: Users can send or purchase system RAM bytes and convert them into `WRAM` tokens. These tokens are then credited to the sender's account, reflecting the amount of RAM they've tokenized.
- **Unwrap**: Users can convert their `WRAM` tokens back into system RAM bytes. This process retires the `WRAM` tokens and credits the user with the equivalent amount of RAM bytes.

### Fee Structure

- Transactions using `buyram` and `buyrambytes` actions to issue `WRAM` tokens incur a 0.5% fee from the system.
- The `ramtransfer` action, on the other hand, does not attract any fee when used for issuing `WRAM`.

### Security and Restrictions

- `eosio.ram` system account is prohibited from receiving `WRAM` tokens. This measure is designed to prevent accidental transfers that could result in RAM loss.

## Contract Deployment

The `eosio.wram` contract is deployed under the `eosio.wram` account with `eosio@active` permissions, ensuring robust security and control over the contract's operations.

## Development and Testing

### Build Instructions

To compile the contract, developers can use the following command:

```sh
$ cdt-cpp eosio.wram.cpp -I ./include
```

### Testing Framework

The contract includes a comprehensive testing suite designed to validate its functionality. The tests are executed using the following commands:

```sh
$ npm test

> test
> bun test
```

The testing suite covers various scenarios, including token issuance, RAM wrapping and unwrapping, and error handling, ensuring the contract's reliability and robustness.

## Conclusion

The `eosio.wram` contract represents a significant advancement in the EOS blockchain's functionality, offering users a flexible and efficient mechanism for managing system RAM through tokenization. By enabling the wrapping and unwrapping of RAM bytes, the contract provides an innovative solution for RAM allocation and management within the EOS ecosystem.