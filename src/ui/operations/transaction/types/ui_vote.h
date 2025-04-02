#pragma once

#include <stdbool.h>
#include <stdint.h>  // uint*_t

#include "transaction/types.h"

#include "ui/ui_constants.h"

/* -------------------------------------------------------------------------- */

#define VOTE_LABEL "Vote"

#if defined(SCREEN_SIZE_NANO)             // nanosp, nanox
extern const char CANCEL_VOTE_LABEL[12];  // "Cancel vote"
#endif

#if defined(SCREEN_SIZE_WALLET)  // stax, flex
#define VOTE_INTENT        "vote for block producers"
#define CANCEL_VOTE_INTENT "cancel voting for block producers"
#endif

/* -------------------------------------------------------------------------- */

bool get_vote_record(const transaction_t *transaction,
                     char label[MAX_ITEM_LEN],
                     char value[MAX_VALUE_LEN],
                     uint16_t index);

bool vote_search(const uint8_t *tx_bytes, vote_record_t *vote, uint8_t vote_pos, uint8_t max_votes);
