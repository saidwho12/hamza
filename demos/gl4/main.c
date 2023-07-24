
#include "../../hz/hz_ucd_5_2_0.h"
#define HZ_IMPLEMENTATION
#define HZ_DEBUG_LOGGING
#include "../../hz/hz.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define HZ_GL4_IMPLEMENTATION
#include "hz_gl4.h"

#include <errno.h>

#define WIDTH 800
#define HEIGHT 800

#define ARRAYSIZE(x) (sizeof(x)/sizeof((x)[0]))

static int load_font_face(stbtt_fontinfo *fontinfo, const char *path)
{
    uint8_t *filedata;
    FILE *file;
    size_t length;

    if ((file = fopen(path, "rb")) != NULL) {
        fseek(file, 0, SEEK_END);
        length = ftell(file);
        fseek(file, 0, SEEK_SET);

        filedata = malloc(length);
        fread(filedata, 1, length, file);
    } else {
        fprintf(stderr, "Error: failed to open file \"%s\" (errno %d)\n", path, errno);
        return 0;
    }

    if (!stbtt_InitFont(fontinfo, filedata,0)) {
        fprintf(stderr, "Error: failed to initialize font info!\n");
        return 0;
    }

    return 1;
}


//   (-1,1)       (1,1)
//   +-----------+
//   |\          |
//   |  \        |
//   |    \      |
//   |      \    |
//   |        \  |
//   |          \|
//   +-----------+
//   (-1,-1)      (1,-1)
//  
//
GLuint create_fs_quad_shader()
{
    static const char fs_quad_vert[] = "#version 450 core\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "const vec2 fs_quad_verts[6] = vec2[6](vec2(-1,-1),vec2(-0.5,-1),vec2(-1,-0.5),vec2(-1,-0.5),vec2(-0.5,-1), vec2(-0.5,-0.5));\n"
    "const vec2 fs_quad_uvs[6] = vec2[6](vec2(0,0),vec2(1,0),vec2(0,1),vec2(0,1), vec2(1,0), vec2(1,1));\n"
    "out vec2 fragTexCoord;\n"
    "void main(){\n"
    "    gl_Position = vec4(fs_quad_verts[gl_VertexID],0.0,1.0);\n"
    "    fragTexCoord = fs_quad_uvs[gl_VertexID];\n"
    "}\n";

    static const char fs_quad_frag[] = "#version 450 core\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "in vec2 fragTexCoord;\n"
    "uniform int has_texture;\n"
    "uniform sampler2D uMainTex;\n"
    "layout(location=0) out vec4 outColor;\n"
    "void main(){\n"
    "    if (has_texture == 1){\n"
    "        outColor = vec4(texture(uMainTex,fragTexCoord).xyz,1.0);\n"
    "    }else{\n"
    "        outColor=vec4(1,0,0,1);\n"
    "    }"
    "}\n";

    GLuint vert_shader, frag_shader;
    vert_shader = hz_gl4_create_shader(fs_quad_vert, GL_VERTEX_SHADER);
    frag_shader = hz_gl4_create_shader(fs_quad_frag, GL_FRAGMENT_SHADER);
    
    GLuint program = hz_gl4_create_program(vert_shader, frag_shader);
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    return program;
}

#define GL_CHECKERR(id) { GLint err; while ((err = glGetError()) != GL_NO_ERROR) printf("%s: %d\n", id,err); }

int main(int argc, char *argv[])
{
    hz_config_t cfg = {.ucd_version = HZ_MAKE_VERSION(15,0,0)};

    if (hz_init(&cfg) != 0) {
        fprintf(stderr, "Failed to initialize Hamza!\n");
        return EXIT_FAILURE;
    }

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW!\n");
        exit(-1);
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Test", NULL, NULL);

    if (window == NULL) {
        fprintf(stderr, "Failed to create window!\n");
        exit(-1);
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to load OpenGL (gladLoadGLLoader)!\n");
        exit(-1);
    }

    printf("OpenGL version: %s\n", glGetString(GL_VERSION));
    printf("OpenGL vendor: %s\n", glGetString(GL_VENDOR));
    printf("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("OpenGL renderer: %s\n", glGetString(GL_RENDERER));

    // glFrontFace(GL_CCW);
    // glCullFace(GL_BACK);
    // glEnable(GL_CULL_FACE);
    // glEnable(GL_DEPTH_TEST);

    // setup sdf glyph cache texture
    hz_sdf_cache_opts_t sdf_opts;
    sdf_opts.width = 1024;
    sdf_opts.height = 1024;
    sdf_opts.max_sdf_distance = 24.0f; 
    sdf_opts.padd = 0.0f;
    sdf_opts.x_cells = 16;
    sdf_opts.y_cells = 16;

    hz_context_t ctx;
    hz_context_init(&ctx, &sdf_opts);
    hz_gl4_device_t dev;
    hz_gl4_device_init(&dev, &sdf_opts); 

    stbtt_fontinfo fontinfo, fontinfo2;
    // if (!load_font_face(&fontinfo, argv[1])) {
    // if (!load_font_face(&fontinfo, "../../../data/fonts/Quran/OmarNaskh-Regular.ttf")) {
    if (!load_font_face(&fontinfo, "../../../data/fonts/Times New Roman.ttf")) {
        hz_logln(HZ_LOG_ERROR, "Failed to load font file!");
        exit(-1);
    }

    if (!load_font_face(&fontinfo2, "../../../data/fonts/Quran/OmarNaskh-Regular.ttf")) {
        hz_logln(HZ_LOG_ERROR, "Failed to load font file!");
        exit(-1);
    }

    hz_font_t *font = hz_stbtt_font_create(&fontinfo);
    hz_font_t *font2 = hz_stbtt_font_create(&fontinfo2);

    hz_font_data_t font_data, font_data2;
    hz_font_data_init(&font_data, HZ_DEFAULT_FONT_DATA_ARENA_SIZE);
    hz_font_data_load(&font_data, font);
    hz_font_data_init(&font_data2, HZ_DEFAULT_FONT_DATA_ARENA_SIZE);
    hz_font_data_load(&font_data2, font2);

    hz_feature_t features[] = {
        HZ_FEATURE_CCMP,
        HZ_FEATURE_ISOL,
        HZ_FEATURE_INIT,
        HZ_FEATURE_MEDI,
        HZ_FEATURE_FINA,
        HZ_FEATURE_RLIG,
        HZ_FEATURE_CALT,
        HZ_FEATURE_LIGA,
        HZ_FEATURE_DLIG,
        // HZ_FEATURE_SWSH,
        HZ_FEATURE_MSET,
        HZ_FEATURE_CURS,
        HZ_FEATURE_KERN,
        HZ_FEATURE_MARK,
        HZ_FEATURE_MKMK,
    };

    uint16_t arialid = hz_context_stash_font(&ctx, &font_data);
    uint16_t omarid = hz_context_stash_font(&ctx, &font_data2);

    hz_shaper_t shaper;
    hz_shaper_init(&shaper);
    hz_shaper_set_direction(&shaper, HZ_DIRECTION_RTL);
    hz_shaper_set_script(&shaper, HZ_SCRIPT_ARABIC);
    hz_shaper_set_language(&shaper, HZ_LANGUAGE_ARABIC);
    hz_shaper_set_features(&shaper, ARRAYSIZE(features), features);

    hz_buffer_t ar_sample, en_sample, ru_sample;
    hz_buffer_init(&ar_sample);
    hz_shape_sz1(&shaper, &font_data2, HZ_ENCODING_UTF8, "ذهبت دليلة الى المدرسة", &ar_sample);

    hz_buffer_init(&en_sample);
    hz_shaper_set_direction(&shaper, HZ_DIRECTION_LTR);
    hz_shaper_set_script(&shaper, HZ_SCRIPT_LATIN);
    hz_shaper_set_language(&shaper, HZ_LANGUAGE_ENGLISH);
    hz_shape_sz1(&shaper, &font_data, HZ_ENCODING_UTF8, "Welcome! ♥", &en_sample);

    hz_buffer_init(&ru_sample);
    hz_shaper_set_direction(&shaper, HZ_DIRECTION_LTR);
    hz_shaper_set_script(&shaper, HZ_SCRIPT_CYRILLIC);
    hz_shaper_set_language(&shaper, HZ_LANGUAGE_RUSSIAN);
    hz_shape_sz1(&shaper, &font_data, HZ_ENCODING_UTF8, "Са́мый холо́дный го́род на Земле́", &ru_sample);
    
    glfwSwapInterval(0); // disable V-Sync

    // create fs_quad_shader
    GLuint fs_quad_shader = create_fs_quad_shader();

    // glDrawBuffer( GL_COLOR_ATTACHMENT0 );
    GLuint emptyVAO; glGenVertexArrays(1, &emptyVAO);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        hz_frame_begin(&ctx);
        hz_draw_buffer(&ctx, &ar_sample, omarid, (hz_vec2){-260.0f,100.0f},80.0f);
        hz_draw_buffer(&ctx, &en_sample, arialid, (hz_vec2){-200.0f,150.0f},42.0f);
        hz_draw_buffer(&ctx, &ru_sample, arialid, (hz_vec2){-200.0f,200.0f},42.0f);
        hz_frame_end(&ctx);
        hz_gl4_render_frame(&ctx,&dev);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0,0,width,height);
        glBindVertexArray(emptyVAO);
        glUseProgram(fs_quad_shader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, dev.sdf_texture);
        glUniform1i(glGetUniformLocation(fs_quad_shader,"has_texture"),1);
        glDrawArrays(GL_TRIANGLES,0,6);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glUniform1i(glGetUniformLocation(fs_quad_shader,"has_texture"),0);
        glDrawArrays(GL_TRIANGLES,0,6);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    hz_deinit();
    return 0;
}
