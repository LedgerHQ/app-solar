#pragma once

#include <stdbool.h>

#include "transaction/types.h"

#include "ui/ui_constants.h"

bool ui_set_memo(const transaction_t *transaction,
                 char item[MAX_ITEM_LEN],
                 char value[MAX_VALUE_LEN]);
