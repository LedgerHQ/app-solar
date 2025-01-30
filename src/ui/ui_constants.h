#pragma once

#define MAX_VALUE_LEN ((size_t)256)

#if defined(SCREEN_SIZE_NANO)  // nanosp, nanox
#define MAX_ITEM_LEN ((size_t)16)
#elif defined(SCREEN_SIZE_WALLET)  // stax, flex
#define MAX_ITEM_LEN ((size_t)32)
#endif
