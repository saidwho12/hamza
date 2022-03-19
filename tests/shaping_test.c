#include <stdio.h>
#include <stdlib.h>

#include <hz/hz.h>
#include <hz/backends/hz_vulkan.h>
#include <minmax.h>

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

void blit_image(unsigned char *dstPixels, long dstW, long dstH, unsigned char *srcPixels,
                long xpos, long ypos,
                long srcW, long srcH)
{
    for (long y = 0; y < srcH; ++y) {
        for (long x = 0; x < srcW; ++x) {
            size_t dx = xpos + x;
            size_t dy = ypos + y;
            size_t i = dy * dstW + dx;
            size_t j = y * srcW + x;
            if (dx >= 0 && dx <= dstW
            && dy >= 0 && dy <= dstH) {
                dstPixels[i] = max(dstPixels[i],srcPixels[j]);
            }
        }
    }
}

void render_text_to_png(const char *filename,
                        stbtt_fontinfo *font,
                        const hz_buffer_t *buffer)
{
    float scale = stbtt_ScaleForPixelHeight(font, 300);
    // Compute shaped glyphs axis-aligned bounding box
    int xmin = INT_MAX, ymin = INT_MAX, xmax = INT_MIN, ymax = INT_MIN;

    int pen_x = 0, pen_y = 0;
    for (size_t i = 0; i < buffer->glyph_count; ++i) {
        uint16_t glyph_index = buffer->glyph_indices[i];
        /* how wide is this character */
        int ax;
        int lsb;
        stbtt_GetGlyphHMetrics(font, glyph_index, &ax, &lsb);

        int ix0,iy0,ix1,iy1;
        stbtt_GetGlyphBitmapBox(font, glyph_index,
                                scale, scale,
                                &ix0,&iy0,&ix1,&iy1 );

        xmin = fminf(xmin, pen_x + ix0);
        ymin = fminf(ymin, iy0);
        xmax = fmaxf(xmax, pen_x + ix1);
        ymax = fmaxf(ymax, iy1);

        pen_x += roundf(ax * scale);
    }

//    int rw = xmax-xmin;
//    int rh = ymax-ymin;

//    xmin -= 64;
//    ymin -= 100;
//    xmax += 64;
//    ymax += 100;

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(font, &ascent, &descent, &lineGap);
    ascent = roundf(ascent * scale);
    descent = roundf(descent * scale);

    int w = xmax-xmin, h=(ymax-ymin)*2;
    uint8_t *pixels = malloc(w*h);
    memset(pixels,0,w*h);

    printf("x0: %d, y0: %d, x1: %d, y1: %d, w: %d, h: %d\n", xmin, ymin, xmax, ymax,
           w,h);

    pen_x = -xmin; pen_y = 0;//ymin/2;

    for (size_t i = 0; i < buffer->glyph_count; ++i) {
        uint16_t glyph_index = buffer->glyph_indices[i];
        /* how wide is this character */
        int ax;
        int lsb;
        stbtt_GetGlyphHMetrics(font, glyph_index, &ax, &lsb);

        if (glyph_index != 0) {
            /* (Note that each Codepoint call has an alternative Glyph version which caches the work required to lookup the character word[i].) */

            /* get bounding box for character (may be offset to account for chars that dip above or below the line */
            int c_x1, c_y1, c_x2, c_y2;
            stbtt_GetGlyphBitmapBox(font, glyph_index, scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

            /* compute y (different characters have different heights */
            int y = pen_y + ascent + c_y1;

            /* render character (stride and offset is important here) */
            int byteOffset = pen_x + roundf(lsb * scale) + (y * w);
            int dstx = pen_x + roundf(lsb * scale);
            int dsty = y;

            int c_w = c_x2 - c_x1;
            int c_h = c_y2 - c_y1;

            unsigned char *glyph_buffer = malloc(c_w * c_h);

            stbtt_MakeGlyphBitmap(font, glyph_buffer, c_w,
                                  c_h, c_w, scale, scale, glyph_index);

            blit_image(pixels, w, h, glyph_buffer, dstx, dsty, c_w, c_h);

            free(glyph_buffer);
        }

        pen_x += roundf(ax * scale);
    }

    stbi_write_png(filename, w,h,1,pixels,w);

    free(pixels);
}

struct hz_freelist_allocator_t {

    uint8_t *data;
    size_t size;
};



int main(int argc, char *argv[]) {
//    App app;
//    initApp(&app);

    hz_setup();

    stbtt_fontinfo  fontinfo;
//    load_font_face(&fontinfo, "../data/fonts/ACaslonPro-Regular.otf");
//    load_font_face(&fontinfo, "../data/fonts/TimesNewRoman.ttf");
//    load_font_face(&fontinfo, "../data/fonts/Jameel Noori Nastaleeq Kasheeda.ttf");
    load_font_face(&fontinfo, "../data/fonts/OmarNaskh-Regular.ttf");
//    load_font_face(&fontinfo, "../data/fonts/Devnew.ttf");
//    load_font_face(&fontinfo, "../data/fonts/UthmanicHafs1 Ver13.ttf");
//    load_font_face(&fontinfo,"../data/fonts/ScheherazadeNew-Regular.ttf");
//    load_font_face(&fontinfo,"../data/fonts/ScheherazadeRegOT.ttf");
//    load_font_face(&fontinfo,"../data/fonts/Literata-Regular.ttf");

    hz_font_t *font = hz_stbtt_font_create(&fontinfo);
#if 0
    const char *text = "ffi, tt, ct, ft, fi, fj, The quick brown fox jumped over the lazy dog";

    hz_segment_t *seg = hz_segment_create();
    hz_segment_load_utf8(seg, text);
    hz_segment_set_direction(seg, HZ_DIRECTION_LTR);
    hz_segment_set_script(seg, HZ_SCRIPT_LATIN);
    hz_segment_set_language(seg, hz_lang("eng"));
#else

//    const char *text = "هُوَ الَّذِي أَرْسَلَ رَسُولَهُ بِالْهُدَى وَدِينِ الْحَقِّ لِيُظْهِرَهُ عَلَى الدِّينِ كُلِّهِ وَكَفَى بِاللَّهِ شَهِيدًا";
//    const char *text= "حمزہ ایک ہلکا پھلکا، تیز اور پورٹیبل اوپن ٹائپ کی شکل دینے والی لائبریری ہے۔";
const char *text = "قرون الرجال الصبيان لا اله الا الله محمد رسول اللهﷺﷺﷺﷺ";

//const char *text="Hamza — это легкая, быстрая и портативная библиотека форм C99 OpenType. Он построен вместе с библиотекой TrueType от stb.";
//    const char *text = "Hamza ist eine leichtgewichtige, schnelle und portable C99 OpenType-Shaping-Bibliothek. Es wurde zusammen mit der TrueType-Bibliothek von stb erstellt.";
//    const char *text="Das Schriftzeichen ẞ (Großbuchstabe) bzw. ß (Kleinbuchstabe) ist ein Buchstabe des deutschen Alphabets. Er wird als Eszett [ɛsˈt͜sɛt] oder scharfes S bezeichnet,[1][2]";

    hz_segment_t *seg = hz_segment_create();
    hz_segment_load_utf8(seg, text);
    hz_segment_set_direction(seg, HZ_DIRECTION_RTL);
    hz_segment_set_script(seg, HZ_SCRIPT_ARABIC);
    hz_segment_set_language(seg, HZ_LANGUAGE_ARABIC);

//    const char *text = "हमजा एक हल्का, तेज और पोर्टेबल ओपन टाइप आकार देने वाला पुस्तकालय है।";
//    hz_segment_t *seg = hz_segment_create();
//    hz_segment_load_utf8(seg, text);
//    hz_segment_set_direction(seg, HZ_DIRECTION_LTR);
//    hz_segment_set_script(seg, HZ_SCRIPT_DEVANAGARI);
//    hz_segment_set_language(seg, HZ_LANGUAGE_HINDI);
#endif

    hz_feature_t features[] = {
//            HZ_FEATURE_CCMP,
            HZ_FEATURE_ISOL,
            HZ_FEATURE_FINA,
            HZ_FEATURE_MEDI,
            HZ_FEATURE_INIT,
            HZ_FEATURE_RLIG,
            HZ_FEATURE_CALT,
            HZ_FEATURE_LIGA,
            HZ_FEATURE_DLIG,
            HZ_FEATURE_CSWH,
    };

    hz_shape(font, seg, features, ARRAYSIZE(features));
//    hz_shape(font, seg, NULL, 0);

    const hz_buffer_t *buffer = hz_segment_get_buffer(seg);
    printf("glyph count: %llu\n", buffer->glyph_count);
    render_text_to_png("out.png", &fontinfo, buffer);

    hz_segment_destroy(seg);
    hz_cleanup();

    return EXIT_SUCCESS;
}
