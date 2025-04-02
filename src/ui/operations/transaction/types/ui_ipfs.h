#pragma once

#include <stdbool.h>
#include <stdint.h>  // uint*_t

#include "transaction/types.h"

#include "ui/ui_constants.h"

/* -------------------------------------------------------------------------- */

extern const uint16_t IPFS_RECORD_COUNT;  // 1u

#if defined(SCREEN_SIZE_NANO)     // nanosp, nanox
extern const char IPFS_LABEL[5];  // "IPFS"
#endif

#if defined(SCREEN_SIZE_WALLET)  // stax, flex
#define IPFS_INTENT "register an IPFS hash"
#endif

/* -------------------------------------------------------------------------- */

bool get_ipfs_record(const transaction_t *transaction,
                     char label[MAX_ITEM_LEN],
                     char value[MAX_VALUE_LEN],
                     uint16_t index);
