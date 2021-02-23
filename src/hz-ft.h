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
hz_ft_load_snft_table(FT_Face face, hz_tag tag)
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

    hz_font_t *font = hz_font_create();
    hz_face_t *face = hz_face_create();
    hz_face_set_upem(face, ft_face->units_per_EM);

    hz_tag tags[] = {
            HZ_TAG('G','S','U','B'),
            HZ_TAG('G','P','O','S'),
            HZ_TAG('G','D','E','F'),
            HZ_TAG('c','m','a','p'),
            HZ_TAG('m','a','x','p'),
            HZ_TAG('g','l','y','f'),
            HZ_TAG('h','m','t','x'),
    };

    size_t i = 0;
    while (i < HZ_ARRAY_SIZE(tags)) {
        hz_blob_t *blob = hz_ft_load_snft_table(ft_face, tags[i]);
        if (blob != NULL) {
            hz_face_set_table(face, tags[i], blob);
        }

        ++ i;
    }

    hz_font_set_face(font, face);
    return font;

//    hz_face_set_table(HZ_TAG('G','S','U','B'), hz_ft_load_snft_table());
//    face->gsub_table = (hz_byte *) GSUB;
//    face->gpos_table = (hz_byte *) GPOS;
//    face->gdef_table = (hz_byte *) GDEF;
//
//    /* load cmap table into buffer */
//    face->cmap_buf = hz_ft_load_snft_table(ft_face, HZ_TAG('c','m','a','p'));
//
//    /* load glyf table into buffer */
//    face->glyf_buf = hz_ft_load_snft_table(ft_face, HZ_TAG('g','l','y','f'));
//
//    /* load hmtx table into buffer */
//    face->hmtx_buf = hz_ft_load_snft_table(ft_face, HZ_TAG('h','m','t','x'));
//
//    face->ascender = ft_face->ascender;
//    face->descender = ft_face->descender;
//    face->linegap = ft_face->size->metrics.height;//ft_face->height;
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