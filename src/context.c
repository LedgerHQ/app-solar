/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 ******************************************************************************/

#include "context.h"

#include <os.h>  // explicit_bzero

#include "app_types.h"
#include "globals.h"

/**
 * @brief Clear the app's contextual storage and set its global state to 'STATE_NONE'.
 */
void reset_app_context(void) {
    explicit_bzero(&G_context, sizeof(G_context));
    G_context.state = STATE_NONE;
    G_context.req_num = 0u;
}
