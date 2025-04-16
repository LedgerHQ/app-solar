#pragma once

#include <stdbool.h>
#include <stdint.h>  // uint*_t

#include "transaction/types.h"

#include "ui/ui_constants.h"

/* -------------------------------------------------------------------------- */

#if defined(SCREEN_SIZE_NANO)         // nanosp, NanoX
extern const char TRANSFER_LABEL[9];  // "Transfer"
#elif defined(SCREEN_SIZE_WALLET)     // stax, flex
extern const char PAYMENT_LABEL[8];  // "Payment"
#endif

#if defined(SCREEN_SIZE_WALLET)  // stax, flex
#define TRANSFER_INTENT "send SXP"
#endif

/* -------------------------------------------------------------------------- */

bool get_payment_record(const transaction_t *transaction,
                        char label[MAX_ITEM_LEN],
                        char value[MAX_VALUE_LEN],
                        uint16_t step);
