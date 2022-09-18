#include "hz-glyph-cache.h"

hz_glyph_shape_t *
hz_stbtt_get_glyph_shape(stbtt_fontinfo *fontinfo, hz_index_t glyph_index)
{
    if (stbtt_IsGlyphEmpty(fontinfo, glyph_index)) {
        return NULL;
    }

    hz_glyph_shape_t *shape = malloc(sizeof(*shape));
    hz_vector(hz_bezier_vertex_t) bezier_result = NULL;

    stbtt_vertex *vertices;
    size_t nverts = stbtt_GetGlyphShape(fontinfo, glyph_index, &vertices);

    //float scale = stbtt_ScaleForPixelHeight(fontinfo, 300.);

    hz_vec2_t pos = {0};
    for (int i = 0; i < nverts; ++i) {
        switch (vertices[i].type) {
            case HZ_VERTEX_TYPE_MOVETO: { // moveto
                pos.x = vertices[i].x;
                pos.y = vertices[i].y;
                break;
            }

            case HZ_VERTEX_TYPE_LINE: { // line
                hz_bezier_vertex_t v;
                v.type = vertices[i].type;
                v.v1 = pos;
                v.v2.x = vertices[i].x;
                v.v2.y = vertices[i].y;
                hz_vector_push_back(bezier_result, v);
                pos.x = vertices[i].x;
                pos.y = vertices[i].y;
                break;
            }
            case HZ_VERTEX_TYPE_QUADRATIC_BEZIER: { // quadratic bezier
                hz_bezier_vertex_t v;
                v.type = vertices[i].type;
                v.v1 = pos;
                v.v2.x = vertices[i].x;
                v.v2.y = vertices[i].y;
                v.c1.x = vertices[i].cx;
                v.c1.y = vertices[i].cy;
                hz_vector_push_back(bezier_result, v);
                pos.x = vertices[i].x;
                pos.y = vertices[i].y;
                break;
            }
            case HZ_VERTEX_TYPE_CUBIC_BEZIER: { // cubic bezier
                hz_bezier_vertex_t v;
                v.type = vertices[i].type;
                v.v1 = pos;
                v.v2.x = vertices[i].x;
                v.v2.y = vertices[i].y;
                v.c1.x = vertices[i].cx;
                v.c1.y = vertices[i].cy;
                v.c2.x = vertices[i].cx1;
                v.c2.y = vertices[i].cy1;
                hz_vector_push_back(bezier_result, v);
                pos.x = vertices[i].x;
                pos.y = vertices[i].y;
                break;
            }
        }
    }

    shape->vertices = bezier_result;
    shape->vertex_count = hz_vector_size(bezier_result);
    printf("vertex_count: %d\n", shape->vertex_count);
    stbtt_FreeShape(fontinfo, vertices);

    return shape;
}

void hz_glyph_shape_destroy(hz_glyph_shape_t *shape)
{
    hz_vector_destroy(shape->vertices);
    free(shape);
}