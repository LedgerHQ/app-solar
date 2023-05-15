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

#include <stdint.h>   // uint*_t
#include <stdbool.h>  // bool
#include <string.h>   // memmove
#include <stdio.h>    // snprintf

#include "format.h"
#include "types.h"

#include "utils.h"

/**
 * Remove trailing zeros up to the decimal + padding.
 *
 */
static void unpad_amount(char *amount, size_t len, size_t padding) {
    char *ptr = amount + len - 1;
    while (*ptr == '0' && *(ptr - padding) != '.') {
        *ptr-- = 0;
    }
}

#if (IS_NANOS)  // Estimate amount/fee line pixel count for the Nano S.

// extern from:
// https://github.com/LedgerHQ/nanos-secure-sdk/blob/master/lib_ux/src/ux_layout_paging_compute.c#L20-#L117
extern const char nanos_characters_width[96];

// adapted from:
// https://github.com/LedgerHQ/nanos-secure-sdk/blob/master/lib_ux/src/ux_layout_paging_compute.c#L147-#L175
static uint32_t get_pixels(const char *text, uint8_t text_length) {
    char current_char;
    uint32_t line_width = 0;

    while (text_length--) {
        current_char = *text;
        line_width += (nanos_characters_width[current_char - 0x20] >> 0x04) & 0x0F;
        text++;
    }

    return line_width;
}

#define PXLS(num_str) (get_pixels((num_str), strlen((num_str))))
#else  // if NOT NANOS
#define PXLS(num_str) (0)
#endif

/**
 * Determine if a new page should be used for ticker display on the Nano S.
 *
 * (e.g., avoid `123456789012345678 / 9.00 SXP` being misread as `9.00 SXP`)
 *
 */
#define TICKER_SPACING(num_str) (IS_NANOS && PXLS((num_str)) >= NANO_PXLS_PER_LINE - 1 ? '\n' : ' ')

bool format_amount(char *dst,
                   size_t dst_len,
                   const uint64_t value,
                   uint8_t decimals,
                   const char *ticker,
                   size_t ticker_len) {
    char amount[22] = {0};
    if (dst_len < 22 + 5 || ticker_len > 5) {
        return false;
    }
    if (!format_fpu64(amount, 22, value, decimals)) {
        return false;
    }

    unpad_amount(amount, strlen(amount), 2);

    snprintf(dst, dst_len, "%s%c%s", amount, TICKER_SPACING(amount), ticker);
    return true;
}

bool format_percentage(char *dst, size_t dst_len, const uint16_t value, uint8_t decimals) {
    char amount[22] = {0};
    if (dst_len < 9) {
        return false;
    }
    if (!format_fpu64(amount, 22, (const uint64_t) value, decimals)) {
        return false;
    }
    snprintf(dst, dst_len, "%s%%", amount);
    return true;
}

bool transaction_utils_check_ascii(const uint8_t *text, uint64_t text_len, bool allow_new_lines) {
    for (uint64_t i = 0; i < text_len; i++) {
        bool lf = text[i] == 0x0A;
        bool crlf = text[i] == 0x0D && i + 1 < text_len && text[i + 1] == 0x0A;
        if (!((allow_new_lines && (lf || crlf)) || (text[i] >= 0x20 && text[i] <= 0x7E))) {
            return false;
        }
    }
    return true;
}
