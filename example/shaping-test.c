#include <stdio.h>
#include <stdlib.h>

#include <hz/hz.h>
#include <hz/backends/hz-vulkan.h>
#include <minmax.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define ARRAYSIZE(x) (sizeof(x)/sizeof((x)[0]))

typedef struct {
    GLFWwindow *window;
    hz_vk_context_t *vkContext;
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

    app->vkContext = hz_vk_context_create(app->window, 1);
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
        hz_vk_render_frame(app->vkContext);
        glfwSwapBuffers(app->window);
    }
}

typedef struct {
    float red,green,blue,alpha;
} Color;

Color BlendOver(Color b, Color a)
{
    Color o;
    o.alpha = a.alpha + b.alpha * (1.0f - a.alpha);
    o.red = (a.red * a.alpha + b.red * (1.0f - a.alpha));
    o.green = (a.green * a.alpha + b.green * (1.0f - a.alpha));
    o.blue = (a.blue * a.alpha + b.blue * (1.0f - a.alpha));
    return o;
}

void blit_image(unsigned char *dstPixels, long dstW, long dstH, unsigned char *srcPixels,
                long xpos, long ypos,
                long srcW, long srcH, Color color)
{
    for (long y = 0; y < srcH; ++y) {
        for (long x = 0; x < srcW; ++x) {
            size_t dx = xpos + x;
            size_t dy = ypos + y;
            size_t i = dy * dstW + dx;
            size_t j = y * srcW + x;
            if (dx >= 0 && dx <= dstW
            && dy >= 0 && dy <= dstH) {
                Color A,B;

                A.red = (float)dstPixels[i*4] / 255.0f;
                A.green = (float)dstPixels[i*4+1] / 255.0f;
                A.blue = (float)dstPixels[i*4+2] / 255.0f;
                A.alpha = (float)dstPixels[i*4+3] / 255.0f;

                B = color;
                B.alpha *= (float)srcPixels[j] / 255.0f;

                Color result = BlendOver(A,B);

                dstPixels[i*4] = (uint8_t)(result.red * 255.0f);
                dstPixels[i*4+1] = (uint8_t)(result.green * 255.0f);
                dstPixels[i*4+2] = (uint8_t)(result.blue * 255.0f);
                dstPixels[i*4+3] = (uint8_t)(result.alpha * 255.0f);
            }
        }
    }
}

void render_text_to_png(const char *filename,
                        stbtt_fontinfo *font,
                        const hz_buffer_t *buffer)
{
    float scale = stbtt_ScaleForPixelHeight(font, 450);
    // Compute shaped glyphs axis-aligned bounding box
    int xmin = INT_MAX, ymin = INT_MAX, xmax = INT_MIN, ymax = INT_MIN;

    int pen_x = 0, pen_y = 0;
    for (size_t i = 0; i < buffer->glyph_count; ++i) {
        uint16_t glyph_index = buffer->glyph_indices[i];
        hz_glyph_metrics_t *glyph_metrics = &buffer->glyph_metrics[i];
        int32_t x_advance = glyph_metrics->x_advance;
        int32_t y_advance = glyph_metrics->y_advance;
        int32_t x_offset = glyph_metrics->x_offset;
        int32_t y_offset = glyph_metrics->y_offset;

        int ix0,iy0,ix1,iy1;
        stbtt_GetGlyphBitmapBox(font, glyph_index,
                                scale, scale,
                                &ix0, &iy0, &ix1, &iy1);

        float bx0 = pen_x + ix0 ;//+ pen_x + roundf(x_offset * scale);
        float bx1 = pen_x + ix1 ;//+ pen_x + roundf(x_offset * scale);
        float by0 = pen_y + iy0 ;//+ pen_y + roundf(y_offset * scale);
        float by1 = pen_y + iy1 ;//+ pen_y + roundf(y_offset * scale);

        float xx = roundf(x_offset * scale);
        float yy = roundf(y_offset * scale);

        xmin = fminf(xmin, bx0+xx);
        ymin = fminf(ymin, by0+yy);
        xmax = fmaxf(xmax, bx1+xx);
        ymax = fmaxf(ymax, by1+yy);

        pen_x += roundf(x_advance * scale);
    }

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(font, &ascent, &descent, &lineGap);
    ascent = roundf(ascent * scale);
    descent = roundf(descent * scale);

    int w = xmax-xmin, h=(ymax-ymin)*2;
    uint8_t *pixels = malloc(w*h*4);
    memset(pixels,0,w*h*4);


    Color clearColor = {1.0f,//77.0f/255.0f,
                        1.0f,//104.0f/255.0f,
                        1.0f,//147.0f/255.0f,
                        1.0f};

    for (size_t i = 0; i < w*h; ++i) {
        pixels[i*4] = clearColor.red * 255.0f;
        pixels[i*4+1] = clearColor.green * 255.0f;
        pixels[i*4+2] = clearColor.blue * 255.0f;
        pixels[i*4+3] = 0;
    }

    printf("x0: %d, y0: %d, x1: %d, y1: %d, w: %d, h: %d\n", xmin, ymin, xmax, ymax, w, h);

    pen_x = 0; pen_y = 0;//ymin/2;

    for (size_t i = 0; i < buffer->glyph_count; ++i) {
        uint16_t glyph_index = buffer->glyph_indices[i];
        /* how wide is this character */
        int ax;
        int lsb;
        stbtt_GetGlyphHMetrics(font, glyph_index, &ax, &lsb);

        if (glyph_index != 0) {
            hz_glyph_metrics_t *glyph_metrics = &buffer->glyph_metrics[i];
            int32_t x_advance = glyph_metrics->x_advance;
            int32_t y_advance = glyph_metrics->y_advance;
            int32_t x_offset = glyph_metrics->x_offset;
            int32_t y_offset = glyph_metrics->y_offset;

            int ix0,iy0,ix1,iy1;
            stbtt_GetGlyphBitmapBox(font, glyph_index,
                                    scale, scale,
                                    &ix0,&iy0,&ix1,&iy1);

            ix0 += roundf(x_offset * scale);
            ix1 += roundf(x_offset * scale);
            iy0 += roundf(-y_offset * scale);
            iy1 += roundf(-y_offset * scale);

            /* compute y (different characters have different heights */
            int y = pen_y + ascent + iy0;

            /* render character (stride and offset is important here) */
            int dstx = pen_x + roundf((x_offset + lsb) * scale);
            int dsty = y;

            int c_w = ix1 - ix0;
            int c_h = iy1 - iy0;

            unsigned char *glyph_buffer = malloc(c_w * c_h);

            stbtt_MakeGlyphBitmap(font, glyph_buffer, c_w,
                                  c_h, c_w, scale, scale, glyph_index);

            Color col = {0.214f,//138.0f/255.0f,
                         0.214f,//176.0f/255.0f,
                         0.214f,//238.0f/255.0f,
                         1.0f};
#if 0
            if (buffer->glyph_classes[i] & HZ_GLYPH_CLASS_MARK) {
                col = (Color){0.53f,0.15f,.45f,1.0f};
            }

            if (buffer->glyph_classes[i] & HZ_GLYPH_CLASS_LIGATURE) {
                col = (Color){0.95f,0.85f,.25f,1.0f};
            }
#endif
//            if (buffer->glyph_classes[i] & HZ_GLYPH_CLASS_MARK) {
//                col = (Color){42.0f/255.0f,52.0f/255.0f,99.0f/255.0f,1.0f};
//            }
#if 0
            col.red = (float)(rand() % 255) / 255.0f * .7f;
            col.green = (float)(rand() % 255) / 255.0f * .7f;
            col.blue = (float)(rand() % 255) / 255.0f * .7f;
#endif
            blit_image(pixels, w, h, glyph_buffer, dstx, dsty, c_w, c_h,col);

            free(glyph_buffer);
            pen_x += roundf(x_advance * scale);
        }

    }

    for (size_t i = 0; i < w*h; ++i) {
        pixels[i*4+3] = 255;
    }

    stbi_write_png(filename, w,h,4,pixels,w*4);

    free(pixels);
}

int main(int argc, char *argv[]) {

    hz_setup();

    stbtt_fontinfo  fontinfo;
//    load_font_face(&fontinfo, "../data/fonts/TimesNewRoman.ttf");
//        load_font_face(&fontinfo, "../data/fonts/ACaslonPro-Regular.otf");
//load_font_face(&fontinfo, "../data/fonts/UthmanTN1 Ver10.otf");
//load_font_face(&fontinfo, "../data/fonts/Amiri-Regular.ttf");
//    load_font_face(&fontinfo, "../data/fonts/Jameel Noori Nastaleeq Regular.ttf");
load_font_face(&fontinfo, "../data/fonts/Quran/OmarNaskh-Light.ttf");
//load_font_face(&fontinfo, "../data/fonts/arabic-ejaza.ttf");
//load_font_face(&fontinfo, "../data/fonts/AyeshaQuran-Regular.ttf");
//    load_font_face(&fontinfo, "../data/fonts/Devnew.ttf");
//load_font_face(&fontinfo, "../data/fonts/NotoKufiArabic-Bold.ttf");
//    load_font_face(&fontinfo, "../data/fonts/UthmanicHafs1 Ver13.ttf");
//    load_font_face(&fontinfo,"../data/fonts/Quran/AyeshaNaskh-ExtraLight.ttf");
//    load_font_face(&fontinfo,"../data/fonts/ScheherazadeNew-Regular.ttf");
//    load_font_face(&fontinfo,"../data/fonts/ScheherazadeRegOT.ttf");
//    load_font_face(&fontinfo,"../data/fonts/Literata-Regular.ttf");

    hz_font_t *font = hz_stbtt_font_create(&fontinfo);

//    const char *text = "الحَمْدُ لِلَّه رب العالمين";
//const char *text = "AT AV AW AY Av Aw Ay \n"
//                   "\n"
//                   "Fa Fe Fo Kv Kw Ky LO \n"
//                   "\n"
//                   "LV LY PA Pa Pe Po TA \n"
//                   "\n"
//                   "Hello, World! How's it going today?";

    const char *text = "وَدَّ كَثِيرٌ مِّنْ أَهْلِ ٱلْكِتَـٰبِ لَوْ يَرُدُّونَكُم مِّنۢ بَعْدِ إِيمَـٰنِكُمْ كُفَّارًا حَسَدًا مِّنْ عِندِ أَنفُسِهِم مِّنۢ بَعْدِ مَا تَبَيَّنَ لَهُمُ ٱلْحَقُّ ۖ فَٱعْفُوا۟ وَٱصْفَحُوا۟ حَتَّىٰ يَأْتِىَ ٱللَّهُ بِأَمْرِهِۦٓ ۗ إِنَّ ٱللَّهَ عَلَىٰ كُلِّ شَىْءٍ قَدِيرٌ";

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

//    hz_feature_t features[] = {
//        HZ_FEATURE_LOCL,
//        HZ_FEATURE_NUKT,
//        HZ_FEATURE_AKHN,
//        HZ_FEATURE_RPHF,
//        HZ_FEATURE_RKRF,
//        HZ_FEATURE_BLWF,
//        HZ_FEATURE_HALF,
//        HZ_FEATURE_VATU,
//        HZ_FEATURE_CJCT
//    };

    hz_feature_t features[] = {
            HZ_FEATURE_CCMP,
//            HZ_FEATURE_RVRN,
//            HZ_FEATURE_LOCL,
            HZ_FEATURE_ISOL,
            HZ_FEATURE_FINA,
            HZ_FEATURE_MEDI,
            HZ_FEATURE_INIT,
            HZ_FEATURE_RCLT,
            HZ_FEATURE_CALT,
            HZ_FEATURE_RLIG,
//            HZ_FEATURE_MSET,
//            HZ_FEATURE_CLIG,
//            HZ_FEATURE_LIGA,
//            HZ_FEATURE_SS07,
//            HZ_FEATURE_SS12,
            HZ_FEATURE_KERN,
            HZ_FEATURE_CURS,
            HZ_FEATURE_MARK,
            HZ_FEATURE_MKMK,
//            HZ_FEATURE_CSWH,
    };

    hz_shape(font, seg, features, ARRAYSIZE(features), 0);
//    hz_shape(font, seg, NULL, 0, HZ_SHAPE_FLAG_AUTO_LOAD_FEATURES);
//hz_shape(font, seg, NULL, 0, 0);

    const hz_buffer_t *buffer = hz_segment_get_buffer(seg);
    printf("glyph count: %llu\n", buffer->glyph_count);
    render_text_to_png("out.png", &fontinfo, buffer);


//    App app;
//    initApp(&app);
//    mainLoop(&app);

    hz_segment_destroy(seg);
    hz_cleanup();

    return EXIT_SUCCESS;
}
