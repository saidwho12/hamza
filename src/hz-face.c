#include "hz-face.h"
#include "util/hz-map.h"
#include "hz-ot.h"

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

    hz_map_t *class_map;
    hz_map_t *attach_class_map;
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
    face->class_map = hz_map_create();
    face->attach_class_map = hz_map_create();
    return face;
}

void
hz_face_destroy(hz_face_t *face)
{

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
    buf_t table = createbuf(hz_blob_get_data(blob), BUF_BSWAP);

    uint32_t version;
    uint16_t num_glyphs;

    version = unpacki(&table);

    switch (version) {
        case 0x00005000: {
            /* version 0.5 */
            num_glyphs = unpackh(&table);
            break;
        }
        case 0x00010000: {
            /* version 1.0 with full information */
            num_glyphs = unpackh(&table);
            break;
        }
        default:
            /* error */
            break;
    }

    face->num_glyphs = num_glyphs;
}

void
hz_face_load_class_maps(hz_face_t *face)
{
    if (hz_face_get_ot_tables(face)->GDEF_table != NULL) {
        buf_t table = createbuf(hz_face_get_ot_tables(face)->GDEF_table, BUF_BSWAP);
        uint32_t version;

        hz_offset16_t glyph_class_def_offset;
        hz_offset16_t attach_list_offset;
        hz_offset16_t lig_caret_list_offset;
        hz_offset16_t mark_attach_class_def_offset;
        hz_offset16_t mark_glyph_sets_def_offset;

        version = unpacki(&table);

        switch (version) {
            case 0x00010000: /* 1.0 */
                unpackv(&table, "hhhh",
                        &glyph_class_def_offset,
                        &attach_list_offset,
                        &lig_caret_list_offset,
                        &mark_attach_class_def_offset);
                break;
            case 0x00010002: /* 1.2 */
                unpackv(&table, "hhhhh",
                        &glyph_class_def_offset,
                        &attach_list_offset,
                        &lig_caret_list_offset,
                        &mark_attach_class_def_offset,
                        &mark_glyph_sets_def_offset);
                break;
            case 0x00010003: /* 1.3 */
                break;
            default: /* error */
                break;
        }

        if (glyph_class_def_offset) {
            /* glyph class def isn't nil */
            buf_t subtable = createbuf(table.data + glyph_class_def_offset, BUF_BSWAP);
            uint16_t class_format;
            class_format = unpackh(&subtable);
            switch (class_format) {
                case 1:
                    break;
                case 2: {
                    uint16_t range_index = 0, class_range_count;
                    class_range_count = unpackh(&subtable);

                    while (range_index < class_range_count) {
                        uint16_t start_glyph_id, end_glyph_id, glyph_class;
                        unpackv(&subtable, "hhh",
                                &start_glyph_id,
                                &end_glyph_id,
                                &glyph_class);
                        HZ_ASSERT(glyph_class >= 1 && glyph_class <= 4);
                        hz_map_set_value_for_keys(face->class_map, start_glyph_id, end_glyph_id, HZ_BIT(glyph_class - 1));

                        ++range_index;
                    }
                    break;
                }
                default:
                    break;
            }
        }

        if (mark_attach_class_def_offset) {
            buf_t subtable = createbuf(table.data + mark_attach_class_def_offset, BUF_BSWAP);
            uint16_t class_format;
            class_format = unpackh(&subtable);
            switch (class_format) {
                case 1:
                    break;
                case 2: {
                    uint16_t range_index = 0, class_range_count;
                    class_range_count = unpackh(&subtable);

                    while (range_index < class_range_count) {
                        uint16_t start_glyph_id, end_glyph_id, glyph_class;
                        unpackv(&subtable, "hhh",
                                &start_glyph_id,
                                &end_glyph_id,
                                &glyph_class);
                        HZ_ASSERT(glyph_class >= 1 && glyph_class <= 4);
                        hz_map_set_value_for_keys(face->attach_class_map, start_glyph_id, end_glyph_id, HZ_BIT(glyph_class - 1));

                        ++range_index;
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
}

hz_glyph_class_t
hz_face_get_glyph_class(hz_face_t *face, hz_index_t id)
{
    if (hz_map_value_exists(face->class_map, id)) {
        return hz_map_get_value(face->class_map, id);
    }

    return HZ_GLYPH_CLASS_BASE;
}

uint8_t
hz_face_get_glyph_attach_class(hz_face_t *face, hz_index_t id) {
    if (hz_map_value_exists(face->attach_class_map, id)) {
        return hz_map_get_value(face->attach_class_map, id);
    }

    return 0;
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