/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#include "ui/operations/transaction/types/ui_ipfs.h"

#include <stdbool.h>
#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t
#include <string.h>  // strlcpy

#include <base58.h>  // base58_encode, MAX_ENC_INPUT_SIZE

#include "transaction/types.h"

#include "ui/ui_constants.h"

/* -------------------------------------------------------------------------- */

const uint16_t IPFS_RECORD_COUNT = 1u;

#if defined(SCREEN_SIZE_NANO)  // nanosp, nanox
const char IPFS_LABEL[5] = "IPFS";
#endif

/* -------------------------------------------------------------------------- */

/**
 * @brief Provides the asset record for an IPFS transaction.
 *
 * @param[in]   transaction Pointer to transaction structure.
 * @param[out]  label       Label output.
 * @param[out]  value       Value output.
 * @param[in]   index       Current record index.
 *
 * @return true if successful, otherwise false.
 *
 * @note An IPFS asset contains one record with a single value:
 *       - The IPFS Hash
 *
 * @see RECORD_TYPE_SINGLE
 */
bool get_ipfs_record(const transaction_t *transaction,
                     char label[MAX_ITEM_LEN],
                     char value[MAX_VALUE_LEN],
                     uint16_t index) {
    static const char IPFS_HASH_LABEL[] = "Content ID";

    if ((transaction == NULL) || (index != 0u)) {
        return false;
    }

    (void)strlcpy(label, IPFS_HASH_LABEL, MAX_ITEM_LEN);

    return base58_encode(transaction->asset.Ipfs.hash,
                         transaction->asset.Ipfs.hash_length,
                         value,
                         MAX_ENC_INPUT_SIZE) > 0;
}
