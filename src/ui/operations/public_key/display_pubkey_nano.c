/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#if defined(SCREEN_SIZE_NANO)  // nanosp, nanox

#include "ui/operations/public_key/display_pubkey.h"

#include <stddef.h>  // NULL

#include <io.h>  // io_send_sw
#include <os.h>  // explicit_bzero
#include <ux.h>  // UX_FLOW, UX_STEP_NOCB

#include "glyphs.h"  // C_Check_Circle_64px, C_logo_solar_64px

#include "app_types.h"  // global_ctx_t

#include "constants.h"
#include "globals.h"
#include "sw.h"

#include "ui/ui_constants.h"
#include "ui/ui_utils.h"

#include "ui/action/ux_approve.h"

/* -------------------------------------------------------------------------- */

static const char TEXT_LABEL_PUBKEY[] = "Public Key";
static const char TEXT_LABEL_PUBKEY_VERIFY[] = "Verify Public Key";

/* -------------------------------------------------------------------------- */

static char ui_pubkey_text[PUBKEY_CHAR_LEN] = {0};

/* -------------------------------------------------------------------------- */

UX_STEP_NOCB(ux_display_pubkey_confirm_step, pn, {&C_icon_eye, TEXT_LABEL_PUBKEY_VERIFY});

UX_STEP_NOCB(ux_display_pubkey_step,
             bnnn_paging,
             {
                 TEXT_LABEL_PUBKEY,
                 ui_pubkey_text,
             });

UX_APPROVE_STEP(ux_display_pubkey_approve_step, PUBKEY_RESPONSE);
UX_REJECT_STEP(ux_display_pubkey_reject_step, PUBKEY_RESPONSE);

/* -------------------------------------------------------------------------- */

UX_FLOW(ux_display_pubkey_flow,
        &ux_display_pubkey_confirm_step,
        &ux_display_pubkey_step,
        &ux_display_pubkey_approve_step,
        &ux_display_pubkey_reject_step);

/* -------------------------------------------------------------------------- */

/**
 * @brief Displays the public key on the device and asks for user confirmation.
 *
 * @return 0 if successful, otherwise an error code.
 */
int ui_display_public_key(void) {
    if ((G_context.req_type != CONFIRM_PUBLICKEY) || (G_context.state != STATE_NONE)) {
        G_context.state = STATE_NONE;
        return io_send_sw(SW_BAD_STATE);
    }

    explicit_bzero(&ui_pubkey_text, PUBKEY_CHAR_LEN);

    if (format_hex_lower(G_context.pk_info.raw_public_key,
                         PUBKEY_BYTE_LEN,
                         ui_pubkey_text,
                         MAX_VALUE_LEN) < 0) {
        return io_send_sw(SW_PUBKEY_PARSING_FAIL);
    }

    ux_flow_init(0u, ux_display_pubkey_flow, NULL);

    return 0;
}

#endif  // if defined(SCREEN_SIZE_NANO)
