/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#include "ipfs.h"

#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

#include <buffer.h>  // buffer_t, buffer_seek_cur

#include "transaction/transaction_errors.h"

/* -------------------------------------------------------------------------- */

static const size_t IPFS_HASH_TYPE_SIZE = 1u;
static const uint8_t IPFS_HASH_PREFIX_SIZE = 2u;
static const uint8_t IPFS_HASH_LEN_MIN = 1u;
static const uint8_t IPFS_HASH_LEN_MAX = 64u;

/* -------------------------------------------------------------------------- */

/**
 * @brief Deserialise an IPFS asset of transaction.
 *
 * @param[in, out]  buf     Pointer to buffer with serialised transaction.
 * @param[out]      asset   Pointer to transaction asset structure.
 *
 * @return PARSING_OK if successful, otherwise an error status (parser_status_e)
 */
parser_status_e deserialise_ipfs(buffer_t *buf, ipfs_asset_t *ipfs) {
    // ipfs
    ipfs->hash = &buf->ptr[buf->offset];

    // hash function
    if (!buffer_seek_cur(buf, IPFS_HASH_TYPE_SIZE)) {
        return TX_ASSET_PARSING_ERROR;
    }

    // length
    if (!buffer_read_u8(buf, &ipfs->hash_length)) {
        return TX_ASSET_PARSING_ERROR;
    }

    if ((ipfs->hash_length < IPFS_HASH_LEN_MIN) || (ipfs->hash_length > IPFS_HASH_LEN_MAX)) {
        return WRONG_LENGTH_ERROR;
    }

    if (!buffer_seek_cur(buf, ipfs->hash_length)) {
        return TX_ASSET_PARSING_ERROR;
    }

    ipfs->hash_length += IPFS_HASH_PREFIX_SIZE;

    return (buf->offset == buf->size) ? PARSING_OK : WRONG_LENGTH_ERROR;
}
