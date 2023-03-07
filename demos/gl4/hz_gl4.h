#ifndef HZ_GL4_H
#define HZ_GL4_H

#ifdef __cplusplus
extern "C" {
#endif

#include <glad/glad.h>



typedef struct {
    float f32[3][3];
} hz_mat3;

typedef struct {
    float f32[4][4];
} hz_mat4;

HZ_DECL hz_mat3 hz_mat3_identity(void)
{
    return (hz_mat3){1.f,0.f,0.f, 0.f,1.f,0.f, 0.f,0.f,1.f};
}

HZ_DECL hz_mat3 hz_mat3_translate(float tx, float ty) {
    hz_mat3 m = hz_mat3_identity();
    m.f32[0][2] = tx;
    m.f32[1][2] = ty;
    return m;
}

HZ_DECL hz_mat4 hz_mat4_identity()
{
    return (hz_mat4){
        1.0f,0.0f,0.0f,0.0f,
        0.0f,1.0f,0.0f,0.0f,
        0.0f,0.0f,1.0f,0.0f,
        0.0f,0.0f,0.0f,1.0f};
}

HZ_DECL hz_mat4 hz_mat4_scale(float sx, float sy) {
    hz_mat4 m = hz_mat4_identity();
    m.f32[0][0] = sx;
    m.f32[1][1] = sy;
    return m;
}

HZ_DECL void hz_mat3_mult(hz_mat3 *m1, hz_mat3 *m2, hz_mat3 *m_out)
{
    m_out->f32[0][0] = m1->f32[0][0]*m2->f32[0][0];
    m_out->f32[0][1] = m1->f32[0][1]*m2->f32[1][0];
    m_out->f32[0][2] = m1->f32[0][2]*m2->f32[2][0];
    
    m_out->f32[1][0] = m1->f32[1][0]*m2->f32[0][1];
    m_out->f32[1][1] = m1->f32[1][1]*m2->f32[1][1];
    m_out->f32[1][2] = m1->f32[1][2]*m2->f32[2][1];

    m_out->f32[2][0] = m1->f32[2][0]*m2->f32[0][2];
    m_out->f32[2][1] = m1->f32[2][1]*m2->f32[1][2];
    m_out->f32[2][2] = m1->f32[2][2]*m2->f32[2][2];
}

HZ_DECL hz_vec2 hz_mat3_mult_vec2(hz_mat3 *m, hz_vec2 v)
{
    hz_vec2 result;
    hz_vec3 v3 = (hz_vec3){v.x,v.y,1.0f};
    result.x = v3.x * m->f32[0][0] + v3.y * m->f32[0][1] + v3.z * m->f32[0][2];
    result.y = v3.x * m->f32[1][0] + v3.y * m->f32[1][1] + v3.z * m->f32[1][2];
    // result.z = v3.x * m->f32[2][0] + v3.y * m->f32[2][1] + v3.z * m->f32[2][2];
    return result;
}

HZ_DECL hz_mat4 hz_mat4_ortho(float l, float r, float b, float t) {
    hz_mat4 m = hz_mat4_identity();
    m.f32[0][0] = 2.0f / (r-l);
    m.f32[1][1] = 2.0f / (t-b);
    m.f32[0][3] = - (r+l)/(r-l);
    m.f32[1][3] = - (t+b)/(t-b);
    return m;
}

// hz_mat3x3f hz_trs_transform(hz_vec2f pos, hz_vec2f scale, float rot)
// {
//     hz_mat3f m;
// }

typedef struct {
    hz_mat3 trs;
    hz_font_t *font;
    hz_index_t glyph_id;
} hz_gl4_table_glyph_t;

typedef struct {
    // hz_mat4 view_matrix;
    float max_sdf_distance;
} hz_gl4_view_ubo_t;

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

    GLuint ubo_handle;
    hz_gl4_view_ubo_t ubo_data;

    hz_sdf_cache_opts_t opts;
} hz_gl4_device_t;

int hz_gl4_device_init(hz_gl4_device_t *dev, hz_sdf_cache_opts_t *opts);

#ifdef HZ_GL4_IMPLEMENTATION

#include "hz_gl4_curve_to_sdf_vertex_shader.h"
#include "hz_gl4_curve_to_sdf_fragment_shader.h"
#include "hz_gl4_stencil_kokojima_vertex_shader.h"
#include "hz_gl4_stencil_kokojima_fragment_shader.h"
#include "hz_gl4_fs_triangle_vertex_shader.h"
#include "hz_gl4_fs_triangle_fragment_shader.h"

struct hz_gl4_curve_vertex {
    float v[2];
};

GLuint hz_gl4_create_shader(const char *code, GLenum type)
{
    GLuint shader = glCreateShader(type);
    GLint success;

    glShaderSource(shader, 1, (const GLchar *const*)&code, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, (int *)&success);

    if (success != GL_TRUE) {
        hz_logln(HZ_LOG_FATAL, "failed to create shader!");
    }

    return shader;
}

GLuint hz_gl4_create_program(GLuint vert_shader, GLuint frag_shader)
{
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert_shader);
    glAttachShader(prog, frag_shader);
    glLinkProgram(prog);

    GLint success;
    glGetProgramiv(prog, GL_LINK_STATUS, (int *)&success);

    if (success != GL_TRUE) {
        hz_logln(HZ_LOG_FATAL, "ERROR!!!");
    }

    hz_logln(HZ_LOG_INFO, "created shader program!");

    return prog;
}

#define HZ_ARG_ARRAY(x) x, (sizeof(x)/sizeof((x)[0]))

int hz_gl4_device_init(hz_gl4_device_t *dev, hz_sdf_cache_opts_t *opts)
{
    HZ_ASSERT(opts->width == opts->height);
    HZ_ASSERT(hz_is_power_of_two(opts->width));

    {
        GLuint vertex_shader = hz_gl4_create_shader(hz_gl4_curve_to_sdf_vertex_shader, GL_VERTEX_SHADER);
        GLuint fragment_shader = hz_gl4_create_shader(hz_gl4_curve_to_sdf_fragment_shader, GL_FRAGMENT_SHADER);
        dev->curve_to_sdf_program = hz_gl4_create_program(vertex_shader, fragment_shader);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }
    
    {
        GLuint vertex_shader = hz_gl4_create_shader(hz_gl4_stencil_kokojima_vertex_shader, GL_VERTEX_SHADER);
        GLuint fragment_shader = hz_gl4_create_shader(hz_gl4_stencil_kokojima_fragment_shader, GL_FRAGMENT_SHADER);
        dev->stencil_kokojima_prog = hz_gl4_create_program(vertex_shader, fragment_shader);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }

    {
        GLuint vertex_shader = hz_gl4_create_shader(hz_gl4_fs_triangle_vertex_shader, GL_VERTEX_SHADER);
        GLuint fragment_shader = hz_gl4_create_shader(hz_gl4_fs_triangle_fragment_shader, GL_FRAGMENT_SHADER);
        dev->fs_triangle_prog = hz_gl4_create_program(vertex_shader, fragment_shader);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }

    // compute ubo and upload
    glGenBuffers(1, &dev->ubo_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, dev->ubo_handle);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(hz_gl4_view_ubo_t), &dev->ubo_data, GL_STREAM_DRAW);

    dev->opts = *opts;
    glGenFramebuffers(1, &dev->fbo);
    
    // create destination texture for storing the signed distance field
    glGenTextures(1, &dev->sdf_texture);
    glBindTexture(GL_TEXTURE_2D, dev->sdf_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, opts->width, opts->height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D,0);//unbind

    // create depth/stencil buffer as an rbo
    glGenRenderbuffers(1, &dev->stencil_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, dev->stencil_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, opts->width, opts->height);

    // attach both targets to the fbo 
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dev->fbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dev->sdf_texture, 0);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, dev->stencil_rbo);

    if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "Framebuffer is incomplete!\n");
        return -1;
    }

    // unbind fbo
    glBindRenderbuffer(GL_RENDERBUFFER,0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);

    return 0;
}

void hz_gl4_bind_fb(hz_gl4_device_t *dev) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dev->fbo);
}

void hz_gl4_unbind_fb() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void hz_gl4_generate_glyph_sdf(hz_gl4_device_t *dev,
                               hz_font_t *font,
                               hz_vec2 pos, hz_vec2 scale,
                               hz_index_t glyph_id)
{
    hz_shape_draw_data_t draw_data;
    hz_zero_struct(draw_data);
    hz_vector(hz_stencil_vertex_t) mask_verts = NULL;

    hz_face_get_glyph_shape(hz_font_get_face(font), &draw_data, glyph_id);

    // transform shape vertices
    // for (int i = 0; i < hz_vector_size(draw_data.verts); ++i) {
    //     hz_bezier_vertex_t *v = &draw_data.verts[i];
    //     v->c1 = hz_mat3_mult_vec2(&tf,v->c1);
    //     v->c2 = hz_mat3_mult_vec2(&tf,v->c2);
    //     v->v1 = hz_mat3_mult_vec2(&tf,v->v1);
    //     v->v2 = hz_mat3_mult_vec2(&tf,v->v2);
    // }
    // for (int i = 0; i < hz_vector_size(draw_data.contours); ++i) {
    //     hz_contour_t *contour = &draw_data.contours[i];
    //     contour->pos = hz_mat3_mult_vec2(&tf,contour->pos);
    // }

    for (int i = 0; i < hz_vector_size(draw_data.contours); ++i) {
        hz_contour_t *contour = &draw_data.contours[i];
        for (int j = 0; j < contour->curve_count; ++j) {
            hz_bezier_vertex_t seg = draw_data.verts[contour->first_curve+j];
            hz_stencil_vertex_t f1 = (hz_stencil_vertex_t){contour->origin,(hz_vec2){0.0f,1.0f}};
            hz_stencil_vertex_t f2 = (hz_stencil_vertex_t){seg.v2,(hz_vec2){0.0f,1.0f}};
            hz_stencil_vertex_t f3 = (hz_stencil_vertex_t){seg.v1,(hz_vec2){0.0f,1.0f}};
            hz_vector_push_back(mask_verts, f1);
            hz_vector_push_back(mask_verts, f2);
            hz_vector_push_back(mask_verts, f3);
            if (seg.type == HZ_VERTEX_TYPE_QUADRATIC_BEZIER) {// quadratic curve
                hz_stencil_vertex_t c1 = (hz_stencil_vertex_t){seg.v1,(hz_vec2){-1.0f, 1.0f}};
                hz_stencil_vertex_t c2 = (hz_stencil_vertex_t){seg.v2,(hz_vec2){1.0f, 1.0f}};
                hz_stencil_vertex_t c3 = (hz_stencil_vertex_t){seg.c1,(hz_vec2){0.0f, -1.0f}};
                hz_vector_push_back(mask_verts, c1);
                hz_vector_push_back(mask_verts, c2);
                hz_vector_push_back(mask_verts, c3);
            }
        }
    }

    // create actual curves vertex buffers
    GLuint curves_vao, curves_vbo, mask_vao, mask_vbo;
    size_t curve_draw_count = hz_vector_size(draw_data.verts);
    size_t stencil_draw_count = hz_vector_size(mask_verts);
    
    {
        // create segments VAO and VBO
        glGenVertexArrays(1, &curves_vao);
        glBindVertexArray(curves_vao);

        glGenBuffers(1,&curves_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, curves_vbo);
        glBufferData(GL_ARRAY_BUFFER, curve_draw_count*sizeof(draw_data.verts[0]), draw_data.verts, GL_STATIC_DRAW);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(hz_bezier_vertex_t), (void *)offsetof(hz_bezier_vertex_t,v1));
        glVertexAttribDivisor(0, 1);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(hz_bezier_vertex_t), (void *)offsetof(hz_bezier_vertex_t,v2));
        glVertexAttribDivisor(1, 1);

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(hz_bezier_vertex_t), (void *)offsetof(hz_bezier_vertex_t,c1));
        glVertexAttribDivisor(2, 1);

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(hz_bezier_vertex_t), (void *)offsetof(hz_bezier_vertex_t,c2));
        glVertexAttribDivisor(3, 1);

        glEnableVertexAttribArray(4);
        glVertexAttribIPointer(4, 1, GL_INT, sizeof(hz_bezier_vertex_t), (void *)offsetof(hz_bezier_vertex_t,type));
        glVertexAttribDivisor(4, 1);
        glBindVertexArray(0);
    }

    {
        // Create stencil mask VAO and VBO
        glGenVertexArrays(1,&mask_vao);
        glBindVertexArray(mask_vao);

        glGenBuffers(1,&mask_vbo);

        glBindBuffer(GL_ARRAY_BUFFER, mask_vbo);
        glBufferData(GL_ARRAY_BUFFER, stencil_draw_count*sizeof(mask_verts[0]), mask_verts, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(hz_stencil_vertex_t), (void *)offsetof(hz_stencil_vertex_t,pos));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(hz_stencil_vertex_t), (void *)offsetof(hz_stencil_vertex_t,uv));
        glBindVertexArray(0);
    }

    dev->ubo_data.max_sdf_distance = dev->opts.max_sdf_distance;
    // dev->ubo_data.view_matrix = hz_mat4_ortho(0.0f,dev->opts.width,0.0f,dev->opts.height);

    hz_gl4_bind_fb(dev);
    glViewport(0,0,dev->opts.width, dev->opts.height);
        
    // clear stencil from previous operations
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

    // use the curve to sdf shader program
    glEnable(GL_BLEND);
    glBlendEquation(GL_MAX);
    glBlendFunc(GL_ONE, GL_ONE);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_STENCIL_TEST);
    
    glUseProgram(dev->curve_to_sdf_program);
    glBindVertexArray(curves_vao);
    
    {
        GLuint block_index = glGetUniformBlockIndex( dev->curve_to_sdf_program, "UboData" );
        if (block_index == GL_INVALID_INDEX) {
            fprintf(stderr,"Block index invalid 1!\n");
            exit(-1);
        }
        glBindBuffer(GL_UNIFORM_BUFFER, dev->ubo_handle);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(hz_gl4_view_ubo_t), &dev->ubo_data);
        glBindBufferBase(GL_UNIFORM_BUFFER, block_index, dev->ubo_handle);
    }

    static const uint32_t indices[]={0,1,2,2,1,3};
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices, curve_draw_count);
#if 1
    // kokojima et al. technique used for generating a stencil mask used for inverting distance
    // in the glyph texture. Front-facing triangles increase stencil value and
    // back-facing ones decrease it.
    glUseProgram(dev->stencil_kokojima_prog);
    {
        GLuint block_index = glGetUniformBlockIndex( dev->stencil_kokojima_prog, "UboData" );
        if (block_index == GL_INVALID_INDEX) {
            fprintf(stderr,"Block index invalid 2!\n");
            exit(-1);
        }

        glBindBuffer(GL_UNIFORM_BUFFER, dev->ubo_handle);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(hz_gl4_view_ubo_t), &dev->ubo_data);
        glBindBufferBase(GL_UNIFORM_BUFFER, block_index, dev->ubo_handle);
    }

    glBindVertexArray(mask_vao);
    glEnable(GL_STENCIL_TEST);
    glDisable(GL_CULL_FACE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    glStencilFunc(GL_ALWAYS, 0, 0xff);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_INCR_WRAP, GL_INCR_WRAP);
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_DECR_WRAP, GL_DECR_WRAP);
    glDrawArrays(GL_TRIANGLES, 0, stencil_draw_count);

    // draw full screen triangle and invert internal distanceglEnable( GL_BLEND );
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
    glStencilFunc(GL_NOTEQUAL, 0, 0xff);
    glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glUseProgram(dev->fs_triangle_prog);
    glDrawArrays(GL_TRIANGLES,0,3);
#endif
    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);
    
    hz_gl4_unbind_fb();
    
    //unbind
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    // cleanup
    glDeleteBuffers(1,&curves_vbo);
    glDeleteVertexArrays(1,&curves_vao);

    //delete arr
    hz_vector_destroy(mask_verts);
    hz_shape_draw_data_clear(&draw_data);
}

void hz_gl4_render_frame(hz_context_t *ctx, hz_gl4_device_t *dev)
{
    hz_command_list_t *cmds = &ctx->frame_cmds;
    hz_ht_iter_t it = hz_ht_iter_begin(&cmds->unique_glyph_ht);
    
    while (hz_ht_iter_valid(&it)) {
        // if (!hz_glyph_cache_has_hit(&ctx->glyph_cache, it.key)) {
            hz_font_t *font = ctx->fonts[it.key>>16u];
            hz_index_t glyph_id = it.key & 0xffffu;

            float cell_w = (float)dev->opts.width / (float)dev->opts.x_cells;
            float cell_h = (float)dev->opts.height / (float)dev->opts.y_cells;

            float cell_x = 0;//(float)(i % dev->opts.x_subdiv) * cell_w;
            float cell_y = 0;//(float)(i / dev->opts.y_subdiv) * cell_h;

            float min_dim = MIN(cell_w, cell_h);
            float y_scale = hz_font_scale_for_pixel_height(font, min_dim);
            hz_gl4_generate_glyph_sdf(dev,font,(hz_vec2){cell_x,cell_y},(hz_vec2){y_scale,y_scale},glyph_id);    
        // }

        
        hz_ht_iter_next(&cmds->unique_glyph_ht, &it);
    }
}

#endif // HZ_GL4_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // HZ_GL4_H