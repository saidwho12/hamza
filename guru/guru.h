#ifndef GURU_H
#define GURU_H

#include "guru-base.h"
#include "guru-ot.h"


#ifdef __cplusplus
extern "C" {
#endif

enum guru_gsub_lookup_type_t {
    GURU_GSUB_LOOKUP_TYPE_SINGLE = 1,
    GURU_GSUB_LOOKUP_TYPE_MULTIPLE = 2,
    GURU_GSUB_LOOKUP_TYPE_ALTERNATE = 3,
    GURU_GSUB_LOOKUP_TYPE_LIGATURE = 4,
    GURU_GSUB_LOOKUP_TYPE_CONTEXT = 5,
    GURU_GSUB_LOOKUP_TYPE_CHAINING_CONTEXT = 6,
    GURU_GSUB_LOOKUP_TYPE_EXTENSION_SUBSTITUTION = 7,
    GURU_GSUB_LOOKUP_TYPE_REVERSE_CHAINING_CONTEXT_SINGLE = 8,
};

typedef struct guru_ctx_t {
    guru_face_t *face;
    guru_script_t script;
    guru_language_t language;
    guru_dir_t dir;
    guru_bitset_t *features;
} guru_ctx_t;

guru_bool
guru_ctx_set_features(guru_ctx_t *ctx, guru_bitset_t *features);

void
guru_ctx_set_script(guru_ctx_t *ctx, guru_script_t script);

void
guru_ctx_set_language(guru_ctx_t *ctx, guru_language_t language);

void
guru_ctx_set_dir(guru_ctx_t *ctx, guru_dir_t dir);
/*
void
guru_context_collect_GIDs(guru_context_t *context, guru_set_t *GIDs);
*/
guru_ctx_t *
guru_ctx_create(guru_face_t *face);

typedef struct guru_shaped_glyph_t {
    guru_id id;
    guru_float32 x0;
    guru_float32 y0;
    guru_float32 x1;
    guru_float32 y1;
} guru_shaped_glyph_t;

guru_status_t
guru_shape(guru_ctx_t *ctx, guru_run_t *run);

#ifdef __cplusplus
};
#endif

#endif /* GURU_H */