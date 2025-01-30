/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#include "ui/operations/transaction/types/ui_transfer.h"

#include <stdbool.h>
#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t
#include <string.h>  // strlcpy, strnlen

#include <os.h>    // snprintf
#include <read.h>  // read_u64_le

#include "address.h"
#include "constants.h"

#include "transaction/types.h"
#include "transaction/types/types.h"
#include "transaction/types/transfer.h"

#include "ui/ui_utils.h"
#include "ui/ui_constants.h"

/* -------------------------------------------------------------------------- */

#if defined(SCREEN_SIZE_NANO)  // nanosp, nanox
const char TRANSFER_LABEL[9] = "Transfer";
#elif defined(SCREEN_SIZE_WALLET)  // stax, flex
const char PAYMENT_LABEL[8] = "Payment";
#endif

#if defined(SCREEN_SIZE_WALLET)  // stax, flex
#define TRANSFER_INTENT "send SXP"
#endif

#if defined(SCREEN_SIZE_NANO)  // nanosp, nanox
static const char RECIPIENT_LABEL[] = "Address";
#elif defined(SCREEN_SIZE_WALLET)  // stax, flex
static const char RECIPIENT_LABEL[] = "Recipient";
#endif

static const char PAYMENT_AMOUNT_LABEL[] = "Amount";

static const size_t AMOUNT_BYTE_SIZE = 8u;
static const uint16_t PAYMENT_INDEX_ZERO_OFFSET = 1u;  // (e.g., "Payment 0/n" => "Payment 1/n")

/* -------------------------------------------------------------------------- */

static inline void prepare_recipient_label(char label[MAX_ITEM_LEN],
                                           uint16_t index,
                                           uint16_t payment_count);
static inline void prepare_amount_label(char label[MAX_ITEM_LEN],
                                        uint16_t index,
                                        uint16_t payment_count);

/* -------------------------------------------------------------------------- */

/**
 * @brief Provides payment records for a transfer transaction.
 *
 * @param[in]   transaction Pointer to transaction structure.
 * @param[out]  label       Label output.
 * @param[out]  value       Value output.
 * @param[in]   index       Current record index.
 *
 * @return true if successful, otherwise false.
 *
 * @note Each payment record contains two values:
 *       - A recipient of funds being sent
 *       - The amount of funds that recipient will receive
 *
 * @see RECORD_TYPE_PAIR
 */
bool get_payment_record(const transaction_t *transaction,
                        char label[MAX_ITEM_LEN],
                        char value[MAX_VALUE_LEN],
                        uint16_t index) {
    if (transaction == NULL) {
        return false;
    }

    const uint16_t payment_idx = index >> 1u;
    if ((transaction->asset.Transfer.payment_count == 0u) ||
        (payment_idx >= transaction->asset.Transfer.payment_count)) {
        return false;
    }

    const size_t offset = payment_idx * (PUBKEY_HASH_LEN + AMOUNT_BYTE_SIZE);
    const uint16_t payment_offset = payment_idx + PAYMENT_INDEX_ZERO_OFFSET;

    if ((index % 2u) == 0u) {
        prepare_recipient_label(label, payment_offset, transaction->asset.Transfer.payment_count);

        if (address_from_pubkey_hash(
                &transaction->asset.Transfer.payments[offset + AMOUNT_BYTE_SIZE],
                PUBKEY_HASH_LEN,
                value,
                MAX_VALUE_LEN) < 0) {
            return false;
        }
    } else {
        prepare_amount_label(label, payment_offset, transaction->asset.Transfer.payment_count);
        if (!format_amount_sxp(value,
                               MAX_VALUE_LEN,
                               read_u64_le(transaction->asset.Transfer.payments, offset))) {
            return false;
        }
    }

    return true;
}

/* -------------------------------------------------------------------------- */

#if defined(SCREEN_SIZE_WALLET)  // stax, flex

static inline void prepare_recipient_label(char label[MAX_ITEM_LEN],
                                           uint16_t index,
                                           uint16_t payment_count) {
    (void)index;          // Unused
    (void)payment_count;  // Unused
    (void)strlcpy(label, RECIPIENT_LABEL, MAX_ITEM_LEN);
}

static inline void prepare_amount_label(char label[MAX_ITEM_LEN],
                                        uint16_t index,
                                        uint16_t payment_count) {
    (void)index;          // Unused
    (void)payment_count;  // Unused
    (void)strlcpy(label, PAYMENT_AMOUNT_LABEL, MAX_ITEM_LEN);
}

#elif defined(SCREEN_SIZE_NANO)  // nanosp, nanox

static inline void prepare_recipient_label(char label[MAX_ITEM_LEN],
                                           uint16_t index,
                                           uint16_t payment_count) {
    (void)snprintf(label,
                   MAX_ITEM_LEN,
                   "%.*s %u/%u",
                   strnlen(RECIPIENT_LABEL, sizeof(RECIPIENT_LABEL)),
                   RECIPIENT_LABEL,
                   index,
                   payment_count);
}

static inline void prepare_amount_label(char label[MAX_ITEM_LEN],
                                        uint16_t index,
                                        uint16_t payment_count) {
    (void)snprintf(label,
                   MAX_ITEM_LEN,
                   "%.*s %u/%u",
                   strnlen(PAYMENT_AMOUNT_LABEL, sizeof(PAYMENT_AMOUNT_LABEL)),
                   PAYMENT_AMOUNT_LABEL,
                   index,
                   payment_count);
}

#endif  // elif defined(SCREEN_SIZE_NANO
