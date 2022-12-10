# Hamza

Hamza is a light-weight, fast and portable C99 [OpenType](https://docs.microsoft.com/en-us/typography/opentype/spec) shaping and rendering library. It's built alongside the [stb_truetype.h](https://github.com/nothings/stb) TrueType library. It's designed to be a small, 
portable and optimized shaper that's easy to integrate into an existing project.
Originally Hamza was a research project to learn about font shaping for an Arabic calligraphy software I had in mind.
Later, with much research and work I have only developed more fascination with the general shaping process and possible strategies of optimization.    

## Objective
* Keep the library as a whole excluding backends and auto-generated tables under 10k SLOC.
* The goal is a non-pessimized, small and simple shaping library that can easily be included into any existing project.    
* The intent is to optimize lookup application slow paths using SIMD + multi-threading while keeping the code as simple and clear as possible.  Many think that shaping is a linear process and very difficult to multi-thread, this couldn't be further from the truth. GSUB table single substitution as an example is a perfect use-case for multi-threading, Unicode parsing can be vectorized and ligature matching can be optimized with SSE.
* Support for ASCII, Latin1, UTF-8, UTF-16, UTF-32, UCS2 and Johab encodings.

## Features
- [x] Joining script support and RTL writing
- [x] Kerning
- [x] Ligatures
- [x] Support for new [OpenType language tags](https://docs.microsoft.com/en-us/typography/opentype/spec/languagetags) (mixture of [ISO 639-3](https://iso639-3.sil.org/) and [ISO 639-2](https://www.loc.gov/standards/iso639-2/php/code_list.php) codes)
- [ ] Support for old Two-Letter [ISO 639-1:2002](https://id.loc.gov/vocabulary/iso639-1.html) language tags (same as HarfBuzz)
- [ ] Vertical layout (i.e. for Chinese)
- [ ] Emojis
- [ ] Multi-Threading

## Script Support
- [x] Standard Scripts (Latin, Cyrillic, Greek, etc)
- [x] Arabic
- [ ] Buginese
- [ ] Hangul
- [ ] Hebrew
- [ ] Indic
  - [ ] Bengali
  - [ ] Devanagari
  - [ ] Gujarati
  - [ ] Gurmukhi
  - [ ] Kannada
  - [ ] Malayalam
  - [ ] Odia
  - [ ] Tamil
  - [ ] Telugu
- [ ] Javanese
- [ ] Khmer
- [ ] Lao
- [ ] Myanmar
- [ ] Sinhala
- [ ] Syriac
- [ ] Thaana
- [ ] Thai
- [ ] Tibetan
  hello it is a new line 
## Usage
For usage of API functions, check `hz.h` and respective files. There is also documentation at <https://saidwho12.github.io/hamza/>.
