/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#if defined(SCREEN_SIZE_WALLET)  // stax, flex

#include "ui/operations/public_key/display_pubkey.h"

#include <stdbool.h>
#include <stddef.h>  // NULL
#include <stdint.h>  // uint*_t

#include <io.h>             // io_send_sw
#include <nbgl_content.h>   // nbgl_contentTagValueList_t...
#include <nbgl_page.h>      // nbgl_pageInfoLongPress_t
#include <nbgl_use_case.h>  // nbgl_useCaseReviewStart...

#include "glyphs.h"  // C_Check_Circle_64px, C_logo_solar_64px

#include "app_types.h"
#include "constants.h"
#include "globals.h"
#include "sw.h"

#include "helper/send_response.h"

#include "ui/ui_constants.h"
#include "ui/ui_utils.h"

#include "ui/action/choice.h"

/* -------------------------------------------------------------------------- */

static const char PUBKEY_CANCEL_TEXT[] = "Cancel";

static nbgl_contentTagValueList_t pair_list;

#define PUBKEY_PAIR_COUNT ((uint8_t)1u)

/* -------------------------------------------------------------------------- */

/**
 * @brief Display the public key and confirmation screen.
 */
static void handle_review_continue(void) {
    static const char PUBKEY_CONFIRM_BUTTON_TEXT[] = "Confirm";
    static const char PUBKEY_CONFIRM_INTENT[] = "Confirm public key";

    // Confirmation / long press page
    static const nbgl_pageInfoLongPress_t LONG_PRESS_INFO_PUBKEY = {
        .text = PUBKEY_CONFIRM_INTENT,
        .icon = &C_Check_Circle_64px,
        .longPressText = PUBKEY_CONFIRM_BUTTON_TEXT,
    };

    nbgl_useCaseStaticReviewLight(&pair_list,
                                  &LONG_PRESS_INFO_PUBKEY,
                                  PUBKEY_CANCEL_TEXT,
                                  handle_review_choice);
}

/**
 * @brief Displays the public key on the device and asks for user confirmation.
 *
 * @return 0 if successful, otherwise an error code.
 */
int ui_display_public_key(void) {
    static const char PUBKEY_LABEL[] = "Public Key";
    static const char PUBKEY_REVIEW_INTENT[] = "Review public key";
    static const char CHOICE_PUBKEY_CONFIRMED[] = "Public key confirmed";
    static const char CHOICE_PUBKEY_REJECT[] = "Reject public key?";
    static const char CHOICE_PUBKEY_REJECTED[] = "Public key rejected";

    static char ui_pubkey_text[PUBKEY_CHAR_LEN + NULL_TERMINATOR_LEN] = {0};
    static nbgl_contentTagValue_t pairs[PUBKEY_PAIR_COUNT];

    if ((G_context.req_type != CONFIRM_PUBLICKEY) || (G_context.state != STATE_NONE)) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    if (!initialise_choice_context(PUBKEY_RESPONSE,
                                   CHOICE_PUBKEY_CONFIRMED,
                                   CHOICE_PUBKEY_REJECT,
                                   CHOICE_PUBKEY_REJECTED)) {
        return io_send_sw(SW_PUBKEY_PARSING_FAIL);
    }

    if (format_hex_lower(G_context.pk_info.raw_public_key,
                         PUBKEY_BYTE_LEN,
                         ui_pubkey_text,
                         PUBKEY_CHAR_LEN + NULL_TERMINATOR_LEN) < 0) {
        return io_send_sw(SW_PUBKEY_PARSING_FAIL);
    }

    pairs[0].item = PUBKEY_LABEL;
    pairs[0].value = ui_pubkey_text;

    // Setup list
    pair_list.pairs = pairs;
    pair_list.nbPairs = PUBKEY_PAIR_COUNT;
    pair_list.nbMaxLinesForValue = 0u;
    pair_list.smallCaseForValue = true;
    pair_list.wrapping = true;

    nbgl_useCaseReviewStart(&C_logo_solar_64px,
                            PUBKEY_REVIEW_INTENT,
                            NULL,
                            PUBKEY_CANCEL_TEXT,
                            handle_review_continue,
                            handle_review_silent_rejection);

    return 0;
}

#endif  // if defined(SCREEN_SIZE_WALLET)
