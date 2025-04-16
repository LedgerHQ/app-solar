/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#if defined(SCREEN_SIZE_WALLET)  // stax, flex

#include "ui/operations/message/display_message.h"

#include <stdbool.h>
#include <stddef.h>  // NULL
#include <stdint.h>  // uint*_t

#include <cx.h>             // CX_OK
#include <io.h>             // io_send_sw
#include <nbgl_content.h>   // nbgl_contentTagValueList_t...
#include <nbgl_use_case.h>  // nbgl_useCaseReview, TYPE_MESSAGE

#include <glyphs.h>  // C_Review_64px

#include "app_types.h"
#include "globals.h"
#include "sw.h"

#include "crypto/crypto.h"

#include "helper/send_response.h"

#include "ui/action/choice.h"

/* -------------------------------------------------------------------------- */

#define MESSAGE_PAIR_COUNT ((uint8_t)1u)

/* -------------------------------------------------------------------------- */

/**
 * @brief Displays the message on the device and ask for user confirmation to sign.
 *
 * @return 0 if successful, otherwise an error code.
 */
int ui_display_message(void) {
    static const char MESSAGE_NAME[] = "Message";
    static const char REVIEW_INTENT_MESSAGE[] = "Review off-chain message";
    static const char SIGN_INTENT_MESSAGE[] = "Sign off-chain message";

    static const char CHOICE_SIGNED_MESSAGE[] = "Message signed";
    static const char CHOICE_REJECT_MESSAGE[] = "Reject message?";
    static const char CHOICE_REJECTED_MESSAGE[] = "Message rejected";

    static const int MESSAGE_LENGTH_SIZE = 2;

    static nbgl_contentTagValue_t pairs[MESSAGE_PAIR_COUNT];
    static nbgl_contentTagValueList_t pair_list;

    if ((G_context.req_type != CONFIRM_MESSAGE) || (G_context.state != STATE_PARSED)) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    // Second verification of the message hash
    if (verify_message_hash() != CX_OK) {
        return io_send_sw(SW_VALIDATION_HASH_MISMATCH);
    }

    if (!initialise_choice_context(SIGNATURE_RESPONSE,
                                   CHOICE_SIGNED_MESSAGE,
                                   CHOICE_REJECT_MESSAGE,
                                   CHOICE_REJECTED_MESSAGE)) {
        return io_send_sw(SW_DISPLAY_FAIL);
    }

    pairs[0].item = MESSAGE_NAME;
    pairs[0].value = (const char *)&G_context.tx_info.raw_tx[MESSAGE_LENGTH_SIZE];

    // Setup list
    pair_list.pairs = pairs;
    pair_list.nbPairs = MESSAGE_PAIR_COUNT;
    pair_list.nbMaxLinesForValue = 0u;
    pair_list.wrapping = true;

    nbgl_useCaseReview(TYPE_MESSAGE,
                       &pair_list,
                       &C_Review_64px,
                       REVIEW_INTENT_MESSAGE,
                       NULL,
                       SIGN_INTENT_MESSAGE,
                       handle_review_choice);

    return 0;
}

#endif  // if defined(SCREEN_SIZE_WALLET)
