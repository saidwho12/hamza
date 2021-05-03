#include <hz.h>
#include <hz-ft.h>

int main(int argc, char *argv[]) {
    /* variable declarations */
    FT_Library ft_library;
    FT_Face ft_face;
    hz_font_t *font;
    hz_sequence_t *sequence;
    hz_context_t *context;
    hz_array_t *features;

    /* initialize variables state */
    if (FT_Init_FreeType(&ft_library)) {
        fputs("Failed to load FreeType!\n", stderr);
        goto failed_to_load_freetype;
    }

    if (FT_New_Face(ft_library, "NotoSans-Regular.ttf", 0, &ft_face)) {
        fputs("Failed to load Font!\n", stderr);
        goto failed_to_load_face;
    }

    font = hz_ft_font_create(ft_face);
    context = hz_context_create(font);

    /* set typographic features */
    features = hz_array_create();

    hz_context_set_direction(context, HZ_DIRECTION_LTR);
    hz_context_set_script(context, HZ_SCRIPT_LATIN);
    hz_context_set_language(context, HZ_LANGUAGE_ENGLISH);
    hz_context_set_features(context, features);

    sequence = hz_sequence_create();
    hz_sequence_load_utf8(sequence, "Hello, World!");

    /* shape the actual sequence of glyphs */
    hz_shape_full(context, sequence);

    /* render (using FreeType, or your method of choice) */

    /* free resources */
    failed_to_load_face:
    FT_Done_FreeType(ft_library);
    failed_to_load_freetype:
    return 0;
}
