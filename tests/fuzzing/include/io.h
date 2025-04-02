#pragma once

#include <stddef.h>  // size_t
#include <stdint.h>  // uint^_t

typedef enum { READY, RECEIVED, WAITING } io_state_e;

int io_send_sw(uint16_t sw);

static inline int io_send_response_pointer(__attribute__((unused)) const uint8_t *ptr,
                                           __attribute__((unused)) size_t size,
                                           __attribute__((unused)) uint16_t sw) {
    return -1;
}
