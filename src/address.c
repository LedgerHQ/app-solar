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

#include "address.h"

#include <stdbool.h>
#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t
#include <string.h>  // memcpy

#include <base58.h>  // base58_encode

#include "constants.h"
#include "crypto/crypto.h"

/* -------------------------------------------------------------------------- */

// Generates a ripemd160 hash of public key prefixed by a network byte.
static bool hash_public_key(const uint8_t public_key[PUBKEY_BYTE_LEN],
                            uint8_t out[PUBKEY_HASH_LEN],
                            uint8_t network) {
    if (out == NULL) {
        return false;
    }

    uint8_t pubkeyhash[PUBKEY_HASH_LEN] = {0};

    if (hash_ripemd160(public_key, PUBKEY_BYTE_LEN, pubkeyhash) == false) {
        return false;
    }

    out[0] = network;

    if (memcpy(&out[NETWORK_BYTE_SIZE], pubkeyhash, RIPEMD160_HASH_LEN) == NULL) {
        return false;
    }

    return true;
}

/**
 * @brief Create an address using a public key and a network byte.
 *
 * @param[in]   public_key  Pointer to a buffer containing a public key.
 * @param[in]   network     Network byte.
 * @param[out]  out         Pointer to the output byte buffer for the address.
 *
 * @return true if successful, otherwise false.
 */
bool address_from_public_key(const uint8_t public_key[PUBKEY_BYTE_LEN],
                             char out[ADDRESS_CHAR_LEN],
                             const uint8_t network) {
    if (out == NULL) {
        return false;
    }

    uint8_t pubkey_hash[PUBKEY_HASH_LEN] = {0};

    if (!hash_public_key(public_key, pubkey_hash, network)) {
        return false;
    }

    if (address_from_pubkey_hash(pubkey_hash,
                                 PUBKEY_HASH_LEN,
                                 out,
                                 ADDRESS_CHAR_LEN + NULL_TERMINATOR_LEN) < 0) {
        return false;
    }

    return true;
}

// Creates a 4-byte error detection code used for address validation
static bool calculate_address_checksum(const uint8_t *in,
                                       size_t in_len,
                                       uint8_t out[ADDRESS_CHECKSUM_LEN]) {
    uint8_t buffer[HASH_32_LEN];

    if (hash_sha256(in, in_len, buffer) == false) {
        return false;
    }

    if (hash_sha256(buffer, HASH_32_LEN, buffer) == false) {
        return false;
    }

    return (bool)(memcpy(out, buffer, ADDRESS_CHECKSUM_LEN) != NULL);
}

/**
 * @brief Convert address hash to base58 address.
 *
 * @param[in]   in      Pointer to byte buffer with address.
 * @param[in]   in_len  Length of input address bytes.
 * @param[out]  out     Pointer to output byte buffer for address.
 * @param[in]   out_len Maximum length to write in output byte buffer.
 *
 * @return the number of bytes decoded, otherwise -1.
 */
int address_from_pubkey_hash(const uint8_t *in, size_t in_len, char *out, size_t out_len) {
    if (in_len != PUBKEY_HASH_LEN) {
        return -1;
    }

    uint8_t tmp[PUBKEY_HASH_LEN + ADDRESS_CHECKSUM_LEN + NULL_TERMINATOR_LEN] = {0};

    if (memcpy(tmp, in, in_len) == NULL) {
        return false;
    }

    if (!calculate_address_checksum(tmp, in_len, &tmp[in_len])) {
        return -1;
    }

    int len = base58_encode(tmp, in_len + ADDRESS_CHECKSUM_LEN, out, out_len - NULL_TERMINATOR_LEN);
    if (len < 1) {
        return len;
    }

    out[len] = '\0';
    len++;

    return len;
}
