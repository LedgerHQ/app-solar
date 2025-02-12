#pragma once

#include <stdbool.h>
#include <stdint.h>  // uint*_t

#include <buffer.h>

int handler_sign_tx(buffer_t *cdata, uint8_t chunk, bool more, bool is_message);
