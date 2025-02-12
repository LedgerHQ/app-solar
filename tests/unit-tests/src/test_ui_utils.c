#include "ui/ui_utils.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include <cmocka.h>

#include "constants.h"
#include "ui/ui_constants.h"

/* -------------------------------------------------------------------------- */

#define AMOUNT_BUFFER_SIZE ((size_t)30u) /** @see ui/ui_utils.c */

/* -------------------------------------------------------------------------- */

// Fills a buffer of a given length with random bytes.
// Used in 'format_hex_lower' tests
static inline void generate_random_buffer(uint8_t *buffer, size_t length) {
    for (size_t idx = 0u; idx < length; idx++) {
        buffer[idx] = (uint8_t)(rand() % 256u);
    }
}

/* -------------------------------------------------------------------------- */

static void test_ui_utils_format_amount(void **state) {
    (void)state;

    char temp[AMOUNT_BUFFER_SIZE] = {0};

    // Test case 1: 1 SXP (1.00 SXP)
    uint64_t amount = 100000000ull;
    assert_true(format_amount(temp, sizeof(temp), amount, 8u, "SXP", 3u));
    assert_string_equal(temp, "1.00 SXP");
    memset(temp, 0, sizeof(temp));

    // Test case 2: Random value (0.24964823 SXP)
    amount = 24964823ull;
    assert_true(format_amount(temp, sizeof(temp), amount, 8u, "SXP", 3u));
    assert_string_equal(temp, "0.24964823 SXP");
    memset(temp, 0, sizeof(temp));

    // Test case 3: Small value (0.000001 SXP)
    amount = 1ull;
    assert_true(format_amount(temp, sizeof(temp), amount, 8u, "SXP", 3u));
    assert_string_equal(temp, "0.00000001 SXP");
    memset(temp, 0, sizeof(temp));

    // Test case 4: Zero value (0.00 SXP)
    amount = 0ull;
    assert_true(format_amount(temp, sizeof(temp), amount, 8u, "SXP", 3u));
    assert_string_equal(temp, "0.00 SXP");
    memset(temp, 0, sizeof(temp));

    // Test case 5: Large value (100000000000.00 SXP)
    amount = 10000000000000000000ull;
    assert_true(format_amount(temp, sizeof(temp), amount, 8u, "SXP", 3u));
    assert_string_equal(temp, "100000000000.00 SXP");
    memset(temp, 0, sizeof(temp));

    // Test case 6: Max value (UINT64_MAX: 184467440737.09551615\nSXP)
    amount = UINT64_MAX;
    assert_true(format_amount(temp, sizeof(temp), amount, 8u, "SXP", 3u));
    assert_string_equal(temp, "184467440737.09551615 SXP");
    memset(temp, 0, sizeof(temp));

    // Test case 7: Buffer too small
    assert_false(format_amount(temp, 20u, amount, 8u, "SXP", 3u));

    // Test case 8: NULL destination buffer
    assert_false(format_amount(NULL, sizeof(temp), amount, 8u, "SXP", 3u));

    // Test case 9: Zero output length
    assert_false(format_amount(temp, 0u, amount, 8u, "SXP", 3u));

    // Test case 10: Excessive decimals
    // Triggers a failure within the Ledger standard library's 'format_fpu64'
    amount = 1ull;           // Small value
    uint8_t decimals = 30u;  // Force decimals to exceed digits in value
    assert_false(format_amount(temp, 5u, amount, decimals, "SXP", 3u));
}

static void test_ui_utils_format_sxp_amount(void **state) {
    (void)state;

    char temp[AMOUNT_BUFFER_SIZE] = {0};

    // Test case 1: 1 SXP (1.00 SXP)
    uint64_t amount = 100000000ull;
    memset(temp, 0, sizeof(temp));
    assert_true(format_amount_sxp(temp, sizeof(temp), amount));
    assert_string_equal(temp, "1.00 SXP");

    // Test case 2: Random value (0.24964823 SXP)
    amount = 24964823ull;
    memset(temp, 0, sizeof(temp));
    assert_true(format_amount_sxp(temp, sizeof(temp), amount));
    assert_string_equal(temp, "0.24964823 SXP");

    // Test case 3: Small value (0.000001 SXP)
    amount = 1ull;
    memset(temp, 0, sizeof(temp));
    assert_true(format_amount_sxp(temp, sizeof(temp), amount));
    assert_string_equal(temp, "0.00000001 SXP");

    // Test case 5: Large value (100000000000.00 SXP)
    amount = 10000000000000000000ull;
    assert_true(format_amount_sxp(temp, sizeof(temp), amount));
    assert_string_equal(temp, "100000000000.00 SXP");

    // Test case 6: Max value (UINT64_MAX: 184467440737.09551615\nSXP)
    amount = UINT64_MAX;
    assert_true(format_amount_sxp(temp, sizeof(temp), amount));
    assert_string_equal(temp, "184467440737.09551615 SXP");
}

static void test_ui_utils_format_percent(void **state) {
    (void)state;

    char temp[9] = {0};
    uint16_t percent = 0u;
    uint8_t precision = 0u;

    // Test case 1: 100.00%
    percent = 10000u;
    precision = 2u;
    assert_true(format_percent(temp, sizeof(temp), percent, precision));
    assert_string_equal(temp, "100.00%");
    memset(temp, 0, sizeof(temp));

    // Test case 2: Zero value (0.00%)
    percent = 0u;
    assert_true(format_percent(temp, sizeof(temp), percent, precision));
    assert_string_equal(temp, "0.00%");
    memset(temp, 0, sizeof(temp));

    // Test case 3: Random value (12.34%)
    percent = 1234u;
    assert_true(format_percent(temp, sizeof(temp), percent, precision));
    assert_string_equal(temp, "12.34%");
    memset(temp, 0u, sizeof(temp));

    // Test case 4: Small value (0.02%)
    percent = 2u;
    assert_true(format_percent(temp, sizeof(temp), percent, precision));
    assert_string_equal(temp, "0.02%");
    memset(temp, 0, sizeof(temp));

    // // Test case 5: Buffer too small
    percent = 100u;
    assert_false(format_percent(temp, sizeof(temp) - 1u, percent, precision));

    // Test case 6: NULL destination buffer
    precision = 3u;
    assert_false(format_percent(NULL, sizeof(temp), percent, precision));

    // Test case 7: Excessive precision
    // Triggers a failure within the Ledger standard library's 'format_fpu64'
    uint64_t percent_ull = 1000000ull;  // Small value
    precision = 50u;                    // Force decimals to exceed digits in value
    assert_false(format_percent(&temp[2], sizeof(temp), (uint16_t)percent_ull, precision));
}

static void test_format_percent_success(void **state) {
    (void)state;

    char temp[10] = {0};
    uint16_t value = 50u;
    uint8_t decimals = 2u;

    // Test successful formatting with enough buffer
    assert_true(format_percent(temp, sizeof(temp), value, decimals));
    assert_string_equal(temp, "0.50%");
}

static void test_format_percent_buffer_too_small(void **state) {
    (void)state;

    char temp[5];  // Intentionally too small buffer
    uint16_t value = 50u;
    uint8_t decimals = 2u;

    // Test failure when the buffer is too small
    assert_false(format_percent(temp, sizeof(temp), value, decimals));
}

static void test_format_percent_edge_case(void **state) {
    (void)state;

    char temp[10];  // Just enough buffer space for "0.50%"
    uint16_t value = 50u;
    uint8_t decimals = 2u;

    // Test edge case with exactly enough buffer space
    assert_true(format_percent(temp, sizeof(temp), value, decimals));
    assert_string_equal(temp, "0.50%");
}

static void test_ui_utils_format_hex_lower(void **state) {
    (void)state;

    const char pubkey_str[] = "02937e1b9294d07c91b46d511851d698ad606a64950dd3a332fcfac9c0a3fb0bad";
    const uint8_t pubkey_dec[PUBKEY_BYTE_LEN] = {
        2,  147, 126, 27,  146, 148, 208, 124, 145, 180, 109, 81,  24,  81,  214, 152, 173,
        96, 106, 100, 149, 13,  211, 163, 50,  252, 250, 201, 192, 163, 251, 11,  173};
    const uint8_t pubkey_hex[PUBKEY_BYTE_LEN] = {
        0x02, 0x93, 0x7e, 0x1b, 0x92, 0x94, 0xd0, 0x7c, 0x91, 0xb4, 0x6d,
        0x51, 0x18, 0x51, 0xd6, 0x98, 0xad, 0x60, 0x6a, 0x64, 0x95, 0x0d,
        0xd3, 0xa3, 0x32, 0xfc, 0xfa, 0xc9, 0xc0, 0xa3, 0xfb, 0x0b, 0xad};
    const uint8_t single_byte[] = {0xab};

    char pubkey_out[PUBKEY_CHAR_LEN] = {0};
    char temp_out[256] = {0};

    // Test case 1: Public key dec bytes input
    assert_int_equal(
        format_hex_lower(pubkey_dec, PUBKEY_BYTE_LEN, pubkey_out, PUBKEY_CHAR_LEN + 1u),
        PUBKEY_CHAR_LEN + 1);
    assert_string_equal(pubkey_out, pubkey_str);
    memset(pubkey_out, 0, sizeof(pubkey_out));

    // Test case 2: Public key hex bytes input
    assert_int_equal(
        format_hex_lower(pubkey_hex, PUBKEY_BYTE_LEN, pubkey_out, PUBKEY_CHAR_LEN + 1u),
        PUBKEY_CHAR_LEN + 1);
    assert_string_equal(pubkey_out, pubkey_str);
    memset(pubkey_out, 0, sizeof(pubkey_out));

    // Test case 3: Correct number of chars written
    assert_int_equal(
        format_hex_lower(pubkey_hex, PUBKEY_BYTE_LEN, pubkey_out, PUBKEY_CHAR_LEN + 1u),
        sizeof(pubkey_hex) * 2u + 1u);
    assert_string_equal(pubkey_out, pubkey_str);
    memset(pubkey_out, 0, sizeof(pubkey_out));

    // Test case 4: Single byte input
    assert_int_equal(format_hex_lower(single_byte, sizeof(single_byte), temp_out, sizeof(temp_out)),
                     3);
    assert_string_equal(temp_out, "ab");
    memset(temp_out, 0, sizeof(temp_out));

    // Test case 5: Random input buffer
    srand(time(NULL));  // Seed the random number generator
    for (int index = 0; index < UINT8_MAX; index++) {
        const uint16_t rand_size = (uint16_t)(rand() % UINT8_MAX) + 1u;
        const uint16_t out_size = rand_size * 2 + 1;

        uint8_t *rand_in = calloc(rand_size, sizeof(uint8_t));
        char *rand_out = calloc(out_size, sizeof(char));

        generate_random_buffer(rand_in, rand_size);

        assert_int_equal(format_hex_lower(rand_in, rand_size, rand_out, out_size), out_size);
        assert_string_not_equal(rand_out, "");

        free(rand_in);
        free(rand_out);
    }

    // Test case 6: Null input buffer
    assert_int_equal(format_hex_lower(NULL, 1u, temp_out, sizeof(temp_out)), -1);
    assert_string_equal(temp_out, "");

    // Test case 7: Null output buffer
    assert_int_equal(format_hex_lower(single_byte, sizeof(single_byte), NULL, sizeof(temp_out)),
                     -1);
    assert_string_equal(temp_out, "");

    // Test case 8: Insufficient output buffer space
    // (excludes space for the null-terminator)
    assert_int_equal(format_hex_lower(pubkey_hex, PUBKEY_BYTE_LEN, temp_out, PUBKEY_CHAR_LEN), -1);
    assert_string_equal(temp_out, "");

    // Test case 9: Zero-length output buffer
    assert_int_equal(format_hex_lower(single_byte, sizeof(single_byte), temp_out, 0u), -1);
    assert_string_equal(temp_out, "");
}

int main() {
    const struct CMUnitTest tests[] = {cmocka_unit_test(test_ui_utils_format_amount),
                                       cmocka_unit_test(test_ui_utils_format_sxp_amount),
                                       cmocka_unit_test(test_ui_utils_format_percent),
                                       cmocka_unit_test(test_ui_utils_format_hex_lower),
                                       cmocka_unit_test(test_format_percent_edge_case),
                                       cmocka_unit_test(test_format_percent_buffer_too_small),
                                       cmocka_unit_test(test_format_percent_success)};

    return cmocka_run_group_tests(tests, NULL, NULL);
}
