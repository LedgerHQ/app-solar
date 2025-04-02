/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#if defined(SCREEN_SIZE_WALLET)  // stax, flex

#include "ui/operations/transaction/display_error.h"

#include <stdbool.h>

#include <nbgl_use_case.h>  // nbgl_useCaseStatus

#include "ui/action/choice.h"

#include "ui/menu/display_menu.h"

#include "ui/operations/transaction/review_context.h"

/* -------------------------------------------------------------------------- */

#define ERROR_PAGE_TEXT                                              \
    "WARNING: "                                                      \
    "This transaction cannot be displayed due to an internal error." \
    "\n\n"                                                           \
    "It has been securely rejected."

void display_error_page_and_exit(void) {
    clear_review_context();
    handle_review_silent_rejection();
    nbgl_useCaseStatus(ERROR_PAGE_TEXT, false, ui_menu_main);
}

#endif  // if defined(SCREEN_SIZE_WALLET)
