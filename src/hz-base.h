#ifndef HZ_BASE_H
#define HZ_BASE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <wchar.h>
#define HZ_LOG(...) fprintf(stdout, __VA_ARGS__)
#define HZ_DBGLOG(...) fprintf(stdout, __VA_ARGS__)
#define HZ_ERROR(...) fprintf(stderr, __VA_ARGS__)
#define HZ_MALLOC(size) malloc(size)
#define HZ_FREE(p) free(p)
#define HZ_REALLOC(p, size) realloc(p, size)
#define HZ_ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))


#define HZ_PACKED __attribute__((__packed__))

/*
 * Define base sized integer and float types
 * */
typedef unsigned char hz_uint8;
typedef unsigned short hz_uint16;
typedef unsigned long hz_uint32;
typedef unsigned long long hz_uint64;
typedef char hz_int8;
typedef short hz_int16;
typedef long hz_int32;
typedef long long hz_int64;
typedef float hz_float32;
typedef double hz_float64;

typedef uint8_t hz_byte, hz_bool, hz_char;
typedef uint16_t hz_offset16;
typedef uint32_t hz_tag;
typedef uint32_t hz_offset32;
typedef uint32_t hz_unicode; /* 32 bit unicode type */
typedef uint16_t hz_id;
typedef uint16_t hz_fixed;

#define HZ_F2DOT14 uint16_t
#define HZ_FWORD uint16_t
#define HZ_UFWORD uint16_t
typedef uint32_t HZ_Version16Dot16;

typedef uint64_t hz_uintptr;
typedef size_t hz_size;

static hz_float32
hz_cast_f32_f2d14(const HZ_F2DOT14 x)
{
    hz_float32 dec = (x & 0x3FFFU) / (hz_float32) 0x3FFFU;
    hz_float32 unit = (x & 0xC000U) >> 14U;
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

#define HZ_TRUE 1
#define HZ_FALSE 0

#define HZ_ASSERT(cond) assert(cond)
#define HZ_TAG(c1, c2, c3, c4) ((hz_tag)c4 | ((hz_tag)c3 << 8U) | ((hz_tag)c2 << 16U) | ((hz_tag)c1 << 24U))
#define HZ_UNTAG(tag) (tag >> 24) & 0xFF, (tag >> 16) & 0xFF, (tag >> 8) & 0xFF, tag & 0xFF
#define HZ_ALLOC(T) (T *) HZ_MALLOC(sizeof(T))


typedef struct hz_buf_t {
    uint8_t *data;
    size_t len;
} hz_buf_t;

typedef struct hz_face_t {
    hz_byte *base_table;
    hz_byte *gsub_table;
    hz_byte *gpos_table;
    hz_byte *gdef_table;
    hz_byte *jstf_table;
    hz_buf_t cmap_buf;
    void *handle; /* freetype handle */
} hz_face_t;

typedef enum hz_status_t {
    HZ_SUCCESS = 0,
    HZ_FAILURE = 1
} hz_status_t;

#define HZ_STREAM_BOUND_FLAG 0x01
#define HZ_STREAM_FLIP_ENDIANNESS_FLAG 0x02

#define HZ_STREAM_OVERFLOW 0

typedef struct hz_stream_t {
    uint8_t *data;
    size_t length;
    size_t offset;
    uint8_t flags;
} hz_stream_t;

static hz_stream_t *
hz_stream_create(uint8_t *data, size_t length, uint8_t flags)
{
    hz_stream_t *stream = (hz_stream_t *) HZ_MALLOC(sizeof(hz_stream_t));
    stream->offset = 0;
    stream->data = data;
    stream->length = length;
    stream->flags = flags;
    return stream;
}

static unsigned int
hz_stream_read8(hz_stream_t *stream, uint8_t *val) {
    const uint8_t *valptr = stream->data + stream->offset;

    if (stream->flags & HZ_STREAM_BOUND_FLAG && stream->offset + 1 >= stream->length)
        return HZ_STREAM_OVERFLOW;

    (*val) = valptr[0];
    stream->offset += 1;
    return 1;
}

static unsigned int
hz_stream_read16(hz_stream_t *stream, uint16_t *val) {
    uint16_t tmpval = 0;
    const uint8_t *valptr = stream->data + stream->offset;

    if (stream->flags & HZ_STREAM_BOUND_FLAG && stream->offset + 2 >= stream->length)
        return HZ_STREAM_OVERFLOW;

    tmpval |= (uint16_t) valptr[0] << 8;
    tmpval |= valptr[1];

    if (stream->flags & HZ_STREAM_FLIP_ENDIANNESS_FLAG)
        tmpval = bswap16( tmpval );

    *val = tmpval;
    stream->offset += 2;
    return 2;
}

static unsigned int
hz_stream_read32(hz_stream_t *stream, uint32_t *val) {
    uint32_t tmpval = 0;
    const uint8_t *valptr = stream->data + stream->offset;

    if (stream->flags & HZ_STREAM_BOUND_FLAG && stream->offset + 4 >= stream->length)
        return HZ_STREAM_OVERFLOW;

    tmpval |= (uint32_t) valptr[0] << 24;
    tmpval |= (uint32_t) valptr[1] << 16;
    tmpval |= (uint32_t) valptr[2] << 8;
    tmpval |= valptr[3];

    if (stream->flags & HZ_STREAM_FLIP_ENDIANNESS_FLAG)
        tmpval = bswap32( tmpval );

    *val = tmpval;
    stream->offset += 4;
    return 4;
}

static void
hz_stream_seek(hz_stream_t *stream, int offset) {
    stream->offset += offset;
}

#endif /* HZ_BASE_H */