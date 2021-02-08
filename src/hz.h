#ifndef HZ_H
#define HZ_H

#include "hz-ot.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hz_context_t {
    hz_face_t *face;
    hz_script_t script;
    hz_language_t language;
    hz_dir_t dir;
    hz_bitset_t *features;
} hz_context_t;

hz_bool
hz_context_set_features(hz_context_t *ctx, hz_bitset_t *features);

void
hz_context_set_script(hz_context_t *ctx, hz_script_t script);

void
hz_context_set_language(hz_context_t *ctx, hz_language_t language);

void
hz_context_set_dir(hz_context_t *ctx, hz_dir_t dir);

hz_context_t *
hz_context_create(hz_face_t *face);

void
hz_context_destroy(hz_context_t *ctx);

hz_status_t
hz_shape_full(hz_context_t *ctx, hz_section_t *sect);

void
hz_context_collect_required_glyphs(hz_context_t *ctx,
                                   hz_set_t *glyphs);


hz_id
hz_face_map_unicode_to_id(hz_face_t *face, hz_unicode c);


void
hz_decode_maxp_table(hz_face_t *face, hz_buf_t *maxp_buf);

void
hz_decode_hhea_table(hz_face_t *face, hz_buf_t *hhea_buf);

#ifdef __cplusplus
}
#endif

#endif /* HZ_H */