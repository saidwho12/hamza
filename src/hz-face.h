#ifndef HZ_FACE_H
#define HZ_FACE_H

#include "hz-base.h"
#include "util/hz-blob.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hz_face_tables_t hz_face_tables_t;
typedef struct hz_face_t hz_face_t;

typedef struct hz_face_ot_tables_t {
    hz_byte *BASE_table;
    hz_byte *GDEF_table;
    hz_byte *GSUB_table;
    hz_byte *GPOS_table;
    hz_byte *JSTF_table;
} hz_face_ot_tables_t;

hz_face_t *
hz_face_create();

void
hz_face_destroy(hz_face_t *face);

uint16_t
hz_face_get_upem(hz_face_t *face);

void
hz_face_set_upem(hz_face_t *face, uint16_t upem);

void
hz_face_set_table(hz_face_t *face, hz_tag_t tag, hz_blob_t *blob);

hz_blob_t *
hz_face_reference_table(hz_face_t *face, hz_tag_t tag);

void
hz_face_set_num_glyphs(hz_face_t *face, uint16_t num_glyphs);

uint16_t
hz_face_get_num_glyphs(hz_face_t *face);

uint16_t
hz_face_get_num_of_h_metrics(hz_face_t *face);

uint16_t
hz_face_get_num_of_v_metrics(hz_face_t *face);

hz_metrics_t *
hz_face_get_glyph_metrics(hz_face_t *face, hz_index_t id);

void
hz_face_set_num_of_h_metrics(hz_face_t *face, uint16_t num_of_h_metrics);

void
hz_face_load_num_glyphs(hz_face_t *face);

void
hz_face_load_upem(hz_face_t *face);

float
hz_face_line_skip(hz_face_t *face);

void
hz_face_set_ot_tables(hz_face_t *face, const hz_face_ot_tables_t *tables);

const hz_face_ot_tables_t *
hz_face_get_ot_tables(hz_face_t *face);

void
hz_face_alloc_metrics(hz_face_t *face);

#ifdef __cplusplus
}
#endif

#endif /* HZ_FACE_H */