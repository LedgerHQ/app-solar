#pragma once

#include <stdint.h>  // uint*_t

#include <buffer.h>

#include "transaction/transaction_errors.h"

/* -------------------------------------------------------------------------- */

typedef struct {
    uint64_t amount;
} burn_asset_t;

/* -------------------------------------------------------------------------- */

parser_status_e deserialise_burn(buffer_t *buf, burn_asset_t *burn);
