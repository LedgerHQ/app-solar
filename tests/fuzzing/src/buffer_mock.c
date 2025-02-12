#include <buffer.h>

bool buffer_move(buffer_t *buffer, uint8_t *out, size_t out_len) {
    if (buffer->ptr == NULL || buffer->size == 0u || out == NULL || out_len < buffer->size) {
        return false;
    }

    // Catch the `0xdeadbeef` flag from the various seeds.
    if (buffer->size >= 4u) {
        const uint32_t value = ((uint32_t)buffer->ptr[0] << 24) | ((uint32_t)buffer->ptr[1] << 16) |
                               ((uint32_t)buffer->ptr[2] << 8) | (uint32_t)buffer->ptr[3];

        if (value == 0xDEADBEEF) {
            return false;
        }
    }

    if (!buffer_copy(buffer, out, out_len)) {
        return false;
    }

    buffer_seek_cur(buffer, out_len);

    return true;
}

bool buffer_read_u8(buffer_t *buffer, uint8_t *value) {
    if (buffer->ptr == NULL) {
        return false;
    }

    // Catch the `0xde(adbeef)` flag from the various seeds.
    if (buffer->ptr[0] == 0xDE) {
        return false;
    }

    if (!buffer_can_read(buffer, 1)) {
        *value = 0u;

        return false;
    }

    *value = buffer->ptr[buffer->offset];
    buffer_seek_cur(buffer, 1u);

    return true;
}
