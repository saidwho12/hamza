#ifndef HZ_GLYPH_CACHE_H
#define HZ_GLYPH_CACHE_H

#include "hz.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hz_glyph_cache_config_t {
    int width;
    int height;
    int grid_divisions;
    float distance_scale;
    float margin;
} hz_glyph_cache_config_t;

typedef enum {
    HZ_VERTEX_TYPE_MOVETO = 1,
    HZ_VERTEX_TYPE_LINE,
    HZ_VERTEX_TYPE_QUADRATIC_BEZIER,
    HZ_VERTEX_TYPE_CUBIC_BEZIER
} hz_vertex_type_t;

// curve vertex could be of a line, quadratic bezier curve or cubic bezier curve.
// cx,cy,cx1,cy1 are the optional control points. 
typedef struct {
    float x,y;
    float cx,cy;
    float cx1,cy1;
    int type; // hz_vertex_type_t
} hz_vertex_t;

typedef struct {
    int vertexCount;
    hz_vertex_t *vertices;
} hz_glyph_shape_t;

HZDECL hz_glyph_shape_t *
hz_stbtt_get_glyph_shape(stbtt_fontinfo *fontinfo, hz_index_t glyph_index);

HZDECL void hz_glyph_shape_destroy(hz_glyph_shape_t *shape);

#ifdef __cplusplus
}
#endif

#endif // HZ_GLYPH_CACHE_H