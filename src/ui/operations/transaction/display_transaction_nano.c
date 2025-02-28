/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#if defined(SCREEN_SIZE_NANO)  // nanosp, nanox

#include "ui/operations/transaction/display_transaction.h"

#include <stdbool.h>
#include <stdint.h>  // uint*_t
#include <string.h>  // strnlen

#include <io.h>  // io_send_sw
#include <cx.h>  // CX_OK
#include <os.h>  // explicit_bzero
#include <ux.h>  // ux_flow_init, UX_FLOW, UX_STEP_NOCB...

#include "glyphs.h"  // C_icon_eye

#include "app_types.h"
#include "globals.h"
#include "sw.h"

#include "crypto/crypto.h"

#include "helper/send_response.h"

#include "transaction/types.h"

#include "ui/ui_constants.h"
#include "ui/ui_text.h"

#include "ui/action/ux_approve.h"

#include "ui/operations/transaction/helpers/fee.h"
#include "ui/operations/transaction/helpers/memo.h"

#include "ui/operations/transaction/types/ui_ipfs.h"
#include "ui/operations/transaction/types/ui_transfer.h"
#include "ui/operations/transaction/types/ui_burn.h"
#include "ui/operations/transaction/types/ui_vote.h"

/* -------------------------------------------------------------------------- */

static const char TRANSACTION_REVIEW_TEXT[] = "Review";

static const uint16_t EXTENDED_ASSET_MULTIPLIER = 2u;
static const uint8_t FIELD_OFFSET = 1u;

/* -------------------------------------------------------------------------- */

typedef enum {
    STATIC_STATE,
    DYNAMIC_STATE,
} screen_state_t;

typedef bool (*ui_asset_record_fn_t)(const transaction_t *, char *, char *, uint16_t);

typedef struct {
    ui_text_t text;                              // 272
    char label[MAX_ITEM_LEN];                    // 16
    transaction_t *transaction;                  // 4
    ui_asset_record_fn_t asset_record_provider;  // 4
    uint16_t offset;                             // 2
    uint16_t asset_record_count;                 // was size_t  // 2
    screen_state_t state;                        // 1

} ui_tx_review_ctx_t;

static ui_tx_review_ctx_t txreview_ctx = {0};

/* -------------------------------------------------------------------------- */

static bool set_asset(void) {
    return txreview_ctx.asset_record_provider(&G_context.tx_info.transaction,
                                              txreview_ctx.text.item,
                                              txreview_ctx.text.value,
                                              txreview_ctx.offset);
}

static bool set_fee(void) {
    return ui_set_fee(&G_context.tx_info.transaction,
                      txreview_ctx.text.item,
                      txreview_ctx.text.value);
}

static bool set_memo(void) {
    return ui_set_memo(&G_context.tx_info.transaction,
                       txreview_ctx.text.item,
                       txreview_ctx.text.value);
}

static inline bool has_memo(void) {
    return txreview_ctx.transaction->memo_len > 0u;
}

static inline void set_dynamic_state(void) {
    txreview_ctx.state = DYNAMIC_STATE;
}

static inline void set_static_state(void) {
    txreview_ctx.state = STATIC_STATE;
}

/* -------------------------------------------------------------------------- */

static bool handle_forward_navigation(void) {
    if (txreview_ctx.offset < txreview_ctx.asset_record_count) {
        return set_asset();
    } else if (txreview_ctx.offset == txreview_ctx.asset_record_count) {
        return set_fee();
    } else if (txreview_ctx.offset == (txreview_ctx.asset_record_count + FIELD_OFFSET) &&
               has_memo()) {
        return set_memo();
    } else {
        return false;
    }
}

static bool handle_backward_navigation(void) {
    if (txreview_ctx.offset == 0u) {
        return false;
    }

    txreview_ctx.offset--;

    if (txreview_ctx.offset < txreview_ctx.asset_record_count) {
        return set_asset();
    }

    if (txreview_ctx.offset == (txreview_ctx.asset_record_count + FIELD_OFFSET)) {
        if (has_memo()) {
            return set_memo();
        } else {
            txreview_ctx.offset--;
        }
    }

    if (txreview_ctx.offset == txreview_ctx.asset_record_count) {
        return set_fee();
    }

    return false;
}

static bool get_next_data(ux_flow_direction_t direction) {
    if (direction == FLOW_DIRECTION_FORWARD) {
        bool result = handle_forward_navigation();
        if (result) {
            txreview_ctx.offset++;
        }
        return result;
    }
    return handle_backward_navigation();
}

/* -------------------------------------------------------------------------- */

/*
 * Handle edge case for paging.
 *
 * Adjusts the flow stack indices to handle a paging edge case.
 * Only performs the adjustment if the current index is at least 2.
 */
static void bnnn_paging_edgecase(void) {
    const unsigned char stack_slot = G_ux.stack_count - 1u;

    G_ux.flow_stack[stack_slot].prev_index =
        G_ux.flow_stack[G_ux.stack_count - ((unsigned char)1u)].index - 2u;
    G_ux.flow_stack[G_ux.stack_count - 1u].index--;
    (void)ux_flow_relayout();
}

static void handle_upper_delimiter(void) {
    if (txreview_ctx.state == STATIC_STATE) {
        if (get_next_data(FLOW_DIRECTION_FORWARD)) {
            set_dynamic_state();
        }

        ux_flow_next();
    } else {
        if (get_next_data(FLOW_DIRECTION_BACKWARD)) {
            set_dynamic_state();
            ux_flow_next();
        } else {
            set_static_state();
            ux_flow_prev();
        }
    }
}

static void handle_lower_delimiter(void) {
    if (txreview_ctx.state == STATIC_STATE) {
        bool result = get_next_data(FLOW_DIRECTION_BACKWARD);
        if (result) {
            set_dynamic_state();
        }
        ux_flow_prev();
    } else {
        if (get_next_data(FLOW_DIRECTION_FORWARD)) {
            set_dynamic_state();
            bnnn_paging_edgecase();
        } else if ((txreview_ctx.offset == txreview_ctx.asset_record_count) && (has_memo())) {
            set_dynamic_state();
            txreview_ctx.offset++;
            bnnn_paging_edgecase();
        } else {
            set_static_state();
            ux_flow_next();
        }
    }
}

static void display_next_state(bool is_upper_delimiter) {
    explicit_bzero(&txreview_ctx.text, sizeof(txreview_ctx.text));

    if (is_upper_delimiter) {
        handle_upper_delimiter();
    } else {
        handle_lower_delimiter();
    }
}

/* -------------------------------------------------------------------------- */

static int configure_core_strategy(void) {
    switch (txreview_ctx.transaction->type) {
        case CORE_TRANSACTION_TYPE_IPFS: {
            (void)strlcpy(txreview_ctx.label, IPFS_LABEL, MAX_ITEM_LEN);
            txreview_ctx.asset_record_count = IPFS_RECORD_COUNT;
            txreview_ctx.asset_record_provider = get_ipfs_record;
            break;
        }
        case CORE_TRANSACTION_TYPE_TRANSFER: {
            (void)strlcpy(txreview_ctx.label, TRANSFER_LABEL, MAX_ITEM_LEN);
            txreview_ctx.asset_record_count =
                txreview_ctx.transaction->asset.Transfer.payment_count * EXTENDED_ASSET_MULTIPLIER;
            txreview_ctx.asset_record_provider = get_payment_record;
            break;
        }
        default:
            return io_send_sw(SW_TX_PARSING_FAIL);
    }

    return 0;
}

static int configure_solar_strategy(void) {
    switch (txreview_ctx.transaction->type) {
        case SOLAR_TRANSACTION_TYPE_BURN: {
            (void)strlcpy(txreview_ctx.label, BURN_LABEL, MAX_ITEM_LEN);
            txreview_ctx.asset_record_provider = get_burn_record;
            txreview_ctx.asset_record_count = BURN_RECORD_COUNT;
            break;
        }
        case SOLAR_TRANSACTION_TYPE_VOTE: {
            if (txreview_ctx.transaction->asset.Vote.vote_count > CANCEL_VOTE_COUNT) {
                (void)strlcpy(txreview_ctx.label, VOTE_LABEL, MAX_ITEM_LEN);
            } else {
                (void)strlcpy(txreview_ctx.label, CANCEL_VOTE_LABEL, MAX_ITEM_LEN);
            }

            txreview_ctx.asset_record_count =
                txreview_ctx.transaction->asset.Vote.vote_count * EXTENDED_ASSET_MULTIPLIER;
            txreview_ctx.asset_record_provider = get_vote_record;
            break;
        }
        default:
            return io_send_sw(SW_TX_PARSING_FAIL);
    }

    return 0;
}

/* -------------------------------------------------------------------------- */

UX_STEP_NOCB(ux_display_tx_review_step,
             pnn,
             {
                 &C_icon_eye,
                 TRANSACTION_REVIEW_TEXT,
                 txreview_ctx.label,
             });

UX_STEP_INIT(ux_upper_delimiter_step, NULL, NULL, { display_next_state(true); });

UX_STEP_NOCB(ux_display_general_step,
             bnnn_paging,
             {
                 txreview_ctx.text.item,
                 txreview_ctx.text.value,
             });

UX_STEP_INIT(ux_lower_delimiter_step, NULL, NULL, { display_next_state(false); });

UX_APPROVE_STEP(ux_display_tx_approve_step, SIGNATURE_RESPONSE);
UX_REJECT_STEP(ux_display_tx_reject_step, SIGNATURE_RESPONSE);

/* -------------------------------------------------------------------------- */

// FLOW to display transaction:
// #1 screen: eye icon + "Confirm" + transaction type
// #x screen: Dynamic Screen
// #n-2 screen: memo (if present)
// #n-1 screen: approve button
// #n screen: reject button
UX_FLOW(ux_display_tx_flow,
        &ux_display_tx_review_step,
        &ux_upper_delimiter_step,
        &ux_display_general_step,
        &ux_lower_delimiter_step,
        &ux_display_tx_approve_step,
        &ux_display_tx_reject_step);

/* -------------------------------------------------------------------------- */

int ui_display_transaction(void) {
    if ((G_context.req_type != CONFIRM_TRANSACTION) || (G_context.state != STATE_PARSED)) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    // Second verification of the message hash
    if (verify_message_hash() != CX_OK) {
        return io_send_sw(SW_VALIDATION_HASH_MISMATCH);
    }

    (void)explicit_bzero(&txreview_ctx, sizeof(txreview_ctx));
    txreview_ctx.state = STATIC_STATE;
    txreview_ctx.offset = 0u;
    txreview_ctx.transaction = &G_context.tx_info.transaction;

    const int error = (G_context.tx_info.transaction.typeGroup == TYPEGROUP_CORE)
                          ? configure_core_strategy()
                          : configure_solar_strategy();
    if (error > 0) {
        return error;
    }

    ux_flow_init(0u, ux_display_tx_flow, NULL);

    return 0;
}

#endif  // if defined(SCREEN_SIZE_NANO)
