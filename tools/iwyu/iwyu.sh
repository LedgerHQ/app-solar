#!/usr/bin/env bash

################################################################################
#  Copyright (c) Solar Network <hello@solar.org>                               #
#                                                                              #
#  This work is licensed under a Creative Commons Attribution-NoDerivatives    #
#  4.0 International License.                                                  #
################################################################################

################################################################################
# Supported Systems:
#   - Alpine Linux (apk)
#   - Ubuntu (apt-get)
#
# Prerequisites:
#   - Python3
#
# Usage:
#   - bash tools/iwyu/iwyu.sh
#        -> Build and check for all devices
#   - bash tests/functional/run.sh stax
#        -> Build and check for a specific device
################################################################################

set -e

printf "\n%s\n%s\n%s\n\n" \
    "==================================================" \
    "      Solar App Tools: Include What You Use       " \
    "=================================================="

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
        printf "\n%s\n" "Error: Unsupported device: ${DEVICE_TARGET}"
        printf "\n%s" "Supported devices are:"
        printf "\n - %s" "${!BOLOS_SDK_MAP[@]}"
        printf "\n\n"
        exit 1
    fi
fi

bash scripts/install_iwyu.sh || { exit 1; }

bash scripts/build_bear.sh "${DEVICE_TARGET}" || { exit 1; }

printf "\n%s\n\n" "Checking Solar app with 'include-what-you-use..."

python3 tools/iwyu/iwyu_tool.py -p . src

exit 0
