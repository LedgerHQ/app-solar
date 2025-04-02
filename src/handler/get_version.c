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

#include "handler/get_version.h"

#include <stdint.h>  // uint*_t

#include <io.h>  // io_send_response_pointer

#include "constants.h"
#include "sw.h"

/* -------------------------------------------------------------------------- */

/**
 * @brief Handler for GET_VERSION command.
 *
 * Sends an APDU response containing the app version.
 *
 * @see MAJOR_VERSION, MINOR_VERSION and PATCH_VERSION in Makefile.
 *
 * @return zero or positive integer if successful, otherwise a negative integer.
 */
int handler_get_version(void) {
    _Static_assert(APPVERSION_LEN == 3, "Length of (MAJOR || MINOR || PATCH) must be 3!");
    _Static_assert((MAJOR_VERSION >= 0) && (MAJOR_VERSION <= (int)UINT8_MAX),
                   "MAJOR version must be between 0 and 255!");
    _Static_assert((MINOR_VERSION >= 0) && (MINOR_VERSION <= (int)UINT8_MAX),
                   "MINOR version must be between 0 and 255!");
    _Static_assert((PATCH_VERSION >= 0) && (PATCH_VERSION <= (int)UINT8_MAX),
                   "PATCH version must be between 0 and 255!");

    return io_send_response_pointer(
        (const uint8_t *)&(uint8_t[APPVERSION_LEN]){(uint8_t)MAJOR_VERSION,
                                                    (uint8_t)MINOR_VERSION,
                                                    (uint8_t)PATCH_VERSION},
        APPVERSION_LEN,
        SW_OK);
}
