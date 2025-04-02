/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#if defined(SCREEN_SIZE_WALLET)  // stax, flex

#include "ui/operations/address/display_address.h"

#include <stddef.h>  // NULL

#include <io.h>             // io_send_sw
#include <nbgl_use_case.h>  // nbgl_useCaseAddressReview

#include "glyphs.h"  // C_logo_solar_64px

#include "address.h"
#include "app_types.h"
#include "constants.h"
#include "globals.h"
#include "sw.h"

#include "helper/send_response.h"

#include "ui/action/choice.h"

/* -------------------------------------------------------------------------- */

/**
 * @brief Displays the address on the device and asks for user confirmation.
 *
 * @return 0 if successful, otherwise an error code.
 */
int ui_display_address(void) {
    static const char REVIEW_INTENT_ADDRESS[] = "Review Solar Network (SXP) address";
    static const char CHOICE_ADDRESS_CONFIRMED[] = "Address confirmed";
    static const char CHOICE_ADDRESS_REJECT[] = "Reject address?";
    static const char CHOICE_ADDRESS_REJECTED[] = "Address rejected";

    static char ui_address_text[ADDRESS_CHAR_LEN + NULL_TERMINATOR_LEN] = {0};

    if ((G_context.req_type != CONFIRM_ADDRESS) || (G_context.state != STATE_NONE)) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    if (!initialise_choice_context(ADDRESS_RESPONSE,
                                   CHOICE_ADDRESS_CONFIRMED,
                                   CHOICE_ADDRESS_REJECT,
                                   CHOICE_ADDRESS_REJECTED)) {
        return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    if (!address_from_public_key(G_context.pk_info.raw_public_key,
                                 ui_address_text,
                                 G_context.network)) {
        return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    nbgl_useCaseAddressReview(ui_address_text,
                              NULL,
                              &C_logo_solar_64px,
                              REVIEW_INTENT_ADDRESS,
                              NULL,
                              handle_review_choice);

    return 0;
}

#endif  // if defined(SCREEN_SIZE_WALLET)
