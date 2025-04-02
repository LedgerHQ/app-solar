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

#include "utils/check_ascii.h"

#include <stdbool.h>
#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

/* -------------------------------------------------------------------------- */

static const unsigned char ASCII_CHAR_MIN = 0x20;
static const unsigned char ASCII_CHAR_MAX = 0x7E;

static const unsigned char CR_CHAR = 0x0D;
static const unsigned char LF_CHAR = 0x0A;

static const size_t INDEX_OFFSET = 1u;

/* -------------------------------------------------------------------------- */

/**
 * @brief Check that a given input is encoded using ASCII characters.
 *
 * @param[in]   text            Pointer to input byte buffer.
 * @param[in]   text_len        Length of input byte buffer.
 * @param[in]   allow_new_lines Allow ASCII text to contain new lines.
 *
 * @return true if the text is valid ascii, otherwise false.
 */
bool check_ascii(const uint8_t *text, size_t text_len, bool allow_new_lines) {
    for (size_t idx = 0u; idx < text_len; idx++) {
        const bool is_cr = (unsigned char)text[idx] == CR_CHAR;
        const bool is_lf = (unsigned char)text[idx] == LF_CHAR;
        const bool is_crlf = (is_cr) && ((idx + INDEX_OFFSET) < text_len) &&
                             ((unsigned char)(text[idx + INDEX_OFFSET]) == LF_CHAR);

        // Reject on standalone carriage return when newlines are allowed
        if (allow_new_lines && is_cr && !is_crlf) {
            return false;
        }

        // Reject on unpermitted control character or invalid ascii
        if (!((allow_new_lines && (is_lf || is_crlf || is_cr)) ||
              ((((unsigned char)text[idx]) >= ASCII_CHAR_MIN) &&
               (((unsigned char)text[idx]) <= ASCII_CHAR_MAX)))) {
            return false;
        }
    }
    return true;
}
