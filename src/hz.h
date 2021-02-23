#ifndef HZ_H
#define HZ_H

#include "hz-ot.h"

#ifdef __cplusplus
extern "C" {
#endif

/*  Struct: hz_context_t
 *      Shaping context structure.
 *
 *  Fields:
 *      font - Font used.
 *      script - Script.
 *      language - Language.
 *      dir - Writing direction (RTL/LTR)
 *      features - Array of wanted features.
 * */
typedef struct hz_context_t {
    hz_font_t *font;
    hz_script_t script;
    hz_language_t language;
    hz_dir_t dir;
    hz_array_t *features;
} hz_context_t;

void
hz_context_set_features(hz_context_t *ctx, hz_array_t *features);

void
hz_context_set_script(hz_context_t *ctx, hz_script_t script);

void
hz_context_set_language(hz_context_t *ctx, hz_language_t language);

void
hz_context_set_dir(hz_context_t *ctx, hz_dir_t dir);

hz_context_t *
hz_context_create(hz_font_t *font);

void
hz_context_destroy(hz_context_t *ctx);

/*  Function: hz_shape_full
 *      Shapes a section of text.
 *
 *  Parameters:
 *      ctx - The shaping context.
 *      sect - The section to shape.
 * */
void
hz_shape_full(hz_context_t *ctx, hz_section_t *sect);

void
hz_context_collect_required_glyphs(hz_context_t *ctx,
                                   hz_set_t *glyphs);


hz_id
hz_face_map_unicode_to_id(hz_face_t *face, hz_unicode c);

#ifdef __cplusplus
}
#endif

#endif /* HZ_H */