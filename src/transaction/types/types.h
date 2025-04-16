#pragma once

#include "transaction/types/ipfs.h"
#include "transaction/types/transfer.h"
#include "transaction/types/burn.h"
#include "transaction/types/vote.h"

typedef union {
    ipfs_asset_t Ipfs;
    transfer_asset_t Transfer;
    burn_asset_t Burn;
    vote_asset_t Vote;
} transaction_asset_t;
