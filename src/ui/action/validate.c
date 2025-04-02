/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#include "ui/action/validate.h"

#include <stdbool.h>

#include <cx.h>  // CX_OK
#include <io.h>  // io_send_sw

#include "app_types.h"
#include "globals.h"
#include "sw.h"

#include "crypto/crypto.h"

#include "helper/send_response.h"

#include "ui/menu/display_menu.h"

/* -------------------------------------------------------------------------- */

/**
 * @brief Handles the validation and APDU response based on the user's choice.
 *
 * @param[in]   response_type   The type of helper response.
 * @param[in]   choice          User choice (either approved or rejected).
 */
void ui_validate(helper_response_t response_type, bool choice) {
    if (choice) {
        if (response_type == SIGNATURE_RESPONSE) {
            G_context.state = STATE_APPROVED;

            // Third and final verification of the message hash
            if (verify_message_hash() != CX_OK) {
                (void)io_send_sw(SW_VALIDATION_HASH_MISMATCH);
            }

            // Sign if hash verification passes
            if (sign_message() != CX_OK) {
                (void)io_send_sw(SW_SIGNATURE_FAIL);
            }
        }

        if (helper_send(response_type) < 0) {
            (void)io_send_sw(SW_SIGNATURE_FAIL);
        }
    } else {
        (void)io_send_sw(SW_DENY);
    }

    ui_menu_main();
}
