/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#include "transfer.h"

#include <stdint.h>  // uint*_t

#include <buffer.h>  // buffer_t, buffer_read_u16

#include "constants.h"

#include "transaction/transaction_errors.h"

/* -------------------------------------------------------------------------- */

static const uint16_t PAYMENT_COUNT_MIN = 1u;
static const uint16_t PAYMENT_COUNT_MAX = 127u;

/* -------------------------------------------------------------------------- */

/**
 * @brief Deserialise a Transfer asset.
 *
 * @param[in, out]  buf     Pointer to buffer with serialised transaction.
 * @param[out]      asset   Pointer to transaction asset structure.
 *
 * @return PARSING_OK if successful, otherwise an error status (parser_status_e)
 */
parser_status_e deserialise_transfer(buffer_t *buf, transfer_asset_t *transfer) {
    // length
    if (!buffer_read_u16(buf, &transfer->payment_count, LE)) {
        return WRONG_LENGTH_ERROR;
    }

    if ((transfer->payment_count < PAYMENT_COUNT_MIN) ||
        (transfer->payment_count > PAYMENT_COUNT_MAX)) {
        return TX_ASSET_PARSING_ERROR;
    }

    // payments
    transfer->payments = &buf->ptr[buf->offset];

    if (!buffer_seek_cur(buf, (PUBKEY_HASH_LEN + sizeof(uint64_t)) * transfer->payment_count)) {
        return WRONG_LENGTH_ERROR;
    }

    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
}
