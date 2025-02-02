/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#if defined(SCREEN_SIZE_WALLET)  // stax, flex

#include "ui/operations/transaction/review.h"

#include <stdbool.h>
#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t
#include <string.h>  // strlcpy

#include <nbgl_obj.h>       // for CHEVRON_NEXT_ICON
#include <nbgl_use_case.h>  // FIRST_USER_TOKEN, nbgl_contentTagValue_t
#include <os.h>             // explicit_bzero, snprintf

#include "glyphs.h"  // C_logo_solar_64px

#include "transaction/types.h"

#include "ui/ui_constants.h"
#include "ui/ui_text.h"

#include "ui/action/choice.h"

#include "ui/operations/transaction/display_error.h"

#include "ui/operations/transaction/helpers/fee.h"
#include "ui/operations/transaction/helpers/memo.h"

#include "ui/operations/transaction/modals.h"
#include "ui/operations/transaction/review_constants.h"
#include "ui/operations/transaction/review_context.h"

/* -------------------------------------------------------------------------- */
/* ---------------------------- Review Variables ---------------------------- */

static const char REVIEW_INTENT_PAGE_SUBTEXT[] = "Swipe to review";
static const char SIGN_ACTION_TEXT[] = "Hold to sign";

/* -------------------------------------------------------------------------- */

#define TXINFO_FIELD_COUNT 4

static const uint16_t TXINFO_FIELD_COUNT_ZERO = 0u;
static const uint8_t TXINFO_FIELD_COUNT_INCREMENT = 1u;

static const uint8_t TXINFO_FIELD_POS_ZERO = 0u;
static const uint8_t TXINFO_FIELD_POS_MAX = 3u;

/* -------------------------------------------------------------------------- */

enum { REVIEW_INTENT_PAGE = 0, TXINFO_PAGE, SIGN_PAGE };
enum { TXINFO_PAGE_ACTION_TOKEN = FIRST_USER_TOKEN, SIGN_PAGE_ACTION_TOKEN };

/* -------------------------------------------------------------------------- */

static ui_text_t txinfo_texts[TXINFO_FIELD_COUNT];
static nbgl_contentTagValue_t txinfo_pairs[TXINFO_FIELD_COUNT];

/* -------------------------------------------------------------------------- */
/* ----------------------------- Review Helpers ----------------------------- */

static void clear_review_locals(void) {
    explicit_bzero(&txinfo_texts, sizeof(txinfo_texts));
    explicit_bzero(&txinfo_pairs, sizeof(txinfo_pairs));
}

static void handle_review_error(void) {
    clear_review_locals();
    display_error_page_and_exit();
}

/* -------------------------------------------------------------------------- */
/* ------------------------- Review Page Callbacks -------------------------- */

static void txinfo_page_action_callback(int token, uint8_t index, int page) {
    // Extended asset field was tapped.
    if ((page == TXINFO_PAGE) && (token == TXINFO_PAGE_ACTION_TOKEN) &&
        (index <= TXINFO_FIELD_POS_MAX)) {
        display_modal(&G_ui_txreview_ctx, index);
    }
}

static void sign_page_action_callback(int token, uint8_t index, int page) {
    (void)index;  // UNUSED
    if ((page == SIGN_PAGE) && (token == SIGN_PAGE_ACTION_TOKEN)) {
        handle_review_choice(true);
    }
}

/* -------------------------------------------------------------------------- */
/* -------------------------- TxInfo Page Helpers --------------------------- */

static uint8_t set_txinfo_asset_field_pair(ui_tx_review_ctx_t *ctx) {
    // One asset record pair needs to be displayed.
    // (single-payment/vote transactions)
    if (ctx->asset.record_count < ASSET_MODAL_THRESHOLD) {
        const uint16_t asset_record_pair_count = 2u;

        for (uint16_t idx = 0u; idx < asset_record_pair_count; idx++) {
            (void)G_ui_txreview_ctx.asset.record_provider(ctx->tx_data,
                                                          txinfo_texts[idx].item,
                                                          txinfo_texts[idx].value,
                                                          idx);

            txinfo_pairs[idx].item = txinfo_texts[idx].item;
            txinfo_pairs[idx].value = txinfo_texts[idx].value;

#if defined(TARGET_FLEX)
            const uint16_t line_limit = 2u;

            // Check that an address or username does not wrap to a 3rd line on flex devices when
            // the optional tx memo contains text, which could result in the memo field being pushed
            // outside the screen's bounds. While unlikely to happen with a payment address — and
            // impossible for a username (20 char limit), the transaction is gracefully rejected in
            // the event that a 3rd line would be used.
            if ((idx == 0u) && (ctx->tx_data->memo_len > 0u) &&
                (nbgl_getTextNbLinesInWidth(SMALL_BOLD_FONT,
                                            txinfo_texts[idx].value,
                                            AVAILABLE_WIDTH,
                                            true) > line_limit)) {
                handle_review_error();
                return TXINFO_FIELD_COUNT_ZERO;
            }
#endif
        }

        return asset_record_pair_count;
    }

    // One tappable asset field needs to be displayed.
    // All asset records will be displayed via modal.
    static char extended_content_title[MAX_ITEM_LEN] = {0};
    static char extended_content_value[MAX_ITEM_LEN] = {0};

    if (snprintf(extended_content_title, MAX_ITEM_LEN, "%ss", ctx->asset.label) < 0) {
        handle_review_error();
        return TXINFO_FIELD_COUNT_ZERO;
    }

    if (snprintf(extended_content_value,
                 MAX_ITEM_LEN,
                 "%u %ss",
                 (unsigned int)ctx->asset.record_count,
                 ctx->asset.label) < 0) {
        handle_review_error();
        return TXINFO_FIELD_COUNT_ZERO;
    }

    txinfo_pairs[TXINFO_FIELD_POS_ZERO].item = extended_content_title;
    txinfo_pairs[TXINFO_FIELD_POS_ZERO].value = extended_content_value;
    txinfo_pairs[TXINFO_FIELD_POS_ZERO].valueIcon = &CHEVRON_NEXT_ICON;

    return TXINFO_FIELD_COUNT_INCREMENT;
}

static uint8_t set_txinfo_asset_field(ui_tx_review_ctx_t *ctx) {
    // No asset records need to be displayed.
    if (ctx->asset.record_type == RECORD_TYPE_NONE) {
        return TXINFO_FIELD_COUNT_ZERO;
    }

    // One asset record needs to be displayed.
    if (ctx->asset.record_type == RECORD_TYPE_SINGLE) {
        (void)G_ui_txreview_ctx.asset.record_provider(ctx->tx_data,
                                                      txinfo_texts[TXINFO_FIELD_POS_ZERO].item,
                                                      txinfo_texts[TXINFO_FIELD_POS_ZERO].value,
                                                      TXINFO_FIELD_POS_ZERO);

        txinfo_pairs[TXINFO_FIELD_POS_ZERO].item = txinfo_texts[TXINFO_FIELD_POS_ZERO].item;
        txinfo_pairs[TXINFO_FIELD_POS_ZERO].value = txinfo_texts[TXINFO_FIELD_POS_ZERO].value;

        return TXINFO_FIELD_COUNT_INCREMENT;
    }

    // One or more pair-type asset records need to be displayed.
    if (ctx->asset.record_type == RECORD_TYPE_PAIR) {
        return set_txinfo_asset_field_pair(ctx);
    }

    handle_review_error();
    return TXINFO_FIELD_COUNT_ZERO;
}

static uint8_t set_txinfo_fee_field(const ui_tx_review_ctx_t *ctx, const uint8_t offset) {
    if (!ui_set_fee(ctx->tx_data, txinfo_texts[offset].item, txinfo_texts[offset].value)) {
        return TXINFO_FIELD_COUNT_ZERO;
    }

    txinfo_pairs[offset].item = txinfo_texts[offset].item;
    txinfo_pairs[offset].value = txinfo_texts[offset].value;

    return TXINFO_FIELD_COUNT_INCREMENT;
}

static uint8_t set_txinfo_memo_field(const ui_tx_review_ctx_t *ctx, const uint8_t offset) {
    if (ctx->tx_data->memo_len == 0u) {
        return TXINFO_FIELD_COUNT_ZERO;
    }

    if (!ui_set_memo(ctx->tx_data, txinfo_texts[offset].item, txinfo_texts[offset].value)) {
        return TXINFO_FIELD_COUNT_ZERO;
    }

    txinfo_pairs[offset].item = txinfo_texts[offset].item;
    txinfo_pairs[offset].value = txinfo_texts[offset].value;

    // Truncate long memos. The full memo will be displayed via modal.
    if (ctx->tx_data->memo_len > MEMO_TRUNCACTION_THRESHOLD) {
        (void)strlcpy(&txinfo_texts[offset].value[MEMO_TRUNCACTION_THRESHOLD],
                      TEXT_TRUNCATION_SUFFIX,
                      MAX_VALUE_LEN);
        txinfo_pairs[offset].valueIcon = &CHEVRON_NEXT_ICON;
    }

    return TXINFO_FIELD_COUNT_INCREMENT;
}

static uint8_t set_txinfo_page_fields(ui_tx_review_ctx_t *ctx) {
    uint8_t field_count = 0u;
    field_count += set_txinfo_asset_field(ctx);
    field_count += set_txinfo_fee_field(ctx, field_count);
    field_count += set_txinfo_memo_field(ctx, field_count);

    return field_count;
}

/* -------------------------------------------------------------------------- */
/* ------------------------ Transaction Review Pages ------------------------ */

static void set_review_intent_page(const ui_tx_review_ctx_t *ctx, nbgl_content_t *content) {
    content->type = CENTERED_INFO;
    content->contentActionCallback = NULL;
    content->content.centeredInfo.icon = &C_logo_solar_64px;
    content->content.centeredInfo.style = LARGE_CASE_GRAY_INFO;
    content->content.centeredInfo.offsetY = 0u;
    content->content.centeredInfo.text1 = ctx->review_intent;
    content->content.centeredInfo.text2 = NULL;
    content->content.centeredInfo.text3 = REVIEW_INTENT_PAGE_SUBTEXT;
}

static void set_txinfo_page(ui_tx_review_ctx_t *ctx, nbgl_content_t *content) {
    const uint8_t field_count = set_txinfo_page_fields(ctx);

    content->type = TAG_VALUE_LIST;
    content->content.tagValueList.pairs = txinfo_pairs;
    content->content.tagValueList.callback = NULL;
    content->content.tagValueList.nbPairs = field_count;
    content->content.tagValueList.startIndex = 0u;
    content->content.tagValueList.nbMaxLinesForValue = 0u;
    content->content.tagValueList.smallCaseForValue = true;
    content->content.tagValueList.wrapping = true;

    // Extended asset records and long memos are displayed using modals.
    if ((ctx->asset.record_type == RECORD_TYPE_PAIR) ||
        (ctx->tx_data->memo_len > MEMO_TRUNCACTION_THRESHOLD)) {
        content->contentActionCallback = txinfo_page_action_callback;
        content->content.tagValueList.token = TXINFO_PAGE_ACTION_TOKEN;
    }
}

static void set_sign_page(const ui_tx_review_ctx_t *ctx, nbgl_content_t *content) {
    content->type = INFO_LONG_PRESS;
    content->contentActionCallback = sign_page_action_callback;
    content->content.infoLongPress.text = ctx->sign_intent;
    content->content.infoLongPress.icon = &C_logo_solar_64px;
    content->content.infoLongPress.longPressText = SIGN_ACTION_TEXT;
    content->content.infoLongPress.longPressToken = SIGN_PAGE_ACTION_TOKEN;
}

/* -------------------------------------------------------------------------- */

void review_content_provider(uint8_t page, nbgl_content_t *content) {
    clear_review_locals();

    switch (page) {
        case REVIEW_INTENT_PAGE:
            set_review_intent_page(&G_ui_txreview_ctx, content);
            break;
        case TXINFO_PAGE:
            set_txinfo_page(&G_ui_txreview_ctx, content);
            break;
        case SIGN_PAGE:
            set_sign_page(&G_ui_txreview_ctx, content);
            break;
        default:
            handle_review_error();
            break;
    }
}

#endif  // if defined(SCREEN_SIZE_WALLET)
