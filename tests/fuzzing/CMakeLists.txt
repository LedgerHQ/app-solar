cmake_minimum_required(VERSION 3.10)

if(${CMAKE_VERSION} VERSION_LESS 3.10)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

################################################################################
# Project Definition                                                           #
################################################################################

project(
  SolarFuzzing
  VERSION 1.2.0
  DESCRIPTION "Fuzzing of the Solar app"
  LANGUAGES C)

################################################################################
# Project Configuration                                                        #
################################################################################

# Ensure Clang is used
if(NOT CMAKE_C_COMPILER_ID MATCHES "Clang")
  message(FATAL_ERROR "Fuzzer needs to be built with Clang")
endif()

# Guard against in-source builds
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR
    "In-source builds are not allowed. Create a separate build directory:\n"
    "  rm -rf CMakeCache.txt\n"
    "  mkdir build && cd build\n"
    "  cmake .."
  )
endif()

# Set C Standard and Debug Type
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

################################################################################
# Ledger Configuration                                                         #
################################################################################

set(BOLOS_SDK $ENV{NANOX_SDK})

################################################################################
# Common Directories                                                           #
################################################################################

set(SOLAR_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../../src")
set(SOLAR_FUZZING_SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(SOLAR_FUZZING_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

################################################################################
# Compilation Flags                                                            #
################################################################################

set(
  COMPILATION_FLAGS
  "-Wall"
  "-Wcast-align"
  "-Wextra"
  "-pedantic"
  "-g"
  "-O0")

if(NOT DEFINED "${LIB_FUZZING_ENGINE}")
  set(SANITISER_FLAGS
    "-fsanitize=fuzzer,address,alignment,leak,undefined"
    "-fno-sanitize-recover=undefined")
else()
  set(SANITISER_FLAGS "${LIB_FUZZING_ENGINE}" "${CFLAGS}")
endif()

list(APPEND COMPILATION_FLAGS "${SANITISER_FLAGS}")

# Enable Code Coverage if requested
if(CODE_COVERAGE)
  if(CMAKE_C_COMPILER_ID MATCHES "(Apple)?[Cc]lang")
    set(COVERAGE_FLAGS "-fprofile-instr-generate" "-fcoverage-mapping")
    list(APPEND COMPILATION_FLAGS "${COVERAGE_FLAGS}")
  else()
    message(FATAL_ERROR "Code coverage is only supported with Clang.")
  endif()
endif()

################################################################################
# Compilation Options                                                          #
################################################################################

# Apply compilation and linker flags
add_compile_options("${COMPILATION_FLAGS}")
add_link_options("${COMPILATION_FLAGS}")

# Export compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

################################################################################
# Fuzzing Target Source Directory                                              #
################################################################################

add_subdirectory("${SOLAR_FUZZING_SRC_DIR}")
