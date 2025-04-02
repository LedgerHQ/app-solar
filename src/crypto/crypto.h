#pragma once

#include <stdbool.h>
#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

#include <buffer.h>

#include "constants.h"

bool hash_ripemd160(const uint8_t *in, size_t in_len, uint8_t out[HASH_20_LEN]);

bool hash_sha256(const uint8_t *in, size_t in_len, uint8_t out[HASH_32_LEN]);

int parse_bip32_path(buffer_t *cdata);

int prepare_public_key(buffer_t *cdata, bool use_chain_code);

int sign_message(void);

int verify_message_hash(void);
