#include "hz.h"
#include "hz-array.h"
#include "hz-map.h"


hz_bool
hz_context_set_features(hz_context_t *ctx, hz_bitset_t *features)
{
    ctx->features = features;
    if (features->bit_count == HZ_FEATURE_COUNT) {
        hz_bitset_copy(ctx->features, features);
        return HZ_TRUE;
    }

    return HZ_FALSE;
}

hz_context_t *
hz_context_create(hz_face_t *face)
{
    hz_context_t *ctx = HZ_ALLOC(hz_context_t);

    ctx->face = face;
    ctx->features = hz_bitset_create(HZ_FEATURE_COUNT);

    return ctx;
}

void
hz_context_set_script(hz_context_t *ctx, hz_script_t script)
{
    ctx->script = script;
}

void
hz_context_set_language(hz_context_t *ctx, hz_language_t language)
{
    ctx->language = language;
}

void
hz_context_set_dir(hz_context_t *ctx, hz_dir_t dir)
{
    ctx->dir = dir;
}

void
hz_context_destroy(hz_context_t *ctx)
{
    free(ctx);
}

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


//typedef enum hz_cmap_subtable_type_t {
//    HZ_CMAP_SUBTABLE_BYTE_ENCODING_TABLE = 0,
//    HZ_CMAP_SUBTABLE_SEGMENT_MAPPING_TO_DELTA_VALUES = 4
//} hz_cmap_subtable_type_t;

//typedef enum hz_cmap_unicode_encoding_t {
//
//} hz_cmap_unicode_encoding_t;

typedef struct hz_cmap_encoding_t {
    uint16_t        platform_id; /* Platform ID. */
    uint16_t        encoding_id; /* Platform-specific encoding ID. */
    hz_offset32   subtable_offset; /* Byte offset from beginning of table to the subtable for this encoding. */
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


typedef struct hz_vector_t hz_vector_t;

static hz_id
hz_cmap_unicode_to_id(hz_cmap_subtable_format4_t *st, hz_unicode c) {
    uint16_t range_count = st->seg_count_x2 >> 1;
    uint16_t i = 0;
    hz_id id;

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
//    HZ_ASSERT(start_code == 0xFFFF && end_code == 0xFFFF);
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
hz_context_collect_required_glyphs(hz_context_t *ctx,
                                     hz_set_t *glyphs)
{
//    hz_tag script_tag = ctx->script;
//    hz_tag language_tag = ctx->language;
//    hz_set_t *lookup_indices = hz_set_create();
//
//    if (!hz_set_is_empty(glyphs))
//        hz_set_clear(glyphs);
//
//    hz_ot_layout_collect_lookups(ctx->face, HZ_OT_TAG_GSUB,
//                                   script_tag,
//                                   language_tag,
//                                   ctx->features,
//                                   lookup_indices);

}

hz_id
hz_face_cmap_unicode_to_id(hz_face_t *face, hz_unicode c)
{
    /* Parse cmap table and convert unicode run to GID run */
    hz_buf_t *cmap_buf = &face->cmap_buf;
    hz_stream_t *cmap_table = hz_stream_create(cmap_buf->data, cmap_buf->len, HZ_STREAM_BOUND_FLAG);

    uint16_t version;
    hz_stream_read16(cmap_table, &version);

    /* Table version number must be 0 */
    HZ_ASSERT(version == 0);

    uint16_t num_encodings, enc_idx;
    hz_stream_read16(cmap_table, &num_encodings);

//    HZ_LOG("cmap table encoding count: %d\n", num_encodings);

    for (enc_idx = 0; enc_idx < num_encodings; ++enc_idx) {
        hz_cmap_encoding_t enc = {};
        hz_stream_read16(cmap_table, &enc.platform_id);
        hz_stream_read16(cmap_table, &enc.encoding_id);
        hz_stream_read32(cmap_table, &enc.subtable_offset);

//        HZ_LOG("platform: %s\n", hz_cmap_platform_to_string(enc.platform_id));
//        HZ_LOG("encoding: %d\n", enc.encoding_id);
//        HZ_LOG("subtable-offset: %d\n", enc.subtable_offset);

        // TODO: Handle length properly with bounding
        if (enc.platform_id == HZ_CMAP_PLATFORM_UNICODE) {
            uint16_t subtable_format = 0;
            hz_stream_t *subtable_stream = hz_stream_create(cmap_buf->data + enc.subtable_offset,
                                                            0, 0);
            hz_stream_read16(subtable_stream, &subtable_format);

//            HZ_LOG("subtable_format: %d\n", subtable_format);

            if (subtable_format == 4) {
                /* Format 4: Segment mapping to delta values */
                hz_cmap_subtable_format4_t subtable;
                hz_stream_read16(subtable_stream, &subtable.length);
                hz_stream_read16(subtable_stream, &subtable.language);
                hz_stream_read16(subtable_stream, &subtable.seg_count_x2);
                hz_stream_read16(subtable_stream, &subtable.search_range);
                hz_stream_read16(subtable_stream, &subtable.entry_selector);
                hz_stream_read16(subtable_stream, &subtable.range_shift);

//                HZ_LOG("length: %d\n", subtable.length);
//                HZ_LOG("language: %d\n", subtable.language);
//                HZ_LOG("seg_count_x2: %d\n", subtable.seg_count_x2);
//                HZ_LOG("search_range: %d\n", subtable.search_range);
//                HZ_LOG("entry_selector: %d\n", subtable.entry_selector);
//                HZ_LOG("range_shift: %d\n", subtable.range_shift);

                uint16_t seg_jmp = (subtable.seg_count_x2>>1) * sizeof(uint16_t);

                uint8_t *curr_addr = subtable_stream->data + subtable_stream->offset;
                subtable.end_code = (uint16_t *)curr_addr;
                subtable.start_code = (uint16_t *)(curr_addr + seg_jmp + sizeof(uint16_t));
                subtable.id_delta = (int16_t *)(curr_addr + 2*seg_jmp + sizeof(uint16_t));
                subtable.id_range_offsets = (uint16_t *)(curr_addr + 3*seg_jmp + sizeof(uint16_t));

                /* map unicode characters to glyph indices in run */
                return hz_cmap_unicode_to_id(&subtable, c);
            }

            /* Found encoding */
            break;
        }
    }

    return 0;
}

void
hz_map_to_nominal_form_glyphs(hz_context_t *ctx,
                              hz_section_t *sect)
{
    /* Parse cmap table and convert unicode run to GID run */
    hz_buf_t *cmap_buf = &ctx->face->cmap_buf;
    hz_stream_t *cmap_table = hz_stream_create(cmap_buf->data, cmap_buf->len, HZ_STREAM_BOUND_FLAG);

    uint16_t version;
    hz_stream_read16(cmap_table, &version);

    /* Table version number must be 0 */
    HZ_ASSERT(version == 0);

    uint16_t num_encodings, enc_idx;
    hz_stream_read16(cmap_table, &num_encodings);

    HZ_LOG("cmap table encoding count: %d\n", num_encodings);

    for (enc_idx = 0; enc_idx < num_encodings; ++enc_idx) {
        hz_cmap_encoding_t enc = {};
        hz_stream_read16(cmap_table, &enc.platform_id);
        hz_stream_read16(cmap_table, &enc.encoding_id);
        hz_stream_read32(cmap_table, &enc.subtable_offset);

        HZ_LOG("platform: %s\n", hz_cmap_platform_to_string(enc.platform_id));
        HZ_LOG("encoding: %d\n", enc.encoding_id);
        HZ_LOG("subtable-offset: %d\n", enc.subtable_offset);


        // TODO: Handle length properly with bounding
        if (enc.platform_id == HZ_CMAP_PLATFORM_UNICODE) {
            uint16_t subtable_format = 0;
            hz_stream_t *subtable_stream = hz_stream_create(cmap_buf->data + enc.subtable_offset,
                                                                0, 0);
            hz_stream_read16(subtable_stream, &subtable_format);

            HZ_LOG("subtable_format: %d\n", subtable_format);

            if (subtable_format == 4) {
                /* Format 4: Segment mapping to delta values */
                hz_cmap_subtable_format4_t subtable;
                hz_stream_read16(subtable_stream, &subtable.length);
                hz_stream_read16(subtable_stream, &subtable.language);
                hz_stream_read16(subtable_stream, &subtable.seg_count_x2);
                hz_stream_read16(subtable_stream, &subtable.search_range);
                hz_stream_read16(subtable_stream, &subtable.entry_selector);
                hz_stream_read16(subtable_stream, &subtable.range_shift);

                HZ_LOG("length: %d\n", subtable.length);
                HZ_LOG("language: %d\n", subtable.language);
                HZ_LOG("seg_count_x2: %d\n", subtable.seg_count_x2);
                HZ_LOG("search_range: %d\n", subtable.search_range);
                HZ_LOG("entry_selector: %d\n", subtable.entry_selector);
                HZ_LOG("range_shift: %d\n", subtable.range_shift);

                uint16_t seg_jmp = (subtable.seg_count_x2>>1) * sizeof(uint16_t);

                uint8_t *curr_addr = subtable_stream->data + subtable_stream->offset;
                subtable.end_code = (uint16_t *)curr_addr;
                subtable.start_code = (uint16_t *)(curr_addr + seg_jmp + sizeof(uint16_t));
                subtable.id_delta = (int16_t *)(curr_addr + 2*seg_jmp + sizeof(uint16_t));
                subtable.id_range_offsets = (uint16_t *)(curr_addr + 3*seg_jmp + sizeof(uint16_t));

                /* map unicode characters to glyph indices in run */
                hz_section_node_t *curr_node = sect->root;

                while (curr_node != NULL) {
                    curr_node->data.id = hz_cmap_unicode_to_id(&subtable, curr_node->data.codepoint);
                    curr_node = curr_node->next;
                }
            }

            /* Found encoding */
            break;
        }
    }
}


void
hz_ot_parse_gdef_table(hz_context_t *ctx, hz_section_t *sect)
{
    hz_stream_t *table;
    uint32_t ver;

    hz_offset16 glyph_class_def_offset;
    hz_offset16 attach_list_offset;
    hz_offset16 lig_caret_list_offset;
    hz_offset16 mark_attach_class_def_offset;

    table = hz_stream_create(ctx->face->gdef_table, 0, 0);
    hz_stream_read32(table, &ver);

    switch (ver) {
        case 0x00010000: /* 1.0 */
            hz_stream_read16(table, &glyph_class_def_offset);
            hz_stream_read16(table, &attach_list_offset);
            hz_stream_read16(table, &lig_caret_list_offset);
            hz_stream_read16(table, &mark_attach_class_def_offset);
            break;
        case 0x00010002: /* 1.2 */
            break;
        case 0x00010003: /* 1.3 */
            break;
        default:
            break;
    }

    if (glyph_class_def_offset != 0) {
        /* glyph class def isn't nil */
        hz_stream_t *subtable = hz_stream_create(table->data + glyph_class_def_offset, 0, 0);
        hz_map_t *class_map = hz_map_create();
        hz_section_node_t *curr_node = sect->root;
        uint16_t class_format;
        hz_stream_read16(subtable, &class_format);
        switch (class_format) {
            case 1:
                break;
            case 2: {
                uint16_t range_index = 0, class_range_count;
                hz_stream_read16(subtable, &class_range_count);

                while (range_index < class_range_count) {
                    uint16_t start_glyph_id, end_glyph_id, glyph_class;
                    hz_stream_read16(subtable, &start_glyph_id);
                    hz_stream_read16(subtable, &end_glyph_id);
                    hz_stream_read16(subtable, &glyph_class);
                    HZ_ASSERT(glyph_class >= 1 && glyph_class <= 4);
                    hz_map_set_value_for_keys(class_map, start_glyph_id, end_glyph_id, HZ_BIT(glyph_class - 1));

                    ++range_index;
                }
                break;
            }
            default:
                break;
        }

        /* set glyph class values if in map */
        while (curr_node != NULL) {
            hz_id gid = curr_node->data.id;
            if (hz_map_value_exists(class_map, gid)) {
                curr_node->data.g_class = hz_map_get_value(class_map, gid);
            } else {
                /* set default glyph class if current glyph id isn't found */
                curr_node->data.g_class = HZ_GLYPH_CLASS_ZERO;
            }

            curr_node = curr_node->next;
        }

        hz_map_destroy(class_map);
    }


}

hz_status_t
hz_shape_full(hz_context_t *ctx, hz_section_t *sect)
{
    hz_face_t *face = ctx->face;
    hz_lookup_table_t *lookups = NULL;
    hz_tag script_tag = hz_ot_script_to_tag(ctx->script);
    hz_tag language_tag = hz_ot_language_to_tag(ctx->language);

    HZ_LOG("language: \"%c%c%c%c\"\n", HZ_UNTAG(language_tag), language_tag);
    HZ_LOG("script: \"%c%c%c%c\"\n", HZ_UNTAG(script_tag), script_tag);

    /* Map unicode characters to nominal glyph indices */
    hz_map_to_nominal_form_glyphs(ctx, sect);
    hz_ot_parse_gdef_table(ctx, sect);

    hz_ot_layout_apply_gsub_features(face, script_tag,
                                     language_tag,
                                     ctx->features,
                                     sect);

    hz_ot_layout_apply_gpos_features(face, script_tag,
                                     language_tag,
                                     ctx->features,
                                     sect);

    return HZ_SUCCESS;
}