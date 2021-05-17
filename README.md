# Hamza
Hamza is a lightweight, fast and portable C89 (ANSI C) [OpenType](https://docs.microsoft.com/en-us/typography/opentype/spec) library.
It was written primarily for Arabic but [Unicode](https://unicode-table.com)/[OpenType](https://docs.microsoft.com/en-us/typography/opentype/spec) are very general so it does work with other scripts. That is as long as you are not using special features which are not implemented for a specific writing system. Works well with Arabic, Greek, Latin and Roman writing systems. Han, Indic and Cyrillic writing systems should *hopefully* be supported soon. Note that this is _**not**_ a renderer, it handles shaping only.

![Example of Complex Arabic rendered using Hamza](https://user-images.githubusercontent.com/70410392/118463012-49f0c980-b6cd-11eb-8b98-749719c96e12.png)


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

## Bindings
There are no bindings yet for Hamza, but I have planned to work on a python one soon.

## Build


## TODO
- [x] RTL
- [x] Ligatures
- [ ] Kerning
- [ ] Vertical Layouts (for Kanji)
- [ ] Emojis
- [ ] Justification
- [ ] OpenMP / TinyCThread parallelization
- [ ] Optimize shaping
- [ ] BCP47 / ISO-639 language tag handling
- [x] Documentation (Natural Docs)
