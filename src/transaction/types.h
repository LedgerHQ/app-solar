#pragma once

#include <stdint.h>  // uint*_t

#include "transaction/types/types.h"

/* -------------------------------------------------------------------------- */

#define STARTING_BYTE ((uint8_t)0xFF)

#define TRANSACTION_VERSION_3 ((uint8_t)0x03)

#define TYPEGROUP_CORE  ((uint32_t)0x01)
#define TYPEGROUP_SOLAR ((uint32_t)0x02)

#define CORE_TRANSACTION_TYPE_IPFS     ((uint16_t)0x05)
#define CORE_TRANSACTION_TYPE_TRANSFER ((uint16_t)0x06)
#define SOLAR_TRANSACTION_TYPE_BURN    ((uint16_t)0x00)
#define SOLAR_TRANSACTION_TYPE_VOTE    ((uint16_t)0x02)

/* -------------------------------------------------------------------------- */

typedef struct {
    transaction_asset_t asset;  // transaction specific assets
    uint64_t fee;               // fee
    union {
        const uint8_t *message;  // message
        const uint8_t *memo;     // memo (MAX length 255)
    };
    const uint8_t *sender_publickey;  // sender public key (33 bytes)
    uint32_t typeGroup;               // typeGroup
    uint16_t type;                    // type
    union {
        uint16_t message_length;  // Message length
        uint8_t memo_len;         // length of memo
    };
} transaction_t;

// typedef struct {
//     // Message
//     const uint8_t *message;  // message
//     uint8_t message_length;   // Message length

//     // Transaction
//     const uint8_t *sender_publickey;  // sender public key (33 bytes)
//     uint64_t fee;                     // fee
//     uint32_t typeGroup;               // typeGroup
//     uint16_t type;                    // type
//     uint8_t memo_len;                 // length of memo
//     const uint8_t *memo;              // memo (MAX length 255)
//     transaction_asset_t asset;        // transaction specific assets (variable length and
//     structure)
// } transaction_t;
