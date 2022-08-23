#ifndef HZ_H
#define HZ_H

#define HZ_DISABLE 0
#define HZ_ENABLE 1

#define HZ_STATIC static

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
#pragma warning(disable:4068) /* disable #pragma related warnings */
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

/* NOTE: SSE4A is AMD-only */
#define HZ_ARCH_X86_BIT     0x00010000ul
#define HZ_ARCH_ARM_BIT     0x00020000ul
#define HZ_ARCH_POWERPC_BIT 0x00040000ul
#define HZ_ARCH_MIPS_BIT    0x00080000ul
#define HZ_ARCH_RISCV_BIT   0x00100000ul
#define HZ_ARCH_SSE_BIT     0x00000001ul
#define HZ_ARCH_SSE2_BIT    0x00000002ul
#define HZ_ARCH_SSE3_BIT    0x00000004ul
#define HZ_ARCH_SSSE3_BIT   0x00000008ul
#define HZ_ARCH_SSE41_BIT   0x00000010ul
#define HZ_ARCH_SSE42_BIT   0x00000020ul
#define HZ_ARCH_SSE4A_BIT   0x00000040ul
#define HZ_ARCH_AVX_BIT     0x00000080ul
#define HZ_ARCH_AVX2_BIT    0x00000100ul
#define HZ_ARCH_AES_BIT     0x00000200ul

#define HZ_ARCH_UNKNOWN 0
#define HZ_ARCH_X86     (HZ_ARCH_X86_BIT)
#define HZ_ARCH_SSE     (HZ_ARCH_X86 | HZ_ARCH_SSE_BIT)
#define HZ_ARCH_SSE2    (HZ_ARCH_SSE | HZ_ARCH_SSE2_BIT)
#define HZ_ARCH_SSE3    (HZ_ARCH_SSE2 | HZ_ARCH_SSE3_BIT)
#define HZ_ARCH_SSSE3   (HZ_ARCH_SSE3 | HZ_ARCH_SSSE3_BIT)
#define HZ_ARCH_SSE41   (HZ_ARCH_SSSE3 | HZ_ARCH_SSE41_BIT)
#define HZ_ARCH_SSE42   (HZ_ARCH_SSE41 | HZ_ARCH_SSE42_BIT)
#define HZ_ARCH_SSE4A   (HZ_ARCH_SSE42 | HZ_ARCH_SSE4A_BIT)
#define HZ_ARCH_AVX     (HZ_ARCH_SSE42 | HZ_ARCH_AVX_BIT)
#define HZ_ARCH_AVX2    (HZ_ARCH_SSE42 | HZ_ARCH_AVX2_BIT)
#define HZ_ARCH_AES     (HZ_ARCH_AES_BIT)

#if defined(__AVX2__)
#   define HZ_ARCH HZ_ARCH_AVX2
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
#elifd defined(__arm__) || defined(_M_ARM)
#   define HZ_ARCH (HZ_ARCH_ARM)
#elif defined(__mips__)
#   define HZ_ARCH (HZ_ARCH_MIPS)
#elif defined(__powerpc__) || defined(_M_PPC)
#   define HZ_ARCH (HZ_ARCH_PPC)
#else
#   define HZ_ARCH (HZ_ARCH_UNKNOWN)
#endif

#if HZ_COMPILER & HZ_COMPILER_VC
#   define HZ_FORCEINLINE __forceinline
#   define HZ_FASTCALL __fastcall
#   define HZ_DLLEXPORT __declspec(dllexport)
#elif HZ_COMPILER & HZ_COMPILER_GCC
#   define HZ_FORCEINLINE __attribute__((always_inline))
#   define HZ_FASTCALL __attribute__((fastcall))
#   define HZ_DLLEXPORT __attribute__((dllexport))
#else
#   define HZ_FORCEINLINE inline
#   define HZ_FASTCALL
#   define HZ_DLLEXPORT
#endif

#define HZ_INLINE inline

#if defined(HZ_BUILD_SHARED) && defined(_WIN32)
#define HZDEF HZ_DLLEXPORT
#else
#define HZDECL
#endif

#if HZ_COMPILER & HZ_COMPILER_VC
#define HZ_DBGBREAK() __debugbreak()
#elif HZ_COMPILER & HZ_COMPILER_GCC
#define HZ_DBGBREAK() __builtin_trap()
#else
#define HZ_DBGBREAK()
#endif

#define HZ_ARRLEN(x) (sizeof(x)/sizeof((x)[0]))
#define HZ_UNARR(x) x, (sizeof(x)/sizeof((x)[0]))
#define HZ_ASSERT(cond) assert(cond)
#define HZ_TAG(a, b, c, d) ((hz_tag_t)d | ((hz_tag_t)c << 8) | ((hz_tag_t)b << 16) | ((hz_tag_t)a << 24U))
#define HZ_UNTAG(tag) (char) ((tag >> 24) & 0xFF), (char)((tag >> 16) & 0xFF), (char)((tag >> 8) & 0xFF), (char)(tag & 0xFF)
#define HZ_TAG_NONE ((hz_tag_t)0)

#ifdef __cplusplus
extern "C" {
#endif

typedef int8_t hz_char8_t;
typedef uint8_t hz_byte_t;
typedef int32_t hz_coord_t;
typedef uint32_t hz_unicode_t;
typedef uint32_t hz_tag_t;
typedef uint32_t hz_bool_t;

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
hz_bool_t hz_vector_is_empty(void *v);
void hz_vector_clear_impl(void **v);
void hz_vector_destroy_impl(void **v);
void hz_vector_reserve_impl(void **v, size_t new_cap);
size_t hz_vector_size_impl(void *v);
void hz_vector_resize_impl(void **v, size_t new_size);
void hz_vector_grow(void **v, int extra);
hz_bool_t hz_vector_need_grow(void *v, size_t extra);
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
    HZ_DIRECTION_LTR = 0x00000001,
    HZ_DIRECTION_RTL = 0x00000002,
    HZ_DIRECTION_TTB = 0x00000004,
    HZ_DIRECTION_BTT = 0x00000008
} hz_direction_t;

#define HZ_DIRECTION_IS_HORIZONTAL(dir) (hz_bool_t)(dir & 0x3)
#define HZ_DIRECTION_IS_VERTICAL(dir) (hz_bool_t)(dir & 0xC)

/* enum: HzError */
typedef enum {
    HZ_OK = 0ul,
    HZ_ERROR_INVALID_TABLE_TAG              = 0x00000001ul,
    HZ_ERROR_INVALID_TABLE_VERSION          = 0x00000002ul,
    HZ_ERROR_INVALID_LOOKUP_TYPE            = 0x00000004ul,
    HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT = 0x00000008ul,
    HZ_ERROR_INVALID_PARAM                  = 0x00000010ul,
    HZ_ERROR_INVALID_FORMAT                 = 0x00000020ul,
    HZ_ERROR_TABLE_DOES_NOT_EXIST           = 0x00000040ul,
    HZ_ERROR_UNEXPECTED_VALUE               = 0x00000080ul,
    HZ_ERROR_SETUP_FAILED                   = 0x10000000ul
} hz_error_t;

/*  enum: hz_glyph_class_t
 *      HZ_GLYPH_CLASS_ZERO - Nothing.
 *      HZ_GLYPH_CLASS_BASE - Base glyph.
 *      HZ_GLYPH_CLASS_LIGATURE - Ligature glyph.
 *      HZ_GLYPH_CLASS_MARK - Mark glyph.
 *      HZ_GLYPH_CLASS_COMPONENT - Mark component.
 * */
typedef enum {
    HZ_GLYPH_CLASS_ZERO      = 0,
    HZ_GLYPH_CLASS_BASE      = 0x00000001ul,
    HZ_GLYPH_CLASS_LIGATURE  = 0x00000002ul,
    HZ_GLYPH_CLASS_MARK      = 0x00000004ul,
    HZ_GLYPH_CLASS_COMPONENT = 0x00000008ul
} hz_glyph_class_t;

/* include tables, types, and functions and other generated code */
#include "hz-data-tables.h"

/* function: hz_segment_t */
typedef struct hz_segment_t hz_segment_t;

/* function: hz_segment_create */
HZDECL hz_segment_t* hz_segment_create(void);

/* function: hz_segment_destroy */
HZDECL void
hz_segment_destroy(hz_segment_t *seg);

/* function: hz_segment_set_script */
HZDECL void hz_segment_set_script(hz_segment_t *seg, hz_script_t script);

/* function: hz_segment_set_language */
HZDECL void hz_segment_set_language(hz_segment_t *seg, hz_language_t language);

/* function: hz_segment_set_direction */
HZDECL void hz_segment_set_direction(hz_segment_t *seg, hz_direction_t direction);

/* function: hz_segment_load_utf8 */
HZDECL void hz_segment_load_utf8(hz_segment_t *seg, hz_byte_t *data);

/* function: hz_segment_load_latin1 */
HZDECL void hz_segment_load_latin1(hz_segment_t *seg, hz_byte_t *data);

// struct: hz_char32_t
typedef uint32_t hz_char32_t;

/* function: hz_segment_load_utf32 */
HZDECL void
hz_segment_load_utf32(hz_segment_t *seg, hz_char32_t *data);

// struct: hz_char16_t
typedef uint16_t hz_char16_t;

/* function: hz_segment_load_ucs2 */
HZDECL void
hz_segment_load_ucs2(hz_segment_t *seg, hz_char16_t *data);

/* function: hz_segment_load_johab */
HZDECL void
hz_segment_load_johab(hz_segment_t *seg, hz_byte_t *data);

/* struct: hz_glyph_metrics_t */
typedef struct  hz_glyph_metrics_t {
    hz_fixed26dot6_t xAdvance;
    hz_fixed26dot6_t yAdvance;
    hz_fixed26dot6_t xOffset;
    hz_fixed26dot6_t yOffset;
} hz_glyph_metrics_t;

/* struct: hz_glyph_attrib_flags_t */
typedef enum hz_glyph_attrib_flags_t {
    HZ_GLYPH_ATTRIB_METRICS_BIT = 0x00000001,
    HZ_GLYPH_ATTRIB_INDEX_BIT = 0x00000002,
    HZ_GLYPH_ATTRIB_CODEPOINT_BIT = 0x00000004,
    HZ_GLYPH_ATTRIB_GLYPH_CLASS_BIT = 0x00000008,
    HZ_GLYPH_ATTRIB_ATTACHMENT_CLASS_BIT = 0x00000010,
    HZ_GLYPH_ATTRIB_COMPONENT_INDEX_BIT = 0x00000020,
} hz_glyph_attrib_flags_t;

/* struct: hz_buffer_t */
typedef struct hz_buffer_t {
    size_t              glyph_count;
    hz_glyph_metrics_t *    glyph_metrics;
    hz_index_t *           glyph_indices;
    hz_unicode_t *         codepoints;
    uint16_t *          glyph_classes;
    uint16_t *          attachment_classes;
    uint16_t *          component_indices;
    hz_glyph_attrib_flags_t  attrib_flags;
} hz_buffer_t;

/*  function: hz_segment_get_buffer
 * */
HZDECL const hz_buffer_t* hz_segment_get_buffer(hz_segment_t *seg);


/* enum: hz_shape_flags_t */
typedef enum hz_shape_flags_t {
    HZ_SHAPE_FLAG_AUTO_LOAD_FEATURES = 0x00000001,
    HZ_SHAPE_FLAG_REMOVE_MARKS = 0x00000002,
    HZ_SHAPE_FLAG_REMOVE_BASES = 0x00000004
} hz_shape_flags_t;

/*  enum: hz_setup_flags_t
 *      Set of special flags passed in to <hz_setup> used to configure the library.
 */
typedef enum hz_setup_flags_t {
    HZ_USE_CPUID_FOR_SIMD_CHECKS = 1 << 0,
} hz_setup_flags_t;

/*  function: hz_setup
 *      Sets up the library's internal structures and lookup tables.
 *      If HZ_USE_CPUID_FOR_SIMD_CHECKS flag is set it will also initialize a special hardware specific info to allow for runtime
 *      SIMD specialization. Otherwise, it will use compile-time macros to determine which instruction set to optimize with.
 *
 *  Returns:
 *      Returns HZ_OK on success, otherwise HZ_ERROR_SETUP_FAILED.
 * */
HZDECL hz_error_t hz_setup(hz_setup_flags_t flags);

/*  function: hz_cleanup
 *      Cleans up all the data initialized by <hz_setup> as well as any shaper cache that might've been allocated.
 */
HZDECL void hz_cleanup(void);

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
HZDECL hz_language_t hz_lang(const char *tag);

/*  Function: hz_lang_iso639_1_2002
 *      Finds language based on old <ISO 639-1:2002: https://id.loc.gov/vocabulary/iso639-1.html> two-character language tags.
 *
 *  Parameters:
 *      tag - An <ISO 639-1:2002: https://id.loc.gov/vocabulary/iso639-1.html> language tag.
 *
 *  Returns:
 *      Returns <hz_language_t> enum value based on OpenType language tag.
 */
HZDECL hz_language_t hz_lang_iso639_1_2002(const char *tag);

/* function: hz_script */
HZDECL hz_script_t hz_script(const char *tag);

/* struct: hz_face_t */
typedef struct hz_face_t hz_face_t;

/* function: hz_face_create */
HZDECL hz_face_t* hz_face_create(void);

/* function: hz_face_destroy */
HZDECL void hz_face_destroy(hz_face_t *face);

/* function: hz_face_get_upem */
HZDECL uint16_t hz_face_get_upem(hz_face_t *face);

/* function: hz_face_set_upem */
HZDECL void hz_face_set_upem(hz_face_t *face, uint16_t upem);

/* function: hz_face_get_glyph_metrics */
HZDECL hz_metrics_t *
hz_face_get_glyph_metrics(hz_face_t *face, hz_index_t id);

/* function: hz_face_get_num_glyphs */
HZDECL uint16_t
hz_face_get_num_glyphs(hz_face_t *face);

/* struct: hz_font_t */
typedef struct hz_font_t hz_font_t;

/* function: hz_font_create */
HZDECL hz_font_t* hz_font_create(void);

/* function: hz_font_destroy */
HZDECL void hz_font_destroy(hz_font_t *font);

/* function: hz_font_get_face */
HZDECL hz_face_t* hz_font_get_face(hz_font_t *font);

/* function: hz_font_set_face */
HZDECL void hz_font_set_face(hz_font_t *font, hz_face_t *face);

HZDECL void hz_font_get_glyph_shape();

/*  function: hz_stbtt_font_create
 *      Loads a font from a stbtt_fontinfo structure.
 * */
HZDECL hz_font_t* hz_stbtt_font_create(stbtt_fontinfo *info);

/*  function: hz_shape
 *      Shapes a segment of text with specified font, features and shaping flags.
 *
 *  Parameters:
 *      font - The font for the shaper to use.
 *      seg - Segment of text.
 *      features - List of features to apply.
 *      num_features - Number of features.
 *      flags - Flags to configure shaping process.
 *
 *  See Also:
 *      <hz_font_t>, <hz_segment_t> and <hz_feature_t>.
 * */
HZDECL void
hz_shape(hz_font_t *font,
         hz_segment_t *seg,
         hz_feature_t *features,
         unsigned int num_features,
         hz_shape_flags_t flags);

#ifdef __cplusplus
};
#endif

#endif /* HZ_H */
