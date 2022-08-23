#include "hz-glyph-cache.h"

hz_glyph_shape_t *
hz_stbtt_get_glyph_shape(stbtt_fontinfo *fontinfo, hz_index_t glyph_index)
{
    if (stbtt_IsGlyphEmpty(fontinfo, glyph_index)) {
        return NULL;
    }

    hz_glyph_shape_t *shape = malloc(sizeof(*shape));

    stbtt_vertex *vertices;
    shape->vertexCount = stbtt_GetGlyphShape(fontinfo, glyph_index, &vertices);
    shape->vertices = malloc(sizeof(hz_vertex_t) * shape->vertexCount);

    float scale = stbtt_ScaleForPixelHeight(fontinfo, 300.);

    for (int i = 0; i < shape->vertexCount; ++i) {
        hz_vertex_t v;
        // v.padding = vertices[i].padding;
        v.x = (float)vertices[i].x * scale;
        v.y = (float)vertices[i].y * scale;
        v.cx = (float)vertices[i].cx * scale;
        v.cy = (float)vertices[i].cy * scale;
        v.cx1 = (float)vertices[i].cx1 * scale;
        v.cy1 = (float)vertices[i].cy1 * scale;
        v.type = vertices[i].type;
        shape->vertices[i] = v;
    }

    //printf("vertexCount: %d\n", shape->vertexCount);

    stbtt_FreeShape(fontinfo, vertices);

    return shape;
}

void hz_glyph_shape_destroy(hz_glyph_shape_t *shape)
{
    free(shape->vertices);
    free(shape);
}