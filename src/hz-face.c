#include "hz-face.h"

typedef struct hz_face_table_entry_t {
    hz_tag tag;
    hz_blob_t *blob;
} hz_face_table_entry_t;

struct hz_face_table_t {
    struct hz_face_table_entry_t *entries;
    size_t table_count;
};

struct hz_face_t {
    hz_face_table_t table;

    uint16_t num_glyphs;
    uint16_t num_of_h_metrics;
    uint16_t num_of_v_metrics;
    hz_metrics_t *metrics;

    int16_t ascender;
    int16_t descender;
    int16_t linegap;

    uint16_t upem;
};

hz_face_t *
hz_face_create()
{
    hz_face_t *face = malloc(sizeof(hz_face_t));
    face->num_glyphs = 0;
    face->num_of_h_metrics = 0;
    face->num_of_v_metrics = 0;
    face->metrics = NULL;
    face->ascender = 0;
    face->descender = 0;
    face->linegap = 0;
    face->upem = 0;
    return face;
}


uint16_t
hz_face_get_upem(hz_face_t *face)
{
    return face->upem;
}

void
hz_face_set_upem(hz_face_t *face, uint16_t upem)
{
    face->upem = upem;
}


void
hz_face_set_table(hz_face_t *face, hz_tag tag, hz_blob_t *blob)
{

}

hz_blob_t *
hz_face_reference_table(hz_face_t *face, hz_tag tag)
{
    hz_blob_t *blob = NULL;
    size_t table_index = 0;

    while (table_index < face->table.table_count) {
        hz_face_table_entry_t *entry = & face->table.entries[table_index];
        if (entry->tag == tag) {
            /* tags match, found table */
            blob = entry->blob;
            break;
        }

        ++ table_index;
    }

    return blob;
}

void
hz_face_set_num_glyphs(hz_face_t *face, uint16_t num_glyphs)
{
    face->num_glyphs = num_glyphs;
}

uint16_t
hz_face_get_num_glyphs(hz_face_t *face)
{
    return face->num_glyphs;
}

uint16_t
hz_face_get_num_of_h_metrics(hz_face_t *face)
{
    return face->num_of_h_metrics;
}

uint16_t
hz_face_get_num_of_v_metrics(hz_face_t *face)
{
    return face->num_of_v_metrics;
}

hz_metrics_t *
hz_face_get_glyph_metrics(hz_face_t *face, hz_index_t id)
{
    if (id < face->num_glyphs) {
        return face->metrics + id;
    }

    return NULL;
}

void
hz_face_set_num_of_h_metrics(hz_face_t *face, uint16_t num_of_h_metrics)
{
    face->num_of_h_metrics = num_of_h_metrics;
}

void
hz_face_load_num_glyphs(hz_face_t *face)
{
    hz_blob_t *blob = hz_face_reference_table(face, HZ_TAG('m','a','x','p'));
    hz_stream_t *table = hz_blob_to_stream( blob );

    uint32_t version;
    uint16_t num_glyphs;

    hz_stream_read32(table, &version);

    switch (version) {
        case 0x00005000: {
            /* version 0.5 */
            hz_stream_read16(table, &num_glyphs);
            break;
        }
        case 0x00010000: {
            /* version 1.0 with full information */
            hz_stream_read16(table, &num_glyphs);
            break;
        }
        default:
            /* error */
            break;
    }

    face->num_glyphs = num_glyphs;
}


void
hz_face_load_upem(hz_face_t *face)
{
    /* No-Op */
}

float
hz_face_line_skip(hz_face_t *face)
{
    return (float)(face->ascender - face->descender + face->linegap) / 64.0f;
}