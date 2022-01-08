#include <stdio.h>
#include <stdlib.h>

#define HZ_USE_FREETYPE
#include <hz/hz.h>
#include <hz/backends/hz_vulkan.h>

typedef struct {
    FT_Library ft_lib;
    GLFWwindow *window;
    hz_vk_renderer_t *renderer;
} App;

static void exitApp(App *app)
{
    hz_cleanup();
}

static void initApp(App *app)
{
    hz_setup();

    if (!glfwInit()) {
        printf("Failed to initialize GLFW!");
    }

    if (FT_Init_FreeType(&app->ft_lib) != FT_Err_Ok) {
        fprintf_s(stderr, "%s\n", "failed to load freetype.");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);

    app->window = glfwCreateWindow(800, 600, "Hamza Demo", NULL, NULL);

    if (app->window == NULL) {
        fprintf(stderr, "%s\n", "Failed to create window!");
    }

    glfwSwapInterval(0);

    app->renderer = hz_vk_renderer_create(app->window, 1);
}

static FT_Face load_font_face(FT_Library lib, const char *filename)
{
    FT_Face face;

    if (FT_New_Face(lib, filename, 0, &face) != FT_Err_Ok) {
        // Failed to load face, return NULL
        fprintf_s(stderr, "failed to open \"%s\" font.\n", filename);
        return NULL;
    }

    FT_Set_Char_Size(face, 12*64, 0, 0, 0);
    FT_Select_Charmap(face, FT_ENCODING_UNICODE);

    return face;
}

static void mainLoop(App *app)
{
    while (!glfwWindowShouldClose(app->window)) {
        glfwPollEvents();
        glfwSwapBuffers(app->window);
    }
}

int main(int argc, char *argv[]) {
    App app;
    initApp(&app);

    FT_Face face = load_font_face(app.ft_lib, "../data/fonts/Adobe Caslon Pro Regular.ttf");
    hz_font_t *font = hz_ft_font_create(face);

    const char *text = "The quick brown fox jumps over the lazy dog.";

    hz_segment_t *seg = hz_segment_create();
    hz_segment_load_utf8(seg, text);
    hz_segment_set_direction(seg, HZ_DIRECTION_LTR);
    hz_segment_set_script(seg, HZ_SCRIPT_LATIN);
    hz_segment_set_language(seg, hz_lang("eng"));

    hz_shape(font, seg, NULL, 0);

    mainLoop(&app);

    exitApp(&app);

    return EXIT_SUCCESS;
}