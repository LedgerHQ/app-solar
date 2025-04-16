#!/usr/bin/env bash

################################################################################
#  Copyright (c) Solar Network <hello@solar.org>                               #
#                                                                              #
#  This work is licensed under a Creative Commons Attribution-NoDerivatives    #
#  4.0 International License.                                                  #
################################################################################

set -e

# Check if bear exists, install if not.
if ! command -v bear &>/dev/null; then
    printf "\n%s\n" "bear not found. Installing..."
    if apk add bear; then
        printf "\n%s\n" "bear installed successfully!"
    else
        printf "\n%s\n" ">>> Error: Failed to install bear"
        exit 1
    fi
else
    printf "\n%s\n" "bear is already installed"
fi

exit 0
