/*
     This file is part of Hamza.

    Hamza is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
    Hamza is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
    You should have received a copy of the GNU Lesser General Public License along with Hamza. If not, see <https://www.gnu.org/licenses/>. 
*/

#ifndef _HZ_H_
#define _HZ_H_

#define HZ_MAKE_VERSION(major, minor, patch) (uint32_t)((major<<16)|(minor<<8)|(patch))

#define HZ_VERSION HZ_MAKE_VERSION(0,4,1)
#define HZ_STRINGIFY(S) HZ_STRINGIFY_(S)
#define HZ_STRINGIFY_(S) #S
#define HZ_UCD_HEADER_NAME(major,minor,patch) HZ_STRINGIFY(HZ_UCD_HEADER_NAME_(major,minor,patch))
#define HZ_UCD_HEADER_NAME_(major,minor,patch) hz_ucd_##major##_##minor##_##patch.h

#include "hz_config.h"
#ifdef HZ_INCLUDE_STBTT
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#endif /* HZ_INCLUDE_STBTT */

#if !defined(HZ_UCD_VERSIONS_LIST)
#warning "Must define required UCD versions in hz_config.h"
#endif

enum hz_ucd_version {
#define X(A, B, C) HZ_UCD_VERSION_ ## A ## _ ## B ## _ ## C = HZ_MAKE_VERSION(A,B,C),
HZ_UCD_VERSIONS_LIST(X)
#undef X
};

#define HZ_COMPILER_UNKNOWN 0ul
#define HZ_COMPILER_GCC 0x00000001ul 
#define HZ_COMPILER_CLANG 0x00000002ul
#define HZ_COMPILER_TURBOC 0x00000004ul
#define HZ_COMPILER_VC 0x00000008ul
#define HZ_COMPILER_INTEL 0x00000010ul
#define HZ_COMPILER_BORLAND 0x00000020ul
#define HZ_COMPILER_EMBARCADERO 0x00000040ul

#if defined(_MSC_VER) && !defined(__clang__)
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

#if !defined(HZ_NOSTDLIB)
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#endif

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

#if HZ_COMPILER & (HZ_COMPILER_VC | HZ_COMPILER_CLANG)
#define HZ_DBG_BREAK __debugbreak
#elif HZ_COMPILER & HZ_COMPILER_GCC
#define HZ_DBG_BREAK __builtin_trap
#else
#endif

#define HZ_ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))
#define HZ_UNARR(x) x, (sizeof(x)/sizeof((x)[0]))

#if defined(HZ_NOSTDLIB)
#define HZ_ASSERT(cond) if (!(cond)) { HZ_DBG_BREAK(); }
#else
#include <assert.h>
#define HZ_ASSERT(cond) assert((cond))
#endif

#define HZ_TAG(a, b, c, d) ((hz_tag_t)d | ((hz_tag_t)c << 8) | ((hz_tag_t)b << 16) | ((hz_tag_t)a << 24U))
#define HZ_STRTAG(str) ((hz_tag_t)(str[3]) | ((hz_tag_t)(str[2]) << 8) | ((hz_tag_t)(str[1]) << 16) | ((hz_tag_t)(str[0]) << 24U))
#define HZ_UNTAG(tag) (char) ((tag >> 24) & 0xFF), (char)((tag >> 16) & 0xFF), (char)((tag >> 8) & 0xFF), (char)(tag & 0xFF)
#define HZ_TAG_NONE ((hz_tag_t)0)

uint64_t HZ_ALWAYS_INLINE hz__clzll(uint64_t x) {
    uint64_t r = 0;
    while(!(x&((uint64_t)1<<(uint64_t)63))){x<<=1; ++r;}
    return r;
}

#define HZ_ISP2(N) (~((uint64_t)(N)&(uint64_t)((N)-1)))
#define HZ_NXP2(N) ((uint64_t)1<<63)>>(HZ_CLZ(N)-1)
#define HZ_CLZ(N) hz__clzll(N)
#define HZ_MIN(X,Y) ((X)<(Y)?(X):(Y))
#define HZ_MAX(X,Y) ((X)>(Y)?(X):(Y))

typedef uint32_t hz_version_t;

typedef enum {
    HZ_LOG_SEVERITY_ERROR,
    HZ_LOG_SEVERITY_WARNING,
    HZ_LOG_SEVERITY_DEBUG,
    HZ_LOG_SEVERITY_INFO,
    HZ_LOG_SEVERITY_VERBOSE,
} hz_log_severity_t;

typedef struct {
    hz_log_severity_t log_severity;
    enum hz_ucd_version ucd_version;
} hz_config_t;

typedef enum {
    HZ_NFD, // Normalization Forms D (Canonical Decomposition)
    HZ_NFC, // Normalization Forms C (Canonical Decomposition, followed by Canonical Composition)
    HZ_NFKD, // Normalization Forms KD (Compatibility Decomposition)
    HZ_NFKC, // Normalization Forms KC (Compatibility Decomposition, followed by Canonical Composition)
} hz_normalization_form_t;

#define HZ_FLAG(n) (1ul << (n))
#define Fixed26Dot6 uint32_t

#define HZ_IGNORE_ARG(x) (void)(x)

#define HZ_DEFAULT_FONT_DATA_ARENA_SIZE (1024*1024)/*1MiB*/

#ifdef __cplusplus
extern "C" {
#endif

typedef int8_t hz_int8;
typedef uint8_t hz_byte, hz_uint8, hz_uchar;
typedef uint16_t hz_uint16;
typedef int16_t hz_int16;
typedef uint32_t hz_uint32;
typedef int32_t hz_int32;
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
hz_memcpy((__ARR) + hz_vector_header(__ARR)->size, __PTR, (__LEN) * sizeof((__ARR)[0]));\
hz_vector_header(__ARR)->size += (__LEN);\
} while(0)

#define hz_vector_pop(__ARR) hz_vector_resize(__ARR, hz_vector_size(__ARR)-1)
#define hz_vector_top(__ARR) (&((__ARR)[hz_vector_size(__ARR)-1]))


/////////////////// hz_memory_arena_t ///////////////////

typedef struct {
    uint8_t *mem;
    size_t size;
    uintptr_t pos;
} hz_memory_arena_t;

HZ_STATIC HZ_INLINE hz_memory_arena_t hz_memory_arena_create(uint8_t *mem, size_t sz)
{
    return (hz_memory_arena_t){.mem = mem, .size = sz, .pos = 0};
}

HZ_STATIC HZ_INLINE void hz_memory_arena_init(hz_memory_arena_t *a, uint8_t *p, size_t sz)
{
    *a = (hz_memory_arena_t){.mem = p, .size = sz, .pos = 0 };
}

// align must be a power of two
HZ_STATIC HZ_INLINE uintptr_t hz_align_forward(uintptr_t addr, uintptr_t align)
{
    return (~addr+1) & (align-1);
}

HZ_STATIC HZ_INLINE void *hz_memory_arena_alloc_aligned(hz_memory_arena_t *arena, size_t size, size_t align)
{
    HZ_ASSERT(align > 0);
    HZ_ASSERT(HZ_ISP2(align));
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


#define DEFAULT_ALIGNMENT (2*sizeof(void*))

HZ_STATIC HZ_INLINE void *hz_memory_arena_alloc(hz_memory_arena_t *arena, size_t size) {
    return hz_memory_arena_alloc_aligned(arena, size, DEFAULT_ALIGNMENT);
}

HZ_STATIC HZ_INLINE void hz_memory_arena_release(hz_memory_arena_t *arena) {
    HZ_IGNORE_ARG(arena);
    // NoOp
}

HZ_STATIC HZ_INLINE void hz_memory_arena_reset(hz_memory_arena_t *arena) {
    arena->pos = 0;
}

typedef struct { int x0,y0,x1,y1; } hz_bbox_t;
typedef struct { int x,y,w,h; } hz_rect_t; 

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
    HZ_ERROR_SETUP_FAILED                   = HZ_FLAG(8),
    HZ_ERROR_ALREADY_INITIALIZED            = HZ_FLAG(9),
    HZ_ERROR_BROTLI_STREAM_REJECTED         = HZ_FLAG(10),
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

HZ_DECL hz_error_t hz_init(const hz_config_t *cfg);

HZ_DECL void hz_deinit(void);

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

/* function: hz_font_get_glyph_shape */
HZ_DECL void hz_font_get_glyph_shape(void);

HZ_DECL float hz_face_scale_for_pixel_h(hz_face_t *face, float height);

HZ_DECL void hz_face_get_scaled_glyph(hz_face_t *face, hz_uint16 glyph_id, float y_scale, hz_bbox_t *b);

HZ_DECL
void hz_face_get_glyph_box(hz_face_t *face, uint16_t glyph_id, hz_bbox_t *b);

#ifdef HZ_INCLUDE_STBTT
/*  function: hz_stbtt_font_create
 *      Loads a font from a stbtt_fontinfo structure.
 */
HZ_DECL hz_font_t* hz_stbtt_font_create(stbtt_fontinfo *info);
#endif /* HZ_INCLUDE_STBTT */

// enum: hz_base85_encoding 
enum hz_base85_encoding {
    HZ_BASE85_ENCODING_ADOBE, HZ_BASE85_ENCODING_Z85, HZ_BASE85_ENCODING_IPV6,
};

// function: hz_font_load_woff2_from_memory_base85
//   Creates a font from a Ascii85 string with user-requested encoding.
HZ_DECL hz_font_t *hz_font_load_woff2_from_memory_base85(enum hz_base85_encoding encoding, const char *base85_string, unsigned int input_size);

typedef struct hz_shaper_t hz_shaper_t;
typedef struct hz_font_data_t hz_font_data_t;

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

// set a custom generic internal allocator function.
HZ_DECL void hz_set_allocator_fn(hz_allocator_fn_t allocfn);
// set the user pointer for the internal allocator.
HZ_DECL void hz_set_allocator_user_pointer(void *user);

HZ_DECL hz_font_data_t *hz_font_data_create(hz_font_t *font);
HZ_DECL void hz_font_data_release(hz_font_data_t *fd);

HZ_DECL hz_shaper_t *hz_shaper_create(void);
HZ_DECL void hz_shaper_destroy(hz_shaper_t *shaper);
HZ_DECL void hz_shaper_set_features(hz_shaper_t *shaper, size_t sz,
                                    const hz_feature_t features[]);
HZ_DECL void hz_shaper_set_flags(hz_shaper_t *shaper, hz_shaper_flags_t flags);
HZ_DECL void hz_shaper_set_direction(hz_shaper_t *shaper, hz_direction_t direction);
HZ_DECL void hz_shaper_set_script(hz_shaper_t *shaper, hz_script_t script);
HZ_DECL void hz_shaper_set_language(hz_shaper_t *shaper, hz_language_t language);

HZ_DECL void hz_buffer_init(hz_buffer_t *buffer);
HZ_DECL void hz_buffer_release(hz_buffer_t *buffer);

typedef float hz_float, hz_f32;
typedef double hz_f64;

typedef struct {
    hz_float max_sdf_distance;
    hz_float padd;
    int width, height;
    int x_cells, y_cells;
    int ss_level; // supersampling
    int type; // 0 for sdf, 1 for msdf
} hz_glyph_cache_opts_t;


HZ_DECL hz_rect_t hz_glyph_cache_compute_cell_rect(hz_glyph_cache_opts_t *opts, int cell);

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

typedef struct {
    hz_vector(hz_bezier_vertex_t) verts;
    hz_vector(hz_contour_t) contours;
} hz_shape_draw_data_t;

HZ_DECL void hz_shape_draw_data_clear(hz_shape_draw_data_t *draw_data);

HZ_DECL int hz_face_get_glyph_shape(hz_face_t *face, hz_shape_draw_data_t *draw_data, hz_vec2 translate, float y_scale, hz_index_t glyph_index );


///////////////////////// hz_ht_t ////////////////////////////
typedef struct hz_ht_t hz_ht_t;

#define HZ_HT_EMPTY HZ_FLAG(0)
#define HZ_HT_OCCUPIED HZ_FLAG(1)
#define HZ_HT_TOMBSTONE HZ_FLAG(2)

#define HZ_HT_INVALID_INDEX (UINT32_MAX) 

typedef struct hz_ht_t hz_ht_t;

typedef struct {
    uint32_t index; // if index is HZ_HT_INVALID_INDEX, iterator is invalid
    uint32_t key;
    uint32_t *ptr_value;
} hz_ht_iter_t;

#define hz_ht_iter_valid(_IT) ((_IT)->index != HZ_HT_INVALID_INDEX)

HZ_DECL void hz_ht_clear(hz_ht_t *ht);
HZ_DECL hz_ht_t* hz_ht_create(hz_allocator_t *alctr, size_t size);
HZ_DECL void hz_ht_destroy(hz_ht_t *ht);
HZ_DECL uint32_t hz_ht_next_valid_index(hz_ht_t *ht, uint32_t index);
HZ_DECL hz_bool hz_ht_iter_next(hz_ht_t *ht, hz_ht_iter_t *it);
HZ_DECL hz_ht_iter_t hz_ht_iter_begin(hz_ht_t *ht);
HZ_DECL hz_bool hz_ht_search(hz_ht_t *ht, uint32_t key, hz_ht_iter_t *it);
HZ_DECL hz_bool hz_ht_insert(hz_ht_t *ht, uint32_t key, uint32_t value);
HZ_DECL hz_bool hz_ht_remove(hz_ht_t *ht, uint32_t key);
HZ_DECL size_t hz_ht_size(hz_ht_t* ht);

// LRU cache slot
typedef union { uint32_t u32; struct {
    uint16_t font_id, glyph_id;
}; } hz_cache_id_t;

#define HZ_LRU_ID_INVALID (hz_cache_id_t){.u32=UINT32_MAX}

struct hz_cache_slot_t {
    hz_cache_id_t id;
    float u0,v0,u1,v1;
};

struct hz_cache_node_t {
    struct hz_cache_node_t *prev, *next;
    uint16_t slot;
};

typedef struct {
    // msi hash table for fast lookup into cache
    //hz_msi_ht_t msi;
    // following doubly linked list nodes are stored contiguously for better coherence
    struct hz_cache_slot_t *slots;
    struct hz_cache_node_t *fn, *ln;
    int slots_occupied;
    int sz; // size
    int p2sz; // power of two size
    int max_replace_sz; // insert max per batch
} hz_glyph_cache_t;

struct hz_cache_stat_t{uint16_t avail,unavail;};

HZ_DECL void hz_lru_cache_init(hz_memory_arena_t *ma, hz_glyph_cache_t *c, int sz, int max_replace_sz);
HZ_DECL void hz_lru_cache_replace_slots(hz_glyph_cache_t *lru, uint16_t slots_sz, uint16_t open_slots[]);
HZ_DECL struct hz_cache_node_t *hz_lru_cache_get_node(hz_glyph_cache_t *c, hz_cache_id_t id);
HZ_DECL struct hz_cache_stat_t hz_lru_cache_stat(hz_glyph_cache_t *c, hz_ht_t *ids_ht, hz_cache_id_t *avail_id_list, hz_cache_id_t *unavail_id_list);
HZ_DECL void hz_lru_cache_replace_slots(hz_glyph_cache_t *lru, uint16_t slots_sz, uint16_t open_slots[]);

HZ_DECL void hz_lru_write_slot(hz_glyph_cache_t *lru, int slot_index, struct hz_cache_slot_t slot);

#define HZ_ALIGNAS(N) _Alignas(N)

typedef struct { float x,y,z,w; } hz_vec4,hz_quat;

typedef union {
    struct { float e00,e01,e02,e03,
                   e10,e11,e12,e13,
                   e20,e21,e22,e23,
                   e30,e31,e32,e33; };
    float e[4][4];
    float f[16];
} hz_mat4;

HZ_ALWAYS_INLINE hz_mat4 hz_mat4_identity()
{
    return (hz_mat4){ .f = {
            1.0f,0.0f,0.0f,0.0f,
            0.0f,1.0f,0.0f,0.0f,
            0.0f,0.0f,1.0f,0.0f,
            0.0f,0.0f,0.0f,1.0f
        }};
}

HZ_ALWAYS_INLINE hz_mat4 hz_mat4_scale(float sx, float sy, float sz) {
    hz_mat4 m = hz_mat4_identity();
    m.e00 = sx;
    m.e11 = sy;
    m.e22 = sz;
    return m;
}

HZ_ALWAYS_INLINE hz_mat4 hz_mat4_translate(float tx, float ty, float tz) {
    hz_mat4 m = hz_mat4_identity();
    m.e30 = tx;
    m.e31 = ty;
    m.e32 = tz;
    return m;
}

HZ_ALWAYS_INLINE void hz_mat4_mult(const hz_mat4 *a, const hz_mat4 *b, hz_mat4 *r)
{
    hz_mat4 A=*a,B=*b;

    r->e00 = A.e00 * B.e00 + A.e10 * B.e01 + A.e20 * B.e02 + A.e30 * B.e03;
    r->e01 = A.e01 * B.e00 + A.e11 * B.e01 + A.e21 * B.e02 + A.e31 * B.e03;
    r->e02 = A.e02 * B.e00 + A.e12 * B.e01 + A.e22 * B.e02 + A.e32 * B.e03;
    r->e03 = A.e03 * B.e00 + A.e13 * B.e01 + A.e23 * B.e02 + A.e33 * B.e03;

    r->e10 = A.e00 * B.e10 + A.e10 * B.e11 + A.e20 * B.e12 + A.e30 * B.e13;
    r->e11 = A.e01 * B.e10 + A.e11 * B.e11 + A.e21 * B.e12 + A.e31 * B.e13;
    r->e12 = A.e02 * B.e10 + A.e12 * B.e11 + A.e22 * B.e12 + A.e32 * B.e13;
    r->e13 = A.e03 * B.e10 + A.e13 * B.e11 + A.e23 * B.e12 + A.e33 * B.e13;

    r->e20 = A.e00 * B.e20 + A.e10 * B.e21 + A.e20 * B.e22 + A.e30 * B.e23;
    r->e21 = A.e01 * B.e20 + A.e11 * B.e21 + A.e21 * B.e22 + A.e31 * B.e23;
    r->e22 = A.e02 * B.e20 + A.e12 * B.e21 + A.e22 * B.e22 + A.e32 * B.e23;
    r->e23 = A.e03 * B.e20 + A.e13 * B.e21 + A.e23 * B.e22 + A.e33 * B.e23;

    r->e30 = A.e00 * B.e30 + A.e10 * B.e31 + A.e20 * B.e32 + A.e30 * B.e33;
    r->e31 = A.e01 * B.e30 + A.e11 * B.e31 + A.e21 * B.e32 + A.e31 * B.e33;
    r->e32 = A.e02 * B.e30 + A.e12 * B.e31 + A.e22 * B.e32 + A.e32 * B.e33;
    r->e33 = A.e03 * B.e30 + A.e13 * B.e31 + A.e23 * B.e32 + A.e33 * B.e33;

}

HZ_ALWAYS_INLINE hz_vec4 hz_mat4_mult_vec4(hz_mat4 *m, hz_vec4 v) {
    return (hz_vec4) {
     .x = v.x * m->e00 + v.y * m->e01 + v.z * m->e02 + v.w * m->e03,
     .y = v.x * m->e10 + v.y * m->e11 + v.z * m->e12 + v.w * m->e13,
     .z = v.x * m->e20 + v.y * m->e21 + v.z * m->e22 + v.w * m->e23,
     .w = v.x * m->e30 + v.y * m->e31 + v.z * m->e32 + v.w * m->e33
    };
}

HZ_ALWAYS_INLINE hz_mat4 hz_mat4_ortho(float l, float r, float b, float t) {
    hz_mat4 m = hz_mat4_identity();
    m.e00 = 2.0f / (r-l);
    m.e11 = 2.0f / (t-b);
    m.e30 = -(r+l)/(r-l);
    m.e31 = -(t+b)/(t-b);
    return m;
}

typedef struct {
    hz_mat4 vp;
    hz_vec4 v4_ts;
    struct { // style
        uint32_t color_rgba;
        uint32_t outline_color_rgba;
        uint32_t glow_outline;
        uint32_t weight_sheer;
    } sv;
    hz_cache_id_t lru_id;
} hz_glyph_instance_t;

typedef struct {
    hz_vector(hz_glyph_instance_t) draw_data;
    hz_ht_t *unique_glyph_ht;
} hz_command_list_t;

typedef struct hz_context_t hz_context_t;

HZ_DECL hz_context_t *hz_context_create(hz_glyph_cache_opts_t *opts);
HZ_DECL void hz_context_release (hz_context_t *ctx);
HZ_DECL void hz_frame_begin(hz_context_t *ctx);
HZ_DECL void hz_frame_end(hz_context_t *ctx);
HZ_DECL uint16_t hz_context_stash_font(hz_context_t *ctx, const hz_font_data_t *font);
HZ_DECL hz_face_t *hz_context_get_face(hz_context_t *ctx, uint16_t font_id);
HZ_DECL hz_glyph_cache_t *hz_context_get_lru(hz_context_t *ctx);
HZ_DECL hz_memory_arena_t *hz_get_frame_arena(hz_context_t *ctx);
HZ_DECL hz_command_list_t *hz_command_list_get(hz_context_t *ctx);

typedef uint16_t hz_Fx16;

typedef struct {
    float shear;
    float weight;
    hz_vec4 col;
    hz_vec4 outline_col;
    float outline_w;
    float glow;
    float scale;
} hz_buffer_style_t;

HZ_DECL void hz_draw_buffer(hz_context_t *ctx,
                            hz_buffer_t *buffer,
                            uint16_t font_id,
                            hz_vec3 pos,
                            hz_buffer_style_t *style,
                            float px_size);

HZ_DECL void hz_camera_begin_ortho(hz_context_t *ctx, float l, float r, float b, float t);

HZ_DECL 
void hz_camera_set_zoom(hz_context_t *ctx, float zoomlvl);

#ifdef __cplusplus
};
#endif

#endif /*_HZ_H_*/
