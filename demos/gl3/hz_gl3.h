#ifndef HZ_GL3_H
#define HZ_GL3_H

#include "../../hz/hz.h"
#include "hz_gl3_glsl_shaders.h"

#include <glad/glad.h>

typedef struct {
    hz_mat4 view_matrix;
    float max_sdf_distance;
} hz_gl3_ubo_data_t;

struct hz_texture {
    union {
        GLuint handle;
        void *ptr;
    } handle;
};

typedef void (*hz_gl3_render_fn_t)(struct hz_texture glyphs_table_tex, GLuint glyphs_vao, size_t instance_count);

typedef struct {
    // fast sdf gpu generation with loop-blinn masking
    GLuint sdf_texture;
    GLuint stencil_rbo;
    GLuint sdf_data, stencil_buf;
    GLuint fbo;
    int mipmap_levels;

    GLuint curve_to_sdf_program;
    GLuint stencil_kokojima_prog;
    GLuint fs_triangle_prog;
    GLuint char_quad_shader;

    GLuint glyphs_vao, glyphs_vbo;

    GLuint ubo_handle;
    hz_gl3_ubo_data_t ubo_data;

    GLuint slots_ubo_handle;
    hz_glyph_cache_opts_t opts;
} hz_renderer_gl3_t;

int hz_gl3_device_init(hz_renderer_gl3_t *g, hz_glyph_cache_opts_t *opts);
void hz_gl3_render_frame(hz_context_t *ctx, hz_renderer_gl3_t *g);

#endif // HZ_GL3_H