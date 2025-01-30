#pragma once

#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

/* -------------------------------------------------------------------------- */
/* -------------------------------- App Vars -------------------------------- */
#define APPNAME_LEN     ((size_t)5)  // "Solar"
#define APPNAME_MAX_LEN ((size_t)64)
#define APPVERSION_LEN  3  // (MAJOR_VERSION || MINOR_VERSION || PATCH_VERSION)

/* -------------------------------------------------------------------------- */
/* --------------------------------- Hashes --------------------------------- */
#define HASH_20_LEN ((size_t)20)
#define HASH_32_LEN ((size_t)32)
#define HASH_64_LEN ((size_t)64)

#define PUBKEY_HASH_LEN    ((size_t)21)  // [network[1], pubKeyHash[20]]
#define RIPEMD160_HASH_LEN ((size_t)20)
#define SIG_SCHNORR_LEN    ((size_t)64)

/* -------------------------------------------------------------------------- */
/* -------------------------------- Identity -------------------------------- */
#define ADDRESS_CHECKSUM_LEN ((size_t)4)
#define ADDRESS_CHAR_LEN     ((size_t)34)  // does not include the null-terminator

#define PUBKEY_BYTE_LEN ((uint8_t)33)  // compressed-type
#define PUBKEY_CHAR_LEN ((uint8_t)66)  // not including null-terminator

#define MIN_BIP32_PATH ((size_t)0x02)
// #define BIP32_PATH_MAX_LEN 10 (in Ledger SDK <bip32.h>)

/* -------------------------------- Network --------------------------------- */

#define NETWORK_BYTE_SIZE     ((size_t)1)
#define NETWORK_SOLAR_MAINNET ((uint8_t)0x3F)  // 63
#define NETWORK_SOLAR_TESTNET ((uint8_t)0x1E)  // 30

#define NULL_TERMINATOR     '\0'
#define NULL_TERMINATOR_LEN ((uint8_t)1)

/* -------------------------------------------------------------------------- */

/**
 * @brief Maximum transaction length (bytes).
 *
 * The largest possible transaction is a Transfer, which contains one or many payments
 * and an optional 'memo'.
 *
 * Each payment consists of two values: an amount (8 bytes) and an address hash (21 bytes)
 *
 * The size (in bytes) of a serialised Transfer is therefore calculated as:
 *   - header (59 bytes) + memo (0..255 bytes) + payments (2 bytes + 29n)
 *
 * The maximum size of a Transfer occurs when it contains a full memo and 256 payments:
 *   - 59 + 255 + 2 + (29 * 256) = 7,740 bytes.
 *
 * However, the Ledger SDK uses `uint8_t` for pair callbacks, limiting the index count to 255.
 * Because each payment consists of 2 values, the maximum payment count is:
 *   - floor(255 / 2) = 127.
 *
 * Substituting 127 payments into the formula:
 *   - 59 + 255 + 2 + (29 * 127) = 3,999 bytes.
 *
 * To optimise for alignment and performance, we use the nearest power of 2:
 *   - TRANSACTION_LEN_MAX = 4096 bytes.
 */
#define TRANSACTION_LEN_MAX ((size_t)4096)
