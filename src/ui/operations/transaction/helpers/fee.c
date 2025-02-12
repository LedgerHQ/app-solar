/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#include "ui/operations/transaction/helpers/fee.h"

#include <stdbool.h>
#include <string.h>  // strlcpy

#include "transaction/types.h"

#include "ui/ui_constants.h"
#include "ui/ui_utils.h"

/**
 * @brief Set the transaction fee.
 *
 * @param[in]   transaction Pointer to transaction structure.
 * @param[out]  item        Title output.
 * @param[out]  value       Text output.
 *
 * @return true if successful, otherwise false.
 */
bool ui_set_fee(const transaction_t *transaction,
                char item[MAX_ITEM_LEN],
                char value[MAX_VALUE_LEN]) {
    static const char FEE_LABEL[] = "Fee";

    if (transaction == NULL) {
        return false;
    }

    (void)strlcpy(item, FEE_LABEL, MAX_ITEM_LEN);

    return format_amount_sxp(value, MAX_VALUE_LEN, transaction->fee);
}
