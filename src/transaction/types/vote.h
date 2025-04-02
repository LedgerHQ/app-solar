#pragma once

#include <stdint.h>  // uint*_t

#include <buffer.h>  // buffer_t

#include "transaction/transaction_errors.h"

/* -------------------------------------------------------------------------- */

extern const uint8_t CANCEL_VOTE_COUNT;  // 0u

typedef struct {
    const uint8_t *username;
    uint16_t percent;
    uint8_t username_length;  // (1..20 bytes)
    uint8_t reserved;         // padding for alignment
} vote_record_t;

typedef struct {
    const uint8_t *votes;  // Pointer to start of vote records
    uint8_t vote_count;
    uint8_t reserved[3];  // padding for alignment
} vote_asset_t;

/* -------------------------------------------------------------------------- */

parser_status_e deserialise_vote(buffer_t *buf, vote_asset_t *votes);
