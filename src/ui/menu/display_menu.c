/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#if defined(SCREEN_SIZE_WALLET)  // stax, flex

#include "ui/menu/display_menu.h"

#include <stddef.h>  // NULL
#include <stdint.h>  // uint*_t

#include <nbgl_content.h>   // nbgl_contentInfoList_t
#include <nbgl_use_case.h>  // nbgl_useCaseHomeAndSettings, INIT_HOME_PAGE

#include "glyphs.h"  // C_logo_solar_64px

#include "ui/action/quit.h"

/* -------------------------------------------------------------------------- */

static const char MENU_TAGLINE[] = "This app enables signing transactions on the Solar Network.";

static const char INFO_TYPE_VERSION[] = "Version";
static const char INFO_TYPE_DEVELOPER[] = "Developer";
static const char INFO_TYPE_COPYRIGHT[] = "Copyright";

static const char INFO_CONTENT_COPYRIGHT[] = "(c) Solar Network";

#define MENU_INFO_NB 3

/* -------------------------------------------------------------------------- */

static const char* const INFO_TYPES[MENU_INFO_NB] = {INFO_TYPE_VERSION,
                                                     INFO_TYPE_DEVELOPER,
                                                     INFO_TYPE_COPYRIGHT};
static const char* const INFO_CONTENTS[MENU_INFO_NB] = {APPVERSION,
                                                        APPNAME,
                                                        INFO_CONTENT_COPYRIGHT};

static const nbgl_contentInfoList_t MENU_INFO_LIST = {.infoTypes = INFO_TYPES,
                                                      .infoContents = INFO_CONTENTS,
                                                      .nbInfos = MENU_INFO_NB};

/* -------------------------------------------------------------------------- */

/**
 * @brief Displays the Solar app's main menu.
 */
void ui_menu_main(void) {
    nbgl_useCaseHomeAndSettings(APPNAME,
                                &C_logo_solar_64px,
                                MENU_TAGLINE,
                                (const uint8_t)INIT_HOME_PAGE,
                                NULL,
                                &MENU_INFO_LIST,
                                NULL,
                                quit_app);
}

#endif  // if defined(SCREEN_SIZE_WALLET)
