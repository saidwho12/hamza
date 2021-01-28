#ifndef HM_FT_H
#define HM_FT_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OPENTYPE_VALIDATE_H
#include FT_GLYPH_H
#include FT_TRUETYPE_TABLES_H

#include "hm.h"

#ifdef __cplusplus
extern "C" {
#endif

static hm_face_t *
hm_ft_face_create(FT_Face ft_face) {
    hm_face_t *face = (hm_face_t *) HM_MALLOC(sizeof(hm_face_t));
    //face->handle = (mk_rawptr)malloc(sizeof(FT_Face));
    //memcpy(face->handle, &ft_face, sizeof(FT_Face));
    face->cmap_buf.data = NULL;
    face->cmap_buf.len = 0;

    FT_Bytes BASE = NULL;
    FT_Bytes GDEF = NULL;
    FT_Bytes GPOS = NULL;
    FT_Bytes GSUB = NULL;
    FT_Bytes JSTF = NULL;

    FT_Error errorValidate = FT_OpenType_Validate(ft_face, FT_VALIDATE_OT, &BASE, &GDEF, &GPOS, &GSUB, &JSTF);
    if (errorValidate)
        HM_LOG("%s\n", "Couldn't validate opentype datas");

    if (GSUB == NULL)
        HM_LOG("%s\n", "Failed to load GSUB table.");

    face->gsub_table = (uint8_t *) GSUB;

    {
        /* Load cmap table into buffer */
        FT_ULong tag = FT_MAKE_TAG('c','m','a','p');

        FT_Load_Sfnt_Table(ft_face, tag, 0, NULL, &face->cmap_buf.len);

        face->cmap_buf.data = (uint8_t *) HM_MALLOC(face->cmap_buf.len);

        FT_Load_Sfnt_Table(ft_face, tag, 0, face->cmap_buf.data, &face->cmap_buf.len);
    }

    return face;
}

static void
hm_ft_face_destroy(hm_face_t *face) {
    FT_OpenType_Free((FT_Face) face->handle, (FT_Bytes) face->base_table);
    FT_OpenType_Free((FT_Face) face->handle, (FT_Bytes) face->gdef_table);
    FT_OpenType_Free((FT_Face) face->handle, (FT_Bytes) face->gpos_table);
    FT_OpenType_Free((FT_Face) face->handle, (FT_Bytes) face->gsub_table);
    FT_OpenType_Free((FT_Face) face->handle, (FT_Bytes) face->jstf_table);
}


#ifdef __cplusplus
}
#endif

#endif /* HM_FT_H */