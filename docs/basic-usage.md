## Load Fonts :material-abjad-arabic:
### From Memory
It is possible to load a font from memory using `hz_font_load_woff2_from_memory`.
```c
hz_font_t *arial = hz_font_load_woff2_from_memory(arial_data, arial_size);
```

### From A Base85 String
It's possible to load a font encoded in Base85. Your Base85 string must NUL-terminated, you must also pass in an encoding standard which can be either "Ascii85", "Z85" or "IPV6".

```c
static const char *mybase85 = ...;
hz_font_t *segoui = hz_font_load_woff2_from_base85("Z85", mybase85);
```

It is recommended to use Z85 or IPV6 encoding to embed fonts. Not only will it use less bytes of source code, as these encodings don't need escaping like the original "Ascii85", but they are also less error prone.
