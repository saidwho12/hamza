#ifndef HZ_BUF_H
#define HZ_BUF_H

#include "../hz-base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hz_buf_t hz_buf_t;

hz_buf_t *hz_buf_create(void);

void hz_buf_destroy(hz_buf_t *buf);

void hz_buf_resize(hz_buf_t *buf, size_t new_size);

size_t hz_buf_size(hz_buf_t *buf);

hz_byte *hz_buf_data(hz_buf_t *buf);

hz_bool hz_buf_is_empty(hz_buf_t *buf);

hz_stream_t *
hz_buf_to_stream(hz_buf_t *buf);

#ifdef __cplusplus
};
#endif

#endif /* HZ_BUF_H */