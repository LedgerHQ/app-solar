#pragma once

#include <stdbool.h>
#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

bool check_ascii(const uint8_t *text, size_t text_len, bool allow_new_lines);
