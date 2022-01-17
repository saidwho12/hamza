#include <stdio.h>
#include <stdlib.h>

#include <hz/hz.h>
#include <hz/backends/hz_vulkan.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

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

void render_text_to_png(const char *filename,
                        stbtt_fontinfo *font,
                        hz_shaped_glyph_t *shaped_glyphs,
                        size_t shaped_glyph_count)
{
    float scale = stbtt_ScaleForPixelHeight(font, 192);
    // Compute shaped glyphs axis-aligned bounding box
    int xmin = INT_MAX, ymin = INT_MAX, xmax = INT_MIN,ymax = INT_MIN;

    int pen_x = 0, pen_y = 0;
    for (size_t i = 0; i < shaped_glyph_count; ++i) {
        uint16_t glyph_index = shaped_glyphs[i].glyph_index;
        /* how wide is this character */
        int ax;
        int lsb;
        stbtt_GetGlyphHMetrics(font, glyph_index, &ax, &lsb);

        int ix0,iy0,ix1,iy1;
        stbtt_GetGlyphBitmapBox(font, shaped_glyphs[i].glyph_index,
                                scale, scale,
                                &ix0,&iy0,&ix1,&iy1 );

        xmin = fminf(xmin, pen_x + ix0);
        ymin = fminf(ymin, iy0);
        xmax = fmaxf(xmax, pen_x + ix1);
        ymax = fmaxf(ymax, iy1);

        pen_x += roundf(ax * scale);
    }

//    xmin -= 64;
    ymin -= 64;
//    xmax += 64;
    ymax += 64;

    int w = xmax-xmin, h=ymax-ymin;
    uint8_t *buffer = malloc(w*h);
    memset(buffer,0,w*h);

    printf("x0: %d, y0: %d, x1: %d, y1: %d, w: %d, h: %d\n", xmin, ymin, xmax, ymax,
           w,h);

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(font, &ascent, &descent, &lineGap);
    ascent = roundf(ascent * scale);
    descent = roundf(descent * scale);

    pen_x = -xmin; pen_y = -ymin/2;

    for (size_t i = 0; i < shaped_glyph_count; ++i) {
        hz_shaped_glyph_t  *g = &shaped_glyphs[i];
        uint16_t glyph_index = g->glyph_index;
        /* how wide is this character */
        int ax;
        int lsb;
        stbtt_GetGlyphHMetrics(font, glyph_index, &ax, &lsb);

        /* (Note that each Codepoint call has an alternative Glyph version which caches the work required to lookup the character word[i].) */

        /* get bounding box for character (may be offset to account for chars that dip above or below the line */
        int c_x1, c_y1, c_x2, c_y2;
        stbtt_GetGlyphBitmapBox(font, glyph_index, scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

        /* compute y (different characters have different heights */
        int y = pen_y + ascent + c_y1;

        /* render character (stride and offset is important here) */
        int byteOffset = pen_x + roundf(lsb * scale) + (y * w);
        stbtt_MakeGlyphBitmap(font, buffer + byteOffset, c_x2 - c_x1,
                              c_y2 - c_y1, w, scale, scale, glyph_index);

        pen_x += roundf(ax * scale);
    }

    stbi_write_png(filename, w,h,1,buffer,w);

    free(buffer);
}

int main(int argc, char *argv[]) {
//    App app;
//    initApp(&app);

    hz_setup();

    stbtt_fontinfo  fontinfo;
//    load_font_face(&fontinfo, "../data/fonts/ArnoPro-Regular.otf");
//load_font_face(&fontinfo, "../data/fonts/NotoSansArabic-Regular.ttf");
    load_font_face(&fontinfo, "../data/fonts/arial.ttf");

    hz_font_t *font = hz_stbtt_font_create(&fontinfo);
/*
    const char *text = "ffi, tt, ct, ft, fi, fj, The quick brown fox jumped over the lazy dog";

    hz_segment_t *seg = hz_segment_create();
    hz_segment_load_utf8(seg, text);
    hz_segment_set_direction(seg, HZ_DIRECTION_LTR);
    hz_segment_set_script(seg, HZ_SCRIPT_LATIN);
    hz_segment_set_language(seg, hz_lang("eng"));*/

//const char *text= "أَلَمْ تَرَ إِلَى الَّذِينَ يَزْعُمُونَ أَنَّهُمْ آمَنُوا بِمَا أُنْزِلَ إِلَيْكَ وَمَا أُنْزِلَ مِنْ قَبْلِكَ يُرِيدُونَ أَنْ يَتَحَاكَمُوا إِلَى الطَّاغُوتِ";
const char *text= "فإن من رضي أن يعبده الناس من دون الله ، فإنه يكون طاغوتًا كما قال تعالى";
    hz_segment_t *seg = hz_segment_create();
    hz_segment_load_utf8(seg, text);
    hz_segment_set_direction(seg, HZ_DIRECTION_RTL);
    hz_segment_set_script(seg, HZ_SCRIPT_ARABIC);
    hz_segment_set_language(seg, HZ_LANGUAGE_ARABIC);

    static const hz_feature_t features[] = {
            HZ_FEATURE_ISOL,
            HZ_FEATURE_FINA,
            HZ_FEATURE_MEDI,
            HZ_FEATURE_INIT,
            HZ_FEATURE_RLIG,
            HZ_FEATURE_CALT,
            HZ_FEATURE_LIGA,
            HZ_FEATURE_DLIG
    };

    for (int i = 0; i < 1; ++i)
        hz_shape(font, seg, features, ARRAYSIZE(features));
//    hz_shape(font, seg, NULL, 0);

    size_t shaped_glyph_count;
    hz_segment_get_shaped_glyphs(seg, NULL, &shaped_glyph_count);
    hz_shaped_glyph_t *shaped_glyphs = malloc(shaped_glyph_count * sizeof(hz_shaped_glyph_t));
    hz_segment_get_shaped_glyphs(seg, shaped_glyphs, &shaped_glyph_count);

//    for (size_t i = 0; i < shaped_glyph_count; ++i) {
//        printf("{ U+%04X | %s } ",
//               shaped_glyphs[i].glyph_index,
//               shaped_glyphs[i].glyph_class & HZ_GLYPH_CLASS_MARK ? "mark" : "other"
//               );
//    }
//
//    printf("\n");

    render_text_to_png("out.png", &fontinfo, shaped_glyphs, shaped_glyph_count);

    free(shaped_glyphs);
    hz_cleanup();
//    mainLoop(&app);
//    exitApp(&app);

    return EXIT_SUCCESS;
}