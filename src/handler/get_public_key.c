/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 *
 *  This software also incorporates work covered by the following copyright
 *  and permission notice:
 *
 *   Ledger App Boilerplate.
 *   (c) 2020 Ledger SAS.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/

#include "handler/get_public_key.h"

#include <stdbool.h>

#include <buffer.h>     // buffer_t
#include <cx_errors.h>  // CX_OK
#include <io.h>         // io_send_sw

#include "app_types.h"
#include "constants.h"
#include "context.h"
#include "globals.h"
#include "sw.h"

#include "crypto/crypto.h"

#include "helper/send_response.h"

#include "ui/operations/public_key/display_pubkey.h"

/* -------------------------------------------------------------------------- */

/**
 * @brief Handler for GET_PUBLIC_KEY command.
 *
 * Sends an APDU response containing an public key.
 *
 * @see G_context.bip32_path, G_context.pk_info.raw_public_key, G_context.pk_info.chain_code.
 *
 * @param[in,out]   cdata           Command data with BIP32 path.
 * @param[in]       user_approval   0 for not displaying on screen, otherwise 1.
 * @param[in]       use_chain_code  0 for not requesting the chain code, otherwise 1.
 *
 * @return zero or positive integer if successful, otherwise a negative integer.
 */
int handler_get_public_key(buffer_t *cdata, bool user_approval, bool use_chain_code) {
    if (cdata->size != PUBKEY_HASH_LEN) {
        return io_send_sw(SW_PUBKEY_PARSING_FAIL);
    }

    reset_app_context();

    G_context.req_type = CONFIRM_PUBLICKEY;
    G_context.pk_info.use_chaincode = use_chain_code;

    const int status = prepare_public_key(cdata, use_chain_code);
    if (status != CX_OK) {
        return io_send_sw(status);
    }

    if (user_approval) {
        return ui_display_public_key();
    }

    return helper_send(PUBKEY_RESPONSE);
}
