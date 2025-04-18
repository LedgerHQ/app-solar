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

#include <io.h>      // io_init, io_recv_command, io_send_sw
#include <os.h>      // app_main, G_io_apdu_buffer, PRINTF
#include <parser.h>  // command_t, apdu_parser

#include "context.h"
#include "globals.h"
#include "sw.h"

#include "apdu/dispatcher.h"

#include "ui/menu/display_menu.h"

/* -------------------------------------------------------------------------- */

global_ctx_t G_context;

/* -------------------------------------------------------------------------- */

/**
 * @brief Handle APDU command received and send back APDU response using handlers.
 */
void app_main(void) {
    // Structured APDU command
    command_t cmd;

    io_init();

    ui_menu_main();

    // Reset context
    reset_app_context();

    for (;;) {
        // Length of APDU command received in G_io_apdu_buffer
        int input_len = 0;

        // Receive command bytes in G_io_apdu_buffer
        input_len = io_recv_command();
        if (input_len < 0) {
            PRINTF("=> io_recv_command failure\n");
            return;
        }

        // Parse APDU command from G_io_apdu_buffer.
        if (!apdu_parser(&cmd, G_io_apdu_buffer, input_len)) {
            (void)io_send_sw(SW_WRONG_LENGTH);
            continue;
        }

        PRINTF(
            "=> CLA=%02X | INS=%02X | P1=%02X | P2=%02X | Lc=%02X | "
            "CData=%.*H\n",
            cmd.cla,
            cmd.ins,
            cmd.p1,
            cmd.p2,
            cmd.lc,
            cmd.lc,
            cmd.data);

        // Dispatch structured APDU command to handler.
        if (apdu_dispatcher(&cmd) < 0) {
            PRINTF("=> apdu_dispatcher failure\n");
            return;
        }
    }
}
