#include "hz.h"
#include "util/hz-array.h"
#include "util/hz-map.h"

/* https://docs.microsoft.com/en-us/typography/opentype/spec/cmap#platform-ids */
typedef enum hz_cmap_platform_t  {
    HZ_CMAP_PLATFORM_UNICODE = 0, /* Various */
    HZ_CMAP_PLATFORM_MACINTOSH = 1, /* Script manager code */
    HZ_CMAP_PLATFORM_ISO = 2, /* ISO encoding [deprecated] */
    HZ_CMAP_PLATFORM_WINDOWS = 3, /* Windows encoding */
    HZ_CMAP_PLATFORM_CUSTOM = 4, /* Custom */
    /* Platform ID values 240 through 255 are reserved for user-defined platforms.
     * This specification will never assign these values to a registered platform.
     * Platform ID 2 (ISO) was deprecated as of OpenType version v1.3.
     * */
} hz_cmap_platform_t;

typedef enum hz_cmap_subtable_format_t {
    HZ_CMAP_SUBTABLE_FORMAT_BYTE_ENCODING_TABLE = 0,
    HZ_CMAP_SUBTABLE_FORMAT_SEGMENT_MAPPING_TO_DELTA_VALUES = 4
} hz_cmap_subtable_format_t;

typedef struct hz_cmap_encoding_t {
    uint16_t        platform_id; /* Platform ID. */
    uint16_t        encoding_id; /* Platform-specific encoding ID. */
    hz_offset32_t   subtable_offset; /* Byte offset from beginning of table to the subtable for this encoding. */
} hz_cmap_encoding_t;

static const char *
hz_cmap_platform_to_string(hz_cmap_platform_t platform) {
    switch (platform) {
        case HZ_CMAP_PLATFORM_UNICODE: return "Unicode";
        case HZ_CMAP_PLATFORM_MACINTOSH: return "Macintosh";
        case HZ_CMAP_PLATFORM_ISO: return "ISO";
        case HZ_CMAP_PLATFORM_WINDOWS: return "Windows";
        case HZ_CMAP_PLATFORM_CUSTOM: return "Custom";
        default: return NULL;
    }
}

/*
 * Format 0: Byte encoding table
 * https://docs.microsoft.com/en-us/typography/opentype/spec/cmap#format-0-byte-encoding-table
 * */
typedef struct hz_cmap_subtable_format0_t {
    uint16_t format; /* Format number is set to 0. */
    uint16_t length; /* This is the length in bytes of the subtable. */
    /* For requirements on use of the language field,
     * see “Use of the language field in 'cmap' subtables” in this document.
     * */
    uint16_t language;
    /* An array that maps character codes to glyph index values. */
    uint8_t glyph_id_array[256];
} hz_cmap_subtable_format0_t;


typedef struct hz_cmap_subtable_format4_t {
    uint16_t format;
    uint16_t length;
    uint16_t language;
    uint16_t seg_count_x2;
    uint16_t search_range;
    uint16_t entry_selector;
    uint16_t range_shift;
    uint16_t *end_code;
    uint16_t reserved_pad;
    uint16_t *start_code;
    int16_t *id_delta;
    uint16_t *id_range_offsets;
    uint16_t *glyph_id_array;
} hz_cmap_subtable_format4_t;

static hz_index_t
hz_cmap_unicode_to_id(hz_cmap_subtable_format4_t *st, hz_unicode_t c) {
    uint16_t range_count = st->seg_count_x2 >> 1;
    uint16_t i = 0;
    hz_index_t id;

    /* search for code range */
    while (i < range_count) {
        uint16_t start_code = bswap16(st->start_code[i]);
        uint16_t end_code = bswap16(st->end_code[i]);
        int16_t id_delta = bswap16(st->id_delta[i]);
        uint16_t id_range_offset = bswap16(st->id_range_offsets[i]);

        if (end_code >= c && start_code <= c) {
            if (id_range_offset != 0) {
                uint16_t raw_val = *(&st->id_range_offsets[i] + id_range_offset/2 + (c - start_code));
                id = bswap16( raw_val );
                if (id != 0) id += id_delta;
            } else
                id = id_delta + c;

            return id;
        }

        ++i;
    }

    return 0; /* map to .notdef */
}

hz_bool
hz_cmap_apply_encoding(hz_stream_t *table, hz_sequence_t *sequence,
                       hz_cmap_encoding_t enc)
{
    hz_stream_t subtable = hz_stream_create(table->data + enc.subtable_offset, HZ_BSWAP);
    uint16_t format = unpackh(&subtable);

    switch (format) {
        case HZ_CMAP_SUBTABLE_FORMAT_BYTE_ENCODING_TABLE: break;
        case 2: break;
        case HZ_CMAP_SUBTABLE_FORMAT_SEGMENT_MAPPING_TO_DELTA_VALUES: {
            hz_cmap_subtable_format4_t st;
            unpackv(&subtable, "hhhhhh",
                    &st.length,
                    &st.language,
                    &st.seg_count_x2,
                    &st.search_range,
                    &st.entry_selector,
                    &st.range_shift);

            uint16_t seg_jmp = (st.seg_count_x2>>1) * sizeof(uint16_t);

            const uint8_t *curr_addr = subtable.data + subtable.idx;
            st.end_code = (uint16_t *)curr_addr;
            st.start_code = (uint16_t *)(curr_addr + seg_jmp + sizeof(uint16_t));
            st.id_delta = (int16_t *)(curr_addr + 2*seg_jmp + sizeof(uint16_t));
            st.id_range_offsets = (uint16_t *)(curr_addr + 3*seg_jmp + sizeof(uint16_t));

            /* map unicode characters to glyph indices in sequenceion */
            hz_sequence_node_t *curr_node = sequence->root;

            while (curr_node != NULL) {
                curr_node->id = hz_cmap_unicode_to_id(&st, curr_node->codepoint);
                curr_node = curr_node->next;
            }
            break;
        }
        default:
            return HZ_FALSE;
    }

    return HZ_TRUE;
}

//hz_bool
//hz_cmap_apply_encoding_to_set(buf_t *table,
//                              hz_set_t *codepoints,
//                              hz_set_t *glyphs,
//                              hz_cmap_encoding_t enc)
//{
//    buf_t subtable = createbuf(table->data + enc.subtable_offset, BUF_BSWAP);
//    uint16_t format = unpackh(&subtable);
//
//    switch (format) {
//        case HZ_CMAP_SUBTABLE_FORMAT_BYTE_ENCODING_TABLE: break;
//        case 2: break;
//        case HZ_CMAP_SUBTABLE_FORMAT_SEGMENT_MAPPING_TO_DELTA_VALUES: {
//            hz_cmap_subtable_format4_t st;
//            unpackv(&subtable, "hhhhhh",
//                    &st.length,
//                    &st.language,
//                    &st.seg_count_x2,
//                    &st.search_range,
//                    &st.entry_selector,
//                    &st.range_shift);
//
//            uint16_t seg_jmp = (st.seg_count_x2>>1) * sizeof(uint16_t);
//
//            const uint8_t *curr_addr = subtable.data + subtable.idx;
//            st.end_code = (uint16_t *)curr_addr;
//            st.start_code = (uint16_t *)(curr_addr + seg_jmp + sizeof(uint16_t));
//            st.id_delta = (int16_t *)(curr_addr + 2*seg_jmp + sizeof(uint16_t));
//            st.id_range_offsets = (uint16_t *)(curr_addr + 3*seg_jmp + sizeof(uint16_t));
//
//            /* map unicode characters to glyph indices in sequenceion */
//            size_t index = 0;
//            size_t num_codes = codepoints->count;
//
//            while (index < num_codes) {
//                hz_set_add_no_duplicate(glyphs, hz_cmap_unicode_to_id(&st, codepoints->values[index]));
//                ++index;
//            }
//
//            break;
//        }
//        default:
//            return HZ_FALSE;
//    }
//
//    return HZ_TRUE;
//}

void
hz_map_to_nominal_forms(hz_face_t *face,
                        hz_sequence_t *sequence)
{
    hz_blob_t *blob = hz_face_reference_table(face, HZ_TAG('c','m','a','p'));
    hz_stream_t table = hz_stream_create(hz_blob_get_data(blob), HZ_BSWAP);

    uint16_t version = unpackh(&table);

    /* Table version number must be 0 */
    HZ_ASSERT(version == 0);

    uint16_t num_encodings, enc_idx;
    num_encodings = unpackh(&table);

    HZ_LOG("cmap table encoding count: %d\n", num_encodings);

//    for (enc_idx = 0; enc_idx < num_encodings; ++enc_idx)
    {
        hz_cmap_encoding_t enc = {};
        unpackv(&table, "hhi",
                &enc.platform_id,
                &enc.encoding_id,
                &enc.subtable_offset);

        HZ_LOG("platform: %s\n", hz_cmap_platform_to_string(enc.platform_id));
        HZ_LOG("encoding: %d\n", enc.encoding_id);
        HZ_LOG("subtable-offset: %d\n", enc.subtable_offset);
        hz_cmap_apply_encoding(&table, sequence, enc);
//        if (enc.platform_id == HZ_CMAP_PLATFORM_WINDOWS) {
//            hz_cmap_apply_encoding(table, sequence, enc);
//            break;
//        }


//        switch (enc.platform_id) {
//            case HZ_CMAP_PLATFORM_UNICODE: {
//                uint16_t subtable_format = 0;
//                hz_stream_t *subtable_stream = hz_stream_create(cmap_table->data + enc.subtable_offset,
//                                                                0, 0);
//                hz_stream_read16(subtable_stream, &subtable_format);
//
//                if (subtable_format == 4) {
//
//                }
//                break;
//            }
//            case HZ_CMAP_PLATFORM_MACINTOSH: {
//                break;
//            }
//            case HZ_CMAP_PLATFORM_ISO: {
//                break;
//            }
//            case HZ_CMAP_PLATFORM_WINDOWS: {
//                /* https://docs.microsoft.com/en-us/typography/opentype/spec/cmap#windows-platform-platform-id--3 */
//                switch (enc.encoding_id) {
//                    case 0: { /* Symbol */
//                        break;
//                    }
//                    case 1: { /* Unicode BMP */
//                        break;
//                    }
//                    case 2: { /* ShiftJIS */
//                        break;
//                    }
//                    case 3: { /* PRC */
//                        break;
//                    }
//                    case 4: { /* Big5 */
//                        break;
//                    }
//                    case 5: { /* Wansung */
//                        break;
//                    }
//                    case 6: { /* Johab */
//                        hz_cmap_windows_johab(cmap_table->data + enc.subtable_offset);
//                        break;
//                    }
//
////                    case 7 ... 9: { /* Reserved */
////                        break;
////                    }
//
//                    case 10: { /* Unicode full repertoire */
//                        hz_cmap_windows_unicode_full(cmap_table->data + enc.subtable_offset);
//                        break;
//                    }
//
//                    default:
//                        break;
//                }
//                break;
//            }
//
//            default: /* error */
//                break;
//        }
    }
}


typedef struct hz_long_hor_metric_t {
    uint16_t advance_width; /* advance width */
    int16_t lsb; /* left side bearing */
} hz_long_hor_metric_t;

void
hz_read_h_metrics(hz_stream_t *table, size_t metrics_count, hz_long_hor_metric_t *metrics) {
    size_t index = 0;

    while (index < metrics_count) {
        hz_long_hor_metric_t *metric = &metrics[ index ];
        unpackv(table, "hh", &metric->advance_width, (uint16_t *) &metric->lsb);
        ++index;
    }
}

/*
void
hz_read_lv_metrics();
*/

void
hz_apply_tt1_metrics(hz_face_t *face, hz_sequence_t *sequence)
{
    hz_long_hor_metric_t *h_metrics;
    int16_t *left_side_bearings;

    hz_blob_t *blob = hz_face_reference_table(face, HZ_TAG('h','m','t','x'));
    hz_stream_t table = hz_stream_create(hz_blob_get_data(blob), HZ_BSWAP);

    uint16_t num_of_h_metrics = hz_face_get_num_of_h_metrics(face);
    uint16_t glyph_count = hz_face_get_num_glyphs(face);
    uint16_t num_left_side_bearings = glyph_count - num_of_h_metrics;

    /* read long horizontal metrics */
    h_metrics = HZ_MALLOC(sizeof(hz_long_hor_metric_t) * num_of_h_metrics);
    hz_read_h_metrics(&table, num_of_h_metrics, h_metrics);
    left_side_bearings = HZ_MALLOC(sizeof(int16_t) * num_left_side_bearings);
    unpackv(&table, "h:*", (uint16_t *)left_side_bearings, num_left_side_bearings);

    //hz_fill_tt1_metrics(face, metrics, h_metrics, left_side_bearings, num_of_h_metrics, glyph_count);

//    uint16_t glyph_index = 0;
//    while (glyph_index < glyph_count) {
//
//        hz_metrics_t *metric = &face->metrics[glyph_index];
//        if (glyph_index < num_of_h_metrics) {
//            hz_long_hor_metric_t *h_metric = &h_metrics[glyph_index];
//            metric->x_advance = h_metric->advance_width;
//            int16_t rsb = h_metrics->advance_width - (h_metrics->lsb + metric->x_max - metric->x_min);
//            metric->x_bearing = rsb + metric->width;
//        } else {
//            int16_t lsb = left_side_bearings[num_of_h_metrics + glyph_index];
//            int16_t rsb = lsb + (metric->x_max - metric->x_min);
//            metric->x_bearing = rsb;
//        }
//
//        ++glyph_index;
//    }

    /* apply the metrics to position the glyphs */
    hz_sequence_node_t *curr_node = sequence->root;
    while (curr_node != NULL) {
        hz_index_t id = curr_node->id;
        hz_metrics_t *metric = hz_face_get_glyph_metrics(face, id);

        curr_node->x_advance = metric->x_advance;
        curr_node->y_advance = metric->y_advance;
        curr_node->x_offset = 0;
        curr_node->y_offset = 0;

        curr_node = curr_node->next;
    }

    HZ_FREE(left_side_bearings);
    HZ_FREE(h_metrics);
}

static void
hz_apply_rtl_switch(hz_sequence_t *sequence) {
    hz_sequence_node_t *node = hz_sequence_last_node(sequence->root);
    sequence->root = node;

    while (node != NULL) {
        hz_sequence_node_t *prev, *next;
        prev = node->prev;
        next = node->next;
        node->next = prev;
        node->prev = next;
        node = prev;
    }
}

static void
hz_apply_remove_marks(hz_sequence_t *sequence) {
    hz_sequence_node_t *g = sequence->root;

    while (g != NULL) {
        hz_sequence_node_t *next = g->next;
        if (g->gc & HZ_GLYPH_CLASS_MARK)
            hz_sequence_pop_node(sequence, g);

        g = next;
    }
}

void
hz_decode_hhea_table(hz_face_t *face, hz_blob_t *blob)
{
    hz_stream_t table = hz_stream_create(hz_blob_get_data(blob), HZ_BSWAP);

    uint32_t version;
    FWORD ascender, descender, line_gap;
    UFWORD advance_width_max;
    FWORD min_left_side_bearing;
    FWORD min_right_side_bearing;
    FWORD x_max_extent;
    int16_t caret_slope_rise;
    int16_t caret_slope_run;
    int16_t caret_offset;
    int16_t metric_data_format;
    uint16_t num_of_h_metrics;

    version = unpacki(&table);

    if (version == 0x00010000) {
        /* version 1.0 */
        unpackv(&table, "hhhhhhhhhh",
         (uint16_t *) &ascender,
         (uint16_t *) &descender,
         (uint16_t *) &line_gap,
         &advance_width_max,
         (uint16_t *) &min_left_side_bearing,
         (uint16_t *) &min_right_side_bearing,
         (uint16_t *) &x_max_extent,
         (uint16_t *) &caret_slope_rise,
         (uint16_t *) &caret_slope_run,
         (uint16_t *) &caret_offset);

        /* skip over 8 bytes of reserved space */
        bufseek(&table, 8);

        unpackv(&table, "hh", (uint16_t *) &metric_data_format, &num_of_h_metrics);
    } else {
        /* error */
    }

    hz_face_set_num_of_h_metrics(face, num_of_h_metrics);
}


static hz_set_t *
hz_gather_script_codepoints(hz_script_t script)
{
    hz_set_t *codepoints = hz_set_create();
    int i;

    for (i = 0; i < HZ_ARRLEN(script_ranges); ++i) {
        hz_script_range_t range = script_ranges[i];
        if (range.script == script) {
            hz_set_add_range_no_duplicate(codepoints, range.first_code, range.last_code);
        }
    }

    return codepoints;
}

static void
hz_gather_script_glyphs(hz_face_t *face, hz_script_t script, hz_set_t *glyphs)
{
    hz_set_t *codepoints = hz_gather_script_codepoints(script);
    hz_blob_t *cmap_blob = hz_face_reference_table(face, HZ_TAG('c','m','a','p'));
    hz_stream_t table = hz_stream_create(hz_blob_get_data(cmap_blob), HZ_BSWAP);

    uint16_t version = unpackh(&table);

    if (version == 0) {
        uint16_t num_encodings, enc_idx;
        num_encodings = unpackh(&table);

        hz_cmap_encoding_t enc = {};
        unpackv(&table, "hhi",
                &enc.platform_id,
                &enc.encoding_id,
                &enc.subtable_offset);

//        hz_cmap_apply_encoding_to_set(&table, codepoints, glyphs, enc);

    } else {
        /* error, table version must be 0 */
        HZ_ERROR("cmap table version must be zero!");
    }

    hz_set_destroy(codepoints);
}

/* hz_set_t * */
/* hz_context_gather_required_glyphs(hz_context_t *ctx) */
/* { */
/*     hz_set_t *glyphs = hz_set_create(); */
/*     hz_face_t *face = hz_font_get_face(ctx->font); */
/*     hz_tag_t script_tag = hz_ot_script_to_tag(ctx->script); */
/*     hz_tag_t language_tag = hz_ot_language_to_tag(ctx->language); */

/*     hz_gather_script_glyphs(face, HZ_SCRIPT_COMMON, glyphs); */
/*     hz_gather_script_glyphs(face, HZ_SCRIPT_INHERITED, glyphs); */
/*     hz_gather_script_glyphs(face, ctx->script, glyphs); */

/*     if (hz_face_get_ot_tables(face)->GSUB_table != NULL) */
/*         //hz_ot_layout_gather_glyphs(face, script_tag, language_tag, ctx->features, glyphs); */

/*     return glyphs; */
/* } */

typedef struct hz_language_info_t {
    hz_language_t language;
    hz_tag_t tag;
    const char *iso_639_codes;
} hz_language_info_t;


//#define HZ_TAG4(s) ((s[0]) | ((hz_tag_t)(s[1]) << 8) | ())

// TODO: write variadic HZ_TAG() which fills the remaining characters in 4 byte tag with spaces as
// most tags here are 3 bytes
//

// Created list manullay from https://docs.microsoft.com/en-us/typography/opentype/spec/languagetags
static const hz_language_info_t language_info_list[] = {
    {HZ_LANGUAGE_ABAZA, HZ_TAG('A','B','A',' '), "abq"},
    {HZ_LANGUAGE_ABKHAZIAN, HZ_TAG('A','B','K',' '), "abk"},
    {HZ_LANGUAGE_ACHOLI, HZ_TAG('A','C','H',' '), "ach"},
    {HZ_LANGUAGE_ACHI, HZ_TAG('A','C','R',' '), "acr"},
    {HZ_LANGUAGE_ADYGHE, HZ_TAG('A','D','Y',' '), "ady"},
    {HZ_LANGUAGE_AFRIKAANS, HZ_TAG('A','F','K',' '), "afr"},
    {HZ_LANGUAGE_AFAR, HZ_TAG('A','F','R',' '), "aar"},
    {HZ_LANGUAGE_AGAW, HZ_TAG('A','G','W',' '), "ahg"},
    {HZ_LANGUAGE_AITON, HZ_TAG('A','I','O',' '), "aio"},
    {HZ_LANGUAGE_AKAN, HZ_TAG('A','K','A',' '), "aka, fat, twi"},
    {HZ_LANGUAGE_BATAK_ALGKOLA, HZ_TAG('A','K','B',' '), "akb"},
    {HZ_LANGUAGE_ALSATIAN, HZ_TAG('A','L','S',' '), "gsw"},
    {HZ_LANGUAGE_ALTAI, HZ_TAG('A','L','T',' '), "atv, alt"},
    {HZ_LANGUAGE_AMHARIC, HZ_TAG('A','M','H',' '), "amh"},
    {HZ_LANGUAGE_ANGLO_SAXON, HZ_TAG('A','N','G',' '), "ang"},
    {HZ_LANGUAGE_PHONETIC_TRANSCRIPTION, HZ_TAG('A','P','P','H'), NULL},
    {HZ_LANGUAGE_ARABIC, HZ_TAG('A','R','A',' '), "ara"},
    {HZ_LANGUAGE_ARGONESE, HZ_TAG('A','R','G',' '), "arg"},
    {HZ_LANGUAGE_AARI, HZ_TAG('A','R','I',' '), "aiw"},
    {HZ_LANGUAGE_RAKHINE, HZ_TAG('A','R','K',' '), "mhv, rmz, rki"},
    {HZ_LANGUAGE_ASSAMESE, HZ_TAG('A','S','M',' '), "asm"},
    {HZ_LANGUAGE_ASTURIAN, HZ_TAG('A','S','T',' '), "ast"},
    {HZ_LANGUAGE_ATHAPASKAN, HZ_TAG('A','T','H',' '), "aht, apa, apk, apj, apl, apm, apw, ath, bea, sek, bcr, caf, chp, clc, coq, crx, ctc, den, dgr, gce, gwi, haa, hoi, hup, ing, kkz, koy, ktw, kuu, mvb, nav, qwt, scs, srs, taa, tau, tcb, tce, tfn, tgx, tht, tol, ttm, tuu, txc, wlk, xup, xsl"},
    {HZ_LANGUAGE_AVAR, HZ_TAG('A','V','R',' '), "ava"},
    {HZ_LANGUAGE_AWADHI, HZ_TAG('A','W','A',' '), "awa"},
    {HZ_LANGUAGE_AYMARA, HZ_TAG('A','Y','M',' '), "aym"},
    {HZ_LANGUAGE_TORKI, HZ_TAG('A','Z','B',' '), "azb"},
    {HZ_LANGUAGE_AZERBEIJANI, HZ_TAG('A','Z','E',' '), "aze"},
    {HZ_LANGUAGE_BADAGA, HZ_TAG('B','A','D',' '), "bfq"},
    {HZ_LANGUAGE_BANDA, HZ_TAG('B','A','D','0'), "bad, bbp, bfl, bjo, bpd, bqk, gox, kuw, liy, lna, lnl, mnh, nue, nuu, tor, yaj, zmz"},
    {HZ_LANGUAGE_BAGHELKHANDI, HZ_TAG('B','A','G',' '), "bfy"},
    {HZ_LANGUAGE_BALKAR, HZ_TAG('B','A','L',' '), "krc"},
    {HZ_LANGUAGE_BALINESE, HZ_TAG('B','A','N',' '), "ban"},
    {HZ_LANGUAGE_BAVARIAN, HZ_TAG('B','A','R',' '), "bar"},
    {HZ_LANGUAGE_BAULE, HZ_TAG('B','A','U',' '), "bci"},
    {HZ_LANGUAGE_BATAK_TOBA, HZ_TAG('B','B','C',' '), "bbc"},
    {HZ_LANGUAGE_BERBER, HZ_TAG('B','B','R',' '), "auj, ber, cnu, gha, gho, grr, jbe, jbn, kab, mzb, oua, rif, sds, shi, shy, siz, sjs, swn, taq, tez, thv, thz, tia, tjo, tmh, ttq, tzm, zen, zgh"},
    {HZ_LANGUAGE_BENCH, HZ_TAG('B','C','H',' '), "bcq"},
    {HZ_LANGUAGE_BIBLE_CREE, HZ_TAG('B','C','R',' '), NULL},
    {HZ_LANGUAGE_BANDJALANG, HZ_TAG('B','D','Y',' '), "bdy"},
    {HZ_LANGUAGE_BELARUSSIAN, HZ_TAG('B','E','L',' '), "bel"},
    {HZ_LANGUAGE_BEMBA, HZ_TAG('B','E','M',' '), "bem"},
    {HZ_LANGUAGE_BENGALI, HZ_TAG('B','E','N',' '), "ben"},
    {HZ_LANGUAGE_HARYANVI, HZ_TAG('B','G','C',' '), "bgc"},
    {HZ_LANGUAGE_BAGRI, HZ_TAG('B','G','Q',' '), "bgq"},
    {HZ_LANGUAGE_BULGARIAN, HZ_TAG('B','G','R',' '), "bul"},
    {HZ_LANGUAGE_BHILI, HZ_TAG('B','H','I',' '), "bhi, bhb"},
    {HZ_LANGUAGE_BHOJPURI, HZ_TAG('B','H','O',' '), "bho"},
    {HZ_LANGUAGE_BIKOL, HZ_TAG('B','I','K',' '), "bik, bhk, bcl, bto, cts, bln, fbl, lbl, rbl, ubl"},
    {HZ_LANGUAGE_BILEN, HZ_TAG('B','I','L',' '), "byn"},
    {HZ_LANGUAGE_BISLAMA, HZ_TAG('B','I','S',' '), "bis"},
    {HZ_LANGUAGE_KANAUJI, HZ_TAG('B','J','J',' '), "bjj"},
    {HZ_LANGUAGE_BLACKFOOT, HZ_TAG('B','K','F',' '), "bla"},
    {HZ_LANGUAGE_BALUCHI, HZ_TAG('B','L','I',' '), "bal"},
    {HZ_LANGUAGE_PAO_KAREN, HZ_TAG('B','L','K',' '), "blk"}, /* Pa’o Karen */
    {HZ_LANGUAGE_BALANTE, HZ_TAG('B','L','N',' '), "bjt, ble"},
    {HZ_LANGUAGE_BALTI, HZ_TAG('B','L','T',' '), "bft"},
    {HZ_LANGUAGE_BAMBARA, HZ_TAG('B','M','B',' '), "bam"}, /* Bambara (Bamanankan)  */
    {HZ_LANGUAGE_BAMILEKE, HZ_TAG('B','M','L',' '), "bai, bbj, bko, byv, fmp, jgo, nla, nnh, nnz, nwe, xmg, ybb"},
    {HZ_LANGUAGE_BOSNIAN, HZ_TAG('B','O','S',' '), "bos"},
    {HZ_LANGUAGE_BISHNUPRIYA_MANIPURI, HZ_TAG('B','P','Y',' '), "bpy"},
    {HZ_LANGUAGE_BRETON, HZ_TAG('B','R','E',' '), "bre"},
    {HZ_LANGUAGE_BRAHUI, HZ_TAG('B','R','H',' '), "brh"},
    {HZ_LANGUAGE_BRAJ_BHASHA, HZ_TAG('B','R','I',' '), "bra"},
    {HZ_LANGUAGE_BURMESE, HZ_TAG('B','R','M',' '), "mya"},
    {HZ_LANGUAGE_BODO, HZ_TAG('B','R','X',' '), "brx"},
    {HZ_LANGUAGE_BASHKIR, HZ_TAG('B','S','H',' '), "bak"},
    {HZ_LANGUAGE_BURUSHASKI, HZ_TAG('B','S','K',' '), "bsk"},
    {HZ_LANGUAGE_BATAK_DAIRI, HZ_TAG('B','T','D',' '), "btd"}, /* Batak Dairi (Pakpak) */
    {HZ_LANGUAGE_BETI, HZ_TAG('B','T','I',' '), "btb, beb, bum, bxp, eto, ewo, mct"},
    {HZ_LANGUAGE_BETAK_LANGUAGES, HZ_TAG('B','T','K',' '), "akb, bbc, btd, btk, btm, bts, btx, btz"}, /* Betak languages */
    {HZ_LANGUAGE_BATAK_MANDAILING, HZ_TAG('B','T','M',' '), "btm"},
    {HZ_LANGUAGE_BATAK_SIMALUNGUN, HZ_TAG('B','T','S',' '), "bts"},
    {HZ_LANGUAGE_BATAK_KARO, HZ_TAG('B','T','X',' '), "btx"},
    {HZ_LANGUAGE_BATAK_ALAS_KLUET, HZ_TAG('B','T','Z',' '), "btz"}, /* Batak Alas-Kluet */
    {HZ_LANGUAGE_BUGIS, HZ_TAG('B','U','G',' '), "bug"},
    {HZ_LANGUAGE_MEDUMBA, HZ_TAG('B','Y','V',' '), "byv"},
    {HZ_LANGUAGE_KAQCHIKEL, HZ_TAG('C','A','K',' '), "cak"},
    {HZ_LANGUAGE_CATALAN, HZ_TAG('C','A','T',' '), "cat"},
    {HZ_LANGUAGE_ZAMBOANGA_CHAVACANO, HZ_TAG('C','B','K',' '), "cbk"},
    {HZ_LANGUAGE_CHINANTEC, HZ_TAG('C','C','H','N'), "cco, chj, chq, chz, cle, cnl, cnt, cpa, csa, cso, cte, ctl, cuc, cvn"},
    {HZ_LANGUAGE_CEBUANO, HZ_TAG('C','E','B',' '), "ceb"},
    {HZ_LANGUAGE_CHIGA, HZ_TAG('C','G','G',' '), "cgg"},
    {HZ_LANGUAGE_CHAMORRO, HZ_TAG('C','H','A',' '), "cha"},
    {HZ_LANGUAGE_CHECHEN, HZ_TAG('C','H','E',' '), "che"},
    {HZ_LANGUAGE_CHAHA_GURAGE, HZ_TAG('C','H','G',' '), "sgw"},
    {HZ_LANGUAGE_CHATTISGARHI, HZ_TAG('C','H','H',' '), "hne"},
    {HZ_LANGUAGE_CHICHEWA, HZ_TAG('C','H','I',' '),"nya"}, /* Chichewa (Chewa, Nyanja) */
    {HZ_LANGUAGE_CHUKCHI, HZ_TAG('C','H','K',' '), "ckt"},
    {HZ_LANGUAGE_CHUUKESE, HZ_TAG('C','H','K','0'), "chk"},
    {HZ_LANGUAGE_CHOCTAW, HZ_TAG('C','H','O',' '), "cho"},
    {HZ_LANGUAGE_CHIPEWYAN, HZ_TAG('C','H','P',' '), "chp"},
    {HZ_LANGUAGE_CHEROKEE, HZ_TAG('C','H','R',' '), "chr"},
    {HZ_LANGUAGE_CHUVASH, HZ_TAG('C','H','U',' '), "chv"},
    {HZ_LANGUAGE_CHEYENNE, HZ_TAG('C','H','Y',' '), "chy"},
    {HZ_LANGUAGE_WESTERN_CHAM, HZ_TAG('C','J','A',' '), "cja"},
    {HZ_LANGUAGE_EASTERN_CHAM, HZ_TAG('C','J','M',' '), "cjm"},
    {HZ_LANGUAGE_COMORIAN, HZ_TAG('C','M','R',' '), "swb, wlc, wni, zdj"},
    {HZ_LANGUAGE_COPTIC, HZ_TAG('C','O','P',' '), "cop"},
    {HZ_LANGUAGE_CORNISH, HZ_TAG('C','O','R',' '), "cor"},
    {HZ_LANGUAGE_CORSICAN, HZ_TAG('C','O','S',' '), "cos"},
    {HZ_LANGUAGE_CREOLES, HZ_TAG('C','P','P',' '), "abs, acf, afs, aig, aoa, bah, bew, bis, bjs, bpl, bpq, brc, bxo, bzj, bzk, cbk, ccl, ccm, chn, cks, cpe, cpf, cpi, cpp, cri, crp, crs, dcr, dep, djk, fab, fng, fpe, gac, gcf, gcl, gcr, gib, goq, gpe, gul, gyn, hat, hca, hmo, hwc, icr, idb, ihb, jam, jvd, kcn, kea, kmv, kri, kww, lir, lou, lrt, max, mbf, mcm, mfe, mfp, mkn, mod, msi, mud, mzs, nag, nef, ngm, njt, onx, oor, pap, pcm, pea, pey, pga, pih, pis, pln, pml, pmy, pov, pre, rcf, rop, scf, sci, skw, srm, srn, sta, svc, tas, tch, tcs, tgh, tmg, tpi, trf, tvy, uln, vic, vkp, wes, xmm"},
    {HZ_LANGUAGE_CREE, HZ_TAG('C','R','E',' '), "cre"},
    {HZ_LANGUAGE_CARRIER, HZ_TAG('C','R','R',' '), "crx, caf"},
    {HZ_LANGUAGE_CRIMEAN_TATAR, HZ_TAG('C','R','T',' '), "crh"},
    {HZ_LANGUAGE_KASHUBIAN, HZ_TAG('C','S','B',' '), "csb"},
    {HZ_LANGUAGE_CHURCH_SLAVONIC, HZ_TAG('C','S','L',' '), "chu"},
    {HZ_LANGUAGE_CZECH, HZ_TAG('C','S','Y',' '), "ces"},
    {HZ_LANGUAGE_CHITTAGONIAN, HZ_TAG('C','T','G',' '), "ctg"},
    {HZ_LANGUAGE_SAN_BLAS_KUNA, HZ_TAG('C','U','K',' '), "cuk"},
    {HZ_LANGUAGE_DAGBANI, HZ_TAG('D','A','G',' '), "dag"},
    {HZ_LANGUAGE_DANISH, HZ_TAG('D','A','N',' '), "dan"},
    {HZ_LANGUAGE_DARGWA, HZ_TAG('D','A','R',' '), "dar"},
    {HZ_LANGUAGE_DAYI, HZ_TAG('D','A','X',' '), "dax"},
    {HZ_LANGUAGE_WOODS_CREE, HZ_TAG('D','C','R',' '), "cwd"},
    {HZ_LANGUAGE_GERMAN, HZ_TAG('D','E','U',' '), "deu"},
    {HZ_LANGUAGE_DOGRI, HZ_TAG('D','G','O',' '), "dgo"}, /* Dogri (individual language) */
    {HZ_LANGUAGE_DOGRI_MACRO, HZ_TAG('D','G','R',' '), "doi"}, /* Dogri (macrolanguage) */
    {HZ_LANGUAGE_DHANGU, HZ_TAG('D','H','G',' '), "dhg"},
/*    {HZ_LANGUAGE_DIVEHI, HZ_TAG('D','H','V',' '), "div"},*/ /* deprecated tag 'DHV ' */
    {HZ_LANGUAGE_DIMLI, HZ_TAG('D','I','Q',' '), "diq"},
    {HZ_LANGUAGE_DIVEHI, HZ_TAG('D','I','V',' '), "div"}, /* Divehi (Dhivehi, Maldivian) */
    {HZ_LANGUAGE_ZARMA, HZ_TAG('D','J','R',' '), "dje"},
    {HZ_LANGUAGE_DJAMBARRPUYNGU, HZ_TAG('D','J','R','0'), "djr"},
    {HZ_LANGUAGE_DANGME, HZ_TAG('D','N','G',' '), "ada"},
    {HZ_LANGUAGE_DAN, HZ_TAG('D','N','J',' '), "dnj"},
    {HZ_LANGUAGE_DINKA, HZ_TAG('D','N','K',' '), "din"},
    {HZ_LANGUAGE_DARI, HZ_TAG('D','R','I',' '), "prs"},
    {HZ_LANGUAGE_DHUWAL, HZ_TAG('D','U','J',' '), "duj, dwu, dwy"},
    {HZ_LANGUAGE_DUNGAN, HZ_TAG('D','U','N',' '), "dng"},
    {HZ_LANGUAGE_DZONGKHA, HZ_TAG('D','Z','N',' '), "dzo"},
    {HZ_LANGUAGE_EBIRA, HZ_TAG('E','B','I',' '), "igb"},
    {HZ_LANGUAGE_EASTERN_CREE, HZ_TAG('E','C','R',' '), "crj, crl"},
    {HZ_LANGUAGE_EDO, HZ_TAG('E','D','O',' '), "bin"},
    {HZ_LANGUAGE_EFIK, HZ_TAG('E','F','I',' '), "efi"},
    {HZ_LANGUAGE_GREEK, HZ_TAG('E','L','L',' '), "ell"},
    {HZ_LANGUAGE_EASTERN_MANINKAKAN, HZ_TAG('E','M','K',' '), "emk"},
    {HZ_LANGUAGE_ENGLISH, HZ_TAG('E','N','G',' '), "eng"},
    {HZ_LANGUAGE_ERZYA, HZ_TAG('E','R','Z',' '), "myv"},
    {HZ_LANGUAGE_SPANISH, HZ_TAG('E','S','P',' '), "spa"},
    {HZ_LANGUAGE_CENTRAL_YUPIK, HZ_TAG('E','S','U',' '), "esu"},
    {HZ_LANGUAGE_ESTONIAN, HZ_TAG('E','T','I',' '), "est"},
    {HZ_LANGUAGE_BASQUE, HZ_TAG('E','U','Q',' '), "eus"},
    {HZ_LANGUAGE_EVENKI, HZ_TAG('E','V','K',' '), "evn"},
    {HZ_LANGUAGE_EVEN, HZ_TAG('E','V','N',' '), "eve"},
    {HZ_LANGUAGE_EWE, HZ_TAG('E','W','E',' '), "ewe"},
    {HZ_LANGUAGE_FRENCH_ANTILLEAN, HZ_TAG('F','A','N',' '), "acf"},
    {HZ_LANGUAGE_FANG, HZ_TAG('F','A','N','0'), "fan"},
    {HZ_LANGUAGE_PERSIAN, HZ_TAG('F','A','R',' '), "fas"},
    {HZ_LANGUAGE_FANTI, HZ_TAG('F','A','T',' '), "fat"},
    {HZ_LANGUAGE_FINNISH, HZ_TAG('F','I','N',' '), "fin"},
    {HZ_LANGUAGE_FIJIAN, HZ_TAG('F','J','I',' '), "fij"},
    {HZ_LANGUAGE_DUTCH, HZ_TAG('F','L','E',' '), "vls"}, /* Dutch (Flemish) */
    {HZ_LANGUAGE_FEFE, HZ_TAG('F','M','P',' '), "fmp"}, /* Fe'fe' */
    {HZ_LANGUAGE_FOREST_ENETS, HZ_TAG('F','N','E',' '), "enf"},
    {HZ_LANGUAGE_FON, HZ_TAG('F','O','N',' '), "fon"},
    {HZ_LANGUAGE_FAROESE, HZ_TAG('F','O','S',' '), "fao"},
    {HZ_LANGUAGE_FRENCH, HZ_TAG('F','R','A',' '), "fra"},
    {HZ_LANGUAGE_CAJUN_FRENCH, HZ_TAG('F','R','C',' '), "frc"},
    {HZ_LANGUAGE_FRISIAN, HZ_TAG('F','R','I',' '), "fry"},
    {HZ_LANGUAGE_FRIULIAN, HZ_TAG('F','R','L',' '), "fur"},
    {HZ_LANGUAGE_ARPITAN, HZ_TAG('F','R','P',' '), "frp"},
    {HZ_LANGUAGE_FUTA, HZ_TAG('F','T','A',' '), "fuf"},
    {HZ_LANGUAGE_FULAH, HZ_TAG('F','U','L',' '), "ful"},
    {HZ_LANGUAGE_NIGERIAN_FULFULDE, HZ_TAG('F','U','V',' '), "fuv"},
    {HZ_LANGUAGE_GA, HZ_TAG('G','A','D',' '), "gaa"},
    {HZ_LANGUAGE_SCOTTISH_GAELIC, HZ_TAG('G','A','E',' '), "gla"}, /* Scottish Gaelic (Gaelic) */
    {HZ_LANGUAGE_GAGAUZ, HZ_TAG('G','A','G',' '), "gag"},
    {HZ_LANGUAGE_GALICIAN, HZ_TAG('G','A','L',' '), "glg"},
    {HZ_LANGUAGE_GARSHUNI, HZ_TAG('G','A','R',' '), NULL},
    {HZ_LANGUAGE_GARHWALI, HZ_TAG('G','A','W',' '), "gbm"},
    {HZ_LANGUAGE_GEEZ, HZ_TAG('G','E','Z',' '), "gez"},
    {HZ_LANGUAGE_GITHABUL, HZ_TAG('G','I','H',' '), "gih"},
    {HZ_LANGUAGE_GILYAK, HZ_TAG('G','I','L',' '), "niv"},
    {HZ_LANGUAGE_KIRIBATI, HZ_TAG('G','I','L','0'), "gil"}, /* Kiribati (Gilbertese) */
    {HZ_LANGUAGE_KPELLE, HZ_TAG('G','K','P',' '), "gkp"}, /* Kpelle (Guinea) */
    {HZ_LANGUAGE_GILAKI, HZ_TAG('G','L','K',' '), "glk"},
    {HZ_LANGUAGE_GUMUZ, HZ_TAG('G','M','Z',' '), "guk"},
    {HZ_LANGUAGE_GUMATJ, HZ_TAG('G','N','N',' '), "gnn"},
    {HZ_LANGUAGE_GOGO, HZ_TAG('G','O','G',' '), "gog"},
    {HZ_LANGUAGE_GONDI, HZ_TAG('G','O','N',' '), "gon"},
    {HZ_LANGUAGE_GREENLANDIC, HZ_TAG('G','R','N',' '), "kal"},
    {HZ_LANGUAGE_GARO, HZ_TAG('G','R','O',' '), "grt"},
    {HZ_LANGUAGE_GUARANI, HZ_TAG('G','U','A',' '), "grn"},
    {HZ_LANGUAGE_WAYUU, HZ_TAG('G','U','C',' '), "guc"},
    {HZ_LANGUAGE_GUPAPUYNGU, HZ_TAG('G','U','F',' '), "guf"},
    {HZ_LANGUAGE_GUJARATI, HZ_TAG('G','U','J',' '), "guj"},
    {HZ_LANGUAGE_GUSII, HZ_TAG('G','U','Z',' '), "guz"},
    {HZ_LANGUAGE_HAITIAN, HZ_TAG('H','A','I',' '), "hat"}, /* Haitian (Haitian Creole) */
    {HZ_LANGUAGE_HALAM, HZ_TAG('H','A','L',' '), "cfm"}, /* Halam (Falam Chin) */
    {HZ_LANGUAGE_HARAUTI, HZ_TAG('H','A','R',' '), "hoj"},
    {HZ_LANGUAGE_HAUSA, HZ_TAG('H','A','U',' '), "hau"},
    {HZ_LANGUAGE_HAWAIIAN, HZ_TAG('H','A','W',' '), "haw"},
    {HZ_LANGUAGE_HAYA, HZ_TAG('H','A','Y', ' '), "hay"},
    {HZ_LANGUAGE_HAZARAGI, HZ_TAG('H','A','Z',' '), "haz"},
    {HZ_LANGUAGE_HAMMER_BANNA, HZ_TAG('H','B','N',' '), "amf"}, /* Hammer-Banna */
    {HZ_LANGUAGE_HERERO, HZ_TAG('H','E','R',' '), "her"},
    {HZ_LANGUAGE_HILIGAYNON, HZ_TAG('H','I','L',' '), "hil"},
    {HZ_LANGUAGE_HINDI, HZ_TAG('H','I','N',' '), "hin"},
    {HZ_LANGUAGE_HIGH_MARI, HZ_TAG('H','M','A',' '), "mrj"},
    {HZ_LANGUAGE_HMONG, HZ_TAG('H','M','N',' '), "hmn"},
    {HZ_LANGUAGE_HIRI_MOTU, HZ_TAG('H','M','O',' '), "hmo"},
    {HZ_LANGUAGE_HINDKO, HZ_TAG('H','N','D',' '), "hno, hnd"},
    {HZ_LANGUAGE_HO, HZ_TAG('H','O',' ',' '), "hoc"},
    {HZ_LANGUAGE_HARARI, HZ_TAG('H','R','I',' '), "har"},
    {HZ_LANGUAGE_CROATIAN, HZ_TAG('H','R','V',' '), "hrv"},
    {HZ_LANGUAGE_HUNGARIAN, HZ_TAG('H','U','N',' '), "hun"},
    {HZ_LANGUAGE_ARMENIAN, HZ_TAG('H','Y','E',' '), "hye, hyw"},
    {HZ_LANGUAGE_ARMENIAN_EAST, HZ_TAG('H','Y','E','0'), "hye"},
    {HZ_LANGUAGE_IBAN, HZ_TAG('I','B','A',' '), "iba"},
    {HZ_LANGUAGE_IBIBIO, HZ_TAG('I','B','B',' '), "ibb"},
    {HZ_LANGUAGE_IGBO, HZ_TAG('I','B','O',' '), "ibo"},
    {HZ_LANGUAGE_IDO, HZ_TAG('I','D','O',' '), "ido"},
    {HZ_LANGUAGE_IJO_LANGUAGES, HZ_TAG('I','J','O',' '), "iby, ijc, ije, ijn, ijo, ijs, nkx, okd, okr, orr"},
    {HZ_LANGUAGE_INTERLINGUE, HZ_TAG('I','L','E',' '), "ile"},
    {HZ_LANGUAGE_ILOKANO, HZ_TAG('I','L','O',' '), "ilo"},
    {HZ_LANGUAGE_INTERLINGUA, HZ_TAG('I','N','A',' '), "ina"},
    {HZ_LANGUAGE_INDONESIAN, HZ_TAG('I','N','D',' '), "ind"},
    {HZ_LANGUAGE_INGUSH, HZ_TAG('I','N','G',' '), "inh"},
    {HZ_LANGUAGE_INUKTITUT, HZ_TAG('I','N','U',' '), "iku"},
    {HZ_LANGUAGE_INUPIAT, HZ_TAG('I','P','K',' '), "ipk"},
    {HZ_LANGUAGE_PHONETIC_TRANSCRIPTION_IPA_CONVENTIONS, HZ_TAG('I','P','P','H'), NULL}, /* Phonetic transcription—IPA conventions  */
    {HZ_LANGUAGE_IRISH, HZ_TAG('I','R','I',' '), "gle"},
    {HZ_LANGUAGE_IRISH_TRADITIONAL, HZ_TAG('I','R','T',' '), "gle"},
    {HZ_LANGUAGE_ICELANDIC, HZ_TAG('I','S','L',' '), "isl"},
    {HZ_LANGAUGE_INARI_SAMI, HZ_TAG('I','S','M',' '), "smn"},
    {HZ_LANGUAGE_ITALIAN, HZ_TAG('I','T','A',' '), "ita"},
    {HZ_LANGUAGE_HEBREW, HZ_TAG('I','W','R',' '), "heb"},
    {HZ_LANGUAGE_JAMAICAN_CREOLE, HZ_TAG('J','A','M',' '), "jam"},
    {HZ_LANGUAGE_JAPANESE, HZ_TAG('J','A','N',' '), "jpn"},
    {HZ_LANGUAGE_JAVANESE, HZ_TAG('J','A','V',' '), "jav"},
    {HZ_LANGUAGE_LOJBAN, HZ_TAG('J','B','O',' '), "jbo"},
    {HZ_LANGUAGE_KRYMCHAK, HZ_TAG('J','C','T',' '), "jct"},
    {HZ_LANGUAGE_YIDDISH, HZ_TAG('J','I','I',' '), "yid"},
    {HZ_LANGUAGE_LADINO, HZ_TAG('J','U','D',' '), "lad"},
    {HZ_LANGUAGE_JULA, HZ_TAG('J','U','L',' '), "dyu"},
    {HZ_LANGUAGE_KABARDIAN, HZ_TAG('K','A','B',' '), "kbd"},
    {HZ_LANGUAGE_KABYLE, HZ_TAG('K','A','B','0'), "kab"},
    {HZ_LANGUAGE_KACHCHI, HZ_TAG('K','A','C',' '), "kfr"},
    {HZ_LANGUAGE_KALENJIN, HZ_TAG('K','A','L',' '), "kln"},
    {HZ_LANGUAGE_KANNADA, HZ_TAG('K','A','N',' '), "kan"},
    {HZ_LANGUAGE_KARACHAY, HZ_TAG('K','A','R',' '), "krc"},
    {HZ_LANGUAGE_GEORGIAN, HZ_TAG('K','A','T',' '), "kat"},
    {HZ_LANGUAGE_KAZAKH, HZ_TAG('K','A','Z',' '), "kaz"},
    {HZ_LANGUAGE_MAKONDE, HZ_TAG('K','D','E',' '), "kde"},
    {HZ_LANGUAGE_KABUVERDIANU, HZ_TAG('K','E','A',' '), "kea"}, /* Kabuverdianu (Crioulo) */
    {HZ_LANGUAGE_KEBENA, HZ_TAG('K','E','B',' '), "ktb"},
    {HZ_LANGUAGE_KEKCHI, HZ_TAG('K','E','K',' '), "kek"},
    {HZ_LANGUAGE_KHUTSURI_GEORGIAN, HZ_TAG('K','G','E',' '), "kat"},
    {HZ_LANGUAGE_KHAKASS, HZ_TAG('K','H','A',' '), "kjh"},
    {HZ_LANGUAGE_KHANTY_KAZIM, HZ_TAG('K','H','K',' '), "kca"}, /* Khanty-Kazim */
    {HZ_LANGUAGE_KHMER, HZ_TAG('K','H','M',' '), "khm"},
    {HZ_LANGUAGE_KHANTI_SHURISHKAR, HZ_TAG('K','H','S',' '), "kca"}, /* Khanty-Shurishkar */
    {HZ_LANGUAGE_KHAMTI_SHAN, HZ_TAG('K','H','T',' '), "kht"},
    {HZ_LANGUAGE_KHANTY_VAKHI, HZ_TAG('K','H','V',' '), "kca"}, /* Khanty-Shurishkar */
    {HZ_LANGUAGE_KHOWAR, HZ_TAG('K','H','W',' '), "khw"},
    {HZ_LANGUAGE_KIKUYU, HZ_TAG('K','I','K',' '), "kik"}, /* Kikuyu (Gikuyu) */
    {HZ_LANGUAGE_KIRGHIZ, HZ_TAG('K','I','R',' '), "kir"},
    {HZ_LANGUAGE_KISII, HZ_TAG('K','I','S',' '), "kqs, kss"},
    {HZ_LANGUAGE_KIRMANJKI, HZ_TAG('K','I','U',' '), "kiu"},
    {HZ_LANGUAGE_SOUTHERN_KIWAI, HZ_TAG('K','J','D',' '), "kjd"},
    {HZ_LANGUAGE_EASTERN_PWO_KAREN, HZ_TAG('K','J','P',' '), "kjp"},
    {HZ_LANGUAGE_BUMTHANKGKHA, HZ_TAG('K','J','Z',' '), "jkz"},
    {HZ_LANGUAGE_KOKNI, HZ_TAG('K','K','N',' '), "kex"},
    {HZ_LANGUAGE_KALMYK, HZ_TAG('K','L','M',' '), "xal"},
    {HZ_LANGUAGE_KAMBA, HZ_TAG('K','M','B',' '), "kam"},
    {HZ_LANGUAGE_KUMAONI, HZ_TAG('K','M','O',' '), "kfw"},
    {HZ_LANGUAGE_KOMO, HZ_TAG('K','M','O',' '), "kmw"},
    {HZ_LANGUAGE_KOMSO, HZ_TAG('K','M','S',' '), "kxc"},
    {HZ_LANGUAGE_KHORASANI_TURKIC, HZ_TAG('K','M','Z',' '), "kmz"},
    {HZ_LANGUAGE_KANURI, HZ_TAG('K','N','R',' '), "kau"},
    {HZ_LANGUAGE_KODAGU, HZ_TAG('K','O','D',' '), "kfa"},
    {HZ_LANGUAGE_KOREAN_OLD_HANGUL, HZ_TAG('K','O','H',' '), "kor, okm"},
    {HZ_LANGUAGE_KONKANI, HZ_TAG('K','O','K',' '), "kok"},
    {HZ_LANGUAGE_KOMI, HZ_TAG('K','O','M',' '), "kom"},
    {HZ_LANGUAGE_KIKONGO, HZ_TAG('K','O','N',' '), "ktu"},
    {HZ_LANGUAGE_KONGO, HZ_TAG('K','O','N','0'), "kon"},
    {HZ_LANGUAGE_KOMI_PERMYAK, HZ_TAG('K','O','P',' '), "koi"}, /* Komi-Permyak */
    {HZ_LANGUAGE_KOREAN, HZ_TAG('K','O','R',' '), "kor"},
    {HZ_LANGUAGE_KOSRAEAN, HZ_TAG('K','O','S',' '), "kos"},
    {HZ_LANGUAGE_KOMI_ZYRIAN, HZ_TAG('K','O','Z',' '), "kpv"}, /* Komi-Zyrian */
    {HZ_LANGUAGE_KPELLE, HZ_TAG('K','P','L',' '), "kpe"},
    {HZ_LANGUAGE_KRIO, HZ_TAG('K','R','I',' '), "kri"},
    {HZ_LANGUAGE_KARAKALPAK, HZ_TAG('K','R','K',' '), "kaa"},
    {HZ_LANGUAGE_KARELIAN, HZ_TAG('K','R','L',' '), "krl"},
    {HZ_LANGUAGE_KARAIM, HZ_TAG('K','R','M',' '), "kdr"},
    {HZ_LANGUAGE_KAREN, HZ_TAG('K','R','N',' '), "blk, bwe, eky, ghk, jkm, jkp, kar, kjp, kjt, ksw, kvl, kvq, kvt, kvu, kvy, kxf, kxk, kyu, pdu, pwo, pww, wea"},
    {HZ_LANGUAGE_KOORETE, HZ_TAG('K','R','T',' '), "kqy"},
    {HZ_LANGUAGE_KASHMIRI, HZ_TAG('K','S','H',' '), "kas"},
    {HZ_LANGUAGE_RIPUARIAN, HZ_TAG('K','S','H','0'), "ksh"},
    {HZ_LANGUAGE_KHASI, HZ_TAG('K','S','I',' '), "kha"},
    {HZ_LANGUAGE_KILDIN_SAMI, HZ_TAG('K','S','M',' '), "sjd"},
    {HZ_LANGUAGE_SGAW_KAREN, HZ_TAG('K','S','W',' '), "ksw"}, /* S’gaw Karen */
    {HZ_LANGUAGE_KUANYAMA, HZ_TAG('K','U','A',' '), "kua"},
    {HZ_LANGUAGE_KUI, HZ_TAG('K','U','I',' '), "kxu"},
    {HZ_LANGUAGE_KULVI, HZ_TAG('K','U','L',' '), "kfx"},
    {HZ_LANGUAGE_KUMYK, HZ_TAG('K','U','M',' '), "kum"},
    {HZ_LANGUAGE_KURDISH, HZ_TAG('K','U','R',' '), "kur"},
    {HZ_LANGUAGE_KURUKH, HZ_TAG('K','U','U',' '), "kru"},
    {HZ_LANGUAGE_KUY, HZ_TAG('K','U','Y',' '), "kdt"},
    {HZ_LANGUAGE_KORYAK, HZ_TAG('K','Y','K',' '), "kpy"},
    {HZ_LANGUAGE_WESTERN_KAYAH, HZ_TAG('K','Y','U',' '), "kyu"},
    {HZ_LANGUAGE_LADIN, HZ_TAG('L','A','D',' '), "lld"},
    {HZ_LANGUAGE_LAHULI, HZ_TAG('L','A','H',' '), "bfu"},
    {HZ_LANGUAGE_LAK, HZ_TAG('L','A','K',' '), "lbe"},
    {HZ_LANGUAGE_LAMBANI, HZ_TAG('L','A','M',' '), "lmn"},
    {HZ_LANGUAGE_LAO, HZ_TAG('L','A','O',' '), "lao"},
    {HZ_LANGUAGE_LATIN, HZ_TAG('L','A','T',' '), "lat"},
    {HZ_LANGUAGE_LAZ, HZ_TAG('L','A','Z',' '), "llz"},
    {HZ_LANGUAGE_L_CREE, HZ_TAG('L','C','R',' '), "crm"}, /* L-Cree */
    {HZ_LANGUAGE_LADAKHI, HZ_TAG('L','D','K',' '), "lbj"},
    {HZ_LANGUAGE_LEZGI, HZ_TAG('L','E','Z',' '), "lez"},
    {HZ_LANGUAGE_LIGURIAN, HZ_TAG('L','I','J',' '), "lij"},
    {HZ_LANGUAGE_LIMBURGISH, HZ_TAG('L','I','M',' '), "lim"},
    {HZ_LANGUAGE_LINGALA, HZ_TAG('L','I','N',' '), "lin"},
};

hz_language_t
hz_lang(const char *tag) {
#if HZ_LANG_USE_ISO_639_ONLY

#else
    /* parse bcp-47 tag */
#endif
}


void
hz_shape(hz_font_t *font, hz_sequence_t *sequence, hz_feature_t *features, unsigned int num_features)
{
    hz_face_t *face;
    hz_tag_t script_tag, language_tag;
    const hz_face_ot_tables_t *tables;
    int loaded_standards = 0;

    if (!num_features || features == NULL) {
        /* no features explicitly specified, load standard features for script */
        hz_ot_script_load_features(sequence->script, &features, &num_features);
        loaded_standards = 1;
    }

    face = hz_font_get_face(font);
    tables = hz_face_get_ot_tables(face);
    script_tag = hz_ot_script_to_tag(sequence->script);
    language_tag = hz_ot_language_to_tag(sequence->language);
    
    /* map unicode characters to nominal glyph indices */
    hz_map_to_nominal_forms(face, sequence);

    /* sets glyph class information */
    hz_set_sequence_glyph_info(face, sequence);

    if (sequence->flags & HZ_NO_MARKS) {
        hz_apply_remove_marks(sequence);
    }

    /* substitute glyphs */
    if (tables->GSUB_table != NULL) {
        hz_ot_layout_apply_features(face,
                                    HZ_OT_TAG_GSUB,
                                    script_tag,
                                    language_tag,
                                    features,
                                    num_features,
                                    sequence);
    }
    
    /* position glyphs */
    hz_apply_tt1_metrics(face, sequence);
    if (tables->GPOS_table != NULL) {
        hz_ot_layout_apply_features(face,
                                    HZ_OT_TAG_GPOS,
                                    script_tag,
                                    language_tag,
                                    features,
                                    num_features,
                                    sequence);
    }
    
    if (sequence->direction == HZ_DIRECTION_RTL)
        hz_apply_rtl_switch(sequence);

    if (loaded_standards) free(features);
}
