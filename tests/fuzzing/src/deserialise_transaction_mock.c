#include <buffer.h>

#include "transaction/types.h"
#include "transaction/deserialise_transaction.h"

parser_status_e deserialise_transaction(buffer_t *buf, __attribute__((unused)) transaction_t *tx) {
    if (buf->ptr[0] != (const uint8_t)0xFF) {
        // Simulate failure
        return WRONG_LENGTH_ERROR;
    }

    parser_status_e parse_common = deserialise_transaction_common(buf, tx);

    if (parse_common != PARSING_OK) {
        return parse_common;
    }

    return deserialise_transaction_asset(buf, &tx->asset, tx->type, tx->typeGroup);
}
