#pragma once

#include <stdbool.h>

#include "helper/send_response.h"

/* -------------------------------------------------------------------------- */

typedef struct {
    const char *confirmed_text;  // Displayed upon review approval                   (4bytes)
    const char *rejected_text;   // Displayed upon review rejection                  (4bytes)
    const char *reject_text;     // Displayed when asking for rejection confirmation (4bytes)
} ui_choice_ctx_t;

/* -------------------------------------------------------------------------- */

bool initialise_choice_context(helper_response_t type,
                               const char *choice_confirmed,
                               const char *choice_reject,
                               const char *choice_rejected);

void handle_review_choice(bool confirm);

void handle_review_rejection_with_confirmation(void);

void handle_review_silent_rejection(void);
