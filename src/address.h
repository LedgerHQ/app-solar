#pragma once

#include <stdbool.h>
#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

#include "constants.h"

bool address_from_public_key(const uint8_t public_key[PUBKEY_BYTE_LEN],
                             char out[ADDRESS_CHAR_LEN + NULL_TERMINATOR_LEN],
                             const uint8_t network);

int address_from_pubkey_hash(const uint8_t *in, size_t in_len, char *out, size_t out_len);
