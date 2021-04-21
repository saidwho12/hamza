#include <hz.h>
#include <hz-ft.h>

#include <inttypes.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

//typedef uint8_t BYTE;
//typedef uint32_t DWORD;
//
//struct DDPIXELFORMAT {
//    DWORD dwSize;
//    DWORD dwFlags;
//    DWORD dwFourCC;
//    DWORD dwRGBBitCount;
//    DWORD dwRBitMask;
//    DWORD dwGBitMask;
//    DWORD dwBBitMask;
//    DWORD dwRGBAlphaBitMask;
//};
//
//struct DDCAPS2 {
//    DWORD dwCaps1;
//    DWORD dwCaps2;
//    DWORD Reserved[2];
//};
//
//struct DDSURFACEDESC2 {
//    DWORD dwSize;
//    DWORD dwFlags;
//    DWORD dwHeight;
//    DWORD dwWidth;
//    DWORD dwPitchOrLinearSize;
//    DWORD dwDepth;
//    DWORD dwMipMapCount;
//    DWORD dwReserved1[11];
//    DDPIXELFORMAT ddpfPixelFormat;
//    DDCAPS2 ddsCaps;
//    DWORD dwReserved2;
//};
//
//struct DDSFILE {
//    DWORD dwMagic;
//    DDSURFACEDESC2 ddsd;
//    BYTE *bData1;
//    BYTE *bData2;
//};

#define WIDTH 800
#define HEIGHT 200

int blit(uint8_t *dst, int dw, int dh, int bpp,
          const uint8_t *src,
          int sx, int sy, int sw, int sh)
{
    int y;
    int ss = sw * bpp; /* source stride */
    int ds = dw * bpp; /* destination stride */

    /* bounds check */
    if (!(sx >= 0 && (sx + sw) < dw && sy >= 0 && (sy + sh) < dh)) {
        return 0;
    }

    for (y = 0; y < sh; ++y) {
        const unsigned char *sl = src + y * ss;
        unsigned char *dl = dst + (sx + y) * ds + sy * bpp;
        memcpy(dl, sl, ss);
    }

    return 1;
}

int main(int argc, char *argv[]) {
    const char *text = "لاَ تَمْشِ فِي الأَرْضِ مَرَحًا إِنَّكَ لَن تَخْرِقَ الأَرْضَ وَلَن تَبْلُغَ الْجِبَالَ طُولاً";

    // Load font using FreeType
    FT_Library ft_library;
    FT_Init_FreeType(&ft_library);

    FT_Face ft_face;
    FT_New_Face(ft_library, "../../../data/fonts/UthmanicHafs1 Ver13.ttf", 0, &ft_face);
    FT_Set_Char_Size(ft_face, 1000, 0, 0, 0);


    // setup shaping context and buffer
    hz_font_t *font = hz_ft_font_create(ft_face);

    // setup language features
    hz_array_t *features = hz_array_create();
    hz_array_push_back(features, HZ_FEATURE_CCMP);
    hz_array_push_back(features, HZ_FEATURE_ISOL);
    hz_array_push_back(features, HZ_FEATURE_FINA);
    hz_array_push_back(features, HZ_FEATURE_MEDI);
    hz_array_push_back(features, HZ_FEATURE_INIT);
    hz_array_push_back(features, HZ_FEATURE_RLIG);
    hz_array_push_back(features, HZ_FEATURE_CALT);
    hz_array_push_back(features, HZ_FEATURE_LIGA);
    hz_array_push_back(features, HZ_FEATURE_DLIG);

    // positional features
    hz_array_push_back(features, HZ_FEATURE_MARK);
    hz_array_push_back(features, HZ_FEATURE_MKMK);


    hz_context_t *ctx = hz_context_create(font);
    hz_context_set_direction(ctx, HZ_DIRECTION_RTL);
    hz_context_set_script(ctx, HZ_SCRIPT_ARABIC);
    hz_context_set_language(ctx, HZ_LANGUAGE_ARABIC);
    hz_context_set_features(ctx, features);

    // Load utf8 string "_zt" stands for zero-terminated
    hz_sequence_t *section = hz_sequence_create();
    hz_sequence_load_utf8(section, (const hz_char *) text);
    hz_shape_full(ctx, section);


    uint8_t *image = malloc(WIDTH * HEIGHT);
    memset(image,0, WIDTH * HEIGHT);

    int xpos = 400, ypos = 100;

    hz_sequence_node_t *node = ctx->dir == HZ_DIRECTION_RTL ? hz_sequence_last_node(section->root) : section->root;
    while (node != NULL) {
        FT_GlyphSlot slot = ft_face->glyph;
        FT_Glyph glyph;

        FT_Load_Glyph(ft_face, node->glyph.id, FT_LOAD_DEFAULT);
//        FT_Get_Glyph(slot, &glyph);
        FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);

        unsigned int w = slot->bitmap.width;
        unsigned int h = slot->bitmap.rows;
        int xb = slot->bitmap_left >> 6;
        int yb = slot->bitmap_top >> 6;
        int xo = node->x_offset >> 6;
        int yo = node->y_offset >> 6;

        uint16_t x0 = xpos + xo + xb;
        uint16_t y0 = ypos + yo + (h - yb);
        /*
        uint16_t x1 = x0 + w;
        uint16_t y1 = y0 + h;
        */
        int tw = pow(2, ceil(log(w)/log(2)));
        int th = pow(2, ceil(log(h)/log(2)));

        uint8_t *tdata = malloc(tw * th);
        int iy;

        for(iy = 0; iy < h; ++iy) {
            memcpy(tdata + iy * tw, slot->bitmap.buffer + iy * w, h);
        }

        blit(image, WIDTH, HEIGHT, 1, tdata, x0, y0, w, h);

        free(tdata);

//        FT_Done_Glyph(glyph);

        xpos += node->glyph.x_advance;
        node = ctx->dir == HZ_DIRECTION_RTL ? node->prev : node->next;
    }

    stbi_write_bmp("./example.bmp", WIDTH, HEIGHT, 1, image);
    free(image);


    return 0;
}