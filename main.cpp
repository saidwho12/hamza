#include <hz.h>
#include <hz-ft.h>

int main(int argc, char *argv[]) {
    // Text string we want to shape
    const char *text = u8"ق والقرءان المجيد";

    // Load font using FreeType
    FT_Library ft_library;
    FT_Init_FreeType(&ft_library);

    FT_Face ft_face;
    FT_New_Face(ft_library, "../data/fonts/Cairo-Regular.ttf", 0, &ft_face);

    // Setup shaping context and buffer
    hz_face_t *face = hz_ft_face_create(ft_face);
    hz_section_t *run = hz_section_create();

    hz_context_t *ctx = hz_context_create(face);
    hz_context_set_dir(ctx, HZ_DIR_RTL);
    hz_context_set_script(ctx, HZ_SCRIPT_ARABIC);
    hz_context_set_language(ctx, HZ_LANGUAGE_ARABIC);

    // Load utf8 string "_zt" stands for zero-terminated
    hz_section_load_utf8_zt(run, (const hz_char *) text);

//    for (size_t i = 0; i < hz_array_size(run->output); ++i) {
//        HZ_LOG("U+%04X ", hz_array_at(run->output, i));
//    }
//    HZ_LOG("\n");

    hz_shape_full(ctx, run);

    return 0;
}