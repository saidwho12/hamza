#ifndef HZ_OT_H
#define HZ_OT_H

#include "hz-base.h"
#include "hz-set.h"
#include "hz-array.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Registered Features
 * https://docs.microsoft.com/en-us/typography/opentype/spec/featurelist
 *
 */
typedef enum hz_feature_t {
    HZ_FEATURE_AALT = 0, /* Access All Alternates */
    HZ_FEATURE_ABVF = 1, /* Above-base forms */
    HZ_FEATURE_ABVM = 2, /* Above-base Mark Positioning */
    HZ_FEATURE_ABVS = 3, /* Above-base Substitutions */
    HZ_FEATURE_AFRC = 4, /* Alternative Fractions */
    HZ_FEATURE_AKHN = 5, /* Akhands */
    HZ_FEATURE_BLWF = 6, /* Below-base forms */
    HZ_FEATURE_BLWM = 7, /* Below-base Mark Positioning */
    HZ_FEATURE_BLWS = 8, /* Below-base Substitutions */
    HZ_FEATURE_CALT = 9, /* Contextual Alternates */
    HZ_FEATURE_CASE = 10, /* Case-Sensitive Forms */
    HZ_FEATURE_CCMP = 11, /* Glyph Composition / Decomposition */
    HZ_FEATURE_CFAR = 12, /* Conjunct Form After Ro */
    HZ_FEATURE_CJCT = 13, /* Conjunct forms */
    HZ_FEATURE_CLIG = 14, /* Contextual Ligatures */
    HZ_FEATURE_CPCT = 15, /* Centered CJK Punctuation */
    HZ_FEATURE_CPSP = 16, /* Capital Spacing */
    HZ_FEATURE_CSWH = 17, /* Contextual Swash */
    HZ_FEATURE_CURS = 18, /* Cursive Positioning */
    /* 'cv01' – 'cv99' Character Variants */
    HZ_FEATURE_C2PC = 117, /* Petite Capitals From Capitals */
    HZ_FEATURE_C2SC = 118, /* Small Capitals From Capitals */
    HZ_FEATURE_DIST = 119, /* Distances */
    HZ_FEATURE_DLIG = 120, /* Discretionary Ligatures */
    HZ_FEATURE_DNOM = 121, /* Denominators */
    HZ_FEATURE_DTLS = 122, /* Dotless Forms */
    HZ_FEATURE_EXPT = 123, /* Expert Forms */
    HZ_FEATURE_FALT = 124, /* Final Glyph on Line Alternates */
    HZ_FEATURE_FIN2 = 125, /* Terminal Forms #2 */
    HZ_FEATURE_FIN3 = 126, /* Terminal Forms #3 */
    HZ_FEATURE_FINA = 127, /* Terminal Forms */
    HZ_FEATURE_FLAC = 128, /* Flattened accent forms */
    HZ_FEATURE_FRAC = 129, /* Fractions */
    HZ_FEATURE_FWID = 130, /* Full Widths */
    HZ_FEATURE_HALF = 131, /* Half Forms */
    HZ_FEATURE_HALN = 132, /* Halant Forms */
    HZ_FEATURE_HALT = 133, /* Alternate Half Widths */
    HZ_FEATURE_HIST = 134, /* Historical Forms */
    HZ_FEATURE_HKNA = 135, /* Horizontal Kana Alternates */
    HZ_FEATURE_HLIG = 136, /* Historical Ligaturess */
    HZ_FEATURE_HNGL = 137, /* Hangul */
    HZ_FEATURE_HOJO = 138, /* Hojo Kanji Forms (JIS X 0212-1990 Kanji Forms) */
    HZ_FEATURE_HWID = 139, /* Half Widths */
    HZ_FEATURE_INIT = 140, /* Initial Forms */
    HZ_FEATURE_ISOL = 141, /* Isolated Forms */
    HZ_FEATURE_ITAL = 142, /* Italics */
    HZ_FEATURE_JALT = 143, /* Justification Alternates */
    HZ_FEATURE_JP78 = 144, /* JIS78 Forms */
    HZ_FEATURE_JP83 = 145, /* JIS83 Forms */
    HZ_FEATURE_JP90 = 146, /* JIS90 Forms */
    HZ_FEATURE_JP04 = 147, /* JIS2004 Forms */
    HZ_FEATURE_KERN = 148, /* Kerning */
    HZ_FEATURE_LFBD = 149, /* Left Bounds */
    HZ_FEATURE_LIGA = 150, /* Standard Ligatures */
    HZ_FEATURE_LJMO = 151, /* Leading Jamo Forms */
    HZ_FEATURE_LNUM = 152, /* Lining Figures */
    HZ_FEATURE_LOCL = 153, /* Localized Forms */
    HZ_FEATURE_LTRA = 154, /* Left-to-right alternates */
    HZ_FEATURE_LTRM = 155, /* Left-to-right mirrored forms */
    HZ_FEATURE_MARK = 156, /* Mark Positioning */
    HZ_FEATURE_MED2 = 157, /* Medial Forms #2 */
    HZ_FEATURE_MEDI = 158, /* Medial Forms */
    HZ_FEATURE_MGRK = 159, /* Mathematical Greek */
    HZ_FEATURE_MKMK = 160, /* Mark to Mark Positioning */
    HZ_FEATURE_MSET = 161, /* Mark Positioning via Substitution */
    HZ_FEATURE_NALT = 162, /* Alternate Annotation Forms */
    HZ_FEATURE_NLCK = 163, /* NLC Kanji Forms */
    HZ_FEATURE_NUKT = 164, /* Nukta Forms */
    HZ_FEATURE_NUMR = 165, /* Numerators */
    HZ_FEATURE_ONUM = 166, /* Oldstyle Figures */
    HZ_FEATURE_OPBD = 167, /* Optical Bounds */
    HZ_FEATURE_ORDN = 168, /* Ordinals */
    HZ_FEATURE_ORNM = 169, /* Ornaments */
    HZ_FEATURE_PALT = 170, /* Proportional Alternate Widths */
    HZ_FEATURE_PCAP = 171, /* Petite Capitals */
    HZ_FEATURE_PKNA = 172, /* Proportional Kana */
    HZ_FEATURE_PNUM = 173, /* Proportional Figures */
    HZ_FEATURE_PREF = 174, /* Pre-Base Forms */
    HZ_FEATURE_PRES = 175, /* Pre-base Substitutions */
    HZ_FEATURE_PSTF = 176, /* Post-base Forms */
    HZ_FEATURE_PSTS = 177, /* Post-base Substitutions */
    HZ_FEATURE_PWID = 178, /* Proportional Widths */
    HZ_FEATURE_QWID = 179, /* Quarter Widths */
    HZ_FEATURE_RAND = 180, /* Randomize */
    HZ_FEATURE_RCLT = 181, /* Required Contextual Alternates */
    HZ_FEATURE_RKRF = 182, /* Rakar Forms */
    HZ_FEATURE_RLIG = 183, /* Required Ligatures */
    HZ_FEATURE_RPHF = 184, /* Reph Forms */
    HZ_FEATURE_RTBD = 185, /* Right Bounds */
    HZ_FEATURE_RTLA = 186, /* Right-to-left alternates */
    HZ_FEATURE_RTLM = 187,  /* Right-to-left mirrored forms */
    HZ_FEATURE_RUBY = 188, /* Ruby Notation Forms */
    HZ_FEATURE_RVRN = 189, /* Required Variation Alternates */
    HZ_FEATURE_SALT = 190, /* Stylistic alternates */
    HZ_FEATURE_SINF = 191, /* Scientific Inferiors */
    HZ_FEATURE_SIZE = 192, /* Optical size */
    HZ_FEATURE_SMCP = 193, /* Small Capitals */
    HZ_FEATURE_SMPL = 194, /* Simplified Forms */
    /* 	ss01 - ss20 Stylistic Sets */
    HZ_FEATURE_SSTY = 215, /* Math script style alternates */
    HZ_FEATURE_STCH = 216, /* Stretching Glyph Decomposition */
    HZ_FEATURE_SUBS = 217, /* Subscript */
    HZ_FEATURE_SUPS = 218, /* Superscript */
    HZ_FEATURE_SWSH = 219, /* Swash */
    HZ_FEATURE_TITL = 220, /* Titling */
    HZ_FEATURE_TJMO = 221, /* Trailing Jamo Forms */
    HZ_FEATURE_TNAM = 222, /* Traditional Name Forms */
    HZ_FEATURE_TNUM = 223, /* Tabular Figures */
    HZ_FEATURE_TRAD = 224, /* Traditional Forms */
    HZ_FEATURE_TWID = 225, /* Third Widths */
    HZ_FEATURE_UNIC = 226, /* Unicase */
    HZ_FEATURE_VALT = 227, /* Alternate Vertical Metrics */
    HZ_FEATURE_VATU = 228, /* Vattu Variants */
    HZ_FEATURE_VERT = 229, /* Vertical Writing */
    HZ_FEATURE_VHAL = 230, /* Alternate Vertical Half Metrics */
    HZ_FEATURE_VJMO = 231, /* Vowel Jamo Forms */
    HZ_FEATURE_VKNA = 232, /* Vertical Kana Alternates */
    HZ_FEATURE_VKRN = 233, /* Vertical Kerning */
    HZ_FEATURE_VPAL = 234, /* Proportional Alternate Vertical Metrics */
    HZ_FEATURE_VRT2 = 235, /* Vertical Alternates and Rotation */
    HZ_FEATURE_VRTR = 236, /* Vertical Alternates for Rotation */
    HZ_FEATURE_ZERO = 237, /* Slashed Zero */
    HZ_FEATURE_COUNT
} hz_feature_t;

typedef struct hz_feature_info_t {
    hz_feature_t feature;
    hz_tag tag;
} hz_feature_info_t;

static const hz_feature_info_t HZ_FEATURE_INFO_LUT[HZ_FEATURE_COUNT] = {
        {HZ_FEATURE_AALT, HZ_TAG('a', 'a', 'l', 't')}, /* Access All Alternates */
        {HZ_FEATURE_ABVF, HZ_TAG('a', 'b', 'v', 'f')}, /* Above-base forms */
        {HZ_FEATURE_ABVM, HZ_TAG('a', 'b', 'v', 'm')}, /* Above-base Mark Positioning */
        {HZ_FEATURE_ABVS, HZ_TAG('a', 'b', 'v', 's')}, /* Above-base Substitutions */
        {HZ_FEATURE_AFRC, HZ_TAG('a', 'f', 'r', 'c')}, /* Alternative Fractions */
        {HZ_FEATURE_AKHN, HZ_TAG('a', 'k', 'h', 'n')}, /* Akhands */
        {HZ_FEATURE_BLWF, HZ_TAG('b', 'l', 'w', 'f')}, /* Below-base forms */
        {HZ_FEATURE_BLWM, HZ_TAG('b', 'l', 'w', 'm')}, /* Below-base Mark Positioning */
        {HZ_FEATURE_BLWS, HZ_TAG('b', 'l', 'w', 's')}, /* Below-base Substitutions */
        {HZ_FEATURE_CALT, HZ_TAG('c', 'a', 'l', 't')}, /* Contextual Alternates */
        {HZ_FEATURE_CASE, HZ_TAG('c', 'a', 's', 'e')}, /* Case-Sensitive Forms */
        {HZ_FEATURE_CCMP, HZ_TAG('c', 'c', 'm', 'p')}, /* Glyph Composition / Decomposition */
        {HZ_FEATURE_CFAR, HZ_TAG('c', 'f', 'a', 'r')}, /* Conjunct Form After Ro */
        {HZ_FEATURE_CJCT, HZ_TAG('c', 'j', 'c', 't')}, /* Conjunct forms */
        {HZ_FEATURE_CLIG, HZ_TAG('c', 'l', 'i', 'g')}, /* Contextual Ligatures */
        {HZ_FEATURE_CPCT, HZ_TAG('c', 'p', 'c', 't')}, /* Centered CJK Punctuation */
        {HZ_FEATURE_CPSP, HZ_TAG('c', 'p', 's', 'p')}, /* Capital Spacing */
        {HZ_FEATURE_CSWH, HZ_TAG('c', 's', 'w', 'h')}, /* Contextual Swash */
        {HZ_FEATURE_CURS, HZ_TAG('c', 'u', 'r', 's')}, /* Cursive Positioning */
        /* 'cv01' – 'cv99' Character Variants */
        {HZ_FEATURE_C2PC, HZ_TAG('c', '2', 'p', 'c')}, /* Petite Capitals From Capitals */
        {HZ_FEATURE_C2SC, HZ_TAG('c', '2', 's', 'c')}, /* Small Capitals From Capitals */
        {HZ_FEATURE_DIST, HZ_TAG('d', 'i', 's', 't')}, /* Distances */
        {HZ_FEATURE_DLIG, HZ_TAG('d', 'l', 'i', 'g')}, /* Discretionary Ligatures */
        {HZ_FEATURE_DNOM, HZ_TAG('d', 'n', 'o', 'm')}, /* Denominators */
        {HZ_FEATURE_DTLS, HZ_TAG('d', 't', 'l', 's')}, /* Dotless Forms */
        {HZ_FEATURE_EXPT, HZ_TAG('e', 'x', 'p', 't')}, /* Expert Forms */
        {HZ_FEATURE_FALT, HZ_TAG('f', 'a', 'l', 't')}, /* Final Glyph on Line Alternates */
        {HZ_FEATURE_FIN2, HZ_TAG('f', 'i', 'n', '2')}, /* Terminal Forms #2 */
        {HZ_FEATURE_FIN3, HZ_TAG('f', 'i', 'n', '3')}, /* Terminal Forms #3 */
        {HZ_FEATURE_FINA, HZ_TAG('f', 'i', 'n', 'a')}, /* Terminal Forms */
        {HZ_FEATURE_FLAC, HZ_TAG('f', 'l', 'a', 'c')}, /* Flattened accent forms */
        {HZ_FEATURE_FRAC, HZ_TAG('f', 'r', 'a', 'c')}, /* Fractions */
        {HZ_FEATURE_FWID, HZ_TAG('f', 'w', 'i', 'd')}, /* Full Widths */
        {HZ_FEATURE_HALF, HZ_TAG('h', 'a', 'l', 'f')}, /* Half Forms */
        {HZ_FEATURE_HALN, HZ_TAG('h', 'a', 'l', 'n')}, /* Halant Forms */
        {HZ_FEATURE_HALT, HZ_TAG('h', 'a', 'l', 't')}, /* Alternate Half Widths */
        {HZ_FEATURE_HIST, HZ_TAG('h', 'i', 's', 't')}, /* Historical Forms */
        {HZ_FEATURE_HKNA, HZ_TAG('h', 'k', 'n', 'a')}, /* Horizontal Kana Alternates */
        {HZ_FEATURE_HLIG, HZ_TAG('h', 'l', 'i', 'g')}, /* Historical Ligaturess */
        {HZ_FEATURE_HNGL, HZ_TAG('h', 'n', 'g', 'l')}, /* Hangul */
        {HZ_FEATURE_HOJO, HZ_TAG('h', 'o', 'j', 'o')}, /* Hojo Kanji Forms (JIS X 0212-1990 Kanji Forms) */
        {HZ_FEATURE_HWID, HZ_TAG('h', 'w', 'i', 'd')}, /* Half Widths */
        {HZ_FEATURE_INIT, HZ_TAG('i', 'n', 'i', 't')}, /* Initial Forms */
        {HZ_FEATURE_ISOL, HZ_TAG('i', 's', 'o', 'l')}, /* Isolated Forms */
        {HZ_FEATURE_ITAL, HZ_TAG('i', 't', 'a', 'l')}, /* Italics */
        {HZ_FEATURE_JALT, HZ_TAG('j', 'a', 'l', 't')}, /* Justification Alternates */
        {HZ_FEATURE_JP78, HZ_TAG('j', 'p', '7', '8')}, /* JIS78 Forms */
        {HZ_FEATURE_JP83, HZ_TAG('j', 'p', '8', '3')}, /* JIS83 Forms */
        {HZ_FEATURE_JP90, HZ_TAG('j', 'p', '9', '0')}, /* JIS90 Forms */
        {HZ_FEATURE_JP04, HZ_TAG('j', 'p', '0', '4')}, /* JIS2004 Forms */
        {HZ_FEATURE_KERN, HZ_TAG('k', 'e', 'r', 'n')}, /* Kerning */
        {HZ_FEATURE_LFBD, HZ_TAG('l', 'f', 'b', 'd')}, /* Left Bounds */
        {HZ_FEATURE_LIGA, HZ_TAG('l', 'i', 'g', 'a')}, /* Standard Ligatures */
        {HZ_FEATURE_LJMO, HZ_TAG('l', 'j', 'm', 'o')}, /* Leading Jamo Forms */
        {HZ_FEATURE_LNUM, HZ_TAG('l', 'n', 'u', 'm')}, /* Lining Figures */
        {HZ_FEATURE_LOCL, HZ_TAG('l', 'o', 'c', 'l')}, /* Localized Forms */
        {HZ_FEATURE_LTRA, HZ_TAG('l', 't', 'r', 'a')}, /* Left-to-right alternates */
        {HZ_FEATURE_LTRM, HZ_TAG('l', 't', 'r', 'm')}, /* Left-to-right mirrored forms */
        {HZ_FEATURE_MARK, HZ_TAG('m', 'a', 'r', 'k')}, /* Mark Positioning */
        {HZ_FEATURE_MED2, HZ_TAG('m', 'e', 'd', '2')}, /* Medial Forms #2 */
        {HZ_FEATURE_MEDI, HZ_TAG('m', 'e', 'd', 'i')}, /* Medial Forms */
        {HZ_FEATURE_MGRK, HZ_TAG('m', 'g', 'r', 'k')}, /* Mathematical Greek */
        {HZ_FEATURE_MKMK, HZ_TAG('m', 'k', 'm', 'k')}, /* Mark to Mark Positioning */
        {HZ_FEATURE_MSET, HZ_TAG('m', 's', 'e', 't')}, /* Mark Positioning via Substitution */
        {HZ_FEATURE_NALT, HZ_TAG('n', 'a', 'l', 't')}, /* Alternate Annotation Forms */
        {HZ_FEATURE_NLCK, HZ_TAG('n', 'l', 'c', 'k')}, /* NLC Kanji Forms */
        {HZ_FEATURE_NUKT, HZ_TAG('n', 'u', 'k', 't')}, /* Nukta Forms */
        {HZ_FEATURE_NUMR, HZ_TAG('n', 'u', 'm', 'r')}, /* Numerators */
        {HZ_FEATURE_ONUM, HZ_TAG('o', 'n', 'u', 'm')}, /* Oldstyle Figures */
        {HZ_FEATURE_OPBD, HZ_TAG('o', 'p', 'b', 'd')}, /* Optical Bounds */
        {HZ_FEATURE_ORDN, HZ_TAG('o', 'r', 'd', 'n')}, /* Ordinals */
        {HZ_FEATURE_ORNM, HZ_TAG('o', 'r', 'n', 'm')}, /* Ornaments */
        {HZ_FEATURE_PALT, HZ_TAG('p', 'a', 'l', 't')}, /* Proportional Alternate Widths */
        {HZ_FEATURE_PCAP, HZ_TAG('p', 'c', 'a', 'p')}, /* Petite Capitals */
        {HZ_FEATURE_PKNA, HZ_TAG('p', 'k', 'n', 'a')}, /* Proportional Kana */
        {HZ_FEATURE_PNUM, HZ_TAG('p', 'n', 'u', 'm')}, /* Proportional Figures */
        {HZ_FEATURE_PREF, HZ_TAG('p', 'r', 'e', 'f')}, /* Pre-Base Forms */
        {HZ_FEATURE_PRES, HZ_TAG('p', 'r', 'e', 's')}, /* Pre-base Substitutions */
        {HZ_FEATURE_PSTF, HZ_TAG('p', 's', 't', 'f')}, /* Post-base Forms */
        {HZ_FEATURE_PSTS, HZ_TAG('p', 's', 't', 's')}, /* Post-base Substitutions */
        {HZ_FEATURE_PWID, HZ_TAG('p', 'w', 'i', 'd')}, /* Proportional Widths */
        {HZ_FEATURE_QWID, HZ_TAG('q', 'w', 'i', 'd')}, /* Quarter Widths */
        {HZ_FEATURE_RAND, HZ_TAG('r', 'a', 'n', 'd')}, /* Randomize */
        {HZ_FEATURE_RCLT, HZ_TAG('r', 'c', 'l', 't')}, /* Required Contextual Alternates */
        {HZ_FEATURE_RKRF, HZ_TAG('r', 'k', 'r', 'f')}, /* Rakar Forms */
        {HZ_FEATURE_RLIG, HZ_TAG('r', 'l', 'i', 'g')}, /* Required Ligatures */
        {HZ_FEATURE_RPHF, HZ_TAG('r', 'p', 'h', 'f')}, /* Reph Forms */
        {HZ_FEATURE_RTBD, HZ_TAG('r', 't', 'b', 'd')}, /* Right Bounds */
        {HZ_FEATURE_RTLA, HZ_TAG('r', 't', 'l', 'a')}, /* Right-to-left alternates */
        {HZ_FEATURE_RTLM, HZ_TAG('r', 't', 'l', 'm')},  /* Right-to-left mirrored forms */
        {HZ_FEATURE_RUBY, HZ_TAG('r', 'u', 'b', 'y')}, /* Ruby Notation Forms */
        {HZ_FEATURE_RVRN, HZ_TAG('r', 'v', 'r', 'n')}, /* Required Variation Alternates */
        {HZ_FEATURE_SALT, HZ_TAG('s', 'a', 'l', 't')}, /* Stylistic alternates */
        {HZ_FEATURE_SINF, HZ_TAG('s', 'i', 'n', 'f')}, /* Scientific Inferiors */
        {HZ_FEATURE_SIZE, HZ_TAG('s', 'i', 'z', 'e')}, /* Optical size */
        {HZ_FEATURE_SMCP, HZ_TAG('s', 'm', 'c', 'p')}, /* Small Capitals */
        {HZ_FEATURE_SMPL, HZ_TAG('s', 'm', 'p', 'l')}, /* Simplified Forms */
        /* 	ss01 - ss20 Stylistic Sets */
        {HZ_FEATURE_SSTY, HZ_TAG('s', 's', 't', 'y')}, /* Math script style alternates */
        {HZ_FEATURE_STCH, HZ_TAG('s', 't', 'c', 'h')}, /* Stretching Glyph Decomposition */
        {HZ_FEATURE_SUBS, HZ_TAG('s', 'u', 'b', 's')}, /* Subscript */
        {HZ_FEATURE_SUPS, HZ_TAG('s', 'u', 'p', 's')}, /* Superscript */
        {HZ_FEATURE_SWSH, HZ_TAG('s', 'w', 's', 'h')}, /* Swash */
        {HZ_FEATURE_TITL, HZ_TAG('t', 'i', 't', 'l')}, /* Titling */
        {HZ_FEATURE_TJMO, HZ_TAG('t', 'j', 'm', 'o')}, /* Trailing Jamo Forms */
        {HZ_FEATURE_TNAM, HZ_TAG('t', 'n', 'a', 'm')}, /* Traditional Name Forms */
        {HZ_FEATURE_TNUM, HZ_TAG('t', 'n', 'u', 'm')}, /* Tabular Figures */
        {HZ_FEATURE_TRAD, HZ_TAG('t', 'r', 'a', 'd')}, /* Traditional Forms */
        {HZ_FEATURE_TWID, HZ_TAG('t', 'w', 'i', 'd')}, /* Third Widths */
        {HZ_FEATURE_UNIC, HZ_TAG('u', 'n', 'i', 'c')}, /* Unicase */
        {HZ_FEATURE_VALT, HZ_TAG('v', 'a', 'l', 't')}, /* Alternate Vertical Metrics */
        {HZ_FEATURE_VATU, HZ_TAG('v', 'a', 't', 'u')}, /* Vattu Variants */
        {HZ_FEATURE_VERT, HZ_TAG('v', 'e', 'r', 't')}, /* Vertical Writing */
        {HZ_FEATURE_VHAL, HZ_TAG('v', 'h', 'a', 'l')}, /* Alternate Vertical Half Metrics */
        {HZ_FEATURE_VJMO, HZ_TAG('v', 'j', 'm', 'o')}, /* Vowel Jamo Forms */
        {HZ_FEATURE_VKNA, HZ_TAG('v', 'k', 'n', 'a')}, /* Vertical Kana Alternates */
        {HZ_FEATURE_VKRN, HZ_TAG('v', 'k', 'r', 'n')}, /* Vertical Kerning */
        {HZ_FEATURE_VPAL, HZ_TAG('v', 'p', 'a', 'l')}, /* Proportional Alternate Vertical Metrics */
        {HZ_FEATURE_VRT2, HZ_TAG('v', 'r', 't', '2')}, /* Vertical Alternates and Rotation */
        {HZ_FEATURE_VRTR, HZ_TAG('v', 'r', 't', 'r')}, /* Vertical Alternates for Rotation */
        {HZ_FEATURE_ZERO, HZ_TAG('z', 'e', 'r', 'o')}, /* Slashed Zero */
};

static uint32_t
hz_ot_calc_table_checksum(const uint32_t *table, uint32_t len) {
    uint32_t sum = 0;
    const uint32_t *endptr = table + ((len + 3) & ~3) / sizeof(uint32_t);

    while (table < endptr)
        sum += *table++;

    return sum;
}


/*
    TTC Header Version 2.0
*/
typedef struct HZ_PACKED hz_ttc_header_t {
    /* Font Collection ID string: 'ttcf' (used for fonts with CFF or CFF2 outlines as well as TrueType outlines) */
    hz_tag ttcTag;

    /* 	Major version of the TTC Header */
    hz_uint16 majorVersion;

    /* Minor version of the TTC Header */
    hz_uint16 minorVersion;

    /* Number of fonts in TTC */
    uint32_t numFonts;

    /* Array of offsets to the OffsetTable for each font from the beginning of the file */
    hz_offset32 *offsetTable;

    /* Tag indicating that a DSIG table exists, 0x44534947 ('DSIG') (null if no signature) */
    uint32_t dsigTag;

    /* The length (in bytes) of the DSIG table (null if no signature) */
    uint32_t dsigLength;

    /* The offset (in bytes) of the DSIG table from the beginning of the TTC file (null if no signature) */
    uint32_t dsigOffset;
} hz_ttc_header_t;
/*
typedef struct hz_font_t {
    FT_Face ft_face;
    FT_Bytes base_table;
    FT_Bytes gdef_table;
    FT_Bytes gpos_table;
    FT_Bytes gsub_table;
    FT_Bytes jstf_table;
} hz_font_t;
*/
typedef enum hz_script_t {
    HZ_SCRIPT_ARABIC,
    HZ_SCRIPT_LATIN
} hz_script_t;

typedef enum hz_language_t {
    HZ_LANGUAGE_ARABIC,
    HZ_LANGUAGE_ENGLISH,
    HZ_LANGUAGE_FRENCH,
} hz_language_t;

typedef enum hz_dir_t {
    HZ_DIR_LTR,
    HZ_DIR_RTL,
} hz_dir_t;

typedef enum hz_glyph_class_t {
    HZ_GLYPH_CLASS_ZERO          = 0,
    HZ_GLYPH_CLASS_BASE_BIT      = 0x1,
    HZ_GLYPH_CLASS_LIGATURE_BIT  = 0x2,
    HZ_GLYPH_CLASS_MARK_BIT      = 0x4,
    HZ_GLYPH_CLASS_COMPONENT_BIT = 0x8,
} hz_glyph_class_t;

#define HZ_GLYPH_CLASS_BIT_FIELD 4
#define HZ_BIT(x) (1 << (x))


typedef struct hz_section_node_t hz_section_node_t;

typedef struct hz_section_glyph_t {
    hz_unicode codepoint;
    hz_id id;
    uint16_t dx, dy;
    uint16_t ax, ay;
    uint8_t g_class: HZ_GLYPH_CLASS_BIT_FIELD;
} hz_section_glyph_t;

struct hz_section_node_t {
    hz_section_node_t *prev, *next;
    hz_section_glyph_t data;
};

typedef struct hz_section_t {
    hz_section_node_t *root;
    unsigned int flags;
} hz_section_t;

static hz_language_t
hz_lang(const char *s) {
    if (!strcmp(s, "ar")) return HZ_LANGUAGE_ARABIC;
    else if (!strcmp(s, "en")) return HZ_LANGUAGE_ENGLISH;
}

#define HZ_LANG(lang_str) hz_lang(lang_str)

static hz_section_t *
hz_section_create(void) {
    hz_section_t *sect = (hz_section_t *) HZ_MALLOC(sizeof(hz_section_t));
    sect->root = NULL;
    return sect;
}

static void
hz_section_add(hz_section_t *sect, const hz_section_glyph_t *node_data)
{
    hz_section_node_t *new_node = (hz_section_node_t *) HZ_MALLOC(sizeof(hz_section_node_t));
    new_node->data = *node_data;
    new_node->prev = NULL;
    new_node->next = NULL;

    if (sect->root == NULL)
        sect->root = new_node;
    else {
        hz_section_node_t *curr_node;
        curr_node = sect->root;
        while (curr_node->next != NULL) {
            curr_node = curr_node->next;
        }

        curr_node->next = new_node;
        new_node->prev = curr_node;
    }
}

static size_t
hz_section_node_count(hz_section_node_t *node) {
    size_t count = 0;

    while (node != NULL) {
        ++count;
        node = node->next;
    }

    return count;
}

static hz_section_node_t *
hz_section_last_node(hz_section_node_t *node) {
    while (node->next != NULL) {
        node = node->next;
    }

    return node;
}

static hz_bool
hz_section_rem_n_next_nodes(hz_section_node_t *start, size_t n)
{
    /* assumes n passed in doesn't cause segfault :) */
    hz_section_node_t *curr = start->next, *prev;
    size_t i = 0;

    while (i < n && curr != NULL) {
        curr = curr->next;

        if (curr == NULL) break;
        prev = curr->prev;

        ++i;
    }

    start->next = curr;
    if (curr != NULL) curr->prev = start;
}

typedef struct {
    const hz_byte *mem;
    hz_size length;
    hz_size offset;
} hz_utf8_dec_t;

#define HZ_UTF8_END -1
#define HZ_UTF8_ERROR -2

static int
hz_utf8_get(hz_utf8_dec_t *dec) {
    if (dec->offset >= dec->length)
        return HZ_UTF8_END;

    return dec->mem[dec->offset++];
}

static int
hz_utf8_cont(hz_utf8_dec_t *dec) {
    int c = hz_utf8_get(dec);
    return ((c & 0xC0) == 0x80)
           ? (c & 0x3F)
           : HZ_UTF8_ERROR;
}

static int
hz_utf8_next(hz_utf8_dec_t *dec) {
    int c;
    int c1;
    int c2;
    int c3;
    int r;
    c = hz_utf8_get(dec);

    /*
        Zero continuation (0 to 127)
    */
    if ((c & 0x80) == 0) {
        return c;
    }

    /*
        One continuation (128 to 2047)
    */
    if ((c & 0xE0) == 0xC0) {
        c1 = hz_utf8_cont(dec);
        if (c1 >= 0) {
            r = ((c & 0x1F) << 6) | c1;
            if (r >= 128) {
                return r;
            }
        }

        /*
            Two continuations (2048 to 55295 and 57344 to 65535)
        */
    } else if ((c & 0xF0) == 0xE0) {
        c1 = hz_utf8_cont(dec);
        c2 = hz_utf8_cont(dec);
        if ((c1 | c2) >= 0) {
            r = ((c & 0x0F) << 12) | (c1 << 6) | c2;
            if (r >= 2048 && (r < 55296 || r > 57343)) {
                return r;
            }
        }

        /*
            Three continuations (65536 to 1114111)
        */
    } else if ((c & 0xF8) == 0xF0) {
        c1 = hz_utf8_cont(dec);
        c2 = hz_utf8_cont(dec);
        c3 = hz_utf8_cont(dec);
        if ((c1 | c2 | c3) >= 0) {
            r = ((c & 0x07) << 18) | (c1 << 12) | (c2 << 6) | c3;
            if (r >= 65536 && r <= 1114111) {
                return r;
            }
        }
    }

    return HZ_UTF8_ERROR;
}


static void
hz_section_load_utf8(hz_section_t *sect, const hz_char *text, size_t len) {
    hz_unicode code;
    int ch;

    hz_utf8_dec_t dec;
    dec.mem = (const hz_byte *) text;
    dec.length = len;
    dec.offset = 0;

    /* TODO: do proper error handling for the UTF-8 decoder */
    while ((ch = hz_utf8_next(&dec)) > 0) {
        hz_section_glyph_t data;
        data.codepoint = ch;
        data.id = 0;
        data.g_class = HZ_GLYPH_CLASS_ZERO;
        data.ax = 0;
        data.ay = 0;
        data.dx = 0;
        data.dy = 0;
        hz_section_add(sect, &data);
    }
}

static void
hz_section_load_utf8_zt(hz_section_t *sect, const hz_char *text) {
    hz_section_load_utf8(sect, text, UINT64_MAX);
}


typedef struct hz_rec16_t {
    hz_tag tag;
    hz_offset16 offset;
} hz_rec16_t;

typedef struct hz_rec32_t {
    hz_tag tag;
    hz_offset32 offset;
} hz_rec32_t;

typedef struct hz_script_list_t {
    hz_uint16 scriptCount;
    //hz_rec32_t *scriptRecords;
} hz_script_list_t;

typedef struct hz_script_table_t {
    hz_offset16 defaultLangSys;
    hz_uint16 langSysCount;
    //hz_rec16_t *langSysRecords;
} hz_script_table_t;

typedef struct hz_lang_sys_t {
    /* = NULL (reserved for an offset to a reordering table) */
    hz_offset16 lookupOrder;

    /* Index of a feature required for this language system; if no required features = 0xFFFF */
    hz_uint16 requiredFeatureIndex;

    /* Number of feature index values for this language system — excludes the required feature */
    hz_uint16 featureIndexCount;

    /* Array of indices into the FeatureList, in arbitrary order */
    //HZ_Uint16 *featureIndices;
} hz_lang_sys_t;

typedef struct hz_feature_list_t {
    hz_uint16 featureCount;
    hz_rec16_t *featureRecord;
} hz_feature_list_t;

typedef struct hz_feature_table_t {
    /* = NULL (reserved for offset to FeatureParams) */
    hz_offset16 feature_params;

    /* Number of LookupList indices for this feature */
    uint16_t lookup_index_count;

    /* 	Array of indices into the LookupList — zero-based (first lookup is LookupListIndex = 0) */
//    uint16_t *lookup_list_indices;
} hz_feature_table_t;


struct hz_lookup_list_t {
    /* Number of lookups in this table */
    hz_uint16 lookupCount;

    /* Array of offsets to Lookup tables, from beginning of LookupList — zero based (first lookup is Lookup index = 0) */
    //HZ_Offset16 *lookups;
};
/*
static const char *hz_GSUB_lookup_type_string(HZ_Uint16 type);
static const char *hz_GPOS_lookup_type_string(HZ_Uint16 type);
*/


typedef enum hz_lookup_flag_t {
    /*
    This bit relates only to
    the correct processing of the cursive attachment lookup type (GPOS lookup type 3).
    When this bit is set, the last glyph in a given sequence to
    which the cursive attachment lookup is applied, will be positioned on the baseline.
    */
    HZ_LOOKUP_FLAG_RIGHT_TO_LEFT = 0x0001,

    /* If set, skips over base glyphs */
    HZ_LOOKUP_FLAG_IGNORE_BASE_GLYPHS = 0x0002,

    /* If set, skips over ligatures */
    HZ_LOOKUP_FLAG_IGNORE_LIGATURES = 0x0004,

    /* If set, skips over all combining marks */
    HZ_LOOKUP_FLAG_IGNORE_MARKS = 0x0008,

    /*
    If set, indicates that the lookup table structure is followed by a MarkFilteringSet field.
    The layout engine skips over all mark glyphs not in the mark filtering set indicated.
    */
    HZ_LOOKUP_FLAG_USE_MARK_FILTERING_SET = 0x0010,

    /* 0x00E0 - For future use (Set to zero) */

    /* If not zero, skips over all marks of attachment type different from specified. */
    HZ_LOOKUP_FLAG_MARK_ATTACHMENT_TYPE_MASK = 0xFF00
} hz_lookup_flag_t;

typedef enum hz_gsub_lookup_type_t {
    HZ_GSUB_LOOKUP_TYPE_SINGLE_SUBSTITUTION = 1,
    HZ_GSUB_LOOKUP_TYPE_MULTIPLE_SUBSTITUTION = 2,
    HZ_GSUB_LOOKUP_TYPE_ALTERNATE_SUBSTITUTION = 3,
    HZ_GSUB_LOOKUP_TYPE_LIGATURE_SUBSTITUTION = 4,
    HZ_GSUB_LOOKUP_TYPE_CONTEXTUAL_SUBSTITUTION = 5,
    HZ_GSUB_LOOKUP_TYPE_CHAINED_CONTEXTS_SUBSTITUTION = 6,
    HZ_GSUB_LOOKUP_TYPE_EXTENSION_SUBSTITUTION = 7,
    HZ_GSUB_LOOKUP_TYPE_REVERSE_CHAINING_CONTEXTUAL_SINGLE_SUBSTITUTION = 8,
} hz_gsub_lookup_type_t;

typedef enum hz_gpos_lookup_type_t {
    HZ_GPOS_LOOKUP_TYPE_SINGLE_ADJUSTMENT = 1,
    HZ_GPOS_LOOKUP_TYPE_PAIR_ADJUSTMENT = 2,
    HZ_GPOS_LOOKUP_TYPE_CURSIVE_ATTACHMENT = 3,
    HZ_GPOS_LOOKUP_TYPE_MARK_TO_BASE_ATTACHMENT = 4,
    HZ_GPOS_LOOKUP_TYPE_MARK_TO_LIGATURE_ATTACHMENT = 5,
    HZ_GPOS_LOOKUP_TYPE_MARK_TO_MARK_ATTACHMENT = 6,
    HZ_GPOS_LOOKUP_TYPE_CONTEXT_POSITIONING = 7,
    HZ_GPOS_LOOKUP_TYPE_CHAINED_CONTEXT_POSITIONING = 8,
    HZ_GPOS_LOOKUP_TYPE_EXTENSION_POSITIONING = 9,
} hz_gpos_lookup_type_t;

typedef struct hz_lookup_table_t {
    uint16_t lookup_type;
    uint16_t lookup_flag;
    uint16_t subtable_count;
    hz_offset32 *subtable_offsets;
    uint16_t mark_filtering_set;
} hz_lookup_table_t;

typedef struct hz_coverage_format1_t {
    hz_uint16 coverageFormat; /* Format identifier — format = 1 */
    hz_uint16 glyphCount; /* Number of glyphs in the glyph array */
    hz_id *glyphArray; /* Array of glyph IDs — in numerical order */
} hz_coverage_format1_t;

typedef struct hz_range_rec_t {
    hz_id start_glyph_id;
    hz_id end_glyph_id;
    uint16_t start_coverage_index;
} hz_range_rec_t;

typedef struct hz_coverage_format2_t {
    hz_uint16 coverageFormat; /* Format identifier — format = 2 */
    hz_uint16 rangeCount; /* Number of RangeRecords */
    hz_range_rec_t *rangeRecords; /* Array of glyph ranges — ordered by startGlyphID. */
} hz_coverage_format2_t;


struct hz_class_def_format1_t {
    hz_uint16 format; /* Format identifier — format = 1 */
    hz_id startGID; /* First glyph ID of the classValueArray */
    hz_uint16 glyphCount; /* Size of the classValueArray */
    hz_uint16 *classValues; /* Array of Class Values — one per glyph ID */
};

struct hz_class_range_rec_t {
    hz_id startGlyphID;
    hz_id endGlyphID;
    hz_uint16 classValue;
};

struct hz_class_def_format2_t {
    hz_uint16 format;
    hz_uint16 ranageCount;
    struct hz_class_range_rec_t *rangeRecords;
};

typedef enum hz_delta_format_t {
    /* Signed 2-bit value, 8 values per uint16 */
    HZ_DELTA_FORMAT_LOCAL_2_BIT = 0x0001,

    /* Signed 4-bit value, 4 values per uint16 */
    HZ_DELTA_FORMAT_LOCAL_4_BIT = 0x0002,

    /* Signed 8-bit value, 2 values per uint16 */
    HZ_DELTA_FORMAT_LOCAL_8_BIT = 0x0003,

    /* VariationIndex table, contains a delta-set index pair. */
    HZ_DELTA_FORMAT_VARIATION_INDEX = 0x8000,

    /* 0x7FFC Reserved */
} hz_delta_format_t;

typedef struct HZ_PACKED hz_delta_table_t {
    hz_uint16 smallestPPEM;
    hz_uint16 largestPPEM;
    hz_delta_format_t format;
    hz_uint16 *deltaValues;
} hz_delta_table_t;

typedef struct HZ_PACKED hz_variation_index_table_t {
    /* A delta-set outer index — used to select an item variation data subtable within the item variation store. */
    hz_uint16 outerIndex;

    /* A delta-set inner index — used to select a delta-set row within an item variation data subtable. */
    hz_uint16 innerIndex;

    /* Format, = 0x8000 */
    hz_delta_format_t format;
} hz_variation_index_table_t;

typedef struct HZ_PACKED hz_feature_variation_record_t {
    /* Offset to a condition set table, from beginning of FeatureVariations table. */
    hz_offset32 conditionSetOffset;

    /*
        Offset to a feature table substitution table,
        from beginning of the FeatureVariations table.
    */
    hz_offset32 featureTableSubstitutionOffset;
} hz_feature_variation_record_t;


typedef struct HZ_PACKED hz_feature_variations_table_t {
    /* Major version of the FeatureVariations table — set to 1. */
    hz_uint16 versionMajor;

    /* Minor version of the FeatureVariations table — set to 0. */
    hz_uint16 versionMinor;

    /* Number of feature variation records. */
    hz_uint16 featureVariationRecordCount;

    /* Array of feature variation records. */
    hz_feature_variation_record_t *featureVariationRecord;
} hz_feature_variations_table_t;

typedef struct HZ_PACKED hz_condition_set_t {
    /* Number of conditions for this condition set. */
    hz_uint16 conditionCount;

    /* Array of offsets to condition tables, from beginning of the ConditionSet table. */
    hz_offset32 *conditions;
} hz_condition_set_t;

typedef struct HZ_PACKED hz_condition_format1_t {
    /* Format, = 1 */
    hz_uint16 format;

    /* Index (zero-based) for the variation axis within the 'fvar' table. */
    hz_uint16 axisIndex;

    /* Minimum value of the font variation instances that satisfy this condition. */
    HZ_F2DOT14 filterRangeMinValue;

    /* Maximum value of the font variation instances that satisfy this condition. */
    HZ_F2DOT14 filterRangeMaxValue;
} hz_condition_format1_t;

typedef struct HZ_PACKED hz_feature_table_substitution_record_t {
    /* The feature table index to match. */
    hz_uint16 featureIndex;

    /* Offset to an alternate feature table, from start of the FeatureTableSubstitution table. */
    hz_offset32 alternateFeatureOffset;
} hz_feature_table_substitution_record_t;

typedef struct HZ_PACKED hz_feature_table_substitution_table_t {
    /* Major version of the feature table substitution table — set to 1 */
    hz_uint16 majorVersion;

    /* Minor version of the feature table substitution table — set to 0. */
    hz_uint16 minorVersion;

    /* Number of feature table substitution records. */
    hz_uint16 substitutionCount;

    /* Array of feature table substitution records. */
    hz_feature_table_substitution_record_t *substitutions;
} hz_feature_table_substitution_table_t;

typedef struct hz_bitset_t {
    uint8_t *data;
    uint16_t bit_count;
} hz_bitset_t;

static hz_uint64 hz_next_pow2m1(hz_uint64 x) {
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x |= x >> 32;
    return x;
}

static hz_bitset_t *
hz_bitset_create(uint16_t bit_count) {
    hz_bitset_t *bitset = (hz_bitset_t *) malloc(sizeof(hz_bitset_t));

    bitset->bit_count = bit_count;
    uint16_t byte_count = (bit_count / 8) + 1;

    bitset->data = (uint8_t *) calloc(byte_count, 1);

    return bitset;
}

static void
hz_bitset_destroy(hz_bitset_t *bitset) {
    free(bitset);
}

static hz_bool
hz_bitset_set(hz_bitset_t *bitset, uint16_t index, hz_bool value) {
    if (index < bitset->bit_count) {
        uint8_t *byte = bitset->data + (index / 8);
        uint8_t mask = 1 << (index % 8);

        if (value == HZ_TRUE)
            *byte |= mask;
        else if (value == HZ_FALSE)
            *byte &= ~mask;

        return HZ_TRUE;
    }

    return HZ_FALSE;
}

static hz_bool
hz_bitset_check(const hz_bitset_t *bitset, uint16_t index) {
    hz_bool value = HZ_FALSE;

    if (index < bitset->bit_count) {
        uint8_t byte = *(bitset->data + (index / 8));
        uint8_t mask = 1 << (index % 8);

        if (byte & mask)
            value = HZ_TRUE;
    }

    return value;
}

static void
hz_bitset_copy(hz_bitset_t *dst, const hz_bitset_t *src) {
    size_t byte_count = src->bit_count / 8;
    HZ_ASSERT(src->bit_count == dst->bit_count);

    memcpy(dst->data, src->data, byte_count);
}


#define HZ_OT_TAG_GSUB HZ_TAG('G','S','U','B')
#define HZ_OT_TAG_GPOS HZ_TAG('G','P','O','S')
#define HZ_OT_TAG_GDEF HZ_TAG('G','D','E','F')
#define HZ_OT_TAG_JSTF HZ_TAG('J','S','T','F')


hz_feature_t
hz_ot_feature_from_tag(hz_tag tag);


hz_bool
hz_ot_layout_apply_gsub_features(hz_face_t *face,
                                 hz_tag script,
                                 hz_tag language,
                                 const hz_bitset_t *feature_bits,
                                 hz_section_t *sect);

hz_bool
hz_ot_layout_apply_gpos_features(hz_face_t *face,
                                 hz_tag script,
                                 hz_tag language,
                                 const hz_bitset_t *feature_bits,
                                 hz_section_t *sect);

void
hz_ot_layout_lookups_substitute_closure(hz_face_t *face,
                                          const hz_set_t *lookups,
                                          hz_set_t *glyphs);


hz_bool
hz_ot_layout_lookup_would_substitute(hz_face_t *face,
                                       unsigned int lookup_index,
                                       const hz_id *glyphs,
                                       unsigned int glyph_count,
                                       hz_bool zero_context);


void
hz_ot_layout_apply_gsub_lookup(hz_face_t *face,
                               hz_stream_t *table,
                               hz_feature_t feature,
                               hz_section_t *sect);
void
hz_ot_layout_apply_gpos_lookup(hz_face_t *face,
                               hz_stream_t *table,
                               hz_feature_t feature,
                               hz_section_t *sect);

hz_tag
hz_ot_script_to_tag(hz_script_t script);

hz_tag
hz_ot_language_to_tag(hz_language_t language);

#ifdef __cplusplus
}
#endif

#endif /* HZ_OT_H */