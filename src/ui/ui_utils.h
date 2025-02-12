#pragma once

#include <stdbool.h>
#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

bool format_amount(char *dst,
                   size_t dst_len,
                   const uint64_t value,
                   uint8_t decimals,
                   const char *ticker,
                   uint8_t ticker_len);

bool format_amount_sxp(char *dst, size_t dst_len, const uint64_t value);

bool format_percent(char *dst, size_t dst_len, const uint16_t value, uint8_t decimals);

int format_hex_lower(const uint8_t *in, size_t in_len, char *out, size_t out_len);

void unpad_amount(char *amount, size_t len, size_t padding);

char get_ticker_line_break(char *num_str);
