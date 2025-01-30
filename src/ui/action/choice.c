/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#if defined(SCREEN_SIZE_WALLET)  // stax, flex

#include "ui/action/choice.h"

#include <stdbool.h>
#include <string.h>  // NULL

#include <nbgl_use_case.h>  // nbgl_useCaseStatus, nbgl_useCaseConfirm
#include <os.h>             // explicit_bzero

#include "helper/send_response.h"

#include "ui/action/validate.h"

#include "ui/menu/display_menu.h"

/* -------------------------------------------------------------------------- */

static ui_choice_ctx_t choice_context;
static helper_response_t response_type;  // ADDRESS, PUBKEY or SIGNATURE_RESPONSE

/* -------------------------------------------------------------------------- */

/**
 * @brief Resets the review choice context.
 */
static void ui_choice_context_destroy(void) {
    explicit_bzero(&choice_context, sizeof(choice_context));
}

/**
 * @brief Handles confirmation of the review choice.
 */
static void handle_confirmation(void) {
    ui_validate(response_type, true);
    nbgl_useCaseStatus(choice_context.confirmed_text, true, ui_menu_main);
    ui_choice_context_destroy();
}

/**
 * @brief Handles rejection of the review choice.
 */
static void handle_rejection(void) {
    ui_validate(response_type, false);
    nbgl_useCaseStatus(choice_context.rejected_text, false, ui_menu_main);
    ui_choice_context_destroy();
}

/* -------------------------------------------------------------------------- */

/**
 * @brief Initialises the review choice context.
 *
 * @param[in]   type                The type of helper response.
 * @param[in]   choice_confirmed    The text to display on the "confirmed" status screen.
 * @param[in]   choice_rejectThe    The text to display on the "reject" confirmation dialog.
 * @param[in]   choice_rejected     The text to display on the "rejected" status screen.
 *
 * @return true if initialisation was successful, otherwise false.
 */
bool initialise_choice_context(helper_response_t type,
                               const char *choice_confirmed,
                               const char *choice_reject,
                               const char *choice_rejected) {
    if ((choice_confirmed == NULL) || (choice_reject == NULL) || (choice_rejected == NULL)) {
        return false;
    }

    ui_choice_context_destroy();

    response_type = type;
    choice_context.confirmed_text = choice_confirmed;
    choice_context.reject_text = choice_reject;
    choice_context.rejected_text = choice_rejected;

    return true;
}

/**
 * @brief Handles the approval or rejection UI of a review.
 *
 * @param[in]   confirm User choice (either approved or rejected).
 */
void handle_review_choice(bool confirm) {
    if (confirm) {
        handle_confirmation();
    } else {
        handle_rejection();
    }
}

/**
 * @brief Handles the rejection UI of a review with an additional screen to confirm rejection.
 */
void handle_review_rejection_with_confirmation(void) {
    const char choice_reject[12] = "Yes, reject";
    const char choice_resume[18] = "Go back to review";

    nbgl_useCaseConfirm(choice_context.reject_text,
                        NULL,
                        choice_reject,
                        choice_resume,
                        handle_rejection);
}

/**
 * @brief Handles silent rejection of the review.
 */
void handle_review_silent_rejection(void) {
    ui_validate(response_type, false);
    ui_choice_context_destroy();
}

#endif  // if defined(SCREEN_SIZE_WALLET)
