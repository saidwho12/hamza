#include "hz_gl3.h"

struct hz_gl3_curve_vertex {
    float v[2];
};

GLuint hz_gl3_create_shader(const char *code, GLenum type)
{
    GLuint shader = glCreateShader(type);
    GLint success;

    glShaderSource(shader, 1, (const GLchar *const*)&code, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, (int *)&success);

    if (success != GL_TRUE) {
        char infoLog[1024];
        glGetShaderInfoLog(shader, 1024, NULL,(GLchar*)infoLog);
        printf("failed to create shader!\n%s",infoLog);
    }

    return shader;
}

GLuint hz_gl3_create_program(GLuint vert_shader, GLuint frag_shader)
{
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert_shader);
    glAttachShader(prog, frag_shader);
    glLinkProgram(prog);

    GLint success;
    glGetProgramiv(prog, GL_LINK_STATUS, (int *)&success);

    if (success != GL_TRUE) {
        char infoLog[1024];
        glGetProgramInfoLog(prog, 1024, NULL,(GLchar*)infoLog);
        printf("failed to create program!\n%s",infoLog);
    }

    return prog;
}

#define HZ_ARG_ARRAY(x) x, (sizeof(x)/sizeof((x)[0]))

void hz_gl3_device_deinit(hz_renderer_gl3_t *g)
{
    glDeleteBuffers(1,&g->glyphs_vbo);
    glDeleteVertexArrays(1,&g->glyphs_vao);
}

int hz_gl3_device_init(hz_renderer_gl3_t *g, hz_glyph_cache_opts_t *opts)
{
    HZ_ASSERT(opts->width == opts->height);
    HZ_ASSERT(HZ_ISP2(opts->width));

    glGenVertexArrays(1, &g->glyphs_vao);
    glBindVertexArray(g->glyphs_vao);
    glGenBuffers(1, &g->glyphs_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, g->glyphs_vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    {
        GLuint vertex_shader = hz_gl3_create_shader(hz_gl3_curve_to_sdf_vsh, GL_VERTEX_SHADER);
        GLuint fragment_shader = hz_gl3_create_shader(hz_gl3_curve_to_sdf_fsh, GL_FRAGMENT_SHADER);
        g->curve_to_sdf_program = hz_gl3_create_program(vertex_shader, fragment_shader);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }

    {
        GLuint vertex_shader = hz_gl3_create_shader(hz_gl3_char_quad_vsh, GL_VERTEX_SHADER);
        GLuint fragment_shader = hz_gl3_create_shader(hz_gl3_char_quad_fsh, GL_FRAGMENT_SHADER);
        g->char_quad_shader = hz_gl3_create_program(vertex_shader, fragment_shader);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }
    
    {
        GLuint vertex_shader = hz_gl3_create_shader(hz_gl3_stencil_kokojima_vsh, GL_VERTEX_SHADER);
        GLuint fragment_shader = hz_gl3_create_shader(hz_gl3_stencil_kokojima_fsh, GL_FRAGMENT_SHADER);
        g->stencil_kokojima_prog = hz_gl3_create_program(vertex_shader, fragment_shader);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }

    {
        GLuint vertex_shader = hz_gl3_create_shader(hz_gl3_fs_triangle_vsh, GL_VERTEX_SHADER);
        GLuint fragment_shader = hz_gl3_create_shader(hz_gl3_fs_triangle_fsh, GL_FRAGMENT_SHADER);
        g->fs_triangle_prog = hz_gl3_create_program(vertex_shader, fragment_shader);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }

    // compute ubo and upload
    glGenBuffers(1, &g->ubo_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, g->ubo_handle);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(hz_gl3_ubo_data_t), &g->ubo_data, GL_STREAM_DRAW);
    

    glGenBuffers(1, &g->slots_ubo_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, g->slots_ubo_handle);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(struct hz_cache_slot_t)*opts->x_cells*opts->y_cells, NULL, GL_STREAM_DRAW);

    g->opts = *opts;
    glGenFramebuffers(1, &g->fbo);
    
    // create destination texture for storing the signed distance field
    glGenTextures(1, &g->sdf_texture);
    glBindTexture(GL_TEXTURE_2D, g->sdf_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, opts->width, opts->height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D,0);//unbind

    // create depth/stencil buffer as an rbo
    glGenRenderbuffers(1, &g->stencil_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, g->stencil_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, opts->width, opts->height);

    // attach both targets to the fbo 
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, g->fbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g->sdf_texture, 0);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, g->stencil_rbo);

    if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "Framebuffer is incomplete!\n");
        return -1;
    }

    // unbind fbo
    glBindRenderbuffer(GL_RENDERBUFFER,0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);

    return 0;
}

void hz_gl3_bind_fb(hz_renderer_gl3_t *g) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, g->fbo);
}

void hz_gl3_unbind_fb() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
}

void hz_gl3_refill_cache(hz_context_t *ctx, hz_renderer_gl3_t *g, uint16_t unavail_ids_sz, hz_cache_id_t *unavail_ids)
{
    /*
     * @NOTE: We should adjust the LRU cache API to move a lot of this logic insize hz.c.
     * We want a clean interface and the implementation should only have to define a single slot fill function and pass it in
     * to the context. Adding batch rendering into the atlas texture would also make this much more efficient but it requires some
     * planning. The goal is always to maintain simplicity.
     */
    uint8_t buffer[1024];
    hz_memory_arena_t arena = hz_memory_arena_create(buffer, sizeof(buffer));
    uint16_t *open_slots = hz_memory_arena_alloc(&arena, sizeof(uint16_t)*unavail_ids_sz);
    hz_glyph_cache_t *lru = hz_context_get_lru(ctx);
    hz_lru_cache_replace_slots(lru, unavail_ids_sz, open_slots);
    
    hz_vector(hz_vec4) clear_rects = NULL;
    hz_vector(hz_bezier_vertex_t) bezier_verts = NULL;
    hz_vector(hz_stencil_vertex_t) mask_verts = NULL;
    size_t bezier_vertex_count = 0;

    for (uint16_t i = 0; i < unavail_ids_sz; ++i) {
        hz_cache_id_t lru_id = unavail_ids[i];
        uint16_t slot_index = open_slots[i];

        // Get face needed to render this slot's glyph
        hz_face_t *face = hz_context_get_face(ctx, lru_id.font_id);

        // Get Atlas bounds
        float cellw = (float)g->opts.width / (float)g->opts.x_cells;
        float cellh = (float)g->opts.height / (float)g->opts.y_cells;
        float cellx = (float)(slot_index % g->opts.x_cells) * cellw;
        float celly = (float)(slot_index / g->opts.y_cells) * cellh;
        // Add to clear rects
        hz_vec4 r = (hz_vec4){cellx, celly, cellw, cellh};
        hz_vector_push_back(clear_rects, r);

        float scale = hz_face_scale_for_pixel_h(face, cellh);
        hz_bbox_t bounds;
        hz_face_get_glyph_box(face, lru_id.glyph_id, &bounds);

        float atlas_x0 = cellx + cellw/2 - (bounds.x1-bounds.x0)/2*scale;
        float atlas_y0 = celly + cellh/2 - (bounds.y1-bounds.y0)/2*scale;
        float atlas_x1 = atlas_x0 + (bounds.x1-bounds.x0)*scale;
        float atlas_y1 = atlas_y0 + (bounds.y1-bounds.y0)*scale;

        // Store texture coordinates and new ID into LRU slot
        hz_lru_write_slot(lru, slot_index,
            (struct hz_cache_slot_t){
                .id = lru_id,
                .u0 = atlas_x0 / (float)g->opts.width,
                .v0 = atlas_y0 / (float)g->opts.height,
                .u1 = atlas_x1 / (float)g->opts.width,
                .v1 = atlas_y1 / (float)g->opts.height
            });

        hz_vec2 translate = (hz_vec2){ cellx + cellw/2 - (bounds.x0+bounds.x1)/2*scale , celly + cellh/2 - (bounds.y0+bounds.y1)/2*scale };

        hz_shape_draw_data_t draw_data = {0};
        hz_face_get_glyph_shape(face, &draw_data, translate, scale, lru_id.glyph_id);

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

        size_t vertex_count = hz_vector_size(draw_data.verts);
        hz_vector_push_many(bezier_verts, draw_data.verts, vertex_count);
        bezier_vertex_count += vertex_count;
        hz_shape_draw_data_clear(&draw_data);
    }

    // clear the cells which will be rendered into, and later for inverting the negative (interior) distance.
    

    // create actual curves vertex buffers
    GLuint curves_vao, curves_vbo, mask_vao, mask_vbo;
    size_t curve_draw_count = bezier_vertex_count;
    size_t stencil_draw_count = hz_vector_size(mask_verts);

    {
        // create segments VAO and VBO
        glGenVertexArrays(1, &curves_vao);
        glBindVertexArray(curves_vao);

        glGenBuffers(1,&curves_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, curves_vbo);
        glBufferData(GL_ARRAY_BUFFER, curve_draw_count * sizeof(hz_bezier_vertex_t), bezier_verts, GL_STATIC_DRAW);
        
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

    // use the curve to sdf shader program
    glEnable(GL_BLEND);
    glBlendEquation(GL_MAX);
    glBlendFunc(GL_ONE, GL_ONE);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_STENCIL_TEST);
    
    glUseProgram(g->curve_to_sdf_program);
    glBindVertexArray(curves_vao);
    

    static const uint32_t indices[]={0,1,2,2,1,3};
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices, curve_draw_count);

    // kokojima et al. technique used for generating a stencil mask used for inverting distance
    // in the glyph texture. Front-facing triangles increase stencil value and
    // back-facing ones decrease it.
    glUseProgram(g->stencil_kokojima_prog);
    {
        GLuint block_index = glGetUniformBlockIndex( g->stencil_kokojima_prog, "UboData" );
        if (block_index == GL_INVALID_INDEX) {
            fprintf(stderr,"Block index invalid 2!\n");
            exit(-1);
        }

        glBindBuffer(GL_UNIFORM_BUFFER, g->ubo_handle);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(hz_gl3_ubo_data_t), &g->ubo_data);
        glBindBufferBase(GL_UNIFORM_BUFFER, block_index, g->ubo_handle);
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
    glUseProgram(g->fs_triangle_prog);
    glDrawArrays(GL_TRIANGLES,0,3);

    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);
    
    //unbind
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    // cleanup
    glDeleteBuffers(1,&curves_vbo);
    glDeleteVertexArrays(1,&curves_vao);

    //delete arrays
    hz_vector_destroy(mask_verts);
    hz_vector_destroy(bezier_verts);
    hz_vector_destroy(clear_rects);
}

void hz_gl3_render_frame(hz_context_t *ctx, hz_renderer_gl3_t *g)
{
    hz_command_list_t *cmds = hz_command_list_get(ctx);
    hz_memory_arena_t *frame_arena = hz_get_frame_arena(ctx);
    hz_glyph_cache_t *lru = hz_context_get_lru(ctx);

    if (hz_vector_size(cmds->draw_data)) {
        size_t gid_ht_sz = hz_ht_size(cmds->unique_glyph_ht);

        if (gid_ht_sz) {
            hz_cache_id_t *avail_ids = hz_memory_arena_alloc(frame_arena, sizeof(hz_cache_id_t)*gid_ht_sz);
            hz_cache_id_t *unavail_ids = hz_memory_arena_alloc(frame_arena, sizeof(hz_cache_id_t)*gid_ht_sz);

            for(;;){
                struct hz_cache_stat_t stat = hz_lru_cache_stat(lru, cmds->unique_glyph_ht,
                                                            avail_ids, unavail_ids);

                // Render glyphs
                hz_vector(hz_glyph_instance_t) instance_glyphs = NULL;
                // Build glyph instances
                for (size_t v = 0; v < hz_vector_size(cmds->draw_data); ++v) {
                    struct hz_cache_node_t *n;
                    if ( (n = hz_lru_cache_get_node(lru, cmds->draw_data[v].lru_id)) != NULL ) {
                        hz_glyph_instance_t p = cmds->draw_data[v];
                        p.lru_id.u32 = n->slot;
                        hz_vector_push_back(instance_glyphs, p);
                    }
                }

                size_t glyphs_sz = hz_vector_size(instance_glyphs);

                if (glyphs_sz) {
                    glBindVertexArray(g->glyphs_vao);
                    glBindBuffer(GL_ARRAY_BUFFER, g->glyphs_vbo);
                    glBufferData(GL_ARRAY_BUFFER, sizeof(hz_glyph_instance_t)*glyphs_sz,
                        instance_glyphs, GL_DYNAMIC_DRAW);

                    // mvp
                    glEnableVertexAttribArray(0);
                    glEnableVertexAttribArray(1);
                    glEnableVertexAttribArray(2);
                    glEnableVertexAttribArray(3);
                    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(hz_glyph_instance_t), (void *)offsetof(hz_glyph_instance_t,vp));
                    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(hz_glyph_instance_t), (void *)(offsetof(hz_glyph_instance_t,vp) + sizeof(float)*4));
                    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(hz_glyph_instance_t), (void *)(offsetof(hz_glyph_instance_t,vp) + sizeof(float)*8));
                    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(hz_glyph_instance_t), (void *)(offsetof(hz_glyph_instance_t,vp) + sizeof(float)*12));
                    glVertexAttribDivisor(0, 1);
                    glVertexAttribDivisor(1, 1);
                    glVertexAttribDivisor(2, 1);
                    glVertexAttribDivisor(3, 1);

                    // scale
                    glEnableVertexAttribArray(4);
                    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(hz_glyph_instance_t), (void *)offsetof(hz_glyph_instance_t, v4_ts));
                    glVertexAttribDivisor(4, 1);
                    // style vars
                    glEnableVertexAttribArray(5);
                    glVertexAttribIPointer(5, 4, GL_UNSIGNED_INT, sizeof(hz_glyph_instance_t), (void *)offsetof(hz_glyph_instance_t, sv));
                    glVertexAttribDivisor(5, 1);

                    // id
                    glEnableVertexAttribArray(6);
                    glVertexAttribIPointer(6, 1, GL_UNSIGNED_INT, sizeof(hz_glyph_instance_t), (void *)offsetof(hz_glyph_instance_t, lru_id));
                    glVertexAttribDivisor(6, 1);

                    glBindVertexArray(g->glyphs_vao);
                    glUseProgram(g->char_quad_shader);

                    GLuint block_index = glGetUniformBlockIndex(g->char_quad_shader, "u_cache_slots" );
                    glBindBuffer(GL_UNIFORM_BUFFER, g->slots_ubo_handle);
                    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(struct hz_cache_slot_t)*lru->sz, lru->slots );
                    glBindBufferBase(GL_UNIFORM_BUFFER, block_index, g->slots_ubo_handle);

                    glBindTexture(GL_TEXTURE_2D, g->sdf_texture);
                    glEnable(GL_BLEND);
                    glDisable(GL_CULL_FACE);
                    glDisable(GL_DEPTH_TEST);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, glyphs_sz);
                    glBindVertexArray(0);
                }

                // TODO: Remove drawn glyphs from ht

                hz_vector_destroy(instance_glyphs); // destroy glyph instances buffer

                if (!stat.unavail) {
                    break;
                }

                // Write glyphs into texture
                hz_gl3_bind_fb(g);
                // Clear stencil from previous operations
                glClear(GL_STENCIL_BUFFER_BIT);
                glViewport(0,0, g->opts.width, g->opts.height);
                // Update UBO
                glUseProgram(g->curve_to_sdf_program);
                g->ubo_data.max_sdf_distance = g->opts.max_sdf_distance;
                g->ubo_data.view_matrix = hz_mat4_ortho(0.0f,g->opts.width,0.0f,g->opts.height);

                {
                    GLuint block_index = glGetUniformBlockIndex( g->curve_to_sdf_program, "UboData" );
                    if (block_index == GL_INVALID_INDEX) {
                        fprintf(stderr,"Block index invalid 1!\n");
                        exit(-1);
                    }

                    glBindBuffer(GL_UNIFORM_BUFFER, g->ubo_handle);
                    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(hz_gl3_ubo_data_t), &g->ubo_data);
                    glBindBufferBase(GL_UNIFORM_BUFFER, block_index, g->ubo_handle);
                }

                hz_gl3_refill_cache(ctx, g, stat.unavail, unavail_ids);
                hz_gl3_unbind_fb();
            }
        }
    }
}