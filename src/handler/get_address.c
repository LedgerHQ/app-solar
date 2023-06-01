/*****************************************************************************
 *  Copyright (c) Solar Network <hello@solar.org>
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 *****************************************************************************/

#include "get_public_key.h"

#include <stdint.h>   // uint*_t
#include <stdbool.h>  // bool
#include <stddef.h>   // size_t
#include <string.h>   // memset, explicit_bzero

#include "cx.h"
#include "io.h"
#include "os.h"

#include "buffer.h"

#include "context.h"
#include "globals.h"
#include "sw.h"
#include "types.h"

#include "crypto/crypto.h"
#include "helper/send_response.h"
#include "ui/display.h"

int handler_get_address(buffer_t *cdata, bool user_approval, uint8_t network) {
    reset_app_context();
    G_context.req_type = CONFIRM_ADDRESS;
    G_context.network = network;

    cx_ecfp_private_key_t private_key = {0};
    cx_ecfp_public_key_t public_key = {0};

    if (!buffer_read_u8(cdata, &G_context.bip32_path_len) ||
        !buffer_read_bip32_path(cdata, G_context.bip32_path, (size_t) G_context.bip32_path_len)) {
        return io_send_sw(SW_WRONG_DATA_LENGTH);
    }

    // derive private key according to BIP32 path
    crypto_derive_private_key(&private_key, NULL, G_context.bip32_path, G_context.bip32_path_len);

    // generate corresponding public key
    crypto_init_public_key(&private_key, &public_key, G_context.pk_info.raw_public_key);

    // reset private key
    explicit_bzero(&private_key, sizeof(private_key));

    if (user_approval > 0) {
        return ui_display_address();
    }

    return helper_send_response_address();
}
