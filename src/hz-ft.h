#ifndef HZ_FT_H
#define HZ_FT_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OPENTYPE_VALIDATE_H
#include FT_GLYPH_H
#include FT_TRUETYPE_TABLES_H

#include "hz.h"
#include "hz-face.h"
#include "hz-font.h"

#ifdef __cplusplus
extern "C" {
#endif

static hz_blob_t*
hz_ft_load_snft_table(FT_Face face, hz_tag_t tag)
{
    hz_blob_t *blob = hz_blob_create();
    FT_ULong size = 0;
    FT_Load_Sfnt_Table(face, tag, 0, NULL, &size);
    hz_blob_resize(blob, size);
    FT_Load_Sfnt_Table(face, tag, 0, hz_blob_get_data(blob), &size);
    return blob;
}

static hz_font_t *
hz_ft_font_create(FT_Face ft_face) {
    HZ_ASSERT(ft_face != NULL);

    hz_tag_t tags[] = {
            HZ_TAG('c','m','a','p'),
            HZ_TAG('m','a','x','p'),
            HZ_TAG('g','l','y','f'),
            HZ_TAG('h','m','t','x'),
    };

    size_t tag_index, glyph_index;
    hz_metrics_t *metrics = NULL;

    hz_font_t *font = hz_font_create();
    hz_face_t *face = hz_face_create();
    hz_face_set_upem(face, ft_face->units_per_EM);

    hz_face_ot_tables_t ot_tables;
    FT_Bytes BASE_table;
    FT_Bytes GDEF_table;
    FT_Bytes GSUB_table;
    FT_Bytes GPOS_table;
    FT_Bytes JSTF_table;

    if (FT_OpenType_Validate(ft_face, FT_VALIDATE_OT,
                             &BASE_table, &GDEF_table, &GPOS_table, &GSUB_table, &JSTF_table)
                            != FT_Err_Ok) {
        HZ_ERROR("Failed to validate OpenType tables!");
        hz_face_destroy(face);
        hz_font_destroy(font);
        return NULL;
    }

    ot_tables.BASE_table = (hz_byte_t *)BASE_table;
    ot_tables.GDEF_table = (hz_byte_t *)GDEF_table;
    ot_tables.GSUB_table = (hz_byte_t *)GSUB_table;
    ot_tables.GPOS_table = (hz_byte_t *)GPOS_table;
    ot_tables.JSTF_table = (hz_byte_t *)JSTF_table;

    hz_face_set_ot_tables(face, &ot_tables);

    for (tag_index = 0; tag_index < HZ_ARRAY_SIZE(tags); ++tag_index) {
        hz_blob_t *blob = hz_ft_load_snft_table(ft_face, tags[tag_index]);
        if (blob != NULL) {
            hz_face_set_table(face, tags[tag_index], blob);
        }
    }

    hz_face_load_num_glyphs(face);
    hz_face_alloc_metrics(face);

    for (glyph_index = 0; glyph_index < hz_face_get_num_glyphs(face); ++glyph_index) {
        FT_GlyphSlot slot = ft_face->glyph;
        FT_Load_Glyph(ft_face, glyph_index, FT_LOAD_NO_BITMAP | FT_LOAD_NO_SCALE);
        FT_Glyph_Metrics metrics = ft_face->glyph->metrics;
        FT_Glyph glyph;
        FT_Get_Glyph(slot, &glyph);

        FT_BBox bbox;
        FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_SUBPIXELS, &bbox);

        FT_Done_Glyph(glyph);

        hz_metrics_t *out_metrics = hz_face_get_glyph_metrics(face, glyph_index);
        out_metrics->x_advance = metrics.horiAdvance;
        out_metrics->y_advance = 0;
        out_metrics->x_bearing = metrics.horiBearingX;
        out_metrics->y_bearing = metrics.horiBearingY;
        out_metrics->width = metrics.width;
        out_metrics->height = metrics.height;
        out_metrics->x_min = bbox.xMin;
        out_metrics->x_max = bbox.xMax;
        out_metrics->y_min = bbox.yMin;
        out_metrics->y_max = bbox.yMax;
    }

    hz_font_set_face(font, face);
    return font;
//
//    {
//        /* load few variables from maxp table */
//        hz_blob_t *maxp_buf = hz_ft_load_snft_table(ft_face, HZ_TAG('m', 'a', 'x', 'p'));
//        hz_decode_maxp_table(face, maxp_buf);
//        hz_blob_destroy(maxp_buf);
//    }
//
//    {
//        /* load few variables from hhea table */
//        /* Horizontal Header Table */
//        hz_blob_t *hhea_buf = hz_ft_load_snft_table(ft_face, HZ_TAG('h', 'h', 'e', 'a'));
//        hz_decode_hhea_table(face, hhea_buf);
//        hz_blob_destroy(hhea_buf);
//    }
//
//    face->metrics = (hz_metrics_t *) calloc(face->num_glyphs, sizeof(hz_metrics_t));
//    uint16_t glyph_index = 0;
//    while (glyph_index < face->num_glyphs) {
//        FT_GlyphSlot glyph_slot = ft_face->glyph;
//        FT_Load_Glyph(ft_face, glyph_index, FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING);
//
//        hz_metrics_t *metric = &face->metrics[glyph_index];
//        metric->x_advance = glyph_slot->metrics.horiAdvance;
//        metric->y_advance = 0;//glyph_slot->metrics.vertAdvance;
//        metric->x_bearing = glyph_slot->metrics.horiBearingX;
//        metric->y_bearing = glyph_slot->metrics.horiBearingY;
//
//        FT_Glyph glyph;
//        FT_Get_Glyph(glyph_slot, &glyph);
//
//        FT_BBox bbox;
//        FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_PIXELS, &bbox);
//
//        metric->x_min = bbox.xMin;
//        metric->x_max = bbox.xMax;
//        metric->y_min = bbox.yMin;
//        metric->y_max = bbox.yMax;
//        metric->width = metric->x_max - metric->x_min;
//        metric->height = metric->y_max - metric->y_min;
//
//        ++glyph_index;
//    }
}


#ifdef __cplusplus
}
#endif

#endif /* HZ_FT_H */