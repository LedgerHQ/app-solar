#!/usr/bin/env bash

################################################################################
#  Copyright (c) Solar Network <hello@solar.org>                               #
#                                                                              #
#  This work is licensed under a Creative Commons Attribution-NoDerivatives    #
#  4.0 International License.                                                  #
################################################################################

################################################################################
# Run cppcheck.
#
# Uses:
#   - sudo docker run --rm -ti --privileged -v "$(pwd -P):/app" ghcr.io/ledgerhq/ledger-app-builder/ledger-app-dev-tools:latest
#
# Devices:
#   - flex
# 	- nanosp
# 	- nanox
# 	- stax
#
# Usage:
#   - bash tools/cppcheck/cppcheck.sh stax
#        -> Run cppcheck for a specific device
################################################################################

if [[ $# -eq 0 ]]; then
    printf "\n%s" ">>> Error: You must pass a device: flex, nanosp, nanox, or stax"
    printf "\n%s\n\n" "(e.g., 'bash tools/cppcheck/cppcheck.sh stax')"
    exit 1
fi

# Required
DEVICE_TARGET=""

# Parse device argument
for arg in "$@"; do
    DEVICE_TARGET="$arg"
done

# Map device names to paths
declare -A BOLOS_SDK_MAP=(
    ["flex"]="/opt/flex-secure-sdk"
    ["nanosp"]="/opt/nanosplus-secure-sdk"
    ["nanox"]="/opt/nanox-secure-sdk"
    ["stax"]="/opt/stax-secure-sdk"
)

# Decide which devices to build
if [[ -n "$DEVICE_TARGET" ]]; then
    # Validate the device
    if [[ -z "${BOLOS_SDK_MAP[$DEVICE_TARGET]:-}" ]]; then
        printf "\n%s\n" ">>> Error: Unsupported device: ${DEVICE_TARGET}"
        printf "\n%s" "Supported devices are:"
        printf "\n - %s" "${!BOLOS_SDK_MAP[@]}"
        printf "\n\n"
        exit 1
    fi
fi

export BOLOS_SDK="${BOLOS_SDK_MAP[$DEVICE_TARGET]}"
printf "\n%s\n" "BOLOS_SDK: ${BOLOS_SDK}"

# Check if cppcheck exists, install if not.
bash scripts/install_cppcheck.sh || { exit 1; }

rm -rf \
    compile_commands.json \
    cppcheck \
    cppcheck_report \
    cppcheck_output.xml

mkdir -p cppcheck

bash scripts/build_bear.sh "${DEVICE_TARGET}" || { exit 1; }

cppcheck \
    --cppcheck-build-dir=cppcheck \
    --project=compile_commands.json \
    --platform=tools/cppcheck/ledger-platform.xml \
    --library=tools/cppcheck/ledger-sdk.xml \
    --output-file=cppcheck_output.xml \
    --xml-version=2 \
    --addon=tools/cppcheck/misra.json \
    --suppressions-list=tools/cppcheck/misra-suppressions.txt \
    --enable=all \
    --check-level=exhaustive \
    --std=c11 \
    --suppress=*:"${BOLOS_SDK}"* \
    --suppress=*:build \
    --check-library \
    --suppress=missingIncludeSystem

cppcheck-htmlreport \
    --title=app-solar \
    --file=cppcheck_output.xml \
    --report-dir=cppcheck_report

exit 0
