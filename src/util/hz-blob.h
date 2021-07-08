#ifndef HZ_BLOB_H
#define HZ_BLOB_H

#include "../hz-base.h"

#ifdef __cplusplus
extern "C" {
#endif


/*  Struct: hz_blob_t
 *      Blob of raw data.
 * */
typedef struct hz_blob_t hz_blob_t;

/*  Function: hz_blob_create
 *      Create empty blob.
 *
 *  Returns:
 *      The blob.
 * */
hz_blob_t *hz_blob_create(void);

/*  Function: hz_blob_destroy
 *      Destroys blob.
 *
 *  Parameters:
 *      blob - The blob.
 * */
void hz_blob_destroy(hz_blob_t *blob);

/*  Function: hz_blob_resize
 *      Resizes blob.
 *
 *  Parameters:
 *      blob - The blob.
 *      new_size - New size for the blob data.
 * */
void hz_blob_resize(hz_blob_t *blob, size_t new_size);

/*  Function: hz_blob_get_size
 *      Gets size of the blob.
 *
 *  Parameters:
 *      blob - The blob.
 *
 *  Returns:
 *      Size of the blob.
 * */
size_t hz_blob_get_size(hz_blob_t *blob);

/*  Function: hz_blob_get_data
 *      Gets data of the blob.
 *
 *  Parameters:
 *      blob - The blob.
 *
 *  Returns:
 *      Data of the blob.
 * */
hz_byte *hz_blob_get_data(hz_blob_t *blob);

/*  Function: hz_blob_is_empty
 *      Check if blob is empty.
 *
 *  Parameters:
 *      blob - The blob.
 *
 *  Returns:
 *      True if empty.
 * */
hz_bool_t hz_blob_is_empty(hz_blob_t *blob);

/*  Function: hz_blob_to_stream
 *      Get stream from blob.
 *
 *  Parameters:
 *      blob - The blob.
 *
 *  Returns:
 *      Byte stream to read into the blob.
 * */
hz_stream_t
hz_blob_to_stream(hz_blob_t *blob);

#ifdef __cplusplus
}
#endif

#endif /* HZ_BLOB_H */