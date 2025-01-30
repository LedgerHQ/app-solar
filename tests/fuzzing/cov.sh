#!/usr/bin/env bash

################################################################################
#  Copyright (c) Solar Network <hello@solar.org>                               #
#                                                                              #
#  This work is licensed under a Creative Commons Attribution-NoDerivatives    #
#  4.0 International License.                                                  #
################################################################################

################################################################################
# Run fuzzer with coverage (uses Nano X as target device).
#
# Uses:
#   - sudo docker run --rm -ti --user "$(id -u):$(id -g)" -v "$(realpath .):/app" ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder-legacy:latest
#
# Usage:
#   - bash tests/fuzzing/cov.sh
#        -> Run fuzzer (supports subsequent runs)
#   - bash tests/fuzzing/cov.sh clean
#        -> Run fuzzer after deleting all run artifacts
################################################################################

set -e

printf "\n%s\n%s\n%s\n" \
    "==================================================" \
    "              Solar App Tests: Fuzzing            " \
    "=================================================="

# Test parameters
FUZZING_COMPILER=clang
FUZZING_DURATION=60
FUZZING_TIMEOUT=10

printf "\n%s\t%s" "Compiler:" "${FUZZING_COMPILER}"
printf "\n%s\t%s" "Test duration:" "${FUZZING_DURATION}"
printf "\n%s\t%s" "Test timeout:" "${FUZZING_TIMEOUT}"

# Set above Solar app's max tx length of `4096` to ensure better coverage.
MAX_TRANSACTION_LEN=4300

printf "\n\n%s\t%s\n" "Max transaction length:" "${MAX_TRANSACTION_LEN}"

# Fuzzing targets
TARGET_BINARIES=(
    "dispatcher_fuzzer"
)

printf "\n%s" "Fuzzing Targets:"
for target in "${TARGET_BINARIES[@]}"; do
    printf "\n%s" "• ${target}"
done

printf "\n\n%s\n" "Configuring project..."

# Directories
SCRIPT_DIR="$(cd -- "$(dirname "$0")" >/dev/null 2>&1 && pwd -P)"
BUILD_DIR="${SCRIPT_DIR}/build"
BINARY_DIR="${SCRIPT_DIR}/build/src"
CORPUS_DIR="${SCRIPT_DIR}/corpus"
COVERAGE_DIR="${SCRIPT_DIR}/coverage"
SEEDS_DIR="${SCRIPT_DIR}/seeds"
PROFDATA_DIR="${SCRIPT_DIR}/profdata"

printf "\n%s\t%s" "Build directory:" "${BUILD_DIR}"
printf "\n%s\t%s" "Binary directory:" "${BINARY_DIR}"
printf "\n%s\t%s" "Corpus directory:" "${CORPUS_DIR}"
printf "\n%s\t%s" "Profdata directory:" "${PROFDATA_DIR}"
printf "\n%s\t%s" "Seeds directory:" "${SEEDS_DIR}"

# Files
DICT_FILE="${SEEDS_DIR}/fuzzing.dict"
TEMP_DICT_FILE="${SEEDS_DIR}/dict.tmp"

printf "\n\n%s\t%s" "Fuzzing dictionary:" "${DICT_FILE}"
printf "\n%s\t%s" "Temp dictionary:" "${TEMP_DICT_FILE}"

MERGE_CONTROL_FILE="${CORPUS_DIR}/merge.ctrl"

printf "\n\n%s\t%s\n" "Merge control file:" "${MERGE_CONTROL_FILE}"

# Remove the build files (Optional) - `bash cov.sh clean`
if [[ $1 == "clean" ]]; then
    printf "\n%s\n" "Removing previous build files..."
    rm -rf "${BUILD_DIR}"
    rm -rf "${CORPUS_DIR}"
    rm -rf "${COVERAGE_DIR}"
    rm -rf "${MERGE_CONTROL_FILE}"
    rm -rf "${PROFDATA_DIR}"
fi

if [[ ! -d ${COVERAGE_DIR} ]]; then
    mkdir -p "${COVERAGE_DIR}"
fi

if [[ ! -d ${PROFDATA_DIR} ]]; then
    mkdir -p "${PROFDATA_DIR}"
fi

if [[ ! -d ${SEEDS_DIR} ]]; then
    printf "\n%s\n" ">>> Error: Seeds directory not found."
    exit
fi

for target in "${TARGET_BINARIES[@]}"; do
    if [[ ! -d "${CORPUS_DIR}/${target}" ]]; then
        mkdir -p "${CORPUS_DIR}/${target}"
    fi
done

# Set the dictionary file
DICT_OPTION=""
if [[ -f "${DICT_FILE}" ]]; then
    DICT_OPTION="-dict=${DICT_FILE}"
else
    printf "\n%s\n" ">>> Error: Dictionary file not found."
    exit 1
fi

printf "\n%s\n\n" "Building fuzzing tests..."

cd tests/fuzzing || {
    printf "\n%s\n%s\n" ">>> Error: tests/fuzzing/ directory not found." "Please call this script from project root."
    exit 1
}

# Configure the fuzzing project
cmake \
    -DBOLOS_SDK="${BOLOS_SDK}" \
    -DCMAKE_C_COMPILER="${FUZZING_COMPILER}" \
    -DCODE_COVERAGE=1 \
    -B"${BUILD_DIR}" \
    -H.

cmake --build "${BUILD_DIR}"

printf "\n%s\n\n" "Running fuzzers..."

# Execute each fuzzing target binary
for target in "${TARGET_BINARIES[@]}"; do
    # Set the profiling data file
    export LLVM_PROFILE_FILE="${PROFDATA_DIR}/${target}_%p.profraw"

    # Run the target binary and save recommended dictionary entries to file
    "${BINARY_DIR}/${target}" "${CORPUS_DIR}/${target}" "${SEEDS_DIR}/${target}" \
        "${DICT_OPTION}" \
        -reload=1 \
        -runs=1 \
        -artifact_prefix="${CORPUS_DIR}/${target}/artifacts" \
        -max_len="${MAX_TRANSACTION_LEN}" \
        -max_total_time="${FUZZING_DURATION}" \
        -timeout="${FUZZING_TIMEOUT}" 2>&1 | tee >(awk \
            '/^###### Recommended dictionary. ######$/,/^###### End of recommended dictionary. ######$/ {
            if (!/^######.*######$/) print
    }' >>"${TEMP_DICT_FILE}")

    # Using the `-runs=1` flag enables a single run using only the predefined seeds. `FUZZING_DURATION` is ignored in this case.
    # Removing this flag will enable the fuzzer to search for more paths. Be sure to set the `FUZZING_DURATION` flag appropriately.
    # -runs=1 \

    # Running the fuzzer typically results in the creation of dictionary entries (or "interesting inputs").
    # These entries are written directly into the `TEMP_DICT_FILE` in a format not supported by llvm.
    # Format and append recommended dictionary entries to the main dictionary using the following:
    # { printf '\n'; sed -E 's/^(.*)(# Uses: .*)$/\2\n\1\n/' "${TEMP_DICT_FILE}"; } >> "${DICT_FILE}" && > "${TEMP_DICT_FILE}"
    # { printf '\n'; sed -E 's/^(.*)(# Uses: .*)$/\2\n\1\n/' seeds/dict.tmp; } >> seeds/fuzzing.dict && > seeds/dict.tmp

    # Merge and minimize the corpora
    "${BINARY_DIR}/${target}" "${CORPUS_DIR}/${target}" "${SEEDS_DIR}/${target}" \
        -merge=1 \
        -merge_control_file="${MERGE_CONTROL_FILE}" \
        -reduce_inputs=1 \
        -max_len="${MAX_TRANSACTION_LEN}" \
        -timeout=5
done

# Merge profdata
llvm-profdata merge \
    --sparse "${PROFDATA_DIR}"/*.profraw \
    -o "${PROFDATA_DIR}/combined.profdata" ||
    {
        printf "\n%s\n" "Warning: Failed to merge profiling data."
    }

# Remove old data after merging
rm -f "${PROFDATA_DIR}"/*.profraw

printf "\n%s\n\n" "Generating coverage report..."

# Prepare an object list of fuzzing target binaries
TARGET_BINARY_COVERAGE_OBJECTS=()
for target in "${TARGET_BINARIES[@]}"; do
    TARGET_BINARY_COVERAGE_OBJECTS+=("--object" "${BINARY_DIR}/${target}")
done

# Prepare coverage result data
llvm-cov show \
    "${TARGET_BINARY_COVERAGE_OBJECTS[@]}" \
    -instr-profile="${PROFDATA_DIR}/combined.profdata" \
    -format=html \
    -output-dir="${COVERAGE_DIR}" \
    --ignore-filename-regex="opt/|tests/|utils|mock|context\.c$|\.h$" ||
    {
        printf "\n%s\n" "Warning: Failed to prepare coverage data."
        exit 1
    }

# Export lcov data
llvm-cov export \
    "${TARGET_BINARY_COVERAGE_OBJECTS[@]}" \
    -instr-profile="${PROFDATA_DIR}/combined.profdata" \
    -format=lcov \
    --ignore-filename-regex="opt/|tests/|utils|mock|context\.c$|\.h$" \
    >"${COVERAGE_DIR}/lcov.info" ||
    {
        printf "\n%s\n" "Warning: Failed to export lcov data."
        exit 1
    }

# Prepare coverage report
llvm-cov report \
    "${TARGET_BINARY_COVERAGE_OBJECTS[@]}" \
    -instr-profile="${PROFDATA_DIR}/combined.profdata" \
    --ignore-filename-regex="opt/|tests/|utils|mock|context\.c$|\.h$" ||
    {
        printf "\n%s\n" "Warning: Failed to generate coverage report."
        exit 1
    }

exit 0
