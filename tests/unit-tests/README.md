# Unit tests

## Prerequisite

Be sure to have installed:

- CMake >= 3.10
- CMocka >= 1.1.5

and for code coverage generation:

- lcov >= 1.14

## Overview

Launch the Ledger App Builder (Dev Tools) Docker image

```bash
sudo docker run  --rm -ti --user "$(id -u):$(id -g)" --privileged -v "$(pwd -P):/app" ghcr.io/ledgerhq/ledger-app-builder/ledger-app-dev-tools:latest
```

In `unit-tests` folder, compile with

```bash
cmake -DBOLOS_SDK=/opt/ledger-secure-sdk -Bbuild -H. && make -C build
```

and run tests with

```bash
CTEST_OUTPUT_ON_FAILURE=1 make -C build test
```

## One-shot coverage

Build, run and coverage can be executed in one shot using the following command (while in the `unit-tests` folder):

```bash
bash cov.sh clean
```

It will output the `coverage/` folder with HTML details (in `coverage/index.html`).
