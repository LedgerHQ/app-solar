/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#include "vote.h"

#include <stdbool.h>
#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

#include <buffer.h>  // buffer_t, buffer_read_u8

#include "transaction/transaction_errors.h"

#include "utils/check_ascii.h"

/* -------------------------------------------------------------------------- */

const uint8_t CANCEL_VOTE_COUNT = 0u;

static const uint8_t VOTE_COUNT_MAX = 53u;

static const uint8_t USERNAME_LENGTH_MIN = 1u;
static const uint8_t USERNAME_LENGTH_MAX = 20u;

static const uint16_t PERCENTAGE_MIN = 1u;
static const uint16_t PERCENTAGE_MAX = 10000u;

/* -------------------------------------------------------------------------- */

/**
 * @brief Deserialise a Vote asset.
 *
 * @param[in, out]  buf     Pointer to buffer with serialised transaction.
 * @param[out]      asset   Pointer to transaction asset structure.
 *
 * @return PARSING_OK if successful, otherwise an error status (parser_status_e)
 */
parser_status_e deserialise_vote(buffer_t *buf, vote_asset_t *votes) {
    // Number of votes
    uint8_t temp_vote_length = 0u;
    if (!buffer_read_u8(buf, &temp_vote_length)) {
        return WRONG_LENGTH_ERROR;
    }

    votes->vote_count = (size_t)temp_vote_length;

    if (votes->vote_count == CANCEL_VOTE_COUNT) {
        return PARSING_OK;
    }

    if (votes->vote_count > VOTE_COUNT_MAX) {
        return TX_ASSET_PARSING_ERROR;
    }

    // votes
    votes->votes = &buf->ptr[buf->offset];
    uint16_t total_percentage = 0u;

    for (uint8_t i = 0u; i < votes->vote_count; i++) {
        uint8_t username_length = 0u;

        if (!buffer_read_u8(buf, &username_length)) {
            return WRONG_LENGTH_ERROR;
        }

        if ((username_length < USERNAME_LENGTH_MIN) || (username_length > USERNAME_LENGTH_MAX)) {
            return TX_ASSET_PARSING_ERROR;
        }

        const uint8_t *username = &buf->ptr[buf->offset];

        if (!buffer_seek_cur(buf, username_length)) {
            return WRONG_LENGTH_ERROR;
        }

        if (!check_ascii(username, username_length, false)) {
            return TX_ASSET_PARSING_ERROR;
        }

        uint16_t percent = 0u;

        if (!buffer_read_u16(buf, &percent, LE)) {
            return WRONG_LENGTH_ERROR;
        }

        if ((percent < PERCENTAGE_MIN) || (percent > PERCENTAGE_MAX)) {
            return TX_ASSET_PARSING_ERROR;
        }

        total_percentage += percent;
    }

    if (total_percentage != PERCENTAGE_MAX) {
        return TX_ASSET_PARSING_ERROR;
    }

    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
}
