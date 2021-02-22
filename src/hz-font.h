#ifndef HZ_FONT_H
#define HZ_FONT_H

#include "hz-base.h"
#include "hz-face.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hz_font_t hz_font_t;

hz_font_t *
hz_font_create();

void
hz_font_destroy(hz_font_t *font);

hz_face_t *
hz_font_get_face(hz_font_t *font);

void
hz_font_set_face(hz_font_t *font, hz_face_t *face);

float
hz_font_em_fscale_x(hz_font_t *font, int16_t v);

float
hz_font_em_fscale_y(hz_font_t *font, int16_t v);

#ifdef __cplusplus
}
#endif

#endif /* HZ_FONT_H */
