/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#if defined(SCREEN_SIZE_WALLET)  // stax, flex

#include "ui/operations/transaction/modals.h"

#include <stdbool.h>
#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t
#include <string.h>  // strnlen, strlcpy, explicit_bzero

#include <nbgl_layout.h>    // nbgl_layoutHeader, nbgl_layoutFooter...
#include <nbgl_use_case.h>  // nbgl_contentTagValue_t, FIRST_USER_TOKEN
#include <os.h>             // snprintf

#include "glyphs.h"  // PUSH_ICON

#include "transaction/types.h"

#include "ui/ui_constants.h"
#include "ui/ui_text.h"

#include "ui/operations/transaction/display_error.h"
#include "ui/operations/transaction/review_constants.h"
#include "ui/operations/transaction/review_context.h"

#include "ui/operations/transaction/helpers/fee.h"
#include "ui/operations/transaction/helpers/memo.h"

/* -------------------------------------------------------------------------- */
/* ---------------------------- Modal Variables ----------------------------- */

static const char MODAL_CLOSE_TEXT[] = "Close";
static const char MODAL_DISMISS_TEXT[] = "Dismiss";

/* -------------------------------------------------------------------------- */

#if defined(TARGET_FLEX)
#define MODAL_PAGE_BAR_COUNT_MAX ((uint8_t)4u)
#elif defined(TARGET_STAX)
#define MODAL_PAGE_BAR_COUNT_MAX ((uint8_t)5u)
#endif

#if defined(TARGET_FLEX)
#define TEXT_TRUNCATION_THRESHOLD ((unsigned int)14u)
#elif defined(TARGET_STAX)
#define TEXT_TRUNCATION_THRESHOLD ((unsigned int)13u)
#endif

#define EXTENDED_ASSET_MULTIPLIER ((uint16_t)2u)
#define SUMMARY_LINE_LENGTH_MAX   ((size_t)24u)

/**
 * @brief The maximum buffer size for review pairs.
 */
#define MODAL_PAIRS_BUFFER_SIZE ((uint8_t)(MODAL_PAGE_BAR_COUNT_MAX * EXTENDED_ASSET_MULTIPLIER))

/* -------------------------------------------------------------------------- */

enum {
    BAR_1_TOKEN = FIRST_USER_TOKEN,  // 20 (Ledger SDK)
    BAR_2_TOKEN,
    BAR_3_TOKEN,
    BAR_4_TOKEN,
    BAR_5_TOKEN
};

static const int FIRST_MODAL_TOKEN = 40;

enum {
    PAGED_MODAL_CLOSE_TOKEN = FIRST_MODAL_TOKEN,
    PAGED_MODAL_NAV_TOKEN,
    DETAILS_MODAL_DISMISS_TOKEN,
    MEMO_MODAL_DISMISS_TOKEN,
};

static const uint8_t PAGED_MODAL_EXIT_INDEX = 255u;  // Ledger SDK 'EXIT_PAGE' nav token

/* -------------------------------------------------------------------------- */

static nbgl_layout_t *modal_layout_ptr;
static nbgl_contentTagValueList_t modal_pairs_list;
static nbgl_contentTagValue_t modal_pairs[MODAL_PAIRS_BUFFER_SIZE];
static ui_text_t modal_texts[MODAL_PAIRS_BUFFER_SIZE];

/* -------------------------------------------------------------------------- */
/* -------------------------- Forward Declarations -------------------------- */

static void modal_action_callback(int token, uint8_t index);

/* -------------------------------------------------------------------------- */
/* ----------------------------- Modal Helpers ------------------------------ */

static void release_layout(nbgl_layout_t *layout_ptr) {
    // Ignore the return value.
    // It's only neg (-) if layout is already NULL internally.
    (void)nbgl_layoutRelease(layout_ptr);
}

static void clear_modal_locals(void) {
    release_layout(modal_layout_ptr);
    explicit_bzero(&modal_pairs_list, sizeof(modal_pairs_list));
    explicit_bzero(&modal_pairs, sizeof(modal_pairs));
}

static void handle_modal_error(void) {
    clear_modal_locals();
    display_error_page_and_exit();
}

/* -------------------------------------------------------------------------- */

static inline uint8_t get_asset_bounds(const ui_tx_review_ctx_t *ctx) {
    return (ctx->asset.record_type == RECORD_TYPE_SINGLE)
               ? ctx->asset.record_count
               : (ctx->asset.record_count * EXTENDED_ASSET_MULTIPLIER);
}

static uint16_t get_bar_count_for_page(uint8_t page) {
    const uint16_t bar_count_U16 = (uint16_t)MODAL_PAGE_BAR_COUNT_MAX;
    uint16_t field_count = G_ui_txreview_ctx.asset.record_count;
    uint16_t count_for_page = 0u;

    for (uint8_t idx = 0u; (field_count > 0u) && (idx <= page); idx++) {
        if (idx == page) {
            count_for_page = (field_count > bar_count_U16) ? bar_count_U16 : field_count;
        }
        field_count -= (field_count > bar_count_U16) ? bar_count_U16 : field_count;
    }

    return count_for_page;
}

static uint8_t get_modal_page_count(void) {
    const uint16_t bar_count_U16 = (uint16_t)MODAL_PAGE_BAR_COUNT_MAX;
    uint16_t field_count = G_ui_txreview_ctx.asset.record_count;
    uint8_t page_count = 0u;

    while (field_count > 0u) {
        field_count = (field_count > bar_count_U16) ? (field_count - bar_count_U16) : 0u;
        page_count++;
    }

    return page_count;
}

/* -------------------------------------------------------------------------- */

static nbgl_contentTagValue_t *get_field_value(uint8_t index) {
    const uint8_t asset_bounds = get_asset_bounds(&G_ui_txreview_ctx);
    const uint8_t storage_slot = index % MODAL_PAIRS_BUFFER_SIZE;

    explicit_bzero(&modal_pairs[storage_slot], sizeof(nbgl_contentTagValue_t));

    char *item_ptr = modal_texts[storage_slot].item;
    char *value_ptr = modal_texts[storage_slot].value;

    explicit_bzero(item_ptr, MAX_ITEM_LEN);
    explicit_bzero(value_ptr, MAX_VALUE_LEN);

    bool success = false;

    if (index < asset_bounds) {
        success = G_ui_txreview_ctx.asset.record_provider(G_ui_txreview_ctx.tx_data,
                                                          item_ptr,
                                                          value_ptr,
                                                          index);
    }

    if (index == asset_bounds) {
        success = ui_set_fee(G_ui_txreview_ctx.tx_data, item_ptr, value_ptr);
    }

    if (index == (asset_bounds + FIELD_OFFSET)) {
        success = ui_set_memo(G_ui_txreview_ctx.tx_data, item_ptr, value_ptr);
    }

    if (!success) {
        handle_modal_error();
        return NULL;
    }

    modal_pairs[storage_slot].item = item_ptr;
    modal_pairs[storage_slot].value = value_ptr;

    return &modal_pairs[storage_slot];
}

/* -------------------------------------------------------------------------- */
/* ------------------------------ Paged Modal ------------------------------- */

/*
 * A paged modal is used to display multiple multi-value asset records (payments or votes).
 * Its layout contains:
 *   - a title header
 *   - a list of bar items (each containing a summary of an asset record)
 *   - a navigation footer
 * It's called from the TxInfo page when a user taps the icon to the right of a summarised asset
 * field.
 * When a bar item is tapped, the associated record's full values are displayed in a details modal.
 * When the 'X' footer navigation icon is tapped, the paged modal is closed and the user is
 * returned to the TxInfo page.
 */

static bool prepare_paged_modal_fields(
    const ui_tx_review_ctx_t *ctx,
    const uint8_t page,
    const uint16_t bar_count_for_page,
    char text_storage[MODAL_PAGE_BAR_COUNT_MAX][SUMMARY_LINE_LENGTH_MAX],
    const char *text_ptrs[MODAL_PAGE_BAR_COUNT_MAX]) {
    if (ctx == NULL) {
        return false;
    }

    const uint8_t page_offset = page * MODAL_PAGE_BAR_COUNT_MAX;
    const uint16_t pair_offset = ((uint16_t)page_offset) * EXTENDED_ASSET_MULTIPLIER;
    const uint8_t label_len = strnlen(ctx->asset.label, MAX_VALUE_LEN);
    const uint8_t available_len =
        SUMMARY_LINE_LENGTH_MAX - label_len - sizeof(TEXT_TRUNCATION_SUFFIX);
    const uint16_t total_fields = bar_count_for_page * EXTENDED_ASSET_MULTIPLIER;

    // Prepare asset record fields
    for (uint16_t idx = 0u; idx < total_fields; idx++) {
        if ((pair_offset + idx) > UINT8_MAX) {
            return false;
        }

        const nbgl_layoutTagValue_t *pair = get_field_value(pair_offset + idx);
        const char *pair_value = pair->value;
        const size_t pair_value_len = strnlen(pair_value, MAX_VALUE_LEN);

        char buffer[SUMMARY_LINE_LENGTH_MAX] = {0};

        if ((idx & 1u) == 0u) {
            const uint16_t storage_slot = (idx == 0u) ? idx : (idx >> 1);
            if (storage_slot > MODAL_PAGE_BAR_COUNT_MAX) {
                return false;
            }

            const char *value = pair_value;

            if (pair_value_len > available_len) {
                if (snprintf(buffer,
                             sizeof(buffer),
                             "%.*s%s",
                             TEXT_TRUNCATION_THRESHOLD,
                             pair_value,
                             TEXT_TRUNCATION_SUFFIX) < 0) {
                    return false;
                }
                value = buffer;
            }

            const uint8_t position_u8 = page_offset + ((uint8_t)storage_slot) + FIELD_OFFSET;
            if (snprintf(text_storage[storage_slot],
                         sizeof(text_storage[storage_slot]),
                         "%u) %s",
                         (unsigned int)position_u8,
                         value) < 0) {
                return false;
            }

            text_ptrs[storage_slot] = text_storage[storage_slot];
        }
    }

    return true;
}

static void configure_paged_modal_footer(const uint8_t page,
                                         nbgl_layoutFooter_t *footer_description) {
    const uint8_t modal_page_count = get_modal_page_count();
    const uint8_t footer_nav_threshold = 1u;

    footer_description->separationLine = true;

    // Single-page modals (page_count == 1) use a text button footer.
    if (modal_page_count == footer_nav_threshold) {
        footer_description->type = FOOTER_SIMPLE_TEXT;
        footer_description->button.icon = NULL;
        footer_description->button.style = NO_BORDER;
        footer_description->button.fittingContent = false;
        footer_description->button.onBottom = false;
        footer_description->button.token = PAGED_MODAL_CLOSE_TOKEN;
        footer_description->button.tuneId = TUNE_TAP_CASUAL;
        footer_description->simpleText.text = MODAL_CLOSE_TEXT;
        footer_description->simpleText.token = PAGED_MODAL_CLOSE_TOKEN;
        footer_description->simpleText.tuneId = TUNE_TAP_CASUAL;
    }
    // Multi-page modals (page_count > 1) use a nav footer.
    else {
        footer_description->type = FOOTER_NAV;
        footer_description->navigation.activePage = page;
        footer_description->navigation.nbPages = modal_page_count;
        footer_description->navigation.withExitKey = true;
        footer_description->navigation.withBackKey = true;
        footer_description->navigation.withPageIndicator = false;
        footer_description->navigation.token = PAGED_MODAL_NAV_TOKEN;
        footer_description->navigation.tuneId = TUNE_TAP_CASUAL;
    }
}

static bool draw_paged_modal(const uint8_t page,
                             const uint16_t bar_count_for_page,
                             const char *title,
                             const char *text_ptrs[MODAL_PAGE_BAR_COUNT_MAX]) {
    static const uint8_t bar_tokens_table[] = {BAR_1_TOKEN,
                                               BAR_2_TOKEN,
                                               BAR_3_TOKEN,
                                               BAR_4_TOKEN,
                                               BAR_5_TOKEN};
    const int separator_line_threshold = 10;
    int available_height = SCREEN_HEIGHT;

    if ((title == NULL) || (text_ptrs == NULL)) {
        return false;
    }

    const nbgl_layoutDescription_t layout_description = {
        .modal = true,
        .withLeftBorder = false,
        .onActionCallback = &modal_action_callback};

    const nbgl_layoutHeader_t header_description = {.type = HEADER_TITLE,
                                                    .separationLine = true,
                                                    .title.text = title};

    nbgl_layoutFooter_t footer_description;
    configure_paged_modal_footer(page, &footer_description);

    modal_layout_ptr = nbgl_layoutGet(&layout_description);
    if (modal_layout_ptr == NULL) {
        return false;
    }

    const int header_height = nbgl_layoutAddHeader(modal_layout_ptr, &header_description);
    if (header_height < 0) {
        return false;
    }
    available_height -= header_height;

    const int footer_height = nbgl_layoutAddExtendedFooter(modal_layout_ptr, &footer_description);
    if (footer_height < 0) {
        return false;
    }
    available_height -= footer_height;

    // Add bars with separator lines to the page iteratively
    for (uint8_t idx = 0u; idx < (uint8_t)bar_count_for_page; idx++) {
        nbgl_layoutBar_t bar;
        (void)memset(&bar, 0, sizeof(bar));
        bar.text = text_ptrs[idx];
        bar.subText = NULL;
        bar.iconRight = &PUSH_ICON;
        bar.iconLeft = NULL;
        bar.token = bar_tokens_table[idx];
        bar.tuneId = TUNE_TAP_CASUAL;
        bar.large = false;
        bar.inactive = false;

        const int bar_height = nbgl_layoutAddTouchableBar(modal_layout_ptr, &bar);
        if (bar_height < 0) {
            return false;
        }
        available_height -= bar_height;

        if (available_height > separator_line_threshold) {
            if (nbgl_layoutAddSeparationLine(modal_layout_ptr) < 0) {
                return false;
            }
        }
    }

    if (nbgl_layoutDraw(modal_layout_ptr) != 0) {
        return false;
    }

    return true;
}

static void display_modal_page(uint8_t page) {
    static char asset_modal_title[SUMMARY_LINE_LENGTH_MAX] = {0};
    static char text_storage[MODAL_PAGE_BAR_COUNT_MAX][SUMMARY_LINE_LENGTH_MAX] = {0};
    static const char *text_ptrs[MODAL_PAGE_BAR_COUNT_MAX] = {0};

    const uint16_t bar_count_for_page = get_bar_count_for_page(page);
    if (bar_count_for_page == 0u) {
        handle_modal_error();
        return;
    }

    const uint8_t page_offset = page * MODAL_PAGE_BAR_COUNT_MAX;
    const uint8_t first_pos = page_offset + FIELD_OFFSET;
    const uint16_t last_pos = ((uint16_t)page_offset) + bar_count_for_page;
    const uint8_t storage_slot = page % MODAL_PAIRS_BUFFER_SIZE;
    const uint8_t base_index = page * ((uint8_t)EXTENDED_ASSET_MULTIPLIER);

    // Title for a multi-page model (e.g., "Votes 1-5 of 20").
    if (snprintf(asset_modal_title,
                 SUMMARY_LINE_LENGTH_MAX,
                 "%.*ss %u-%u of %u",
                 strnlen(G_ui_txreview_ctx.asset.label, MAX_VALUE_LEN),
                 G_ui_txreview_ctx.asset.label,
                 (unsigned int)first_pos,
                 (unsigned int)last_pos,
                 (unsigned int)G_ui_txreview_ctx.asset.record_count) < 0) {
        handle_modal_error();
        return;
    }

    if (!prepare_paged_modal_fields(&G_ui_txreview_ctx,
                                    page,
                                    bar_count_for_page,
                                    text_storage,
                                    text_ptrs)) {
        explicit_bzero(asset_modal_title, SUMMARY_LINE_LENGTH_MAX);
        handle_modal_error();
        return;
    }

    modal_pairs_list.pairs = &modal_pairs[storage_slot];
    modal_pairs_list.callback = NULL;
    modal_pairs_list.nbPairs = (uint8_t)bar_count_for_page;
    modal_pairs_list.startIndex = base_index;
    modal_pairs_list.nbMaxLinesForValue = 0u;
    modal_pairs_list.smallCaseForValue = true;
    modal_pairs_list.wrapping = true;

    if (!draw_paged_modal(page, bar_count_for_page, asset_modal_title, text_ptrs)) {
        explicit_bzero(asset_modal_title, SUMMARY_LINE_LENGTH_MAX);
        handle_modal_error();
        return;
    }
}

/* -------------------------------------------------------------------------- */
/* ----------------------------- Details Modal ------------------------------ */

/*
 * A details modal is used to display a single multi-value asset record (a payment or vote).
 * Its layout contains:
 *   - multiple tag-value pairs
 *   - a "Dismiss" button in the footer
 * It is called from a paged modal when a user taps the bar item for a specific record.
 * When "Dismiss" is tapped, the details modal is closed and the user is returned to the paged
 * modal.
 */

static bool draw_details_modal(char title[SUMMARY_LINE_LENGTH_MAX]) {
    const nbgl_layoutDescription_t layout_description = {
        .modal = true,
        .withLeftBorder = false,
        .onActionCallback = &modal_action_callback};

    const nbgl_layoutHeader_t header_description = {.type = HEADER_TITLE,
                                                    .separationLine = true,
                                                    .title.text = title};

    const nbgl_layoutFooter_t footer_description = {.type = FOOTER_SIMPLE_TEXT,
                                                    .separationLine = true,
                                                    .simpleText.text = MODAL_DISMISS_TEXT,
                                                    .simpleText.token = DETAILS_MODAL_DISMISS_TOKEN,
                                                    .simpleText.tuneId = TUNE_TAP_CASUAL};

    modal_layout_ptr = nbgl_layoutGet(&layout_description);
    if (modal_layout_ptr == NULL) {
        return false;
    }

    if (nbgl_layoutAddHeader(modal_layout_ptr, &header_description) < 0) {
        return false;
    }

    if (nbgl_layoutAddTagValueList(modal_layout_ptr, &modal_pairs_list) < 0) {
        return false;
    }

    if (nbgl_layoutAddExtendedFooter(modal_layout_ptr, &footer_description) < 0) {
        return false;
    }

    if (nbgl_layoutDraw(modal_layout_ptr) != 0) {
        return false;
    }

    return true;
}

static void display_details_modal(const ui_tx_review_ctx_t *ctx, uint8_t token) {
    if (ctx == NULL) {
        handle_modal_error();
        return;
    }

    static char details_modal_title[SUMMARY_LINE_LENGTH_MAX] = {0};

    // 'FIRST_USER_TOKEN' is used (per Ledger guidelines) and the following
    // check should not be needed, we do it anyway as an extra precaution.
    const int bar_token_int = (int)token - FIRST_USER_TOKEN;
    if (bar_token_int < 0) {
        handle_modal_error();
        return;
    }

    const uint8_t bar_token = (uint8_t)bar_token_int;
    const uint8_t page_offset = ctx->asset.current_modal_page * MODAL_PAGE_BAR_COUNT_MAX;
    const uint8_t details_modal_pair_count = 2u;

    if (modal_layout_ptr != NULL) {
        release_layout(modal_layout_ptr);
    }

    // Title for detail modal with only one field (e.g., "Payment 1 of 20").
    if (snprintf(details_modal_title,
                 SUMMARY_LINE_LENGTH_MAX,
                 "%.*s %u of %u",
                 strnlen(ctx->asset.label, MAX_VALUE_LEN),
                 ctx->asset.label,
                 (unsigned int)page_offset + bar_token + FIELD_OFFSET,
                 (unsigned int)ctx->asset.record_count) < 0) {
        handle_modal_error();
        return;
    }

    const uint16_t field_offset = ((uint16_t)bar_token) * EXTENDED_ASSET_MULTIPLIER;

    modal_pairs_list.pairs = &modal_pairs[field_offset];
    modal_pairs_list.callback = NULL;
    modal_pairs_list.nbPairs = details_modal_pair_count;
    modal_pairs_list.startIndex = 0u;
    modal_pairs_list.nbMaxLinesForValue = 0u;
    modal_pairs_list.smallCaseForValue = true;
    modal_pairs_list.wrapping = true;

    if (!draw_details_modal(details_modal_title)) {
        handle_modal_error();
        return;
    }
}

/* -------------------------------------------------------------------------- */
/* ------------------------------- Memo Modal ------------------------------- */

/*
 * A memo modal is used to display the full text of a memo that is too long to fit on the TxInfo
 * page.
 * Its layout contains:
 *   - a single tag-value pair
 *   - a "Dismiss" button in the footer
 * It is called from the TxInfo page when a user taps the icon to the right of a truncated memo
 * field.
 * When "Dismiss" is tapped, the memo modal is closed and the user is returned to the TxInfo
 * page.
 */

static bool draw_memo_modal(void) {
    const nbgl_layoutDescription_t layout_description = {
        .modal = true,
        .withLeftBorder = false,
        .onActionCallback = &modal_action_callback};

    const nbgl_layoutHeader_t header_description = {.type = HEADER_EMPTY,
                                                    .separationLine = false,
                                                    .emptySpace.height = SMALL_CENTERING_HEADER};

    const nbgl_layoutFooter_t footer_description = {.type = FOOTER_SIMPLE_TEXT,
                                                    .separationLine = true,
                                                    .simpleText.text = MODAL_DISMISS_TEXT,
                                                    .simpleText.token = MEMO_MODAL_DISMISS_TOKEN,
                                                    .simpleText.tuneId = TUNE_TAP_CASUAL};

    modal_layout_ptr = nbgl_layoutGet(&layout_description);
    if (modal_layout_ptr == NULL) {
        return false;
    }

    if (nbgl_layoutAddHeader(modal_layout_ptr, &header_description) < 0) {
        return false;
    }

    if (nbgl_layoutAddTagValueList(modal_layout_ptr, &modal_pairs_list) < 0) {
        return false;
    }

    if (nbgl_layoutAddExtendedFooter(modal_layout_ptr, &footer_description) < 0) {
        return false;
    }

    if (nbgl_layoutDraw(modal_layout_ptr) != 0) {
        return false;
    }

    return true;
}

static void display_memo_modal(const ui_tx_review_ctx_t *ctx) {
    if (ctx == NULL) {
        handle_modal_error();
        return;
    }

    // A Solar transfer tx can contain an asset record count that exeeds UINT8_MAX.
    // The Solar app does not currently support a count > UINT8_MAX.
    // While limitations are enforeced elsewhere that prevent this,
    // we add a paranoid check here as an extra precaution.
    const uint16_t memo_idx = get_asset_bounds(ctx) + FIELD_OFFSET;
    if (memo_idx > UINT8_MAX) {
        handle_modal_error();
        return;
    }

    const uint8_t memo_idx_u8 = (uint8_t)memo_idx;
    const uint8_t memo_modal_pair_count = 1u;

    modal_pairs_list.pairs = NULL;
    modal_pairs_list.callback = get_field_value;
    modal_pairs_list.nbPairs = memo_modal_pair_count;
    modal_pairs_list.startIndex = memo_idx_u8;
    modal_pairs_list.nbMaxLinesForValue = 0u;
    modal_pairs_list.smallCaseForValue = true;
    modal_pairs_list.wrapping = true;

    if (!draw_memo_modal()) {
        handle_modal_error();
        return;
    }
}

/* -------------------------------------------------------------------------- */
/* ------------------------- Modal Display Entry Fn ------------------------- */

enum { TXINFO_ITEM_INDEX_0 = 0, TXINFO_ITEM_INDEX_1, TXINFO_ITEM_INDEX_2 };

void display_modal(ui_tx_review_ctx_t *ctx, uint8_t index) {
    // Extended asset field is tapped.
    if ((index == TXINFO_ITEM_INDEX_0) && (ctx->asset.record_type == RECORD_TYPE_PAIR)) {
        ctx->asset.current_modal_page = 0u;
        display_modal_page(ctx->asset.current_modal_page);
    }

    // Long memo field is tapped.
    if ((index >= TXINFO_ITEM_INDEX_1) && (ctx->tx_data->memo_len > MEMO_TRUNCACTION_THRESHOLD)) {
        display_memo_modal(ctx);
    }
}

/* -------------------------------------------------------------------------- */
/* ----------------------------- Modal Callbacks ---------------------------- */

static void handle_paged_modal_navigation(uint8_t index) {
    release_layout(modal_layout_ptr);

    // Back/Next nav button is tapped.
    if (index < PAGED_MODAL_EXIT_INDEX) {
        G_ui_txreview_ctx.asset.current_modal_page = index;
        display_modal_page(index);
    }

    // Dismiss the modal and return to the txinfo page.
    if (index == PAGED_MODAL_EXIT_INDEX) {
        G_ui_txreview_ctx.asset.current_modal_page = 0u;
        nbgl_screenRedraw();
    }
}

static void modal_action_callback(int token, uint8_t index) {
    if ((token >= BAR_1_TOKEN) && (token <= BAR_5_TOKEN)) {
        display_details_modal(&G_ui_txreview_ctx, token);
    }

    if (token == PAGED_MODAL_NAV_TOKEN) {
        handle_paged_modal_navigation(index);
    }

    if (token == DETAILS_MODAL_DISMISS_TOKEN) {
        release_layout(modal_layout_ptr);
        display_modal_page(G_ui_txreview_ctx.asset.current_modal_page);
    }

    if (token == PAGED_MODAL_CLOSE_TOKEN) {
        release_layout(modal_layout_ptr);
        nbgl_screenRedraw();
    }

    if (token == MEMO_MODAL_DISMISS_TOKEN) {
        release_layout(modal_layout_ptr);
        nbgl_screenRedraw();
    }
}

#endif  // if defined(SCREEN_SIZE_WALLET)
