/*****************************************************************************
 *  Copyright (c) Solar Network <hello@solar.org>
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 *****************************************************************************/

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "../transaction/types.h"

#define MAX_TITLE_LEN 16
#define MAX_TEXT_LEN  385

typedef struct {
    bool (*f)(transaction_t *, char *, char *, uint16_t);
    uint16_t offset;
} ctx_t;

/**
 * Get next content to display.
 *
 * @param[in, out] ctx
 *   Pointer to the contect structure.
 * @param[in] tx
 *   Pointer to transaction structure.
 * @param[out] title
 *   Title output.
 * @param[out] text
 *   Text output.
 *
 * @return true if there is content, false otherwise.
 *
 */
bool context_get_next(ctx_t *ctx, transaction_t *tx, char title[], char text[]);

/**
 * Get previous content to display.
 *
 * @param[in, out] ctx
 *   Pointer to the contect structure.
 * @param[in] tx
 *   Pointer to transaction structure.
 * @param[out] title
 *   Title output.
 * @param[out] text
 *   Text output.
 *
 * @return true if there is content, false otherwise.
 *
 */
bool context_get_previous(ctx_t *ctx, transaction_t *tx, char title[], char text[]);
