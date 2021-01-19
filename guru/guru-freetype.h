#ifndef GURU_FREETYPE_H
#define GURU_FREETYPE_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OPENTYPE_VALIDATE_H
#include FT_GLYPH_H
#include FT_TRUETYPE_TABLES_H

#include "guru.h"

#ifdef __cplusplus
extern "C" {
#endif

static guru_face_t *
guru_ft_face_create(FT_Face ft_face) {
    guru_face_t *face = (guru_face_t *) malloc(sizeof(guru_face_t));
    //face->handle = (mk_rawptr)malloc(sizeof(FT_Face));
    //memcpy(face->handle, &ft_face, sizeof(FT_Face));

    FT_Bytes BASE = NULL;
    FT_Bytes GDEF = NULL;
    FT_Bytes GPOS = NULL;
    FT_Bytes GSUB = NULL;
    FT_Bytes JSTF = NULL;

    FT_Error errorValidate = FT_OpenType_Validate(ft_face, FT_VALIDATE_OT, &BASE, &GDEF, &GPOS, &GSUB, &JSTF);
    if (errorValidate)
        GURU_LOG("%s\n", "Couldn't validate opentype datas");

    if (GSUB == NULL)
        GURU_LOG("%s\n", "Failed to load GSUB table.");

    face->GSUB = (guru_byte *) GSUB;

    {
        /* Load cmap table into buffer */
        FT_ULong tag = FT_MAKE_TAG('c','m','a','p');

        FT_Load_Sfnt_Table(ft_face, tag, 0, NULL, &face->cmap_buf.len);

        face->cmap_buf.data = (uint8_t *) GURU_MALLOC(face->cmap_buf.len);

        FT_Load_Sfnt_Table(ft_face, tag, 0, face->cmap_buf.data, &face->cmap_buf.len);
    }

    return face;
}

static void
guru_ft_face_destroy(guru_face_t *face) {
    FT_OpenType_Free((FT_Face) face->handle, (FT_Bytes) face->BASE);
    FT_OpenType_Free((FT_Face) face->handle, (FT_Bytes) face->GDEF);
    FT_OpenType_Free((FT_Face) face->handle, (FT_Bytes) face->GPOS);
    FT_OpenType_Free((FT_Face) face->handle, (FT_Bytes) face->GSUB);
    FT_OpenType_Free((FT_Face) face->handle, (FT_Bytes) face->JSTF);
}


#ifdef __cplusplus
}
#endif

#endif /* GURU_FREETYPE_H */