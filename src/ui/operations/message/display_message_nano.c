/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#if defined(SCREEN_SIZE_NANO)  // nanosp, nanox

#include "ui/operations/message/display_message.h"

#include <cx.h>  // CX_OK
#include <io.h>  // io_send_sw
#include <ux.h>  // ux_flow_init, UX_FLOW, UX_STEP_NOCB

#include "glyphs.h"  // C_Review_64px, ux_layout_pb_init...

#include "globals.h"
#include "sw.h"

#include "crypto/crypto.h"

#include "ui/action/ux_approve.h"

/* -------------------------------------------------------------------------- */

static const char MESSAGE_REVIEW[] = "Review";
static const char MESSAGE_NAME[] = "Message";

static const int MESSAGE_TEXT_OFFSET = 2;

/* -------------------------------------------------------------------------- */

UX_STEP_NOCB(ux_display_message_review_step,
             pnn,
             {
                 &C_icon_eye,
                 MESSAGE_REVIEW,
                 MESSAGE_NAME,
             });

UX_STEP_NOCB(ux_display_message_content_step,
             bnnn_paging,
             {
                 MESSAGE_NAME,
                 (const char *)&G_context.tx_info.raw_tx[MESSAGE_TEXT_OFFSET],
             });

UX_APPROVE_STEP(ux_display_message_approve_step, SIGNATURE_RESPONSE);
UX_REJECT_STEP(ux_display_message_reject_step, SIGNATURE_RESPONSE);

/* -------------------------------------------------------------------------- */

UX_FLOW(ux_display_message_flow,
        &ux_display_message_review_step,
        &ux_display_message_content_step,
        &ux_display_message_approve_step,
        &ux_display_message_reject_step);

/* -------------------------------------------------------------------------- */

/**
 * @brief Displays the message on the device and ask for user confirmation to sign.
 *
 * @return 0 if successful, otherwise an error code.
 */
int ui_display_message(void) {
    if ((G_context.req_type != CONFIRM_MESSAGE) || (G_context.state != STATE_PARSED)) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    // Second verification of the message hash
    if (verify_message_hash() != CX_OK) {
        return io_send_sw(SW_VALIDATION_HASH_MISMATCH);
    }

    ux_flow_init(0u, ux_display_message_flow, NULL);

    return 0;
}

#endif  // if defined(SCREEN_SIZE_NANO)
