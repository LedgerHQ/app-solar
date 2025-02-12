#pragma once

#include <stdbool.h>
#include <stdint.h>  // uint*_t

#include <buffer.h>

int handler_get_address(buffer_t *cdata, bool user_approval, uint8_t network);
