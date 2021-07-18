#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <inttypes.h>
#include <time.h>

#ifdef WIN32

#include <windows.h>
double get_time()
{
    LARGE_INTEGER t, f;
    QueryPerformanceCounter(&t);
    QueryPerformanceFrequency(&f);
    return (double)t.QuadPart/(double)f.QuadPart;
}

#else

#include <sys/time.h>
#include <sys/resource.h>

double get_time() 
{
    struct timeval t;
    /*struct timezone tzp;*/
    gettimeofday(&t, NULL/*&tzp*/);
    return t.tv_sec + t.tv_usec*1e-6;
}

#endif


#define KIB 1024
#define MIB 1048576
#define GIB 1073741824

const char *
mem_unit (size_t n)
{
    if (n > GIB) return "GiB";
    else if (n > MIB) return "MiB";
    else if (n > KIB) return "KiB";
    else return "byte(s)";
}

double
mem_value (size_t n)
{
    if (n > GIB) return (double)n / (double)GIB;
    else if (n > MIB) return (double)n / (double)MIB;
    else if (n > KIB) return (double)n / (double)KIB;
    else return n;
}

#define PRINTMEM(n) mem_value(n), mem_unit(n)

#define NS 1.e-9
#define US 1.e-6
#define MS 1.e-3
#define SEC 1.

const double
time_print_val(double t)
{
    if (t < US) return t/NS;
    else if (t < MS) return t/US;
    else if (t < SEC) return t/MS;
    else return t;
}

const char *
get_time_period_suffix(double t)
{
    if (t < US) return "ns";
    else if (t < MS) return "us";
    else if (t < SEC) return "ms";
    else return "s";
}

#define PRINTTIME(t) time_print_val(t), get_time_period_suffix(t)

#define CHUNKSIZE 1024
#define UC_UNK 0xfffd
#define UC_NULL 0x0000

typedef uint8_t char8_t;
typedef uint32_t char32_t, unicode_t;
typedef uint16_t ucs2_char_t;

typedef struct DecoderState {
    const char8_t      *data;
    unicode_t           code;
    uintptr_t           ptr;
    size_t              len; /* if 0, no limit */
    int                 step;
} DecoderState;

/*
This table decodes the first byte of a utf-8 sequence.
It returns the number of expected continuation bytes
in the rest of the sequence.
From wikipedia:
# Bits      First       Last        Sequence    Byte 1      Byte 2      Byte 3      Byte 4      Byte 5      Byte 6
# 7         0x0000      0x007F      1           0xxxxxxx
# 11        0x0080      0x07FF      2           110xxxxx    10xxxxxx
# 16        0x0800      0xFFFF      3           1110xxxx    10xxxxxx    10xxxxxx
# 21        0x10000     0x1FFFFF    4           11110xxx    10xxxxxx    10xxxxxx    10xxxxxx
# 26        0x200000    0x3FFFFFF   5           111110xx    10xxxxxx    10xxxxxx    10xxxxxx    10xxxxxx
# 31        0x4000000   0x7FFFFFFF  6           1111110x    10xxxxxx    10xxxxxx    10xxxxxx    10xxxxxx    10xxxxxx
*/
static const int8_t decode_byte_table[] = {
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  /* 0x00 */
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  /* 0x10 */
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  /* 0x20 */
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  /* 0x30 */
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  /* 0x40 */
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  /* 0x50 */
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  /* 0x60 */
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  /* 0x70 */
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* 0x80 */
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* 0x90 */
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* 0xa0 */
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* 0xb0 */
 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  /* 0xc0 */
 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  /* 0xd0 */
 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  /* 0xe0 */
 3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  5,  5, -1, -1,  /* 0xf0 */
};

/* This table returns the mask for the data in the first byte
of an n-byte sequence, where n is the index into the table.
*/
static const uint8_t decode_mask_table[] = {
    0x7f, 0x1f, 0x0f, 0x07, 0x03, 0x01,
};

#define START 0

void
decoder_state_init(DecoderState *state)
{
    state->data = NULL;
    state->ptr = 0;
    state->len = 0;
    state->code = 0;
    state->step = START;
}

void decoder_state_reset(DecoderState *state)
{
    state->ptr = 0;
    state->len = 0;
    state->code = 0;
    state->step = START;
}

void
decoder_state_set_data(DecoderState *state, const char8_t *data)
{
    state->data = data;
}

/*
    Implementation of fast UTF-8 decoding by Chris Venter:
    https://gist.github.com/gorb314/7888804
*/
void
decode_byte_chris_venter(DecoderState *state)
{
    char8_t byte;

retry:
    byte = state->data[state->ptr++];
    if (state->step == START)
    {
        /* decode the number of steps using the first byte */
        state->step = decode_byte_table[byte];
        if (state->step < 0) goto error; /* invalid starting byte */
        /* extract the data from the first byte, using the proper mask */
        state->code = ((uint32_t)byte & decode_mask_table[state->step]) << (6*state->step);
    }
    else
    {
        if ((byte & 0xc0) != 0x80) /* invalid continuation byte */
        {
            /* retry as starting byte */
            state->step = START;
            goto retry;
        }

        if (state->step == 0) goto error; /* too many continuation bytes */
        state->step -= 1;
        /* extract the data from the continuation byte */
        state->code |= ((uint32_t)byte & 0x3f) << (6*state->step);
    }
    return;
error:
    /* return the unicode "unknown" character and start again */
    state->step = START;
    state->code = UC_UNK;
}

unicode_t
decode_next_codepoint_chris_venter(DecoderState *state)
{
    do {
        decode_byte_chris_venter(state);
    } while (state->step != START);

    return state->code;
}


void
bench_decode_chris_venter_scalar(const char8_t *data, size_t len, int runs)
{
    int r;
    double t1, t2, delta;
    DecoderState state;
    printf("Benchmarking Chris Venter UTF-8 scalar decode...\n");
    printf("Decoding %0.2lf %s of UTF-8...\n", PRINTMEM(len));

    decoder_state_init(&state);
    decoder_state_set_data(&state, data);

    for (r = 0; r < runs; ++r) {
        unicode_t code;
        t1 = get_time();
        decoder_state_reset(&state);
        do {
            code = decode_next_codepoint_chris_venter(&state);
            /*printf("U+%X\n", code);*/
        } while (state.ptr < len);

        t2 = get_time();
        delta = t2 - t1;
        printf("Run %d: took %0.2lf %s\n", r+1, PRINTTIME(delta));
    }
}

static const int8_t prefix_to_length_table[32] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 3
};

typedef union vector128 {
    uint8_t _u8[16];
    int8_t _i8[16];
    uint16_t _u16[8];
    int16_t _i16[8];
    uint32_t _u32[4];
    int32_t _i32[4];
    uint64_t _u64[2];
    int64_t _i64[2];
} vector128;

vector128
shuffle_v128(const vector128 a, const vector128 b)
{
    vector128 v;
    int i;
    memset(v._u8, 0, 16);

    for (i = 0; i < 16; ++i)
        if (b._i8[i] >= 0)
            v._u8[i] = a._u8[b._i8[i]];

    return v;
}

vector128
and_v128(const register vector128 a, const register vector128 b)
{
    vector128 v;
    v._u64[0] = a._u64[0] & b._u64[0];
    v._u64[1] = a._u64[1] & b._u64[1];
    return v;
}

vector128
load_v128(const void *ptr)
{
    return *(vector128 *)ptr;
}

vector128
loadi8_v128(int8_t x)
{
    vector128 v;
    int i;

    for (i = 0; i < 16; ++i) {
        v._i8[i] = x;
    }

    return v;
}

/* 256 * 16 = 4096 */
static const vector128 ibm_pattern_lut_128[256] = {
    {-1,-1,-1,0,-1,-1,-1,1,-1,-1,-1,2,-1,-1,-1,3},
    {-1,-1,1,0,-1,-1,-1,2,-1,-1,-1,3,-1,-1,-1,4},
    {-1,2,1,0,-1,-1,-1,3,-1,-1,-1,4,-1,-1,-1,5},
    {3,2,1,0,-1,-1,-1,4,-1,-1,-1,5,-1,-1,-1,6},
    {-1,-1,-1,0,-1,-1,2,1,-1,-1,-1,3,-1,-1,-1,4},
    {-1,-1,1,0,-1,-1,3,2,-1,-1,-1,4,-1,-1,-1,5},
    {-1,2,1,0,-1,-1,4,3,-1,-1,-1,5,-1,-1,-1,6},
    {3,2,1,0,-1,-1,5,4,-1,-1,-1,6,-1,-1,-1,7},
    {-1,-1,-1,0,-1,3,2,1,-1,-1,-1,4,-1,-1,-1,5},
    {-1,-1,1,0,-1,4,3,2,-1,-1,-1,5,-1,-1,-1,6},
    {-1,2,1,0,-1,5,4,3,-1,-1,-1,6,-1,-1,-1,7},
    {3,2,1,0,-1,6,5,4,-1,-1,-1,7,-1,-1,-1,8},
    {-1,-1,-1,0,4,3,2,1,-1,-1,-1,5,-1,-1,-1,6},
    {-1,-1,1,0,5,4,3,2,-1,-1,-1,6,-1,-1,-1,7},
    {-1,2,1,0,6,5,4,3,-1,-1,-1,7,-1,-1,-1,8},
    {3,2,1,0,7,6,5,4,-1,-1,-1,8,-1,-1,-1,9},
    {-1,-1,-1,0,-1,-1,-1,1,-1,-1,3,2,-1,-1,-1,4},
    {-1,-1,1,0,-1,-1,-1,2,-1,-1,4,3,-1,-1,-1,5},
    {-1,2,1,0,-1,-1,-1,3,-1,-1,5,4,-1,-1,-1,6},
    {3,2,1,0,-1,-1,-1,4,-1,-1,6,5,-1,-1,-1,7},
    {-1,-1,-1,0,-1,-1,2,1,-1,-1,4,3,-1,-1,-1,5},
    {-1,-1,1,0,-1,-1,3,2,-1,-1,5,4,-1,-1,-1,6},
    {-1,2,1,0,-1,-1,4,3,-1,-1,6,5,-1,-1,-1,7},
    {3,2,1,0,-1,-1,5,4,-1,-1,7,6,-1,-1,-1,8},
    {-1,-1,-1,0,-1,3,2,1,-1,-1,5,4,-1,-1,-1,6},
    {-1,-1,1,0,-1,4,3,2,-1,-1,6,5,-1,-1,-1,7},
    {-1,2,1,0,-1,5,4,3,-1,-1,7,6,-1,-1,-1,8},
    {3,2,1,0,-1,6,5,4,-1,-1,8,7,-1,-1,-1,9},
    {-1,-1,-1,0,4,3,2,1,-1,-1,6,5,-1,-1,-1,7},
    {-1,-1,1,0,5,4,3,2,-1,-1,7,6,-1,-1,-1,8},
    {-1,2,1,0,6,5,4,3,-1,-1,8,7,-1,-1,-1,9},
    {3,2,1,0,7,6,5,4,-1,-1,9,8,-1,-1,-1,10},
    {-1,-1,-1,0,-1,-1,-1,1,-1,4,3,2,-1,-1,-1,5},
    {-1,-1,1,0,-1,-1,-1,2,-1,5,4,3,-1,-1,-1,6},
    {-1,2,1,0,-1,-1,-1,3,-1,6,5,4,-1,-1,-1,7},
    {3,2,1,0,-1,-1,-1,4,-1,7,6,5,-1,-1,-1,8},
    {-1,-1,-1,0,-1,-1,2,1,-1,5,4,3,-1,-1,-1,6},
    {-1,-1,1,0,-1,-1,3,2,-1,6,5,4,-1,-1,-1,7},
    {-1,2,1,0,-1,-1,4,3,-1,7,6,5,-1,-1,-1,8},
    {3,2,1,0,-1,-1,5,4,-1,8,7,6,-1,-1,-1,9},
    {-1,-1,-1,0,-1,3,2,1,-1,6,5,4,-1,-1,-1,7},
    {-1,-1,1,0,-1,4,3,2,-1,7,6,5,-1,-1,-1,8},
    {-1,2,1,0,-1,5,4,3,-1,8,7,6,-1,-1,-1,9},
    {3,2,1,0,-1,6,5,4,-1,9,8,7,-1,-1,-1,10},
    {-1,-1,-1,0,4,3,2,1,-1,7,6,5,-1,-1,-1,8},
    {-1,-1,1,0,5,4,3,2,-1,8,7,6,-1,-1,-1,9},
    {-1,2,1,0,6,5,4,3,-1,9,8,7,-1,-1,-1,10},
    {3,2,1,0,7,6,5,4,-1,10,9,8,-1,-1,-1,11},
    {-1,-1,-1,0,-1,-1,-1,1,5,4,3,2,-1,-1,-1,6},
    {-1,-1,1,0,-1,-1,-1,2,6,5,4,3,-1,-1,-1,7},
    {-1,2,1,0,-1,-1,-1,3,7,6,5,4,-1,-1,-1,8},
    {3,2,1,0,-1,-1,-1,4,8,7,6,5,-1,-1,-1,9},
    {-1,-1,-1,0,-1,-1,2,1,6,5,4,3,-1,-1,-1,7},
    {-1,-1,1,0,-1,-1,3,2,7,6,5,4,-1,-1,-1,8},
    {-1,2,1,0,-1,-1,4,3,8,7,6,5,-1,-1,-1,9},
    {3,2,1,0,-1,-1,5,4,9,8,7,6,-1,-1,-1,10},
    {-1,-1,-1,0,-1,3,2,1,7,6,5,4,-1,-1,-1,8},
    {-1,-1,1,0,-1,4,3,2,8,7,6,5,-1,-1,-1,9},
    {-1,2,1,0,-1,5,4,3,9,8,7,6,-1,-1,-1,10},
    {3,2,1,0,-1,6,5,4,10,9,8,7,-1,-1,-1,11},
    {-1,-1,-1,0,4,3,2,1,8,7,6,5,-1,-1,-1,9},
    {-1,-1,1,0,5,4,3,2,9,8,7,6,-1,-1,-1,10},
    {-1,2,1,0,6,5,4,3,10,9,8,7,-1,-1,-1,11},
    {3,2,1,0,7,6,5,4,11,10,9,8,-1,-1,-1,12},
    {-1,-1,-1,0,-1,-1,-1,1,-1,-1,-1,2,-1,-1,4,3},
    {-1,-1,1,0,-1,-1,-1,2,-1,-1,-1,3,-1,-1,5,4},
    {-1,2,1,0,-1,-1,-1,3,-1,-1,-1,4,-1,-1,6,5},
    {3,2,1,0,-1,-1,-1,4,-1,-1,-1,5,-1,-1,7,6},
    {-1,-1,-1,0,-1,-1,2,1,-1,-1,-1,3,-1,-1,5,4},
    {-1,-1,1,0,-1,-1,3,2,-1,-1,-1,4,-1,-1,6,5},
    {-1,2,1,0,-1,-1,4,3,-1,-1,-1,5,-1,-1,7,6},
    {3,2,1,0,-1,-1,5,4,-1,-1,-1,6,-1,-1,8,7},
    {-1,-1,-1,0,-1,3,2,1,-1,-1,-1,4,-1,-1,6,5},
    {-1,-1,1,0,-1,4,3,2,-1,-1,-1,5,-1,-1,7,6},
    {-1,2,1,0,-1,5,4,3,-1,-1,-1,6,-1,-1,8,7},
    {3,2,1,0,-1,6,5,4,-1,-1,-1,7,-1,-1,9,8},
    {-1,-1,-1,0,4,3,2,1,-1,-1,-1,5,-1,-1,7,6},
    {-1,-1,1,0,5,4,3,2,-1,-1,-1,6,-1,-1,8,7},
    {-1,2,1,0,6,5,4,3,-1,-1,-1,7,-1,-1,9,8},
    {3,2,1,0,7,6,5,4,-1,-1,-1,8,-1,-1,10,9},
    {-1,-1,-1,0,-1,-1,-1,1,-1,-1,3,2,-1,-1,5,4},
    {-1,-1,1,0,-1,-1,-1,2,-1,-1,4,3,-1,-1,6,5},
    {-1,2,1,0,-1,-1,-1,3,-1,-1,5,4,-1,-1,7,6},
    {3,2,1,0,-1,-1,-1,4,-1,-1,6,5,-1,-1,8,7},
    {-1,-1,-1,0,-1,-1,2,1,-1,-1,4,3,-1,-1,6,5},
    {-1,-1,1,0,-1,-1,3,2,-1,-1,5,4,-1,-1,7,6},
    {-1,2,1,0,-1,-1,4,3,-1,-1,6,5,-1,-1,8,7},
    {3,2,1,0,-1,-1,5,4,-1,-1,7,6,-1,-1,9,8},
    {-1,-1,-1,0,-1,3,2,1,-1,-1,5,4,-1,-1,7,6},
    {-1,-1,1,0,-1,4,3,2,-1,-1,6,5,-1,-1,8,7},
    {-1,2,1,0,-1,5,4,3,-1,-1,7,6,-1,-1,9,8},
    {3,2,1,0,-1,6,5,4,-1,-1,8,7,-1,-1,10,9},
    {-1,-1,-1,0,4,3,2,1,-1,-1,6,5,-1,-1,8,7},
    {-1,-1,1,0,5,4,3,2,-1,-1,7,6,-1,-1,9,8},
    {-1,2,1,0,6,5,4,3,-1,-1,8,7,-1,-1,10,9},
    {3,2,1,0,7,6,5,4,-1,-1,9,8,-1,-1,11,10},
    {-1,-1,-1,0,-1,-1,-1,1,-1,4,3,2,-1,-1,6,5},
    {-1,-1,1,0,-1,-1,-1,2,-1,5,4,3,-1,-1,7,6},
    {-1,2,1,0,-1,-1,-1,3,-1,6,5,4,-1,-1,8,7},
    {3,2,1,0,-1,-1,-1,4,-1,7,6,5,-1,-1,9,8},
    {-1,-1,-1,0,-1,-1,2,1,-1,5,4,3,-1,-1,7,6},
    {-1,-1,1,0,-1,-1,3,2,-1,6,5,4,-1,-1,8,7},
    {-1,2,1,0,-1,-1,4,3,-1,7,6,5,-1,-1,9,8},
    {3,2,1,0,-1,-1,5,4,-1,8,7,6,-1,-1,10,9},
    {-1,-1,-1,0,-1,3,2,1,-1,6,5,4,-1,-1,8,7},
    {-1,-1,1,0,-1,4,3,2,-1,7,6,5,-1,-1,9,8},
    {-1,2,1,0,-1,5,4,3,-1,8,7,6,-1,-1,10,9},
    {3,2,1,0,-1,6,5,4,-1,9,8,7,-1,-1,11,10},
    {-1,-1,-1,0,4,3,2,1,-1,7,6,5,-1,-1,9,8},
    {-1,-1,1,0,5,4,3,2,-1,8,7,6,-1,-1,10,9},
    {-1,2,1,0,6,5,4,3,-1,9,8,7,-1,-1,11,10},
    {3,2,1,0,7,6,5,4,-1,10,9,8,-1,-1,12,11},
    {-1,-1,-1,0,-1,-1,-1,1,5,4,3,2,-1,-1,7,6},
    {-1,-1,1,0,-1,-1,-1,2,6,5,4,3,-1,-1,8,7},
    {-1,2,1,0,-1,-1,-1,3,7,6,5,4,-1,-1,9,8},
    {3,2,1,0,-1,-1,-1,4,8,7,6,5,-1,-1,10,9},
    {-1,-1,-1,0,-1,-1,2,1,6,5,4,3,-1,-1,8,7},
    {-1,-1,1,0,-1,-1,3,2,7,6,5,4,-1,-1,9,8},
    {-1,2,1,0,-1,-1,4,3,8,7,6,5,-1,-1,10,9},
    {3,2,1,0,-1,-1,5,4,9,8,7,6,-1,-1,11,10},
    {-1,-1,-1,0,-1,3,2,1,7,6,5,4,-1,-1,9,8},
    {-1,-1,1,0,-1,4,3,2,8,7,6,5,-1,-1,10,9},
    {-1,2,1,0,-1,5,4,3,9,8,7,6,-1,-1,11,10},
    {3,2,1,0,-1,6,5,4,10,9,8,7,-1,-1,12,11},
    {-1,-1,-1,0,4,3,2,1,8,7,6,5,-1,-1,10,9},
    {-1,-1,1,0,5,4,3,2,9,8,7,6,-1,-1,11,10},
    {-1,2,1,0,6,5,4,3,10,9,8,7,-1,-1,12,11},
    {3,2,1,0,7,6,5,4,11,10,9,8,-1,-1,13,12},
    {-1,-1,-1,0,-1,-1,-1,1,-1,-1,-1,2,-1,5,4,3},
    {-1,-1,1,0,-1,-1,-1,2,-1,-1,-1,3,-1,6,5,4},
    {-1,2,1,0,-1,-1,-1,3,-1,-1,-1,4,-1,7,6,5},
    {3,2,1,0,-1,-1,-1,4,-1,-1,-1,5,-1,8,7,6},
    {-1,-1,-1,0,-1,-1,2,1,-1,-1,-1,3,-1,6,5,4},
    {-1,-1,1,0,-1,-1,3,2,-1,-1,-1,4,-1,7,6,5},
    {-1,2,1,0,-1,-1,4,3,-1,-1,-1,5,-1,8,7,6},
    {3,2,1,0,-1,-1,5,4,-1,-1,-1,6,-1,9,8,7},
    {-1,-1,-1,0,-1,3,2,1,-1,-1,-1,4,-1,7,6,5},
    {-1,-1,1,0,-1,4,3,2,-1,-1,-1,5,-1,8,7,6},
    {-1,2,1,0,-1,5,4,3,-1,-1,-1,6,-1,9,8,7},
    {3,2,1,0,-1,6,5,4,-1,-1,-1,7,-1,10,9,8},
    {-1,-1,-1,0,4,3,2,1,-1,-1,-1,5,-1,8,7,6},
    {-1,-1,1,0,5,4,3,2,-1,-1,-1,6,-1,9,8,7},
    {-1,2,1,0,6,5,4,3,-1,-1,-1,7,-1,10,9,8},
    {3,2,1,0,7,6,5,4,-1,-1,-1,8,-1,11,10,9},
    {-1,-1,-1,0,-1,-1,-1,1,-1,-1,3,2,-1,6,5,4},
    {-1,-1,1,0,-1,-1,-1,2,-1,-1,4,3,-1,7,6,5},
    {-1,2,1,0,-1,-1,-1,3,-1,-1,5,4,-1,8,7,6},
    {3,2,1,0,-1,-1,-1,4,-1,-1,6,5,-1,9,8,7},
    {-1,-1,-1,0,-1,-1,2,1,-1,-1,4,3,-1,7,6,5},
    {-1,-1,1,0,-1,-1,3,2,-1,-1,5,4,-1,8,7,6},
    {-1,2,1,0,-1,-1,4,3,-1,-1,6,5,-1,9,8,7},
    {3,2,1,0,-1,-1,5,4,-1,-1,7,6,-1,10,9,8},
    {-1,-1,-1,0,-1,3,2,1,-1,-1,5,4,-1,8,7,6},
    {-1,-1,1,0,-1,4,3,2,-1,-1,6,5,-1,9,8,7},
    {-1,2,1,0,-1,5,4,3,-1,-1,7,6,-1,10,9,8},
    {3,2,1,0,-1,6,5,4,-1,-1,8,7,-1,11,10,9},
    {-1,-1,-1,0,4,3,2,1,-1,-1,6,5,-1,9,8,7},
    {-1,-1,1,0,5,4,3,2,-1,-1,7,6,-1,10,9,8},
    {-1,2,1,0,6,5,4,3,-1,-1,8,7,-1,11,10,9},
    {3,2,1,0,7,6,5,4,-1,-1,9,8,-1,12,11,10},
    {-1,-1,-1,0,-1,-1,-1,1,-1,4,3,2,-1,7,6,5},
    {-1,-1,1,0,-1,-1,-1,2,-1,5,4,3,-1,8,7,6},
    {-1,2,1,0,-1,-1,-1,3,-1,6,5,4,-1,9,8,7},
    {3,2,1,0,-1,-1,-1,4,-1,7,6,5,-1,10,9,8},
    {-1,-1,-1,0,-1,-1,2,1,-1,5,4,3,-1,8,7,6},
    {-1,-1,1,0,-1,-1,3,2,-1,6,5,4,-1,9,8,7},
    {-1,2,1,0,-1,-1,4,3,-1,7,6,5,-1,10,9,8},
    {3,2,1,0,-1,-1,5,4,-1,8,7,6,-1,11,10,9},
    {-1,-1,-1,0,-1,3,2,1,-1,6,5,4,-1,9,8,7},
    {-1,-1,1,0,-1,4,3,2,-1,7,6,5,-1,10,9,8},
    {-1,2,1,0,-1,5,4,3,-1,8,7,6,-1,11,10,9},
    {3,2,1,0,-1,6,5,4,-1,9,8,7,-1,12,11,10},
    {-1,-1,-1,0,4,3,2,1,-1,7,6,5,-1,10,9,8},
    {-1,-1,1,0,5,4,3,2,-1,8,7,6,-1,11,10,9},
    {-1,2,1,0,6,5,4,3,-1,9,8,7,-1,12,11,10},
    {3,2,1,0,7,6,5,4,-1,10,9,8,-1,13,12,11},
    {-1,-1,-1,0,-1,-1,-1,1,5,4,3,2,-1,8,7,6},
    {-1,-1,1,0,-1,-1,-1,2,6,5,4,3,-1,9,8,7},
    {-1,2,1,0,-1,-1,-1,3,7,6,5,4,-1,10,9,8},
    {3,2,1,0,-1,-1,-1,4,8,7,6,5,-1,11,10,9},
    {-1,-1,-1,0,-1,-1,2,1,6,5,4,3,-1,9,8,7},
    {-1,-1,1,0,-1,-1,3,2,7,6,5,4,-1,10,9,8},
    {-1,2,1,0,-1,-1,4,3,8,7,6,5,-1,11,10,9},
    {3,2,1,0,-1,-1,5,4,9,8,7,6,-1,12,11,10},
    {-1,-1,-1,0,-1,3,2,1,7,6,5,4,-1,10,9,8},
    {-1,-1,1,0,-1,4,3,2,8,7,6,5,-1,11,10,9},
    {-1,2,1,0,-1,5,4,3,9,8,7,6,-1,12,11,10},
    {3,2,1,0,-1,6,5,4,10,9,8,7,-1,13,12,11},
    {-1,-1,-1,0,4,3,2,1,8,7,6,5,-1,11,10,9},
    {-1,-1,1,0,5,4,3,2,9,8,7,6,-1,12,11,10},
    {-1,2,1,0,6,5,4,3,10,9,8,7,-1,13,12,11},
    {3,2,1,0,7,6,5,4,11,10,9,8,-1,14,13,12},
    {-1,-1,-1,0,-1,-1,-1,1,-1,-1,-1,2,6,5,4,3},
    {-1,-1,1,0,-1,-1,-1,2,-1,-1,-1,3,7,6,5,4},
    {-1,2,1,0,-1,-1,-1,3,-1,-1,-1,4,8,7,6,5},
    {3,2,1,0,-1,-1,-1,4,-1,-1,-1,5,9,8,7,6},
    {-1,-1,-1,0,-1,-1,2,1,-1,-1,-1,3,7,6,5,4},
    {-1,-1,1,0,-1,-1,3,2,-1,-1,-1,4,8,7,6,5},
    {-1,2,1,0,-1,-1,4,3,-1,-1,-1,5,9,8,7,6},
    {3,2,1,0,-1,-1,5,4,-1,-1,-1,6,10,9,8,7},
    {-1,-1,-1,0,-1,3,2,1,-1,-1,-1,4,8,7,6,5},
    {-1,-1,1,0,-1,4,3,2,-1,-1,-1,5,9,8,7,6},
    {-1,2,1,0,-1,5,4,3,-1,-1,-1,6,10,9,8,7},
    {3,2,1,0,-1,6,5,4,-1,-1,-1,7,11,10,9,8},
    {-1,-1,-1,0,4,3,2,1,-1,-1,-1,5,9,8,7,6},
    {-1,-1,1,0,5,4,3,2,-1,-1,-1,6,10,9,8,7},
    {-1,2,1,0,6,5,4,3,-1,-1,-1,7,11,10,9,8},
    {3,2,1,0,7,6,5,4,-1,-1,-1,8,12,11,10,9},
    {-1,-1,-1,0,-1,-1,-1,1,-1,-1,3,2,7,6,5,4},
    {-1,-1,1,0,-1,-1,-1,2,-1,-1,4,3,8,7,6,5},
    {-1,2,1,0,-1,-1,-1,3,-1,-1,5,4,9,8,7,6},
    {3,2,1,0,-1,-1,-1,4,-1,-1,6,5,10,9,8,7},
    {-1,-1,-1,0,-1,-1,2,1,-1,-1,4,3,8,7,6,5},
    {-1,-1,1,0,-1,-1,3,2,-1,-1,5,4,9,8,7,6},
    {-1,2,1,0,-1,-1,4,3,-1,-1,6,5,10,9,8,7},
    {3,2,1,0,-1,-1,5,4,-1,-1,7,6,11,10,9,8},
    {-1,-1,-1,0,-1,3,2,1,-1,-1,5,4,9,8,7,6},
    {-1,-1,1,0,-1,4,3,2,-1,-1,6,5,10,9,8,7},
    {-1,2,1,0,-1,5,4,3,-1,-1,7,6,11,10,9,8},
    {3,2,1,0,-1,6,5,4,-1,-1,8,7,12,11,10,9},
    {-1,-1,-1,0,4,3,2,1,-1,-1,6,5,10,9,8,7},
    {-1,-1,1,0,5,4,3,2,-1,-1,7,6,11,10,9,8},
    {-1,2,1,0,6,5,4,3,-1,-1,8,7,12,11,10,9},
    {3,2,1,0,7,6,5,4,-1,-1,9,8,13,12,11,10},
    {-1,-1,-1,0,-1,-1,-1,1,-1,4,3,2,8,7,6,5},
    {-1,-1,1,0,-1,-1,-1,2,-1,5,4,3,9,8,7,6},
    {-1,2,1,0,-1,-1,-1,3,-1,6,5,4,10,9,8,7},
    {3,2,1,0,-1,-1,-1,4,-1,7,6,5,11,10,9,8},
    {-1,-1,-1,0,-1,-1,2,1,-1,5,4,3,9,8,7,6},
    {-1,-1,1,0,-1,-1,3,2,-1,6,5,4,10,9,8,7},
    {-1,2,1,0,-1,-1,4,3,-1,7,6,5,11,10,9,8},
    {3,2,1,0,-1,-1,5,4,-1,8,7,6,12,11,10,9},
    {-1,-1,-1,0,-1,3,2,1,-1,6,5,4,10,9,8,7},
    {-1,-1,1,0,-1,4,3,2,-1,7,6,5,11,10,9,8},
    {-1,2,1,0,-1,5,4,3,-1,8,7,6,12,11,10,9},
    {3,2,1,0,-1,6,5,4,-1,9,8,7,13,12,11,10},
    {-1,-1,-1,0,4,3,2,1,-1,7,6,5,11,10,9,8},
    {-1,-1,1,0,5,4,3,2,-1,8,7,6,12,11,10,9},
    {-1,2,1,0,6,5,4,3,-1,9,8,7,13,12,11,10},
    {3,2,1,0,7,6,5,4,-1,10,9,8,14,13,12,11},
    {-1,-1,-1,0,-1,-1,-1,1,5,4,3,2,9,8,7,6},
    {-1,-1,1,0,-1,-1,-1,2,6,5,4,3,10,9,8,7},
    {-1,2,1,0,-1,-1,-1,3,7,6,5,4,11,10,9,8},
    {3,2,1,0,-1,-1,-1,4,8,7,6,5,12,11,10,9},
    {-1,-1,-1,0,-1,-1,2,1,6,5,4,3,10,9,8,7},
    {-1,-1,1,0,-1,-1,3,2,7,6,5,4,11,10,9,8},
    {-1,2,1,0,-1,-1,4,3,8,7,6,5,12,11,10,9},
    {3,2,1,0,-1,-1,5,4,9,8,7,6,13,12,11,10},
    {-1,-1,-1,0,-1,3,2,1,7,6,5,4,11,10,9,8},
    {-1,-1,1,0,-1,4,3,2,8,7,6,5,12,11,10,9},
    {-1,2,1,0,-1,5,4,3,9,8,7,6,13,12,11,10},
    {3,2,1,0,-1,6,5,4,10,9,8,7,14,13,12,11},
    {-1,-1,-1,0,4,3,2,1,8,7,6,5,12,11,10,9},
    {-1,-1,1,0,5,4,3,2,9,8,7,6,13,12,11,10},
    {-1,2,1,0,6,5,4,3,10,9,8,7,14,13,12,11},
    {3,2,1,0,7,6,5,4,11,10,9,8,15,14,13,12}
};

static const vector128 ibm_mask_lut_128[256] = {

};

#define UTF_CHUNKSIZE 4096
#define SUPPORT_SIX_BYTE_UTF8 0

/*

    Maximum characters loaded into chunk (in case of 7-bit ASCII) is
    UTF_CHUNKSIZE
    Otherwise, the least number of characters that would be loaded into the chunk is
    UTF_CHUNKSIZE / 4
    If all characters are 4-byte characters.

*/
typedef struct utf_chunk_decoder_t {
    char8_t *data;
    uintptr_t ptr;
    size_t size;
    /* margin at the end of the buffer to avoid vector loading from garbage memory. */
    size_t end_margin;
    /* how many characters to process at once, by default 4 */
    unsigned int chars_per_group;
    /* loaded slices into the chunk */
    size_t groups_in;
    uint8_t chunkin[UTF_CHUNKSIZE];
    uint8_t chunkout[UTF_CHUNKSIZE];

} utf_chunk_decoder_t;

utf_chunk_decoder_t *
utf_chunk_decoder_create(void)
{
    utf_chunk_decoder_t *state = malloc(sizeof(*state));
    state->data = NULL;
    state->ptr = 0;
    state->size = 0;
    state->chars_per_group = 4;
    state->groups_in = 0;
    memset(state->chunkin, 0, UTF_CHUNKSIZE);
    memset(state->chunkout, 0, UTF_CHUNKSIZE);
    return state;
}

void
utf_chunk_decoder_set_chars_per_slice(utf_chunk_decoder_t *state, unsigned int n)
{
    state->chars_per_group = n;
}

void
utf_chunk_decoder_destroy(utf_chunk_decoder_t *state)
{
    free(state);
}

void
utf_chunk_decoder_load_next_chunk_utf8(utf_chunk_decoder_t *state)
{
    size_t groupsize; /* group size in bytes */
    int i;
    uintptr_t chunkptr;

    /* memset previous chunk to zero, in the case where there remains
     * data from a previous iteration
     */
    if (state->groups_in > 0) {
        memset(state->chunkin, 0, UTF_CHUNKSIZE);
        state->groups_in = 0;
    }

    chunkptr = 0;
    while (chunkptr < UTF_CHUNKSIZE) {
        uintptr_t currpos = state->ptr;

        for (i = 0; i < state->chars_per_group; ++i) {
            int8_t len = prefix_to_length_table[state->data[currpos] >> 3];
            currpos += len + 1;
        }

        groupsize = (size_t)((intptr_t)currpos - (intptr_t)state->ptr);
        if (chunkptr + groupsize <= UTF_CHUNKSIZE) {
            /* space left to memcpy another group */
            memcpy(state->chunkin + chunkptr, state->data + state->ptr, groupsize);
            state->groups_in ++;
            chunkptr += groupsize;
        } else {
            /* no space left, leave loop with extra padding */
            break;
        }
    }
}

void
decode_utf8_to_ucs2()
{}

/*
 * Reads in groups of 4 codepoints, or 16 bytes of original UTF-8 data (which can overflow)
 */
void
decode_4_chars_utf8_to_utf32(utf_chunk_decoder_t *decoder)
{

}


/*
    ((((((((0*3+3)*3+3)*3+3)*3+3)*3+3)*3+3)*3+3)*3+3)
    9840
    ((((0*3+3)*3+3)*3+3)*3+3)
    120
*/

/* Accept only 4-byte UTF-8 (21-bits),
 * decodes in chunks of 4 unicode codepoints (in a vector128).
 * https://researcher.watson.ibm.com/researcher/files/jp-INOUEHRS/IPSJPRO2008_SIMDdecoding.pdf
 */
static vector128
decode_utf8_ibm_emulate_sse1(utf_chunk_decoder_t *state)
{
    /*
        mask is the data mask for the loaded lengths,
        pattern is the permutation pattern 
    */
    register vector128 vin,vout;
    register vector128 mask,pattern;
    
    unsigned int i;
    uintptr_t pos; /* currpos from last offset */
    uint16_t lenreg; /* gathered prefix, accumulated lengths (interleaved) */
    uint8_t prefix, len;

    for (lenreg = 0, pos = 0, i = 0; i < 4; ++i) {
        prefix = state->data[state->ptr + pos] >> 3;
        len = prefix_to_length_table[prefix];
        lenreg = (lenreg<<2) | len;
        pos += len+1;
    }

    pattern = ibm_pattern_lut_128[lenreg];
    mask = ibm_mask_lut_128[lenreg];
    
    vin = load_v128(state->data + state->ptr);
    vin = shuffle_v128(vin, pattern);
    vout = and_v128(vin, mask);

    return vout;
}

static void
generate_ibm_pattern_lut_128(void)
{
    int i, j, k;

    for (i = 0; i < 256; ++i) {
        uint8_t index;
        int8_t counter;
        vector128 pattern, mask;
        pattern = loadi8_v128(-1);

        index = i;
        counter = 0; /* byte index counter */

        for (j = 0; j < 4; ++j) {
            uint8_t len = ((index>>(j<<1)) & 0x03) + 1;

            for (k = 0; k < len; ++k) {
                /* I think the following step depends on endianness
                 (big-endian & little-endian) */
                
                #if 0
                /* little-endian order */
                pattern._i8[j*4+k] = counter++;
                #else
                /* big-endian order */
                pattern._i8[j*4+(3-k)] = counter++;
                #endif
            }
        }


        /* print table entry */
        printf("{");

        for (j = 0; j < 16; ++j) {
            int8_t v = pattern._i8[j];
            if (j < 15) 
                printf("%d,",v);
            else
                printf("%d",v);
        }

         if (i < 255)
            printf("},\n");
         else 
            printf("}\n");
    }
}

static void
generate_ibm_mask_lut_128(void)
{

}

static void
generate_length_table(void)
{
    int i = 0, l;
    for (i = 0; i < 32; ++i) {
        /*
            xxx0xxxx
            xxx110xx
            xxx1110x
            xxx11110
        */
        if (~i & 0x10) {
            l = 0;
        } else if ((i & 0x1c) == 0x18) {
            l = 1;
        } else if ((i & 0x1e) == 0x1c) {
            l = 2;
        } else if ((i & 0x1f) == 0x1e) {
            l = 3;
        }

        printf("%d, ", l);
    }

    fflush(stdout);
}

int main(int argc, char *argv[])
{

    char8_t *data;
    size_t len;
    FILE *infile;
    infile = fopen("./UTF-8_sequence_separated/utf8_sequence_0-0x10ffff_assigned_printable.txt", "rb");

    if (infile == NULL) {
        fprintf(stderr, "Failed to load input file.\n");
        exit(-1);
    }

    fseek(infile,0,SEEK_END);
    len = ftell(infile);
    fseek(infile,0,SEEK_SET);

    data = malloc(len+1);
    fread(data, 1, len, infile);
    data[len] = '\0';

    /*bench_decode_chris_venter_scalar(data, len+1, 20);*/

    free(data);

    generate_ibm_pattern_lut_128();


    return 0;
}
