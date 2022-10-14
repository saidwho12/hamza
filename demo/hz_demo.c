#include <stdio.h>
#include <stdlib.h>

#include <hz/hz.h>
#include <hz/backends/hz_renderer_vulkan.h>
#include <errno.h>
#include <limits.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define ARRAYSIZE(x) (sizeof(x)/sizeof((x)[0]))

typedef struct {
    GLFWwindow *window;
    hz_vk_renderer_t *vk_renderer;
    stbtt_fontinfo fontinfo;
} App;

static void exitApp(App *app)
{
    glfwDestroyWindow(app->window);
    glfwTerminate();
    hz_cleanup();
}

static void initApp(App *app)
{
    if (!glfwInit()) {
        printf("Failed to initialize GLFW!");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);

    app->window = glfwCreateWindow(800, 800, "Hamza Demo", NULL, NULL);

    if (app->window == NULL) {
        fprintf(stderr, "%s\n", "Failed to create window!");
    }

    glfwSwapInterval(0);

    app->vk_renderer = hz_vk_create_renderer(app->window, 1);
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
        hz_index_t gid = stbtt_FindGlyphIndex(&app->fontinfo, 'a');
        hz_vk_render_frame(app->vk_renderer, &app->fontinfo, gid);
    }

    hz_vk_wait_idle(app->vk_renderer);
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

float rand_float() {
    #define RAND_MAX 1000
    return (float)(rand() % RAND_MAX)/RAND_MAX;  
}

float rand_float_in_range(float a, float b) {
    return rand_float() * (b-a) + a;
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
        int32_t xAdvance = glyph_metrics->xAdvance;
        int32_t yAdvance = glyph_metrics->yAdvance;
        int32_t xOffset = glyph_metrics->xOffset;
        int32_t yOffset = glyph_metrics->yOffset;

        int ix0,iy0,ix1,iy1;
        stbtt_GetGlyphBitmapBox(font, glyph_index,
                                scale, scale,
                                &ix0, &iy0, &ix1, &iy1);

        float bx0 = pen_x + ix0 ;//+ pen_x + roundf(xOffset * scale);
        float bx1 = pen_x + ix1 ;//+ pen_x + roundf(xOffset * scale);
        float by0 = pen_y + iy0 ;//+ pen_y + roundf(yOffset * scale);
        float by1 = pen_y + iy1 ;//+ pen_y + roundf(yOffset * scale);

        float xx = roundf(xOffset * scale);
        float yy = roundf(yOffset * scale);

        xmin = fminf(xmin, bx0+xx);
        ymin = fminf(ymin, by0+yy);
        xmax = fmaxf(xmax, bx1+xx);
        ymax = fmaxf(ymax, by1+yy);

        pen_x += roundf(xAdvance * scale);
    }

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(font, &ascent, &descent, &lineGap);
    ascent = roundf(ascent * scale);
    descent = roundf(descent * scale);


    int bw = xmax-xmin;
    int bh = ymax-ymin;
    int w = bw+100, h=bh*2;
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

    pen_x = w/2-bw/2; pen_y = h/2-bh/2;//ymin/2;

    for (size_t i = 0; i < buffer->glyph_count; ++i) {
        uint16_t glyph_index = buffer->glyph_indices[i];

        //printf("%d\n", glyph_index);
        /* how wide is this character */
        int ax;
        int lsb;
        stbtt_GetGlyphHMetrics(font, glyph_index, &ax, &lsb);

        if (glyph_index != 0) {
            hz_glyph_metrics_t *glyph_metrics = &buffer->glyph_metrics[i];
            int32_t xAdvance = glyph_metrics->xAdvance;
            int32_t yAdvance = glyph_metrics->yAdvance;
            int32_t xOffset = glyph_metrics->xOffset;
            int32_t yOffset = glyph_metrics->yOffset;

            int ix0,iy0,ix1,iy1;
            stbtt_GetGlyphBitmapBox(font, glyph_index,
                                    scale, scale,
                                    &ix0,&iy0,&ix1,&iy1);

            ix0 += roundf(xOffset * scale);
            ix1 += roundf(xOffset * scale);
            iy0 += roundf(-yOffset * scale);
            iy1 += roundf(-yOffset * scale);

            /* compute y (different characters have different heights */
            int y = pen_y + ascent + iy0;

            /* render character (stride and offset is important here) */
            int dstx = pen_x + roundf((xOffset + lsb) * scale);
            int dsty = y;

            int c_w = ix1 - ix0;
            int c_h = iy1 - iy0;

            unsigned char *glyph_buffer = malloc(c_w * c_h);

            stbtt_MakeGlyphBitmap(font, glyph_buffer, c_w,
                                  c_h, c_w, scale, scale, glyph_index);

            Color col = {0.0f,//rand_float(),//138.0f/255.0f,
                         0.0f,//rand_float(),//176.0f/255.0f,
                         0.0f,//rand_float(),//238.0f/255.0f,
                         1.0F};


#if 0
            if (buffer->glyph_classes[i] & HZ_GLYPH_CLASS_MARK) {
                float val = (buffer->component_indices[i]+1.0f)/3.0f;
                val = 0.2f + 0.6f*val;
                col = (Color){0.96f, 0.1f,0.11f,1.0f};
            } else if (buffer->glyph_classes[i] & HZ_GLYPH_CLASS_LIGATURE) {
                col = (Color){0.75f,0.01f,0.8f,1.0f};
            }
#endif

            blit_image(pixels, w, h, glyph_buffer, dstx, dsty, c_w, c_h,col);

            free(glyph_buffer);
            pen_x += roundf(xAdvance * scale);
        }

    }

    for (size_t i = 0; i < w*h; ++i) {
        pixels[i*4+3] = 255;
    }

    stbi_write_png(filename, w,h,4,pixels,w*4);

    free(pixels);
}

int main(int argc, char *argv[]) {

    hz_setup(HZ_USE_CPUID_FOR_SIMD_CHECKS);

    stbtt_fontinfo  fontinfo;
    load_font_face(&fontinfo, "../data/fonts/TimesNewRoman.ttf");
    const char *text = "Hello, World!";

    hz_segment_t *seg = hz_segment_create();
    hz_segment_load_utf8(seg, text);
    hz_segment_set_direction(seg, HZ_DIRECTION_LTR);
    hz_segment_set_script(seg, HZ_SCRIPT_LATIN);
    hz_segment_set_language(seg, HZ_LANGUAGE_ENGLISH);

    hz_feature_t features[] = {
            HZ_FEATURE_CCMP,
            HZ_FEATURE_LOCL,
            HZ_FEATURE_RCLT,
            HZ_FEATURE_CALT,
            HZ_FEATURE_RLIG,
            HZ_FEATURE_MSET,
            HZ_FEATURE_CLIG,
            HZ_FEATURE_LIGA,
            HZ_FEATURE_KERN,
            HZ_FEATURE_CURS,
            HZ_FEATURE_MARK,
            HZ_FEATURE_MKMK
    };

    hz_shape(font, seg, features, ARRAYSIZE(features), 0);

    const hz_buffer_t *buffer = hz_segment_get_buffer(seg);
    render_text_to_png("out.png", &fontinfo, buffer);
    hz_segment_destroy(seg);

    // App app;
    // initApp(&app);
    // app.fontinfo = fontinfo;
    // mainLoop(&app);

    hz_cleanup();

    return EXIT_SUCCESS;
}
