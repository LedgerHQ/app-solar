#!/usr/bin/env bash

################################################################################
#  Copyright (c) Solar Network <hello@solar.org>                               #
#                                                                              #
#  This work is licensed under a Creative Commons Attribution-NoDerivatives    #
#  4.0 International License.                                                  #
################################################################################

set -e

if command -v include-what-you-use &>/dev/null; then
    printf "\n%s\n" "IWYU is already installed"
    exit 0
fi

# Detect OS
if [ -f /etc/os-release ]; then
    # shellcheck disable=SC1091
    . /etc/os-release

    case "$ID" in
    "alpine")
        apk add --no-cache include-what-you-use
        ;;

    "ubuntu")
        apt-get update
        apt-get install -y include-what-you-use
        ;;

    *)
        printf "\n%s\n" ">>> Error: Unsupported operating system: $ID"
        exit 1
        ;;
    esac
else
    printf "\n%s\n" ">>> Error: Cannot detect operating system"
    exit 1
fi

# Verify installation
if command -v include-what-you-use &>/dev/null; then
    printf "\n%s\n" "IWYU installation completed"
else
    printf "\n%s\n" ">>> Error: IWYU installation failed"
    exit 1
fi

exit 0
