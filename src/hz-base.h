#ifndef HZ_BASE_H
#define HZ_BASE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <wchar.h>

#define CMAS_IMPL
#include "cmas.h"

#include <stdarg.h>

#ifdef HZ_ENABLE_LOGGING
#define HZ_LOG(...) fprintf(stdout, __VA_ARGS__)
#define HZ_DBGLOG(...) fprintf(stdout, __VA_ARGS__)
#define HZ_ERROR(...) fprintf(stderr, __VA_ARGS__)
#else
#define HZ_LOG(...)
#define HZ_DBGLOG(...)
#define HZ_ERROR(...)
#endif
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
typedef void hz_void;

typedef int32_t hz_position;

/* signed 16-bit */
typedef int16_t FWORD;

/* unsigned 16-bit */
typedef uint16_t UFWORD;

/* unsigned 16-bit */
typedef uint16_t F2DOT14;

typedef unsigned char hz_byte, hz_bool, hz_char;
typedef uint16_t hz_offset16_t;
typedef uint32_t hz_tag_t;
typedef uint32_t hz_offset32_t;
typedef uint32_t hz_unicode_t; /* 32 bit unicode type */
typedef uint16_t hz_index_t;
typedef uint16_t hz_fixed_t;

#define HZ_F2DOT14 uint16_t
#define HZ_FWORD uint16_t
#define HZ_UFWORD uint16_t
typedef uint32_t HZ_Version16Dot16;

typedef uint64_t hz_uintptr;
typedef size_t hz_size_t;

static hz_float32
hz_cast_f32_f2d14(const HZ_F2DOT14 x)
{
    hz_float32 dec = (x & 0x3FFFU) / (hz_float32) 0x3FFFU;
    hz_float32 unit = (x & 0xC000U) >> 14U;
    return unit + dec;
}

static unsigned short
bswap16(unsigned short val) {
    unsigned short res = 0;
    res |= (val & 0x00ff) << 8;
    res |= (val & 0xff00) >> 8;
    return res;
}

static unsigned int
bswap32(unsigned int val) {
    unsigned int res = 0;
    res |= (val & 0x000000ff) << 24;
    res |= (val & 0x0000ff00) << 8;
    res |= (val & 0x00ff0000) >> 8;
    res |= (val & 0xff000000) >> 24;
    return res;
}

static unsigned long
bswap64(unsigned long val) {
    unsigned long res = 0;
    res |= (val & 0x00000000000000ff) << 56;
    res |= (val & 0x000000000000ff00) << 40;
    res |= (val & 0x0000000000ff0000) << 24;
    res |= (val & 0x00000000ff000000) << 8;
    res |= (val & 0x000000ff00000000) >> 8;
    res |= (val & 0x0000ff0000000000) >> 24;
    res |= (val & 0x00ff000000000000) >> 40;
    res |= (val & 0xff00000000000000) >> 56;
    return res;
}


#define HZ_TRUE 1
#define HZ_FALSE 0

#define HZ_ASSERT(cond) assert(cond)
#define HZ_TAG(c1, c2, c3, c4) ((hz_tag_t)c4 | ((hz_tag_t)c3 << 8U) | ((hz_tag_t)c2 << 16U) | ((hz_tag_t)c1 << 24U))
#define HZ_UNTAG(tag) (tag >> 24) & 0xFF, (tag >> 16) & 0xFF, (tag >> 8) & 0xFF, tag & 0xFF
#define HZ_ALLOC(T) (T *) HZ_MALLOC(sizeof(T))

/* V is the variable name, while T is the type/structure */
#define HZ_HEAPVAR(V, T) T *V = HZ_ALLOC(T)

#define HZ_HEAPARR(V, N, T) T *V = HZ_MALLOC(sizeof(T) * (N))

typedef struct buf_t {
    const unsigned char *data;
    long length;
    long idx;
    int flags;
} buf_t;

static buf_t
createbuf(const unsigned char *data, int flags) {
    buf_t buf;
    buf.idx = 0;
    buf.data = data;
    buf.length = 0;
    buf.flags = flags;
    return buf;
}

//static unsigned int
//hz_stream_read8(buf_t *stream, uint8_t *val) {
//    const uint8_t *valptr = stream->data + stream->idx;
//
//    if (stream->flags & HZ_STREAM_BOUND_FLAG && stream->idx + 1 >= stream->length)
//        return HZ_STREAM_OVERFLOW;
//
//    (*val) = valptr[0];
//    stream->idx += 1;
//    return 1;
//}
//
//static unsigned int
//hz_stream_read16(buf_t *stream, uint16_t *val) {
//    uint16_t tmpval = 0;
//    const uint8_t *valptr = stream->data + stream->idx;
//
//    if (stream->flags & HZ_STREAM_BOUND_FLAG && stream->idx + 2 >= stream->length)
//        return HZ_STREAM_OVERFLOW;
//
//    tmpval |= (uint16_t) valptr[0] << 8;
//    tmpval |= valptr[1];
//
//    if (stream->flags & HZ_STREAM_FLIP_ENDIANNESS_FLAG)
//        tmpval = bswap16( tmpval );
//
//    *val = tmpval;
//    stream->idx += 2;
//    return 2;
//}
//
//static unsigned int
//hz_stream_read32(buf_t *stream, uint32_t *val) {
//    uint32_t tmpval = 0;
//    const uint8_t *valptr = stream->data + stream->idx;
//
//    if (stream->flags & HZ_STREAM_BOUND_FLAG && stream->idx + 4 >= stream->length)
//        return HZ_STREAM_OVERFLOW;
//
//    tmpval |= (uint32_t) valptr[0] << 24;
//    tmpval |= (uint32_t) valptr[1] << 16;
//    tmpval |= (uint32_t) valptr[2] << 8;
//    tmpval |= valptr[3];
//
//    if (stream->flags & HZ_STREAM_FLIP_ENDIANNESS_FLAG)
//        tmpval = bswap32( tmpval );
//
//    *val = tmpval;
//    stream->idx += 4;
//    return 4;
//}

//static void
//hz_stream_seek(buf_t *stream, int offset) {
//    stream->idx += offset;
//}

//static void
//hz_stream_read16_n(buf_t *stream, size_t count, uint16_t *A)
//{
//    if (A != NULL) {
//        size_t i = 0;
//
//        while (i < count) {
//            uint16_t *val = A + i;
//
//            hz_stream_read16(stream, val);
//
//            ++i;
//        }
//    }
//}

typedef struct hz_metrics_t {
    int32_t x_advance;
    int32_t y_advance;
    int32_t x_bearing;
    int32_t y_bearing;

    int32_t x_min;
    int32_t x_max;
    int32_t y_min;
    int32_t y_max;
    int32_t width;
    int32_t height;
} hz_metrics_t;


#define BUF_BSWAP  0x01
#define BUF_OF     0x02 /* overflow */
#define BUF_BCHECK 0x04 /* bounds check */

static unsigned char
unpackb(buf_t *buf) {
    return buf->data[buf->idx++];
}
/*
static char
unpacksb(buf_t *buf) {
    return *(char *)&buf->data[buf->idx++];
}
*/
static unsigned short
unpackh(buf_t *buf) {
    unsigned short val = 0;
    val |= (unsigned short) unpackb(buf);
    val |= (unsigned short) unpackb(buf) << 8;

    if (buf->flags & BUF_BSWAP) val = bswap16(val);

    return val;
}

static unsigned int
unpacki(buf_t *buf) {
    unsigned int val = 0;
    val |= (unsigned int) unpackb(buf);
    val |= (unsigned int) unpackb(buf) << 8;
    val |= (unsigned int) unpackb(buf) << 16;
    val |= (unsigned int) unpackb(buf) << 24;

    if (buf->flags & BUF_BSWAP) val = bswap32(val);

    return val;
}

static unsigned long
unpackl(buf_t *buf) {
    unsigned long val = 0;
    val |= (unsigned long) unpackb(buf);
    val |= (unsigned long) unpackb(buf) << 8;
    val |= (unsigned long) unpackb(buf) << 16;
    val |= (unsigned long) unpackb(buf) << 24;
    val |= (unsigned long) unpackb(buf) << 32;
    val |= (unsigned long) unpackb(buf) << 40;
    val |= (unsigned long) unpackb(buf) << 48;
    val |= (unsigned long) unpackb(buf) << 56;

    if (buf->flags & BUF_BSWAP) val = bswap64(val);

    return val;
}

static void
enablebswap(buf_t *buf) {
    buf->flags |= BUF_BSWAP;
}

static char
peek(const char *p, int i) {
    return *(p + i);
}

static void
bufseek(buf_t *buf, int n) {
    buf->idx += n;
}

static void
unpackv(buf_t *buf, const char *f, ...)
{
    const char *c;
    va_list v;
    va_start(v,f);

    for (c=f; *c!='\0'; ++c) {
        switch (*c) {
            case 'b': { /* byte (8-bit unsigned) */
                unsigned char *val = va_arg(v, unsigned char *);

                if (peek(c,1) == ':') {
                    ++c;
                    if (peek(c, 1) == '*') {
                        int i, n;
                        n = va_arg(v, int);
                        for (i=0;i<n;++i)
                            val[i] = unpackb(buf);
                    }
                } else {
                    *val = unpackb(buf);
                }

                break;
            }
            case 'h': { /* half (short, 16-bit unsigned */
                unsigned short *val = va_arg(v, unsigned short *);

                if (peek(c,1) == ':') {
                    ++c;
                    if (peek(c, 1) == '*') {
                        int i, n;
                        n = va_arg(v, int);
                        for (i=0;i<n;++i)
                            val[i] = unpackh(buf);
                    }
                } else {
                    *val = unpackh(buf);
                }

                break;
            }
            case 'i': { /* int (32-bit unsigned) */
                unsigned int *val = va_arg(v, unsigned int *);

                if (peek(c,1) == ':') {
                    ++c;
                    if (peek(c, 1) == '*') {
                        int i, n;
                        n = va_arg(v, int);
                        for (i=0;i<n;++i)
                            val[i] = unpacki(buf);
                    }
                } else {
                    *val = unpacki(buf);
                }

                break;
            }
            case 'l': /* long (64-bit unsigned) */
                break;
            case 'f': /* float */
                break;
            case 'd': /* double */
                break;
            case 's':
                break;
        }
    }

    va_end(v);
}

/*
 * unpackf(buf, "bbh", &a, &b, &c);
 *
 * unpackf(
 * */


/* Enum: hz_glyph_class_t
 * HZ_GLYPH_CLASS_ZERO - No class.
 * HZ_GLYPH_CLASS_BASE - Base glyph class.
 * HZ_GLYPH_CLASS_LIGATURE - Ligature glyph class (composed of smaller subglyphs).
 * HZ_GLYPH_CLASS_MARK - Mark glyph class, as in Arabic tashkeel or accents.
 * HZ_GLYPH_CLASS_COMPONENT - Component glyph class.
 */
typedef enum hz_glyph_class_t {
    HZ_GLYPH_CLASS_ZERO      = 0x00,
    HZ_GLYPH_CLASS_BASE      = 0x01,
    HZ_GLYPH_CLASS_LIGATURE  = 0x02,
    HZ_GLYPH_CLASS_MARK      = 0x04,
    HZ_GLYPH_CLASS_COMPONENT = 0x08,
} hz_glyph_class_t;

#define HZ_GLYPH_CLASS_BIT_FIELD 4
#define HZ_BIT(x) (1 << (x))

#endif /* HZ_BASE_H */