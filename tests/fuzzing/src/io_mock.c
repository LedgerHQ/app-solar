
// #include <io.h>

#include <stdint.h>

#include "sw.h"

int io_send_sw(uint16_t sw) {
    return (sw == SW_OK) ? 0 : -1;
}
