/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#include "ui/operations/transaction/helpers/memo.h"

#include <stdbool.h>
#include <stdio.h>   // snprintf
#include <string.h>  // memcpy, strlcpy

#include "transaction/types.h"

#include "constants.h"
#include "ui/ui_constants.h"

/* -------------------------------------------------------------------------- */

/**
 * @brief Set the optional transaction memo.
 *
 * @param[in]   transaction Pointer to transaction structure.
 * @param[out]  item        Title output.
 * @param[out]  value       Text output.
 *
 * @return true if successful, otherwise false.
 */
bool ui_set_memo(const transaction_t *transaction,
                 char item[MAX_ITEM_LEN],
                 char value[MAX_VALUE_LEN]) {
    static const char MEMO_LABEL[] = "Memo";

    if (transaction == NULL) {
        return false;
    }

    (void)strlcpy(item, MEMO_LABEL, MAX_ITEM_LEN);

    (void)memcpy(value, transaction->memo, transaction->memo_len);
    value[transaction->memo_len] = NULL_TERMINATOR;

    return true;
}
