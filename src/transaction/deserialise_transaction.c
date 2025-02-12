/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#include "deserialise_transaction.h"

#include <stdbool.h>
#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

#include <buffer.h>  // buffer_t, buffer_read_u8, buffer_seek_cur...

#include "constants.h"

#include "transaction/transaction_errors.h"

#include "transaction/types.h"

#include "transaction/types/types.h"
#include "transaction/types/burn.h"
#include "transaction/types/ipfs.h"
#include "transaction/types/transfer.h"
#include "transaction/types/vote.h"

#include "utils/check_ascii.h"

/* -------------------------------------------------------------------------- */

/**
 * @brief Deserialise raw transaction in structure.
 *
 * @param[in, out]  buf Pointer to buffer with serialised transaction.
 * @param[out]      tx  Pointer to transaction structure.
 *
 * @return PARSING_OK if successful, otherwise an error status (parser_status_e)
 */
parser_status_e deserialise_transaction(buffer_t *buf, transaction_t *tx) {
    parser_status_e parse_common = deserialise_transaction_common(buf, tx);

    if (parse_common != PARSING_OK) {
        return parse_common;
    }

    return deserialise_transaction_asset(buf, &tx->asset, tx->type, tx->typeGroup);
}

/**
 * @brief Deserialise common part of the raw transaction.
 *
 * @param[in, out]  buf Pointer to buffer with serialised transaction.
 * @param[out]      tx  Pointer to transaction structure.
 *
 * @return PARSING_OK if successful, otherwise an error status (parser_status_e)
 */
parser_status_e deserialise_transaction_common(buffer_t *buf, transaction_t *tx) {
    const size_t NONCE_SIZE = 8u;

    uint8_t starting_byte = 0u;
    uint8_t version = 0u;
    uint8_t network = 0u;
    uint8_t memo_len = 0u;

    // starting byte
    if (!buffer_read_u8(buf, &starting_byte)) {
        return WRONG_LENGTH_ERROR;
    }

    if (starting_byte != STARTING_BYTE) {
        return STARTING_BYTE_PARSING_ERROR;
    }

    // version
    if (!buffer_read_u8(buf, &version)) {
        return WRONG_LENGTH_ERROR;
    }

    if (version != TRANSACTION_VERSION_3) {
        return VERSION_PARSING_ERROR;
    }

    // network
    if (!buffer_read_u8(buf, &network)) {
        return WRONG_LENGTH_ERROR;
    }

    if ((network != NETWORK_SOLAR_MAINNET) && (network != NETWORK_SOLAR_TESTNET)) {
        return NETWORK_PARSING_ERROR;
    }

    // typeGroup
    if (!buffer_read_u32(buf, &tx->typeGroup, LE)) {
        return WRONG_LENGTH_ERROR;
    }

    // type
    if (!buffer_read_u16(buf, &tx->type, LE)) {
        return WRONG_LENGTH_ERROR;
    }

    // nonce
    if (!buffer_seek_cur(buf, NONCE_SIZE)) {
        return WRONG_LENGTH_ERROR;
    }

    tx->sender_publickey = &buf->ptr[buf->offset];

    // Sender publickey
    if (!buffer_seek_cur(buf, (size_t)PUBKEY_BYTE_LEN)) {
        return WRONG_LENGTH_ERROR;
    }

    // fee
    if (!buffer_read_u64(buf, &tx->fee, LE)) {
        return WRONG_LENGTH_ERROR;
    }

    // memo length
    if (!buffer_read_u8(buf, &memo_len)) {
        return WRONG_LENGTH_ERROR;
    }
    tx->memo_len = memo_len;

    // memo
    tx->memo = &buf->ptr[buf->offset];

    if (!buffer_seek_cur(buf, (size_t)tx->memo_len)) {
        return WRONG_LENGTH_ERROR;
    }

    if (!check_ascii(tx->memo, tx->memo_len, false)) {
        return MEMO_ENCODING_ERROR;
    }

    return PARSING_OK;
}

/**
 * @brief Deserialise transaction specific asset of the raw transaction.
 *
 * @param[in, out]  buf         Pointer to buffer with serialised transaction.
 * @param[out]      tx          Pointer to transaction assets structure.
 * @param[in]       type        Transaction type.
 * @param[in]       typeGroup   Transaction typeGroup.
 *
 * @return PARSING_OK if successful, otherwise an error status (parser_status_e)
 */
parser_status_e deserialise_transaction_asset(buffer_t *buf,
                                              transaction_asset_t *asset,
                                              uint16_t type,
                                              uint32_t typeGroup) {
    if (typeGroup == TYPEGROUP_CORE) {
        switch (type) {
            case CORE_TRANSACTION_TYPE_IPFS:
                return deserialise_ipfs(buf, &asset->Ipfs);
            case CORE_TRANSACTION_TYPE_TRANSFER:
                return deserialise_transfer(buf, &asset->Transfer);
            default:
                return TYPE_PARSING_ERROR;
        }
    }

    if (typeGroup == TYPEGROUP_SOLAR) {
        switch (type) {
            case SOLAR_TRANSACTION_TYPE_BURN:
                return deserialise_burn(buf, &asset->Burn);
            case SOLAR_TRANSACTION_TYPE_VOTE:
                return deserialise_vote(buf, &asset->Vote);
            default:
                return TYPE_PARSING_ERROR;
        }
    }

    return TYPE_PARSING_ERROR;
}
