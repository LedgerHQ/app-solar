/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#if defined(SCREEN_SIZE_WALLET)  // stax, flex

#include "ui/operations/transaction/helpers/strategy.h"

#include <stdbool.h>
#include <stddef.h>  //  NULL

#include "transaction/types.h"

#include "ui/operations/transaction/types/ui_burn.h"
#include "ui/operations/transaction/types/ui_ipfs.h"
#include "ui/operations/transaction/types/ui_transfer.h"
#include "ui/operations/transaction/types/ui_vote.h"

#include "ui/operations/transaction/review_context.h"
#include "ui/operations/transaction/helpers/intent.h"

/* -------------------------------------------------------------------------- */

static bool configure_core_strategy(ui_tx_review_ctx_t *ctx) {
    if (ctx == NULL) {
        return false;
    }

    switch (ctx->tx_data->type) {
        case CORE_TRANSACTION_TYPE_IPFS: {
            ctx->review_intent = REVIEW_INTENT(IPFS_INTENT);
            ctx->sign_intent = SIGN_INTENT(IPFS_INTENT);

            ctx->asset.label = NULL;
            ctx->asset.record_type = RECORD_TYPE_SINGLE;
            ctx->asset.record_count = IPFS_RECORD_COUNT;
            ctx->asset.record_provider = get_ipfs_record;
            break;
        }
        case CORE_TRANSACTION_TYPE_TRANSFER: {
            ctx->review_intent = REVIEW_INTENT(TRANSFER_INTENT);
            ctx->sign_intent = SIGN_INTENT(TRANSFER_INTENT);

            ctx->asset.label = PAYMENT_LABEL;
            ctx->asset.record_type = RECORD_TYPE_PAIR;
            ctx->asset.record_count = ctx->tx_data->asset.Transfer.payment_count;
            ctx->asset.record_provider = get_payment_record;
            break;
        }
        default:
            return false;
    }

    return true;
}

static bool configure_solar_strategy(ui_tx_review_ctx_t *ctx) {
    if (ctx == NULL) {
        return false;
    }

    switch (ctx->tx_data->type) {
        case SOLAR_TRANSACTION_TYPE_BURN: {
            ctx->review_intent = REVIEW_INTENT(BURN_INTENT);
            ctx->sign_intent = SIGN_INTENT(BURN_INTENT);

            ctx->asset.label = NULL;
            ctx->asset.record_type = RECORD_TYPE_SINGLE;
            ctx->asset.record_count = BURN_RECORD_COUNT;
            ctx->asset.record_provider = get_burn_record;
            break;
        }
        case SOLAR_TRANSACTION_TYPE_VOTE: {
            if (ctx->tx_data->asset.Vote.vote_count == CANCEL_VOTE_COUNT) {
                ctx->review_intent = REVIEW_INTENT(CANCEL_VOTE_INTENT);
                ctx->sign_intent = SIGN_INTENT(CANCEL_VOTE_INTENT);
                ctx->asset.record_type = RECORD_TYPE_NONE;
            } else {
                ctx->review_intent = REVIEW_INTENT(VOTE_INTENT);
                ctx->sign_intent = SIGN_INTENT(VOTE_INTENT);
                ctx->asset.record_type = RECORD_TYPE_PAIR;
            }

            ctx->asset.label = VOTE_LABEL;
            ctx->asset.record_count = ctx->tx_data->asset.Vote.vote_count;
            ctx->asset.record_provider = get_vote_record;
            break;
        }
        default:
            return false;
    }

    return true;
}

/* -------------------------------------------------------------------------- */

/**
 * @brief Prepares the transaction review strategy based on transaction type.
 *
 * @param[in,out]   ctx The transaction context to be prepared.
 *
 * @return true if the configuration was successful, otherwise false.
 */
bool configure_review_strategy(ui_tx_review_ctx_t *ctx) {
    return (ctx->tx_data->typeGroup == TYPEGROUP_CORE) ? configure_core_strategy(ctx)
                                                       : configure_solar_strategy(ctx);
}

#endif  // if defined(SCREEN_SIZE_WALLET)
