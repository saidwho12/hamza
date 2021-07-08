/* About: License
 * This file is licensed under MIT.
 */

#include "hz.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef unsigned int uint;

typedef u16 Offset16;
typedef u32 Offset32;

#include <assert.h>

#ifdef HZ_USE_TINYCTHREAD
#include "tinycthread.c"
#include "tinycthread.h"
#endif

static hz_global_allocator_t _main_allocator = { (HZMALLOCFUNC)malloc, (HZFREEFUNC)free };

typedef struct hz_pool_allocator_t {

} hz_pool_allocator_t;


HZ_INLINE void *
hz_malloc(size_t size)
{
    return _main_allocator.malloc_func(size);
}

HZ_INLINE void *
hz_realloc(void *ptr, size_t size)
{
    return realloc(ptr,size);
}

HZ_INLINE void
hz_free(void *ptr)
{
    _main_allocator.free_func(ptr);
}

static void
log_impl(const char *filename,
         int line,
         const char *fmt,
         ...)
{

}

#define DEBUG_MSG(msg) fprintf(stdout,"[DEBUG:%s:%d]: %s\n",__FILE__,__LINE__,msg)
#define ERROR_MSG(msg) fprintf(stderr,"[ERROR:%s:%d]: %s\n",__FILE__,__LINE__,msg)
#define QUIT_MSG(msg) { ERROR_MSG(msg); exit(-1); }

/* 
    Read this article for better solutions implementing 64-bit log2:
    http://graphics.stanford.edu/~seander/bithacks.html
*/

HZ_INLINE int
fastlog2l(u64 n)
{
    #if HZ_COMPILER & (HZ_COMPILER_GCC | HZ_COMPILER_CLANG)
    return 64 - __builtin_clzl(n);
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
    return multiply_de_bruijin_bit_position[((u64)((n - (n >> 1))*0x07EDD5E59A4E28C2)) >> 58];
    
    #endif
}

/*
    Struct: hz_bump_allocator_t
        
        A bump allocator, meant to be used on a
        temporary stack buffer.
        The blocks are allocated and stored on the multiple of their size
        rounded to a power of two.
        Another name for this is "monotonic allocator".
*/
typedef struct hz_bump_allocator_t {
    u8 *data;
    size_t size;
    uintptr_t ptr;
} hz_bump_allocator_t;

HZ_INLINE void
hz_bump_allocator_init(hz_bump_allocator_t *a,
                       void *data,
                       size_t size)
{
    a->data = data;
    a->size = size;
    a->ptr = 0;
}

/*
    Function: hz_bump_allocator_alloc
        Allocates new block of memory, and pushes pointer forward.
        Blocks are allocated on the multiple of the size rounded up to
        the next power of two.

    Arguments:
        a - Pointer to the allocator.
        size - Size of the block to allocate.

    Returns:
        Pointer to the newly allocated block. 
*/
HZ_INLINE void *
hz_bump_allocator_alloc(hz_bump_allocator_t *a,
                        size_t size)
{
    size_t potsize, potmask;
    uintptr_t startptr, endptr;

    potsize = fastlog2l(size);
    potmask = potsize - 1;
    startptr = a->ptr + (~a->ptr & potmask); /* adds offset to next block */
    endptr = startptr + size;

    if (endptr > a->size) {
        /* end address of block, and over-flow address */
        void *ptraddr, *endaddr;
        ptraddr = a->data + endptr;
        endaddr = a->data + a->size;
        fprintf(stderr, "monotonic allocator overflowed"
            "(end address: %p, pointer address: %p)\n",endaddr,ptraddr);
        return NULL;
    }

    a->ptr = endptr;
    return a->data + startptr;
}

/*
    Function: hz_bump_allocator_free
        Frees a previously allocated block. (currently a no-op)

    Arguments:
        a - Pointer to the allocator.
        p - Pointer to a previously allocated block.
*/
HZ_INLINE void
hz_bump_allocator_free(hz_bump_allocator_t *a,
                       void *p)
{
    /* No-Op */
}

/*
    Function: hz_bump_allocator_release
        Releases all resources held by the allocator. (currently a no-op)

    Arguments:
        a - A pointer to the allocator.
*/
HZ_INLINE void
hz_bump_allocator_release(hz_bump_allocator_t *a)
{
    /* No-Op */
}

/* no bound for buffer */
#define BS_BNDCHECK 0x00000001

/*
    Struct: hz_byte_stream_t
        
        A struct to store a buffer.
        This is also used to handle reading bytes sequentially.
        Unpacks in network-order (big endian).
*/
typedef struct hz_byte_stream_t {
    u8 *data;
    size_t size;
    intptr_t ptr;
    int flags;
} hz_byte_stream_t;

/*
    Function: hz_byte_stream_create

        Creates a byte stream, holds data and
        a bump pointer allowing seeking, unpacking various primitives from memory.

    Parameters:
        data - The pointer to the memory.
        size - The size of the block (If this is 0, then there's no bound check).

    Returns:
        A <hz_byte_stream_t> instance.
*/
HZ_INLINE hz_byte_stream_t
hz_byte_stream_create(u8 *data, size_t size)
{
    hz_byte_stream_t bs;
    bs.data = data;
    bs.size = size;
    bs.ptr = 0;
    bs.flags = 0;

    if (data != NULL && size > 0)
        bs.flags |= BS_BNDCHECK;

    return bs;
}

HZ_INLINE u8
unpack8(hz_byte_stream_t *bs)
{
    return *(bs->data + bs->ptr++);
}

HZ_INLINE u16
unpack16(hz_byte_stream_t *bs)
{
    u16 val = 0;

    val |= (u16) bs->data[bs->ptr+0] << 8;
    val |= (u16) bs->data[bs->ptr+1];

    bs->ptr += 2;
    return val;
}

HZ_INLINE u32
unpack32(hz_byte_stream_t *bs)
{
    u32 val = 0;

    val |= (u32) bs->data[bs->ptr+0] << 24;
    val |= (u32) bs->data[bs->ptr+1] << 16;
    val |= (u32) bs->data[bs->ptr+2] << 8;
    val |= (u32) bs->data[bs->ptr+3];

    bs->ptr += 4;
    return val;
}

HZ_INLINE u64
unpack64(hz_byte_stream_t *bs)
{
    u64 val = 0;

    val |= (u64) bs->data[bs->ptr+0] << 56;
    val |= (u64) bs->data[bs->ptr+1] << 48;
    val |= (u64) bs->data[bs->ptr+2] << 40;
    val |= (u64) bs->data[bs->ptr+3] << 32;
    val |= (u64) bs->data[bs->ptr+4] << 24;
    val |= (u64) bs->data[bs->ptr+5] << 16;
    val |= (u64) bs->data[bs->ptr+6] << 8;
    val |= (u64) bs->data[bs->ptr+7];

    bs->ptr += 8;
    return val;
}

HZ_INLINE char
peek_next(const char *p)
{
    return p[1];
}

HZ_INLINE void
unpackf(hz_byte_stream_t *bs,
        const char *f,
        ...)
{
    va_list ap;
    const char *c;

    if (bs == NULL || f == NULL) {
        DEBUG_MSG("read buffer or format string NULL!");
        return;
    }

    va_start(ap,f);

    for (c = f; *c != '\0'; ++c) {
        switch (*c) {
        case 'b': { /* byte (8-bit unsigned) */
            u8 *val = va_arg(ap, u8*);

            if (peek_next(c) == ':') {
                ++c;
                if (peek_next(c) == '*') {
                    int i, n;
                    n = va_arg(ap, int);
                    for (i = 0; i < n; ++i)
                        val[i] = unpack8(bs);
                }
            } else
                *val = unpack8(bs);

            break;
        }
        case 'h': { /* half (short, 16-bit unsigned */
            u16 *val = va_arg(ap, u16*);

            if (peek_next(c) == ':') {
                ++c;
                if (peek_next(c) == '*') {
                    int i, n;
                    n = va_arg(ap, int);
                    for (i = 0; i < n; ++i)
                        val[i] = unpack16(bs);
                }
            } else
                *val = unpack16(bs);

            break;
        }
        case 'i': { /* int (32-bit unsigned) */
            u32 *val = va_arg(ap, u32*);

            if (peek_next(c) == ':') {
                ++c;
                if (peek_next(c) == '*') {
                    int i, n;
                    n = va_arg(ap, int);
                    for (i = 0; i < n; ++i)
                        val[i] = unpack32(bs);
                }
            } else {
                *val = unpack32(bs);
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

#define FNV_OFFSET_BASIS_32 0x811c9dc5
#define FNV_PRIME_32 0x01000193

HZ_INLINE u32
hash_fnv1a(u32 val)
{
    u32 hash;
    u32 m;
    hash = FNV_OFFSET_BASIS_32;

    for (m = val; m; m>>=8) {
        hash ^= m & 0xff;
        hash *= FNV_PRIME_32;
    }

    return hash;
}

typedef struct hz_map_bucket_node_t {
    struct hz_map_bucket_node_t *prev, *next;
    u32 key;
    u32 value;
} hz_map_bucket_node_t;

typedef struct hz_map_bucket_t {
    struct hz_map_bucket_node_t *root;
} hz_map_bucket_t;

HZ_INLINE void
hz_map_bucket_init(hz_map_bucket_t *b)
{
    b->root = NULL;
}

typedef struct hz_map_t {
    hz_map_bucket_t *buckets;
    size_t bucket_count;
} hz_map_t;

HZ_INLINE hz_map_t *
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

HZ_INLINE void
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

HZ_INLINE hz_bool_t
hz_map_set_value(hz_map_t *map, u32 key, u32 value)
{
    u32 hash;
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

HZ_INLINE u32
hz_map_get_value(hz_map_t *map, u32 key)
{
    u32 hash = hash_fnv1a(key);
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

HZ_INLINE void
hz_map_remove(hz_map_t *map, u32 key)
{

}

HZ_INLINE hz_bool_t
hz_map_value_exists(hz_map_t *map, u32 key)
{
    u32 hash = hash_fnv1a(key);
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


HZ_INLINE hz_bool_t
hz_map_set_value_for_keys(hz_map_t *map, u32 k0, u32 k1, u32 value)
{
    hz_bool_t any_set = HZ_FALSE;
    u32 k = k0;

    while (k <= k1) {
        if (hz_map_set_value(map, k, value))
            any_set = HZ_TRUE;

        ++k;
    }

    return any_set;
}

/* Blob */
typedef struct hz_blob_t {
    u8 *data;
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

u8 *
hz_blob_get_data(hz_blob_t *blob)
{
    return blob->data;
}

hz_byte_stream_t
hz_blob_to_byte_stream(hz_blob_t *blob) {
    return hz_byte_stream_create(blob->data, blob->size);
}

typedef struct hz_face_table_node_t hz_face_table_node_t;

struct hz_face_table_node_t {
    hz_tag_t tag;
    hz_blob_t *blob;
    hz_face_table_node_t *prev, *next;
};

typedef struct hz_face_tables_t {
    hz_face_table_node_t *root;
} hz_face_tables_t;

typedef struct hz_face_ot_tables_t {
    u8 *BASE_table;
    u8 *GDEF_table;
    u8 *GSUB_table;
    u8 *GPOS_table;
    u8 *JSTF_table;
} hz_face_ot_tables_t;

struct hz_face_t {
    hz_face_tables_t tables;
    hz_face_ot_tables_t ot_tables;

    u16 num_glyphs;
    u16 num_of_h_metrics;
    u16 num_of_v_metrics;
    hz_metrics_t *metrics;

    i16 ascender;
    i16 descender;
    i16 linegap;

    u16 upem;

    hz_map_t *class_map;
    hz_map_t *attach_class_map;
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
    face->tables.root = NULL;
    face->class_map = hz_map_create();
    face->attach_class_map = hz_map_create();
    return face;
}

void
hz_face_destroy(hz_face_t *face)
{

}

u16
hz_face_get_upem(hz_face_t *face)
{
    return face->upem;
}

void
hz_face_set_upem(hz_face_t *face, u16 upem)
{
    face->upem = upem;
}


void
hz_face_set_table(hz_face_t *face, hz_tag_t tag, hz_blob_t *blob)
{
    hz_face_table_node_t *new_node = hz_malloc(sizeof(hz_face_table_node_t));
    new_node->tag = tag;
    new_node->blob = blob;
    new_node->next = NULL;

    if (face->tables.root == NULL) {
        new_node->prev = NULL;
        face->tables.root = new_node;
    } else {
        hz_face_table_node_t *node = face->tables.root;
        while (node != NULL) {
            if (node->next == NULL) {
                /* found last node */
                new_node->prev = node;
                node->next = new_node;
                break;
            }

            node = node->next;
        }
    }
}

hz_blob_t *
hz_face_reference_table(hz_face_t *face, hz_tag_t tag)
{
    hz_face_table_node_t *node = face->tables.root;

    while (node != NULL) {
        if (node->tag == tag)
            return node->blob; /* tags match, found table */

        node = node->next;
    }

    return NULL;
}

void
hz_face_set_num_glyphs(hz_face_t *face, u16 num_glyphs)
{
    face->num_glyphs = num_glyphs;
}

u16
hz_face_get_num_glyphs(hz_face_t *face)
{
    return face->num_glyphs;
}

u16
hz_face_get_num_of_h_metrics(hz_face_t *face)
{
    return face->num_of_h_metrics;
}

u16
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
hz_face_set_num_of_h_metrics(hz_face_t *face, u16 num_of_h_metrics)
{
    face->num_of_h_metrics = num_of_h_metrics;
}

void
hz_face_load_num_glyphs(hz_face_t *face)
{
    hz_blob_t *blob = hz_face_reference_table(face, HZ_TAG('m','a','x','p'));
    hz_byte_stream_t bs = hz_byte_stream_create(blob->data, blob->size);

    u32 version;
    u16 num_glyphs;

    version = unpack32(&bs);

    switch (version) {
        case 0x00005000: {
            /* version 0.5 */
            num_glyphs = unpack16(&bs);
            break;
        }
        case 0x00010000: {
            /* version 1.0 with full information */
            num_glyphs = unpack16(&bs);
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
    if (face->ot_tables.GDEF_table != NULL) {
        hz_byte_stream_t bs = hz_byte_stream_create(face->ot_tables.GDEF_table, 0);
        u32 version;

        Offset16 glyph_class_def_offset;
        Offset16 attach_list_offset;
        Offset16 lig_caret_list_offset;
        Offset16 mark_attach_class_def_offset;
        Offset16 mark_glyph_sets_def_offset;

        version = unpack32(&bs);

        switch (version) {
            case 0x00010000: /* 1.0 */
                unpackf(&bs, "hhhh",
                        &glyph_class_def_offset,
                        &attach_list_offset,
                        &lig_caret_list_offset,
                        &mark_attach_class_def_offset);
                break;
            case 0x00010002: /* 1.2 */
                unpackf(&bs, "hhhhh",
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
            hz_byte_stream_t subtable = hz_byte_stream_create(bs.data + glyph_class_def_offset, 0);
            u16 class_format;
            class_format = unpack16(&subtable);
            switch (class_format) {
                case 1:
                    break;
                case 2: {
                    u16 range_index = 0, class_range_count;
                    class_range_count = unpack16(&subtable);

                    while (range_index < class_range_count) {
                        u16 start_glyph_id, end_glyph_id, glyph_class;
                        unpackf(&subtable, "hhh",
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
            hz_byte_stream_t subtable = hz_byte_stream_create(bs.data + mark_attach_class_def_offset, 0);
            u16 class_format;
            class_format = unpack16(&subtable);
            switch (class_format) {
                case 1:
                    break;
                case 2: {
                    u16 range_index = 0, class_range_count;
                    class_range_count = unpack16(&subtable);

                    while (range_index < class_range_count) {
                        u16 start_glyph_id, end_glyph_id, glyph_class;
                        unpackf(&subtable, "hhh",
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
    u16 data;
    hz_kern_coverage_field_t field;
} hz_kern_coverage_t;

hz_error_t
hz_face_load_kerning_pairs(hz_face_t *face)
{
    hz_byte_stream_t bs;
    u16 version, i, n;
    hz_blob_t *blob = hz_face_reference_table(face, HZ_TAG('k','e','r','n'));

    if (blob == NULL) {
        return HZ_ERROR_TABLE_DOES_NOT_EXIST;
    }

    bs = hz_byte_stream_create(blob->data, blob->size);
    version = unpack16(&bs);
    n = unpack16(&bs);

    if (version != 0) {
        return HZ_ERROR_INVALID_TABLE_VERSION;
    }

    for (i = 0; i < n; ++i) {
        u16 version, length;
        hz_kern_coverage_field_t coverage;
        unpackf(&bs, "hhh", &version, &length, &coverage);

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

/*
    Struct: hz_shape_call_cache_t

        Holds the cache of all OpenType lookups required in a
        call to <hz_shape>.

*/
typedef struct hz_shape_call_cache_t {
    u32 lookup_count;
} hz_shape_call_cache_t;

void
hz_shape_cached_call(hz_font_t *font,
                     hz_sequence_t *sequence,
                     const hz_feature_t *features,
                     unsigned int num_features)
{
    hz_shape_call_cache_t cache;

}

void
hz_shape(hz_font_t *font,
         hz_sequence_t *sequence,
         const hz_feature_t *features,
         unsigned int num_features)
{
    hz_shape_cached_call(font, sequence, features, num_features);
}

hz_blob_t*
hz_ft_load_snft_table(FT_Face face, hz_tag_t tag)
{
    FT_ULong size, ft_tag, length;
    
    size = 0;
    ft_tag = tag;
    
    FT_Load_Sfnt_Table(face, tag, 0, NULL, &size);

    if (size) {
        hz_blob_t *blob = hz_blob_create();
        hz_blob_resize(blob, size);
        FT_Load_Sfnt_Table(face, tag, 0, hz_blob_get_data(blob), &size);
        return blob;
    }

    return NULL;
}

static const hz_tag_t HZ_REQUIRED_FT_TABLE_TAGS[] = {
        HZ_TAG('c','m','a','p'),
        HZ_TAG('m','a','x','p'),
        HZ_TAG('g','l','y','f'),
        HZ_TAG('h','m','t','x'),
        HZ_TAG('k','e','r','n'),
};

hz_font_t *
hz_ft_font_create(FT_Face ft_face)
{
    size_t     i;
    hz_tag_t   *tags;
    size_t     n_tags;
    hz_face_t  *face;
    hz_font_t  *font;

    FT_Bytes BASE_table;
    FT_Bytes GDEF_table;
    FT_Bytes GSUB_table;
    FT_Bytes GPOS_table;
    FT_Bytes JSTF_table;

    if (ft_face == NULL) {
        DEBUG_MSG("FreeType face passed in is NULL!");
        return NULL;
    }

    tags = HZ_REQUIRED_FT_TABLE_TAGS;
    n_tags = HZ_ARRLEN(HZ_REQUIRED_FT_TABLE_TAGS);

    font = hz_font_create();
    face = hz_face_create();
    hz_face_set_upem(face, ft_face->units_per_EM);

    if (FT_OpenType_Validate(ft_face, FT_VALIDATE_OT,
                             &BASE_table, &GDEF_table, &GPOS_table, &GSUB_table, &JSTF_table)
                             != FT_Err_Ok) {
        ERROR_MSG("Failed to validate OpenType tables!");
        hz_face_destroy(face);
        hz_font_destroy(font);
        return NULL;
    }

    face->ot_tables.BASE_table = (hz_byte_t *)BASE_table;
    face->ot_tables.GDEF_table = (hz_byte_t *)GDEF_table;
    face->ot_tables.GSUB_table = (hz_byte_t *)GSUB_table;
    face->ot_tables.GPOS_table = (hz_byte_t *)GPOS_table;
    face->ot_tables.JSTF_table = (hz_byte_t *)JSTF_table;

    for (i = 0; i < n_tags; ++i) {
        hz_blob_t *blob = hz_ft_load_snft_table(ft_face, tags[i]);
        if (blob != NULL) {
            hz_face_set_table(face, tags[i], blob);
        }
    }

    hz_face_load_num_glyphs(face);
    face->metrics = hz_malloc(sizeof(hz_metrics_t) * face->num_glyphs);

    for (i = 0; i < face->num_glyphs; ++i) {
        FT_GlyphSlot slot = ft_face->glyph;
        if (FT_Load_Glyph(ft_face, i, FT_LOAD_NO_BITMAP | FT_LOAD_NO_SCALE)  == FT_Err_Ok) {
            hz_metrics_t *mo;
            FT_Glyph_Metrics *mi;
            FT_BBox bbox;
            FT_Glyph glyph;
            mi = &ft_face->glyph->metrics;

            /* Load glyph bounding box */
            FT_Get_Glyph(slot, &glyph);
            FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_SUBPIXELS, &bbox);
            FT_Done_Glyph(glyph);

            mo = hz_face_get_glyph_metrics(face, i);
            
            mo->xa = mi->horiAdvance;
            mo->ya = mi->vertAdvance;
            mo->xb = mi->horiBearingX;
            mo->yb = mi->horiBearingY;
            mo->w = mi->width;
            mo->h = mi->height;
            mo->bbox.x0 = bbox.xMin;
            mo->bbox.x1 = bbox.xMax;
            mo->bbox.y0 = bbox.yMin;
            mo->bbox.y1 = bbox.yMax;
        }
    }

    hz_face_load_class_maps(face);
    hz_face_load_kerning_pairs(face);
    hz_font_set_face(font, face);
    return font;
}
