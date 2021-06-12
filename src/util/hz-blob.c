#include "hz-blob.h"


struct hz_blob_t {
    hz_byte *data;
    size_t size;
};

hz_blob_t *hz_blob_create(void)
{
    hz_blob_t * blob = (hz_blob_t *) malloc(sizeof(hz_blob_t));
    blob->data = NULL;
    blob->size = 0;
    return blob;
}

void hz_blob_destroy(hz_blob_t *blob)
{
    if (blob->data != NULL) free(blob->data);
    free(blob);
}

void
hz_blob_resize(hz_blob_t *blob, size_t new_size)
{
    if (hz_blob_is_empty(blob)) {
        blob->data = malloc(new_size);
    } else {
        blob->data = realloc(blob->data, new_size);
    }

    blob->size = new_size;
}

size_t hz_blob_get_size(hz_blob_t *blob)
{
    return blob->size;
}

hz_byte *hz_blob_get_data(hz_blob_t *blob)
{
    return blob->data;
}


hz_bool hz_blob_is_empty(hz_blob_t *blob) {
    return blob->data == NULL || blob->size == 0;
}

hz_stream_t
hz_blob_to_stream(hz_blob_t *blob) {
    return hz_stream_create(blob->data, HZ_BSWAP);
}
