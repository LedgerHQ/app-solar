/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#include "ui/action/quit.h"

#include <os.h>  // os_sched_exit

/**
 * @brief Exit the Solar app.
 */
void quit_app(void) {
    os_sched_exit(-1);
}
