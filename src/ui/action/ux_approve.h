#pragma once

#if defined(SCREEN_SIZE_NANO)  // nanosp, nanox

#include <ux.h>  // UX_STEP_CB

#include "glyphs.h"  // C_icon_validate_14, C_icon_crossmark

#include "ui/action/validate.h"

/* -------------------------------------------------------------------------- */

#define VALIDATION_APPROVE "Approve"
#define VALIDATION_REJECT  "Reject"

/* -------------------------------------------------------------------------- */

#define UX_APPROVE_STEP(step_name, response_type) \
    UX_STEP_CB(step_name,                         \
               pb,                                \
               ui_validate(response_type, true),  \
               {&C_icon_validate_14, VALIDATION_APPROVE})

#define UX_REJECT_STEP(step_name, response_type)  \
    UX_STEP_CB(step_name,                         \
               pb,                                \
               ui_validate(response_type, false), \
               {&C_icon_crossmark, VALIDATION_REJECT})

#endif  // if defined(SCREEN_SIZE_NANO)
