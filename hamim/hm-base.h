#ifndef HM_BASE_H
#define HM_BASE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <wchar.h>
#define HM_LOG(...) fprintf(stdout, __VA_ARGS__)
#define HM_DBGLOG(...) fprintf(stdout, __VA_ARGS__)
#define HM_ERROR(...) fprintf(stderr, __VA_ARGS__)
#define HM_MALLOC(size) malloc(size)
#define HM_FREE(p) free(p)
#define HM_REALLOC(p, size) realloc(p, size)


#define HM_PACKED __attribute__((__packed__))

/*
 * Define base sized integer and float types
 * */
typedef unsigned char hm_uint8;
typedef unsigned short hm_uint16;
typedef unsigned long hm_uint32;
typedef unsigned long long hm_uint64;
typedef char hm_int8;
typedef short hm_int16;
typedef long hm_int32;
typedef long long hm_int64;
typedef float hm_float32;
typedef double hm_float64;

typedef uint8_t hm_byte, hm_bool, hm_char;
typedef uint16_t hm_offset16;
typedef uint32_t hm_tag;
typedef uint32_t hm_offset32;
typedef uint32_t hm_unicode; /* 32 bit unicode type */
typedef uint16_t hm_id;
typedef uint16_t hm_fixed;

#define HM_F2DOT14 uint16_t
#define HM_FWORD uint16_t
#define HM_UFWORD uint16_t
typedef uint32_t HM_Version16Dot16;

typedef uint64_t hm_uintptr;
typedef size_t hm_size;

static hm_float32
hm_cast_f32_f2d14(const HM_F2DOT14 x)
{
    hm_float32 dec = (x & 0x3FFFU) / (hm_float32) 0x3FFFU;
    hm_float32 unit = (x & 0xC000U) >> 14U;
    return unit + dec;
}

static uint16_t bswap16(uint16_t val)
{
    uint16_t res = 0;
    res |= (val & 0x00FFU) << 8;
    res |= (val & 0xFF00U) >> 8;
    return res;
}

static uint32_t bswap32(uint32_t val)
{
    uint32_t res = 0;
    res |= (val & 0x0000FFFFU) << 16;
    res |= (val & 0xFFFF0000U) >> 16;
    return res;
}

#define HM_TRUE 1
#define HM_FALSE 0

#define HM_ASSERT(cond) assert(cond)
#define HM_TAG(c1, c2, c3, c4) ((hm_tag)c4 | ((hm_tag)c3 << 8U) | ((hm_tag)c2 << 16U) | ((hm_tag)c1 << 24U))
#define HM_UNTAG(tag) (tag >> 24) & 0xFF, (tag >> 16) & 0xFF, (tag >> 8) & 0xFF, tag & 0xFF
#define HM_ALLOC(T) (T *) HM_MALLOC(sizeof(T))


typedef struct hm_buf_t {
    uint8_t *data;
    size_t len;
} hm_buf_t;

typedef struct hm_face_t {
    hm_byte *base_table;
    hm_byte *gsub_table;
    hm_byte *gpos_table;
    hm_byte *gdef_table;
    hm_byte *jstf_table;
    hm_buf_t cmap_buf;
    void *handle; /* freetype handle */
} hm_face_t;

typedef enum hm_status_t {
    HM_SUCCESS = 0,
    HM_FAILURE = 1
} hm_status_t;

#define HM_STREAM_BOUND_FLAG 0x01
#define HM_STREAM_FLIP_ENDIANNESS_FLAG 0x02

#define HM_STREAM_OVERFLOW 0

typedef struct hm_stream_t {
    uint8_t *data;
    size_t length;
    size_t offset;
    uint8_t flags;
} hm_stream_t;

static hm_stream_t *
hm_stream_create(uint8_t *data, size_t length, uint8_t flags)
{
    hm_stream_t *stream = (hm_stream_t *) HM_MALLOC(sizeof(hm_stream_t));
    stream->offset = 0;
    stream->data = data;
    stream->length = length;
    stream->flags = flags;
    return stream;
}

static unsigned int
hm_stream_read8(hm_stream_t *stream, uint8_t *val) {
    const uint8_t *valptr = stream->data + stream->offset;

    if (stream->flags & HM_STREAM_BOUND_FLAG && stream->offset + 1 >= stream->length)
        return HM_STREAM_OVERFLOW;

    (*val) = valptr[0];
    stream->offset += 1;
    return 1;
}

static unsigned int
hm_stream_read16(hm_stream_t *stream, uint16_t *val) {
    uint16_t tmpval = 0;
    const uint8_t *valptr = stream->data + stream->offset;

    if (stream->flags & HM_STREAM_BOUND_FLAG && stream->offset + 2 >= stream->length)
        return HM_STREAM_OVERFLOW;

    tmpval |= (uint16_t) valptr[0] << 8;
    tmpval |= valptr[1];

    if (stream->flags & HM_STREAM_FLIP_ENDIANNESS_FLAG)
        tmpval = bswap16( tmpval );

    *val = tmpval;
    stream->offset += 2;
    return 2;
}

static unsigned int
hm_stream_read32(hm_stream_t *stream, uint32_t *val) {
    uint32_t tmpval = 0;
    const uint8_t *valptr = stream->data + stream->offset;

    if (stream->flags & HM_STREAM_BOUND_FLAG && stream->offset + 4 >= stream->length)
        return HM_STREAM_OVERFLOW;

    tmpval |= (uint32_t) valptr[0] << 24;
    tmpval |= (uint32_t) valptr[1] << 16;
    tmpval |= (uint32_t) valptr[2] << 8;
    tmpval |= valptr[3];

    if (stream->flags & HM_STREAM_FLIP_ENDIANNESS_FLAG)
        tmpval = bswap32( tmpval );

    *val = tmpval;
    stream->offset += 4;
    return 4;
}

static void
hm_stream_seek(hm_stream_t *stream, int offset) {
    stream->offset += offset;
}

#endif /* HM_BASE_H */