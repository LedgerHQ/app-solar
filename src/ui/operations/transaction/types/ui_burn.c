/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#include "ui/operations/transaction/types/ui_burn.h"

#include <stdbool.h>
#include <stdint.h>  // uint*_t
#include <string.h>  // strnlcpy, NULL

#include "transaction/types.h"

#include "transaction/types/types.h"

#include "ui/ui_constants.h"
#include "ui/ui_utils.h"

/* -------------------------------------------------------------------------- */

const uint16_t BURN_RECORD_COUNT = 1u;

#if defined(SCREEN_SIZE_NANO)  // nanosp, nanox
const char BURN_LABEL[5] = "Burn";
#endif

#if defined(SCREEN_SIZE_NANO)  // nanosp, nanox
static const char BURN_AMOUNT_LABEL[] = "Amount";
#elif defined(SCREEN_SIZE_WALLET)  // stax, flex
static const char BURN_AMOUNT_LABEL[] = "Amount (to burn)";
#endif

/* -------------------------------------------------------------------------- */

/**
 * @brief Provides the asset record for a Burn transaction.
 *
 * @param[in]   transaction Pointer to transaction structure.
 * @param[out]  label       Label output.
 * @param[out]  value       Value output.
 * @param[in]   index       Current record index.
 *
 * @return true if successful, otherwise false.
 *
 * @note A Burn asset contains one record with a single value:
 *       - The amount to be burned
 *
 * @see RECORD_TYPE_SINGLE
 */
bool get_burn_record(const transaction_t *transaction,
                     char label[MAX_ITEM_LEN],
                     char value[MAX_VALUE_LEN],
                     uint16_t index) {
    if ((transaction == NULL) || (index != 0u)) {
        return false;
    }

    (void)strlcpy(label, BURN_AMOUNT_LABEL, MAX_ITEM_LEN);

    return format_amount_sxp(value, MAX_VALUE_LEN, transaction->asset.Burn.amount);
}
