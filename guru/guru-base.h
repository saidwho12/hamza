#ifndef GURU_BASE_H
#define GURU_BASE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#define GURU_LOG(...) fprintf(stdout, __VA_ARGS__)
#define GURU_DBGLOG(...) fprintf(stdout, __VA_ARGS__)
#define GURU_ERROR(...) fprintf(stderr, __VA_ARGS__)
#define GURU_MALLOC(size) malloc(size)
#define GURU_FREE(p) free(p)
#define GURU_REALLOC(p, size) realloc(p, size)


#define GURU_PACKED __attribute__((__packed__))

/*
 * Define base sized integer and float types
 * */
typedef unsigned char guru_uint8;
typedef unsigned short guru_uint16;
typedef unsigned long guru_uint32;
typedef unsigned long long guru_uint64;
typedef char guru_int8;
typedef short guru_int16;
typedef long guru_int32;
typedef long long guru_int64;
typedef float guru_float32;
typedef double guru_float64;

typedef uint8_t guru_byte, guru_bool, guru_char;
typedef uint16_t guru_offset16;
typedef uint32_t guru_tag;
typedef uint32_t guru_offset32;
typedef uint32_t guru_unicode; /* 32 bit unicode type */
typedef uint16_t guru_id;
typedef uint16_t guru_fixed;

#define GURU_F2DOT14 uint16_t
#define GURU_FWORD uint16_t
#define GURU_UFWORD uint16_t
typedef uint32_t GURU_Version16Dot16;

typedef uint64_t guru_uintptr;
typedef size_t guru_size;

static guru_float32
guru_cast_f32_f2d14(const GURU_F2DOT14 x)
{
    guru_float32 dec = (x & 0x3FFFU) / (guru_float32) 0x3FFFU;
    guru_float32 unit = (x & 0xC000U) >> 14U;
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

#define GURU_TRUE 1
#define GURU_FALSE 0

#define GURU_ASSERT(cond) assert(cond)
#define GURU_TAG(c1, c2, c3, c4) ((guru_tag)c4 | ((guru_tag)c3 << 8U) | ((guru_tag)c2 << 16U) | ((guru_tag)c1 << 24U))
#define GURU_UNTAG(tag) (tag >> 24) & 0xFF, (tag >> 16) & 0xFF, (tag >> 8) & 0xFF, tag & 0xFF
#define GURU_ALLOC(T) (T *) GURU_MALLOC(sizeof(T))


typedef struct guru_buf_t {
    uint8_t *data;
    size_t len;
} guru_buf_t;

typedef struct guru_face_t {
    guru_byte *BASE;
    guru_byte *GSUB;
    guru_byte *GPOS;
    guru_byte *GDEF;
    guru_byte *JSTF;
    guru_buf_t cmap_buf;
    void *handle; /* freetype handle */
} guru_face_t;

typedef enum guru_status_t {
    GURU_SUCCESS = 0,
    GURU_FAILURE = 1
} guru_status_t;

#define GURU_STREAM_BOUND_FLAG 0x01
#define GURU_STREAM_FLIP_ENDIANNESS_FLAG 0x02

#define GURU_STREAM_OVERFLOW 0

typedef struct guru_stream_t {
    guru_byte *data;
    guru_size length;
    guru_size offset;
    guru_uint8 flags;
} guru_stream_t;

static guru_stream_t
guru_stream_create(guru_byte *data, guru_size length, guru_uint8 flags)
{
    guru_stream_t stream;
    stream.offset = 0;
    stream.data = data;
    stream.length = length;
    stream.flags = flags;
    return stream;
}

static unsigned int
guru_stream_read8(guru_stream_t *stream, uint8_t *val) {
    const uint8_t *valptr = stream->data + stream->offset;

    if (stream->flags & GURU_STREAM_BOUND_FLAG && stream->offset + 1 >= stream->length)
        return GURU_STREAM_OVERFLOW;

    (*val) = valptr[0];
    stream->offset += 1;
    return 1;
}

static unsigned int
guru_stream_read16(guru_stream_t *stream, uint16_t *val) {
    uint16_t tmpval = 0;
    const uint8_t *valptr = stream->data + stream->offset;

    if (stream->flags & GURU_STREAM_BOUND_FLAG && stream->offset + 2 >= stream->length)
        return GURU_STREAM_OVERFLOW;

    tmpval |= (uint16_t) valptr[0] << 8;
    tmpval |= valptr[1];

    if (stream->flags & GURU_STREAM_FLIP_ENDIANNESS_FLAG)
        tmpval = bswap16( tmpval );

    *val = tmpval;
    stream->offset += 2;
    return 2;
}

static unsigned int
guru_stream_read32(guru_stream_t *stream, uint32_t *val) {
    uint32_t tmpval = 0;
    const uint8_t *valptr = stream->data + stream->offset;

    if (stream->flags & GURU_STREAM_BOUND_FLAG && stream->offset + 4 >= stream->length)
        return GURU_STREAM_OVERFLOW;

    tmpval |= (uint32_t) valptr[0] << 24;
    tmpval |= (uint32_t) valptr[1] << 16;
    tmpval |= (uint32_t) valptr[2] << 8;
    tmpval |= valptr[3];

    if (stream->flags & GURU_STREAM_FLIP_ENDIANNESS_FLAG)
        tmpval = bswap32( tmpval );

    *val = tmpval;
    stream->offset += 4;
    return 4;
}

#endif /* GURU_BASE_H */