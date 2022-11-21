#ifndef HZ_GL4_H
#define HZ_GL4_H

#ifdef __cplusplus
extern "C" {
#endif

#include <glad/glad.h>

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

    hz_sdf_cache_opts_t opts;
} hz_gl4_context_t;

int hz_gl4_context_init(hz_gl4_context_t *ctx, hz_sdf_cache_opts_t *opts);

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

GLuint hz_gl4_create_spirv_shader(const uint8_t *spirv, size_t size, GLenum type)
{
    GLuint shader = glCreateShader(type);
    glShaderBinary(1,&shader,GL_SHADER_BINARY_FORMAT_SPIR_V,spirv,size);
    glSpecializeShader(shader, "main", 0, NULL, NULL);

    GLint ok;
    glGetShaderiv(shader, GL_SPIR_V_BINARY, &ok);

    if (ok != GL_TRUE) {
        hz_logln(HZ_LOG_FATAL, "failed to load spirv binary!");
    }

    return shader;
}

GLuint hz_gl4_create_program(GLuint vert_shader, GLuint frag_shader)
{
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert_shader);
    glAttachShader(prog, frag_shader);
    glLinkProgram(prog);

    GLint status;
    glGetProgramiv(prog, GL_LINK_STATUS, &status);

    if (status != GL_TRUE) {
        hz_logln(HZ_LOG_FATAL, "ERROR!!!");
    }

    hz_logln(HZ_LOG_INFO, "created shader program!");

    return prog;
}

#define HZ_ARG_ARRAY(x) x, (sizeof(x)/sizeof((x)[0]))

int hz_gl4_context_init(hz_gl4_context_t *ctx, hz_sdf_cache_opts_t *opts)
{
    HZ_ASSERT(opts->width == opts->height);
    HZ_ASSERT(is_power_of_two(opts->width));

    {
        GLuint vertex_shader = hz_gl4_create_spirv_shader(HZ_ARG_ARRAY(hz_gl4_curve_to_sdf_vertex_shader), GL_VERTEX_SHADER);
        GLuint fragment_shader = hz_gl4_create_spirv_shader(HZ_ARG_ARRAY(hz_gl4_curve_to_sdf_fragment_shader), GL_FRAGMENT_SHADER);
        ctx->curve_to_sdf_program = hz_gl4_create_program(vertex_shader, fragment_shader);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }

    
    {
        GLuint vertex_shader = hz_gl4_create_spirv_shader(HZ_ARG_ARRAY(hz_gl4_stencil_kokojima_vertex_shader), GL_VERTEX_SHADER);
        GLuint fragment_shader = hz_gl4_create_spirv_shader(HZ_ARG_ARRAY(hz_gl4_stencil_kokojima_fragment_shader), GL_FRAGMENT_SHADER);
        ctx->stencil_kokojima_prog = hz_gl4_create_program(vertex_shader, fragment_shader);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }

    {
        GLuint vertex_shader = hz_gl4_create_spirv_shader(HZ_ARG_ARRAY(hz_gl4_fs_triangle_vertex_shader), GL_VERTEX_SHADER);
        GLuint fragment_shader = hz_gl4_create_spirv_shader(HZ_ARG_ARRAY(hz_gl4_fs_triangle_fragment_shader), GL_FRAGMENT_SHADER);
        ctx->fs_triangle_prog = hz_gl4_create_program(vertex_shader, fragment_shader);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }
    

    ctx->opts = *opts;
    glGenFramebuffers(1, &ctx->fbo);
    
    // create destination texture for storing the signed distance field
    glGenTextures(1, &ctx->sdf_texture);
    glBindTexture(GL_TEXTURE_2D, ctx->sdf_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, opts->width, opts->height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D,0);//unbind

    // create depth/stencil buffer as an rbo
    glGenRenderbuffers(1, &ctx->stencil_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, ctx->stencil_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, opts->width, opts->height);

    // attach both targets to the fbo 
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ctx->fbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ctx->sdf_texture, 0);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ctx->stencil_rbo);

    if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "Framebuffer is incomplete!\n");
    }

    // unbind fbo
    glBindRenderbuffer(GL_RENDERBUFFER,0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
}

typedef struct {
    float elem[3][3];
} hz_mat3f;

// hz_mat3x3f hz_trs_transform(hz_vec2f pos, hz_vec2f scale, float rot)
// {
//     hz_mat3f m;
// }

typedef struct {
    hz_mat3f trs;
    stbtt_fontinfo *fontinfo;
    hz_index_t gid;
} hz_gl4_transformed_glyph_t;

void hz_gl4_bind_framebuffer(hz_gl4_context_t *ctx)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ctx->fbo);
}

void hz_gl4_unbind_framebuffer(hz_gl4_context_t *ctx)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void hz_gl4_generate_glyphs_sdf(hz_gl4_context_t *ctx,
                                hz_gl4_transformed_glyph_t *glyphs,
                                size_t glyph_count)
{
    hz_shape_draw_data_t draw_data;
    hz_zero_struct(draw_data);
    hz_vector(hz_stencil_vertex_t) mask_verts = NULL;

    for (size_t i = 0; i < glyph_count; ++i) {
        hz_stbtt_get_glyph_shape(glyphs[i].fontinfo, &draw_data, glyphs[i].gid);
    }

    for (int i = 0; i < hz_vector_size(draw_data.contours); ++i) {
        hz_contour_t *contour = &draw_data.contours[i];
        for (int j = 0; j < contour->curve_count; ++j) {
            hz_bezier_vertex_t seg = draw_data.verts[contour->start+j];
            hz_stencil_vertex_t f1 = (hz_stencil_vertex_t){contour->pos,hz_vec2(0.0f,1.0f)};
            hz_stencil_vertex_t f2 = (hz_stencil_vertex_t){seg.v2,hz_vec2(0.0f,1.0f)};
            hz_stencil_vertex_t f3 = (hz_stencil_vertex_t){seg.v1,hz_vec2(0.0f,1.0f)};
            hz_vector_push_back(mask_verts, f1);
            hz_vector_push_back(mask_verts, f2);
            hz_vector_push_back(mask_verts, f3);
            if (seg.type == HZ_VERTEX_TYPE_QUADRATIC_BEZIER) {// quadratic curve
                hz_stencil_vertex_t c1 = (hz_stencil_vertex_t){seg.v1,hz_vec2(-1.0f, 1.0f)};
                hz_stencil_vertex_t c2 = (hz_stencil_vertex_t){seg.v2,hz_vec2(1.0f, 1.0f)};
                hz_stencil_vertex_t c3 = (hz_stencil_vertex_t){seg.c1,hz_vec2(0.0f, -1.0f)};
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
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(hz_bezier_vertex_t), offsetof(hz_bezier_vertex_t,v1));
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
    }

    {
        // Create stencil mask VAO and VBO
        glGenVertexArrays(1,&mask_vao);
        glBindVertexArray(mask_vao);

        glGenBuffers(1,&mask_vbo);

        glBindBuffer(GL_ARRAY_BUFFER, mask_vbo);
        glBufferData(GL_ARRAY_BUFFER, hz_vector_size(mask_verts)*sizeof(mask_verts[0]), mask_verts, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(hz_stencil_vertex_t), offsetof(hz_stencil_vertex_t,pos));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(hz_stencil_vertex_t), (void *)offsetof(hz_stencil_vertex_t,uv));
    }

    hz_gl4_bind_framebuffer(ctx);
    glViewport(0,0,ctx->opts.width, ctx->opts.height);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
        
    // clear stencil from previous operations
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

    // use the curve to sdf shader program
    glEnable(GL_BLEND);
    glBlendEquation(GL_MAX);
    glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    
    glUseProgram(ctx->curve_to_sdf_program);
    glBindVertexArray(curves_vao);
    static const unsigned int indices[]={0,1,2,2,1,3};
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices, (GLsizei)curve_draw_count);

    // kokojima et al. technique used for masking and inverting internal distance
    // in the glyph texture using the stencil buffer. Front-facing triangles increase stencil value and
    // back-facing ones decrease it.
    glUseProgram(ctx->stencil_kokojima_prog);
    glBindVertexArray(mask_vao);
    glEnable( GL_STENCIL_TEST );
    glDisable( GL_CULL_FACE );
    glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );

    glStencilFunc( GL_ALWAYS, 0, UINT8_MAX );
    glStencilOpSeparate( GL_FRONT, GL_KEEP, GL_INCR_WRAP, GL_INCR_WRAP );
    glStencilOpSeparate( GL_BACK, GL_KEEP, GL_DECR_WRAP, GL_DECR_WRAP );
    glDrawArrays(GL_TRIANGLES, 0, stencil_draw_count);

    // draw full screen triangle and invert internal distanceglEnable( GL_BLEND );
    glBlendEquation( GL_FUNC_ADD );
    glBlendFunc( GL_ONE_MINUS_DST_COLOR, GL_ZERO );
    glStencilFunc( GL_NOTEQUAL, 0, UINT8_MAX );
    glStencilOp( GL_ZERO, GL_ZERO, GL_ZERO );
    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glUseProgram(ctx->fs_triangle_prog);
    glDrawArrays(GL_TRIANGLES,0,3);

    glDisable( GL_BLEND );
    glDisable( GL_STENCIL_TEST );
    
    hz_gl4_unbind_framebuffer(ctx);
    
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

// void hz_gl4_render_frame(hz_gl4_context_t *ctx, hz_draw_list_t *drawlist)
// {
//     drawlist->cmds[]
// }

#endif // HZ_GL4_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // HZ_GL4_H