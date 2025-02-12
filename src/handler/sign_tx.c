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

#include "handler/sign_tx.h"

#include <stdbool.h>
#include <stdint.h>  // uint*_t

#include <buffer.h>     // buffer_t
#include <cx_errors.h>  // CX_OK
#include <io.h>         // io_send_sw

#include "app_types.h"
#include "constants.h"
#include "context.h"
#include "globals.h"
#include "sw.h"

#include "crypto/crypto.h"

#include "transaction/deserialise_message.h"
#include "transaction/deserialise_transaction.h"
#include "transaction/transaction_errors.h"

#include "ui/operations/message/display_message.h"
#include "ui/operations/transaction/display_transaction.h"

/* -------------------------------------------------------------------------- */

/**
 * @brief Handles the last chunk of transaction data.
 *
 * Deserialises the transaction, hashes it, and calls the UI for display.
 *
 * @param [in]  cdata   Pointer to the buffer containing the last chunk of data.
 *
 * @return the result of UI display call if successful, otherwise an error code.
 */
static int handle_last_chunk(void) {
    buffer_t buf = {.ptr = G_context.tx_info.raw_tx,
                    .size = G_context.tx_info.raw_tx_len,
                    .offset = 0u};

    // Create a hash of the message to be signed.
    if (hash_sha256(G_context.tx_info.raw_tx,
                    G_context.tx_info.raw_tx_len,
                    G_context.tx_info.m_hash) == false) {
        return io_send_sw(SW_TX_HASH_FAIL);
    }

    const parser_status_e deserialisation =
        (G_context.req_type == CONFIRM_TRANSACTION)
            ? deserialise_transaction(&buf, &G_context.tx_info.transaction)
            : deserialise_message(&buf, &G_context.tx_info.transaction);

    if (deserialisation != PARSING_OK) {
        return io_send_sw(SW_TX_PARSING_FAIL);
    }

    G_context.state = STATE_PARSED;

    // First verification of the message hash
    if (verify_message_hash() != CX_OK) {
        return io_send_sw(SW_VALIDATION_HASH_MISMATCH);
    }

    return (G_context.req_type == CONFIRM_TRANSACTION) ? ui_display_transaction()
                                                       : ui_display_message();
}

/**
 * @brief Appends chunk to the raw tx buffer and processes it on the last chunk.
 *
 * @param [in]  cdata   Pointer to the buffer containing the chunk of data.
 * @param [in]  more    Boolean indicating if more chunks are expected.
 *
 * @return SW_OK if successful, otherwise an error code.
 */
static int handle_chunks(buffer_t *cdata, bool more) {
    if ((G_context.tx_info.raw_tx_len + cdata->size) > TRANSACTION_LEN_MAX) {
        return io_send_sw(SW_WRONG_TX_LENGTH);
    }

    if (!buffer_move(cdata, &G_context.tx_info.raw_tx[G_context.tx_info.raw_tx_len], cdata->size)) {
        return io_send_sw(SW_TX_PARSING_FAIL);
    }

    G_context.tx_info.raw_tx_len += cdata->size;

    return (more) ? io_send_sw(SW_OK) : handle_last_chunk();
}

/* -------------------------------------------------------------------------- */

/**
 * @brief Handler for SIGN_TX command.
 *
 * On Sign, sends an APDU response containing the transaction or message signature.
 *
 * @see G_context.bip32_path, G_context.tx_info.raw_transaction, G_context.tx_info.signature,
 * G_context.tx_info.v
 *
 * @param[in,out]   cdata       Command data with BIP32 path and raw transaction serialised.
 * @param[in]       chunk       Index number of the APDU chunk.
 * @param[in]       is_last     Whether or not this is the final APDU chunk to be received.
 * @param[in]       is_message  Whether the data is a transaction or a message.
 *
 * @return the result of signing if successful, otherwise an error code.
 */
int handler_sign_tx(buffer_t *cdata, uint8_t chunk, bool more, bool is_message) {
    if (chunk == 0u) {
        reset_app_context();

        G_context.req_type = is_message ? CONFIRM_MESSAGE : CONFIRM_TRANSACTION;

        if (parse_bip32_path(cdata) != CX_OK) {
            return io_send_sw(SW_WRONG_LENGTH);
        }

        return io_send_sw(SW_OK);
    }
    // Handle transaction / message chunk(s)
    else {
        if (((!is_message) && (G_context.req_type != CONFIRM_TRANSACTION)) ||
            ((is_message) && (G_context.req_type != CONFIRM_MESSAGE))) {
            return io_send_sw(SW_BAD_STATE);
        }

        if ((G_context.req_num + 1u) != chunk) {
            return io_send_sw(SW_REQ_ORDER_FAIL);
        }

        G_context.req_num++;

        return handle_chunks(cdata, more);
    }
}
