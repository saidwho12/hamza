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
hm_ctx_set_features(hm_context_t *ctx, hm_bitset_t *features);

void
hm_ctx_set_script(hm_context_t *ctx, hm_script_t script);

void
hm_ctx_set_language(hm_context_t *ctx, hm_language_t language);

void
hm_ctx_set_dir(hm_context_t *ctx, hm_dir_t dir);
/*
void
hm_context_collect_GIDs(hm_context_t *context, hm_set_t *GIDs);
*/
hm_context_t *
hm_ctx_create(hm_face_t *face);

typedef struct hm_shaped_glyph_t {
    hm_id id;
    hm_float32 x0;
    hm_float32 y0;
    hm_float32 x1;
    hm_float32 y1;
} hm_shaped_glyph_t;

hm_status_t
hm_shape(hm_context_t *ctx, hm_run_t *run);

void
hm_context_collect_required_glyphs(hm_context_t *ctx,
                                   hm_set_t *glyphs);

#ifdef __cplusplus
}
#endif

#endif /* HM_H */