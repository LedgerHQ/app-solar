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

#include "handler/get_address.h"

#include <stdbool.h>
#include <stdint.h>  // uint*_t

#include <buffer.h>     // buffer_t
#include <cx_errors.h>  // CX_OK
#include <io.h>         // io_send_sw

#include "app_types.h"
#include "context.h"
#include "globals.h"

#include "crypto/crypto.h"

#include "helper/send_response.h"

#include "ui/operations/address/display_address.h"

/* -------------------------------------------------------------------------- */

/**
 * @brief Handler for GET_ADDRESS command.
 *
 * Sends an APDU response containing an address.
 *
 * @see G_context.bip32_path, G_context.pk_info.raw_public_key.
 *
 * @param[in,out]   cdata           Command data with BIP32 path.
 * @param[in]       user_approval   0 for not displaying on screen, otherwise 1.
 *
 * @return zero or positive integer if successful, otherwise -1.
 */
int handler_get_address(buffer_t *cdata, bool user_approval, uint8_t network) {
    reset_app_context();

    G_context.req_type = CONFIRM_ADDRESS;
    G_context.network = network;

    const int status = prepare_public_key(cdata, false);
    if (status != CX_OK) {
        return io_send_sw(status);
    }

    if (user_approval) {
        return ui_display_address();
    }

    return helper_send(ADDRESS_RESPONSE);
}
