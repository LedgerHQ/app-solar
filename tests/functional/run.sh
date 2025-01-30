#!/usr/bin/env bash

################################################################################
#  Copyright (c) Solar Network <hello@solar.org>                               #
#                                                                              #
#  This work is licensed under a Creative Commons Attribution-NoDerivatives    #
#  4.0 International License.                                                  #
################################################################################

################################################################################
# Devices:
#   - flex
# 	- nanosp
# 	- nanox
# 	- stax
#
# Usage:
#   - bash tests/functional/run.sh
#        -> Build and test for all devices
#   - bash tests/functional/run.sh stax
#        -> Build and test for 'stax'
#   - bash tests/functional/run.sh --golden_run
#        -> Build for all devices while producing snapshots
#   - bash tests/functional/run.sh nanox --golden_run
#        -> Build for 'nanox' while producing snapshots
################################################################################

set -e

printf "\n%s\n%s\n%s\n" \
    "==================================================" \
    "            Solar App Tests: Functional           " \
    "=================================================="

# Optional: flag to generate snapshots (--golden_run)
GOLDEN_RUN=""
# Optional: Specified device, or all supported devices when empty
SELECTED_DEVICE=""

# Parse arguments
for arg in "$@"; do
    case "$arg" in
    --golden_run)
        GOLDEN_RUN="--golden_run"
        ;;
    *)
        SELECTED_DEVICE="$arg"
        ;;
    esac
done

# Map device names to paths
declare -A BOLOS_SDK_MAP=(
    ["flex"]="/opt/flex-secure-sdk"
    ["nanosp"]="/opt/nanosplus-secure-sdk"
    ["nanox"]="/opt/nanox-secure-sdk"
    ["stax"]="/opt/stax-secure-sdk"
)

# Decide which devices to build
if [[ -n "$SELECTED_DEVICE" ]]; then
    # Validate the device
    if [[ -z "${BOLOS_SDK_MAP[$SELECTED_DEVICE]:-}" ]]; then
        printf "\n%s %s" ">>> Error: Unsupported device:" "${SELECTED_DEVICE}"
        printf "\n\n%s" "Supported devices are:"
        printf "\n - %s" "${!BOLOS_SDK_MAP[@]}"
        printf "\n\n"
        exit 1
    fi
    DEVICES=("$SELECTED_DEVICE")
    TEST_TARGET="$SELECTED_DEVICE"
else
    DEVICES=("${!BOLOS_SDK_MAP[@]}")
    TEST_TARGET="all"
fi

printf "\n%s\n\n" "Installing requirements..."

pip3 install -r tests/functional/requirements.txt >/dev/null

printf "\n%s\n" "Building App(s)..."

make clean >/dev/null 2>&1

for device in "${DEVICES[@]}"; do
    sdk_path="${BOLOS_SDK_MAP[$device]}"

    if [[ ! -d "$sdk_path" ]]; then
        printf "\n%s\n\n" ">>> Error: BOLOS_SDK path not found: $sdk_path"
        exit 1
    fi

    printf "\n%s\n" "BOLOS_SDK: $sdk_path"
    export BOLOS_SDK="$sdk_path"

    make -j4 >/dev/null
done

printf "\n%s\n\n" "Running functional tests..."

pytest tests/functional --tb=short --device "$TEST_TARGET" $GOLDEN_RUN
