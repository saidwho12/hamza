#ifndef HM_H
#define HM_H

#include "hm-base.h"
#include "hm-ot.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct hm_ctx_t {
    hm_face_t *face;
    hm_script_t script;
    hm_language_t language;
    hm_dir_t dir;
    hm_bitset_t *features;
} hm_context_t;

hm_bool
hm_context_set_features(hm_context_t *ctx, hm_bitset_t *features);

void
hm_context_set_script(hm_context_t *ctx, hm_script_t script);

void
hm_context_set_language(hm_context_t *ctx, hm_language_t language);

void
hm_context_set_dir(hm_context_t *ctx, hm_dir_t dir);

hm_context_t *
hm_context_create(hm_face_t *face);

void
hm_context_destroy(hm_context_t *ctx);

typedef struct hm_shaped_glyph_t {
    hm_id id;
    hm_float32 x0;
    hm_float32 y0;
    hm_float32 x1;
    hm_float32 y1;
} hm_shaped_glyph_t;

hm_status_t
hm_shape_full(hm_context_t *ctx, hm_section_t *sect);

void
hm_context_collect_required_glyphs(hm_context_t *ctx,
                                   hm_set_t *glyphs);


hm_id
hm_face_cmap_unicode_to_id(hm_face_t *face, hm_unicode c);

#ifdef __cplusplus
}
#endif

#endif /* HM_H */