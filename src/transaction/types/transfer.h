#pragma once

#include <stdint.h>  // uint*_t

#include <buffer.h>  // buffer_t

#include "transaction/transaction_errors.h"

/* -------------------------------------------------------------------------- */

typedef struct {
    const uint8_t *payments;  // Pointer to the start of payments records.
    uint16_t payment_count;
    uint16_t reserved;  // padding for alignment
} transfer_asset_t;

/* -------------------------------------------------------------------------- */

parser_status_e deserialise_transfer(buffer_t *buf, transfer_asset_t *transfer);
