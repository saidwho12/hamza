#include "hz-buf.h"

struct hz_buf_t {
    hz_byte *data;
    size_t size;
};

hz_buf_t *hz_buf_create(void)
{
    hz_buf_t * buf = (hz_buf_t *) malloc(sizeof(hz_buf_t));
    buf->data = NULL;
    buf->size = 0;
    return buf;
}

void hz_buf_destroy(hz_buf_t *buf)
{
    if (buf->data != NULL) free(buf->data);
    free(buf);
}

void
hz_buf_resize(hz_buf_t *buf, size_t new_size)
{
    if (hz_buf_is_empty(buf)) {
        buf->data = malloc(new_size);
    } else {
        buf->data = realloc(buf->data, new_size);
    }

    buf->size = new_size;
}

size_t hz_buf_size(hz_buf_t *buf)
{
    return buf->size;
}

hz_byte *hz_buf_data(hz_buf_t *buf)
{
    return buf->data;
}


hz_bool hz_buf_is_empty(hz_buf_t *buf) {
    return buf->data == NULL || buf->size == 0;
}

hz_stream_t *
hz_buf_to_stream(hz_buf_t *buf) {
    return hz_stream_create(buf->data, buf->size, HZ_STREAM_BOUND_FLAG);
}