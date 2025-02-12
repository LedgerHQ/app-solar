#pragma once

#include <stdbool.h>

#include <buffer.h>  // buffer_t

int handler_get_public_key(buffer_t *cdata, bool user_approval, bool use_chain_code);
