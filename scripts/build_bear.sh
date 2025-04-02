#!/usr/bin/env bash

################################################################################
#  Copyright (c) Solar Network <hello@solar.org>                               #
#                                                                              #
#  This work is licensed under a Creative Commons Attribution-NoDerivatives    #
#  4.0 International License.                                                  #
################################################################################

################################################################################
# Using 'bear' to build the Solar app ensures that 'compile_commands.json' is
# produced as a build artifact. This artifact is used by various tools including
# 'cppcheck' and 'include-what-you-use'
#
# Devices:
#   - flex
# 	- nanosp
# 	- nanox
# 	- stax
#
# Usage:
#   - bash scripts/build.sh
#        -> Builds all devices using bear
#   - bash scripts/build.sh stax
#        -> Builds only the 'stax' device using bear
################################################################################

set -e

# Optional: Specified device, or all supported devices when empty
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
    TARGET_DEVICE=("$DEVICE_TARGET")
else
    TARGET_DEVICE=("${!BOLOS_SDK_MAP[@]}")
fi

bash scripts/install_bear.sh

printf "\n%s\n" "Building app(s)..."

make clean >/dev/null 2>&1

for device in "${TARGET_DEVICE[@]}"; do
    sdk_path="${BOLOS_SDK_MAP[$device]}"

    if [[ ! -d "$sdk_path" ]]; then
        printf "\n%s\n\n" ">>> Error: BOLOS_SDK path not found: $sdk_path"
        exit 1
    fi

    printf "\n%s\n" "BOLOS_SDK: $sdk_path"
    export BOLOS_SDK="$sdk_path"

    printf "\n"

    bear -- make -j4 || {
        printf "\n%s\n" ">>> Error: Solar app bear build failed. Exiting."
        exit 1
    }

done

printf "\n%s\n\n" "Finished bear building Solar app(s)."

exit 0
