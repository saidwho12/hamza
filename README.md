# Hamza
_Hamza_ is a light-weight, fast and portable C89 (ANSI C) [OpenType](https://docs.microsoft.com/en-us/typography/opentype/spec) library.
It was written primarily for Arabic but supports any [Unicode](https://unicode-table.com)/[OpenType](https://docs.microsoft.com/en-us/typography/opentype/spec) script. That is as long as you are not using special features which are not implemented for a specific writing system. Note that this is _**not**_ a renderer, it handles shaping only.

### Feature Overview
- [x] Joining script support and Right-To-Left writing
- [x] OpenType Kerning
- [x] Ligatures
- [x] Support for new [OpenType language tags](https://docs.microsoft.com/en-us/typography/opentype/spec/languagetags) (mixture of [ISO 639-3](https://iso639-3.sil.org/) and [ISO 639-2](https://www.loc.gov/standards/iso639-2/php/code_list.php) codes)
- [ ] Support for old Two-Letter [ISO 639-1:2002](https://id.loc.gov/vocabulary/iso639-1.html) language tags (same as HarfBuzz)
- [ ] Vertical layout (i.e. for Chinese)
- [ ] Emojis
- [ ] OpenMP / TinyCThread Multi-Threading
- [ ] Justification

![image](https://user-images.githubusercontent.com/70410392/121972354-ddf39680-cd48-11eb-947e-b1c6d51ff768.png)

# Why?
_Hamza_ was written primarily for the development of a text rendering library, _Hamza_'s goal was to be a pure C OpenType library. I wrote this because I wanted a simpler API for shaping and dealing with caches, atlases, and glyph attributes. I needed the knowledge on OpenType to write an efficient and powerful renderer. This is even more exacerbated especially for a language like Arabic or Chinese with often thousands of glyphs and complex interactions.
