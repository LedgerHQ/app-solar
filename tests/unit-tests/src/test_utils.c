
#include "utils/check_ascii.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <cmocka.h>

static void test_utils_check_ascii(void **state) {
    (void)state;

    const uint8_t good_ascii[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x21};                 // Hello!
    const uint8_t bad_ascii[] = {0x32, 0x00, 0x32, 0x3d, 0x34};                        // 2\02=4
    const uint8_t good_newline_ascii1[] = {0x63, 0x69, 0x61, 0x6F, 0x0a};              // ciao\n
    const uint8_t good_newline_ascii2[] = {0x32, 0x0a, 0x32, 0x3d, 0x34, 0x0d, 0x0a};  // ciao\r\n
    const uint8_t bad_newline_ascii[] = {0x32, 0x0a, 0x32, 0x3d, 0x34, 0x0d};          // ciao\r

    // Test case 1: Valid standard input
    assert_true(check_ascii(good_ascii, sizeof(good_ascii), false));

    // Test case 2: Valid input containing a line feed (LF) control character
    assert_true(check_ascii(good_newline_ascii1, sizeof(good_newline_ascii1), true));

    // Test case 3: Valid input containing a carriage return (CR) and a
    // line feed (LF) control character
    assert_true(check_ascii(good_newline_ascii2, sizeof(good_newline_ascii2), true));
    // Test case 4: Invalid input containing only a carriage return (CR)
    // control character
    assert_false(check_ascii(bad_newline_ascii, sizeof(bad_newline_ascii), true));

    // Test case 5: Invalid ASCII
    assert_false(check_ascii(bad_ascii, sizeof(bad_ascii), false));

    // Test case 6: Invalid input containing a line feed (LF) control
    // character when false is passed to the 'allow_new_lines' parameter
    assert_false(check_ascii(good_newline_ascii1, sizeof(good_newline_ascii1), false));

    // Test case 7: Invalid input containing a carriage return line feed (CRLF)
    // control character when false is passed to the 'allow_new_lines' parameter
    assert_false(check_ascii(good_newline_ascii2, sizeof(good_newline_ascii2), false));
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_utils_check_ascii),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
