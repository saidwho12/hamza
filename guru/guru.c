#include "guru.h"


guru_bool
guru_ctx_set_features(guru_ctx_t *ctx, guru_bitset_t *features)
{
    if (features->bit_count == GURU_FEATURE_COUNT) {
        guru_bitset_copy(ctx->features, features);
        return GURU_TRUE;
    }

    return GURU_FALSE;
}

guru_ctx_t *
guru_ctx_create(guru_face_t *face)
{
    guru_ctx_t *context = GURU_ALLOC(guru_ctx_t);

    context->face = face;
    context->features = guru_bitset_create(GURU_FEATURE_COUNT);

    return context;
}

void
guru_ctx_set_script(guru_ctx_t *ctx, guru_script_t script)
{
    ctx->script = script;
}

void
guru_ctx_set_language(guru_ctx_t *ctx, guru_language_t language)
{
    ctx->language = language;
}

void
guru_ctx_set_dir(guru_ctx_t *ctx, guru_dir_t dir)
{
    ctx->dir = dir;
}

/* https://docs.microsoft.com/en-us/typography/opentype/spec/cmap#platform-ids */
typedef enum guru_cmap_platform_t  {
    GURU_CMAP_PLATFORM_UNICODE = 0, /* Various */
    GURU_CMAP_PLATFORM_MACINTOSH = 1, /* Script manager code */
    GURU_CMAP_PLATFORM_ISO = 2, /* ISO encoding [deprecated] */
    GURU_CMAP_PLATFORM_WINDOWS = 3, /* Windows encoding */
    GURU_CMAP_PLATFORM_CUSTOM = 4, /* Custom */
    /* Platform ID values 240 through 255 are reserved for user-defined platforms.
     * This specification will never assign these values to a registered platform.
     * Platform ID 2 (ISO) was deprecated as of OpenType version v1.3.
     * */
} guru_cmap_platform_t;


//typedef enum guru_cmap_subtable_type_t {
//    GURU_CMAP_SUBTABLE_BYTE_ENCODING_TABLE = 0,
//    GURU_CMAP_SUBTABLE_SEGMENT_MAPPING_TO_DELTA_VALUES = 4
//} guru_cmap_subtable_type_t;

//typedef enum guru_cmap_unicode_encoding_t {
//
//} guru_cmap_unicode_encoding_t;

typedef struct guru_cmap_encoding_t {
    uint16_t        platform_id; /* Platform ID. */
    uint16_t        encoding_id; /* Platform-specific encoding ID. */
    guru_offset32   subtable_offset; /* Byte offset from beginning of table to the subtable for this encoding. */
} guru_cmap_encoding_t;

static const char *
guru_cmap_platform_to_string(guru_cmap_platform_t platform) {
    switch (platform) {
        case GURU_CMAP_PLATFORM_UNICODE: return "Unicode";
        case GURU_CMAP_PLATFORM_MACINTOSH: return "Macintosh";
        case GURU_CMAP_PLATFORM_ISO: return "ISO";
        case GURU_CMAP_PLATFORM_WINDOWS: return "Windows";
        case GURU_CMAP_PLATFORM_CUSTOM: return "Custom";
        default: return NULL;
    }
}

/*
 * Format 0: Byte encoding table
 * https://docs.microsoft.com/en-us/typography/opentype/spec/cmap#format-0-byte-encoding-table
 * */
typedef struct guru_cmap_subtable_format0_t {
    uint16_t format; /* Format number is set to 0. */
    uint16_t length; /* This is the length in bytes of the subtable. */
    /* For requirements on use of the language field,
     * see “Use of the language field in 'cmap' subtables” in this document.
     * */
    uint16_t language;
    /* An array that maps character codes to glyph index values. */
    uint8_t glyph_id_array[256];
} guru_cmap_subtable_format0_t;


typedef struct guru_cmap_subtable_format4_t {
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
} guru_cmap_subtable_format4_t;


static guru_id
guru_cmap_unicode_to_id(guru_cmap_subtable_format4_t *st, guru_unicode c) {
    uint16_t range_count = st->seg_count_x2 >> 1;
    uint16_t i = 0;

    /* search for code range */
    while (i < range_count) {
        uint16_t start_code = bswap16(st->start_code[i]);
        uint16_t end_code = bswap16(st->end_code[i]);
        uint16_t id_delta = bswap16(st->id_delta[i]);
        uint16_t id_range_offset = bswap16(st->id_range_offsets[i]);

        if (end_code >= c && start_code <= c) {
            guru_id id;

            if (id_range_offset != 0) {
                id = *(&st->id_range_offsets[i] + (id_range_offset >> 1) + (c - start_code));
                if (id != 0) id = (id + id_delta) % 65536;
            } else
                id = (id_delta + c) % 65536;

            return id;
        }

        ++i;
    }

    /* couldn't find a range, TODO: get missingGlyph */
    uint16_t start_code = st->start_code[i];
    uint16_t end_code = st->end_code[i];
    GURU_ASSERT(start_code == 0xFFFF && end_code == 0xFFFF);
    return 0xFFFF;
}

guru_status_t
guru_shape(guru_ctx_t *ctx, guru_run_t *run) {
    guru_face_t *face = ctx->face;
    guru_lookup_table_t *lookups = NULL;
    guru_tag script_tag = guru_ot_script_to_tag(ctx->script);
    guru_tag language_tag = guru_ot_language_to_tag(ctx->language);

    GURU_LOG("language: \"%c%c%c%c\"\n", GURU_UNTAG(language_tag), language_tag);
    GURU_LOG("script: \"%c%c%c%c\"\n", GURU_UNTAG(script_tag), script_tag);

    {
        /* Parse cmap table and convert unicode run to GID run */
        guru_buf_t *cmap_buf = &ctx->face->cmap_buf;
        guru_stream_t cmap_table = guru_stream_create(cmap_buf->data, cmap_buf->len, GURU_STREAM_BOUND_FLAG);

        uint16_t version;
        guru_stream_read16(&cmap_table, &version);

        /* Table version number must be 0 */
        GURU_ASSERT(version == 0);

        uint16_t num_encodings, enc_idx;
        guru_stream_read16(&cmap_table, &num_encodings);

        GURU_LOG("cmap table encoding count: %d\n", num_encodings);

        for (enc_idx = 0; enc_idx < num_encodings; ++enc_idx) {
            guru_cmap_encoding_t enc = {};
            guru_stream_read16(&cmap_table, &enc.platform_id);
            guru_stream_read16(&cmap_table, &enc.encoding_id);
            guru_stream_read32(&cmap_table, &enc.subtable_offset);

            GURU_LOG("platform: %s\n", guru_cmap_platform_to_string(enc.platform_id));
            GURU_LOG("encoding: %d\n", enc.encoding_id);
            GURU_LOG("subtable-offset: %d\n", enc.subtable_offset);


            // TODO: Handle length properly with bounding
            if (enc.platform_id == GURU_CMAP_PLATFORM_UNICODE) {
                uint16_t subtable_format = 0;
                guru_stream_t subtable_stream = guru_stream_create(cmap_buf->data + enc.subtable_offset,
                                                                   0, 0);
                guru_stream_read16(&subtable_stream, &subtable_format);

                GURU_LOG("subtable_format: %d\n", subtable_format);

                if (subtable_format == 4) {
                    /* Format 4: Segment mapping to delta values */
                    guru_cmap_subtable_format4_t subtable;
                    guru_stream_read16(&subtable_stream, &subtable.length);
                    guru_stream_read16(&subtable_stream, &subtable.language);
                    guru_stream_read16(&subtable_stream, &subtable.seg_count_x2);
                    guru_stream_read16(&subtable_stream, &subtable.search_range);
                    guru_stream_read16(&subtable_stream, &subtable.entry_selector);
                    guru_stream_read16(&subtable_stream, &subtable.range_shift);

                    GURU_LOG("length: %d\n", subtable.length);
                    GURU_LOG("language: %d\n", subtable.language);
                    GURU_LOG("seg_count_x2: %d\n", subtable.seg_count_x2);
                    GURU_LOG("search_range: %d\n", subtable.search_range);
                    GURU_LOG("entry_selector: %d\n", subtable.entry_selector);
                    GURU_LOG("range_shift: %d\n", subtable.range_shift);

                    uint16_t seg_jmp = subtable.seg_count_x2;

                    uint8_t *curr_addr = subtable_stream.data + subtable_stream.offset;
                    subtable.end_code = (uint16_t *)curr_addr;
                    subtable.start_code = (uint16_t *)(curr_addr + seg_jmp + sizeof(uint16_t));
                    subtable.id_delta = (int16_t *)(curr_addr + 2*seg_jmp + sizeof(uint16_t));
                    subtable.id_range_offsets = (uint16_t *)(curr_addr + 3*seg_jmp + sizeof(uint16_t));

                    /* map unicode characters to glyph indices in run */
                    run->id_arr = GURU_MALLOC(run->len * sizeof(guru_id));
                    run->id_count = run->len;
                    int c_idx = 0;
                    while (c_idx < run->len) {
                        guru_unicode c = run->text[c_idx];
                        run->id_arr[c_idx] = guru_cmap_unicode_to_id(&subtable, c);
                        GURU_LOG("U+%04X ", run->id_arr[c_idx]);
                        ++c_idx;
                    }
                }

                break;
            }
        }
    }

//    guru_set_t *lookup_indices = guru_set_create();
//    guru_ot_layout_collect_lookups(face, GURU_OT_TAG_GSUB,
//                              script_tag,
//                              language_tag,
//                              ctx->features,
//                              lookup_indices);
}