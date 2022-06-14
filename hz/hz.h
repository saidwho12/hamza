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
#define HZ_API HZ_DLLEXPORT
#else
#define HZ_API
#endif

#if HZ_COMPILER & HZ_COMPILER_VC
#define HZ_DBGBRK __debugbreak();
#elif HZ_COMPILER & HZ_COMPILER_GCC
#define HZ_DBGBRK __builtin_trap();
#else
#define HZ_BDGBRK
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

typedef unsigned char hz_byte_t;
typedef int32_t hz_coord_t;
typedef uint32_t hz_unicode_t;
typedef uint32_t hz_tag_t;

typedef uint32_t hz_bool_t;
#define HZ_TRUE 1
#define HZ_FALSE 0

typedef uint16_t hz_index_t;
typedef int32_t hz_fixed32_t, hz_position_t, hz_fixed26dot6_t;

// dynamic array header
typedef struct hz_vector_hdr_t {
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


typedef struct hz_bbox_t {
    hz_position_t x0, y0, x1, y1;
} hz_bbox_t;

typedef struct hz_metrics_t {
    hz_bbox_t bbox;
    hz_position_t x_advance, y_advance, x_bearing, y_bearing;
    hz_position_t w, h;
} hz_metrics_t;

typedef enum hz_direction_t {
    HZ_DIRECTION_INVALID = 0,
    HZ_DIRECTION_LTR = 0x00000001,
    HZ_DIRECTION_RTL = 0x00000002,
    HZ_DIRECTION_TTB = 0x00000004,
    HZ_DIRECTION_BTT = 0x00000008
} hz_direction_t;

#define HZ_DIRECTION_IS_HORIZONTAL(dir) (hz_bool_t)(dir & 0x3)
#define HZ_DIRECTION_IS_VERTICAL(dir) (hz_bool_t)(dir & 0xC)

typedef enum hz_error_t {
    HZ_OK = 0ul,
    HZ_ERROR_INVALID_TABLE_TAG              = 0x00000001ul,
    HZ_ERROR_INVALID_TABLE_VERSION          = 0x00000002ul,
    HZ_ERROR_INVALID_LOOKUP_TYPE            = 0x00000004ul,
    HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT = 0x00000008ul,
    HZ_ERROR_INVALID_PARAM                  = 0x00000010ul,
    HZ_ERROR_INVALID_FORMAT                 = 0x00000020ul,
    HZ_ERROR_TABLE_DOES_NOT_EXIST           = 0x00000040ul,
    HZ_ERROR_UNEXPECTED_VALUE               = 0x00000080ul
} hz_error_t;

typedef enum hz_glyph_class_t {
    HZ_GLYPH_CLASS_ZERO      = 0,
    HZ_GLYPH_CLASS_BASE      = 0x00000001ul,
    HZ_GLYPH_CLASS_LIGATURE  = 0x00000002ul,
    HZ_GLYPH_CLASS_MARK      = 0x00000004ul,
    HZ_GLYPH_CLASS_COMPONENT = 0x00000008ul
} hz_glyph_class_t;

/* include tables, types, and functions and other generated code */
#include "hz-data-tables.h"

typedef void *(*HZMALLOCFN)(size_t);
typedef void (*HZFREEFN)(void *);

/*
 *  Struct: hz_allocator_t
 *      Global heap allocator, an instance of this structure is stored internally and
 *      is used by the main memory pool. By default <hz_allocator_t.mallocfn> and
 *      <hz_allocator_t.freefn> are set to stdlib's malloc() and free().
 *      If you want to set a custom internal allocator, use <hz_set_custom_allocator> as follows:
 *      --- C ---
 *      hz_allocator_t a;
 *      a.mallocfn = (HZMALLOCFN)mymalloc;
 *      a.freefn = (HZFREEFN)myfree;
 *      hz_set_custom_allocator(a);
 *      ---------
 */
typedef struct hz_allocator_t {
    HZMALLOCFN mallocfn;
    HZFREEFN freefn;
} hz_allocator_t;

/*  Function: hz_set_custom_allocator
 *      Sets the internal allocator to a custom user-provided allocator.
 *  Arguments:
 *      a - Custom allocator.
 */
HZ_API void
hz_set_custom_allocator(hz_allocator_t a);

/*
 *  Struct: hz_segment_t
 *      A segment of text, can be loaded with utf8 or unicode data.
 *      Holds direction, script, language,
 *      as well as other data relevant for the shaping process.
 */
typedef struct hz_segment_t hz_segment_t;


/*  Function: hz_segment_create
 *      Creates a new segment.
 */
HZ_API hz_segment_t* hz_segment_create(void);

/*  Function: hz_segment_destroy
 *      Destroys a segment.
 *  Arguments:
 *      seg - The segment.
 */
HZ_API void
hz_segment_destroy(hz_segment_t *seg);

HZ_API void
hz_segment_set_script(hz_segment_t *seg, hz_script_t script);

HZ_API void
hz_segment_set_language(hz_segment_t *seg, hz_language_t language);

HZ_API void
hz_segment_set_direction(hz_segment_t *seg, hz_direction_t direction);


/*  Function: hz_segment_load_utf8
 *      Loads segment with UTF-8 encoded string.
 *  Arguments:
 *      seg - The segment.
 *      str - UTF-8 encoded string.
 */
HZ_API void
hz_segment_load_utf8(hz_segment_t *seg, const char *text);

HZ_API void
hz_segment_load_latin1(hz_segment_t *seg, const char *text);

/*  Function: hz_segment_load_utf32
 *      Loads a segment with a Unicode string.
 *  Arguments:
 *      seg - The segment.
 *      str - UTF-32 string.
 */
HZ_API void
hz_segment_load_utf32(hz_segment_t *seg, const uint32_t *str);

typedef struct hz_glyph_metrics_t {
    int32_t x_advance;
    int32_t y_advance;
    int32_t x_offset;
    int32_t y_offset;
} hz_glyph_metrics_t;

typedef enum hz_glyph_attrib_flag_t {
    HZ_GLYPH_ATTRIB_METRICS_BIT = 0x00000001,
    HZ_GLYPH_ATTRIB_INDEX_BIT = 0x00000002,
    HZ_GLYPH_ATTRIB_CODEPOINT_BIT = 0x00000004,
    HZ_GLYPH_ATTRIB_GLYPH_CLASS_BIT = 0x00000008,
    HZ_GLYPH_ATTRIB_ATTACHMENT_CLASS_BIT = 0x00000010,
    HZ_GLYPH_ATTRIB_ATTACHMENT_INDEX_BIT = 0x00000020,
} hz_glyph_attrib_flag_t;

typedef struct hz_buffer_t {
    size_t                  glyph_count;
    hz_glyph_metrics_t *    glyph_metrics;
    hz_index_t *            glyph_indices;
    hz_unicode_t *          codepoints;
    uint16_t *              glyph_classes;
    uint16_t *              attachment_classes;
    uint16_t *              attachment_indices;
    hz_glyph_attrib_flag_t  attrib_flags;
} hz_buffer_t;

HZ_API const hz_buffer_t *
hz_segment_get_buffer(hz_segment_t *seg);

#define HZ_OT_TAG_GPOS HZ_TAG('G','P','O','S')
#define HZ_OT_TAG_GSUB HZ_TAG('G','S','U','B')

typedef enum hz_lookup_flag_t {
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
} hz_lookup_flag_t;

typedef enum hz_value_format_flag_t {
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
} hz_value_format_flag_t;

typedef enum hz_gsub_lookup_type_t {
    HZ_GSUB_LOOKUP_TYPE_SINGLE_SUBSTITUTION = 1,
    HZ_GSUB_LOOKUP_TYPE_MULTIPLE_SUBSTITUTION = 2,
    HZ_GSUB_LOOKUP_TYPE_ALTERNATE_SUBSTITUTION = 3,
    HZ_GSUB_LOOKUP_TYPE_LIGATURE_SUBSTITUTION = 4,
    HZ_GSUB_LOOKUP_TYPE_CONTEXTUAL_SUBSTITUTION = 5,
    HZ_GSUB_LOOKUP_TYPE_CHAINED_CONTEXTS_SUBSTITUTION = 6,
    HZ_GSUB_LOOKUP_TYPE_EXTENSION_SUBSTITUTION = 7,
    HZ_GSUB_LOOKUP_TYPE_REVERSE_CHAINING_CONTEXTUAL_SINGLE_SUBSTITUTION = 8
} hz_gsub_lookup_type_t;

typedef enum hz_gpos_lookup_type_t {
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

typedef enum hz_shape_flags_t {
    HZ_SHAPE_FLAG_AUTO_LOAD_FEATURES = 0x00000001,
    HZ_SHAPE_FLAG_REMOVE_MARKS = 0x00000002,
    HZ_SHAPE_FLAG_REMOVE_BASES = 0x00000004
} hz_shape_flags_t;

HZ_API int hz_setup (void);

HZ_API int hz_cleanup (void);

/*  Function: hz_lang
 *      Returns language from an ISO-639 tag.
 */
HZ_API hz_language_t hz_lang(const char *tag);

/*  Function: hz_script
 *      Returns script from an ISO-15924 tag.
 */
HZ_API hz_script_t hz_script(const char *tag);

typedef struct hz_face_t hz_face_t;

HZ_API hz_face_t* hz_face_create(void);

HZ_API void
hz_face_destroy(hz_face_t *face);

HZ_API uint16_t
hz_face_get_upem(hz_face_t *face);

HZ_API void
hz_face_set_upem(hz_face_t *face, uint16_t upem);

HZ_API uint16_t
hz_face_get_num_of_h_metrics(hz_face_t *face);

HZ_API uint16_t
hz_face_get_num_of_v_metrics(hz_face_t *face);

HZ_API hz_metrics_t *
hz_face_get_glyph_metrics(hz_face_t *face, hz_index_t id);


HZ_API uint16_t
hz_face_get_num_glyphs(hz_face_t *face);

/* font functions, and loading font using FreeType */
typedef struct hz_font_t hz_font_t;

/*  Function: hz_font_create
 *      Creates a font.
 */
HZ_API hz_font_t* hz_font_create(void);

/*  Function: hz_font_destroy
 *     Destroys a font.
 *  Arguments:
 *     font - The font.
 *  See Also:
 *     <hz_font_create>
 */
HZ_API void
hz_font_destroy(hz_font_t *font);

HZ_API hz_face_t *
hz_font_get_face(hz_font_t *font);

HZ_API void
hz_font_set_face(hz_font_t *font, hz_face_t *face);

/*  Function: hz_stbtt_font_create
 *      Creates a font from a stb_truetype font info structure.
 *  Arguments:
 *      fontinfo - The font info structure.
 *  Returns:
 *      A pointer to the font.
 *  See Also:
 *      <hz_font_destroy>
 */
HZ_API hz_font_t *
hz_stbtt_font_create(stbtt_fontinfo *info);

/*  Function: hz_shape
 *      Shapes a text seg to positioned glyphs. If features is *NULL*,
 *      then default features are loaded for the segment's script.
 *  Arguments:
 *      font - Pointer to the font.
 *      seg - Pointer to segment of text.
 *      features - Array of features to apply.
 *      num_features - Number of features.
 *      shape_flags - Shaping flags.
 */
HZ_API void
hz_shape(hz_font_t *font,
         hz_segment_t *seg,
         const hz_feature_t *features,
         unsigned int num_features,
         hz_shape_flags_t shape_flags);

#ifdef __cplusplus
};
#endif

#endif /* HZ_H */
