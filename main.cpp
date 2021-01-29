#include <hamim/hm.h>
#include <hamim/hm-ft.h>

int main(int argc, char *argv[]) {
    // Text string we want to shape
    const char *text = u8"ق والقرءان المجيد";

    // Load font using FreeType
    FT_Library ft_library;
    FT_Init_FreeType(&ft_library);

    FT_Face ft_face;
    FT_New_Face(ft_library, "../Cairo-Regular.ttf", 0, &ft_face);

    // Setup shaping context and buffer
    hm_face_t *face = hm_ft_face_create(ft_face);
    hm_section_t *run = hm_section_create();

    hm_context_t *ctx = hm_ctx_create(face);
    hm_ctx_set_dir(ctx, HM_DIR_RTL);
    hm_ctx_set_script(ctx, HM_SCRIPT_ARABIC);
    hm_ctx_set_language(ctx, HM_LANGUAGE_ARABIC);

    // Load utf8 string "_zt" stands for zero-terminated
    hm_section_load_utf8_zt(run, (const hm_char *) text);

//    for (size_t i = 0; i < hm_array_size(run->output); ++i) {
//        HM_LOG("U+%04X ", hm_array_at(run->output, i));
//    }
    HM_LOG("\n");

    hm_shape_full(ctx, run);

    return 0;
}