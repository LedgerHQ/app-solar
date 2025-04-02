/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#include "burn.h"

#include <buffer.h>  //  buffer_t, buffer_read_u64

#include "transaction/transaction_errors.h"

/* -------------------------------------------------------------------------- */

/**
 * @brief Deserialise a Burn asset.
 *
 * @param[in, out]  buf     Pointer to buffer with serialised transaction.
 * @param[out]      asset   Pointer to transaction asset structure.
 *
 * @return PARSING_OK if successful, otherwise an error status (parser_status_e)
 */
parser_status_e deserialise_burn(buffer_t *buf, burn_asset_t *burn) {
    if (!buffer_read_u64(buf, &burn->amount, LE)) {
        return WRONG_LENGTH_ERROR;
    }

    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
}
