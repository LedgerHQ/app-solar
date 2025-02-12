/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#include "ui/operations/transaction/types/ui_vote.h"

#include <stdbool.h>
#include <stdint.h>  // uint*_t
#include <string.h>  // strlcpy, strnlen

#include <read.h>  // read_u16_le
#include <os.h>    // snprintf

#include "transaction/types.h"
#include "transaction/types/types.h"
#include "transaction/types/vote.h"

#include "ui/ui_utils.h"
#include "ui/ui_constants.h"

/* -------------------------------------------------------------------------- */

#if defined(SCREEN_SIZE_NANO)  // nanosp, nanox
const char CANCEL_VOTE_LABEL[12] = "Cancel vote";
#endif

#if defined(SCREEN_SIZE_NANO)  // nanosp, nanox
static const char VOTE_PERCENT_LABEL[] = "Vote %";
#elif defined(SCREEN_SIZE_WALLET)  // stax, flex
static const char BLOCK_PRODUCER_LABEL[] = "Block Producer";
static const char VOTE_PERCENT_LABEL[] = "Vote Percent";
#endif

static const uint16_t VOTE_RECORDS_BYTE_OFFSET = 1u;
static const size_t USERNAME_LEN_BYTE_SIZE = 1u;
static const size_t VOTE_PERCENT_BYTE_SIZE = 2u;
static const uint8_t VOTE_PERCENT_PRECISION = 2u;  // Number of decimal places (e.g., 12.34%)

/* -------------------------------------------------------------------------- */

static inline void prepare_block_producer_label(char label[MAX_ITEM_LEN],
                                                uint16_t position,
                                                uint16_t vote_count);
static inline void prepare_vote_percent_label(char label[MAX_ITEM_LEN],
                                              uint16_t position,
                                              uint16_t vote_count);

/* -------------------------------------------------------------------------- */

/**
 * @brief Provides vote records for a Vote transaction.
 *
 * @param[in]   transaction Pointer to transaction structure.
 * @param[out]  label       Label output.
 * @param[out]  value       Value output.
 * @param[in]   index       Current record index.
 *
 * @return true if successful, otherwise false.
 *
 * @note Each vote record contains two values:
 *       - The username of a block producer to vote for
 *       - The percent of vote weight to apply to that block producer
 *
 * @see RECORD_TYPE_PAIR
 *
 * @note A cancel vote record is empty and contains no values.
 *
 * @see RECORD_TYPE_NONE
 */
bool get_vote_record(const transaction_t *transaction,
                     char label[MAX_ITEM_LEN],
                     char value[MAX_VALUE_LEN],
                     uint16_t index) {
    if (transaction == NULL) {
        return false;
    }

    const uint16_t vote_pos = index >> 1;
    const uint16_t vote_count = transaction->asset.Vote.vote_count;

    if ((vote_count == CANCEL_VOTE_COUNT) || (vote_pos >= vote_count)) {
        return false;
    }

    vote_record_t vote = {0};
    if (!vote_search(transaction->asset.Vote.votes, &vote, vote_pos, vote_count)) {
        return false;
    }

    if ((index % 2u) == 0u) {
        prepare_block_producer_label(label, vote_pos + VOTE_RECORDS_BYTE_OFFSET, vote_count);
        (void)snprintf(value, MAX_VALUE_LEN, "%.*s", (int)vote.username_length, vote.username);
    } else {
        prepare_vote_percent_label(label, vote_pos + VOTE_RECORDS_BYTE_OFFSET, vote_count);
        if (!format_percent(value, MAX_VALUE_LEN, vote.percent, VOTE_PERCENT_PRECISION)) {
            return false;
        }
    }

    return true;
}

/* -------------------------------------------------------------------------- */

/**
 * @brief Iterate the vote asset to find the given vote record.
 */
bool vote_search(const uint8_t *tx_bytes,
                 vote_record_t *vote,
                 uint8_t vote_pos,
                 uint8_t max_votes) {
    if ((tx_bytes == NULL) || (vote_pos > max_votes) || (vote == NULL)) {
        return false;
    }

    size_t offset = 0u;

    // Iterate through the votes to reach the desired vote record
    for (uint8_t idx = 0u; idx < vote_pos; idx++) {
        uint8_t username_length = tx_bytes[offset];
        offset += (USERNAME_LEN_BYTE_SIZE + username_length + VOTE_PERCENT_BYTE_SIZE);
    }

    uint8_t username_length = tx_bytes[offset];
    offset++;

    vote->username_length = username_length;
    vote->username = &tx_bytes[offset];
    offset += username_length;

    vote->percent = read_u16_le(tx_bytes, offset);

    return true;
}

/* -------------------------------------------------------------------------- */

#if defined(SCREEN_SIZE_NANO)  // nanosp, nanox

static inline void prepare_block_producer_label(char label[MAX_ITEM_LEN],
                                                uint16_t position,
                                                uint16_t vote_count) {
    (void)snprintf(label, MAX_ITEM_LEN, "%s %u/%u", VOTE_LABEL, position, vote_count);
}

static inline void prepare_vote_percent_label(char label[MAX_ITEM_LEN],
                                              uint16_t position,
                                              uint16_t vote_count) {
    (void)snprintf(label,
                   MAX_ITEM_LEN,
                   "%.*s %u/%u",
                   (int)strnlen(VOTE_PERCENT_LABEL, MAX_ITEM_LEN),
                   VOTE_PERCENT_LABEL,
                   position,
                   vote_count);
}

#elif defined(SCREEN_SIZE_WALLET)  // flex, stax

static inline void prepare_block_producer_label(char label[MAX_ITEM_LEN],
                                                uint16_t position,
                                                uint16_t vote_count) {
    (void)position;    // Unused
    (void)vote_count;  // Unused
    (void)strlcpy(label, BLOCK_PRODUCER_LABEL, MAX_ITEM_LEN);
}

static inline void prepare_vote_percent_label(char label[MAX_ITEM_LEN],
                                              uint16_t position,
                                              uint16_t vote_count) {
    (void)position;    // Unused
    (void)vote_count;  // Unused
    (void)strlcpy(label, VOTE_PERCENT_LABEL, MAX_ITEM_LEN);
}

#endif  // elif defined(SCREEN_SIZE_WALLET)
