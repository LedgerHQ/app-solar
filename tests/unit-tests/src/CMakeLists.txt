cmake_minimum_required(VERSION 3.10)

if(${CMAKE_VERSION} VERSION_LESS 3.10)
  cmake_policy(VERSION "${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}")
endif()

################################################################################
# Module Libraries                                                             #
################################################################################

# Message Deserialiser
include("${CMAKE_MODULE_PATH}/MessageDeserialiser.cmake")
add_executable(test_message_deserialiser "${SOLAR_UNIT_TEST_SRC_DIR}/test_message_parser.c")
target_link_libraries(test_message_deserialiser PUBLIC cmocka gcov message_deserialiser)
add_test(test_message_deserialiser "${SOLAR_UNIT_TEST_BINARY_DIR}/test_message_deserialiser")

# Transaction Deserialiser
include("${CMAKE_MODULE_PATH}/TransactionDeserialiser.cmake")
add_executable(test_transaction_deserialiser "${SOLAR_UNIT_TEST_SRC_DIR}/test_tx_parser.c")
target_link_libraries(test_transaction_deserialiser PUBLIC cmocka gcov transaction_deserialiser)
add_test(test_transaction_deserialiser "${SOLAR_UNIT_TEST_BINARY_DIR}/test_transaction_deserialiser")

# Transaction Utils
include("${CMAKE_MODULE_PATH}/Utils.cmake")
add_executable(test_utils "${SOLAR_UNIT_TEST_SRC_DIR}/test_utils.c")
target_link_libraries(test_utils PUBLIC cmocka gcov utils)
add_test(test_utils "${SOLAR_UNIT_TEST_BINARY_DIR}/test_utils")

# UI Utils
include("${CMAKE_MODULE_PATH}/UiUtils.cmake")
add_executable(test_ui_utils "${SOLAR_UNIT_TEST_SRC_DIR}/test_ui_utils.c")
target_link_libraries(test_ui_utils PUBLIC cmocka gcov ui_utils)
add_test(test_ui_utils "${SOLAR_UNIT_TEST_BINARY_DIR}/test_ui_utils")
