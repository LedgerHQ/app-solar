#pragma once

#include <stdbool.h>
#include <stdint.h>  // uint*_t

#include "transaction/types.h"

#include "ui/ui_constants.h"

/* -------------------------------------------------------------------------- */

extern const uint16_t BURN_RECORD_COUNT;  // 1u

#if defined(SCREEN_SIZE_NANO)     // nanosp, nanox
extern const char BURN_LABEL[5];  // "Burn"
#endif

#if defined(SCREEN_SIZE_WALLET)  // stax, flex
#define BURN_INTENT "burn SXP"
#endif

/* -------------------------------------------------------------------------- */

bool get_burn_record(const transaction_t *transaction,
                     char label[MAX_ITEM_LEN],
                     char value[MAX_VALUE_LEN],
                     uint16_t index);
