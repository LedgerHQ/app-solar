/*****************************************************************************
 *  Copyright (c) Solar Network <hello@solar.org>
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 *****************************************************************************/

#pragma once

#include <stddef.h>   // size_t
#include <stdbool.h>  // bool
#include <stdint.h>   // uint*_t

#include "buffer.h"

/**
 * Handler for GET_ADDRESS command.
 *
 * @see G_context.bip32_path, G_context.pk_info.raw_public_key.
 *
 * @param[in,out] cdata
 *   Command data with BIP32 path.
 * @param[in]     user_approval
 *   0 for not displaying on screen, 1 otherwise.
 *
 * @return zero or positive integer if success, negative integer otherwise.
 *
 */
int handler_get_address(buffer_t *cdata, bool user_approval, uint8_t network);
