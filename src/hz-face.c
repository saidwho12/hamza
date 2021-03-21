#include "hz-face.h"

typedef struct hz_face_table_node_t hz_face_table_node_t;

struct hz_face_table_node_t {
    hz_tag_t tag;
    hz_blob_t *blob;
    hz_face_table_node_t *prev, *next;
};

struct hz_face_tables_t {
    struct hz_face_table_node_t *root;
};

struct hz_face_t {
    hz_face_tables_t tables;
    hz_face_ot_tables_t ot_tables;

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
    face->tables.root = NULL;
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
hz_face_set_table(hz_face_t *face, hz_tag_t tag, hz_blob_t *blob)
{
    hz_face_table_node_t *new_node = HZ_MALLOC(sizeof(hz_face_table_node_t));
    new_node->tag = tag;
    new_node->blob = blob;
    new_node->next = NULL;

    if (face->tables.root == NULL) {
        new_node->prev = NULL;
        face->tables.root = new_node;
    } else {
        hz_face_table_node_t *node = face->tables.root;
        while (node != NULL) {
            if (node->next == NULL) {
                /* found last node */
                new_node->prev = node;
                node->next = new_node;
                break;
            }

            node = node->next;
        }
    }
}

hz_blob_t *
hz_face_reference_table(hz_face_t *face, hz_tag_t tag)
{
    hz_face_table_node_t *node = face->tables.root;

    while (node != NULL) {
        if (node->tag == tag)
            return node->blob; /* tags match, found table */

        node = node->next;
    }

    return NULL;
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
    if (id < face->num_glyphs && face->metrics != NULL) {
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


void
hz_face_set_ot_tables(hz_face_t *face, const hz_face_ot_tables_t *tables)
{
    memcpy(&face->ot_tables, tables, sizeof(hz_face_ot_tables_t));
}

const hz_face_ot_tables_t *
hz_face_get_ot_tables(hz_face_t *face)
{
   return &face->ot_tables;
}

void
hz_face_alloc_metrics(hz_face_t *face) {
    if (face->metrics == NULL)
        face->metrics = HZ_MALLOC(sizeof(hz_metrics_t) * face->num_glyphs);
}