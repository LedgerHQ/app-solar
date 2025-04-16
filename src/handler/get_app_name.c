/*******************************************************************************
 *  Copyright (c) Solar Network [hello@solar.org]
 *
 *  This work is licensed under a Creative Commons Attribution-NoDerivatives
 *  4.0 International License.
 *
 *  This software also incorporates work covered by the following copyright
 *  and permission notice:
 *
 *   Ledger App Boilerplate.
 *   (c) 2020 Ledger SAS.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/

#include "handler/get_app_name.h"

#include <io.h>      // io_send_response_pointer
#include <os_pic.h>  // PIC

#include "constants.h"
#include "sw.h"

/* -------------------------------------------------------------------------- */

/**
 * @brief Handler for GET_APP_NAME command.
 *
 * Sends an APDU response containing the ASCII text name of the app.
 *
 * @see variable APPNAME in Makefile.
 *
 * @return zero or positive integer if successful, otherwise a negative integer.
 */
int handler_get_app_name(void) {
    _Static_assert(APPNAME_LEN < APPNAME_MAX_LEN, "APPNAME must be at most 64 characters!");

    return io_send_response_pointer(PIC(APPNAME), APPNAME_LEN, SW_OK);
}
