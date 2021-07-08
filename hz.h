/* 
    About: License
        This file is licensed under MIT.

    Topic: Usage
    
    Shaping with font created from an FT_Face (requires Hamza to be built with Freetype)
    
    --- C
    #define HZ_USE_FREETYPE
    #include <hz.h>

    int main(int argc, char *argv[])
    {
        const char *text = "Hello, World!";
        FT_Library ft_lib;

    }

    ---
*/

#ifndef HZ_H
#define HZ_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OPENTYPE_VALIDATE_H
#include FT_GLYPH_H
#include FT_TRUETYPE_TABLES_H


#define HZ_ENABLE 1
#define HZ_DISABLE 0

#if defined(HZ_FORCE_INLINE_FUNCS)
    #if defined(_MSC_VER)
        #define HZ_INLINE __forceinline
    #elif defined(__GNUC__)
        #define HZ_INLINE __attribute__((always_inline))
    #else
        #define HZ_INLINE inline
    #endif
#else
    #define HZ_INLINE inline
#endif

#if defined(_MSC_VER)
    #define HZ_CALLCONV __fastcall
    #define HZ_EXPORT __declspec(dllexport)
#elif defined(__GNUC__) && (defined(_WIN32) || defined(__MINGW32__) || defined(__CYGWIN__))
    #define HZ_CALLCONV __attribute__((fastcall))
    #define HZ_EXPORT __attribute__((dllexport))
#else
    #define HZ_CALLCONV
    #define HZ_EXPORT
#endif

#define HZ_COMPILER_GCC 0x00000001l
#define HZ_COMPILER_CLANG 0x00000002l
#define HZ_COMPILER_TURBOC 0x00000004l
#define HZ_COMPILER_VC 0x00000008l
#define HZ_COMPILER_INTEL 0x00000010l

#if defined(_MSC_VER)
    #define HZ_COMPILER HZ_COMPILER_VC
#elif defined(__GNUC__) || defined(__GCC__)
    #define HZ_COMPILER HZ_COMPILER_GCC
#elif defined(__clang__)
    #define HZ_COMPILER HZ_COMPILER_CLANG
#endif

#define HZ_API HZ_CALLCONV HZ_EXPORT 

#define HZ_ARRLEN(x) (sizeof(x)/sizeof((x)[0]))
#define HZ_UNARR(x) x, (sizeof(x)/sizeof((x)[0]))
#define HZ_ASSERT(cond) assert(cond)
#define HZ_TAG(a, b, c, d) ((hz_tag_t)d | ((hz_tag_t)c << 8) | ((hz_tag_t)b << 16) | ((hz_tag_t)a << 24U))
#define HZ_UNTAG(tag) (char) ((tag >> 24) & 0xFF), (char)((tag >> 16) & 0xFF), (char)((tag >> 8) & 0xFF), (char)(tag & 0xFF)


#define HZ_ENABLE_OVERFLOW_CHECKS

typedef uint16_t hz_offset16_t;
typedef uint32_t hz_offset32_t;
typedef unsigned char hz_byte_t;
typedef int32_t hz_coord_t;
typedef uint32_t hz_unicode_t;
typedef uint32_t hz_tag_t;

#if defined(__cplusplus)
    /* C++ */
    typedef bool hz_bool_t;
    #define HZ_TRUE true
    #define HZ_FALSE false
#elif defined(__STDC__)
    /* ISO standard C */
    #if __STDC_VERSION__ < 199901L
        /* earlier than the 1999 C standard */
        typedef uint8_t hz_bool_t;
        #define HZ_TRUE (hz_bool_t)1
        #define HZ_FALSE (hz_bool_t)0
    #else
        /* 1999 C standard and above */
        #include <stdbool.h>
        typedef bool hz_bool_t;
        #define HZ_TRUE true
        #define HZ_FALSE false
    #endif
#endif

typedef uint16_t hz_index_t;
typedef int32_t hz_fixed32_t, hz_position_t, hz_fixed26dot6_t;

typedef struct hz_bbox_t {
    hz_position_t x0, y0, x1, y1;
} hz_bbox_t;

typedef struct hz_metrics_t {
    hz_bbox_t bbox; /* x advance, y advance, x bearing, y bearing */
    hz_position_t xa, ya, xb, yb;
    hz_position_t w, h; /* width and height of glyph */
} hz_metrics_t;

typedef struct hz_sequence_t hz_sequence_t;

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
    HZ_OK = 0,
    HZ_ERROR_INVALID_TABLE_TAG = 0x00000001,
    HZ_ERROR_INVALID_TABLE_VERSION = 0x00000002,
    HZ_ERROR_INVALID_LOOKUP_TYPE = 0x00000004,
    HZ_ERROR_INVALID_LOOKUP_SUBTABLE_FORMAT = 0x00000008,
    HZ_ERROR_INVALID_PARAM = 0x00000010,
    HZ_ERROR_INVALID_FORMAT = 0x00000020,
    HZ_ERROR_TABLE_DOES_NOT_EXIST = 0x00000040
} hz_error_t;

typedef enum hz_glyph_class_t {
    HZ_GLYPH_CLASS_ZERO = 0,
    HZ_GLYPH_CLASS_BASE = 0x00000001,
    HZ_GLYPH_CLASS_LIGATURE = 0x00000002,
    HZ_GLYPH_CLASS_MARK = 0x00000004,
    HZ_GLYPH_CLASS_COMPONENT = 0x00000008
} hz_glyph_class_t;

/* include tables, types, and functions and other generated code */
#include "hz_defs.h"

typedef void *(*HZMALLOCFUNC)(size_t);
typedef void (*HZFREEFUNC)(void *);

typedef struct hz_global_allocator_t {
    HZMALLOCFUNC malloc_func;
    HZFREEFUNC free_func;
} hz_global_allocator_t;

/* common functions */

/*  Function: hz_lang
 *      returns language from an ISO-639 tag.
 */
HZ_API hz_language_t
hz_lang(const char *tag);

/*  Function: hz_script
 *      returns script from an ISO-15924 tag.
 */
HZ_API hz_script_t
hz_script(const char *tag);

/* font face public functions */
typedef struct hz_face_t hz_face_t;

HZ_API hz_face_t *
hz_face_create();

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

/* font functions, and loading font using FreeType */
typedef struct hz_font_t hz_font_t;

/*
    Function: hz_font_create 
        
        Creates a font.
*/

HZ_API hz_font_t *
hz_font_create(void);

/*
    Function: hz_font_destroy 
        
        Destroys a font.

    Arguments:

        font - The font.
    
    See Also:
        <hz_font_create>
*/
HZ_API void
hz_font_destroy(hz_font_t *font);

HZ_API hz_face_t *
hz_font_get_face(hz_font_t *font);

HZ_API void
hz_font_set_face(hz_font_t *font, hz_face_t *face);

/*
    Function: hz_ft_font_create 
        
        Creates a font from a Freetype face handle.

    Arguments:

        ft_face - The handle to the Freetype typeface.

    Returns:
        A pointer to the font.
    
    See Also:
        <hz_font_destroy>
*/
HZ_API hz_font_t *
hz_ft_font_create(FT_Face ft_face);

/*
HZ_API hz_font_t *
hz_stbtt_font_create(stbtt_fontinfo *font);
*/

/*
    Function: hz_shape
        Shapes a text sequence to positioned glyphs. If features is *NULL*,
        then default features are loaded for the sequence's script.

    Arguments:
        font - Pointer to the font.
        sequence - Pointer to sequence with loaded text.
        features - Array of features to apply.
        num_features - Number of features.
*/
HZ_API void
hz_shape(hz_font_t *font,
         hz_sequence_t *sequence,
         const hz_feature_t *features,
         unsigned int num_features);


#endif /* HZ_H */