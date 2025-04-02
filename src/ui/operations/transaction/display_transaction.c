/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#if defined(SCREEN_SIZE_WALLET)  // stax, flex

#include "ui/operations/transaction/display_transaction.h"

#include <stdbool.h>

#include <cx.h>             // CX_OK
#include <nbgl_use_case.h>  // nbgl_useCaseGenericReview, nbgl_genericContents_t
#include <io.h>             // io_send_sw

#include "app_types.h"
#include "globals.h"
#include "sw.h"

#include "crypto/crypto.h"

#include "helper/send_response.h"

#include "ui/action/choice.h"

#include "ui/operations/transaction/review_context.h"
#include "ui/operations/transaction/review.h"

/* -------------------------------------------------------------------------- */

/**
 * @brief Displays the transaction on the device and asks for user confirmation to sign.
 *
 * @return 0 if successful, otherwise an error code.
 */
int ui_display_transaction(void) {
    static const char REJECT_TRANSACTION_FOOTER_TEXT[] = "Reject";
    static const char CHOICE_CONFIRMED_TRANSACTION[] = "Transaction signed";
    static const char CHOICE_REJECT_TRANSACTION[] = "Reject transaction?";
    static const char CHOICE_REJECTED_TRANSACTION[] = "Transaction rejected";

    static nbgl_genericContents_t review_content;

    if ((G_context.req_type != CONFIRM_TRANSACTION) || (G_context.state != STATE_PARSED)) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    // Second verification of the message hash
    if (verify_message_hash() != CX_OK) {
        return io_send_sw(SW_VALIDATION_HASH_MISMATCH);
    }

    if (!initialise_choice_context(SIGNATURE_RESPONSE,
                                   CHOICE_CONFIRMED_TRANSACTION,
                                   CHOICE_REJECT_TRANSACTION,
                                   CHOICE_REJECTED_TRANSACTION)) {
        return io_send_sw(SW_DISPLAY_FAIL);
    }

    if (!init_review_context()) {
        return io_send_sw(SW_TX_PARSING_FAIL);
    }

    review_content.callbackCallNeeded = true;
    review_content.contentGetterCallback = review_content_provider;
    review_content.nbContents = REVIEW_PAGE_COUNT;

    nbgl_useCaseGenericReview(&review_content,
                              REJECT_TRANSACTION_FOOTER_TEXT,
                              handle_review_rejection_with_confirmation);

    return 0;
}

#endif  // if defined(SCREEN_SIZE_WALLET)
