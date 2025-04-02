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

#include "helper/send_response.h"

#include <stdbool.h>
#include <stddef.h>  // NULL, size_t
#include <stdint.h>  // uint*_t
#include <string.h>  // memcpy, memmove

#include <io.h>  // io_send_sw, io_send_response_pointer

#include "address.h"
#include "app_types.h"
#include "constants.h"
#include "context.h"
#include "globals.h"
#include "sw.h"

/* -------------------------------------------------------------------------- */

#define RESPONSE_VALUE_LENGTH_SIZE ((uint8_t)1u)

/* -------------------------------------------------------------------------- */

/**
 * @brief Helper to send the APDU response of an address.
 *
 * response = ADDRESS_CHAR_LEN(1) | ADDRESS(34)
 *
 * @return zero or positive integer if successful, otherwise -1.
 */
static int helper_send_response_address(void) {
    char address[ADDRESS_CHAR_LEN + NULL_TERMINATOR_LEN] = {0};

    uint8_t resp[ADDRESS_CHAR_LEN + NULL_TERMINATOR_LEN] = {0};
    size_t offset = 0u;

    resp[offset] = ADDRESS_CHAR_LEN;
    offset++;

    if (!address_from_public_key(G_context.pk_info.raw_public_key, address, G_context.network)) {
        reset_app_context();
        return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }

    const bool success = memmove(&resp[offset], address, ADDRESS_CHAR_LEN) != NULL;
    reset_app_context();

    if (success) {
        offset += ADDRESS_CHAR_LEN;
        return io_send_response_pointer(resp, offset, SW_OK);
    } else {
        return io_send_sw(SW_DISPLAY_ADDRESS_FAIL);
    }
}

/**
 * @brief Helper to send the APDU response of a public key and chain code.
 *
 * response = PUBKEY_LEN (1) ||
 *            G_context.pk_info.public_key (PUBKEY_LEN) ||
 *            CHAINCODE_LEN (1) ||
 *            G_context.pk_info.chain_code (CHAINCODE_LEN)
 *
 * @return zero or positive integer if successful, otherwise -1.
 */
static int helper_send_response_pubkey(void) {
    uint8_t resp[RESPONSE_VALUE_LENGTH_SIZE + PUBKEY_BYTE_LEN + RESPONSE_VALUE_LENGTH_SIZE +
                 CHAINCODE_LEN] = {0};
    uint8_t offset = 0;

    resp[offset] = PUBKEY_BYTE_LEN;
    offset++;

    if (memmove(&resp[offset], G_context.pk_info.raw_public_key, PUBKEY_BYTE_LEN) == NULL) {
        reset_app_context();
        return io_send_sw(SW_PUBKEY_PARSING_FAIL);
    }
    offset += PUBKEY_BYTE_LEN;

    if (G_context.pk_info.use_chaincode) {
        resp[offset] = CHAINCODE_LEN;
        offset++;

        if (memmove(&resp[offset], G_context.pk_info.chain_code, CHAINCODE_LEN) == NULL) {
            reset_app_context();
            return io_send_sw(SW_PUBKEY_PARSING_FAIL);
        }
        offset += (uint8_t)CHAINCODE_LEN;
    }

    reset_app_context();

    return io_send_response_pointer(resp, offset, SW_OK);
}

/**
 * @brief Helper to send the APDU response of a signature.
 *
 * response = G_context.tx_info.signature
 *
 * @return zero or positive integer if successful, otherwise -1.
 */
static int helper_send_response_sig(void) {
    uint8_t resp[SIG_SCHNORR_LEN] = {0};

    const bool success = memcpy(resp, G_context.tx_info.signature, SIG_SCHNORR_LEN) != NULL;
    reset_app_context();

    return success ? io_send_response_pointer(resp, SIG_SCHNORR_LEN, SW_OK)
                   : io_send_sw(SW_SIGNATURE_FAIL);
}

/* -------------------------------------------------------------------------- */

/**
 * @brief Helper function to send APDU responses.
 *
 * @see helper_send_response_address, helper_send_response_pubkey, helper_send_response_sig
 */
int helper_send(helper_response_t responseType) {
    switch (responseType) {
        case ADDRESS_RESPONSE:
            return helper_send_response_address();
        case PUBKEY_RESPONSE:
            return helper_send_response_pubkey();
        case SIGNATURE_RESPONSE:
            return helper_send_response_sig();
        default:
            reset_app_context();
            return io_send_sw(SW_DISPLAY_FAIL);
    }
}
