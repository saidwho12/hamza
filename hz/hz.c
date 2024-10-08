#include "hz.h"

#if HZ_COMPILER & (HZ_COMPILER_CLANG | HZ_COMPILER_GCC)
#    define HZ_PACK(__Declaration__) __Declaration__ __attribute__((__packed__))
#elif HZ_COMPILER & HZ_COMPILER_VC
#    define HZ_PACK(__Declaration__) __pragma(pack(push,1)) __Declaration__ __pragma(pack(pop))
#endif 

#define ArraySize(A) (sizeof(A)/sizeof(A[0]))

// define some helper typedefs to help shorten our code
typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
typedef int64_t s64;
typedef size_t sz;

#define IS_BIG_ENDIAN (!*(u8*)&(u16){1})

HZ_ALWAYS_INLINE void hz_memset(void *data, int val, size_t size) {
    memset(data,val,size);
}

HZ_ALWAYS_INLINE void hz_memcpy(void *dst, const void *src, size_t size) {
    memcpy(dst,src,size);
}

#ifndef HZ_MEMSET
#  define HZ_MEMSET hz_memset
#endif

#define HZ_MEMCPY hz_memcpy

#define hz_zero(_Data, _Size) HZ_MEMSET(_Data,0,_Size)
#define hz_zero_struct(_Struct) HZ_MEMSET((void *)&(_Struct),0,sizeof(_Struct))
#define HZ_PRIVATE static
#define Offset16 hz_uint16
#define Offset32 hz_uint32
#define Version16Dot16 hz_uint32

#include <assert.h>
#include <stdarg.h>

#define SIZEOF_VOIDPTR sizeof(void*)

#define KIB 1024

#define UTF_FAILED (-1)
#define UTF_END_OF_BUFFER (-2)
#define UTF_OK 1

#include "hzconfig.h"

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

#if HZ_COMPILER & (HZ_COMPILER_CLANG | HZ_COMPILER_GCC)
#define hz_unreachable() __builtin_unreachable()
#define hz_likely(x) __builtin_expect(!!(x), 1)
#define hz_unlikely(x) __builtin_expect(!!(x), 0)
#else
#define hz_unreachable() __assume(0)
#define hz_likely(x) (x)
#define hz_unlikely(x) (x)
#endif



typedef int16_t hz_segment_sz_t;

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

#define UTF_CACHE_LINE_SIZE 128
#define UTF_TINY_CHUNK_SIZE (1 * KIB)
#define UTF_SMALL_CHUNK_SIZE (4 * KIB)
#define UTF_MEDIUM_CHUNK_SIZE (16 * KIB)
#define UTF_LARGE_CHUNK_SIZE (64 * KIB)

typedef uint8_t char8_t;
typedef uint32_t char32_t;

typedef hz_uint16 F2DOT14, FWORD, UFWORD;

#define DEBUG_MSG(msg) fprintf(stdout,"[DEBUG:%s:%d]: %s\n",__FILE__,__LINE__,msg)
#define ERROR_MSG(msg) fprintf(stderr,"[ERROR: %s:%d]: %s\n",__FILE__,__LINE__,msg)
#define QUIT_MSG(msg) { ERROR_MSG(msg); exit(-1); }
#define ASSERT_MSG(cond, msg) assert((cond) && message)

typedef enum {
    HZ_LOG_INFO,
    HZ_LOG_DEBUG,
    HZ_LOG_WARNING,
    HZ_LOG_ERROR,
    HZ_LOG_FATAL
} hz_severity_t;

static FILE *_logfile = NULL;

void hz_flog(hz_severity_t severity, const char *filename, const char *func, int linenum, const char *fmt, ...)
{
    va_list ap;
    va_start(ap,fmt);
    FILE *output = stdout;
    if (_logfile != NULL) {
        output = _logfile;
    }

    const char *severity_titles[] = { "INFO", "DEBUG", "WARNING", "ERROR", "FATAL ERROR"};
    fprintf(output, "[%s][%s:%d <%s>] ",severity_titles[severity], filename, linenum, func);
    vfprintf(output, fmt, ap);
    fflush(output);

    va_end(ap);

    if (severity == HZ_LOG_FATAL) {
        exit(-1);
    }
}

#ifdef HZ_DEBUG_LOGGING
#define hz_logf(Severity, ...) hz_flog(Severity, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define hz_logln(Severity,Msg) hz_flog(Severity,__FILE__, __FUNCTION__, __LINE__,"%s\n",Msg)
#else
#define hz_logf(Severity, ...) 
#define hz_logln(Severity,Msg)
#endif

struct hz_lib_t {
    hz_config_t cfg;
    hz_bool is_already_initialized;
    hz_allocator_t allocator;
};

static struct hz_lib_t hz_;

void *hz_aligned_alloc(hz_allocator_t *alctr, size_t size, size_t alignment)
{
    return alctr->allocfn(alctr->user, HZ_CMD_ALLOC, NULL, size, alignment);
}

void *hz_allocate(hz_allocator_t *alctr, size_t size)
{
    return alctr->allocfn(alctr->user, HZ_CMD_ALLOC, NULL, size, DEFAULT_ALIGNMENT);
}

void hz_deallocate(hz_allocator_t *alctr, void *ptr)
{
    alctr->allocfn(alctr->user, HZ_CMD_FREE, ptr, 0, 0);
}

void *hz_reallocate(hz_allocator_t *alctr, void *ptr, size_t size)
{
    return alctr->allocfn(alctr->user, HZ_CMD_REALLOC, ptr, size, DEFAULT_ALIGNMENT);
}

void hz_release(hz_allocator_t *alctr)
{
    alctr->allocfn(alctr->user, HZ_CMD_RELEASE, NULL, 0, 0);
}

//////////////////////////////// main allocator ////////////////////////////////////////
void *hz_standard_c_allocator_fn(void *user, hz_allocator_cmd_t cmd, void *ptr, size_t size, size_t align)
{
    HZ_IGNORE_ARG(user); HZ_IGNORE_ARG(align);
    
    switch (cmd) {
        case HZ_CMD_ALLOC:
            return malloc(size);
        case HZ_CMD_REALLOC: // no-op
            return realloc(ptr, size);
        case HZ_CMD_FREE:
            free(ptr);
        case HZ_CMD_RELEASE:
        default: // error, cmd not handled
            return NULL;
    }
}

HZ_ALWAYS_INLINE uint32_t hz_hash32_fnv1a(uint32_t k) {
    uint32_t h = 0x811c9dc5ul; // offset basis

    h = 0x01000193 * (h ^ (k & 255)); // multiply by prime
    k >>= 8;
    h = 0x01000193 * (h ^ (k & 255));
    k >>= 8;
    h = 0x01000193 * (h ^ (k & 255));
    k >>= 8;
    h = 0x01000193 * (h ^ (k & 255));
    k >>= 8;

    return h;
}

HZ_ALWAYS_INLINE uint64_t hz_hash64_fnv1a(uint64_t k) {
    uint64_t h = 0xcbf29ce484222325ull; // offset basis
    h = 0x00000100000001b3 * (h ^ (k & 255)); // multiply by prime
    k >>= 8;
    h = 0x00000100000001b3 * (h ^ (k & 255));
    k >>= 8;
    h = 0x00000100000001b3 * (h ^ (k & 255));
    k >>= 8;
    h = 0x00000100000001b3 * (h ^ (k & 255));
    k >>= 8;
    h = 0x00000100000001b3 * (h ^ (k & 255));
    k >>= 8;
    h = 0x00000100000001b3 * (h ^ (k & 255));
    k >>= 8;
    h = 0x00000100000001b3 * (h ^ (k & 255));
    k >>= 8;
    h = 0x00000100000001b3 * (h ^ (k & 255));
    k >>= 8;
    return h;
}

uint32_t hz_hash2_lowbias32(uint32_t k1, uint32_t k2) {
    uint32_t h = k1;
    h ^= h >> 16;
    h *= 0x7feb352d;
    h ^= h >> 15;
    h *= 0x846ca68b;
    h ^= h >> 16;

    h ^= k2;
    h ^= h >> 16;
    h *= 0x7feb352d;
    h ^= h >> 15;
    h *= 0x846ca68b;
    h ^= h >> 16;
    return h;
}

typedef struct hz_ht_t {
    uint8_t *flags;
    uint32_t *keys;
    uint32_t *values;
    size_t size;
    size_t num_occupied;
    hz_allocator_t *alctr;
} hz_ht_t;

void hz_ht_clear(hz_ht_t *ht) {
    HZ_MEMSET(ht->flags, HZ_HT_EMPTY, ht->size);
    HZ_MEMSET(ht->keys, 0, ht->size*sizeof(uint32_t));
    HZ_MEMSET(ht->values, 0, ht->size*sizeof(uint32_t));
    ht->num_occupied = 0;
}

void hz_ht_init(hz_ht_t *ht, hz_allocator_t *alctr, size_t size)
{
    ht->alctr = alctr;
    ht->keys = hz_allocate(alctr, size*sizeof(uint32_t));
    ht->values = hz_allocate(alctr,size*sizeof(uint32_t));
    ht->flags = hz_allocate(alctr,size*sizeof(uint8_t));
    ht->num_occupied = 0;
    ht->size = size;
    hz_ht_clear(ht);
}

hz_ht_t *hz_ht_create(hz_allocator_t *alctr, size_t size){
    hz_ht_t *ht = hz_allocate(alctr, sizeof(*ht));
    hz_ht_init(ht,alctr,size);
    return ht;
}

void hz_ht_destroy(hz_ht_t *ht)
{
    hz_deallocate(ht->alctr,ht->flags);
    hz_deallocate(ht->alctr,ht->keys);
    hz_deallocate(ht->alctr,ht->values);
}

uint32_t hz_ht_next_valid_index(hz_ht_t *ht, uint32_t index) {
    if (index >= ht->size) return HZ_HT_INVALID_INDEX;

    while ((ht->flags[index] & (HZ_HT_EMPTY|HZ_HT_TOMBSTONE)) && index < ht->size)
        ++index;

    if (index != ht->size)
        return index;

    return HZ_HT_INVALID_INDEX;
}

hz_bool hz_ht_iter_next(hz_ht_t *ht, hz_ht_iter_t *it) {
    it->index = hz_ht_next_valid_index(ht,it->index+1);
    if (it->index != HZ_HT_INVALID_INDEX) {
        it->key = ht->keys[it->index];
        it->ptr_value = &ht->values[it->index];
        return HZ_TRUE;//success
    }

    return HZ_FALSE;
}

hz_ht_iter_t hz_ht_iter_begin(hz_ht_t *ht) {
    hz_ht_iter_t it;
    it.index = hz_ht_next_valid_index(ht,0);
    if (it.index != HZ_HT_INVALID_INDEX) {
        it.key = ht->keys[it.index];
        it.ptr_value = &ht->values[it.index];
    }

    return it;
}

hz_bool hz_ht_search(hz_ht_t *ht, uint32_t key, hz_ht_iter_t *it) {
    uint32_t h = hz_hash32_fnv1a(key) % ht->size;
    for (uint32_t i = 0; i < ht->size && !(ht->flags[h] & HZ_HT_EMPTY); ++i, h = (h + i) % ht->size) {
        if (ht->flags[h] & HZ_HT_TOMBSTONE)
            continue; // skip tombstone

        // bucket is occupied
        if (ht->keys[h] == key) {
            // keys match, set iterator pointers and return successfully
            it->key = ht->keys[h];
            it->ptr_value = &ht->values[h];
            it->index = h;
            return HZ_TRUE;
        }
    }

    return HZ_FALSE; // didn't find item
}

// returns true if insert succeeded, and false if it didn't
hz_bool hz_ht_insert(hz_ht_t *ht, uint32_t key, uint32_t value)
{
    uint32_t h = hz_hash32_fnv1a(key) % ht->size;

    for (uint32_t i = 0; i < ht->size; ++i, h = (h + i) % ht->size) {
        if ((ht->flags[h] & HZ_HT_OCCUPIED) && ht->keys[h] == key) { // entry aready exists, replace with our value
            ht->values[h] = value;
            ++ht->num_occupied;
            return HZ_TRUE;
        }

        if (ht->flags[h] & (HZ_HT_EMPTY | HZ_HT_TOMBSTONE)) { // found empty/tombstone slot, insert value and return
            ht->keys[h] = key;
            ht->values[h] = value;
            ht->flags[h] = HZ_HT_OCCUPIED;
            ++ht->num_occupied;
            return HZ_TRUE;
        }
    }
    
    // insertion failed
    return HZ_FALSE;
}
size_t hz_ht_size(hz_ht_t* ht) { return ht->num_occupied;}
hz_bool hz_ht_remove(hz_ht_t *ht, uint32_t key)
{
    hz_ht_iter_t it;
    if (hz_ht_search(ht, key, &it)) {   
        ht->flags[it.index] = HZ_HT_TOMBSTONE;
        --ht->num_occupied;
        return HZ_TRUE;
    }

    return HZ_FALSE;
}

hz_allocator_t *hz_get_allocator(void)
{
    return &hz_.allocator;
}

void hz_set_allocator_fn(hz_allocator_fn_t allocfn)
{
    hz_.allocator.allocfn = allocfn;
}

void hz_set_allocator_user_data(void *user)
{
    hz_.allocator.user = user;
}

void* hz_malloc(size_t size)
{
    return hz_.allocator.allocfn(hz_.allocator.user,HZ_CMD_ALLOC,NULL,size,1);
}

void* hz_realloc(void* pointer, size_t size)
{
    return hz_.allocator.allocfn(hz_.allocator.user,HZ_CMD_REALLOC,pointer,size,1);
}

void hz_free(void *pointer)
{
    hz_.allocator.allocfn(hz_.allocator.user,HZ_CMD_FREE,pointer,0,1);
}

HZ_ALWAYS_INLINE uint16_t hz_bswap16(uint16_t x)
{
    return (x << 8) | (x >> 8);
}

HZ_ALWAYS_INLINE uint32_t hz_bswap32(uint32_t x)
{
    uint32_t v = 0;
    v |= (x & 0x000000ff) << 24;
    v |= (x & 0x0000ff00) << 8;
    v |= (x & 0x00ff0000) >> 8;
    v |= (x & 0xff000000) >> 24;
    return v;
}

HZ_ALWAYS_INLINE void hz_byte_swap_u16x4(uint64_t *p) {
    uint64_t q = 0;
    q |= (*p & 0xff00ff00ff00ff00) >> 8;
    q |= (*p & 0x00ff00ff00ff00ff) << 8;
    *p = q;
}

HZ_ALWAYS_INLINE void hz_byte_swap_u16x2(u32 *p) {
    *p = ((*p & 0xff00ff00) >> 8) | ((*p & 0x00ff00ff) << 8);
}

HZ_ALWAYS_INLINE void hz_byte_swap_16(uint16_t *p)
{
    *p = (*p << 8) | (*p >> 8);
}

HZ_ALWAYS_INLINE void hz_byte_swap_32(uint32_t *p)
{
    uint32_t q = 0;
    q |= (*p & 0xff000000UL) >> 24;
    q |= (*p & 0x00ff0000UL) >> 8;
    q |= (*p & 0x0000ff00UL) << 8;
    q |= (*p & 0x000000ffUL) << 24; 
    *p = q;
}

HZ_ALWAYS_INLINE void hz_byte_swap_u32x2(uint64_t *p) {
    uint64_t q = 0;
    q |= (*p & 0xff000000ff000000) >> 24;
    q |= (*p & 0x00ff000000ff0000) >> 8;
    q |= (*p & 0x0000ff000000ff00) << 8;
    q |= (*p & 0x000000ff000000ff) << 24;
    *p = q;
}

HZ_ALWAYS_INLINE uint64_t hz_bswap64(uint64_t x)
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
HZ_ALWAYS_INLINE uint64_t hz_qlog2_i64(uint64_t n)
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
    return multiply_de_bruijin_bit_position[((uint64_t)((n - (n >> 1))*0x07edd5e59a4e28c2)) >> 58];
#endif
}

void *hz_memory_arena_alloc_fn(void *user, hz_allocator_cmd_t cmd, void *ptr, size_t size, size_t align) {
    HZ_IGNORE_ARG(ptr);
    hz_memory_arena_t *arena = (hz_memory_arena_t *)user;

    switch (cmd) {
        case HZ_CMD_ALLOC:
            return hz_memory_arena_alloc_aligned(arena, size, align);
        case HZ_CMD_RELEASE:
            hz_memory_arena_release(arena);
        case HZ_CMD_RESET:
            hz_memory_arena_reset(arena);
        case HZ_CMD_FREE: default:
            break;
    }

    return NULL;
}

//////////////////////////// stack allocator //////////////////////////////////
typedef struct {
    uint8_t *mem;
    size_t size;
    uintptr_t pos;
    size_t last_alloc_size;
} hz_stack_allocator_t;

typedef struct {
    uint32_t prev_size;
    uint8_t padding;
} hz_stack_allocator_header_t;

hz_stack_allocator_t hz_stack_create(uint8_t *mem, size_t size)
{
    return (hz_stack_allocator_t){.mem=mem,.size=size,.pos=0};
}

void hz_stack_init(hz_stack_allocator_t *stack, uint8_t *mem, size_t size)
{
    stack->mem = mem;
    stack->size = size;
    stack->pos = 0;
    stack->last_alloc_size = 0;
}

void *hz_stack_alloc_align(hz_stack_allocator_t *stack, size_t size, size_t align) {
    HZ_ASSERT(align > 0);
    HZ_ASSERT(HZ_ISP2(align));

    uintptr_t ptr = (uintptr_t)(stack->mem + stack->pos);
    size_t hdr_size = sizeof(hz_stack_allocator_header_t);
    size_t padd = hdr_size + hz_align_forward(hdr_size + ptr, align);
    uintptr_t start = stack->pos + padd;
    uintptr_t end = start + size;

    if (end > stack->size)
        return NULL; // out of memory error

    hz_stack_allocator_header_t *hdr = (hz_stack_allocator_header_t *)(stack->mem + start - sizeof(*hdr));
    hdr->padding = padd;
    hdr->prev_size = stack->last_alloc_size;

    stack->last_alloc_size = size;
    stack->pos = end;

    return stack->mem + start;
}

void *hz_stack_alloc(hz_stack_allocator_t *stack, size_t size) {
    return hz_stack_alloc_align(stack, size, DEFAULT_ALIGNMENT);
}

void *hz_stack_free(hz_stack_allocator_t *stack, void *ptr) {
    if (!ptr) return NULL;

    uintptr_t start = (uintptr_t)stack->mem, end = (uintptr_t)(start + stack->size);
    uintptr_t curr = (uintptr_t)ptr;

    if (curr >= start + stack->pos)
        return NULL;

    hz_stack_allocator_header_t *hdr = (hz_stack_allocator_header_t *)ptr - 1;
    uintptr_t prev_pos = curr - start - hdr->padding;
    stack->pos = prev_pos;
    stack->last_alloc_size = hdr->prev_size;

    if (hdr->prev_size)
        return (stack->mem + stack->pos) - hdr->prev_size;

    return NULL;
}

void hz_stack_reset(hz_stack_allocator_t *stack) {
    stack->pos = 0;
    stack->last_alloc_size = 0;
}

void hz_stack_release(hz_stack_allocator_t *stack) {}

typedef struct { uint32_t jump, prev_offset; } hz_parser_state_t;

#define HZ_PARSER_STACK_SIZE 1024

typedef struct {
    char *stackmem;
    const uint8_t *mem;
    int must_bswap;
    size_t start;
    size_t offset;
    hz_stack_allocator_t stack;
    hz_parser_state_t *curr_state;
} hz_parser_t;

void hz_parser_init(hz_parser_t *p, const uint8_t *mem) {
    p->mem = mem;
    p->must_bswap = !IS_BIG_ENDIAN;
    p->curr_state = NULL;
    p->offset = 0;
    p->start = 0;
    p->stackmem = hz_malloc(HZ_PARSER_STACK_SIZE);
    hz_stack_init(&p->stack, p->stackmem, HZ_PARSER_STACK_SIZE);
}

void hz_parser_deinit(hz_parser_t *p) {
    hz_free(p->stackmem);
}

hz_parser_t hz_parser_create(const uint8_t *mem) {
    hz_parser_t p;
    hz_parser_init(&p,mem);
    return p;
}

void hz_parser_pop_state(hz_parser_t *p)
{
    p->offset = p->curr_state->prev_offset;
    p->start -= p->curr_state->jump;
    p->curr_state = hz_stack_free(&p->stack, p->curr_state);
}

void hz_parser_push_state(hz_parser_t *p, uint32_t jump)
{   
    p->curr_state = hz_stack_alloc(&p->stack, sizeof(hz_parser_state_t));
    p->curr_state->jump = jump;
    p->curr_state->prev_offset = p->offset;
    p->start += jump;
    p->offset = 0;
}

static inline void hz_parser_advance(hz_parser_t *p, size_t count)
{
    p->offset += count;
}

static inline const uint8_t* hz_parser_at_cursor(hz_parser_t *p)
{
    return p->mem + p->start + p->offset;
}

uint8_t hz_parser_read_u8(hz_parser_t *p)
{
    uint8_t v = *(uint8_t *)hz_parser_at_cursor(p);
    hz_parser_advance(p, 1);
    return v;
}

uint16_t hz_parser_read_u16(hz_parser_t *p)
{
    uint16_t v = *(uint16_t *)hz_parser_at_cursor(p);
    hz_parser_advance(p, 2);
    return p->must_bswap ? hz_bswap16(v) : v;
}

uint32_t hz_parser_read_u32(hz_parser_t *p)
{
    uint32_t v = *(uint32_t *)hz_parser_at_cursor(p);
    hz_parser_advance(p, 4);
    return p->must_bswap ? hz_bswap32(v) : v;
}

uint64_t hz_parser_read_u64(hz_parser_t *p)
{
    uint64_t v = *(uint64_t *)hz_parser_at_cursor(p);
    hz_parser_advance(p, 8);
    return p->must_bswap ? hz_bswap64(v) : v;
}

void hz_parser_read_block(hz_parser_t *p, uint8_t *write_addr, size_t size)
{
    hz_memcpy(write_addr, hz_parser_at_cursor(p), size);
    hz_parser_advance(p, size);
}

void hz_parser_read_u16_block(hz_parser_t *p, uint16_t *write_addr, size_t size)
{
    hz_memcpy(write_addr, hz_parser_at_cursor(p), size*2);
    p->offset += size*2;

    if (p->must_bswap) {
        size_t swap_index = 0;
        while(swap_index+4 <= size) {
            hz_byte_swap_u16x4((uint64_t*)&write_addr[swap_index]);
            swap_index += 4;
        }

        while (swap_index < size) {
            hz_byte_swap_16(write_addr + swap_index);
            ++swap_index;
        }
    }
}

void hz_parser_read_u32_block(hz_parser_t *p, uint32_t *write_addr, size_t size)
{
    hz_memcpy(write_addr, hz_parser_at_cursor(p), size*4);
    p->offset += size*4;

    if (p->must_bswap) {
        size_t swap_index = 0;
        while(swap_index+2 <= size) {
            hz_byte_swap_u32x2((uint64_t*)&write_addr[swap_index]);
            swap_index += 2;
        }

        while (swap_index < size) {
            hz_byte_swap_32(write_addr + swap_index);
            ++swap_index;
        }
    }
}

void hz_parser_read_u64_block(hz_parser_t *p, uint64_t *write_addr, size_t size)
{
    while (size > 0) {
        *write_addr++ = hz_parser_read_u64(p);
        --size;
    }
}

int hz_cmdread(hz_parser_t *p, int c_struct_align, void *dataptr, const char *cmd, ...)
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
                hz_parser_read_block(p, (uint8_t *)mem, member_array_count);
                member_offset += member_array_count;
                member_array_count = 1;
                break;
            }

            case 'w': {
                ++curs;
                member_offset += hz_align_forward(member_offset, c_struct_align ? 2 * member_array_count : 1);
                hz_parser_read_u16_block(p,
                            (uint16_t *)(mem + member_offset),
                            member_array_count);
                member_offset += 2 * member_array_count;
                member_array_count = 1;
                break;
            }

            case 'd': {
                ++curs;
                member_offset += hz_align_forward(member_offset, c_struct_align ? 4 * member_array_count : 1);
                hz_parser_read_u32_block(p,
                            (uint32_t *)(mem + member_offset),
                            member_array_count);
                member_offset += 4 * member_array_count;
                member_array_count = 1;
                break;
            }

            case 'q': {
                ++curs;
                member_offset += hz_align_forward(member_offset, c_struct_align ? 8 * member_array_count : 1);
                hz_parser_read_u64_block(p,
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

hz_bool hz_array_is_empty(const hz_array_t *array)
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
    hz_bool is_empty = hz_array_is_empty(array);
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

#if 0
HZ_STATIC uint32_t hz_map_get_value(hz_map_t *map, uint32_t key)
{
    uint32_t hash = hz_hash_fnv1a(key);
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

HZ_STATIC hz_bool hz_ht_search(const hz_ht_t *ht, hz_ht_iter_t *it, uint32_t key)
{
    uint32_t hash = hz_hash_fnv1a(key);
    size_t index = hash % map->size;

    return HZ_FALSE;
}
#endif

HZ_STATIC hz_bool hz_ht_set_value_for_keys(hz_ht_t *ht, uint32_t k0, uint32_t k1, uint32_t value)
{
    hz_bool any_set = HZ_FALSE;
    uint32_t k = k0;

    while (k <= k1) {
        if (hz_ht_insert(ht, k, value))
            any_set = HZ_TRUE;

        ++k;
    }

    return any_set;
}

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

hz_bool hz_vector_need_grow(void *v, size_t extra)
{
    assert(v != NULL);
    hz_vector_hdr_t *hdr = hz_vector_header(v);
    return hdr->size + extra > hdr->capacity;
}

void hz_buffer_init(hz_buffer_t *buffer)
{
    buffer->glyph_count = 0;
    buffer->glyph_indices = NULL;
    buffer->codepoints = NULL;
    buffer->glyph_classes = NULL;
    buffer->attachment_classes = NULL;
    buffer->component_indices = NULL;
    buffer->glyph_metrics = NULL;
    buffer->attrib_flags = 0;
}

hz_buffer_t *hz_buffer_create(void) {
    hz_buffer_t *buffer = hz_malloc(sizeof(*buffer));
    hz_buffer_init(buffer);
    return buffer;
}

HZ_STATIC void
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

typedef struct {
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

void hz_buffer_release(hz_buffer_t *buffer)
{
    if (buffer->glyph_indices != NULL) {
        hz_vector_destroy(buffer->glyph_indices);
    }
    if (buffer->codepoints != NULL) {
        hz_vector_destroy(buffer->codepoints);
    }
    if (buffer->glyph_classes != NULL) {
        hz_vector_destroy(buffer->glyph_classes);
    }
    if (buffer->glyph_metrics != NULL) {
        hz_vector_destroy(buffer->glyph_metrics);
    }
    if (buffer->component_indices != NULL) {
        hz_vector_destroy(buffer->component_indices);
    }
    if (buffer->attachment_classes != NULL) {
        hz_vector_destroy(buffer->attachment_classes);
    }
    hz_buffer_init(buffer);
}

void hz_buffer_destroy(hz_buffer_t *buffer)
{
    hz_buffer_clear(buffer);
    hz_free(buffer);
}

typedef struct {
    const char8_t      *data;
    hz_unicode_t        codepoint;
    uintptr_t           ptr;
    size_t              size; /* if 0, no limit */
    int                 step;
    uint32_t            flags;
} hz_utf8_decoder_t;

HZ_ALWAYS_INLINE uint32_t hz_mph_table_lookup(uint32_t n, int32_t G[], uint32_t k)
{
    uint32_t h = hz_hash2_lowbias32((uint32_t)k,0)%n;
    int32_t g = G[h]; // sample second param

    // check if g is negative, in which case we don't need to hash again to get the final slot.
    // that's because this bucket had a single key in it.
    return g & 0x80000000 ? (-g)-1 :
            hz_hash2_lowbias32((uint32_t)k, (uint32_t)g)%n;
}

HZ_ALWAYS_INLINE uint32_t hz_ucd_get_arabic_joining_data(hz_unicode_t k) {
    static const size_t N = HZ_ARRAY_SIZE(hz_ucd_arabic_joining_data);
    uint32_t slot = hz_mph_table_lookup(N,hz_ucd_arabic_joining_k2, k);

    // Verify if unicode codepoints match, otherwise return default
    if (hz_ucd_arabic_joining_ucs_codepoints[slot] != k) return HZ_JOINING_GROUP_NONE|HZ_JOINING_TYPE_T;

    return hz_ucd_arabic_joining_data[slot];
}

typedef struct {
    uint16_t *keys;
    uint16_t *g_middle;
    uint16_t *values; 
} hz_ht_coverage_t;


typedef struct {
    uint16_t start_glyph_id;
    uint16_t end_glyph_id;
    union {
        uint16_t start_coverage_index;
        uint16_t glyph_class;
    }; 
} hz_coverage_range_t;

typedef struct {
    union {
        uint16_t* values;
        hz_coverage_range_t* ranges;
    };
    uint16_t format;
    uint16_t count;
} hz_coverage_t;

typedef struct {
    union {
        uint16_t* values;
        hz_coverage_range_t* ranges;
    };
    uint16_t start_glyph_id;
    uint16_t count;
    uint16_t format;
} hz_class_def_t;

HZ_STATIC void hz_read_coverage(hz_memory_arena_t *memory_arena, hz_parser_t *p, hz_coverage_t *cov)
{
    cov->format = hz_parser_read_u16(p);
    cov->count = 0;
    cov->values = NULL;

    switch (cov->format) {
        default: break;
        case 1:
        cov->count = hz_parser_read_u16(p);
        cov->values = hz_memory_arena_alloc(memory_arena, sizeof(uint16_t)*cov->count);
        hz_parser_read_u16_block(p, cov->values, cov->count);
        break;
        case 2:
        cov->count = hz_parser_read_u16(p);
        cov->ranges = hz_memory_arena_alloc(memory_arena, sizeof(hz_coverage_range_t)*cov->count);
        hz_parser_read_u16_block(p, (uint16_t*) cov->ranges, cov->count * 3);
        break;
    }
}

HZ_ALWAYS_INLINE int32_t hz_coverage_scalar_search(uint16_t *array, uint16_t size, uint16_t val)
{
    int32_t low = 0, high = size-1, mid = (low+high)/2;

    if (val < array[low] || val > array[high]) {
        // error, glyph not found within coverage
        return -1;
    }

    // binary search
    while (high >= low) {
        if (val < array[mid]) {
            high = mid-1;
        } else if (val > array[mid]) {
            low = mid+1;
        } else {
            return mid;
        }

        mid = (low+high)/2;
    }

    return -1;
}

HZ_ALWAYS_INLINE int32_t hz_coverage_range_search(hz_coverage_range_t *ranges, uint16_t size, uint16_t val)
{
    int32_t low = 0, high = size-1, mid = (low+high)/2;
    hz_coverage_range_t *low_range = &ranges[low];
    hz_coverage_range_t *high_range = &ranges[high];
    hz_coverage_range_t *mid_range = &ranges[mid];

    if (val < low_range->start_glyph_id || val > high_range->end_glyph_id) {
        // early return as glyph cannot possibly be in this coverage.
        return -1;
    }

    while (high >= low) {
        low_range = &ranges[low];
        high_range = &ranges[high];
        mid_range = &ranges[mid];

        if (val < mid_range->start_glyph_id) {
            high = mid-1;
        } else if (val > mid_range->end_glyph_id) {
            low = mid+1;
        } else {
            return mid;
        }

        mid = (low+high)/2;
    }

    return -1;
}

HZ_ALWAYS_INLINE int32_t
hz_coverage_search(const hz_coverage_t *coverage, uint16_t glyph_id)
{
    switch (coverage->format) {
        default: break;
        case 1:
        return hz_coverage_scalar_search(coverage->values, coverage->count, glyph_id);
        case 2: {
            int32_t index = hz_coverage_range_search(coverage->ranges, coverage->count, glyph_id);
            if (index != -1) {
                hz_coverage_range_t const *range = &coverage->ranges[index];
                index = range->start_coverage_index + glyph_id - range->start_glyph_id;
            }
            return index;
        }
    }
}

#define hz_coverage_contains(c,g) (hz_coverage_search(c,g) != -1)

HZ_ALWAYS_INLINE int32_t hz_class_def_search(hz_class_def_t *class_def, uint16_t glyph_id) {
    switch (class_def->format) {
        default: break;
        case 1: {
            if (glyph_id >= class_def->start_glyph_id && glyph_id < class_def->start_glyph_id + class_def->count)
                return class_def->values[glyph_id - class_def->start_glyph_id];
            
            break;
        }
        
        case 2: {
            int32_t index = hz_coverage_range_search(class_def->ranges, class_def->count, glyph_id);
            if (index != -1) {
                const hz_coverage_range_t *range = &class_def->ranges[index];
                return range->glyph_class;
            }
            
            break;
        }
    }

    return -1;
}

#define hz_class_def_contains(c,g) (hz_class_def_search(c,g) != -1)

HZ_STATIC hz_error_t hz_read_class_def_table(hz_memory_arena_t *memory_arena, hz_parser_t *p, hz_class_def_t *class_def) {
    class_def->format = hz_parser_read_u16(p);
    switch (class_def->format) {
        default: break;
        case 1: {
            class_def->start_glyph_id = hz_parser_read_u16(p);
            class_def->count = hz_parser_read_u16(p);
            class_def->values = hz_memory_arena_alloc(memory_arena, sizeof(uint16_t)*class_def->count);
            hz_parser_read_u16_block(p, class_def->values, class_def->count);
            break;
        }

        case 2: {
            class_def->count = hz_parser_read_u16(p);
            class_def->ranges = hz_memory_arena_alloc(memory_arena, sizeof(hz_coverage_range_t)*class_def->count);
            hz_parser_read_u16_block(p, (uint16_t*)class_def->ranges, 3*class_def->count);
            break;
        }
    }

    return HZ_OK;
}

typedef enum hz_joining_dir_t {
    JOINING_NEXT,
    JOINING_PREV
} hz_joining_dir_t;

struct hz_face_t {
    stbtt_fontinfo *fontinfo;
    unsigned char *data;
    unsigned int gpos,gsub,gdef,jstf,cmap,maxp,glyf,hmtx,kern,hhea;

    uint16_t num_glyphs;
    uint16_t num_of_h_metrics;
    uint16_t num_of_v_metrics;
    hz_metrics_t *metrics;

    int16_t ascender;
    int16_t descender;
    int16_t linegap;
    int16_t fheight;

    uint16_t upem;

    uint8_t *arenamem;
    hz_memory_arena_t memory_arena;
    hz_class_def_t class_def;
    hz_class_def_t attach_class_def;
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

    face->arenamem = hz_malloc(500000);
    hz_memory_arena_init(&face->memory_arena, face->arenamem, 500000);
    face->mark_glyph_set = NULL;
    return face;
}

void
hz_face_destroy(hz_face_t *face)
{
    hz_memory_arena_release(&face->memory_arena);
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
    hz_parser_t p = hz_parser_create(face->data + face->maxp);

    Version16Dot16 version;
    uint16_t num_glyphs;

    version = hz_parser_read_u16(&p);

    switch (version) {
        case 0x00005000: {
            /* version 0.5 */
            num_glyphs = hz_parser_read_u16(&p);
            break;
        }
        case 0x00010000: {
            /* version 1.0 with full information */
            num_glyphs = hz_parser_read_u16(&p);
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
    uint8_t tmp_buffer[8000];
    hz_memory_arena_t tmp_arena = hz_memory_arena_create(tmp_buffer, sizeof tmp_buffer);

    if (face->gdef) {
        hz_parser_t p = hz_parser_create(face->data);
        hz_parser_push_state(&p, face->gdef);

        struct {
            Offset16 glyph_class_def_offset,
                     attach_list_offset,
                     lig_caret_list_offset,
                     mark_attach_class_def_offset,
                     mark_glyph_sets_def_offset; } hdr;

        Version16Dot16 version = hz_parser_read_u32(&p);

        switch (version) {
            case 0x00010000: // 1.0
                hz_cmdread(&p, 1, &hdr, "wwww");
                break;
            case 0x00010002: // 1.2
                hz_cmdread(&p, 1, &hdr, "wwwww");
                break;
            case 0x00010003: // 1.3
                break;
            default: // error
                break;
        }

        if (hdr.glyph_class_def_offset) {
            // glyph class def isn't nil
            hz_parser_push_state(&p, hdr.glyph_class_def_offset);
            hz_read_class_def_table(&face->memory_arena, &p, &face->class_def);
            hz_parser_pop_state(&p);
        }

        if (hdr.mark_attach_class_def_offset) {
            hz_parser_push_state(&p, hdr.mark_attach_class_def_offset);
            hz_read_class_def_table(&face->memory_arena, &p, &face->attach_class_def);
            hz_parser_pop_state(&p);
        }

        if (hdr.mark_glyph_sets_def_offset) {
            hz_parser_push_state(&p, hdr.mark_glyph_sets_def_offset);
            uint16_t format = hz_parser_read_u16(&p);
            if (format == 1) {
                uint16_t mark_glyph_set_count = hz_parser_read_u16(&p);
                if (mark_glyph_set_count) {
                    Offset32 *mark_glyph_set_offsets = hz_memory_arena_alloc(&tmp_arena, sizeof(Offset32)*mark_glyph_set_count);
                    hz_parser_read_u32_block(&p, mark_glyph_set_offsets, mark_glyph_set_count);

                    face->mark_glyph_set = hz_memory_arena_alloc(&face->memory_arena, sizeof(hz_coverage_t)*mark_glyph_set_count);

                    for (int i = 0; i < mark_glyph_set_count; ++i) {
                        hz_coverage_t *coverage = &face->mark_glyph_set[i];
                        hz_parser_push_state(&p, mark_glyph_set_offsets[i]);
                        hz_read_coverage(&face->memory_arena, &p, coverage);
                        hz_parser_pop_state(&p);
                    }
                }
            } else {
                // error
            }

            hz_parser_pop_state(&p);
        }
    
        hz_parser_pop_state(&p);
    }
}

typedef struct {
    unsigned horizontal : 1;
    unsigned minimum : 1;
    unsigned cross_stream : 1;
    unsigned override : 1;
    unsigned reserved1 : 4;
    unsigned format : 8;
} hz_kern_coverage_field_t;

typedef union {
    uint16_t data;
    hz_kern_coverage_field_t field;
} hz_kern_coverage_t;

hz_error_t
hz_face_load_kerning_pairs(hz_face_t *face)
{
    hz_parser_t p;
    uint16_t version, i, n;

    if (!face->kern) {
        return HZ_ERROR_TABLE_DOES_NOT_EXIST;
    }

    p = hz_parser_create(face->data + face->kern);
    version = hz_parser_read_u16(&p);
    n = hz_parser_read_u16(&p);

    if (version != 0) {
        return HZ_ERROR_INVALID_TABLE_VERSION;
    }

    for (i = 0; i < n; ++i) {
        struct {
        uint16_t version, length;
        hz_kern_coverage_field_t coverage; } hdr;
        hz_cmdread(&p,1,&hdr,"www");

        switch (hdr.coverage.format) {
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
    int32_t index;
    if ((index = hz_class_def_search(&face->class_def, id)) != -1) {
        return 1 << (index-1);
    }

    return HZ_GLYPH_CLASS_BASE;
}

uint8_t
hz_face_get_glyph_attach_class(hz_face_t *face, hz_index_t id) {
    int32_t index;
    if ((index = hz_class_def_search(&face->attach_class_def, id)) != -1) {
        return index;
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
    face->fontinfo = info;
    face->data = info->data;
    face->gsub = stbtt__find_table(info->data,0,"GSUB");
    face->gpos = info->gpos;
    face->gdef = stbtt__find_table(info->data,0,"GDEF");
    face->maxp = stbtt__find_table(info->data,0,"maxp");
    face->glyf = info->glyf;
    face->cmap = stbtt__find_table(info->data,0,"cmap");
    face->hhea = info->hhea;
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

    {
        hz_parser_t p = hz_parser_create(face->data + face->hhea + 4);
        int16_t f1 = hz_parser_read_u16(&p),f2 = hz_parser_read_u16(&p);
        face->fheight = f1 - f2;
    }

    hz_font_set_face(font, face);

    return font;
}

typedef struct {
    u32 data[4];
    u8 length;
} uint_base128;

struct hz_woff2_header{
    u32 signature, flavor, length;
    u16 num_tables, reserved; // reserved
    u32 total_sfnt_size, total_compressed_size;
    u16 major_version, minor_version;
    u32 meta_offset, meta_length, meta_orig_length;
    u32 priv_offset, priv_length;
};

struct hz_woff2_table_dir_entry{
    union {
        unsigned int c;
        struct {
            unsigned known_tag:6, preprocessing_transformation_version_number:2;
        } s;
    } flags; 
    u32 tag;
    u32 orig_length;
    u32 transform_length;
};

HZ_ALWAYS_INLINE s32 hz_read_base_uint128(u32 *w, u8 *restrict mem)
{
    u32 accum=0;
    for (u32 i = 0; i < 5; ++i) {
        u8 byte = *mem++;
        // No leading 0's
        // If any of top 7 bits are set then << 7 would overflow
        if ((!i && byte == 0x80) || (accum & 0xfe000000)) return HZ_FALSE;
        accum = (accum << 7) | (byte & 0x7f);

        if (!(byte & 0x80)) { // most significand bit is 0, end of sequence
            *w = accum;
            return i;
        }
    }

    return -1;
}

static const hz_tag_t woff2_known_table_tags[63] = {
    'cmap', 'head', 'hhea', 'hmtx',
    'maxp', 'name', 'OS/2', 'post',
    'cvt ', 'fpgm', 'glyf', 'loca',
    'prep', 'CFF ', 'VORG', 'EBDT',
    'EBLC', 'gasp', 'hdmx', 'kern',
    'LTSH', 'PCLT', 'VDMX', 'vhea',
    'vmtx', 'BASE', 'GDEF', 'GPOS',
    'GSUB', 'EBSC', 'JSTF', 'MATH',
    'CBDT', 'CBLC', 'COLR', 'CPAL',
    'SVG ', 'sbix', 'acnt', 'avar',
    'bdat', 'bloc', 'bsln', 'cvar',
    'fdsc', 'feat', 'fmtx', 'fvar',
    'gvar', 'hsty', 'just', 'lcar',
    'mort', 'morx', 'opbd', 'prop',
    'trak', 'Zapf', 'Silf', 'Glat',
    'Gloc', 'Feat', 'Sill'
};

#define WOFF2_SIGNATURE 0x774F4632 /* wOF2 */

struct woff2_collection_header{u32 version;u16 num_fonts;};

u32 read_255_uint16(u8* ptr, u16 *outval)
{
    *outval = ptr[0];
    if (*outval == 253) {
        *outval = ((u16)ptr[1]) << 8 | ((u16)ptr[2]);
        return 3;
    }
    else if (*outval == 254) {
        *outval = (u16)ptr[1] + 253;
        return 2;
    }else if (*outval == 255) {
        *outval = (u16)ptr[1] + 253*2;
        return 2;
    }
    return 1;
}

typedef struct {
    u16 code;
    u16 symbol;
    u8 code_length_in_bits;
    u8 extension_bits;
} huffman_symbol;

struct huffman_tree {
    u16 num_entries;
    u8 max_code_length_in_bits;
    huffman_symbol* entries; 
    // huffman_entry* entries;
    // u8 *code_lengths_in_bits;
    // u8 num_code_lengths;
};


// rfc7932 9.1
// lookup table to decode WBITS field in the Brotli stream's header, this is used to determine the length of he sliding
// window
static const huffman_symbol wbits_huffman_symbols[] = {
    {0b1000010, 10, 7},
    {0b1000110, 11, 7},
    {0b1000001, 12, 7},
    {0b1000101, 13, 7},
    {0b1000011, 14, 7},
    {0b1000111, 15, 7},
    {0b0, 16, 1},
    {0b1000000, 17, 7},
    {0b1100, 18, 4},
    {0b1010, 19, 4},
    {0b1110, 20, 4},
    {0b1001, 21, 4},
    {0b1101, 22, 4},
    {0b1011, 23, 4},
    {0b1111, 24, 4}
};

static const huffman_symbol nbltypes_huffman_symbols[] = {
    {0x0 /* 0 */,    1,   1, 0},//1
    {0x8 /* 1000 */, 2,   4, 0},//2
    {0xC /*1100*/, 3,   4, 1},// 3..4
    {0xA /*1010*/, 5,   4, 2},// 5..8
    {0xE /*1110*/, 9,   4, 3},// 9..16
    {0x9 /*1001*/, 17,  4, 4},// 17..32
    {0xD /*1101*/, 33,  4, 5},//33..64
    {0xB /*1011*/, 65,  4, 6},//65..128
    {0xF /*1111*/, 129, 4, 7},//129..256
};

struct streaming_buffer {
    uint_least8_t *data;
    uint64_t bit_offset;
};

u32 streaming_buffer_read(struct streaming_buffer *input)
{
    // reads single bit from the stream
    u8 shift = input->bit_offset & 0x7u;
    u32 byte_index = input->bit_offset >> 3u;
    u8 byte = input->data[byte_index];
    u8 result = (byte >> shift) & 0x1u;
    ++input->bit_offset;
    return (u32)result;
}

uint64_t streaming_buffer_read_integer(struct streaming_buffer *input, int num_bits)
{
    assert(num_bits <= 64);
    uint64_t value = 0;

    if (!num_bits) return 0; // don't read anything

    int x0 = input->bit_offset;
    int x1 = x0 + num_bits - 1;

    int start_byte = x0 >> 3, end_byte = x1 >> 3;
    int num_bytes = (end_byte - start_byte) + 1;

    // We shift down the bytes as we read them, this will
    // make sure the first byte we read is the lsb, and the last would be msb.
    // We then correct the
    while (start_byte <= end_byte) {
        value >>= 8;
        value |= (uint64_t)input->data[start_byte++] << 56;
    }

    // Shift down by inner-byte offset
    value >>= (x0 & 0x7) + (64 - 8*num_bytes);
    // Mask off unused bits
    value &= (1 << num_bits) - 1;

    // Update buffer
    input->bit_offset += num_bits;

    return value;
}



uint64_t sb_read_msb_to_lsb(struct streaming_buffer* sb, int nbits)
{
    HZ_ASSERT(nbits <= 64);
    uint64_t value = 0;
    if (!nbits) return value;

    int x0 = sb->bit_offset;
    int x1 = x0 + nbits - 1;

    int start_byte = x0 >> 3, end_byte = x1 >> 3;
    int nbytes = (end_byte - start_byte) + 1;
    
    // reverse byte bits (method by Rich Schroeppel)
    while (start_byte <= end_byte) {
        value <<= 8;
        uint8_t b = sb->data[ start_byte ++ ];
        b = (b * 0x0202020202ull & 0x010884422010ull) % 1023;
        value |= b;
    }

    // Shift down by inner-byte offset
    value >>= (x0 & 0x7);
    // Mask off unused bits
    value &= (1 << nbits) - 1;
    // Update buffer
    sb->bit_offset += nbits;

    return value;
}

static u8 streaming_buffer_read_byte(struct streaming_buffer *input)
{
    u8 val;
    u32 bit_offset = input->bit_offset;
    u32 byte_index = bit_offset >> 3u;
    u32 shift = input->bit_offset & 0x7u;
    // if the shift is on a byte boundary, all we have to do is return the current byte
    if (shift) {
        // combine parts of current byte, and next byte
        u32 next_byte_index = byte_index + 1;
        u8 byte1 = input->data[byte_index];
        u8 byte2 = input->data[next_byte_index];
        val = (byte1 >> shift) | (byte2 << (8u - shift));
    } else {
        val = input->data[byte_index];
    }

    input->bit_offset += 8;
    return val;
}

static void streaming_buffer_align_to_next_byte(struct streaming_buffer *input)
{
    u32 offset = input->bit_offset;
    input->bit_offset += ~offset & 0x7u;
}

static void streaming_buffer_skip(struct streaming_buffer *input, int number)
{
    input->bit_offset += number;
}

#define HUFFMAN_ALLOCATE(A) huffman_allocate(ArraySize(A), A)

static struct huffman_tree* huffman_allocate(u16 num_symbols, const huffman_symbol* symbols)
{
    struct huffman_tree *huffman = hz_malloc(sizeof(*huffman));
    u32 max_code_length_in_bits = 0;
    for (u16 i = 0; i < num_symbols; ++i) {
        max_code_length_in_bits = MAX(max_code_length_in_bits, symbols[i].code_length_in_bits);
    }

    u16 num_entries = 1u << max_code_length_in_bits;
    // printf("%d\n", num_entries);

    huffman->entries = hz_malloc(num_entries * sizeof(huffman_symbol));//FIXME:use own allocator
    huffman->num_entries = num_entries;
    huffman->max_code_length_in_bits = max_code_length_in_bits;
    
    hz_memset(huffman->entries, 0, num_entries * sizeof(huffman_symbol));
    
    for (u16 symbol_index = 0; symbol_index < num_symbols; ++symbol_index) {
        huffman_symbol sym = symbols[symbol_index];
        u16 code = sym.code;
        // printf("CODE: %u\n", code);

        huffman->entries[code] = sym;
    }

    return huffman;
}

static int huffman_decode(struct streaming_buffer *input, struct huffman_tree *huffman, u32 *output)
{
    u32 bits = 0;
    huffman_symbol *sym;

    for (u32 code_length = 1; code_length <= huffman->max_code_length_in_bits; code_length++) {
        // read single bit into code
        u32 bit = streaming_buffer_read(input);
        bits = (bits << 1) | bit;
        u32 code = bits;
        sym = &huffman->entries[code];
        // printf("Reversed: 0x%02x\n", code);
        if (sym->code_length_in_bits == code_length) { // match length
            // if extra bits to read, read them then combine with the code
            // printf("Sym: %u, Code length: %u, Code: %u Ext: %u\n", sym->symbol, sym->code_length_in_bits, sym->code, sym->extension_bits);
            u32 ext = sb_read_msb_to_lsb(input, sym->extension_bits);//streaming_buffer_read_integer(input, sym->extension_bits);
            *output = (u32)sym->symbol + ext;
            return 1;
        }
    }

    // could not match any huffman code
    return 0;
}

#define ALPHABET_SIZE_LITERAL 256
#define ALPHABET_SIZE_INSERT_AND_COPY_LENGTH 704
#define ALPHABET_SIZE_BLOCK_COUNT 26

HZ_ALWAYS_INLINE u32 fit_bits_for_max_value(u32 value)
{
    u32 clz = HZ_CLZ(value);
    return 0x80000000u >> clz;
}

HZ_ALWAYS_INLINE u32 symbol_exists_in_buffer(u32 *buffer, u32 n, u32 sym)
{
    for (u32 i = 0; i < n; ++i) {
        if ( buffer[i] == sym) {
            return HZ_TRUE;
        }
    }
    return HZ_FALSE;
}

typedef struct {
    
} brotli_prefix_code;


static hz_error_t brotli_decode_prefix_code(struct streaming_buffer *input,
                                            u32 alphabet_size,
                                            u32 alphabet_size_bits,
                                            brotli_prefix_code *output
                                            )
{
    u32 hskip = streaming_buffer_read_integer(input,2);
    printf("hskip (simple code if 1): %d\n", hskip);
    if (hskip == 1) {//simple code
        printf("simple prefix code\n");
        u32 nsym_minus_one = streaming_buffer_read_integer(input,2);
        u32 nsym = nsym_minus_one + 1;
        // read nsym symbols
        u32 nsym_buffer[4];
        for (u32 n = 0; n < nsym; ++n) {
            u32 sym = streaming_buffer_read_integer(input, alphabet_size_bits);
            if (sym >= alphabet_size || symbol_exists_in_buffer(nsym_buffer, n, sym))
                return HZ_ERROR_BROTLI_STREAM_REJECTED;
            
            nsym_buffer[n] = sym;
        }
        u32 tree_select;
        
        if (nsym == 4)
            tree_select = streaming_buffer_read(input);

        

    }
    else
    {
        // complex code
        printf("complex prefix code\n");
        /* The following is more than slightly complicated...
         * The way it works is that as opposed to simple prefix codes, which encode symbols as fixed bit-length integers,
         * complex codes compress the symbol lengths themselves using a prefix code. The table for that is defined below
         * as 's_prefix_c_over_l_huffman_symbols'.
         * We have to read 18 **code length lengths** for a predefined list of symbols
         * provided in section 3.5 of the Brotli spec. and defined as 's_ll_symbols'.
         */
        static const huffman_symbol s_prefix_c_over_l_huffman_symbols[6] = {
            {0x0/* 00 */, 0, 2},
            {0xe/* 1110 */, 1, 4},
            {0x6/* 110 */, 2, 3},
            {0x1/* 01 */, 3, 2},
            {0x2/* 10 */, 4, 2},
            {0xf/* 1111 */, 5, 4}
        }; 
        struct huffman_tree *prefix_c_over_l_huffman = HUFFMAN_ALLOCATE(s_prefix_c_over_l_huffman_symbols);
        int code_lengths[18], code_lls[18];

        for(int i = 0; i < 18; ++i) {
            u32 cl = 0;
            if (!huffman_decode(input, prefix_c_over_l_huffman, &cl)){
                printf("can't decode huffman code\n");
            }
            printf("CL: %d\n", cl);
            code_lls[i] = cl;
        }
        
        static const int s_ll_symbols[18] = {1,2,3,4,0,5,17,6,16,7,8,9,10,11,12,13,14,15};
        {
            /* check if sum of (32 >> cl) for all non-zero code-lengths is 32 to validate if 
             * we're parsing the code lengths properly, and that the file is valid.
             */
            int cl_sum = 0, nz = 0;
            for (int i = 0; i < 18; ++i) {
                int cl = code_lls[i];
                nz += (int){cl > 0};
                cl_sum += cl ? 32 >> cl : 0;
            }
            printf("nz: %u / 18\n", nz);
            printf("cl_sum: %u\n", cl_sum);
            if (nz >= 2 && cl_sum != 32 && code_lls[17] != 0) {
                printf("ERROR: Invalid result for cl_sum!\n");
                return HZ_ERROR_BROTLI_STREAM_REJECTED;
            }
        }

        /* build huffman tree */
        // struct huffman_tree* code_ll_huffman = BUILD_HUFFMAN_TREE(s_ll_symbols, code_lls);

        /* TODO: Finish this shit */
    }

    return HZ_OK;
}

// block category
enum brotli_blcat {
    BROTLI_BLCAT_L,
    BROTLI_BLCAT_I,
    BROTLI_BLCAT_D,
    BROTLI_BLCAT_COUNT
};

typedef struct {
    u32 htree_btype;
    u32 htree_blen;
    u32 blen;
    u32 btype;
    u32 nbltypes;
} brotli_blcat;

enum brotli_cmode {
    BROTLI_CMODE_LSB6,
    BROTLI_CMODE_MSB6,
    BROTLI_CMODE_UTF8,
    BROTLI_CMODE_SIGNED,
    NUM_BROTLI_CMODES
};

static const char* s_brotli_cmode_name[] = {"LSB6","MSB6","UTF8","Signed"};

typedef struct {
    unsigned int is_last;
    unsigned int is_last_empty;
    unsigned int num_nibbles;
    brotli_blcat blcats[BROTLI_BLCAT_COUNT];
    enum brotli_cmode* cmode;
    enum brotli_cmode* cmapl;
    enum brotli_cmode* cmapd;
} brotli_meta_block;

void woff2_brotli_decompress(struct streaming_buffer *input) {
    // parse wbits, 1-7 bits variable length
    struct huffman_tree* wbits_huffman = HUFFMAN_ALLOCATE(wbits_huffman_symbols);
    struct huffman_tree* nbltypes_huffman = HUFFMAN_ALLOCATE(nbltypes_huffman_symbols);

    u32 wbits;
    if (!huffman_decode(input, wbits_huffman, &wbits)) {
        printf("Failed to decode huffman!\n");
    }
    u32 window_size = (1u << wbits) - 16u;

    printf("wbits: %u\n", wbits);
    printf("sliding window: %u bytes\n\n", window_size);
    for(;;) {
        brotli_meta_block blk = {0};

        // read meta block header
        u32 is_last = streaming_buffer_read(input);
        if (is_last){
            u32 is_last_empty = streaming_buffer_read(input);
            if (is_last_empty)
                break;
        }

        printf("is_last: %d\n", is_last);

        // read fixed-length mnibbles
        u32 m_nibbles = ((int[]){4,5,6,0})[streaming_buffer_read_integer(input, 2)];
        printf("m_nibbles: %d\n", m_nibbles);
        
        if (!m_nibbles){
#if 1
            u32 reserved = streaming_buffer_read(input);
            HZ_ASSERT(reserved == 0);
#else
            streaming_buffer_skip(input, 1);
#endif
            u32 m_skip_bytes = streaming_buffer_read_integer(input, 2);
            printf("skip bytes: %d\n", m_skip_bytes);
            u32 m_skip_len = 0;
            if (m_skip_bytes) {
                for (u32 i = 0; i < m_skip_bytes; ++i) {
                    u32 byte = streaming_buffer_read_byte(input);
                    m_skip_len = (m_skip_len >> 8u) | (byte << 24u);
                }

                m_skip_len >>= (32u - (m_skip_bytes << 3u));
                m_skip_len += 1;
            }

            printf("skip length: %d\n", m_skip_len);
            streaming_buffer_align_to_next_byte(input);
            if (m_skip_len){
                
            }
        }

        u32 m_len_minus_one = streaming_buffer_read_integer(input, 4 * m_nibbles);
        u32 m_len = m_len_minus_one + 1;

        u32 is_uncompressed;
        if (!is_last) {
            is_uncompressed = streaming_buffer_read(input);
        }

        printf("m_len: %u\n", m_len);
        printf("is_uncompressed: %u\n", is_uncompressed);

        for (u32 i = 0; i < BROTLI_BLCAT_COUNT; ++i) {//iterate I,L,D
            brotli_blcat* c = &blk.blcats[i];
            
            if (!huffman_decode(input, nbltypes_huffman, &c->nbltypes)) {
                printf("Failed to decode huffman symbol.\n");
            }
            printf("nbltypes_i: %u\n", c->nbltypes);
            
            if (c->nbltypes >= 2){

            } else {
                c->btype = 0, c->blen = 16777216;
            }
        }

        u32 nbltypesi = blk.blcats[BROTLI_BLCAT_I].nbltypes;
        u32 nbltypesl = blk.blcats[BROTLI_BLCAT_L].nbltypes;
        u32 nbltypesd = blk.blcats[BROTLI_BLCAT_D].nbltypes;

        u32 npostfix = streaming_buffer_read_integer(input, 2);
        printf("npostfix: %u\n", npostfix);
        
        u32 ndirect_four_msb_bits = streaming_buffer_read_integer(input, 4);
        u32 ndirect = ndirect_four_msb_bits << npostfix;
        printf("ndirect: %u\n", ndirect);
        
        // read cmode[]
        // read context mode for each literal block type
        blk.cmode = hz_malloc(sizeof(blk.cmode[0]) * nbltypesl); 
        for (u32 i = 0; i < nbltypesl; ++i) {
            u32 cmode = streaming_buffer_read_integer(input, 2);
            printf("cmode: %s\n", s_brotli_cmode_name[cmode]);
            blk.cmode[i] = cmode;
        }

        u32 ntreesl, ntreesd;
        huffman_decode(input, nbltypes_huffman, &ntreesl);
        if (ntreesl >= 2) { // read context map, CMAPL[]

        } else {
            // fill CMAPL[] with zeros
        }
        huffman_decode(input, nbltypes_huffman, &ntreesd);
        if (ntreesd >= 2) { // read context map, CMAPD[]

        } else {
            // fill CMAPD[] with zeros
        }

        printf("ntreesl: %u\n", ntreesl);
        printf("ntreesd: %u\n", ntreesd);
        
        u32 alphabet_size_distance = 16 + ndirect + (48 << npostfix);
        u32 alphabet_size_distance_bits = fit_bits_for_max_value(alphabet_size_distance);
        
        brotli_prefix_code *literal_codes = hz_malloc(sizeof(brotli_prefix_code)*nbltypesl);
        brotli_prefix_code *insert_and_copy_length_codes = hz_malloc(sizeof(brotli_prefix_code)*nbltypesi);
        brotli_prefix_code *distance_codes = hz_malloc(sizeof(brotli_prefix_code)*nbltypesd);

        for (u32 i = 0; i<ntreesl; ++i) {
            if (HZ_OK != brotli_decode_prefix_code(input, ALPHABET_SIZE_LITERAL, 8, &literal_codes[i])) {
                // reject stream
                break;
            }
        }

        for (u32 i = 0; i<nbltypesi; ++i) {
            if (HZ_OK != brotli_decode_prefix_code(input, ALPHABET_SIZE_INSERT_AND_COPY_LENGTH, 10, &insert_and_copy_length_codes[i])) {
                // reject stream
                break;
            }
        }

        for (u32 i = 0; i<ntreesd; ++i) {
            if (HZ_OK != brotli_decode_prefix_code(input, alphabet_size_distance, alphabet_size_distance_bits, &distance_codes[i])) {
                // reject stream
                break;
            }
        }

        hz_free(distance_codes);
        hz_free(insert_and_copy_length_codes);
        hz_free(literal_codes);

        break;
    }
}

// This is where we start parsing a WOFF2 file from an in-memory buffer
// provided by the caller. This does modify the memory passed in
hz_font_t *hz_font_load_woff2_from_memory(u8 *mem, size_t sz) {
    //  Ensure user doesn't pass in garbage or malicious values
    HZ_ASSERT((mem != NULL) && "Passed in null value");
    HZ_ASSERT(sz > 0); // ensure size is non-zero

    struct hz_woff2_header *hdr = (struct hz_woff2_header*)mem;
    mem += 48;
    if (!IS_BIG_ENDIAN){
        hz_byte_swap_u32x2((u64*)&hdr->signature);
        hz_byte_swap_32(&hdr->length);
        hz_byte_swap_16(&hdr->num_tables);
        hz_byte_swap_u32x2((u64*)&hdr->total_sfnt_size);
        hz_byte_swap_u16x2((u32*)&hdr->major_version);
        hz_byte_swap_u32x2((u64*)&hdr->meta_offset);
        hz_byte_swap_u32x2((u64*)&hdr->meta_orig_length);
        hz_byte_swap_32((u32*)&hdr->priv_length);
    }

    if (hdr->signature != 0x774F4632) { //'wOF2'
        return NULL; // error, signature mismatch
    }
    printf("Woff2 Header:\n");
    printf("\tFlavor: 0x%08x\n", hdr->flavor);
    printf("\tLength: %u\n", hdr->length);
    printf("\tVer: %hu.%hu\n", hdr->major_version, hdr->minor_version);
    printf("\tNumber Of Tables: %u\n", hdr->num_tables);
    printf("\tTotal Compressed Size: %u\n", hdr->total_compressed_size);
    printf("\tTotal Snft Size: %u\n", hdr->total_sfnt_size);
    printf("\tMeta Offset: %u\n", hdr->meta_offset);
    printf("\tMeta Length: %u\n", hdr->meta_length);
    printf("\tMeta Original Length: %u\n", hdr->meta_orig_length);

    if (sz != hdr->total_compressed_size) {
        printf("warning: total compressed size mismatch, sz: %u, total_compressed_size: %u\n",
        (u32)sz, hdr->total_compressed_size);
    }

    printf("table directory:\n");
    for (u32 i = 0; i < hdr->num_tables; ++i){
        struct hz_woff2_table_dir_entry d = {0};
        d.flags.c = (unsigned int) *mem++;
        if (d.flags.s.known_tag == 63) {//arbitrary tag
            d.tag = *(u32*)(mem); mem += 4;
            if (!IS_BIG_ENDIAN) hz_byte_swap_32(&d.tag);
        } else {
            d.tag = woff2_known_table_tags[d.flags.s.known_tag];
        }

        mem += hz_read_base_uint128(&d.orig_length, mem);
        if (d.flags.s.preprocessing_transformation_version_number != 3) {
            mem += hz_read_base_uint128(&d.transform_length, mem);
        }
        printf("flags: 0x%02x, orig_length: %u, transform_length: %u, tag: '%c%c%c%c', transform: %u\n",
            (u32)d.flags.c&0x3f, d.orig_length, d.transform_length, HZ_UNTAG(d.tag), d.flags.s.preprocessing_transformation_version_number);
    }

    if (hdr->flavor == 0x74746366){//ttcf
        // read collection directory format
        struct woff2_collection_header collection_hdr = {0}; 
        collection_hdr.version = *(u32*)mem;
        mem += 4;
        if (!IS_BIG_ENDIAN) hz_byte_swap_32(&collection_hdr.version);
        mem += read_255_uint16(mem, &collection_hdr.num_fonts);
        printf("version: %d, num_fonts: %d\n", collection_hdr.version, collection_hdr.num_fonts);
    }

    // decompress brotli stream
    struct streaming_buffer buffer = {.data=mem, .bit_offset=0};
    woff2_brotli_decompress(&buffer);

    return NULL;
}

#define HZ_BASE85_VALIDATE_CHARS

/* The Base85/Ascii85 decoder is based on the RFC 1924
 * version of the encoding: https://datatracker.ietf.org/doc/html/rfc1924,
 * The character set is as follows:
 *      0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!#$%&()*+-;<=>?@^_`{|}~
 */
static const s32 base85_alphabet_rfc_1924[127] = {
    [0 ... 32] = -1,
    [33] = 62,-1,63,64,65,66,-1,67,68,69,70,-1,71,-1,-1,0,1,2,3,4,5,6,7,8,9,-1,72,73,74,75,76,77,
    10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,
    -1,-1,-1, 78,79,80,
    36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,
    81,82,84,84
};

enum base85_error {
    BASE85_OVERFLOW = -1,
    BASE85_INVALID_CHARS = -2,
};

s32 hz_base85_max_size(const s8 *const inp, s32 *restrict max_outp_sz)
{
    s32 inp_sz = strlen(inp);
    *max_outp_sz = ((inp_sz + 3) / 4) * 5;
    return inp_sz;
}

s32 base85_char_ng(s8 c)
{
    return ((c < 33u) || (c > 117u));
}

static s32 decode_base85_adobe(u8 *restrict outp, s32 max_outp_sz, const s8 *const inp, s32 inp_sz)
{
    s32 outp_sz = 0, i = 0;
    u32 chunk;

    while (i < inp_sz) {
        s32 chunk_sz = inp_sz - i;
        if (inp[i] == (u8)' ') {
            ++i; continue; // ignore whitespace
        } else if (inp[i] == (u8)'z') {
            ++i;
            chunk = 0;
            chunk_sz = 5;
        }
        else if (base85_char_ng(inp[i])
            || ((chunk_sz > 1) && base85_char_ng(inp[i+1]))
            || ((chunk_sz > 2) && base85_char_ng(inp[i+2]))
            || ((chunk_sz > 3) && base85_char_ng(inp[i+3]))
            || ((chunk_sz > 4) && base85_char_ng(inp[i+4])))
        {
            return BASE85_INVALID_CHARS;
        } else if (chunk_sz >= 5) {
            chunk =  inp[i++] - 33u;
            chunk *= 85u; // max: 84 * 85 = 7,140
            chunk += inp[i++] - 33u;
            chunk *= 85u; // max: (84 * 85 + 84) * 85 = 614,040
            chunk += inp[i++] - 33u;
            chunk *= 85u; // max: (((84 * 85 + 84) * 85) + 84) * 85 = 52,200,540
            chunk += inp[i++] - 33u;
                // max: (((((84 * 85 + 84) * 85) + 84) * 85) + 84) * 85 = 4,437,053,040 oops! 0x108780E70
        
            if (chunk > (UINT32_MAX / 85u))
            {
                return BASE85_OVERFLOW;//can't fit in u32
            }
            else
            {
                u8 addend = inp[i++] - 33u;
                
                chunk *= 85u;
                if (chunk > (UINT32_MAX - addend)) return BASE85_OVERFLOW;// add would overflow
                chunk += addend;
            }
        } else {
            chunk  = inp[i++] - 33u;
            chunk *= 85u; // max: 84 * 85 = 7,140
            chunk += (i < inp_sz) ? inp[i++] - 33u : 84u;
            chunk *= 85u; // max: (84 * 85 + 84) * 85 = 614,040
            chunk += (i < inp_sz) ? inp[i++] - 33u : 84u;
            chunk *= 85u; // max: (((84 * 85 + 84) * 85) + 84) * 85 = 52,200,540
            chunk += (i < inp_sz) ? inp[i++] - 33u : 84u;
            // max: (((((84 * 85 + 84) * 85) + 84) * 85) + 84) * 85 = 4,437,053,040 oops! 0x108780E70
            if (chunk > (UINT32_MAX / 85u))
            {
                return BASE85_OVERFLOW;//can't fit in u32
            }
            else
            {
                u8 addend = (i < inp_sz) ? inp[i++] - 33u : 84u;
                
                chunk *= 85u;
                if (chunk > (UINT32_MAX - addend)) return BASE85_OVERFLOW;// add would overflow
                chunk += addend;
            }
        }

        outp[outp_sz + 3] = chunk & 255u;
        chunk >>= 8u;
        outp[outp_sz + 2] = chunk & 255u;
        chunk >>= 8u;
        outp[outp_sz + 1] = chunk & 255u;
        chunk >>= 8u;
        outp[outp_sz] = chunk;
        
        outp_sz += chunk_sz >= 5 ? 4 : chunk_sz - 1;
    }

    decode_error:
    return outp_sz;
}

hz_font_t *hz_font_load_woff2_from_memory_base85(enum hz_base85_encoding encoding, const char *base85_string)
{
    s32 max_outp_sz, inp_sz, sz;
    inp_sz = hz_base85_max_size((const s8* const)base85_string, &max_outp_sz);
    u8 *decoded = hz_malloc(max_outp_sz);
    switch (encoding) {
        default:break;
        case HZ_BASE85_ENCODING_ADOBE:
        sz = decode_base85_adobe(decoded, max_outp_sz, (const s8* const) base85_string, inp_sz);
        break;
        case HZ_BASE85_ENCODING_Z85:/*No Impl*/
        break;
        case HZ_BASE85_ENCODING_IPV6: /*No Impl*/
        break;
    }
    printf("%d\n", sz);//danger!
    
    hz_font_load_woff2_from_memory(decoded, sz);
    hz_free(decoded);
    return NULL;
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

    /* Number of feature index values for this language system � excludes the required feature */
    uint16_t feature_index_count;
} hz_lang_sys_t;

typedef struct {
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
    // Array of indices into the LookupList � zero-based (first lookup is LookupListIndex = 0)
    uint16_t *lookup_list_indices;
} hz_feature_table_t;

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
hz_read_value_record(hz_parser_t *p, hz_value_record_t *record, uint16_t valueFormat) {

    if (valueFormat & HZ_VALUE_FORMAT_X_PLACEMENT)
        record->xPlacement = (int16_t) hz_parser_read_u16(p);

    if (valueFormat & HZ_VALUE_FORMAT_Y_PLACEMENT)
        record->yPlacement = (int16_t) hz_parser_read_u16(p);

    if (valueFormat & HZ_VALUE_FORMAT_X_ADVANCE)
        record->xAdvance = (int16_t) hz_parser_read_u16(p);

    if (valueFormat & HZ_VALUE_FORMAT_Y_ADVANCE)
        record->yAdvance  = (int16_t) hz_parser_read_u16(p);

    if (valueFormat & HZ_VALUE_FORMAT_X_PLACEMENT_DEVICE)
        record->xPlaDeviceOffset = hz_parser_read_u16(p);

    if (valueFormat & HZ_VALUE_FORMAT_Y_PLACEMENT_DEVICE)
        record->yPlaDeviceOffset = hz_parser_read_u16(p);

    if (valueFormat & HZ_VALUE_FORMAT_X_ADVANCE_DEVICE)
        record->xAdvDeviceOffset = hz_parser_read_u16(p);

    if (valueFormat & HZ_VALUE_FORMAT_Y_ADVANCE_DEVICE)
        record->yAdvDeviceOffset = hz_parser_read_u16(p);
}

typedef struct hz_ot_single_pos_format1_table_t {
    hz_coverage_t coverage;
    uint16_t value_format;
    hz_value_record_t value_record;
} hz_ot_single_pos_format1_table_t;

typedef struct hz_ot_single_pos_format2_table_t {
    hz_coverage_t coverage;
    uint16_t value_format;
    uint16_t value_count;
    hz_value_record_t *value_records;
} hz_ot_single_pos_format2_table_t;

typedef struct hz_entry_exit_record_t {
    Offset16 entry_anchor_offset, exit_anchor_offset;
} hz_entry_exit_record_t;

typedef struct {
    int16_t x_coord, y_coord;
} hz_anchor_t;

typedef struct {
    hz_bool has_entry, has_exit;
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
    hz_coverage_t coverage;
    uint16_t value_format1;
    uint16_t value_format2;
    uint16_t pair_set_count;
    hz_pair_set_t *pair_sets;
} hz_pair_pos_format1_subtable_t;

typedef struct {
    uint16_t format;
    hz_coverage_t coverage;
    uint16_t value_format1;
    uint16_t value_format2;
    hz_class_def_t class_def1;
    hz_class_def_t class_def2;
    uint16_t class1_count;
    uint16_t class2_count;
    hz_class1_record_t *class1_records;
} hz_pair_pos_format2_subtable_t;

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
hz_parse_chained_sequence_rule(hz_memory_arena_t *a, hz_parser_t *p, hz_chained_sequence_rule_t *s) {
    s->prefix_count = hz_parser_read_u16(p);
    if (s->prefix_count) {
        s->prefix_sequence = hz_memory_arena_alloc(a, sizeof(uint16_t)*s->prefix_count);
        hz_parser_read_u16_block(p, s->prefix_sequence, s->prefix_count);
    }

    s->input_count = hz_parser_read_u16(p);
    if (s->input_count) {
        s->input_sequence = hz_memory_arena_alloc(a, sizeof(uint16_t)*(s->input_count - 1));
        hz_parser_read_u16_block(p, s->input_sequence, s->input_count - 1);
    }

    s->suffix_count = hz_parser_read_u16(p);
    if (s->suffix_count) {
        s->suffix_sequence = hz_memory_arena_alloc(a, sizeof(uint16_t)*s->suffix_count);
        hz_parser_read_u16_block(p, s->suffix_sequence, s->suffix_count);
    }

    s->lookup_count = hz_parser_read_u16(p);
    s->lookup_records = hz_memory_arena_alloc(a, sizeof(hz_sequence_lookup_record_t)*s->lookup_count);
    hz_parser_read_u16_block(p, (uint16_t *)s->lookup_records, s->lookup_count*2);
}

void
hz_parse_chained_sequence_rule_set(hz_memory_arena_t *memory_arena, hz_parser_t *p, hz_chained_sequence_rule_set_t *rule_set) {
    rule_set->count = hz_parser_read_u16(p);
    Offset16* offsets = hz_malloc(sizeof(Offset16) * rule_set->count);
    hz_parser_read_u16_block(p,offsets,rule_set->count);

    rule_set->rules = hz_memory_arena_alloc(memory_arena, sizeof(hz_chained_sequence_rule_t)*rule_set->count);

    for (uint16_t i = 0; i < rule_set->count; ++i) {
        hz_parser_push_state(p,offsets[i]);
        hz_parse_chained_sequence_rule(memory_arena, p, rule_set->rules + i);
        hz_parser_pop_state(p);
    }

    hz_free(offsets);
}

typedef struct {
    uint16_t format;
    uint16_t prefix_count;
    hz_coverage_t *prefix_coverages;
    uint16_t input_count;
    hz_coverage_t *input_coverages;
    uint16_t suffix_count;
    hz_coverage_t *suffix_coverages;
    uint16_t lookup_count;
    hz_sequence_lookup_record_t *lookup_records;
} hz_chained_sequence_context_format3_subtable_t;

void
hz_ot_load_chained_sequence_context_format3_subtable(hz_memory_arena_t *memory_arena,
                                                     hz_parser_t *p,
                                                     hz_chained_sequence_context_format3_subtable_t *table)
{
    char tmp_buffer[4000];
    hz_memory_arena_t tmp_arena = hz_memory_arena_create(tmp_buffer, sizeof tmp_buffer);

    // Read prefix glyph coverages
    table->prefix_count = hz_parser_read_u16(p);
    Offset16 *prefix_offsets = hz_memory_arena_alloc(&tmp_arena, table->prefix_count * sizeof(Offset16));
    hz_parser_read_u16_block(p, prefix_offsets, table->prefix_count);

    table->prefix_coverages = hz_memory_arena_alloc(memory_arena, sizeof(hz_coverage_t) * table->prefix_count);

    for (uint16_t i = 0; i < table->prefix_count; ++i) {
        hz_parser_push_state(p, prefix_offsets[i]);
        hz_read_coverage(memory_arena, p, &table->prefix_coverages[i]);
        hz_parser_pop_state(p);
    }

    hz_memory_arena_reset(&tmp_arena);

    // Read input glyph coverages
    table->input_count = hz_parser_read_u16(p);
    Offset16 *input_offsets = hz_memory_arena_alloc(&tmp_arena, table->input_count * sizeof(Offset16));
    hz_parser_read_u16_block(p, input_offsets, table->input_count);

    table->input_coverages = hz_memory_arena_alloc(memory_arena, sizeof(hz_coverage_t) * table->input_count);

    for (uint16_t i = 0; i < table->input_count; ++i) {
        hz_parser_push_state(p, input_offsets[i]);
        hz_read_coverage(memory_arena, p, &table->input_coverages[i]);
        hz_parser_pop_state(p);
    }

    hz_memory_arena_reset(&tmp_arena);

    // Read suffix glyph coverages
    table->suffix_count = hz_parser_read_u16(p);
    Offset16 *suffix_offsets = hz_memory_arena_alloc(&tmp_arena, table->suffix_count * sizeof(Offset16));
    hz_parser_read_u16_block(p, suffix_offsets, table->suffix_count);

    table->suffix_coverages = hz_memory_arena_alloc(memory_arena, sizeof(hz_coverage_t) * table->suffix_count);

    for (uint16_t i = 0; i < table->suffix_count; ++i) {
        hz_parser_push_state(p, suffix_offsets[i]);
        hz_read_coverage(memory_arena, p, &table->suffix_coverages[i]);
        hz_parser_pop_state(p);
    }

    // Read lookup records
    table->lookup_count = hz_parser_read_u16(p);
    table->lookup_records = hz_memory_arena_alloc(memory_arena, sizeof(hz_sequence_lookup_record_t) * table->lookup_count);
    hz_parser_read_u16_block(p, (uint16_t *)table->lookup_records, table->lookup_count * 2);
}

typedef struct hz_sequence_table_t {
    uint16_t glyph_count;
    uint16_t *glyphs;
} hz_sequence_table_t;

typedef struct hz_multiple_subst_format1_t {
    hz_ht_t coverage;
    uint16_t sequence_count;
    hz_sequence_table_t *segs;
} hz_multiple_subst_format1_t;

hz_anchor_t
hz_read_anchor(hz_parser_t *p) {
    hz_anchor_t anchor;
    uint16_t format = hz_parser_read_u16(p);
    HZ_ASSERT(format >= 1 && format <= 3);
    anchor.x_coord = hz_parser_read_u16(p);
    anchor.y_coord = hz_parser_read_u16(p);
    return anchor;
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
    HZ_FEATURE_FLAG_NONE           = 0,
    HZ_FEATURE_FLAG_REQUIRED       = HZ_FLAG(0),
    HZ_FEATURE_FLAG_ALWAYS_APPLIED = HZ_FLAG(1),
    HZ_FEATURE_FLAG_ON_BY_DEFAULT  = HZ_FLAG(2),
    HZ_FEATURE_FLAG_OFF_BY_DEFAULT = HZ_FLAG(3)
} hz_feature_flag_t;

typedef struct hz_feature_layout_op_t {
    hz_feature_t feature;
    hz_tag_t layout_op; // 'GSUB' or 'GPOS'
    uint8_t flags;
} hz_feature_layout_op_t;

static const hz_feature_layout_op_t std_feature_ops_arabic[] = {
    /* Language based forms */
    { HZ_FEATURE_CCMP, HZ_TAG_GSUB, HZ_FEATURE_FLAG_ALWAYS_APPLIED },
    { HZ_FEATURE_ISOL, HZ_TAG_GSUB, HZ_FEATURE_FLAG_ALWAYS_APPLIED },
    { HZ_FEATURE_FINA, HZ_TAG_GSUB, HZ_FEATURE_FLAG_ALWAYS_APPLIED },
    { HZ_FEATURE_MEDI, HZ_TAG_GSUB, HZ_FEATURE_FLAG_ALWAYS_APPLIED },
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

typedef struct {
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

hz_bool
hz_ot_is_complex_script(hz_script_t script)
{
    int i;

    for (i = 0; i < HZ_ARRAY_SIZE(complex_script_list); ++i) {
        if (complex_script_list[i] == script) {
            return HZ_TRUE;
        }
    }

    return HZ_FALSE;
}

HZ_STATIC void
hz_auto_load_script_features(hz_memory_arena_t *memory_arena, hz_script_t script, hz_feature_t **featuresptr, unsigned int *countptr)
{
    unsigned int i, j, inclmask;

    inclmask = HZ_FEATURE_FLAG_REQUIRED | HZ_FEATURE_FLAG_ALWAYS_APPLIED
        | HZ_FEATURE_FLAG_ON_BY_DEFAULT;

    if (hz_ot_is_complex_script(script)) {
        for (i=0; i<HZ_ARRAY_SIZE(complex_script_feature_orders); ++i) {
            hz_script_feature_order_t order = complex_script_feature_orders[i];
            if (order.script == script) {
                unsigned int cnt = 0;

                // count required, on by default and always applied features
                for (j=0;j<order.num_ops;++j)
                    if (order.ops[j].flags & inclmask)
                        ++cnt;

                if (cnt) {
                    unsigned int f;
                    *featuresptr = hz_memory_arena_alloc(memory_arena, cnt * sizeof(hz_feature_t));
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

    for (i = 0; i < HZ_ARRAY_SIZE(language_map_list); ++i) {
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
     * see �Use of the language field in 'cmap' subtables� in this document.
     */
    uint16_t language;
    // An array that maps character codes to glyph index values.
    uint8_t glyph_id_array[256];
} hz_cmap_byte_encoding_subtable_t;


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
                id = IS_BIG_ENDIAN ? offset : hz_bswap16(offset);
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

hz_error_t hz_init(const hz_config_t *cfg)
{
    hz_.allocator.allocfn = hz_standard_c_allocator_fn;
    hz_.allocator.user = NULL;

    if (hz_.is_already_initialized)
        return HZ_ERROR_ALREADY_INITIALIZED;
    
    hz_.cfg = *cfg;
    hz_.is_already_initialized = HZ_TRUE;
    return HZ_OK;
}

void hz_deinit(void)
{
}

#if HZ_COMPILER & HZ_COMPILER_GCC
#define HZ_NAKEDFN __attribute__((naked))
#elif HZ_COMPILER & HZ_COMPILER_VC
#define HZ_NAKEDFN __declspec(naked)
#else
#define HZ_NAKEDFN
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

HZ_STATIC void
hz_apply_cmap_encoding(hz_parser_t *p,
                       hz_cmap_encoding_t encoding,
                       hz_index_t glyphIndices[],
                       hz_unicode_t codepoints[],
                       size_t size)
{
    hz_parser_push_state(p,encoding.subtable_offset);
    uint16_t format = hz_parser_read_u16(p);

    switch (format) {
        default: break;
        case HZ_CMAP_SUBTABLE_FORMAT_BYTE_ENCODING_TABLE: break;
        case 2: break;
        case HZ_CMAP_SUBTABLE_FORMAT_SEGMENT_MAPPING_TO_DELTA_VALUES: {
            hz_cmap_format4_subtable_t cmapSubtable;
            size_t i;

            cmapSubtable.length = hz_parser_read_u16(p);
            cmapSubtable.language = hz_parser_read_u16(p);
            cmapSubtable.segCountX2 = hz_parser_read_u16(p);
            cmapSubtable.searchRange = hz_parser_read_u16(p);
            cmapSubtable.entrySelector = hz_parser_read_u16(p);
            cmapSubtable.rangeShift = hz_parser_read_u16(p);

            uint16_t numSegments = cmapSubtable.segCountX2 / 2;
            size_t table_size = numSegments * sizeof(uint16_t);

            cmapSubtable.endCode = hz_malloc(table_size);
            cmapSubtable.startCode = hz_malloc(table_size);
            cmapSubtable.idDelta = hz_malloc(table_size);
            cmapSubtable.idRangeOffsets = hz_malloc(table_size);

            hz_parser_read_u16_block(p, cmapSubtable.endCode, numSegments);
            cmapSubtable.reservedPad = hz_parser_read_u16(p);
            hz_parser_read_u16_block(p, cmapSubtable.startCode, numSegments);
            hz_parser_read_u16_block(p, (uint16_t *) cmapSubtable.idDelta, numSegments);
            hz_parser_read_u16_block(p, cmapSubtable.idRangeOffsets, numSegments);
            cmapSubtable.glyphIdArray = (uint16_t *)hz_parser_at_cursor(p);
            hz_apply_cmap_format4_subtable(&cmapSubtable, glyphIndices, codepoints, size);

            hz_free(cmapSubtable.endCode);
            hz_free(cmapSubtable.startCode);
            hz_free(cmapSubtable.idDelta);
            hz_free(cmapSubtable.idRangeOffsets);

            break;
        }
    }

    hz_parser_pop_state(p);
}

HZ_STATIC void
hz_map_to_nominal_forms(hz_face_t *face,
                        hz_index_t glyph_indices[],
                        hz_unicode_t codepoints[],
                        size_t size)
{
    hz_parser_t p = hz_parser_create(face->data + face->cmap);
    uint16_t version = hz_parser_read_u16(&p);

    // Table version number must be 0
    HZ_ASSERT(version == 0);

    uint16_t num_encodings, enc_idx;
    num_encodings = hz_parser_read_u16(&p);

    {
        hz_cmap_encoding_t encoding = {0};
        encoding.platform_id = hz_parser_read_u16(&p);
        encoding.encoding_id = hz_parser_read_u16(&p);
        encoding.subtable_offset = hz_parser_read_u32(&p);

        hz_apply_cmap_encoding(&p, encoding, glyph_indices, codepoints, size);
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

HZ_STATIC hz_bool
hz_should_ignore_glyph(hz_buffer_t *buffer, size_t index, uint16_t flags, const hz_coverage_t *mark_filtering_set) {
    if (buffer->attrib_flags & (HZ_GLYPH_ATTRIB_GLYPH_CLASS_BIT | HZ_GLYPH_ATTRIB_ATTACHMENT_CLASS_BIT)) {
        uint8_t attach_type = (flags & HZ_LOOKUP_FLAG_MARK_ATTACHMENT_TYPE_MASK) >> 8;
        uint16_t ignored_classes = hz_ignored_classes_from_lookup_flags(flags);

        if (buffer->glyph_classes[index] & ignored_classes) return HZ_TRUE;

        if (buffer->glyph_classes[index] & HZ_GLYPH_CLASS_MARK) {
            if (flags & HZ_LOOKUP_FLAG_USE_MARK_FILTERING_SET) {
                if (hz_coverage_search(mark_filtering_set, buffer->glyph_indices[index]) == -1) {
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

HZ_STATIC hz_bool hz_check_cpu_flags(hz_cpu_flags_t features) {
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

hz_bool hz_buffer_contains_range(const hz_buffer_t *buf, int v1, int v2) {
    size_t size = hz_vector_size(buf->glyph_indices);
    return v1 >= 0 && v1 < size
    && v2 >= 0 && v2 < size
    && v2 >= v1;
}

hz_buffer_t *hz_buffer_copy_range(hz_buffer_t *from, int v1, int v2){
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
    hz_coverage_t *mark_filtering_set;
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


struct hz_font_data_t {
    hz_face_t *face;
    uint8_t *memory_arena_data;
    hz_memory_arena_t memory_arena;
    hz_allocator_t allocator;
    /* OT data */
    hz_gsub_table_t gsub_table;
    hz_gpos_table_t gpos_table;
};

#define HZ_SHAPER_ARENA_SIZE 5000

struct hz_shaper_t {
    uint8_t ar [HZ_SHAPER_ARENA_SIZE]; // used to store the feature list
    hz_memory_arena_t memory_arena;
    hz_feature_t *features;
    size_t num_features;
    hz_direction_t direction;
    hz_script_t script;
    hz_language_t language;
    hz_shaper_flags_t flags;
};

hz_shaper_t *hz_shaper_create() {
    hz_shaper_t *s = hz_malloc(sizeof(*s));
    *s = (hz_shaper_t){
        .direction = HZ_DIRECTION_LTR,
        .script = HZ_SCRIPT_LATIN,
        .language = HZ_LANGUAGE_ENGLISH,
    };

    hz_memory_arena_init(&s->memory_arena, s->ar, sizeof s->ar);
    return s;
}

void hz_shaper_destroy(hz_shaper_t *shaper){
    hz_free(shaper);
}

void hz_shaper_set_features(hz_shaper_t *shaper, size_t sz,
                                const hz_feature_t features[])
{
    hz_memory_arena_reset(&shaper->memory_arena);
    shaper->features = hz_memory_arena_alloc(&shaper->memory_arena, sizeof(hz_feature_t)*sz);
    shaper->num_features = sz;
    memcpy(shaper->features, features, sizeof(hz_feature_t)*sz);
}

void hz_shaper_set_flags(hz_shaper_t *shaper, hz_shaper_flags_t flags)
{
    shaper->flags = flags;
}

void hz_shaper_set_direction(hz_shaper_t *shaper, hz_direction_t direction) {
    shaper->direction = direction;
}

void hz_shaper_set_script(hz_shaper_t *shaper, hz_script_t script) {
    shaper->script = script;
}

void hz_shaper_set_language(hz_shaper_t *shaper, hz_language_t language) {
    shaper->language = language;
}

HZ_STATIC void hz_load_feature_table(hz_memory_arena_t *memory_arena, hz_parser_t *p, hz_feature_table_t *table) {
    table->feature_params = hz_parser_read_u16(p);
    table->lookup_index_count = hz_parser_read_u16(p);
    table->lookup_list_indices = hz_memory_arena_alloc(memory_arena, sizeof(uint16_t) * table->lookup_index_count);
    hz_parser_read_u16_block(p, table->lookup_list_indices, table->lookup_index_count);
}

typedef struct {
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
hz_read_gsub_single_substitution_subtable(hz_memory_arena_t *memory_arena,
                                          hz_parser_t *p,
                                          hz_lookup_table_t *lookup,
                                          uint16_t subtable_index,
                                          uint16_t format)
{
    switch (format) {
        default: return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
        case 1: {
            hz_single_substitution_format1_subtable_t *subtable = hz_memory_arena_alloc(memory_arena, sizeof(*subtable));
            subtable->format = format;
            Offset16 coverage_offset = hz_parser_read_u16(p);

            hz_parser_push_state(p, coverage_offset);
            hz_read_coverage(memory_arena, p, &subtable->coverage);
            hz_parser_pop_state(p);

            subtable->delta_glyph_id = hz_parser_read_u16(p);
            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }

        case 2: {
            hz_single_substitution_format2_subtable_t *subtable = hz_memory_arena_alloc(memory_arena, sizeof(*subtable));

            subtable->format = format;
            Offset16 coverage_offset = hz_parser_read_u16(p);
            
            hz_parser_push_state(p, coverage_offset);
            hz_read_coverage(memory_arena, p, &subtable->coverage);
            hz_parser_pop_state(p);

            subtable->glyph_count = hz_parser_read_u16(p);
            subtable->substitute_glyph_ids = hz_memory_arena_alloc(memory_arena, subtable->glyph_count * sizeof(uint16_t));
            hz_parser_read_u16_block(p, subtable->substitute_glyph_ids, subtable->glyph_count);

            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }

    }

    return HZ_OK;
}

typedef struct hz_multiple_substitution_format1_subtable_t {
    uint16_t format;
    hz_coverage_t coverage;
    uint16_t sequence_count;
    hz_sequence_table_t *sequences;
} hz_multiple_substitution_format1_subtable_t;

typedef struct {
    uint16_t ligature_glyph;
    uint16_t component_count;
    uint16_t *component_glyph_ids;
} hz_ligature_t;

typedef struct {
    uint16_t ligature_count;
    hz_ligature_t *ligatures;
} hz_ligature_set_table_t;

typedef struct {
    uint16_t format;
    hz_coverage_t coverage;
    uint16_t ligature_set_count;
    hz_ligature_set_table_t *ligature_sets;
} hz_ligature_substitution_format1_subtable_t;

HZ_STATIC hz_error_t
hz_read_gsub_ligature_substitution_subtable(hz_memory_arena_t *memory_arena,
                                            hz_parser_t *p,
                                            hz_lookup_table_t *lookup,
                                            uint16_t subtable_index,
                                            uint16_t format)
{
    char tmp_buffer[4000];
    hz_stack_allocator_t tmp_stack = hz_stack_create(tmp_buffer,sizeof tmp_buffer);

    if (format != 1)
        return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;

    hz_ligature_substitution_format1_subtable_t *subtable = hz_memory_arena_alloc(memory_arena, sizeof(*subtable));

    subtable->format = format;
    Offset16 coverage_offset = hz_parser_read_u16(p);

    hz_parser_push_state(p,coverage_offset);
    hz_read_coverage(memory_arena, p, &subtable->coverage);
    hz_parser_pop_state(p);

    subtable->ligature_set_count = hz_parser_read_u16(p);
    subtable->ligature_sets = hz_memory_arena_alloc(memory_arena, sizeof(hz_ligature_set_table_t) * subtable->ligature_set_count);

    Offset16 *ligature_set_offsets = hz_stack_alloc(&tmp_stack, subtable->ligature_set_count * sizeof(Offset16));
    hz_parser_read_u16_block(p, ligature_set_offsets, subtable->ligature_set_count);

    for (uint16_t i = 0; i < subtable->ligature_set_count; ++i) {
        hz_parser_push_state(p,ligature_set_offsets[i]);
        hz_ligature_set_table_t *ligature_set = subtable->ligature_sets + i;

        ligature_set->ligature_count = hz_parser_read_u16(p);
        ligature_set->ligatures = hz_memory_arena_alloc(memory_arena, sizeof(hz_ligature_t) * ligature_set->ligature_count);
        
        Offset16 *ligature_offsets = hz_stack_alloc(&tmp_stack, ligature_set->ligature_count * sizeof(Offset16));
        hz_parser_read_u16_block(p, ligature_offsets, ligature_set->ligature_count);

        for (uint16_t j = 0; j < ligature_set->ligature_count; ++j) {
            hz_ligature_t *ligature = ligature_set->ligatures + j;

            hz_parser_push_state(p,ligature_offsets[j]);
            ligature->ligature_glyph = hz_parser_read_u16(p);
            ligature->component_count = hz_parser_read_u16(p);
            if (ligature->component_count > 1) {
                ligature->component_glyph_ids = hz_memory_arena_alloc(memory_arena, (ligature->component_count - 1) * sizeof(uint16_t));
                hz_parser_read_u16_block(p, ligature->component_glyph_ids, ligature->component_count - 1);
            } else {
                ligature->component_glyph_ids = NULL;
            }

            hz_parser_pop_state(p);
        }

        hz_stack_free(&tmp_stack, ligature_offsets);
        hz_parser_pop_state(p);
    }
    
    lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
    return HZ_OK;
}

typedef struct hz_chained_sequence_context_format1_subtable_t {
    uint16_t format;
    hz_coverage_t coverage;
    uint16_t rule_set_count;
    hz_chained_sequence_rule_set_t *rule_sets;
} hz_chained_sequence_context_format1_subtable_t;

HZ_STATIC hz_error_t
hz_read_gsub_chained_contexts_substitution_subtable(hz_memory_arena_t *memory_arena,
                                                    hz_parser_t *p,
                                                    hz_lookup_table_t *lookup,
                                                    uint16_t subtable_index,
                                                    uint16_t format)
{
    uint8_t tmp_buffer[4000];
    hz_memory_arena_t tmp_arena = hz_memory_arena_create(tmp_buffer, sizeof tmp_buffer);
    switch (format) {
        case 1: {
            // 6.1 Chained Contexts Substitution Format 1: Simple Glyph Contexts
            // https://docs.microsoft.com/en-us/typography/opentype/spec/gsub#61-chained-contexts-substitution-format-1-simple-glyph-contexts
            hz_chained_sequence_context_format1_subtable_t *subtable = hz_memory_arena_alloc(memory_arena, sizeof(*subtable));
            subtable->format = format;

            Offset16 coverage_offset = hz_parser_read_u16(p);
            hz_parser_push_state(p,coverage_offset);
            hz_read_coverage(memory_arena, p, &subtable->coverage);
            hz_parser_pop_state(p);

            subtable->rule_set_count = hz_parser_read_u16(p);

            Offset16 *rule_set_offsets = hz_memory_arena_alloc(&tmp_arena, sizeof(Offset16) * subtable->rule_set_count);
            hz_parser_read_u16_block(p, rule_set_offsets, subtable->rule_set_count);

            subtable->rule_sets = hz_memory_arena_alloc(memory_arena, sizeof(*subtable->rule_sets) * subtable->rule_set_count);
            for (int i = 0; i < subtable->rule_set_count; ++i) {
                if (rule_set_offsets[i]) {
                    hz_parser_push_state(p, rule_set_offsets[i]);
                    hz_parse_chained_sequence_rule_set(memory_arena, p, subtable->rule_sets+i);
                    hz_parser_pop_state(p);
                } else {
                    subtable->rule_sets[i].count = 0;
                    subtable->rule_sets[i].rules = NULL;
                }
            }

            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }

        case 2: {
            break;
        }
        case 3: {
            hz_chained_sequence_context_format3_subtable_t *subtable = hz_memory_arena_alloc(memory_arena, sizeof(*subtable));
            subtable->format = format;
            hz_ot_load_chained_sequence_context_format3_subtable(memory_arena, p, subtable);
            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }
        default:
            return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
    }

    return HZ_OK;
}

HZ_STATIC hz_error_t
hz_read_gsub_multiple_substitution_subtable(hz_memory_arena_t *memory_arena,
                                            hz_parser_t *p,
                                            hz_lookup_table_t *lookup,
                                            uint16_t subtable_index,
                                            uint16_t format)
{
    uint8_t tmp_buffer[1024];
    hz_memory_arena_t tmp_arena = hz_memory_arena_create(tmp_buffer, sizeof tmp_buffer);

    if (format != 1) {
        return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
    }

    hz_multiple_substitution_format1_subtable_t *subtable = hz_memory_arena_alloc(memory_arena, sizeof(*subtable));

    subtable->format = format;
    Offset16 coverage_offset = hz_parser_read_u16(p);
    subtable->sequence_count = hz_parser_read_u16(p);

    hz_parser_push_state(p, coverage_offset);
    hz_read_coverage(memory_arena, p, &subtable->coverage);
    hz_parser_pop_state(p);

    subtable->sequences = hz_memory_arena_alloc(memory_arena, subtable->sequence_count * sizeof(hz_sequence_table_t));
    Offset16 *sequence_offsets = hz_memory_arena_alloc(&tmp_arena, subtable->sequence_count * sizeof(Offset16));
    hz_parser_read_u16_block(p, sequence_offsets, subtable->sequence_count);

    for (uint16_t i = 0 ; i < subtable->sequence_count; ++i) {
        hz_sequence_table_t *seq = &subtable->sequences[i];
        if (sequence_offsets[i]) {
            hz_parser_push_state(p, sequence_offsets[i]);
            seq->glyph_count = hz_parser_read_u16(p);
            seq->glyphs = hz_memory_arena_alloc(memory_arena, seq->glyph_count * sizeof(uint16_t));
            hz_parser_read_u16_block(p, seq->glyphs, seq->glyph_count);
            hz_parser_pop_state(p);
        }
    }

    lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
    return HZ_OK;
}

void hz_error_and_quit(const char *msg, const char *filename, int linenum)
{
    fprintf(stderr, "[ERROR in %s:%d]: %s\n", filename, linenum, msg);
    exit(EXIT_FAILURE);
}

#define HZ_EXIT_WITH_MSG(msg) hz_error_and_quit(msg, __FILE__, __LINE__)

HZ_STATIC hz_error_t
hz_read_gsub_lookup_subtable(hz_memory_arena_t *memory_arena,
                             hz_parser_t *p,
                             hz_lookup_table_t *lookup,
                             uint16_t lookup_type,
                             uint16_t subtable_index)
{
    int extension = 0;
    hz_error_t error = HZ_OK;
    uint16_t format;

    extension_label:
    format = hz_parser_read_u16(p);

    switch (lookup_type) {
        case HZ_GSUB_LOOKUP_TYPE_SINGLE_SUBSTITUTION:
            error = hz_read_gsub_single_substitution_subtable(memory_arena, p, lookup, subtable_index, format);
            break;
        case HZ_GSUB_LOOKUP_TYPE_MULTIPLE_SUBSTITUTION:
            error = hz_read_gsub_multiple_substitution_subtable(memory_arena, p, lookup, subtable_index, format);
            break;
        case HZ_GSUB_LOOKUP_TYPE_ALTERNATE_SUBSTITUTION: break;
        case HZ_GSUB_LOOKUP_TYPE_LIGATURE_SUBSTITUTION:
            error = hz_read_gsub_ligature_substitution_subtable(memory_arena, p, lookup, subtable_index, format);
            break;
        case HZ_GSUB_LOOKUP_TYPE_CONTEXTUAL_SUBSTITUTION: break;
        case HZ_GSUB_LOOKUP_TYPE_CHAINED_CONTEXTS_SUBSTITUTION:
            error = hz_read_gsub_chained_contexts_substitution_subtable(memory_arena, p, lookup, subtable_index, format);
            break;
        
        case HZ_GSUB_LOOKUP_TYPE_EXTENSION_SUBSTITUTION: { // implemented inline
            if (format != 1) {
                // error
            }

            lookup_type = lookup->lookup_type = hz_parser_read_u16(p);
            Offset32 extension_offset = hz_parser_read_u32(p);
            hz_parser_push_state(p, extension_offset);
            extension = 1;
            goto extension_label;
        }

        case HZ_GSUB_LOOKUP_TYPE_REVERSE_CHAINING_CONTEXTUAL_SINGLE_SUBSTITUTION: break;

        default: // error
            error = HZ_ERROR_INVALID_LOOKUP_TYPE;
            break;
    }
    
    if (extension)
        hz_parser_pop_state( p );

    return error;
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
    hz_coverage_t mark_coverage;
    hz_coverage_t base_coverage;
    uint16_t mark_class_count;
    hz_mark_array_t mark_array;
    hz_base_array_t base_array;
} hz_mark_to_base_attachment_subtable_t;

HZ_STATIC void hz_load_base_array(hz_memory_arena_t *memory_arena, hz_parser_t *p, hz_base_array_t *base_array, uint16_t mark_class_count)
{
    uint8_t tmp_buffer[4000];
    hz_memory_arena_t tmp_arena = hz_memory_arena_create(tmp_buffer, sizeof tmp_buffer);
    base_array->base_count = hz_parser_read_u16(p);
    base_array->base_records = hz_memory_arena_alloc(memory_arena, base_array->base_count * sizeof(hz_base_record_t));

    for (size_t i = 0; i < base_array->base_count; ++i) {
        hz_base_record_t *record = &base_array->base_records[i];
        record->base_anchors = hz_memory_arena_alloc(memory_arena, mark_class_count * sizeof(hz_anchor_t));
        Offset16 *anchor_offsets = hz_memory_arena_alloc(&tmp_arena, mark_class_count * sizeof(Offset16));
        hz_parser_read_u16_block(p, anchor_offsets, mark_class_count);

        for (size_t j = 0; j < mark_class_count; ++j) {
            if (anchor_offsets[j] > 0) {
                hz_parser_push_state(p, anchor_offsets[j]);
                record->base_anchors[j] = hz_read_anchor(p);
                hz_parser_pop_state(p);
            }
        }

        hz_memory_arena_reset(&tmp_arena);
    }
}

HZ_STATIC void hz_load_mark2_array(hz_memory_arena_t *memory_arena, hz_parser_t *p, hz_mark2_array_t *mark2_array, uint16_t mark_class_count)
{
    char tmp_buffer[4000];
    hz_memory_arena_t tmp_arena = hz_memory_arena_create(tmp_buffer, sizeof tmp_buffer);
    mark2_array->mark2_count = hz_parser_read_u16(p);
    mark2_array->mark2_records = hz_memory_arena_alloc(memory_arena, mark2_array->mark2_count * sizeof(hz_mark2_record_t));
    for (size_t i = 0; i < mark2_array->mark2_count; ++i) {
        hz_mark2_record_t *record = &mark2_array->mark2_records[i];
        record->mark2_anchors = hz_memory_arena_alloc(memory_arena,mark_class_count * sizeof(hz_anchor_t));
        Offset16 *anchor_offsets = hz_memory_arena_alloc(&tmp_arena, mark_class_count * 2);
        hz_parser_read_u16_block(p, anchor_offsets,mark_class_count);

        for (size_t j = 0; j < mark_class_count; ++j) {
            if (anchor_offsets[j] > 0) {
                hz_parser_push_state(p, anchor_offsets[j]);
                record->mark2_anchors[j] = hz_read_anchor(p);
                hz_parser_pop_state(p);
            }
        }

        hz_memory_arena_reset(&tmp_arena);
    }
}

HZ_STATIC void hz_read_mark_array(hz_memory_arena_t *memory_arena, hz_parser_t *p, hz_mark_array_t *mark_array) {
    mark_array->mark_count = hz_parser_read_u16(p);
    mark_array->mark_records = hz_memory_arena_alloc(memory_arena, mark_array->mark_count * sizeof(hz_mark_record_t));

    for (size_t i = 0; i < mark_array->mark_count; ++i) {
        hz_mark_record_t *record = &mark_array->mark_records[i];
        record->mark_class = hz_parser_read_u16(p);
        Offset16 mark_anchor_offset = hz_parser_read_u16(p);
        hz_parser_push_state(p,mark_anchor_offset);
        record->mark_anchor = hz_read_anchor(p);
        hz_parser_pop_state(p);
    }
}

typedef struct {
    uint16_t format;
    hz_coverage_t coverage;
    uint16_t value_format;
    hz_value_record_t value_record;
} hz_single_adjustment_format1_subtable_t;

typedef struct {
    uint16_t format;
    hz_coverage_t coverage;
    uint16_t value_format;
    uint16_t value_count;
    hz_value_record_t *value_records;
} hz_single_adjustment_format2_subtable_t;

HZ_STATIC hz_error_t
hz_read_gpos_single_adjustment_subtable(hz_memory_arena_t *memory_arena,
                                        hz_parser_t *p,
                                        hz_lookup_table_t *lookup,
                                        uint16_t subtable_index,
                                        uint16_t format)
{
    switch (format) {
        case 1: {
            hz_single_adjustment_format1_subtable_t *subtable = hz_memory_arena_alloc(memory_arena,sizeof(*subtable));
            subtable->format = format;
            Offset16 coverage_offset = hz_parser_read_u16(p);
            subtable->value_format = hz_parser_read_u16(p);
            hz_read_value_record(p,&subtable->value_record,subtable->value_format);
            hz_parser_push_state(p, coverage_offset);
            hz_read_coverage(memory_arena,p,&subtable->coverage);
            hz_parser_pop_state(p);
            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }
        case 2: {
            hz_single_adjustment_format2_subtable_t *subtable = hz_memory_arena_alloc(memory_arena,sizeof(*subtable));
            subtable->format = format;
            Offset16 coverage_offset =  hz_parser_read_u16(p);
            subtable->value_format = hz_parser_read_u16(p);
            subtable->value_count = hz_parser_read_u16(p);
            subtable->value_records = hz_memory_arena_alloc(memory_arena,sizeof(hz_value_record_t) * subtable->value_count);

            for (int i = 0; i < subtable->value_count; ++i) {
                hz_read_value_record(p, &subtable->value_records[i], subtable->value_format);
            }

            hz_parser_push_state(p, coverage_offset);
            hz_read_coverage(memory_arena,p,&subtable->coverage);
            hz_parser_pop_state(p);
            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }
        default:
            return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
    }

    return HZ_OK;
}

HZ_STATIC void
hz_read_pair_value_record(hz_parser_t *p,
                          hz_pair_value_record_t *pair_value_record,
                          uint16_t v1, uint16_t v2)
{
    pair_value_record->second_glyph = hz_parser_read_u16(p);
    hz_read_value_record(p, &pair_value_record->value_record1, v1);
    hz_read_value_record(p, &pair_value_record->value_record2, v2);
}

HZ_STATIC void hz_read_pair_set(hz_memory_arena_t *memory_arena, hz_parser_t *p, hz_pair_set_t *pair_set, uint16_t v1, uint16_t v2) {
    pair_set->pair_value_count = hz_parser_read_u16(p);
    pair_set->pair_value_records = hz_memory_arena_alloc(memory_arena, pair_set->pair_value_count * sizeof(hz_pair_value_record_t));
    for (int i = 0; i < pair_set->pair_value_count; ++i) {
        hz_read_pair_value_record(p,&pair_set->pair_value_records[i],v1,v2);
    }
}

HZ_STATIC hz_error_t
hz_read_gpos_pair_adjustment_subtable(hz_memory_arena_t *memory_arena,
                                      hz_parser_t *p,
                                      hz_lookup_table_t *lookup,
                                      uint16_t subtable_index,
                                      uint16_t format)
{
    uint8_t tmp_buffer[4000];
    hz_memory_arena_t tmp_arena = hz_memory_arena_create(tmp_buffer,sizeof tmp_buffer);

    switch (format) {
        case 1: {
            // individual glyphs
            hz_pair_pos_format1_subtable_t *subtable = hz_memory_arena_alloc(memory_arena, sizeof(*subtable));
            subtable->format = format;
            Offset16 coverage_offset = hz_parser_read_u16(p);
            subtable->value_format1 = hz_parser_read_u16(p);
            subtable->value_format2 = hz_parser_read_u16(p);
            subtable->pair_set_count = hz_parser_read_u16(p);

            Offset16 *pair_set_offsets = hz_memory_arena_alloc(&tmp_arena, sizeof(Offset16)*subtable->pair_set_count);
            hz_parser_read_u16_block(p, pair_set_offsets, subtable->pair_set_count);
            subtable->pair_sets = hz_memory_arena_alloc(memory_arena, sizeof(hz_pair_set_t)*subtable->pair_set_count);

            for (int i = 0; i < subtable->pair_set_count; ++i) {
                hz_parser_push_state(p,pair_set_offsets[i]);
                hz_read_pair_set(memory_arena, p,&subtable->pair_sets[i], subtable->value_format1, subtable->value_format2);
                hz_parser_pop_state(p);
            }

            hz_parser_push_state(p,coverage_offset);
            hz_read_coverage(memory_arena,p,&subtable->coverage);
            hz_parser_pop_state(p);

            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }
        case 2: {
            // glyph classes
            hz_pair_pos_format2_subtable_t *subtable = hz_memory_arena_alloc(memory_arena,sizeof(*subtable));
            subtable->format = format;

            Offset16 coverage_offset = hz_parser_read_u16(p);
            
            subtable->value_format1 = hz_parser_read_u16(p);
            subtable->value_format2 = hz_parser_read_u16(p);
            
            Offset16 class_def1_offset = hz_parser_read_u16(p);
            Offset16 class_def2_offset = hz_parser_read_u16(p);

            subtable->class1_count = hz_parser_read_u16(p);
            subtable->class2_count = hz_parser_read_u16(p);

            subtable->class1_records = hz_memory_arena_alloc(memory_arena,sizeof(hz_class1_record_t) * subtable->class1_count);

            for (int i = 0; i < subtable->class1_count; ++i) {
                hz_class1_record_t  *class1_record = &subtable->class1_records[i];
                class1_record->class2_records = hz_memory_arena_alloc(memory_arena, sizeof(hz_class2_record_t) * subtable->class2_count);
                for (int j = 0; j < subtable->class2_count; ++j) {
                    hz_class2_record_t *class2_record = &class1_record->class2_records[j];
                    hz_read_value_record(p,&class2_record->value_record1,subtable->value_format1);
                    hz_read_value_record(p,&class2_record->value_record2,subtable->value_format2);
                }
            }

            hz_parser_push_state(p, coverage_offset);
            hz_read_coverage(memory_arena, p, &subtable->coverage);
            hz_parser_pop_state(p);

            hz_parser_push_state(p, class_def1_offset);
            hz_read_class_def_table(memory_arena, p, &subtable->class_def1);
            hz_parser_pop_state(p);

            hz_parser_push_state(p, class_def2_offset);
            hz_read_class_def_table(memory_arena, p, &subtable->class_def2);
            hz_parser_pop_state(p);
            
            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }
        default:
            return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
    }

    return HZ_OK;
}

HZ_STATIC hz_error_t
hz_read_gpos_mark_to_base_attachment_subtable(hz_memory_arena_t *memory_arena,
                                              hz_parser_t *p,
                                              hz_lookup_table_t *lookup,
                                              uint16_t subtable_index,
                                              uint16_t format)
{
    if (hz_unlikely(format != 1)) {
        // error
        return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
    }

    hz_mark_to_base_attachment_subtable_t *subtable = hz_memory_arena_alloc(memory_arena,sizeof(hz_mark_to_base_attachment_subtable_t));
    subtable->format = format;

    Offset16 mark_coverage_offset = hz_parser_read_u16(p);
    Offset16 base_coverage_offset = hz_parser_read_u16(p);
    subtable->mark_class_count = hz_parser_read_u16(p);
    Offset16 mark_array_offset = hz_parser_read_u16(p);
    Offset16 base_array_offset = hz_parser_read_u16(p);
    
    hz_parser_push_state(p,mark_coverage_offset);
    hz_read_coverage(memory_arena, p, &subtable->mark_coverage);
    hz_parser_pop_state(p);

    hz_parser_push_state(p,base_coverage_offset);
    hz_read_coverage(memory_arena, p, &subtable->base_coverage);
    hz_parser_pop_state(p);

    hz_parser_push_state(p,mark_array_offset);
    hz_read_mark_array(memory_arena, p, &subtable->mark_array);
    hz_parser_pop_state(p);

    hz_parser_push_state(p,base_array_offset);
    hz_load_base_array(memory_arena, p, &subtable->base_array, subtable->mark_class_count);
    hz_parser_pop_state(p);
    
    lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
    return HZ_OK;
}


typedef struct hz_mark_to_mark_attachment_format1_subtable_t {
    uint16_t format;
    hz_coverage_t mark1_coverage;
    hz_coverage_t mark2_coverage;
    uint16_t mark_class_count;
    hz_mark_array_t mark1_array;
    hz_mark2_array_t mark2_array;
} hz_mark_to_mark_attachment_format1_subtable_t;

HZ_STATIC hz_error_t
hz_read_gpos_mark_to_mark_attachment_subtable(hz_memory_arena_t *memory_arena,
                                              hz_parser_t *p,
                                              hz_lookup_table_t *lookup,
                                              uint16_t subtable_index,
                                              uint16_t format)
{
    if (hz_unlikely(format != 1)) {
        // error
        return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
    }

    hz_mark_to_mark_attachment_format1_subtable_t *subtable = hz_memory_arena_alloc(memory_arena, sizeof(*subtable));
    subtable->format = format;
    Offset16 mark1_coverage_offset = hz_parser_read_u16(p);
    Offset16 mark2_coverage_offset = hz_parser_read_u16(p);
    subtable->mark_class_count = hz_parser_read_u16(p);
    Offset16 mark1_array_offset = hz_parser_read_u16(p);
    Offset16 mark2_array_offset = hz_parser_read_u16(p);
    
    hz_parser_push_state(p, mark1_coverage_offset);
    hz_read_coverage(memory_arena, p, &subtable->mark1_coverage);
    hz_parser_pop_state(p);

    hz_parser_push_state(p, mark2_coverage_offset);
    hz_read_coverage(memory_arena, p, &subtable->mark2_coverage);
    hz_parser_pop_state(p);
    
    hz_parser_push_state(p, mark1_array_offset);
    hz_read_mark_array(memory_arena, p,  &subtable->mark1_array);
    hz_parser_pop_state(p);
    
    hz_parser_push_state(p, mark2_array_offset);
    hz_load_mark2_array(memory_arena, p, &subtable->mark2_array, subtable->mark_class_count);
    hz_parser_pop_state(p);

    lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;

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

HZ_STATIC void hz_load_ligature_attachment(hz_memory_arena_t *memory_arena, hz_parser_t *p, uint16_t mark_class_count, hz_ligature_attachment_t *ligature_attachment)
{
    char tmp_buffer[4000];
    hz_memory_arena_t tmp_arena = hz_memory_arena_create(tmp_buffer, sizeof tmp_buffer);

    ligature_attachment->component_count = hz_parser_read_u16(p);
    ligature_attachment->component_records = hz_memory_arena_alloc(memory_arena, sizeof(hz_component_record_t) * ligature_attachment->component_count);
    
    for (uint16_t i = 0; i < ligature_attachment->component_count; ++i) {
        hz_component_record_t *record = &ligature_attachment->component_records[i];
        record->ligature_anchors = hz_memory_arena_alloc(memory_arena, mark_class_count * sizeof(hz_anchor_t));
        Offset16 *offsets = hz_memory_arena_alloc(&tmp_arena, mark_class_count * sizeof(Offset16));
        hz_parser_read_u16_block(p, offsets, mark_class_count);

        for (int j = 0; j < mark_class_count; ++j) {
            if (offsets[j]) {
                hz_parser_push_state(p,offsets[j]);
                record->ligature_anchors[j] = hz_read_anchor(p);
                hz_parser_pop_state(p);
            }
        }
    }
}

HZ_STATIC void
hz_load_ligature_array(hz_memory_arena_t *memory_arena, hz_parser_t *p, uint16_t mark_class_count, hz_ligature_array_t *ligature_array)
{
    char tmp_buffer[4000];
    hz_memory_arena_t tmp_arena = hz_memory_arena_create(tmp_buffer, sizeof tmp_buffer);

    ligature_array->ligature_count = hz_parser_read_u16(p);
    ligature_array->ligature_attachments = hz_memory_arena_alloc(memory_arena, sizeof(hz_ligature_attachment_t) * ligature_array->ligature_count);
    Offset16 *offsets = hz_memory_arena_alloc(&tmp_arena, sizeof(Offset16) * ligature_array->ligature_count);
    hz_parser_read_u16_block(p, offsets, ligature_array->ligature_count);

    for (uint16_t i = 0; i < ligature_array->ligature_count; ++i) {
        hz_parser_push_state(p, offsets[i]);
        hz_load_ligature_attachment(memory_arena, p, mark_class_count, &ligature_array->ligature_attachments[i]);
        hz_parser_pop_state(p);
    }
}

typedef struct {
    uint16_t format;
    hz_coverage_t mark_coverage;
    hz_coverage_t ligature_coverage;
    uint16_t mark_class_count;
    hz_mark_array_t mark_array;
    hz_ligature_array_t ligature_array;
} hz_mark_to_ligature_attachment_format1_subtable_t;

HZ_STATIC hz_error_t
hz_read_gpos_mark_to_ligature_attachment_subtable(hz_memory_arena_t *memory_arena,
                                                  hz_parser_t *p,
                                                  hz_lookup_table_t *lookup,
                                                  uint16_t subtable_index,
                                                  uint16_t format)
{
    switch (format) {
        case 1: {
            hz_mark_to_ligature_attachment_format1_subtable_t *subtable = hz_memory_arena_alloc(memory_arena, sizeof(*subtable));
            subtable->format = format;

            Offset16 mark_coverage_offset = hz_parser_read_u16(p);
            Offset16 ligature_coverage_offset = hz_parser_read_u16(p);
            subtable->mark_class_count = hz_parser_read_u16(p);
            Offset16 mark_array_offset = hz_parser_read_u16(p);
            Offset16 ligature_array_offset = hz_parser_read_u16(p);


            hz_parser_push_state(p,mark_coverage_offset);
            hz_read_coverage(memory_arena, p, &subtable->mark_coverage);
            hz_parser_pop_state(p);

            hz_parser_push_state(p,ligature_coverage_offset);
            hz_read_coverage(memory_arena, p, &subtable->ligature_coverage);
            hz_parser_pop_state(p);

            hz_parser_push_state(p,mark_array_offset);
            hz_read_mark_array(memory_arena, p, &subtable->mark_array);
            hz_parser_pop_state(p);

            hz_parser_push_state(p,ligature_array_offset);
            hz_load_ligature_array(memory_arena, p, subtable->mark_class_count, &subtable->ligature_array);
            hz_parser_pop_state(p);

            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }

        default: return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
    }

    return HZ_OK;
}

HZ_STATIC hz_error_t
hz_load_gpos_chained_context_positioning_subtable(hz_memory_arena_t *memory_arena,
                                                  hz_parser_t *p,
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
            hz_chained_sequence_context_format3_subtable_t *subtable = hz_memory_arena_alloc(memory_arena, sizeof(*subtable));
            subtable->format = format;
            hz_ot_load_chained_sequence_context_format3_subtable(memory_arena, p, subtable);
            lookup->subtables[subtable_index] = (hz_lookup_subtable_t *)subtable;
            break;
        }

        default:
            return HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT;
    }

    return HZ_OK;
}

HZ_STATIC hz_error_t
hz_read_gpos_lookup_subtable(hz_memory_arena_t *memory_arena,
                             hz_parser_t *p,
                             hz_lookup_table_t *lookup,
                             uint16_t lookup_type,
                             uint16_t subtable_index)
 {
    int extension = 0;
    hz_error_t error = HZ_OK;
    uint16_t format;

    extension_label:
    format = hz_parser_read_u16(p);

    switch (lookup_type) {
        case HZ_GPOS_LOOKUP_TYPE_SINGLE_ADJUSTMENT:
            error = hz_read_gpos_single_adjustment_subtable(memory_arena, p, lookup, subtable_index, format);
            break;
        case HZ_GPOS_LOOKUP_TYPE_PAIR_ADJUSTMENT:
            error = hz_read_gpos_pair_adjustment_subtable(memory_arena, p, lookup, subtable_index, format);
            break;

        case HZ_GPOS_LOOKUP_TYPE_CURSIVE_ATTACHMENT:
            // error = hz_read_gpos_cursive_attachment_subtable(alctr, p, lookup, subtable_index, format);
            break;

        case HZ_GPOS_LOOKUP_TYPE_MARK_TO_BASE_ATTACHMENT:
            error = hz_read_gpos_mark_to_base_attachment_subtable(memory_arena, p, lookup, subtable_index, format);
            break;

        case HZ_GPOS_LOOKUP_TYPE_MARK_TO_LIGATURE_ATTACHMENT:
            error = hz_read_gpos_mark_to_ligature_attachment_subtable(memory_arena, p, lookup, subtable_index, format);
            break;

        case HZ_GPOS_LOOKUP_TYPE_MARK_TO_MARK_ATTACHMENT:
            error = hz_read_gpos_mark_to_mark_attachment_subtable(memory_arena, p, lookup, subtable_index, format);
            break;

        case HZ_GPOS_LOOKUP_TYPE_CONTEXT_POSITIONING: {
            break;
        }

        case HZ_GPOS_LOOKUP_TYPE_CHAINED_CONTEXT_POSITIONING: {
            error = hz_load_gpos_chained_context_positioning_subtable(memory_arena,p, lookup, subtable_index, format);
            break;
        }
        
        case HZ_GPOS_LOOKUP_TYPE_EXTENSION_POSITIONING: { // implemented inline
            if (format != 1) {
                // error
            }

            lookup_type = lookup->lookup_type = hz_parser_read_u16(p);
            Offset32 extension_offset = hz_parser_read_u32(p);
            hz_parser_push_state(p, extension_offset);
            extension = 1;
            goto extension_label;
        }

        default:
            hz_unreachable();
    }

    if (extension)
        hz_parser_pop_state( p );

    return error;
}

HZ_STATIC hz_error_t
hz_load_gsub_lookup_table(hz_memory_arena_t *memory_arena,
                          hz_parser_t *p,
                          hz_face_t *face,
                          hz_lookup_table_t *table)
{
    table->lookup_type = hz_parser_read_u16(p);
    table->lookup_flags = hz_parser_read_u16(p);
    table->subtable_count = hz_parser_read_u16(p);

    if (table->subtable_count > 0) {
        Offset16 *offsets = hz_malloc(table->subtable_count * sizeof(Offset16));
        hz_parser_read_u16_block(p, offsets, table->subtable_count);

        // set pointers to NULL by default
        table->subtables = hz_memory_arena_alloc(memory_arena, SIZEOF_VOIDPTR * table->subtable_count);
        hz_zero(table->subtables, sizeof(void*) * table->subtable_count); // null-out the subtale pointers

        // load & parse subtables
        uint16_t original_lookup_type = table->lookup_type;
        for (uint16_t i = 0; i < table->subtable_count; ++i) {
            hz_parser_push_state(p, offsets[i]);
            hz_read_gsub_lookup_subtable(memory_arena, p, table, original_lookup_type, i);
            hz_parser_pop_state(p);
        }

        hz_free(offsets);
    }

    // Only include mark filtering set if flag USE_MARK_FILTERING_SET is enabled
    if (table->lookup_flags & HZ_LOOKUP_FLAG_USE_MARK_FILTERING_SET) {
        uint16_t mark_filtering_set_index = hz_parser_read_u16(p);
        table->mark_filtering_set = &face->mark_glyph_set[mark_filtering_set_index];
    }

    return HZ_OK;
}

HZ_STATIC hz_error_t
hz_load_gpos_lookup_table(hz_memory_arena_t *memory_arena,
                          hz_parser_t *p,
                          hz_face_t *face,
                          hz_lookup_table_t *table)
{
    uint8_t tmp_buffer[4000];
    hz_memory_arena_t tmp_arena = hz_memory_arena_create(tmp_buffer, sizeof tmp_buffer);
    table->lookup_type = hz_parser_read_u16(p);
    table->lookup_flags = hz_parser_read_u16(p);
    table->subtable_count = hz_parser_read_u16(p);

    if (table->subtable_count > 0) {
        Offset16 *offsets = hz_memory_arena_alloc(&tmp_arena, table->subtable_count * sizeof(Offset16));
        hz_parser_read_u16_block(p, offsets, table->subtable_count);

        // Set pointers to NULL by default
        table->subtables = hz_memory_arena_alloc(memory_arena, SIZEOF_VOIDPTR * table->subtable_count);
        hz_zero(table->subtables, SIZEOF_VOIDPTR * table->subtable_count); // null-out the subtale pointers

        // Load & parse subtables
        uint16_t original_lookup_type = table->lookup_type;
        for (uint16_t i = 0; i < table->subtable_count; ++i) {
            hz_parser_push_state(p, offsets[i]);
            hz_read_gpos_lookup_subtable(memory_arena, p, table, original_lookup_type, i);
            hz_parser_pop_state(p);
        }
    }

    // Only include mark filtering set if flag USE_MARK_FILTERING_SET is enabled
    if (table->lookup_flags & HZ_LOOKUP_FLAG_USE_MARK_FILTERING_SET) {
        uint16_t mark_filtering_set_index = hz_parser_read_u16(p);
        table->mark_filtering_set = &face->mark_glyph_set[mark_filtering_set_index];
    }

    return HZ_OK;
}

HZ_STATIC hz_error_t hz_load_gsub_table(hz_parser_t *p, hz_font_data_t *font_data)
{
    char tmp[4096];
    hz_memory_arena_t arena_tmp = hz_memory_arena_create(tmp, sizeof tmp);
    hz_face_t *face = font_data->face; 

    if (!face->gsub) {
        return HZ_ERROR_TABLE_DOES_NOT_EXIST;
    }

    hz_parser_push_state(p, face->gsub);
    hz_gsub_table_t *gsub_table = &font_data->gsub_table;

    struct { uint16_t script_list_offset,
                      feature_list_offset,
                      lookup_list_offset,
                      feature_variations_offset; } hdr;

    gsub_table->version = hz_parser_read_u32(p);

    switch (gsub_table->version) {
    default: return HZ_ERROR_INVALID_TABLE_VERSION;
    case 0x00010000: // 1.0
        hz_cmdread(p, 1, &hdr, "www");
        break;
    case 0x00010001: // 1.1
        hz_cmdread(p, 1, &hdr, "wwww");
        break;
    }

    {
        // parse feature list table
        hz_parser_push_state(p, hdr.feature_list_offset);
        gsub_table->num_features = hz_parser_read_u16(p);
        gsub_table->features = hz_memory_arena_alloc(&font_data->memory_arena, sizeof(hz_feature_list_item_t) * gsub_table->num_features);
        
        for (int i = 0; i < gsub_table->num_features; ++i) {
            hz_feature_list_item_t *it = &gsub_table->features[i];
            it->tag = hz_parser_read_u32(p);
            Offset16 offset = hz_parser_read_u16(p);
            hz_parser_push_state(p, offset);
            hz_load_feature_table(&font_data->memory_arena, p, &it->table);
            hz_parser_pop_state(p);
        }

        hz_parser_pop_state(p);
    }

    {
        // parse lookup list table
        hz_parser_push_state(p, hdr.lookup_list_offset);
        gsub_table->num_lookups = hz_parser_read_u16(p);
        gsub_table->lookups = hz_memory_arena_alloc(&font_data->memory_arena, sizeof(hz_lookup_table_t) * gsub_table->num_lookups);
        Offset16* offsets = hz_memory_arena_alloc(&arena_tmp, sizeof(Offset16) * gsub_table->num_lookups);
        hz_parser_read_u16_block(p, offsets, gsub_table->num_lookups);

        for (uint16_t i = 0; i < gsub_table->num_lookups; ++i) {
            hz_parser_push_state(p, offsets[i]);
            hz_load_gsub_lookup_table(&font_data->memory_arena, p, face, &gsub_table->lookups[i]);
            hz_parser_pop_state(p);
        }

        hz_parser_pop_state(p);
    }

    hz_parser_pop_state(p);

    return HZ_OK;
}


HZ_STATIC hz_error_t hz_load_gpos_table(hz_parser_t *p, hz_font_data_t *font_data)
{
    char tmp[5000];
    hz_memory_arena_t tmp_arena = hz_memory_arena_create(tmp, sizeof tmp);
    hz_face_t *face = font_data->face;

    if (!face->gpos) {
         return HZ_ERROR_TABLE_DOES_NOT_EXIST;
    }

    hz_parser_push_state(p, face->gpos);
    hz_gpos_table_t *gpos_table = &font_data->gpos_table;

    struct { uint16_t script_list_offset,
                      feature_list_offset,
                      lookup_list_offset,
                      feature_variations_offset; } hdr;
    
    gpos_table->version = hz_parser_read_u32(p);

    switch (gpos_table->version) {
    case 0x00010000: // 1.0
        hz_cmdread(p, 1, &hdr, "www");
        break;
    case 0x00010001: // 1.1
        hz_cmdread(p, 1, &hdr, "wwww");
        break;
    default: // error
        return HZ_ERROR_INVALID_TABLE_VERSION;
    }

    {
        // parse feature list table
        hz_parser_push_state(p, hdr.feature_list_offset);
        gpos_table->num_features = hz_parser_read_u16(p);
        gpos_table->features = hz_memory_arena_alloc(&font_data->memory_arena, sizeof(hz_feature_list_item_t) * gpos_table->num_features);
        
        for (int i = 0; i < gpos_table->num_features; ++i) {
            hz_feature_list_item_t *it = &gpos_table->features[i];
            it->tag = hz_parser_read_u32(p);
            Offset16 offset = hz_parser_read_u16(p);
            hz_parser_push_state(p, offset);
            hz_load_feature_table(&font_data->memory_arena, p, &it->table);
            hz_parser_pop_state(p);
        }

        hz_parser_pop_state(p);
    }

    {
        // parse lookup list table
        hz_parser_push_state(p, hdr.lookup_list_offset);
        gpos_table->num_lookups = hz_parser_read_u16(p);
        gpos_table->lookups = hz_memory_arena_alloc(&font_data->memory_arena, sizeof(hz_lookup_table_t) * gpos_table->num_lookups);
        Offset16* offsets = hz_memory_arena_alloc(&tmp_arena, sizeof(Offset16) * gpos_table->num_lookups);
        hz_parser_read_u16_block(p, offsets, gpos_table->num_lookups);

        for (uint16_t i = 0; i < gpos_table->num_lookups; ++i) {
            hz_parser_push_state(p, offsets[i]);
            hz_load_gpos_lookup_table(&font_data->memory_arena, p, face, &gpos_table->lookups[i]);
            hz_parser_pop_state(p);
        }

        hz_parser_pop_state(p);
    }

    hz_parser_pop_state(p);

    return HZ_OK;
}

HZ_STATIC void hz_font_data_load(hz_font_data_t *fd, hz_font_t *font) {
    hz_parser_t p;
    hz_parser_init(&p, font->face->data);
    hz_memory_arena_reset(&fd->memory_arena); /* reset arena before parsing new font */
    fd->face = font->face;

    hz_load_gsub_table(&p, fd);
    hz_load_gpos_table(&p, fd);
    
    hz_parser_deinit(&p);
}

hz_font_data_t *hz_font_data_create(hz_font_t* font) {
    const size_t arena_size = HZ_DEFAULT_FONT_DATA_ARENA_SIZE;
    hz_font_data_t* fd = hz_malloc(sizeof(*fd));
    *fd = (hz_font_data_t){
        .memory_arena_data = hz_malloc(arena_size),
        .allocator = (hz_allocator_t){
            .allocfn = hz_memory_arena_alloc_fn,
            .user = &fd->memory_arena
        }
    };

    hz_memory_arena_init(&fd->memory_arena, fd->memory_arena_data, arena_size);
    hz_font_data_load(fd, font);
    return fd;
}

void hz_font_data_release(hz_font_data_t *fd){
    hz_free(fd->memory_arena_data);
    hz_free(fd);
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
    uint32_t prev_joining, curr_joining, next_joining;
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
    triplet.curr_joining = hz_ucd_get_arabic_joining_data(buffer->codepoints[index]);
    if (triplet.curr_joining != (HZ_JOINING_GROUP_NONE | HZ_JOINING_TYPE_U)) {
        int64_t prev_index = prev_joining_arabic_glyph(buffer, index, HZ_LOOKUP_FLAG_IGNORE_MARKS, mark_filtering_set);
        int64_t next_index = next_joining_arabic_glyph(buffer, index, HZ_LOOKUP_FLAG_IGNORE_MARKS, mark_filtering_set);

        if (prev_index == -1)
            triplet.prev_joining = HZ_JOINING_GROUP_NONE | HZ_JOINING_TYPE_T;
        else
            triplet.prev_joining = hz_ucd_get_arabic_joining_data(buffer->codepoints[prev_index]);

        if (next_index == -1)
            triplet.next_joining = HZ_JOINING_GROUP_NONE | HZ_JOINING_TYPE_T;
        else
            triplet.next_joining = hz_ucd_get_arabic_joining_data(buffer->codepoints[next_index]);

        triplet.init = triplet.curr_joining & (HZ_JOINING_TYPE_L | HZ_JOINING_TYPE_D)
                && triplet.next_joining & (HZ_JOINING_TYPE_R | HZ_JOINING_TYPE_D | HZ_JOINING_TYPE_C);

        triplet.fina = triplet.curr_joining & (HZ_JOINING_TYPE_R | HZ_JOINING_TYPE_D)
                && triplet.prev_joining & (HZ_JOINING_TYPE_L | HZ_JOINING_TYPE_D | HZ_JOINING_TYPE_C);

        triplet.medi = triplet.curr_joining & HZ_JOINING_TYPE_D
                && triplet.prev_joining & (HZ_JOINING_TYPE_L | HZ_JOINING_TYPE_C | HZ_JOINING_TYPE_D)
                && triplet.next_joining & (HZ_JOINING_TYPE_R | HZ_JOINING_TYPE_C | HZ_JOINING_TYPE_D);

        triplet.does_apply = 1;
    } else {
        triplet.does_apply = 0;
    }

    return triplet;
}

HZ_STATIC hz_bool hz_shape_complex_arabic_init(hz_buffer_t *buffer, int64_t index, uint16_t lookup_flag, const hz_coverage_t *mark_filtering_set)
{
    hz_arabic_joining_triplet_t triplet = hz_shape_complex_arabic_joining(buffer,index,lookup_flag, mark_filtering_set);
    return triplet.does_apply ? triplet.init && !(triplet.medi || triplet.fina) : HZ_FALSE;
}

HZ_STATIC hz_bool hz_shape_complex_arabic_medi(hz_buffer_t *buffer, int64_t index, uint16_t lookup_flag, const hz_coverage_t *mark_filtering_set)
{
    hz_arabic_joining_triplet_t triplet = hz_shape_complex_arabic_joining(buffer,index,lookup_flag, mark_filtering_set);
    return triplet.does_apply ? triplet.medi : HZ_FALSE;
}

HZ_STATIC hz_bool hz_shape_complex_arabic_fina(hz_buffer_t *buffer, int64_t index, uint16_t lookup_flag, const hz_coverage_t *mark_filtering_set)
{
    hz_arabic_joining_triplet_t triplet = hz_shape_complex_arabic_joining(buffer,index,lookup_flag,mark_filtering_set);
    return triplet.does_apply ? triplet.fina && !(triplet.medi || triplet.init) : HZ_FALSE;
}

HZ_STATIC hz_bool hz_shape_complex_arabic_isol(hz_buffer_t *buffer, int64_t index, uint16_t lookup_flag, const hz_coverage_t *mark_filtering_set)
{
    hz_arabic_joining_triplet_t triplet = hz_shape_complex_arabic_joining(buffer,index,lookup_flag,mark_filtering_set);
    return triplet.does_apply ? !(triplet.fina || triplet.medi || triplet.init) : HZ_FALSE;
}

HZ_STATIC hz_bool
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
    hz_bool is_ignored;
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
        hz_bool curr_ign = hz_should_ignore_glyph(buffer, i, lookup_flag, mark_filtering_set);
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

        hz_bool next_ign = hz_should_ignore_glyph(buffer, i + 1, lookup_flag, mark_filtering_set);

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

HZ_STATIC void
hz_shaper_apply_gsub_lookup(hz_shaper_t *shaper,
                            hz_font_data_t *font_data,
                            hz_feature_t feature,
                            uint16_t lookup_index,
                            hz_buffer_t *in, hz_buffer_t *out,
                            int v1, int v2, int depth)
{
    char tmp[8192];
    hz_memory_arena_t arena = hz_memory_arena_create(tmp, sizeof tmp);

    HZ_ASSERT(in != NULL && out != NULL);
    HZ_ASSERT(in != out);
    HZ_ASSERT(hz_buffer_contains_range(in,v1,v2));

    if (depth >= HZ_MAX_RECURSE_DEPTH) {
        // hit max recurse depth, wind back up the call stack
        return;
    }

    hz_gsub_table_t *gsub_table = &font_data->gsub_table;
    hz_lookup_table_t *table = &gsub_table->lookups[lookup_index];
    hz_face_t *face = font_data->face;

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
        if (base == NULL) continue;
        
        // subtable requested is loaded
        hz_memory_arena_reset(&arena);
        hz_buffer_compute_info(b1, face);

        // reserve second buffer with size of first buffer as the result of the substitution is likely going to be
        // around the size of the first buffer in most cases.
        hz_buffer_reserve(b2, hz_vector_size(b1->glyph_indices));
        hz_range_list_t *range_list = hz_compute_range_list(b1, table->lookup_flags, table->mark_filtering_set);

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
                                    if (hz_should_replace(b1, feature, g, table->lookup_flags, table->mark_filtering_set)
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
                                    if (hz_should_replace(b1, feature, g, table->lookup_flags, table->mark_filtering_set)
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
                                    if (hz_should_replace(b1, feature, g, table->lookup_flags, table->mark_filtering_set)
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
                                    hz_bool matched = HZ_FALSE;

                                    int32_t index;
                                    if (hz_should_replace(b1, feature, g, table->lookup_flags, table->mark_filtering_set)
                                        && (index = hz_coverage_search(&subtable->coverage, b1->glyph_indices[g])) != -1) {
                                        const hz_ligature_set_table_t *ligature_set = subtable->ligature_sets + index;

                                        // compare ligatures in ligature set to following unignored glyphs for match
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
                                                    hz_index_t *block = hz_memory_arena_alloc(&arena, (component_count - 1) * 2);
                                                    
                                                    for (uint16_t k = 0; k < component_count-1; ++k) {
                                                        block[k] = b1->glyph_indices[range_list->unignored_indices[s1 + k + 1]];
                                                    }

                                                    test = !memcmp(ligature->component_glyph_ids, block, (component_count-1)*2);
                                                }

                                                if (test) {
                                                    // GID match found with ligature, push ligature glyph to buffer
                                                    hz_buffer_add_glyph(b2, (hz_glyph_object_t) {
                                                            .id = ligature->ligature_glyph,
                                                            .codepoint = 0,
                                                            .component_index = b1->component_indices[g]});

                                                    // Push ignored glyphs found within the matched range
                                                    for (int k = s1; k < s2; ++k) {
                                                        int min_index = range_list->unignored_indices[k];
                                                        int max_index = range_list->unignored_indices[k+1];
                                                        for (int m = min_index+1; m <= max_index-1; ++m) {
                                                            hz_buffer_add_glyph(b2, (hz_glyph_object_t) {
                                                                .id = b1->glyph_indices[m],
                                                                .codepoint = b1->codepoints[m],
                                                                .component_index = k-s1});
                                                        }
                                                    }

                                                    // Jump over context
                                                    g = range_list->unignored_indices[s2];
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
                                    hz_bool match = HZ_FALSE;
                                    if (hz_should_replace(b1, feature, g, table->lookup_flags, table->mark_filtering_set)
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
                                                    uint16_t *sequence = hz_memory_arena_alloc(&arena, context_len * 2);
                                                    uint16_t *context = hz_memory_arena_alloc(&arena, context_len * 2);
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
                                                            int *context_index_list = hz_buffer_get_unignored_indices(ctx1, table->lookup_flags, table->mark_filtering_set);
                                                            int sequence_idx = context_index_list[rule->lookup_records[z].sequence_index];

                                                            hz_shaper_apply_gsub_lookup(shaper, font_data, feature,
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
                                    hz_bool match = HZ_FALSE;
                                    if (hz_should_replace(b1, feature, g, table->lookup_flags, table->mark_filtering_set)) {
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
                                                if (!hz_coverage_contains(&subtable->prefix_coverages[k],
                                                                        b1->glyph_indices[range_list->unignored_indices[
                                                                                u - (k + 1)]])) {
                                                    prefix_match = 0;
                                                    break;
                                                }
                                            }

                                            // input
                                            for (int k = 0; k < subtable->input_count; ++k) {
                                                if (!hz_coverage_contains(&subtable->input_coverages[k],
                                                                        b1->glyph_indices[range_list->unignored_indices[
                                                                                u + k]])) {
                                                    input_match = 0;
                                                    break;
                                                }
                                            }

                                            // suffix
                                            for (int k = 0; k < subtable->suffix_count; ++k) {
                                                if (!hz_coverage_contains(&subtable->suffix_coverages[k],
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
                                                    hz_vector(int) context_index_list = hz_buffer_get_unignored_indices(ctx1, table->lookup_flags, table->mark_filtering_set);
                                                    int sequence_idx = context_index_list[subtable->lookup_records[z].sequence_index];
                                                    hz_vector_destroy(context_index_list);

                                                    hz_shaper_apply_gsub_lookup(shaper, font_data, feature,
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
            
            default:
                continue;
        }
        // move glyphs from source buffer to destination buffer
        hz_swap_buffers(b1, b2, face);
        hz_range_list_destroy(range_list);
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
        metrics->yAdvance  += value_record->yAdvance;
    if (value_format & HZ_VALUE_FORMAT_X_PLACEMENT)
        metrics->xOffset += value_record->xPlacement;
    if (value_format & HZ_VALUE_FORMAT_Y_PLACEMENT)
        metrics->yOffset += value_record->yPlacement;
}

void
hz_shaper_apply_gpos_lookup(hz_shaper_t *shaper,
                            hz_font_data_t *font_data,
                            hz_feature_t feature,
                            uint16_t lookup_index,
                            hz_buffer_t *in, hz_buffer_t *out,
                            int v1, int v2, int depth)
{
    char tmp[4096];
    hz_memory_arena_t arena = hz_memory_arena_create(tmp, sizeof tmp);

    HZ_ASSERT(in != NULL && out != NULL);
    HZ_ASSERT(in != out);
    HZ_ASSERT(hz_buffer_contains_range(in,v1,v2));

    if (depth >= HZ_MAX_RECURSE_DEPTH) {
        // hit max recurse depth, wind back up the call stack
        return;
    }

    const hz_lookup_table_t *table = &font_data->gpos_table.lookups[lookup_index];
    hz_face_t *face = font_data->face;

    // copy segment glyph ids and info into a read-only buffer
    hz_buffer_t *b1, *b2;
    b1 = hz_buffer_create();
    b1->attrib_flags = in->attrib_flags;
    b2 = hz_buffer_create();
    b2->attrib_flags = HZ_GLYPH_ATTRIB_METRICS_BIT;
    hz_buffer_add_range(b1, in, v1, v2);

    for (uint16_t i = 0; i < table->subtable_count; ++i) {
        hz_lookup_subtable_t *base = table->subtables[i];
        if (base == NULL) continue;
        // subtable requested is loaded
        hz_memory_arena_reset(&arena);
        hz_buffer_compute_info(b1, face);
        hz_range_list_t *range_list = hz_compute_range_list(b1, table->lookup_flags, table->mark_filtering_set);

        switch (table->lookup_type) {
            case HZ_GPOS_LOOKUP_TYPE_SINGLE_ADJUSTMENT: {
                switch (base->format) {
                    case 1: {
                        hz_single_adjustment_format1_subtable_t *subtable = (hz_single_adjustment_format1_subtable_t *)base;
                        for (size_t r = 0; r < hz_vector_size(range_list->ranges); ++r) {
                            hz_range_t *range = &range_list->ranges[r];

                            if (range->is_ignored) {
                                hz_buffer_add_range(b2, b1, range->mn, range->mx);
                            } else {
                                for (hz_segment_sz_t g = range->mn; g <= range->mx; ++g) {
                                    hz_glyph_metrics_t metrics = b1->glyph_metrics[g];
                                    if (hz_should_replace(b1, feature, g, table->lookup_flags, table->mark_filtering_set) &&
                                        hz_coverage_contains(&subtable->coverage, b1->glyph_indices[g])) {
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
                        for (size_t r = 0; r < hz_vector_size(range_list->ranges); ++r) {
                            hz_range_t *range = &range_list->ranges[r];

                            if (range->is_ignored) {
                                hz_buffer_add_range(b2, b1, range->mn, range->mx);
                            } else {
                                for (hz_segment_sz_t g = range->mn; g <= range->mx; ++g) {
                                    hz_glyph_metrics_t metrics = b1->glyph_metrics[g];

                                    int32_t record_index;
                                    if (hz_should_replace(b1, feature, g, table->lookup_flags, table->mark_filtering_set) &&
                                        (record_index = hz_coverage_search(&subtable->coverage, b1->glyph_indices[g]))!= -1 ) {
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
                        for (size_t r = 0; r < hz_vector_size(range_list->ranges); ++r) {
                            hz_range_t *range = &range_list->ranges[r];

                            if (range->is_ignored) {
                                hz_buffer_add_range(b2, b1, range->mn, range->mx);
                            } else {
                                for (hz_segment_sz_t g = range->mn; g <= range->mx; ++g) {
                                    int32_t cov_index;
                                    if (hz_should_replace(b1, feature, g, table->lookup_flags, table->mark_filtering_set)
                                        && (cov_index = hz_coverage_search(&subtable->coverage, b1->glyph_indices[g])) != -1) {
                                        hz_segment_sz_t base_idx = range->base + (g - range->mn);

                                        if (base_idx+1 < hz_vector_size(range_list->unignored_indices)) {
                                            hz_segment_sz_t g2 = range_list->unignored_indices[base_idx + 1];
                                            hz_pair_set_t *pair_set = &subtable->pair_sets[cov_index];

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
                        for (size_t r = 0; r < hz_vector_size(range_list->ranges); ++r) {
                            hz_range_t *range = &range_list->ranges[r];

                            if (range->is_ignored) {
                                hz_buffer_add_range(b2, b1, range->mn, range->mx);
                            } else {
                                // unignored
                                for (hz_segment_sz_t g = range->mn; g <= range->mx; ++g) {
                                    
                                    int32_t class1_index, class2_index;
                                    if (hz_should_replace(b1, feature, g, table->lookup_flags, table->mark_filtering_set)
                                        && (class1_index = hz_coverage_search(&subtable->coverage, b1->glyph_indices[g])) != -1) {
                                        hz_segment_sz_t base_idx = range->base + (g - range->mn);

                                        if (base_idx+1 < hz_vector_size(range_list->unignored_indices)) {
                                            hz_segment_sz_t g2 = range_list->unignored_indices[base_idx + 1];
                
                                            if ((class1_index = hz_class_def_search(&subtable->class_def1, b1->glyph_classes[g])) != -1
                                                && (class2_index = hz_class_def_search(&subtable->class_def2, b1->glyph_classes[g2])) != -1) {

                                                const hz_class2_record_t *class2_record = &subtable->class1_records[class1_index].class2_records[class2_index];
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
            case HZ_GPOS_LOOKUP_TYPE_CURSIVE_ATTACHMENT: {
                break;
            }
            case HZ_GPOS_LOOKUP_TYPE_MARK_TO_BASE_ATTACHMENT: {
                switch (base->format) {
                    case 1: {
                        hz_mark_to_base_attachment_subtable_t *subtable = (hz_mark_to_base_attachment_subtable_t *)base;

                        for (size_t g = 0; g < b1->glyph_count; ++g) {
                            hz_glyph_metrics_t metrics = b1->glyph_metrics[g];

                            int32_t mark_index, base_index;
                            if (!hz_should_ignore_glyph(b1, g, table->lookup_flags, table->mark_filtering_set)
                               && (mark_index = hz_coverage_search(&subtable->mark_coverage, b1->glyph_indices[g])) != -1)
                            {
                                hz_segment_sz_t prev_base = hz_search_prev_glyph(b1, g, HZ_GLYPH_CLASS_MARK, HZ_GLYPH_CLASS_BASE);
                                if (prev_base != -1 && (base_index = hz_coverage_search(&subtable->base_coverage, b1->glyph_indices[prev_base])) != -1) {
                                    // both coverages match
                                    hz_mark_record_t *mark_record = &subtable->mark_array.mark_records[mark_index];
                                    hz_anchor_t *base_anchor = &subtable->base_array.base_records[base_index].base_anchors[mark_record->mark_class];
                                    hz_anchor_t *mark_anchor = &mark_record->mark_anchor;

                                    hz_glyph_metrics_t base_metrics = b1->glyph_metrics[prev_base];
                                    int32_t placement_x1 = mark_anchor->x_coord - base_metrics.xOffset;
                                    int32_t placement_y1 = mark_anchor->y_coord - base_metrics.yOffset;
                                    int32_t placement_x2 = base_anchor->x_coord + base_metrics.xOffset;
                                    int32_t placement_y2 = base_anchor->y_coord + base_metrics.yOffset;

                                    int32_t xOffset = placement_x2 - placement_x1;
                                    int32_t yOffset = placement_y2 - placement_y1;

                                    metrics.xOffset = xOffset;
                                    metrics.yOffset = yOffset;
                                }
                            }

                            hz_buffer_add_glyph(b2, (hz_glyph_object_t) {
                                    .metrics = metrics
                            });
                        }

                        break;
                    }
                }
                break;
            }

            case HZ_GPOS_LOOKUP_TYPE_MARK_TO_LIGATURE_ATTACHMENT: {
                hz_mark_to_ligature_attachment_format1_subtable_t *subtable = (hz_mark_to_ligature_attachment_format1_subtable_t *)base;
                for (size_t r = 0; r < hz_vector_size(range_list->ranges); ++r) {
                    hz_range_t *range = &range_list->ranges[r];
                    if (range->is_ignored) {
                        hz_buffer_add_range(b2, b1, range->mn, range->mx);
                    } else {
                        for (hz_segment_sz_t g = range->mn; g <= range->mx; ++g) {
                            hz_glyph_metrics_t metrics = b1->glyph_metrics[g];

                            int32_t cov_index1, cov_index2;
                            if (hz_should_replace(b1, feature, g, table->lookup_flags, table->mark_filtering_set)
                                && (cov_index1 = hz_coverage_search(&subtable->mark_coverage, b1->glyph_indices[g])) != -1) {
                                int prev_ligature = hz_search_prev_glyph(b1, g, HZ_GLYPH_CLASS_MARK, HZ_GLYPH_CLASS_LIGATURE);
                                if (prev_ligature != -1) {
                                    if ((cov_index2 = hz_coverage_search(&subtable->ligature_coverage, b1->glyph_indices[prev_ligature])) != -1) {
                                        // both coverages match
                                        uint16_t component_index = b1->component_indices[g];
                                        uint16_t attachment_class = b1->attachment_classes[g];
                                        
                                        uint16_t mark_index = cov_index1;
                                        uint16_t ligature_index = cov_index2;
                                        hz_mark_record_t *mark_record = &subtable->mark_array.mark_records[mark_index];
                                        hz_ligature_attachment_t *ligature_attachment = &subtable->ligature_array.ligature_attachments[ligature_index];
                                        hz_component_record_t *component = &ligature_attachment->component_records[component_index];
                                        hz_anchor_t *mark_anchor = &mark_record->mark_anchor;
                                        hz_anchor_t *ligature_anchor = &component->ligature_anchors[mark_record->mark_class];

                                        hz_glyph_metrics_t lig_metrics = b1->glyph_metrics[prev_ligature];
                                        int32_t placement_x1 = mark_anchor->x_coord;
                                        int32_t placement_y1 = mark_anchor->y_coord;
                                        int32_t placement_x2 = ligature_anchor->x_coord + lig_metrics.xOffset;
                                        int32_t placement_y2 = ligature_anchor->y_coord + lig_metrics.yOffset;// + lig_metrics.yAdvance;// - lig_metrics.yAdvance;

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
                        for (size_t r = 0; r < hz_vector_size(range_list->ranges); ++r) {
                            hz_range_t *range = &range_list->ranges[r];

                            if (range->is_ignored) {
                                hz_buffer_add_range(b2, b1, range->mn, range->mx);
                            } else {
                                for (hz_segment_sz_t g = range->mn; g <= range->mx; ++g) {
                                    hz_glyph_metrics_t metrics = b1->glyph_metrics[g];
                                    int32_t mark1_index, mark2_index;
                                    if (hz_should_replace(b1, feature, g, table->lookup_flags, table->mark_filtering_set)) {
                                        int prev_mark = hz_search_prev_glyph(b1, g, HZ_GLYPH_CLASS_MARK, HZ_GLYPH_CLASS_MARK);//range->base + (g - range->mn) - 1;
                                        if (prev_mark != -1) {
                                            if ((mark2_index = hz_coverage_search(&subtable->mark2_coverage, b1->glyph_indices[prev_mark])) != -1 &&
                                                ( mark1_index = hz_coverage_search(&subtable->mark1_coverage, b1->glyph_indices[g])) != -1 &&
                                                (b1->component_indices[g] == b1->component_indices[prev_mark]))
                                            {
                                                // valid second mark found
                                                hz_mark_record_t *mark_record = &subtable->mark1_array.mark_records[mark1_index];
                                                hz_mark2_record_t *mark2_record = &subtable->mark2_array.mark2_records[mark2_index];
                                                hz_anchor_t *base_anchor = &mark2_record->mark2_anchors[mark_record->mark_class];
                                                hz_anchor_t *mark_anchor = &mark_record->mark_anchor;

                                                hz_glyph_metrics_t base_metrics = b1->glyph_metrics[prev_mark];
                                                int32_t placement_x1 = mark_anchor->x_coord;
                                                int32_t placement_y1 = mark_anchor->y_coord;
                                                int32_t placement_x2 = base_anchor->x_coord + base_metrics.xOffset;
                                                int32_t placement_y2 = base_anchor->y_coord + base_metrics.yOffset;

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

                        for (size_t r = 0; r < hz_vector_size(range_list->ranges); ++r) {
                            const hz_range_t *range = &range_list->ranges[r];

                            if (range->is_ignored) {
                                hz_buffer_add_range(b2, b1, range->mn, range->mx);
                            } else {
                                for (hz_segment_sz_t g = range->mn; g <= range->mx; ++g) {
                                    hz_bool match = HZ_FALSE;
                                    if (hz_should_replace(b1, feature, g, table->lookup_flags, table->mark_filtering_set)
                                        && hz_coverage_contains(&subtable->coverage, b1->glyph_indices[g]))  {
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
                                                    uint16_t *sequence = hz_memory_arena_alloc(&arena, context_len * 2);
                                                    uint16_t *context = hz_memory_arena_alloc(&arena, context_len * 2);
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
                                                        int context_high = range_list->unignored_indices[u + rule->input_count];

                                                        // create context from input glyphs
                                                        hz_buffer_t *ctx1 = hz_buffer_copy_range(b1, context_low, context_high);
                                                        hz_buffer_t *ctx2 = hz_buffer_create();
                                                        ctx2->attrib_flags = b2->attrib_flags;

                                                        for (uint16_t z = 0; z < rule->lookup_count; ++z) {
                                                            hz_buffer_compute_info(ctx1, face);
                                                            int *context_index_list = hz_buffer_get_unignored_indices(ctx1, table->lookup_flags, table->mark_filtering_set);
                                                            int sequence_idx = context_index_list[rule->lookup_records[z].sequence_index];

                                                            hz_shaper_apply_gpos_lookup(shaper, font_data, feature,
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
                                                        g = range_list->unignored_indices[skip_loc];
                                                        r = hz_range_list_search(range_list, g);
                                                        range = &range_list->ranges[r];

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

                        for (size_t r = 0; r < hz_vector_size(range_list->ranges); ++r) {
                            const hz_range_t *range = &range_list->ranges[r];

                            if (range->is_ignored) {
                                hz_buffer_add_range(b2, b1, range->mn, range->mx);
                            } else {
                                // unignored
                                for (short g = range->mn; g <= range->mx; ++g) {
                                    hz_bool match = HZ_FALSE;
                                    hz_glyph_metrics_t metrics = b1->glyph_metrics[g];

                                    if (hz_should_replace(b1, feature, g, table->lookup_flags, table->mark_filtering_set)) {
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
                                                if (!hz_coverage_contains(&subtable->prefix_coverages[k],
                                                                        b1->glyph_indices[range_list->unignored_indices[
                                                                                u - (k + 1)]])) {
                                                    prefix_match = 0;
                                                    break;
                                                }
                                            }

                                            // input
                                            for (int k = 0; k < subtable->input_count; ++k) {
                                                if (!hz_coverage_contains(&subtable->input_coverages[k],
                                                                        b1->glyph_indices[range_list->unignored_indices[
                                                                                u + k]])) {
                                                    input_match = 0;
                                                    break;
                                                }
                                            }

                                            // suffix
                                            for (int k = 0; k < subtable->suffix_count; ++k) {
                                                if (!hz_coverage_contains(&subtable->suffix_coverages[k],
                                                                        b1->glyph_indices[range_list->unignored_indices[
                                                                                u + subtable->input_count + k]])) {
                                                    suffix_match = 0;
                                                    break;
                                                }
                                            }

                                            if (input_match && suffix_match && prefix_match) {
                                                int context_low = range_list->unignored_indices[u];
                                                int context_high = range_list->unignored_indices[u + subtable->input_count - 1];

                                                // create context from input glyphs
                                                hz_buffer_t *ctx1 = hz_buffer_copy_range(b1, context_low, context_high);
                                                hz_buffer_t *ctx2 = hz_buffer_create();
                                                ctx2->attrib_flags = b2->attrib_flags;

                                                for (uint16_t z = 0; z < subtable->lookup_count; ++z) {
                                                    hz_buffer_compute_info(ctx1, face);
                                                    hz_vector(int) context_index_list = hz_buffer_get_unignored_indices(ctx1, table->lookup_flags, table->mark_filtering_set);
                                                    int sequence_idx = context_index_list[subtable->lookup_records[z].sequence_index];
                                                    hz_vector_destroy(context_index_list);

                                                    hz_shaper_apply_gpos_lookup(shaper, font_data, feature,
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
                                                g = range_list->unignored_indices[skip_loc];
                                                r = hz_range_list_search(range_list, g);
                                                range = &range_list->ranges[r];

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
        hz_range_list_destroy(range_list);
    }

    // write slices caused by range into the output buffer
    hz_buffer_add_range(out, in, 0, v1 - 1);
    hz_buffer_add_other(out, b1);
    hz_buffer_add_range(out, in, v2 + 1, in->glyph_count - 1);

    // cleanup buffers
    hz_buffer_destroy(b1);
    hz_buffer_destroy(b2);
}

HZ_STATIC int
hz_feature_list_search(hz_feature_list_item_t *features, uint16_t num_features, hz_feature_t feature)
{
    const hz_feature_info_t *feature_info = hz_ot_get_feature_info(feature);

    if (feature_info != NULL) {
        for (int i = 0; i < num_features; ++i) {
            if (hz_unlikely(features[i].tag == feature_info->tag)) { return i; }
        }
    }

    return -1;
}


HZ_STATIC void hz_buffer_setup_metrics(hz_buffer_t *buffer, hz_face_t *face)
{
    if (buffer != NULL) {
        size_t size = buffer->glyph_count;
        hz_vector_resize(buffer->glyph_metrics, size);

        for (size_t i = 0; i < size; ++i) {
            // Marks should not have advance, but this is a hack
            {
                hz_index_t glyph_index = buffer->glyph_indices[i];
                buffer->glyph_metrics[i].xAdvance = face->metrics[glyph_index].xAdvance;
                buffer->glyph_metrics[i].yAdvance = face->metrics[glyph_index].yAdvance;
            }

            buffer->glyph_metrics[i].xOffset = 0;
            buffer->glyph_metrics[i].yOffset = 0;
        }

        buffer->attrib_flags |= HZ_GLYPH_ATTRIB_METRICS_BIT;
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

HZ_STATIC void hz_shaper_apply_gsub_features(hz_shaper_t *shaper, hz_font_data_t *font_data, hz_buffer_t *in_buffer, hz_buffer_t *out_buffer)
{
    hz_face_t *face = font_data->face;
    hz_gsub_table_t *gsub = &font_data->gsub_table;
    out_buffer->attrib_flags = HZ_GLYPH_ATTRIB_CODEPOINT_BIT | HZ_GLYPH_ATTRIB_INDEX_BIT | HZ_GLYPH_ATTRIB_COMPONENT_INDEX_BIT;

    hz_vector(hz_lookup_reference_t) lookup_refs = NULL;

    for (uint32_t i = 0; i < shaper->num_features; ++i) {
        hz_feature_t feature = shaper->features[i];
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
        hz_shaper_apply_gsub_lookup(shaper, font_data, ref->feature, ref->index, in_buffer, out_buffer, 0, in_buffer->glyph_count - 1, 0);
        hz_swap_buffers(in_buffer, out_buffer, face);
    }

    hz_vector_destroy(lookup_refs);
}

HZ_STATIC void hz_shaper_apply_gpos_features(hz_shaper_t *shaper, hz_font_data_t *font_data, hz_buffer_t *in_buffer, hz_buffer_t *out_buffer)
{
    hz_face_t *face = font_data->face;
    hz_gpos_table_t *gpos = &font_data->gpos_table;
    out_buffer->attrib_flags = HZ_GLYPH_ATTRIB_METRICS_BIT;

    hz_vector(hz_lookup_reference_t) lookup_refs = NULL;

    for (uint32_t i = 0; i < shaper->num_features; ++i) {
        hz_feature_t feature = shaper->features[i];
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
        hz_shaper_apply_gpos_lookup(shaper, font_data, ref->feature, ref->index, in_buffer, out_buffer, 0, in_buffer->glyph_count - 1, 0);
        hz_swap_buffers(in_buffer, out_buffer, face);
    }

    hz_vector_destroy(lookup_refs);
}

HZ_STATIC void hz_buffer_correct_metrics(hz_buffer_t *buffer) {
    for (size_t i = 0; i < buffer->glyph_count; ++i) {
        if (buffer->glyph_classes[i] & HZ_GLYPH_CLASS_MARK) {
            buffer->glyph_metrics[i].xAdvance = 0;
            buffer->glyph_metrics[i].yAdvance = 0;
        }
    }
}

HZ_STATIC void hz_shape_buffer(hz_shaper_t *shaper, hz_font_data_t *font_data, hz_buffer_t *in_buffer)
{
    hz_buffer_t out_buffer;
    hz_buffer_init(&out_buffer);

    if (in_buffer->glyph_count) {
        hz_shaper_apply_gsub_features(shaper, font_data, in_buffer, &out_buffer);
        hz_buffer_setup_metrics(in_buffer, font_data->face);
        hz_shaper_apply_gpos_features(shaper, font_data, in_buffer, &out_buffer);
        hz_buffer_compute_info(in_buffer, font_data->face);
        hz_buffer_correct_metrics(in_buffer);

        if (shaper->direction == HZ_DIRECTION_RTL || shaper->direction == HZ_DIRECTION_BTT) {
            hz_buffer_flip_direction(in_buffer);
        }
    }

    hz_buffer_release(&out_buffer);
}

HZ_STATIC const hz_language_map_t *
hz_get_language_map(hz_language_t lang) {
    size_t i;
    for (i = 0; i < HZ_ARRAY_SIZE(language_map_list); ++i) {
        if (language_map_list[i].language == lang) {
            return &language_map_list[i];
        }
    }
    return NULL;
}

hz_language_t hz_lang(const char *tag)
{
    const hz_language_map_t *currlang, *foundlang;
    size_t i, n;
    size_t len;
    const char *p;
    char code[3]; /* expects only 3 or 2 char codes */
    foundlang = NULL;
    len = strlen(tag);

    /* use ISO 639-2 and ISO 639-3 codes, same as in https://docs.microsoft.com/en-us/typography/opentype/spec/languagetags */
    for (i = 0; i < HZ_ARRAY_SIZE(language_map_list); ++i) {
        currlang = &language_map_list[i];
        p = currlang->codes;

        if (p == NULL) continue;

        while (1) {
            n = 0;
            while (*p != ':' && *p != '\0')
                code[n++] = *p++;

            if (hz_unlikely(!memcmp(code, tag, 3))) {
                foundlang = currlang;
                goto done_searching;
            }

            if (*p == '\0') break;
            ++p;
        }
    }

    done_searching:
    if (hz_unlikely(foundlang == NULL))
        return HZ_LANGUAGE_DFLT;

    return foundlang->language;
}

#define UTF_START 0

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
static const int8_t hz_decode_byte_table[] = {
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
static const uint8_t hz_decode_mask_table[] = {
        0x7f, 0x1f, 0x0f, 0x07, 0x03, 0x01,
};

static const int8_t hz_prefix_to_length_table[32] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 3
};

HZ_STATIC void
hz_utf8_decoder_init(hz_utf8_decoder_t *state)
{
    state->data = NULL;
    state->ptr = 0;
    state->size = 0;
    state->codepoint = 0;
    state->step = UTF_START;
    state->flags = 0;
}

HZ_STATIC void
hz_utf8_decoder_set_flags(hz_utf8_decoder_t *state, uint32_t flags) {
    state->flags = flags;
}

HZ_STATIC void
hz_utf8_decoder_reset(hz_utf8_decoder_t *state)
{
    state->ptr = 0;
    state->size = 0;
    state->codepoint = 0;
    state->step = UTF_START;
}

HZ_STATIC void
hz_utf8_decoder_set_data(hz_utf8_decoder_t *state, const char8_t *data, size_t size)
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

HZ_STATIC void hz_decode_utf8_byte(hz_utf8_decoder_t *state)
{
    hz_byte_t byte;

    retry:
    byte = state->data[state->ptr++];
    if (state->step == UTF_START) {
        // decode the number of steps using the first byte
        state->step = hz_decode_byte_table[byte];
        if (state->step < 0) goto error; // invalid starting byte
        // extract the data from the first byte, using the proper mask
        state->codepoint = ((uint32_t)byte & hz_decode_mask_table[state->step]) << (6*state->step);
    } else {
        if ((byte & 0xc0) != 0x80) { // invalid continuation byte
            // retry as starting byte
            state->step = UTF_START;
            goto retry;
        }

        if (state->step == 0) goto error; // too many continuation bytes
        state->step -= 1;
        // extract the data from the continuation byte
        state->codepoint |= ((uint32_t)byte & 0x3f) << (6*state->step);
    }
    return;

    error:
    // return the unicode "unknown" character and start again
    state->step = UTF_START;
    state->codepoint = 0xfffd;
}

HZ_STATIC hz_unicode_t hz_decode_next_utf8_codepoint(hz_utf8_decoder_t *state) {
    state->step = UTF_START;
    do hz_decode_utf8_byte(state); while (state->step != UTF_START);
    if (!state->codepoint) {
        state->step = UTF_END_OF_BUFFER;
    }
    return state->codepoint;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void hz_buffer_load_ascii_sz(hz_buffer_t *buffer, const char *sz_input) { // works for ascii too
    while (*sz_input != '\0') {
        hz_vector_push_back(buffer->codepoints, (hz_unicode_t)(*sz_input));
        ++sz_input;
    }
}

HZ_STATIC void hz_buffer_load_utf8_unaligned(hz_buffer_t *buffer, hz_memory_arena_t *memory_arena, const uint8_t *input, size_t size)
{
    HZ_ASSERT(input != NULL);
    hz_buffer_clear(buffer); // clear any previously loaded data

#if HZ_ARCH & HZ_ARCH_AVX2_BIT
    buffer->codepoints = hz_memory_arena_alloc_aligned(memory_arena, size*4, 32); // m256 must be aligned on 32-byte boundary
    uint32_t *dest = (uint32_t *)buffer->codepoints;
    // UTF-8 to UTF-32 conversion
    for (size_t i = 0; i+32 <= size; i+=32) {
        // Load 32-bytes into register
        __m256i chunk = _mm256_loadu_si256((const __m256i *)&input[i]);

        // When none of the bytes has the MSB set, all characters of the chunk are Ascii
        if (!_mm256_movemask_epi8(chunk)) {
            // Extract two halves of the chunk
            __m128i lo = _mm256_castsi256_si128(chunk);
            __m128i hi = _mm256_extracti128_si256(chunk, 1);
            // Convert to 32-bit unsigned integers with ZeroExtend32
            __m256i r0 = _mm256_cvtepu8_epi32(lo);
            __m256i r1 = _mm256_cvtepu8_epi32(_mm_srli_si128(lo,8));
            __m256i r2 = _mm256_cvtepu8_epi32(hi);
            __m256i r3 = _mm256_cvtepu8_epi32(_mm_srli_si128(hi,8));
            // Store results in destination buffer
            _mm256_store_si256((__m256i *)(dest + 0), r0);
            _mm256_store_si256((__m256i *)(dest + 8), r1);
            _mm256_store_si256((__m256i *)(dest + 16), r2);
            _mm256_store_si256((__m256i *)(dest + 24), r3);
            dest += 32;
        } else {
            // Non-Ascii encountered, decode chars
            // 1-byte character 0xxxxxxx
            // 2-byte character 110xxxxx 10xxxxxxx
            // 3-byte character 1110xxxx 10xxxxxxx 10xxxxxxx
            // 4-byte character 11110xxx 10xxxxxxx 10xxxxxxx 10xxxxxxx
            __m256i c1 = _mm256_cmpgt_epi8(chunk, _mm256_set1_epi8(0xbf)); // gtequal 0xc0 <-> gt 0xbf
            __m256i c2 = _mm256_cmpgt_epi8(chunk, _mm256_set1_epi8(0xdf)); // gtequal 0xe0 <-> gt 0xdf
            __m256i c3 = _mm256_cmpgt_epi8(chunk, _mm256_set1_epi8(0xef)); // gtequal 0xf0 <-> gt 0xef

            __m256i data_mask = _mm256_set1_epi8(0x80); // ascii or continuation mask

            data_mask = _mm256_blendv_epi8(data_mask, _mm256_set1_epi8(0xc0), c1);
            data_mask = _mm256_blendv_epi8(data_mask, _mm256_set1_epi8(0xe0), c2);
            data_mask = _mm256_blendv_epi8(data_mask, _mm256_set1_epi8(0xf0), c3);

            // chunk contains only 1-byte or 2-byte codepoints
            if (!_mm256_movemask_epi8(c3)) {
                
            }

            for (int k = 0; k < 32; ++k) {
                uint8_t *p = (uint8_t *)&data_mask;
                printf("0x%0x ",p[k]);
            }
            printf("\n");
        }
    }
#endif
}

void hz_buffer_load_utf8_sz(hz_buffer_t *buffer, const unsigned char *sz_input) {
    hz_utf8_decoder_t state = {
        .flags = UTF_END_AT_NULL,
        .data = sz_input
    };

    while (state.step != UTF_END_OF_BUFFER) {
        hz_unicode_t codepoint = hz_decode_next_utf8_codepoint(&state);
        if (!codepoint) break;
        hz_vector_push_back(buffer->codepoints, codepoint);
    }
}

void hz_buffer_load_ucs2_sz(hz_buffer_t *buffer, const hz_ucs2_char_t *sz_input) {
    while (*sz_input != '\0') {
        hz_vector_push_back(buffer->codepoints, (hz_unicode_t)(*sz_input));
        ++sz_input;
    }
}

void HzBuffero_nfd(hz_buffer_t *buffer)
{
    
}
void HzBuffero_nfc(hz_buffer_t *buffer)
{

}

void HzBuffero_nfkd(hz_buffer_t *buffer)
{
    
}
void HzBuffero_nfkc(hz_buffer_t *buffer)
{
    
}

void HzBuffero_nf(hz_buffer_t *buffer, hz_normalization_form_t nf)
{
    switch (nf) {
        default: break;
        case HZ_NFD: HzBuffero_nfd(buffer); break;
        case HZ_NFC: HzBuffero_nfc(buffer); break;
        case HZ_NFKD: HzBuffero_nfkd(buffer); break;
        case HZ_NFKC: HzBuffero_nfkc(buffer); break;
    }
}

void hz_shape_sz1(hz_shaper_t *shaper, hz_font_data_t *font_data, hz_encoding_t encoding, const void* sz_input, hz_buffer_t *out_buffer)
{
    HZ_ASSERT(sz_input != NULL);
    hz_face_t *face = font_data->face;

    switch (encoding) {
        default:
        case HZ_ENCODING_ASCII: hz_buffer_load_ascii_sz(out_buffer, (const char*)sz_input); break;
        case HZ_ENCODING_LATIN1: break;
        case HZ_ENCODING_UCS2: hz_buffer_load_ucs2_sz(out_buffer, (const hz_ucs2_char_t*)sz_input); break;
        case HZ_ENCODING_UTF8: hz_buffer_load_utf8_sz(out_buffer, (const unsigned char*)sz_input); break;
        case HZ_ENCODING_UTF16: break;
        case HZ_ENCODING_UTF32: break;
    }

    // set initial buffer attrib flags
    out_buffer->attrib_flags = HZ_GLYPH_ATTRIB_CODEPOINT_BIT | HZ_GLYPH_ATTRIB_INDEX_BIT | HZ_GLYPH_ATTRIB_COMPONENT_INDEX_BIT;
    // set buffer size based on number of codepoints decoded
    out_buffer->glyph_count = hz_vector_size(out_buffer->codepoints);
    // compute glyph indices for the input buffer
    hz_vector_resize(out_buffer->glyph_indices, out_buffer->glyph_count);
    
    hz_map_to_nominal_forms(face, out_buffer->glyph_indices, out_buffer->codepoints,
                            out_buffer->glyph_count);

    hz_vector_resize(out_buffer->component_indices, out_buffer->glyph_count);
    hz_zero(out_buffer->component_indices, sizeof(uint16_t)*out_buffer->glyph_count);

    hz_buffer_compute_info(out_buffer, face);
    hz_shape_buffer(shaper, font_data, out_buffer);
}

// NOTE: On ARM, it is possible to make use of the hardware types such as __fp16 and _Float16.
// half-float (16-bit) type.
typedef uint16_t hz_half;

#define HZ_FLOAT_MAX_BIASED_EXP (0xff << 23)
#define HZ_HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP (0x38000000)
#define HZ_HALF_FLOAT_MAX_BIASED_EXP_AS_SINGLE_FP_EXP (0x47800000)
#define HZ_HALF_FLOAT_MAX_BIASED_EXP (0x1f << 10)

typedef float hz_float;

// convert from 16-bit half float to single float
HZ_ALWAYS_INLINE hz_half hz_float_to_half(hz_float f)
{
    hz_half hf;
    uint32_t x = *(uint32_t *)&f;
    uint32_t sign = (uint16_t)(x >> 31);
    // get mantissa
    uint32_t mantissa = x & ((1 << 23) - 1);
    // get exponent bits
    uint32_t exp = x & HZ_FLOAT_MAX_BIASED_EXP;

    if (exp >= HZ_HALF_FLOAT_MAX_BIASED_EXP_AS_SINGLE_FP_EXP) {
        // check if the original single precision float number is a NaN
        if (mantissa && (exp == HZ_FLOAT_MAX_BIASED_EXP)) {
            // we have a single precision NaN
            mantissa = (1 << 23) - 1;
        } else {
            // 16-bit half-float stores number as Inf
            mantissa = 0;
        }

        hf = (hz_half)(sign << 15) | (hz_half)(HZ_HALF_FLOAT_MAX_BIASED_EXP) | (hz_half)(mantissa >> 13);
    } else if (exp <= HZ_HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP) { // check if exponent is <= -15
        // store a denorm half-float value or zero
        exp = (HZ_HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP - exp) >> 23;
        mantissa >>= 14 + exp;
        
        hf = ((hz_half)sign << 15) | (hz_half)(mantissa);
    } else {
        hf = (hz_half)(sign << 15)
           | (hz_half)((exp - HZ_HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP) >> 13)
           | (hz_half)(mantissa >> 13);
    }

    return hf;
}

HZ_ALWAYS_INLINE hz_float hz_half_to_float(hz_half hf)
{
    uint32_t sign = (uint32_t)(hf >> 15);
    uint32_t mantissa = (uint32_t)(hf & ((1 << 10) - 1));
    uint32_t exp = (uint32_t)(hf & HZ_HALF_FLOAT_MAX_BIASED_EXP);
    uint32_t f;

    if (exp == HZ_HALF_FLOAT_MAX_BIASED_EXP) {
        // we have a half-float NaN or Inf
        // half-float NaNs will be converted to a single precision NaN
        // half-float Infs will be converted to a single precision Inf
        exp = HZ_FLOAT_MAX_BIASED_EXP;
        if (mantissa)
            mantissa = (1 << 23) - 1;
    } else if (exp == 0) {
        // convert half-float zero/denorm to single precision value
        if (mantissa) {
            mantissa <<= 1;
            exp = HZ_HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP;
            // check for leading 1 in denorm mantissa
            while ((mantissa & (1 << 10)) == 0) {
                // for every leading 0, decrement single precision exponent by 1
                // and shift half-float mantissa value to the left
                mantissa <<= 1;
                exp -= (1 << 23);
            }
            // clamp the mantissa to 10-bits
            mantissa &= ((1 << 10) - 1);
            // shift left to generate single-precision mantissa of 24 bits
            mantissa <<= 13;
        }
    } else {
        // shift left to generate single-precision mantissa of 23-bits
        mantissa <<= 13;
        // generate single precision biased exponent value
        exp = (exp << 13) + HZ_HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP;
    }

    f = (sign << 31) | exp | mantissa;
    return *((hz_float *)&f);
}

typedef enum {
    HZ_LAYOUT_ALIGN_LEFT    = HZ_FLAG(0),
    HZ_LAYOUT_ALIGN_RIGHT   = HZ_FLAG(1),
    HZ_LAYOUT_ALIGN_BOTTOM  = HZ_FLAG(2),
    HZ_LAYOUT_ALIGN_TOP     = HZ_FLAG(3),
    HZ_LAYOUT_AUTO_ALIGN    = HZ_FLAG(4),
    HZ_LAYOUT_JUSTIFY       = HZ_FLAG(5),
    HZ_LAYOUT_WRAP          = HZ_FLAG(6),
    HZ_LAYOUT_X_CENTER      = HZ_FLAG(7),
    HZ_LAYOUT_Y_CENTER      = HZ_FLAG(8),
} hz_layout_flags_t;

/* MSI Hash table

    Hash table inspired by Chris Wellons' on the idea of the "MSI" hash table found here: https://nullprogram.com/blog/2022/08/08/.
    The "MSI" acronym stands for "mask-step-index" which describes the function of this hash table pattern.
*/
typedef uint32_t HzMSIKey;

typedef struct {
    HzMSIKey *ht;
    int32_t len;
    int exp;
} HzMSI;

#define HZ_MSI_NOT_FOUND -1
#define HZ_MSI_EMPTY_KEY -2

void hz_msi_ht_init(hz_memory_arena_t *memory_arena, HzMSI *msi, size_t sz) {
    HZ_ASSERT(HZ_ISP2(sz));
    *msi = (HzMSI){
        .ht=hz_memory_arena_alloc(memory_arena, sz*sizeof(HzMSIKey)),
        .exp = hz_qlog2_i64(sz)
    };
}

int32_t hz_msi_ht_intern(HzMSI *msi, uint32_t key) {
    uint32_t h = hz_hash32_fnv1a(key);

    for (uint32_t i = h;;) {
        // iterate
        uint32_t mask = ((uint32_t)1 << msi->exp) - 1;
        uint32_t step = (h >> (64 - msi->exp)) | 1;
        i = (i + step) & mask;

        if (msi->ht[i] == HZ_MSI_EMPTY_KEY) {
             if ((uint32_t)msi->len+1 == ((uint32_t)1 << msi->exp)) {
                break; // out of memory
            }

            msi->ht[i] = key; ++msi->len;
            return i; // return index of entry
        } else if (msi->ht[i] == key) { // return index of existing item
            return i;
        }
    }

    return HZ_MSI_NOT_FOUND;
}

void hz_lru_cache_init(hz_memory_arena_t *ma, hz_glyph_cache_t *c, int sz, int max_replace_sz)
{
    HZ_ASSERT(sz);
    c->slots_occupied = 0;
    c->sz = sz;
    c->p2sz = HZ_NXP2(sz);
    c->slots = hz_memory_arena_alloc(ma, sizeof(struct hz_cache_slot_t)*c->p2sz);
    c->fn = hz_malloc(sizeof(*c->fn));
    c->ln = hz_malloc(sizeof(*c->ln));
    (*c->fn) = (struct hz_cache_node_t){.next = c->ln, .prev = NULL};
    (*c->ln) = (struct hz_cache_node_t){.prev = c->fn, .next = NULL};
}

struct hz_cache_node_t *hz_lru_cache_get_node(hz_glyph_cache_t *c, hz_cache_id_t id)
{
    for(struct hz_cache_node_t *n = c->fn->next; n != c->ln; n = n->next){
        if(c->slots[n->slot].id.u32 == id.u32) return n;
    }

    return NULL;
}

struct hz_cache_stat_t hz_lru_cache_stat(hz_glyph_cache_t *c,
                                         hz_ht_t *ids_ht,
                                         hz_cache_id_t *avail_id_list,
                                         hz_cache_id_t *unavail_id_list)
{
    struct hz_cache_stat_t stat={0};

    for (hz_ht_iter_t it = hz_ht_iter_begin(ids_ht);
            hz_ht_iter_valid(&it); hz_ht_iter_next(ids_ht, &it)) {
        struct hz_cache_node_t *n;
        hz_cache_id_t id = {.u32 = it.key};
        if ((n = hz_lru_cache_get_node(c, id)) != NULL) {
            avail_id_list[++stat.avail] = id;
        } else {
            unavail_id_list[stat.unavail++] = id;
        }
    }

    return stat;
}

HZ_ALWAYS_INLINE int hz_lru_cache_is_full(hz_glyph_cache_t *lru) { return lru->slots_occupied>=lru->sz; }

HZ_ALWAYS_INLINE struct hz_cache_node_t *hz_lru_cache_add_node(hz_glyph_cache_t *lru)
{
    // When adding node, we always move it to the front of the list
    struct hz_cache_node_t *n = hz_malloc(sizeof(*n));
    n->slot = lru->slots_occupied++;

    // Link node with the LRU list
    struct hz_cache_node_t *old_next = lru->fn->next;

    n->next = old_next; n->prev = lru->fn;
    lru->fn->next = n;
    
    return n;
}

void hz_lru_cache_replace_slots(hz_glyph_cache_t *lru,
                                uint16_t slots_sz,
                                uint16_t open_slots[])
{
    uint16_t slot_index = 0;
    // Add nodes until LRU is full
    for (slot_index = 0; slot_index < slots_sz && !hz_lru_cache_is_full(lru); ++slot_index) {
        struct hz_cache_node_t *tmp_node = hz_lru_cache_add_node(lru);
        open_slots[slot_index] = tmp_node->slot;
    }

    // If slots remain to be filled, start replacing last recently used nodes
    // from back of list.
    for (;slot_index < slots_sz; ++slot_index) {
        struct hz_cache_node_t *n = lru->ln->prev, *n_left = n->prev, *n_right = n->next;

        // Link prev and next nodes together
        n_left->next = n_right;
        n_right->prev = n_left;

        // Move node to front
        struct hz_cache_node_t *old_first = lru->fn->next;
        lru->fn->next = n;
        n->prev = lru->fn;
        n->next = old_first;

        open_slots[slot_index] = n->slot;
    }
}


void hz_lru_write_slot(hz_glyph_cache_t *lru, int slot_index, struct hz_cache_slot_t slot) {
    lru->slots[slot_index] = slot;
}

typedef struct {
    size_t first_glyph;
    size_t size;
} hz_segment_command_t;

typedef struct {
    hz_direction_t dir;
    hz_layout_flags_t flags;
    float sx, sy, max_length;
    float v_advance;
    hz_vector(hz_segment_command_t) segments;
} hz_line_layout_t;

#define HZ_MAX_UNIQUE_GLYPHS_PER_FRAME 1000

void hz_command_list_init(hz_command_list_t *cmd_list)
{
    cmd_list->draw_data = NULL;
    cmd_list->unique_glyph_ht = hz_ht_create(hz_get_allocator(), HZ_MAX_UNIQUE_GLYPHS_PER_FRAME);
}

void hz_command_list_clear(hz_command_list_t *cmd_list)
{
    hz_vector_clear(cmd_list->draw_data);
    hz_ht_clear(cmd_list->unique_glyph_ht);
}

#define HZ_CONTEXT_DFLT_MEMORY_SIZE (1024*1024) /*1MiB*/
#ifndef HZ_CONTEXT_MEMORY_SIZE
#   define HZ_CONTEXT_MEMORY_SIZE HZ_CONTEXT_DFLT_MEMORY_SIZE
#endif

#define HZ_CONTEXT_FRAME_MEMORY_SIZE 1048576
#define HZ_CONTEXT_FONT_TABLE_SIZE 64

struct hz_context_t {
    hz_command_list_t frame_cmds;
    hz_font_data_t font_table [HZ_CONTEXT_FONT_TABLE_SIZE];
    uint16_t font_id_counter;
    hz_glyph_cache_t lru;
    hz_memory_arena_t memory_arena;
    hz_memory_arena_t frame_arena;
    void *arena_buffer, *frame_arena_buffer;
    hz_mat4 camera_matrix;
};

hz_context_t *hz_context_create (hz_glyph_cache_opts_t *opts) {
    hz_context_t *ctx = hz_malloc(sizeof(*ctx));
    hz_command_list_init(&ctx->frame_cmds);

    ctx->arena_buffer = hz_malloc(HZ_CONTEXT_MEMORY_SIZE);
    hz_memory_arena_init(&ctx->memory_arena, (uint8_t *)ctx->arena_buffer, HZ_CONTEXT_MEMORY_SIZE);
    ctx->frame_arena_buffer = hz_malloc(HZ_CONTEXT_FRAME_MEMORY_SIZE);
    hz_memory_arena_init(&ctx->frame_arena, (uint8_t *)ctx->frame_arena_buffer, HZ_CONTEXT_FRAME_MEMORY_SIZE);
    hz_lru_cache_init(&ctx->memory_arena, &ctx->lru, opts->x_cells * opts->y_cells, 0.5f);
    ctx->font_id_counter = 0;
    return ctx;
}

hz_glyph_cache_t *hz_context_get_lru(hz_context_t *ctx) {
    return &ctx->lru;
}

hz_memory_arena_t *hz_get_frame_arena(hz_context_t *ctx) {
    return &ctx->frame_arena;
}

void hz_context_release (hz_context_t *ctx) {
    hz_free(ctx->arena_buffer);
    hz_free(ctx->frame_arena_buffer);
    hz_free(ctx);
}


hz_command_list_t *hz_command_list_get(hz_context_t *ctx) {
    return &ctx->frame_cmds;
}

hz_face_t *hz_context_get_face(hz_context_t *ctx, uint16_t font_id) {
    return ctx->font_table[font_id].face;
}

uint16_t hz_context_stash_font(hz_context_t *ctx, const hz_font_data_t *fd)
{
    uint16_t id = ctx->font_id_counter++;
    ctx->font_table[id] = *fd;
    return id;
}

void hz_frame_begin(hz_context_t *ctx) {
    hz_command_list_clear(&ctx->frame_cmds);
    hz_memory_arena_reset(&ctx->frame_arena);
}

typedef struct {
    size_t instance_cnt;
    hz_glyph_instance_t *instances;
    hz_ht_t unique_ids;
} hz_glyph_batch_command_t;

void hz_context_build_next_batch(hz_context_t *ctx, uint16_t *unique_ids) {
    
}

void hz_frame_end(hz_context_t *ctx) {
    hz_command_list_t *frame_cmds = &ctx->frame_cmds;

    for (size_t i = 0; i < hz_vector_size(frame_cmds->draw_data); ++i) {
        hz_glyph_instance_t *g = &frame_cmds->draw_data[i];

        hz_ht_iter_t it;
        uint32_t key = g->lru_id.u32;

        if (hz_ht_search(frame_cmds->unique_glyph_ht, key, &it))
            (*it.ptr_value)++;
        else
            hz_ht_insert(frame_cmds->unique_glyph_ht, key, 1);
    }
}

typedef struct {
    char r,g,b,a;
} hz_color_t;

float hz_face_scale_for_pixel_h(hz_face_t *face, float height)
{
   // return (float) height / (float) face->fheight;
    return stbtt_ScaleForPixelHeight(face->fontinfo, height);
}

uint32_t hz_vec4_to_u32(hz_vec4 *v)
{
    uint32_t r = 0;
    r |= (uint32_t)(v->x*255.0f);
    r |= (uint32_t)(v->y*255.0f) << 8;
    r |= (uint32_t)(v->z*255.0f) << 16;
    r |= (uint32_t)(v->w*255.0f) << 24;
    return r;
}

void hz_draw_buffer(hz_context_t *ctx, hz_buffer_t *buffer, uint16_t font_id,
                    hz_vec3 pos, hz_buffer_style_t *style,
                    float px_size)
{
    const hz_font_data_t *font_data = &ctx->font_table[font_id];
    hz_face_t *face = font_data->face;
    float text_width = 0.0f;
    float pen_x=pos.x,pen_y=pos.y;
    float v_scale = hz_face_scale_for_pixel_h(face,px_size);

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(face->fontinfo, &ascent, &descent, &lineGap);
    ascent = ascent * v_scale;
    descent = descent * v_scale;

    for (size_t i = 0; i < buffer->glyph_count; ++i) {
        hz_glyph_metrics_t metrics = buffer->glyph_metrics[i];
        hz_glyph_instance_t g;
        hz_index_t gid = buffer->glyph_indices[i];
        g.lru_id.glyph_id = gid;
        g.lru_id.font_id = font_id;

        int32_t xAdvance = metrics.xAdvance;
        int32_t yAdvance = metrics.yAdvance;
        int32_t x_offset  = metrics.xOffset;
        int32_t y_offset  = metrics.yOffset;

        int ax;
        int lsb;
        stbtt_GetGlyphHMetrics(face->fontinfo, gid, &ax, &lsb);

        int ix0,iy0,ix1,iy1;
        stbtt_GetGlyphBox(face->fontinfo, gid,
                          &ix0, &iy0, &ix1, &iy1);

        /* compute y (different characters have different heights */
        int y = pen_y - (ascent - iy0);

        /* render character (stride and offset is important here) */
        float c_w = (ix1 - ix0)*v_scale;
        float c_h = (iy1 - iy0)*v_scale;
        float dstx = pen_x + (ix0 + x_offset)*v_scale;
        float dsty = pen_y + (iy0 + y_offset)*v_scale;//(ascent - iy0*v_scale) + y_offset*v_scale + 0.5f;
        // c_w -= 0.5f;
        // c_h -= 0.5f;

        hz_mat4 mvp;
        // hz_mat4 scale_matrix = hz_mat4_scale(c_w, c_h, 1.0f);
        // hz_mat4 translation_matrix = hz_mat4_translate(dstx, dsty, 0.0f);
        // hz_mat4_mult(&translation_matrix, &scale_matrix, &mvp);
        // hz_mat4_mult(&ctx->camera_matrix, &mvp, &mvp);
        g.vp = ctx->camera_matrix;
        g.v4_ts = (hz_vec4){dstx, dsty, c_w, c_h};
        g.sv.color_rgba = hz_vec4_to_u32(&style->col);
        g.sv.outline_color_rgba = hz_vec4_to_u32(&style->outline_col);
        g.sv.weight_sheer = ((uint32_t)(style->shear*65535.0f) & 0xffffu);

        hz_vector_push_back(ctx->frame_cmds.draw_data,g);
        pen_x += metrics.xAdvance * v_scale;
    }
}

hz_command_list_t *hz_get_frame_commands(hz_context_t *ctx) {
    return &ctx->frame_cmds;
}

void hz_shape_draw_data_clear(hz_shape_draw_data_t *draw_data)
{
    if (draw_data->verts != NULL) {
        hz_vector_destroy(draw_data->verts);
    }
}

hz_rect_t hz_glyph_cache_compute_cell_rect(hz_glyph_cache_opts_t *opts, int cell)
{
    HZ_ASSERT(cell < opts->x_cells*opts->y_cells);
    float cw = (float)opts->width / (float)opts->x_cells;
    float ch = (float)opts->height / (float)opts->y_cells;
    int icx = cell % opts->x_cells;
    int icy = cell / opts->x_cells;;
    return (hz_rect_t){icx * cw, icy * ch, cw, ch};
}

hz_vec2 hz_vec2_add(hz_vec2 v1, hz_vec2 v2) {
    return (hz_vec2){v1.x+v2.x, v1.y+v2.y};
}

hz_vec2 hz_vec2_mult_s(hz_vec2 v1, float scale) {
    return (hz_vec2){v1.x*scale,v1.y*scale};
}

int hz_face_get_glyph_shape(hz_face_t *face, hz_shape_draw_data_t *draw_data, hz_vec2 translate, float y_scale, hz_index_t glyph_index)
{
    #define PUSH_CURVE() do{hz_contour_t _contour = {hz_vector_size(draw_data->verts),0,pen}; hz_vector_push_back(draw_data->contours, _contour); c = hz_vector_top(draw_data->contours); } while(0)

    if (stbtt_IsGlyphEmpty(face->fontinfo, glyph_index)) {
        return 0;
    }
    
    stbtt_vertex *vertices = NULL;
    size_t nverts = stbtt_GetGlyphShape(face->fontinfo, glyph_index, &vertices);
    
    hz_vec2 pen = {0,0};
    hz_contour_t *c = NULL;
    size_t i = 0;
    
    while (i < nverts) {
        switch (vertices[i].type) {
            default:break;
            case HZ_VERTEX_TYPE_MOVETO: { // moveto
                pen.x = translate.x + vertices[i].x* y_scale; pen.y = translate.y + vertices[i].y* y_scale;
                PUSH_CURVE();
                break;
            }

            case HZ_VERTEX_TYPE_LINE: { // line
                hz_bezier_vertex_t v;
                v.type = vertices[i].type;
                v.v1 = pen;
                v.v2 = hz_vec2_add(translate, hz_vec2_mult_s((hz_vec2){vertices[i].x,vertices[i].y}, y_scale));
                hz_vector_push_back(draw_data->verts, v);
                pen = v.v2;
                ++c->curve_count;
                break;
            }
            case HZ_VERTEX_TYPE_QUADRATIC_BEZIER: { // quadratic bezier
                hz_bezier_vertex_t v;
                v.type = vertices[i].type;
                v.v1 = pen;
                v.v2 = hz_vec2_add(translate, hz_vec2_mult_s((hz_vec2){vertices[i].x,vertices[i].y}, y_scale));
                v.c1 = hz_vec2_add(translate, hz_vec2_mult_s((hz_vec2){vertices[i].cx,vertices[i].cy}, y_scale));
                hz_vector_push_back(draw_data->verts, v);
                pen = v.v2;
                ++c->curve_count;
                break;
            }
            case HZ_VERTEX_TYPE_CUBIC_BEZIER: { // cubic bezier
                hz_bezier_vertex_t v;
                v.type = vertices[i].type;
                v.v1 = pen;
                v.v2 = hz_vec2_add(translate, hz_vec2_mult_s((hz_vec2){vertices[i].x,vertices[i].y}, y_scale));
                v.c1 = hz_vec2_add(translate, hz_vec2_mult_s((hz_vec2){vertices[i].cx,vertices[i].cy}, y_scale));
                v.c2 = hz_vec2_add(translate, hz_vec2_mult_s((hz_vec2){vertices[i].cx1,vertices[i].cy1}, y_scale));
                hz_vector_push_back(draw_data->verts, v);
                pen = v.v2;
                ++c->curve_count;
                break;
            }
        }
        
        ++i;
    }

    stbtt_FreeShape(face->fontinfo, vertices);
    return nverts;
    #undef PUSH_CURVE
}

void hz_face_get_glyph_box(hz_face_t *face, uint16_t glyph_id, hz_bbox_t *b)
{
    stbtt_GetGlyphBox(face->fontinfo, glyph_id, 
                            &b->x0, &b->y0, &b->x1, &b->y1);
}

void hz_face_get_scaled_glyph
(hz_face_t *face, hz_uint16 glyph_id, float y_scale, hz_bbox_t *b) {
    // NOTE: Get rid of the dependency on stbtt
    stbtt_GetGlyphBitmapBox(face->fontinfo, glyph_id,
                            y_scale, y_scale,
                            &b->x0, &b->y0, &b->x1, &b->y1);
}

void hz_camera_begin_ortho(hz_context_t *ctx, float l, float r, float b, float t)
{
    ctx->camera_matrix = hz_mat4_ortho(l,r,b,t);
}

void hz_camera_set_zoom(hz_context_t *ctx, float zoomlvl)
{
    hz_mat4 z = hz_mat4_identity();
    z.e00 = 1.0/zoomlvl;
    z.e11 = 1.0/zoomlvl;
    hz_mat4_mult(&z, &ctx->camera_matrix, &ctx->camera_matrix);
}