#pragma once

#define MAX_VALUE_LEN ((size_t)256)

#if defined(SCREEN_SIZE_NANO)  // nanosp, nanox
#define MAX_ITEM_LEN ((size_t)16)
#elif defined(SCREEN_SIZE_WALLET)
#define MAX_ITEM_LEN ((size_t)32)
#endif

#if defined(TARGET_STAX) || defined(TARGET_FLEX)
#define ICON_APP_HOME C_logo_solar_64px
#elif defined(TARGET_APEX_P)
#define ICON_APP_HOME C_logo_solar_48px
#endif

