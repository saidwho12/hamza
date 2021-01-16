#include "guru.h"


guru_bool
guru_ctx_set_features(guru_ctx_t *ctx, guru_bitset_t *features)
{
    if (features->bit_count == GURU_FEATURE_COUNT) {
        guru_bitset_copy(ctx->features, features);
        return GURU_TRUE;
    }

    return GURU_FALSE;
}

guru_ctx_t *
guru_ctx_create(guru_face_t *face)
{
    guru_ctx_t *context = GURU_ALLOC(guru_ctx_t);

    context->face = face;
    context->features = guru_bitset_create(GURU_FEATURE_COUNT);

    return context;
}

void
guru_ctx_set_script(guru_ctx_t *ctx, guru_script_t script)
{
    ctx->script = script;
}

void
guru_ctx_set_language(guru_ctx_t *ctx, guru_language_t language)
{
    ctx->language = language;
}

void
guru_ctx_set_dir(guru_ctx_t *ctx, guru_dir_t dir)
{
    ctx->dir = dir;
}



guru_status_t
guru_shape(guru_ctx_t *ctx, guru_buf_t *buf)
{
    guru_face_t *face = ctx->face;
    guru_lookup_table_t *lookups = NULL;
    guru_tag script_tag = guru_ot_script_to_tag(ctx->script);
    guru_tag language_tag = guru_ot_language_to_tag(ctx->language);

    GURU_LOG("language: \"%c%c%c%c\"\n", GURU_UNTAG(language_tag), language_tag);
    GURU_LOG("script: \"%c%c%c%c\"\n", GURU_UNTAG(script_tag), script_tag);

    guru_set_t *lookup_indices = guru_set_create();
    guru_ot_layout_collect_lookups(face, GURU_OT_TAG_GSUB,
                              script_tag,
                              language_tag,
                              ctx->features,
                              lookup_indices);
}