#ifndef GURU_H
#define GURU_H

#include "guru-base.h"
#include "guru-ot.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct guru_ctx_t {
    guru_face_t *face;
    guru_script_t script;
    guru_language_t language;
    guru_dir_t dir;
    guru_bitset_t *features;
} guru_context_t;

guru_bool
guru_ctx_set_features(guru_context_t *ctx, guru_bitset_t *features);

void
guru_ctx_set_script(guru_context_t *ctx, guru_script_t script);

void
guru_ctx_set_language(guru_context_t *ctx, guru_language_t language);

void
guru_ctx_set_dir(guru_context_t *ctx, guru_dir_t dir);
/*
void
guru_context_collect_GIDs(guru_context_t *context, guru_set_t *GIDs);
*/
guru_context_t *
guru_ctx_create(guru_face_t *face);

typedef struct guru_shaped_glyph_t {
    guru_id id;
    guru_float32 x0;
    guru_float32 y0;
    guru_float32 x1;
    guru_float32 y1;
} guru_shaped_glyph_t;

guru_status_t
guru_shape(guru_context_t *ctx, guru_run_t *run);

void
guru_context_collect_required_glyphs(guru_context_t *ctx,
                                     guru_set_t *glyphs);

#ifdef __cplusplus
}
#endif

#endif /* GURU_H */