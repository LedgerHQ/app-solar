#!/usr/bin/env bash

################################################################################
#  Copyright (c) Solar Network <hello@solar.org>                               #
#                                                                              #
#  This work is licensed under a Creative Commons Attribution-NoDerivatives    #
#  4.0 International License.                                                  #
################################################################################

################################################################################
# Run unit-tests with coverage.
#
# Uses:
#   - sudo docker run  --rm -ti --user "$(id -u):$(id -g)" --privileged -v "$(pwd -P):/app" ghcr.io/ledgerhq/ledger-app-builder/ledger-app-dev-tools:latest
#
# Devices:
#   - BOLOS_SDK=$FLEX_SDK
# 	- BOLOS_SDK=$NANOSP_SDK
# 	- BOLOS_SDK=$NANOX_SDK
# 	- BOLOS_SDK=$STAX_SDK
#
# Usage:
#   - BOLOS_SDK=$STAX_SDK bash tests/unit-tests/cov.sh
#        -> Run unit-tests
#   - BOLOS_SDK=$STAX_SDK bash tests/unit-tests/cov.sh rebuild
#        -> Run unit-tests after deleting all build artifacts
################################################################################

set -e

printf "\n%s\n%s\n%s\n" \
    "==================================================" \
    "             Solar App Tests: Unit-Tests          " \
    "=================================================="

if [[ ! -d ${BOLOS_SDK} ]]; then
    printf "\n%s\t%s\n\n%s\n\n" "Error:" "BOLOS_SDK is not set" "Exiting..."
    exit
else
    printf "\n%s\t%s\n" "BOLOS_SDK:" "${BOLOS_SDK}"
fi

printf "\n%s\n" "Configuring project..."

# Set the directory paths
SCRIPT_DIR="$(cd -- "$(dirname "$0")" >/dev/null 2>&1 && pwd -P)"
BUILD_DIR="${SCRIPT_DIR}/build"
COVERAGE_DIR="${SCRIPT_DIR}/coverage"

printf "\n%s\t%s" "Build directory:" "${BUILD_DIR}"
printf "\n%s\t%s" "Coverage directory:" "${COVERAGE_DIR}"

# Remove the build files (Optional) - `bash cov.sh rebuild`
if [[ $1 == "rebuild" ]]; then
    printf "\n\n%s\n" ">>> Removing previous build files..."
    rm -rf "${BUILD_DIR}"
    rm -rf "${COVERAGE_DIR}"
fi

if [[ ! -d ${COVERAGE_DIR} ]]; then
    mkdir -p "${COVERAGE_DIR}"
fi

printf "\n%s\n\n" "Building unit-tests..."

cd tests/unit-tests || {
    printf "\n%s\n%s\n\n" "tests/unit-tests/ directory not found." "Please call this script from project root."
    exit 1
}

# Build unit tests for all targets
cmake \
    -DBOLOS_SDK="${BOLOS_SDK}" \
    -Bbuild \
    -H. &&
    make -C build

printf "\n%s\n\n" "Running unit-tests..."

# Run the tests
CTEST_OUTPUT_ON_FAILURE=1 make -C build test

printf "\n%s\n\n" "Generating coverage report..."

# Create the coverage directory
mkdir -p "${COVERAGE_DIR}"

# Generate the initial coverage data
lcov --directory . \
    -b "${BUILD_DIR}" \
    -q \
    --capture \
    --initial \
    -o "${COVERAGE_DIR}"/coverage.base

# Capture the test coverage data
lcov --rc branch_coverage=1 \
    --directory . \
    -q \
    -b "${BUILD_DIR}" \
    --capture \
    -o "${COVERAGE_DIR}"/coverage.capture

# Combine the initial and test coverage data
lcov --directory . \
    -q \
    -b "${BUILD_DIR}" \
    --add-tracefile "${COVERAGE_DIR}"/coverage.base \
    --add-tracefile "${COVERAGE_DIR}"/coverage.capture \
    -o "${COVERAGE_DIR}"/coverage.info

# Remove coverage data for specified directories
lcov --directory . \
    -q \
    -b "${BUILD_DIR}" \
    --remove "${COVERAGE_DIR}"/coverage.info '*/unit-tests/*' '*/lib_standard_app/*' \
    -o "${COVERAGE_DIR}"/coverage.info

# Generate HTML report from the coverage data
genhtml "${COVERAGE_DIR}"/coverage.info -o coverage

# Clean up intermediate coverage files
rm -f "${COVERAGE_DIR}"/coverage.base coverage/coverage.capture

exit 0
