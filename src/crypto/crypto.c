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

#include "crypto/crypto.h"

#include <stdbool.h>
#include <stddef.h>  // NULL, size_t
#include <stdint.h>  // uint*_t
#include <string.h>  // memcmp, memcpy

#include <bip32.h>           // MAX_BIP32_PATH
#include <buffer.h>          // buffer_read_bip32_path, buffer_read_u8
#include <crypto_helpers.h>  // bip32_derive_init_privkey_256
#include <cx.h>              // CX_OK, CX_SHA256, cx_ecschnorr_sign_no_throw...
#include <os.h>              // explicit_bzero

#include "app_types.h"
#include "constants.h"
#include "globals.h"
#include "sw.h"

/* -------------------------------------------------------------------------- */

/**
 * @brief Hash raw data to produce a ripemd160 hash.
 *
 * @param[in]   in      The data to be hashed.
 * @param[in]   in_len  The length of the data to be hashed.
 * @param[out]  out     The output buffer where the hash will be written.
 *
 * @return CX_OK if successful, otherwise an error code.
 */
bool hash_ripemd160(const uint8_t *in, size_t in_len, uint8_t out[HASH_20_LEN]) {
    return cx_hash_ripemd160(in, in_len, out, HASH_20_LEN) == HASH_20_LEN;
}

/**
 * @brief Hash raw data to produce a sha256 hash.
 *
 * @param[in]   in      The data to be hashed.
 * @param[in]   in_len  The length of the data to be hashed.
 * @param[out]  out     The output buffer where the hash will be written.
 *
 * @return CX_OK if successful, otherwise an error code.
 */
bool hash_sha256(const uint8_t *in, size_t in_len, uint8_t out[HASH_32_LEN]) {
    return cx_hash_sha256(in, in_len, out, HASH_32_LEN) == HASH_32_LEN;
}

/**
 * @brief Parse the BIP32 path sent in APDU chunk[0].
 *
 * @see G_context.bip32_path
 *
 * @param[in]   cdata   Command data with BIP32 path
 *
 * @return CX_OK if successful, otherwise an error code.
 */
int parse_bip32_path(buffer_t *cdata) {
    if (cdata == NULL) {
        return SW_DISPLAY_BIP32_PATH_FAIL;
    }

    if (!buffer_read_u8(cdata, &G_context.bip32_path_len)) {
        return SW_WRONG_LENGTH;
    }

    if ((G_context.bip32_path_len < (uint8_t)MIN_BIP32_PATH) ||
        (G_context.bip32_path_len > (uint8_t)MAX_BIP32_PATH)) {
        return SW_WRONG_LENGTH;
    }

    if (!buffer_read_bip32_path(cdata, G_context.bip32_path, (size_t)G_context.bip32_path_len)) {
        return SW_DISPLAY_BIP32_PATH_FAIL;
    }

    return CX_OK;
}

/**
 * @brief Derive a public key given a private key and path.
 *
 * Public key will be placed in `G_context.pk_info.raw_public_key`
 *
 * @param[in]   cdata           Command data with BIP32 path
 * @param[in]   use_chain_code  Whether to use the chain
 *
 * @return CX_OK if successful, otherwise an error code.
 */
int prepare_public_key(buffer_t *cdata, bool use_chain_code) {
    const uint8_t SECP256K1_TAG_PUBKEY_EVEN = 0x02;
    const uint8_t SECP256K1_TAG_PUBKEY_ODD = 0x03;

    cx_ecfp_private_key_t private_key = {0};
    cx_ecfp_public_key_t public_key = {0};
    int status = CX_OK;

    if (cdata == NULL) {
        return SW_DISPLAY_BIP32_PATH_FAIL;
    }

    status = parse_bip32_path(cdata);
    if (status != CX_OK) {
        return status;
    }

    status = bip32_derive_init_privkey_256(CX_CURVE_256K1,
                                           G_context.bip32_path,
                                           G_context.bip32_path_len,
                                           &private_key,
                                           (use_chain_code) ? G_context.pk_info.chain_code : NULL);
    if (status != CX_OK) {
        explicit_bzero(&private_key, sizeof(cx_ecfp_private_key_t));
        return status;
    }

    status = cx_ecfp_generate_pair_no_throw(CX_CURVE_256K1, &public_key, &private_key, true);
    explicit_bzero(&private_key, sizeof(cx_ecfp_private_key_t));
    if (status != CX_OK) {
        explicit_bzero(&public_key, sizeof(cx_ecfp_public_key_t));
        return status;
    }

    const bool y_is_odd = (*(&public_key.W[HASH_64_LEN]) & 1);
    G_context.pk_info.raw_public_key[0] =
        y_is_odd ? SECP256K1_TAG_PUBKEY_ODD : SECP256K1_TAG_PUBKEY_EVEN;

    (void)memcpy(&G_context.pk_info.raw_public_key[1], &public_key.W[1], HASH_32_LEN);

    return status;
}

/**
 * @brief Sign message hash in the global context.
 *
 * @see G_context.bip32_path, G_context.tx_info.m_hash, G_context.tx_info.signature.
 *
 * @return CX_OK if successful, otherwise an error code.
 */
int sign_message(void) {
    cx_ecfp_private_key_t private_key = {0};
    size_t signature_len = sizeof(G_context.tx_info.signature);

    int status = CX_OK;

    status = bip32_derive_init_privkey_256(CX_CURVE_256K1,
                                           G_context.bip32_path,
                                           G_context.bip32_path_len,
                                           &private_key,
                                           NULL);
    if (status != CX_OK) {
        (void)explicit_bzero(&private_key, sizeof(cx_ecfp_private_key_t));
        return status;
    }

    status = cx_ecschnorr_sign_no_throw(&private_key,
                                        CX_ECSCHNORR_BIP0340 | CX_RND_TRNG,
                                        CX_SHA256,
                                        G_context.tx_info.m_hash,
                                        sizeof(G_context.tx_info.m_hash),
                                        G_context.tx_info.signature,
                                        &signature_len);
    explicit_bzero(&private_key, sizeof(cx_ecfp_private_key_t));

    return status;
}

/**
 * @brief Paranoid check to verify the integrity of a transaction.
 *
 * Creates a hash of the current transaction and compares it against a precomputed reference hash.
 *
 * This function ensures that the raw transaction stored in the global context has not been altered
 * or corrupted between receipt, deserialisation, display, and signing. It recomputes the hash of
 * the raw transaction and compares it to the previously calculated hash stored during the final
 * chunk handling. If the hashes do not match, the transaction is rejected to prevent signing of
 * inconsistent or tampered data.
 *
 * @return CX_OK if successful, otherwise an error code.
 */
int verify_message_hash(void) {
    uint8_t validation_hash[HASH_32_LEN];
    if (cx_hash_sha256(G_context.tx_info.raw_tx,
                       G_context.tx_info.raw_tx_len,
                       validation_hash,
                       HASH_32_LEN) == false) {
        return (int)SW_TX_HASH_FAIL;
    }

    if (memcmp(G_context.tx_info.m_hash, validation_hash, sizeof(validation_hash)) != 0) {
        return (int)SW_VALIDATION_HASH_MISMATCH;
    }

    return CX_OK;
}
