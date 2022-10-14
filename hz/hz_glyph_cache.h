#ifndef HZ_GLYPH_CACHE_H
#define HZ_GLYPH_CACHE_H

#include "hz.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int width;
    int height;
    int cols;
    int rows;
    float max_sdf_distance;
    float padd;
    int oversample;
} hz_glyph_cache_opts_t;

typedef struct {
    int x, y, w, h;
} hz_rect_t;

HZDECL void hz_glyph_cache_opts_create(hz_glyph_cache_opts_t *opts);

HZDECL hz_rect_t hz_glyph_cache_compute_cell_rect(hz_glyph_cache_opts_t *opts, int cell);

typedef enum {
    HZ_VERTEX_TYPE_MOVETO = 1,
    HZ_VERTEX_TYPE_LINE,
    HZ_VERTEX_TYPE_QUADRATIC_BEZIER,
    HZ_VERTEX_TYPE_CUBIC_BEZIER
} hz_vertex_type_t;

typedef struct {
    float x, y;
} hz_vec2_t;

HZDECL hz_vec2_t hz_vec2(float a, float b);

// curve vertex could be of a line, quadratic bezier curve or cubic bezier curve.
// c1,c2 are the optional control points. 
typedef struct {
    hz_vec2_t v1,v2,c1,c2;
    hz_vertex_type_t type; // hz_vertex_type_t
} hz_bezier_vertex_t;

typedef struct {
    hz_vec2_t pos;
    hz_vec2_t uv;
} hz_stencil_vertex_t;

typedef struct {
    int start, curve_count;
    int winding;
    hz_vec2_t pos;
} hz_contour_t;

typedef struct {
    int vertex_count;
    hz_bezier_vertex_t *vertices;
    int contour_count;
    hz_contour_t *contours;
} hz_bezier_shape_t;

typedef struct {
    float max_sdf_distance;
} hz_sdf_variables_t;

HZDECL hz_bezier_shape_t *
hz_stbtt_get_glyph_shape(stbtt_fontinfo *fontinfo, hz_index_t glyph_index);

HZDECL void
hz_glyph_shape_destroy(hz_bezier_shape_t *shape);

HZDECL void
hz_compute_shape_mask_triangles(hz_bezier_shape_t *shape);

#ifdef __cplusplus
}
#endif

#endif // HZ_GLYPH_CACHE_H