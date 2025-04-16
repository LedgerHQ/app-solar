#pragma once

#include <buffer.h>  // buffer_t

#include "transaction/transaction_errors.h"
#include "transaction/types.h"

parser_status_e deserialise_message(buffer_t *buf, transaction_t *tx);
