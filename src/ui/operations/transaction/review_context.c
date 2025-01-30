/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#if defined(SCREEN_SIZE_WALLET)  // stax, flex

#include "ui/operations/transaction/review_context.h"

#include <stdbool.h>

#include <os.h>  // explicit_bzero

#include "app_types.h"
#include "globals.h"

#include "ui/operations/transaction/helpers/strategy.h"

/* -------------------------------------------------------------------------- */

ui_tx_review_ctx_t G_ui_txreview_ctx;

/* -------------------------------------------------------------------------- */

bool init_review_context(void) {
    clear_review_context();

    G_ui_txreview_ctx.tx_data = &G_context.tx_info.transaction;

    return configure_review_strategy(&G_ui_txreview_ctx);
}

void clear_review_context(void) {
    explicit_bzero(&G_ui_txreview_ctx, sizeof(G_ui_txreview_ctx));
}

#endif  // if defined(SCREEN_SIZE_WALLET)
