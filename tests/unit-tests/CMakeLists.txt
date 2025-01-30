cmake_minimum_required(VERSION 3.10)

if(${CMAKE_VERSION} VERSION_LESS 3.10)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

################################################################################
# Project Definition                                                           #
################################################################################

project(
  SolarUnitTests
  VERSION 1.1.3
  DESCRIPTION "Unit tests for the Solar app"
  LANGUAGES C)

################################################################################
# Project Configuration                                                        #
################################################################################

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

# Ensure BOLOS_SDK is defined
if(NOT DEFINED ENV{BOLOS_SDK})
  message(FATAL_ERROR "BOLOS_SDK environment variable not found.")
endif()

################################################################################
# Common Directories                                                           #
################################################################################

set(SOLAR_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../../src")
set(SOLAR_UNIT_TEST_SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(SOLAR_UNIT_TEST_BINARY_DIR "${CMAKE_SOURCE_DIR}/build/src")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

################################################################################
# Compilation Flags                                                            #
################################################################################

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -pedantic -g -O0 --coverage")
set(GCC_COVERAGE_LINK_FLAGS "--coverage -lgcov")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

################################################################################
# Test Configuration                                                           #
################################################################################

include(CTest)
enable_testing()

add_compile_definitions(TEST)

################################################################################
# Test Modules                                                                 #
################################################################################

add_subdirectory("${SOLAR_UNIT_TEST_SRC_DIR}")
