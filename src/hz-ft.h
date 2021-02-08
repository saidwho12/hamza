#ifndef HZ_FT_H
#define HZ_FT_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OPENTYPE_VALIDATE_H
#include FT_GLYPH_H
#include FT_TRUETYPE_TABLES_H

#include "hz.h"

#ifdef __cplusplus
extern "C" {
#endif

static hz_buf_t*
hz_ft_load_snft_table(FT_Face face, hz_tag tag)
{
    hz_buf_t *buf = hz_buf_create();
    size_t size = 0;
    FT_Load_Sfnt_Table(face, tag, 0, NULL, &size);
    hz_buf_resize(buf, size);
    FT_Load_Sfnt_Table(face, tag, 0, hz_buf_data(buf), &size);
    return buf;
}


static hz_face_t *
hz_ft_face_create(FT_Face ft_face) {
    hz_face_t *face = (hz_face_t *) HZ_MALLOC(sizeof(hz_face_t));

    FT_Bytes BASE = NULL;
    FT_Bytes GDEF = NULL;
    FT_Bytes GPOS = NULL;
    FT_Bytes GSUB = NULL;
    FT_Bytes JSTF = NULL;

    FT_Error errorValidate = FT_OpenType_Validate(ft_face, FT_VALIDATE_OT, &BASE, &GDEF, &GPOS, &GSUB, &JSTF);
    if (errorValidate)
        HZ_LOG("%s\n", "Couldn't validate opentype datas");

    if (GSUB == NULL)
        HZ_LOG("%s\n", "Failed to load GSUB table.");

    face->gsub_table = (hz_byte *) GSUB;
    face->gpos_table = (hz_byte *) GPOS;
    face->gdef_table = (hz_byte *) GDEF;

    /* load cmap table into buffer */
    face->cmap_buf = hz_ft_load_snft_table(ft_face, HZ_TAG('c','m','a','p'));

    /* load glyf table into buffer */
    face->glyf_buf = hz_ft_load_snft_table(ft_face, HZ_TAG('g','l','y','f'));

    /* load hmtx table into buffer */
    face->hmtx_buf = hz_ft_load_snft_table(ft_face, HZ_TAG('h','m','t','x'));

    {
        /* load few variables from maxp table */
        hz_buf_t *maxp_buf = hz_ft_load_snft_table(ft_face, HZ_TAG('m','a','x','p'));
        hz_decode_maxp_table(face, maxp_buf);
        hz_buf_destroy(maxp_buf);
    }

    {
        /* load few variables from hhea table */
        hz_buf_t *hhea_buf = hz_ft_load_snft_table(ft_face, HZ_TAG('h','h','e','a'));
        hz_decode_hhea_table(face, hhea_buf);
        hz_buf_destroy(hhea_buf);
    }

    return face;
}

static void
hz_ft_face_destroy(hz_face_t *face) {
    FT_OpenType_Free((FT_Face) face->handle, (FT_Bytes) face->base_table);
    FT_OpenType_Free((FT_Face) face->handle, (FT_Bytes) face->gdef_table);
    FT_OpenType_Free((FT_Face) face->handle, (FT_Bytes) face->gpos_table);
    FT_OpenType_Free((FT_Face) face->handle, (FT_Bytes) face->gsub_table);
    FT_OpenType_Free((FT_Face) face->handle, (FT_Bytes) face->jstf_table);
}


#ifdef __cplusplus
}
#endif

#endif /* HZ_FT_H */