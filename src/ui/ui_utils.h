/*****************************************************************************
 *  Copyright (c) Solar Network <hello@solar.org>
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 *****************************************************************************/

#pragma once

#include <stdint.h>   // uint*_t
#include <stdbool.h>  // bool

#if !defined (TARGET_STAX)
    #include "bagl.h"
#endif

#include "ux.h"

/**
 * Remove trailing zeros up to the decimal + padding.
 *
 * @param[in] amount
 *   Pointer to amount string.
 * @param[in]  len
 *   Length of amount string.
 * @param[in]  padding
 *   Length of zero padding to keep.
 *
 */
void unpad_amount(char *amount, size_t len, size_t padding);

/**
 * Estimate amount/fee line pixel count for Nano S.
 *
 * @param[in] num_str
 *   Pointer to formatted number string.
 *
 * @return estimated pixel count if Nano S, otherwise zero.
 *
 */

#ifdef IS_TARGET_NANOS
    #define GET_NANOS_PIXELS(num_str) (             \
        bagl_compute_line_width(                    \
            (BAGL_FONT_OPEN_SANS_REGULAR_8_11PX),   \
            (0),                                    \
            (num_str),                              \
            (strlen((num_str))),                    \
            (G_ux.layout_paging.format)             \
        ))
#else
    #define GET_NANOS_PIXELS(num_str) (0)
#endif

/**
 * Determine if a new page should be used for ticker display on the Nano S.
 *
 * (e.g., avoid `123456789012345678 / 9.00 SXP` being misread as `9.00 SXP`)
 *
 * @param[in] num_str
 *   Pointer to formatted number string.
 *
 * @return new line (page) character if needed.
 *
 */
#define GET_TICKER_PAGING(num_str) (                                                    \
    ((IS_TARGET_NANOS) && (GET_NANOS_PIXELS((num_str)) >= (PIXEL_PER_LINE - 1)))        \
    ? ('\n')                                                                            \
    : (' ')                                                                             \
)
