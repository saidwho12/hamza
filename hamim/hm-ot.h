#ifndef HM_OT_H
#define HM_OT_H

#include "hm-base.h"
#include "hm-set.h"
#include "hm-array.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Registered Features
 * https://docs.microsoft.com/en-us/typography/opentype/spec/featurelist
 *
 */
typedef enum hm_feature_t {
    HM_FEATURE_AALT = 0, /* Access All Alternates */
    HM_FEATURE_ABVF = 1, /* Above-base forms */
    HM_FEATURE_ABVM = 2, /* Above-base Mark Positioning */
    HM_FEATURE_ABVS = 3, /* Above-base Substitutions */
    HM_FEATURE_AFRC = 4, /* Alternative Fractions */
    HM_FEATURE_AKHN = 5, /* Akhands */
    HM_FEATURE_BLWF = 6, /* Below-base forms */
    HM_FEATURE_BLWM = 7, /* Below-base Mark Positioning */
    HM_FEATURE_BLWS = 8, /* Below-base Substitutions */
    HM_FEATURE_CALT = 9, /* Contextual Alternates */
    HM_FEATURE_CASE = 10, /* Case-Sensitive Forms */
    HM_FEATURE_CCMP = 11, /* Glyph Composition / Decomposition */
    HM_FEATURE_CFAR = 12, /* Conjunct Form After Ro */
    HM_FEATURE_CJCT = 13, /* Conjunct forms */
    HM_FEATURE_CLIG = 14, /* Contextual Ligatures */
    HM_FEATURE_CPCT = 15, /* Centered CJK Punctuation */
    HM_FEATURE_CPSP = 16, /* Capital Spacing */
    HM_FEATURE_CSWH = 17, /* Contextual Swash */
    HM_FEATURE_CURS = 18, /* Cursive Positioning */
    /* 'cv01' – 'cv99' Character Variants */
    HM_FEATURE_C2PC = 117, /* Petite Capitals From Capitals */
    HM_FEATURE_C2SC = 118, /* Small Capitals From Capitals */
    HM_FEATURE_DIST = 119, /* Distances */
    HM_FEATURE_DLIG = 120, /* Discretionary Ligatures */
    HM_FEATURE_DNOM = 121, /* Denominators */
    HM_FEATURE_DTLS = 122, /* Dotless Forms */
    HM_FEATURE_EXPT = 123, /* Expert Forms */
    HM_FEATURE_FALT = 124, /* Final Glyph on Line Alternates */
    HM_FEATURE_FIN2 = 125, /* Terminal Forms #2 */
    HM_FEATURE_FIN3 = 126, /* Terminal Forms #3 */
    HM_FEATURE_FINA = 127, /* Terminal Forms */
    HM_FEATURE_FLAC = 128, /* Flattened accent forms */
    HM_FEATURE_FRAC = 129, /* Fractions */
    HM_FEATURE_FWID = 130, /* Full Widths */
    HM_FEATURE_HALF = 131, /* Half Forms */
    HM_FEATURE_HALN = 132, /* Halant Forms */
    HM_FEATURE_HALT = 133, /* Alternate Half Widths */
    HM_FEATURE_HIST = 134, /* Historical Forms */
    HM_FEATURE_HKNA = 135, /* Horizontal Kana Alternates */
    HM_FEATURE_HLIG = 136, /* Historical Ligaturess */
    HM_FEATURE_HNGL = 137, /* Hangul */
    HM_FEATURE_HOJO = 138, /* Hojo Kanji Forms (JIS X 0212-1990 Kanji Forms) */
    HM_FEATURE_HWID = 139, /* Half Widths */
    HM_FEATURE_INIT = 140, /* Initial Forms */
    HM_FEATURE_ISOL = 141, /* Isolated Forms */
    HM_FEATURE_ITAL = 142, /* Italics */
    HM_FEATURE_JALT = 143, /* Justification Alternates */
    HM_FEATURE_JP78 = 144, /* JIS78 Forms */
    HM_FEATURE_JP83 = 145, /* JIS83 Forms */
    HM_FEATURE_JP90 = 146, /* JIS90 Forms */
    HM_FEATURE_JP04 = 147, /* JIS2004 Forms */
    HM_FEATURE_KERN = 148, /* Kerning */
    HM_FEATURE_LFBD = 149, /* Left Bounds */
    HM_FEATURE_LIGA = 150, /* Standard Ligatures */
    HM_FEATURE_LJMO = 151, /* Leading Jamo Forms */
    HM_FEATURE_LNUM = 152, /* Lining Figures */
    HM_FEATURE_LOCL = 153, /* Localized Forms */
    HM_FEATURE_LTRA = 154, /* Left-to-right alternates */
    HM_FEATURE_LTRM = 155, /* Left-to-right mirrored forms */
    HM_FEATURE_MARK = 156, /* Mark Positioning */
    HM_FEATURE_MED2 = 157, /* Medial Forms #2 */
    HM_FEATURE_MEDI = 158, /* Medial Forms */
    HM_FEATURE_MGRK = 159, /* Mathematical Greek */
    HM_FEATURE_MKMK = 160, /* Mark to Mark Positioning */
    HM_FEATURE_MSET = 161, /* Mark Positioning via Substitution */
    HM_FEATURE_NALT = 162, /* Alternate Annotation Forms */
    HM_FEATURE_NLCK = 163, /* NLC Kanji Forms */
    HM_FEATURE_NUKT = 164, /* Nukta Forms */
    HM_FEATURE_NUMR = 165, /* Numerators */
    HM_FEATURE_ONUM = 166, /* Oldstyle Figures */
    HM_FEATURE_OPBD = 167, /* Optical Bounds */
    HM_FEATURE_ORDN = 168, /* Ordinals */
    HM_FEATURE_ORNM = 169, /* Ornaments */
    HM_FEATURE_PALT = 170, /* Proportional Alternate Widths */
    HM_FEATURE_PCAP = 171, /* Petite Capitals */
    HM_FEATURE_PKNA = 172, /* Proportional Kana */
    HM_FEATURE_PNUM = 173, /* Proportional Figures */
    HM_FEATURE_PREF = 174, /* Pre-Base Forms */
    HM_FEATURE_PRES = 175, /* Pre-base Substitutions */
    HM_FEATURE_PSTF = 176, /* Post-base Forms */
    HM_FEATURE_PSTS = 177, /* Post-base Substitutions */
    HM_FEATURE_PWID = 178, /* Proportional Widths */
    HM_FEATURE_QWID = 179, /* Quarter Widths */
    HM_FEATURE_RAND = 180, /* Randomize */
    HM_FEATURE_RCLT = 181, /* Required Contextual Alternates */
    HM_FEATURE_RKRF = 182, /* Rakar Forms */
    HM_FEATURE_RLIG = 183, /* Required Ligatures */
    HM_FEATURE_RPHF = 184, /* Reph Forms */
    HM_FEATURE_RTBD = 185, /* Right Bounds */
    HM_FEATURE_RTLA = 186, /* Right-to-left alternates */
    HM_FEATURE_RTLM = 187,  /* Right-to-left mirrored forms */
    HM_FEATURE_RUBY = 188, /* Ruby Notation Forms */
    HM_FEATURE_RVRN = 189, /* Required Variation Alternates */
    HM_FEATURE_SALT = 190, /* Stylistic alternates */
    HM_FEATURE_SINF = 191, /* Scientific Inferiors */
    HM_FEATURE_SIZE = 192, /* Optical size */
    HM_FEATURE_SMCP = 193, /* Small Capitals */
    HM_FEATURE_SMPL = 194, /* Simplified Forms */
    /* 	ss01 - ss20 Stylistic Sets */
    HM_FEATURE_SSTY = 215, /* Math script style alternates */
    HM_FEATURE_STCH = 216, /* Stretching Glyph Decomposition */
    HM_FEATURE_SUBS = 217, /* Subscript */
    HM_FEATURE_SUPS = 218, /* Superscript */
    HM_FEATURE_SWSH = 219, /* Swash */
    HM_FEATURE_TITL = 220, /* Titling */
    HM_FEATURE_TJMO = 221, /* Trailing Jamo Forms */
    HM_FEATURE_TNAM = 222, /* Traditional Name Forms */
    HM_FEATURE_TNUM = 223, /* Tabular Figures */
    HM_FEATURE_TRAD = 224, /* Traditional Forms */
    HM_FEATURE_TWID = 225, /* Third Widths */
    HM_FEATURE_UNIC = 226, /* Unicase */
    HM_FEATURE_VALT = 227, /* Alternate Vertical Metrics */
    HM_FEATURE_VATU = 228, /* Vattu Variants */
    HM_FEATURE_VERT = 229, /* Vertical Writing */
    HM_FEATURE_VHAL = 230, /* Alternate Vertical Half Metrics */
    HM_FEATURE_VJMO = 231, /* Vowel Jamo Forms */
    HM_FEATURE_VKNA = 232, /* Vertical Kana Alternates */
    HM_FEATURE_VKRN = 233, /* Vertical Kerning */
    HM_FEATURE_VPAL = 234, /* Proportional Alternate Vertical Metrics */
    HM_FEATURE_VRT2 = 235, /* Vertical Alternates and Rotation */
    HM_FEATURE_VRTR = 236, /* Vertical Alternates for Rotation */
    HM_FEATURE_ZERO = 237, /* Slashed Zero */
    HM_FEATURE_COUNT
} hm_feature_t;

typedef struct hm_feature_info_t {
    hm_feature_t feature;
    hm_tag tag;
} hm_feature_info_t;

static const hm_feature_info_t HM_FEATURE_INFO_LUT[HM_FEATURE_COUNT] = {
        {HM_FEATURE_AALT, HM_TAG('a', 'a', 'l', 't')}, /* Access All Alternates */
        {HM_FEATURE_ABVF, HM_TAG('a', 'b', 'v', 'f')}, /* Above-base forms */
        {HM_FEATURE_ABVM, HM_TAG('a', 'b', 'v', 'm')}, /* Above-base Mark Positioning */
        {HM_FEATURE_ABVS, HM_TAG('a', 'b', 'v', 's')}, /* Above-base Substitutions */
        {HM_FEATURE_AFRC, HM_TAG('a', 'f', 'r', 'c')}, /* Alternative Fractions */
        {HM_FEATURE_AKHN, HM_TAG('a', 'k', 'h', 'n')}, /* Akhands */
        {HM_FEATURE_BLWF, HM_TAG('b', 'l', 'w', 'f')}, /* Below-base forms */
        {HM_FEATURE_BLWM, HM_TAG('b', 'l', 'w', 'm')}, /* Below-base Mark Positioning */
        {HM_FEATURE_BLWS, HM_TAG('b', 'l', 'w', 's')}, /* Below-base Substitutions */
        {HM_FEATURE_CALT, HM_TAG('c', 'a', 'l', 't')}, /* Contextual Alternates */
        {HM_FEATURE_CASE, HM_TAG('c', 'a', 's', 'e')}, /* Case-Sensitive Forms */
        {HM_FEATURE_CCMP, HM_TAG('c', 'c', 'm', 'p')}, /* Glyph Composition / Decomposition */
        {HM_FEATURE_CFAR, HM_TAG('c', 'f', 'a', 'r')}, /* Conjunct Form After Ro */
        {HM_FEATURE_CJCT, HM_TAG('c', 'j', 'c', 't')}, /* Conjunct forms */
        {HM_FEATURE_CLIG, HM_TAG('c', 'l', 'i', 'g')}, /* Contextual Ligatures */
        {HM_FEATURE_CPCT, HM_TAG('c', 'p', 'c', 't')}, /* Centered CJK Punctuation */
        {HM_FEATURE_CPSP, HM_TAG('c', 'p', 's', 'p')}, /* Capital Spacing */
        {HM_FEATURE_CSWH, HM_TAG('c', 's', 'w', 'h')}, /* Contextual Swash */
        {HM_FEATURE_CURS, HM_TAG('c', 'u', 'r', 's')}, /* Cursive Positioning */
        /* 'cv01' – 'cv99' Character Variants */
        {HM_FEATURE_C2PC, HM_TAG('c', '2', 'p', 'c')}, /* Petite Capitals From Capitals */
        {HM_FEATURE_C2SC, HM_TAG('c', '2', 's', 'c')}, /* Small Capitals From Capitals */
        {HM_FEATURE_DIST, HM_TAG('d', 'i', 's', 't')}, /* Distances */
        {HM_FEATURE_DLIG, HM_TAG('d', 'l', 'i', 'g')}, /* Discretionary Ligatures */
        {HM_FEATURE_DNOM, HM_TAG('d', 'n', 'o', 'm')}, /* Denominators */
        {HM_FEATURE_DTLS, HM_TAG('d', 't', 'l', 's')}, /* Dotless Forms */
        {HM_FEATURE_EXPT, HM_TAG('e', 'x', 'p', 't')}, /* Expert Forms */
        {HM_FEATURE_FALT, HM_TAG('f', 'a', 'l', 't')}, /* Final Glyph on Line Alternates */
        {HM_FEATURE_FIN2, HM_TAG('f', 'i', 'n', '2')}, /* Terminal Forms #2 */
        {HM_FEATURE_FIN3, HM_TAG('f', 'i', 'n', '3')}, /* Terminal Forms #3 */
        {HM_FEATURE_FINA, HM_TAG('f', 'i', 'n', 'a')}, /* Terminal Forms */
        {HM_FEATURE_FLAC, HM_TAG('f', 'l', 'a', 'c')}, /* Flattened accent forms */
        {HM_FEATURE_FRAC, HM_TAG('f', 'r', 'a', 'c')}, /* Fractions */
        {HM_FEATURE_FWID, HM_TAG('f', 'w', 'i', 'd')}, /* Full Widths */
        {HM_FEATURE_HALF, HM_TAG('h', 'a', 'l', 'f')}, /* Half Forms */
        {HM_FEATURE_HALN, HM_TAG('h', 'a', 'l', 'n')}, /* Halant Forms */
        {HM_FEATURE_HALT, HM_TAG('h', 'a', 'l', 't')}, /* Alternate Half Widths */
        {HM_FEATURE_HIST, HM_TAG('h', 'i', 's', 't')}, /* Historical Forms */
        {HM_FEATURE_HKNA, HM_TAG('h', 'k', 'n', 'a')}, /* Horizontal Kana Alternates */
        {HM_FEATURE_HLIG, HM_TAG('h', 'l', 'i', 'g')}, /* Historical Ligaturess */
        {HM_FEATURE_HNGL, HM_TAG('h', 'n', 'g', 'l')}, /* Hangul */
        {HM_FEATURE_HOJO, HM_TAG('h', 'o', 'j', 'o')}, /* Hojo Kanji Forms (JIS X 0212-1990 Kanji Forms) */
        {HM_FEATURE_HWID, HM_TAG('h', 'w', 'i', 'd')}, /* Half Widths */
        {HM_FEATURE_INIT, HM_TAG('i', 'n', 'i', 't')}, /* Initial Forms */
        {HM_FEATURE_ISOL, HM_TAG('i', 's', 'o', 'l')}, /* Isolated Forms */
        {HM_FEATURE_ITAL, HM_TAG('i', 't', 'a', 'l')}, /* Italics */
        {HM_FEATURE_JALT, HM_TAG('j', 'a', 'l', 't')}, /* Justification Alternates */
        {HM_FEATURE_JP78, HM_TAG('j', 'p', '7', '8')}, /* JIS78 Forms */
        {HM_FEATURE_JP83, HM_TAG('j', 'p', '8', '3')}, /* JIS83 Forms */
        {HM_FEATURE_JP90, HM_TAG('j', 'p', '9', '0')}, /* JIS90 Forms */
        {HM_FEATURE_JP04, HM_TAG('j', 'p', '0', '4')}, /* JIS2004 Forms */
        {HM_FEATURE_KERN, HM_TAG('k', 'e', 'r', 'n')}, /* Kerning */
        {HM_FEATURE_LFBD, HM_TAG('l', 'f', 'b', 'd')}, /* Left Bounds */
        {HM_FEATURE_LIGA, HM_TAG('l', 'i', 'g', 'a')}, /* Standard Ligatures */
        {HM_FEATURE_LJMO, HM_TAG('l', 'j', 'm', 'o')}, /* Leading Jamo Forms */
        {HM_FEATURE_LNUM, HM_TAG('l', 'n', 'u', 'm')}, /* Lining Figures */
        {HM_FEATURE_LOCL, HM_TAG('l', 'o', 'c', 'l')}, /* Localized Forms */
        {HM_FEATURE_LTRA, HM_TAG('l', 't', 'r', 'a')}, /* Left-to-right alternates */
        {HM_FEATURE_LTRM, HM_TAG('l', 't', 'r', 'm')}, /* Left-to-right mirrored forms */
        {HM_FEATURE_MARK, HM_TAG('m', 'a', 'r', 'k')}, /* Mark Positioning */
        {HM_FEATURE_MED2, HM_TAG('m', 'e', 'd', '2')}, /* Medial Forms #2 */
        {HM_FEATURE_MEDI, HM_TAG('m', 'e', 'd', 'i')}, /* Medial Forms */
        {HM_FEATURE_MGRK, HM_TAG('m', 'g', 'r', 'k')}, /* Mathematical Greek */
        {HM_FEATURE_MKMK, HM_TAG('m', 'k', 'm', 'k')}, /* Mark to Mark Positioning */
        {HM_FEATURE_MSET, HM_TAG('m', 's', 'e', 't')}, /* Mark Positioning via Substitution */
        {HM_FEATURE_NALT, HM_TAG('n', 'a', 'l', 't')}, /* Alternate Annotation Forms */
        {HM_FEATURE_NLCK, HM_TAG('n', 'l', 'c', 'k')}, /* NLC Kanji Forms */
        {HM_FEATURE_NUKT, HM_TAG('n', 'u', 'k', 't')}, /* Nukta Forms */
        {HM_FEATURE_NUMR, HM_TAG('n', 'u', 'm', 'r')}, /* Numerators */
        {HM_FEATURE_ONUM, HM_TAG('o', 'n', 'u', 'm')}, /* Oldstyle Figures */
        {HM_FEATURE_OPBD, HM_TAG('o', 'p', 'b', 'd')}, /* Optical Bounds */
        {HM_FEATURE_ORDN, HM_TAG('o', 'r', 'd', 'n')}, /* Ordinals */
        {HM_FEATURE_ORNM, HM_TAG('o', 'r', 'n', 'm')}, /* Ornaments */
        {HM_FEATURE_PALT, HM_TAG('p', 'a', 'l', 't')}, /* Proportional Alternate Widths */
        {HM_FEATURE_PCAP, HM_TAG('p', 'c', 'a', 'p')}, /* Petite Capitals */
        {HM_FEATURE_PKNA, HM_TAG('p', 'k', 'n', 'a')}, /* Proportional Kana */
        {HM_FEATURE_PNUM, HM_TAG('p', 'n', 'u', 'm')}, /* Proportional Figures */
        {HM_FEATURE_PREF, HM_TAG('p', 'r', 'e', 'f')}, /* Pre-Base Forms */
        {HM_FEATURE_PRES, HM_TAG('p', 'r', 'e', 's')}, /* Pre-base Substitutions */
        {HM_FEATURE_PSTF, HM_TAG('p', 's', 't', 'f')}, /* Post-base Forms */
        {HM_FEATURE_PSTS, HM_TAG('p', 's', 't', 's')}, /* Post-base Substitutions */
        {HM_FEATURE_PWID, HM_TAG('p', 'w', 'i', 'd')}, /* Proportional Widths */
        {HM_FEATURE_QWID, HM_TAG('q', 'w', 'i', 'd')}, /* Quarter Widths */
        {HM_FEATURE_RAND, HM_TAG('r', 'a', 'n', 'd')}, /* Randomize */
        {HM_FEATURE_RCLT, HM_TAG('r', 'c', 'l', 't')}, /* Required Contextual Alternates */
        {HM_FEATURE_RKRF, HM_TAG('r', 'k', 'r', 'f')}, /* Rakar Forms */
        {HM_FEATURE_RLIG, HM_TAG('r', 'l', 'i', 'g')}, /* Required Ligatures */
        {HM_FEATURE_RPHF, HM_TAG('r', 'p', 'h', 'f')}, /* Reph Forms */
        {HM_FEATURE_RTBD, HM_TAG('r', 't', 'b', 'd')}, /* Right Bounds */
        {HM_FEATURE_RTLA, HM_TAG('r', 't', 'l', 'a')}, /* Right-to-left alternates */
        {HM_FEATURE_RTLM, HM_TAG('r', 't', 'l', 'm')},  /* Right-to-left mirrored forms */
        {HM_FEATURE_RUBY, HM_TAG('r', 'u', 'b', 'y')}, /* Ruby Notation Forms */
        {HM_FEATURE_RVRN, HM_TAG('r', 'v', 'r', 'n')}, /* Required Variation Alternates */
        {HM_FEATURE_SALT, HM_TAG('s', 'a', 'l', 't')}, /* Stylistic alternates */
        {HM_FEATURE_SINF, HM_TAG('s', 'i', 'n', 'f')}, /* Scientific Inferiors */
        {HM_FEATURE_SIZE, HM_TAG('s', 'i', 'z', 'e')}, /* Optical size */
        {HM_FEATURE_SMCP, HM_TAG('s', 'm', 'c', 'p')}, /* Small Capitals */
        {HM_FEATURE_SMPL, HM_TAG('s', 'm', 'p', 'l')}, /* Simplified Forms */
        /* 	ss01 - ss20 Stylistic Sets */
        {HM_FEATURE_SSTY, HM_TAG('s', 's', 't', 'y')}, /* Math script style alternates */
        {HM_FEATURE_STCH, HM_TAG('s', 't', 'c', 'h')}, /* Stretching Glyph Decomposition */
        {HM_FEATURE_SUBS, HM_TAG('s', 'u', 'b', 's')}, /* Subscript */
        {HM_FEATURE_SUPS, HM_TAG('s', 'u', 'p', 's')}, /* Superscript */
        {HM_FEATURE_SWSH, HM_TAG('s', 'w', 's', 'h')}, /* Swash */
        {HM_FEATURE_TITL, HM_TAG('t', 'i', 't', 'l')}, /* Titling */
        {HM_FEATURE_TJMO, HM_TAG('t', 'j', 'm', 'o')}, /* Trailing Jamo Forms */
        {HM_FEATURE_TNAM, HM_TAG('t', 'n', 'a', 'm')}, /* Traditional Name Forms */
        {HM_FEATURE_TNUM, HM_TAG('t', 'n', 'u', 'm')}, /* Tabular Figures */
        {HM_FEATURE_TRAD, HM_TAG('t', 'r', 'a', 'd')}, /* Traditional Forms */
        {HM_FEATURE_TWID, HM_TAG('t', 'w', 'i', 'd')}, /* Third Widths */
        {HM_FEATURE_UNIC, HM_TAG('u', 'n', 'i', 'c')}, /* Unicase */
        {HM_FEATURE_VALT, HM_TAG('v', 'a', 'l', 't')}, /* Alternate Vertical Metrics */
        {HM_FEATURE_VATU, HM_TAG('v', 'a', 't', 'u')}, /* Vattu Variants */
        {HM_FEATURE_VERT, HM_TAG('v', 'e', 'r', 't')}, /* Vertical Writing */
        {HM_FEATURE_VHAL, HM_TAG('v', 'h', 'a', 'l')}, /* Alternate Vertical Half Metrics */
        {HM_FEATURE_VJMO, HM_TAG('v', 'j', 'm', 'o')}, /* Vowel Jamo Forms */
        {HM_FEATURE_VKNA, HM_TAG('v', 'k', 'n', 'a')}, /* Vertical Kana Alternates */
        {HM_FEATURE_VKRN, HM_TAG('v', 'k', 'r', 'n')}, /* Vertical Kerning */
        {HM_FEATURE_VPAL, HM_TAG('v', 'p', 'a', 'l')}, /* Proportional Alternate Vertical Metrics */
        {HM_FEATURE_VRT2, HM_TAG('v', 'r', 't', '2')}, /* Vertical Alternates and Rotation */
        {HM_FEATURE_VRTR, HM_TAG('v', 'r', 't', 'r')}, /* Vertical Alternates for Rotation */
        {HM_FEATURE_ZERO, HM_TAG('z', 'e', 'r', 'o')}, /* Slashed Zero */
};

static uint32_t
hm_ot_calc_table_checksum(const uint32_t *table, uint32_t len) {
    uint32_t sum = 0;
    const uint32_t *endptr = table + ((len + 3) & ~3) / sizeof(uint32_t);

    while (table < endptr)
        sum += *table++;

    return sum;
}


/*
    TTC Header Version 2.0
*/
typedef struct HM_PACKED hm_ttc_header_t {
    /* Font Collection ID string: 'ttcf' (used for fonts with CFF or CFF2 outlines as well as TrueType outlines) */
    hm_tag ttcTag;

    /* 	Major version of the TTC Header */
    hm_uint16 majorVersion;

    /* Minor version of the TTC Header */
    hm_uint16 minorVersion;

    /* Number of fonts in TTC */
    uint32_t numFonts;

    /* Array of offsets to the OffsetTable for each font from the beginning of the file */
    hm_offset32 *offsetTable;

    /* Tag indicating that a DSIG table exists, 0x44534947 ('DSIG') (null if no signature) */
    uint32_t dsigTag;

    /* The length (in bytes) of the DSIG table (null if no signature) */
    uint32_t dsigLength;

    /* The offset (in bytes) of the DSIG table from the beginning of the TTC file (null if no signature) */
    uint32_t dsigOffset;
} hm_ttc_header_t;
/*
typedef struct hm_font_t {
    FT_Face ft_face;
    FT_Bytes base_table;
    FT_Bytes gdef_table;
    FT_Bytes gpos_table;
    FT_Bytes gsub_table;
    FT_Bytes jstf_table;
} hm_font_t;
*/
typedef enum hm_script_t {
    HM_SCRIPT_ARABIC,
    HM_SCRIPT_LATIN
} hm_script_t;

typedef enum hm_language_t {
    HM_LANGUAGE_ARABIC,
    HM_LANGUAGE_ENGLISH,
    HM_LANGUAGE_FRENCH,
} hm_language_t;

typedef enum hm_dir_t {
    HM_DIR_LTR,
    HM_DIR_RTL,
} hm_dir_t;

typedef struct hm_run_t {
    hm_array_t *input;
    hm_array_t *output;

    unsigned int flags;
} hm_run_t;

static hm_language_t
hm_lang(const char *s) {
    if (!strcmp(s, "ar")) return HM_LANGUAGE_ARABIC;
    else if (!strcmp(s, "en")) return HM_LANGUAGE_ENGLISH;
}

#define HM_LANG(lang_str) hm_lang(lang_str)

static hm_run_t *
hm_run_create(void) {
    hm_run_t *run = (hm_run_t *) malloc(sizeof(hm_run_t));
    run->input = hm_array_create();
    run->output = hm_array_create();
    return run;
}

typedef struct {
    const hm_byte *mem;
    hm_size length;
    hm_size offset;
} hm_utf8_dec_t;

#define HM_UTF8_END -1
#define HM_UTF8_ERROR -2

static int
hm_utf8_get(hm_utf8_dec_t *dec) {
    if (dec->offset >= dec->length)
        return HM_UTF8_END;

    return dec->mem[dec->offset++];
}

static int
hm_utf8_cont(hm_utf8_dec_t *dec) {
    int c = hm_utf8_get(dec);
    return ((c & 0xC0) == 0x80)
           ? (c & 0x3F)
           : HM_UTF8_ERROR;
}

static int
hm_utf8_next(hm_utf8_dec_t *dec) {
    int c;
    int c1;
    int c2;
    int c3;
    int r;
    c = hm_utf8_get(dec);

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
        c1 = hm_utf8_cont(dec);
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
        c1 = hm_utf8_cont(dec);
        c2 = hm_utf8_cont(dec);
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
        c1 = hm_utf8_cont(dec);
        c2 = hm_utf8_cont(dec);
        c3 = hm_utf8_cont(dec);
        if ((c1 | c2 | c3) >= 0) {
            r = ((c & 0x07) << 18) | (c1 << 12) | (c2 << 6) | c3;
            if (r >= 65536 && r <= 1114111) {
                return r;
            }
        }
    }

    return HM_UTF8_ERROR;
}

static void
hm_run_load_utf8(hm_run_t *run, const hm_char *text, size_t len) {
    hm_unicode code;
    int ch;

    hm_utf8_dec_t dec;
    dec.mem = (const hm_byte *) text;
    dec.length = len;
    dec.offset = 0;

    /* TODO: do proper error handling for the UTF-8 decoder */
    while ((ch = hm_utf8_next(&dec)) > 0) {
        code = (hm_unicode) ch;
        hm_array_push_back(run->input, code);
    }
}

static void
hm_run_load_utf8_zt(hm_run_t *run, const hm_char *text) {
    hm_run_load_utf8(run, text, UINT64_MAX);
}


typedef struct HM_PACKED hm_rec16_t {
    hm_tag tag;
    hm_offset16 offset;
} hm_rec16_t;

typedef struct HM_PACKED hm_rec32_t {
    hm_tag tag;
    hm_offset32 offset;
} hm_rec32_t;

typedef struct HM_PACKED hm_script_list_t {
    hm_uint16 scriptCount;
    //hm_rec32_t *scriptRecords;
} hm_script_list_t;

typedef struct HM_PACKED hm_script_table_t {
    hm_offset16 defaultLangSys;
    hm_uint16 langSysCount;
    //hm_rec16_t *langSysRecords;
} hm_script_table_t;

typedef struct hm_lang_sys_t {
    /* = NULL (reserved for an offset to a reordering table) */
    hm_offset16 lookupOrder;

    /* Index of a feature required for this language system; if no required features = 0xFFFF */
    hm_uint16 requiredFeatureIndex;

    /* Number of feature index values for this language system — excludes the required feature */
    hm_uint16 featureIndexCount;

    /* Array of indices into the FeatureList, in arbitrary order */
    //HM_Uint16 *featureIndices;
} hm_lang_sys_t;

typedef struct HM_PACKED hm_feature_list_t {
    hm_uint16 featureCount;
    hm_rec16_t *featureRecord;
} hm_feature_list_t;

typedef struct hm_feature_table_t {
    /* = NULL (reserved for offset to FeatureParams) */
    hm_offset16 feature_params;

    /* Number of LookupList indices for this feature */
    uint16_t lookup_index_count;

    /* 	Array of indices into the LookupList — zero-based (first lookup is LookupListIndex = 0) */
//    uint16_t *lookup_list_indices;
} hm_feature_table_t;


struct hm_lookup_list_t {
    /* Number of lookups in this table */
    hm_uint16 lookupCount;

    /* Array of offsets to Lookup tables, from beginning of LookupList — zero based (first lookup is Lookup index = 0) */
    //HM_Offset16 *lookups;
};
/*
static const char *hm_GSUB_lookup_type_string(HM_Uint16 type);
static const char *hm_GPOS_lookup_type_string(HM_Uint16 type);
*/


typedef enum hm_lookup_flag_t {
    /*
    This bit relates only to
    the correct processing of the cursive attachment lookup type (GPOS lookup type 3).
    When this bit is set, the last glyph in a given sequence to
    which the cursive attachment lookup is applied, will be positioned on the baseline.
    */
    HM_LOOKUP_FLAG_RIGHT_TO_LEFT = 0x0001,

    /* If set, skips over base glyphs */
    HM_LOOKUP_FLAG_IGNORE_BASE_GLYPHS = 0x0002,

    /* If set, skips over ligatures */
    HM_LOOKUP_FLAG_IGNORE_LIGATURES = 0x0004,

    /* If set, skips over all combining marks */
    HM_LOOKUP_FLAG_IGNORE_MARKS = 0x0008,

    /*
    If set, indicates that the lookup table structure is followed by a MarkFilteringSet field.
    The layout engine skips over all mark glyphs not in the mark filtering set indicated.
    */
    HM_LOOKUP_FLAG_USE_MARK_FILTERING_SET = 0x0010,

    /* 0x00E0 - For future use (Set to zero) */

    /* If not zero, skips over all marks of attachment type different from specified. */
    HM_LOOKUP_FLAG_MARK_ATTACHMENT_TYPE_MASK = 0xFF00
} hm_lookup_flag_t;

typedef enum hm_gsub_lookup_type_t {
    HM_GSUB_LOOKUP_TYPE_SINGLE_SUBSTITUTION = 1,
    HM_GSUB_LOOKUP_TYPE_MULTIPLE_SUBSTITUTION = 2,
    HM_GSUB_LOOKUP_TYPE_ALTERNATE_SUBSTITUTION = 3,
    HM_GSUB_LOOKUP_TYPE_LIGATURE_SUBSTITUTION = 4,
    HM_GSUB_LOOKUP_TYPE_CONTEXTUAL_SUBSTITUTION = 5,
    HM_GSUB_LOOKUP_TYPE_CHAINED_CONTEXTS_SUBSTITUTION = 6,
    HM_GSUB_LOOKUP_TYPE_EXTENSION_SUBSTITUTION = 7,
    HM_GSUB_LOOKUP_TYPE_REVERSE_CHAINING_CONTEXTUAL_SINGLE_SUBSTITUTION = 8,
} hm_gsub_lookup_type_t;

typedef struct hm_lookup_table_t {
    uint16_t lookup_type;
    uint16_t lookup_flag;
    uint16_t subtable_count;
    hm_offset32 *subtable_offsets;
    uint16_t mark_filtering_set;
} hm_lookup_table_t;

typedef struct hm_coverage_format1_t {
    hm_uint16 coverageFormat; /* Format identifier — format = 1 */
    hm_uint16 glyphCount; /* Number of glyphs in the glyph array */
    hm_id *glyphArray; /* Array of glyph IDs — in numerical order */
} hm_coverage_format1_t;

typedef struct hm_range_rec_t {
    hm_id startGlyphID;
    hm_id endGlyphID;
    hm_uint16 startCoverageIndex;
} hm_range_rec_t;

typedef struct hm_coverage_format2_t {
    hm_uint16 coverageFormat; /* Format identifier — format = 2 */
    hm_uint16 rangeCount; /* Number of RangeRecords */
    hm_range_rec_t *rangeRecords; /* Array of glyph ranges — ordered by startGlyphID. */
} hm_coverage_format2_t;


struct hm_class_def_format1_t {
    hm_uint16 format; /* Format identifier — format = 1 */
    hm_id startGID; /* First glyph ID of the classValueArray */
    hm_uint16 glyphCount; /* Size of the classValueArray */
    hm_uint16 *classValues; /* Array of Class Values — one per glyph ID */
};

struct hm_class_range_rec_t {
    hm_id startGlyphID;
    hm_id endGlyphID;
    hm_uint16 classValue;
};

struct hm_class_def_format2_t {
    hm_uint16 format;
    hm_uint16 ranageCount;
    struct hm_class_range_rec_t *rangeRecords;
};

typedef enum hm_delta_format_t {
    /* Signed 2-bit value, 8 values per uint16 */
    HM_DELTA_FORMAT_LOCAL_2_BIT = 0x0001,

    /* Signed 4-bit value, 4 values per uint16 */
    HM_DELTA_FORMAT_LOCAL_4_BIT = 0x0002,

    /* Signed 8-bit value, 2 values per uint16 */
    HM_DELTA_FORMAT_LOCAL_8_BIT = 0x0003,

    /* VariationIndex table, contains a delta-set index pair. */
    HM_DELTA_FORMAT_VARIATION_INDEX = 0x8000,

    /* 0x7FFC Reserved */
} hm_delta_format_t;

typedef struct HM_PACKED hm_delta_table_t {
    hm_uint16 smallestPPEM;
    hm_uint16 largestPPEM;
    hm_delta_format_t format;
    hm_uint16 *deltaValues;
} hm_delta_table_t;

typedef struct HM_PACKED hm_variation_index_table_t {
    /* A delta-set outer index — used to select an item variation data subtable within the item variation store. */
    hm_uint16 outerIndex;

    /* A delta-set inner index — used to select a delta-set row within an item variation data subtable. */
    hm_uint16 innerIndex;

    /* Format, = 0x8000 */
    hm_delta_format_t format;
} hm_variation_index_table_t;

typedef struct HM_PACKED hm_feature_variation_record_t {
    /* Offset to a condition set table, from beginning of FeatureVariations table. */
    hm_offset32 conditionSetOffset;

    /*
        Offset to a feature table substitution table,
        from beginning of the FeatureVariations table.
    */
    hm_offset32 featureTableSubstitutionOffset;
} hm_feature_variation_record_t;


typedef struct HM_PACKED hm_feature_variations_table_t {
    /* Major version of the FeatureVariations table — set to 1. */
    hm_uint16 versionMajor;

    /* Minor version of the FeatureVariations table — set to 0. */
    hm_uint16 versionMinor;

    /* Number of feature variation records. */
    hm_uint16 featureVariationRecordCount;

    /* Array of feature variation records. */
    hm_feature_variation_record_t *featureVariationRecord;
} hm_feature_variations_table_t;

typedef struct HM_PACKED hm_condition_set_t {
    /* Number of conditions for this condition set. */
    hm_uint16 conditionCount;

    /* Array of offsets to condition tables, from beginning of the ConditionSet table. */
    hm_offset32 *conditions;
} hm_condition_set_t;

typedef struct HM_PACKED hm_condition_format1_t {
    /* Format, = 1 */
    hm_uint16 format;

    /* Index (zero-based) for the variation axis within the 'fvar' table. */
    hm_uint16 axisIndex;

    /* Minimum value of the font variation instances that satisfy this condition. */
    HM_F2DOT14 filterRangeMinValue;

    /* Maximum value of the font variation instances that satisfy this condition. */
    HM_F2DOT14 filterRangeMaxValue;
} hm_condition_format1_t;

typedef struct HM_PACKED hm_feature_table_substitution_record_t {
    /* The feature table index to match. */
    hm_uint16 featureIndex;

    /* Offset to an alternate feature table, from start of the FeatureTableSubstitution table. */
    hm_offset32 alternateFeatureOffset;
} hm_feature_table_substitution_record_t;

typedef struct HM_PACKED hm_feature_table_substitution_table_t {
    /* Major version of the feature table substitution table — set to 1 */
    hm_uint16 majorVersion;

    /* Minor version of the feature table substitution table — set to 0. */
    hm_uint16 minorVersion;

    /* Number of feature table substitution records. */
    hm_uint16 substitutionCount;

    /* Array of feature table substitution records. */
    hm_feature_table_substitution_record_t *substitutions;
} hm_feature_table_substitution_table_t;

typedef struct hm_bitset_t {
    uint8_t *data;
    uint16_t bit_count;
} hm_bitset_t;

static hm_uint64 hm_next_pow2m1(hm_uint64 x) {
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x |= x >> 32;
    return x;
}

static hm_bitset_t *
hm_bitset_create(uint16_t bit_count) {
    hm_bitset_t *bitset = (hm_bitset_t *) malloc(sizeof(hm_bitset_t));

    bitset->bit_count = bit_count;
    uint16_t byte_count = (bit_count / 8) + 1;

    bitset->data = (uint8_t *) calloc(byte_count, 1);

    return bitset;
}

static hm_bool
hm_bitset_set(hm_bitset_t *bitset, uint16_t index, hm_bool value) {
    if (index < bitset->bit_count) {
        uint8_t *byte = bitset->data + (index / 8);
        uint8_t mask = 1 << (index % 8);

        if (value == HM_TRUE)
            *byte |= mask;
        else if (value == HM_FALSE)
            *byte &= ~mask;

        return HM_TRUE;
    }

    return HM_FALSE;
}

static hm_bool
hm_bitset_check(const hm_bitset_t *bitset, uint16_t index) {
    hm_bool value = HM_FALSE;

    if (index < bitset->bit_count) {
        uint8_t byte = *(bitset->data + (index / 8));
        uint8_t mask = 1 << (index % 8);

        if (byte & mask)
            value = HM_TRUE;
    }

    return value;
}

static void
hm_bitset_copy(hm_bitset_t *dst, const hm_bitset_t *src) {
    size_t byte_count = src->bit_count / 8;
    HM_ASSERT(src->bit_count == dst->bit_count);

    memcpy(dst->data, src->data, byte_count);
}


#define HM_OT_TAG_GSUB HM_TAG('G','S','U','B')
#define HM_OT_TAG_GPOS HM_TAG('G','P','O','S')
#define HM_OT_TAG_GDEF HM_TAG('G','D','E','F')
#define HM_OT_TAG_JSTF HM_TAG('J','S','T','F')


hm_feature_t
hm_ot_feature_from_tag(hm_tag tag);


void
hm_ot_layout_collect_lookups(hm_face_t *face,
                             hm_tag table_tag,
                             hm_tag script,
                             hm_tag language,
                             const hm_bitset_t *feature_bits,
                             hm_set_t *lookup_indices);


void
hm_ot_layout_lookups_substitute_closure(hm_face_t *face,
                                          const hm_set_t *lookups,
                                          hm_set_t *glyphs);


hm_bool
hm_ot_layout_lookup_would_substitute(hm_face_t *face,
                                       unsigned int lookup_index,
                                       const hm_id *glyphs,
                                       unsigned int glyph_count,
                                       hm_bool zero_context);


void
hm_ot_layout_apply_lookup(hm_face_t *face,
                            hm_tag table_tag,
                            uint16_t lookup_index,
                            hm_array_t *glyph_array);

hm_tag
hm_ot_script_to_tag(hm_script_t script);

hm_tag
hm_ot_language_to_tag(hm_language_t language);

#ifdef __cplusplus
}
#endif

#endif /* HM_OT_H */