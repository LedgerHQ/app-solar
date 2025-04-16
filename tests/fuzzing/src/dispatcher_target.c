
#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

#include <io.h>
#include <parser.h>  // apdu_parser
#include <ux.h>

#include "apdu_offsets.h"
#include "app_types.h"
#include "context.h"
#include "apdu/apdu_constants.h"

/* -------------------------------------------------------------------------- */

static const size_t APDU_HEADER_LEN = 5u;

global_ctx_t G_context;
bolos_ux_params_t G_ux_params;

/* -------------------------------------------------------------------------- */

// External function to dispatch APDU commands
extern int apdu_dispatcher(const command_t *cmd);

/* -------------------------------------------------------------------------- */

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (data == NULL || size == 0u) {
        return 0;
    }

    if (size < APDU_HEADER_LEN) {
        return 0;
    }

    // Ensure buffer length can accommodate the data len specified by lc
    if ((size > OFFSET_LC) && (data[OFFSET_LC] > (size - OFFSET_CDATA))) {
        return 0;
    }

    command_t cmd;
    size_t offset = 0u;

    while ((offset + APDU_HEADER_LEN) <= size) {
        const size_t current_chunk_size = data[offset + OFFSET_LC];
        if (offset + APDU_HEADER_LEN + current_chunk_size > size) {
            return 0;
        }

        // Parse and dispatch the APDU chunk
        if (!apdu_parser(&cmd, (uint8_t *)&data[offset], APDU_HEADER_LEN + current_chunk_size)) {
            return 0;
        }

        if (apdu_dispatcher(&cmd) < 0) {
            return 0;
        }

        offset += APDU_HEADER_LEN + current_chunk_size;
    }

    return 0;
}
