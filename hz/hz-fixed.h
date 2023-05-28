#ifndef HZ_H
#define HZ_H

#define HZ_COMPILER_UNKNOWN 0ul
#define HZ_COMPILER_GCC 0x00000001ul
#define HZ_COMPILER_CLANG 0x00000002ul
#define HZ_COMPILER_TURBOC 0x00000004ul
#define HZ_COMPILER_VC 0x00000008ul
#define HZ_COMPILER_INTEL 0x00000010ul
#define HZ_COMPILER_BORLAND 0x00000020ul
#define HZ_COMPILER_EMBARCADERO 0x00000040ul

#if defined(_MSC_VER)
#   define HZ_COMPILER HZ_COMPILER_VC
#elif defined(__GNUC__) || defined(__GCC__)
#   define HZ_COMPILER HZ_COMPILER_GCC
#elif defined(__clang__)
#   define HZ_COMPILER HZ_COMPILER_CLANG
#else
#   define HZ_COMPILER HZ_COMPILER_UNKNOWN
#endif

#if HZ_COMPILER & HZ_COMPILER_VC
#pragma warning(disable:4068) // disable #pragma related warnings
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#ifdef HZ_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#endif
#include <stb_truetype.h>

#define HZ_ARCH_X86_BIT        0x00010000ul
#define HZ_ARCH_ARM_BIT        0x00020000ul
#define HZ_ARCH_ARMV7_BIT      0x00040000ul
#define HZ_ARCH_ARMV8_BIT      0x00080000ul
#define HZ_ARCH_NEON_BIT       0x00100000ul
#define HZ_ARCH_POWERPC_BIT    0x00200000ul
#define HZ_ARCH_MIPS_BIT       0x00400000ul
#define HZ_ARCH_RISCV_BIT      0x00800000ul
#define HZ_ARCH_SSE_BIT        0x00000001ul
#define HZ_ARCH_SSE2_BIT       0x00000002ul
#define HZ_ARCH_SSE3_BIT       0x00000004ul
#define HZ_ARCH_SSSE3_BIT      0x00000008ul
#define HZ_ARCH_SSE41_BIT      0x00000010ul
#define HZ_ARCH_SSE42_BIT      0x00000020ul
#define HZ_ARCH_SSE4A_BIT      0x00000040ul
#define HZ_ARCH_AVX_BIT        0x00000080ul
#define HZ_ARCH_AVX2_BIT       0x00000100ul
#define HZ_ARCH_AVX512_BIT     0x00000200ul
#define HZ_ARCH_AVX512_VL_BIT  0x00000400ul
#define HZ_ARCH_AES_BIT        0x00000400ul

#define HZ_ARCH_UNKNOWN   0
#define HZ_ARCH_X86       (HZ_ARCH_X86_BIT)
#define HZ_ARCH_SSE       (HZ_ARCH_X86 | HZ_ARCH_SSE_BIT)
#define HZ_ARCH_SSE2      (HZ_ARCH_SSE | HZ_ARCH_SSE2_BIT)
#define HZ_ARCH_SSE3      (HZ_ARCH_SSE2 | HZ_ARCH_SSE3_BIT)
#define HZ_ARCH_SSSE3     (HZ_ARCH_SSE3 | HZ_ARCH_SSSE3_BIT)
#define HZ_ARCH_SSE41     (HZ_ARCH_SSSE3 | HZ_ARCH_SSE41_BIT)
#define HZ_ARCH_SSE42     (HZ_ARCH_SSE41 | HZ_ARCH_SSE42_BIT)
#define HZ_ARCH_SSE4A     (HZ_ARCH_SSE42 | HZ_ARCH_SSE4A_BIT)
#define HZ_ARCH_AVX       (HZ_ARCH_SSE42 | HZ_ARCH_AVX_BIT)
#define HZ_ARCH_AVX2      (HZ_ARCH_AVX | HZ_ARCH_AVX2_BIT)
#define HZ_ARCH_AVX512    (HZ_ARCH_AVX2 | HZ_ARCH_AVX512_BIT)
#define HZ_ARCH_AVX512_VL (HZ_ARCH_AVX512 | HZ_ARCH_AVX512_VL_BIT)
#define HZ_ARCH_AES       (HZ_ARCH_AES_BIT)
#define HZ_ARCH_ARM       (HZ_ARCH_ARM_BIT)
#define HZ_ARCH_ARMV7     (HZ_ARCH_ARM | HZ_ARCH_ARMV7_BIT)
#define HZ_ARCH_ARMV8     (HZ_ARCH_ARM | HZ_ARCH_ARMV8_BIT)
#define HZ_ARCH_NEON      (HZ_ARCH_ARMV7 | HZ_ARCH_NEON_BIT)

#if defined(__AVX2__)
#   define HZ_ARCH HZ_ARCH_AVX2
#   include <immintrin.h>
#elif defined(__AVX__)
#   define HZ_ARCH HZ_ARCH_AVX
#elif defined(__SSE4_2__)
#   define HZ_ARCH HZ_ARCH_SSE42
#elif defined(__SSE4_1__)
#   define HZ_ARCH HZ_ARCH_SSE41
#elif defined(__SSSE3__)
#   define HZ_ARCH HZ_ARCH_SSSE3
#elif defined(__SSE2__) || defined(__x86_64__) || defined(_M_X64) || defined(_M_IX86_FP)
#   define HZ_ARCH HZ_ARCH_SSE2
#elif defined(__i386__)
#   define HZ_ARCH (HZ_ARCH_X86)
#elif defined(__ARM_ARCH) && (__ARM_ARCH >= 8)
#   define HZ_ARCH (HZ_ARCH_ARMV8)
#elif defined(__ARM_NEON)
#   define HZ_ARCH (HZ_ARCH_ARM | HZ_ARCH_NEON)
#   include <neon.h>
#elif defined(__arm__) || defined(_M_ARM)
#   define HZ_ARCH (HZ_ARCH_ARM)
#elif defined(__mips__)
#   define HZ_ARCH (HZ_ARCH_MIPS)
#elif defined(__powerpc__) || defined(_M_PPC)
#   define HZ_ARCH (HZ_ARCH_PPC)
#else
#   define HZ_ARCH (HZ_ARCH_UNKNOWN)
#endif

#if HZ_COMPILER & HZ_COMPILER_VC
#   define HZ_ALWAYS_INLINE inline __forceinline
#   define HZ_FASTCALL __fastcall
#   define HZ_DLLEXPORT __declspec(dllexport)
#elif HZ_COMPILER & (HZ_COMPILER_GCC | HZ_COMPILER_CLANG)
#   define HZ_ALWAYS_INLINE inline __attribute__((always_inline))
#   define HZ_FASTCALL __attribute__((fastcall))
#   define HZ_DLLEXPORT __attribute__((dllexport))
#else
#   define HZ_ALWAYS_INLINE inline
#   define HZ_FASTCALL
#   define HZ_DLLEXPORT
#endif

#define HZ_STATIC static
#define HZ_INLINE inline

#if defined(HZ_BUILD_SHARED) && defined(_WIN32)
#define HZ_DECL HZ_DLLEXPORT
#else
#define HZ_DECL
#endif

#if HZ_COMPILER & HZ_COMPILER_VC
#define HZ_BREAKPOINT __debugbreak()
#elif HZ_COMPILER & HZ_COMPILER_GCC
#define HZ_BREAKPOINT __builtin_trap()
#else
#endif

#define HZ_ARRLEN(x) (sizeof(x)/sizeof((x)[0]))
#define HZ_UNARR(x) x, (sizeof(x)/sizeof((x)[0]))
#define HZ_ASSERT(cond) assert(cond)
#define HZ_TAG(a, b, c, d) ((hz_tag_t)d | ((hz_tag_t)c << 8) | ((hz_tag_t)b << 16) | ((hz_tag_t)a << 24U))
#define HZ_UNTAG(tag) (char) ((tag >> 24) & 0xFF), (char)((tag >> 16) & 0xFF), (char)((tag >> 8) & 0xFF), (char)(tag & 0xFF)
#define HZ_TAG_NONE ((hz_tag_t)0)

#define HZ_FLAG(n) (1ul << (n))
#define Fixed26Dot6 uint32_t

#define HZ_IGNORE_ARG(x) (void)(x)

#define HZ_DEFAULT_FONT_DATA_ARENA_SIZE (1024*1024)/*1MiB*/

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t hz_ucs2_char_t;
typedef uint32_t hz_ucs4_char_t;
typedef int8_t hz_char8_t;
typedef uint8_t hz_byte_t;
typedef int32_t hz_coord_t;
typedef uint32_t hz_unicode_t;
typedef uint32_t hz_tag_t;
typedef uint32_t hz_bool;

#define HZ_TRUE 1
#define HZ_FALSE 0

typedef uint16_t hz_index_t;
typedef int32_t hz_fixed32_t, hz_position_t, hz_fixed26dot6_t;

// dynamic array header
typedef struct {
    size_t size, capacity;
    size_t member_size;
} hz_vector_hdr_t;

void hz_vector_init(void **v, size_t member_size);
hz_vector_hdr_t *hz_vector_header(void *v);
hz_bool hz_vector_is_empty(void *v);
void hz_vector_clear_impl(void **v);
void hz_vector_destroy_impl(void **v);
void hz_vector_reserve_impl(void **v, size_t new_cap);
size_t hz_vector_size_impl(void *v);
void hz_vector_resize_impl(void **v, size_t new_size);
void hz_vector_grow(void **v, int extra);
hz_bool hz_vector_need_grow(void *v, size_t extra);
#define hz_vector(__T) __T *
#define hz_vector_size(__ARR) hz_vector_size_impl((void*)(__ARR))
#define hz_vector_resize(__ARR, __SIZE) do { hz_vector_init((void**)&(__ARR), sizeof(*(__ARR))); hz_vector_resize_impl((void**)&(__ARR), __SIZE); } while(0)
#define hz_vector_destroy(__ARR) hz_vector_destroy_impl((void**)&(__ARR))
#define hz_vector_reserve(__ARR, __CAPACITY) do { hz_vector_init((void**)&(__ARR), sizeof(*(__ARR))); hz_vector_reserve_impl((void**)&(__ARR), __CAPACITY); } while(0)
#define hz_vector_clear(__ARR) hz_vector_clear_impl((void**)&(__ARR))
#define hz_vector_push_back(__ARR, __ARRVAL) do {\
hz_vector_init((void **)&(__ARR), sizeof(*(__ARR)));\
if (!(__ARR) || ((__ARR) && hz_vector_need_grow(__ARR, 1))) {\
hz_vector_grow((void **)&(__ARR),1);\
}\
(__ARR)[hz_vector_size(__ARR)] = (__ARRVAL);\
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

#define hz_vector_pop(__ARR) hz_vector_resize(__ARR, hz_vector_size(__ARR)-1)
#define hz_vector_top(__ARR) (&((__ARR)[hz_vector_size(__ARR)-1]))

typedef struct {
    hz_position_t x0, y0, x1, y1;
} hz_bounds2i_t;

typedef struct {
    hz_bounds2i_t bounds;
    hz_position_t xAdvance, yAdvance, xBearing, yBearing;
    hz_position_t w, h;
} hz_metrics_t;

typedef enum {
    HZ_DIRECTION_INVALID = 0,
    HZ_DIRECTION_LTR = HZ_FLAG(0),
    HZ_DIRECTION_RTL = HZ_FLAG(1),
    HZ_DIRECTION_TTB = HZ_FLAG(2),
    HZ_DIRECTION_BTT = HZ_FLAG(3)
} hz_direction_t;

#define HZ_DIRECTION_IS_HORIZONTAL(dir) (hz_bool_t)(dir & 0x3)
#define HZ_DIRECTION_IS_VERTICAL(dir) (hz_bool_t)(dir & 0xC)

typedef enum {
    HZ_OK = 0ul,
    HZ_ERROR_INVALID_TABLE_TAG              = HZ_FLAG(0),
    HZ_ERROR_INVALID_TABLE_VERSION          = HZ_FLAG(1),
    HZ_ERROR_INVALID_LOOKUP_TYPE            = HZ_FLAG(2),
    HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT = HZ_FLAG(3),
    HZ_ERROR_INVALID_PARAM                  = HZ_FLAG(4),
    HZ_ERROR_INVALID_FORMAT                 = HZ_FLAG(5),
    HZ_ERROR_TABLE_DOES_NOT_EXIST           = HZ_FLAG(6),
    HZ_ERROR_UNEXPECTED_VALUE               = HZ_FLAG(7),
    HZ_ERROR_SETUP_FAILED                   = HZ_FLAG(8)
} hz_error_t;

/*  Enum: hz_glyph_class_t
 *      HZ_GLYPH_CLASS_BASE - Base glyph.
 *      HZ_GLYPH_CLASS_LIGATURE - Ligature glyph.
 *      HZ_GLYPH_CLASS_MARK - Mark glyph.
 *      HZ_GLYPH_CLASS_COMPONENT - Mark component.
 * */
typedef enum {
    HZ_GLYPH_CLASS_ZERO      = 0,
    HZ_GLYPH_CLASS_BASE      = HZ_FLAG(0),
    HZ_GLYPH_CLASS_LIGATURE  = HZ_FLAG(1),
    HZ_GLYPH_CLASS_MARK      = HZ_FLAG(2),
    HZ_GLYPH_CLASS_COMPONENT = HZ_FLAG(3)
} hz_glyph_class_t;

/* include tables, types, and functions and other generated code */
#include "hz_data_tables.h"

/* Enum: hz_encoding_t 
 *      HZ_ENCODING_ASCII - ASCII (7-bit signed char)
 *      HZ_ENCODING_UTF8 - UTF-8
 *      HZ_ENCODING_UTF16 - UTF-16
 *      HZ_ENCODING_UTF32 - UTF-32
 *      HZ_ENCODING_UCS2 - UCS-2 (16-bit Universal Character Set, Fixed-Width encoding)
 *      HZ_ENCODING_UCS4 - UCS-4
 *      HZ_ENCODING_JOHAB - Johab
 *      HZ_DFLT_ENCODING - Default encoding (ASCII)
 */
typedef enum {
    HZ_ENCODING_ASCII,
    HZ_ENCODING_LATIN1,
    HZ_ENCODING_UTF8,
    HZ_ENCODING_UTF16,
    HZ_ENCODING_UTF32,
    HZ_ENCODING_UCS2,
    HZ_ENCODING_UCS4,
    HZ_ENCODING_JOHAB,
    HZ_DFLT_ENCODING = HZ_ENCODING_ASCII,
} hz_encoding_t;

typedef uint32_t hz_char32_t;
typedef uint16_t hz_char16_t;

typedef struct  hz_glyph_metrics_t {
    Fixed26Dot6 xAdvance;
    Fixed26Dot6 yAdvance;
    Fixed26Dot6 xOffset;
    Fixed26Dot6 yOffset;
} hz_glyph_metrics_t;

typedef enum hz_glyph_attrib_flags_t {
    HZ_GLYPH_ATTRIB_METRICS_BIT          = HZ_FLAG(0),
    HZ_GLYPH_ATTRIB_INDEX_BIT            = HZ_FLAG(1),
    HZ_GLYPH_ATTRIB_CODEPOINT_BIT        = HZ_FLAG(2),
    HZ_GLYPH_ATTRIB_GLYPH_CLASS_BIT      = HZ_FLAG(3),
    HZ_GLYPH_ATTRIB_ATTACHMENT_CLASS_BIT = HZ_FLAG(4),
    HZ_GLYPH_ATTRIB_COMPONENT_INDEX_BIT  = HZ_FLAG(5),
} hz_glyph_attrib_flags_t;

/* Struct: hz_buffer_t */
typedef struct {
    size_t                  glyph_count;
    hz_glyph_metrics_t *    glyph_metrics;
    hz_index_t *            glyph_indices;
    hz_unicode_t *          codepoints;
    uint16_t *              glyph_classes;
    uint16_t *              attachment_classes;
    uint16_t *              component_indices;
    hz_glyph_attrib_flags_t attrib_flags;
} hz_buffer_t;

/* enum: hz_shape_flags_t */
typedef enum hz_shape_flags_t {
    HZ_SHAPER_AUTO_LOAD_FEATURES = HZ_FLAG(0),
    HZ_SHAPER_CULL_MARKS         = HZ_FLAG(1),
    HZ_SHAPER_CULL_BASES         = HZ_FLAG(2)
} hz_shaper_flags_t;

/*  enum: hz_setup_flags_t
 *      Set of special flags passed in to <hz_setup> used to configure the library.
 */
typedef enum hz_setup_flags_t {
    HZ_QUERY_CPU_FOR_SIMD = HZ_FLAG(0),
} hz_setup_flags_t;

/*  Function: hz_setup
 *      Sets up the library's internal structures and lookup tables.
 *      If HZ_USE_CPUID_FOR_SIMD_CHECKS flag is set it will also initialize a special hardware specific info to allow for runtime
 *      SIMD specialization. Otherwise, it will use compile-time macros to determine which instruction set to optimize with.
 *
 *  Returns:
 *      Returns HZ_OK on success, otherwise HZ_ERROR_SETUP_FAILED.
 * */
HZ_DECL hz_error_t hz_setup(hz_setup_flags_t flags);

/*  function: hz_cleanup
 *      Cleans up all the data initialized by <hz_setup> as well as any shaper cache that might've been allocated.
 */
HZ_DECL void hz_cleanup(void);

/*  Function: hz_lang
 *      Finds language based on <ISO 639-2: https://www.loc.gov/standards/iso639-2/php/code_list.php">
 *      or <ISO 639-3: https://iso639-3.sil.org/"> language tag.
 *
 *  Parameters:
 *      tag - An <ISO 639-2: https://www.loc.gov/standards/iso639-2/php/code_list.php> or <ISO 639-3: https://iso639-3.sil.org/> language tag.
 *
 *  Returns:
 *      Returns <hz_language_t> enum value based on OpenType language tag.
 */
HZ_DECL hz_language_t hz_lang(const char *tag);

/*  Function: hz_lang_iso639_1_2002
 *      Finds language based on old <ISO 639-1:2002: https://id.loc.gov/vocabulary/iso639-1.html> two-character language tags.
 *
 *  Parameters:
 *      tag - An <ISO 639-1:2002: https://id.loc.gov/vocabulary/iso639-1.html> language tag.
 *
 *  Returns:
 *      Returns <hz_language_t> enum value based on OpenType language tag.
 */
HZ_DECL hz_language_t hz_lang_iso639_1_2002(const char *tag);

/* function: hz_script */
HZ_DECL hz_script_t hz_script(const char *tag);

/* struct: hz_face_t */
typedef struct hz_face_t hz_face_t;

/* function: hz_face_create */
HZ_DECL hz_face_t* hz_face_create(void);

/* function: hz_face_destroy */
HZ_DECL void hz_face_destroy(hz_face_t *face);

/* function: hz_face_get_upem */
HZ_DECL uint16_t hz_face_get_upem(hz_face_t *face);

/* function: hz_face_set_upem */
HZ_DECL void hz_face_set_upem(hz_face_t *face, uint16_t upem);

/* function: hz_face_get_glyph_metrics */
HZ_DECL hz_metrics_t *
hz_face_get_glyph_metrics(hz_face_t *face, hz_index_t id);

/* function: hz_face_get_num_glyphs */
HZ_DECL uint16_t
hz_face_get_num_glyphs(hz_face_t *face);

/* struct: hz_font_t */
typedef struct hz_font_t hz_font_t;

/* function: hz_font_create */
HZ_DECL hz_font_t* hz_font_create(void);

/* function: hz_font_destroy */
HZ_DECL void hz_font_destroy(hz_font_t *font);

/* function: hz_font_get_face */
HZ_DECL hz_face_t* hz_font_get_face(hz_font_t *font);

/* function: hz_font_set_face */
HZ_DECL void hz_font_set_face(hz_font_t *font, hz_face_t *face);

HZ_DECL void hz_font_get_glyph_shape();

/*  function: hz_stbtt_font_create
 *      Loads a font from a stbtt_fontinfo structure.
 * */
HZ_DECL hz_font_t* hz_stbtt_font_create(stbtt_fontinfo *info);

typedef struct hz_shaper_t hz_shaper_t;

typedef int8_t hz_int8;
typedef uint8_t hz_byte, hz_uint8;
typedef uint8_t char8_t;
typedef uint16_t hz_uint16;
typedef int16_t hz_int16;
typedef uint32_t hz_uint32;
typedef int32_t hz_int32;
#define HZ_MEMSET hz_memset
#define hz_zero(_Data, _Size) HZ_MEMSET(_Data,0,_Size)
#define hz_zero_struct(_Struct) HZ_MEMSET((void *)&(_Struct),0,sizeof(_Struct))
#define HZ_PRIVATE static
#define Offset16 hz_uint16
#define Offset32 hz_uint32
#define Version16Dot16 hz_uint32

typedef struct {
    uint16_t format;
} hz_lookup_subtable_t;

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
    uint16_t lookup_type, lookup_flags;
    uint16_t subtable_count;
    hz_lookup_subtable_t **subtables;
    /* Index (base 0) into GDEF mark glyph sets structure.
     * This field is only present if the USE_MARK_FILTERING_SET lookup flag is set.
     */
    hz_coverage_t *mark_filtering_set;
} hz_lookup_table_t;

typedef struct {
    uint8_t *mem;
    size_t size;
    uintptr_t pos;
} hz_memory_arena_t;

typedef enum {
    HZ_CMD_ALLOC,
    HZ_CMD_FREE,
    HZ_CMD_REALLOC,
    HZ_CMD_RESET,
    HZ_CMD_RELEASE
} hz_allocator_cmd_t;

// if align argument is 0, then the allocator should try and align the data optimally
// in the way it sees best fit the block size.
typedef void* (*hz_allocator_fn_t)(void *user, hz_allocator_cmd_t cmd, void *ptr, size_t size, size_t align);

typedef struct {
    hz_allocator_fn_t allocfn;
    void *user;
} hz_allocator_t;

typedef struct {
    // = NULL (reserved for offset to FeatureParams)
    Offset16 feature_params;
    // Number of LookupList indices for this feature
    uint16_t lookup_index_count;
    // Array of indices into the LookupList — zero-based (first lookup is LookupListIndex = 0)
    uint16_t *lookup_list_indices;
} hz_feature_table_t;

typedef struct {
    hz_tag_t tag;
    hz_feature_table_t table;
} hz_feature_list_item_t;

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

typedef struct hz_font_data_t {
    hz_face_t *face;
    uint8_t *memory_arena_data;
    hz_memory_arena_t memory_arena;
    hz_allocator_t allocator;
    /* OT data */
    hz_gsub_table_t gsub_table;
    hz_gpos_table_t gpos_table;
} hz_font_data_t;

typedef struct {
    const char8_t      *data;
    hz_unicode_t        codepoint;
    uintptr_t           ptr;
    size_t              size; /* if 0, no limit */
    int                 step;
    uint32_t            flags;
} hz_utf8_decoder_t;

#define HZ_SHAPER_ARENA_SIZE 5000

struct hz_shaper_t {
    char ar [HZ_SHAPER_ARENA_SIZE]; // used to store the feature list

    hz_memory_arena_t memory_arena;

    hz_feature_t *features;
    size_t num_features;
    hz_direction_t direction;
    hz_script_t script;
    hz_language_t language;
    hz_shaper_flags_t flags;
};

/*
 *  Function: hz_shape_sz1
 *      Shape procedure for NUL-terminated string of user-provided character encoding. For out_buffer the <hz_buffer_init> function must be called prior to passing it in.
 *      Otherwise, it must be an empty <hz_buffer_t> (with <hz_buffer_release> or <hz_buffer_clear> being called on it).
 *
 *  Parameters:
 *      shaper - The shaper.
 *      font_data - Font data required for the shaper.
 *      encoding - Text encoding for sz_input, if HZ_DFLT_ENCODING is passed then the function reads it as ASCII.
 *      sz_input - NUL-terminated string of user-defined encoding.
 *      out_buffer - The output buffer.
 */
HZ_DECL void hz_shape_sz1(hz_shaper_t* shaper, hz_font_data_t* font_data, hz_encoding_t encoding, const void* sz_input, hz_buffer_t *out_buffer);

// set a custom generic internal allocator function.
HZ_DECL void hz_set_allocator_fn(hz_allocator_fn_t allocfn);
// set the user pointer for the internal allocator.
HZ_DECL void hz_set_allocator_user_pointer(void *user);

HZ_DECL void hz_font_data_init(hz_font_data_t *font_data, size_t arena_size);
HZ_DECL void hz_font_data_load(hz_font_data_t *font_data, hz_font_t *font);
HZ_DECL void hz_font_data_release(hz_font_data_t *font_data);

HZ_DECL void hz_buffer_init(hz_buffer_t *buffer);
HZ_DECL void hz_buffer_release(hz_buffer_t *buffer);

HZ_DECL void hz_font_destroy(hz_font_t *font);

HZ_DECL void hz_shaper_init(hz_shaper_t *shaper);
HZ_DECL void hz_shaper_set_direction(hz_shaper_t *shaper, hz_direction_t direction);
HZ_DECL void hz_shaper_set_script(hz_shaper_t *shaper, hz_script_t script);  
HZ_DECL void hz_shaper_set_language(hz_shaper_t *shaper, hz_language_t language);
HZ_DECL void hz_shaper_set_features(hz_shaper_t *shaper, const hz_feature_t *features, size_t num_features);

#ifdef HZ_IMPLEMENTATION

HZ_ALWAYS_INLINE void hz_memset(void *data, int val, size_t size)
{
    memset(data,val,size);
}

HZ_ALWAYS_INLINE void hz_memcpy(void *dst, const void *src, size_t size)
{
    memcpy(dst,src,size);
}

#include <assert.h>
#include <stdarg.h>

#define SIZEOF_VOIDPTR sizeof(void*)
#define DEFAULT_ALIGNMENT (2*sizeof(void*))

#define KIB 1024

#define UTF_FAILED (-1)
#define UTF_END_OF_BUFFER (-2)
#define UTF_OK 1

// Configuration macros
#define HZ_RELY_ON_UNSAFE_CMAP_CONSTANTS HZ_TRUE
#define HZ_ENABLE_SIMD 0
#define HZ_USE_ISO639_1_2002_TAGS HZ_FALSE

// Max depth of nested OpenType lookups
#define HZ_MAX_RECURSE_DEPTH 16

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

#ifdef HZ_DEBUG_LOGGING

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

#define hz_logf(Severity, ...) hz_flog(Severity, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define hz_logln(Severity,Msg) hz_flog(Severity,__FILE__, __FUNCTION__, __LINE__,"%s\n",Msg)
#else
#define hz_logf(Severity, ...) 
#define hz_logln(Severity,Msg)
#endif

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

static hz_allocator_t s_hz_alctr = { .allocfn = hz_standard_c_allocator_fn, .user = NULL };

hz_allocator_t *hz_get_allocator(void)
{
    return &s_hz_alctr;
}

void hz_set_allocator_fn(hz_allocator_fn_t allocfn)
{
    s_hz_alctr.allocfn = allocfn;
}

void hz_set_allocator_user_data(void *user)
{
    s_hz_alctr.user = user;
}

void* hz_malloc(size_t size)
{
    return s_hz_alctr.allocfn(s_hz_alctr.user,HZ_CMD_ALLOC,NULL,size,1);
}

void* hz_realloc(void* pointer, size_t size)
{
    return s_hz_alctr.allocfn(s_hz_alctr.user,HZ_CMD_REALLOC,pointer,size,1);
}

void hz_free(void *pointer)
{
    s_hz_alctr.allocfn(s_hz_alctr.user,HZ_CMD_FREE,pointer,0,1);
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

HZ_ALWAYS_INLINE void hz_byte_swap_16(uint16_t *p)
{
    *p = (*p << 8) | (*p >> 8);
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

uint64_t hz_clzll(uint64_t x) {
    uint64_t r = 0;
    while (!(x & ((uint64_t)1 << (uint64_t)63))) {
        x <<= 1;
        ++r;
    }

    return r;
}

int hz_is_power_of_two(uint64_t val) {
    return ~(val & (val-1));
}

hz_memory_arena_t hz_memory_arena_create(uint8_t *mem, size_t size)
{
    return (hz_memory_arena_t){.mem = mem, .size = size, .pos = 0};
}

void hz_memory_arena_init(hz_memory_arena_t *arena, uint8_t *mem, size_t size)
{
    arena->mem = mem; arena->size = size; arena->pos = 0;
}

// align must be a power of two
uintptr_t hz_align_forward(uintptr_t addr, uintptr_t align)
{
    return (~addr+1) & (align-1);
}

uint64_t hz_calc_next_pow2_size(uint64_t size)
{
    return ((uint64_t)1 << 63) >> (hz_clzll(size)-1);
}

void *hz_memory_arena_alloc_aligned(hz_memory_arena_t *arena, size_t size, size_t align)
{
    HZ_ASSERT(align > 0);
    HZ_ASSERT(hz_is_power_of_two(align));
    uint64_t ptr = (uint64_t)(arena->mem + arena->pos);
    uint64_t padd = hz_align_forward(ptr, align);

    // do we have enough memory for this allocation?
    uint64_t start = arena->pos + padd, end = start + size;
    if (end > arena->size) {
        return NULL;
    }

    arena->pos = end;
    return arena->mem + start;
}

void *hz_memory_arena_alloc(hz_memory_arena_t *arena, size_t size)
{
    return hz_memory_arena_alloc_aligned(arena, size, DEFAULT_ALIGNMENT);
}

void hz_memory_arena_release(hz_memory_arena_t *arena) {}

void hz_memory_arena_reset(hz_memory_arena_t *arena) {
    arena->pos = 0;
}

void *hz_memory_arena_alloc_fn(void *user, hz_allocator_cmd_t cmd, void *ptr, size_t size, size_t align)
{
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
    HZ_ASSERT(hz_is_power_of_two(align));

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

typedef enum {
    HZ_LITTLE_ENDIAN = 0x03020100ul,
    HZ_BIG_ENDIAN = 0x00010203ul,
    HZ_PDP_ENDIAN = 0x01000302ul
} hz_endianness_t;

static const union { unsigned char bytes[4]; uint32_t value; } hz_host_order =
    { { 0, 1, 2, 3 } };

#define HZ_HOST_ORDER (hz_host_order.value)

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
    p->must_bswap = HZ_HOST_ORDER == HZ_LITTLE_ENDIAN;
    p->curr_state = NULL;
    p->offset = 0;
    p->start = 0;
    p->stackmem = hz_malloc(HZ_PARSER_STACK_SIZE);
    hz_stack_init(&p->stack, (uint8_t *)p->stackmem, HZ_PARSER_STACK_SIZE);
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
    while (size > 0) {
        *write_addr++ = hz_parser_read_u32(p);
        --size;
    }
}

void hz_parser_read_u64_block(hz_parser_t *p, uint64_t *write_addr, size_t size)
{
    while (size > 0) {
        *write_addr++ = hz_parser_read_u64(p);
        --size;
    }
}

void hz_cmdread(hz_parser_t *p, int c_struct_align, void *dataptr, const char *cmd, ...)
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

#define HZ_HT_EMPTY HZ_FLAG(0)
#define HZ_HT_OCCUPIED HZ_FLAG(1)
#define HZ_HT_TOMBSTONE HZ_FLAG(2)

#define HZ_HT_INVALID_INDEX (UINT32_MAX)

typedef struct {
    uint32_t index; // if index is HZ_HT_INVALID_INDEX, iterator is invalid
    uint32_t key;
    uint32_t *ptr_value;
} hz_ht_iter_t;

HZ_ALWAYS_INLINE hz_bool hz_ht_iter_valid(hz_ht_iter_t *it) {
    return it->index != HZ_HT_INVALID_INDEX;
}

typedef struct {
    uint8_t *flags;
    uint32_t *keys;
    uint32_t *values;
    size_t size;
    size_t num_occupied;
    hz_allocator_t *alctr;
} hz_ht_t;

HZ_STATIC void hz_ht_clear(hz_ht_t *ht) {
    HZ_MEMSET(ht->flags, HZ_HT_EMPTY, ht->size);
    HZ_MEMSET(ht->keys, 0, ht->size*sizeof(uint32_t));
    HZ_MEMSET(ht->values, 0, ht->size*sizeof(uint32_t));
}

HZ_STATIC void hz_ht_init(hz_ht_t *ht, hz_allocator_t *alctr, size_t size){
    ht->alctr = alctr;
    ht->keys = hz_allocate(alctr, size*sizeof(uint32_t));
    ht->values = hz_allocate(alctr,size*sizeof(uint32_t));
    ht->flags = hz_allocate(alctr,size*sizeof(uint8_t));
    ht->num_occupied = 0;
    ht->size = size;
    hz_ht_clear(ht);
}

HZ_STATIC void hz_ht_destroy(hz_ht_t *ht)
{
    hz_deallocate(ht->alctr,ht->flags);
    hz_deallocate(ht->alctr,ht->keys);
    hz_deallocate(ht->alctr,ht->values);
}

HZ_STATIC uint32_t hz_ht_next_valid_index(hz_ht_t *ht, uint32_t index) {
    if (index >= ht->size) return HZ_HT_INVALID_INDEX;

    while ((ht->flags[index] & (HZ_HT_EMPTY|HZ_HT_TOMBSTONE)) && index < ht->size)
        ++index;

    if (index != ht->size)
        return index;

    return HZ_HT_INVALID_INDEX;
}

HZ_STATIC hz_bool hz_ht_iter_next(hz_ht_t *ht, hz_ht_iter_t *it) {
    it->index = hz_ht_next_valid_index(ht,it->index+1);
    if (it->index != HZ_HT_INVALID_INDEX) {
        it->key = ht->keys[it->index];
        it->ptr_value = &ht->values[it->index];
        return HZ_TRUE;//success
    }

    return HZ_FALSE;
}

HZ_STATIC hz_ht_iter_t hz_ht_iter_begin(hz_ht_t *ht) {
    hz_ht_iter_t it;
    it.index = hz_ht_next_valid_index(ht,0);
    if (it.index != HZ_HT_INVALID_INDEX) {
        it.key = ht->keys[it.index];
        it.ptr_value = &ht->values[it.index];
    }

    return it;
}

HZ_STATIC hz_bool hz_ht_search(hz_ht_t *ht, uint32_t key, hz_ht_iter_t *it) {
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
HZ_STATIC hz_bool hz_ht_insert(hz_ht_t *ht, uint32_t key, uint32_t value)
{
    uint32_t h = hz_hash32_fnv1a(key) % ht->size;

    for (uint32_t i = 0; i < ht->size; ++i, h = (h + i) % ht->size) {
        if ((ht->flags[h] & HZ_HT_OCCUPIED) && ht->keys[h] == key) { // entry aready exists, replace with our value
            ht->values[h] = value;
            return HZ_TRUE;
        }

        if (ht->flags[h] & (HZ_HT_EMPTY | HZ_HT_TOMBSTONE)) { // found empty/tombstone slot, insert value and return
            ht->keys[h] = key;
            ht->values[h] = value;
            ht->flags[h] = HZ_HT_OCCUPIED;
            return HZ_TRUE;
        }
    }
    
    // insertion failed
    return HZ_FALSE;
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

#if 0
HZ_STATIC hz_mph_map_t *hz_mph_map_create(hz_ht_t *ht)
{
    uint32_t size = hz_map_size(ht);

    hz_mph_map_t *map = hz_malloc(sizeof(*map));
    map->N = size;
    map->buckets = hz_malloc(sizeof(*map->buckets) * size);
    map->G = hz_malloc(sizeof(*map->G) * size);
    map->values = hz_malloc(sizeof(*map->values) * size);
    hz_bool *value_set = hz_malloc(sizeof(*value_set) * size);

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
#endif 

typedef struct {
    hz_bool was_setup;
    hz_ht_t arabic_joining;
} hz_lib_t;

static hz_lib_t s_hz_lib;

void hz_build_luts(void)
{
    hz_ht_init(&s_hz_lib.arabic_joining, &s_hz_alctr, HZ_ARRLEN(hz_arabic_joining_list));

    for (int i = 0; i < HZ_ARRLEN(hz_arabic_joining_list); ++i) {
        hz_ht_insert(&s_hz_lib.arabic_joining, hz_arabic_joining_list[i].codepoint, hz_arabic_joining_list[i].joining);
    }
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

HZ_STATIC hz_bool hz_shape_complex_arabic_char_joining(hz_unicode_t codepoint, uint16_t *joining)
{
    hz_ht_t *ht = &s_hz_lib.arabic_joining;
    hz_ht_iter_t it;

    if (hz_ht_search(ht,codepoint,&it)) {
        *joining = *it.ptr_value;
        return HZ_TRUE; // found value
    }

    return HZ_FALSE;
}



typedef struct {
    uint16_t *keys;
    uint16_t *g_middle;
    uint16_t *values; 
} hz_ht_coverage_t;

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
        cov->values = hz_memory_arena_alloc(memory_arena, cov->count * sizeof(uint16_t));
        hz_parser_read_u16_block(p, cov->values, cov->count);
        break;
        case 2:
        cov->count = hz_parser_read_u16(p);
        cov->ranges = hz_memory_arena_alloc(memory_arena, cov->count * sizeof(hz_coverage_range_t));
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
        default:
        return -1;
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
            class_def->values = hz_memory_arena_alloc(memory_arena, class_def->count * sizeof(uint16_t));
            hz_parser_read_u16_block(p, class_def->values, class_def->count);
            break;
        }

        case 2: {
            class_def->count = hz_parser_read_u16(p);
            class_def->ranges = hz_memory_arena_alloc(memory_arena, class_def->count * sizeof(hz_coverage_range_t));
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
    if (NULL != face->metrics) free(face->metrics);
    hz_memory_arena_release(&face->memory_arena);
    hz_free(face->arenamem);
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
                    Offset32 *mark_glyph_set_offsets = hz_memory_arena_alloc(&tmp_arena, mark_glyph_set_count * sizeof(Offset32));
                    hz_parser_read_u32_block(&p, mark_glyph_set_offsets, mark_glyph_set_count);

                    face->mark_glyph_set = hz_memory_arena_alloc(&face->memory_arena, mark_glyph_set_count * sizeof(hz_coverage_t));

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
        hz_parser_deinit(&p);
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
        hz_parser_deinit(&p);
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
    
    hz_parser_deinit(&p);
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
    hz_face_destroy(font->face);
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
        hz_parser_deinit(&p);
    }

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
hz_parse_chained_sequence_rule(hz_memory_arena_t *memory_arena, hz_parser_t *p, hz_chained_sequence_rule_t *rule) {
    rule->prefix_count = hz_parser_read_u16(p);
    if (rule->prefix_count) {
        rule->prefix_sequence = hz_memory_arena_alloc(memory_arena, sizeof(uint16_t) * rule->prefix_count);
        hz_parser_read_u16_block(p, rule->prefix_sequence, rule->prefix_count);
    }

    rule->input_count = hz_parser_read_u16(p);
    if (rule->input_count) {
        rule->input_sequence = hz_memory_arena_alloc(memory_arena, sizeof(uint16_t) * (rule->input_count - 1));
        hz_parser_read_u16_block(p, rule->input_sequence, rule->input_count - 1);
    }

    rule->suffix_count = hz_parser_read_u16(p);
    if (rule->suffix_count) {
        rule->suffix_sequence = hz_memory_arena_alloc(memory_arena, sizeof(uint16_t) * rule->suffix_count);
        hz_parser_read_u16_block(p, rule->suffix_sequence, rule->suffix_count);
    }

    rule->lookup_count = hz_parser_read_u16(p);
    rule->lookup_records = hz_memory_arena_alloc(memory_arena, sizeof(hz_sequence_lookup_record_t) * rule->lookup_count);
    hz_parser_read_u16_block(p, (uint16_t *)rule->lookup_records, rule->lookup_count*2);
}

void
hz_parse_chained_sequence_rule_set(hz_memory_arena_t *memory_arena, hz_parser_t *p, hz_chained_sequence_rule_set_t *rule_set) {
    rule_set->count = hz_parser_read_u16(p);
    Offset16* offsets = hz_malloc(sizeof(Offset16) * rule_set->count);
    hz_parser_read_u16_block(p,offsets,rule_set->count);

    rule_set->rules = hz_memory_arena_alloc(memory_arena, sizeof(hz_chained_sequence_rule_t) * rule_set->count);

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
    hz_memory_arena_t tmp_arena = hz_memory_arena_create((uint8_t*)tmp_buffer, sizeof tmp_buffer);

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

    for (i = 0; i < HZ_ARRLEN(complex_script_list); ++i) {
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
                id = (HZ_HOST_ORDER == HZ_LITTLE_ENDIAN) ? hz_bswap16(offset) : offset;
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

hz_error_t hz_setup (hz_setup_flags_t flags)
{
    if (!s_hz_lib.was_setup) {
        hz_build_luts();

        s_hz_lib.was_setup = HZ_TRUE;
    }

    return HZ_OK;
}

void hz_cleanup(void)
{
}

#if HZ_COMPILER & HZ_COMPILER_GCC
#define HZ_NAKEDFN __attribute__((naked))
#elif HZ_COMPILER & HZ_COMPILER_VC
#define HZ_NAKEDFN __declspec(naked)
#else
#define HZ_NAKEDFN
#endif

// This routine is optimized with avx2 on inline assembly Intel syntax.
// When defining HZ_ENABLE_SIMD, Hamza should be compiled with the "-masm=att" flag as
// the library uses Intel syntax rather than AT&T.
// NOTE: This works with GCC itself and ICC, but for clang only clang 14 and later.
// gids and codepoints memory must be aligned on a 32-byte boundary.
void 
hz_apply_cmap_format4_avx2_aligned(const hz_cmap_format4_subtable_t *restrict subtable,
                                   register uint32_t *restrict gids,
                                   register const uint32_t *restrict u32string,
                                   register size_t size)
{
//    register size_t v __asm__("%r8") = 0;
//
//    #pragma GCC ivdep
//    while (v + 8 < size) {
//        // load 8 unicode chars into ymm1
//        asm volatile ("vmovntdqa (%0), %%ymm1\n\t" :: "r" (u32string + v));
//
//        // set low and high variables for binary search algo
//        // lo: ymm2 hi: ymm3
//        asm ("vpxor %ymm2, %ymm2, %ymm2\n\t");
//        asm ("vpbroadcastd (%0),%%ymm3\n\t" :: "r" (size));
//
//        // compute mid, shift right by 1
//        asm ("vpaddd %ymm3, %ymm5, %ymm2\n\t");
//        asm ("vpsrad 1, %ymm5, %ymm4\n\t");
//
//        v += 8;
//    }
}

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

    hz_parser_deinit(&p);
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

hz_bool
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

void hz_shaper_init(hz_shaper_t *shaper) {
    hz_memory_arena_init(&shaper->memory_arena, (uint8_t*)shaper->ar, sizeof shaper->ar);

    shaper->features = NULL;
    shaper->num_features = 0;

    shaper->direction = HZ_DIRECTION_LTR;
    shaper->script = HZ_SCRIPT_LATIN;
    shaper->language = HZ_LANGUAGE_ENGLISH;

    shaper->flags = 0;
}

void hz_shaper_set_features(hz_shaper_t *shaper, const hz_feature_t *features, size_t num_features)
{
    hz_memory_arena_reset(&shaper->memory_arena);
    shaper->features = hz_memory_arena_alloc(&shaper->memory_arena, sizeof(hz_feature_t) * num_features);
    shaper->num_features = num_features;
    memcpy(shaper->features, features, num_features * sizeof(hz_feature_t));
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
    hz_stack_allocator_t tmp_stack = hz_stack_create((uint8_t*)tmp_buffer,sizeof tmp_buffer);

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
#ifdef HZ_DEBUG_LOGGING
  fprintf(stderr, "[ERROR in %s:%d]: %s\n", filename, linenum, msg);
#endif
  //exit(EXIT_FAILURE);
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
    hz_memory_arena_t tmp_arena = hz_memory_arena_create((uint8_t*)tmp_buffer, sizeof tmp_buffer);
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
    hz_memory_arena_t tmp_arena = hz_memory_arena_create((uint8_t*)tmp_buffer, sizeof tmp_buffer);

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
    hz_memory_arena_t tmp_arena = hz_memory_arena_create((uint8_t*)tmp_buffer, sizeof tmp_buffer);

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
    hz_memory_arena_t arena_tmp = hz_memory_arena_create((uint8_t*)tmp, sizeof tmp);
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
    hz_memory_arena_t tmp_arena = hz_memory_arena_create((uint8_t*)tmp, sizeof tmp);
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

void hz_font_data_init(hz_font_data_t *font_data, size_t arena_size) {
    font_data->memory_arena_data = hz_malloc(arena_size);
    hz_memory_arena_init(&font_data->memory_arena,
                         font_data->memory_arena_data,
                         arena_size);
    
    font_data->allocator.allocfn = hz_memory_arena_alloc_fn;
    font_data->allocator.user = &font_data->memory_arena;
}

void hz_font_data_release(hz_font_data_t *font_data){
    hz_free(font_data->memory_arena_data);
}

void hz_font_data_load(hz_font_data_t *font_data, hz_font_t *font) {
    hz_parser_t p;
    hz_parser_init(&p, font->face->data);
    hz_memory_arena_reset(&font_data->memory_arena); /* reset arena before parsing new font */
    font_data->face = font->face;

    hz_load_gsub_table(&p, font_data);
    hz_load_gpos_table(&p, font_data);
    
    hz_parser_deinit(&p);
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
    hz_memory_arena_t arena = hz_memory_arena_create((uint8_t*)tmp, sizeof tmp);

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
    hz_memory_arena_t arena = hz_memory_arena_create((uint8_t*)tmp, sizeof tmp);

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
    for (i = 0; i < HZ_ARRLEN(language_map_list); ++i) {
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
    for (i = 0; i < HZ_ARRLEN(language_map_list); ++i) {
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
    if (state->codepoint == 0) {
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

void hz_buffer_load_utf8_aligned(hz_buffer_t *buffer, hz_memory_arena_t *memory_arena, const uint8_t *aligned_input, size_t size)
{
    buffer->codepoints = hz_memory_arena_alloc_aligned(memory_arena, size*4, 4);
    uint32_t *dest = (uint32_t *)buffer->codepoints;

#if HZ_ARCH & HZ_ARCH_AVX2_BIT
    // UTF-8 to UTF-32 conversion
    for (size_t rp = 0; rp+32 <= size; rp+=32) {
        // Load 32-bytes into register
        __m256i chunk = _mm256_load_si256((const __m256i *)&aligned_input[rp]);

        // When none of the bytes has the MSB set, all characters of the chunk are ASCII
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
            // Non-ascii encountered, handle differently

        }
    }
#endif
}

void hz_buffer_load_utf8_sz(hz_buffer_t *buffer, const unsigned char *sz_input) {
    hz_utf8_decoder_t state;
    hz_utf8_decoder_init(&state);
    hz_utf8_decoder_set_flags(&state, UTF_END_AT_NULL);
    hz_utf8_decoder_set_data(&state, sz_input, 0);

    hz_unicode_t cp = hz_decode_next_utf8_codepoint(&state);
    while (cp != 0) {
        hz_vector_push_back(buffer->codepoints, cp);
        cp = hz_decode_next_utf8_codepoint(&state);
    }
}

void hz_buffer_load_ucs2_sz(hz_buffer_t *buffer, const hz_ucs2_char_t *sz_input) {
    while (*sz_input != '\0') {
        hz_vector_push_back(buffer->codepoints, (hz_unicode_t)(*sz_input));
        ++sz_input;
    }
}

void hz_shape_sz1(hz_shaper_t* shaper, hz_font_data_t* font_data, hz_encoding_t encoding, const void* sz_input, hz_buffer_t *out_buffer)
{
    HZ_ASSERT(sz_input != NULL);
    HZ_ASSERT(out_buffer != NULL);
    HZ_ASSERT(out_buffer != NULL);
    hz_face_t *face = font_data->face;

    switch (encoding) {
        default: case HZ_ENCODING_ASCII: hz_buffer_load_ascii_sz(out_buffer, (const char*)sz_input); break;
        case HZ_ENCODING_LATIN1: break;
        case HZ_ENCODING_UCS2: hz_buffer_load_ucs2_sz(out_buffer, (const hz_ucs2_char_t*)sz_input); break;
        case HZ_ENCODING_UTF8: hz_buffer_load_utf8_sz(out_buffer,
            (const unsigned char*)sz_input); break;
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
    hz_zero(out_buffer->component_indices, out_buffer->glyph_count * sizeof(uint16_t));

    hz_buffer_compute_info(out_buffer, face);

    hz_shape_buffer(shaper, font_data, out_buffer);
}



#ifndef HZ_STRIP_GFX_CODE
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

typedef struct {
    hz_float max_sdf_distance;
    hz_float padd;
    int width, height;
    int x_cells, y_cells;
    int ss_level; // supersampling
    int type; // 0 for sdf, 1 for msdf
} hz_sdf_cache_opts_t;

typedef struct {
    int x, y, w, h;
} hz_rect_t;

HZ_DECL hz_rect_t hz_glyph_cache_compute_cell_rect(hz_sdf_cache_opts_t *opts, int cell);

typedef enum {
    HZ_VERTEX_TYPE_MOVETO = 1,
    HZ_VERTEX_TYPE_LINE,
    HZ_VERTEX_TYPE_QUADRATIC_BEZIER,
    HZ_VERTEX_TYPE_CUBIC_BEZIER
} hz_vertex_type_t;

typedef struct {
    float x, y;
} hz_vec2;

typedef struct {
    float x, y, z;
} hz_vec3;

// curve vertex could be of a line, quadratic bezier curve or cubic bezier curve.
// c1,c2 are the optional control points. 
typedef struct {
    hz_vec2 v1,v2,c1,c2;
    int type; // hz_vertex_type_t
    // int pad0[3];
} hz_bezier_vertex_t;

typedef struct {
    hz_vec2 pos;
    hz_vec2 uv;
} hz_stencil_vertex_t;

typedef struct {
    uint16_t first_curve, curve_count;
    hz_vec2 origin;
} hz_contour_t;

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

typedef uint32_t hz_msi_key_t;

typedef struct {
    hz_msi_key_t *ht;
    int32_t len;
    int exp;
} hz_msi_ht_t;

#define HZ_MSI_NOT_FOUND -1
#define HZ_MSI_EMPTY_KEY 0x10000000ul

void hz_msi_ht_init(hz_memory_arena_t *memory_arena, hz_msi_ht_t *msi, size_t size) {
    HZ_ASSERT(hz_is_power_of_two(size));
    msi->ht = hz_memory_arena_alloc(memory_arena, size*sizeof(hz_msi_key_t));
    msi->len = 0;
    msi->exp = hz_qlog2_i64(size);
}

int32_t hz_msi_ht_intern(hz_msi_ht_t *msi, uint32_t key) {
    uint32_t h = hz_hash32_fnv1a(key);

    for (uint32_t i = h;;) {
        // iterate
        uint32_t mask = ((uint32_t)1 << msi->exp) - 1;
        uint32_t step = (h >> (64 - msi->exp)) | 1;
        i = (i + step) & mask;

        if (msi->ht[i] == HZ_MSI_EMPTY_KEY) {
             if ((uint32_t)msi->len+1 == ((uint32_t)1 << msi->exp)) {
                return HZ_MSI_NOT_FOUND; // out of memory
            }

            msi->ht[i] = key; ++msi->len;
            return i; // return index of entry
        } else if (msi->ht[i] == key) { // return index of existing item
            return i;
        }
    }

    return HZ_MSI_NOT_FOUND;
}

// LRU cache code
struct hz_lru_node {
    struct hz_lru_node *prev, *next;
    uint32_t key; // font|glyph
    int16_t slot_index;
};

typedef struct hz_lru_node hz_lru_node_t;

typedef struct {
    // msi hash table for fast lookup into cache
    hz_msi_ht_t msi;
    // following doubly linked list nodes are stored contiguously for better coherence
    hz_lru_node_t **t; // msi accelerated table
    // begin and end nodes for quick insertion
    hz_lru_node_t *q1, *q2;
    // size and greater POT size
    int size;
    int pow2_size;
    float max_insert_ratio;
} hz_lru_cache_t;

HZ_ALWAYS_INLINE hz_lru_node_t *hz_lru_node_alloc(hz_memory_arena_t *memory_arena) {
    hz_lru_node_t *n = hz_memory_arena_alloc(memory_arena, sizeof(hz_lru_node_t));
    n->prev = NULL;
    n->next = NULL;
    return n;
}

HZ_ALWAYS_INLINE void hz_lru_nodes_link(hz_lru_node_t *restrict q1, hz_lru_node_t *restrict q2) {
    HZ_ASSERT(q1 != q2);
    q1->next = q2; q2->prev = q1;
}

HZ_ALWAYS_INLINE void hz_lru_cache_init(hz_memory_arena_t *memory_arena, hz_lru_cache_t *lru, uint32_t size, float max_insert_ratio){
    HZ_ASSERT(lru != NULL);
    HZ_ASSERT(memory_arena != NULL);
    HZ_ASSERT(size);

    lru->size = size;
    if (!hz_is_power_of_two(size)) // ensure size is a power of two, this is required for the msi ht
        size = hz_calc_next_pow2_size(size);

    lru->pow2_size = size;
    lru->t = hz_memory_arena_alloc(memory_arena, sizeof(lru->t[0])*size);
    HZ_MEMSET(lru->t, 0, sizeof(lru->t[0])*size);

    // allocate begin and end nodes
    lru->q1 = hz_lru_node_alloc(memory_arena);
    lru->q2 = hz_lru_node_alloc(memory_arena);
    
    // link dummy nodes together
    hz_lru_nodes_link(lru->q1, lru->q2);
    hz_msi_ht_init(memory_arena, &lru->msi, size);

    lru->max_insert_ratio = max_insert_ratio;
}

#define HZ_MIN(X,Y) ((X)<(Y)?(X):(Y))

void hz_lru_cache_evict_and_replace(hz_lru_cache_t *lru, const uint32_t sorted_unique_ids[], size_t count){
    size_t n = HZ_MIN(lru->max_insert_ratio, count);

    for (int i = 0; i < n; ++i) {
        
    }
}

size_t hz_lru_cache_hit_keys(hz_lru_cache_t *cache,
                             hz_msi_key_t *in_keys,
                             int32_t *hit_indices,
                             int32_t *missed_indices,
                             size_t key_count) {
    int hits = 0;

    for (int i = 0; i < key_count; ++i) {
        int32_t hash_idx = hz_msi_ht_intern(&cache->msi, in_keys[i]);
        if (hash_idx != HZ_MSI_NOT_FOUND) {
            hit_indices[hits++] = in_keys[i];
        }
    }

    return hits;
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

typedef struct {
    union {
        struct {
            unsigned gid : 16;
            unsigned font_id : 16;
        };
        uint32_t id32;
    } unique_id;

    hz_vec3 pos;
    hz_vec3 rot;
    hz_vec2 scale;
    float sheer_factor;
    float weight;
} hz_glyph_instance_t;

typedef struct {
    hz_vector(hz_glyph_instance_t) draw_data;
    hz_ht_t unique_glyph_ht;
} hz_command_list_t;

#define HZ_MAX_UNIQUE_GLYPHS_PER_FRAME 1000

void hz_command_list_init(hz_command_list_t *cmd_list)
{
    cmd_list->draw_data = NULL;
    hz_ht_init(&cmd_list->unique_glyph_ht, hz_get_allocator(), HZ_MAX_UNIQUE_GLYPHS_PER_FRAME);
}

void hz_command_list_clear(hz_command_list_t *cmd_list)
{
    hz_vector_clear(cmd_list->draw_data);
    hz_ht_clear(&cmd_list->unique_glyph_ht);
}

typedef struct {
    int x;
} hz_lru_cache_ref_t;

typedef struct {
    hz_lru_cache_ref_t *unique_gid_refs;
} hz_glyph_batch_t;

#define HZ_CONTEXT_DFLT_MEMORY_SIZE (1024*1024) /*1MiB*/
#ifndef HZ_CONTEXT_MEMORY_SIZE
#   define HZ_CONTEXT_MEMORY_SIZE HZ_CONTEXT_DFLT_MEMORY_SIZE
#endif

#define HZ_CONTEXT_FONT_TABLE_SIZE 64

typedef struct {
    hz_command_list_t frame_cmds;
    hz_font_data_t font_table[HZ_CONTEXT_FONT_TABLE_SIZE];
    uint16_t font_id_counter;
    hz_lru_cache_t lru;
    hz_memory_arena_t memory_arena;
    uint8_t *arena_buffer;
} hz_context_t;

void hz_context_init (hz_context_t *ctx, hz_sdf_cache_opts_t *opts) {
    hz_command_list_init(&ctx->frame_cmds);

    ctx->arena_buffer = (uint8_t *)hz_malloc(HZ_CONTEXT_MEMORY_SIZE);
    hz_memory_arena_init(&ctx->memory_arena, ctx->arena_buffer, HZ_CONTEXT_MEMORY_SIZE);
    hz_lru_cache_init(&ctx->memory_arena, &ctx->lru, opts->x_cells * opts->y_cells, 0.5f);
    ctx->font_id_counter = 0;
}

uint16_t hz_context_stash_font(hz_context_t *ctx, const hz_font_data_t *font)
{
    uint16_t id = ctx->font_id_counter;
    ctx->font_table[id] = *font;
    ++ctx->font_id_counter;
    return id;
}

void hz_frame_begin(hz_context_t *ctx) {
    hz_command_list_clear(&ctx->frame_cmds);
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
        uint32_t key = g->unique_id.id32;

        if (hz_ht_search(&frame_cmds->unique_glyph_ht, key, &it))
            (*it.ptr_value)++;
        else
            hz_ht_insert(&frame_cmds->unique_glyph_ht, key, 1);
    }

    hz_ht_iter_t it = hz_ht_iter_begin(&frame_cmds->unique_glyph_ht);

    while (hz_ht_iter_valid(&it)) {
        hz_ht_iter_next(&frame_cmds->unique_glyph_ht, &it);
    }
}

typedef struct {
    char r,g,b,a;
} hz_color_t;

float hz_face_scale_for_pixel_h(hz_face_t *face, float height)
{
   return (float) height / (float) face->fheight;
}

void hz_draw_buffer(hz_context_t *ctx,
                    hz_buffer_t *buffer,
                    uint16_t font_id,
                    hz_vec2 pos,
                    float px_size)
{
    const hz_font_data_t *font_data = &ctx->font_table[font_id];
    hz_face_t *face = font_data->face;
    float text_width = 0.0f;
    float xpen=pos.x,ypen=pos.y;
    float v_scale = hz_face_scale_for_pixel_h(face,px_size);

    for (size_t i = 0; i < buffer->glyph_count; ++i) {
        hz_glyph_metrics_t metrics = buffer->glyph_metrics[i];
        hz_glyph_instance_t g;
        g.unique_id.gid = buffer->glyph_indices[i];
        g.unique_id.font_id = font_id;

        float gx = xpen + metrics.xOffset * v_scale;
        float gy = ypen + metrics.yOffset * v_scale;

        g.pos = (hz_vec3){gx,gy,0.f};
        g.scale = (hz_vec2){1.0f,1.0f};
        g.sheer_factor = 0.f;
        g.weight = 0.f;

        hz_vector_push_back(ctx->frame_cmds.draw_data,g);
        
        xpen += metrics.xAdvance * v_scale;
    }
}

hz_command_list_t *hz_get_frame_commands(hz_context_t *ctx) {
    return &ctx->frame_cmds;
}

typedef struct {
    hz_vector(hz_bezier_vertex_t) verts;
    hz_vector(hz_contour_t) contours;
} hz_shape_draw_data_t;

void hz_shape_draw_data_clear(hz_shape_draw_data_t *draw_data)
{
    if (draw_data->verts != NULL) {
        hz_vector_destroy(draw_data->verts);
    }
}

hz_rect_t hz_glyph_cache_compute_cell_rect(hz_sdf_cache_opts_t *opts, int cell)
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


#endif // !HZ_STRIP_GFX_CODE

#undef Version16Dot16
#undef Offset32
#undef Offset16
#endif // HZ_IMPLEMENTATION

#undef Fixed26Dot6

#ifdef __cplusplus
};
#endif

#endif // HZ_H
