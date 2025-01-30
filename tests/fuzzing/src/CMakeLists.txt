cmake_minimum_required(VERSION 3.10)

if(${CMAKE_VERSION} VERSION_LESS 3.10)
  cmake_policy(VERSION "${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}")
endif()

################################################################################
# Target Libraries                                                             #
################################################################################

# APDU Dispatcher Fuzzer
include("${CMAKE_MODULE_PATH}/Dispatcher.cmake")
target_compile_options(dispatcher PUBLIC "${COVERAGE_FLAGS}")
add_executable(dispatcher_fuzzer "${SOLAR_FUZZING_SRC_DIR}/dispatcher_target.c")
target_link_libraries(dispatcher_fuzzer PUBLIC dispatcher)
