#pragma once

#include <stdint.h>  // uint*_t

#include <buffer.h>  // buffer_t

#include "transaction/transaction_errors.h"  // parser_status_e

#include "transaction/types.h"        // transaction_t
#include "transaction/types/types.h"  // transaction_asset_t

parser_status_e deserialise_transaction(buffer_t *buf, transaction_t *tx);

parser_status_e deserialise_transaction_common(buffer_t *buf, transaction_t *tx);

parser_status_e deserialise_transaction_asset(buffer_t *buf,
                                              transaction_asset_t *asset,
                                              uint16_t type,
                                              uint32_t typeGroup);
