/* About: License
 * This file is licensed under MIT.
 */

#include "hz.h"
#include <assert.h>
#include <stdarg.h>

#define SIZEOF_VOIDPTR sizeof(void*)

#define KIB 1024


#define UTF_FAILED (-1)
#define UTF_END_OF_BUFFER (-2)
#define UTF_OK 1

// Configuration macros
#define HZ_RELY_ON_UNSAFE_CMAP_CONSTANTS HZ_TRUE
#define HZ_CONFIG_USE_SIMD HZ_FALSE
#define HZ_USE_ISO639_1_2002_TAGS HZ_FALSE

// Max depth of nested OpenType lookups
#define HZ_MAX_RECURSE_DEPTH 16

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

#if HZ_COMPILER & (HZ_COMPILER_CLANG | HZ_COMPILER_GCC)
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

typedef int16_t hz_segment_sz_t;

#if HZ_ARCH & HZ_ARCH_AVX2_BIT
#   include <immintrin.h>
#elif HZ_ARCH & HZ_ARCH_AVX_BIT
#   include <immintrin.h>
#elif HZ_ARCH & HZ_ARCH_SSE42_BIT
#   if HZ_COMPILER & HZ_COMPILER_CLANG
#   include <popcntintrin.h>
#   endif
#   include <nmmintrin.h>
#elif HZ_ARCH & HZ_ARCH_SSE41_BIT
#   include <smmintrin.h>
#elif HZ_ARCH & HZ_ARCH_SSSE3_BIT
#   include <tmmintrin.h>
#elif HZ_ARCH & HZ_ARCH_SSE3_BIT
#   include <pmmintrin.h>
#elif HZ_ARCH & HZ_ARCH_SSE2_BIT
#   include <emmintrin.h>
#elif HZ_ARCH & HZ_ARCH_NEON_BIT
// Include ARM NEON intrinsics headers
#endif

// Fast hashing intrinsics used in hash maps
#if HZ_ARCH & HZ_ARCH_AES_BIT
#   include <wmmintrin.h>
#endif

#define HZ_TAG_GPOS HZ_TAG('G','P','O','S')
#define HZ_TAG_GSUB HZ_TAG('G','S','U','B')

/* enum: hz_lookup_flags_t */
typedef enum {
    /* This bit relates only to
     * the correct processing of the cursive attachment lookup type (GPOS lookup type 3).
     * When this bit is set, the last glyph in a given sequence to
     * which the cursive attachment lookup is applied, will be positioned on the baseline.
     */
    HZ_LOOKUP_FLAG_RIGHT_TO_LEFT = 0x0001,

    /* If set, skips over base glyphs */
    HZ_LOOKUP_FLAG_IGNORE_BASE_GLYPHS = 0x0002,

    /* If set, skips over ligatures */
    HZ_LOOKUP_FLAG_IGNORE_LIGATURES = 0x0004,

    /* If set, skips over all combining marks */
    HZ_LOOKUP_FLAG_IGNORE_MARKS = 0x0008,


    /* If set, indicates that the lookup table structure is followed by a MarkFilteringSet field.
     * The layout engine skips over all mark glyphs not in the mark filtering set indicated.
     */
    HZ_LOOKUP_FLAG_USE_MARK_FILTERING_SET = 0x0010,

    /* 0x00E0 - For future use (Set to zero) */

    /* If not zero, skips over all marks of attachment type different from specified. */
    HZ_LOOKUP_FLAG_MARK_ATTACHMENT_TYPE_MASK = 0xFF00
} hz_lookup_flags_t;

/* enum: hz_value_format_flags_t */
typedef enum {
    /* Includes horizontal adjustment for placement */
    HZ_VALUE_FORMAT_X_PLACEMENT = 0x0001,

    /* Includes vertical adjustment for placement */
    HZ_VALUE_FORMAT_Y_PLACEMENT = 0x0002,

    /* Includes horizontal adjustment for advance */
    HZ_VALUE_FORMAT_X_ADVANCE = 0x0004,

    /* Includes vertical adjustment for advance */
    HZ_VALUE_FORMAT_Y_ADVANCE = 0x0008,

    /* Includes Device table (non-variable font) / VariationIndex table (variable font) for horizontal placement */
    HZ_VALUE_FORMAT_X_PLACEMENT_DEVICE = 0x0010,

    /* Includes Device table (non-variable font) / VariationIndex table (variable font) for vertical placement */
    HZ_VALUE_FORMAT_Y_PLACEMENT_DEVICE = 0x0020,

    /* Includes Device table (non-variable font) / VariationIndex table (variable font) for horizontal advance */
    HZ_VALUE_FORMAT_X_ADVANCE_DEVICE = 0x0040,

    /* Includes Device table (non-variable font) / VariationIndex table (variable font) for vertical advance */
    HZ_VALUE_FORMAT_Y_ADVANCE_DEVICE = 0x0080
} hz_value_format_flags_t;

/* enum: hz_gsub_lookup_type_t */
typedef enum {
    HZ_GSUB_LOOKUP_TYPE_SINGLE_SUBSTITUTION = 1,
    HZ_GSUB_LOOKUP_TYPE_MULTIPLE_SUBSTITUTION = 2,
    HZ_GSUB_LOOKUP_TYPE_ALTERNATE_SUBSTITUTION = 3,
    HZ_GSUB_LOOKUP_TYPE_LIGATURE_SUBSTITUTION = 4,
    HZ_GSUB_LOOKUP_TYPE_CONTEXTUAL_SUBSTITUTION = 5,
    HZ_GSUB_LOOKUP_TYPE_CHAINED_CONTEXTS_SUBSTITUTION = 6,
    HZ_GSUB_LOOKUP_TYPE_EXTENSION_SUBSTITUTION = 7,
    HZ_GSUB_LOOKUP_TYPE_REVERSE_CHAINING_CONTEXTUAL_SINGLE_SUBSTITUTION = 8
} hz_gsub_lookup_type_t;

/* enum: hz_gpos_lookup_type_t */
typedef enum {
    HZ_GPOS_LOOKUP_TYPE_SINGLE_ADJUSTMENT = 1,
    HZ_GPOS_LOOKUP_TYPE_PAIR_ADJUSTMENT = 2,
    HZ_GPOS_LOOKUP_TYPE_CURSIVE_ATTACHMENT = 3,
    HZ_GPOS_LOOKUP_TYPE_MARK_TO_BASE_ATTACHMENT = 4,
    HZ_GPOS_LOOKUP_TYPE_MARK_TO_LIGATURE_ATTACHMENT = 5,
    HZ_GPOS_LOOKUP_TYPE_MARK_TO_MARK_ATTACHMENT = 6,
    HZ_GPOS_LOOKUP_TYPE_CONTEXT_POSITIONING = 7,
    HZ_GPOS_LOOKUP_TYPE_CHAINED_CONTEXT_POSITIONING = 8,
    HZ_GPOS_LOOKUP_TYPE_EXTENSION_POSITIONING = 9
} hz_gpos_lookup_type_t;

#define HZ_IGNORE_ARG(x) (void)(x)

#define UTF_CACHE_LINE_SIZE 128
#define UTF_TINY_CHUNK_SIZE (1 * KIB)
#define UTF_SMALL_CHUNK_SIZE (4 * KIB)
#define UTF_MEDIUM_CHUNK_SIZE (16 * KIB)
#define UTF_LARGE_CHUNK_SIZE (64 * KIB)

typedef uint8_t char8_t;
typedef uint32_t char32_t;

typedef uint16_t Offset16;
typedef uint32_t Offset32;
typedef uint32_t Version16Dot16;
typedef uint16_t F2DOT14, FWORD, UFWORD;


#define DEBUG_MSG(msg) fprintf(stdout,"[DEBUG:%s:%d]: %s\n",__FILE__,__LINE__,msg)
#define ERROR_MSG(msg) fprintf(stderr,"[ERROR: %s:%d]: %s\n",__FILE__,__LINE__,msg)
#define QUIT_MSG(msg) { ERROR_MSG(msg); exit(-1); }
#define ASSERT_MSG(cond, msg) assert((cond) && message)



void *hz_allocate(hz_allocator_t *allocator, size_t size)
{
    return allocator->allocate(allocator->user, HZ_CMD_ALLOC, NULL, size, 1);
}

void hz_deallocate(hz_allocator_t *allocator, void *pointer)
{
    allocator->allocate(allocator->user, HZ_CMD_DEALLOC, pointer, 0, 0);
}

void *hz_reallocate(hz_allocator_t *allocator, void *pointer, size_t new_size)
{
    return allocator->allocate(allocator->user, HZ_CMD_REALLOC, pointer, new_size, 1);
}

void hz_release(hz_allocator_t *allocator)
{
    allocator->allocate(allocator->user, HZ_CMD_RELEASE,NULL,0,0);
}

//////////////////////////////// main allocator ////////////////////////////////////////
void *hz_standard_c_allocate_func(void *user, hz_allocation_cmd_t cmd, void *pointer, size_t size, size_t alignment)
{
    HZ_IGNORE_ARG(user); HZ_IGNORE_ARG(alignment);
    
    switch (cmd) {
        case HZ_CMD_ALLOC:
            return malloc(size);
        case HZ_CMD_REALLOC: // no-op
            return realloc(pointer, size);
        case HZ_CMD_DEALLOC:
            free(pointer);
        case HZ_CMD_RELEASE:    
        default: // error, cmd not handled
            return NULL;
    }
}

static hz_allocator_t _allocator = { .allocate = hz_standard_c_allocate_func, .user = NULL };

void *hz_malloc(size_t size)
{
    return _allocator.allocate(_allocator.user,HZ_CMD_ALLOC,NULL,size,1);
}

void *hz_realloc(void *pointer, size_t size)
{
    return _allocator.allocate(_allocator.user,HZ_CMD_REALLOC,pointer,size,1);
}

void hz_free(void *pointer)
{
    _allocator.allocate(_allocator.user,HZ_CMD_DEALLOC,pointer,0,1);
}

// void hz_override_lib_allocator(hz_allocator_t allocator)
// {
//     _allocator = allocator;
// }

HZ_STATIC uint16_t
bswap16(uint16_t x)
{
    uint16_t v = 0;
    v |= (x & 0x00ff) << 8;
    v |= (x & 0xff00) >> 8;
    return v;
}

HZ_STATIC uint32_t
bswap32(uint32_t x)
{
    uint32_t v = 0;
    v |= (x & 0x000000ff) << 24;
    v |= (x & 0x0000ff00) << 8;
    v |= (x & 0x00ff0000) >> 8;
    v |= (x & 0xff000000) >> 24;
    return v;
}

HZ_STATIC uint64_t
bswap64(uint64_t x)
{
    uint64_t v = 0;
    v |= (x & 0x00000000000000ff) << 56;
    v |= (x & 0x000000000000ff00) << 40;
    v |= (x & 0x0000000000ff0000) << 24;
    v |= (x & 0x00000000ff000000) << 8;
    v |= (x & 0x000000ff00000000) >> 8;
    v |= (x & 0x0000ff0000000000) >> 24;
    v |= (x & 0x00ff000000000000) >> 40;
    v |= (x & 0xff00000000000000) >> 56;
    return v;
}


/*
    Read this article for better solutions implementing 64-bit log2:
    http://graphics.stanford.edu/~seander/bithacks.html
*/
uint64_t
fastlog2l(uint64_t n)
{
    #if HZ_COMPILER & (HZ_COMPILER_GCC | HZ_COMPILER_CLANG)
    return 63 - __builtin_clzl(n);
    #else
    static const int multiply_de_bruijin_bit_position[64] = {
        63,  0, 58,  1, 59, 47, 53,  2,
        60, 39, 48, 27, 54, 33, 42,  3,
        61, 51, 37, 40, 49, 18, 28, 20,
        55, 30, 34, 11, 43, 14, 22,  4,
        62, 57, 46, 52, 38, 26, 32, 41,
        50, 36, 17, 19, 29, 10, 13, 21,
        56, 45, 25, 31, 35, 16,  9, 12,
        44, 24, 15,  8, 23,  7,  6,  5
    };
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
    return multiply_de_bruijin_bit_position[((uint64_t)((n - (n >> 1))*0x07EDD5E59A4E28C2)) >> 58];
    #endif
}

#define LINEAR_ALLOCATOR_MAX_ALIGNED_SIZE 512
#define LINEAR_ALLOCATOR_ALIGN_TO_POWER_OF_TWO_BOUNDARY 1

typedef struct {
    uint8_t *data;
    size_t size;
    uintptr_t offset;
    int alignment;
} hz_memory_arena_t;

static hz_memory_arena_t hz_memory_arena_create(void *mem, size_t size)
{
    hz_memory_arena_t arena;
    arena.data = mem;
    arena.size = size;
    arena.offset = 0;
    arena.alignment = 8;
    return arena;
}

static uint64_t Minll(uint64_t x, uint64_t y)
{
    return x<y?x:y;
}

static int hz_check_power_of_two(uint64_t v)
{
    return v && ~(v & (v-1));
}

static int Ctzll(uint64_t x)
{
#if HZ_COMPILER & (HZ_COMPILER_GCC | HZ_COMPILER_CLANG)
    return __builtin_ctzll(x);
#else
    return _tzcnt_u64(x);
#endif
}

static uint64_t Ftzll(uint64_t x)
{
    return (1 << Ctzll(x)) - 1;
}

static uint64_t Align(uint64_t x, uint64_t n)
{
    // Aligns on largest power of two divisor of allocation size
    assert(hz_check_power_of_two(n)); // n must be a power of two
    uint64_t m = (x && n) ? Ftzll(n) & 0x1f : 0;
    return x + ((~x + 1) & m);
}

// Assume alignment is a power of two
uint64_t compute_memory_alignment(uint64_t memptr, uint64_t alignment)
{
    return (~memptr + 1) & Ftzll(alignment);
}

// The following is a linear allocation function optimized for allocation of integers and integer arrays
// since this is what it's mostly used for realistically in the code.
static void* hz_memory_arena_allocate(hz_memory_arena_t *arena, size_t size)
{
    if (size > 0 && size < arena->size) {
        uint64_t p = arena->offset;

        p = Align(p,size);

        if (p + size <= arena->size) {
            arena->offset = p + size;
            return arena->data + p;
        }
    }

    return NULL;
}

static void hz_memory_arena_reset(hz_memory_arena_t *arena)
{
    arena->offset = 0;
}


typedef struct {
    void *prev;
    uint64_t cursor;
} hz_chunk_t;

typedef struct {
    size_t chunk_size;
    size_t chunk_capacity;
    hz_chunk_t *curr_chunk;
} hz_memory_pool_t;

hz_memory_pool_t hz_memory_pool_create(size_t chunk_size)
{
    hz_memory_pool_t pool;
    pool.chunk_size = chunk_size;
    pool.chunk_capacity = chunk_size - sizeof(hz_chunk_t);
    pool.curr_chunk = NULL;
    return pool;
}

hz_chunk_t *hz_memory_pool_create_chunk(size_t size)
{
    hz_chunk_t* chunk = hz_malloc(size);
    chunk->prev = NULL;
    chunk->cursor = 0;
    return chunk;
}

// alignment is absolute
void* hz_memory_pool_allocate_aligned(hz_memory_pool_t *pool, size_t size, size_t alignment)
{
    HZ_ASSERT(hz_check_power_of_two(alignment));
    
    // Requested size is larger than the chunk's capacity, cannot allocate.
    if (size > pool->chunk_capacity) {
        return NULL;
    }

    // Check if first chunk exists, otherwise allocate it.
    if (pool->curr_chunk == NULL) {
        pool->curr_chunk = hz_memory_pool_create_chunk(pool->chunk_size);
    }
    
    // Not enough room in chunk, allocate other chunk:
    if (pool->curr_chunk->cursor + size > pool->chunk_capacity) {
        hz_chunk_t *chunk = hz_memory_pool_create_chunk(pool->chunk_size);
        chunk->prev = pool->curr_chunk;
        pool->curr_chunk = chunk;
    }

    uint8_t *mem = (uint8_t *)(pool->curr_chunk + 1) + pool->curr_chunk->cursor;
    uint64_t align = MAX(alignment,1);
    uint64_t memptr = (uint64_t)mem;
    uint64_t alignment_offset = compute_memory_alignment(memptr, align);
    pool->curr_chunk->cursor += size + alignment_offset;
    return mem + alignment_offset;
}

void* hz_memory_pool_allocate(hz_memory_pool_t *pool, size_t size)
{
    hz_memory_pool_allocate_aligned(pool,size,1);
}

void hz_memory_pool_release(hz_memory_pool_t *pool) {
    hz_chunk_t *chunk = pool->curr_chunk;

    while(chunk != NULL) {
        hz_chunk_t *prev_chunk = chunk->prev;
        hz_free(chunk);
        chunk = prev_chunk;
    }
}

void *hz_memory_pool_allocate_func(void *user, hz_allocation_cmd_t cmd, void *pointer, size_t size, size_t alignment)
{
    hz_memory_pool_t *pool = (hz_memory_pool_t *)user;

    switch (cmd) {
        case HZ_CMD_ALLOC:
            return hz_memory_pool_allocate_aligned(pool, size, alignment);
        case HZ_CMD_RELEASE:
            hz_memory_pool_release(pool);
        case HZ_CMD_DEALLOC: case HZ_CMD_REALLOC: // no-op
        default: // error, cmd not handled
            return NULL;
    }
}

/* no bound for buffer */
#define BNDCHECK 0x00000001

/*
 *   Struct: hz_stream_t
 *
 *       A struct to store a buffer.
 *      This is also used by <hz_deserializer_t> to read off bytes.
 *
 *   See Also:
 *      <hz_deserializer_t>
 */
typedef struct {
    const uint8_t *data;
    size_t size;
    uintptr_t ptr;
    uint8_t flags;
} hz_stream_t;

/*
    Function: hz_stream_create

        Creates a byte stream, holds data and
        a bump pointer allowing seeking, unpacking various primitives from memory.

    Parameters:
        data - The pointer to the memory.
        size - The size of the block (If this is 0, then there's no bound check).

    Returns:
        A <hz_stream_t> instance.
*/
HZ_STATIC hz_stream_t
hz_stream_create(const uint8_t *data, size_t size)
{
    hz_stream_t bs;
    bs.data = data;
    bs.size = size;
    bs.ptr = 0;
    bs.flags = 0;

    if (data != NULL && size > 0)
        bs.flags |= BNDCHECK;

    return bs;
}

HZ_STATIC void
stream_seek(hz_stream_t *stream, uintptr_t pos)
{
    stream->ptr = pos;
}

HZ_STATIC uintptr_t
stream_tell(hz_stream_t *stream)
{
    return stream->ptr;
}

/* checks processor endianness, returns 0 if big-endian (be), 1 if little-endian (le) */
static int check_cpu_le(void)
{
    static const int x = 1;
    return (int)(*(unsigned char *)&x);
}

#define CPU_LITTLE_ENDIAN() (check_cpu_le())
#define CPU_BIG_ENDIAN() (!check_cpu_le())

HZ_STATIC void UnpackArray16(hz_stream_t *stream, size_t count, uint16_t *dest)
{
    if (count > 0) {
        if (check_cpu_le()) {
            // TODO: optimize using SSE/AVX
            for (size_t i = 0; i < count; ++i) {
                uint16_t val = *(uint16_t *)(stream->data + stream->ptr);
                *dest = bswap16(val);
                stream->ptr += 2;
                ++dest;
            }
        } else {
            memcpy(dest, stream->data + stream->ptr, count * 2);
            stream->ptr += count * 2;
        }
    }
}

HZ_STATIC void UnpackArray32(hz_stream_t *stream, size_t count, uint32_t *dest)
{
    if (count > 0) {
        if (check_cpu_le()) {
            // TODO: optimize using SSE/AVX
            for (size_t i = 0; i < count; ++i) {
                uint32_t val = *(uint16_t *)(stream->data + stream->ptr);
                *dest = bswap32(val);
                stream->ptr += 4;
                ++dest;
            }
        } else {
            memcpy(dest, stream->data + stream->ptr, count * 4);
            stream->ptr += count * 4;
        }
    }
}

HZ_STATIC uint8_t
Unpack8(hz_stream_t *bs)
{
    return *(bs->data + bs->ptr++);
}

HZ_STATIC uint16_t
Unpack16(hz_stream_t *bs)
{
    uint16_t val = 0;

    val |= (uint16_t) bs->data[bs->ptr+0] << 8;
    val |= (uint16_t) bs->data[bs->ptr+1];

    bs->ptr += 2;
    return val;
}


HZ_STATIC uint32_t Unpack32(hz_stream_t *bs)
{
    uint32_t val = 0;

    val |= (uint32_t) bs->data[bs->ptr+0] << 24;
    val |= (uint32_t) bs->data[bs->ptr+1] << 16;
    val |= (uint32_t) bs->data[bs->ptr+2] << 8;
    val |= (uint32_t) bs->data[bs->ptr+3];

    bs->ptr += 4;
    return val;
}

HZ_STATIC uint64_t
unpack64(hz_stream_t *bs)
{
    uint64_t val = 0;

    val |= (uint64_t) bs->data[bs->ptr+0] << 56;
    val |= (uint64_t) bs->data[bs->ptr+1] << 48;
    val |= (uint64_t) bs->data[bs->ptr+2] << 40;
    val |= (uint64_t) bs->data[bs->ptr+3] << 32;
    val |= (uint64_t) bs->data[bs->ptr+4] << 24;
    val |= (uint64_t) bs->data[bs->ptr+5] << 16;
    val |= (uint64_t) bs->data[bs->ptr+6] << 8;
    val |= (uint64_t) bs->data[bs->ptr+7];

    bs->ptr += 8;
    return val;
}

HZ_STATIC char
peek_next(const char *p)
{
    return p[1];
}

HZ_STATIC void
Unpackv(hz_stream_t *bs, const char *f, ...)
{
    va_list ap;
    const char *c;

    if (bs == NULL || f == NULL) {
        DEBUG_MSG("byte stream or format string NULL!");
        return;
    }

    va_start(ap,f);

    for (c = f; *c != '\0'; ++c) {
        switch (*c) {
            case 'b': { /* byte (8-bit unsigned) */
                uint8_t *val = va_arg(ap, uint8_t*);

                if (peek_next(c) == ':') {
                    ++c;
                    if (peek_next(c) == '*') {
                        int i, n;
                        n = va_arg(ap, int);
                        for (i = 0; i < n; ++i)
                            val[i] = Unpack8(bs);
                    }
                } else
                    *val = Unpack8(bs);

                break;
            }
            case 'h': { /* half (short, 16-bit unsigned */
                uint16_t *val = va_arg(ap, uint16_t*);

                if (peek_next(c) == ':') {
                    ++c;
                    if (peek_next(c) == '*') {
                        int i, n;
                        n = va_arg(ap, int);
                        for (i = 0; i < n; ++i)
                            val[i] = Unpack16(bs);
                    }
                } else
                    *val = Unpack16(bs);

                break;
            }
            case 'i': { /* int (32-bit unsigned) */
                uint32_t *val = va_arg(ap, uint32_t*);

                if (peek_next(c) == ':') {
                    ++c;
                    if (peek_next(c) == '*') {
                        int i, n;
                        n = va_arg(ap, int);
                        for (i = 0; i < n; ++i)
                            val[i] = Unpack32(bs);
                    }
                } else {
                    *val = Unpack32(bs);
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
            default:
                break;
        }
    }

    va_end(ap);
}

typedef struct { uint32_t jump, prev_offset; } hz_deserializer_state_t;

typedef struct {
    const uint8_t *mem;
    int network_order;
    int bswap_required;
    size_t start;
    size_t offset;
    hz_vector(hz_deserializer_state_t) state;
} hz_deserializer_t;

hz_deserializer_t hz_deserializer_create(const uint8_t *mem, int network_order)
{
    hz_deserializer_t self;
    self.mem = mem;
    self.network_order = network_order;
    self.bswap_required = (network_order && check_cpu_le()) || (!network_order && !check_cpu_le());
    self.state = NULL;
    self.offset = 0;
    self.start = 0;
    return self;
}

hz_deserializer_state_t *hz_deserializer_last_state(hz_deserializer_t *self)
{
    return &self->state[hz_vector_size(self->state)-1];
}

void hz_deserializer_pop_state(hz_deserializer_t *self)
{
    hz_deserializer_state_t *state = hz_deserializer_last_state(self);
    self->offset = state->prev_offset;
    self->start -= state->jump;
    if (hz_vector_size(self->state) > 1) {
        hz_vector_pop(self->state);
    } else {
        hz_vector_destroy(self->state);
    }
}

void hz_deserializer_push_state(hz_deserializer_t *self, uint32_t jump)
{
    hz_deserializer_state_t save;
    save.jump = jump;
    save.prev_offset = self->offset;
    hz_vector_push_back(self->state, save);
    self->start += jump;
    self->offset = 0;
}

static inline void hz_deserializer_advance(hz_deserializer_t *self, size_t count)
{
    self->offset += count;
}

static inline const uint8_t* hz_deserializer_at_cursor(hz_deserializer_t *self)
{
    return self->mem + self->start + self->offset;
}

uint8_t hz_deserializer_read_u8(hz_deserializer_t *self)
{
    uint8_t v = *(uint8_t *)hz_deserializer_at_cursor(self);
    hz_deserializer_advance(self, 1);
    return v;
}

uint16_t hz_deserializer_read_u16(hz_deserializer_t *self)
{
    uint16_t v = *(uint16_t *)hz_deserializer_at_cursor(self);
    hz_deserializer_advance(self, 2);   
    return self->bswap_required ? bswap16(v) : v;
}

uint32_t hz_deserializer_read_u32(hz_deserializer_t *self)
{
    uint32_t v = *(uint32_t *)hz_deserializer_at_cursor(self);
    hz_deserializer_advance(self, 4);
    return self->bswap_required ? bswap32(v) : v;
}

uint64_t hz_deserializer_read_u64(hz_deserializer_t *self)
{
    uint64_t v = *(uint64_t *)hz_deserializer_at_cursor(self);
    hz_deserializer_advance(self, 8);
    return self->bswap_required ? bswap64(v) : v;
}

void hz_deserializer_read_block(hz_deserializer_t *self, uint8_t *write_addr, size_t size)
{
    memcpy(write_addr, hz_deserializer_at_cursor(self), size);
    hz_deserializer_advance(self, size);
}

void hz_deserializer_read_u16_block(hz_deserializer_t *self, uint16_t *write_addr, size_t size)
{
    while (size > 0) {
        *write_addr++ = hz_deserializer_read_u16(self);
        --size;
    }
}

void hz_deserializer_read_u32_block(hz_deserializer_t *self, uint32_t *write_addr, size_t size)
{
    while (size > 0) {
        *write_addr++ = hz_deserializer_read_u32(self);
        --size;
    }
}

void hz_deserializer_read_u64_block(hz_deserializer_t *self, uint64_t *write_addr, size_t size)
{
    while (size > 0) {
        *write_addr++ = hz_deserializer_read_u64(self);
        --size;
    }
}

int cmdread(hz_deserializer_t *self, int c_struct_align, void *dataptr, const char *cmd, ...)
{
    va_list va; va_start(va, cmd);

    const char *curs = cmd;
    uint8_t *mem = (uint8_t *)dataptr;
    size_t member_offset = 0;
    size_t member_array_count = 1;

    while (*curs != '\0') {
        switch (*curs) {
            case '^': {
                ++curs; // skip '^'
                member_array_count = 0;
                while (*curs >= '0' && *curs <= '9') {
                    member_array_count *= 10;
                    member_array_count += *curs - '0';
                    ++curs;
                }

                break;
            }
            case 'b': {
                ++curs;
                hz_deserializer_read_block(self, (uint8_t *)mem, member_array_count);
                member_offset += member_array_count;
                member_array_count = 1;
                break;
            }

            case 'w': {
                ++curs;
                member_offset += compute_memory_alignment(member_offset, c_struct_align ? 2 * member_array_count : 1);
                hz_deserializer_read_u16_block(self,
                            (uint16_t *)(mem + member_offset),
                            member_array_count);
                member_offset += 2 * member_array_count;
                member_array_count = 1;
                break;
            }

            case 'd': {
                ++curs;
                member_offset += compute_memory_alignment(member_offset, c_struct_align ? 4 * member_array_count : 1);
                hz_deserializer_read_u32_block(self,
                            (uint32_t *)(mem + member_offset),
                            member_array_count);
                member_offset += 4 * member_array_count;
                member_array_count = 1;
                break;
            }

            case 'q': {
                ++curs;
                member_offset += compute_memory_alignment(member_offset, c_struct_align ? 8 * member_array_count : 1);
                hz_deserializer_read_u64_block(self,
                            (uint64_t *)(mem + member_offset),
                            member_array_count);
                member_offset += 8 * member_array_count;
                member_array_count = 1;
                break;
            }

            case ' ': default: {
                ++curs;
                break;
            }
        }
    }

    va_end(va);
}

typedef struct hz_array_t {
    size_t size;
    uint32_t *data;
} hz_array_t;

hz_array_t * hz_array_create(void)
{
    hz_array_t *array = hz_malloc(sizeof(*array));
    array->data = NULL;
    array->size = 0;
    return array;
}

void hz_array_append(hz_array_t *array, uint32_t val)
{
    size_t new_size = array->size + 1;

    if (array->data == NULL)
        array->data = hz_malloc(new_size * sizeof(uint32_t));
    else
        array->data = hz_realloc(array->data, new_size * sizeof(uint32_t));

    array->data[new_size - 1] = val;
    array->size = new_size;
}

size_t hz_array_size(const hz_array_t *array)
{
    return array->size;
}

hz_bool_t hz_array_is_empty(const hz_array_t *array)
{
    return array->size == 0;
}

void hz_array_clear(hz_array_t *array)
{
    if (!hz_array_is_empty(array)) {
        hz_free(array->data);
        array->data = NULL;
        array->size = 0;
    }
}

void
hz_array_resize(hz_array_t *array, size_t new_size)
{
    hz_bool_t is_empty = hz_array_is_empty(array);
    if (new_size == 0 && !is_empty) {
        hz_free(array->data);
        array->data = NULL;
    } else {
        if (is_empty)
            array->data = hz_malloc(new_size * sizeof(uint32_t));
        else
            array->data = hz_realloc(array->data, new_size * sizeof(uint32_t));
    }

    array->size = new_size;
}

uint32_t hz_array_pop(hz_array_t *array)
{
    if (array->size >= 1 && !hz_array_is_empty(array))  {
        uint32_t val = array->data[array->size-1];
        hz_array_resize(array, array->size-1);
        return val;
    }

    return 0;
}


void
hz_array_pop_at(hz_array_t *array, size_t index)
{

}

void
hz_array_pop_range_at(hz_array_t *array, size_t index, size_t count)
{
    size_t new_size = array->size - count;

    size_t remaining_len = array->size - (index + count);
    uint32_t *remaining = hz_malloc(remaining_len * sizeof(uint32_t));
    memcpy(remaining, &array->data[index + count], remaining_len * sizeof(uint32_t));

    array->data = hz_realloc(array->data, new_size * sizeof(uint32_t));
    array->size = new_size;

    memcpy(&array->data[index], remaining, remaining_len * sizeof(uint32_t));
    hz_free(remaining);
}

void
hz_array_insert(hz_array_t *array, size_t index, uint32_t val)
{
    if (index > array->size) return;

    if (index == array->size) {
        hz_array_append(array, val);
    } else {
        size_t new_size = array->size + 1;

        size_t sec_len = array->size - (index + 1);
        uint32_t *sec = hz_malloc(sec_len * sizeof(uint32_t));
        memcpy(sec, &array->data[index], sec_len);

        array->data = hz_realloc(array->data, new_size * sizeof(uint32_t));
        array->size = new_size;
        memcpy(&array->data[index + 1], sec, sec_len);
        array->data[index] = val;

        hz_free(sec);
    }
}

void
hz_array_destroy(hz_array_t *array)
{
    hz_free(array->data);
    hz_free(array);
}

uint32_t
hz_array_at(const hz_array_t *array, size_t index)
{
    return array->data[index];
}

hz_bool_t
hz_array_has(hz_array_t *array, uint32_t val, size_t *val_idx)
{
    size_t index = 0;
    while (index < array->size) {
        if (array->data[index] == val) {
            if (val_idx != NULL)
                *val_idx = index;
            return HZ_TRUE;
        }
        ++index;
    }

    return HZ_FALSE;
}

void
hz_array_set(hz_array_t *array, size_t index, uint32_t val)
{
    array->data[index] = val;
}

hz_bool_t
hz_array_range_eq(const hz_array_t *a_arr, size_t a_index,
                  const hz_array_t *b_arr, size_t b_index,
                  size_t len)
{
    size_t offset = 0;
    HZ_ASSERT(a_arr != NULL && b_arr != NULL);
    HZ_ASSERT(len > 0);

    if(a_index + len > a_arr->size) return HZ_FALSE;
    if(b_index + len > b_arr->size) return HZ_FALSE;

    while (offset < len) {
        if (a_arr->data[a_index + offset] != b_arr->data[b_index + offset]) return HZ_FALSE;
        ++offset;
    }

    return HZ_TRUE;
}


#define FNV_OFFSET_BASIS_32 0x811c9dc5
#define FNV_PRIME_32 0x01000193

HZ_STATIC uint32_t
hash_fnv1a(uint32_t val)
{
    uint32_t hash = FNV_OFFSET_BASIS_32;

    while (val) {
        hash ^= val & 0xff;
        hash *= FNV_PRIME_32;
        val >>= 8;
    }

    return hash;
}

typedef struct hz_map_bucket_node_t {
    struct hz_map_bucket_node_t *prev, *next;
    uint32_t key;
    uint32_t value;
} hz_map_bucket_node_t;

typedef struct {
    struct hz_map_bucket_node_t *root;
} hz_map_bucket_t;

static void
hz_map_bucket_init(hz_map_bucket_t *b)
{
    b->root = NULL;
}

typedef struct {
    hz_map_bucket_t *buckets;
    size_t bucket_count;
} hz_map_t;

HZ_STATIC hz_map_t *
hz_map_create(void)
{
    size_t i;
    hz_map_t *map;

    map = hz_malloc(sizeof(hz_map_t));
    map->bucket_count = 64;
    map->buckets = hz_malloc(sizeof(hz_map_bucket_t) * map->bucket_count);

    for (i = 0; i < map->bucket_count; ++i)
        hz_map_bucket_init(&map->buckets[i]);

    return map;
}

// Counts the number of elements in the hash map
HZ_STATIC uint32_t hz_map_size(hz_map_t *map)
{
    uint32_t result = 0;
    for (uint32_t i = 0; i < map->bucket_count; ++i) {
        hz_map_bucket_t *bucket = &map->buckets[i];
        if (bucket != NULL) {
            hz_map_bucket_node_t *node = bucket->root;
            while (node != NULL) {
                ++result;
                node = node->next;
            }
        }
    }

    return result;
}

HZ_STATIC void
hz_map_destroy(hz_map_t *map)
{
    size_t i;

    for (i = 0; i < map->bucket_count; ++i) {
        hz_map_bucket_t *bucket = &map->buckets[i];
        if (bucket != NULL) {
            hz_map_bucket_node_t *node = bucket->root;

            while (node != NULL) {
                hz_map_bucket_node_t *tmp = node;
                node = node->next;
                hz_free(tmp);
            }

            bucket->root = NULL;
        }
    }

    hz_free(map->buckets);
    hz_free(map);
}

/* Returns true if value exists, and false if it didn't */
HZ_STATIC hz_bool_t
hz_map_set_value(hz_map_t *map, uint32_t key, uint32_t value)
{
    uint32_t hash;
    hz_map_bucket_t *bucket;

    hash = hash_fnv1a(key);
    bucket = &map->buckets[hash % map->bucket_count];

    if (bucket->root == NULL) {
        hz_map_bucket_node_t *new_node = hz_malloc(sizeof(hz_map_bucket_node_t));
        new_node->prev = NULL;
        new_node->next = NULL;
        new_node->value = value;
        new_node->key = key;
        bucket->root = new_node;
    } else {
        /* loop over nodes, if one with equal key is found set value, otherwise insert node */
        hz_map_bucket_node_t *curr_node, *new_node;

        for (curr_node = bucket->root; curr_node->next != NULL; curr_node = curr_node->next) {
            if (curr_node->key == key) {
                curr_node->value = value;
                return HZ_TRUE;
            }
        }

        new_node = hz_malloc(sizeof(hz_map_bucket_node_t));
        new_node->prev = curr_node;
        new_node->value = value;
        new_node->key = key;
        new_node->next = NULL;
        curr_node->next = new_node;
    }

    return HZ_FALSE;
}

HZ_STATIC uint32_t
hz_map_get_value(hz_map_t *map, uint32_t key)
{
    uint32_t hash = hash_fnv1a(key);
    size_t index = hash % map->bucket_count;
    hz_map_bucket_t *bucket = &map->buckets[index];

    if (bucket->root != NULL) {
        hz_map_bucket_node_t *curr_node = bucket->root;

        /* if only single node is the root of bucket, no need to compare keys  */
        while (curr_node != NULL) {
            if (curr_node->key == key) {
                return curr_node->value;
            }

            curr_node = curr_node->next;
        }
    }

    return 0;
}

HZ_STATIC void
hz_map_remove(hz_map_t *map, uint32_t key)
{

}

HZ_STATIC hz_bool_t
hz_map_contains(const hz_map_t *map, uint32_t key)
{
    if (map == NULL) return HZ_FALSE;

    uint32_t hash = hash_fnv1a(key);
    size_t index = hash % map->bucket_count;
    hz_map_bucket_t *bucket = &map->buckets[index];

    if (bucket->root != NULL) {
        hz_map_bucket_node_t *curr_node = bucket->root;

        do {
            if (curr_node->key == key) {
                return HZ_TRUE;
            }

            curr_node = curr_node->next;
        } while (curr_node != NULL);
    }

    return HZ_FALSE;
}


HZ_STATIC hz_bool_t
hz_map_set_value_for_keys(hz_map_t *map, uint32_t k0, uint32_t k1, uint32_t value)
{
    hz_bool_t any_set = HZ_FALSE;
    uint32_t k = k0;

    while (k <= k1) {
        if (hz_map_set_value(map, k, value))
            any_set = HZ_TRUE;

        ++k;
    }

    return any_set;
}

typedef struct hz_map_iterator_t {
    uint32_t index;
    hz_map_bucket_node_t *node;
    uint32_t key, value;
} hz_map_iterator_t;


HZ_STATIC void hz_map_iterator_next(hz_map_t *map, hz_map_iterator_t *it)
{
    if (it->node == NULL) {
        thing:
        ++it->index;
        while (it->index < map->bucket_count) {
            if (map->buckets[it->index].root != NULL) {
                // Found non-empty bucket
                it->node = map->buckets[it->index].root;
                it->key = it->node->key;
                it->value = it->node->value;
                return;
            }

            ++it->index;
        }
    } else {
        it->node = it->node->next;

        if (it->node != NULL) {
            it->key = it->node->key;
            it->value = it->node->value;
        } else {
            goto thing;
        }
    }
}

HZ_STATIC hz_map_iterator_t hz_map_begin(hz_map_t *map)
{
    hz_map_iterator_t it = {.node = NULL, .index = 0};

    while (map->buckets[it.index].root == NULL && it.index < map->bucket_count) {
        ++it.index;
    }

    if (it.index < map->bucket_count) {
        // Found non-empty bucket
        it.node = map->buckets[it.index].root;
        it.key = it.node->key;
        it.value = it.node->value;
    }

    return it;
}


// Section: Minimal Perfect Hash Table
uint32_t Hash(uint32_t d, const unsigned char *key,
              long size)
{
    if (d == 0) d = 0x01000193;

    // Use the FNV algorithm from http://isthe.com/chongo/tech/comp/fnv/
    for (long i = 0; i < size; ++i) {
        unsigned char b = key[i];
        d = ( (d * 0x01000193) ^ b );
    }

    return d;
}

uint32_t HashUInt32(uint32_t d, uint32_t x)
{
    return Hash(d, (const unsigned char *) &x, sizeof(uint32_t));
}

typedef struct hz_mph_map_t {
    hz_array_t **buckets;
    int32_t *G;
    uint32_t *values;
    uint32_t N;
} hz_mph_map_t;

HZ_STATIC int compare_arrays(const void *a, const void *b)
{
    const hz_array_t *p = *(const hz_array_t **)a;
    const hz_array_t *q = *(const hz_array_t **)b;
    int l = (int)(p->size);
    int r = (int)(q->size);
    return r-l;
}

HZ_STATIC hz_mph_map_t *hz_mph_map_create(hz_map_t *from)
{
    uint32_t size = hz_map_size(from);

    hz_mph_map_t *map = hz_malloc(sizeof(*map));
    map->N = size;
    map->buckets = hz_malloc(sizeof(*map->buckets) * size);
    map->G = hz_malloc(sizeof(*map->G) * size);
    map->values = hz_malloc(sizeof(*map->values) * size);
    hz_bool_t *value_set = hz_malloc(sizeof(*value_set) * size);

    for (uint32_t i = 0; i < size; ++i) {
        map->buckets[i] = hz_array_create();
        map->G[i] = 0;
        map->values[i] = 0;
        value_set[i] = HZ_FALSE;
    }

    int x = 0;
    for (hz_map_iterator_t it = hz_map_begin(from); it.node != NULL;
            hz_map_iterator_next(from, &it))
    {
        uint32_t h = HashUInt32(0, it.key) % size;
        hz_array_append(map->buckets[h], it.key);
        ++x;
    }

    assert(x == size);

    // Sort the buckets and process in decreasing order (largest -> smallest)
    size_t buckets_size = sizeof(*map->buckets) * size;
    hz_array_t **sorted_buckets = hz_malloc(buckets_size);
    memcpy(sorted_buckets, map->buckets, buckets_size);

    qsort((void *)sorted_buckets, size, sizeof(void *),
          compare_arrays);

    uint32_t b;
    for (b = 0; b < size; ++b) {
        hz_array_t *bucket = sorted_buckets[b];
        if (bucket->size <= 1) break;

        int32_t d = 1, item = 0;
        hz_array_t *slots = hz_array_create();

        // Repeatedly try different values of d until we find a hash function
        // that places all items in the bucket into free slots
        while (item < bucket->size) {
            uint32_t slot = HashUInt32( d, bucket->data[item] ) % size;
            if (value_set[slot] || hz_array_has(slots, slot, NULL)) {
                ++d; item = 0;
                hz_array_clear(slots);
            } else {
                hz_array_append(slots, slot);
                ++item;
            }
        }

        map->G[HashUInt32(0,bucket->data[0]) % size] = d;

        // Set values for each of the slots
        for (uint32_t i = 0; i < bucket->size; ++i) {
            map->values[slots->data[i]] = hz_map_get_value(from, bucket->data[i]);
            value_set[slots->data[i]] = HZ_TRUE;
        }

        hz_array_destroy(slots);
    }

    // Only buckets with 1 item remain. Process them more quickly by directly
    // placing them into a free slot. Use a negative value fo d to indicate this.
    hz_array_t *freelist = hz_array_create();
    for (uint32_t i = 0; i < size; ++i) {
        if (!value_set[i]) hz_array_append(freelist, i);
    }

    for (; b < size; ++b) {
        hz_array_t *bucket = sorted_buckets[b];
        if (bucket->size == 0) break;

        assert(!hz_array_is_empty(freelist));
        uint32_t slot = hz_array_pop(freelist);
        assert(slot < size);

        // We subtract one to ensure it's negative even if the zeroeth slot was
        // used.
        map->G[HashUInt32(0, bucket->data[0]) % size] = -(int32_t)slot-1;
        map->values[slot] = hz_map_get_value(from, bucket->data[0]);
        value_set[slot] = HZ_TRUE;
    }

    hz_array_destroy(freelist);
    hz_free(value_set);
    return map;
}

typedef struct hz_optimized_lib_t {
    hz_mph_map_t *arabic_joining;
} hz_optimized_lib_t;

static int oz_lib_initialized = 0;
static hz_optimized_lib_t oz_lib;

void hz_optimize(void)
{
    {
        hz_map_t *arabic_joining_tmp = hz_map_create();
        for (int i = 0; i < HZ_ARRLEN(hz_arabic_joining_list); ++i) {
            hz_map_set_value(arabic_joining_tmp, hz_arabic_joining_list[i].codepoint,
                             hz_arabic_joining_list[i].joining);
        }

        oz_lib.arabic_joining = hz_mph_map_create(arabic_joining_tmp);

        hz_map_destroy(arabic_joining_tmp);
    }

    oz_lib_initialized = 1;
}


/* Blob */
typedef struct hz_blob_t {
    uint8_t *data;
    size_t size;
} hz_blob_t;

hz_blob_t *
hz_blob_create(void)
{
    hz_blob_t *blob = hz_malloc(sizeof(hz_blob_t));
    blob->data = NULL;
    blob->size = 0;
    return blob;
}

hz_bool_t
hz_blob_is_empty(hz_blob_t *blob)
{
    return blob->data == NULL || blob->size == 0;
}

void
hz_blob_destroy(hz_blob_t *blob)
{
    if (!hz_blob_is_empty(blob)) hz_free(blob->data);
    hz_free(blob);
}

void
hz_blob_resize(hz_blob_t *blob, size_t new_size)
{
    if (hz_blob_is_empty(blob)) {
        blob->data = hz_malloc(new_size);
    } else {
        blob->data = hz_realloc(blob->data, new_size);
    }

    blob->size = new_size;
}

size_t
hz_blob_get_size(hz_blob_t *blob)
{
    return blob->size;
}

typedef struct hz_glyph_position_t {
    int32_t xOffset;
    int32_t yOffset;
    int32_t xAdvance;
    int32_t yAdvance ;
} hz_glyph_position_t;

void hz_vector_init(void **v, size_t member_size)
{
    if (*v == NULL) {
        hz_vector_hdr_t *hdr = hz_malloc(sizeof(*hdr));
        hdr->member_size = member_size;
        hdr->size = 0;
        hdr->capacity = 0;
        *v = (void *)((uint8_t*)hdr+sizeof(hz_vector_hdr_t)); // data is at the end of the header
    }
}

hz_vector_hdr_t *hz_vector_header(void *v)
{
    return (hz_vector_hdr_t*)((uint8_t*)v-sizeof(hz_vector_hdr_t));
}

hz_bool_t hz_vector_is_empty(void *v)
{
    if (v != NULL) {
        return hz_vector_header(v)->size == 0;
    }

    return HZ_TRUE;
}

void hz_vector_clear_impl(void **v)
{
    if (*v != NULL) {
        hz_vector_hdr_t *hdr = hz_vector_header(*v);
        if (hdr->size != 0) {
            hdr = hz_realloc(hdr, sizeof(hz_vector_hdr_t));
            hdr->size = 0;
            hdr->capacity = 0;
            *v = (void *)((uint8_t*)hdr+sizeof(hz_vector_hdr_t));
        }
    }
}

void hz_vector_destroy_impl(void **v)
{
    if (*v != NULL) {
        hz_vector_hdr_t *hdr = hz_vector_header(*v);
        hz_free(hdr);
        *v = NULL;
    }
}

void hz_vector_reserve_impl(void **v, size_t new_cap)
{
    hz_vector_hdr_t *hdr = hz_vector_header(*v);
    size_t sz = new_cap * hdr->member_size;

    if (new_cap < hdr->size) {
        hdr->size = new_cap;
    }

    hdr->capacity = new_cap;
    hdr = hz_realloc(hdr, sizeof(hz_vector_hdr_t) + sz);
    *v = (void *)((uint8_t*)hdr + sizeof(hz_vector_hdr_t));
}

size_t hz_vector_size_impl(void *v)
{
    if (v != NULL) {
        return hz_vector_header(v)->size;
    }

    return 0;
}

void hz_vector_resize_impl(void **v, size_t new_size)
{
    hz_vector_reserve_impl(v,new_size);
    hz_vector_header(*v)->size = new_size;
}

void hz_vector_grow(void **v, int extra)
{
    hz_vector_hdr_t *hdr = hz_vector_header(*v);
    size_t new_cap = hdr->capacity ? hdr->capacity : 1;

    while (hdr->size + extra > new_cap) {
        new_cap <<= 1; // 2x factor
    }

    hz_vector_reserve_impl(v, new_cap);
}

hz_bool_t hz_vector_need_grow(void *v, size_t extra)
{
    assert(v != NULL);
    hz_vector_hdr_t *hdr = hz_vector_header(v);
    return hdr->size + extra > hdr->capacity;
}

hz_buffer_t *hz_buffer_create(void)
{
    hz_buffer_t *buffer = hz_malloc(sizeof(*buffer));
    buffer->glyph_count = 0;
    buffer->glyph_indices = NULL;
    buffer->codepoints = NULL;
    buffer->glyph_classes = NULL;
    buffer->attachment_classes = NULL;
    buffer->component_indices = NULL;
    buffer->glyph_metrics = NULL;
    buffer->attrib_flags = 0;
    return buffer;
}

HZ_STATIC HZ_INLINE void
hz_buffer_clear_attribs(hz_buffer_t *self, hz_glyph_attrib_flags_t attribs)
{
    if (attribs != 0){
        if (attribs & self->attrib_flags) {
            if (attribs & HZ_GLYPH_ATTRIB_METRICS_BIT)
                hz_vector_clear(self->glyph_metrics);
            if (attribs & HZ_GLYPH_ATTRIB_INDEX_BIT)
                hz_vector_clear(self->glyph_indices);
            if (attribs & HZ_GLYPH_ATTRIB_CODEPOINT_BIT)
                hz_vector_clear(self->codepoints);
            if (attribs & HZ_GLYPH_ATTRIB_GLYPH_CLASS_BIT)
                hz_vector_clear(self->glyph_classes);
            if (attribs & HZ_GLYPH_ATTRIB_ATTACHMENT_CLASS_BIT)
                hz_vector_clear(self->attachment_classes);
            if (attribs & HZ_GLYPH_ATTRIB_COMPONENT_INDEX_BIT)
                hz_vector_clear(self->component_indices);

        }

        if (self->attrib_flags == attribs) {
            self->glyph_count = 0;
        }
    }
}

void hz_buffer_clear(hz_buffer_t *buffer)
{
    hz_buffer_clear_attribs(buffer, buffer->attrib_flags);
}

typedef struct hz_glyph_object_t {
    hz_glyph_metrics_t metrics; // 16 bytes
    hz_unicode_t codepoint; // 4 bytes
    hz_index_t id; // 2 bytes
    uint16_t glyph_class; // 2 bytes
    uint16_t attachment_class; // 2 bytes
    uint16_t component_index; // 2 bytes
} hz_glyph_object_t; // 28 bytes

void hz_buffer_reserve(hz_buffer_t *self, size_t capacity)
{
    hz_vector_reserve(self->glyph_indices, capacity);
    hz_vector_reserve(self->codepoints, capacity);
}

void hz_buffer_add_glyph(hz_buffer_t *self,
                         hz_glyph_object_t go)
{
    if (self->attrib_flags & HZ_GLYPH_ATTRIB_METRICS_BIT)
        hz_vector_push_back(self->glyph_metrics, go.metrics);
    if (self->attrib_flags & HZ_GLYPH_ATTRIB_INDEX_BIT)
        hz_vector_push_back(self->glyph_indices, go.id);
    if (self->attrib_flags & HZ_GLYPH_ATTRIB_GLYPH_CLASS_BIT)
        hz_vector_push_back(self->glyph_classes, go.glyph_class);
    if (self->attrib_flags & HZ_GLYPH_ATTRIB_CODEPOINT_BIT)
        hz_vector_push_back(self->codepoints, go.codepoint);
    if (self->attrib_flags & HZ_GLYPH_ATTRIB_ATTACHMENT_CLASS_BIT)
        hz_vector_push_back(self->attachment_classes, go.attachment_class);
    if (self->attrib_flags & HZ_GLYPH_ATTRIB_COMPONENT_INDEX_BIT)
        hz_vector_push_back(self->component_indices, go.component_index);

    ++self->glyph_count;
}

hz_glyph_object_t hz_buffer_get_glyph(hz_buffer_t *self, size_t index)
{
    hz_glyph_object_t go;

    if (self->attrib_flags & HZ_GLYPH_ATTRIB_METRICS_BIT)
        go.metrics = self->glyph_metrics[index];
    if (self->attrib_flags & HZ_GLYPH_ATTRIB_INDEX_BIT)
        go.id = self->glyph_indices[index];
    if (self->attrib_flags & HZ_GLYPH_ATTRIB_GLYPH_CLASS_BIT)
        go.glyph_class = self->glyph_classes[index];
    if (self->attrib_flags & HZ_GLYPH_ATTRIB_CODEPOINT_BIT)
        go.codepoint = self->codepoints[index];
    if (self->attrib_flags & HZ_GLYPH_ATTRIB_ATTACHMENT_CLASS_BIT)
        go.attachment_class = self->attachment_classes[index];
    if (self->attrib_flags & HZ_GLYPH_ATTRIB_COMPONENT_INDEX_BIT)
        go.component_index = self->component_indices[index];

    return go;
}

void hz_buffer_add_range(hz_buffer_t *self,
                         const hz_buffer_t *other,
                         int v1, int v2)
{
    if (v2 >= v1 && (self->attrib_flags & other->attrib_flags) == self->attrib_flags) {
        int gap = (v2-v1)+1;
        if (self->attrib_flags & HZ_GLYPH_ATTRIB_METRICS_BIT)
            hz_vector_push_many(self->glyph_metrics, other->glyph_metrics+v1, gap);
        if (self->attrib_flags & HZ_GLYPH_ATTRIB_INDEX_BIT)
            hz_vector_push_many(self->glyph_indices, other->glyph_indices + v1, gap);
        if (self->attrib_flags & HZ_GLYPH_ATTRIB_CODEPOINT_BIT)
            hz_vector_push_many(self->codepoints, other->codepoints+v1, gap);
        if (self->attrib_flags & HZ_GLYPH_ATTRIB_GLYPH_CLASS_BIT)
            hz_vector_push_many(self->glyph_classes, other->glyph_classes+v1, gap);
        if (self->attrib_flags & HZ_GLYPH_ATTRIB_ATTACHMENT_CLASS_BIT)
            hz_vector_push_many(self->attachment_classes, other->attachment_classes+v1, gap);
        if (self->attrib_flags & HZ_GLYPH_ATTRIB_COMPONENT_INDEX_BIT)
            hz_vector_push_many(self->component_indices, other->component_indices+v1, gap);

        self->glyph_count += gap;
    }
}

void hz_buffer_add_other(hz_buffer_t *self, hz_buffer_t *other)
{
    hz_buffer_add_range(self, other, 0, other->glyph_count - 1);
}

void hz_buffer_destroy(hz_buffer_t *buffer)
{
    hz_buffer_clear(buffer);
    hz_free(buffer);
}

/* struct: hz_segment_t
 *      A struct used to store a segment of text and the information required to shape it.
 *
 *  See Also:
 *      <hz_buffer_t>
 */
struct hz_segment_t {
    hz_language_t language;
    hz_script_t script;
    hz_direction_t direction;
    size_t num_codepoints;
    hz_unicode_t *codepoints;

    // buffers used in shaping process
    hz_buffer_t *in, *out;
};

hz_segment_t *
hz_segment_create(void)
{
    hz_segment_t *seg = hz_malloc(sizeof(hz_segment_t));

    seg->num_codepoints = 0;
    seg->codepoints = NULL;

    seg->in = hz_buffer_create();
    seg->out = hz_buffer_create();

    seg->language = HZ_LANGUAGE_ENGLISH;
    seg->script = HZ_SCRIPT_LATIN;
    seg->direction = HZ_DIRECTION_LTR;

    return seg;
}

void
hz_segment_clear_shaping_objects(hz_segment_t *seg)
{
    hz_buffer_clear(seg->in);
    hz_buffer_clear(seg->out);
}

void
hz_segment_clear(hz_segment_t *seg)
{
    hz_segment_clear_shaping_objects(seg);

    if (seg->num_codepoints != 0) {
        hz_free(seg->codepoints);
        seg->codepoints = NULL;
        seg->num_codepoints = 0;
    }
}

void
hz_segment_destroy(hz_segment_t *seg)
{
    hz_segment_clear(seg);
    hz_free(seg);
}

void
hz_segment_set_script(hz_segment_t *seg, hz_script_t script)
{
    seg->script = script;
}

void
hz_segment_set_language(hz_segment_t *seg, hz_language_t language)
{
    seg->language = language;
}

void
hz_segment_set_direction(hz_segment_t *seg, hz_direction_t direction)
{
    seg->direction = direction;
}

const hz_buffer_t *
hz_segment_get_buffer(hz_segment_t *seg)
{
    // get main buffer that holds shaping results
    return seg->in;
}

typedef struct {
    const uint8_t *data;
    size_t length;
    size_t offset;
} hz_utf8_dec_t;

#define HZ_UTF8_END -1
#define HZ_UTF8_ERROR -2

static int
hz_utf8_get(hz_utf8_dec_t *dec) {
    if (dec->offset >= dec->length)
        return HZ_UTF8_END;

    return dec->data[dec->offset++];
}

static int
hz_utf8_cont(hz_utf8_dec_t *dec) {
    int c = hz_utf8_get(dec);
    return ((c & 0xC0) == 0x80)
           ? (c & 0x3F)
           : HZ_UTF8_ERROR;
}

static int32_t
hz_utf8_next(hz_utf8_dec_t *dec) {
    int c;
    int c1;
    int c2;
    int c3;
    int32_t r;
    c = hz_utf8_get(dec);

    /*
        Zero continuation (0 to 127)
    */
    if ((c & 0x80) == 0) {
        return c;
    }

    /*
        One continuation (128 to 2047)
    */
    if ((c & 0xE0) == 0xC0) {
        c1 = hz_utf8_cont(dec);
        if (c1 >= 0) {
            r = ((c & 0x1F) << 6) | c1;
            if (r >= 128) {
                return r;
            }
        }

        /*
            Two continuations (2048 to 55295 and 57344 to 65535)
        */
    } else if ((c & 0xF0) == 0xE0) {
        c1 = hz_utf8_cont(dec);
        c2 = hz_utf8_cont(dec);
        if ((c1 | c2) >= 0) {
            r = ((c & 0x0F) << 12) | (c1 << 6) | c2;
            if (r >= 2048 && (r < 55296 || r > 57343)) {
                return r;
            }
        }

        /*
            Three continuations (65536 to 1114111)
        */
    } else if ((c & 0xF8) == 0xF0) {
        c1 = hz_utf8_cont(dec);
        c2 = hz_utf8_cont(dec);
        c3 = hz_utf8_cont(dec);
        if ((c1 | c2 | c3) >= 0) {
            r = ((c & 0x07) << 18) | (c1 << 12) | (c2 << 6) | c3;
            if (r >= 65536 && r <= 1114111) {
                return r;
            }
        }
    }

    return HZ_UTF8_ERROR;
}


typedef struct utf_naive_decoder_t {
    const char8_t      *data;
    char32_t            code;
    uintptr_t           ptr;
    size_t              size; /* if 0, no limit */
    int                 step;
} utf8_decoder_t;

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

static const int8_t prefix_to_length_table[32] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 3
};

/* 256 * 16 = 4096 */
static const int8_t length_to_pattern1_table[256][16] = {
        {0,-1,-1,-1,1,-1,-1,-1,2,-1,-1,-1,3,-1,-1,-1},
        {0,-1,-1,-1,1,-1,-1,-1,2,-1,-1,-1,4,3,-1,-1},
        {0,-1,-1,-1,1,-1,-1,-1,2,-1,-1,-1,5,4,3,-1},
        {0,-1,-1,-1,1,-1,-1,-1,2,-1,-1,-1,6,5,4,3},
        {0,-1,-1,-1,1,-1,-1,-1,3,2,-1,-1,4,-1,-1,-1},
        {0,-1,-1,-1,1,-1,-1,-1,3,2,-1,-1,5,4,-1,-1},
        {0,-1,-1,-1,1,-1,-1,-1,3,2,-1,-1,6,5,4,-1},
        {0,-1,-1,-1,1,-1,-1,-1,3,2,-1,-1,7,6,5,4},
        {0,-1,-1,-1,1,-1,-1,-1,4,3,2,-1,5,-1,-1,-1},
        {0,-1,-1,-1,1,-1,-1,-1,4,3,2,-1,6,5,-1,-1},
        {0,-1,-1,-1,1,-1,-1,-1,4,3,2,-1,7,6,5,-1},
        {0,-1,-1,-1,1,-1,-1,-1,4,3,2,-1,8,7,6,5},
        {0,-1,-1,-1,1,-1,-1,-1,5,4,3,2,6,-1,-1,-1},
        {0,-1,-1,-1,1,-1,-1,-1,5,4,3,2,7,6,-1,-1},
        {0,-1,-1,-1,1,-1,-1,-1,5,4,3,2,8,7,6,-1},
        {0,-1,-1,-1,1,-1,-1,-1,5,4,3,2,9,8,7,6},
        {0,-1,-1,-1,2,1,-1,-1,3,-1,-1,-1,4,-1,-1,-1},
        {0,-1,-1,-1,2,1,-1,-1,3,-1,-1,-1,5,4,-1,-1},
        {0,-1,-1,-1,2,1,-1,-1,3,-1,-1,-1,6,5,4,-1},
        {0,-1,-1,-1,2,1,-1,-1,3,-1,-1,-1,7,6,5,4},
        {0,-1,-1,-1,2,1,-1,-1,4,3,-1,-1,5,-1,-1,-1},
        {0,-1,-1,-1,2,1,-1,-1,4,3,-1,-1,6,5,-1,-1},
        {0,-1,-1,-1,2,1,-1,-1,4,3,-1,-1,7,6,5,-1},
        {0,-1,-1,-1,2,1,-1,-1,4,3,-1,-1,8,7,6,5},
        {0,-1,-1,-1,2,1,-1,-1,5,4,3,-1,6,-1,-1,-1},
        {0,-1,-1,-1,2,1,-1,-1,5,4,3,-1,7,6,-1,-1},
        {0,-1,-1,-1,2,1,-1,-1,5,4,3,-1,8,7,6,-1},
        {0,-1,-1,-1,2,1,-1,-1,5,4,3,-1,9,8,7,6},
        {0,-1,-1,-1,2,1,-1,-1,6,5,4,3,7,-1,-1,-1},
        {0,-1,-1,-1,2,1,-1,-1,6,5,4,3,8,7,-1,-1},
        {0,-1,-1,-1,2,1,-1,-1,6,5,4,3,9,8,7,-1},
        {0,-1,-1,-1,2,1,-1,-1,6,5,4,3,10,9,8,7},
        {0,-1,-1,-1,3,2,1,-1,4,-1,-1,-1,5,-1,-1,-1},
        {0,-1,-1,-1,3,2,1,-1,4,-1,-1,-1,6,5,-1,-1},
        {0,-1,-1,-1,3,2,1,-1,4,-1,-1,-1,7,6,5,-1},
        {0,-1,-1,-1,3,2,1,-1,4,-1,-1,-1,8,7,6,5},
        {0,-1,-1,-1,3,2,1,-1,5,4,-1,-1,6,-1,-1,-1},
        {0,-1,-1,-1,3,2,1,-1,5,4,-1,-1,7,6,-1,-1},
        {0,-1,-1,-1,3,2,1,-1,5,4,-1,-1,8,7,6,-1},
        {0,-1,-1,-1,3,2,1,-1,5,4,-1,-1,9,8,7,6},
        {0,-1,-1,-1,3,2,1,-1,6,5,4,-1,7,-1,-1,-1},
        {0,-1,-1,-1,3,2,1,-1,6,5,4,-1,8,7,-1,-1},
        {0,-1,-1,-1,3,2,1,-1,6,5,4,-1,9,8,7,-1},
        {0,-1,-1,-1,3,2,1,-1,6,5,4,-1,10,9,8,7},
        {0,-1,-1,-1,3,2,1,-1,7,6,5,4,8,-1,-1,-1},
        {0,-1,-1,-1,3,2,1,-1,7,6,5,4,9,8,-1,-1},
        {0,-1,-1,-1,3,2,1,-1,7,6,5,4,10,9,8,-1},
        {0,-1,-1,-1,3,2,1,-1,7,6,5,4,11,10,9,8},
        {0,-1,-1,-1,4,3,2,1,5,-1,-1,-1,6,-1,-1,-1},
        {0,-1,-1,-1,4,3,2,1,5,-1,-1,-1,7,6,-1,-1},
        {0,-1,-1,-1,4,3,2,1,5,-1,-1,-1,8,7,6,-1},
        {0,-1,-1,-1,4,3,2,1,5,-1,-1,-1,9,8,7,6},
        {0,-1,-1,-1,4,3,2,1,6,5,-1,-1,7,-1,-1,-1},
        {0,-1,-1,-1,4,3,2,1,6,5,-1,-1,8,7,-1,-1},
        {0,-1,-1,-1,4,3,2,1,6,5,-1,-1,9,8,7,-1},
        {0,-1,-1,-1,4,3,2,1,6,5,-1,-1,10,9,8,7},
        {0,-1,-1,-1,4,3,2,1,7,6,5,-1,8,-1,-1,-1},
        {0,-1,-1,-1,4,3,2,1,7,6,5,-1,9,8,-1,-1},
        {0,-1,-1,-1,4,3,2,1,7,6,5,-1,10,9,8,-1},
        {0,-1,-1,-1,4,3,2,1,7,6,5,-1,11,10,9,8},
        {0,-1,-1,-1,4,3,2,1,8,7,6,5,9,-1,-1,-1},
        {0,-1,-1,-1,4,3,2,1,8,7,6,5,10,9,-1,-1},
        {0,-1,-1,-1,4,3,2,1,8,7,6,5,11,10,9,-1},
        {0,-1,-1,-1,4,3,2,1,8,7,6,5,12,11,10,9},
        {1,0,-1,-1,2,-1,-1,-1,3,-1,-1,-1,4,-1,-1,-1},
        {1,0,-1,-1,2,-1,-1,-1,3,-1,-1,-1,5,4,-1,-1},
        {1,0,-1,-1,2,-1,-1,-1,3,-1,-1,-1,6,5,4,-1},
        {1,0,-1,-1,2,-1,-1,-1,3,-1,-1,-1,7,6,5,4},
        {1,0,-1,-1,2,-1,-1,-1,4,3,-1,-1,5,-1,-1,-1},
        {1,0,-1,-1,2,-1,-1,-1,4,3,-1,-1,6,5,-1,-1},
        {1,0,-1,-1,2,-1,-1,-1,4,3,-1,-1,7,6,5,-1},
        {1,0,-1,-1,2,-1,-1,-1,4,3,-1,-1,8,7,6,5},
        {1,0,-1,-1,2,-1,-1,-1,5,4,3,-1,6,-1,-1,-1},
        {1,0,-1,-1,2,-1,-1,-1,5,4,3,-1,7,6,-1,-1},
        {1,0,-1,-1,2,-1,-1,-1,5,4,3,-1,8,7,6,-1},
        {1,0,-1,-1,2,-1,-1,-1,5,4,3,-1,9,8,7,6},
        {1,0,-1,-1,2,-1,-1,-1,6,5,4,3,7,-1,-1,-1},
        {1,0,-1,-1,2,-1,-1,-1,6,5,4,3,8,7,-1,-1},
        {1,0,-1,-1,2,-1,-1,-1,6,5,4,3,9,8,7,-1},
        {1,0,-1,-1,2,-1,-1,-1,6,5,4,3,10,9,8,7},
        {1,0,-1,-1,3,2,-1,-1,4,-1,-1,-1,5,-1,-1,-1},
        {1,0,-1,-1,3,2,-1,-1,4,-1,-1,-1,6,5,-1,-1},
        {1,0,-1,-1,3,2,-1,-1,4,-1,-1,-1,7,6,5,-1},
        {1,0,-1,-1,3,2,-1,-1,4,-1,-1,-1,8,7,6,5},
        {1,0,-1,-1,3,2,-1,-1,5,4,-1,-1,6,-1,-1,-1},
        {1,0,-1,-1,3,2,-1,-1,5,4,-1,-1,7,6,-1,-1},
        {1,0,-1,-1,3,2,-1,-1,5,4,-1,-1,8,7,6,-1},
        {1,0,-1,-1,3,2,-1,-1,5,4,-1,-1,9,8,7,6},
        {1,0,-1,-1,3,2,-1,-1,6,5,4,-1,7,-1,-1,-1},
        {1,0,-1,-1,3,2,-1,-1,6,5,4,-1,8,7,-1,-1},
        {1,0,-1,-1,3,2,-1,-1,6,5,4,-1,9,8,7,-1},
        {1,0,-1,-1,3,2,-1,-1,6,5,4,-1,10,9,8,7},
        {1,0,-1,-1,3,2,-1,-1,7,6,5,4,8,-1,-1,-1},
        {1,0,-1,-1,3,2,-1,-1,7,6,5,4,9,8,-1,-1},
        {1,0,-1,-1,3,2,-1,-1,7,6,5,4,10,9,8,-1},
        {1,0,-1,-1,3,2,-1,-1,7,6,5,4,11,10,9,8},
        {1,0,-1,-1,4,3,2,-1,5,-1,-1,-1,6,-1,-1,-1},
        {1,0,-1,-1,4,3,2,-1,5,-1,-1,-1,7,6,-1,-1},
        {1,0,-1,-1,4,3,2,-1,5,-1,-1,-1,8,7,6,-1},
        {1,0,-1,-1,4,3,2,-1,5,-1,-1,-1,9,8,7,6},
        {1,0,-1,-1,4,3,2,-1,6,5,-1,-1,7,-1,-1,-1},
        {1,0,-1,-1,4,3,2,-1,6,5,-1,-1,8,7,-1,-1},
        {1,0,-1,-1,4,3,2,-1,6,5,-1,-1,9,8,7,-1},
        {1,0,-1,-1,4,3,2,-1,6,5,-1,-1,10,9,8,7},
        {1,0,-1,-1,4,3,2,-1,7,6,5,-1,8,-1,-1,-1},
        {1,0,-1,-1,4,3,2,-1,7,6,5,-1,9,8,-1,-1},
        {1,0,-1,-1,4,3,2,-1,7,6,5,-1,10,9,8,-1},
        {1,0,-1,-1,4,3,2,-1,7,6,5,-1,11,10,9,8},
        {1,0,-1,-1,4,3,2,-1,8,7,6,5,9,-1,-1,-1},
        {1,0,-1,-1,4,3,2,-1,8,7,6,5,10,9,-1,-1},
        {1,0,-1,-1,4,3,2,-1,8,7,6,5,11,10,9,-1},
        {1,0,-1,-1,4,3,2,-1,8,7,6,5,12,11,10,9},
        {1,0,-1,-1,5,4,3,2,6,-1,-1,-1,7,-1,-1,-1},
        {1,0,-1,-1,5,4,3,2,6,-1,-1,-1,8,7,-1,-1},
        {1,0,-1,-1,5,4,3,2,6,-1,-1,-1,9,8,7,-1},
        {1,0,-1,-1,5,4,3,2,6,-1,-1,-1,10,9,8,7},
        {1,0,-1,-1,5,4,3,2,7,6,-1,-1,8,-1,-1,-1},
        {1,0,-1,-1,5,4,3,2,7,6,-1,-1,9,8,-1,-1},
        {1,0,-1,-1,5,4,3,2,7,6,-1,-1,10,9,8,-1},
        {1,0,-1,-1,5,4,3,2,7,6,-1,-1,11,10,9,8},
        {1,0,-1,-1,5,4,3,2,8,7,6,-1,9,-1,-1,-1},
        {1,0,-1,-1,5,4,3,2,8,7,6,-1,10,9,-1,-1},
        {1,0,-1,-1,5,4,3,2,8,7,6,-1,11,10,9,-1},
        {1,0,-1,-1,5,4,3,2,8,7,6,-1,12,11,10,9},
        {1,0,-1,-1,5,4,3,2,9,8,7,6,10,-1,-1,-1},
        {1,0,-1,-1,5,4,3,2,9,8,7,6,11,10,-1,-1},
        {1,0,-1,-1,5,4,3,2,9,8,7,6,12,11,10,-1},
        {1,0,-1,-1,5,4,3,2,9,8,7,6,13,12,11,10},
        {2,1,0,-1,3,-1,-1,-1,4,-1,-1,-1,5,-1,-1,-1},
        {2,1,0,-1,3,-1,-1,-1,4,-1,-1,-1,6,5,-1,-1},
        {2,1,0,-1,3,-1,-1,-1,4,-1,-1,-1,7,6,5,-1},
        {2,1,0,-1,3,-1,-1,-1,4,-1,-1,-1,8,7,6,5},
        {2,1,0,-1,3,-1,-1,-1,5,4,-1,-1,6,-1,-1,-1},
        {2,1,0,-1,3,-1,-1,-1,5,4,-1,-1,7,6,-1,-1},
        {2,1,0,-1,3,-1,-1,-1,5,4,-1,-1,8,7,6,-1},
        {2,1,0,-1,3,-1,-1,-1,5,4,-1,-1,9,8,7,6},
        {2,1,0,-1,3,-1,-1,-1,6,5,4,-1,7,-1,-1,-1},
        {2,1,0,-1,3,-1,-1,-1,6,5,4,-1,8,7,-1,-1},
        {2,1,0,-1,3,-1,-1,-1,6,5,4,-1,9,8,7,-1},
        {2,1,0,-1,3,-1,-1,-1,6,5,4,-1,10,9,8,7},
        {2,1,0,-1,3,-1,-1,-1,7,6,5,4,8,-1,-1,-1},
        {2,1,0,-1,3,-1,-1,-1,7,6,5,4,9,8,-1,-1},
        {2,1,0,-1,3,-1,-1,-1,7,6,5,4,10,9,8,-1},
        {2,1,0,-1,3,-1,-1,-1,7,6,5,4,11,10,9,8},
        {2,1,0,-1,4,3,-1,-1,5,-1,-1,-1,6,-1,-1,-1},
        {2,1,0,-1,4,3,-1,-1,5,-1,-1,-1,7,6,-1,-1},
        {2,1,0,-1,4,3,-1,-1,5,-1,-1,-1,8,7,6,-1},
        {2,1,0,-1,4,3,-1,-1,5,-1,-1,-1,9,8,7,6},
        {2,1,0,-1,4,3,-1,-1,6,5,-1,-1,7,-1,-1,-1},
        {2,1,0,-1,4,3,-1,-1,6,5,-1,-1,8,7,-1,-1},
        {2,1,0,-1,4,3,-1,-1,6,5,-1,-1,9,8,7,-1},
        {2,1,0,-1,4,3,-1,-1,6,5,-1,-1,10,9,8,7},
        {2,1,0,-1,4,3,-1,-1,7,6,5,-1,8,-1,-1,-1},
        {2,1,0,-1,4,3,-1,-1,7,6,5,-1,9,8,-1,-1},
        {2,1,0,-1,4,3,-1,-1,7,6,5,-1,10,9,8,-1},
        {2,1,0,-1,4,3,-1,-1,7,6,5,-1,11,10,9,8},
        {2,1,0,-1,4,3,-1,-1,8,7,6,5,9,-1,-1,-1},
        {2,1,0,-1,4,3,-1,-1,8,7,6,5,10,9,-1,-1},
        {2,1,0,-1,4,3,-1,-1,8,7,6,5,11,10,9,-1},
        {2,1,0,-1,4,3,-1,-1,8,7,6,5,12,11,10,9},
        {2,1,0,-1,5,4,3,-1,6,-1,-1,-1,7,-1,-1,-1},
        {2,1,0,-1,5,4,3,-1,6,-1,-1,-1,8,7,-1,-1},
        {2,1,0,-1,5,4,3,-1,6,-1,-1,-1,9,8,7,-1},
        {2,1,0,-1,5,4,3,-1,6,-1,-1,-1,10,9,8,7},
        {2,1,0,-1,5,4,3,-1,7,6,-1,-1,8,-1,-1,-1},
        {2,1,0,-1,5,4,3,-1,7,6,-1,-1,9,8,-1,-1},
        {2,1,0,-1,5,4,3,-1,7,6,-1,-1,10,9,8,-1},
        {2,1,0,-1,5,4,3,-1,7,6,-1,-1,11,10,9,8},
        {2,1,0,-1,5,4,3,-1,8,7,6,-1,9,-1,-1,-1},
        {2,1,0,-1,5,4,3,-1,8,7,6,-1,10,9,-1,-1},
        {2,1,0,-1,5,4,3,-1,8,7,6,-1,11,10,9,-1},
        {2,1,0,-1,5,4,3,-1,8,7,6,-1,12,11,10,9},
        {2,1,0,-1,5,4,3,-1,9,8,7,6,10,-1,-1,-1},
        {2,1,0,-1,5,4,3,-1,9,8,7,6,11,10,-1,-1},
        {2,1,0,-1,5,4,3,-1,9,8,7,6,12,11,10,-1},
        {2,1,0,-1,5,4,3,-1,9,8,7,6,13,12,11,10},
        {2,1,0,-1,6,5,4,3,7,-1,-1,-1,8,-1,-1,-1},
        {2,1,0,-1,6,5,4,3,7,-1,-1,-1,9,8,-1,-1},
        {2,1,0,-1,6,5,4,3,7,-1,-1,-1,10,9,8,-1},
        {2,1,0,-1,6,5,4,3,7,-1,-1,-1,11,10,9,8},
        {2,1,0,-1,6,5,4,3,8,7,-1,-1,9,-1,-1,-1},
        {2,1,0,-1,6,5,4,3,8,7,-1,-1,10,9,-1,-1},
        {2,1,0,-1,6,5,4,3,8,7,-1,-1,11,10,9,-1},
        {2,1,0,-1,6,5,4,3,8,7,-1,-1,12,11,10,9},
        {2,1,0,-1,6,5,4,3,9,8,7,-1,10,-1,-1,-1},
        {2,1,0,-1,6,5,4,3,9,8,7,-1,11,10,-1,-1},
        {2,1,0,-1,6,5,4,3,9,8,7,-1,12,11,10,-1},
        {2,1,0,-1,6,5,4,3,9,8,7,-1,13,12,11,10},
        {2,1,0,-1,6,5,4,3,10,9,8,7,11,-1,-1,-1},
        {2,1,0,-1,6,5,4,3,10,9,8,7,12,11,-1,-1},
        {2,1,0,-1,6,5,4,3,10,9,8,7,13,12,11,-1},
        {2,1,0,-1,6,5,4,3,10,9,8,7,14,13,12,11},
        {3,2,1,0,4,-1,-1,-1,5,-1,-1,-1,6,-1,-1,-1},
        {3,2,1,0,4,-1,-1,-1,5,-1,-1,-1,7,6,-1,-1},
        {3,2,1,0,4,-1,-1,-1,5,-1,-1,-1,8,7,6,-1},
        {3,2,1,0,4,-1,-1,-1,5,-1,-1,-1,9,8,7,6},
        {3,2,1,0,4,-1,-1,-1,6,5,-1,-1,7,-1,-1,-1},
        {3,2,1,0,4,-1,-1,-1,6,5,-1,-1,8,7,-1,-1},
        {3,2,1,0,4,-1,-1,-1,6,5,-1,-1,9,8,7,-1},
        {3,2,1,0,4,-1,-1,-1,6,5,-1,-1,10,9,8,7},
        {3,2,1,0,4,-1,-1,-1,7,6,5,-1,8,-1,-1,-1},
        {3,2,1,0,4,-1,-1,-1,7,6,5,-1,9,8,-1,-1},
        {3,2,1,0,4,-1,-1,-1,7,6,5,-1,10,9,8,-1},
        {3,2,1,0,4,-1,-1,-1,7,6,5,-1,11,10,9,8},
        {3,2,1,0,4,-1,-1,-1,8,7,6,5,9,-1,-1,-1},
        {3,2,1,0,4,-1,-1,-1,8,7,6,5,10,9,-1,-1},
        {3,2,1,0,4,-1,-1,-1,8,7,6,5,11,10,9,-1},
        {3,2,1,0,4,-1,-1,-1,8,7,6,5,12,11,10,9},
        {3,2,1,0,5,4,-1,-1,6,-1,-1,-1,7,-1,-1,-1},
        {3,2,1,0,5,4,-1,-1,6,-1,-1,-1,8,7,-1,-1},
        {3,2,1,0,5,4,-1,-1,6,-1,-1,-1,9,8,7,-1},
        {3,2,1,0,5,4,-1,-1,6,-1,-1,-1,10,9,8,7},
        {3,2,1,0,5,4,-1,-1,7,6,-1,-1,8,-1,-1,-1},
        {3,2,1,0,5,4,-1,-1,7,6,-1,-1,9,8,-1,-1},
        {3,2,1,0,5,4,-1,-1,7,6,-1,-1,10,9,8,-1},
        {3,2,1,0,5,4,-1,-1,7,6,-1,-1,11,10,9,8},
        {3,2,1,0,5,4,-1,-1,8,7,6,-1,9,-1,-1,-1},
        {3,2,1,0,5,4,-1,-1,8,7,6,-1,10,9,-1,-1},
        {3,2,1,0,5,4,-1,-1,8,7,6,-1,11,10,9,-1},
        {3,2,1,0,5,4,-1,-1,8,7,6,-1,12,11,10,9},
        {3,2,1,0,5,4,-1,-1,9,8,7,6,10,-1,-1,-1},
        {3,2,1,0,5,4,-1,-1,9,8,7,6,11,10,-1,-1},
        {3,2,1,0,5,4,-1,-1,9,8,7,6,12,11,10,-1},
        {3,2,1,0,5,4,-1,-1,9,8,7,6,13,12,11,10},
        {3,2,1,0,6,5,4,-1,7,-1,-1,-1,8,-1,-1,-1},
        {3,2,1,0,6,5,4,-1,7,-1,-1,-1,9,8,-1,-1},
        {3,2,1,0,6,5,4,-1,7,-1,-1,-1,10,9,8,-1},
        {3,2,1,0,6,5,4,-1,7,-1,-1,-1,11,10,9,8},
        {3,2,1,0,6,5,4,-1,8,7,-1,-1,9,-1,-1,-1},
        {3,2,1,0,6,5,4,-1,8,7,-1,-1,10,9,-1,-1},
        {3,2,1,0,6,5,4,-1,8,7,-1,-1,11,10,9,-1},
        {3,2,1,0,6,5,4,-1,8,7,-1,-1,12,11,10,9},
        {3,2,1,0,6,5,4,-1,9,8,7,-1,10,-1,-1,-1},
        {3,2,1,0,6,5,4,-1,9,8,7,-1,11,10,-1,-1},
        {3,2,1,0,6,5,4,-1,9,8,7,-1,12,11,10,-1},
        {3,2,1,0,6,5,4,-1,9,8,7,-1,13,12,11,10},
        {3,2,1,0,6,5,4,-1,10,9,8,7,11,-1,-1,-1},
        {3,2,1,0,6,5,4,-1,10,9,8,7,12,11,-1,-1},
        {3,2,1,0,6,5,4,-1,10,9,8,7,13,12,11,-1},
        {3,2,1,0,6,5,4,-1,10,9,8,7,14,13,12,11},
        {3,2,1,0,7,6,5,4,8,-1,-1,-1,9,-1,-1,-1},
        {3,2,1,0,7,6,5,4,8,-1,-1,-1,10,9,-1,-1},
        {3,2,1,0,7,6,5,4,8,-1,-1,-1,11,10,9,-1},
        {3,2,1,0,7,6,5,4,8,-1,-1,-1,12,11,10,9},
        {3,2,1,0,7,6,5,4,9,8,-1,-1,10,-1,-1,-1},
        {3,2,1,0,7,6,5,4,9,8,-1,-1,11,10,-1,-1},
        {3,2,1,0,7,6,5,4,9,8,-1,-1,12,11,10,-1},
        {3,2,1,0,7,6,5,4,9,8,-1,-1,13,12,11,10},
        {3,2,1,0,7,6,5,4,10,9,8,-1,11,-1,-1,-1},
        {3,2,1,0,7,6,5,4,10,9,8,-1,12,11,-1,-1},
        {3,2,1,0,7,6,5,4,10,9,8,-1,13,12,11,-1},
        {3,2,1,0,7,6,5,4,10,9,8,-1,14,13,12,11},
        {3,2,1,0,7,6,5,4,11,10,9,8,12,-1,-1,-1},
        {3,2,1,0,7,6,5,4,11,10,9,8,13,12,-1,-1},
        {3,2,1,0,7,6,5,4,11,10,9,8,14,13,12,-1},
        {3,2,1,0,7,6,5,4,11,10,9,8,15,14,13,12}
};

static const uint8_t length_to_mask1_table[256][16] = {
        {0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00},
        {0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07},
        {0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00},
        {0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07},
        {0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07},
        {0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00},
        {0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00,0x3F,0x3F,0x3F,0x07},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x7F,0x00,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x3F,0x1F,0x00,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x0F,0x00},
        {0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07,0x3F,0x3F,0x3F,0x07}
};

static const int8_t length_to_counts_table[256][16] = {
        {0,-1,-1,-1,0,-1,-1,-1,0,-1,-1,-1,0,-1,-1,-1},
        {0,-1,-1,-1,0,-1,-1,-1,0,-1,-1,-1,1,0,-1,-1},
        {0,-1,-1,-1,0,-1,-1,-1,0,-1,-1,-1,2,1,0,-1},
        {0,-1,-1,-1,0,-1,-1,-1,0,-1,-1,-1,3,2,1,0},
        {0,-1,-1,-1,0,-1,-1,-1,1,0,-1,-1,0,-1,-1,-1},
        {0,-1,-1,-1,0,-1,-1,-1,1,0,-1,-1,1,0,-1,-1},
        {0,-1,-1,-1,0,-1,-1,-1,1,0,-1,-1,2,1,0,-1},
        {0,-1,-1,-1,0,-1,-1,-1,1,0,-1,-1,3,2,1,0},
        {0,-1,-1,-1,0,-1,-1,-1,2,1,0,-1,0,-1,-1,-1},
        {0,-1,-1,-1,0,-1,-1,-1,2,1,0,-1,1,0,-1,-1},
        {0,-1,-1,-1,0,-1,-1,-1,2,1,0,-1,2,1,0,-1},
        {0,-1,-1,-1,0,-1,-1,-1,2,1,0,-1,3,2,1,0},
        {0,-1,-1,-1,0,-1,-1,-1,3,2,1,0,0,-1,-1,-1},
        {0,-1,-1,-1,0,-1,-1,-1,3,2,1,0,1,0,-1,-1},
        {0,-1,-1,-1,0,-1,-1,-1,3,2,1,0,2,1,0,-1},
        {0,-1,-1,-1,0,-1,-1,-1,3,2,1,0,3,2,1,0},
        {0,-1,-1,-1,1,0,-1,-1,0,-1,-1,-1,0,-1,-1,-1},
        {0,-1,-1,-1,1,0,-1,-1,0,-1,-1,-1,1,0,-1,-1},
        {0,-1,-1,-1,1,0,-1,-1,0,-1,-1,-1,2,1,0,-1},
        {0,-1,-1,-1,1,0,-1,-1,0,-1,-1,-1,3,2,1,0},
        {0,-1,-1,-1,1,0,-1,-1,1,0,-1,-1,0,-1,-1,-1},
        {0,-1,-1,-1,1,0,-1,-1,1,0,-1,-1,1,0,-1,-1},
        {0,-1,-1,-1,1,0,-1,-1,1,0,-1,-1,2,1,0,-1},
        {0,-1,-1,-1,1,0,-1,-1,1,0,-1,-1,3,2,1,0},
        {0,-1,-1,-1,1,0,-1,-1,2,1,0,-1,0,-1,-1,-1},
        {0,-1,-1,-1,1,0,-1,-1,2,1,0,-1,1,0,-1,-1},
        {0,-1,-1,-1,1,0,-1,-1,2,1,0,-1,2,1,0,-1},
        {0,-1,-1,-1,1,0,-1,-1,2,1,0,-1,3,2,1,0},
        {0,-1,-1,-1,1,0,-1,-1,3,2,1,0,0,-1,-1,-1},
        {0,-1,-1,-1,1,0,-1,-1,3,2,1,0,1,0,-1,-1},
        {0,-1,-1,-1,1,0,-1,-1,3,2,1,0,2,1,0,-1},
        {0,-1,-1,-1,1,0,-1,-1,3,2,1,0,3,2,1,0},
        {0,-1,-1,-1,2,1,0,-1,0,-1,-1,-1,0,-1,-1,-1},
        {0,-1,-1,-1,2,1,0,-1,0,-1,-1,-1,1,0,-1,-1},
        {0,-1,-1,-1,2,1,0,-1,0,-1,-1,-1,2,1,0,-1},
        {0,-1,-1,-1,2,1,0,-1,0,-1,-1,-1,3,2,1,0},
        {0,-1,-1,-1,2,1,0,-1,1,0,-1,-1,0,-1,-1,-1},
        {0,-1,-1,-1,2,1,0,-1,1,0,-1,-1,1,0,-1,-1},
        {0,-1,-1,-1,2,1,0,-1,1,0,-1,-1,2,1,0,-1},
        {0,-1,-1,-1,2,1,0,-1,1,0,-1,-1,3,2,1,0},
        {0,-1,-1,-1,2,1,0,-1,2,1,0,-1,0,-1,-1,-1},
        {0,-1,-1,-1,2,1,0,-1,2,1,0,-1,1,0,-1,-1},
        {0,-1,-1,-1,2,1,0,-1,2,1,0,-1,2,1,0,-1},
        {0,-1,-1,-1,2,1,0,-1,2,1,0,-1,3,2,1,0},
        {0,-1,-1,-1,2,1,0,-1,3,2,1,0,0,-1,-1,-1},
        {0,-1,-1,-1,2,1,0,-1,3,2,1,0,1,0,-1,-1},
        {0,-1,-1,-1,2,1,0,-1,3,2,1,0,2,1,0,-1},
        {0,-1,-1,-1,2,1,0,-1,3,2,1,0,3,2,1,0},
        {0,-1,-1,-1,3,2,1,0,0,-1,-1,-1,0,-1,-1,-1},
        {0,-1,-1,-1,3,2,1,0,0,-1,-1,-1,1,0,-1,-1},
        {0,-1,-1,-1,3,2,1,0,0,-1,-1,-1,2,1,0,-1},
        {0,-1,-1,-1,3,2,1,0,0,-1,-1,-1,3,2,1,0},
        {0,-1,-1,-1,3,2,1,0,1,0,-1,-1,0,-1,-1,-1},
        {0,-1,-1,-1,3,2,1,0,1,0,-1,-1,1,0,-1,-1},
        {0,-1,-1,-1,3,2,1,0,1,0,-1,-1,2,1,0,-1},
        {0,-1,-1,-1,3,2,1,0,1,0,-1,-1,3,2,1,0},
        {0,-1,-1,-1,3,2,1,0,2,1,0,-1,0,-1,-1,-1},
        {0,-1,-1,-1,3,2,1,0,2,1,0,-1,1,0,-1,-1},
        {0,-1,-1,-1,3,2,1,0,2,1,0,-1,2,1,0,-1},
        {0,-1,-1,-1,3,2,1,0,2,1,0,-1,3,2,1,0},
        {0,-1,-1,-1,3,2,1,0,3,2,1,0,0,-1,-1,-1},
        {0,-1,-1,-1,3,2,1,0,3,2,1,0,1,0,-1,-1},
        {0,-1,-1,-1,3,2,1,0,3,2,1,0,2,1,0,-1},
        {0,-1,-1,-1,3,2,1,0,3,2,1,0,3,2,1,0},
        {1,0,-1,-1,0,-1,-1,-1,0,-1,-1,-1,0,-1,-1,-1},
        {1,0,-1,-1,0,-1,-1,-1,0,-1,-1,-1,1,0,-1,-1},
        {1,0,-1,-1,0,-1,-1,-1,0,-1,-1,-1,2,1,0,-1},
        {1,0,-1,-1,0,-1,-1,-1,0,-1,-1,-1,3,2,1,0},
        {1,0,-1,-1,0,-1,-1,-1,1,0,-1,-1,0,-1,-1,-1},
        {1,0,-1,-1,0,-1,-1,-1,1,0,-1,-1,1,0,-1,-1},
        {1,0,-1,-1,0,-1,-1,-1,1,0,-1,-1,2,1,0,-1},
        {1,0,-1,-1,0,-1,-1,-1,1,0,-1,-1,3,2,1,0},
        {1,0,-1,-1,0,-1,-1,-1,2,1,0,-1,0,-1,-1,-1},
        {1,0,-1,-1,0,-1,-1,-1,2,1,0,-1,1,0,-1,-1},
        {1,0,-1,-1,0,-1,-1,-1,2,1,0,-1,2,1,0,-1},
        {1,0,-1,-1,0,-1,-1,-1,2,1,0,-1,3,2,1,0},
        {1,0,-1,-1,0,-1,-1,-1,3,2,1,0,0,-1,-1,-1},
        {1,0,-1,-1,0,-1,-1,-1,3,2,1,0,1,0,-1,-1},
        {1,0,-1,-1,0,-1,-1,-1,3,2,1,0,2,1,0,-1},
        {1,0,-1,-1,0,-1,-1,-1,3,2,1,0,3,2,1,0},
        {1,0,-1,-1,1,0,-1,-1,0,-1,-1,-1,0,-1,-1,-1},
        {1,0,-1,-1,1,0,-1,-1,0,-1,-1,-1,1,0,-1,-1},
        {1,0,-1,-1,1,0,-1,-1,0,-1,-1,-1,2,1,0,-1},
        {1,0,-1,-1,1,0,-1,-1,0,-1,-1,-1,3,2,1,0},
        {1,0,-1,-1,1,0,-1,-1,1,0,-1,-1,0,-1,-1,-1},
        {1,0,-1,-1,1,0,-1,-1,1,0,-1,-1,1,0,-1,-1},
        {1,0,-1,-1,1,0,-1,-1,1,0,-1,-1,2,1,0,-1},
        {1,0,-1,-1,1,0,-1,-1,1,0,-1,-1,3,2,1,0},
        {1,0,-1,-1,1,0,-1,-1,2,1,0,-1,0,-1,-1,-1},
        {1,0,-1,-1,1,0,-1,-1,2,1,0,-1,1,0,-1,-1},
        {1,0,-1,-1,1,0,-1,-1,2,1,0,-1,2,1,0,-1},
        {1,0,-1,-1,1,0,-1,-1,2,1,0,-1,3,2,1,0},
        {1,0,-1,-1,1,0,-1,-1,3,2,1,0,0,-1,-1,-1},
        {1,0,-1,-1,1,0,-1,-1,3,2,1,0,1,0,-1,-1},
        {1,0,-1,-1,1,0,-1,-1,3,2,1,0,2,1,0,-1},
        {1,0,-1,-1,1,0,-1,-1,3,2,1,0,3,2,1,0},
        {1,0,-1,-1,2,1,0,-1,0,-1,-1,-1,0,-1,-1,-1},
        {1,0,-1,-1,2,1,0,-1,0,-1,-1,-1,1,0,-1,-1},
        {1,0,-1,-1,2,1,0,-1,0,-1,-1,-1,2,1,0,-1},
        {1,0,-1,-1,2,1,0,-1,0,-1,-1,-1,3,2,1,0},
        {1,0,-1,-1,2,1,0,-1,1,0,-1,-1,0,-1,-1,-1},
        {1,0,-1,-1,2,1,0,-1,1,0,-1,-1,1,0,-1,-1},
        {1,0,-1,-1,2,1,0,-1,1,0,-1,-1,2,1,0,-1},
        {1,0,-1,-1,2,1,0,-1,1,0,-1,-1,3,2,1,0},
        {1,0,-1,-1,2,1,0,-1,2,1,0,-1,0,-1,-1,-1},
        {1,0,-1,-1,2,1,0,-1,2,1,0,-1,1,0,-1,-1},
        {1,0,-1,-1,2,1,0,-1,2,1,0,-1,2,1,0,-1},
        {1,0,-1,-1,2,1,0,-1,2,1,0,-1,3,2,1,0},
        {1,0,-1,-1,2,1,0,-1,3,2,1,0,0,-1,-1,-1},
        {1,0,-1,-1,2,1,0,-1,3,2,1,0,1,0,-1,-1},
        {1,0,-1,-1,2,1,0,-1,3,2,1,0,2,1,0,-1},
        {1,0,-1,-1,2,1,0,-1,3,2,1,0,3,2,1,0},
        {1,0,-1,-1,3,2,1,0,0,-1,-1,-1,0,-1,-1,-1},
        {1,0,-1,-1,3,2,1,0,0,-1,-1,-1,1,0,-1,-1},
        {1,0,-1,-1,3,2,1,0,0,-1,-1,-1,2,1,0,-1},
        {1,0,-1,-1,3,2,1,0,0,-1,-1,-1,3,2,1,0},
        {1,0,-1,-1,3,2,1,0,1,0,-1,-1,0,-1,-1,-1},
        {1,0,-1,-1,3,2,1,0,1,0,-1,-1,1,0,-1,-1},
        {1,0,-1,-1,3,2,1,0,1,0,-1,-1,2,1,0,-1},
        {1,0,-1,-1,3,2,1,0,1,0,-1,-1,3,2,1,0},
        {1,0,-1,-1,3,2,1,0,2,1,0,-1,0,-1,-1,-1},
        {1,0,-1,-1,3,2,1,0,2,1,0,-1,1,0,-1,-1},
        {1,0,-1,-1,3,2,1,0,2,1,0,-1,2,1,0,-1},
        {1,0,-1,-1,3,2,1,0,2,1,0,-1,3,2,1,0},
        {1,0,-1,-1,3,2,1,0,3,2,1,0,0,-1,-1,-1},
        {1,0,-1,-1,3,2,1,0,3,2,1,0,1,0,-1,-1},
        {1,0,-1,-1,3,2,1,0,3,2,1,0,2,1,0,-1},
        {1,0,-1,-1,3,2,1,0,3,2,1,0,3,2,1,0},
        {2,1,0,-1,0,-1,-1,-1,0,-1,-1,-1,0,-1,-1,-1},
        {2,1,0,-1,0,-1,-1,-1,0,-1,-1,-1,1,0,-1,-1},
        {2,1,0,-1,0,-1,-1,-1,0,-1,-1,-1,2,1,0,-1},
        {2,1,0,-1,0,-1,-1,-1,0,-1,-1,-1,3,2,1,0},
        {2,1,0,-1,0,-1,-1,-1,1,0,-1,-1,0,-1,-1,-1},
        {2,1,0,-1,0,-1,-1,-1,1,0,-1,-1,1,0,-1,-1},
        {2,1,0,-1,0,-1,-1,-1,1,0,-1,-1,2,1,0,-1},
        {2,1,0,-1,0,-1,-1,-1,1,0,-1,-1,3,2,1,0},
        {2,1,0,-1,0,-1,-1,-1,2,1,0,-1,0,-1,-1,-1},
        {2,1,0,-1,0,-1,-1,-1,2,1,0,-1,1,0,-1,-1},
        {2,1,0,-1,0,-1,-1,-1,2,1,0,-1,2,1,0,-1},
        {2,1,0,-1,0,-1,-1,-1,2,1,0,-1,3,2,1,0},
        {2,1,0,-1,0,-1,-1,-1,3,2,1,0,0,-1,-1,-1},
        {2,1,0,-1,0,-1,-1,-1,3,2,1,0,1,0,-1,-1},
        {2,1,0,-1,0,-1,-1,-1,3,2,1,0,2,1,0,-1},
        {2,1,0,-1,0,-1,-1,-1,3,2,1,0,3,2,1,0},
        {2,1,0,-1,1,0,-1,-1,0,-1,-1,-1,0,-1,-1,-1},
        {2,1,0,-1,1,0,-1,-1,0,-1,-1,-1,1,0,-1,-1},
        {2,1,0,-1,1,0,-1,-1,0,-1,-1,-1,2,1,0,-1},
        {2,1,0,-1,1,0,-1,-1,0,-1,-1,-1,3,2,1,0},
        {2,1,0,-1,1,0,-1,-1,1,0,-1,-1,0,-1,-1,-1},
        {2,1,0,-1,1,0,-1,-1,1,0,-1,-1,1,0,-1,-1},
        {2,1,0,-1,1,0,-1,-1,1,0,-1,-1,2,1,0,-1},
        {2,1,0,-1,1,0,-1,-1,1,0,-1,-1,3,2,1,0},
        {2,1,0,-1,1,0,-1,-1,2,1,0,-1,0,-1,-1,-1},
        {2,1,0,-1,1,0,-1,-1,2,1,0,-1,1,0,-1,-1},
        {2,1,0,-1,1,0,-1,-1,2,1,0,-1,2,1,0,-1},
        {2,1,0,-1,1,0,-1,-1,2,1,0,-1,3,2,1,0},
        {2,1,0,-1,1,0,-1,-1,3,2,1,0,0,-1,-1,-1},
        {2,1,0,-1,1,0,-1,-1,3,2,1,0,1,0,-1,-1},
        {2,1,0,-1,1,0,-1,-1,3,2,1,0,2,1,0,-1},
        {2,1,0,-1,1,0,-1,-1,3,2,1,0,3,2,1,0},
        {2,1,0,-1,2,1,0,-1,0,-1,-1,-1,0,-1,-1,-1},
        {2,1,0,-1,2,1,0,-1,0,-1,-1,-1,1,0,-1,-1},
        {2,1,0,-1,2,1,0,-1,0,-1,-1,-1,2,1,0,-1},
        {2,1,0,-1,2,1,0,-1,0,-1,-1,-1,3,2,1,0},
        {2,1,0,-1,2,1,0,-1,1,0,-1,-1,0,-1,-1,-1},
        {2,1,0,-1,2,1,0,-1,1,0,-1,-1,1,0,-1,-1},
        {2,1,0,-1,2,1,0,-1,1,0,-1,-1,2,1,0,-1},
        {2,1,0,-1,2,1,0,-1,1,0,-1,-1,3,2,1,0},
        {2,1,0,-1,2,1,0,-1,2,1,0,-1,0,-1,-1,-1},
        {2,1,0,-1,2,1,0,-1,2,1,0,-1,1,0,-1,-1},
        {2,1,0,-1,2,1,0,-1,2,1,0,-1,2,1,0,-1},
        {2,1,0,-1,2,1,0,-1,2,1,0,-1,3,2,1,0},
        {2,1,0,-1,2,1,0,-1,3,2,1,0,0,-1,-1,-1},
        {2,1,0,-1,2,1,0,-1,3,2,1,0,1,0,-1,-1},
        {2,1,0,-1,2,1,0,-1,3,2,1,0,2,1,0,-1},
        {2,1,0,-1,2,1,0,-1,3,2,1,0,3,2,1,0},
        {2,1,0,-1,3,2,1,0,0,-1,-1,-1,0,-1,-1,-1},
        {2,1,0,-1,3,2,1,0,0,-1,-1,-1,1,0,-1,-1},
        {2,1,0,-1,3,2,1,0,0,-1,-1,-1,2,1,0,-1},
        {2,1,0,-1,3,2,1,0,0,-1,-1,-1,3,2,1,0},
        {2,1,0,-1,3,2,1,0,1,0,-1,-1,0,-1,-1,-1},
        {2,1,0,-1,3,2,1,0,1,0,-1,-1,1,0,-1,-1},
        {2,1,0,-1,3,2,1,0,1,0,-1,-1,2,1,0,-1},
        {2,1,0,-1,3,2,1,0,1,0,-1,-1,3,2,1,0},
        {2,1,0,-1,3,2,1,0,2,1,0,-1,0,-1,-1,-1},
        {2,1,0,-1,3,2,1,0,2,1,0,-1,1,0,-1,-1},
        {2,1,0,-1,3,2,1,0,2,1,0,-1,2,1,0,-1},
        {2,1,0,-1,3,2,1,0,2,1,0,-1,3,2,1,0},
        {2,1,0,-1,3,2,1,0,3,2,1,0,0,-1,-1,-1},
        {2,1,0,-1,3,2,1,0,3,2,1,0,1,0,-1,-1},
        {2,1,0,-1,3,2,1,0,3,2,1,0,2,1,0,-1},
        {2,1,0,-1,3,2,1,0,3,2,1,0,3,2,1,0},
        {3,2,1,0,0,-1,-1,-1,0,-1,-1,-1,0,-1,-1,-1},
        {3,2,1,0,0,-1,-1,-1,0,-1,-1,-1,1,0,-1,-1},
        {3,2,1,0,0,-1,-1,-1,0,-1,-1,-1,2,1,0,-1},
        {3,2,1,0,0,-1,-1,-1,0,-1,-1,-1,3,2,1,0},
        {3,2,1,0,0,-1,-1,-1,1,0,-1,-1,0,-1,-1,-1},
        {3,2,1,0,0,-1,-1,-1,1,0,-1,-1,1,0,-1,-1},
        {3,2,1,0,0,-1,-1,-1,1,0,-1,-1,2,1,0,-1},
        {3,2,1,0,0,-1,-1,-1,1,0,-1,-1,3,2,1,0},
        {3,2,1,0,0,-1,-1,-1,2,1,0,-1,0,-1,-1,-1},
        {3,2,1,0,0,-1,-1,-1,2,1,0,-1,1,0,-1,-1},
        {3,2,1,0,0,-1,-1,-1,2,1,0,-1,2,1,0,-1},
        {3,2,1,0,0,-1,-1,-1,2,1,0,-1,3,2,1,0},
        {3,2,1,0,0,-1,-1,-1,3,2,1,0,0,-1,-1,-1},
        {3,2,1,0,0,-1,-1,-1,3,2,1,0,1,0,-1,-1},
        {3,2,1,0,0,-1,-1,-1,3,2,1,0,2,1,0,-1},
        {3,2,1,0,0,-1,-1,-1,3,2,1,0,3,2,1,0},
        {3,2,1,0,1,0,-1,-1,0,-1,-1,-1,0,-1,-1,-1},
        {3,2,1,0,1,0,-1,-1,0,-1,-1,-1,1,0,-1,-1},
        {3,2,1,0,1,0,-1,-1,0,-1,-1,-1,2,1,0,-1},
        {3,2,1,0,1,0,-1,-1,0,-1,-1,-1,3,2,1,0},
        {3,2,1,0,1,0,-1,-1,1,0,-1,-1,0,-1,-1,-1},
        {3,2,1,0,1,0,-1,-1,1,0,-1,-1,1,0,-1,-1},
        {3,2,1,0,1,0,-1,-1,1,0,-1,-1,2,1,0,-1},
        {3,2,1,0,1,0,-1,-1,1,0,-1,-1,3,2,1,0},
        {3,2,1,0,1,0,-1,-1,2,1,0,-1,0,-1,-1,-1},
        {3,2,1,0,1,0,-1,-1,2,1,0,-1,1,0,-1,-1},
        {3,2,1,0,1,0,-1,-1,2,1,0,-1,2,1,0,-1},
        {3,2,1,0,1,0,-1,-1,2,1,0,-1,3,2,1,0},
        {3,2,1,0,1,0,-1,-1,3,2,1,0,0,-1,-1,-1},
        {3,2,1,0,1,0,-1,-1,3,2,1,0,1,0,-1,-1},
        {3,2,1,0,1,0,-1,-1,3,2,1,0,2,1,0,-1},
        {3,2,1,0,1,0,-1,-1,3,2,1,0,3,2,1,0},
        {3,2,1,0,2,1,0,-1,0,-1,-1,-1,0,-1,-1,-1},
        {3,2,1,0,2,1,0,-1,0,-1,-1,-1,1,0,-1,-1},
        {3,2,1,0,2,1,0,-1,0,-1,-1,-1,2,1,0,-1},
        {3,2,1,0,2,1,0,-1,0,-1,-1,-1,3,2,1,0},
        {3,2,1,0,2,1,0,-1,1,0,-1,-1,0,-1,-1,-1},
        {3,2,1,0,2,1,0,-1,1,0,-1,-1,1,0,-1,-1},
        {3,2,1,0,2,1,0,-1,1,0,-1,-1,2,1,0,-1},
        {3,2,1,0,2,1,0,-1,1,0,-1,-1,3,2,1,0},
        {3,2,1,0,2,1,0,-1,2,1,0,-1,0,-1,-1,-1},
        {3,2,1,0,2,1,0,-1,2,1,0,-1,1,0,-1,-1},
        {3,2,1,0,2,1,0,-1,2,1,0,-1,2,1,0,-1},
        {3,2,1,0,2,1,0,-1,2,1,0,-1,3,2,1,0},
        {3,2,1,0,2,1,0,-1,3,2,1,0,0,-1,-1,-1},
        {3,2,1,0,2,1,0,-1,3,2,1,0,1,0,-1,-1},
        {3,2,1,0,2,1,0,-1,3,2,1,0,2,1,0,-1},
        {3,2,1,0,2,1,0,-1,3,2,1,0,3,2,1,0},
        {3,2,1,0,3,2,1,0,0,-1,-1,-1,0,-1,-1,-1},
        {3,2,1,0,3,2,1,0,0,-1,-1,-1,1,0,-1,-1},
        {3,2,1,0,3,2,1,0,0,-1,-1,-1,2,1,0,-1},
        {3,2,1,0,3,2,1,0,0,-1,-1,-1,3,2,1,0},
        {3,2,1,0,3,2,1,0,1,0,-1,-1,0,-1,-1,-1},
        {3,2,1,0,3,2,1,0,1,0,-1,-1,1,0,-1,-1},
        {3,2,1,0,3,2,1,0,1,0,-1,-1,2,1,0,-1},
        {3,2,1,0,3,2,1,0,1,0,-1,-1,3,2,1,0},
        {3,2,1,0,3,2,1,0,2,1,0,-1,0,-1,-1,-1},
        {3,2,1,0,3,2,1,0,2,1,0,-1,1,0,-1,-1},
        {3,2,1,0,3,2,1,0,2,1,0,-1,2,1,0,-1},
        {3,2,1,0,3,2,1,0,2,1,0,-1,3,2,1,0},
        {3,2,1,0,3,2,1,0,3,2,1,0,0,-1,-1,-1},
        {3,2,1,0,3,2,1,0,3,2,1,0,1,0,-1,-1},
        {3,2,1,0,3,2,1,0,3,2,1,0,2,1,0,-1},
        {3,2,1,0,3,2,1,0,3,2,1,0,3,2,1,0}
};

uint8_t *
hz_blob_get_data(hz_blob_t *blob)
{
    return blob->data;
}

hz_stream_t
hz_blob_to_stream(hz_blob_t *blob) {
    return hz_stream_create(blob->data, blob->size);
}

/* Group: Arabic joining */
HZ_STATIC hz_bool_t
hz_is_arabic_codepoint(hz_unicode_t c)
{
    return (c >= 0x0600u && c <= 0x06FFu) || /* Arabic (0600–06FF) */
            (c >= 0x0750u && c <= 0x077Fu) || /* Arabic Supplement (0750–077F) */
            (c >= 0x08A0u && c <= 0x08FFu) || /* Arabic Extended-A (08A0–08FF) */
            (c >= 0xFB50u && c <= 0xFDFFu) || /* Arabic Presentation Forms-B (FE70–FEFF) */
            (c >= 0xFE70u && c <= 0xFEFFu) || /* Arabic Presentation Forms-B (FE70–FEFF) */
            (c >= 0x1EE00u && c <= 0x1EEFFu); /* Arabic Mathematical Alphabetic Symbols (1EE00–1EEFF) */
}

HZ_STATIC hz_bool_t
hz_shape_complex_arabic_char_joining(hz_unicode_t codepoint,
                                     uint16_t *joining)
{
    if (oz_lib_initialized) {
        hz_mph_map_t *lut = oz_lib.arabic_joining;
        uint32_t key = codepoint;

        uint32_t h1 = HashUInt32(0,key) % lut->N;
        if (hz_array_has(lut->buckets[h1], key, NULL))
        {
            int32_t d = lut->G[h1];
            if (d < 0)
                *joining = lut->values[-d-1];
            else {
                uint32_t h2 = HashUInt32(d, key) % lut->N;
                *joining = lut->values[h2];
            }

            return HZ_TRUE;
        }

    } else {
        if (hz_is_arabic_codepoint(codepoint)) {
            for (size_t index = 0; index < HZ_ARRLEN(hz_arabic_joining_list); ++index) {
                const hz_arabic_joining_entry_t *curr_entry = &hz_arabic_joining_list[index];

                if (curr_entry->codepoint == codepoint) {
                    // Found entry
                    *joining = curr_entry->joining;
                    return HZ_TRUE;
                }
            }
        }
    }

    return HZ_FALSE;
}

typedef struct {
    uint16_t start_glyph_id;
    uint16_t end_glyph_id;
    uint16_t start_coverage_index;
} hz_coverage_range_t;

typedef struct {
    union {
        uint16_t* glyph_indices;
        hz_coverage_range_t* ranges;
    };

    uint16_t count;
    uint16_t format;
} hz_coverage_t;

static inline void
hz_read_coverage(hz_allocator_t *alctr, hz_deserializer_t *ds, hz_coverage_t *cov)
{
    cov->format = hz_deserializer_read_u16(ds);

    if (cov->format == 1) {
        cov->count = hz_deserializer_read_u16(ds);
        cov->glyph_indices = hz_allocate(alctr, cov->count * sizeof(uint16_t));
        hz_deserializer_read_u16_block(ds, cov->glyph_indices, cov->count);
    } else if (cov->format == 2) {
        cov->count = hz_deserializer_read_u16(ds);
        cov->ranges = hz_allocate(alctr, cov->count * sizeof(hz_coverage_range_t));
        hz_deserializer_read_u16_block(ds, (uint16_t*) cov->ranges, cov->count * 3);
    } else {
        // error
    }
}

static inline int32_t
hz_coverage_search(const hz_coverage_t *coverage, uint16_t glyph_id)
{
    if (coverage->format == 1) {
        int32_t low = 0, high = coverage->count-1, mid = (low+high)/2;

        if (glyph_id < coverage->glyph_indices[low] || glyph_id > coverage->glyph_indices[high]) {
            // error, glyph not found within coverage
            return -1;
        }

        // binary search
        while (high >= low) {
            if (glyph_id < coverage->glyph_indices[mid]) {
                high = mid-1;
            } else if (glyph_id > coverage->glyph_indices[mid]) {
                low = mid+1;
            } else {
                return mid;
            }

            mid = (low+high)/2;
        }
    } else if (coverage->format == 2) {
        // Same as format 1, except binary search through the ranges which should be ordered
        // sequentially when read.
        int32_t low = 0, high = coverage->count-1, mid = (low+high)/2;
        hz_coverage_range_t *low_range = &coverage->ranges[low];
        hz_coverage_range_t *high_range = &coverage->ranges[high];
        hz_coverage_range_t *mid_range = &coverage->ranges[mid];

        if (glyph_id < low_range->start_glyph_id || glyph_id > high_range->end_glyph_id) {
            // early return as glyph cannot possibly be in this coverage.
            return -1;
        }

        while (high >= low) {
            low_range = &coverage->ranges[low];
            high_range = &coverage->ranges[high];
            mid_range = &coverage->ranges[mid];

            if (glyph_id < mid_range->start_glyph_id) {
                high = mid-1;
            } else if (glyph_id > mid_range->end_glyph_id) {
                low = mid+1;
            } else {
                // found the glyph range, compute coverage index
                return mid_range->start_coverage_index + glyph_id - mid_range->start_glyph_id;
            }

            mid = (low+high)/2;
        }
    }

    return -1;
}

#define hz_coverage_contains(c,g) (hz_coverage_search(c,g) != -1)

typedef enum hz_joining_dir_t {
    JOINING_NEXT,
    JOINING_PREV
} hz_joining_dir_t;

struct hz_face_t {
    unsigned char *data;
    unsigned int gpos,gsub,gdef,jstf,cmap,maxp,glyf,hmtx,kern;

    uint16_t num_glyphs;
    uint16_t num_of_h_metrics;
    uint16_t num_of_v_metrics;
    hz_metrics_t *metrics;

    int16_t ascender;
    int16_t descender;
    int16_t linegap;

    uint16_t upem;

    hz_memory_pool_t memory_pool;
    hz_allocator_t allocator;
    hz_map_t *class_map;
    hz_map_t *attach_class_map;
    hz_coverage_t *mark_glyph_set;
};

hz_face_t *
hz_face_create()
{
    hz_face_t *face = hz_malloc(sizeof(hz_face_t));
    face->num_glyphs = 0;
    face->num_of_h_metrics = 0;
    face->num_of_v_metrics = 0;
    face->metrics = NULL;
    face->ascender = 0;
    face->descender = 0;
    face->linegap = 0;
    face->upem = 0;
    face->memory_pool = hz_memory_pool_create(8192);
    face->allocator.allocate = &hz_memory_pool_allocate_func;
    face->allocator.user = &face->memory_pool;
    face->class_map = hz_map_create();
    face->attach_class_map = hz_map_create();
    face->mark_glyph_set = NULL;
    return face;
}

void
hz_face_destroy(hz_face_t *face)
{
    hz_map_destroy(face->class_map);
    hz_map_destroy(face->attach_class_map);
    hz_memory_pool_release(&face->memory_pool);
    hz_free(face);
}

uint16_t
hz_face_get_upem(hz_face_t *face)
{
    return face->upem;
}

void
hz_face_set_upem(hz_face_t *face, uint16_t upem)
{
    face->upem = upem;
}

void
hz_face_set_num_glyphs(hz_face_t *face, uint16_t num_glyphs)
{
    face->num_glyphs = num_glyphs;
}

uint16_t
hz_face_get_num_glyphs(hz_face_t *face)
{
    return face->num_glyphs;
}

hz_metrics_t* hz_face_get_glyph_metrics(hz_face_t* face, hz_index_t id)
{
    if (id < face->num_glyphs && face->metrics != NULL) {
        return face->metrics + id;
    }

    return NULL;
}

void
hz_face_set_num_of_h_metrics(hz_face_t *face, uint16_t num_of_h_metrics)
{
    face->num_of_h_metrics = num_of_h_metrics;
}

void
hz_face_load_num_glyphs(hz_face_t *face)
{
    hz_stream_t bs = hz_stream_create(face->data + face->maxp, 0);

    Version16Dot16 version;
    uint16_t num_glyphs;

    version = Unpack32(&bs);

    switch (version) {
        case 0x00005000: {
            /* version 0.5 */
            num_glyphs = Unpack16(&bs);
            break;
        }
        case 0x00010000: {
            /* version 1.0 with full information */
            num_glyphs = Unpack16(&bs);
            break;
        }
        default:
            /* error */
            break;
    }

    face->num_glyphs = num_glyphs;
}

void
hz_face_load_class_maps(hz_face_t *face)
{
    uint8_t arenamem[4096];
    hz_memory_arena_t arena = hz_memory_arena_create(arenamem, sizeof arenamem);
    if (face->gdef) {
        hz_deserializer_t ds = hz_deserializer_create(face->data, 1);
        hz_deserializer_push_state(&ds, face->gdef);

        struct { 
            Offset16 glyph_class_def_offset,
                     attach_list_offset,
                     lig_caret_list_offset,
                     mark_attach_class_def_offset,
                     mark_glyph_sets_def_offset; } hdr;

        Version16Dot16 version = hz_deserializer_read_u32(&ds);

        switch (version) {
            case 0x00010000: // 1.0
                cmdread(&ds, 1, &hdr, "wwww");
                break;
            case 0x00010002: // 1.2
                cmdread(&ds, 1, &hdr, "wwwww");
                break;
            case 0x00010003: // 1.3
                break;
            default: // error
                break;
        }

        if (hdr.glyph_class_def_offset) {
            // glyph class def isn't nil
            hz_deserializer_push_state(&ds, hdr.glyph_class_def_offset);
            uint16_t class_format = hz_deserializer_read_u16(&ds);
            switch (class_format) {
                case 1:
                    break;
                case 2: {
                    uint16_t range_index = 0, class_range_count;
                    class_range_count = hz_deserializer_read_u16(&ds);

                    while (range_index < class_range_count) {
                        struct {
                            uint16_t start_glyph_id, end_glyph_id, glyph_class;
                        } range;

                        cmdread(&ds, 1, &range, "www");

                        hz_map_set_value_for_keys(face->class_map,
                                                  range.start_glyph_id,
                                                  range.end_glyph_id, 1 << (range.glyph_class - 1));
                        ++range_index;
                    }
                    break;
                }
                default:
                    break;
            }
            hz_deserializer_pop_state(&ds);
        }

        if (hdr.mark_attach_class_def_offset) {
            hz_deserializer_push_state(&ds, hdr.mark_attach_class_def_offset);
            uint16_t class_format;
            class_format = hz_deserializer_read_u16(&ds);
            switch (class_format) {
                case 1:
                    break;
                case 2: {
                    uint16_t range_index = 0, class_range_count;
                    class_range_count = hz_deserializer_read_u16(&ds);

                    while (range_index < class_range_count) {
                        struct {
                            uint16_t start_glyph_id, end_glyph_id, glyph_class;
                        } range;

                        cmdread(&ds, 1, &range, "hhh");

                        hz_map_set_value_for_keys(face->attach_class_map,
                                                  range.start_glyph_id,
                                                  range.end_glyph_id, 1 << (range.glyph_class - 1));
                        ++range_index;
                    }
                    break;
                }
                default:
                    break;
            }
            hz_deserializer_pop_state(&ds);
        }

        if (hdr.mark_glyph_sets_def_offset) {
            hz_deserializer_push_state(&ds, hdr.mark_glyph_sets_def_offset);
            uint16_t format = hz_deserializer_read_u16(&ds);
            if (format == 1) {
                uint16_t mark_glyph_set_count = hz_deserializer_read_u16(&ds);
                if (mark_glyph_set_count) {
                    Offset32 *mark_glyph_set_offsets = hz_memory_arena_allocate(&arena, mark_glyph_set_count * sizeof(Offset32));
                    hz_deserializer_read_u32_block(&ds, mark_glyph_set_offsets, mark_glyph_set_count);

                    face->mark_glyph_set = hz_allocate(&face->allocator, mark_glyph_set_count * sizeof(hz_coverage_t));

                    for (int i = 0; i < mark_glyph_set_count; ++i) {
                        hz_coverage_t *coverage = &face->mark_glyph_set[i];
                        hz_deserializer_push_state(&ds, mark_glyph_set_offsets[i]);
                        hz_read_coverage(&face->allocator, &ds, coverage);
                        hz_deserializer_pop_state(&ds);
                    }
                }
            } else {
                // error
            }

            hz_deserializer_pop_state(&ds);
        }
    
        hz_deserializer_pop_state(&ds);
    }
}

typedef struct {
    unsigned horizontal : 1;
    unsigned minimum : 1;
    unsigned cross_stream : 1;
    unsigned override : 1;
    unsigned reserved1 : 4;
    unsigned format : 8;
} HzKernCoverageField;

typedef union {
    uint16_t data;
    HzKernCoverageField field;
} HzKernCoverage;

hz_error_t
hz_face_load_kerning_pairs(hz_face_t *face)
{
    hz_stream_t table;
    uint16_t version, i, n;

    if (!face->kern) {
        return HZ_ERROR_TABLE_DOES_NOT_EXIST;
    }

    table = hz_stream_create(face->data + face->kern, 0);
    version = Unpack16(&table);
    n = Unpack16(&table);

    if (version != 0) {
        return HZ_ERROR_INVALID_TABLE_VERSION;
    }

    for (i = 0; i < n; ++i) {
        uint16_t version, length;
        HzKernCoverageField coverage;
        Unpackv(&table, "hhh", &version, &length, &coverage);

        switch (coverage.format) {
            case 0:
                break;
            case 2:
                break;
            default:
                return HZ_ERROR_INVALID_FORMAT;
        }
    }

    return HZ_OK;
}

hz_glyph_class_t
hz_face_get_glyph_class(hz_face_t *face, hz_index_t id)
{
    /* TODO: This is slow, implement faster hash table */
    if (hz_map_contains(face->class_map, id)) {
        return hz_map_get_value(face->class_map, id);
    }

    return HZ_GLYPH_CLASS_BASE;
}

uint8_t
hz_face_get_glyph_attach_class(hz_face_t *face, hz_index_t id) {
    if (hz_map_contains(face->attach_class_map, id)) {
        return hz_map_get_value(face->attach_class_map, id);
    }

    return 0;
}

void
hz_face_load_upem(hz_face_t *face)
{

}

float
hz_face_line_skip(hz_face_t *face)
{
    return (float)(face->ascender - face->descender + face->linegap) / 64.0f;
}

HZ_STATIC const hz_tag_t required_ft_table_tags[] = {
        HZ_TAG('c','m','a','p'),
        HZ_TAG('m','a','x','p'),
        HZ_TAG('g','l','y','f'),
        HZ_TAG('h','m','t','x'),
        HZ_TAG('k','e','r','n'),
};

struct hz_font_t {
    hz_face_t *face;

    int32_t x_scale;
    int32_t y_scale;
    int64_t x_mult;
    int64_t y_mult;
    uint32_t x_ppem;
    uint32_t y_ppem;

    float ptem;
};

hz_font_t *
hz_font_create(void)
{
    hz_font_t *font = hz_malloc(sizeof(hz_font_t));
    font->face = NULL;
    font->x_ppem = 1000;
    font->y_ppem = 1000;
    font->ptem = 12.0f;
    font->x_scale = 0;
    font->y_scale = 0;
    return font;
}

void
hz_font_destroy(hz_font_t *font)
{
    hz_free(font);
}

void
hz_font_set_face(hz_font_t *font, hz_face_t *face)
{
    font->face = face;
}

hz_face_t *
hz_font_get_face(hz_font_t *font)
{
    return font->face;
}

hz_font_t *
hz_stbtt_font_create(stbtt_fontinfo *info)
{
    int g;
    hz_font_t *font;
    hz_face_t *face;

    font = hz_font_create();
    face = hz_face_create();
    face->data = info->data;

    face->gsub = stbtt__find_table(info->data,0,"GSUB");
    face->gpos = info->gpos;
    face->gdef = stbtt__find_table(info->data,0,"GDEF");
    face->maxp = stbtt__find_table(info->data,0,"maxp");
    face->glyf = info->glyf;
    face->cmap = stbtt__find_table(info->data,0,"cmap");
    face->kern = info->kern;

    face->num_glyphs = info->numGlyphs;

    face->metrics = hz_malloc(sizeof(hz_metrics_t) * face->num_glyphs);

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(info, &ascent, &descent, &lineGap);

    for (g = 0; g < face->num_glyphs; ++g) {
        /* how wide is this character */
        int ax;
        int lsb;
        stbtt_GetGlyphHMetrics(info, g, &ax, &lsb);

        int cx1, cy1, cx2, cy2;
        stbtt_GetGlyphBox(info, g, &cx1, &cy1, &cx2, &cy2);

        face->metrics[g].w = cx2 - cx1;
        face->metrics[g].h = cy2 - cy1;
        face->metrics[g].xAdvance = ax;
        face->metrics[g].yAdvance  = 0;
        face->metrics[g].xBearing = lsb;
    }

    hz_face_load_class_maps(face);
    hz_face_load_kerning_pairs(face);
    hz_font_set_face(font, face);

    return font;
}

uint16_t
hz_ignored_classes_from_lookup_flags(hz_lookup_flags_t flags)
{
    uint16_t ignored_classes = HZ_GLYPH_CLASS_ZERO;

    if (flags & HZ_LOOKUP_FLAG_IGNORE_BASE_GLYPHS) ignored_classes |= HZ_GLYPH_CLASS_BASE;
    if (flags & HZ_LOOKUP_FLAG_IGNORE_LIGATURES) ignored_classes |= HZ_GLYPH_CLASS_LIGATURE;
    if (flags & HZ_LOOKUP_FLAG_IGNORE_MARKS) ignored_classes |= HZ_GLYPH_CLASS_MARK;

    return ignored_classes;
}

hz_feature_t
hz_ot_feature_from_tag(hz_tag_t tag) {
    /* loop over feature tag lookup table for it's id */
    int i;
    const hz_feature_info_t *feature_info = NULL;

    for (i = 0; i < HZ_FEATURE_COUNT; ++i) {
        feature_info = &HZ_FEATURE_INFO_LUT[i];
        if (feature_info->tag == tag) {
            return feature_info->feature;
        }
    }

    return -1;
}

hz_tag_t
hz_ot_tag_from_feature(hz_feature_t feature) {
    /* loop over feature tag lookup table for it's id */
    int i;
    const hz_feature_info_t *feature_info = NULL;

    for (i = 0; i < HZ_FEATURE_COUNT; ++i) {
        feature_info = &HZ_FEATURE_INFO_LUT[i];
        if (feature_info->feature == feature) {
            return feature_info->tag;
        }
    }

    return 0;
}

typedef struct hz_rec16_t {
    hz_tag_t tag;
    uint16_t offset;
} Record16;

typedef struct hz_lang_sys_t {
    /* = NULL (reserved for an offset to a reordering table) */
    Offset16 lookup_order;

    /* Index of a feature required for this language system; if no required features = 0xFFFF */
    uint16_t required_feature_index;

    /* Number of feature index values for this language system — excludes the required feature */
    uint16_t feature_index_count;
} hz_lang_sys_t;

typedef struct hz_sequence_lookup_record_t {
    uint16_t sequence_index;
    uint16_t lookup_list_index;
} hz_sequence_lookup_record_t;

typedef struct hz_sequence_rule_t {
    uint16_t glyph_count;
    uint16_t seq_lookup_count;
    uint16_t *input_sequence;
    hz_sequence_lookup_record_t *seq_lookup_records;
} hz_sequence_rule_t;

typedef struct hz_sequence_rule_set_t {
    uint16_t rule_count;
    hz_sequence_rule_t *rules;
} hz_sequence_rule_set_t;

typedef struct hz_chained_sequence_rule_t {
    uint16_t prefix_count;
    uint16_t *prefix_sequence;
    uint16_t input_count;
    uint16_t *input_sequence;
    uint16_t suffix_count;
    uint16_t *suffix_sequence;
    uint16_t lookup_count;
    hz_sequence_lookup_record_t *lookup_records;
} hz_chained_sequence_rule_t;

typedef struct hz_chained_sequence_rule_set_t {
    uint16_t count;
    hz_chained_sequence_rule_t *rules;
} hz_chained_sequence_rule_set_t;

typedef struct {
    // = NULL (reserved for offset to FeatureParams)
    Offset16 feature_params;
    // Number of LookupList indices for this feature
    uint16_t lookup_index_count;
    // Array of indices into the LookupList — zero-based (first lookup is LookupListIndex = 0)
    uint16_t *lookup_list_indices;
} hz_feature_table_t;

static const uint8_t *
hz_ot_layout_choose_lang_sys(hz_face_t *face,
                             uint8_t *data,
                             hz_tag_t script,
                             hz_tag_t language)
{
    uint8_t buffer[4096];
    hz_memory_arena_t la = hz_memory_arena_create(buffer, sizeof(buffer));

    hz_stream_t subtable = hz_stream_create(data, 0);
    uint16_t script_count = 0;
    uint16_t index = 0;
    Record16 *script_records = NULL;
    uint16_t found_script = 0;
    const uint8_t *found_addr;

    script_count = Unpack16(&subtable);
    script_records = hz_memory_arena_allocate(&la, sizeof(Record16) * script_count);

    while (index < script_count) {
        hz_tag_t curr_tag;
        uint16_t curr_offset;

        Unpackv(&subtable, "ih", &curr_tag, &curr_offset);

        script_records[index].offset = curr_offset;
        script_records[index].tag = curr_tag;

        if (script == curr_tag) {
            found_script = index;
            break;
        }

        ++index;
    }

    /* Found script */
    uint16_t script_offset = script_records[found_script].offset;
    hz_stream_t script_stream = hz_stream_create(data + script_offset, 0);
    Offset16 default_lang_sys_offset;
    uint16_t lang_sys_count;
    Unpackv(&script_stream, "hh", &default_lang_sys_offset, &lang_sys_count);
    found_addr = script_stream.data + default_lang_sys_offset;

    uint16_t lang_sysIndex = 0;
    while (lang_sysIndex < lang_sys_count) {
        Record16 lang_sys_rec;
        Unpackv(&script_stream, "ih", &lang_sys_rec.tag,
                &lang_sys_rec.offset);

        if (lang_sys_rec.tag == language) {
            /* Found language system */
            found_addr = script_stream.data + lang_sys_rec.offset;
            break;
        }

        ++lang_sysIndex;
    }

    /* Couldn't find alterior language system, return default. */
    return found_addr;
}

void
hz_ot_layout_feature_get_lookups(uint8_t *data,
                                 hz_array_t *lookup_indices)
{
    int i = 0;
    hz_stream_t table = hz_stream_create(data, 0);
    hz_feature_table_t feature_table;
    Unpackv(&table, "hh", &feature_table.feature_params,
            &feature_table.lookup_index_count);

    for (i=0; i<feature_table.lookup_index_count; ++i) {
        hz_array_append(lookup_indices, Unpack16(&table));
    }
}

typedef struct {
    hz_tag_t tag;
    hz_feature_table_t table;
} hz_feature_list_item_t;

typedef struct {
    int16_t xPlacement;
    int16_t yPlacement;
    int16_t xAdvance;
    int16_t yAdvance;
    Offset16 xPlaDeviceOffset;
    Offset16 yPlaDeviceOffset;
    Offset16 xAdvDeviceOffset;
    Offset16 yAdvDeviceOffset;
} hz_value_record_t;


static void
hz_read_value_record(hz_stream_t *stream, hz_value_record_t *record, uint16_t valueFormat) {

    if (valueFormat & HZ_VALUE_FORMAT_X_PLACEMENT)
        record->xPlacement = (int16_t) Unpack16(stream);

    if (valueFormat & HZ_VALUE_FORMAT_Y_PLACEMENT)
        record->yPlacement = (int16_t) Unpack16(stream);

    if (valueFormat & HZ_VALUE_FORMAT_X_ADVANCE)
        record->xAdvance = (int16_t) Unpack16(stream);

    if (valueFormat & HZ_VALUE_FORMAT_Y_ADVANCE)
        record->yAdvance  = (int16_t) Unpack16(stream);

    if (valueFormat & HZ_VALUE_FORMAT_X_PLACEMENT_DEVICE)
        record->xPlaDeviceOffset = Unpack16(stream);

    if (valueFormat & HZ_VALUE_FORMAT_Y_PLACEMENT_DEVICE)
        record->yPlaDeviceOffset = Unpack16(stream);

    if (valueFormat & HZ_VALUE_FORMAT_X_ADVANCE_DEVICE)
        record->xAdvDeviceOffset = Unpack16(stream);

    if (valueFormat & HZ_VALUE_FORMAT_Y_ADVANCE_DEVICE)
        record->yAdvDeviceOffset = Unpack16(stream);
}

typedef struct hz_ot_single_pos_format1_table_t {
    hz_map_t *coverage;
    uint16_t value_format;
    hz_value_record_t value_record;
} hz_ot_single_pos_format1_table_t;

typedef struct hz_ot_single_pos_format2_table_t {
    hz_map_t *coverage;
    uint16_t value_format;
    uint16_t value_count;
    hz_value_record_t *value_records;
} hz_ot_single_pos_format2_table_t;

typedef struct hz_entry_exit_record_t {
    Offset16 entry_anchor_offset, exit_anchor_offset;
} hz_entry_exit_record_t;

typedef struct {
    int16_t xCoord, yCoord;
} hz_anchor_t;

typedef struct {
    hz_bool_t has_entry, has_exit;
    hz_anchor_t entry, exit;
} hz_anchor_pair_t;

typedef struct {
    uint16_t mark_class;
    hz_anchor_t mark_anchor;
} hz_mark_record_t;

typedef struct {
    hz_value_record_t value_record1;
    hz_value_record_t value_record2;
} hz_class2_record_t;

typedef struct {
    hz_class2_record_t *class2_records;
} hz_class1_record_t;

typedef struct {
    uint16_t second_glyph;
    hz_value_record_t value_record1;
    hz_value_record_t value_record2;
} hz_pair_value_record_t;

typedef struct {
    uint16_t pair_value_count;
    hz_pair_value_record_t *pair_value_records;
} hz_pair_set_t;

typedef struct {
    uint16_t format;
    hz_map_t *coverage;
    uint16_t value_format1;
    uint16_t value_format2;
    uint16_t pair_set_count;
    hz_pair_set_t *pair_sets;
} hz_pair_pos_format1_subtable_t;

typedef struct {
    uint16_t format;
    hz_map_t *coverage;
    uint16_t value_format1;
    uint16_t value_format2;
    hz_map_t *class_def1;
    hz_map_t *class_def2;
    uint16_t class1_count;
    uint16_t class2_count;
    hz_class1_record_t *class1_records;
} hz_pair_pos_format2_subtable_t;

void
hz_ot_read_single_pos_format1_table(hz_stream_t *stream, hz_ot_single_pos_format1_table_t *table) {
    Offset16 coverage_offset = Unpack16(stream);
    table->coverage = hz_map_create();
    table->value_format = Unpack16(stream);

    hz_read_coverage(stream->data + coverage_offset,
                     table->coverage,
                     NULL);

    hz_read_value_record(stream, &table->value_record, table->value_format);
}

void
hz_ot_free_single_pos_format1_table(hz_ot_single_pos_format1_table_t *table) {
    hz_map_destroy(table->coverage);
}

void
hz_ot_read_single_pos_format2_table(hz_stream_t *stream, hz_ot_single_pos_format2_table_t *table) {
    uint16_t i;
    Offset16 coverage_offset = Unpack16(stream);
    table->value_format = Unpack16(stream);
    table->value_count = Unpack16(stream);
    table->coverage = hz_map_create();

    hz_read_coverage(stream->data + coverage_offset,
                     table->coverage,
                     NULL);

    table->value_records = hz_malloc(sizeof(hz_value_record_t) * table->value_count);

    for (i = 0; i < table->value_count; ++i) {
        hz_read_value_record(stream, &table->value_records[i], table->value_format);
    }
}

void
hz_ot_free_single_pos_format2_table(hz_ot_single_pos_format2_table_t *table) {
    hz_map_destroy(table->coverage);
    hz_free(table->value_records);
}

hz_sequence_rule_t
hz_sequence_rule_create(void)
{
    hz_sequence_rule_t rule;

    rule.glyph_count = 0;
    rule.seq_lookup_count = 0;
    rule.input_sequence = NULL;
    rule.seq_lookup_records = NULL;

    return rule;
}

void
hz_sequence_rule_destroy(hz_sequence_rule_t *rule)
{
    if (rule->input_sequence != NULL) hz_free(rule->input_sequence);
    if (rule->seq_lookup_records != NULL) hz_free(rule->seq_lookup_records);
}

void
hz_read_sequence_lookup_record(hz_stream_t *buf,
                               hz_sequence_lookup_record_t *record)
{
    Unpackv(buf, "hh", &record->sequence_index, &record->lookup_list_index);
}

void
hz_parse_sequence_rule(uint8_t *data, hz_sequence_rule_t *rule) {
    size_t i;
    hz_stream_t buf = hz_stream_create(data, 0);

    Unpackv(&buf, "hh", &rule->glyph_count, &rule->seq_lookup_count);
    rule->input_sequence = hz_malloc(rule->glyph_count - 1);
    Unpackv(&buf, "h:*", rule->input_sequence, rule->glyph_count - 1);

    rule->seq_lookup_records = hz_malloc(sizeof(hz_sequence_lookup_record_t) * rule->seq_lookup_count);

    for (i = 0; i < rule->seq_lookup_count; ++i)
        hz_read_sequence_lookup_record(&buf, &rule->seq_lookup_records[i]);
}

void
hz_parse_sequence_rule_set(uint8_t *data, hz_sequence_rule_set_t *rule_set)
{
    uint8_t buffer[4096];
    hz_memory_arena_t arena = hz_memory_arena_create(buffer, sizeof buffer);
    hz_stream_t bs;
    uint8_t *rule_offsets;

    rule_offsets = hz_memory_arena_allocate(&arena,
                                   rule_set->rule_count * sizeof(uint16_t));

    bs = hz_stream_create(data, 0);
    rule_set->rule_count = Unpack16(&bs);

    if (rule_set->rule_count) {
        uint16_t rule_index;
        Unpackv(&bs, "h:*", rule_offsets, rule_set->rule_count);

        rule_set->rules = hz_malloc(sizeof(hz_sequence_rule_t) * rule_set->rule_count);

        for (rule_index = 0; rule_index < rule_set->rule_count; ++rule_index) {
            uint16_t rule_offset = rule_offsets[rule_index];
            if (rule_offset) {
                /* If the offset is not NULL */
                hz_parse_sequence_rule(bs.data + rule_offset, &rule_set->rules[rule_index]);
            }
        }
    }
}

void
hz_parse_chained_sequence_rule(uint8_t *data, hz_chained_sequence_rule_t *rule) {
    uint8_t buffer[4096];
    hz_memory_arena_t arena = hz_memory_arena_create(buffer, sizeof buffer);
    hz_stream_t buf;

    buf = hz_stream_create(data, 0);

    rule->prefix_count = Unpack16(&buf);
    rule->prefix_sequence = hz_malloc(sizeof(uint16_t) * rule->prefix_count);
    UnpackArray16(&buf, rule->prefix_count, rule->prefix_sequence);

    rule->input_count = Unpack16(&buf);
    rule->input_sequence = hz_malloc(sizeof(uint16_t) * (rule->input_count - 1));
    UnpackArray16(&buf, rule->input_count - 1, rule->input_sequence);

    rule->suffix_count = Unpack16(&buf);
    rule->suffix_sequence = hz_malloc(sizeof(uint16_t) * rule->suffix_count);
    UnpackArray16(&buf, rule->suffix_count, rule->suffix_sequence);

    rule->lookup_count = Unpack16(&buf);
    rule->lookup_records = hz_malloc(sizeof(hz_sequence_lookup_record_t) * rule->lookup_count);

    for (uint16_t i = 0; i < rule->lookup_count; ++i) {
        hz_read_sequence_lookup_record(&buf, rule->lookup_records + i);
    }
}

void
hz_parse_chained_sequence_rule_set(uint8_t *data, hz_chained_sequence_rule_set_t *rule_set) {
    hz_stream_t buf = hz_stream_create(data, 0);

    rule_set->count = Unpack16(&buf);
    Offset16* offsets = hz_malloc(sizeof(uint16_t) * rule_set->count);
    UnpackArray16(&buf,rule_set->count,offsets);

    rule_set->rules = hz_malloc(sizeof(hz_chained_sequence_rule_t) * rule_set->count);

    for (uint16_t i = 0; i < rule_set->count; ++i) {
        hz_parse_chained_sequence_rule(buf.data + offsets[i], rule_set->rules + i);
    }

    hz_free(offsets);
}

typedef struct hz_chained_sequence_context_format3_subtable_t {
    uint16_t format;
    uint16_t prefix_count;
    hz_map_t **prefix_maps; /* prefix coverages */
    uint16_t input_count;
    hz_map_t **input_maps; /* input coverages */
    uint16_t suffix_count;
    hz_map_t **suffix_maps; /* suffix coverages */
    uint16_t lookup_count;
    hz_sequence_lookup_record_t *lookup_records;
} hz_chained_sequence_context_format3_subtable_t;

void
hz_ot_load_chained_sequence_context_format3_subtable(hz_stream_t *stream,
                                                     hz_chained_sequence_context_format3_subtable_t *table)
{
    Offset16 *prefix_offsets;
    Offset16 *input_offsets;
    Offset16 *suffix_offsets;

    {
        /* read backtrack glyph offsets */
        table->prefix_count = Unpack16(stream);
        prefix_offsets = hz_malloc(table->prefix_count * sizeof(Offset16));
        UnpackArray16(stream, table->prefix_count, prefix_offsets);

        table->prefix_maps = hz_malloc(sizeof(hz_map_t *) * table->prefix_count);
        for (size_t i=0; i<table->prefix_count; ++i) {
            table->prefix_maps[i] = hz_map_create();
            hz_read_coverage(stream->data + prefix_offsets[i],
                             table->prefix_maps[i], NULL);
        }

    }

    {
        /* read input glyph offsets */
        table->input_count = Unpack16(stream);
        input_offsets = hz_malloc(table->input_count * sizeof(Offset16));
        UnpackArray16(stream,table->input_count, input_offsets);

        table->input_maps = hz_malloc(sizeof(hz_map_t *) * table->input_count);

        for (size_t i=0; i<table->input_count; ++i) {
            table->input_maps[i] = hz_map_create();
            hz_read_coverage(stream->data + input_offsets[i],
                             table->input_maps[i],
                             NULL);
        }
    }

    {
        /* read lookahead glyph offsets */
        table->suffix_count = Unpack16(stream);
        suffix_offsets = hz_malloc(table->suffix_count * sizeof(Offset16));
        UnpackArray16(stream, table->suffix_count,suffix_offsets);

        table->suffix_maps = hz_malloc(sizeof(hz_map_t *) * table->suffix_count);

        for (size_t i=0; i<table->suffix_count; ++i) {
            table->suffix_maps[i] = hz_map_create();
            hz_read_coverage(stream->data + suffix_offsets[i],
                             table->suffix_maps[i],
                             NULL);
        }

    }

    {
        /* read lookup records */
        table->lookup_count = Unpack16(stream);

        table->lookup_records = hz_malloc(sizeof(hz_sequence_lookup_record_t) * table->lookup_count);

        for (size_t i=0;i<table->lookup_count;++i) {
            hz_sequence_lookup_record_t *record = table->lookup_records + i;
            hz_read_sequence_lookup_record(stream, record);
        }
    }


    hz_free(input_offsets);
    hz_free(prefix_offsets);
    hz_free(suffix_offsets);
}

void
hz_ot_clear_chained_sequence_context_format3_table(hz_chained_sequence_context_format3_subtable_t *table) {
    size_t i;

    for (i=0; i<table->prefix_count; ++i) {
        hz_map_destroy(table->prefix_maps[i]);
    }

    for (i=0; i<table->input_count; ++i) {
        hz_map_destroy(table->input_maps[i]);
    }

    for (i=0; i<table->suffix_count; ++i) {
        hz_map_destroy(table->suffix_maps[i]);
    }

    hz_free(table->prefix_maps);
    hz_free(table->input_maps);
    hz_free(table->suffix_maps);
    hz_free(table->lookup_records);
}

typedef struct hz_sequence_table_t {
    uint16_t glyph_count;
    uint16_t *glyphs;
} hz_sequence_table_t;

void
hz_ot_read_sequence_table(uint8_t *data, hz_sequence_table_t *table) {
    hz_stream_t buf = hz_stream_create(data, 0);
    table->glyph_count = Unpack16(&buf);
    table->glyphs = hz_malloc(sizeof(uint16_t) * table->glyph_count);
    UnpackArray16(&buf, table->glyph_count,table->glyphs);
}

void
hz_ot_clear_sequence_table(hz_sequence_table_t *table) {
    hz_free(table->glyphs);
}

typedef struct hz_multiple_subst_format1_t {
    hz_map_t *coverage;
    uint16_t sequence_count;
    hz_sequence_table_t *segs;
} hz_multiple_subst_format1_t;


void
hz_ot_clear_multiple_subst_format1(hz_multiple_subst_format1_t *table) {
    size_t i;
    hz_map_destroy(table->coverage);

    for (i = 0; i < table->sequence_count; ++i) {
        hz_ot_clear_sequence_table(table->segs + i);
    }

    hz_free(table->segs);
}

typedef struct HzReverseChainSingleSubstFormat1 {
    hz_map_t *coverage;
    uint16_t prefix_count;
    hz_map_t **prefix_maps; /* prefix coverages */
    uint16_t suffix_count;
    hz_map_t **suffix_maps; /* suffix coverages */
    uint16_t glyph_count;
    uint16_t *glyphs;
} HzReverseChainSingleSubstFormat1;

static void
hz_read_reverse_chain_single_subst_format1(hz_stream_t *buf,
                                           HzReverseChainSingleSubstFormat1 *subst)
{
    uint8_t buffer[4096];
    hz_memory_arena_t arena = hz_memory_arena_create(buffer, sizeof buffer);
    Offset16             coverage_offset;
    Offset16            *prefix_coverage_offsets;
    Offset16            *suffix_coverage_offsets;
    uint16_t            i;

    coverage_offset = Unpack16(buf);
    subst->coverage = hz_map_create();
    hz_read_coverage(buf->data + coverage_offset, subst->coverage, NULL);

    subst->prefix_count = Unpack16(buf);
    prefix_coverage_offsets = hz_memory_arena_allocate(&arena, subst->prefix_count * sizeof(uint16_t));
    Unpackv(buf, "h:*", prefix_coverage_offsets, subst->prefix_count);
    subst->prefix_maps = hz_malloc(sizeof(hz_map_t *) * subst->prefix_count);
    for (i = 0; i < subst->prefix_count; ++i) {
        subst->prefix_maps[i] = hz_map_create();
        hz_read_coverage(buf->data + prefix_coverage_offsets[i], subst->prefix_maps[i], NULL);
    }

    subst->suffix_count = Unpack16(buf);
    suffix_coverage_offsets = hz_memory_arena_allocate(&arena, subst->suffix_count * sizeof(uint16_t));
    Unpackv(buf, "h:*", suffix_coverage_offsets, subst->suffix_count);
    subst->suffix_maps = hz_malloc(sizeof(hz_map_t *) * subst->suffix_count);
    for (i = 0; i < subst->suffix_count; ++i) {
        subst->suffix_maps[i] = hz_map_create();
        hz_read_coverage(buf->data + suffix_coverage_offsets[i], subst->suffix_maps[i], NULL);
    }

    subst->glyph_count = Unpack16(buf);
    subst->glyphs = hz_malloc(subst->glyph_count * sizeof(uint16_t));
    Unpackv(buf, "h:*", subst->glyphs, subst->glyph_count);
}

void
hz_free_reverse_chain_single_subst_format1(HzReverseChainSingleSubstFormat1 *subst)
{
    size_t i;
    hz_map_destroy(subst->coverage);

    for(i=0; i<subst->prefix_count; ++i) {
        hz_map_destroy(subst->prefix_maps[i]);
    }

    for(i=0; i<subst->suffix_count; ++i) {
        hz_map_destroy(subst->suffix_maps[i]);
    }

    hz_free(subst->prefix_maps);
    hz_free(subst->suffix_maps);
    hz_free(subst->glyphs);
}

hz_anchor_t
hz_read_anchor(const unsigned char *data) {
    hz_stream_t stream = hz_stream_create(data, 0);
    hz_anchor_t anchor;
    uint16_t format = Unpack16(&stream);
    HZ_ASSERT(format >= 1 && format <= 3);
    Unpackv(&stream, "hh", &anchor.xCoord, &anchor.yCoord);
    return anchor;
}

hz_anchor_pair_t
hz_ot_layout_read_anchor_pair(const uint8_t *subtable, const hz_entry_exit_record_t *rec) {
    hz_anchor_pair_t anchor_pair;

    anchor_pair.has_entry = rec->entry_anchor_offset ? 1 : 0;
    anchor_pair.has_exit = rec->exit_anchor_offset ? 1 : 0;

    if (anchor_pair.has_entry)
        anchor_pair.entry = hz_read_anchor(subtable + rec->entry_anchor_offset);

    if (anchor_pair.has_exit)
        anchor_pair.exit = hz_read_anchor(subtable + rec->exit_anchor_offset);

    return anchor_pair;
}

static hz_error_t
hz_read_class_def_table(const uint8_t *data, hz_map_t *class_map) {
    hz_stream_t table = hz_stream_create(data, 0);
    uint16_t class_format;
    class_format = Unpack16(&table);
    switch (class_format) {
        case 1:
            break;
        case 2: {
            uint16_t range_index = 0, class_range_count;
            class_range_count = Unpack16(&table);

            while (range_index < class_range_count) {
                uint16_t start_glyph_id, end_glyph_id, glyph_class;
                Unpackv(&table, "hhh", &start_glyph_id, &end_glyph_id, &glyph_class);
                hz_map_set_value_for_keys(class_map, start_glyph_id, end_glyph_id, glyph_class);
                ++range_index;
            }
            break;
        }
        default:
            return HZ_ERROR_INVALID_FORMAT;
    }

    return HZ_OK;
}

static const hz_script_t complex_script_list[] = {
    HZ_SCRIPT_ARABIC,
    HZ_SCRIPT_BUGINESE,
    HZ_SCRIPT_HANGUL,
    HZ_SCRIPT_HEBREW,
    HZ_SCRIPT_BENGALI,
    HZ_SCRIPT_DEVANAGARI,
    HZ_SCRIPT_GUJARATI,
    HZ_SCRIPT_GURMUKHI,
    HZ_SCRIPT_KANNADA,
    HZ_SCRIPT_MALAYALAM,
    HZ_SCRIPT_ODIA,
    HZ_SCRIPT_TAMIL,
    HZ_SCRIPT_TELUGU,
    HZ_SCRIPT_JAVANESE,
    HZ_SCRIPT_KHMER,
    HZ_SCRIPT_LAO,
    HZ_SCRIPT_MYANMAR,
    HZ_SCRIPT_SINHALA,
    HZ_SCRIPT_SYRIAC,
    HZ_SCRIPT_THAANA,
    HZ_SCRIPT_THAI,
    HZ_SCRIPT_TIBETAN
};

typedef enum hz_feature_flag_t {
    HZ_FEATURE_FLAG_NONE = 0,
    HZ_FEATURE_FLAG_REQUIRED = 0x00000001,
    HZ_FEATURE_FLAG_ALWAYS_APPLIED = 0x00000002,
    HZ_FEATURE_FLAG_ON_BY_DEFAULT = 0x00000004,
    HZ_FEATURE_FLAG_OFF_BY_DEFAULT = 0x00000008
} hz_feature_flag_t;

typedef struct hz_feature_layout_op_t {
    hz_feature_t feature;
    hz_tag_t layout_op; // 'GSUB' or 'GPOS'
    uint8_t flags;
} hz_feature_layout_op_t;

static const hz_feature_layout_op_t std_feature_ops_arabic[] = {
    /* Language based forms */
    { HZ_FEATURE_CCMP, HZ_TAG_GSUB,    HZ_FEATURE_FLAG_ALWAYS_APPLIED },
    { HZ_FEATURE_ISOL, HZ_TAG_GSUB,    HZ_FEATURE_FLAG_ALWAYS_APPLIED },
    { HZ_FEATURE_FINA, HZ_TAG_GSUB,    HZ_FEATURE_FLAG_ALWAYS_APPLIED },
    { HZ_FEATURE_MEDI, HZ_TAG_GSUB,    HZ_FEATURE_FLAG_ALWAYS_APPLIED },
    { HZ_FEATURE_INIT, HZ_TAG_GSUB, HZ_FEATURE_FLAG_ALWAYS_APPLIED },
    { HZ_FEATURE_RLIG, HZ_TAG_GSUB, HZ_FEATURE_FLAG_ALWAYS_APPLIED },
    { HZ_FEATURE_RCLT, HZ_TAG_GSUB, HZ_FEATURE_FLAG_ALWAYS_APPLIED },
    { HZ_FEATURE_CALT, HZ_TAG_GSUB, HZ_FEATURE_FLAG_ALWAYS_APPLIED},
    /* Typographical forms */
    { HZ_FEATURE_LIGA, HZ_TAG_GSUB, HZ_FEATURE_FLAG_ON_BY_DEFAULT },
    { HZ_FEATURE_DLIG, HZ_TAG_GSUB, HZ_FEATURE_FLAG_OFF_BY_DEFAULT },
    { HZ_FEATURE_CSWH, HZ_TAG_GSUB, HZ_FEATURE_FLAG_OFF_BY_DEFAULT },
    { HZ_FEATURE_MSET, HZ_TAG_GSUB, HZ_FEATURE_FLAG_ALWAYS_APPLIED },
    /* Positioning features */
    { HZ_FEATURE_CURS, HZ_TAG_GPOS, HZ_FEATURE_FLAG_ALWAYS_APPLIED },
    { HZ_FEATURE_KERN, HZ_TAG_GPOS, HZ_FEATURE_FLAG_ALWAYS_APPLIED },
    { HZ_FEATURE_MARK, HZ_TAG_GPOS, HZ_FEATURE_FLAG_ALWAYS_APPLIED },
    { HZ_FEATURE_MKMK, HZ_TAG_GPOS, HZ_FEATURE_FLAG_ALWAYS_APPLIED }
};

static const hz_feature_layout_op_t std_feature_ops_buginese[] = {
    /* Localized forms */
    { HZ_FEATURE_LOCL, HZ_TAG_GSUB, HZ_FEATURE_FLAG_NONE },
    /* Presentation forms */
    { HZ_FEATURE_CCMP, HZ_TAG_GSUB, HZ_FEATURE_FLAG_REQUIRED },
    { HZ_FEATURE_RLIG, HZ_TAG_GSUB, HZ_FEATURE_FLAG_REQUIRED },
    { HZ_FEATURE_LIGA, HZ_TAG_GSUB, HZ_FEATURE_FLAG_NONE },
    { HZ_FEATURE_CLIG, HZ_TAG_GSUB, HZ_FEATURE_FLAG_NONE },
    /* Kerning */
    { HZ_FEATURE_KERN, HZ_TAG_GPOS, HZ_FEATURE_FLAG_NONE },
    { HZ_FEATURE_DIST, HZ_TAG_GPOS, HZ_FEATURE_FLAG_REQUIRED },
    /* Mark placement */
    { HZ_FEATURE_MARK, HZ_TAG_GPOS, HZ_FEATURE_FLAG_REQUIRED },
    { HZ_FEATURE_MKMK, HZ_TAG_GPOS, HZ_FEATURE_FLAG_REQUIRED }
};

static const hz_feature_layout_op_t std_feature_ops_hangul[] = {
    /* Language based forms */
    { HZ_FEATURE_CCMP, HZ_TAG_GSUB, HZ_FEATURE_FLAG_NONE },
    { HZ_FEATURE_LJMO, HZ_TAG_GSUB, HZ_FEATURE_FLAG_REQUIRED },
    { HZ_FEATURE_VJMO, HZ_TAG_GSUB, HZ_FEATURE_FLAG_REQUIRED },
    { HZ_FEATURE_TJMO, HZ_TAG_GSUB, HZ_FEATURE_FLAG_REQUIRED }
};

static const hz_feature_layout_op_t std_feature_ops_hebrew[] = {
    /* Language based forms */
    { HZ_FEATURE_CCMP, HZ_TAG_GSUB, HZ_FEATURE_FLAG_NONE },
    /* Typographical forms */
    { HZ_FEATURE_DLIG, HZ_TAG_GSUB, HZ_FEATURE_FLAG_NONE },
    /* Positioning features */
    { HZ_FEATURE_KERN, HZ_TAG_GPOS, HZ_FEATURE_FLAG_NONE },
    { HZ_FEATURE_MARK, HZ_TAG_GPOS, HZ_FEATURE_FLAG_REQUIRED }
};

/* TODO: complete remaining tables */

typedef struct hz_script_feature_order_t {
    hz_script_t script;
    const hz_feature_layout_op_t *ops;
    size_t num_ops;
} hz_script_feature_order_t;

static const hz_script_feature_order_t complex_script_feature_orders[] = {
    { HZ_SCRIPT_ARABIC, HZ_UNARR(std_feature_ops_arabic) },
    { HZ_SCRIPT_BUGINESE, HZ_UNARR(std_feature_ops_buginese) },
    { HZ_SCRIPT_HANGUL, HZ_UNARR(std_feature_ops_hangul) },
    { HZ_SCRIPT_HEBREW, HZ_UNARR(std_feature_ops_hebrew) }
};

/* https://docs.microsoft.com/en-us/typography/script-development/standard */
static const hz_feature_layout_op_t simple_script_feature_orders[] = {
    { HZ_FEATURE_CCMP, HZ_TAG_GSUB, HZ_FEATURE_FLAG_NONE },
    { HZ_FEATURE_LIGA, HZ_TAG_GSUB, HZ_FEATURE_FLAG_NONE },
    { HZ_FEATURE_CLIG, HZ_TAG_GSUB, HZ_FEATURE_FLAG_NONE },
    { HZ_FEATURE_DIST, HZ_TAG_GPOS, HZ_FEATURE_FLAG_REQUIRED },
    { HZ_FEATURE_KERN, HZ_TAG_GPOS, HZ_FEATURE_FLAG_REQUIRED },
    { HZ_FEATURE_MARK, HZ_TAG_GPOS, HZ_FEATURE_FLAG_NONE },
    { HZ_FEATURE_MKMK, HZ_TAG_GPOS, HZ_FEATURE_FLAG_REQUIRED },
};

hz_bool_t
hz_ot_is_complex_script(hz_script_t script)
{
    int i;

    for (i = 0; i < HZ_ARRLEN(complex_script_list); ++i) {
        if (complex_script_list[i] == script) {
            return HZ_TRUE;
        }
    }

    return HZ_FALSE;
}

void
hz_ot_script_load_features(hz_script_t script, hz_feature_t **featuresptr, unsigned int *countptr)
{
    unsigned int i, j, inclmask;

    inclmask = HZ_FEATURE_FLAG_REQUIRED | HZ_FEATURE_FLAG_ALWAYS_APPLIED
        | HZ_FEATURE_FLAG_ON_BY_DEFAULT;

    if (hz_ot_is_complex_script(script)) {
        for (i=0; i<HZ_ARRLEN(complex_script_feature_orders); ++i) {
            hz_script_feature_order_t order = complex_script_feature_orders[i];
            if (order.script == script) {
                unsigned int cnt = 0;

                // count required, on by default and always applied features
                for (j=0;j<order.num_ops;++j)
                    if (order.ops[j].flags & inclmask)
                        ++cnt;

                if (cnt) {
                    unsigned int f;
                    *featuresptr = hz_malloc(cnt * sizeof(hz_feature_t));
                    *countptr = cnt;

                    // again, go over the list and copy features
                    for (j=f=0; f<cnt && j<order.num_ops; ++j) {
                        if (order.ops[j].flags & inclmask) {
                            (*featuresptr)[f] = order.ops[j].feature;
                            ++f;
                        }
                    }
                }

                break;
            }
        }
    } else {
        /* standard scripts (Latin, Cyrillic, Greek, etc) */
    }
}

HZ_STATIC hz_tag_t
hz_language_to_ot_tag(hz_language_t lang)
{
    const HzLanguageMap *langmap;
    size_t i;

    for (i = 0; i < HZ_ARRLEN(language_map_list); ++i) {
        langmap = &language_map_list[i];

        if (langmap->language == lang) {
            return langmap->tag;
        }
    }

    return 0;
}

/*  Enum: hz_cmap_platform_t
 *      <https://docs.microsoft.com/en-us/typography/opentype/spec/cmap#platform-ids>
 */
typedef enum {
    /* Various */
    HZ_CMAP_PLATFORM_UNICODE = 0,
    /* Script manager code */
    HZ_CMAP_PLATFORM_MACINTOSH = 1,
    /* ISO encoding [deprecated] */
    HZ_CMAP_PLATFORM_ISO = 2,
    /* Windows encoding */
    HZ_CMAP_PLATFORM_WINDOWS = 3,
    /* Custom */
    HZ_CMAP_PLATFORM_CUSTOM = 4,
    /* Platform ID values 240 through 255 are reserved for user-defined platforms.
     * This specification will never assign these values to a registered platform.
     * Platform ID 2 (ISO) was deprecated as of OpenType version v1.3.
     */
} hz_cmap_platform_t;

typedef enum hz_cmap_subtable_format_t {
    HZ_CMAP_SUBTABLE_FORMAT_BYTE_ENCODING_TABLE = 0,
    HZ_CMAP_SUBTABLE_FORMAT_SEGMENT_MAPPING_TO_DELTA_VALUES = 4
} hz_cmap_subtable_format_t;

typedef struct {
    uint16_t platform_id; /* Platform ID. */
    uint16_t encoding_id; /* Platform-specific encoding ID. */
    Offset32 subtable_offset; /* Byte offset from beginning of table to the subtable for this encoding. */
} hz_cmap_encoding_t;

static const char *
hz_cmap_platform_to_string(hz_cmap_platform_t platform) {
    switch (platform) {
        case HZ_CMAP_PLATFORM_UNICODE: return "Unicode";
        case HZ_CMAP_PLATFORM_MACINTOSH: return "Macintosh";
        case HZ_CMAP_PLATFORM_ISO: return "ISO";
        case HZ_CMAP_PLATFORM_WINDOWS: return "Windows";
        case HZ_CMAP_PLATFORM_CUSTOM: return "Custom";
        default: return NULL;
    }
}

/* Format 0: Byte encoding table
 * https://docs.microsoft.com/en-us/typography/opentype/spec/cmap#format-0-byte-encoding-table
 */
typedef struct {
    uint16_t format; // Format number is set to 0.
    uint16_t length; // This is the length in bytes of the subtable.
    /* For requirements on use of the language field,
     * see “Use of the language field in 'cmap' subtables” in this document.
     */
    uint16_t language;
    // An array that maps character codes to glyph index values.
    uint8_t glyph_id_array[256];
} hz_cmap_format0_subtable_t;


typedef struct {
    uint16_t format;
    uint16_t length;
    uint16_t language;
    uint16_t segCountX2;
    uint16_t searchRange;
    uint16_t entrySelector;
    uint16_t rangeShift;
    uint16_t *endCode;
    uint16_t reservedPad;
    uint16_t *startCode;
    int16_t *idDelta;
    uint16_t *idRangeOffsets;
    uint16_t *glyphIdArray;
} hz_cmap_format4_subtable_t;

hz_index_t hz_map_unicode_to_id(hz_cmap_format4_subtable_t *subtable, hz_unicode_t codepoint) {
    uint16_t segCount = subtable->segCountX2/2;
    uint16_t i = 0;
    hz_index_t id;

    /* search for code range */
    while (i < segCount) {
        uint16_t start_code = subtable->startCode[i];
        uint16_t end_code = subtable->endCode[i];
        int16_t id_delta = subtable->idDelta[i];
        uint16_t id_range_offset = subtable->idRangeOffsets[i];

        if (codepoint >= start_code && codepoint <= end_code) {
            if (id_range_offset != 0) {
                uint16_t offset = *(subtable->glyphIdArray + (id_range_offset / 2 + (codepoint - start_code) - (segCount - i)));
                id = check_cpu_le() ? bswap16(offset) : offset;
                if (id != 0) id += id_delta;
            } else {
                id = codepoint + id_delta;
            }

            return id;
        }

        ++i;
    }

    return 0; // map to .notdef
}

#if HZ_ARCH & HZ_ARCH_AVX2_BIT
void avx256_print_16bit(const char * prefix, const char *fmt, const __m256i *val)
{
    uint16_t *p = (uint16_t *)val;

    #pragma GCC unroll 16
    for (size_t i = 0; i < 16; ++i) {
        printf(fmt, p[i]);
    }

    printf("(%s)\n", prefix);
}

void avx256_print_i16(const char *prefix, const __m256i *val)
{
    int16_t *p = (int16_t *)val;

    #pragma GCC unroll 16
    for (size_t i = 0; i < 16; ++i) {
        printf("%d ", p[i]);
    }

    printf("(%s)\n", prefix);

}

HZ_STATIC __m256i
simulate__mm256_i16gather_epi16(short const* base_addr, __m256i vindex, const int scale)
{
    __m256i vout;
    unsigned short out[16], index_array[16];

    _mm256_storeu_si256((__m256i *)index_array, vindex);

    #pragma GCC unroll 16
    for (size_t i = 0; i < 16; ++i) {
        out[i] = *(unsigned short *)((char *)base_addr + index_array[i] * scale);
    }

    vout = _mm256_loadu_si256((__m256i *)out);
    return vout;
}

HZ_INLINE HZ_STATIC __m256i
_mm256_bswap16(__m256i a)
{
    return _mm256_or_si256(_mm256_slli_epi16(a, 8), _mm256_srli_epi16(a, 8));
}

#endif

HZ_STATIC void
x86cpuid (long int a[4], long int fid)
{

}

HZ_STATIC int setup_x86cpu (void)
{
    long int result[4];
    x86cpuid(result, 0);
}

HZ_STATIC void setup_armcpu (void)
{

}

hz_error_t hz_setup (hz_setup_flags_t flags)
{
    if (flags & HZ_USE_CPUID_FOR_SIMD_CHECKS) {
#if  (HZ_ARCH & HZ_ARCH_X86)
        setup_x86cpu();
#elif (HZ_ARCH & HZ_ARCH_ARM_BIT)
        setup_armcpu();
#endif
    } else {
        // use standard #ifdef checks to determining CPU capabilities
    }

    hz_optimize();
}

void hz_cleanup(void)
{
}


#if HZ_ARCH & HZ_ARCH_AVX2_BIT
// AVX2 codepoint to glyph index convert function using TrueType's cmap format 4 subtable
// Assumes the codepoint arrays are sorted
// Supports only the Unicode Basic Multilingual Plane (U+0000 to U+FFFF)
HZ_STATIC void
hz_apply_cmap_format4_subtable_avx2(const hz_cmap_format4_subtable_t *subtable,
                                    uint16_t outdata[],
                                    const uint32_t indata[],
                                    size_t count)
{
    size_t dataptr;
    uint16_t segment_count = subtable->segCountX2 / 2;
#if HZ_RELY_ON_UNSAFE_CMAP_CONSTANTS
    // Rely on binary search data embedded in the OpenType table
#else
    // Manually calculate binary search constants
#endif

    #pragma clang loop unroll(enable)
    #pragma GCC ivdep
    for (dataptr = 0; dataptr + 16 <= count; dataptr += 16) {
        __m256i v1 = _mm256_loadu_si256((const __m256i *)(indata + dataptr)); // read first 8 32-bit values
        __m256i v2 = _mm256_loadu_si256((const __m256i *)(indata + dataptr + 8)); // read following 8 32-bit values
        __m256i vin = _mm256_packus_epi32(v1, v2);//_mm256_permute4x64_epi64(_mm256_packus_epi32(v1,v2), 0xd8); // 0b11011000 [0,2,1,3] -> 0xd8

        // Binary search for 16 elements
        __m256i start, end, mid, done_mask, found_indices, found_start;

        start = _mm256_setzero_si256();
        found_start = _mm256_setzero_si256();
        found_indices = _mm256_setzero_si256();
        done_mask = _mm256_setzero_si256();
        end = _mm256_set1_epi16(segment_count-1);

        // while(!done)
        while (likely(~_mm256_movemask_epi8(_mm256_or_si256(_mm256_cmpgt_epi16(start, end), done_mask)))) {
            __m256i segment_start, segment_end;

            mid = _mm256_adds_epu16(start, _mm256_srli_epi16(_mm256_adds_epu16(_mm256_subs_epu16(end, start),
                              _mm256_set1_epi16(1)), 1)); // start + (start-end)/2 to avoid overflow


            // read in values at mid
            segment_start = simulate__mm256_i16gather_epi16((short const *)subtable->startCode, mid, 2);
            segment_end = simulate__mm256_i16gather_epi16((short const *)subtable->endCode, mid, 2);

            // Update search parameters
            // !(x == start) && start > x
            __m256i lt_cond = _mm256_andnot_si256(_mm256_cmpeq_epi16(segment_start, vin), _mm256_cmpgt_epi16(segment_start, vin));
            __m256i gt_cond = _mm256_cmpgt_epi16(vin, segment_end);
            __m256i within_cond = _mm256_xor_si256(_mm256_or_si256(lt_cond, gt_cond), _mm256_set1_epi16(0xffffu)); // x >= start && x <= end

            // Update parameters
            __m256i blend_mask = _mm256_andnot_si256(done_mask, within_cond);
            done_mask = _mm256_or_si256(done_mask, blend_mask);
            found_indices = _mm256_or_si256(found_indices, _mm256_and_si256(blend_mask, mid));//_mm256_or_si256(found_indices, _mm256_and_si256(blend_mask, mid));
            found_start = _mm256_or_si256(found_start, _mm256_and_si256(blend_mask, segment_start));

            // if (x > A[mid]) start = mid + 1
            start = _mm256_or_si256(_mm256_andnot_si256(gt_cond, start),
            _mm256_and_si256(_mm256_adds_epi16(mid, _mm256_set1_epi16(1)), gt_cond));

            // if (x < A[mid]) end = mid - 1
            end = _mm256_or_si256(_mm256_andnot_si256(lt_cond, end),
            _mm256_and_si256(_mm256_subs_epi16(mid, _mm256_set1_epi16(1)), lt_cond));
        }

        {
            // Find glyph index for the codepoints
            __m256i id_delta, id_range_offsets;

            id_delta = simulate__mm256_i16gather_epi16((short const *)subtable->idDelta,
                                                       found_indices,
                                                       2);

            id_range_offsets = simulate__mm256_i16gather_epi16((short const *)subtable->idRangeOffsets,
                                                       found_indices,
                                                       2);

            __m256i offset_cond = _mm256_cmpgt_epi16(id_range_offsets, _mm256_setzero_si256());
            __m256i mask1 = _mm256_andnot_si256(offset_cond, done_mask);
            __m256i mask2 = _mm256_and_si256(offset_cond, done_mask);

            __m256i final_id = _mm256_and_si256(mask1, _mm256_add_epi16(vin, id_delta));

            // Compute the index into sutable->glyph_id_array

            // P/2 + (x - S) - (R - y)
            // P : offset
            // S : start code of the range
            // R : range count
            // x : codepoint
            // y : range index
            __m256i glyph_id_index = _mm256_add_epi16(_mm256_srli_epi16(id_range_offsets, 1),
                _mm256_sub_epi16(_mm256_sub_epi16(vin, found_start),
                _mm256_sub_epi16(_mm256_set1_epi16(segment_count), found_indices)));

            glyph_id_index = _mm256_and_si256(glyph_id_index, mask2);

            __m256i glyph_id = _mm256_bswap16(simulate__mm256_i16gather_epi16((short const *)subtable->glyphIdArray,
                                                                                 glyph_id_index,
                                                                                 2));

            final_id = _mm256_or_si256(final_id, _mm256_and_si256(_mm256_add_epi16(glyph_id, id_delta), mask2));
            final_id = _mm256_permute4x64_epi64(final_id, 0xd8); // 0b11011000 [0,2,1,3] -> 0xd8
            _mm256_storeu_si256((__m256i *)(outdata + dataptr), final_id);
        }
    }
}

#endif

HZ_STATIC void
hz_apply_cmap_format4_subtable(hz_cmap_format4_subtable_t *subtable,
                               hz_index_t glyph_indices[],
                               hz_unicode_t codepoints[],
                               size_t size)
{
    for (size_t i = 0; i < size; ++i) {
        // map unicode characters to glyph indices in sequence
        glyph_indices[i] = hz_map_unicode_to_id(subtable, codepoints[i]);
    }
}

HZ_STATIC hz_bool_t
hz_apply_cmap_encoding(hz_stream_t *table,
                       hz_cmap_encoding_t encoding,
                       hz_index_t glyphIndices[],
                       hz_unicode_t codepoints[],
                       size_t size)
{
    hz_stream_t subtable = hz_stream_create(table->data + encoding.subtable_offset, 0);
    uint16_t format = Unpack16(&subtable);

    switch (format) {
        case HZ_CMAP_SUBTABLE_FORMAT_BYTE_ENCODING_TABLE: break;
        case 2: break;
        case HZ_CMAP_SUBTABLE_FORMAT_SEGMENT_MAPPING_TO_DELTA_VALUES: {
            hz_cmap_format4_subtable_t cmapSubtable;
            size_t i;

            Unpackv(&subtable, "hhhhhh",
                    &cmapSubtable.length,
                    &cmapSubtable.language,
                    &cmapSubtable.segCountX2,
                    &cmapSubtable.searchRange,
                    &cmapSubtable.entrySelector,
                    &cmapSubtable.rangeShift);

            uint16_t numSegments = cmapSubtable.segCountX2 / 2;
            size_t table_size = numSegments * sizeof(uint16_t);

            cmapSubtable.endCode = hz_malloc(table_size);
            cmapSubtable.startCode = hz_malloc(table_size);
            cmapSubtable.idDelta = hz_malloc(table_size);
            cmapSubtable.idRangeOffsets = hz_malloc(table_size);

            UnpackArray16(&subtable, numSegments, cmapSubtable.endCode);
            cmapSubtable.reservedPad = Unpack16(&subtable);
            UnpackArray16(&subtable, numSegments, cmapSubtable.startCode);
            UnpackArray16(&subtable, numSegments, (uint16_t *) cmapSubtable.idDelta);
            UnpackArray16(&subtable, numSegments, cmapSubtable.idRangeOffsets);
            cmapSubtable.glyphIdArray = (uint16_t *)(subtable.data + subtable.ptr);

#if HZ_CONFIG_USE_SIMD
            #if HZ_ARCH & HZ_ARCH_AVX2_BIT
            hz_apply_cmap_format4_encoding_unaligned_avx2(&cmapSubtable, seg->glyph_indices, seg->codepoints, seg->num_codepoints);
            #else
            #endif
#else
            hz_apply_cmap_format4_subtable(&cmapSubtable, glyphIndices, codepoints, size);
#endif

            hz_free(cmapSubtable.endCode);
            hz_free(cmapSubtable.startCode);
            hz_free(cmapSubtable.idDelta);
            hz_free(cmapSubtable.idRangeOffsets);

            break;
        }
        default:
            return HZ_FALSE;
    }

    return HZ_TRUE;
}

HZ_STATIC void
hz_map_to_nominal_forms(hz_face_t *face,
                        hz_index_t glyph_indices[],
                        hz_unicode_t codepoints[],
                        size_t size)
{
    hz_stream_t table = hz_stream_create(face->data + face->cmap, 0);
    uint16_t version = Unpack16(&table);

    // Table version number must be 0
    HZ_ASSERT(version == 0);

    uint16_t num_encodings, enc_idx;
    num_encodings = Unpack16(&table);

    {
        hz_cmap_encoding_t encoding = {0};
        Unpackv(&table, "hhi",
                &encoding.platform_id,
                &encoding.encoding_id,
                &encoding.subtable_offset);

        hz_apply_cmap_encoding(&table, encoding, glyph_indices, codepoints, size);
    }
}

HZ_STATIC void
hz_mirror_uc_symbols(hz_vector(hz_unicode_t) v)
{
    for (size_t i = 0; i < hz_vector_size(v); ++i) {
        switch (v[i]) {
            case '(': v[i] = ')'; break;
            case ')': v[i] = '('; break;
            case '[': v[i] = ']'; break;
            case ']': v[i] = '['; break;
            case '{': v[i] = '}'; break;
            case '}': v[i] = '{'; break;
            case '<': v[i] = '>'; break;
            case '>': v[i] = '<'; break;
            case ',': v[i] = 0x2E41; break; // reversed comma
//            case ',': v[i] = 0x060C; break; // arabic comma U+060C
            case ';': v[i] = 0x204F; break; // reversed semicolon
            case '?': v[i] = 0x2E2E; break; // reversed question mark
        }
    }
}

typedef struct hz_long_hor_metric_t {
    uint16_t advance_width; // Advance width
    int16_t lsb; // left side bearing
} hz_long_hor_metric_t;

void
hz_read_h_metrics(hz_stream_t *table, size_t metrics_count, hz_long_hor_metric_t *metrics) {
    size_t index = 0;

    while (index < metrics_count) {
        hz_long_hor_metric_t *metric = &metrics[ index ];
        Unpackv(table, "hh", &metric->advance_width, (uint16_t *) &metric->lsb);
        ++index;
    }
}

HZ_STATIC hz_bool_t
hz_should_ignore_glyph(hz_buffer_t *buffer, size_t index, uint16_t flags, const hz_coverage_t *mark_filtering_set) {
    if (buffer->attrib_flags & (HZ_GLYPH_ATTRIB_GLYPH_CLASS_BIT | HZ_GLYPH_ATTRIB_ATTACHMENT_CLASS_BIT)) {
        uint8_t attach_type = (flags & HZ_LOOKUP_FLAG_MARK_ATTACHMENT_TYPE_MASK) >> 8;
        uint16_t ignored_classes = hz_ignored_classes_from_lookup_flags(flags);

        if (buffer->glyph_classes[index] & ignored_classes) return HZ_TRUE;

        if (buffer->glyph_classes[index] & HZ_GLYPH_CLASS_MARK) {
            if (flags & HZ_LOOKUP_FLAG_USE_MARK_FILTERING_SET) {
                if (hz_coverage_search(mark_filtering_set, buffer->glyph_indices[index]) != -1) {
                    return HZ_TRUE;
                }
            }

            if (attach_type && (buffer->attachment_classes[index] != attach_type)) {
                return HZ_TRUE;
            }
        }
    }

    return HZ_FALSE;
}

void hz_buffer_compute_info(hz_buffer_t *buffer, hz_face_t *face)
{
    // realloc info
    size_t size = hz_vector_size(buffer->glyph_indices);

    if (size > 0) {
        hz_glyph_attrib_flags_t info_attributes = HZ_GLYPH_ATTRIB_GLYPH_CLASS_BIT | HZ_GLYPH_ATTRIB_ATTACHMENT_CLASS_BIT;
        hz_buffer_clear_attribs(buffer, info_attributes);

        hz_vector_resize(buffer->glyph_classes,size);
        hz_vector_resize(buffer->attachment_classes,size);

        for (size_t s = 0; s < size; ++s) {
            buffer->glyph_classes[s] = hz_face_get_glyph_class(face, buffer->glyph_indices[s]);
            if (buffer->glyph_classes[s] & HZ_GLYPH_CLASS_MARK) {
                buffer->attachment_classes[s] = hz_face_get_glyph_attach_class(face, buffer->glyph_indices[s]);
            } else {
                buffer->attachment_classes[s] = 0;
            }
        }

        buffer->attrib_flags |= info_attributes;
    }
}

void hz_buffer_copy_attribs(hz_buffer_t *b1, hz_buffer_t *b2)
{
    if (b2->attrib_flags & HZ_GLYPH_ATTRIB_METRICS_BIT) {
        hz_vector_push_many(b1->glyph_metrics, b2->glyph_metrics, b2->glyph_count);
    }
    if (b2->attrib_flags & HZ_GLYPH_ATTRIB_INDEX_BIT) {
        hz_vector_push_many(b1->glyph_indices, b2->glyph_indices, b2->glyph_count);
    }
    if (b2->attrib_flags & HZ_GLYPH_ATTRIB_CODEPOINT_BIT) {
        hz_vector_push_many(b1->codepoints, b2->codepoints, b2->glyph_count);
    }
    if (b2->attrib_flags & HZ_GLYPH_ATTRIB_GLYPH_CLASS_BIT) {
        hz_vector_push_many(b1->glyph_classes, b2->glyph_classes, b2->glyph_count);
    }
    if (b2->attrib_flags & HZ_GLYPH_ATTRIB_ATTACHMENT_CLASS_BIT) {
        hz_vector_push_many(b1->attachment_classes, b2->attachment_classes, b2->glyph_count);
    }
    if (b2->attrib_flags & HZ_GLYPH_ATTRIB_COMPONENT_INDEX_BIT) {
        hz_vector_push_many(b1->component_indices, b2->component_indices, b2->glyph_count);
    }

    b1->glyph_count = b2->glyph_count;
}

void
hz_swap_buffers(hz_buffer_t *b1, hz_buffer_t *b2, hz_face_t *face)
{
    HZ_ASSERT(b1 != NULL && b2 != NULL);
    HZ_ASSERT(b1 != b2);

    if (b1->glyph_count != b2->glyph_count) {
        // clear every attribute
        hz_buffer_clear(b1);
        b1->attrib_flags = b2->attrib_flags;
    } else {
        // clear only the attribute vectors which will be replaced
        hz_buffer_clear_attribs(b1, b2->attrib_flags);
    }

    // move select attribute arrays from b2 to b1
    hz_buffer_copy_attribs(b1,b2);

    // clear destination buffer
    hz_buffer_clear(b2);
}

typedef enum hz_cpu_flags_t {
    HZ_CPU_FLAGS_SUPPORTS_SSE = 0x00000001,
    HZ_CPU_FLAGS_SUPPORTS_SSE2 = 0x00000002,
    HZ_CPU_FLAGS_SUPPORTS_SSE4 = 0x00000004,
    HZ_CPU_FLAGS_SUPPORTS_SSE4_1 = 0x00000008,
    HZ_CPU_FLAGS_SUPPORTS_SSE4_2 = 0x00000010,
} hz_cpu_flags_t;

HZ_STATIC hz_bool_t hz_check_cpu_flags(hz_cpu_flags_t features) {
    return HZ_FALSE;
}

void hz_swap_buffer_elements(void *arr, hz_segment_sz_t len, size_t element_size)
{
    long half_len = len/2;
    for (long i = 0; i < half_len; ++i) {
        uint8_t *p_elem = (uint8_t *)arr + i * element_size;
        uint8_t *q_elem = (uint8_t *)arr + (len-i-1) * element_size;

        for (long j = 0; j < element_size; ++j) {
            uint8_t *p = p_elem+j, *q = q_elem+j;
            *p ^= *q;
            *q ^= *p;
            *p ^= *q;
        }
    }
}

void
hz_buffer_flip_direction(hz_buffer_t *buffer)
{
    size_t len = buffer->glyph_count;

    // swap values at i1 and i2
    if (buffer->attrib_flags & HZ_GLYPH_ATTRIB_INDEX_BIT) {
        hz_swap_buffer_elements(buffer->glyph_indices,len,sizeof(hz_index_t));
    }

    if (buffer->attrib_flags & HZ_GLYPH_ATTRIB_CODEPOINT_BIT) {
        hz_swap_buffer_elements(buffer->codepoints,len,sizeof(hz_unicode_t));
    }

    if (buffer->attrib_flags & HZ_GLYPH_ATTRIB_GLYPH_CLASS_BIT) {
        hz_swap_buffer_elements(buffer->glyph_classes,len,sizeof(uint16_t));
    }

    if (buffer->attrib_flags & HZ_GLYPH_ATTRIB_ATTACHMENT_CLASS_BIT) {
        hz_swap_buffer_elements(buffer->attachment_classes,len,sizeof(uint16_t));
    }

    if (buffer->attrib_flags & HZ_GLYPH_ATTRIB_COMPONENT_INDEX_BIT) {
        hz_swap_buffer_elements(buffer->component_indices,len,sizeof(uint16_t));
    }

    if (buffer->attrib_flags & HZ_GLYPH_ATTRIB_METRICS_BIT) {
        hz_swap_buffer_elements(buffer->glyph_metrics,len,sizeof(hz_glyph_metrics_t));
    }
}

hz_bool_t
hz_buffer_contains_range(const hz_buffer_t *buffer, int v1, int v2)
{
    size_t size = hz_vector_size(buffer->glyph_indices);
    return v1 >= 0 && v1 < size
    && v2 >= 0 && v2 < size
    && v2 >= v1;
}

hz_buffer_t *hz_buffer_copy_range(hz_buffer_t *from, int v1, int v2)
{
    if (hz_buffer_contains_range(from,v1,v2)) {
        int len = (v2-v1)+1;
        hz_buffer_t *to = hz_buffer_create();
        to->attrib_flags = from->attrib_flags;

        if (from->attrib_flags & HZ_GLYPH_ATTRIB_METRICS_BIT)
            hz_vector_push_many(to->glyph_metrics, from->glyph_metrics + v1, len);
        if (from->attrib_flags & HZ_GLYPH_ATTRIB_INDEX_BIT)
            hz_vector_push_many(to->glyph_indices, from->glyph_indices + v1, len);
        if (from->attrib_flags & HZ_GLYPH_ATTRIB_CODEPOINT_BIT)
            hz_vector_push_many(to->codepoints, from->codepoints + v1, len);
        if (from->attrib_flags & HZ_GLYPH_ATTRIB_GLYPH_CLASS_BIT)
            hz_vector_push_many(to->glyph_classes, from->glyph_classes + v1, len);
        if (from->attrib_flags & HZ_GLYPH_ATTRIB_ATTACHMENT_CLASS_BIT)
            hz_vector_push_many(to->attachment_classes, from->attachment_classes + v1, len);
        if (from->attrib_flags & HZ_GLYPH_ATTRIB_COMPONENT_INDEX_BIT)
            hz_vector_push_many(to->component_indices, from->component_indices + v1, len);

        to->glyph_count = len;

        return to;
    }

    return NULL;
}

int *hz_buffer_get_unignored_indices(hz_buffer_t *buffer, uint16_t lookup_flag, const hz_coverage_t *mark_filtering_set)
{
    int *index_list = NULL;

    for (int i = 0; i < hz_vector_size(buffer->glyph_indices); ++i) {
        if (!hz_should_ignore_glyph(buffer, i, lookup_flag, mark_filtering_set)) {
            hz_vector_push_back(index_list, i);
        }
    }

    return index_list;
}

void
hz_segment_setup_shaping_objects(hz_segment_t *seg, hz_face_t *face)
{
    hz_buffer_clear(seg->in);
    hz_buffer_clear(seg->out);

    if (seg->num_codepoints > 0) {
        hz_vector_resize(seg->in->codepoints, seg->num_codepoints);
        memcpy(seg->in->codepoints, seg->codepoints, seg->num_codepoints * sizeof(hz_unicode_t));

        // map unicode characters to nominal glyph indices
        hz_vector_resize(seg->in->glyph_indices, seg->num_codepoints);
        hz_map_to_nominal_forms(face, seg->in->glyph_indices, seg->codepoints, seg->num_codepoints);

        hz_vector_resize(seg->in->component_indices, seg->num_codepoints);
        memset(seg->in->component_indices, 0, seg->num_codepoints * sizeof(uint16_t));

        seg->in->glyph_count = seg->num_codepoints;
        seg->in->attrib_flags |= HZ_GLYPH_ATTRIB_INDEX_BIT | HZ_GLYPH_ATTRIB_CODEPOINT_BIT | HZ_GLYPH_ATTRIB_COMPONENT_INDEX_BIT;

        // sets glyph class information
        hz_buffer_compute_info(seg->in, face);
    }
}

// swaps in and out buffers, compute info for in buffer
// such as glyph and attachment classes
void hz_segment_swap_buffers(hz_segment_t *seg, hz_face_t *face)
{
    hz_swap_buffers(seg->in, seg->out, face);
}

typedef struct {
    uint16_t format;
} hz_lookup_subtable_t;

typedef struct {
    uint16_t lookup_type, lookup_flags;
    uint16_t subtable_count;
    hz_lookup_subtable_t **subtables;
    /* Index (base 0) into GDEF mark glyph sets structure.
     * This field is only present if the USE_MARK_FILTERING_SET lookup flag is set.
     */
    hz_coverage_t mark_filtering_set;
} hz_lookup_table_t;

typedef struct {
    Version16Dot16 version;
    uint16_t num_features;
    hz_feature_list_item_t *features;
    uint16_t num_lookups;
    hz_lookup_table_t *lookups;
} hz_gsub_table_t;

typedef struct {
    Version16Dot16 version;
    uint32_t num_lookups;
    hz_lookup_table_t *lookups;
    uint32_t num_features;
    hz_feature_list_item_t *features;
} hz_gpos_table_t;

typedef struct {
    hz_font_t *font;
    hz_direction_t direction;
    hz_script_t script;
    hz_language_t language;
    hz_feature_t *features;
    unsigned int num_features;
    hz_gsub_table_t gsub_table;
    hz_gpos_table_t gpos_table;
    hz_shape_flags_t shape_flags;
    hz_deserializer_t *deserializer;
    hz_memory_pool_t memory_pool;
    hz_allocator_t allocator;
} hz_shape_plan_t;

HZ_STATIC void
hz_load_feature_table(hz_allocator_t *alctr, hz_deserializer_t *ds, hz_feature_table_t *table)
{
    table->feature_params = hz_deserializer_read_u16(ds);
    table->lookup_index_count = hz_deserializer_read_u16(ds);
    table->lookup_list_indices = hz_allocate(alctr, sizeof(uint16_t) * table->lookup_index_count);
    hz_deserializer_read_u16_block(ds, table->lookup_list_indices, table->lookup_index_count);
}

typedef struct hz_single_substitution_format1_subtable_t {
    uint16_t format;
    hz_coverage_t coverage;
    int16_t delta_glyph_id;
} hz_single_substitution_format1_subtable_t;

typedef struct {
    uint16_t format;
    hz_coverage_t coverage;
    uint16_t glyph_count;
    uint16_t *substitute_glyph_ids;
} hz_single_substitution_format2_subtable_t;

HZ_STATIC hz_error_t
hz_read_gsub_single_substitution_subtable(hz_allocator_t *alctr,
                                          hz_deserializer_t *ds,
                                          hz_lookup_table_t *lookup,
                                          uint16_t subtable_index,
                                          uint16_t format)
{
    switch (format) {
        case 1: {
            hz_single_substitution_format1_subtable_t *subtable = hz_allocate(alctr, sizeof(*subtable));
            subtable->format = format;
            Offset16 coverage_offset = hz_deserializer_read_u16(ds);

            hz_deserializer_push_state(ds, coverage_offset);
            hz_read_coverage(alctr, ds, &subtable->coverage);
            hz_deserializer_pop_state(ds);

            subtable->delta_glyph_id = hz_deserializer_read_u16(ds);
            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }

        case 2: {
            hz_single_substitution_format2_subtable_t *subtable = hz_allocate(alctr, sizeof(*subtable));

            subtable->format = format;
            Offset16 coverage_offset = hz_deserializer_read_u16(ds);
            
            hz_deserializer_push_state(ds, coverage_offset);
            hz_read_coverage(alctr, ds, &subtable->coverage);
            hz_deserializer_pop_state(ds);

            subtable->glyph_count = hz_deserializer_read_u16(ds);
            subtable->substitute_glyph_ids = hz_allocate(alctr, subtable->glyph_count * sizeof(uint16_t));
            hz_deserializer_read_u16_block(ds, subtable->substitute_glyph_ids, subtable->glyph_count);

            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }

        default:
            return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
    }

    return HZ_OK;
}

typedef struct hz_multiple_substitution_format1_subtable_t {
    uint16_t format;
    hz_map_t *coverage;
    uint16_t sequence_count;
    hz_sequence_table_t *sequences;
} hz_multiple_substitution_format1_subtable_t;

typedef struct hz_ligature_t {
    uint16_t ligature_glyph;
    uint16_t component_count;
    uint16_t *component_glyph_ids;
} hz_ligature_t;

typedef struct hz_ligature_set_t {
    uint16_t ligature_count;
    hz_ligature_t *ligatures;
} hz_ligature_set_table_t;

typedef struct hz_ligature_substitution_format1_subtable_t {
    uint16_t format;
    hz_coverage_t coverage;
    uint16_t ligature_set_count;
    hz_ligature_set_table_t *ligature_sets;
} hz_ligature_substitution_format1_subtable_t;

HZ_STATIC hz_error_t
hz_read_gsub_ligature_substitution_subtable(hz_allocator_t *alctr,
                                            hz_deserializer_t *ds,
                                            hz_lookup_table_t *lookup,
                                            uint16_t subtable_index,
                                            uint16_t format)
{
    uint8_t arenamem[10000];
    hz_memory_arena_t arena = hz_memory_arena_create(arenamem,sizeof arenamem);

    if (format != 1)
        return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;

    
    hz_ligature_substitution_format1_subtable_t *subtable = hz_allocate(alctr, sizeof(*subtable));

    subtable->format = format;
    Offset16 coverage_offset = hz_deserializer_read_u16(ds);

    hz_deserializer_push_state(ds,coverage_offset);
    hz_read_coverage(alctr, ds, &subtable->coverage);
    hz_deserializer_pop_state(ds);

    subtable->ligature_set_count = hz_deserializer_read_u16(ds);
    subtable->ligature_sets = hz_allocate(alctr, sizeof(hz_ligature_set_table_t) * subtable->ligature_set_count);

    Offset16 *ligature_set_offsets = hz_memory_arena_allocate(&arena, subtable->ligature_set_count * sizeof(Offset16));
    hz_deserializer_read_u16_block(ds, ligature_set_offsets, subtable->ligature_set_count);

    for (uint16_t i = 0; i < subtable->ligature_set_count; ++i) {
        hz_deserializer_push_state(ds,ligature_set_offsets[i]);
        hz_ligature_set_table_t *ligature_set = subtable->ligature_sets+i;

        ligature_set->ligature_count = hz_deserializer_read_u16(ds);
        ligature_set->ligatures = hz_allocate(alctr, sizeof(hz_ligature_t) * ligature_set->ligature_count);
        
        Offset16 *ligature_offsets = hz_memory_arena_allocate(&arena, ligature_set->ligature_count * sizeof(Offset16));
        hz_deserializer_read_u16_block(ds, ligature_offsets, ligature_set->ligature_count);

        for (uint16_t j = 0; j < ligature_set->ligature_count; ++j) {
            hz_deserializer_push_state(ds,ligature_offsets[j]);
            
            hz_ligature_t *ligature = ligature_set->ligatures + j;
            ligature->ligature_glyph = hz_deserializer_read_u16(ds);
            ligature->component_count = hz_deserializer_read_u16(ds);
            if (ligature->component_count > 1) {
                ligature->component_glyph_ids = hz_allocate(alctr, (ligature->component_count - 1) * sizeof(uint16_t));
                hz_deserializer_read_u16_block(ds, ligature->component_glyph_ids, ligature->component_count - 1);
            } else {
                ligature->component_glyph_ids = NULL;
            }

            hz_deserializer_pop_state(ds);
        }

        hz_deserializer_pop_state(ds);
    }

    lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
    return HZ_OK;
}

typedef struct hz_chained_sequence_context_format1_subtable_t {
    uint16_t format;
    hz_map_t *coverage;
    uint16_t rule_set_count;
    hz_chained_sequence_rule_set_t *rule_sets;
} hz_chained_sequence_context_format1_subtable_t;

HZ_STATIC hz_error_t
hz_load_gsub_chained_contexts_substitution_subtable(hz_stream_t *stream,
                                                    hz_lookup_table_t *lookup,
                                                    uint16_t subtable_index,
                                                    uint16_t format)
{
    switch (format) {
        case 1: {
            // 6.1 Chained Contexts Substitution Format 1: Simple Glyph Contexts
            // https://docs.microsoft.com/en-us/typography/opentype/spec/gsub#61-chained-contexts-substitution-format-1-simple-glyph-contexts
            hz_chained_sequence_context_format1_subtable_t *subtable = hz_malloc(sizeof(*subtable));
            subtable->format = format;

            Offset16 coverage_offset = Unpack16(stream);
            subtable->rule_set_count = Unpack16(stream);
            subtable->coverage = hz_map_create();
            hz_read_coverage(stream->data + coverage_offset, subtable->coverage, NULL);

            Offset16 *rule_set_offsets = hz_malloc(sizeof(Offset16) * subtable->rule_set_count);
            UnpackArray16(stream, subtable->rule_set_count, rule_set_offsets);

            subtable->rule_sets = hz_malloc(sizeof(*subtable->rule_sets) * subtable->rule_set_count);
            for (int i = 0; i < subtable->rule_set_count; ++i) {
                if (rule_set_offsets[i] != 0) {
                    hz_parse_chained_sequence_rule_set(stream->data + rule_set_offsets[i],
                                                       subtable->rule_sets+i);
                } else {
                    subtable->rule_sets[i].count = 0;
                    subtable->rule_sets[i].rules = NULL;
                }
            }

            hz_free(rule_set_offsets);
            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }

        case 2: {
            break;
        }
        case 3: {
            hz_chained_sequence_context_format3_subtable_t *subtable = hz_malloc(sizeof(*subtable));
            subtable->format = format;
            hz_ot_load_chained_sequence_context_format3_subtable(stream, subtable);
            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }
        default:
            return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
    }

    return HZ_OK;
}

HZ_STATIC hz_error_t
hz_load_gsub_multiple_substitution_subtable(hz_stream_t *stream,
                                            hz_lookup_table_t *lookup,
                                            uint16_t subtable_index,
                                            uint16_t format)
{
    switch (format) {
        case 1: {
            hz_multiple_substitution_format1_subtable_t *subtable = hz_malloc(sizeof(*subtable));

            subtable->format = format;
            Offset16 coverage_offset = Unpack16(stream);
            subtable->coverage = hz_map_create();
            hz_read_coverage(stream->data + coverage_offset, subtable->coverage, NULL);

            subtable->sequence_count = Unpack16(stream);

            subtable->sequences = hz_malloc(subtable->sequence_count * sizeof(hz_sequence_table_t));
            Offset16 *sequence_offsets = hz_malloc(subtable->sequence_count * sizeof(Offset16));
            UnpackArray16(stream, subtable->sequence_count, sequence_offsets);

            for (uint16_t i = 0 ; i < subtable->sequence_count; ++i) {
                hz_ot_read_sequence_table(stream->data + (size_t)sequence_offsets[i], &subtable->sequences[i]);
            }

            hz_free(sequence_offsets);
            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }
        default:
            return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
    }

    return HZ_OK;
}

HZ_STATIC hz_error_t
hz_read_gsub_lookup_subtable(hz_allocator_t *alctr,
                             hz_deserializer_t *ds,
                             hz_lookup_table_t *lookup,
                             uint16_t lookup_type,
                             uint16_t subtable_index)
{
    uint16_t format;
    int extension = 0;

    extension_label:
    format = hz_deserializer_read_u16(ds);

    switch (lookup_type) {
        case HZ_GSUB_LOOKUP_TYPE_SINGLE_SUBSTITUTION:
            return hz_read_gsub_single_substitution_subtable(alctr, ds, lookup, subtable_index, format);

        case HZ_GSUB_LOOKUP_TYPE_MULTIPLE_SUBSTITUTION: break;
            // return hz_load_gsub_multiple_substitution_subtable(&stream, lookup, subtable_index, format);
        case HZ_GSUB_LOOKUP_TYPE_ALTERNATE_SUBSTITUTION: break;
        case HZ_GSUB_LOOKUP_TYPE_LIGATURE_SUBSTITUTION:
            return hz_read_gsub_ligature_substitution_subtable(alctr, ds, lookup, subtable_index, format);

        case HZ_GSUB_LOOKUP_TYPE_CONTEXTUAL_SUBSTITUTION: break;
        case HZ_GSUB_LOOKUP_TYPE_CHAINED_CONTEXTS_SUBSTITUTION: break;
            // return hz_load_gsub_chained_contexts_substitution_subtable(&stream, lookup, subtable_index, format);
        case HZ_GSUB_LOOKUP_TYPE_EXTENSION_SUBSTITUTION: { // implemented inline
            if (format != 1) {
            // error
            }

            lookup_type = lookup->lookup_type = hz_deserializer_read_u16(ds);
            Offset32 extension_offset = hz_deserializer_read_u32(ds);
            hz_deserializer_push_state(ds, extension_offset);
            extension = 1;
            goto extension_label;
        }

        case HZ_GSUB_LOOKUP_TYPE_REVERSE_CHAINING_CONTEXTUAL_SINGLE_SUBSTITUTION: break;

        default: // error
            return HZ_ERROR_INVALID_LOOKUP_TYPE;
    }
    
    if (extension)
        hz_deserializer_pop_state( ds );

    return HZ_OK;
}

typedef struct hz_mark_array_t {
    uint16_t mark_count;
    hz_mark_record_t *mark_records;
} hz_mark_array_t;

typedef struct hz_mark2_record_t {
    hz_anchor_t *mark2_anchors;
} hz_mark2_record_t;


typedef struct hz_mark2_array_t {
    uint16_t mark2_count;
    hz_mark2_record_t *mark2_records;
} hz_mark2_array_t;

typedef struct hz_base_record_t {
    hz_anchor_t *base_anchors; // array size of mark_class_count
} hz_base_record_t;

typedef struct hz_base_array_t {
    uint16_t base_count;
    hz_base_record_t *base_records;
} hz_base_array_t;

typedef struct hz_mark_to_base_attachment_subtable_t {
    uint16_t format;
    hz_map_t *mark_coverage;
    hz_map_t *base_coverage;
    uint16_t mark_class_count;
    hz_mark_array_t mark_array;
    hz_base_array_t base_array;
} hz_mark_to_base_attachment_subtable_t;

HZ_STATIC void
hz_load_base_array(const uint8_t *data, hz_base_array_t *base_array, uint16_t mark_class_count)
{
    hz_stream_t stream = hz_stream_create(data, 0);
    base_array->base_count = Unpack16(&stream);
    base_array->base_records = hz_malloc(base_array->base_count * sizeof(hz_base_record_t));

    for (size_t i = 0; i < base_array->base_count; ++i) {
        hz_base_record_t *record = &base_array->base_records[i];
        record->base_anchors = hz_malloc(mark_class_count * sizeof(hz_anchor_t));
        Offset16 *anchor_offsets = hz_malloc(mark_class_count * sizeof(Offset16));
        UnpackArray16(&stream, mark_class_count, anchor_offsets);

        for (size_t j = 0; j < mark_class_count; ++j) {
            if (anchor_offsets[j] > 0) {
                record->base_anchors[j] = hz_read_anchor(data + anchor_offsets[j]);
            }
        }

        hz_free(anchor_offsets);
    }
}

HZ_STATIC void
hz_load_mark2_array(const uint8_t *data, hz_mark2_array_t *mark2_array, uint16_t mark_class_count)
{
    hz_stream_t stream = hz_stream_create(data, 0);
    mark2_array->mark2_count = Unpack16(&stream);
    mark2_array->mark2_records = hz_malloc(mark2_array->mark2_count * sizeof(hz_mark2_record_t));
    for (size_t i = 0; i < mark2_array->mark2_count; ++i) {
        hz_mark2_record_t *record = &mark2_array->mark2_records[i];
        record->mark2_anchors = hz_malloc(mark_class_count * sizeof(hz_anchor_t));
        Offset16 *anchor_offsets = hz_malloc(mark_class_count * 2);
        UnpackArray16(&stream, mark_class_count, anchor_offsets);

        for (size_t j = 0; j < mark_class_count; ++j) {
            if (anchor_offsets[j] > 0) {
                record->mark2_anchors[j] = hz_read_anchor(data + anchor_offsets[j]);
            }
        }

        hz_free(anchor_offsets);
    }
}

HZ_STATIC void
hz_load_mark_array(const uint8_t *data, hz_mark_array_t *mark_array)
{
    hz_stream_t stream = hz_stream_create(data, 0);
    mark_array->mark_count = Unpack16(&stream);
    mark_array->mark_records = hz_malloc(mark_array->mark_count * sizeof(hz_mark_record_t));

    for (size_t i = 0; i < mark_array->mark_count; ++i) {
        hz_mark_record_t *record = &mark_array->mark_records[i];
        record->mark_class = Unpack16(&stream);
        Offset16 mark_anchor_offset = Unpack16(&stream);
        record->mark_anchor = hz_read_anchor(data + mark_anchor_offset);
    }
}

typedef struct hz_single_adjustment_format1_subtable_t {
    uint16_t format;
    hz_map_t *coverage;
    uint16_t value_format;
    hz_value_record_t value_record;
} hz_single_adjustment_format1_subtable_t;

typedef struct hz_single_adjustment_format2_subtable_t {
    uint16_t format;
    hz_map_t *coverage;
    uint16_t value_format;
    uint16_t value_count;
    hz_value_record_t *value_records;
} hz_single_adjustment_format2_subtable_t;

HZ_STATIC hz_error_t
hz_load_gpos_single_adjustment_subtable(hz_stream_t *stream,
                                        hz_lookup_table_t *lookup,
                                        uint16_t subtable_index,
                                        uint16_t format)
{
    switch (format) {
        case 1: {
            hz_single_adjustment_format1_subtable_t *subtable = hz_malloc(sizeof(*subtable));
            subtable->format = format;
            Offset16 coverage_offset = Unpack16(stream);
            subtable->coverage = hz_map_create();
            hz_read_coverage(stream->data + coverage_offset, subtable->coverage, NULL);
            subtable->value_format = Unpack16(stream);
            hz_read_value_record(stream, &subtable->value_record, subtable->value_format);
            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }
        case 2: {
            hz_single_adjustment_format2_subtable_t *subtable = hz_malloc(sizeof(*subtable));
            subtable->format = format;
            Offset16 coverage_offset = Unpack16(stream);
            subtable->coverage = hz_map_create();
            hz_read_coverage(stream->data + coverage_offset, subtable->coverage, NULL);
            subtable->value_format = Unpack16(stream);
            subtable->value_count = Unpack16(stream);
            subtable->value_records = hz_malloc(sizeof(hz_value_record_t) * subtable->value_count);
            for (int i = 0; i < subtable->value_count; ++i) {
                hz_read_value_record(stream, &subtable->value_records[i], subtable->value_format);
            }
            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }
        default:
            return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
    }

    return HZ_OK;
}

HZ_STATIC void
hz_read_pair_value_record(hz_stream_t *stream,
                          hz_pair_value_record_t *pair_value_record,
                          uint16_t v1, uint16_t v2)
{
    pair_value_record->second_glyph = Unpack16(stream);
    hz_read_value_record(stream, &pair_value_record->value_record1, v1);
    hz_read_value_record(stream, &pair_value_record->value_record2, v2);
}

HZ_STATIC void
hz_read_pair_set(uint8_t *data, hz_pair_set_t *pair_set, uint16_t v1, uint16_t v2)
{
    hz_stream_t stream = hz_stream_create(data, 0);
    pair_set->pair_value_count = Unpack16(&stream);
    pair_set->pair_value_records = hz_malloc(pair_set->pair_value_count * sizeof(hz_pair_value_record_t));
    for (int i = 0; i < pair_set->pair_value_count; ++i) {
        hz_read_pair_value_record(&stream, &pair_set->pair_value_records[i], v1,v2);
    }
}

HZ_STATIC hz_error_t
hz_load_gpos_pair_adjustment_subtable(hz_stream_t *stream,
                                      hz_lookup_table_t *lookup,
                                      uint16_t subtable_index,
                                      uint16_t format)
{
    switch (format) {
        case 1: {
            // individual glyphs
            hz_pair_pos_format1_subtable_t *subtable = hz_malloc(sizeof(*subtable));
            subtable->format = format;
            subtable->coverage = hz_map_create();
            Offset16 coverage_offset = Unpack16(stream);
            hz_read_coverage(stream->data + coverage_offset, subtable->coverage, NULL);
            subtable->value_format1 = Unpack16(stream);
            subtable->value_format2 = Unpack16(stream);
            subtable->pair_set_count = Unpack16(stream);
            Offset16 *pair_set_offsets = hz_malloc(sizeof(Offset16) * subtable->pair_set_count);
            UnpackArray16(stream, subtable->pair_set_count, pair_set_offsets);
            subtable->pair_sets = hz_malloc(sizeof(hz_pair_set_t) * subtable->pair_set_count);


            for (int i = 0; i < subtable->pair_set_count; ++i) {
                hz_read_pair_set(stream->data + pair_set_offsets[i],&subtable->pair_sets[i],
                                 subtable->value_format1,
                                 subtable->value_format2);
            }


            hz_free(pair_set_offsets);
            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }
        case 2: {
            // glyph classes
            hz_pair_pos_format2_subtable_t *subtable = hz_malloc(sizeof(*subtable));
            subtable->format = format;
            subtable->coverage = hz_map_create();
            subtable->class_def1 = hz_map_create();
            subtable->class_def2 = hz_map_create();

            Offset16 coverage_offset, class_def1_offset, class_def2_offset;

            coverage_offset = Unpack16(stream);
            hz_read_coverage(stream->data + coverage_offset, subtable->coverage, NULL);

            subtable->value_format1 = Unpack16(stream);
            subtable->value_format2 = Unpack16(stream);

            class_def1_offset = Unpack16(stream);
            class_def2_offset = Unpack16(stream);
            hz_read_class_def_table(stream->data + class_def1_offset, subtable->class_def1);
            hz_read_class_def_table(stream->data + class_def2_offset, subtable->class_def2);

            subtable->class1_count = Unpack16(stream);
            subtable->class2_count = Unpack16(stream);

            subtable->class1_records = hz_malloc(sizeof(hz_class1_record_t) * subtable->class1_count);

            for (int i = 0; i < subtable->class1_count; ++i) {
                hz_class1_record_t  *class1_record = &subtable->class1_records[i];
                class1_record->class2_records = hz_malloc(sizeof(hz_class2_record_t) * subtable->class2_count);
                for (int j = 0; j < subtable->class2_count; ++j) {
                    hz_class2_record_t *class2_record = &class1_record->class2_records[j];
                    hz_read_value_record(stream,&class2_record->value_record1,subtable->value_format1);
                    hz_read_value_record(stream,&class2_record->value_record2,subtable->value_format2);
                }
            }

            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }
        default:
            return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
    }

    return HZ_OK;
}

HZ_STATIC hz_error_t
hz_load_gpos_cursive_attachment_subtable(hz_stream_t *stream,
                                         hz_lookup_table_t *lookup,
                                         uint16_t subtable_index,
                                         uint16_t format)
{
    return HZ_OK;
}

HZ_STATIC hz_error_t
hz_load_gpos_mark_to_base_attachment_subtable(hz_stream_t *stream,
                                              hz_lookup_table_t *lookup,
                                              uint16_t subtable_index,
                                              uint16_t format)
{
    switch (format) {
        case 1: {
            hz_mark_to_base_attachment_subtable_t *subtable = hz_malloc(sizeof (hz_mark_to_base_attachment_subtable_t));
            subtable->format = format;
            subtable->mark_coverage = hz_map_create();
            subtable->base_coverage = hz_map_create();
            Offset16 mark_coverage_offset = Unpack16(stream);
            Offset16 base_coverage_offset = Unpack16(stream);
            hz_read_coverage(stream->data + mark_coverage_offset, subtable->mark_coverage, NULL);
            hz_read_coverage(stream->data + base_coverage_offset, subtable->base_coverage, NULL);
            subtable->mark_class_count = Unpack16(stream);
            Offset16 mark_array_offset = Unpack16(stream);
            Offset16 base_array_offset = Unpack16(stream);
            hz_load_mark_array(stream->data + mark_array_offset, &subtable->mark_array);
            hz_load_base_array(stream->data + base_array_offset, &subtable->base_array, subtable->mark_class_count);
            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }

        default:
            return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
    }

    return HZ_OK;
}


typedef struct hz_mark_to_mark_attachment_format1_subtable_t {
    uint16_t format;
    hz_map_t *mark1_coverage;
    hz_map_t *mark2_coverage;
    uint16_t mark_class_count;
    hz_mark_array_t mark1_array;
    hz_mark2_array_t mark2_array;
} hz_mark_to_mark_attachment_format1_subtable_t;

HZ_STATIC hz_error_t
hz_load_gpos_mark_to_mark_attachment_subtable(hz_stream_t *stream,
                                              hz_lookup_table_t *lookup,
                                              uint16_t subtable_index,
                                              uint16_t format)
{
    switch (format) {
        case 1 :{
            hz_mark_to_mark_attachment_format1_subtable_t *subtable = hz_malloc(sizeof(*subtable));
            subtable->format = format;
            subtable->mark1_coverage = hz_map_create();
            subtable->mark2_coverage = hz_map_create();
            Offset16 mark1_coverage_offset = Unpack16(stream);
            Offset16 mark2_coverage_offset = Unpack16(stream);
            hz_read_coverage(stream->data + mark1_coverage_offset, subtable->mark1_coverage, NULL);
            hz_read_coverage(stream->data + mark2_coverage_offset, subtable->mark2_coverage, NULL);

            subtable->mark_class_count = Unpack16(stream);

            Offset16 mark1_array_offset = Unpack16(stream);
            Offset16 mark2_array_offset = Unpack16(stream);
            hz_load_mark_array(stream->data + mark1_array_offset, &subtable->mark1_array);
            hz_load_mark2_array(stream->data + mark2_array_offset, &subtable->mark2_array, subtable->mark_class_count);

            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }

        default:
            return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
    }

    return HZ_OK;
}

typedef struct hz_component_record_t {
    hz_anchor_t *ligature_anchors;
} hz_component_record_t;

typedef struct hz_ligature_attachment_t {
    uint16_t component_count;
    hz_component_record_t *component_records;
} hz_ligature_attachment_t;

typedef struct hz_ligature_array_t {
    uint16_t ligature_count;
    hz_ligature_attachment_t *ligature_attachments;
} hz_ligature_array_t;

HZ_STATIC void
hz_load_ligature_attachment(const uint8_t *data, uint16_t mark_class_count, hz_ligature_attachment_t *ligature_attachment)
{
    hz_stream_t stream = hz_stream_create(data, 0);
    ligature_attachment->component_count = Unpack16(&stream);
    ligature_attachment->component_records = hz_malloc(sizeof(hz_component_record_t) * ligature_attachment->component_count);
    
    for (uint16_t i = 0; i < ligature_attachment->component_count; ++i) {
        hz_component_record_t *record = &ligature_attachment->component_records[i];
        record->ligature_anchors = hz_malloc(mark_class_count * sizeof(hz_anchor_t));
        Offset16 *offsets = hz_malloc(mark_class_count * sizeof(Offset16));
        UnpackArray16(&stream, mark_class_count, offsets);

        for (int j = 0; j < mark_class_count; ++j) {
            if (offsets[j]) {
                hz_anchor_t *anchor = &record->ligature_anchors[j];
                *anchor = hz_read_anchor(stream.data + offsets[j]);
            }
        }

        hz_free(offsets);
    }

}

HZ_STATIC void
hz_load_ligature_array(const uint8_t *data, uint16_t mark_class_count, hz_ligature_array_t *ligature_array)
{
    hz_stream_t stream = hz_stream_create(data, 0);
    ligature_array->ligature_count = Unpack16(&stream);
    ligature_array->ligature_attachments = hz_malloc(sizeof(hz_ligature_attachment_t) * ligature_array->ligature_count);
    Offset16 *offsets = hz_malloc(sizeof(Offset16) * ligature_array->ligature_count);
    UnpackArray16(&stream, ligature_array->ligature_count, offsets);

    for (uint16_t i = 0; i < ligature_array->ligature_count; ++i) {
        hz_load_ligature_attachment(stream.data + offsets[i], mark_class_count, &ligature_array->ligature_attachments[i]);
    }

    hz_free(offsets);
}

typedef struct hz_mark_to_ligature_attachment_format1_subtable_t {
    uint16_t format;
    hz_map_t *mark_coverage;
    hz_map_t *ligature_coverage;
    uint16_t mark_class_count;
    hz_mark_array_t mark_array;
    hz_ligature_array_t ligature_array;
} hz_mark_to_ligature_attachment_format1_subtable_t;

HZ_STATIC hz_error_t
hz_load_gpos_mark_to_ligature_attachment_subtable(hz_stream_t *stream,
                                                  hz_lookup_table_t *lookup,
                                                  uint16_t subtable_index,
                                                  uint16_t format)
{
    switch (format) {
        case 1: {
            hz_mark_to_ligature_attachment_format1_subtable_t *subtable = hz_malloc(sizeof(*subtable));
            subtable->format = format;
            subtable->mark_coverage = hz_map_create();
            subtable->ligature_coverage = hz_map_create();

            Offset16 mark_coverage_offset = Unpack16(stream);
            Offset16 ligature_coverage_offset = Unpack16(stream);
            hz_read_coverage(stream->data + mark_coverage_offset, subtable->mark_coverage, NULL);
            hz_read_coverage(stream->data + ligature_coverage_offset, subtable->ligature_coverage, NULL);

            subtable->mark_class_count = Unpack16(stream);

            Offset16 mark_array_offset = Unpack16(stream);
            Offset16 ligature_array_offset = Unpack16(stream);

            hz_load_mark_array(stream->data + mark_array_offset, &subtable->mark_array);
            hz_load_ligature_array(stream->data + ligature_array_offset, subtable->mark_class_count, &subtable->ligature_array);

            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }

        default: return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
    }

    return HZ_OK;
}

HZ_STATIC hz_error_t
hz_load_gpos_chained_context_positioning_subtable(hz_stream_t *stream,
                                                  hz_lookup_table_t *lookup,
                                                  uint16_t subtable_index,
                                                  uint16_t format)
{
    switch (format) {
        case 1: {
            break;
        }

        case 2: {
            break;
        }

        case 3: {
            hz_chained_sequence_context_format3_subtable_t *subtable = hz_malloc(sizeof(*subtable));
            subtable->format = format;
            hz_ot_load_chained_sequence_context_format3_subtable(stream, subtable);
            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }

        default:
            return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
    }

    return HZ_OK;
}

HZ_STATIC hz_error_t
hz_load_gpos_lookup_subtable(const uint8_t *data,
                             hz_lookup_table_t *lookup,
                             uint16_t lookup_type,
                             uint16_t subtable_index)
 {
    uint16_t format;
    hz_stream_t stream = hz_stream_create(data, 0);

    extension_label:
    format = Unpack16(&stream);

    switch (lookup_type) {
        case HZ_GPOS_LOOKUP_TYPE_SINGLE_ADJUSTMENT:
            return hz_load_gpos_single_adjustment_subtable(&stream, lookup, subtable_index, format);

        case HZ_GPOS_LOOKUP_TYPE_PAIR_ADJUSTMENT:
            return hz_load_gpos_pair_adjustment_subtable(&stream, lookup, subtable_index, format);

        case HZ_GPOS_LOOKUP_TYPE_CURSIVE_ATTACHMENT:
            return hz_load_gpos_cursive_attachment_subtable(&stream, lookup, subtable_index, format);

        case HZ_GPOS_LOOKUP_TYPE_MARK_TO_BASE_ATTACHMENT:
            return hz_load_gpos_mark_to_base_attachment_subtable(&stream, lookup, subtable_index, format);

        case HZ_GPOS_LOOKUP_TYPE_MARK_TO_LIGATURE_ATTACHMENT:
            return hz_load_gpos_mark_to_ligature_attachment_subtable(&stream, lookup, subtable_index, format);

        case HZ_GPOS_LOOKUP_TYPE_MARK_TO_MARK_ATTACHMENT:
            return hz_load_gpos_mark_to_mark_attachment_subtable(&stream, lookup, subtable_index, format);

        case HZ_GPOS_LOOKUP_TYPE_CONTEXT_POSITIONING: {
            break;
        }

        case HZ_GPOS_LOOKUP_TYPE_CHAINED_CONTEXT_POSITIONING:
            return hz_load_gpos_chained_context_positioning_subtable(&stream, lookup, subtable_index, format);

        case HZ_GPOS_LOOKUP_TYPE_EXTENSION_POSITIONING: { // implemented inline
            if (format == 1) {
                lookup->lookup_type = Unpack16(&stream);
                lookup_type = lookup->lookup_type;
                Offset32 extension_offset = Unpack32(&stream);
                stream.data += extension_offset;
                stream.ptr = 0;
                goto extension_label;
            }

            return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
        }

        default:
            return HZ_ERROR_INVALID_LOOKUP_TYPE;
    }

    return HZ_OK;
}

HZ_STATIC hz_error_t
hz_load_gsub_lookup_table(hz_allocator_t *alctr,
                          hz_deserializer_t *ds,
                          hz_face_t *face,
                          hz_lookup_table_t *table)
{
    uint8_t buffer[4096];
    hz_memory_arena_t arena = hz_memory_arena_create(buffer, sizeof buffer);

    table->lookup_type = hz_deserializer_read_u16(ds);
    table->lookup_flags = hz_deserializer_read_u16(ds);
    table->subtable_count = hz_deserializer_read_u16(ds);

    if (table->subtable_count > 0) {
        Offset16 *offsets = hz_memory_arena_allocate(&arena, sizeof(Offset16) * table->subtable_count);
        hz_deserializer_read_u16_block(ds, offsets, table->subtable_count);

        // Set pointers to NULL by default
        table->subtables = hz_allocate(alctr, SIZEOF_VOIDPTR * table->subtable_count);
        memset(table->subtables, 0, SIZEOF_VOIDPTR * table->subtable_count); // null-out the subtale pointers

        // Load & parse subtables
        uint16_t original_lookup_type = table->lookup_type;
        for (uint16_t i = 0; i < table->subtable_count; ++i) {
            hz_deserializer_push_state(ds, offsets[i]);
            hz_read_gsub_lookup_subtable(alctr, ds, table, original_lookup_type, i);
            hz_deserializer_pop_state(ds);
        }
    }

    // Only include mark filtering set if flag USE_MARK_FILTERING_SET is enabled
    if (table->lookup_flags & HZ_LOOKUP_FLAG_USE_MARK_FILTERING_SET) {
        uint16_t mark_filtering_set_index = hz_deserializer_read_u16(ds);
        table->mark_filtering_set = face->mark_glyph_set[mark_filtering_set_index];
    }

    return HZ_OK;
}

#if 0 // GPOS FIXME
HZ_STATIC hz_error_t
hz_load_gpos_lookup_table(const uint8_t *data, hz_face_t *face, hz_lookup_table_t *table)
{
    hz_stream_t stream = hz_stream_create(data, 0);

    table->lookup_type = Unpack16(&stream);
    table->lookup_flags = Unpack16(&stream);
    table->subtable_count = Unpack16(&stream);

    if (table->subtable_count > 0) {
        Offset16 *offsets = hz_malloc(sizeof(Offset16) * table->subtable_count);
        UnpackArray16(&stream, table->subtable_count, offsets);

        // Set pointers to NULL by default
        table->subtables = hz_malloc(sizeof(*table->subtables) * table->subtable_count);
        for (uint16_t i = 0; i < table->subtable_count; ++i) {
            table->subtables[i] = NULL;
        }

        // Load & parse subtables
        uint16_t original_lookup_type = table->lookup_type;
        for (uint16_t i = 0; i < table->subtable_count; ++i) {
            hz_load_gpos_lookup_subtable(stream.data + offsets[i], table, original_lookup_type, i);
        }

        hz_free(offsets);
    }

    // Only include mark filtering set if flag USE_MARK_FILTERING_SET is enabled
    if (table->lookup_flags & HZ_LOOKUP_FLAG_USE_MARK_FILTERING_SET) {
        uint16_t mark_filtering_set_index = Unpack16(&stream);
        table->mark_filtering_set = face->mark_glyph_set[mark_filtering_set_index];
    } else {
        table->mark_filtering_set = NULL;
    }
    return HZ_OK;
}
#endif 

HZ_STATIC hz_error_t hz_shape_plan_load_gsub_table(hz_shape_plan_t *plan)
{
    uint8_t buffer[4096];
    hz_memory_arena_t arena = hz_memory_arena_create(buffer, sizeof buffer);
    hz_face_t *face = hz_font_get_face(plan->font);
    hz_deserializer_t ds = hz_deserializer_create(face->data, 1); 

    if (!face->gsub) {
         return HZ_ERROR_TABLE_DOES_NOT_EXIST;
    }

    hz_gsub_table_t *gsub_table = &plan->gsub_table;

    struct { uint16_t script_list_offset,
                      feature_list_offset,
                      lookup_list_offset,
                      feature_variations_offset; } hdr;

    hz_deserializer_push_state(&ds, face->gsub);
    gsub_table->version = hz_deserializer_read_u32(&ds);

    switch (gsub_table->version) {
        case 0x00010000: // 1.0
            cmdread(&ds, 1, &hdr, "www");
            break;
        case 0x00010001: // 1.1
            cmdread(&ds, 1, &hdr, "wwww");
            break;
        default: // error
            return HZ_ERROR_INVALID_TABLE_VERSION;
            break;
    }

    {
        // parse feature list table
        hz_deserializer_push_state(&ds, hdr.feature_list_offset);
        gsub_table->num_features = hz_deserializer_read_u16(&ds);
        gsub_table->features = hz_allocate(&plan->allocator, sizeof(hz_feature_list_item_t) * gsub_table->num_features);
        
        for (int i = 0; i < gsub_table->num_features; ++i) {
            hz_feature_list_item_t *it = &gsub_table->features[i];
            it->tag = hz_deserializer_read_u32(&ds);
            Offset16 offset = hz_deserializer_read_u16(&ds);
            hz_deserializer_push_state(&ds, offset);
            hz_load_feature_table(&plan->allocator, &ds, &it->table);
            hz_deserializer_pop_state(&ds);
        }

        hz_deserializer_pop_state(&ds);
    }

    {
        // parse lookup list table
        hz_deserializer_push_state(&ds, hdr.lookup_list_offset);
        gsub_table->num_lookups = hz_deserializer_read_u16(&ds);
        gsub_table->lookups = hz_allocate(&plan->allocator, sizeof(hz_lookup_table_t) * gsub_table->num_lookups);
        Offset16* offsets = hz_memory_arena_allocate(&arena, sizeof(Offset16) * gsub_table->num_lookups);
        hz_deserializer_read_u16_block(&ds, offsets, gsub_table->num_lookups);

        for (uint16_t i = 0; i < gsub_table->num_lookups; ++i) {
            hz_deserializer_push_state(&ds, offsets[i]);
            hz_load_gsub_lookup_table(&plan->allocator, &ds, face, &gsub_table->lookups[i]);
            hz_deserializer_pop_state(&ds);
        }

        hz_deserializer_pop_state(&ds);
    }

    hz_deserializer_pop_state(&ds);
    return HZ_OK;
}


HZ_STATIC hz_error_t
hz_shape_plan_load_gpos_table(hz_shape_plan_t *plan)
{
    hz_face_t *face = hz_font_get_face(plan->font);

    if (!face->gpos) {
        return HZ_ERROR_TABLE_DOES_NOT_EXIST;
    }

    hz_stream_t table = hz_stream_create(face->data + face->gpos, 0);
    hz_gpos_table_t *gpos_table = &plan->gpos_table;

    uint16_t script_list_offset, feature_list_offset, lookup_list_offset, feature_variations_offset;
    gpos_table->version = Unpack32(&table);

    switch (gpos_table->version) {
        case 0x00010000: /* 1.0 */
        Unpackv(&table, "hhh",
                &script_list_offset,
                &feature_list_offset,
                &lookup_list_offset);
        break;
        case 0x00010001: /* 1.1 */
        Unpackv(&table, "hhhi",
                &script_list_offset,
                &feature_list_offset,
                &lookup_list_offset,
                &feature_variations_offset);
        break;
        default: /* error */
        return HZ_ERROR_INVALID_TABLE_VERSION;
        break;
    }

    {
        // Load feature list table
        hz_stream_t stream = hz_stream_create(table.data + feature_list_offset, 0);
        uint16_t num_features = Unpack16(&stream);
        gpos_table->num_features = num_features;
        // gpos_table->features = hz_memory_pool_allocate(sizeof(hz_feature_list_item_t) * num_features);

        for (uint16_t i = 0; i < num_features; ++i) {
            gpos_table->features[i].tag = Unpack32(&stream);
            Offset16 offset = Unpack16(&stream);

            uintptr_t lastptr = stream_tell(&stream);
            stream_seek(&stream, offset);
            // hz_load_feature_table(&plan->memory_pool, &stream, &gpos_table->features[i].table);
            stream_seek(&stream, lastptr);
        }
    }

    {
        // Load lookups
        uint16_t i, num_lookups;
        Offset16 *offsets;

        hz_stream_t stream = hz_stream_create(table.data + lookup_list_offset, 0);
        num_lookups = Unpack16(&stream);

        gpos_table->num_lookups = num_lookups;
        gpos_table->lookups = hz_malloc(sizeof(hz_lookup_table_t) * num_lookups);

        offsets = hz_malloc(sizeof(Offset16) * num_lookups);
        UnpackArray16(&stream, num_lookups, offsets);

        for (i = 0; i < num_lookups; ++i) {
            // GPOS FIXME
            // hz_load_gpos_lookup_table(stream.data + offsets[i], face, &gpos_table->lookups[i]);
        }

        hz_free(offsets);
    }

    return HZ_OK;
}

HZ_STATIC void
hz_shape_plan_load_tables(hz_shape_plan_t *plan)
{
    hz_shape_plan_load_gsub_table(plan);
    //hz_shape_plan_load_gpos_table(plan);
}

HZ_STATIC hz_shape_plan_t *
hz_shape_plan_create(hz_font_t *font,
                     hz_segment_t *seg,
                     const hz_feature_t *features,
                     unsigned int num_features,
                     hz_shape_flags_t flags)
{
    hz_shape_plan_t *plan = hz_malloc(sizeof(hz_shape_plan_t));
    plan->font = font;
    plan->direction = seg->direction;
    plan->script = seg->script;
    plan->language = seg->language;
    plan->shape_flags = flags;
    plan->memory_pool = hz_memory_pool_create(8192);
    plan->allocator.allocate = &hz_memory_pool_allocate_func;
    plan->allocator.user = &plan->memory_pool;

    plan->features = NULL;
    plan->num_features = 0;

    if (features == NULL && (flags & HZ_AUTO_LOAD_FEATURES)) {
        // no feature list explicitly specified, load standard features for script
        hz_ot_script_load_features(seg->script, &plan->features, &plan->num_features);
    } else {
        plan->features = hz_allocate(&plan->allocator, sizeof(hz_feature_t) * num_features);
        plan->num_features = num_features;
        memcpy(plan->features, features, num_features * sizeof(hz_feature_t));
    }

    hz_shape_plan_load_tables(plan);
    return plan;
}

HZ_STATIC size_t
hz_segment_next_valid_index(hz_segment_t *seg,
                            size_t index,
                            uint16_t lookup_flag)
{

}


HZ_STATIC int64_t next_joining_arabic_glyph(hz_buffer_t *buffer, int64_t g, uint16_t lookup_flag, const hz_coverage_t *mark_filtering_set)
{
    size_t size = hz_vector_size(buffer->glyph_indices);
    do {
        if (++g >= size)
            return -1;

    } while (hz_should_ignore_glyph(buffer,g,lookup_flag, mark_filtering_set));

    return g;
}

HZ_STATIC int64_t prev_joining_arabic_glyph(hz_buffer_t *buffer, int64_t g, uint16_t lookup_flag, const hz_coverage_t *mark_filtering_set)
{
    do {
        if (--g < 0)
            return -1;

    } while (hz_should_ignore_glyph(buffer,g,lookup_flag, mark_filtering_set));

    return g;
}

typedef struct hz_arabic_joining_triplet_t {
    uint16_t prev_joining, curr_joining, next_joining;
    int init,fina,medi;
    int does_apply;
} hz_arabic_joining_triplet_t;

hz_arabic_joining_triplet_t
hz_shape_complex_arabic_joining(hz_buffer_t *buffer,
                                int64_t index,
                                uint16_t lookup_flag,
                                const hz_coverage_t *mark_filtering_set)
{
    hz_arabic_joining_triplet_t triplet;

    if (hz_shape_complex_arabic_char_joining(buffer->codepoints[index], &triplet.curr_joining)) {
        int64_t prev_index = prev_joining_arabic_glyph(buffer, index, HZ_LOOKUP_FLAG_IGNORE_MARKS, mark_filtering_set);
        int64_t next_index = next_joining_arabic_glyph(buffer, index, HZ_LOOKUP_FLAG_IGNORE_MARKS, mark_filtering_set);

        if (prev_index == -1) {
            triplet.prev_joining = NO_JOINING_GROUP | JOINING_TYPE_T;
        } else {
            if (!hz_shape_complex_arabic_char_joining(buffer->codepoints[prev_index], &triplet.prev_joining))
                triplet.prev_joining = NO_JOINING_GROUP | JOINING_TYPE_T;
        }

        if (next_index == -1) {
            triplet.next_joining = NO_JOINING_GROUP | JOINING_TYPE_T;
        } else {
            if (!hz_shape_complex_arabic_char_joining(buffer->codepoints[next_index], &triplet.next_joining))
                triplet.next_joining = NO_JOINING_GROUP | JOINING_TYPE_T;
        }

        triplet.init = triplet.curr_joining & (JOINING_TYPE_L | JOINING_TYPE_D)
                && triplet.next_joining & (JOINING_TYPE_R | JOINING_TYPE_D | JOINING_TYPE_C);

        triplet.fina = triplet.curr_joining & (JOINING_TYPE_R | JOINING_TYPE_D)
                && triplet.prev_joining & (JOINING_TYPE_L | JOINING_TYPE_D | JOINING_TYPE_C);

        triplet.medi = triplet.curr_joining & JOINING_TYPE_D
                && triplet.prev_joining & (JOINING_TYPE_L | JOINING_TYPE_C | JOINING_TYPE_D)
                && triplet.next_joining & (JOINING_TYPE_R | JOINING_TYPE_C | JOINING_TYPE_D);

        triplet.does_apply = 1;
    } else {
        triplet.does_apply = 0;
    }

    return triplet;
}

HZ_STATIC hz_bool_t hz_shape_complex_arabic_init(hz_buffer_t *buffer, int64_t index, uint16_t lookup_flag, const hz_coverage_t *mark_filtering_set)
{
    hz_arabic_joining_triplet_t triplet = hz_shape_complex_arabic_joining(buffer,index,lookup_flag, mark_filtering_set);
    return triplet.does_apply ? triplet.init && !(triplet.medi || triplet.fina) : HZ_FALSE;
}

HZ_STATIC hz_bool_t hz_shape_complex_arabic_medi(hz_buffer_t *buffer, int64_t index, uint16_t lookup_flag, const hz_coverage_t *mark_filtering_set)
{
    hz_arabic_joining_triplet_t triplet = hz_shape_complex_arabic_joining(buffer,index,lookup_flag, mark_filtering_set);
    return triplet.does_apply ? triplet.medi : HZ_FALSE;
}

HZ_STATIC hz_bool_t hz_shape_complex_arabic_fina(hz_buffer_t *buffer, int64_t index, uint16_t lookup_flag, const hz_coverage_t *mark_filtering_set)
{
    hz_arabic_joining_triplet_t triplet = hz_shape_complex_arabic_joining(buffer,index,lookup_flag,mark_filtering_set);
    return triplet.does_apply ? triplet.fina && !(triplet.medi || triplet.init) : HZ_FALSE;
}

HZ_STATIC hz_bool_t hz_shape_complex_arabic_isol(hz_buffer_t *buffer, int64_t index, uint16_t lookup_flag, const hz_coverage_t *mark_filtering_set)
{
    hz_arabic_joining_triplet_t triplet = hz_shape_complex_arabic_joining(buffer,index,lookup_flag,mark_filtering_set);
    return triplet.does_apply ? !(triplet.fina || triplet.medi || triplet.init) : HZ_FALSE;
}

HZ_STATIC hz_bool_t
hz_should_replace(hz_buffer_t *buffer,
                  hz_feature_t feature,
                  uint16_t node_index,
                  uint16_t lookup_flag,
                  const hz_coverage_t *mark_filtering_set)
{
    if (feature == HZ_FEATURE_INIT) {
        return hz_shape_complex_arabic_init(buffer, node_index, lookup_flag, mark_filtering_set);
    } else if (feature == HZ_FEATURE_MEDI) {
        return hz_shape_complex_arabic_medi(buffer, node_index, lookup_flag, mark_filtering_set);
    } else if (feature == HZ_FEATURE_FINA) {
        return hz_shape_complex_arabic_fina(buffer, node_index, lookup_flag, mark_filtering_set);
    } else if (feature == HZ_FEATURE_ISOL) {
        return hz_shape_complex_arabic_isol(buffer, node_index, lookup_flag, mark_filtering_set);
    }

    return HZ_TRUE;
}

typedef struct hz_range_t {
    hz_segment_sz_t mn, mx, base;
    hz_bool_t is_ignored;
} hz_range_t;

typedef struct {
    hz_vector(hz_range_t) ranges; // alternating ignored -> unignored ranges
    hz_vector(hz_segment_sz_t) ignored_indices;
    hz_vector(hz_segment_sz_t) unignored_indices;
} hz_range_list_t;

hz_range_list_t *hz_range_list_create(void)
{
    hz_range_list_t *list = hz_malloc(sizeof(*list));
    list->ranges = NULL;
    list->ignored_indices = NULL;
    list->unignored_indices = NULL;
    return list;
}

void hz_range_list_destroy(hz_range_list_t *list)
{
    hz_vector_destroy(list->ranges);
    hz_vector_destroy(list->ignored_indices);
    hz_vector_destroy(list->unignored_indices);
    hz_free(list);
}

// compute range list from buffer
hz_range_list_t *
hz_compute_range_list(hz_buffer_t *buffer, uint16_t lookup_flag, const hz_coverage_t *mark_filtering_set)
{
    hz_range_list_t *range_list = hz_range_list_create();

    hz_segment_sz_t mn=0,mx=0;
    hz_segment_sz_t ign_base = 0, nign_base = 0;

    for (size_t i = 0; i < hz_vector_size(buffer->glyph_indices); ++i) {
        hz_bool_t curr_ign = hz_should_ignore_glyph(buffer, i, lookup_flag, mark_filtering_set);
        // add current
        if (curr_ign) {
            hz_vector_push_back(range_list->ignored_indices, (hz_segment_sz_t)i);
        } else {
            hz_vector_push_back(range_list->unignored_indices, (hz_segment_sz_t)i);
        }

        if (i + 1 >= hz_vector_size(buffer->glyph_indices)) {
            hz_range_t range;
            range.mn = mn; range.mx = i;
            range.base = curr_ign ? ign_base : nign_base;
            range.is_ignored = curr_ign;
            hz_vector_push_back(range_list->ranges, range);
            break;
        }

        hz_bool_t next_ign = hz_should_ignore_glyph(buffer, i + 1, lookup_flag, mark_filtering_set);

        if (curr_ign != next_ign) {
            mx = i;
            hz_segment_sz_t span = (mx - mn) + 1;
            hz_range_t range;
            range.mn = mn; range.mx = mx;
            range.is_ignored = curr_ign;
            range.base = curr_ign ? ign_base : nign_base;
            hz_vector_push_back(range_list->ranges, range);
            mn = i+1;
            mx = mn;

            if (curr_ign) {
                ign_base += span;
            } else {
                nign_base += span;
            }
        }
    }

    return range_list;
}


static hz_segment_sz_t
hz_range_list_search(hz_range_list_t *range_list, hz_segment_sz_t i)
{
    // find range for index x using binary search algorithm
    hz_segment_sz_t low,mid,high;
    low = 0;
    high = hz_vector_size(range_list->ranges)-1;

    if (i >= 0 && i <= range_list->ranges[high].mx) {
        if (low == high) {
            return 0;
        }

        while (low <= high) {
            mid = (low + high) / 2;

            if (i < range_list->ranges[mid].mn) {
                // less
                high = mid-1;
            } else if (i > range_list->ranges[mid].mx) {
                // greater
                low = mid+1;
            } else {
                //  mn <= i <= mx
                // equal
                return mid;
            }
        }
    }

    return -1; // NOT FOUND
}

/*  Function: hz_search_prev_glyph
 *      Find previous glyph while making use of m1 and m2 glyph class masks.
 *      It searches for a glyph with m2 classes and ensures the glyphs until then match with m1.
 *      NOTE: this is useful to implement MARK_TO_BASE or MARK_TO_LIGATURE positioning.
 *
 *  Returns:
 *      The index of the glyph in case of success, otherwise it returns -1.
 */       
HZ_STATIC int hz_search_prev_glyph(hz_buffer_t *buffer, int index, uint16_t m1, uint16_t m2)
{
    --index;
    while (index >= 0) {
        if ((buffer->glyph_classes[index] & m2) == m2) {
            // found
            return index;
        } else if ((buffer->glyph_classes[index] & m1) != m1) {
            break;
        }

        --index;
    }

    return -1;
}

void
hz_shape_plan_apply_gsub_lookup(hz_shape_plan_t *plan,
                                hz_feature_t feature,
                                uint16_t lookup_index,
                                hz_buffer_t *in, hz_buffer_t *out,
                                int v1, int v2, int depth)
{
    uint8_t temp[4096];
    hz_memory_arena_t arena = hz_memory_arena_create(temp, sizeof temp);

    HZ_ASSERT(in != NULL && out != NULL);
    HZ_ASSERT(in != out);
    HZ_ASSERT(hz_buffer_contains_range(in,v1,v2));

    if (depth >= HZ_MAX_RECURSE_DEPTH) {
        // hit max recurse depth, wind back up the call stack
        return;
    }

    hz_gsub_table_t *gsub_table = &plan->gsub_table;
    hz_lookup_table_t *table = &gsub_table->lookups[lookup_index];
    hz_face_t *face = hz_font_get_face(plan->font);

    // copy segment glyph ids and info into a read-only buffer
    hz_buffer_t *b1, *b2;
    b1 = hz_buffer_create();
    b1->attrib_flags = in->attrib_flags;
    b2 = hz_buffer_create();
    b2->attrib_flags = HZ_GLYPH_ATTRIB_INDEX_BIT | HZ_GLYPH_ATTRIB_CODEPOINT_BIT | HZ_GLYPH_ATTRIB_COMPONENT_INDEX_BIT;
    hz_buffer_add_range(b1, in, v1, v2);

    for (uint16_t i = 0; i < table->subtable_count; ++i) {
        hz_lookup_subtable_t *base = table->subtables[i];
        // NOTE: based on lookup type, if it has no ability to grow or shrink the buffer,
        // it would be more efficient to use a single buffer if there is no cross-glyph interference.
        // Otherwise, a good alternative is to pre-allocate the second buffer
        // to be exactly the size of the first buffer in case where we know the size won't change.
        if (likely(base != NULL)) {
            // subtable requested is loaded
            hz_memory_arena_reset(&arena);
            hz_buffer_compute_info(b1, face);
            // reserve second buffer with size of first buffer as the result of the substitution is likely going to be
            // around the size of the first buffer in most cases.
            hz_buffer_reserve(b2, hz_vector_size(b1->glyph_indices));

            hz_range_list_t *range_list = hz_compute_range_list(b1, table->lookup_flags, &table->mark_filtering_set);

            switch (table->lookup_type) {
                case HZ_GSUB_LOOKUP_TYPE_SINGLE_SUBSTITUTION: {
                    switch (base->format) {
                        case 1: {
                            hz_single_substitution_format1_subtable_t *subtable = (hz_single_substitution_format1_subtable_t *)base;

                            for (size_t r = 0; r < hz_vector_size(range_list->ranges); ++r) {
                                const hz_range_t *range = &range_list->ranges[r];

                                if (range->is_ignored) {
                                    hz_buffer_add_range(b2, b1, range->mn, range->mx);
                                } else {
                                    // unignored
                                    for (hz_segment_sz_t g = range->mn; g <= range->mx; ++g) {
                                        int32_t index;
                                        if (hz_should_replace(b1, feature, g, table->lookup_flags, &table->mark_filtering_set)
                                            && (index = hz_coverage_search(&subtable->coverage, b1->glyph_indices[g])) != -1) {
                                            hz_buffer_add_glyph(b2, (hz_glyph_object_t) {
                                                    .id = b1->glyph_indices[g] + subtable->delta_glyph_id,
                                                    .codepoint = b1->codepoints[g],
                                                    .component_index = b1->component_indices[g]});
                                        } else {
                                            hz_buffer_add_glyph(b2, (hz_glyph_object_t) {
                                                    .id = b1->glyph_indices[g],
                                                    .codepoint = b1->codepoints[g],
                                                    .component_index = b1->component_indices[g]});
                                        }
                                    }
                                }
                            }
                            break;
                        }
                        case 2: {
                            hz_single_substitution_format2_subtable_t *subtable = (hz_single_substitution_format2_subtable_t *)base;

                            for (size_t r = 0; r < hz_vector_size(range_list->ranges); ++r) {
                                const hz_range_t *range = &range_list->ranges[r];

                                if (range->is_ignored) {
                                    hz_buffer_add_range(b2, b1, range->mn, range->mx);
                                } else {
                                    for (hz_segment_sz_t g = range->mn; g <= range->mx; ++g) {
                                        int32_t index;
                                        if (hz_should_replace(b1, feature, g, table->lookup_flags, &table->mark_filtering_set)
                                            && (index = hz_coverage_search(&subtable->coverage, b1->glyph_indices[g])) != -1) {
                                            hz_buffer_add_glyph(b2, (hz_glyph_object_t) {
                                                    .id = subtable->substitute_glyph_ids[index],
                                                    .codepoint = b1->codepoints[g],
                                                    .component_index = b1->component_indices[g]});
                                        } else {
                                            hz_buffer_add_glyph(b2, (hz_glyph_object_t) {
                                                    .id = b1->glyph_indices[g],
                                                    .codepoint = b1->codepoints[g],
                                                    .component_index = b1->component_indices[g]});
                                        }
                                    }
                                }
                            }
                            break;
                        }
                    }
                    break;
                }

#if 0
                case HZ_GSUB_LOOKUP_TYPE_MULTIPLE_SUBSTITUTION: {
                    switch (base->format) {
                        case 1: {
                            hz_multiple_substitution_format1_subtable_t *subtable = (hz_multiple_substitution_format1_subtable_t*) base;

                            for (size_t r = 0; r < hz_vector_size(range_list->ranges); ++r) {
                                const hz_range_t *range = &range_list->ranges[r];

                                if (range->is_ignored) {
                                    hz_buffer_add_range(b2, b1, range->mn, range->mx);
                                } else {
                                    for (hz_segment_sz_t g = range->mn; g <= range->mx; ++g) {
                                        int32_t index;
                                        if (hz_should_replace(b1, feature, g, table->lookup_flags, &table->mark_filtering_set)
                                            && (index = hz_coverage_search(&subtable->coverage, b1->glyph_indices[g])) != -1) {
                                            const hz_sequence_table_t *sequence = &subtable->sequences[index];

                                            for (uint16_t w = 0; w < sequence->glyph_count; ++w) {
                                                hz_buffer_add_glyph(b2, (hz_glyph_object_t) {
                                                        .id = sequence->glyphs[w],
                                                        .codepoint = b1->codepoints[g],
                                                        .component_index = b1->component_indices[g]});
                                            }
                                        } else {
                                            hz_buffer_add_glyph(b2, (hz_glyph_object_t) {
                                                    .id = b1->glyph_indices[g],
                                                    .codepoint = b1->codepoints[g],
                                                    .component_index = b1->component_indices[g]});
                                        }
                                    }
                                }
                            }

                            break;
                        }
                    }
                    break;
                }
#endif
                case HZ_GSUB_LOOKUP_TYPE_LIGATURE_SUBSTITUTION: {
                    switch (base->format) {
                        case 1: {
                            hz_ligature_substitution_format1_subtable_t *subtable = (hz_ligature_substitution_format1_subtable_t*) base;
                            // go over unignored index list

                            for (size_t r = 0; r < hz_vector_size(range_list->ranges); ++r) {
                                const hz_range_t *range = &range_list->ranges[r];

                                if (range->is_ignored) {
                                    hz_buffer_add_range(b2, b1, range->mn, range->mx);
                                } else {
                                    // unignored
                                    for (hz_segment_sz_t g = range->mn; g <= range->mx; ++g) {
                                        hz_bool_t matched = HZ_FALSE;

                                        int32_t index;
                                        if (hz_should_replace(b1, feature, g, table->lookup_flags, &table->mark_filtering_set)
                                            && (index = hz_coverage_search(&subtable->coverage, b1->glyph_indices[g])) != -1) {
                                            const hz_ligature_set_table_t *ligature_set = subtable->ligature_sets + index;

                                            // compare ligatures in ligature set to following unignored nodes for match
                                            for (uint16_t w = 0; w < ligature_set->ligature_count; ++w) {
                                                hz_ligature_t *ligature = &ligature_set->ligatures[w];
                                                uint16_t component_count = ligature->component_count;
                                                hz_segment_sz_t s1 = range->base + (g - range->mn);
                                                hz_segment_sz_t s2 = s1 + component_count - 1;
                                                if (s2 < hz_vector_size(range_list->unignored_indices)) {
                                                    int test = 1;
                                                    if (component_count >= 2) {
                                                        // There are enough unignored glyphs until the end of the buffer
                                                        // to load component glyphs. This could be possibly optimized later with
                                                        // SSE/AVX2 (gather,cmp,shuffle)
                                                        hz_memory_arena_reset(&arena);
                                                        hz_index_t *block = hz_memory_arena_allocate(&arena, (component_count - 1) * 2);
                                                        for (uint16_t k = 0; k < component_count-1; ++k) {
                                                            block[k] = b1->glyph_indices[range_list->unignored_indices[s1 + k + 1]];
                                                        }

                                                        test = !memcmp(ligature->component_glyph_ids, block, (component_count-1)*2);
                                                    }

                                                    if (test) {
                                                        // GID match found with ligature, push ligature glyph to buffer
                                                        //hz_vector_push(b2->glyph_indices, ligature->ligature_glyph);
                                                        hz_buffer_add_glyph(b2, (hz_glyph_object_t) {
                                                                .id = ligature->ligature_glyph,
                                                                .codepoint = 0,
                                                                .component_index = b1->component_indices[g]});

                                                        // Push ignored glyphs found within the matched range
                                                        for (int k = s1; k <= s2; ++k) {
                                                            int min_index = range_list->unignored_indices[k];
                                                            int max_index = range_list->unignored_indices[k+1];
                                                            for (int m = min_index+1; m<=(max_index-1); ++m) {
                                                                hz_buffer_add_glyph(b2, (hz_glyph_object_t) {
                                                                    .id = b1->glyph_indices[m],
                                                                    .codepoint = b1->codepoints[m],
                                                                    .component_index = k-s1});
                                                            }
                                                        }

                                                        // Jump over context
                                                        g = range_list->unignored_indices[s2+1]-1;
                                                        r = hz_range_list_search(range_list, g);
                                                        range = &range_list->ranges[r];

                                                        matched = HZ_TRUE;
                                                        goto check_match_gsub_4_1;
                                                    }
                                                }
                                            }
                                        }

                                        check_match_gsub_4_1:
                                        if (!matched) {
                                            hz_buffer_add_glyph(b2, (hz_glyph_object_t) {
                                                    .id = b1->glyph_indices[g],
                                                    .codepoint = b1->codepoints[g],
                                                    .component_index = b1->component_indices[g]});
                                        }
                                    }

                                }
                            }

                            break;
                        }
                        default:
                            break;
                    }
                    break;
                }
#if 0
                case HZ_GSUB_LOOKUP_TYPE_CHAINED_CONTEXTS_SUBSTITUTION: {
                    switch (base->format) {
                        case 1: {
                            hz_chained_sequence_context_format1_subtable_t *subtable = (hz_chained_sequence_context_format1_subtable_t *)base;

                            for (size_t r = 0; r < hz_vector_size(range_list->ranges); ++r) {
                                const hz_range_t *range = &range_list->ranges[r];

                                if (range->is_ignored) {
                                    hz_buffer_add_range(b2, b1, range->mn, range->mx);
                                } else {
                                    for (hz_segment_sz_t g = range->mn; g <= range->mx; ++g) {
                                        hz_bool_t match = HZ_FALSE;
                                        if (hz_should_replace(b1, feature, g, table->lookup_flags, &table->mark_filtering_set)
                                            && hz_coverage_contains(&subtable->coverage, b1->glyph_indices[g])) {
                                            for (uint16_t m = 0; m < subtable->rule_set_count; ++m) {
                                                hz_chained_sequence_rule_set_t *rs = &subtable->rule_sets[m];
                                                for (uint16_t n = 0; n < rs->count; ++n) {
                                                    // fill both context and sequence buffers, use memcmp to quickly check if they are matching
                                                    // context
                                                    hz_chained_sequence_rule_t *rule = &rs->rules[n];
                                                    int u = range->base + (g - range->mn);
                                                    int u1 = u - rule->prefix_count;
                                                    int u2 = (u + rule->input_count + rule->suffix_count) - 1;

                                                    if (u1 >= 0 && u2 <= hz_vector_size(range_list->unignored_indices) - 1 && u2 >= u1) {
                                                        int context_len = (u2-u1)+1;
                                                        uint16_t *sequence = hz_memory_arena_allocate(&arena, context_len * 2);
                                                        uint16_t *context = hz_memory_arena_allocate(&arena, context_len * 2);
                                                        {
                                                            // load sequence
                                                            for (int k = 0; k < context_len; ++k) {
                                                                // NOTE: this could use SIMD gather if the glyphs were
                                                                // aligned on a 32-bit (4-byte) boundary
                                                                sequence[k] = b1->glyph_indices[range_list->unignored_indices[u1 + k]];
                                                            }
                                                        }

                                                        {
                                                            //load context
                                                            int k = 0;
                                                            // start with prefix
                                                            for (uint16_t c = 0; c < rule->prefix_count; ++c) {
                                                                context[k++] = rule->prefix_sequence[c];
                                                            }

                                                            // load current glyph so memcmp doesn't fail
                                                            context[k++] = b1->glyph_indices[range_list->unignored_indices[u]];

                                                            for (uint16_t c = 0; c < rule->input_count-1; ++c) {
                                                                context[k++] = rule->input_sequence[c];
                                                            }

                                                            for (uint16_t c = 0; c < rule->suffix_count; ++c) {
                                                                context[k++] = rule->suffix_sequence[c];
                                                            }
                                                        }

                                                        // compare context with current glyph sequence
                                                        if (!memcmp(context, sequence, context_len*2)) {
                                                            // if match, apply nested lookups
                                                            hz_segment_sz_t context_low = range_list->unignored_indices[u];
                                                            hz_segment_sz_t context_high = range_list->unignored_indices[u + rule->input_count];

                                                            // create context from input glyphs
                                                            hz_buffer_t *ctx1 = hz_buffer_copy_range(b1, context_low, context_high);
                                                            hz_buffer_t *ctx2 = hz_buffer_create();
                                                            ctx2->attrib_flags = b2->attrib_flags;

                                                            for (uint16_t z = 0; z < rule->lookup_count; ++z) {
                                                                hz_buffer_compute_info(ctx1, face);
                                                                int *context_index_list = hz_buffer_get_unignored_indices(ctx1, table->lookup_flags, &table->mark_filtering_set);
                                                                int sequence_idx = context_index_list[rule->lookup_records[z].sequence_index];

                                                                hz_shape_plan_apply_gsub_lookup(plan, feature,
                                                                                                rule->lookup_records[z].lookup_list_index,
                                                                                                ctx1, ctx2,
                                                                                                sequence_idx,
                                                                                                sequence_idx,
                                                                                                depth + 1);

                                                                hz_swap_buffers(ctx1, ctx2, face);
                                                                hz_vector_destroy(context_index_list);
                                                            }

                                                            // add final result to b2
                                                            hz_buffer_add_other(b2, ctx1);

                                                            match = HZ_TRUE;
                                                            // skip over input context
                                                            g = context_high;
                                                            r = hz_range_list_search(range_list, g);
                                                            range = &range_list->ranges[r];

                                                            hz_buffer_destroy(ctx1);
                                                            hz_buffer_destroy(ctx2);
                                                            goto check_match_gsub_6_1;
                                                        }
                                                    }
                                                }
                                            }
                                        }

                                        check_match_gsub_6_1:
                                        if (!match) {
                                            hz_buffer_add_glyph(b2, (hz_glyph_object_t) {
                                                    .id = b1->glyph_indices[g],
                                                    .codepoint = b1->codepoints[g],
                                                    .component_index = b1->component_indices[g]});
                                        }
                                    }
                                }
                            }

                            break;
                        }

                        case 2: {
                            break;
                        }

                        case 3: {
                            hz_chained_sequence_context_format3_subtable_t *subtable = (hz_chained_sequence_context_format3_subtable_t *)base;

                            for (size_t r = 0; r < hz_vector_size(range_list->ranges); ++r) {
                                const hz_range_t *range = &range_list->ranges[r];

                                if (range->is_ignored) {
                                    hz_buffer_add_range(b2, b1, range->mn, range->mx);
                                } else {
                                    // unignored
                                    for (short g = range->mn; g <= range->mx; ++g) {
                                        hz_bool_t match = HZ_FALSE;
                                        if (hz_should_replace(b1, feature, g, table->lookup_flags, &table->mark_filtering_set)) {
                                            // context bounds check, if this doesn't fit inside the original range
                                            // this context is impossible to match
                                            int u = range->base + (g - range->mn);
                                            int u1 = u - subtable->prefix_count;
                                            int u2 = (u + subtable->input_count + subtable->suffix_count)-1;

                                            if (u1 >= 0 && u2 <= hz_vector_size(range_list->unignored_indices) - 1) {
                                                // check prefix, input and suffix sequences for match

                                                int prefix_match = 1, input_match = 1, suffix_match = 1; // matches

                                                // prefix (reverse order according to spec.)
                                                for (int k = 0; k < subtable->prefix_count; ++k) {
                                                    if (!hz_map_contains(subtable->prefix_maps[k],
                                                                         b1->glyph_indices[range_list->unignored_indices[
                                                                                 u - (k + 1)]])) {
                                                        prefix_match = 0;
                                                        break;
                                                    }
                                                }

                                                // input
                                                for (int k = 0; k < subtable->input_count; ++k) {
                                                    if (!hz_map_contains(subtable->input_maps[k],
                                                                         b1->glyph_indices[range_list->unignored_indices[
                                                                                 u + k]])) {
                                                        input_match = 0;
                                                        break;
                                                    }
                                                }

                                                // suffix
                                                for (int k = 0; k < subtable->suffix_count; ++k) {
                                                    if (!hz_map_contains(subtable->suffix_maps[k],
                                                                         b1->glyph_indices[range_list->unignored_indices[
                                                                                 u + subtable->input_count + k]])) {
                                                        suffix_match = 0;
                                                        break;
                                                    }
                                                }

                                                if (input_match && suffix_match && prefix_match) {
                                                    // if match, apply nested lookups

                                                    hz_segment_sz_t context_low = range_list->unignored_indices[u];
                                                    hz_segment_sz_t context_high = range_list->unignored_indices[u + subtable->input_count - 1];

                                                    // create context from input glyphs
                                                    hz_buffer_t *ctx1 = hz_buffer_copy_range(b1, context_low, context_high);
                                                    hz_buffer_t *ctx2 = hz_buffer_create();
                                                    ctx2->attrib_flags = b2->attrib_flags;

                                                    for (uint16_t z = 0; z < subtable->lookup_count; ++z) {
                                                        hz_buffer_compute_info(ctx1, face);
                                                        hz_vector(int) context_index_list = hz_buffer_get_unignored_indices(ctx1, table->lookup_flags, &table->mark_filtering_set);
                                                        int sequence_idx = context_index_list[subtable->lookup_records[z].sequence_index];
                                                        hz_vector_destroy(context_index_list);

                                                        hz_shape_plan_apply_gsub_lookup(plan, feature,
                                                                                        subtable->lookup_records[z].lookup_list_index,
                                                                                        ctx1, ctx2,
                                                                                        sequence_idx, sequence_idx,
                                                                                        depth + 1);

                                                        hz_swap_buffers(ctx1, ctx2, face);
                                                    }

                                                    // add final result to b2
                                                    hz_buffer_add_other(b2, ctx1);

                                                    match = HZ_TRUE;

                                                    g = context_high;
                                                    r = hz_range_list_search(range_list, g);
                                                    range = &range_list->ranges[r];

                                                    hz_buffer_destroy(ctx1);
                                                    hz_buffer_destroy(ctx2);
                                                    goto check_match_gsub_6_3;
                                                }
                                            }
                                        }

                                        check_match_gsub_6_3:
                                        if (!match) {
                                            hz_buffer_add_glyph(b2, (hz_glyph_object_t) {
                                                    .id = b1->glyph_indices[g],
                                                    .codepoint = b1->codepoints[g],
                                                    .component_index = b1->component_indices[g]});
                                        }
                                    }
                                }
                            }

                            break;
                        }
                        default:  assert(0);
                    }
                    break;
                }
#endif
                default:
                    continue;
            }

            // move glyphs from source buffer to destination buffer
            hz_swap_buffers(b1, b2, face);
            hz_range_list_destroy(range_list);
        }
    }

    // write slices caused by range into the output buffer
    hz_buffer_add_range(out, in, 0, v1 - 1);
    hz_buffer_add_other(out, b1);
    hz_buffer_add_range(out, in, v2 + 1, (int) hz_vector_size(in->glyph_indices) - 1);

    // cleanup buffers
    hz_buffer_destroy(b1);
    hz_buffer_destroy(b2);
}

void hz_apply_value_record_adjustments(hz_glyph_metrics_t *metrics,
                                       const hz_value_record_t *value_record,
                                       uint16_t value_format)
{
    if (value_format & HZ_VALUE_FORMAT_X_ADVANCE)
        metrics->xAdvance += value_record->xAdvance;
    if (value_format & HZ_VALUE_FORMAT_Y_ADVANCE)
        metrics->yAdvance  += value_record->yAdvance ;
    if (value_format & HZ_VALUE_FORMAT_X_PLACEMENT)
        metrics->xOffset += value_record->xPlacement;
    if (value_format & HZ_VALUE_FORMAT_Y_PLACEMENT)
        metrics->yOffset += value_record->yPlacement;
}

#if 0 // GPOS FIXME
void
hz_shape_plan_apply_gpos_lookup(hz_shape_plan_t *plan,
                                hz_feature_t feature,
                                uint16_t lookup_index,
                                hz_buffer_t *in, hz_buffer_t *out,
                                int v1, int v2, int depth)
{
    uint8_t temp[4096];
    hz_memory_arena_t arena = hz_memory_arena_create(temp, sizeof temp);

    HZ_ASSERT(in != NULL && out != NULL);
    HZ_ASSERT(in != out);
    HZ_ASSERT(hz_buffer_contains_range(in,v1,v2));

    if (depth >= HZ_MAX_RECURSE_DEPTH) {
        // hit max recurse depth, wind back up the call stack
        return;
    }

    const hz_lookup_table_t *table = &plan->gpos_table.lookups[lookup_index];
    hz_face_t *face = hz_font_get_face(plan->font);

    // copy segment glyph ids and info into a read-only buffer
    hz_buffer_t *b1, *b2;
    b1 = hz_buffer_create();
    b1->attrib_flags = in->attrib_flags;
    b2 = hz_buffer_create();
    b2->attrib_flags = HZ_GLYPH_ATTRIB_METRICS_BIT;
    hz_buffer_add_range(b1, in, v1, v2);

    for (uint16_t i = 0; i < table->subtable_count; ++i) {
        hz_lookup_subtable_t *base = table->subtables[i];
        if (likely(base != NULL)) {
            // subtable requested is loaded
            hz_memory_arena_reset(&arena);
            hz_buffer_compute_info(b1, face);
            hz_range_list_t *rangeList = hz_compute_range_list(b1, table->lookup_flags, table->mark_filtering_set);

            switch (table->lookup_type) {
                case HZ_GPOS_LOOKUP_TYPE_SINGLE_ADJUSTMENT: {
                    switch (base->format) {
                        case 1: {
                            hz_single_adjustment_format1_subtable_t *subtable = (hz_single_adjustment_format1_subtable_t *)base;
                            for (size_t r = 0; r < hz_vector_size(rangeList->ranges); ++r) {
                                hz_range_t *range = &rangeList->ranges[r];

                                if (range->is_ignored) {
                                    hz_buffer_add_range(b2, b1, range->mn, range->mx);
                                } else {
                                    for (hz_segment_sz_t g = range->mn; g <= range->mx; ++g) {
                                        hz_glyph_metrics_t metrics = b1->glyph_metrics[g];

                                        if (hz_should_replace(b1, feature, g, table->lookup_flags,
                                                              table->mark_filtering_set) &&
                                            hz_map_contains(subtable->coverage, b1->glyph_indices[g])) {
                                            hz_apply_value_record_adjustments(&metrics, &subtable->value_record,
                                                                              subtable->value_format);
                                        }

                                        hz_buffer_add_glyph(b2, (hz_glyph_object_t) {
                                                .metrics = metrics
                                        });
                                    }
                                }
                            }

                            break;
                        }

                        case 2: {
                            hz_single_adjustment_format2_subtable_t *subtable = (hz_single_adjustment_format2_subtable_t *)base;
                            for (size_t r = 0; r < hz_vector_size(rangeList->ranges); ++r) {
                                hz_range_t *range = &rangeList->ranges[r];

                                if (range->is_ignored) {
                                    hz_buffer_add_range(b2, b1, range->mn, range->mx);
                                } else {
                                    for (hz_segment_sz_t g = range->mn; g <= range->mx; ++g) {
                                        hz_glyph_metrics_t metrics = b1->glyph_metrics[g];

                                        if (hz_should_replace(b1, feature, g, table->lookup_flags,
                                                              table->mark_filtering_set) &&
                                            hz_map_contains(subtable->coverage, b1->glyph_indices[g])) {
                                            uint16_t record_index = hz_map_get_value(subtable->coverage,
                                                                                     b1->glyph_indices[g]);
                                            hz_apply_value_record_adjustments(&metrics,
                                                                              &subtable->value_records[record_index],
                                                                              subtable->value_format);
                                        }

                                        hz_buffer_add_glyph(b2, (hz_glyph_object_t) {
                                                .metrics = metrics
                                        });
                                    }
                                }
                            }
                            break;
                        }
                    }
                    break;
                }

                case HZ_GPOS_LOOKUP_TYPE_PAIR_ADJUSTMENT: {
                    switch (base->format) {
                        case 1: {
                            hz_pair_pos_format1_subtable_t *subtable = (hz_pair_pos_format1_subtable_t *)base;
                            for (size_t r = 0; r < hz_vector_size(rangeList->ranges); ++r) {
                                hz_range_t *range = &rangeList->ranges[r];

                                if (range->is_ignored) {
                                    hz_buffer_add_range(b2, b1, range->mn, range->mx);
                                } else {
                                    for (hz_segment_sz_t g = range->mn; g <= range->mx; ++g) {
                                        if (hz_should_replace(b1, feature, g, table->lookup_flags,
                                                              table->mark_filtering_set)
                                            && hz_map_contains(subtable->coverage, b1->glyph_indices[g])) {
                                            hz_segment_sz_t base_idx = range->base + (g - range->mn);

                                            if (base_idx+1 < hz_vector_size(rangeList->unignored_indices)) {
                                                hz_segment_sz_t g2 = rangeList->unignored_indices[base_idx + 1];
                                                uint32_t set_index = hz_map_get_value(subtable->coverage,
                                                                                      b1->glyph_indices[g]);
                                                hz_pair_set_t *pair_set = &subtable->pair_sets[set_index];

                                                for (uint16_t pv = 0; pv < pair_set->pair_value_count; ++pv) {
                                                    hz_pair_value_record_t *pair_value_record = &pair_set->pair_value_records[pv];
                                                    if (pair_value_record->second_glyph == b1->glyph_indices[g2]) {
                                                        // HACK: Adjust the two glyphs in b1 directly,
                                                        // haven't had time or peace to think about what's best to do in this case
                                                        // where we have to write to multiple locations. This should be good enough
                                                        // in most cases.
                                                        hz_apply_value_record_adjustments(&b1->glyph_metrics[g],
                                                                                          &pair_value_record->value_record1,
                                                                                          subtable->value_format1);

                                                        hz_apply_value_record_adjustments(&b1->glyph_metrics[g2],
                                                                                          &pair_value_record->value_record2,
                                                                                          subtable->value_format2);

                                                        break;
                                                    }
                                                }
                                            }
                                        }

                                        hz_buffer_add_glyph(b2, (hz_glyph_object_t) {
                                                .metrics = b1->glyph_metrics[g]
                                        });
                                    }
                                }
                            }


                            break;
                        }

                        case 2: {
                            hz_pair_pos_format2_subtable_t *subtable = (hz_pair_pos_format2_subtable_t *)base;
                            for (size_t r = 0; r < hz_vector_size(rangeList->ranges); ++r) {
                                hz_range_t *range = &rangeList->ranges[r];

                                if (range->is_ignored) {
                                    hz_buffer_add_range(b2, b1, range->mn, range->mx);
                                } else {
                                    // unignored
                                    for (hz_segment_sz_t g = range->mn; g <= range->mx; ++g) {
                                        if (hz_should_replace(b1, feature, g, table->lookup_flags,
                                                              table->mark_filtering_set)
                                            && hz_map_contains(subtable->coverage, b1->glyph_indices[g])) {
                                            hz_segment_sz_t base_idx = range->base + (g - range->mn);

                                            if (base_idx+1 < hz_vector_size(rangeList->unignored_indices)) {
                                                hz_segment_sz_t g2 = rangeList->unignored_indices[base_idx + 1];
                                                if (hz_map_contains(subtable->class_def1, b1->glyph_classes[g])
                                                    && hz_map_contains(subtable->class_def2, b1->glyph_classes[g2])) {
                                                    uint32_t class1_idx = hz_map_get_value(subtable->class_def1,
                                                                                           b1->glyph_classes[g]);
                                                    uint32_t class2_idx = hz_map_get_value(subtable->class_def2,
                                                                                           b1->glyph_classes[g2]);

                                                    const hz_class2_record_t *class2_record = &subtable->class1_records[class1_idx].class2_records[class2_idx];
                                                    hz_apply_value_record_adjustments(&b1->glyph_metrics[g],
                                                                                      &class2_record->value_record1,
                                                                                      subtable->value_format1);

                                                    hz_apply_value_record_adjustments(&b1->glyph_metrics[g2],
                                                                                      &class2_record->value_record2,
                                                                                      subtable->value_format2);
                                                }
                                            }
                                        }

                                        hz_buffer_add_glyph(b2, (hz_glyph_object_t) {
                                                .metrics = b1->glyph_metrics[g]
                                        });
                                    }
                                }
                            }

                            break;
                        }

                        default:
                            // error
                            break;
                    }
                    break;
                }
#if 0
                case HZ_GPOS_LOOKUP_TYPE_CURSIVE_ATTACHMENT: {
                    break;
                }
#endif
                case HZ_GPOS_LOOKUP_TYPE_MARK_TO_BASE_ATTACHMENT: {
                    switch (base->format) {
                        case 1: {
                            hz_mark_to_base_attachment_subtable_t *subtable = (hz_mark_to_base_attachment_subtable_t *)base;

                            for (size_t r = 0; r < hz_vector_size(rangeList->ranges); ++r) {
                                hz_range_t *range = &rangeList->ranges[r];

                                if (range->is_ignored) {
                                    hz_buffer_add_range(b2, b1, range->mn, range->mx);
                                } else {
                                    // unignored
                                    for (hz_segment_sz_t g = range->mn; g <= range->mx; ++g) {
                                        hz_glyph_metrics_t metrics = b1->glyph_metrics[g];

                                        if (hz_should_replace(b1, feature, g, table->lookup_flags,
                                                              table->mark_filtering_set)
                                            && hz_map_contains(subtable->mark_coverage, b1->glyph_indices[g])) {
                                            int prev_base = hz_search_prev_glyph(b1, g, HZ_GLYPH_CLASS_MARK, HZ_GLYPH_CLASS_BASE);
                                            if (prev_base != -1) {
                                                if (hz_map_contains(subtable->mark_coverage, b1->glyph_indices[g])
                                                    && hz_map_contains(subtable->base_coverage,
                                                                       b1->glyph_indices[prev_base])) {
                                                    // both coverages match
                                                    uint16_t mark_index = hz_map_get_value(subtable->mark_coverage, b1->glyph_indices[g]);
                                                    uint16_t base_index = hz_map_get_value(subtable->base_coverage, b1->glyph_indices[prev_base]);
                                                    hz_mark_record_t *mark_record = &subtable->mark_array.mark_records[mark_index];
                                                    hz_anchor_t *base_anchor = &subtable->base_array.base_records[base_index].base_anchors[mark_record->mark_class];
                                                    hz_anchor_t *mark_anchor = &mark_record->mark_anchor;

                                                    hz_glyph_metrics_t base_metrics = b1->glyph_metrics[prev_base];
                                                    int32_t placement_x1 = mark_anchor->xCoord;
                                                    int32_t placement_y1 = mark_anchor->yCoord;
                                                    int32_t placement_x2 = base_anchor->xCoord + base_metrics.xOffset;
                                                    int32_t placement_y2 = base_anchor->yCoord + base_metrics.yOffset;

                                                    int32_t xOffset = placement_x2 - placement_x1;
                                                    int32_t yOffset = placement_y2 - placement_y1;

                                                    metrics.xOffset = xOffset;
                                                    metrics.yOffset = yOffset;
                                                }
                                            }
                                        }

                                        hz_buffer_add_glyph(b2, (hz_glyph_object_t) {
                                                .metrics = metrics
                                        });
                                    }
                                }
                            }

                            break;
                        }
                    }
                    break;
                }

                case HZ_GPOS_LOOKUP_TYPE_MARK_TO_LIGATURE_ATTACHMENT: {
                    hz_mark_to_ligature_attachment_format1_subtable_t *subtable = (hz_mark_to_ligature_attachment_format1_subtable_t *)base;
                    for (size_t r = 0; r < hz_vector_size(rangeList->ranges); ++r) {
                        hz_range_t *range = &rangeList->ranges[r];
                        if (range->is_ignored) {
                            hz_buffer_add_range(b2, b1, range->mn, range->mx);
                        } else {
                            for (hz_segment_sz_t g = range->mn; g <= range->mx; ++g) {
                                hz_glyph_metrics_t metrics = b1->glyph_metrics[g];
                        
                                if (hz_should_replace(b1, feature, g, table->lookup_flags, table->mark_filtering_set)
                                    && hz_map_contains(subtable->mark_coverage, b1->glyph_indices[g])) {
                                    int prev_ligature = hz_search_prev_glyph(b1, g, HZ_GLYPH_CLASS_MARK, HZ_GLYPH_CLASS_LIGATURE);
                                    if (prev_ligature != -1) {
                                        if (hz_map_contains(subtable->ligature_coverage, b1->glyph_indices[prev_ligature])) {
                                            // both coverages match
                                            uint16_t component_index = b1->component_indices[g];
                                            uint16_t attachment_class = b1->attachment_classes[g];
                                            
                                            uint16_t mark_index = hz_map_get_value(subtable->mark_coverage, b1->glyph_indices[g]);
                                            uint16_t ligature_index = hz_map_get_value(subtable->ligature_coverage, b1->glyph_indices[prev_ligature]);
                                            hz_mark_record_t *mark_record = &subtable->mark_array.mark_records[mark_index];
                                            hz_ligature_attachment_t *ligature_attachment = &subtable->ligature_array.ligature_attachments[ligature_index];
                                            hz_component_record_t *component = &ligature_attachment->component_records[component_index];
                                            hz_anchor_t *mark_anchor = &mark_record->mark_anchor;
                                            hz_anchor_t *ligature_anchor = &component->ligature_anchors[mark_record->mark_class];

                                            hz_glyph_metrics_t lig_metrics = b1->glyph_metrics[prev_ligature];
                                            int32_t placement_x1 = mark_anchor->xCoord;
                                            int32_t placement_y1 = mark_anchor->yCoord;
                                            int32_t placement_x2 = ligature_anchor->xCoord + lig_metrics.xOffset;
                                            int32_t placement_y2 = ligature_anchor->yCoord + lig_metrics.yOffset;// + lig_metrics.yAdvance;// - lig_metrics.yAdvance;

                                            int32_t xOffset = placement_x2 - placement_x1;// + lig_metrics.xAdvance;
                                            int32_t yOffset = placement_y2 - placement_y1;// + lig_metrics.yAdvance;

                                            metrics.xOffset = xOffset;
                                            metrics.yOffset = yOffset;
                                        }   
                                    }
                                }

                                hz_buffer_add_glyph(b2, (hz_glyph_object_t) {
                                    .metrics = metrics
                                });
                            }
                        }
                    }

                    break;
                }

                case HZ_GPOS_LOOKUP_TYPE_MARK_TO_MARK_ATTACHMENT: {
                    switch (base->format) {
                        case 1 : {
                            hz_mark_to_mark_attachment_format1_subtable_t *subtable = (hz_mark_to_mark_attachment_format1_subtable_t *)base;
                            for (size_t r = 0; r < hz_vector_size(rangeList->ranges); ++r) {
                                hz_range_t *range = &rangeList->ranges[r];

                                if (range->is_ignored) {
                                    hz_buffer_add_range(b2, b1, range->mn, range->mx);
                                } else {
                                    for (hz_segment_sz_t g = range->mn; g <= range->mx; ++g) {
                                        hz_glyph_metrics_t metrics = b1->glyph_metrics[g];

                                        if (hz_should_replace(b1, feature, g, table->lookup_flags, table->mark_filtering_set)) {
                                            int prev_mark = hz_search_prev_glyph(b1, g, HZ_GLYPH_CLASS_MARK, HZ_GLYPH_CLASS_MARK);//range->base + (g - range->mn) - 1;
                                            if (prev_mark != -1) {
                                                if (hz_map_contains(subtable->mark2_coverage, b1->glyph_indices[prev_mark]) &&
                                                    hz_map_contains(subtable->mark1_coverage, b1->glyph_indices[g])
                                                    && (b1->component_indices[g] == b1->component_indices[prev_mark]))
                                                {
                                                    // valid second mark found
                                                    uint16_t mark1_index = hz_map_get_value(subtable->mark1_coverage, b1->glyph_indices[g]);
                                                    uint16_t mark2_index = hz_map_get_value(subtable->mark2_coverage, b1->glyph_indices[prev_mark]);
                                                    hz_mark_record_t *mark_record = &subtable->mark1_array.mark_records[mark1_index];
                                                    hz_mark2_record_t *mark2_record = &subtable->mark2_array.mark2_records[mark2_index];
                                                    hz_anchor_t *base_anchor = &mark2_record->mark2_anchors[mark_record->mark_class];
                                                    hz_anchor_t *mark_anchor = &mark_record->mark_anchor;

                                                    hz_glyph_metrics_t base_metrics = b1->glyph_metrics[prev_mark];
                                                    int32_t placement_x1 = mark_anchor->xCoord;
                                                    int32_t placement_y1 = mark_anchor->yCoord;
                                                    int32_t placement_x2 = base_anchor->xCoord + base_metrics.xOffset;
                                                    int32_t placement_y2 = base_anchor->yCoord + base_metrics.yOffset;

                                                    int32_t xOffset = placement_x2 - placement_x1;
                                                    int32_t yOffset = placement_y2 - placement_y1;

                                                    metrics.xOffset = xOffset;
                                                    metrics.yOffset = yOffset;
                                                }
                                            }
                                        }

                                        hz_buffer_add_glyph(b2, (hz_glyph_object_t) {
                                                .metrics = metrics
                                        });
                                    }
                                }
                            }

                            break;
                        }
                    }
                    break;
                }
#if 0
                case HZ_GPOS_LOOKUP_TYPE_CONTEXT_POSITIONING: {
                    break;
                }
#endif

                case HZ_GPOS_LOOKUP_TYPE_CHAINED_CONTEXT_POSITIONING: {
                    switch (base->format) {
                        case 1: {
                            hz_chained_sequence_context_format1_subtable_t *subtable = (hz_chained_sequence_context_format1_subtable_t *)base;

                            for (size_t r = 0; r < hz_vector_size(rangeList->ranges); ++r) {
                                const hz_range_t *range = &rangeList->ranges[r];

                                if (range->is_ignored) {
                                    hz_buffer_add_range(b2, b1, range->mn, range->mx);
                                } else {
                                    for (hz_segment_sz_t g = range->mn; g <= range->mx; ++g) {
                                        hz_bool_t match = HZ_FALSE;
                                        if (hz_should_replace(b1, feature, g, table->lookup_flags,
                                                              table->mark_filtering_set)
                                            && hz_map_contains(subtable->coverage, b1->glyph_indices[g]))  {
                                            for (uint16_t m = 0; m < subtable->rule_set_count; ++m) {
                                                hz_chained_sequence_rule_set_t *rs = &subtable->rule_sets[m];
                                                for (uint16_t n = 0; n < rs->count; ++n) {
                                                    // fill both context and sequence buffers, use memcmp to quickly check if they are matching
                                                    // context
                                                    hz_chained_sequence_rule_t *rule = &rs->rules[n];
                                                    int u = range->base + (g - range->mn);
                                                    int u1 = u - rule->prefix_count;
                                                    int u2 = (u + rule->input_count + rule->suffix_count) - 1;

                                                    if (u1 >= 0 && u2 <= hz_vector_size(rangeList->unignored_indices) - 1 && u2 >= u1) {
                                                        int context_len = (u2-u1)+1;
                                                        uint16_t *sequence = hz_memory_arena_allocate(&arena, context_len * 2);
                                                        uint16_t *context = hz_memory_arena_allocate(&arena, context_len * 2);
                                                        {
                                                            // load sequence
                                                            for (int k = 0; k < context_len; ++k) {
                                                                // NOTE: this could use SIMD gather if the glyphs were
                                                                // aligned on a 32-bit (4-byte) boundary
                                                                sequence[k] = b1->glyph_indices[rangeList->unignored_indices[u1 + k]];
                                                            }
                                                        }

                                                        {
                                                            //load context
                                                            int k = 0;
                                                            // start with prefix
                                                            for (uint16_t c = 0; c < rule->prefix_count; ++c) {
                                                                context[k++] = rule->prefix_sequence[c];
                                                            }

                                                            // load current glyph so memcmp doesn't fail
                                                            context[k++] = b1->glyph_indices[rangeList->unignored_indices[u]];

                                                            for (uint16_t c = 0; c < rule->input_count-1; ++c) {
                                                                context[k++] = rule->input_sequence[c];
                                                            }

                                                            for (uint16_t c = 0; c < rule->suffix_count; ++c) {
                                                                context[k++] = rule->suffix_sequence[c];
                                                            }
                                                        }

                                                        // compare context with current glyph sequence
                                                        if (!memcmp(context, sequence, context_len*2)) {
                                                            // if match, apply nested lookups
                                                            int context_low = rangeList->unignored_indices[u];
                                                            int context_high = rangeList->unignored_indices[u + rule->input_count];

                                                            // create context from input glyphs
                                                            hz_buffer_t *ctx1 = hz_buffer_copy_range(b1, context_low, context_high);
                                                            hz_buffer_t *ctx2 = hz_buffer_create();
                                                            ctx2->attrib_flags = b2->attrib_flags;

                                                            for (uint16_t z = 0; z < rule->lookup_count; ++z) {
                                                                hz_buffer_compute_info(ctx1, face);
                                                                int *context_index_list = hz_buffer_get_unignored_indices(ctx1, table->lookup_flags, table->mark_filtering_set);
                                                                int sequence_idx = context_index_list[rule->lookup_records[z].sequence_index];

                                                                hz_shape_plan_apply_gpos_lookup(plan, feature,
                                                                                                rule->lookup_records[z].lookup_list_index,
                                                                                                ctx1, ctx2,
                                                                                                sequence_idx,
                                                                                                sequence_idx,
                                                                                                depth + 1);

                                                                hz_swap_buffers(ctx1, ctx2, face);
                                                                hz_vector_destroy(context_index_list);
                                                            }

                                                            // add final result to b2
                                                            hz_buffer_add_other(b2, ctx1);

                                                            match = HZ_TRUE;
                                                            // skip over input context
                                                            int skip_loc = u + rule->input_count;
                                                            g = rangeList->unignored_indices[skip_loc];
                                                            r = hz_range_list_search(rangeList, g);
                                                            range = &rangeList->ranges[r];

                                                            hz_buffer_destroy(ctx1);
                                                            hz_buffer_destroy(ctx2);
                                                            goto check_match_gpos_6_1;
                                                        }
                                                    }
                                                }
                                            }
                                        }

                                        check_match_gpos_6_1:
                                        if (!match) {
                                            hz_buffer_add_glyph(b2, (hz_glyph_object_t) {
                                                    .id = b1->glyph_indices[g],
                                                    .codepoint = b1->codepoints[g]});
                                        }
                                    }

                                }

                            }

                            break;
                        }

                        case 2: {
                            break;
                        }

                        case 3: {
                            hz_chained_sequence_context_format3_subtable_t *subtable = (hz_chained_sequence_context_format3_subtable_t *)base;

                            for (size_t r = 0; r < hz_vector_size(rangeList->ranges); ++r) {
                                const hz_range_t *range = &rangeList->ranges[r];

                                if (range->is_ignored) {
                                    hz_buffer_add_range(b2, b1, range->mn, range->mx);
                                } else {
                                    // unignored
                                    for (short g = range->mn; g <= range->mx; ++g) {
                                        hz_bool_t match = HZ_FALSE;
                                        hz_glyph_metrics_t metrics = b1->glyph_metrics[g];

                                        if (hz_should_replace(b1, feature, g, table->lookup_flags,
                                                              table->mark_filtering_set)) {
                                            // context bounds check, if this doesn't fit inside the original range
                                            // this context is impossible to match
                                            int u = range->base + (g - range->mn);
                                            int u1 = u - subtable->prefix_count;
                                            int u2 = (u + subtable->input_count + subtable->suffix_count)-1;

                                            if (u1 >= 0 && u2 <= hz_vector_size(rangeList->unignored_indices) - 1) {
                                                // check prefix, input and suffix sequences for match

                                                int prefix_match = 1, input_match = 1, suffix_match = 1; // matches

                                                // prefix (reverse order according to spec.)
                                                for (int k = 0; k < subtable->prefix_count; ++k) {
                                                    if (!hz_map_contains(subtable->prefix_maps[k],
                                                                         b1->glyph_indices[rangeList->unignored_indices[
                                                                                 u - (k + 1)]])) {
                                                        prefix_match = 0;
                                                        break;
                                                    }
                                                }

                                                // input
                                                for (int k = 0; k < subtable->input_count; ++k) {
                                                    if (!hz_map_contains(subtable->input_maps[k],
                                                                         b1->glyph_indices[rangeList->unignored_indices[
                                                                                 u + k]])) {
                                                        input_match = 0;
                                                        break;
                                                    }
                                                }

                                                // suffix
                                                for (int k = 0; k < subtable->suffix_count; ++k) {
                                                    if (!hz_map_contains(subtable->suffix_maps[k],
                                                                         b1->glyph_indices[rangeList->unignored_indices[
                                                                                 u + subtable->input_count + k]])) {
                                                        suffix_match = 0;
                                                        break;
                                                    }
                                                }

                                                if (input_match && suffix_match && prefix_match) {
                                                    int context_low = rangeList->unignored_indices[u];
                                                    int context_high = rangeList->unignored_indices[u + subtable->input_count - 1];

                                                    // create context from input glyphs
                                                    hz_buffer_t *ctx1 = hz_buffer_copy_range(b1, context_low, context_high);
                                                    hz_buffer_t *ctx2 = hz_buffer_create();
                                                    ctx2->attrib_flags = b2->attrib_flags;

                                                    for (uint16_t z = 0; z < subtable->lookup_count; ++z) {
                                                        hz_buffer_compute_info(ctx1, face);
                                                        hz_vector(int) context_index_list = hz_buffer_get_unignored_indices(ctx1, table->lookup_flags, table->mark_filtering_set);
                                                        int sequence_idx = context_index_list[subtable->lookup_records[z].sequence_index];
                                                        hz_vector_destroy(context_index_list);

                                                        // recurse :^)
                                                        hz_shape_plan_apply_gpos_lookup(plan, feature,
                                                                                        subtable->lookup_records[z].lookup_list_index,
                                                                                        ctx1, ctx2,
                                                                                        sequence_idx, sequence_idx,
                                                                                        depth + 1);

                                                        hz_swap_buffers(ctx1, ctx2, face);
                                                    }

                                                    // add final result to b2
                                                    hz_buffer_add_other(b2, ctx1);

                                                    match = HZ_TRUE;

                                                    int skip_loc = u + subtable->input_count - 1;
                                                    g = rangeList->unignored_indices[skip_loc];
                                                    r = hz_range_list_search(rangeList, g);
                                                    range = &rangeList->ranges[r];

                                                    hz_buffer_destroy(ctx1);
                                                    hz_buffer_destroy(ctx2);
                                                    goto check_match_gpos_6_3;
                                                }
                                            }
                                        }

                                        check_match_gpos_6_3:
                                        if (!match) {
                                            hz_buffer_add_glyph(b2, (hz_glyph_object_t) {
                                                    .metrics = metrics});
                                        }
                                    }

                                }
                            }

                            break;
                        }
                        default:  assert(0);
                    }
                    break;
                }

                default :
                    hz_buffer_add_other(b2,b1);
                    break;
            }


            // move glyphs from source buffer to destination buffer
            hz_swap_buffers(b1, b2, face);
            hz_range_list_destroy(rangeList);
        }
    }

    // write slices caused by range into the output buffer
    hz_buffer_add_range(out, in, 0, v1 - 1);
    hz_buffer_add_other(out, b1);
    hz_buffer_add_range(out, in, v2 + 1, in->glyph_count - 1);

    // cleanup buffers
    hz_buffer_destroy(b1);
    hz_buffer_destroy(b2);
}
#endif

HZ_STATIC int
hz_feature_list_search(hz_feature_list_item_t *features, uint16_t num_features, hz_feature_t feature)
{
    const hz_feature_info_t *feature_info = hz_ot_get_feature_info(feature);

    if (feature_info != NULL) {
        for (int i = 0; i < num_features; ++i) {
            if (unlikely(features[i].tag == feature_info->tag)) { return i; }
        }
    }

    return -1;
}


HZ_STATIC void
hz_segment_setup_metrics(hz_segment_t *seg, hz_face_t *face)
{
    hz_buffer_t *in = seg->in;

    if (in != NULL) {
        size_t size = in->glyph_count;
        hz_vector_resize(in->glyph_metrics, size);

        for (size_t i = 0; i < size; ++i) {
            // Marks should not have advance, but this is a hack
            {
                hz_index_t glyph_index = seg->in->glyph_indices[i];
                in->glyph_metrics[i].xAdvance = face->metrics[glyph_index].xAdvance;
                in->glyph_metrics[i].yAdvance = face->metrics[glyph_index].yAdvance;
            }

            in->glyph_metrics[i].xOffset = 0;
            in->glyph_metrics[i].yOffset = 0;
        }

        in->attrib_flags |= HZ_GLYPH_ATTRIB_METRICS_BIT;
    }
}

typedef struct hz_lookup_reference_t {
    uint16_t index;
    hz_feature_t feature;
    hz_tag_t type;
} hz_lookup_reference_t;

int cmp_lookup_ref(const void *a, const void *b)
{
    return (int)((const hz_lookup_reference_t *)a)->index - (int)((const hz_lookup_reference_t *)b)->index;
}

HZ_STATIC void
hz_shape_plan_apply_gsub_features(hz_shape_plan_t *plan, hz_segment_t *seg)
{
    hz_face_t *face = hz_font_get_face(plan->font);
    hz_gsub_table_t *gsub = &plan->gsub_table;
    seg->out->attrib_flags = HZ_GLYPH_ATTRIB_CODEPOINT_BIT | HZ_GLYPH_ATTRIB_INDEX_BIT | HZ_GLYPH_ATTRIB_COMPONENT_INDEX_BIT;

    hz_vector(hz_lookup_reference_t) lookup_refs = NULL;

    for (uint32_t i = 0; i < plan->num_features; ++i) {
        hz_feature_t feature = plan->features[i];
        int feature_index = hz_feature_list_search(gsub->features, gsub->num_features, feature);
        if (feature_index != -1) {
            // Found feature, apply corresponding lookups
            hz_feature_table_t *feature_table = &gsub->features[feature_index].table;

            for (uint16_t j = 0; j < feature_table->lookup_index_count; ++j) {
                hz_lookup_reference_t lookup_ref = (hz_lookup_reference_t){feature_table->lookup_list_indices[j],feature};
                hz_vector_push_back(lookup_refs,lookup_ref);
            }
        }
    }

    qsort(lookup_refs,
          hz_vector_size(lookup_refs),
          sizeof(hz_lookup_reference_t),
          &cmp_lookup_ref);

    for (size_t i = 0; i < hz_vector_size(lookup_refs); ++i) {
        hz_lookup_reference_t *ref = &lookup_refs[i];
        hz_shape_plan_apply_gsub_lookup(plan, ref->feature, ref->index, seg->in, seg->out, 0, seg->in->glyph_count - 1, 0);
        hz_segment_swap_buffers(seg, face);
    }

    hz_vector_destroy(lookup_refs);
}

HZ_STATIC void
hz_shape_plan_apply_gpos_features(hz_shape_plan_t *plan, hz_segment_t *seg)
{
    hz_face_t *face = hz_font_get_face(plan->font);
    hz_gpos_table_t *gpos = &plan->gpos_table;
    seg->out->attrib_flags = HZ_GLYPH_ATTRIB_METRICS_BIT;

    hz_vector(hz_lookup_reference_t) lookup_refs = NULL;

    for (uint32_t i = 0; i < plan->num_features; ++i) {
        hz_feature_t feature = plan->features[i];
        int feature_index = hz_feature_list_search(gpos->features, gpos->num_features, feature);
        if (feature_index != -1) {
            // Found feature, apply corresponding lookups
            hz_feature_table_t *feature_table = &gpos->features[feature_index].table;

            for (uint16_t j = 0; j < feature_table->lookup_index_count; ++j) {
                hz_lookup_reference_t lookup_ref = (hz_lookup_reference_t){feature_table->lookup_list_indices[j],feature};
                hz_vector_push_back(lookup_refs,lookup_ref);
            }
        }
    }

    qsort(lookup_refs,
          hz_vector_size(lookup_refs),
          sizeof(hz_lookup_reference_t),
          &cmp_lookup_ref);

    for (size_t i = 0; i < hz_vector_size(lookup_refs); ++i) {
        hz_lookup_reference_t *ref = &lookup_refs[i];
        // GPOS FIXME
        // hz_shape_plan_apply_gpos_lookup(plan, ref->feature, ref->index, seg->in, seg->out, 0, seg->in->glyph_count - 1, 0);
        hz_segment_swap_buffers(seg, face);
    }

    hz_vector_destroy(lookup_refs);
}

HZ_STATIC void
hz_shape_plan_destroy(hz_shape_plan_t *plan)
{
    hz_memory_pool_release(&plan->memory_pool);
    hz_free(plan);
}

HZ_STATIC void
hz_shape_plan_execute(hz_shape_plan_t *plan, hz_segment_t *seg)
{
    hz_face_t *face = hz_font_get_face(plan->font);

    if (seg->num_codepoints > 0) {
        // if codepoints buffer exist, then setup shaping objects and apply features
        hz_segment_setup_shaping_objects(seg, face);

        hz_shape_plan_apply_gsub_features(plan, seg);
        hz_segment_setup_metrics(seg,face);
        // hz_shape_plan_apply_gpos_features(plan, seg);
        // hz_buffer_compute_info(seg->in, face);

        if (seg->direction == HZ_DIRECTION_RTL || seg->direction == HZ_DIRECTION_BTT) {
            hz_buffer_flip_direction(seg->in);
        }
    }
}

void
hz_shape(hz_font_t *font,
         hz_segment_t *seg,
         hz_feature_t *features,
         unsigned int num_features,
         hz_shape_flags_t flags)
{
    hz_shape_plan_t *plan = hz_shape_plan_create(font, seg, features, num_features, flags);
    hz_shape_plan_execute(plan, seg);
    hz_shape_plan_destroy(plan);
}


HZ_STATIC const HzLanguageMap *
hz_get_language_map(hz_language_t lang) {
    size_t i;
    for (i = 0; i < HZ_ARRLEN(language_map_list); ++i) {
        if (language_map_list[i].language == lang) {
            return &language_map_list[i];
        }
    }
    return NULL;
}

hz_language_t hz_lang(const char *tag)
{
    const HzLanguageMap *currlang, *foundlang;
    size_t i, n;
    size_t len;
    const char *p;
    char code[3]; /* expects only 3 or 2 char codes */
    foundlang = NULL;
    len = strlen(tag);

    /* use ISO 639-2 and ISO 639-3 codes, same as in https://docs.microsoft.com/en-us/typography/opentype/spec/languagetags */
    for (i = 0; i < HZ_ARRLEN(language_map_list); ++i) {
        currlang = &language_map_list[i];
        p = currlang->codes;

        if (p == NULL) continue;

        while (1) {
            n = 0;
            while (*p != ':' && *p != '\0')
                code[n++] = *p++;

            if (unlikely(!memcmp(code, tag, 3))) {
                foundlang = currlang;
                goto done_searching;
            }

            if (*p == '\0') break;
            ++p;
        }
    }

    done_searching:
    if (unlikely(foundlang == NULL))
        return HZ_LANGUAGE_DFLT;

    return foundlang->language;
}

#define START 0

HZ_STATIC void
utf8_decoder_init(utf8_decoder_t *state)
{
    state->data = NULL;
    state->ptr = 0;
    state->size = 0;
    state->code = 0;
    state->step = START;
}

HZ_STATIC void
utf8_decoder_reset(utf8_decoder_t *state)
{
    state->ptr = 0;
    state->size = 0;
    state->code = 0;
    state->step = START;
}

HZ_STATIC void
utf8_decoder_set_data(utf8_decoder_t *state, const char8_t *data, size_t size)
{
    state->data = data;
    state->size = size;
}

typedef enum utf_decoder_flags_t {
    UTF_BOUNDS_CHECK = 0x00000001,
    UTF_ALIGN_CHUNK_BLOCKS = 0x00000002,
    UTF_END_AT_NULL = 0x00000004
} utf_decoder_flags_t;

typedef struct utf8_range_t {
    char8_t *start;
    size_t size;
} utf8_range_t;

/*

    Maximum characters loaded into chunk (in case of 7-bit ASCII) is
    UTF_CHUNKSIZE
    Otherwise, the least number of characters that would be loaded into the chunk is
    UTF_CHUNKSIZE / 4
    If all characters are 4-byte characters.

*/
typedef struct utf8_chunk_decoder_t {
    const char8_t *data;
    uintptr_t ptr;
    size_t size;

    /* loaded blocks into the chunk */
    size_t readchars;

    /* see utf_decoder_flags_t */
    uint32_t flags;

    /* stored in groups of 4-characters which are 16-byte aligned */
    size_t chunksize;
    uint8_t *chunk;

    char32_t code;
    int8_t step;
} utf8_chunk_decoder_t;

void
utf8_chunk_decoder_init(utf8_chunk_decoder_t *state, size_t chunksize)
{
    state->data = NULL;
    state->ptr = 0;
    state->size = 0;
    state->readchars = 0;
    state->flags = 0;
    state->chunksize = chunksize;
    state->chunk = hz_malloc(chunksize);
    state->step = START;
    memset(state->chunk, 0, chunksize);
}

void
utf8_chunk_decoder_set_flags(utf8_chunk_decoder_t *state, uint32_t flags)
{
    state->flags = flags;
}

void
utf8_chunk_decoder_deinit(utf8_chunk_decoder_t *state)
{
    hz_free(state->chunk);
}

HZ_STATIC void
utf8_chunk_decoder_load_data(utf8_chunk_decoder_t *state, const hz_byte_t *data, size_t size)
{
    state->data = data;
    state->size = size;

    if (state->flags & UTF_END_AT_NULL)
        state->size = strlen((char *) state->data);
}

HZ_STATIC void
decode_utf8_byte(utf8_chunk_decoder_t *state)
{
    hz_byte_t byte;

    retry:
    byte = state->data[state->ptr++];
    if (state->step == START) {
        // decode the number of steps using the first byte
        state->step = decode_byte_table[byte];
        if (state->step < 0) goto error; // invalid starting byte
        // extract the data from the first byte, using the proper mask
        state->code = ((uint32_t)byte & decode_mask_table[state->step]) << (6*state->step);
    } else {
        if ((byte & 0xc0) != 0x80) { // invalid continuation byte
            // retry as starting byte
            state->step = START;
            goto retry;
        }

        if (state->step == 0) goto error; // too many continuation bytes
        state->step -= 1;
        // extract the data from the continuation byte
        state->code |= ((uint32_t)byte & 0x3f) << (6*state->step);
    }
    return;

    error:
    // return the unicode "unknown" character and start again
    state->step = START;
    state->code = 0xfffd;
}

HZ_STATIC char32_t
decode_next_utf8_codepoint(utf8_chunk_decoder_t *state)
{
    do decode_utf8_byte(state); while (state->step != START);
    return state->code;
}

/* Decodes UTF-8 data in blocks of 4 characters using lookup tables */
HZ_STATIC int
decode_utf8_to_utf32_unaligned_sse4(utf8_chunk_decoder_t *state)
{
    uintptr_t chunkptr = 0;
    state->readchars = 0;

#if 0
    // decode with 4-wide 32-bit registers at a time
    #pragma clang loop vectorize(enable) unroll(enable)
    #pragma GCC ivdep
    while (likely(state->ptr+16 <= state->size)) {
        __m128i block, *outptr;

        block = _mm_loadu_si128((__m128i *)(state->data + state->ptr));
        outptr = (__m128i *)(state->chunk + chunkptr);

        // Detect if the block has any non-ASCII characters by checking the highest bit.
        // Here, 'likely' simply tells the compiler that it's likely (expected) that 'block' is ASCII-only.
        if (likely(!_mm_movemask_epi8(block) && chunkptr + 64 <= state->chunksize)) {
            // ASCII-only block... unpack to 4 x 4 UTF-32 characters and write to chunk
            __m128i le, ri, z;
            __m128i v0, v1, v2, v3;

            // load low & high 8 bytes interleaved with zeros
            z = _mm_setzero_si128();

            // first level
            le = _mm_unpacklo_epi8(block, z);
            ri = _mm_unpackhi_epi8(block, z);

            // second level
            v0 = _mm_unpacklo_epi16(le, z);
            v1 = _mm_unpackhi_epi16(le, z);
            v2 = _mm_unpacklo_epi16(ri, z);
            v3 = _mm_unpackhi_epi16(ri, z);

            _mm_storeu_si128(outptr + 0, v0);
            _mm_storeu_si128(outptr + 1, v1);
            _mm_storeu_si128(outptr + 2, v2);
            _mm_storeu_si128(outptr + 3, v3);

            state->ptr += 16;
            state->readchars += 16;
            chunkptr += 64;
        } else {
            //            break;
            unsigned int charidx;
            int8_t length;
            uint8_t bytecount, tablekey;
            __m128i pattern, data_mask, mask1;
            __m128i block_left, block_low, block_high, counts;

            bytecount = 0;

            // store data about next 4 multi-byte characters
            #pragma GCC unroll 4
            for (charidx = 0; charidx < 4; ++charidx) {
                char8_t byte;
                byte = state->data[state->ptr + bytecount];
                length = prefix_to_length_table[byte >> 3];
                bytecount += length + 1;
                tablekey = (tablekey << 2) | length;
            }

            pattern = _mm_loadu_si128((__m128i *)(length_to_pattern1_table + tablekey));
            data_mask = _mm_loadu_si128((__m128i *)(length_to_mask1_table + tablekey));
            counts = _mm_loadu_si128((__m128i *)(length_to_counts_table + tablekey));

            block = _mm_and_si128(_mm_shuffle_epi8(block, pattern), data_mask);

            // mask for if first byte is zero
            //            mask1 = _mm_cmpeq_epi8(_mm_and_si128(counts, _mm_set1_epi32(0xff)), _mm_setzero_si128());

            __m128i b1 = _mm_and_si128(block, _mm_set1_epi32(0xff));
            __m128i b2 = _mm_and_si128(_mm_srli_si128(block, 1), _mm_set1_epi32(0xff));
            __m128i b3 = _mm_and_si128(_mm_srli_si128(block, 2), _mm_set1_epi32(0xff));
            __m128i b4 = _mm_and_si128(_mm_srli_si128(block, 3), _mm_set1_epi32(0xff));

            block = _mm_or_si128(_mm_or_si128(_mm_slli_epi32(b1,0), _mm_slli_epi32(b2,6)),
                                 _mm_or_si128(_mm_slli_epi32(b3,12), _mm_slli_epi32(b4,18)));

            _mm_storeu_si128(outptr, block);

            state->ptr += bytecount;
            state->readchars += 4;
            chunkptr += 16;
        }
    }
#endif

    // Check for odd remaining bytes to decode, which is very likely.
    // Finish rest of the chunk off through scalar method.
    while (likely(chunkptr + 4 <= state->chunksize)) {
        if (state->ptr >= state->size)
            return UTF_END_OF_BUFFER;

        state->step = START;
        *(char32_t *)(state->chunk + chunkptr) = decode_next_utf8_codepoint(state);
        ++state->readchars;
        chunkptr += 4;
    }

    return UTF_OK;
}

HZ_STATIC int
decode_utf8_to_utf32_unaligned_avx2(utf8_chunk_decoder_t *state)
{

}

HZ_STATIC int
decode_utf8_to_utf32_unaligned_avx512(utf8_chunk_decoder_t *state)
{

}

HZ_STATIC void
decode_utf8_to_ucs2_aligned_sse4()
{

}

HZ_STATIC void
hz_segment_load_utf8_simd(hz_segment_t *seg, const hz_byte_t *text)
{
    utf8_chunk_decoder_t state;
    int ret;

    utf8_chunk_decoder_init(&state, UTF_SMALL_CHUNK_SIZE);
    utf8_chunk_decoder_set_flags(&state, UTF_END_AT_NULL);
    utf8_chunk_decoder_load_data(&state, text, 0);

    do {
        size_t copysize, newsize;
        ret = decode_utf8_to_utf32_unaligned_sse4(&state);
        copysize = state.readchars << 2;
        newsize = (seg->num_codepoints << 2) + copysize;
        seg->codepoints = hz_realloc(seg->codepoints, newsize);
        memcpy(seg->codepoints + seg->num_codepoints, state.chunk, copysize);
        seg->num_codepoints += state.readchars;
    } while (ret != UTF_END_OF_BUFFER);

    utf8_chunk_decoder_deinit(&state);
}

void
hz_segment_load_utf32(hz_segment_t *seg, hz_char32_t *data)
{
}

void
hz_segment_load_utf8(hz_segment_t *seg, hz_byte_t *data)
{
    hz_segment_clear(seg);
    hz_segment_load_utf8_simd(seg, data);
}

void
hz_segment_load_latin1(hz_segment_t *seg, hz_byte_t *data)
{
    hz_segment_clear(seg);

    size_t len = strlen((char *)data);
    seg->num_codepoints = len;
    seg->codepoints = hz_malloc(len * sizeof(hz_unicode_t));

    for (size_t i = 0; i < len; ++i) {
        seg->codepoints[i] = (hz_unicode_t) data[i];
    }
}
