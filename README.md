<p align="center">
  <img src="hamza.svg" />
</p>

# Hamza

*Hamza* is a light-weight, fast and portable C99 [OpenType](https://docs.microsoft.com/en-us/typography/opentype/spec) shaping and rendering library. It's built alongside the [stb_truetype.h](https://github.com/nothings/stb) TrueType library. It's designed to be a small, 
portable and optimized shaper that's easy to integrate into any existing project.

## Features
- [x] Joining script support and RTL writing
- [x] Kerning
- [x] Ligatures
- [x] Support for new [OpenType language tags](https://docs.microsoft.com/en-us/typography/opentype/spec/languagetags) (mixture of [ISO 639-3](https://iso639-3.sil.org/) and [ISO 639-2](https://www.loc.gov/standards/iso639-2/php/code_list.php) codes)
- [ ] Vertical layout Support (mostly for CJK, Mongolian, etc...)
- [ ] Color Emojis
- [ ] Emoji Combinations
- [ ] Multi-Threading


## Basic Usage
To start using Hamza, define `HZ_IMPLEMENTATION` before including  `hz.h`.
```c
#define HZ_IMPLEMENTATION
#include <hz/hz.h>
```

Before using any of Hamza's functions you must call `hz_init`:
```c
if (hz_init() != HZ_OK) {
    fprintf(stderr, "%s\n", "Failed to initialize Hamza!");
    return -1;
}
```

Next, before you can shape any text you must provide font data. You want to load a font into a `stbtt_fontinfo` struct. Hamza includes `stb_truetype.h` which is intended to be used in reading fonts. To create a `hz_font_t` from a stbtt font, write:
```c
hz_font_t *font = hz_stbtt_font_create(&fontinfo);
```

Hamza aims to let the user manage the memory allocation and the data as much as possible. Before shaping the font data has to be parsed into a `hz_font_data_t` struct. This holds all the OpenType table data required for shaping with a specific font. The `hz_font_data_init` function takes as argument how much memory will be allocated to hold that font's data:
```c
hz_font_data_t font_data;
hz_font_data_init(&font_data, 1024*1024); // 1MiB
hz_font_data_load(&font_data, font);
```
Create a shaper and initialize it:
```c
hz_shaper_t shaper;
hz_shaper_init(&shaper);
```
Set the shaper's required parameters:
```c
hz_shaper_set_direction(&shaper, HZ_DIRECTION_RTL);
hz_shaper_set_script(&shaper, HZ_SCRIPT_ARABIC);
hz_shaper_set_language(&shaper, HZ_LANGUAGE_ARABIC);
```
Set the shaper's typography features:
```c
hz_feature_t features[] = {
      HZ_FEATURE_ISOL,
      HZ_FEATURE_INIT,
      HZ_FEATURE_MEDI,
      HZ_FEATURE_FINA,
      HZ_FEATURE_RLIG,
      HZ_FEATURE_LIGA,
};

hz_shaper_set_features(&shaper, features, sizeof(features)/sizeof(features[0]));
```
Create glyph buffer and shape!
```c
hz_buffer_t buffer;
hz_buffer_init(&buffer);
hz_shape_sz1(&shaper, &font_data, HZ_ENCODING_UTF8, "السلام عليكم", &buffer);
```
After this, you can access the buffer's glyph data and render. After you are done with everything you have to deinitialize.
```c
hz_buffer_release(&buffer);
hz_font_data_release(&font_data);
hz_font_destroy(font);
hz_deinit();
```

## LICENSE
Hamza is licensed under LGPLv3.