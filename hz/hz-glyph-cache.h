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

typedef struct {
    float x, y;
} hz_vec2_t;

// curve vertex could be of a line, quadratic bezier curve or cubic bezier curve.
// c1,c2 are the optional control points. 
typedef struct {
    hz_vec2_t v1,v2,c1,c2;
    hz_vertex_type_t type; // hz_vertex_type_t
} hz_bezier_vertex_t;

typedef struct {
    int vertex_count;
    hz_bezier_vertex_t *vertices;
} hz_glyph_shape_t;

typedef struct {
    float max_sdf_distance;
} hz_sdf_variables_t;

HZDECL hz_glyph_shape_t *
hz_stbtt_get_glyph_shape(stbtt_fontinfo *fontinfo, hz_index_t glyph_index);

HZDECL void hz_glyph_shape_destroy(hz_glyph_shape_t *shape);

#ifdef __cplusplus
}
#endif

#endif // HZ_GLYPH_CACHE_H