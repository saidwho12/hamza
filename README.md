# Hamza
_Hamza_ is a light-weight, fast and portable C89 (ANSI C) [OpenType](https://docs.microsoft.com/en-us/typography/opentype/spec) library.
It was written primarily for Arabic but supports any [Unicode](https://unicode-table.com)/[OpenType](https://docs.microsoft.com/en-us/typography/opentype/spec) script. That is as long as you are not using special features which are not implemented for a specific writing system. Note that this is _**not**_ a renderer, it handles shaping only.

### Feature Overview
- [x] Joining script support and Right-To-Left writing
- [x] OpenType Kerning
- [x] Ligatures
- [x] Support for new [OpenType language tags](https://docs.microsoft.com/en-us/typography/opentype/spec/languagetags) (mixture of [ISO 639-3](https://iso639-3.sil.org/) and [ISO 639-2](https://www.loc.gov/standards/iso639-2/php/code_list.php) codes)
- [] Support for old Two-Letter [ISO 639-1:2002](https://id.loc.gov/vocabulary/iso639-1.html) language tags (same as HarfBuzz)
- [] Vertical layout (i.e. for Chinese)
- [] Emojis
- [] OpenMP / TinyCThread Multi-Threading
- [] Justification

# Why?
_Hamza_ was written primarily for the development of a text rendering library, _Hamza_'s goal was to be a pure C OpenType library. I wrote this because I wanted a simpler API for shaping and dealing with caches, atlases, and glyph attributes. I needed the knowledge on OpenType to write an efficient and powerful renderer. This is even more exacerbated especially for a language like Arabic or Chinese with often thousands of glyphs and complex interactions.


## Basic Usage
This is a minimal "Hello, World!" example using Hamza. For more detailed usage information, refer to the [docs](https://saidwho12.github.io/hamza/).
```c
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
    hz_array_push_back(features, HZ_FEATURE_LIGA); /* enable ligatures */
    hz_array_push_back(features, HZ_FEATURE_CALT); /* enable contextual alternates */
    
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
    hz_array_destroy(features);
    hz_sequence_destroy(sequence);
    hz_context_destroy(context);
    hz_font_destroy(font);
    FT_Done_Face(ft_face);
    failed_to_load_face:
    FT_Done_FreeType(ft_library);
    failed_to_load_freetype:
    return 0;
}


```
