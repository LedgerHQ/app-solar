#!/usr/bin/env bash

################################################################################
#  Copyright (c) Solar Network <hello@solar.org>                               #
#                                                                              #
#  This work is licensed under a Creative Commons Attribution-NoDerivatives    #
#  4.0 International License.                                                  #
################################################################################

# Check if cppcheck exists, install if not.
if ! command -v cppcheck &>/dev/null; then
    printf "\n%s\n" "cppcheck not found. Installing..."

    # Install dependencies and build
    apk add --no-cache \
        g++ \
        make \
        cmake \
        pcre \
        pcre-dev

    rm -rf deps
    mkdir -p deps
    git clone --depth 1 -b 2.16.0 https://github.com/danmar/cppcheck.git deps/cppcheck
    cd deps/cppcheck || {
        printf "\n%s\n" ">>> Error: 'deps/cppcheck' does not exist. Exiting."
        exit 1
    }
    mkdir build
    cd build || {
        printf "\n%s\n" ">>> Error: 'deps/cppcheck/build' does not exist. Exiting."
        exit 1
    }
    cmake .. -DHAVE_RULES=ON
    make -j"$(nproc)"
    make install

    # Link cppcheck-htmlreport
    pip3 install pygments
    ln -sf "/app/deps/cppcheck/htmlreport/cppcheck-htmlreport" /bin/cppcheck-htmlreport

    if command -v cppcheck-htmlreport &>/dev/null; then
        printf "\n%s\n" "cppcheck installed successfully"!
    else
        printf "\n%s\n" ">>> Error: Failed to install cppcheck"
        exit 1
    fi
else
    printf "\n%s\n" "cppcheck is already installed"
fi

exit 0
