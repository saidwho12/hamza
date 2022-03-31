/* About: License
 * This file is licensed under MIT.
 */

#include "hz.h"
#include <assert.h>

#define HZ_BOOLSTR(x) ((x)?"true":"false")
#define HZ_BIT(x) (1<<(x))

#define KIB 1024
#define MIB 1048576
#define GIB 1073741824

#define UTF_FAILED (-1)
#define UTF_END_OF_BUFFER (-2)
#define UTF_OK 1

// Configuration macros
#define HZ_RELY_ON_UNSAFE_CMAP_CONSTANTS HZ_TRUE
#define HZ_CONFIG_USE_SIMD HZ_FALSE

// Max depth of nested OpenType lookups
#define HZ_OT_MAX_RECURSE_DEPTH 16

#if defined(__GNUC__)
#define min(x,y) ( \
    { __auto_type __x = (x); __auto_type __y = (y); \
      __x < __y ? __x : __y; })
#define max(x,y) ( \
    { __auto_type __x = (x); __auto_type __y = (y); \
      __x > __y ? __x : __y; })
#else
#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif

#if HZ_COMPILER & (HZ_COMPILER_CLANG | HZ_COMPILER_GCC)
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

#if HZ_ARCH & HZ_ARCH_AVX2_BIT
#   include <immintrin.h>
#include <strsafe.h>

#elif HZ_ARCH & HZ_ARCH_AVX_BIT
#   include <immintrin.h>
#elif HZ_ARCH & HZ_ARCH_SSE42_BIT
#	if HZ_COMPILER & HZ_COMPILER_CLANG
#		include <popcntintrin.h>
#	endif
#	include <nmmintrin.h>
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

hz_allocator_t internal_allocator = { (HZMALLOCFN)malloc, (HZFREEFN)free };

void
hz_set_custom_allocator(hz_allocator_t a)
{
    internal_allocator = a;
}

HZ_STATIC void *Alloc(size_t size)
{
    return malloc(size);
}

HZ_STATIC void *Realloc(void *ptr, size_t size)
{
    return realloc(ptr,size);
}

HZ_STATIC void Free(void *ptr)
{
    free(ptr);
}

static void
log_impl(const char *filename,
         int line,
         const char *fmt,
         ...)
{

}

#define DEBUG_MSG(msg) fprintf(stdout,"[DEBUG:%s:%d]: %s\n",__FILE__,__LINE__,msg)
#define ERROR_MSG(msg) fprintf(stderr,"[ERROR: %s:%d]: %s\n",__FILE__,__LINE__,msg)
#define QUIT_MSG(msg) { ERROR_MSG(msg); exit(-1); }
#define ASSERT_MSG(cond, msg) assert((cond) && message)

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

typedef struct LinearAllocator {
    uint8_t *data;
    size_t size;
    uintptr_t offset;
    int alignment;
} LinearAllocator;

static LinearAllocator CreateLinearAllocator(void *mem, size_t size)
{
    LinearAllocator la;
    la.data = mem;
    la.size = size;
    la.offset = 0;
    la.alignment = 8;
    return la;
}

static uint64_t Minll(uint64_t x, uint64_t y)
{
    return x<y?x:y;
}

static int CheckPowerOfTwo(uint64_t v)
{
    return v && ~(v & (v-1));
}

static int Ctzll(uint64_t x)
{
    return __builtin_ctzll(x);
}

static uint64_t Ftzll(uint64_t x)
{
    return (1 << Ctzll(x)) - 1;
}

static uint64_t Align(uint64_t x, uint64_t n)
{
    // Aligns on largest power of two divisor of allocation size
    assert(CheckPowerOfTwo(n)); // n must be a power of two
    uint64_t m = (x && n) ? Ftzll(n) & 0x1f : 0;
    return x + ((~x + 1) & m);
}

// The following is a linear allocation function optimized for allocation of integers and integer arrays
// since this is what it's mostly used for realistically in the code.
static void* LinearAlloc(LinearAllocator *la, size_t size)
{
    if (size > 0 && size < la->size) {
        uint64_t p = la->offset;

        p = Align(p,size);

        if (p + size <= la->size) {
            la->offset = p + size;
            return la->data + p;
        }
    }

    return NULL;
}

static void *LinearAlignedAlloc(LinearAllocator *la, size_t n, size_t alignment)
{

}

static void ResetLinearAllocator(LinearAllocator *la)
{
    la->offset = 0;
}

/* no bound for buffer */
#define BNDCHECK 0x00000001

/*
    Struct: hz_stream_t

        A struct to store a buffer.
        This is also used to handle reading bytes sequentially.
        Unpacks in network-order (big endian).
*/
typedef struct hz_stream_t {
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
static int checkle(void)
{
    static const int x = 1;
    return (int)(*(unsigned char *)&x);
}

// Does unpacking OpenType data require byte swapping
static int needsbswap(void)
{
    return checkle();
}

HZ_STATIC void UnpackArray16(hz_stream_t *stream, size_t count, uint16_t *dest)
{
    if (count > 0) {
        if (needsbswap()) {
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

typedef struct hz_array_t {
    size_t size;
    uint32_t *data;
} hz_array_t;

hz_array_t * hz_array_create(void)
{
    hz_array_t *array = Alloc(sizeof(*array));
    array->data = NULL;
    array->size = 0;
    return array;
}

void hz_array_append(hz_array_t *array, uint32_t val)
{
    size_t new_size = array->size + 1;

    if (array->data == NULL)
        array->data = Alloc(new_size * sizeof(uint32_t));
    else
        array->data = Realloc(array->data, new_size * sizeof(uint32_t));

    array->data[new_size - 1] = val;
    array->size = new_size;
}

size_t hz_array_size(const hz_array_t *array)
{
    return array->size;
}

hz_bool hz_array_is_empty(const hz_array_t *array)
{
    return array->size == 0;
}

void hz_array_clear(hz_array_t *array)
{
    if (!hz_array_is_empty(array)) {
        Free(array->data);
        array->data = NULL;
        array->size = 0;
    }
}

void
hz_array_resize(hz_array_t *array, size_t new_size)
{
    hz_bool is_empty = hz_array_is_empty(array);
    if (new_size == 0 && !is_empty) {
        Free(array->data);
        array->data = NULL;
    } else {
        if (is_empty)
            array->data = Alloc(new_size * sizeof(uint32_t));
        else
            array->data = Realloc(array->data, new_size * sizeof(uint32_t));
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
    uint32_t *remaining = Alloc(remaining_len * sizeof(uint32_t));
    memcpy(remaining, &array->data[index + count], remaining_len * sizeof(uint32_t));

    array->data = Realloc(array->data, new_size * sizeof(uint32_t));
    array->size = new_size;

    memcpy(&array->data[index], remaining, remaining_len * sizeof(uint32_t));
    Free(remaining);
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
        uint32_t *sec = Alloc(sec_len * sizeof(uint32_t));
        memcpy(sec, &array->data[index], sec_len);

        array->data = Realloc(array->data, new_size * sizeof(uint32_t));
        array->size = new_size;
        memcpy(&array->data[index + 1], sec, sec_len);
        array->data[index] = val;

        Free(sec);
    }
}

void
hz_array_destroy(hz_array_t *array)
{
    Free(array->data);
    Free(array);
}

uint32_t
hz_array_at(const hz_array_t *array, size_t index)
{
    return array->data[index];
}

hz_bool
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

hz_bool
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

typedef struct hz_map_bucket_t {
    struct hz_map_bucket_node_t *root;
} hz_map_bucket_t;

static void
hz_map_bucket_init(hz_map_bucket_t *b)
{
    b->root = NULL;
}

typedef struct hz_map_t {
    hz_map_bucket_t *buckets;
    size_t bucket_count;
} hz_map_t;

HZ_STATIC hz_map_t *
hz_map_create(void)
{
    size_t i;
    hz_map_t *map;

    map = Alloc(sizeof(hz_map_t));
    map->bucket_count = 64;
    map->buckets = Alloc(sizeof(hz_map_bucket_t) * map->bucket_count);

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
                Free(tmp);
            }

            bucket->root = NULL;
        }
    }

    Free(map->buckets);
    Free(map);
}

/* Returns true if value exists, and false if it didn't */
HZ_STATIC hz_bool
hz_map_set_value(hz_map_t *map, uint32_t key, uint32_t value)
{
    uint32_t hash;
    hz_map_bucket_t *bucket;

    hash = hash_fnv1a(key);
    bucket = &map->buckets[hash % map->bucket_count];

    if (bucket->root == NULL) {
        hz_map_bucket_node_t *new_node = Alloc(sizeof(hz_map_bucket_node_t));
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

        new_node = Alloc(sizeof(hz_map_bucket_node_t));
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

HZ_STATIC hz_bool
hz_map_value_exists(hz_map_t *map, uint32_t key)
{
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


HZ_STATIC hz_bool
hz_map_set_value_for_keys(hz_map_t *map, uint32_t k0, uint32_t k1, uint32_t value)
{
    hz_bool any_set = HZ_FALSE;
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
//    printf("%d\n", r-l);
    return r-l;
}

HZ_STATIC hz_mph_map_t *hz_mph_map_create(hz_map_t *from)
{
    uint32_t size = hz_map_size(from);

    hz_mph_map_t *map = Alloc(sizeof(*map));
    map->N = size;
    map->buckets = Alloc(sizeof(*map->buckets) * size);
    map->G = Alloc(sizeof(*map->G) * size);
    map->values = Alloc(sizeof(*map->values) * size);
    hz_bool *value_set = Alloc(sizeof(*value_set) * size);

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
//    printf("%d\n", x);

    // Sort the buckets and process in decreasing order (largest -> smallest)


    size_t buckets_size = sizeof(*map->buckets) * size;
    hz_array_t **sorted_buckets = Alloc(buckets_size);
    memcpy(sorted_buckets, map->buckets, buckets_size);

    qsort((void *)sorted_buckets, size, sizeof(void *),
          compare_arrays);

//    printf("%lu\n", sorted_buckets[0]->size);

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
    Free(value_set);
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
    hz_blob_t *blob = Alloc(sizeof(hz_blob_t));
    blob->data = NULL;
    blob->size = 0;
    return blob;
}

hz_bool
hz_blob_is_empty(hz_blob_t *blob)
{
    return blob->data == NULL || blob->size == 0;
}

void
hz_blob_destroy(hz_blob_t *blob)
{
    if (!hz_blob_is_empty(blob)) Free(blob->data);
    Free(blob);
}

void
hz_blob_resize(hz_blob_t *blob, size_t new_size)
{
    if (hz_blob_is_empty(blob)) {
        blob->data = Alloc(new_size);
    } else {
        blob->data = Realloc(blob->data, new_size);
    }

    blob->size = new_size;
}

size_t
hz_blob_get_size(hz_blob_t *blob)
{
    return blob->size;
}

typedef struct hz_glyph_info_t {
    hz_unicode_t codepoint;
    uint16_t component_index;
    uint16_t glyph_class, attachment_class;
} hz_glyph_info_t;

typedef struct hz_glyph_position_t {
    int32_t xOffset;
    int32_t yOffset;
    int32_t xAdvance;
    int32_t yAdvance;
} hz_glyph_position_t;

// dynamic array header
typedef struct hz_vector_hdr_t {
    size_t size, capacity;
    size_t member_size;
} hz_vector_hdr_t;

void hz_vector_init(void **v, size_t member_size)
{
    if (*v == NULL) {
        hz_vector_hdr_t *hdr = Alloc(sizeof(*hdr));
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

hz_bool hz_vector_is_empty(void *v)
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
            hdr = Realloc(hdr, sizeof(hz_vector_hdr_t));
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
        Free(hdr);
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
    hdr = Realloc(hdr, sizeof(hz_vector_hdr_t) + sz);
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

hz_bool hz_vector_need_grow(void *v, size_t extra)
{
    assert(v != NULL);
    hz_vector_hdr_t *hdr = hz_vector_header(v);
    return hdr->size + extra > hdr->capacity;
}

void hz_vector_deep_copy(void **to, void *from)
{
    HZ_ASSERT(to != NULL && from != NULL);

    if (*to != from) {
        hz_vector_hdr_t *to_hdr, *from_hdr;
        from_hdr = hz_vector_header(from);

        if (!hz_vector_is_empty(from)) {
            // Clear data already in 'to' array
            hz_vector_clear_impl(to);
            to_hdr = hz_vector_header(*to);

            // From has data to be copied
            to_hdr->capacity = from_hdr->capacity;
            to_hdr->size = from_hdr->size;

            if (from_hdr->size > 0) {
                hz_vector_reserve_impl(to, from_hdr->size);

                // reset the to header as reserve reallocates the structure in another location
                to_hdr = hz_vector_header(*to);
                memcpy(*to, from, from_hdr->member_size * from_hdr->size);
            }
        }
    }
}

#define hz_vector(__T) __T *
#define hz_vector_size(__ARR) hz_vector_size_impl((void*)(__ARR))
#define hz_vector_resize(__ARR, __SIZE) do { hz_vector_init((void**)&(__ARR), sizeof(*(__ARR))); hz_vector_resize_impl((void**)&(__ARR), __SIZE); } while(0)
#define hz_vector_destroy(__ARR) hz_vector_destroy_impl((void**)&(__ARR))
#define hz_vector_reserve(__ARR, __CAPACITY) do { hz_vector_init((void**)&(__ARR), sizeof(*(__ARR))); hz_vector_reserve_impl((void**)&(__ARR), __CAPACITY); } while(0)
#define hz_vector_clear(__ARR) hz_vector_clear_impl((void**)&(__ARR))
#define hz_vector_push(__ARR, __ARRVAL) do {\
    hz_vector_init((void **)&(__ARR), sizeof(*(__ARR)));\
    if (!(__ARR) || ((__ARR) && hz_vector_need_grow(__ARR, 1))) {\
        hz_vector_grow((void **)&(__ARR),1);\
    }\
    (__ARR)[hz_vector_size(__ARR)] = __ARRVAL;\
    hz_vector_header(__ARR)->size++;\
} while(0)

#define hz_vector_push_many(__ARR, __PTR, __LEN) do {\
hz_vector_init((void **)&(__ARR), sizeof(*(__ARR)));\
if (!(__ARR) || ((__ARR) && hz_vector_need_grow(__ARR, __LEN))) {\
hz_vector_grow((void **)&(__ARR), __LEN);\
}\
memcpy((__ARR) + hz_vector_header(__ARR)->size, __PTR, (__LEN) * sizeof((__ARR)[0]));\
hz_vector_header(__ARR)->size += (__LEN);\
} while(0)


hz_buffer_t *hz_buffer_create(void)
{
    hz_buffer_t *buffer = Alloc(sizeof(*buffer));
    buffer->glyph_count = 0;
    buffer->glyph_indices = NULL;
    buffer->codepoints = NULL;
    buffer->has_info = HZ_FALSE;
    buffer->glyph_classes = NULL;
    buffer->attachment_classes = NULL;
    return buffer;
}

void hz_buffer_clear_info(hz_buffer_t *buffer)
{
    if (buffer->has_info) {
        Free(buffer->glyph_classes);
        Free(buffer->attachment_classes);
        buffer->glyph_classes = NULL;
        buffer->attachment_classes = NULL;
        buffer->has_info = HZ_FALSE;
    }
}

void hz_buffer_clear(hz_buffer_t *buffer)
{
    hz_vector_clear(buffer->glyph_indices);
    hz_vector_clear(buffer->codepoints);

    hz_buffer_clear_info(buffer);
    buffer->glyph_count = 0;
}

typedef struct hz_buffer_glyph_t {
    hz_index_t GID;
    hz_unicode_t codepoint;
} hz_buffer_glyph_t;

void hz_buffer_reserve(hz_buffer_t *self, size_t capacity)
{
    hz_vector_reserve(self->glyph_indices, capacity);
    hz_vector_reserve(self->codepoints, capacity);
}

void hz_buffer_add_glyph(hz_buffer_t *self, hz_buffer_glyph_t glyph)
{
    hz_vector_push(self->glyph_indices, glyph.GID);
    hz_vector_push(self->codepoints, glyph.codepoint);
}

void hz_buffer_add_range(hz_buffer_t *self,
                         const hz_buffer_t *other,
                         int x1, int x2)
{
    if (x2 >= x1) {
        int len = (x2-x1)+1;
        hz_vector_push_many(self->glyph_indices, other->glyph_indices+x1, len);
        hz_vector_push_many(self->codepoints, other->codepoints+x1, len);
    }
}

void hz_buffer_add_other(hz_buffer_t *self, hz_buffer_t *other)
{
    size_t len = hz_vector_size(other->glyph_indices);
    hz_vector_push_many(self->glyph_indices, other->glyph_indices, len);
    hz_vector_push_many(self->codepoints, other->codepoints, len);
}

void hz_buffer_destroy(hz_buffer_t *buffer)
{
    hz_buffer_clear(buffer);
    Free(buffer);
}

typedef struct hz_glyph_metrics_t {
    int32_t x_advance;
    int32_t y_advance;
    int32_t x_offset;
    int32_t y_offset;
} hz_glyph_metrics_t;

struct hz_segment_t {
    hz_language_t language;
    hz_script_t script;
    hz_direction_t direction;
    size_t num_codepoints;
    hz_unicode_t *codepoints;
    hz_glyph_metrics_t *glyph_metrics;

    // buffers used in shaping process
    hz_buffer_t *in, *out;
};

hz_segment_t *
hz_segment_create(void)
{
    hz_segment_t *seg = Alloc(sizeof(hz_segment_t));

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

    if (seg->num_codepoints != 0)
    {
        Free(seg->codepoints);
        seg->codepoints = NULL;
        seg->num_codepoints = 0;
    }
}

void
hz_segment_destroy(hz_segment_t *seg)
{
    hz_segment_clear(seg);
    Free(seg);
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
HZ_STATIC hz_bool
hz_is_arabic_codepoint(hz_unicode_t c)
{
    return (c >= 0x0600u && c <= 0x06FFu) || /* Arabic (0600–06FF) */
            (c >= 0x0750u && c <= 0x077Fu) || /* Arabic Supplement (0750–077F) */
            (c >= 0x08A0u && c <= 0x08FFu) || /* Arabic Extended-A (08A0–08FF) */
            (c >= 0xFB50u && c <= 0xFDFFu) || /* Arabic Presentation Forms-B (FE70–FEFF) */
            (c >= 0xFE70u && c <= 0xFEFFu) || /* Arabic Presentation Forms-B (FE70–FEFF) */
            (c >= 0x1EE00u && c <= 0x1EEFFu); /* Arabic Mathematical Alphabetic Symbols (1EE00–1EEFF) */
}

HZ_STATIC hz_bool
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

//            printf("%04x\n", *joining);
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

    hz_map_t *class_map;
    hz_map_t *attach_class_map;
};

hz_face_t *
hz_face_create()
{
    hz_face_t *face = Alloc(sizeof(hz_face_t));
    face->num_glyphs = 0;
    face->num_of_h_metrics = 0;
    face->num_of_v_metrics = 0;
    face->metrics = NULL;
    face->ascender = 0;
    face->descender = 0;
    face->linegap = 0;
    face->upem = 0;
    face->class_map = hz_map_create();
    face->attach_class_map = hz_map_create();
    return face;
}

void
hz_face_destroy(hz_face_t *face)
{

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


//void
//hz_face_set_table(hz_face_t *face, hz_tag_t tag, hz_blob_t *blob)
//{
//    hz_face_table_node_t *new_node = Alloc(sizeof(hz_face_table_node_t));
//    new_node->tag = tag;
//    new_node->blob = blob;
//    new_node->next = NULL;
//
//    if (face->tables.root == NULL) {
//        new_node->prev = NULL;
//        face->tables.root = new_node;
//    } else {
//        hz_face_table_node_t *node = face->tables.root;
//        while (node != NULL) {
//            if (node->next == NULL) {
//                /* found last node */
//                new_node->prev = node;
//                node->next = new_node;
//                break;
//            }
//
//            node = node->next;
//        }
//    }
//}
//
//hz_blob_t *
//hz_face_reference_table(hz_face_t *face, hz_tag_t tag)
//{
//    hz_face_table_node_t *node = face->tables.root;
//
//    while (node != NULL) {
//        if (node->tag == tag)
//            return node->blob; /* tags match, found table */
//
//        node = node->next;
//    }
//
//    return NULL;
//}

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

uint16_t
hz_face_get_num_of_h_metrics(hz_face_t *face)
{
    return face->num_of_h_metrics;
}

uint16_t
hz_face_get_num_of_v_metrics(hz_face_t *face)
{
    return face->num_of_v_metrics;
}

hz_metrics_t *
hz_face_get_glyph_metrics(hz_face_t *face, hz_index_t id)
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
    if (face->gdef) {
        hz_stream_t bs = hz_stream_create(face->data + face->gdef, 0);
        Version16Dot16 version;

        Offset16 glyph_class_def_offset;
        Offset16 attach_list_offset;
        Offset16 lig_caret_list_offset;
        Offset16 mark_attach_class_def_offset;
        Offset16 mark_glyph_sets_def_offset;

        version = Unpack32(&bs);

        switch (version) {
            case 0x00010000: /* 1.0 */
                Unpackv(&bs, "hhhh",
                        &glyph_class_def_offset,
                        &attach_list_offset,
                        &lig_caret_list_offset,
                        &mark_attach_class_def_offset);
                break;
            case 0x00010002: /* 1.2 */
                Unpackv(&bs, "hhhhh",
                        &glyph_class_def_offset,
                        &attach_list_offset,
                        &lig_caret_list_offset,
                        &mark_attach_class_def_offset,
                        &mark_glyph_sets_def_offset);
                break;
            case 0x00010003: /* 1.3 */
                break;
            default: /* error */
                break;
        }

        if (glyph_class_def_offset) {
            /* glyph class def isn't nil */
            hz_stream_t subtable = hz_stream_create(bs.data + glyph_class_def_offset, 0);
            uint16_t class_format;
            class_format = Unpack16(&subtable);
            switch (class_format) {
                case 1:
                    break;
                case 2: {
                    uint16_t range_index = 0, class_range_count;
                    class_range_count = Unpack16(&subtable);

                    while (range_index < class_range_count) {
                        uint16_t start_glyph_id, end_glyph_id, glyph_class;
                        Unpackv(&subtable, "hhh",
                                &start_glyph_id,
                                &end_glyph_id,
                                &glyph_class);

                        HZ_ASSERT(glyph_class >= 1 && glyph_class <= 4);
                        hz_map_set_value_for_keys(face->class_map, start_glyph_id, end_glyph_id, 1<<(glyph_class-1));
                        ++range_index;
                    }
                    break;
                }
                default:
                    break;
            }
        }

        if (mark_attach_class_def_offset) {
            hz_stream_t subtable = hz_stream_create(bs.data + mark_attach_class_def_offset, 0);
            uint16_t class_format;
            class_format = Unpack16(&subtable);
            switch (class_format) {
                case 1:
                    break;
                case 2: {
                    uint16_t range_index = 0, class_range_count;
                    class_range_count = Unpack16(&subtable);

                    while (range_index < class_range_count) {
                        uint16_t start_glyph_id, end_glyph_id, glyph_class;
                        Unpackv(&subtable, "hhh",
                                &start_glyph_id,
                                &end_glyph_id,
                                &glyph_class);

                        HZ_ASSERT(glyph_class >= 1 && glyph_class <= 4);
                        hz_map_set_value_for_keys(face->attach_class_map, start_glyph_id, end_glyph_id, 1<<(glyph_class-1));
                        ++range_index;
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
}

typedef struct hz_kern_coverage_field_t {
    unsigned horizontal : 1;
    unsigned minimum : 1;
    unsigned cross_stream : 1;
    unsigned override : 1;
    unsigned reserved1 : 4;
    unsigned format : 8;
} hz_kern_coverage_field_t;

typedef union hz_kern_coverage_t {
    uint16_t data;
    hz_kern_coverage_field_t field;
} hz_kern_coverage_t;

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
        hz_kern_coverage_field_t coverage;
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
    if (hz_map_value_exists(face->class_map, id)) {
        return hz_map_get_value(face->class_map, id);
    }

    return HZ_GLYPH_CLASS_BASE;
}

uint8_t
hz_face_get_glyph_attach_class(hz_face_t *face, hz_index_t id) {
    if (hz_map_value_exists(face->attach_class_map, id)) {
        return hz_map_get_value(face->attach_class_map, id);
    }

    return 0;
}

void
hz_face_load_upem(hz_face_t *face)
{
    /* No-Op */
}

float
hz_face_line_skip(hz_face_t *face)
{
    return (float)(face->ascender - face->descender + face->linegap) / 64.0f;
}

//HZ_STATIC hz_blob_t*
//hz_ft_load_snft_table(FT_Face face, hz_tag_t tag)
//{
//    FT_ULong size, ft_tag, length;
//
//    size = 0;
//    ft_tag = tag;
//
//    FT_Load_Sfnt_Table(face, tag, 0, NULL, &size);
//
//    if (size) {
//        hz_blob_t *blob = hz_blob_create();
//        hz_blob_resize(blob, size);
//        FT_Load_Sfnt_Table(face, tag, 0, hz_blob_get_data(blob), &size);
//        return blob;
//    }
//
//    return NULL;
//}

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
    hz_font_t *font = Alloc(sizeof(hz_font_t));
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
    Free(font);
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

HZ_API hz_font_t *
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

    face->metrics = Alloc(sizeof(hz_metrics_t) * face->num_glyphs);

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(info, &ascent, &descent, &lineGap);

    for (g = 0; g < face->num_glyphs; ++g) {
        /* how wide is this character */
        int ax;
        int lsb;
        stbtt_GetGlyphHMetrics(info, g, &ax, &lsb);

        int c_x1, c_y1, c_x2, c_y2;
        stbtt_GetGlyphBox(info, g, &c_x1, &c_y1, &c_x2, &c_y2);

        face->metrics[g].w = c_x2 - c_x1;
        face->metrics[g].h = c_y2 - c_y1;
        face->metrics[g].x_advance = ax;
        face->metrics[g].y_advance = 0;
        face->metrics[g].x_bearing = lsb;
    }

    hz_face_load_class_maps(face);
    hz_face_load_kerning_pairs(face);
    hz_font_set_face(font, face);

    return font;
}

//hz_font_t *
//hz_ft_font_create(FT_Face ft_face)
//{
//    FT_Error         err;
//    size_t            i;
//    const hz_tag_t   *tags;
//    size_t            n_tags;
//    hz_face_t        *face;
//    hz_font_t        *font;
//
//    FT_Bytes BASE_table;
//    FT_Bytes GDEF_table;
//    FT_Bytes GSUB_table;
//    FT_Bytes GPOS_table;
//    FT_Bytes JSTF_table;
//
//    if (ft_face == NULL) {
//        DEBUG_MSG("FreeType face passed in is NULL!");
//        return NULL;
//    }
//
//    tags = required_ft_table_tags;
//    n_tags = HZ_ARRLEN(required_ft_table_tags);
//
//    font = hz_font_create();
//    face = hz_face_create();
//    hz_face_set_upem(face, ft_face->units_per_EM);
//
//    if ((err = FT_OpenType_Validate(ft_face, FT_VALIDATE_OT,
//                             &BASE_table, &GDEF_table, &GPOS_table, &GSUB_table, &JSTF_table))
//                             != FT_Err_Ok)
//    {
//        fprintf(stderr, "Failed to validate OpenType Face! Error: %s (code %d).\n",
//                FT_Error_String(err), err);
//
//        hz_face_destroy(face);
//        hz_font_destroy(font);
//        return NULL;
//    }
//
//    face->ot_tables.BASE_table = (hz_byte_t *)BASE_table;
//    face->ot_tables.GDEF_table = (hz_byte_t *)GDEF_table;
//    face->ot_tables.GSUB_table = (hz_byte_t *)GSUB_table;
//    face->ot_tables.GPOS_table = (hz_byte_t *)GPOS_table;
//    face->ot_tables.JSTF_table = (hz_byte_t *)JSTF_table;
//
//    for (i = 0; i < n_tags; ++i) {
//        hz_blob_t *blob = hz_ft_load_snft_table(ft_face, tags[i]);
//        if (blob != NULL) {
//            hz_face_set_table(face, tags[i], blob);
//        }
//    }
//
//    hz_face_load_num_glyphs(face);
//    face->metrics = Alloc(sizeof(hz_metrics_t) * face->num_glyphs);
//
//    for (i = 0; i < face->num_glyphs; ++i) {
//        FT_GlyphSlot slot = ft_face->glyph;
//        if (FT_Load_Glyph(ft_face, i, FT_LOAD_NO_BITMAP | FT_LOAD_NO_SCALE)  == FT_Err_Ok) {
//            hz_metrics_t *mo;
//            FT_Glyph_Metrics *mi;
//            FT_BBox bbox;
//            FT_Glyph glyph;
//            mi = &ft_face->glyph->metrics;
//
//            /* Load glyph bounding box */
//            FT_Get_Glyph(slot, &glyph);
//            FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_SUBPIXELS, &bbox);
//            FT_Done_Glyph(glyph);
//
//            mo = hz_face_get_glyph_metrics(face, i);
//
//            mo->x_advance = (hz_position_t)mi->horiAdvance;
//            mo->y_advance = (hz_position_t)mi->vertAdvance;
//            mo->x_bearing = (hz_position_t)mi->horiBearingX;
//            mo->y_bearing = 0;//mi->horiBearingY;
//            mo->w = (hz_position_t)mi->width;
//            mo->h = (hz_position_t)mi->height;
//            mo->bbox.x0 = (hz_position_t)bbox.xMin;
//            mo->bbox.x1 = (hz_position_t)bbox.xMax;
//            mo->bbox.y0 = (hz_position_t)bbox.yMin;
//            mo->bbox.y1 = (hz_position_t)bbox.yMax;
//        }
//    }
//
//    hz_face_load_class_maps(face);
//    hz_face_load_kerning_pairs(face);
//    hz_font_set_face(font, face);
//    return font;
//}

uint16_t
hz_ignored_classes_from_lookup_flags(hz_lookup_flag_t flags)
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

/*
    Struct: hz_sequence_node_cache_t
        Sequence of pointers to nodes which may have a gap between
        useful for when checking sub-sequences with glyph class skip flags when applying lookups
        as a form of cache. i.e. it's used in contextual substitutions when comparing a subsection of a seg
        and caching those nodes, so that later can easily move to specific subsections of the seg.
*/
typedef struct hz_sequence_node_cache_t {
    hz_glyph_info_t **nodes;
    size_t node_count;
} hz_sequence_node_cache_t;

hz_sequence_node_cache_t *hz_sequence_node_cache_create(void) {
    hz_sequence_node_cache_t *cache = Alloc(sizeof(hz_sequence_node_cache_t));
    cache->node_count = 0;
    cache->nodes = NULL;
    return cache;
}

hz_bool
hz_sequence_node_cache_is_empty(hz_sequence_node_cache_t *cache) {
    return cache->node_count == 0 || cache->nodes == NULL;
}

void
hz_sequence_node_cache_clear(hz_sequence_node_cache_t *cache) {
    if (!hz_sequence_node_cache_is_empty(cache)) {
        Free(cache->nodes);
        cache->nodes = NULL;
        cache->node_count = 0;
    }
}

void
hz_sequence_node_cache_grow(hz_sequence_node_cache_t *cache, size_t amt) {
    if (hz_sequence_node_cache_is_empty(cache)) {
        cache->node_count += amt;
        cache->nodes = Alloc(sizeof(hz_sequence_node_cache_t *) * cache->node_count);
    } else {
        cache->node_count += amt;
        cache->nodes = Realloc(cache->nodes, sizeof(hz_sequence_node_cache_t *) * cache->node_count);
    }
}

void hz_sequence_node_cache_shrink(hz_sequence_node_cache_t *cache, size_t amt) {
    if (amt >= cache->node_count) {
        hz_sequence_node_cache_clear(cache);
    }  else {
        cache->node_count -= amt;
        cache->nodes = Realloc(cache->nodes, sizeof(void *) * cache->node_count);
    }
}

void
hz_sequence_node_cache_add(hz_sequence_node_cache_t *cache, hz_glyph_info_t *node) {
    hz_sequence_node_cache_grow(cache, 1);
    cache->nodes[cache->node_count-1] = node;
}

void
hz_sequence_node_cache_insert(hz_sequence_node_cache_t *cache, size_t index,
                              hz_glyph_info_t *node)
{
    size_t i, rem = cache->node_count - index;
    hz_sequence_node_cache_grow(cache, 1);

    for (i=cache->node_count-1; i > index+1; --i) {
        cache->nodes[i] = cache->nodes[i-1];
    }

    cache->nodes[index] = node;
}

void
hz_sequence_node_cache_remove(hz_sequence_node_cache_t *cache, size_t index)
{
    size_t i;
    for (i = index; i < cache->node_count - 1; ++i) {
        cache->nodes[i] = cache->nodes[i+1];
    }

    hz_sequence_node_cache_shrink(cache, 1);
}

void
hz_sequence_node_cache_destroy(hz_sequence_node_cache_t *cache) {
    hz_sequence_node_cache_clear(cache);
    Free(cache);
}

typedef struct hz_range_rec_t {
    hz_index_t start_glyph_id;
    hz_index_t end_glyph_id;
    uint16_t start_coverage_index;
} hz_range_rec_t;

typedef struct hz_feature_table_t {
    // = NULL (reserved for offset to FeatureParams)
    Offset16 feature_params;
    // Number of LookupList indices for this feature
    uint16_t lookup_index_count;
    // Array of indices into the LookupList — zero-based (first lookup is LookupListIndex = 0)
    uint16_t *lookup_list_indices;
} hz_feature_table_t;

HZ_STATIC hz_error_t
hz_ot_layout_apply_gsub_subtable(hz_face_t *face,
                                 hz_stream_t *subtable,
                                 uint16_t lookup_type,
                                 uint16_t lookup_flags,
                                 hz_feature_t feature,
                                 hz_segment_t *seg);


HZ_STATIC void
hz_ot_layout_apply_gpos_feature(hz_face_t *face,
                                hz_stream_t *table,
                                hz_feature_t feature,
                                hz_segment_t *seg);

HZ_STATIC hz_error_t
hz_ot_layout_apply_gpos_subtable(hz_face_t *face,
                                 hz_stream_t *subtable,
                                 uint16_t lookup_type,
                                 uint16_t lookup_flags,
                                 hz_feature_t feature,
                                 hz_segment_t *seg);

HZ_STATIC void
hz_ot_layout_apply_gsub_feature(hz_face_t *face,
                                hz_stream_t *table,
                                hz_feature_t feature,
                                hz_segment_t *seg);

static const uint8_t *
hz_ot_layout_choose_lang_sys(hz_face_t *face,
                             uint8_t *data,
                             hz_tag_t script,
                             hz_tag_t language)
{
    uint8_t buffer[4096];
    LinearAllocator la = CreateLinearAllocator(buffer, sizeof(buffer));

    hz_stream_t subtable = hz_stream_create(data, 0);
    uint16_t script_count = 0;
    uint16_t index = 0;
    Record16 *script_records = NULL;
    uint16_t found_script = 0;
    const uint8_t *found_addr;

    script_count = Unpack16(&subtable);
    script_records = LinearAlloc(&la, sizeof(Record16) * script_count);

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

static void
hz_ot_layout_parse_lang_sys() {

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

void
hz_ot_layout_apply_gsub_lookup(hz_face_t *face,
                               hz_feature_t feature,
                               hz_segment_t *seg,
                               hz_stream_t *lookup)
{
    uint16_t type, flags, n, i;
    Unpackv(lookup, "hhh", &type, &flags, &n);

    for (i=0; i<n; ++i) {
        Offset16 offset = Unpack16(lookup);
        hz_stream_t subtable = hz_stream_create(lookup->data + offset, 0);
//        hz_ot_layout_apply_gsub_subtable(face, &subtable, type, flags,
//                                         feature, seg);
    }
}

HZ_STATIC hz_error_t
hz_ot_layout_apply_lookup(hz_face_t *face,
                          hz_feature_t feature,
                          hz_tag_t table_tag,
                          hz_segment_t *seg,
                          uint16_t lookup_index)
{
    hz_stream_t table;
    Version16Dot16 version;
    uint16_t script_list_offset;
    uint16_t feature_list_offset;
    uint16_t lookup_list_offset;
    uint32_t feature_variations_offset;

    if (table_tag == HZ_OT_TAG_GSUB) {
        table = hz_stream_create(face->data + face->gsub, 0);
    } else if (table_tag == HZ_OT_TAG_GPOS) {
        table = hz_stream_create(face->data + face->gpos, 0);
    } else {
        /* error */
        return HZ_ERROR_INVALID_TABLE_TAG;
    }

    version = Unpack32(&table);

    switch (version) {
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

        default:
            /* error */
            return HZ_ERROR_INVALID_TABLE_VERSION;
    }

    uint8_t *lookup_list = table.data + lookup_list_offset;
    uint16_t lookup_count = bswap16(*(uint16_t*)&lookup_list[0]);

    if (lookup_index >= lookup_count)
        return HZ_ERROR_INVALID_PARAM;

    uint16_t lookup_offset = bswap16(((uint16_t*)lookup_list)[1+lookup_index]);
    hz_stream_t lookup_table = hz_stream_create(lookup_list + lookup_offset, 0);


    if (table_tag == HZ_OT_TAG_GSUB) {
        hz_ot_layout_apply_gsub_lookup(face, feature, seg, &lookup_table);
    } else if (table_tag == HZ_OT_TAG_GPOS) {

    }

    return HZ_OK;
}

typedef struct hz_feature_list_item_t {
    hz_tag_t tag;
    hz_feature_table_t table;
} hz_feature_list_item_t;

HZ_STATIC hz_bool
hz_ot_layout_apply_features(hz_face_t *face,
                            hz_tag_t table_tag,
                            hz_tag_t script,
                            hz_tag_t language,
                            hz_feature_t *features,
                            unsigned int num_features,
                            hz_segment_t *seg)
{
    hz_stream_t table;

    if (table_tag == HZ_OT_TAG_GSUB) {
        table = hz_stream_create(face->data + face->gsub, 0);
    } else if (table_tag == HZ_OT_TAG_GPOS) {
        table = hz_stream_create(face->data + face->gpos, 0);
    } else {
        /* error */
        return HZ_ERROR_INVALID_TABLE_TAG;
    }

    Version16Dot16 version;
    uint16_t script_list_offset;
    uint16_t feature_list_offset;
    uint16_t lookup_list_offset;
    uint32_t feature_variations_offset;

    version = Unpack32(&table);

    switch (version) {
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

    void *lsaddr = hz_ot_layout_choose_lang_sys(face,
                                                table.data + script_list_offset,
                                                script, language);

    if (lsaddr == NULL) {
        /* Language system was not found */
        return HZ_FALSE;
    }

    hz_array_t *lang_feature_indices = hz_array_create();
    hz_stream_t lsbuf = hz_stream_create(lsaddr, 0);

    hz_lang_sys_t lang_sys;
    Unpackv(&lsbuf, "hhh", &lang_sys.lookup_order,
            &lang_sys.required_feature_index,
            &lang_sys.feature_index_count);

    uint16_t loopIndex = 0;
    while (loopIndex < lang_sys.feature_index_count) {
        uint16_t featureIndex;
        featureIndex = Unpack16(&lsbuf);
        hz_array_append(lang_feature_indices, featureIndex);
        ++loopIndex;
    }

    hz_stream_t lookup_list = hz_stream_create(table.data + lookup_list_offset, 0);
    hz_array_t *lookup_offsets = hz_array_create();

    {
        /* Read lookup offets to table */
        uint16_t lookup_index = 0;
        uint16_t lookup_count = Unpack16(&lookup_list);
        while (lookup_index < lookup_count) {
            uint16_t lookup_offset = Unpack16(&lookup_list);
            hz_array_append(lookup_offsets, lookup_offset);
            ++lookup_index;
        }
    }


    hz_stream_t feature_list = hz_stream_create(table.data + feature_list_offset, 0);


    {
        /* Parsing the FeatureList and applying selected Features */
        uint16_t feature_index = 0;
        uint16_t feature_count = Unpack16(&feature_list);

        hz_map_t *feature_map = hz_map_create();

        /* fill map from feature type to offset */
        while (feature_index < feature_count) {
            hz_tag_t tag;
            uint16_t offset;
            Unpackv(&feature_list, "ih", &tag, &offset);
            hz_feature_t feature = hz_ot_feature_from_tag(tag);
            hz_map_set_value(feature_map, feature, offset);
            ++feature_index;
        }

        uint16_t i, j;
        for (i=0; i < num_features; ++i) {
            hz_feature_t feature = features[i];

            if ( hz_map_value_exists(feature_map, feature) ) {
                /* feature is wanted and exists */
                Offset16 feature_offset = hz_map_get_value(feature_map, feature);
                hz_array_t *lookup_indices = hz_array_create();
                hz_ot_layout_feature_get_lookups(feature_list.data + feature_offset, lookup_indices);

                for (j=0; j<hz_array_size(lookup_indices); ++j) {
                    uint16_t lookup_offset = hz_array_at(lookup_offsets, hz_array_at(lookup_indices, j));
                    hz_stream_t lookup_table = hz_stream_create(lookup_list.data + lookup_offset, 0);

                    if (table_tag == HZ_OT_TAG_GSUB) {
                        hz_ot_layout_apply_gsub_feature(face, &lookup_table, feature, seg);
                    } else if (table_tag == HZ_OT_TAG_GPOS) {
                        hz_ot_layout_apply_gpos_feature(face, &lookup_table, feature, seg);
                    }
                }

                hz_array_destroy(lookup_indices);
            }
        }


        hz_map_destroy(feature_map);
    }

    hz_array_destroy(lookup_offsets);
    return HZ_OK;
}

hz_bool
hz_ot_read_coverage(const unsigned char *data, hz_map_t *map, hz_array_t *id_arr)
{
    uint16_t coverage_format;
    hz_stream_t table = hz_stream_create(data, 0);

    coverage_format = Unpack16(&table);

    switch (coverage_format) {
        case 1: {
            uint16_t coverage_glyph_count = Unpack16(&table);
            for (uint16_t coverage_idx = 0; coverage_idx < coverage_glyph_count; ++coverage_idx) {
                uint16_t glyph_index = Unpack16(&table);
                if (id_arr != NULL)
                    hz_map_set_value(map, glyph_index, hz_array_at(id_arr, coverage_idx));
                else
                    hz_map_set_value(map, glyph_index, coverage_idx);
            }

            break;
        }

        case 2: {
            uint16_t range_index = 0, range_count = Unpack16(&table);

            /* Assuming ranges are ordered from 0 to glyph_count in order */
            while (range_index < range_count) {
                hz_range_rec_t range;

                Unpackv(&table, "hhh",
                        &range.start_glyph_id,
                        &range.end_glyph_id,
                        &range.start_coverage_index);

                for (hz_index_t glyph_id = range.start_glyph_id; glyph_id <= range.end_glyph_id; ++glyph_id) {
                    uint16_t coverage_index = range.start_coverage_index + glyph_id - range.start_glyph_id;
                    hz_map_set_value(map, glyph_id, coverage_index);
                }

                ++range_index;
            }
            break;
        }

        default:
            /* error */
            break;
    }

    return HZ_TRUE;
}

typedef struct hz_value_record_t {
    int16_t x_placement;
    int16_t y_placement;
    int16_t x_advance;
    int16_t y_advance;
    Offset16 x_pla_device_offset;
    Offset16 y_pla_device_offset;
    Offset16 x_adv_device_offset;
    Offset16 y_adv_device_offset;
} hz_value_record_t;


static void
hz_ot_read_value_record(hz_stream_t *stream, hz_value_record_t *record, uint16_t value_format) {

    if (value_format & HZ_VALUE_FORMAT_X_PLACEMENT)
        record->x_placement = (int16_t) Unpack16(stream);

    if (value_format & HZ_VALUE_FORMAT_Y_PLACEMENT)
        record->y_placement = (int16_t) Unpack16(stream);

    if (value_format & HZ_VALUE_FORMAT_X_ADVANCE)
        record->x_advance = (int16_t) Unpack16(stream);

    if (value_format & HZ_VALUE_FORMAT_Y_ADVANCE)
        record->y_advance = (int16_t) Unpack16(stream);

    if (value_format & HZ_VALUE_FORMAT_X_PLACEMENT_DEVICE)
        record->x_pla_device_offset = Unpack16(stream);

    if (value_format & HZ_VALUE_FORMAT_Y_PLACEMENT_DEVICE)
        record->y_pla_device_offset = Unpack16(stream);

    if (value_format & HZ_VALUE_FORMAT_X_ADVANCE_DEVICE)
        record->x_adv_device_offset = Unpack16(stream);

    if (value_format & HZ_VALUE_FORMAT_Y_ADVANCE_DEVICE)
        record->y_adv_device_offset = Unpack16(stream);
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

void
hz_ot_read_single_pos_format1_table(hz_stream_t *stream, hz_ot_single_pos_format1_table_t *table) {
    Offset16 coverage_offset = Unpack16(stream);
    table->coverage = hz_map_create();
    table->value_format = Unpack16(stream);

    hz_ot_read_coverage(stream->data + coverage_offset,
                        table->coverage,
                        NULL);

    hz_ot_read_value_record(stream, &table->value_record, table->value_format);
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

    hz_ot_read_coverage(stream->data + coverage_offset,
                        table->coverage,
                        NULL);

    table->value_records = Alloc(sizeof(hz_value_record_t) * table->value_count);

    for (i = 0; i < table->value_count; ++i) {
        hz_ot_read_value_record(stream, &table->value_records[i], table->value_format);
    }
}

void
hz_ot_free_single_pos_format2_table(hz_ot_single_pos_format2_table_t *table) {
    hz_map_destroy(table->coverage);
    Free(table->value_records);
}

//hz_segment_node_t *
//hz_prev_node_not_of_class(hz_segment_node_t *g,
//                          hz_glyph_class_t gcignore,
//                          int *skip)
//{
//    if (gcignore == HZ_GLYPH_CLASS_ZERO) {
//        /* if no ignored classes, just give the next glyph directly as an optimization */
//        if (skip != NULL) (* skip) --;
//        return g->prev;
//    }
//
//    while (1) {
//        if (skip != NULL) (* skip) --;
//        g = g->prev;
//
//        /* break if prev node is NULL, cannot keep searching */
//        if (g == NULL)
//            break;
//
//        /* if not any of the class flags set, break, as we found what we want */
//        if (~g->glyph_class & gcignore)
//            break;
//    }
//
//    return g;
//}
//
//hz_segment_node_t *
//hz_next_node_not_of_class(hz_segment_node_t *g,
//                          hz_glyph_class_t gcignore,
//                          int64_t *skip)
//{
//    if (gcignore == HZ_GLYPH_CLASS_ZERO) {
//        /* if no ignored classes, just give the next glyph directly as an optimization */
//        if (skip != NULL) (* skip) ++;
//        return g->next;
//    }
//
//    while (1) {
//        if (skip != NULL) (* skip) ++;
//        g = g->next;
//
//        /* break if next node is NULL, cannot keep searching */
//        if (g == NULL)
//            break;
//
//        /* if not any of the class flags set, break, as we found what we want */
//        if (~g->glyph_class & gcignore)
//            break;
//    }
//
//    return g;
//}
//
//HZ_STATIC hz_bool_t
//hz_should_skip_node(hz_segment_node_t *g, uint16_t flags) {
//    uint8_t attach_type = (flags & HZ_LOOKUP_FLAG_MARK_ATTACHMENT_TYPE_MASK) >> 8;
//    uint16_t ignored_classes = hz_ignored_classes_from_lookup_flags(flags);
//
//    if (g == NULL) return HZ_FALSE;
//
//    if ((g->glyph_class & HZ_GLYPH_CLASS_MARK) && attach_type && (attach_type != g->attach_class))
//        return HZ_TRUE;
//
//    if (g->glyph_class & ignored_classes) {
//        return HZ_TRUE;
//    }
//
//    return HZ_FALSE;
//}
//
//hz_segment_node_t *
//hz_next_valid_node(hz_segment_node_t *g, uint16_t flags) {
//    do {
//        g = g->next;
//    } while (hz_should_skip_node(g, flags));
//
//    return g;
//}
//
//hz_segment_node_t *
//hz_prev_valid_node(hz_segment_node_t *g, uint16_t flags) {
//    do {
//        g = g->prev;
//    } while (hz_should_skip_node(g, flags));
//
//    return g;
//}
//
//hz_segment_node_t *
//hz_next_node_of_class(hz_segment_node_t *node,
//                      hz_glyph_class_t glyph_class,
//                      int *skip)
//{
//    if (glyph_class == HZ_GLYPH_CLASS_ZERO) {
//        /* if no wanted classes, just give the next glyph directly as an optimization */
//        if (skip != NULL) (* skip) ++;
//        return node->next;
//    }
//
//    while (1) {
//        if (skip != NULL) (* skip) ++;
//        node = node->next;
//
//        /* break if next node is NULL, cannot keep searching */
//        if (node == NULL)
//            break;
//
//        /* if not any of the class flags set, break, as we found what we want */
//        if (node->glyph_class & glyph_class)
//            break;
//    }
//
//    return node;
//}
//
//hz_segment_node_t *
//hz_next_node_of_class_bound(hz_segment_node_t *node,
//                            hz_glyph_class_t glyph_class,
//                            int *skip,
//                            int max_skip)
//{
//    if (glyph_class == HZ_GLYPH_CLASS_ZERO) {
//        /* if no wanted classes, just give the next glyph directly as an optimization */
//        if (skip != NULL) (* skip) ++;
//        return node->next;
//    }
//
//    while (*skip < max_skip) {
//        if (skip != NULL) (* skip) ++;
//        node = node->next;
//
//        /* break if next node is NULL, cannot keep searching */
//        if (node == NULL)
//            break;
//
//        /* if not any of the class flags set, break, as we found what we want */
//        if (node->glyph_class & glyph_class)
//            break;
//    }
//
//    return node;
//}
//
///* assumes node is not NULL */
//hz_segment_node_t *
//hz_last_node_of_class(hz_segment_node_t *node,
//                      hz_glyph_class_t glyph_class,
//                      int *skip)
//{
//    if (glyph_class == HZ_GLYPH_CLASS_ZERO)
//        return node;
//
//    while(1) {
//        if (node == NULL) break;
//        if (node->next == NULL) break;
//        if (~node->next->glyph_class & glyph_class) break;
//        node = node->next;
//        if (skip != NULL) (* skip) ++;
//    }
//
//    return node;
//}
//
//hz_segment_node_t *
//hz_prev_node_of_class(hz_segment_node_t *node,
//                      hz_glyph_class_t glyph_class,
//                      uint16_t *skip_count)
//{
//    if (glyph_class == HZ_GLYPH_CLASS_ZERO) {
//        /* if no wanted classes, just give the prev glyph directly as an optimization */
//        if (skip_count != NULL) (* skip_count) --;
//        return node->prev;
//    }
//
//    while (1) {
//        if (skip_count != NULL) (* skip_count) --;
//        node = node->prev;
//
//        /* break if next node is NULL, cannot keep searching */
//        if (node == NULL)
//            break;
//
//        /* if not any of the class flags set, break, as we found what we want */
//        if (node->glyph_class & glyph_class)
//            break;
//    }
//
//    return node;
//}
//
//
//#define HZ_MAX(x, y) (((x) > (y)) ? (x) : (y))
//
///* if possible, apply ligature fit for seg of glyphs
// * returns true if replacement occurred
// * */
//void
//hz_ot_layout_apply_fit_ligature(hz_face_t *face,
//                                hz_ligature_t **ligatures,
//                                uint16_t ligature_count,
//                                uint16_t lookup_type,
//                                hz_segment_t *seg,
//                                hz_segment_node_t *start_node)
//{
//    uint16_t i, j;
//
//    for (i=0; i<ligature_count; ++i) {
//        hz_segment_node_t * step_node = start_node;
//        hz_ligature_t *ligature = ligatures[i];
//        hz_bool_t did_match = HZ_TRUE;
//        hz_sequence_node_cache_t *node_cache = hz_sequence_node_cache_create();
//
//        /* go over seg and compare with current ligature */
//        for (j=0; j < ligature->component_count-1; ++j) {
//            step_node = hz_next_valid_node(step_node, lookup_type);
//
//            if (step_node == NULL) {
//                did_match = HZ_FALSE;
//                break;
//            }
//
//            hz_index_t g1 = step_node->gid;
//            hz_index_t g2 = ligature->component_glyph_ids[j];
//            if (g1 != g2) {
//                did_match = HZ_FALSE;
//                break;
//            }
//
//            hz_sequence_node_cache_add(node_cache, step_node);
//        }
//
//        if (did_match) {
//            /* pattern matches, replace it and tag following mark glyphs with
//             * a component id, link all following marks together after the ligature which were attached
//             * to the ligature.
//             * */
//            int comp_count = ligature->component_count;
//
//            if (comp_count-1) {
//                /* base and mark temporary nodes, go over to set marks component indices,
//                   then delete bases */
//                hz_segment_node_t * b, *m;
//                size_t k;
//
//                for (k=0; k<node_cache->node_count; ++k) {
//                    b = node_cache->nodes[k];
//
//                    for (m=b->next; m != NULL && (m->glyph_class & HZ_GLYPH_CLASS_MARK); m=m->next) {
//                        m->comp_idx = k+1;
//                    }
//
////                    hz_sequence_pop_node(seg, b);
//                }
//            }
//
//            /* modify start node to ligature glyph */
//            start_node->gid = ligature->ligature_glyph;
//            start_node->glyph_class = hz_face_get_glyph_class(face, start_node->gid);
//            start_node->attach_class = hz_face_get_glyph_attach_class(face, start_node->gid);
//            hz_sequence_node_cache_destroy(node_cache);
//            break;
//        }
//
//        hz_sequence_node_cache_destroy(node_cache);
//    }
//}

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
    if (rule->input_sequence != NULL) Free(rule->input_sequence);
    if (rule->seq_lookup_records != NULL) Free(rule->seq_lookup_records);
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
    rule->input_sequence = Alloc(rule->glyph_count - 1);
    Unpackv(&buf, "h:*", rule->input_sequence, rule->glyph_count - 1);

    rule->seq_lookup_records = Alloc(sizeof(hz_sequence_lookup_record_t) * rule->seq_lookup_count);

    for (i = 0; i < rule->seq_lookup_count; ++i)
        hz_read_sequence_lookup_record(&buf, &rule->seq_lookup_records[i]);
}

void
hz_parse_sequence_rule_set(uint8_t *data, hz_sequence_rule_set_t *rule_set)
{
    uint8_t buffer[4096];
    LinearAllocator la = CreateLinearAllocator(buffer, sizeof buffer);
    hz_stream_t bs;
    uint8_t *rule_offsets;

    rule_offsets = LinearAlloc(&la,
        rule_set->rule_count * sizeof(uint16_t));

    bs = hz_stream_create(data, 0);
    rule_set->rule_count = Unpack16(&bs);

    if (rule_set->rule_count) {
        uint16_t rule_index;
        Unpackv(&bs, "h:*", rule_offsets, rule_set->rule_count);

        rule_set->rules = Alloc(sizeof(hz_sequence_rule_t) * rule_set->rule_count);

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
    LinearAllocator la = CreateLinearAllocator(buffer, sizeof buffer);
    hz_stream_t buf;

    buf = hz_stream_create(data, 0);

    rule->prefix_count = Unpack16(&buf);
    rule->prefix_sequence = Alloc(sizeof(uint16_t) * rule->prefix_count);
    UnpackArray16(&buf, rule->prefix_count, rule->prefix_sequence);

    rule->input_count = Unpack16(&buf);
    rule->input_sequence = Alloc(sizeof(uint16_t) * (rule->input_count - 1));
    UnpackArray16(&buf, rule->input_count - 1, rule->input_sequence);

    rule->suffix_count = Unpack16(&buf);
    rule->suffix_sequence = Alloc(sizeof(uint16_t) * rule->suffix_count);
    UnpackArray16(&buf, rule->suffix_count, rule->suffix_sequence);

    rule->lookup_count = Unpack16(&buf);
    rule->lookup_records = Alloc(sizeof(hz_sequence_lookup_record_t) * rule->lookup_count);

    for (uint16_t i = 0; i < rule->lookup_count; ++i) {
        hz_read_sequence_lookup_record(&buf, rule->lookup_records + i);
    }
}

void
hz_parse_chained_sequence_rule_set(uint8_t *data, hz_chained_sequence_rule_set_t *rule_set) {
    hz_stream_t buf = hz_stream_create(data, 0);

    rule_set->count = Unpack16(&buf);
    Offset16* offsets = Alloc(sizeof(uint16_t) * rule_set->count);
    UnpackArray16(&buf,rule_set->count,offsets);

    rule_set->rules = Alloc(sizeof(hz_chained_sequence_rule_t) * rule_set->count);
//    printf("%d\n", rule_set->count);

    for (uint16_t i = 0; i < rule_set->count; ++i) {
        hz_parse_chained_sequence_rule(buf.data + offsets[i], rule_set->rules + i);
    }

    Free(offsets);
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
        prefix_offsets = Alloc(table->prefix_count * sizeof(Offset16));
        UnpackArray16(stream, table->prefix_count, prefix_offsets);

        table->prefix_maps = Alloc(sizeof(hz_map_t *) * table->prefix_count);
        for (size_t i=0; i<table->prefix_count; ++i) {
            table->prefix_maps[i] = hz_map_create();
            hz_ot_read_coverage(stream->data + prefix_offsets[i],
                                table->prefix_maps[i], NULL);
        }

    }

    {
        /* read input glyph offsets */
        table->input_count = Unpack16(stream);
        input_offsets = Alloc(table->input_count * sizeof(Offset16));
        UnpackArray16(stream,table->input_count, input_offsets);

        table->input_maps = Alloc(sizeof(hz_map_t *) * table->input_count);

        for (size_t i=0; i<table->input_count; ++i) {
            table->input_maps[i] = hz_map_create();
            hz_ot_read_coverage(stream->data + input_offsets[i],
                                table->input_maps[i],
                                NULL);
        }
    }

    {
        /* read lookahead glyph offsets */
        table->suffix_count = Unpack16(stream);
        suffix_offsets = Alloc(table->suffix_count * sizeof(Offset16));
        UnpackArray16(stream, table->suffix_count,suffix_offsets);

        table->suffix_maps = Alloc(sizeof(hz_map_t *) * table->suffix_count);

        for (size_t i=0; i<table->suffix_count; ++i) {
            table->suffix_maps[i] = hz_map_create();
            hz_ot_read_coverage(stream->data + suffix_offsets[i],
                                table->suffix_maps[i],
                                NULL);
        }

    }

    {
        /* read lookup records */
        table->lookup_count = Unpack16(stream);

//        printf("Lookup count: %d\n", table->lookup_count);
        table->lookup_records = Alloc(sizeof(hz_sequence_lookup_record_t) * table->lookup_count);

        for (size_t i=0;i<table->lookup_count;++i) {
            hz_sequence_lookup_record_t *record = table->lookup_records + i;
            hz_read_sequence_lookup_record(stream, record);
        }
    }


    Free(input_offsets);
    Free(prefix_offsets);
    Free(suffix_offsets);
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

    Free(table->prefix_maps);
    Free(table->input_maps);
    Free(table->suffix_maps);
    Free(table->lookup_records);
}

typedef struct hz_sequence_table_t {
    uint16_t glyph_count;
    uint16_t *glyphs;
} hz_sequence_table_t;

void
hz_ot_read_sequence_table(uint8_t *data, hz_sequence_table_t *table) {
    hz_stream_t buf = hz_stream_create(data, 0);
    table->glyph_count = Unpack16(&buf);
    table->glyphs = Alloc(sizeof(uint16_t) * table->glyph_count);
    UnpackArray16(&buf, table->glyph_count,table->glyphs);
}

void
hz_ot_clear_sequence_table(hz_sequence_table_t *table) {
    Free(table->glyphs);
}

typedef struct hz_multiple_subst_format1_t {
    hz_map_t *coverage;
    uint16_t sequence_count;
    hz_sequence_table_t *segs;
} hz_multiple_subst_format1_t;

//void
//hz_ot_read_multiple_subst_format1(hz_stream_t *buf,
//                                  hz_multiple_subst_format1_t *table)
//{
//    uint8_t             tmpbuf[4096];
//    hz_bump_allocator_t allocator;
//    Offset16       coverage_offset;
//    Offset16       *seg_offsets;
//    size_t              i;
//
//    hz_bump_allocator_init(&allocator, tmpbuf, sizeof tmpbuf);
//
//    /* read coverage table */
//    coverage_offset = unpack16(buf);
//    table->coverage = hz_map_create();
//    hz_ot_read_coverage(buf->data + coverage_offset, table->coverage, NULL);
//
//    /* read and decode sequences */
//    table->sequence_count = unpack16(buf);
//    sequence_offsets = hz_bump_allocator_alloc(&allocator, sizeof(uint16_t) * table->sequence_count);
//    table->sequences = Alloc(sizeof(hz_sequence_table_t) * table->sequence_count);
//
//    unpackf(buf, "h:*", sequence_offsets, table->sequence_count);
//
//    for (i = 0; i < table->sequence_count; ++i) {
//        hz_ot_read_sequence_table(buf->data + sequence_offsets[i], table->sequences + i);
//    }
//
//    /* release memory resources */
//    hz_bump_allocator_free(&allocator, sequence_offsets);
//    hz_bump_allocator_release(&allocator);
//}

void
hz_ot_clear_multiple_subst_format1(hz_multiple_subst_format1_t *table) {
    size_t i;
    hz_map_destroy(table->coverage);

    for (i = 0; i < table->sequence_count; ++i) {
        hz_ot_clear_sequence_table(table->segs + i);
    }

    Free(table->segs);
}

typedef struct hz_reverse_chain_single_subst_format1_t {
    hz_map_t *coverage;
    uint16_t prefix_count;
    hz_map_t **prefix_maps; /* prefix coverages */
    uint16_t suffix_count;
    hz_map_t **suffix_maps; /* suffix coverages */
    uint16_t glyph_count;
    uint16_t *glyphs;
} hz_reverse_chain_single_subst_format1_t;

static void
hz_read_reverse_chain_single_subst_format1(hz_stream_t *buf,
                                           hz_reverse_chain_single_subst_format1_t *subst)
{
    uint8_t buffer[4096];
    LinearAllocator la = CreateLinearAllocator(buffer, sizeof buffer);
    Offset16             coverage_offset;
    Offset16            *prefix_coverage_offsets;
    Offset16            *suffix_coverage_offsets;
    uint16_t            i;

    coverage_offset = Unpack16(buf);
    subst->coverage = hz_map_create();
    hz_ot_read_coverage(buf->data + coverage_offset, subst->coverage, NULL);

    subst->prefix_count = Unpack16(buf);
    prefix_coverage_offsets = LinearAlloc(&la, subst->prefix_count * sizeof(uint16_t));
    Unpackv(buf, "h:*", prefix_coverage_offsets, subst->prefix_count);
    subst->prefix_maps = Alloc(sizeof(hz_map_t *) * subst->prefix_count);
    for (i = 0; i < subst->prefix_count; ++i) {
        subst->prefix_maps[i] = hz_map_create();
        hz_ot_read_coverage(buf->data + prefix_coverage_offsets[i],
                            subst->prefix_maps[i],
                            NULL);
    }

    subst->suffix_count = Unpack16(buf);
    suffix_coverage_offsets = LinearAlloc(&la, subst->suffix_count * sizeof(uint16_t));
    Unpackv(buf, "h:*", suffix_coverage_offsets, subst->suffix_count);
    subst->suffix_maps = Alloc(sizeof(hz_map_t *) * subst->suffix_count);
    for (i = 0; i < subst->suffix_count; ++i) {
        subst->suffix_maps[i] = hz_map_create();
        hz_ot_read_coverage(buf->data + suffix_coverage_offsets[i],
                            subst->suffix_maps[i],
                            NULL);
    }

    subst->glyph_count = Unpack16(buf);
    subst->glyphs = Alloc(subst->glyph_count * sizeof(uint16_t));
    Unpackv(buf, "h:*", subst->glyphs, subst->glyph_count);
}

void
hz_free_reverse_chain_single_subst_format1(hz_reverse_chain_single_subst_format1_t *subst)
{
    size_t i;
    hz_map_destroy(subst->coverage);

    for(i=0; i<subst->prefix_count; ++i) {
        hz_map_destroy(subst->prefix_maps[i]);
    }

    for(i=0; i<subst->suffix_count; ++i) {
        hz_map_destroy(subst->suffix_maps[i]);
    }

    Free(subst->prefix_maps);
    Free(subst->suffix_maps);
    Free(subst->glyphs);
}
//
//hz_bool_t
//hz_ot_layout_pattern_match(hz_segment_node_t *node,
//                           hz_map_t **maps,
//                           uint16_t map_count,
//                           uint16_t lookup_flags,
//                           hz_sequence_node_cache_t *cache,
//                           hz_bool_t reverse)
//{
//    size_t i;
//    hz_bool_t does_match = HZ_TRUE;
//
//    for (i = 0; i < map_count; ++i) {
//
//        if (node == NULL) {
//            does_match = HZ_FALSE;
//            break;
//        }
//
//        if (!hz_map_value_exists(maps[i], node->gid)) {
//            does_match = HZ_FALSE;
//            break;
//        }
//
//        if (cache != NULL)
//            hz_sequence_node_cache_add(cache, node);
//
//
//        if (reverse)
//            node = hz_prev_valid_node(node, lookup_flags);
//        else
//            node = hz_next_valid_node(node, lookup_flags);
//    }
//
//    return does_match;
//}
//
//hz_bool_t
//hz_ot_layout_should_apply_subst(hz_map_t *coverage,
//                                hz_feature_t feature,
//                                const hz_segment_node_t *g)
//{
//    if (hz_map_value_exists(coverage, g->gid)) {
//        switch (feature) {
//            case HZ_FEATURE_ISOL:
//                return hz_ot_shape_complex_arabic_isol(g);
//            case HZ_FEATURE_MEDI:
//                return hz_ot_shape_complex_arabic_medi(g);
//            case HZ_FEATURE_INIT:
//                return hz_ot_shape_complex_arabic_init(g);
//            case HZ_FEATURE_FINA:
//                return hz_ot_shape_complex_arabic_fina(g);
//            default:
//                return HZ_TRUE;
//        }
//    }
//
//    return HZ_FALSE;
//}

//hz_error_t
//hz_ot_layout_apply_nested_gsub_lookup1(hz_face_t *face,
//                                       hz_stream_t *subtable,
//                                       uint16_t format,
//                                       hz_glyph_class_t class_skip,
//                                       hz_feature_t feature,
//                                       hz_segment_t *seg,
//                                       hz_sequence_node_cache_t *seg_context,
//                                       uint16_t nested_index)
//{
//    hz_segment_node_t * nested_node = sequence_context->nodes[nested_index];
//
//    switch (format) {
//        case 1: {
//            hz_map_t *coverage = hz_map_create();
//            uint16_t coverage_offset;
//            int16_t id_delta;
//
//            hz_segment_node_t * g;
//
//            unpackf(subtable, "hh", &coverage_offset, &id_delta);
//            hz_ot_read_coverage(subtable->data + coverage_offset, coverage, NULL);
//
//            /* If nested glyph meets requirements, apply delta */
//            if (hz_ot_layout_should_apply_subst(coverage, feature, nested_node)) {
//                nested_node->gid += id_delta;
//                nested_node->attach_class = hz_face_get_glyph_attach_class(face, nested_node->gid);
//            }
//
//            hz_map_destroy(coverage);
//            break;
//        }
//        case 2: {
//            uint16_t coverage_offset;
//            uint16_t index, glyph_count;
//
//            hz_map_t *coverage = hz_map_create();
//            hz_array_t *subst = hz_array_create();
//
//            hz_segment_node_t * g;
//
//            unpackf(subtable, "hh", &coverage_offset, &glyph_count);
//            hz_array_resize(subst, glyph_count);
//
//            for (index = 0; index < glyph_count; ++index) {
//                uint16_t val = unpack16(subtable);
//                hz_array_set(subst, index, val);
//            }
//
//            hz_ot_read_coverage(subtable->data + coverage_offset, coverage, subst);
//
//            /* If nested glyph meets requirements, replace glyph ID */
//            if (hz_ot_layout_should_apply_subst(coverage, feature, nested_node)) {
//                nested_node->gid = hz_map_get_value(coverage, nested_node->gid);
//                nested_node->attach_class = hz_face_get_glyph_attach_class(face, nested_node->gid);
//            }
//
//            hz_map_destroy(coverage);
//            hz_array_destroy(subst);
//            break;
//        }
//        default:
//            return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
//    }
//
//    return HZ_OK;
//}


//hz_error_t
//hz_ot_layout_apply_gsub_lookup1(hz_face_t *face,
//                                hz_stream_t *subtable,
//                                uint16_t format,
//                                uint16_t lookup_flags,
//                                hz_feature_t feature,
//                                hz_segment_t *seg)
//{
//    switch (format) {
//        case 1: {
//            hz_map_t *coverage = hz_map_create();
//            uint16_t coverage_offset;
//            int16_t id_delta;
//
//            hz_segment_node_t * g;
//
//            unpackf(subtable, "hh", &coverage_offset, &id_delta);
//            hz_ot_read_coverage(subtable->data + coverage_offset, coverage, NULL);
//
//            /* Loop over every glyph in the seg, check condition and substitute. */
//            for (g = seg->root; g != NULL; g = g->next)
//                if (!hz_should_skip_node(g, lookup_flags)) {
//                    if (hz_ot_layout_should_apply_subst(coverage, feature, g)) {
//                        g->gid += id_delta;
//                        g->attach_class = hz_face_get_glyph_attach_class(face, g->gid);
//                    }
//                }
//
//            hz_map_destroy(coverage);
//            break;
//        }
//        case 2: {
//            uint16_t coverage_offset;
//            uint16_t index, glyph_count;
//
//            hz_map_t *coverage = hz_map_create();
//            hz_array_t *subst = hz_array_create();
//
//            hz_segment_node_t * g;
//
//            unpackf(subtable, "hh", &coverage_offset, &glyph_count);
//            hz_array_resize(subst, glyph_count);
//
//            for (index = 0; index < glyph_count; ++index) {
//                uint16_t val = unpack16(subtable);
//                hz_array_set(subst, index, val);
//            }
//
//            hz_ot_read_coverage(subtable->data + coverage_offset, coverage, subst);
//
//            /* Loop over every glyph in the seg, check condition and substitute. */
//            for (g = seg->root; g != NULL; g = g->next)
//                if (!hz_should_skip_node(g, lookup_flags)) {
//                    if (hz_ot_layout_should_apply_subst(coverage, feature, g)) {
//                        g->gid = hz_map_get_value(coverage, g->gid);
//                        g->attach_class = hz_face_get_glyph_attach_class(face, g->gid);
//                    }
//                }
//
//            hz_map_destroy(coverage);
//            hz_array_destroy(subst);
//            break;
//        }
//        default:
//            return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
//    }
//
//    return HZ_OK;
//}
//
//hz_error_t
//hz_ot_layout_apply_gsub_nested_subtable(hz_face_t *face,
//                                        hz_feature_t feature,
//                                        hz_stream_t *subtable,
//                                        uint16_t lookup_type,
//                                        uint16_t lookup_flags,
//                                        hz_segment_t *seg,
//                                        hz_sequence_node_cache_t *seg_context,
//                                        uint16_t nested_index);
//
//hz_error_t
//hz_ot_layout_apply_gsub_nested_subtable(hz_face_t *face,
//                                        hz_feature_t feature,
//                                        hz_stream_t *subtable,
//                                        uint16_t lookup_type,
//                                        uint16_t lookup_flags,
//                                        hz_segment_t *seg,
//                                        hz_sequence_node_cache_t *seg_context,
//                                        uint16_t nested_index)
//{
//    hz_glyph_class_t class_skip = hz_ignored_classes_from_lookup_flags(lookup_flags);
//    uint16_t format = unpack16(subtable);
//
////    printf("%d.%d\n", lookup_type, format);
//
//    switch (lookup_type) {
//        case HZ_GSUB_LOOKUP_TYPE_SINGLE_SUBSTITUTION: {
//            hz_ot_layout_apply_nested_gsub_lookup1(face, subtable, format, class_skip, feature, seg,
//                                                   sequence_context, nested_index);
//            break;
//        }
//        case HZ_GSUB_LOOKUP_TYPE_MULTIPLE_SUBSTITUTION:
//            if (format == 1) {
//                hz_multiple_subst_format1_t table_cache;
//                hz_segment_node_t * g, *h;
//                uint16_t i, j;
//
//                hz_ot_read_multiple_subst_format1(subtable, &table_cache);
//                g = sequence_context->nodes[nested_index];
//
//                if (hz_ot_layout_should_apply_subst(table_cache.coverage, feature, g)) {
//                    /* glyph is covered, replace with appropriate seg */
//                    i = hz_map_get_value(table_cache.coverage, g->gid);
//                    hz_sequence_table_t *seg_table = &table_cache.sequences[i];
//                    hz_segment_node_t * t = g;
//                    for (j=0; j<sequence_table->glyph_count; ++j) {
//                        hz_segment_node_t * node = Alloc(sizeof(*node));
//                        node->gid = sequence_table->glyphs[j];
//                        node->prev = NULL;
//                        node->next = NULL;
//                        node->glyph_class = hz_face_get_glyph_class(face, node->gid);
//                        node->attach_class = hz_face_get_glyph_attach_class(face, node->gid);
//                        node->codepoint = g->codepoint;
//                        hz_segment_insert_node(t, node);
//                        hz_sequence_node_cache_insert(sequence_context, nested_index+(j+1), node);
//                        t = t->next;
//                    }
//
//                    hz_segment_pop_node(seg, g);
//                    hz_sequence_node_cache_remove(sequence_context, nested_index);
//                }
//
//                hz_ot_clear_multiple_subst_format1(&table_cache);
//            } else {
//                /* error */
//                return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
//            }
//            break;
//        case HZ_GSUB_LOOKUP_TYPE_LIGATURE_SUBSTITUTION: {
//            if (format == 1) {
//                Offset16 coverage_offset;
//                uint16_t ligature_set_count;
//                Offset16 *ligature_set_offsets;
//                hz_map_t *coverage_map = hz_map_create();
//
//                unpackf(subtable, "hh", &coverage_offset, &ligature_set_count);
//                ligature_set_offsets = Alloc(ligature_set_count * sizeof(uint16_t));
//                unpackf(subtable, "h:*", ligature_set_offsets, ligature_set_count);
//                hz_ot_read_coverage(subtable->data + coverage_offset, coverage_map, NULL);
//
//
//                hz_segment_node_t * g = sequence_context->nodes[nested_index];
//                /* loop over every non-ignored glyph in the section */
//                if (hz_ot_layout_should_apply_subst(coverage_map, feature, g)) {
//                    /* current glyph is covered, check pattern and replace */
//                    Offset16 ligature_set_offset = ligature_set_offsets[ hz_map_get_value(coverage_map, g->gid) ];
//                    hz_stream_t ligature_set = hz_stream_create(subtable->data + ligature_set_offset, 0);
//                    uint16_t ligature_count = unpack16(&ligature_set);
//                    hz_ligature_t **ligatures = Alloc(sizeof(hz_ligature_t*) * ligature_count);
//                    uint16_t ligature_index = 0;
//                    for (ligature_index = 0; ligature_index < ligature_count; ++ligature_index) {
//                        Offset16 ligature_offset = unpack16(&ligature_set);
//
//                        ligatures[ligature_index] = hz_ligature_create();
//                        hz_ligature_decode(ligatures[ligature_index], ligature_set.data + ligature_offset);
//                    }
//
//                    hz_ot_layout_apply_fit_ligature(face,
//                                                    ligatures,
//                                                    ligature_count,
//                                                    class_skip,
//                                                    seg,
//                                                    g);
//
//                    for (ligature_index = 0; ligature_index < ligature_count; ++ligature_index) {
//                        hz_ligature_destroy(ligatures[ligature_index]);
//                    }
//
//                    Free(ligatures);
//                }
//
//                Free(ligature_set_offsets);
//                hz_map_destroy(coverage_map);
//            } else {
//                /* error */
//                return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
//            }
//
//            break;
//        }
//        case HZ_GSUB_LOOKUP_TYPE_EXTENSION_SUBSTITUTION: {
//            if (format == 1) {
//                uint16_t extension_lookup_type;
//                Offset32 extension_offset;
//
//                unpackf(subtable, "hi", &extension_lookup_type, &extension_offset);
//
//                HZ_ASSERT(extension_lookup_type < 7);
//
//                subtable->data += extension_offset;
//                subtable->ptr = 0;
//
//                hz_ot_layout_apply_gsub_nested_subtable(face, feature, subtable, extension_lookup_type, lookup_flags,
//                                                        seg, sequence_context, nested_index);
//            } else {
//                /* error */
//                return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
//            }
//
//            break;
//        }
//        default:
//            return HZ_ERROR_INVALID_LOOKUP_TYPE;
//    }
//
//    return HZ_OK;
//}
//
//hz_error_t
//hz_ot_layout_apply_gpos_nested_subtable(hz_face_t *face,
//                                        hz_feature_t feature,
//                                        hz_stream_t *subtable,
//                                        uint16_t lookup_type,
//                                        uint16_t lookup_flags,
//                                        hz_segment_t *seg,
//                                        hz_sequence_node_cache_t *seg_context,
//                                        uint16_t nested_index);
//
hz_bool
hz_ot_apply_repos_value(hz_glyph_info_t *g, uint16_t value_format, const hz_value_record_t *record) {
    if (!value_format) return HZ_FALSE;

//    if (value_format & HZ_VALUE_FORMAT_X_PLACEMENT)
//        g->x_offset += record->x_placement;
//    if (value_format & HZ_VALUE_FORMAT_Y_PLACEMENT)
//        g->y_offset += record->y_placement;
//    if (value_format & HZ_VALUE_FORMAT_X_ADVANCE)
//        g->x_advance += record->x_advance;
//    if (value_format & HZ_VALUE_FORMAT_Y_ADVANCE)
//        g->y_advance += record->y_advance;

    return HZ_TRUE;
}

HZ_STATIC void
hz_ot_layout_apply_gsub_feature(hz_face_t *face,
                                hz_stream_t *table,
                                hz_feature_t feature,
                                hz_segment_t *seg)
{
    uint16_t type, flags, i, n;
    Unpackv(table, "hhh", &type, &flags, &n);

    /* NOTE: cache seg nodes which have valid unignored glyph classes as an optimization, so will not use functions
     that have to go through ignored glyphs over and over.
     */

    for (i=0; i<n; ++i) {
        Offset16 offset = Unpack16(table);
        hz_stream_t subtable = hz_stream_create(table->data + offset, 0);
//        hz_ot_layout_apply_gsub_subtable(face, &subtable, type, flags,
//                                         feature, seg);
    }
}

typedef struct hz_entry_exit_record_t {
    Offset16 entry_anchor_offset, exit_anchor_offset;
} hz_entry_exit_record_t;

typedef struct hz_anchor_t {
    int16_t x_coord, y_coord;
} hz_anchor_t;

typedef struct hz_anchor_pair_t {
    hz_bool has_entry, has_exit;
    hz_anchor_t entry, exit;
} hz_anchor_pair_t;


hz_anchor_t
hz_ot_layout_read_anchor(const unsigned char *data) {
    hz_stream_t stream = hz_stream_create(data, 0);
    hz_anchor_t anchor;
    uint16_t format = Unpack16(&stream);
    HZ_ASSERT(format >= 1 && format <= 3);
    Unpackv(&stream, "hh", &anchor.x_coord, &anchor.y_coord);
    return anchor;
}

hz_anchor_pair_t
hz_ot_layout_read_anchor_pair(const uint8_t *subtable, const hz_entry_exit_record_t *rec) {
    hz_anchor_pair_t anchor_pair;

    anchor_pair.has_entry = rec->entry_anchor_offset ? HZ_TRUE : HZ_FALSE;
    anchor_pair.has_exit = rec->exit_anchor_offset ? HZ_TRUE : HZ_FALSE;

    if (anchor_pair.has_entry)
        anchor_pair.entry = hz_ot_layout_read_anchor(subtable + rec->entry_anchor_offset);

    if (anchor_pair.has_exit)
        anchor_pair.exit = hz_ot_layout_read_anchor(subtable + rec->exit_anchor_offset);

    return anchor_pair;
}

typedef struct hz_mark_record_t {
    uint16_t mark_class;
    Offset16 mark_anchor_offset;
} hz_mark_record_t;

typedef struct hz_mark2_record_t {
    uint16_t mark_class;
    Offset16 mark2_anchor_offets;
} hz_mark2_record_t;


//hz_segment_node_t *
//hz_ot_layout_find_prev_and_class(hz_segment_node_t *node, hz_glyph_class_t glyph_class)
//{
//    node = node->prev;
//    while (node != NULL) {
//        if (node->glyph_class & glyph_class) {
//            /* found node with required class */
//            break;
//        }
//
//        node = node->prev;
//    }
//
//    return node;
//}
//
//hz_segment_node_t *
//hz_ot_layout_find_prev_with_class(hz_segment_node_t *node, hz_glyph_class_t glyph_class)
//{
//    node = node->prev;
//    while (node != NULL) {
//        if (node->glyph_class == glyph_class) {
//            /* found node with required class */
//            break;
//        }
//
//        node = node->prev;
//    }
//
//    return node;
//}
//
//hz_segment_node_t *
//hz_ot_layout_find_next_with_class(hz_segment_node_t *node, hz_glyph_class_t glyph_class)
//{
//    node = node->next;
//    while (node != NULL) {
//        if (node->glyph_class == glyph_class) {
//            /* found node with required class */
//            break;
//        }
//
//        node = node->next;
//    }
//
//    return node;
//}

//uint16_t
//hz_ot_layout_find_attach_base_with_anchor(hz_map_t *base_map, uint16_t mark_class, hz_segment_node_t *node) {
//    node = hz_ot_layout_find_prev_with_class(node, HZ_GLYPH_CLASS_BASE);
//
//    if (node != NULL) {
//        hz_index_t cp = node->codepoint;
//        while (node != NULL) {
//            if (node->codepoint != cp || (~node->glyph_class & HZ_GLYPH_CLASS_BASE)) {
//                return 0;
//            }
//
//
//
//            node = node->prev;
//        }
//
//        return 0;
//    }
//
//    return 0;
//}

typedef struct hz_ot_class2_record_t {
    hz_value_record_t value_record1;
    hz_value_record_t value_record2;
} hz_ot_class2_record_t;

typedef struct hz_ot_class1_record_t {
    hz_ot_class2_record_t *class2_records;
} hz_ot_class1_record_t;

typedef struct hz_ot_pair_pos_format2_table_t {
    hz_map_t *coverage;
    uint16_t value_format1;
    uint16_t value_format2;
    hz_map_t *class_def1;
    hz_map_t *class_def2;
    uint16_t class1_count;
    uint16_t class2_count;
    hz_ot_class1_record_t *class1_records;
} hz_ot_pair_pos_format2_table_t;

static hz_error_t
hz_ot_read_class_def_table(const uint8_t *data, hz_map_t *class_map) {
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

static void
hz_ot_read_pair_pos_format2_table(hz_stream_t *stream, hz_ot_pair_pos_format2_table_t *table) {
    uint16_t i, j;
    Offset16 coverage_offset, class_def1_offset, class_def2_offset;

    table->coverage = hz_map_create();
    table->class_def1 = hz_map_create();
    table->class_def2 = hz_map_create();

    coverage_offset = Unpack16(stream);
    hz_ot_read_coverage(stream->data + coverage_offset, table->coverage, NULL);
    table->value_format1 = Unpack16(stream);
    table->value_format2 = Unpack16(stream);
    class_def1_offset = Unpack16(stream);
    class_def2_offset = Unpack16(stream);

    hz_ot_read_class_def_table(stream->data + class_def1_offset, table->class_def1);
    hz_ot_read_class_def_table(stream->data + class_def2_offset, table->class_def2);

    table->class1_count = Unpack16(stream);
    table->class2_count = Unpack16(stream);
    table->class1_records = Alloc(sizeof(hz_ot_class1_record_t *) * table->class1_count);
    for (i = 0; i < table->class1_count; ++i) {
        hz_ot_class1_record_t *c1 = &table->class1_records[i];
        c1->class2_records = Alloc(sizeof(hz_ot_class2_record_t) * table->class2_count);

        for (j = 0; j < table->class2_count; ++j) {
            hz_ot_class2_record_t *c2 = &c1->class2_records[j];
            hz_ot_read_value_record(stream, &c2->value_record1, table->value_format1);
            hz_ot_read_value_record(stream, &c2->value_record2, table->value_format2);
        }
    }
}
//
//static void
//hz_ot_clear_pair_pos_format2_table(hz_ot_pair_pos_format2_table_t *table) {
//    uint16_t i;
//    for (i = 0; i < table->class1_count; ++i) {
//        hz_ot_class1_record_t *c1 = &table->class1_records[i];
//        Free(c1->class2_records);
//    }
//    Free(table->class1_records);
//    hz_map_destroy(table->coverage);
//    hz_map_destroy(table->class_def1);
//    hz_map_destroy(table->class_def2);
//}
//
//HZ_STATIC hz_error_t
//hz_ot_layout_apply_gpos_subtable(hz_face_t *face,
//                                 hz_stream_t *subtable,
//                                 uint16_t lookup_type,
//                                 uint16_t lookup_flags,
//                                 hz_feature_t feature,
//                                 hz_segment_t *seg)
//{
//    uint16_t class_ignore = hz_ignored_classes_from_lookup_flags(lookup_flags);
//    uint16_t format = unpack16(subtable);
//    hz_segment_node_t * g;
//
////    if (feature == HZ_FEATURE_KERN && lookup_type != 9 && lookup_type != 8 && lookup_type != 1) {
////        printf("%d.%d\n", lookup_type, format);
////    }
//
//    switch (lookup_type) {
//        case HZ_GPOS_LOOKUP_TYPE_SINGLE_ADJUSTMENT: {
//            switch (format) {
//                case 1: {
////                    hz_ot_single_pos_format1_table_t table;
////                    hz_ot_read_single_pos_format1_table(subtable, &table);
//                    break;
//                }
//                default:
//                    break;
//            }
//            break;
//        }
//        case HZ_GPOS_LOOKUP_TYPE_PAIR_ADJUSTMENT: {
//            switch (format) {
//                case 1: {
//                    break;
//                }
//
//                case 2: {
//                    hz_ot_pair_pos_format2_table_t table;
//                    hz_ot_read_pair_pos_format2_table(subtable, &table);
//
//                    for (g = seg->root; g != NULL; g = g->next) {
//                        if (!hz_should_skip_node(g, lookup_flags)) {
//                            if (hz_map_value_exists(table.coverage, g->gid)) {
//                                hz_segment_node_t * q = hz_next_valid_node(g, lookup_flags);
//
//                                if (q != NULL) {
//                                    if (hz_map_value_exists(table.class_def1, g->gid)
//                                            && hz_map_value_exists(table.class_def2, q->gid)) {
//                                        uint16_t group_class1, group_class2;
//                                        group_class1 = hz_map_get_value(table.class_def1, g->gid);
//                                        group_class2 = hz_map_get_value(table.class_def2, q->gid);
//
//                                        /* group_class1 should be less than the table's class1_count, similarly,
//                                         * group_class2 should be less than class2_count. */
//                                        if (group_class1 < table.class1_count && group_class2 < table.class2_count) {
//                                            const hz_ot_class2_record_t *cr = &table.class1_records[group_class1]
//                                                    .class2_records[group_class2];
//
//                                            hz_ot_apply_repos_value(g, table.value_format1, &cr->value_record1);
//                                            if (hz_ot_apply_repos_value(q, table.value_format2, &cr->value_record2))
//                                                g = q;
//
//                                            continue;
//                                        }
//                                    }
//                                }
//                            }
//                        }
//                    }
//
//                    hz_ot_clear_pair_pos_format2_table(&table);
//                    break;
//                }
//
//                default:
//                    return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
//            }
//            break;
//        }
//        case HZ_GPOS_LOOKUP_TYPE_CURSIVE_ATTACHMENT: {
//            if (format == 1) {
//                /* 4k stack buffer */
//                uint8_t                  buffer[4096];
//                hz_bump_allocator_t allocator;
//
//                hz_bump_allocator_init(&allocator, buffer, sizeof buffer);
//
//                Offset16 coverage_offset;
//                uint16_t record_count, record_index = 0;
//                hz_entry_exit_record_t *records;
//                hz_map_t *coverage_map = hz_map_create();
//
//                unpackf(subtable, "hh", &coverage_offset, &record_count);
//
//                records = hz_bump_allocator_alloc(&allocator, sizeof(hz_entry_exit_record_t) * record_count);
//
//                while (record_index < record_count) {
//                    hz_entry_exit_record_t *rec = &records[record_index];
//                    unpackf(subtable, "hh", &rec->entry_anchor_offset, &rec->exit_anchor_offset);
//                    ++record_index;
//                }
//
//                /* get coverage glyph to index map */
//                hz_ot_read_coverage(subtable->data + coverage_offset, coverage_map, NULL);
//
//                /* position glyphs */
//                hz_segment_node_t * g;
//
//                for (g = seg->root; g != NULL; g = g->next) {
//                    if (!hz_should_skip_node(g, lookup_flags)) {
//                        if (hz_map_value_exists(coverage_map, g->gid)) {
//                            hz_segment_node_t * q = hz_next_valid_node(g, lookup_flags);
//
//                            uint16_t curr_idx = hz_map_get_value(coverage_map, g->gid);
//                            const hz_entry_exit_record_t *curr_rec = records + curr_idx;
//                            hz_anchor_pair_t curr_pair = hz_ot_layout_read_anchor_pair(subtable->data, curr_rec);
//
//                            if (curr_pair.has_exit && q != NULL) {
//                                uint16_t next_idx = hz_map_get_value(coverage_map, q->gid);
//                                const hz_entry_exit_record_t *next_rec = records + next_idx;
//                                hz_anchor_pair_t next_pair = hz_ot_layout_read_anchor_pair(subtable->data, next_rec);
//
//                                int16_t ay = curr_pair.exit.y_coord;// / (int16_t) hz_face_get_upem(face);
//                                int16_t by = next_pair.entry.y_coord;// / (int16_t) hz_face_get_upem(face);
//                                int16_t ax = curr_pair.exit.x_coord;// / (int16_t) hz_face_get_upem(face);
//                                int16_t bx = next_pair.entry.x_coord;// / (int16_t) hz_face_get_upem(face);
//
//                                /* if (lookup_flags & HZ_LOOKUP_FLAG_RIGHT_TO_LEFT) { */
//                                /*     int16_t x_delta = bx-ax; */
//                                /*     int16_t y_delta = (g->y_offset + ay) - (q->y_offset + by); */
//
//                                /*     q->y_offset += y_delta; */
//                                /* } else { */
//                                /*     int16_t y_delta = (q->y_offset + by) - (g->y_offset + ay); */
//                                /*     g->y_offset += y_delta; */
//                                /* } */
//
//                                int16_t x_delta = (q->x_offset + bx) - (g->x_offset + ax);
//                                int16_t y_delta = (q->y_offset + by) - (g->y_offset + ay);
//                                g->y_offset += y_delta;
//                                /* g->x_offset += x_delta; */
//                            }
//
//                            g = q;
//                            continue;
//                        }
//                    }
//                }
//
//                /* release resources */
//                hz_bump_allocator_free(&allocator, records);
//                hz_bump_allocator_release(&allocator);
//                hz_map_destroy(coverage_map);
//            } else {
//                return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
//            }
//
//            break;
//        }
//        case HZ_GPOS_LOOKUP_TYPE_MARK_TO_BASE_ATTACHMENT: {
//            /* attach mark to base glyph point */
//            if (format == 1) {
//                uint8_t             buffer[4096];
//                hz_bump_allocator_t allocator;
//                hz_bump_allocator_init(&allocator, buffer, sizeof buffer);
//
//                Offset16 mark_coverage_offset;
//                Offset16 base_coverage_offset;
//                uint16_t mark_class_count;
//                Offset16 mark_array_offset;
//                Offset16 base_array_offset;
//                hz_map_t *mark_map = hz_map_create();
//                hz_map_t *base_map = hz_map_create();
//                hz_segment_node_t * m, *b; /* mark and base pointers */
//                hz_mark_record_t *mark_records;
//                uint16_t *base_anchor_offsets;
//
//                unpackf(subtable, "hhhhh",
//                        &mark_coverage_offset,
//                        &base_coverage_offset,
//                        &mark_class_count,
//                        &mark_array_offset,
//                        &base_array_offset);
//
//                /* parse coverages */
//                hz_ot_read_coverage(subtable->data + mark_coverage_offset, mark_map, NULL);
//                hz_ot_read_coverage(subtable->data + base_coverage_offset, base_map, NULL);
//
//                /* parse arrays */
//                uint16_t mark_count;
//                uint16_t base_count;
//
//                {
//                    /* parsing mark array */
//                    hz_stream_t marks = hz_stream_create(subtable->data + mark_array_offset, 0);
//                    mark_count = unpack16(&marks);
//                    mark_records = hz_bump_allocator_alloc(&allocator, sizeof(hz_mark_record_t) * mark_count);
//                    uint16_t mark_index = 0;
//
//                    while (mark_index < mark_count) {
//                        hz_mark_record_t *mark = &mark_records[mark_index];
//
//                        unpackf(&marks, "hh", &mark->mark_class,
//                                &mark->mark_anchor_offset);
//
//                        ++mark_index;
//                    }
//                }
//
//                {
//                    /* parsing base array */
//                    hz_stream_t bases = hz_stream_create(subtable->data + base_array_offset, 0);
//                    base_count = unpack16(&bases);
//                    base_anchor_offsets = Alloc(base_count * mark_class_count * sizeof(uint32_t));
//                    unpackf(&bases, "h:*", base_anchor_offsets,
//                            base_count * mark_class_count);
//                }
//
//                /* go over every glyph and position marks in relation to their base */
//                for (m = seg->root; m != NULL; m = m->next) {
//                    if (!hz_should_skip_node(m, lookup_flags)) {
//                        if (hz_map_value_exists(mark_map, m->gid)) {
//                            /* position mark in relation to previous base if it exists */
//                            uint16_t mark_index = hz_map_get_value(mark_map, m->gid);
//
//                            HZ_ASSERT(mark_index < mark_count);
//                            hz_mark_record_t *mark = &mark_records[ mark_index ];
//
//                            b = hz_ot_layout_find_prev_with_class(m, HZ_GLYPH_CLASS_BASE);//hz_ot_layout_find_attach_base_with_anchor(base_map, base_anchor_offsets, mark_class_count, mark_class, m);
//
//                            if (hz_map_value_exists(base_map, b->gid)) {
//
//                                uint16_t base_index = hz_map_get_value(base_map, b->gid);
//
//                                HZ_ASSERT(mark->mark_class < mark_class_count);
//                                uint16_t base_anchor_offset = base_anchor_offsets[ base_index * mark_class_count + mark->mark_class ];
//
//                                if (base_anchor_offset) {
//                                     hz_anchor_t base_anchor = hz_ot_layout_read_anchor(subtable->data
//                                                                                       + base_array_offset + base_anchor_offset);
//                                     hz_anchor_t mark_anchor = hz_ot_layout_read_anchor(subtable->data
//                                                                                        + mark_array_offset + mark->mark_anchor_offset);
//
//                                     hz_metrics_t *base_metric = hz_face_get_glyph_metrics(face, b->gid);
//                                     hz_metrics_t *mark_metric = hz_face_get_glyph_metrics(face, m->gid);
//
//                                     int32_t x1 = mark_anchor.x_coord;
//                                     int32_t y1 = mark_anchor.y_coord;
//                                     int32_t x2 = base_anchor.x_coord + b->x_offset;
//                                     int32_t y2 = base_anchor.y_coord + b->y_offset;
//
//                                     m->x_offset = x2 - x1;
//                                     m->y_offset = y2 - y1;
//                                }
//                            }
//                        }
//                    }
//                }
//
//                Free(base_anchor_offsets);
//                hz_bump_allocator_free(&allocator, mark_records);
//                hz_bump_allocator_release(&allocator);
//                hz_map_destroy(mark_map);
//                hz_map_destroy(base_map);
//            } else {
//                return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
//            }
//
//            break;
//        }
//        case HZ_GPOS_LOOKUP_TYPE_MARK_TO_LIGATURE_ATTACHMENT: {
//            if (format == 1) {
//                Offset16 mark_coverage_offset;
//                Offset16 ligature_coverage_offset;
//                uint16_t mark_class_count;
//                Offset16 mark_array_offset;
//                Offset16 ligature_array_offset;
//
//                hz_mark_record_t *mark_records;
//                uint16_t mark_count;
//                Offset16 *ligature_attach_offsets;
//                uint16_t ligature_count;
//
//                /* mark and base pointers */
//                hz_segment_node_t * m, *l;
//
//                hz_map_t *mark_map = hz_map_create();
//                hz_map_t *ligature_map = hz_map_create();
//
//                unpackf(subtable, "hhhhh",
//                        &mark_coverage_offset,
//                        &ligature_coverage_offset,
//                        &mark_class_count,
//                        &mark_array_offset,
//                        &ligature_array_offset);
//
//                /* parse coverages */
//                hz_ot_read_coverage(subtable->data + mark_coverage_offset, mark_map, NULL);
//                hz_ot_read_coverage(subtable->data + ligature_coverage_offset, ligature_map, NULL);
//
//                {
//                    /* parse mark array */
//                    uint16_t mark_index;
//                    hz_stream_t marks = hz_stream_create(subtable->data + mark_array_offset, 0);
//                    mark_count = unpack16(&marks);
//                    mark_records = Alloc(sizeof(hz_mark_record_t) * mark_count);
//
//                    for (mark_index = 0; mark_index < mark_count; ++mark_index) {
//                        hz_mark_record_t *mark = &mark_records[mark_index];
//
//                        unpackf(&marks, "hh", &mark->mark_class,
//                                &mark->mark_anchor_offset);
//                    }
//                }
//
//                {
//                    /* parse ligature array */
//                    uint16_t ligature_index;
//                    hz_stream_t ligatures = hz_stream_create(subtable->data + ligature_array_offset, 0);
//                    ligature_count = unpack16(&ligatures);
//                    ligature_attach_offsets = Alloc(ligature_count * sizeof(uint16_t));
//
//                    unpackf(&ligatures, "h:*", ligature_attach_offsets,
//                            ligature_count);
//                }
//
//                /* go through section glyphs and adjust marks */
//                for (m = seg->root; m != NULL; m = m->next) {
//                    if (!hz_should_skip_node(m, lookup_flags)) {
//                        if (hz_map_value_exists(mark_map, m->gid)) {
//                            l = hz_prev_node_not_of_class(m, HZ_GLYPH_CLASS_MARK, NULL);
//
//                            if (l->glyph_class & HZ_GLYPH_CLASS_LIGATURE) {
//                                if (hz_map_value_exists(ligature_map, l->gid)) {
//                                    uint16_t mark_index = hz_map_get_value(mark_map, m->gid);
//                                    hz_mark_record_t *mark_record = mark_records + mark_index;
//
//                                    uint16_t ligature_index = hz_map_get_value(ligature_map, l->gid);
//                                    Offset16 ligature_attach_offset = ligature_attach_offsets[ligature_index];
//
//                                    hz_stream_t ligature_attach_table = hz_stream_create(
//                                            subtable->data + ligature_array_offset + ligature_attach_offset,
//                                            0);
//
//                                    uint16_t component_count = unpack16(&ligature_attach_table);
//
//                                    Offset16 *anchor_offsets = (Offset16 *)
//                                            (ligature_attach_table.data + ligature_attach_table.ptr);
//
//                                    Offset16 ligature_anchor_offset = bswap16(
//                                            anchor_offsets[m->comp_idx * mark_class_count + mark_record->mark_class]);
//
//                                    if (mark_record->mark_anchor_offset && ligature_anchor_offset) {
//                                        hz_anchor_t mark_anchor = hz_ot_layout_read_anchor(subtable->data
//                                                                                           + mark_array_offset
//                                                                                           + mark_record->mark_anchor_offset);
//
//                                        hz_anchor_t ligature_anchor = hz_ot_layout_read_anchor(subtable->data
//                                                                                               + ligature_array_offset
//                                                                                               + ligature_attach_offset
//                                                                                               + ligature_anchor_offset);
//
//                                        int32_t x1 = mark_anchor.x_coord;
//                                        int32_t y1 = mark_anchor.y_coord;
//                                        int32_t x2 = ligature_anchor.x_coord;
//                                        int32_t y2 = ligature_anchor.y_coord;
//
//                                        m->x_offset = x2 - x1;
//                                        m->y_offset = y2 - y1;
//                                    }
//                                }
//                            }
//                        }
//                    }
//                }
//
//                /* destroy */
//                Free(mark_records);
//                Free(ligature_attach_offsets);
//                hz_map_destroy(mark_map);
//                hz_map_destroy(ligature_map);
//            } else {
//                return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
//            }
//            break;
//        }
//        case HZ_GPOS_LOOKUP_TYPE_MARK_TO_MARK_ATTACHMENT: {
//            if (format == 1) {
//                Offset16 mark1_coverage_offset;
//                Offset16 mark2_coverage_offset;
//                uint16_t mark_class_count;
//                Offset16 mark1_array_offset;
//                Offset16 mark2_array_offset;
//                hz_map_t *mark1_map = hz_map_create();
//                hz_map_t *mark2_map = hz_map_create();
//                hz_mark_record_t *mark1_records;
//                Offset16 *mark2_anchor_offsets;
//                uint16_t mark1_count, mark2_count;
//                hz_segment_node_t * node;
//
//                unpackf(subtable, "hhhhh",
//                        &mark1_coverage_offset,
//                        &mark2_coverage_offset,
//                        &mark_class_count,
//                        &mark1_array_offset,
//                        &mark2_array_offset);
//
//                /* parse coverages */
//                hz_ot_read_coverage(subtable->data + mark1_coverage_offset, mark1_map, NULL);
//                hz_ot_read_coverage(subtable->data + mark2_coverage_offset, mark2_map, NULL);
//
//                /* parse mark arrays */
//                {
//                    /* parse mark1 array */
//                    uint16_t mark_index;
//                    hz_stream_t mark_array = hz_stream_create(subtable->data + mark1_array_offset, 0);
//                    mark1_count = unpack16(&mark_array);
//                    mark1_records = Alloc(sizeof(hz_mark_record_t) * mark1_count);
//                    for (mark_index = 0; mark_index < mark1_count; ++mark_index) {
//                        hz_mark_record_t *record = mark1_records + mark_index;
//                        unpackf(&mark_array, "hh",
//                                &record->mark_class,
//                                &record->mark_anchor_offset);
//                    }
//                }
//
//                {
//                    /* parse mark2 array */
//                    hz_stream_t mark_array = hz_stream_create(subtable->data + mark2_array_offset, 0);
//                    mark2_count = unpack16(&mark_array);
//                    mark2_anchor_offsets = Alloc(sizeof(uint16_t) * mark_class_count * mark2_count);
//                    unpackf(&mark_array, "h:*",
//                            mark2_anchor_offsets,
//                            mark_class_count * mark2_count);
//                }
//
//                /* go over every glyph and position marks in relation to their base mark */
//                for (node = seg->root; node != NULL; node = node->next) {
//                    if (node->glyph_class & HZ_GLYPH_CLASS_MARK) {
//                        /* glyph is of mark class, position in relation to last mark */
//                        hz_segment_node_t * prev_node = hz_prev_node_not_of_class(node, class_ignore, NULL);
//                        if (prev_node != NULL && prev_node->glyph_class & HZ_GLYPH_CLASS_MARK) {
//                            /* previous mark found, check if both glyph's ids are found in the
//                             * coverage maps.
//                             * */
//                            if (hz_map_value_exists(mark1_map, node->gid) &&
//                                hz_map_value_exists(mark2_map, prev_node->gid)) {
//                                /* both marks glyphs are covered */
//                                uint16_t mark1_index = hz_map_get_value(mark1_map, node->gid);
//                                HZ_ASSERT(mark1_index < mark1_count);
//                                hz_mark_record_t *mark1 = &mark1_records[ mark1_index ];
//
//                                uint16_t mark2_index = hz_map_get_value(mark2_map, prev_node->gid);
//                                HZ_ASSERT(mark1->mark_class < mark_class_count);
//                                uint16_t mark2_anchor_offset = mark2_anchor_offsets[ mark2_index * mark_class_count
//                                                                                     + mark1->mark_class ];
//
//                                /* check if the base anchor is NULL */
//                                if (mark2_anchor_offset != 0) {
//                                    hz_anchor_t mark2_anchor = hz_ot_layout_read_anchor(
//                                            subtable->data + mark2_array_offset + mark2_anchor_offset);
//                                    hz_anchor_t mark1_anchor = hz_ot_layout_read_anchor(
//                                            subtable->data + mark1_array_offset + mark1->mark_anchor_offset);
//
//                                    hz_metrics_t *base_metric = hz_face_get_glyph_metrics(face, prev_node->gid);
//                                    hz_metrics_t *mark_metric = hz_face_get_glyph_metrics(face, node->gid);
//
//                                    int32_t x1 = mark1_anchor.x_coord;
//                                    int32_t y1 = mark1_anchor.y_coord;
//                                    int32_t x2 = mark2_anchor.x_coord;
//                                    int32_t y2 = mark2_anchor.y_coord;
//
//                                    node->x_offset += x2 - x1;
//                                    node->y_offset += y2 - y1;
//                                }
//                            }
//                        }
//                    }
//                }
//
//
//                Free(mark1_records);
//                Free(mark2_anchor_offsets);
//                hz_map_destroy(mark1_map);
//                hz_map_destroy(mark2_map);
//
//            } else {
//                return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
//            }
//
//            break;
//        }
//        case HZ_GPOS_LOOKUP_TYPE_CONTEXT_POSITIONING: {
//            break;
//        }
//        case HZ_GPOS_LOOKUP_TYPE_CHAINED_CONTEXT_POSITIONING: {
//            switch (format) {
//                case 1: {
//                    break;
//                }
//                case 2: {
//                    break;
//                }
//                case 3: {
//                    hz_segment_node_t * g;
//                    hz_chained_sequence_context_format3_table_t table_cache;
//                    hz_ot_read_chained_sequence_context_format3_table(subtable, &table_cache);
//
//                    for (g = seg->root; g != NULL; g = g->next) {
//                        if (!hz_should_skip_node(g, lookup_flags)) {
//                            hz_sequence_node_cache_t *seg_context = hz_sequence_node_cache_create();
//
//                            if (hz_ot_layout_pattern_match(g,
//                                                           table_cache.input_maps,
//                                                           table_cache.input_count,
//                                                           lookup_flags,
//                                                           sequence_context,
//                                                           HZ_FALSE)) {
//
//                                hz_segment_node_t * n1, *n2;
//                                hz_bool_t prefix_match, suffix_match;
//                                n1 = hz_prev_valid_node(sequence_context->nodes[0], lookup_flags);
//                                n2 = hz_next_valid_node(sequence_context->nodes[sequence_context->node_count - 1], lookup_flags);
//
//                                prefix_match = hz_ot_layout_pattern_match(n1,
//                                                                          table_cache.prefix_maps,
//                                                                          table_cache.prefix_count,
//                                                                          lookup_flags,
//                                                                          NULL,
//                                                                          HZ_TRUE);
//
//                                suffix_match = hz_ot_layout_pattern_match(n2,
//                                                                          table_cache.suffix_maps,
//                                                                          table_cache.suffix_count,
//                                                                          lookup_flags,
//                                                                          NULL,
//                                                                          HZ_FALSE);
//
//                                if (prefix_match && suffix_match) {
//                                    size_t k;
//
//                                    /* apply lookups to input, within context */
//                                    for (k=0; k<table_cache.lookup_count; ++k) {
//                                        hz_sequence_lookup_record_t *record = table_cache.lookup_records + k;
//                                        hz_ot_layout_apply_nested_lookup(face,
//                                                                         feature,
//                                                                         HZ_OT_TAG_GPOS,
//                                                                         seg,
//                                                                         sequence_context,
//                                                                         record);
//                                    }
//
//                                    g = sequence_context->nodes[sequence_context->node_count - 1];
//                                }
//                            }
//                            hz_sequence_node_cache_destroy(sequence_context);
//                        }
//                    }
//
//                    hz_ot_clear_chained_sequence_context_format3_table(&table_cache);
//                    break;
//                }
//                default:
//                    return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
//            }
//            break;
//        }
//        case HZ_GPOS_LOOKUP_TYPE_EXTENSION_POSITIONING: {
//            if (format == 1) {
//                uint16_t extension_lookup_type;
//                Offset32 extension_offset;
//
//                unpackf(subtable, "hi",
//                        &extension_lookup_type,
//                        &extension_offset);
//
//                HZ_ASSERT(extension_lookup_type < 9);
//
//                subtable->data += extension_offset;
//                subtable->ptr = 0;
//
//                hz_ot_layout_apply_gpos_subtable(face, subtable, extension_lookup_type, lookup_flags,
//                                                 feature, seg);
//            } else {
//                return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
//            }
//
//            break;
//        }
//        default: {
//            break;
//        }
//    }
//
//    return HZ_OK;
//}

HZ_STATIC void
hz_ot_layout_apply_gpos_feature(hz_face_t *face,
                                hz_stream_t *table,
                                hz_feature_t feature,
                                hz_segment_t *seg)
{
    uint16_t type, flags, i, n;
    Unpackv(table, "hhh", &type, &flags, &n);

    for (i=0; i<n; ++i) {
        Offset16 offset = Unpack16(table);
        hz_stream_t subtable = hz_stream_create(table->data + offset, 0);
//        hz_ot_layout_apply_gpos_subtable(face, &subtable, type, flags, feature, seg);
    }
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
    { HZ_FEATURE_CCMP, HZ_OT_TAG_GSUB, HZ_FEATURE_FLAG_ALWAYS_APPLIED },
    { HZ_FEATURE_ISOL, HZ_OT_TAG_GSUB, HZ_FEATURE_FLAG_ALWAYS_APPLIED },
    { HZ_FEATURE_FINA, HZ_OT_TAG_GSUB, HZ_FEATURE_FLAG_ALWAYS_APPLIED },
    { HZ_FEATURE_MEDI, HZ_OT_TAG_GSUB, HZ_FEATURE_FLAG_ALWAYS_APPLIED },
    { HZ_FEATURE_INIT, HZ_OT_TAG_GSUB, HZ_FEATURE_FLAG_ALWAYS_APPLIED },
    { HZ_FEATURE_RLIG, HZ_OT_TAG_GSUB, HZ_FEATURE_FLAG_ALWAYS_APPLIED },
    { HZ_FEATURE_RCLT, HZ_OT_TAG_GSUB, HZ_FEATURE_FLAG_ALWAYS_APPLIED },
    { HZ_FEATURE_CALT, HZ_OT_TAG_GSUB, HZ_FEATURE_FLAG_ALWAYS_APPLIED},
    /* Typographical forms */
    { HZ_FEATURE_LIGA, HZ_OT_TAG_GSUB, HZ_FEATURE_FLAG_ON_BY_DEFAULT },
    { HZ_FEATURE_DLIG, HZ_OT_TAG_GSUB, HZ_FEATURE_FLAG_OFF_BY_DEFAULT },
    { HZ_FEATURE_CSWH, HZ_OT_TAG_GSUB, HZ_FEATURE_FLAG_OFF_BY_DEFAULT },
    { HZ_FEATURE_MSET, HZ_OT_TAG_GSUB, HZ_FEATURE_FLAG_ALWAYS_APPLIED },
    /* Positioning features */
    { HZ_FEATURE_CURS, HZ_OT_TAG_GPOS, HZ_FEATURE_FLAG_ALWAYS_APPLIED },
    { HZ_FEATURE_KERN, HZ_OT_TAG_GPOS, HZ_FEATURE_FLAG_ALWAYS_APPLIED },
    { HZ_FEATURE_MARK, HZ_OT_TAG_GPOS, HZ_FEATURE_FLAG_ALWAYS_APPLIED },
    { HZ_FEATURE_MKMK, HZ_OT_TAG_GPOS, HZ_FEATURE_FLAG_ALWAYS_APPLIED }
};

static const hz_feature_layout_op_t std_feature_ops_buginese[] = {
    /* Localized forms */
    { HZ_FEATURE_LOCL, HZ_OT_TAG_GSUB, HZ_FEATURE_FLAG_NONE },
    /* Presentation forms */
    { HZ_FEATURE_CCMP, HZ_OT_TAG_GSUB, HZ_FEATURE_FLAG_REQUIRED },
    { HZ_FEATURE_RLIG, HZ_OT_TAG_GSUB, HZ_FEATURE_FLAG_REQUIRED },
    { HZ_FEATURE_LIGA, HZ_OT_TAG_GSUB, HZ_FEATURE_FLAG_NONE },
    { HZ_FEATURE_CLIG, HZ_OT_TAG_GSUB, HZ_FEATURE_FLAG_NONE },
    /* Kerning */
    { HZ_FEATURE_KERN, HZ_OT_TAG_GPOS, HZ_FEATURE_FLAG_NONE },
    { HZ_FEATURE_DIST, HZ_OT_TAG_GPOS, HZ_FEATURE_FLAG_REQUIRED },
    /* Mark placement */
    { HZ_FEATURE_MARK, HZ_OT_TAG_GPOS, HZ_FEATURE_FLAG_REQUIRED },
    { HZ_FEATURE_MKMK, HZ_OT_TAG_GPOS, HZ_FEATURE_FLAG_REQUIRED }
};

static const hz_feature_layout_op_t std_feature_ops_hangul[] = {
    /* Language based forms */
    { HZ_FEATURE_CCMP, HZ_OT_TAG_GSUB, HZ_FEATURE_FLAG_NONE },
    { HZ_FEATURE_LJMO, HZ_OT_TAG_GSUB, HZ_FEATURE_FLAG_REQUIRED },
    { HZ_FEATURE_VJMO, HZ_OT_TAG_GSUB, HZ_FEATURE_FLAG_REQUIRED },
    { HZ_FEATURE_TJMO, HZ_OT_TAG_GSUB, HZ_FEATURE_FLAG_REQUIRED }
};

static const hz_feature_layout_op_t std_feature_ops_hebrew[] = {
    /* Language based forms */
    { HZ_FEATURE_CCMP, HZ_OT_TAG_GSUB, HZ_FEATURE_FLAG_NONE },
    /* Typographical forms */
    { HZ_FEATURE_DLIG, HZ_OT_TAG_GSUB, HZ_FEATURE_FLAG_NONE },
    /* Positioning features */
    { HZ_FEATURE_KERN, HZ_OT_TAG_GPOS, HZ_FEATURE_FLAG_NONE },
    { HZ_FEATURE_MARK, HZ_OT_TAG_GPOS, HZ_FEATURE_FLAG_REQUIRED }
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
    { HZ_FEATURE_CCMP, HZ_OT_TAG_GSUB, HZ_FEATURE_FLAG_NONE },
    { HZ_FEATURE_LIGA, HZ_OT_TAG_GSUB, HZ_FEATURE_FLAG_NONE },
    { HZ_FEATURE_CLIG, HZ_OT_TAG_GSUB, HZ_FEATURE_FLAG_NONE },
    { HZ_FEATURE_DIST, HZ_OT_TAG_GPOS, HZ_FEATURE_FLAG_REQUIRED },
    { HZ_FEATURE_KERN, HZ_OT_TAG_GPOS, HZ_FEATURE_FLAG_REQUIRED },
    { HZ_FEATURE_MARK, HZ_OT_TAG_GPOS, HZ_FEATURE_FLAG_NONE },
    { HZ_FEATURE_MKMK, HZ_OT_TAG_GPOS, HZ_FEATURE_FLAG_REQUIRED },
};

hz_bool
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
                    *featuresptr = Alloc(cnt * sizeof(hz_feature_t));
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
    const hz_language_map_t *langmap;
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
typedef enum hz_cmap_platform_t  {
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

typedef struct hz_cmap_encoding_t {
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
typedef struct hz_cmap_subtable_format0_t {
    uint16_t format; // Format number is set to 0.
    uint16_t length; // This is the length in bytes of the subtable.
    /* For requirements on use of the language field,
     * see “Use of the language field in 'cmap' subtables” in this document.
     */
    uint16_t language;
    // An array that maps character codes to glyph index values.
    uint8_t glyph_id_array[256];
} hz_cmap_subtable_format0_t;


typedef struct hz_cmap_subtable_format4_t {
    uint16_t format;
    uint16_t length;
    uint16_t language;
    uint16_t seg_count_x2;
    uint16_t search_range;
    uint16_t entry_selector;
    uint16_t range_shift;
    uint16_t *end_code;
    uint16_t reserved_pad;
    uint16_t *start_code;
    int16_t *id_delta;
    uint16_t *id_range_offsets;
    uint16_t *glyph_id_array;
} hz_cmap_subtable_format4_t;

HZ_STATIC hz_index_t
hz_cmap_unicode_to_id(hz_cmap_subtable_format4_t *st, hz_unicode_t c) {
    uint16_t range_count = st->seg_count_x2 >> 1;
    uint16_t i = 0;
    hz_index_t id;

    /* search for code range */
    while (i < range_count) {
        uint16_t start_code = st->start_code[i];
        uint16_t end_code = st->end_code[i];
        int16_t id_delta = st->id_delta[i];
        uint16_t id_range_offset = st->id_range_offsets[i];

        if (c >= start_code && c <= end_code) {
            if (id_range_offset != 0) {
                uint16_t offset = *(st->glyph_id_array + (id_range_offset/2 + (c - start_code) - (range_count - i)));
                id = checkle() ? bswap16(offset) : offset;
                if (id != 0) id += id_delta;
            } else {
                id = c + id_delta;
            }

            return id;
        }

        ++i;
    }

    return 0; // map to .notdef
}

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

HZ_STATIC void
x86cpuid (long int a[4], long int fid)
{

}

HZ_STATIC int setup_x86cpu (void)
{
    long int result[4];
    x86cpuid(result, 0);
}

int hz_setup (void)
{
#if HZ_ARCH & HZ_ARCH_X86
    setup_x86cpu();
#endif

    hz_optimize();
}

int hz_cleanup(void)
{
}

// AVX2 codepoint to glyph index convert function using TrueType's cmap format 4 subtable
// Assumes the codepoint arrays are sorted
// Supports only the Unicode Basic Multilingual Plane (U+0000 to U+FFFF)
HZ_STATIC void
hz_apply_cmap_format4_encoding_unaligned_avx2(const hz_cmap_subtable_format4_t *subtable,
                                              uint16_t outdata[],
                                              const uint32_t indata[],
                                              size_t count)
{
    size_t dataptr;
    uint16_t segment_count = subtable->seg_count_x2/2;

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
            segment_start = simulate__mm256_i16gather_epi16((short const *)subtable->start_code, mid, 2);
            segment_end = simulate__mm256_i16gather_epi16((short const *)subtable->end_code, mid, 2);

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

            id_delta = simulate__mm256_i16gather_epi16((short const *)subtable->id_delta,
                                                       found_indices,
                                                       2);

            id_range_offsets = simulate__mm256_i16gather_epi16((short const *)subtable->id_range_offsets,
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

            __m256i glyph_id = _mm256_bswap16(simulate__mm256_i16gather_epi16((short const *)subtable->glyph_id_array,
                                                                                 glyph_id_index,
                                                                                 2));

            final_id = _mm256_or_si256(final_id, _mm256_and_si256(_mm256_add_epi16(glyph_id, id_delta), mask2));
            final_id = _mm256_permute4x64_epi64(final_id, 0xd8); // 0b11011000 [0,2,1,3] -> 0xd8
            _mm256_storeu_si256((__m256i *)(outdata + dataptr), final_id);
        }
    }
}

HZ_STATIC void
hz_apply_cmap_format4_encoding(hz_cmap_subtable_format4_t *subtable,
                               hz_index_t glyph_indices[],
                               hz_unicode_t codepoints[],
                               size_t size)
{
    for (size_t i = 0; i < size; ++i) {
        // map unicode characters to glyph indices in sequence
        glyph_indices[i] = hz_cmap_unicode_to_id(subtable, codepoints[i]);
    }
}

HZ_STATIC hz_bool
hz_cmap_apply_encoding(hz_stream_t *table,
                       hz_cmap_encoding_t encoding,
                       hz_index_t glyph_indices[],
                       hz_unicode_t codepoints[],
                       size_t size)
{
    hz_stream_t subtable = hz_stream_create(table->data + encoding.subtable_offset, 0);
    uint16_t format = Unpack16(&subtable);

    switch (format) {
        case HZ_CMAP_SUBTABLE_FORMAT_BYTE_ENCODING_TABLE: break;
        case 2: break;
        case HZ_CMAP_SUBTABLE_FORMAT_SEGMENT_MAPPING_TO_DELTA_VALUES: {
            hz_cmap_subtable_format4_t st;
            size_t i;

            Unpackv(&subtable, "hhhhhh", &st.length,
                    &st.language,
                    &st.seg_count_x2,
                    &st.search_range,
                    &st.entry_selector,
                    &st.range_shift);

            uint16_t num_segments = (st.seg_count_x2>>1);
            size_t table_size = num_segments * sizeof(uint16_t);

            st.end_code = Alloc(table_size);
            st.start_code = Alloc(table_size);
            st.id_delta = Alloc(table_size);
            st.id_range_offsets = Alloc(table_size);

            UnpackArray16(&subtable, num_segments, st.end_code);
            st.reserved_pad = Unpack16(&subtable);
            UnpackArray16(&subtable, num_segments, st.start_code);
            UnpackArray16(&subtable, num_segments, (uint16_t *) st.id_delta);
            UnpackArray16(&subtable, num_segments, st.id_range_offsets);
            st.glyph_id_array = (uint16_t *)(subtable.data + subtable.ptr);

#if HZ_CONFIG_USE_SIMD
            #if HZ_ARCH & HZ_ARCH_AVX2_BIT
            hz_apply_cmap_format4_encoding_unaligned_avx2(&st, seg->glyph_indices, seg->codepoints, seg->num_codepoints);
            #else
            #endif
#else
            hz_apply_cmap_format4_encoding(&st, glyph_indices, codepoints, size);
#endif

            Free(st.end_code);
            Free(st.start_code);
            Free(st.id_delta);
            Free(st.id_range_offsets);

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

        hz_cmap_apply_encoding(&table, encoding, glyph_indices, codepoints, size);
    }
}

HZ_STATIC void
hz_mirror_uc_symbols(hz_vector(hz_unicode_t) v)
{
    for (size_t i = 0; i < hz_array_size(v); ++i) {
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

HZ_STATIC hz_bool
hz_should_ignore_glyph(hz_buffer_t *buffer, size_t index, uint16_t flags) {
    if (buffer->has_info) {
        uint8_t attach_type = (flags & HZ_LOOKUP_FLAG_MARK_ATTACHMENT_TYPE_MASK) >> 8;
        uint16_t ignored_classes = hz_ignored_classes_from_lookup_flags(flags);

        return ((buffer->glyph_classes[index] & ignored_classes) ||
        (buffer->glyph_classes[index] & HZ_GLYPH_CLASS_MARK) && attach_type && buffer->attachment_classes[index] != attach_type);
    }

    return HZ_FALSE;
}

void hz_buffer_compute_info(hz_buffer_t *buffer, hz_face_t *face)
{
    hz_buffer_clear_info(buffer);

    // realloc info
    size_t size = hz_vector_size(buffer->glyph_indices);

    if (size > 0) {
        buffer->glyph_classes = Alloc(sizeof(uint16_t) * size);
        buffer->attachment_classes = Alloc(sizeof(uint16_t) * size);

        for (size_t s = 0; s < size; ++s) {
            buffer->glyph_classes[s] = hz_face_get_glyph_class(face, buffer->glyph_indices[s]);
            if (buffer->glyph_classes[s] & HZ_GLYPH_CLASS_MARK) {
                buffer->attachment_classes[s] = hz_face_get_glyph_attach_class(face, buffer->glyph_indices[s]);
            } else {
                buffer->attachment_classes[s] = 0;
            }
        }

        buffer->has_info = HZ_TRUE;
    }
}

void
hz_swap_buffers(hz_buffer_t **b1, hz_buffer_t **b2, hz_face_t *face)
{
    HZ_ASSERT(b1 != NULL && b2 != NULL);
    HZ_ASSERT(*b1 != *b2);

    // swap
    hz_buffer_t *tmp = *b1;
    *b1 = *b2;
    *b2 = tmp;

    hz_buffer_clear(*b2);
    hz_buffer_compute_info(*b1, face);
}

#define hz_swap(x,y) do { (x) ^= (y); (y) ^= (x); (x) ^= (y); } while(0)

void
hz_buffer_flip_direction(hz_buffer_t *buffer)
{
    // NOTE (Said): this can easily be optimized with SIMD later
    size_t size = hz_vector_size(buffer->glyph_indices);
    for (size_t i = 0; i < size/2; ++i) {
        size_t i1 = i, i2 = size - (i+1);

        // swap values at i1 and i2
        hz_swap(buffer->glyph_indices[i1], buffer->glyph_indices[i2]);
        hz_swap(buffer->codepoints[i1], buffer->codepoints[i2]);

        if (buffer->has_info) {
            hz_swap(buffer->glyph_classes[i1], buffer->glyph_classes[i2]);
            hz_swap(buffer->attachment_classes[i1], buffer->attachment_classes[i2]);
        }
    }
}

hz_bool
hz_buffer_contains_range(const hz_buffer_t *buffer, ssize_t i1, ssize_t i2)
{
    size_t size = hz_vector_size(buffer->glyph_indices);
    return i1 >= 0 && i1 < size
    && i2 >= 0 && i2 < size
    && i2 >= i1;
}

hz_buffer_t *hz_buffer_copy_range(hz_buffer_t *from, int x1, int x2)
{
    if (hz_buffer_contains_range(from,x1,x2)) {
        int len = (x2-x1)+1;
        hz_buffer_t *to = hz_buffer_create();
        hz_vector_resize(to->glyph_indices, len);
        hz_vector_resize(to->codepoints, len);
        memcpy(to->glyph_indices, from->glyph_indices + x1, sizeof(hz_index_t)*len);
        memcpy(to->codepoints, from->codepoints + x1, sizeof(hz_unicode_t)*len);
        return to;
    }

    return NULL;
}

int *hz_buffer_get_unignored_indices(hz_buffer_t *buffer, uint16_t lookup_flag)
{
    int *index_list = NULL;

    for (int i = 0; i < hz_vector_size(buffer->glyph_indices); ++i) {
        if (!hz_should_ignore_glyph(buffer, i, lookup_flag)) {
            hz_vector_push(index_list, i);
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
        memcpy(seg->in->codepoints, seg->codepoints, seg->num_codepoints*sizeof(hz_unicode_t));

        // map unicode characters to nominal glyph indices
        hz_vector_resize(seg->in->glyph_indices, seg->num_codepoints);
        hz_map_to_nominal_forms(face, seg->in->glyph_indices, seg->codepoints, seg->num_codepoints);

        seg->in->glyph_count = seg->num_codepoints;

        // sets glyph class information
        hz_buffer_compute_info(seg->in, face);
    }

}

// swaps in and out buffers, compute info for in buffer
// such as glyph and attachment classes
void hz_segment_swap_buffers(hz_segment_t *seg, hz_face_t *face)
{
    hz_swap_buffers(&seg->in,&seg->out,face);
}

typedef struct hz_lookup_subtable_t {
    uint16_t format;
} hz_lookup_subtable_t;

typedef struct hz_lookup_table_t {
    uint16_t lookup_type, lookup_flag;
    uint16_t subtable_count;
    hz_lookup_subtable_t **subtables;
    /* Index (base 0) into GDEF mark glyph sets structure.
     * This field is only present if the USE_MARK_FILTERING_SET lookup flag is set.
     */
    uint16_t mark_filtering_set;
} hz_lookup_table_t;

typedef struct hz_gsub_table_t {
    Version16Dot16 version;
    uint32_t num_lookups;
    hz_lookup_table_t *lookups;
    uint32_t num_features;
    hz_feature_list_item_t *features;
} hz_ot_gsub_table_t;

typedef struct hz_gpos_table_t {
    Version16Dot16 version;
    uint32_t num_lookups;
    hz_lookup_table_t *lookups;
    uint32_t num_features;
    hz_feature_list_item_t *features;
} hz_ot_gpos_table_t;

typedef struct hz_shape_plan_t {
    hz_font_t *font;
    hz_direction_t direction;
    hz_script_t script;
    hz_language_t language;
    hz_feature_t *features;
    unsigned int num_features;
    hz_ot_gsub_table_t gsub_table;
    hz_ot_gpos_table_t gpos_table;
} hz_shape_plan_t;

HZ_STATIC void
hz_load_feature_table(hz_stream_t *stream, hz_feature_table_t *table)
{
    Unpackv(stream, "hh", &table->feature_params,
            &table->lookup_index_count);

    table->lookup_list_indices = Alloc(sizeof(uint16_t)
                                       * table->lookup_index_count);

    UnpackArray16(stream, table->lookup_index_count, table->lookup_list_indices);
}

typedef struct hz_single_substitution_format1_t {
    uint16_t format;
    hz_map_t *coverage;
    int16_t delta_glyph_id;
} hz_single_substitution_format1_t;

typedef struct hz_single_substitution_format2_t {
    uint16_t format;
    hz_map_t *coverage;
    uint16_t glyph_count;
    uint16_t *substitute_glyph_ids;
} hz_single_substitution_format2_t;

HZ_STATIC hz_error_t
hz_load_gsub_single_substitution_subtable(hz_stream_t *stream,
                                          hz_lookup_table_t *lookup,
                                          uint16_t subtable_index,
                                          uint16_t format)
{
    switch (format) {
        case 1: {
            hz_single_substitution_format1_t *subtable = Alloc(sizeof(*subtable));

            subtable->format = format;
            Offset16 coverage_offset = Unpack16(stream);
            subtable->coverage = hz_map_create();
            hz_ot_read_coverage(stream->data + coverage_offset, subtable->coverage, NULL);
            subtable->delta_glyph_id = Unpack16(stream);

            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }

        case 2: {
            hz_single_substitution_format2_t *subtable = Alloc(sizeof(*subtable));

            subtable->format = format;
            Offset16 coverage_offset = Unpack16(stream);
            subtable->coverage = hz_map_create();
            hz_ot_read_coverage(stream->data + coverage_offset, subtable->coverage, NULL);
            subtable->glyph_count = Unpack16(stream);

            if (!subtable->glyph_count) {
                /* error, no glyphs in lookup subtable */
                subtable->substitute_glyph_ids = NULL;
            } else {
                subtable->substitute_glyph_ids = Alloc(sizeof(uint16_t) * subtable->glyph_count);
                UnpackArray16(stream, subtable->glyph_count, subtable->substitute_glyph_ids);
            }

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
    hz_map_t *coverage;
    uint16_t ligature_set_count;
    hz_ligature_set_table_t *ligature_sets;
} hz_ligature_substitution_format1_subtable_t;

HZ_STATIC hz_error_t
hz_load_gsub_ligature_substitution_subtable(hz_stream_t *stream,
                                            hz_lookup_table_t *lookup,
                                            uint16_t subtable_index,
                                            uint16_t format)
{
    switch (format) {
        case 1: {
            hz_ligature_substitution_format1_subtable_t *subtable = Alloc(sizeof(*subtable));

            subtable->format = format;
            Offset16 coverage_offset = Unpack16(stream);
            subtable->coverage = hz_map_create();
            hz_ot_read_coverage(stream->data + coverage_offset, subtable->coverage, NULL);
            subtable->ligature_set_count = Unpack16(stream);
            subtable->ligature_sets = Alloc(sizeof(hz_ligature_set_table_t) * subtable->ligature_set_count);

            Offset16 *ligature_set_offsets = Alloc(subtable->ligature_set_count * sizeof(Offset16));
            UnpackArray16(stream, subtable->ligature_set_count, ligature_set_offsets);

            for (uint16_t i = 0; i < subtable->ligature_set_count; ++i) {
                hz_ligature_set_table_t *ligature_set = subtable->ligature_sets+i;

                uintptr_t setptr = ligature_set_offsets[i];
                stream_seek(stream, setptr);
                ligature_set->ligature_count = Unpack16(stream);
                ligature_set->ligatures = Alloc(sizeof(hz_ligature_t) * ligature_set->ligature_count);
                Offset16 *ligature_offsets = Alloc(ligature_set->ligature_count * sizeof(Offset16));
                UnpackArray16(stream, ligature_set->ligature_count, ligature_offsets);

                for (uint16_t j = 0; j < ligature_set->ligature_count; ++j) {
                    hz_ligature_t *ligature = ligature_set->ligatures + j;
                    stream_seek(stream, setptr + ligature_offsets[j]);
                    ligature->ligature_glyph = Unpack16(stream);
                    ligature->component_count = Unpack16(stream);
                    if (ligature->component_count > 1) {
                        ligature->component_glyph_ids = Alloc((ligature->component_count - 1) * sizeof(uint16_t));
                        UnpackArray16(stream, ligature->component_count - 1, ligature->component_glyph_ids);
                    } else {
                        ligature->component_glyph_ids = NULL;
                    }

                }

                Free(ligature_offsets);
            }

            Free(ligature_set_offsets);

            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }
        default:
            return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
    }

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
            hz_chained_sequence_context_format1_subtable_t *subtable = Alloc(sizeof(*subtable));
            subtable->format = format;

            Offset16 coverage_offset = Unpack16(stream);
            subtable->rule_set_count = Unpack16(stream);
            subtable->coverage =  hz_map_create();
            hz_ot_read_coverage(stream->data + coverage_offset, subtable->coverage, NULL);

            // printf("%d\n", subtable->rule_set_count);
            Offset16 *rule_set_offsets = Alloc(sizeof(Offset16) * subtable->rule_set_count);
            UnpackArray16(stream, subtable->rule_set_count, rule_set_offsets);

            subtable->rule_sets = Alloc(sizeof(*subtable->rule_sets) * subtable->rule_set_count);
            for (int i = 0; i < subtable->rule_set_count; ++i) {
                if (rule_set_offsets[i] != 0) {
                    hz_parse_chained_sequence_rule_set(stream->data + rule_set_offsets[i],
                                                       subtable->rule_sets+i);
                } else {
                    subtable->rule_sets[i].count = 0;
                    subtable->rule_sets[i].rules = NULL;
                }
            }

            Free(rule_set_offsets);
            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }

        case 2: {
            break;
        }
        case 3: {
            hz_chained_sequence_context_format3_subtable_t *subtable = Alloc(sizeof(*subtable));
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
            hz_multiple_substitution_format1_subtable_t *subtable = Alloc(sizeof(*subtable));

            subtable->format = format;
            Offset16 coverage_offset = Unpack16(stream);
            subtable->coverage =  hz_map_create();
            hz_ot_read_coverage(stream->data + coverage_offset, subtable->coverage, NULL);

            subtable->sequence_count = Unpack16(stream);

            subtable->sequences = Alloc(subtable->sequence_count * sizeof(hz_sequence_table_t));
            Offset16 *sequence_offsets = Alloc(subtable->sequence_count * sizeof(Offset16));
            UnpackArray16(stream, subtable->sequence_count, sequence_offsets);

            for (uint16_t i = 0 ; i < subtable->sequence_count; ++i) {
                hz_ot_read_sequence_table(stream->data + (size_t)sequence_offsets[i], &subtable->sequences[i]);
            }

            Free(sequence_offsets);
            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }
        default:
            return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
    }

    return HZ_OK;
}

HZ_STATIC hz_error_t
hz_load_gsub_lookup_subtable(uint8_t *data,
                             hz_lookup_table_t *lookup,
                             uint16_t lookup_type,
                             uint16_t subtable_index)
{
    uint16_t format;
    hz_stream_t stream = hz_stream_create(data,0);

    gsub_lookup_extension_label:
    format = Unpack16(&stream);

    switch (lookup_type) {
        case HZ_GSUB_LOOKUP_TYPE_SINGLE_SUBSTITUTION:
            return hz_load_gsub_single_substitution_subtable(&stream, lookup, subtable_index, format);

        case HZ_GSUB_LOOKUP_TYPE_MULTIPLE_SUBSTITUTION:
            return hz_load_gsub_multiple_substitution_subtable(&stream, lookup, subtable_index, format);
        case HZ_GSUB_LOOKUP_TYPE_ALTERNATE_SUBSTITUTION: break;
        case HZ_GSUB_LOOKUP_TYPE_LIGATURE_SUBSTITUTION:
            return hz_load_gsub_ligature_substitution_subtable(&stream, lookup, subtable_index, format);

        case HZ_GSUB_LOOKUP_TYPE_CONTEXTUAL_SUBSTITUTION: break;
        case HZ_GSUB_LOOKUP_TYPE_CHAINED_CONTEXTS_SUBSTITUTION:
            return hz_load_gsub_chained_contexts_substitution_subtable(&stream, lookup, subtable_index, format);
        case HZ_GSUB_LOOKUP_TYPE_EXTENSION_SUBSTITUTION: {
             // This is implemented inline as it's quite trivial and
             // it's a special lookup type which doesn't allocate a structure.
             if (format == 1) {
                lookup->lookup_type = Unpack16(&stream);
                lookup_type = lookup->lookup_type;
                Offset32 extension_offset = Unpack32(&stream);
                stream.data += extension_offset;
                stream.ptr = 0;
                goto gsub_lookup_extension_label;
            }

            return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
        }

        case HZ_GSUB_LOOKUP_TYPE_REVERSE_CHAINING_CONTEXTUAL_SINGLE_SUBSTITUTION: break;

        default:
            return HZ_ERROR_INVALID_LOOKUP_TYPE;
    }
}

HZ_STATIC hz_error_t
hz_unload_gsub_single_substitution_subtable(hz_lookup_subtable_t *base)
{
    switch (base->format) {
        case 1: {
            hz_single_substitution_format1_t *subtable = (hz_single_substitution_format1_t *)base;
            hz_map_destroy(subtable->coverage);
            Free(subtable);
            break;
        }

        case 2: {
            hz_single_substitution_format2_t *subtable = (hz_single_substitution_format2_t *)base;
            hz_map_destroy(subtable->coverage);
            if (subtable->glyph_count > 0) Free(subtable->substitute_glyph_ids);
            Free(subtable);
            break;
        }

        default:
            return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
    }

    return HZ_OK;
}

HZ_STATIC hz_error_t
hz_unload_gsub_ligature_substitution_subtable(hz_lookup_subtable_t *base)
{
    switch (base->format) {
        case 1: {
            uint16_t i, j;
            hz_ligature_substitution_format1_subtable_t *subtable = (hz_ligature_substitution_format1_subtable_t *)base;
            hz_map_destroy(subtable->coverage);
            for (i = 0; i < subtable->ligature_set_count; ++i) {
                hz_ligature_set_table_t *ligature_set = subtable->ligature_sets + i;
                for (j = 0; j < ligature_set->ligature_count; ++j) {
                    hz_ligature_t *ligature = ligature_set->ligatures + j;
                    Free(ligature->component_glyph_ids);
                }
                Free(ligature_set->ligatures);
            }

            Free(subtable->ligature_sets);
            Free(subtable);
            break;
        }
        default:
            return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
    }

    return HZ_OK;
}

HZ_STATIC void
hz_unload_gsub_lookup_subtable(hz_lookup_table_t *table, uint16_t subtable_index)
{
    if (table->subtables[subtable_index] != NULL) {
        hz_lookup_subtable_t *base = table->subtables[subtable_index];
        switch (table->lookup_type) {
            case HZ_GSUB_LOOKUP_TYPE_SINGLE_SUBSTITUTION: {
                hz_unload_gsub_single_substitution_subtable(base);
                break;
            }

            case HZ_GSUB_LOOKUP_TYPE_LIGATURE_SUBSTITUTION: {
                hz_unload_gsub_ligature_substitution_subtable(base);
                break;
            }
        }
    }
}

HZ_STATIC hz_error_t
hz_load_gsub_lookup_table(const uint8_t *data, hz_lookup_table_t *table)
{
    hz_stream_t stream = hz_stream_create(data,0);

    table->lookup_type = Unpack16(&stream);
    table->lookup_flag = Unpack16(&stream);
    table->subtable_count = Unpack16(&stream);

    if (table->subtable_count > 0) {
        Offset16 *offsets = Alloc(sizeof(Offset16) * table->subtable_count);
        UnpackArray16(&stream, table->subtable_count, offsets);

        // Set pointers to NULL by default
        table->subtables = Alloc(sizeof(*table->subtables) * table->subtable_count);
        for (uint16_t i = 0; i < table->subtable_count; ++i) {
            table->subtables[i] = NULL;
        }

        // Load & parse subtables
        uint16_t original_lookup_type = table->lookup_type;
        for (uint16_t i = 0; i < table->subtable_count; ++i) {
            hz_load_gsub_lookup_subtable(stream.data + offsets[i], table, original_lookup_type, i);
        }

        Free(offsets);
    }

    // Only include mark filtering set if flag USE_MARK_FILTERING_SET is enabled
    if (table->lookup_flag & HZ_LOOKUP_FLAG_USE_MARK_FILTERING_SET)
        table->mark_filtering_set = Unpack16(&stream);

    return HZ_OK;
}


HZ_STATIC void
hz_unload_feature_table(hz_feature_table_t *table)
{
    if (table->lookup_index_count > 0)
        Free(table->lookup_list_indices);
}

HZ_STATIC void
hz_unload_gsub_lookup_table(hz_lookup_table_t *table)
{
    if (table->subtable_count > 0) {
        for (uint16_t i = 0; i < table->subtable_count; ++i)
            hz_unload_gsub_lookup_subtable(table, i);

        Free(table->subtables);
    }
}

HZ_STATIC hz_error_t
hz_shape_plan_load_gsub_table(hz_shape_plan_t *plan)
{
    hz_face_t *face;
    hz_stream_t table;
    hz_ot_gsub_table_t *gsub_table;

    face = hz_font_get_face(plan->font);
    table = hz_stream_create(face->data+face->gsub, 0);
    gsub_table = &plan->gsub_table;

    Version16Dot16 version;
    uint16_t script_list_offset;
    uint16_t feature_list_offset;
    uint16_t lookup_list_offset;
    uint32_t feature_variations_offset;

    version = Unpack32(&table);

    switch (version) {
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

    gsub_table->version = version;

    {
        // Load feature list table
        hz_stream_t stream = hz_stream_create(table.data + feature_list_offset, 0);
        uint16_t num_features = Unpack16(&stream);
        gsub_table->num_features = num_features;
        gsub_table->features = Alloc(sizeof(hz_feature_list_item_t) * num_features);

        for (uint16_t i = 0; i < num_features; ++i) {
            gsub_table->features[i].tag = Unpack32(&stream);
            Offset16 offset = Unpack16(&stream);

            uintptr_t lastptr = stream_tell(&stream);
            stream_seek(&stream, offset);
            hz_load_feature_table(&stream, &gsub_table->features[i].table);
            stream_seek(&stream, lastptr);
        }
    }

    {
        // Load lookups
        uint16_t i, num_lookups;
        Offset16 *offsets;

        hz_stream_t stream = hz_stream_create(table.data + lookup_list_offset, 0);
        num_lookups = Unpack16(&stream);

        gsub_table->num_lookups = num_lookups;
        gsub_table->lookups = Alloc(sizeof(hz_lookup_table_t) * num_lookups);

        offsets = Alloc(sizeof(Offset16) * num_lookups);
        UnpackArray16(&stream, num_lookups, offsets);

        for (i = 0; i < num_lookups; ++i) {
            hz_load_gsub_lookup_table(stream.data + offsets[i], &gsub_table->lookups[i]);
        }

        Free(offsets);
    }

    return HZ_OK;
}

HZ_STATIC void
hz_ot_load_gsub_table(hz_ot_gsub_table_t *gpos_table, hz_font_t *font)
{

}

HZ_STATIC void
hz_shape_plan_load_tables(hz_shape_plan_t *plan)
{
    hz_shape_plan_load_gsub_table(plan);
}

HZ_STATIC hz_shape_plan_t *
hz_shape_plan_create(hz_font_t *font,
                     hz_segment_t *seg,
                     const hz_feature_t *features,
                     unsigned int num_features)
{
    hz_shape_plan_t *plan = Alloc(sizeof(hz_shape_plan_t));
    plan->font = font;
    plan->direction = seg->direction;
    plan->script = seg->script;
    plan->language = seg->language;

    if (features != NULL) {
        plan->features = Alloc(sizeof(hz_feature_t) * num_features);
        plan->num_features = num_features;
        memcpy(plan->features, features, num_features * sizeof(hz_feature_t));
    } else {
        plan->features = NULL;
        plan->num_features = 0;
    }

    hz_shape_plan_load_tables(plan);
    return plan;
}


HZ_STATIC void
hz_shape_plan_unload_gsub_table(hz_shape_plan_t *plan)
{
    hz_ot_gsub_table_t *gsub = &plan->gsub_table;

    if (gsub->num_features > 0) {
        for (uint32_t i = 0; i < gsub->num_features; ++i)
            hz_unload_feature_table(&gsub->features[i].table);

        Free(gsub->features);
    }

    if (gsub->num_lookups > 0) {
        for (uint32_t i = 0; i < gsub->num_lookups; ++i)
            hz_unload_gsub_lookup_table(&gsub->lookups[i]);

        Free(gsub->lookups);
    }
}

HZ_STATIC size_t
hz_segment_next_valid_index(hz_segment_t *seg,
                            size_t index,
                            uint16_t lookup_flag)
{

}


HZ_STATIC int64_t next_joining_arabic_glyph(hz_buffer_t *buffer, int64_t g, uint16_t lookup_flag)
{
    size_t size = hz_vector_size(buffer->glyph_indices);
    do {
        if (++g >= size)
            return -1;

    } while (hz_should_ignore_glyph(buffer,g,lookup_flag));

    return g;
}

HZ_STATIC int64_t prev_joining_arabic_glyph(hz_buffer_t *buffer, int64_t g, uint16_t lookup_flag)
{
    do {
        if (--g < 0)
            return -1;

    } while (hz_should_ignore_glyph(buffer,g,lookup_flag));

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
                                uint16_t lookup_flag)
{
    hz_arabic_joining_triplet_t triplet;

    if (hz_shape_complex_arabic_char_joining(buffer->codepoints[index], &triplet.curr_joining)) {
        int64_t prev_index = prev_joining_arabic_glyph(buffer, index,
                                                       HZ_LOOKUP_FLAG_IGNORE_MARKS);
        int64_t next_index = next_joining_arabic_glyph(buffer, index,
                                                       HZ_LOOKUP_FLAG_IGNORE_MARKS);

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

HZ_STATIC hz_bool hz_shape_complex_arabic_init(hz_buffer_t *buffer, int64_t index, uint16_t lookup_flag)
{
    hz_arabic_joining_triplet_t triplet = hz_shape_complex_arabic_joining(buffer,index,lookup_flag);
    return triplet.does_apply ? triplet.init && !(triplet.medi || triplet.fina) : HZ_FALSE;
}

HZ_STATIC hz_bool hz_shape_complex_arabic_medi(hz_buffer_t *buffer, int64_t index, uint16_t lookup_flag)
{
    hz_arabic_joining_triplet_t triplet = hz_shape_complex_arabic_joining(buffer,index,lookup_flag);
    return triplet.does_apply ? triplet.medi : HZ_FALSE;
}

HZ_STATIC hz_bool hz_shape_complex_arabic_fina(hz_buffer_t *buffer, int64_t index, uint16_t lookup_flag)
{
    hz_arabic_joining_triplet_t triplet = hz_shape_complex_arabic_joining(buffer,index,lookup_flag);
    return triplet.does_apply ? triplet.fina && !(triplet.medi || triplet.init) : HZ_FALSE;
}

HZ_STATIC hz_bool hz_shape_complex_arabic_isol(hz_buffer_t *buffer, int64_t index, uint16_t lookup_flag)
{
    hz_arabic_joining_triplet_t triplet = hz_shape_complex_arabic_joining(buffer,index,lookup_flag);
    return triplet.does_apply ? !(triplet.fina || triplet.medi || triplet.init) : HZ_FALSE;
}

HZ_STATIC hz_bool
hz_should_apply_replacement(hz_buffer_t *buffer,
                            hz_feature_t feature,
                            uint16_t node_index,
                            uint16_t lookup_flag)
{
    if (feature == HZ_FEATURE_INIT) {
        return hz_shape_complex_arabic_init(buffer, node_index, lookup_flag);
    } else if (feature == HZ_FEATURE_MEDI) {
        return hz_shape_complex_arabic_medi(buffer, node_index, lookup_flag);
    } else if (feature == HZ_FEATURE_FINA) {
        return hz_shape_complex_arabic_fina(buffer, node_index, lookup_flag);
    } else if (feature == HZ_FEATURE_ISOL) {
        return hz_shape_complex_arabic_isol(buffer, node_index, lookup_flag);
    }

    return HZ_TRUE;
}

typedef struct hz_range_t {
    short mn, mx;
    short base; // base pointer into index list
    hz_bool is_ignored;
} hz_range_t;

typedef struct hz_range_list_t {
    hz_vector(hz_range_t) ranges; // alternating ignored -> unignored ranges
    hz_vector(short) ignored_indices;
    hz_vector(short) unignored_indices;
} hz_range_list_t;

hz_range_list_t *hz_range_list_create(void)
{
    hz_range_list_t *list = Alloc(sizeof(*list));
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
    Free(list);
}

// compute range list from buffer
hz_range_list_t *
hz_compute_range_list(hz_buffer_t *buffer, uint16_t lookup_flag)
{
    hz_range_list_t *range_list = hz_range_list_create();

    short mn=0,mx=0;
    short ign_base = 0, nign_base = 0;

    for (size_t i = 0; i < hz_vector_size(buffer->glyph_indices); ++i) {
        hz_bool curr_ign = hz_should_ignore_glyph(buffer, i, lookup_flag);
        // add current
        if (curr_ign) {
            hz_vector_push(range_list->ignored_indices, (short)i);
        } else {
            hz_vector_push(range_list->unignored_indices, (short)i);
        }

        if (i + 1 >= hz_vector_size(buffer->glyph_indices)) {
            hz_range_t range;
            range.mn = mn; range.mx = i;
            range.base = curr_ign ? ign_base : nign_base;
            range.is_ignored = curr_ign;
            hz_vector_push(range_list->ranges, range);
            break;
        }

        hz_bool next_ign = hz_should_ignore_glyph(buffer, i+1, lookup_flag);

        if (curr_ign != next_ign) {
            mx = i;
            short span = (mx-mn)+1;
            hz_range_t range;
            range.mn = mn; range.mx = mx;
            range.is_ignored = curr_ign;
            range.base = curr_ign ? ign_base : nign_base;
            hz_vector_push(range_list->ranges, range);
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


short
range_list_find_range(hz_range_list_t *range_list, short i)
{
#if 0
    // find range for index x using binary search algorithm
    short low = 0;
    short high = hz_vector_size(range_list->ranges)-1;

    if (i >= range_list->ranges[low].mn && i <= range_list->ranges[high].mx) {
        if (low == high) {
            return 0;
        }

        while (low <= high) {
            short mid = (low + high) / 2;

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
#else
    for (int r = 0; r < hz_vector_size(range_list->ranges); ++r) {
        if (i >= range_list->ranges[r].mn && i <= range_list->ranges[r].mx)
            return r;
    }
#endif

    return -1; // NOT FOUND
}


// apply GSUB lookup table to glyph range [i1,i2] within the segment buffer.
// depth is the current nested lookup depth index, as this can be recursive.
void
hz_shape_plan_apply_gsub_lookup(hz_shape_plan_t *plan,
                                hz_feature_t feature,
                                uint16_t lookup_index,
                                hz_buffer_t *in, hz_buffer_t *out,
                                int x1, int x2, int depth)
{
    uint8_t temp[4096];
    LinearAllocator la = CreateLinearAllocator(temp, sizeof temp);

    HZ_ASSERT(in != NULL && out != NULL);
    HZ_ASSERT(in != out);
    HZ_ASSERT(hz_buffer_contains_range(in,x1,x2));

    if (depth >= HZ_OT_MAX_RECURSE_DEPTH) {
        // hit max recurse depth, wind back up the call stack
        return;
    }

    hz_ot_gsub_table_t *gsub_table = &plan->gsub_table;
    if (lookup_index >= gsub_table->num_lookups) {
        // error, either user code or font requested for an unavailable lookup
        fprintf(stderr, "warning, requested non-existant lookup %d\n", lookup_index);
        return;
    }

    hz_lookup_table_t *table = &gsub_table->lookups[lookup_index];
    hz_face_t *face = hz_font_get_face(plan->font);

    // copy segment glyph ids and info into a read-only buffer
    hz_buffer_t *b1, *b2;
    b1 = hz_buffer_create();
    b2 = hz_buffer_create();
    hz_buffer_add_range(b1, in, x1, x2);
    printf("lookup index: %d {x1:%d, x2:%d, depth:%d}\n", lookup_index,x1,x2,depth);

    for (uint16_t i = 0; i < table->subtable_count; ++i) {
        hz_lookup_subtable_t *base = table->subtables[i];
        // NOTE: based on lookup type, if it has no ability to grow or shrink the buffer,
        // it would be more efficient to use a single buffer if there is no cross-glyph interference.
        // Otherwise, a good alternative is to pre-allocate the second buffer
        // to be exactly the size of the first buffer in case where we know the size won't change.
        if (base != NULL)  {
            // subtable requested is loaded
            ResetLinearAllocator(&la);
            hz_buffer_compute_info(b1, face);
            // reserve second buffer with size of first buffer as the result of the substitution is likely going to be
            // around the size of the first buffer in most cases.
            hz_buffer_reserve(b2, hz_vector_size(b1->glyph_indices));

            hz_range_list_t *range_list = hz_compute_range_list(b1, table->lookup_flag);

            switch (table->lookup_type) {
                case HZ_GSUB_LOOKUP_TYPE_SINGLE_SUBSTITUTION: {
                    switch (base->format) {
                        case 1: {
                            hz_single_substitution_format1_t *subtable = (hz_single_substitution_format1_t *)base;

                            for (size_t r = 0; r < hz_vector_size(range_list->ranges); ++r) {
                                const hz_range_t *range = &range_list->ranges[r];

                                if (range->is_ignored) {
                                    hz_buffer_add_range(b2, b1, range->mn, range->mx);
                                } else {
                                    // unignored
                                    for (size_t v = range->mn; v <= range->mx; ++v) {
                                        if (hz_should_apply_replacement(b1, feature, v, table->lookup_flag)
                                        && hz_map_value_exists(subtable->coverage, b1->glyph_indices[v])) {
                                            hz_buffer_add_glyph(b2, (hz_buffer_glyph_t){
                                                .GID = b1->glyph_indices[v] + subtable->delta_glyph_id,
                                                .codepoint = b1->codepoints[v]});
                                        } else {
                                            hz_buffer_add_glyph(b2, (hz_buffer_glyph_t){
                                                .GID = b1->glyph_indices[v],
                                                .codepoint = b1->codepoints[v]});
                                        }
                                    }
                                }
                            }
                            break;
                        }
                        case 2: {
                            hz_single_substitution_format2_t *subtable = (hz_single_substitution_format2_t *)base;

                            for (size_t r = 0; r < hz_vector_size(range_list->ranges); ++r) {
                                const hz_range_t *range = &range_list->ranges[r];

                                if (range->is_ignored) {
                                    hz_buffer_add_range(b2, b1, range->mn, range->mx);
                                } else {
                                    // unignored
                                    for (size_t v = range->mn; v <= range->mx; ++v) {
                                        if (hz_should_apply_replacement(b1, feature, v, table->lookup_flag)
                                        && hz_map_value_exists(subtable->coverage, b1->glyph_indices[v])) {
                                            uint16_t index = hz_map_get_value(subtable->coverage, b1->glyph_indices[v]);
                                            hz_buffer_add_glyph(b2, (hz_buffer_glyph_t){
                                                .GID = subtable->substitute_glyph_ids[index],
                                                .codepoint = b1->codepoints[v]});
                                        } else {
                                            hz_buffer_add_glyph(b2, (hz_buffer_glyph_t){
                                                .GID = b1->glyph_indices[v],
                                                .codepoint = b1->codepoints[v]});
                                        }
                                    }
                                }
                            }
                            break;
                        }
                    }
                    break;
                }

                case HZ_GSUB_LOOKUP_TYPE_MULTIPLE_SUBSTITUTION: {
                    switch (base->format) {
                        case 1: {
                            hz_multiple_substitution_format1_subtable_t *subtable = (hz_multiple_substitution_format1_subtable_t*) base;

                            for (size_t r = 0; r < hz_vector_size(range_list->ranges); ++r) {
                                const hz_range_t *range = &range_list->ranges[r];
                                size_t span = (range->mx-range->mn)+1;

                                if (range->is_ignored) {
                                    hz_buffer_add_range(b2,b1,range->mn,range->mx);
                                } else {
                                    // unignored
                                    for (size_t v = range->mn; v <= range->mx; ++v) {
                                        if (hz_should_apply_replacement(b1, feature, v, table->lookup_flag)
                                        && hz_map_value_exists(subtable->coverage, b1->glyph_indices[v])) {
                                            uint32_t index = hz_map_get_value(subtable->coverage, b1->glyph_indices[v]);
                                            const hz_sequence_table_t *sequence = &subtable->sequences[index];

                                            for (uint16_t w = 0; w < sequence->glyph_count; ++w) {
                                                hz_buffer_add_glyph(b2, (hz_buffer_glyph_t){
                                                    .GID = sequence->glyphs[w],
                                                    .codepoint = b1->codepoints[v]});
                                            }
                                        } else {
                                            hz_buffer_add_glyph(b2, (hz_buffer_glyph_t){
                                                .GID = b1->glyph_indices[v],
                                                .codepoint = b1->codepoints[v]});
                                        }
                                    }
                                }
                            }

                            break;
                        }
                    }
                    break;
                }

                case HZ_GSUB_LOOKUP_TYPE_LIGATURE_SUBSTITUTION: {
                    switch (base->format) {
                        case 1: {
                            hz_ligature_substitution_format1_subtable_t *subtable = (hz_ligature_substitution_format1_subtable_t*) base;
                            // go over unignored index list
                            for (size_t r = 0; r < hz_vector_size(range_list->ranges); ++r) {
                                const hz_range_t *range = &range_list->ranges[r];

                                if (range->is_ignored) {
                                    hz_buffer_add_range(b2,b1,range->mn,range->mx);
                                } else {
                                    // unignored
                                    for (short v = range->mn; v <= range->mx; ++v) {
                                        hz_bool matched = HZ_FALSE;

                                        if (hz_should_apply_replacement(b1, feature, v, table->lookup_flag)
                                        && hz_map_value_exists(subtable->coverage, b1->glyph_indices[v])) {
                                            uint16_t index = hz_map_get_value(subtable->coverage, b1->glyph_indices[v]);
                                            const hz_ligature_set_table_t *ligature_set = subtable->ligature_sets + index;

                                            // compare ligatures in ligature set to following unignored nodes for match
                                            for (uint16_t w = 0; w < ligature_set->ligature_count; ++w) {
                                                hz_ligature_t *ligature = &ligature_set->ligatures[w];
                                                uint16_t component_count = ligature->component_count;
                                                short s1 = range->base + (v - range->mn);
                                                short s2 = s1 + component_count - 1;
                                                if (s2 < hz_vector_size(range_list->unignored_indices)) {
                                                    int test = 1;
                                                    if (component_count >= 2) {
                                                        // There are enough unignored glyphs until the end of the buffer
                                                        // to load component glyphs. This could be possibly optimized later with
                                                        // SSE/AVX2 (gather,cmp,shuffle)
                                                        ResetLinearAllocator(&la);
                                                        hz_index_t *block = LinearAlloc(&la, (component_count-1)*2);
                                                        for (uint16_t k = 0; k < component_count-1; ++k) {
                                                            block[k] = b1->glyph_indices[range_list->unignored_indices[s1+k+1]];
                                                        }

                                                        if (ligature->ligature_glyph == 738) {
                                                            for (int z = 0; z < component_count-1; ++z) {
                                                                printf("{%d,%d} ",block[z],ligature->component_glyph_ids[z]);
                                                            }
                                                            printf("\n\n");
                                                        }

                                                        test = !memcmp(ligature->component_glyph_ids, block, (component_count-1)*2);
                                                    }

                                                    if (test) {
                                                        // GID match found with ligature, push ligature glyph to buffer
                                                        //hz_vector_push(b2->glyph_indices, ligature->ligature_glyph);
                                                        hz_buffer_add_glyph(b2, (hz_buffer_glyph_t){
                                                            .GID = ligature->ligature_glyph,
                                                            .codepoint = 0});

                                                        // Push ignored glyphs found within the matched range

                                                        // Jump over context
                                                        v = range_list->unignored_indices[s2];
                                                        r = range_list_find_range(range_list, v);
                                                        range = &range_list->ranges[r];

                                                        matched = HZ_TRUE;
                                                        goto match_check;
                                                    }
                                                }
                                            }
                                        }

                                        match_check:
                                        if (!matched) {
                                            hz_buffer_add_glyph(b2, (hz_buffer_glyph_t){
                                                .GID = b1->glyph_indices[v],
                                                .codepoint = b1->codepoints[v]});
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

                case HZ_GSUB_LOOKUP_TYPE_CHAINED_CONTEXTS_SUBSTITUTION: {
                    switch (base->format) {
                        case 1: {
                            hz_chained_sequence_context_format1_subtable_t *subtable = (hz_chained_sequence_context_format1_subtable_t *)base;

                            for (size_t r = 0; r < hz_vector_size(range_list->ranges); ++r) {
                                const hz_range_t *range = &range_list->ranges[r];

                                if (range->is_ignored) {
                                    hz_buffer_add_range(b2,b1,range->mn,range->mx);
                                } else {
                                    for (short v = range->mn; v <= range->mx; ++v) {
                                        hz_bool match = HZ_FALSE;
                                        if (hz_should_apply_replacement(b1, feature, v, table->lookup_flag)
                                        && hz_map_value_exists(subtable->coverage, b1->glyph_indices[v]))  {
                                            for (uint16_t m = 0; m < subtable->rule_set_count; ++m) {
                                                hz_chained_sequence_rule_set_t *rs = &subtable->rule_sets[m];
                                                for (uint16_t n = 0; n < rs->count; ++n) {
                                                    // fill both context and sequence buffers, use memcmp to quickly check if they are matching
                                                    // context
                                                    hz_chained_sequence_rule_t *rule = &rs->rules[n];
                                                    int u = range->base + (v - range->mn);
                                                    int u1 = u - rule->prefix_count;
                                                    int u2 = (u + rule->input_count + rule->suffix_count) - 1;

                                                    if (u1 >= 0 && u2 <= hz_vector_size(range_list->unignored_indices)-1 && u2 >= u1) {
                                                        int context_len = (u2-u1)+1;
                                                        uint16_t *sequence = LinearAlloc(&la, context_len*2);
                                                        uint16_t *context = LinearAlloc(&la, context_len*2);
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
                                                            int context_low = range_list->unignored_indices[u];
                                                            int context_high = range_list->unignored_indices[u+rule->input_count];

                                                            // create context from input glyphs
                                                            hz_buffer_t *ctx1 = hz_buffer_copy_range(b1, context_low, context_high);
                                                            hz_buffer_t *ctx2 = hz_buffer_create();

                                                            for (uint16_t z = 0; z < rule->lookup_count; ++z) {
                                                                int *context_index_list = hz_buffer_get_unignored_indices(ctx1, table->lookup_flag);
                                                                int sequence_idx = context_index_list[rule->lookup_records[z].sequence_index];

                                                                // recurse :^)
                                                                hz_shape_plan_apply_gsub_lookup(plan, feature,
                                                                                                rule->lookup_records[z].lookup_list_index,
                                                                                                ctx1, ctx2,
                                                                                                sequence_idx, sequence_idx, depth+1);

                                                                hz_swap_buffers(&ctx1, &ctx2, face);
                                                                hz_vector_destroy(context_index_list);
                                                            }

                                                            // add final result to b2
                                                            hz_buffer_add_range(b2, ctx1, 0, (int)hz_vector_size(ctx1->glyph_indices)-1);

                                                            match = HZ_TRUE;
                                                            // skip over input context
                                                            int skip_loc = u + rule->input_count;
                                                            v = range_list->unignored_indices[skip_loc];
                                                            r = range_list_find_range(range_list, v);
                                                            range = &range_list->ranges[r];

                                                            hz_buffer_destroy(ctx1);
                                                            hz_buffer_destroy(ctx2);
                                                            goto check_match;
                                                        }
                                                    }
                                                }
                                            }
                                        }

                                        check_match:
                                        if (!match) {
                                            hz_buffer_add_glyph(b2, (hz_buffer_glyph_t){
                                                .GID = b1->glyph_indices[v],
                                                .codepoint = b1->codepoints[v]});
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

                            if (lookup_index == 50 || lookup_index == 48) {
                                for (int t = 0; t < hz_vector_size(range_list->unignored_indices) - 1; ++t) {
                                    int g1 = range_list->unignored_indices[t];
                                    int g2 = range_list->unignored_indices[t+1];
                                    if (b1->glyph_indices[g1] == 533 && b1->glyph_indices[g2] == 292) {
                                        printf("LAM %d %d \n",g1,g2);
                                    }
                                }
                            }


                            for (size_t r = 0; r < hz_vector_size(range_list->ranges); ++r) {
                                const hz_range_t *range = &range_list->ranges[r];

                                if (range->is_ignored) {
                                    hz_buffer_add_range(b2,b1,range->mn,range->mx);
                                } else {
                                    // unignored
                                    for (short v = range->mn; v <= range->mx; ++v) {
                                        hz_bool match = HZ_FALSE;
                                        if (hz_should_apply_replacement(b1, feature, v, table->lookup_flag)) {
                                            // context bounds check, if this doesn't fit inside the original range
                                            // this context is impossible to match
                                            int u = range->base + (v - range->mn);
                                            int u1 = u - subtable->prefix_count;
                                            int u2 = (u + subtable->input_count + subtable->suffix_count)-1;

                                            if (u1 >= 0 && u2 <= hz_vector_size(range_list->unignored_indices)-1) {
                                                // check prefix, input and suffix sequences for match

                                                int prefix_match = 1, input_match = 1, suffix_match = 1; // matches

                                                // prefix (reverse order according to spec.)
                                                for (int k = 0; k < subtable->prefix_count; ++k) {
                                                    if (!hz_map_value_exists(subtable->prefix_maps[k],
                                                                             b1->glyph_indices[range_list->unignored_indices[u-(k+1)]])) {
                                                        prefix_match = 0;
                                                        break;
                                                    }
                                                }

                                                // input
                                                for (int k = 0; k < subtable->input_count; ++k) {
                                                    if (!hz_map_value_exists(subtable->input_maps[k],
                                                                             b1->glyph_indices[range_list->unignored_indices[u+k]])) {
                                                        input_match = 0;
                                                        break;
                                                    }
                                                }

                                                // suffix
                                                for (int k = 0; k < subtable->suffix_count; ++k) {
                                                    if (!hz_map_value_exists(subtable->suffix_maps[k],
                                                                             b1->glyph_indices[range_list->unignored_indices[u+subtable->input_count+k]])) {
                                                        suffix_match = 0;
                                                        break;
                                                    }
                                                }

                                                if (input_match && suffix_match && prefix_match) {
                                                    // if match, apply nested lookups
//                                                    printf("TEST!\n");
//                                                    printf("PREFIX: %d INPUT: %d SUFFIX: %d\n", prefix_match, input_match, suffix_match);

                                                    int context_low = range_list->unignored_indices[u];
                                                    int context_high = range_list->unignored_indices[u+subtable->input_count-1];

                                                    // create context from input glyphs
                                                    hz_buffer_t *ctx1 = hz_buffer_copy_range(b1, context_low, context_high);
                                                    hz_buffer_t *ctx2 = hz_buffer_create();

                                                    for (uint16_t z = 0; z < subtable->lookup_count; ++z) {
                                                        hz_buffer_compute_info(ctx1, face);
                                                        hz_vector(int) context_index_list = hz_buffer_get_unignored_indices(ctx1, table->lookup_flag);
                                                        int sequence_idx = context_index_list[subtable->lookup_records[z].sequence_index];
                                                        hz_vector_destroy(context_index_list);

                                                        // recurse :^)
                                                        hz_shape_plan_apply_gsub_lookup(plan, feature,
                                                                                        subtable->lookup_records[z].lookup_list_index,
                                                                                        ctx1, ctx2,
                                                                                        sequence_idx, sequence_idx, depth+1);

                                                        hz_swap_buffers(&ctx1, &ctx2, face);
                                                    }
                                                    printf("CONTEXT {v: %d, low: %d, high: %d}\n\n", v,context_low, context_high);

                                                    // add final result to b2
                                                    hz_buffer_add_other(b2, ctx1);

                                                    match = HZ_TRUE;

                                                    int skip_loc = u + subtable->input_count - 1;
                                                    v = range_list->unignored_indices[skip_loc];
                                                    r = range_list_find_range(range_list, v);
                                                    range = &range_list->ranges[r];

                                                    hz_buffer_destroy(ctx1);
                                                    hz_buffer_destroy(ctx2);
                                                    goto gsub_6_3_match;
                                                }
                                            }
                                        }

                                        gsub_6_3_match:
                                        if (!match) {
                                            hz_buffer_add_glyph(b2, (hz_buffer_glyph_t){
                                                .GID = b1->glyph_indices[v],
                                                .codepoint = b1->codepoints[v]});
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

                default: {
                    hz_buffer_clear(b2);
                    hz_buffer_add_range(b2, b1, 0, (int) hz_vector_size(b1->glyph_indices) - 1);
                    break;
                }
            }

            // move glyphs from source buffer to destination buffer
            hz_swap_buffers(&b1, &b2, face);
            hz_range_list_destroy(range_list);
        }
    }


//    printf("lookup_type: %d, size of b1: %llu\n", table->lookup_type, hz_vector_size(b1->glyph_indices));
    // write slices caused by range into the output buffer
    {
        // temporary code
        hz_buffer_add_range(out, in, 0, x1 - 1);
        hz_buffer_add_other(out, b1);
        hz_buffer_add_range(out, in, x2 + 1, (int) hz_vector_size(in->glyph_indices) - 1);
    }

    // cleanup buffers
    hz_buffer_destroy(b1);
    hz_buffer_destroy(b2);
}

HZ_STATIC void
hz_shape_plan_apply_features(hz_shape_plan_t *plan, hz_segment_t *seg)
{
    hz_ot_gsub_table_t *gsub = &plan->gsub_table;
    hz_face_t *face = hz_font_get_face(plan->font);

    for (uint32_t i = 0; i < plan->num_features; ++i) {
        int32_t feature_index;
        hz_feature_t feature = plan->features[i];

        const hz_feature_info_t *feature_info = hz_ot_get_feature_info(feature);

        // TODO: improve search speed with a hash table
        for (uint32_t j = 0; j < plan->gsub_table.num_features; ++j) {
            if (gsub->features[j].tag == feature_info->tag) {
                // Found feature, apply corresponding lookups
                hz_feature_table_t *feature_table = &gsub->features[j].table;
                for (uint16_t k = 0; k < feature_table->lookup_index_count; ++k) {
                    hz_shape_plan_apply_gsub_lookup(plan, feature,
                                                    feature_table->lookup_list_indices[k],
                                                    seg->in, seg->out,
                                                    0,hz_vector_size(seg->in->glyph_indices)-1,
                                                    0);

                    hz_segment_swap_buffers(seg, face);
                }

                break;
            }
        }

    }
}

HZ_STATIC void
hz_shape_plan_destroy(hz_shape_plan_t *plan)
{
    if (plan->num_features > 0) {
        Free(plan->features);
        plan->num_features = 0;
    }

    hz_shape_plan_unload_gsub_table(plan);
    Free(plan);
}

HZ_STATIC void
hz_segment_setup_metrics(hz_segment_t *seg, hz_face_t *face)
{
    if (seg->in != NULL) {
        size_t size = hz_vector_size(seg->in->glyph_indices);
        seg->glyph_metrics = Alloc(size * sizeof(hz_glyph_metrics_t));
        for (size_t i = 0; i < size; ++i) {
            // Marks should not have advance, but this is a hack
            if (seg->in->glyph_classes[i] & HZ_GLYPH_CLASS_MARK) {
                seg->glyph_metrics[i].x_advance = 0;
                seg->glyph_metrics[i].y_advance = 0;
            } else {
                hz_index_t glyph_index = seg->in->glyph_indices[i];
                seg->glyph_metrics[i].x_advance = face->metrics[glyph_index].x_advance;
                seg->glyph_metrics[i].y_advance = face->metrics[glyph_index].y_advance;
            }

            seg->glyph_metrics[i].x_offset = 0;
            seg->glyph_metrics[i].y_offset = 0;
        }
    }

}

HZ_STATIC void
hz_shape_plan_execute(hz_shape_plan_t *plan,
                      hz_segment_t *seg)
{
    hz_face_t *face = hz_font_get_face(plan->font);

    if (seg->num_codepoints > 0) {
        // if codepoints buffer exist, then setup shaping objects and apply features
        if (plan->features == NULL) {
            // no feature list explicitly specified, load standard features for script
            hz_ot_script_load_features(seg->script, &plan->features, &plan->num_features);
        }

        hz_segment_setup_shaping_objects(seg, face);
        hz_shape_plan_apply_features(plan, seg);

        if (seg->direction == HZ_DIRECTION_RTL || seg->direction == HZ_DIRECTION_BTT) {
            hz_buffer_flip_direction(seg->in);
        }

        seg->in->glyph_count = hz_vector_size(seg->in->glyph_indices);

//        hz_segment_setup_metrics(seg, face);
    }
}

void
hz_shape(hz_font_t *font,
         hz_segment_t *seg,
         const hz_feature_t *features,
         unsigned int num_features)
{
    hz_shape_plan_t *plan = hz_shape_plan_create(font, seg, features, num_features);
    hz_shape_plan_execute(plan, seg);
    hz_shape_plan_destroy(plan);
}


HZ_STATIC const hz_language_map_t *
hz_get_language_map(hz_language_t lang) {
    size_t i;
    for (i = 0; i < HZ_ARRLEN(language_map_list); ++i) {
        if (language_map_list[i].language == lang) {
            return &language_map_list[i];
        }
    }
    return NULL;
}

hz_language_t
hz_lang(const char *tag) {
    const hz_language_map_t *currlang, *foundlang;
    size_t i, n;
    size_t len;
    const char *p;
    char code[3]; /* expects only 3 or 2 char codes */
    foundlang = NULL;
    len = strlen(tag);

#if HZ_LANG_USE_ISO_639_1_CODES
    /* use old ISO 639-1 codes (same as HarfBuzz) */
#else
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
#endif
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
    state->chunk = Alloc(chunksize);
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
    Free(state->chunk);
}

HZ_STATIC void
utf8_chunk_decoder_load_data(utf8_chunk_decoder_t *state, const char8_t *data, size_t size)
{
    state->data = data;
    state->size = size;

    if (state->flags & UTF_END_AT_NULL)
        state->size = strlen(state->data);
}

HZ_STATIC void
decode_utf8_byte(utf8_chunk_decoder_t *state)
{
    char8_t byte;

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
hz_segment_load_utf8_simd(hz_segment_t *seg, const char8_t *text)
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
        seg->codepoints = Realloc(seg->codepoints, newsize);
        memcpy(seg->codepoints + seg->num_codepoints, state.chunk, copysize);
        seg->num_codepoints += state.readchars;
    } while (ret != UTF_END_OF_BUFFER);

    utf8_chunk_decoder_deinit(&state);
}

void
hz_segment_load_utf32(hz_segment_t *seg, const uint32_t *text)
{
}

void
hz_segment_load_utf8(hz_segment_t *seg, const char *text)
{
    hz_segment_clear(seg);
    hz_segment_load_utf8_simd(seg, text);
}

void
hz_segment_load_latin1(hz_segment_t *seg, const char *text)
{
    hz_segment_clear(seg);

    size_t len = strlen(text);
    seg->num_codepoints = len;
    seg->codepoints = Alloc(len * sizeof(hz_unicode_t));

    for (size_t i = 0; i < len; ++i) {
        seg->codepoints[i] = (hz_unicode_t) text[i];
    }
}
