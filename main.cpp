#include <guru/guru.h>
#include <guru/guru-freetype.h>

int main(int argc, char *argv[]) {
    // Text string we want to shape
    const char *text = u8"قٓ وَٱلْقُرْءَانِ ٱلْمَجِيدِ";

    // Load font using FreeType
    FT_Library ft_library;
    FT_Init_FreeType(&ft_library);

    FT_Face ft_face;
    FT_New_Face(ft_library, "../Cairo-Regular.ttf", 0, &ft_face);

    // Setup shaping context and buffer
    guru_face_t *face = guru_ft_face_create(ft_face);
    guru_buf_t *buf = guru_buf_create();

    guru_ctx_t *ctx = guru_ctx_create(face);
    guru_ctx_set_dir(ctx, GURU_DIR_RTL);
    guru_ctx_set_script(ctx, GURU_SCRIPT_ARABIC);
    guru_ctx_set_language(ctx, GURU_LANGUAGE_ARABIC);

    // Load utf8 string "_zt" stands for zero-terminated
    guru_buf_load_utf8_zt(buf, (const guru_char *)text);


    GURU_LOG("{ ");

    for (size_t i = 0; i < buf->len; ++i) {
        GURU_LOG("U+%04X ", buf->text[i]);
    }

    GURU_LOG("}\n");


    guru_shape(ctx, buf);

    return 0;
}