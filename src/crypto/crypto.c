/*****************************************************************************
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 *
 *  This software also incorporates work covered by the following copyright
 *  and permission notice:
 *
 *   Ledger App Boilerplate.
 *   (c) 2023 Ledger SAS.
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
 *****************************************************************************/

#include <stdint.h>  // uint*_t
#include <string.h>  // memset, explicit_bzero

#include "crypto.h"

#include "globals.h"

int crypto_derive_private_key(cx_ecfp_private_key_t *private_key,
                              uint8_t chain_code[32],
                              const uint32_t *bip32_path,
                              uint8_t bip32_path_len) {
    uint8_t raw_private_key[32] = {0};

    BEGIN_TRY {
        TRY {
            // derive the seed with bip32_path
            os_perso_derive_node_bip32(CX_CURVE_256K1,
                                       bip32_path,
                                       bip32_path_len,
                                       raw_private_key,
                                       chain_code);
            // new private_key from raw
            cx_ecfp_init_private_key(CX_CURVE_256K1,
                                     raw_private_key,
                                     sizeof(raw_private_key),
                                     private_key);
        }
        CATCH_OTHER(e) {
            THROW(e);
        }
        FINALLY {
            explicit_bzero(&raw_private_key, sizeof(raw_private_key));
        }
    }
    END_TRY;

    return 0;
}

int crypto_init_public_key(cx_ecfp_private_key_t *private_key,
                           cx_ecfp_public_key_t *public_key,
                           uint8_t raw_public_key[33]) {
    // generate corresponding public key
    cx_ecfp_generate_pair(CX_CURVE_256K1, public_key, private_key, 1);

    raw_public_key[0] = ((*(public_key->W + 64) & 1) ? 0x03 : 0x02);
    memmove(raw_public_key + 1, public_key->W + 1, 32);

    return 0;
}

int crypto_sign_message() {
    cx_ecfp_private_key_t private_key = {0};
    size_t signature_len = sizeof(G_context.tx_info.signature);
    cx_err_t error = CX_INTERNAL_ERROR;

    // derive private key according to BIP32 path
    crypto_derive_private_key(&private_key, NULL, G_context.bip32_path, G_context.bip32_path_len);

    BEGIN_TRY {
        TRY {
            error = cx_ecschnorr_sign_no_throw(&private_key,
                                               CX_ECSCHNORR_BIP0340 | CX_RND_TRNG,
                                               CX_SHA256,
                                               G_context.tx_info.m_hash,
                                               sizeof(G_context.tx_info.m_hash),
                                               G_context.tx_info.signature,
                                               &signature_len);
        }
        CATCH_OTHER(e) {
            THROW(e);
        }
        FINALLY {
            explicit_bzero(&private_key, sizeof(private_key));
        }
    }
    END_TRY;

    if (error != CX_OK) {
        return -1;
    }

    return 0;
}
