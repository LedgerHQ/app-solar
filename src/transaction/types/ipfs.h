#pragma once

#include <stdint.h>  // uint*_t

#include <buffer.h>  // buffer_t

#include "transaction/transaction_errors.h"

/* -------------------------------------------------------------------------- */

typedef struct {
    const uint8_t *hash;  // (MAX 64 bytes)
    uint8_t hash_length;
    uint8_t reserved[3];  // padding for alignment
} ipfs_asset_t;

/* -------------------------------------------------------------------------- */

parser_status_e deserialise_ipfs(buffer_t *buf, ipfs_asset_t *ipfs);
