#ifndef HZ_BLOB_H
#define HZ_BLOB_H

#include "../hz-base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hz_blob_t hz_blob_t;

hz_blob_t *hz_blob_create(void);

void hz_blob_destroy(hz_blob_t *blob);

void hz_blob_resize(hz_blob_t *blob, size_t new_size);

size_t hz_blob_size(hz_blob_t *blob);

hz_byte *hz_blob_data(hz_blob_t *blob);

hz_bool hz_blob_is_empty(hz_blob_t *blob);

hz_stream_t *
hz_blob_to_stream(hz_blob_t *blob);

#ifdef __cplusplus
};
#endif

#endif /* HZ_BLOB_H */