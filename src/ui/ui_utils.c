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

#include "ui/ui_utils.h"

#include <stdbool.h>
#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t
#include <string.h>  // strlcat, strnlen, explicit_bzero

#include <format.h>    // format_fpu64, format_hex
#include <os_print.h>  // snprintf

#include "constants.h"

/* -------------------------------------------------------------------------- */

// The minimum number of characters required to represent any uint64_t value as a float string.
// digits (20) + sizeof('.') + decimals (8) + sizeof('\0')
#define AMOUNT_BUFFER_SIZE ((size_t)30u)

static const uint8_t AMOUNT_SXP_DECIMALS = 8u;  // (N SXP = N * 10^8)

static const char TICKER_SXP[] = "SXP";

static const uint8_t TICKER_SXP_LEN = 3u;
static const uint8_t TICKER_MIN_LEN = 3u;
static const uint8_t TICKER_MAX_LEN = 5u;

static const uint8_t AMOUNT_ZEROES_PADDING_LEN = 2u;
static const uint8_t DECIMAL_CHAR_LEN = 1u;
static const uint8_t SPACE_CHAR_LEN = 1u;
static const size_t PERCENT_BUFFER_MIN_LEN = 9u;

static const unsigned char NULL_TERMINATOR_CHAR = 0x00;  // '\0'
static const unsigned char PERIOD_CHAR = 0x2e;           // '.'
static const unsigned char ZERO_CHAR = 0x30;             // '0'
static const unsigned char A_CHAR_UPPER_CASE = 0x41;     // 'A'
static const unsigned char Z_CHAR_UPPER_CASE = 0x5A;     // 'Z'
static const unsigned char A_CHAR_LOWER_CASE = 0x61;     // 'a'

static const char PERCENT_STRING[] = "%";

/* -------------------------------------------------------------------------- */

// Calculate the minimum buffer size needed to create a formatted amount string.
static uint8_t get_amount_out_min(const char *amount, uint8_t ticker_len) {
    uint8_t digits = 0u;
    while ((digits < UINT8_MAX) && ((unsigned char)amount[digits] != NULL_TERMINATOR_CHAR)) {
        digits++;
    }
    return digits + DECIMAL_CHAR_LEN + SPACE_CHAR_LEN + ticker_len + NULL_TERMINATOR_LEN;
}

/**
 * @brief Format 64-bit unsigned integer as string with decimals and ticker.
 *
 * @param[out]  dst         Pointer to output string.
 * @param[in]   dst_len     Length of output string.
 * @param[in]   value       64-bit unsigned integer to format.
 * @param[in]   decimals    Number of digits after decimal separator.
 * @param[in]   ticker      The token's ticker name to be displayed.
 * @param[in]   ticker_len  The length of the ticker name (not including the null-terminator).
 *
 * @return true if successful, otherwise false.
 */
bool format_amount(char *dst,
                   size_t dst_len,
                   const uint64_t value,
                   uint8_t decimals,
                   const char *ticker,
                   uint8_t ticker_len) {
    if (((dst == NULL) || (ticker == NULL)) ||
        ((ticker_len < TICKER_MIN_LEN) || (ticker_len > TICKER_MAX_LEN))) {
        return false;
    }

    char amount[AMOUNT_BUFFER_SIZE] = {0};

    // Format the u64 value to an amount string of the given precision
    if (!format_fpu64(amount, AMOUNT_BUFFER_SIZE, value, decimals)) {
        return false;
    }

    if (dst_len < get_amount_out_min(amount, ticker_len)) {
        return false;
    }

    unpad_amount(amount, (size_t)strnlen(amount, AMOUNT_BUFFER_SIZE), AMOUNT_ZEROES_PADDING_LEN);

    (void)snprintf(dst,
                   (int)dst_len,
                   "%.*s %.*s",
                   (int)strnlen(amount, AMOUNT_BUFFER_SIZE),
                   amount,
                   (int)ticker_len,
                   ticker);

    return true;
}

/**
 * @brief Format 64-bit unsigned integer as an SXP amount string with decimals.
 *
 * @param[out]  dst     Pointer to output string.
 * @param[in]   dst_len Length of output string.
 * @param[in]   value   64-bit unsigned integer to format.
 *
 * @return true if successful, otherwise false.
 */
bool format_amount_sxp(char *dst, size_t dst_len, const uint64_t value) {
    return format_amount(dst, dst_len, value, AMOUNT_SXP_DECIMALS, TICKER_SXP, TICKER_SXP_LEN);
}

/**
 * @brief Format 16-bit unsigned integer as string with decimals and a percent sign.
 *
 * @param[out]  dst         Pointer to the output string.
 * @param[in]   dst_len     Length of the output string.
 * @param[in]   value       16-bit unsigned integer to format.
 * @param[in]   decimals    Number of digits after decimal separator.
 *
 * @return true if successful, otherwise false.
 */
bool format_percent(char *dst, size_t dst_len, const uint16_t value, uint8_t decimals) {
    if (((dst == NULL) || (dst_len < PERCENT_BUFFER_MIN_LEN))) {
        return false;
    }

    if (!format_fpu64(dst, dst_len, (uint64_t)value, decimals)) {
        return false;
    }

    (void)strlcat(dst, PERCENT_STRING, dst_len);

    return true;
}

/**
 * @brief Format a byte buffer to a lowercase hex string including the null-terminator.
 *
 * @param[in]   in      Pointer to input byte buffer.
 * @param[in]   in_len  Length of input.
 * @param[out]  out     Pointer to the output string.
 * @param[in]   out_len Length of the output string (must account for
 *
 * @return the number of characters written (including null-terminator) if successful, otherwise -1.
 */
int format_hex_lower(const uint8_t *in, size_t in_len, char *out, size_t out_len) {
    if ((in == NULL) || (out == NULL)) {
        return -1;
    }

    int written = format_hex(in, in_len, out, out_len);
    if (written < 0) {
        return -1;
    }

    int idx = 0;
    while ((unsigned char)out[idx] != NULL_TERMINATOR_CHAR) {
        if (((unsigned char)out[idx] >= A_CHAR_UPPER_CASE) &&
            ((unsigned char)out[idx] <= Z_CHAR_UPPER_CASE)) {
            out[idx] = (uint8_t)((unsigned char)out[idx] - A_CHAR_UPPER_CASE + A_CHAR_LOWER_CASE);
        }
        idx++;
    }

    return written;
}

/**
 * @brief Remove trailing zeros up to the decimal + padding.
 *
 * @param[in]   amount  Pointer to amount string.
 * @param[in]   len     Length of amount string.
 * @param[in]   padding Length of zero padding to keep.
 */
void unpad_amount(char *amount, size_t len, size_t padding) {
    size_t idx = len - NULL_TERMINATOR_LEN;

    // Loop backwards over the amount string
    while ((idx > 0u) && ((unsigned char)amount[idx] == ZERO_CHAR)) {
        if ((idx >= padding) && ((unsigned char)amount[idx - padding] == PERIOD_CHAR)) {
            break;
        }

        amount[idx] = NULL_TERMINATOR_CHAR;
        idx--;
    }
}
