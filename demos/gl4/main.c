#define HZ_DEBUG_LOGGING
#define HZ_IMPLEMENTATION
#include "../../hz/hz.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define HZ_GL4_IMPLEMENTATION
#include "hz_gl4.h"

#define WIDTH 1280
#define HEIGHT 720


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

int main(int argc, char *argv[])
{
    hz_setup(HZ_QUERY_CPU_FOR_SIMD);

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

    glViewport(0,0,WIDTH,HEIGHT);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    // setup sdf glyph cache texture
    hz_gl4_context_t ctx;
    hz_sdf_cache_opts_t sdf_opts;
    sdf_opts.width = 2048;
    sdf_opts.height = 2048;
    sdf_opts.max_sdf_distance = 16.0f; 
    sdf_opts.padd = 0.0f;
    sdf_opts.x_subdiv = 16;
    sdf_opts.y_subdiv = 16;
    hz_gl4_context_init(&ctx, &sdf_opts);

    stbtt_fontinfo fontinfo;
    if (!load_font_face(&fontinfo, "../../../data/fonts/TimesNewRoman.ttf")) {
        hz_logln(HZ_LOG_ERROR, "Failed to load font file!");
        exit(-1);
    }

    hz_font_t *notosans = hz_stbtt_font_create(&fontinfo);

    glfwSwapInterval(0); // disable V-Sync

    glClearColor(0.15f, 0.25f, 0.15f, 1.0f);
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();

        // hz_cmd_buffer_begin();
        // hz_cmd_buffer_end();
        
        // hz_gl4_render_frame(hz_get_draw_list());
        hz_gl4_transformed_glyph_t g;
        g.fontinfo = &fontinfo;
        g.gid = stbtt_FindGlyphIndex(&fontinfo, 'a');
        hz_gl4_generate_glyphs_sdf(&ctx,&g,1);
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    hz_cleanup();
    return 0;
}