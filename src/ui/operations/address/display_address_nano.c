/*******************************************************************************
 *  Copyright (c) Solar Network (hello@solar.org)
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#if defined(SCREEN_SIZE_NANO)  // nanosp, nanox

#include "ui/operations/address/display_address.h"

#include <stddef.h>  // NULL

#include <io.h>  // io_send_sw
#include <ux.h>  // ux_flow_init, ux_layout_pb_init...

#include "glyphs.h"  // C_icon_eye

#include "address.h"
#include "app_types.h"
#include "constants.h"
#include "globals.h"
#include "sw.h"

#include "helper/send_response.h"

#include "ui/action/ux_approve.h"

/* -------------------------------------------------------------------------- */

static const char TEXT_LABEL_ADDRESS[] = "Address";
static const char TEXT_LABEL_ADDRESS_VERIFY[] = "Verify Address";

/* -------------------------------------------------------------------------- */

static char ui_address_text[ADDRESS_CHAR_LEN + NULL_TERMINATOR_LEN] = {0};

/* -------------------------------------------------------------------------- */

UX_STEP_NOCB(ux_display_address_review_step, pn, {&C_icon_eye, TEXT_LABEL_ADDRESS_VERIFY});

UX_STEP_NOCB(ux_display_address_content_step,
             bnnn_paging,
             {
                 TEXT_LABEL_ADDRESS,
                 ui_address_text,
             });

UX_APPROVE_STEP(ux_display_address_approve_step, ADDRESS_RESPONSE);
UX_REJECT_STEP(ux_display_address_reject_step, ADDRESS_RESPONSE);

/* -------------------------------------------------------------------------- */

UX_FLOW(ux_display_address_flow,
        &ux_display_address_review_step,
        &ux_display_address_content_step,
        &ux_display_address_approve_step,
        &ux_display_address_reject_step);

/* -------------------------------------------------------------------------- */

/**
 * @brief Displays the address on the device and asks for user confirmation.
 *
 * a0 if successful, otherwise an error code.
 */
int ui_display_address(void) {
    if ((G_context.req_type != CONFIRM_ADDRESS) || (G_context.state != STATE_NONE)) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    if (!address_from_public_key(G_context.pk_info.raw_public_key,
                                 ui_address_text,
                                 G_context.network)) {
        return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    ux_flow_init(0u, ux_display_address_flow, NULL);

    return 0;
}

#endif  // if defined(SCREEN_SIZE_NANO)
