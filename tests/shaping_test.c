#include <stdio.h>
#include <stdlib.h>

#include <hz/hz.h>
#include <hz/backends/hz_vulkan.h>

#define ARRAYSIZE(x) (sizeof(x)/sizeof((x)[0]))

typedef struct {
    GLFWwindow *window;
    hz_vulkan_renderer_t *renderer;
} App;

static void exitApp(App *app)
{
    glfwDestroyWindow(app->window);
    glfwTerminate();
    hz_cleanup();
}

static void initApp(App *app)
{
    hz_setup();

    if (!glfwInit()) {
        printf("Failed to initialize GLFW!");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);

    app->window = glfwCreateWindow(800, 600, "Hamza Demo", NULL, NULL);

    if (app->window == NULL) {
        fprintf(stderr, "%s\n", "Failed to create window!");
    }

    glfwSwapInterval(0);

    app->renderer = hz_vulkan_renderer_create(app->window, 1);
}

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

static void mainLoop(App *app)
{
    while (!glfwWindowShouldClose(app->window)) {
        glfwPollEvents();
        glfwSwapBuffers(app->window);
    }
}

int main(int argc, char *argv[]) {
//    App app;
//    initApp(&app);


    stbtt_fontinfo  fontinfo;
    load_font_face(&fontinfo, "../data/fonts/ArnoPro-Regular.otf");

    hz_font_t *font = hz_stbtt_font_create(&fontinfo);

    const char *text = "Affirming the little goal with a lofty result";

    hz_segment_t *seg = hz_segment_create();
    hz_segment_load_utf8(seg, text);
    hz_segment_set_direction(seg, HZ_DIRECTION_LTR);
    hz_segment_set_script(seg, HZ_SCRIPT_LATIN);
    hz_segment_set_language(seg, hz_lang("eng"));

    static const hz_feature_t features[] = {
            HZ_FEATURE_RLIG,
            HZ_FEATURE_CALT,
            HZ_FEATURE_LIGA
    };

//    hz_shape(font, seg, features, ARRAYSIZE(features));
    hz_shape(font, seg, NULL, 0);

    size_t shaped_glyph_count;
    hz_segment_get_shaped_glyphs(seg, NULL, &shaped_glyph_count);
    hz_shaped_glyph_t *shaped_glyphs = malloc(shaped_glyph_count * sizeof(hz_shaped_glyph_t));
    hz_segment_get_shaped_glyphs(seg, shaped_glyphs, &shaped_glyph_count);

    for (size_t i = 0; i < shaped_glyph_count; ++i) {
        printf("U+%04X ",shaped_glyphs[i].gid);
    }

    printf("\n");

    free(shaped_glyphs);

//    mainLoop(&app);
//    exitApp(&app);

    return EXIT_SUCCESS;
}