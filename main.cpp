#include <guru/guru.h>
#include <guru/guru-freetype.h>

int main(int argc, char *argv[]) {
    // Text string we want to shape
    const char *text = u8"ق والقرءان المجيد";

    // Load font using FreeType
    FT_Library ft_library;
    FT_Init_FreeType(&ft_library);

    FT_Face ft_face;
    FT_New_Face(ft_library, "../Changa-Regular.ttf", 0, &ft_face);

    // Setup shaping context and buffer
    guru_face_t *face = guru_ft_face_create(ft_face);
    guru_run_t *run = guru_run_create();

    guru_ctx_t *ctx = guru_ctx_create(face);
    guru_ctx_set_dir(ctx, GURU_DIR_RTL);
    guru_ctx_set_script(ctx, GURU_SCRIPT_ARABIC);
    guru_ctx_set_language(ctx, GURU_LANGUAGE_ARABIC);

    // Load utf8 string "_zt" stands for zero-terminated
    guru_run_load_utf8_zt(run, (const guru_char *) text);

    for (size_t i = 0; i < run->len; ++i) {
        GURU_LOG("U+%04X ", run->text[i]);
    }
    GURU_LOG("\n");

    guru_shape(ctx, run);

    return 0;
}