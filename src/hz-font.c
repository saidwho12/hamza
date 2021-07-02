#include "hz-font.h"

struct hz_font_t {
    hz_face_t *face;

    int32_t x_scale;
    int32_t y_scale;
    int64_t x_mult;
    int64_t y_mult;
    uint32_t x_ppem;
    uint32_t y_ppem;

    float ptem;
};

hz_font_t *
hz_font_create()
{
    hz_font_t *font = malloc(sizeof(hz_font_t));
    font->face = NULL;
    font->x_ppem = 1000;
    font->y_ppem = 1000;
    font->ptem = 12.0f;
    font->x_scale = 0;
    font->y_scale = 0;
    return font;
}

void
hz_font_destroy(hz_font_t *font)
{
    free(font);
}

void
hz_font_set_face(hz_font_t *font, hz_face_t *face)
{
    font->face = face;
}

float
hz_font_em_fscale_x(hz_font_t *font, int16_t v)
{
    return em_fscale(font->face, v, font->x_scale);
}

float
hz_font_em_fscale_y(hz_font_t *font, int16_t v)
{
    return em_fscale(font->face, v, font->y_scale);
}

hz_face_t *
hz_font_get_face(hz_font_t *font)
{
    return font->face;
}
