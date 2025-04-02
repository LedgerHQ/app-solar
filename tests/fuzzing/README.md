
# Fuzzing

## Prerequisite

Be sure to have installed:

- CMake >= 3.10
- CMocka >= 1.1.5
- LLVM >= 12.0.0

and for code coverage generation:

- llvm-cov >= 12.0.0 (included in llvm)

## Overview

Launch the Ledger App Builder Legacy Docker image

```bash
sudo docker run --rm -ti --user "$(id -u):$(id -g)" -v "$(realpath .):/app" ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder-legacy:latest
```

In `fuzzing` folder, compile with

```bash
cmake \
    -DBOLOS_SDK="/opt/ledger-secure-sdk" \
    -DCMAKE_C_COMPILER=clang \
    -DCODE_COVERAGE=1 \
    -Bbuild \
    -H.
```

Then, build using

```bash
cmake --build build
```

and run tests with

```bash
build/src/dispatcher_fuzzer corpus/dispatcher_fuzzer seeds/dispatcher_fuzzer \
    -dict=seeds/fuzzing.dict \
    -reload=1 \
    -runs=1 \
    -max_len=1800 \
    -max_total_time=20
```

## One-shot coverage

Build, run and coverage can be executed in one shot using the following command (while in the `fuzzing` folder):

```bash
bash cov.sh
```

it will output the `coverage/` folder with HTML details (in `coverage/index.html`).
