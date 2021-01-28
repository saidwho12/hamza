#include "hm.h"
#include "hm-array.h"


hm_bool
hm_ctx_set_features(hm_context_t *ctx, hm_bitset_t *features)
{
    ctx->features = features;
    if (features->bit_count == HM_FEATURE_COUNT) {
        hm_bitset_copy(ctx->features, features);
        return HM_TRUE;
    }

    return HM_FALSE;
}

hm_context_t *
hm_ctx_create(hm_face_t *face)
{
    hm_context_t *ctx = HM_ALLOC(hm_context_t);

    ctx->face = face;
    ctx->features = hm_bitset_create(HM_FEATURE_COUNT);

    return ctx;
}

void
hm_ctx_set_script(hm_context_t *ctx, hm_script_t script)
{
    ctx->script = script;
}

void
hm_ctx_set_language(hm_context_t *ctx, hm_language_t language)
{
    ctx->language = language;
}

void
hm_ctx_set_dir(hm_context_t *ctx, hm_dir_t dir)
{
    ctx->dir = dir;
}

/* https://docs.microsoft.com/en-us/typography/opentype/spec/cmap#platform-ids */
typedef enum hm_cmap_platform_t  {
    HM_CMAP_PLATFORM_UNICODE = 0, /* Various */
    HM_CMAP_PLATFORM_MACINTOSH = 1, /* Script manager code */
    HM_CMAP_PLATFORM_ISO = 2, /* ISO encoding [deprecated] */
    HM_CMAP_PLATFORM_WINDOWS = 3, /* Windows encoding */
    HM_CMAP_PLATFORM_CUSTOM = 4, /* Custom */
    /* Platform ID values 240 through 255 are reserved for user-defined platforms.
     * This specification will never assign these values to a registered platform.
     * Platform ID 2 (ISO) was deprecated as of OpenType version v1.3.
     * */
} hm_cmap_platform_t;


//typedef enum hm_cmap_subtable_type_t {
//    HM_CMAP_SUBTABLE_BYTE_ENCODING_TABLE = 0,
//    HM_CMAP_SUBTABLE_SEGMENT_MAPPING_TO_DELTA_VALUES = 4
//} hm_cmap_subtable_type_t;

//typedef enum hm_cmap_unicode_encoding_t {
//
//} hm_cmap_unicode_encoding_t;

typedef struct hm_cmap_encoding_t {
    uint16_t        platform_id; /* Platform ID. */
    uint16_t        encoding_id; /* Platform-specific encoding ID. */
    hm_offset32   subtable_offset; /* Byte offset from beginning of table to the subtable for this encoding. */
} hm_cmap_encoding_t;

static const char *
hm_cmap_platform_to_string(hm_cmap_platform_t platform) {
    switch (platform) {
        case HM_CMAP_PLATFORM_UNICODE: return "Unicode";
        case HM_CMAP_PLATFORM_MACINTOSH: return "Macintosh";
        case HM_CMAP_PLATFORM_ISO: return "ISO";
        case HM_CMAP_PLATFORM_WINDOWS: return "Windows";
        case HM_CMAP_PLATFORM_CUSTOM: return "Custom";
        default: return NULL;
    }
}

/*
 * Format 0: Byte encoding table
 * https://docs.microsoft.com/en-us/typography/opentype/spec/cmap#format-0-byte-encoding-table
 * */
typedef struct hm_cmap_subtable_format0_t {
    uint16_t format; /* Format number is set to 0. */
    uint16_t length; /* This is the length in bytes of the subtable. */
    /* For requirements on use of the language field,
     * see “Use of the language field in 'cmap' subtables” in this document.
     * */
    uint16_t language;
    /* An array that maps character codes to glyph index values. */
    uint8_t glyph_id_array[256];
} hm_cmap_subtable_format0_t;


typedef struct hm_cmap_subtable_format4_t {
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
} hm_cmap_subtable_format4_t;


typedef struct hm_vector_t hm_vector_t;

static hm_id
hm_cmap_unicode_to_id(hm_cmap_subtable_format4_t *st, hm_unicode c) {
    uint16_t range_count = st->seg_count_x2 >> 1;
    uint16_t i = 0;
    hm_id id;

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

    /* couldn't find a range, return missingGlyph */
    uint16_t start_code = bswap16(st->start_code[i]);
    uint16_t end_code = bswap16(st->end_code[i]);
    int16_t id_delta = bswap16(st->id_delta[i]);
    uint16_t id_range_offset = bswap16(st->id_range_offsets[i]);
//    HM_ASSERT(start_code == 0xFFFF && end_code == 0xFFFF);
//
//    if (id_range_offset != 0) {
//        uint16_t raw_val = *(&st->id_range_offsets[i] + id_range_offset/2 + (c - start_code));
//        id = bswap16( raw_val );
//        if (id != 0) id += id_delta;
//    } else
//        id = id_delta + c;

    return 0;
}

void
hm_context_collect_required_glyphs(hm_context_t *ctx,
                                     hm_set_t *glyphs)
{
//    hm_tag script_tag = ctx->script;
//    hm_tag language_tag = ctx->language;
//    hm_set_t *lookup_indices = hm_set_create();
//
//    if (!hm_set_is_empty(glyphs))
//        hm_set_clear(glyphs);
//
//    hm_ot_layout_collect_lookups(ctx->face, HM_OT_TAG_GSUB,
//                                   script_tag,
//                                   language_tag,
//                                   ctx->features,
//                                   lookup_indices);

}

hm_id
hm_face_cmap_unicode_to_id(hm_face_t *face, hm_unicode c)
{
    /* Parse cmap table and convert unicode run to GID run */
    hm_buf_t *cmap_buf = &face->cmap_buf;
    hm_stream_t *cmap_table = hm_stream_create(cmap_buf->data, cmap_buf->len, HM_STREAM_BOUND_FLAG);

    uint16_t version;
    hm_stream_read16(cmap_table, &version);

    /* Table version number must be 0 */
    HM_ASSERT(version == 0);

    uint16_t num_encodings, enc_idx;
    hm_stream_read16(cmap_table, &num_encodings);

//    HM_LOG("cmap table encoding count: %d\n", num_encodings);

    for (enc_idx = 0; enc_idx < num_encodings; ++enc_idx) {
        hm_cmap_encoding_t enc = {};
        hm_stream_read16(cmap_table, &enc.platform_id);
        hm_stream_read16(cmap_table, &enc.encoding_id);
        hm_stream_read32(cmap_table, &enc.subtable_offset);

//        HM_LOG("platform: %s\n", hm_cmap_platform_to_string(enc.platform_id));
//        HM_LOG("encoding: %d\n", enc.encoding_id);
//        HM_LOG("subtable-offset: %d\n", enc.subtable_offset);

        // TODO: Handle length properly with bounding
        if (enc.platform_id == HM_CMAP_PLATFORM_UNICODE) {
            uint16_t subtable_format = 0;
            hm_stream_t *subtable_stream = hm_stream_create(cmap_buf->data + enc.subtable_offset,
                                                            0, 0);
            hm_stream_read16(subtable_stream, &subtable_format);

//            HM_LOG("subtable_format: %d\n", subtable_format);

            if (subtable_format == 4) {
                /* Format 4: Segment mapping to delta values */
                hm_cmap_subtable_format4_t subtable;
                hm_stream_read16(subtable_stream, &subtable.length);
                hm_stream_read16(subtable_stream, &subtable.language);
                hm_stream_read16(subtable_stream, &subtable.seg_count_x2);
                hm_stream_read16(subtable_stream, &subtable.search_range);
                hm_stream_read16(subtable_stream, &subtable.entry_selector);
                hm_stream_read16(subtable_stream, &subtable.range_shift);

//                HM_LOG("length: %d\n", subtable.length);
//                HM_LOG("language: %d\n", subtable.language);
//                HM_LOG("seg_count_x2: %d\n", subtable.seg_count_x2);
//                HM_LOG("search_range: %d\n", subtable.search_range);
//                HM_LOG("entry_selector: %d\n", subtable.entry_selector);
//                HM_LOG("range_shift: %d\n", subtable.range_shift);

                uint16_t seg_jmp = (subtable.seg_count_x2>>1) * sizeof(uint16_t);

                uint8_t *curr_addr = subtable_stream->data + subtable_stream->offset;
                subtable.end_code = (uint16_t *)curr_addr;
                subtable.start_code = (uint16_t *)(curr_addr + seg_jmp + sizeof(uint16_t));
                subtable.id_delta = (int16_t *)(curr_addr + 2*seg_jmp + sizeof(uint16_t));
                subtable.id_range_offsets = (uint16_t *)(curr_addr + 3*seg_jmp + sizeof(uint16_t));

                /* map unicode characters to glyph indices in run */
                return hm_cmap_unicode_to_id(&subtable, c);
            }

            /* Found encoding */
            break;
        }
    }

    return 0;
}

void
hm_map_to_nominal_form_glyphs(hm_context_t *ctx,
                              hm_array_t *input,
                              hm_array_t *output)
{
    /* Parse cmap table and convert unicode run to GID run */
    hm_buf_t *cmap_buf = &ctx->face->cmap_buf;
    hm_stream_t *cmap_table = hm_stream_create(cmap_buf->data, cmap_buf->len, HM_STREAM_BOUND_FLAG);

    uint16_t version;
    hm_stream_read16(cmap_table, &version);

    /* Table version number must be 0 */
    HM_ASSERT(version == 0);

    uint16_t num_encodings, enc_idx;
    hm_stream_read16(cmap_table, &num_encodings);

    HM_LOG("cmap table encoding count: %d\n", num_encodings);

    for (enc_idx = 0; enc_idx < num_encodings; ++enc_idx) {
        hm_cmap_encoding_t enc = {};
        hm_stream_read16(cmap_table, &enc.platform_id);
        hm_stream_read16(cmap_table, &enc.encoding_id);
        hm_stream_read32(cmap_table, &enc.subtable_offset);

        HM_LOG("platform: %s\n", hm_cmap_platform_to_string(enc.platform_id));
        HM_LOG("encoding: %d\n", enc.encoding_id);
        HM_LOG("subtable-offset: %d\n", enc.subtable_offset);


        // TODO: Handle length properly with bounding
        if (enc.platform_id == HM_CMAP_PLATFORM_UNICODE) {
            uint16_t subtable_format = 0;
            hm_stream_t *subtable_stream = hm_stream_create(cmap_buf->data + enc.subtable_offset,
                                                                0, 0);
            hm_stream_read16(subtable_stream, &subtable_format);

            HM_LOG("subtable_format: %d\n", subtable_format);

            if (subtable_format == 4) {
                /* Format 4: Segment mapping to delta values */
                hm_cmap_subtable_format4_t subtable;
                hm_stream_read16(subtable_stream, &subtable.length);
                hm_stream_read16(subtable_stream, &subtable.language);
                hm_stream_read16(subtable_stream, &subtable.seg_count_x2);
                hm_stream_read16(subtable_stream, &subtable.search_range);
                hm_stream_read16(subtable_stream, &subtable.entry_selector);
                hm_stream_read16(subtable_stream, &subtable.range_shift);

                HM_LOG("length: %d\n", subtable.length);
                HM_LOG("language: %d\n", subtable.language);
                HM_LOG("seg_count_x2: %d\n", subtable.seg_count_x2);
                HM_LOG("search_range: %d\n", subtable.search_range);
                HM_LOG("entry_selector: %d\n", subtable.entry_selector);
                HM_LOG("range_shift: %d\n", subtable.range_shift);

                uint16_t seg_jmp = (subtable.seg_count_x2>>1) * sizeof(uint16_t);

                uint8_t *curr_addr = subtable_stream->data + subtable_stream->offset;
                subtable.end_code = (uint16_t *)curr_addr;
                subtable.start_code = (uint16_t *)(curr_addr + seg_jmp + sizeof(uint16_t));
                subtable.id_delta = (int16_t *)(curr_addr + 2*seg_jmp + sizeof(uint16_t));
                subtable.id_range_offsets = (uint16_t *)(curr_addr + 3*seg_jmp + sizeof(uint16_t));

                /* map unicode characters to glyph indices in run */
                int ch_index = 0;
                while (ch_index < hm_array_size(input)) {
                    hm_unicode c = hm_array_at(input, ch_index);
                    hm_array_push_back(output, hm_cmap_unicode_to_id(&subtable, c));
                    ++ch_index;
                }
            }

            /* Found encoding */
            break;
        }
    }
}


hm_status_t
hm_shape(hm_context_t *ctx, hm_run_t *run) {
    hm_face_t *face = ctx->face;
    hm_lookup_table_t *lookups = NULL;
    hm_tag script_tag = hm_ot_script_to_tag(ctx->script);
    hm_tag language_tag = hm_ot_language_to_tag(ctx->language);

    HM_LOG("language: \"%c%c%c%c\"\n", HM_UNTAG(language_tag), language_tag);
    HM_LOG("script: \"%c%c%c%c\"\n", HM_UNTAG(script_tag), script_tag);

    /* Map unicode characters to nominal glyph indices */
    hm_map_to_nominal_form_glyphs(ctx, run->input, run->output);

    hm_ot_layout_apply_features(face, script_tag,
                                language_tag,
                                ctx->features,
                                run->output);


//    if (hm_set_is_empty(lookup_indices))
//        HM_LOG("No lookups collected!\n");
//
//
//    int i;
//    for (i = 0; i < lookup_indices->count; ++i) {
//        HM_LOG("%u\n", lookup_indices->values[i]);
//
//        /* Apply lookup */
//        hm_ot_layout_apply_lookup(face, HM_OT_TAG_GSUB,
//                                    feature_set->values[i],
//                                    lookup_indices->values[i],
//                                    run->output);
//    }



    return HM_SUCCESS;
}