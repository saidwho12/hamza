#ifndef GURU_OT_H
#define GURU_OT_H

#include "guru-base.h"
#include "guru-set.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Registered Features
 * https://docs.microsoft.com/en-us/typography/opentype/spec/featurelist
 *
 */
typedef enum guru_feature_t {
    GURU_FEATURE_AALT = 0, /* Access All Alternates */
    GURU_FEATURE_ABVF = 1, /* Above-base forms */
    GURU_FEATURE_ABVM = 2, /* Above-base Mark Positioning */
    GURU_FEATURE_ABVS = 3, /* Above-base Substitutions */
    GURU_FEATURE_AFRC = 4, /* Alternative Fractions */
    GURU_FEATURE_AKHN = 5, /* Akhands */
    GURU_FEATURE_BLWF = 6, /* Below-base forms */
    GURU_FEATURE_BLWM = 7, /* Below-base Mark Positioning */
    GURU_FEATURE_BLWS = 8, /* Below-base Substitutions */
    GURU_FEATURE_CALT = 9, /* Contextual Alternates */
    GURU_FEATURE_CASE = 10, /* Case-Sensitive Forms */
    GURU_FEATURE_CCMP = 11, /* Glyph Composition / Decomposition */
    GURU_FEATURE_CFAR = 12, /* Conjunct Form After Ro */
    GURU_FEATURE_CJCT = 13, /* Conjunct forms */
    GURU_FEATURE_CLIG = 14, /* Contextual Ligatures */
    GURU_FEATURE_CPCT = 15, /* Centered CJK Punctuation */
    GURU_FEATURE_CPSP = 16, /* Capital Spacing */
    GURU_FEATURE_CSWH = 17, /* Contextual Swash */
    GURU_FEATURE_CURS = 18, /* Cursive Positioning */
    /* 'cv01' – 'cv99' Character Variants */
    GURU_FEATURE_C2PC = 117, /* Petite Capitals From Capitals */
    GURU_FEATURE_C2SC = 118, /* Small Capitals From Capitals */
    GURU_FEATURE_DIST = 119, /* Distances */
    GURU_FEATURE_DLIG = 120, /* Discretionary Ligatures */
    GURU_FEATURE_DNOM = 121, /* Denominators */
    GURU_FEATURE_DTLS = 122, /* Dotless Forms */
    GURU_FEATURE_EXPT = 123, /* Expert Forms */
    GURU_FEATURE_FALT = 124, /* Final Glyph on Line Alternates */
    GURU_FEATURE_FIN2 = 125, /* Terminal Forms #2 */
    GURU_FEATURE_FIN3 = 126, /* Terminal Forms #3 */
    GURU_FEATURE_FINA = 127, /* Terminal Forms */
    GURU_FEATURE_FLAC = 128, /* Flattened accent forms */
    GURU_FEATURE_FRAC = 129, /* Fractions */
    GURU_FEATURE_FWID = 130, /* Full Widths */
    GURU_FEATURE_HALF = 131, /* Half Forms */
    GURU_FEATURE_HALN = 132, /* Halant Forms */
    GURU_FEATURE_HALT = 133, /* Alternate Half Widths */
    GURU_FEATURE_HIST = 134, /* Historical Forms */
    GURU_FEATURE_HKNA = 135, /* Horizontal Kana Alternates */
    GURU_FEATURE_HLIG = 136, /* Historical Ligaturess */
    GURU_FEATURE_HNGL = 137, /* Hangul */
    GURU_FEATURE_HOJO = 138, /* Hojo Kanji Forms (JIS X 0212-1990 Kanji Forms) */
    GURU_FEATURE_HWID = 139, /* Half Widths */
    GURU_FEATURE_INIT = 140, /* Initial Forms */
    GURU_FEATURE_ISOL = 141, /* Isolated Forms */
    GURU_FEATURE_ITAL = 142, /* Italics */
    GURU_FEATURE_JALT = 143, /* Justification Alternates */
    GURU_FEATURE_JP78 = 144, /* JIS78 Forms */
    GURU_FEATURE_JP83 = 145, /* JIS83 Forms */
    GURU_FEATURE_JP90 = 146, /* JIS90 Forms */
    GURU_FEATURE_JP04 = 147, /* JIS2004 Forms */
    GURU_FEATURE_KERN = 148, /* Kerning */
    GURU_FEATURE_LFBD = 149, /* Left Bounds */
    GURU_FEATURE_LIGA = 150, /* Standard Ligatures */
    GURU_FEATURE_LJMO = 151, /* Leading Jamo Forms */
    GURU_FEATURE_LNUM = 152, /* Lining Figures */
    GURU_FEATURE_LOCL = 153, /* Localized Forms */
    GURU_FEATURE_LTRA = 154, /* Left-to-right alternates */
    GURU_FEATURE_LTRM = 155, /* Left-to-right mirrored forms */
    GURU_FEATURE_MARK = 156, /* Mark Positioning */
    GURU_FEATURE_MED2 = 157, /* Medial Forms #2 */
    GURU_FEATURE_MEDI = 158, /* Medial Forms */
    GURU_FEATURE_MGRK = 159, /* Mathematical Greek */
    GURU_FEATURE_MKMK = 160, /* Mark to Mark Positioning */
    GURU_FEATURE_MSET = 161, /* Mark Positioning via Substitution */
    GURU_FEATURE_NALT = 162, /* Alternate Annotation Forms */
    GURU_FEATURE_NLCK = 163, /* NLC Kanji Forms */
    GURU_FEATURE_NUKT = 164, /* Nukta Forms */
    GURU_FEATURE_NUMR = 165, /* Numerators */
    GURU_FEATURE_ONUM = 166, /* Oldstyle Figures */
    GURU_FEATURE_OPBD = 167, /* Optical Bounds */
    GURU_FEATURE_ORDN = 168, /* Ordinals */
    GURU_FEATURE_ORNM = 169, /* Ornaments */
    GURU_FEATURE_PALT = 170, /* Proportional Alternate Widths */
    GURU_FEATURE_PCAP = 171, /* Petite Capitals */
    GURU_FEATURE_PKNA = 172, /* Proportional Kana */
    GURU_FEATURE_PNUM = 173, /* Proportional Figures */
    GURU_FEATURE_PREF = 174, /* Pre-Base Forms */
    GURU_FEATURE_PRES = 175, /* Pre-base Substitutions */
    GURU_FEATURE_PSTF = 176, /* Post-base Forms */
    GURU_FEATURE_PSTS = 177, /* Post-base Substitutions */
    GURU_FEATURE_PWID = 178, /* Proportional Widths */
    GURU_FEATURE_QWID = 179, /* Quarter Widths */
    GURU_FEATURE_RAND = 180, /* Randomize */
    GURU_FEATURE_RCLT = 181, /* Required Contextual Alternates */
    GURU_FEATURE_RKRF = 182, /* Rakar Forms */
    GURU_FEATURE_RLIG = 183, /* Required Ligatures */
    GURU_FEATURE_RPHF = 184, /* Reph Forms */
    GURU_FEATURE_RTBD = 185, /* Right Bounds */
    GURU_FEATURE_RTLA = 186, /* Right-to-left alternates */
    GURU_FEATURE_RTLM = 187,  /* Right-to-left mirrored forms */
    GURU_FEATURE_RUBY = 188, /* Ruby Notation Forms */
    GURU_FEATURE_RVRN = 189, /* Required Variation Alternates */
    GURU_FEATURE_SALT = 190, /* Stylistic alternates */
    GURU_FEATURE_SINF = 191, /* Scientific Inferiors */
    GURU_FEATURE_SIZE = 192, /* Optical size */
    GURU_FEATURE_SMCP = 193, /* Small Capitals */
    GURU_FEATURE_SMPL = 194, /* Simplified Forms */
    /* 	ss01 - ss20 Stylistic Sets */
    GURU_FEATURE_SSTY = 215, /* Math script style alternates */
    GURU_FEATURE_STCH = 216, /* Stretching Glyph Decomposition */
    GURU_FEATURE_SUBS = 217, /* Subscript */
    GURU_FEATURE_SUPS = 218, /* Superscript */
    GURU_FEATURE_SWSH = 219, /* Swash */
    GURU_FEATURE_TITL = 220, /* Titling */
    GURU_FEATURE_TJMO = 221, /* Trailing Jamo Forms */
    GURU_FEATURE_TNAM = 222, /* Traditional Name Forms */
    GURU_FEATURE_TNUM = 223, /* Tabular Figures */
    GURU_FEATURE_TRAD = 224, /* Traditional Forms */
    GURU_FEATURE_TWID = 225, /* Third Widths */
    GURU_FEATURE_UNIC = 226, /* Unicase */
    GURU_FEATURE_VALT = 227, /* Alternate Vertical Metrics */
    GURU_FEATURE_VATU = 228, /* Vattu Variants */
    GURU_FEATURE_VERT = 229, /* Vertical Writing */
    GURU_FEATURE_VHAL = 230, /* Alternate Vertical Half Metrics */
    GURU_FEATURE_VJMO = 231, /* Vowel Jamo Forms */
    GURU_FEATURE_VKNA = 232, /* Vertical Kana Alternates */
    GURU_FEATURE_VKRN = 233, /* Vertical Kerning */
    GURU_FEATURE_VPAL = 234, /* Proportional Alternate Vertical Metrics */
    GURU_FEATURE_VRT2 = 235, /* Vertical Alternates and Rotation */
    GURU_FEATURE_VRTR = 236, /* Vertical Alternates for Rotation */
    GURU_FEATURE_ZERO = 237, /* Slashed Zero */
    GURU_FEATURE_COUNT
} guru_feature_t;

typedef struct guru_feature_ext_t {
    guru_feature_t feature;
    guru_tag tag;
} guru_feature_ext_t;

static const guru_feature_ext_t GURU_FEATURE_EXT_LUT[GURU_FEATURE_COUNT] = {
        {GURU_FEATURE_AALT, GURU_TAG('a', 'a', 'l', 't')}, /* Access All Alternates */
        {GURU_FEATURE_ABVF, GURU_TAG('a', 'b', 'v', 'f')}, /* Above-base forms */
        {GURU_FEATURE_ABVM, GURU_TAG('a', 'b', 'v', 'm')}, /* Above-base Mark Positioning */
        {GURU_FEATURE_ABVS, GURU_TAG('a', 'b', 'v', 's')}, /* Above-base Substitutions */
        {GURU_FEATURE_AFRC, GURU_TAG('a', 'f', 'r', 'c')}, /* Alternative Fractions */
        {GURU_FEATURE_AKHN, GURU_TAG('a', 'k', 'h', 'n')}, /* Akhands */
        {GURU_FEATURE_BLWF, GURU_TAG('b', 'l', 'w', 'f')}, /* Below-base forms */
        {GURU_FEATURE_BLWM, GURU_TAG('b', 'l', 'w', 'm')}, /* Below-base Mark Positioning */
        {GURU_FEATURE_BLWS, GURU_TAG('b', 'l', 'w', 's')}, /* Below-base Substitutions */
        {GURU_FEATURE_CALT, GURU_TAG('c', 'a', 'l', 't')}, /* Contextual Alternates */
        {GURU_FEATURE_CASE, GURU_TAG('c', 'a', 's', 'e')}, /* Case-Sensitive Forms */
        {GURU_FEATURE_CCMP, GURU_TAG('c', 'c', 'm', 'p')}, /* Glyph Composition / Decomposition */
        {GURU_FEATURE_CFAR, GURU_TAG('c', 'f', 'a', 'r')}, /* Conjunct Form After Ro */
        {GURU_FEATURE_CJCT, GURU_TAG('c', 'j', 'c', 't')}, /* Conjunct forms */
        {GURU_FEATURE_CLIG, GURU_TAG('c', 'l', 'i', 'g')}, /* Contextual Ligatures */
        {GURU_FEATURE_CPCT, GURU_TAG('c', 'p', 'c', 't')}, /* Centered CJK Punctuation */
        {GURU_FEATURE_CPSP, GURU_TAG('c', 'p', 's', 'p')}, /* Capital Spacing */
        {GURU_FEATURE_CSWH, GURU_TAG('c', 's', 'w', 'h')}, /* Contextual Swash */
        {GURU_FEATURE_CURS, GURU_TAG('c', 'u', 'r', 's')}, /* Cursive Positioning */
        /* 'cv01' – 'cv99' Character Variants */
        {GURU_FEATURE_C2PC, GURU_TAG('c', '2', 'p', 'c')}, /* Petite Capitals From Capitals */
        {GURU_FEATURE_C2SC, GURU_TAG('c', '2', 's', 'c')}, /* Small Capitals From Capitals */
        {GURU_FEATURE_DIST, GURU_TAG('d', 'i', 's', 't')}, /* Distances */
        {GURU_FEATURE_DLIG, GURU_TAG('d', 'l', 'i', 'g')}, /* Discretionary Ligatures */
        {GURU_FEATURE_DNOM, GURU_TAG('d', 'n', 'o', 'm')}, /* Denominators */
        {GURU_FEATURE_DTLS, GURU_TAG('d', 't', 'l', 's')}, /* Dotless Forms */
        {GURU_FEATURE_EXPT, GURU_TAG('e', 'x', 'p', 't')}, /* Expert Forms */
        {GURU_FEATURE_FALT, GURU_TAG('f', 'a', 'l', 't')}, /* Final Glyph on Line Alternates */
        {GURU_FEATURE_FIN2, GURU_TAG('f', 'i', 'n', '2')}, /* Terminal Forms #2 */
        {GURU_FEATURE_FIN3, GURU_TAG('f', 'i', 'n', '3')}, /* Terminal Forms #3 */
        {GURU_FEATURE_FINA, GURU_TAG('f', 'i', 'n', 'a')}, /* Terminal Forms */
        {GURU_FEATURE_FLAC, GURU_TAG('f', 'l', 'a', 'c')}, /* Flattened accent forms */
        {GURU_FEATURE_FRAC, GURU_TAG('f', 'r', 'a', 'c')}, /* Fractions */
        {GURU_FEATURE_FWID, GURU_TAG('f', 'w', 'i', 'd')}, /* Full Widths */
        {GURU_FEATURE_HALF, GURU_TAG('h', 'a', 'l', 'f')}, /* Half Forms */
        {GURU_FEATURE_HALN, GURU_TAG('h', 'a', 'l', 'n')}, /* Halant Forms */
        {GURU_FEATURE_HALT, GURU_TAG('h', 'a', 'l', 't')}, /* Alternate Half Widths */
        {GURU_FEATURE_HIST, GURU_TAG('h', 'i', 's', 't')}, /* Historical Forms */
        {GURU_FEATURE_HKNA, GURU_TAG('h', 'k', 'n', 'a')}, /* Horizontal Kana Alternates */
        {GURU_FEATURE_HLIG, GURU_TAG('h', 'l', 'i', 'g')}, /* Historical Ligaturess */
        {GURU_FEATURE_HNGL, GURU_TAG('h', 'n', 'g', 'l')}, /* Hangul */
        {GURU_FEATURE_HOJO, GURU_TAG('h', 'o', 'j', 'o')}, /* Hojo Kanji Forms (JIS X 0212-1990 Kanji Forms) */
        {GURU_FEATURE_HWID, GURU_TAG('h', 'w', 'i', 'd')}, /* Half Widths */
        {GURU_FEATURE_INIT, GURU_TAG('i', 'n', 'i', 't')}, /* Initial Forms */
        {GURU_FEATURE_ISOL, GURU_TAG('i', 's', 'o', 'l')}, /* Isolated Forms */
        {GURU_FEATURE_ITAL, GURU_TAG('i', 't', 'a', 'l')}, /* Italics */
        {GURU_FEATURE_JALT, GURU_TAG('j', 'a', 'l', 't')}, /* Justification Alternates */
        {GURU_FEATURE_JP78, GURU_TAG('j', 'p', '7', '8')}, /* JIS78 Forms */
        {GURU_FEATURE_JP83, GURU_TAG('j', 'p', '8', '3')}, /* JIS83 Forms */
        {GURU_FEATURE_JP90, GURU_TAG('j', 'p', '9', '0')}, /* JIS90 Forms */
        {GURU_FEATURE_JP04, GURU_TAG('j', 'p', '0', '4')}, /* JIS2004 Forms */
        {GURU_FEATURE_KERN, GURU_TAG('k', 'e', 'r', 'n')}, /* Kerning */
        {GURU_FEATURE_LFBD, GURU_TAG('l', 'f', 'b', 'd')}, /* Left Bounds */
        {GURU_FEATURE_LIGA, GURU_TAG('l', 'i', 'g', 'a')}, /* Standard Ligatures */
        {GURU_FEATURE_LJMO, GURU_TAG('l', 'j', 'm', 'o')}, /* Leading Jamo Forms */
        {GURU_FEATURE_LNUM, GURU_TAG('l', 'n', 'u', 'm')}, /* Lining Figures */
        {GURU_FEATURE_LOCL, GURU_TAG('l', 'o', 'c', 'l')}, /* Localized Forms */
        {GURU_FEATURE_LTRA, GURU_TAG('l', 't', 'r', 'a')}, /* Left-to-right alternates */
        {GURU_FEATURE_LTRM, GURU_TAG('l', 't', 'r', 'm')}, /* Left-to-right mirrored forms */
        {GURU_FEATURE_MARK, GURU_TAG('m', 'a', 'r', 'k')}, /* Mark Positioning */
        {GURU_FEATURE_MED2, GURU_TAG('m', 'e', 'd', '2')}, /* Medial Forms #2 */
        {GURU_FEATURE_MEDI, GURU_TAG('m', 'e', 'd', 'i')}, /* Medial Forms */
        {GURU_FEATURE_MGRK, GURU_TAG('m', 'g', 'r', 'k')}, /* Mathematical Greek */
        {GURU_FEATURE_MKMK, GURU_TAG('m', 'k', 'm', 'k')}, /* Mark to Mark Positioning */
        {GURU_FEATURE_MSET, GURU_TAG('m', 's', 'e', 't')}, /* Mark Positioning via Substitution */
        {GURU_FEATURE_NALT, GURU_TAG('n', 'a', 'l', 't')}, /* Alternate Annotation Forms */
        {GURU_FEATURE_NLCK, GURU_TAG('n', 'l', 'c', 'k')}, /* NLC Kanji Forms */
        {GURU_FEATURE_NUKT, GURU_TAG('n', 'u', 'k', 't')}, /* Nukta Forms */
        {GURU_FEATURE_NUMR, GURU_TAG('n', 'u', 'm', 'r')}, /* Numerators */
        {GURU_FEATURE_ONUM, GURU_TAG('o', 'n', 'u', 'm')}, /* Oldstyle Figures */
        {GURU_FEATURE_OPBD, GURU_TAG('o', 'p', 'b', 'd')}, /* Optical Bounds */
        {GURU_FEATURE_ORDN, GURU_TAG('o', 'r', 'd', 'n')}, /* Ordinals */
        {GURU_FEATURE_ORNM, GURU_TAG('o', 'r', 'n', 'm')}, /* Ornaments */
        {GURU_FEATURE_PALT, GURU_TAG('p', 'a', 'l', 't')}, /* Proportional Alternate Widths */
        {GURU_FEATURE_PCAP, GURU_TAG('p', 'c', 'a', 'p')}, /* Petite Capitals */
        {GURU_FEATURE_PKNA, GURU_TAG('p', 'k', 'n', 'a')}, /* Proportional Kana */
        {GURU_FEATURE_PNUM, GURU_TAG('p', 'n', 'u', 'm')}, /* Proportional Figures */
        {GURU_FEATURE_PREF, GURU_TAG('p', 'r', 'e', 'f')}, /* Pre-Base Forms */
        {GURU_FEATURE_PRES, GURU_TAG('p', 'r', 'e', 's')}, /* Pre-base Substitutions */
        {GURU_FEATURE_PSTF, GURU_TAG('p', 's', 't', 'f')}, /* Post-base Forms */
        {GURU_FEATURE_PSTS, GURU_TAG('p', 's', 't', 's')}, /* Post-base Substitutions */
        {GURU_FEATURE_PWID, GURU_TAG('p', 'w', 'i', 'd')}, /* Proportional Widths */
        {GURU_FEATURE_QWID, GURU_TAG('q', 'w', 'i', 'd')}, /* Quarter Widths */
        {GURU_FEATURE_RAND, GURU_TAG('r', 'a', 'n', 'd')}, /* Randomize */
        {GURU_FEATURE_RCLT, GURU_TAG('r', 'c', 'l', 't')}, /* Required Contextual Alternates */
        {GURU_FEATURE_RKRF, GURU_TAG('r', 'k', 'r', 'f')}, /* Rakar Forms */
        {GURU_FEATURE_RLIG, GURU_TAG('r', 'l', 'i', 'g')}, /* Required Ligatures */
        {GURU_FEATURE_RPHF, GURU_TAG('r', 'p', 'h', 'f')}, /* Reph Forms */
        {GURU_FEATURE_RTBD, GURU_TAG('r', 't', 'b', 'd')}, /* Right Bounds */
        {GURU_FEATURE_RTLA, GURU_TAG('r', 't', 'l', 'a')}, /* Right-to-left alternates */
        {GURU_FEATURE_RTLM, GURU_TAG('r', 't', 'l', 'm')},  /* Right-to-left mirrored forms */
        {GURU_FEATURE_RUBY, GURU_TAG('r', 'u', 'b', 'y')}, /* Ruby Notation Forms */
        {GURU_FEATURE_RVRN, GURU_TAG('r', 'v', 'r', 'n')}, /* Required Variation Alternates */
        {GURU_FEATURE_SALT, GURU_TAG('s', 'a', 'l', 't')}, /* Stylistic alternates */
        {GURU_FEATURE_SINF, GURU_TAG('s', 'i', 'n', 'f')}, /* Scientific Inferiors */
        {GURU_FEATURE_SIZE, GURU_TAG('s', 'i', 'z', 'e')}, /* Optical size */
        {GURU_FEATURE_SMCP, GURU_TAG('s', 'm', 'c', 'p')}, /* Small Capitals */
        {GURU_FEATURE_SMPL, GURU_TAG('s', 'm', 'p', 'l')}, /* Simplified Forms */
        /* 	ss01 - ss20 Stylistic Sets */
        {GURU_FEATURE_SSTY, GURU_TAG('s', 's', 't', 'y')}, /* Math script style alternates */
        {GURU_FEATURE_STCH, GURU_TAG('s', 't', 'c', 'h')}, /* Stretching Glyph Decomposition */
        {GURU_FEATURE_SUBS, GURU_TAG('s', 'u', 'b', 's')}, /* Subscript */
        {GURU_FEATURE_SUPS, GURU_TAG('s', 'u', 'p', 's')}, /* Superscript */
        {GURU_FEATURE_SWSH, GURU_TAG('s', 'w', 's', 'h')}, /* Swash */
        {GURU_FEATURE_TITL, GURU_TAG('t', 'i', 't', 'l')}, /* Titling */
        {GURU_FEATURE_TJMO, GURU_TAG('t', 'j', 'm', 'o')}, /* Trailing Jamo Forms */
        {GURU_FEATURE_TNAM, GURU_TAG('t', 'n', 'a', 'm')}, /* Traditional Name Forms */
        {GURU_FEATURE_TNUM, GURU_TAG('t', 'n', 'u', 'm')}, /* Tabular Figures */
        {GURU_FEATURE_TRAD, GURU_TAG('t', 'r', 'a', 'd')}, /* Traditional Forms */
        {GURU_FEATURE_TWID, GURU_TAG('t', 'w', 'i', 'd')}, /* Third Widths */
        {GURU_FEATURE_UNIC, GURU_TAG('u', 'n', 'i', 'c')}, /* Unicase */
        {GURU_FEATURE_VALT, GURU_TAG('v', 'a', 'l', 't')}, /* Alternate Vertical Metrics */
        {GURU_FEATURE_VATU, GURU_TAG('v', 'a', 't', 'u')}, /* Vattu Variants */
        {GURU_FEATURE_VERT, GURU_TAG('v', 'e', 'r', 't')}, /* Vertical Writing */
        {GURU_FEATURE_VHAL, GURU_TAG('v', 'h', 'a', 'l')}, /* Alternate Vertical Half Metrics */
        {GURU_FEATURE_VJMO, GURU_TAG('v', 'j', 'm', 'o')}, /* Vowel Jamo Forms */
        {GURU_FEATURE_VKNA, GURU_TAG('v', 'k', 'n', 'a')}, /* Vertical Kana Alternates */
        {GURU_FEATURE_VKRN, GURU_TAG('v', 'k', 'r', 'n')}, /* Vertical Kerning */
        {GURU_FEATURE_VPAL, GURU_TAG('v', 'p', 'a', 'l')}, /* Proportional Alternate Vertical Metrics */
        {GURU_FEATURE_VRT2, GURU_TAG('v', 'r', 't', '2')}, /* Vertical Alternates and Rotation */
        {GURU_FEATURE_VRTR, GURU_TAG('v', 'r', 't', 'r')}, /* Vertical Alternates for Rotation */
        {GURU_FEATURE_ZERO, GURU_TAG('z', 'e', 'r', 'o')}, /* Slashed Zero */
};

static uint32_t
guru_ot_calc_table_checksum(const uint32_t *table, uint32_t len) {
    uint32_t sum = 0;
    const uint32_t *endptr = table + ((len + 3) & ~3) / sizeof(uint32_t);

    while (table < endptr)
        sum += *table++;

    return sum;
}


/*
    TTC Header Version 2.0
*/
typedef struct GURU_PACKED guru_ttc_header_t {
    /* Font Collection ID string: 'ttcf' (used for fonts with CFF or CFF2 outlines as well as TrueType outlines) */
    guru_tag ttcTag;

    /* 	Major version of the TTC Header */
    guru_uint16 majorVersion;

    /* Minor version of the TTC Header */
    guru_uint16 minorVersion;

    /* Number of fonts in TTC */
    uint32_t numFonts;

    /* Array of offsets to the OffsetTable for each font from the beginning of the file */
    guru_offset32 *offsetTable;

    /* Tag indicating that a DSIG table exists, 0x44534947 ('DSIG') (null if no signature) */
    uint32_t dsigTag;

    /* The length (in bytes) of the DSIG table (null if no signature) */
    uint32_t dsigLength;

    /* The offset (in bytes) of the DSIG table from the beginning of the TTC file (null if no signature) */
    uint32_t dsigOffset;
} guru_ttc_header_t;
/*
typedef struct guru_font_t {
    FT_Face ft_face;
    FT_Bytes base_table;
    FT_Bytes gdef_table;
    FT_Bytes gpos_table;
    FT_Bytes gsub_table;
    FT_Bytes jstf_table;
} guru_font_t;
*/
typedef enum guru_script_t {
    GURU_SCRIPT_ARABIC,
    GURU_SCRIPT_LATIN
} guru_script_t;

typedef enum guru_language_t {
    GURU_LANGUAGE_ARABIC,
    GURU_LANGUAGE_ENGLISH,
    GURU_LANGUAGE_FRENCH,
} guru_language_t;

typedef enum guru_dir_t {
    GURU_DIR_LTR,
    GURU_DIR_RTL,
} guru_dir_t;

typedef struct guru_buf_t {
    guru_unicode *text;
    guru_uint16 len;
} guru_buf_t;

static void
guru_buf_push_code(guru_buf_t *buf, guru_unicode code) {
    guru_uint16 new_size = ++buf->len * sizeof(guru_unicode);

    if (buf->text == NULL) buf->text = (guru_unicode *) malloc(new_size);
    else buf->text = (guru_unicode *) realloc(buf->text, new_size);

    buf->text[buf->len - 1] = code;
}

static guru_language_t
guru_lang(const char *s) {
    if (!strcmp(s, "ar")) return GURU_LANGUAGE_ARABIC;
    else if (!strcmp(s, "en")) return GURU_LANGUAGE_ENGLISH;
}

#define GURU_LANG(lang_str) guru_lang(lang_str)

static guru_buf_t *
guru_buf_create(void) {
    guru_buf_t *buf = (guru_buf_t *) malloc(sizeof(guru_buf_t));
    buf->text = NULL;
    buf->len = 0;
    return buf;
}

typedef struct {
    const guru_byte *mem;
    guru_size length;
    guru_size offset;
} guru_utf8_dec_t;

#define GURU_UTF8_END -1
#define GURU_UTF8_ERROR -2

static int
guru_utf8_get(guru_utf8_dec_t *dec) {
    if (dec->offset >= dec->length)
        return GURU_UTF8_END;

    return dec->mem[dec->offset++];
}

static int
guru_utf8_cont(guru_utf8_dec_t *dec) {
    int c = guru_utf8_get(dec);
    return ((c & 0xC0) == 0x80)
           ? (c & 0x3F)
           : GURU_UTF8_ERROR;
}

static int
guru_utf8_next(guru_utf8_dec_t *dec) {
    int c;
    int c1;
    int c2;
    int c3;
    int r;
    c = guru_utf8_get(dec);

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
        c1 = guru_utf8_cont(dec);
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
        c1 = guru_utf8_cont(dec);
        c2 = guru_utf8_cont(dec);
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
        c1 = guru_utf8_cont(dec);
        c2 = guru_utf8_cont(dec);
        c3 = guru_utf8_cont(dec);
        if ((c1 | c2 | c3) >= 0) {
            r = ((c & 0x07) << 18) | (c1 << 12) | (c2 << 6) | c3;
            if (r >= 65536 && r <= 1114111) {
                return r;
            }
        }
    }

    return GURU_UTF8_ERROR;
}

static size_t guru_strlen8(guru_char *text) {
    uint64_t len = UINT64_MAX;

}

static void
guru_buf_load_utf8(guru_buf_t *buf, const guru_char *text, size_t len) {
    guru_unicode code;
    int ch;

    if (buf->text != NULL && buf->len > 0) {
        free(buf->text);
        buf->text = NULL;
        buf->len = 0;
    }

    guru_utf8_dec_t dec;
    dec.mem = (const guru_byte *) text;
    dec.length = len;
    dec.offset = 0;

    /* TODO: do proper error handling for the UTF-8 decoder */
    while ((ch = guru_utf8_next(&dec)) > 0) {
        code = (guru_unicode) ch;
        guru_buf_push_code(buf, code);
    }
}

static void
guru_buf_load_utf8_zt(guru_buf_t *buf, const guru_char *text) {
    guru_buf_load_utf8(buf, text, UINT64_MAX);
}


typedef struct GURU_PACKED guru_rec16_t {
    guru_tag tag;
    guru_offset16 offset;
} guru_rec16_t;

typedef struct GURU_PACKED guru_rec32_t {
    guru_tag tag;
    guru_offset32 offset;
} guru_rec32_t;

typedef struct GURU_PACKED guru_script_list_t {
    guru_uint16 scriptCount;
    //guru_rec32_t *scriptRecords;
} guru_script_list_t;

typedef struct GURU_PACKED guru_script_table_t {
    guru_offset16 defaultLangSys;
    guru_uint16 langSysCount;
    //guru_rec16_t *langSysRecords;
} guru_script_table_t;

typedef struct guru_lang_sys_t {
    /* = NULL (reserved for an offset to a reordering table) */
    guru_offset16 lookupOrder;

    /* Index of a feature required for this language system; if no required features = 0xFFFF */
    guru_uint16 requiredFeatureIndex;

    /* Number of feature index values for this language system — excludes the required feature */
    guru_uint16 featureIndexCount;

    /* Array of indices into the FeatureList, in arbitrary order */
    //GURU_Uint16 *featureIndices;
} guru_lang_sys_t;

typedef struct GURU_PACKED guru_feature_list_t {
    guru_uint16 featureCount;
    guru_rec16_t *featureRecord;
} guru_feature_list_t;

typedef struct GURU_PACKED guru_feature_table_t {
    /* = NULL (reserved for offset to FeatureParams) */
    guru_offset16 featureParams;

    /* Number of LookupList indices for this feature */
    guru_uint16 lookupIndexCount;

    /* 	Array of indices into the LookupList — zero-based (first lookup is LookupListIndex = 0) */
    guru_uint16 *lookupListIndices;
} guru_feature_table_t;


struct guru_lookup_list_t {
    /* Number of lookups in this table */
    guru_uint16 lookupCount;

    /* Array of offsets to Lookup tables, from beginning of LookupList — zero based (first lookup is Lookup index = 0) */
    //GURU_Offset16 *lookups;
};
/*
static const char *guru_GSUB_lookup_type_string(GURU_Uint16 type);
static const char *guru_GPOS_lookup_type_string(GURU_Uint16 type);
*/


typedef enum guru_lookup_flag_t {
    /*
    This bit relates only to
    the correct processing of the cursive attachment lookup type (GPOS lookup type 3).
    When this bit is set, the last glyph in a given sequence to
    which the cursive attachment lookup is applied, will be positioned on the baseline.
    */
    GURU_LOOKUP_FLAG_RIGHT_TO_LEFT = 0x0001,

    /* If set, skips over base glyphs */
    GURU_LOOKUP_FLAG_IGNORE_BASE_GLYPHS = 0x0002,

    /* If set, skips over ligatures */
    GURU_LOOKUP_FLAG_IGNORE_LIGATURES = 0x0004,

    /* If set, skips over all combining marks */
    GURU_LOOKUP_FLAG_IGNORE_MARKS = 0x0008,

    /*
    If set, indicates that the lookup table structure is followed by a MarkFilteringSet field.
    The layout engine skips over all mark glyphs not in the mark filtering set indicated.
    */
    GURU_LOOKUP_FLAG_USE_MARK_FILTERING_SET = 0x0010,

    /* 0x00E0 - For future use (Set to zero) */

    /* If not zero, skips over all marks of attachment type different from specified. */
    GURU_LOOKUP_FLAG_MARK_ATTACHMENT_TYPE_MASK = 0xFF00
} guru_lookup_flag_t;

typedef struct guru_lookup_table_t {
    guru_uint16 lookupType;
    guru_uint16 lookupFlag;
    guru_uint16 subtableCount;
    //GURU_Offset16 *subtableOffsets;
    guru_uint16 markFilteringSet;
} guru_lookup_table_t;

typedef struct guru_coverage_format1_t {
    guru_uint16 coverageFormat; /* Format identifier — format = 1 */
    guru_uint16 glyphCount; /* Number of glyphs in the glyph array */
    guru_id_t *glyphArray; /* Array of glyph IDs — in numerical order */
} guru_coverage_format1_t;

typedef struct guru_range_rec_t {
    guru_id_t startGlyphID;
    guru_id_t endGlyphID;
    guru_uint16 startCoverageIndex;
} guru_range_rec_t;

typedef struct guru_coverage_format2_t {
    guru_uint16 coverageFormat; /* Format identifier — format = 2 */
    guru_uint16 rangeCount; /* Number of RangeRecords */
    guru_range_rec_t *rangeRecords; /* Array of glyph ranges — ordered by startGlyphID. */
} guru_coverage_format2_t;


struct guru_class_def_format1_t {
    guru_uint16 format; /* Format identifier — format = 1 */
    guru_id_t startGID; /* First glyph ID of the classValueArray */
    guru_uint16 glyphCount; /* Size of the classValueArray */
    guru_uint16 *classValues; /* Array of Class Values — one per glyph ID */
};

struct guru_class_range_rec_t {
    guru_id_t startGlyphID;
    guru_id_t endGlyphID;
    guru_uint16 classValue;
};

struct guru_class_def_format2_t {
    guru_uint16 format;
    guru_uint16 ranageCount;
    struct guru_class_range_rec_t *rangeRecords;
};

typedef enum guru_delta_format_t {
    /* Signed 2-bit value, 8 values per uint16 */
    GURU_DELTA_FORMAT_LOCAL_2_BIT = 0x0001,

    /* Signed 4-bit value, 4 values per uint16 */
    GURU_DELTA_FORMAT_LOCAL_4_BIT = 0x0002,

    /* Signed 8-bit value, 2 values per uint16 */
    GURU_DELTA_FORMAT_LOCAL_8_BIT = 0x0003,

    /* VariationIndex table, contains a delta-set index pair. */
    GURU_DELTA_FORMAT_VARIATION_INDEX = 0x8000,

    /* 0x7FFC Reserved */
} guru_delta_format_t;

typedef struct GURU_PACKED guru_delta_table_t {
    guru_uint16 smallestPPEM;
    guru_uint16 largestPPEM;
    guru_delta_format_t format;
    guru_uint16 *deltaValues;
} guru_delta_table_t;

typedef struct GURU_PACKED guru_variation_index_table_t {
    /* A delta-set outer index — used to select an item variation data subtable within the item variation store. */
    guru_uint16 outerIndex;

    /* A delta-set inner index — used to select a delta-set row within an item variation data subtable. */
    guru_uint16 innerIndex;

    /* Format, = 0x8000 */
    guru_delta_format_t format;
} guru_variation_index_table_t;

typedef struct GURU_PACKED guru_feature_variation_record_t {
    /* Offset to a condition set table, from beginning of FeatureVariations table. */
    guru_offset32 conditionSetOffset;

    /*
        Offset to a feature table substitution table,
        from beginning of the FeatureVariations table.
    */
    guru_offset32 featureTableSubstitutionOffset;
} guru_feature_variation_record_t;


typedef struct GURU_PACKED guru_feature_variations_table_t {
    /* Major version of the FeatureVariations table — set to 1. */
    guru_uint16 versionMajor;

    /* Minor version of the FeatureVariations table — set to 0. */
    guru_uint16 versionMinor;

    /* Number of feature variation records. */
    guru_uint16 featureVariationRecordCount;

    /* Array of feature variation records. */
    guru_feature_variation_record_t *featureVariationRecord;
} guru_feature_variations_table_t;

typedef struct GURU_PACKED guru_condition_set_t {
    /* Number of conditions for this condition set. */
    guru_uint16 conditionCount;

    /* Array of offsets to condition tables, from beginning of the ConditionSet table. */
    guru_offset32 *conditions;
} guru_condition_set_t;

typedef struct GURU_PACKED guru_condition_format1_t {
    /* Format, = 1 */
    guru_uint16 format;

    /* Index (zero-based) for the variation axis within the 'fvar' table. */
    guru_uint16 axisIndex;

    /* Minimum value of the font variation instances that satisfy this condition. */
    GURU_F2DOT14 filterRangeMinValue;

    /* Maximum value of the font variation instances that satisfy this condition. */
    GURU_F2DOT14 filterRangeMaxValue;
} guru_condition_format1_t;

typedef struct GURU_PACKED guru_feature_table_substitution_record_t {
    /* The feature table index to match. */
    guru_uint16 featureIndex;

    /* Offset to an alternate feature table, from start of the FeatureTableSubstitution table. */
    guru_offset32 alternateFeatureOffset;
} guru_feature_table_substitution_record_t;

typedef struct GURU_PACKED guru_feature_table_substitution_table_t {
    /* Major version of the feature table substitution table — set to 1 */
    guru_uint16 majorVersion;

    /* Minor version of the feature table substitution table — set to 0. */
    guru_uint16 minorVersion;

    /* Number of feature table substitution records. */
    guru_uint16 substitutionCount;

    /* Array of feature table substitution records. */
    guru_feature_table_substitution_record_t *substitutions;
} guru_feature_table_substitution_table_t;

typedef struct guru_bitset_t {
    guru_byte *data;
    guru_uint16 bit_count;
} guru_bitset_t;

static guru_uint64 guru_next_pow2m1(guru_uint64 x) {
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x |= x >> 32;
    return x;
}

static guru_bitset_t *
guru_bitset_create(guru_uint16 bit_count) {
    guru_bitset_t *bitset = (guru_bitset_t *) malloc(sizeof(guru_bitset_t));

    bitset->bit_count = bit_count;
    guru_uint16 byte_count = bit_count >> 3;

    bitset->data = (guru_byte *) calloc(1, byte_count);

    return bitset;
}

static guru_bool
guru_bitset_set(guru_bitset_t *bitset, guru_uint16 index, guru_bool value) {
    if (index < bitset->bit_count) {
        guru_byte *byte = bitset->data + (index >> 3);
        guru_byte mask = 1 << (index & 0x7);

        if (value == GURU_TRUE)
            *byte |= mask;
        else if (value == GURU_FALSE)
            *byte &= ~mask;

        return GURU_TRUE;
    }

    return GURU_FALSE;
}

static guru_bool
guru_bitset_check(guru_bitset_t *bitset, guru_uint16 index) {
    guru_bool value = GURU_FALSE;

    if (index < bitset->bit_count) {
        guru_byte byte = *(bitset->data + (index >> 3));
        guru_byte mask = 1 << (index & 0x7);

        if (byte & mask)
            value = GURU_TRUE;
    }

    return value;
}

static void
guru_bitset_copy(guru_bitset_t *dst, guru_bitset_t *src) {
    size_t byte_count = src->bit_count >> 3;
    GURU_ASSERT(src->bit_count == dst->bit_count);

    if (dst->data != NULL)
        free(dst->data);

    memcpy(dst->data, src->data, byte_count);
}


#define GURU_OT_TAG_GSUB GURU_TAG('G','S','U','B')
#define GURU_OT_TAG_GPOS guru_tag('G','P','O','S')

void
guru_ot_layout_collect_lookups(guru_face_t *face,
                             guru_tag table_tag,
                             guru_tag language, guru_tag script,
                             const guru_bitset_t *feature_bits,
                             guru_set_t *lookup_indices);
guru_tag
guru_ot_script_to_tag(guru_script_t script);

guru_tag
guru_ot_language_to_tag(guru_language_t language);

#ifdef __cplusplus
};
#endif

#endif /* GURU_OT_H */