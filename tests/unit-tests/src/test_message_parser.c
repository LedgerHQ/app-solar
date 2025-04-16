#include "transaction/deserialise_message.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <cmocka.h>

#include "transaction/transaction_errors.h"

static void test_message_serialization(void **state) {
    (void)state;

    transaction_t tx;
    // clang-format off
    uint8_t valid_message[] = {
        0x07, 0x00, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
        // Messages require to be 1 byte shorter than its buffer
        // to allow space for the null byte
        // This byte will be replaced by the null byte
        0xff
    };

    uint8_t *message_ptr = (uint8_t *) valid_message + 2u;

    uint8_t not_ascii_message[] = {
        0x07, 0x00, 0x63, 0x63, 0x12, 0x63, 0x63, 0x63, 0x63,
        // This byte will be replaced by the null byte
        0xff
    };

    uint8_t empty_message[] = {
        0x00, 0x00,
        // This byte will be replaced by the null byte
        0xff
    };

    uint8_t wrong_length1[] = {
        0x05, 0x00, 0x63, 0x63, 0x63, 0x63,
        // This byte will be replaced by the null byte
        0xff
    };

    uint8_t wrong_length2[] = {
        0x05, 0x00, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
        // This byte will be replaced by the null byte
        0xff
    };

    uint8_t wrong_length3[] = {
        0xff, 0xff
    };

    buffer_t buf1 = {.ptr = valid_message,
                      .size = sizeof(valid_message) - 1u,
                      .offset = 0u};
    buffer_t buf2 = {.ptr = not_ascii_message,
                      .size = sizeof(not_ascii_message) - 1u,
                      .offset = 0u};
    buffer_t buf3 = {.ptr = empty_message,
                      .size = sizeof(empty_message) - 1u,
                      .offset = 0u};
    buffer_t buf4 = {.ptr = wrong_length1,
                      .size = sizeof(wrong_length1) - 1u,
                      .offset = 0u};
    buffer_t buf5 = {.ptr = wrong_length2,
                      .size = sizeof(wrong_length2) - 1u,
                      .offset = 0u};
    buffer_t buf6 = {.ptr = wrong_length3,
                      .size = 0u,
                      .offset = 0u};
    buffer_t buf7 = {.ptr = wrong_length3,
                      .size = 0xFFFFu,
                      .offset = 0u};

    parser_status_e status;

    status = deserialise_message(&buf1, &tx);
    assert_int_equal(status, PARSING_OK);
    assert_int_equal(tx.message_length, 0x7);
    assert_ptr_equal(tx.message, message_ptr);
    memset(&status, 0, sizeof(status));

    status = deserialise_message(&buf2, &tx);
    assert_int_equal(status, MESSAGE_ENCODING_ERROR);
    memset(&status, 0, sizeof(status));

    status = deserialise_message(&buf3, &tx);
    assert_int_equal(status, WRONG_LENGTH_ERROR);
    memset(&status, 0, sizeof(status));

    status = deserialise_message(&buf4, &tx);
    assert_int_equal(status, TX_ASSET_PARSING_ERROR);
    memset(&status, 0, sizeof(status));

    status = deserialise_message(&buf5, &tx);
    assert_int_equal(status, WRONG_LENGTH_ERROR);
    memset(&status, 0, sizeof(status));

    status = deserialise_message(&buf6, &tx);
    assert_int_equal(status, TX_ASSET_PARSING_ERROR);

    status = deserialise_message(&buf7, &tx);
    assert_int_equal(status, WRONG_LENGTH_ERROR);
}

int main() {
    const struct CMUnitTest tests[] = {cmocka_unit_test(test_message_serialization)};

    return cmocka_run_group_tests(tests, NULL, NULL);
}
