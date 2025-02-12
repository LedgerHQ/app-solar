#pragma once

#include <stdbool.h>
#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

#include <bip32.h>  // MAX_BIP32_PATH

#include "constants.h"
#include "transaction/types.h"

#define GET_APP_NAME   ((uint8_t)0xA1)  // name of the application
#define GET_VERSION    ((uint8_t)0xA2)  // version of the application
#define GET_PUBLIC_KEY ((uint8_t)0xB1)  // public key of corresponding BIP32 path
#define GET_ADDRESS    ((uint8_t)0xB2)  // address of corresponding BIP32 path
#define SIGN_MESSAGE   ((uint8_t)0xC1)  // sign message with BIP32 path
#define SIGN_TX        ((uint8_t)0xC2)  // sign transaction with BIP32 path

typedef enum {
    STATE_NONE,     // No state
    STATE_PARSED,   // Transaction data parsed
    STATE_APPROVED  // Transaction data approved
} state_e;

typedef enum {
    CONFIRM_NONE,         // no request type
    CONFIRM_PUBLICKEY,    // confirm public key
    CONFIRM_ADDRESS,      // confirm address
    CONFIRM_TRANSACTION,  // confirm transaction information
    CONFIRM_MESSAGE       // confirm message information
} request_type_e;

typedef struct {
    uint8_t chain_code[HASH_32_LEN];          // for public key derivation
    uint8_t raw_public_key[PUBKEY_BYTE_LEN];  // compressed public key
    bool use_chaincode;                       // whether the chain code should be used
} pubkey_ctx_t;

typedef struct {
    uint8_t raw_tx[TRANSACTION_LEN_MAX];  // raw transaction serialised
    size_t raw_tx_len;                    // length of raw transaction
    uint32_t reserved;                    // padding for alignment
    uint8_t m_hash[HASH_32_LEN];          // message hash digest
    transaction_t transaction;            // structured transaction
    uint8_t signature[SIG_SCHNORR_LEN];   // transaction signature
} transaction_ctx_t;

typedef struct {
    union {
        pubkey_ctx_t pk_info;       // public key context
        transaction_ctx_t tx_info;  // transaction context
    };
    uint32_t bip32_path[MAX_BIP32_PATH];  // BIP32 path
    state_e state;                        // state of the context
    request_type_e req_type;              // user request
    uint8_t req_num;                      // current request number
    uint8_t bip32_path_len;               // length of BIP32 path
    uint8_t network;                      // network byte
    uint8_t reserved[5];                  // padding for alignment
} global_ctx_t;
