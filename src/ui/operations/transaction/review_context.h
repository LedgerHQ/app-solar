#pragma once

#if defined(SCREEN_SIZE_WALLET)  // stax, flex

#include <stdbool.h>
#include <stdint.h>  // uint*_t

#include "transaction/types.h"

/* -------------------------------------------------------------------------- */

/**
 * @brief Function prototype for retrieving asset records.
 */
typedef bool (*ui_asset_record_fn_t)(const transaction_t *, char *, char *, uint16_t);

/**
 * @brief Indicates asset record structure.
 */
typedef enum asset_record_e {
    RECORD_TYPE_NONE = 0,  // No record to display
    RECORD_TYPE_SINGLE,    // Single-value record (e.g., ipfs hash, burn amount)
    RECORD_TYPE_PAIR       // Two-value record (e.g., payment recipient/amount)
} asset_record_t;

/**
 * @brief Global context for managing the transaction review process.
 */
typedef struct ui_transaction_review_ctx_s {
    transaction_t *tx_data;     // Ptr -> global transaction instance
    const char *review_intent;  // Ptr -> intent page text describing transaction's purpose
    const char *sign_intent;    // Ptr -> sign page text describing transaction's purpose

    struct ui_tx_asset_ctx_t {
        const char *label;                     // Ptr -> text describing asset records
        ui_asset_record_fn_t record_provider;  // Fn to retreive asset records
        uint16_t record_count;                 // Total asset records to retrieve
        asset_record_t record_type;            // RECORD_TYPE_NONE, _SINGLE or _PAIR
        uint8_t current_modal_page;            // Asset review modal page counter
    } asset;
} ui_tx_review_ctx_t;

extern ui_tx_review_ctx_t G_ui_txreview_ctx;

/* -------------------------------------------------------------------------- */

/**
 * @brief Initialises the context used for handling and displaying transaction review info.
 */
bool init_review_context(void);

/**
 * @brief Clears the context used for handling and displaying transaction review info.
 */
void clear_review_context(void);

#endif  // if defined(SCREEN_SIZE_WALLET)
