/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#if defined(SCREEN_SIZE_NANO)  // nanosp, nanox

#include "ui/menu/display_menu.h"

#include <stddef.h>  // NULL

#include <ux.h>  // G_ux, UX_STEP_CB, UX_STEP_NOCB, UX_FLOW...

#include "glyphs.h"  // C_logo_solar_16px, C_icon_certificate

#include "ui/action/quit.h"

/* -------------------------------------------------------------------------- */

// App menu
static const char MENU_TITLE[] = "Solar";
static const char MENU_SUBTITLE[] = "is ready";
static const char MENU_INFO_SUBTITLE[] = "(c) Solar Network";

// App info
static const char MENU_VERSION[] = "Version";
static const char MENU_INFO[] = "Info";
static const char MENU_QUIT[] = "Quit";
static const char MENU_BACK[] = "Back";

/* -------------------------------------------------------------------------- */

UX_STEP_NOCB(ux_menu_ready_step, pnn, {&C_logo_solar_16px, MENU_TITLE, MENU_SUBTITLE});
UX_STEP_NOCB(ux_menu_version_step, bn, {MENU_VERSION, APPVERSION});
UX_STEP_CB(ux_menu_info_step, pb, ui_menu_info(), {&C_icon_certificate, MENU_INFO});
UX_STEP_CB(ux_menu_exit_step, pb, quit_app(), {&C_icon_dashboard_x, MENU_QUIT});

/* -------------------------------------------------------------------------- */

UX_FLOW(ux_menu_main_flow,
        &ux_menu_ready_step,
        &ux_menu_version_step,
        &ux_menu_info_step,
        &ux_menu_exit_step,
        FLOW_LOOP);

/* -------------------------------------------------------------------------- */

/**
 * @brief Displays the Solar app's main menu.
 */
void ui_menu_main(void) {
    if (G_ux.stack_count == 0u) {
        (void)ux_stack_push();
    }

    ux_flow_init(0u, ux_menu_main_flow, NULL);
}

/* -------------------------------------------------------------------------- */

UX_STEP_NOCB(ux_menu_info_menu_step, bn, {MENU_TITLE, MENU_INFO_SUBTITLE});
UX_STEP_CB(ux_menu_back_step, pb, ui_menu_main(), {&C_icon_back, MENU_BACK});

/* -------------------------------------------------------------------------- */

UX_FLOW(ux_menu_info_flow, &ux_menu_info_menu_step, &ux_menu_back_step, FLOW_LOOP);

/* -------------------------------------------------------------------------- */

/**
 * @brief Displays the Solar app's info submenu (copyright, date).
 */
void ui_menu_info(void) {
    ux_flow_init(0u, ux_menu_info_flow, NULL);
}

#endif  // if defined(SCREEN_SIZE_NANO)
