/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#include "deserialise_message.h"

#include <stdbool.h>
#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

#include <buffer.h>  // buffer_t, buffer_read_u16

#include "constants.h"

#include "transaction/transaction_errors.h"
#include "transaction/types.h"

#include "utils/check_ascii.h"

/* -------------------------------------------------------------------------- */

static const uint16_t MESSAGE_MIN_LEN = 1u;

_Static_assert(TRANSACTION_LEN_MAX <= UINT16_MAX,
               "TRANSACTION_LEN_MAX too large for 16-bit usage.");
static const uint16_t MESSAGE_MAX_LEN = (uint16_t)(TRANSACTION_LEN_MAX - NULL_TERMINATOR_LEN);

/* -------------------------------------------------------------------------- */

/**
 * @brief Deserialise message in structure.
 *
 * @param[in, out]  buf Pointer to buffer with message.
 * @param[out]      tx  Pointer to transaction structure.
 *
 * @return PARSING_OK if successful, otherwise an error status (parser_status_e)
 */
parser_status_e deserialise_message(buffer_t *buf, transaction_t *tx) {
    uint16_t message_length = 0u;
    if (!buffer_read_u16(buf, &message_length, LE)) {
        return TX_ASSET_PARSING_ERROR;
    }

    tx->message_length = (size_t)message_length;

    if (tx->message_length < MESSAGE_MIN_LEN) {
        return WRONG_LENGTH_ERROR;
    }

    if (tx->message_length > MESSAGE_MAX_LEN) {
        return WRONG_LENGTH_ERROR;
    }

    tx->message = &buf->ptr[buf->offset];

    if (!buffer_seek_cur(buf, tx->message_length)) {
        return TX_ASSET_PARSING_ERROR;
    }

    if (!check_ascii(tx->message, tx->message_length, true)) {
        return MESSAGE_ENCODING_ERROR;
    }

    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
}
