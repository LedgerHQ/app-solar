#pragma once

#include <stdint.h>  // uint*_t

extern const uint8_t ASSET_MODAL_THRESHOLD;   // 2u
extern const uint8_t FIELD_OFFSET;            // 1u
extern const char TEXT_TRUNCATION_SUFFIX[4];  // "..."

#if defined(TARGET_FLEX)
#define MEMO_TRUNCACTION_THRESHOLD ((uint8_t)18)
#elif defined(TARGET_STAX)
#define MEMO_TRUNCACTION_THRESHOLD ((uint8_t)36)
#endif
