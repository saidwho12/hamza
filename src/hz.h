#ifndef HZ_H
#define HZ_H

#include "hz-ot.h"
#include "hz-script-table.h"

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
     *      dir - Writing direction.
     *      features - Array of wanted features.
     * */
    typedef struct hz_context_t {
        hz_font_t *font;
        hz_script_t script;
        hz_language_t language;
        hz_direction_t dir;
        hz_array_t *features;
    } hz_context_t;

    void
    hz_context_set_features(hz_context_t *ctx, hz_array_t *features);

    void
    hz_context_set_script(hz_context_t *ctx, hz_script_t script);

    void
    hz_context_set_language(hz_context_t *ctx, hz_language_t language);

    void
    hz_context_set_direction(hz_context_t *ctx, hz_direction_t dir);

    hz_context_t *
    hz_context_create(hz_font_t *font);

    void
    hz_context_destroy(hz_context_t *ctx);

    /*  Function: hz_shape_full
     *      Shapes a sequenceion of text.
     *
     *  Parameters:
     *      ctx - The shaping context.
     *      sequence - The sequenceion to shape.
     * */
    void
    hz_shape_full(hz_context_t *ctx, hz_sequence_t *sequence);

    hz_set_t *
    hz_context_gather_required_glyphs(hz_context_t *ctx);

    hz_index_t
    hz_face_map_unicode_to_id(hz_face_t *face, hz_unicode_t c);

    hz_language_t    
    hz_lang(const char *tag);
    
#ifdef __cplusplus
}
#endif

#endif /* HZ_H */
