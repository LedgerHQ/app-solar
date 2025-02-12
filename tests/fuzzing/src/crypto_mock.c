
#include <stdbool.h>
#include <stdint.h>  // uint*_t
#include <string.h>  // strstr

#include "constants.h"
#include "globals.h"

/* -------------------------------------------------------------------------- */

static const unsigned int DEADBEEF_FLAG = 0xDEADBEEF;
static const char DEADBEEF_STR[] = "DEADBEEF";

static const unsigned int FACEFEED_FLAG = 0xFACEFEED;
static const char FACEFEED_STR[] = "FACEFEED";

static const size_t GENERIC_FLAG_CASE_LENGTH = 5u;

static const size_t HASH_FAIL_CASE_LENGTH = 15u;
static const int HASH_FAIL_CASE_OFFSET = 2;

static const size_t VERIFY_HASH_FAIL_CASE_LENGTH = 68u;
static const int VERIFY_HASH_FAIL_CASE_OFFSET = 59;

static const int CX_OK_FLAG = 0x00000000;

/* -------------------------------------------------------------------------- */

bool hash_sha256(uint8_t *in, size_t in_len, __attribute__((unused)) uint8_t out[HASH_32_LEN]) {
    // Check for the "DEADBEEF" string in the `sign_tx_crypto_hash_fail.bin` seed.
    // If found, simulate a hash failure by returning -1.
    if (in_len == HASH_FAIL_CASE_LENGTH &&
        strstr((char *)&in[HASH_FAIL_CASE_OFFSET], DEADBEEF_STR) != NULL) {
        return false;
    }

    // Check for the "FACEFEED" string in the `sign_tx_crypto_verify_hash_fail.bin` seed.
    // If found, set the FACEFEED_FLAG in G_context.tx_info.m_hash to simulate
    // a failure condition for `verify_message_hash`.
    if ((in_len == VERIFY_HASH_FAIL_CASE_LENGTH) &&
        (strstr((char *)&in[VERIFY_HASH_FAIL_CASE_OFFSET], FACEFEED_STR) != NULL)) {
        const uint32_t facefeed_flag = FACEFEED_FLAG;
        (void)memset(G_context.tx_info.m_hash, 0, HASH_32_LEN);  // Clear the hash buffer
        (void)memcpy(G_context.tx_info.m_hash, &facefeed_flag,
                     sizeof(uint32_t));  // Write the flag
    }

    return true;
}

int parse_bip32_path(buffer_t *cdata) {
    // Catch the `0xdeadbeef` flag from the various seeds.
    if ((cdata->size >= GENERIC_FLAG_CASE_LENGTH)) {
        const uint32_t unpackedBE = ((uint32_t)cdata->ptr[1] << 24) |
                                    ((uint32_t)cdata->ptr[2] << 16) |
                                    ((uint32_t)cdata->ptr[3] << 8) | ((uint32_t)cdata->ptr[4]);

        if (unpackedBE == DEADBEEF_FLAG) {
            return -1;
        }
    }

    return 0;
}

int prepare_public_key(buffer_t *cdata, __attribute__((unused)) bool use_chain_code) {
    // Catch the `0xdeadbeef` flag from the various seeds.
    if ((cdata->size >= GENERIC_FLAG_CASE_LENGTH)) {
        const uint32_t unpackedBE = ((uint32_t)cdata->ptr[1] << 24) |
                                    ((uint32_t)cdata->ptr[2] << 16) |
                                    ((uint32_t)cdata->ptr[3] << 8) | ((uint32_t)cdata->ptr[4]);

        if (unpackedBE == DEADBEEF_FLAG) {
            return -1;
        }
    }

    return 0;
}

int verify_message_hash(void) {
    // Reverse the byte order since mock::hash_sha256 wrote the flag in little-endian format.
    const uint32_t unpackedLE = ((uint32_t)G_context.tx_info.m_hash[3] << 24) |
                                ((uint32_t)G_context.tx_info.m_hash[2] << 16) |
                                ((uint32_t)G_context.tx_info.m_hash[1] << 8) |
                                ((uint32_t)G_context.tx_info.m_hash[0]);

    if (unpackedLE == FACEFEED_FLAG) {
        return -1;
    }

    return CX_OK_FLAG;
}
