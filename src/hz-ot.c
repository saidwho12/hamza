#include "hz-ot.h"
#include "hz-ot-shape-complex-arabic.h"
#include "util/hz-map.h"

hz_glyph_class_t
hz_ignored_classes_from_lookup_flags(hz_lookup_flag_t flags)
{
    hz_glyph_class_t ignored_classes = HZ_GLYPH_CLASS_ZERO;

    if (flags & HZ_LOOKUP_FLAG_IGNORE_MARKS) ignored_classes |= HZ_GLYPH_CLASS_MARK;
    if (flags & HZ_LOOKUP_FLAG_IGNORE_BASE_GLYPHS) ignored_classes |= HZ_GLYPH_CLASS_BASE;
    if (flags & HZ_LOOKUP_FLAG_IGNORE_LIGATURES) ignored_classes |= HZ_GLYPH_CLASS_LIGATURE;

    return ignored_classes;
}

hz_feature_t
hz_ot_feature_from_tag(hz_tag_t tag) {
    /* loop over feature tag lookup table for it's id */
    int i;
    const hz_feature_info_t *feature_info = NULL;

    for (i = 0; i < HZ_FEATURE_COUNT; ++i) {
        feature_info = &HZ_FEATURE_INFO_LUT[i];
        if (feature_info->tag == tag) {
            return feature_info->feature;
        }
    }

    return -1;
}

hz_tag_t
hz_ot_tag_from_feature(hz_feature_t feature) {
    /* loop over feature tag lookup table for it's id */
    int i;
    const hz_feature_info_t *feature_info = NULL;

    for (i = 0; i < HZ_FEATURE_COUNT; ++i) {
        feature_info = &HZ_FEATURE_INFO_LUT[i];
        if (feature_info->feature == feature) {
            return feature_info->tag;
        }
    }

    return 0;
}

typedef struct hz_ligature_t {
    uint16_t ligature_glyph;
    uint16_t component_count;
    uint16_t *component_glyph_ids;
} hz_ligature_t;

hz_ligature_t *
hz_ligature_create() {
    hz_ligature_t *ligature = HZ_MALLOC(sizeof(hz_ligature_t));
    ligature->ligature_glyph = 0;
    ligature->component_count = 0;
    ligature->component_glyph_ids = NULL;
    return ligature;
}

void
hz_ligature_destroy(hz_ligature_t *ligature) {
    HZ_FREE(ligature->component_glyph_ids);
    HZ_FREE(ligature);
}

typedef struct hz_sequence_lookup_record_t {
    uint16_t sequence_index;
    uint16_t lookup_list_index;
} hz_sequence_lookup_record_t;

typedef struct hz_sequence_rule_t {
    uint16_t glyph_count;
    uint16_t seq_lookup_count;
    uint16_t *input_sequence;
    hz_sequence_lookup_record_t *seq_lookup_records;
} hz_sequence_rule_t;

typedef struct hz_sequence_rule_set_t {
    uint16_t rule_count;
    hz_sequence_rule_t *rules;
} hz_sequence_rule_set_t;


typedef struct hz_chained_sequence_rule_t {
    uint16_t backtrack_glyph_count;
    uint16_t *backtrack_sequence;
    uint16_t input_glyph_count;
    uint16_t *input_sequence;
    uint16_t lookahead_glyph_count;
    uint16_t *lookahead_sequence;
    uint16_t seq_lookup_count;
    hz_sequence_lookup_record_t *seq_lookup_records;
} hz_chained_sequence_rule_t;

typedef struct hz_chained_sequence_rule_set_t {
    uint16_t count;
    hz_chained_sequence_rule_t *rules;
} hz_chained_sequence_rule_set_t;

/* sequence of pointers to nodes which may have a gap between
 * useful for when checking sub-sequences with glyph class skip flags when applying lookups
 * as a form of cache. i.e. it's used in contextual substitutions when comparing a subsection of a sequence
 * and caching those nodes, so that later can easily move to specific subsections of the sequence.
 * */
typedef struct hz_sequence_node_cache_t {
    hz_sequence_node_t **nodes;
    size_t node_count;
} hz_sequence_node_cache_t;

hz_sequence_node_cache_t *hz_sequence_node_cache_create(void) {
    hz_sequence_node_cache_t *cache = HZ_ALLOC(hz_sequence_node_cache_t);
    cache->node_count = 0;
    cache->nodes = NULL;
    return cache;
}

hz_bool
hz_sequence_node_cache_is_empty(hz_sequence_node_cache_t *cache) {
    return cache->node_count == 0 || cache->nodes == NULL;
}

void
hz_sequence_node_cache_add(hz_sequence_node_cache_t *cache, hz_sequence_node_t *node) {
    if (hz_sequence_node_cache_is_empty(cache)) {
        cache->nodes = HZ_MALLOC(sizeof(hz_sequence_node_t *));
        cache->node_count = 1;
        cache->nodes[0] = node;
    } else {
        ++cache->node_count;
        cache->nodes = HZ_REALLOC(cache->nodes,sizeof(hz_sequence_node_t *) * cache->node_count);
        cache->nodes[cache->node_count-1] = node;
    }
}

void
hz_sequence_node_cache_clear(hz_sequence_node_cache_t *cache) {
    if (!hz_sequence_node_cache_is_empty(cache)) {
        HZ_FREE(cache->nodes);
        cache->nodes = NULL;
        cache->node_count = 0;
    }
}

void
hz_sequence_node_cache_destroy(hz_sequence_node_cache_t *cache) {
    hz_sequence_node_cache_clear(cache);
    HZ_FREE(cache);
}

void
hz_ligature_decode(hz_ligature_t *ligature, const hz_byte *data) {
    hz_stream_t *table = hz_stream_create(data, 0, 0);
    hz_stream_read16(table, &ligature->ligature_glyph);
    hz_stream_read16(table, &ligature->component_count);
    ligature->component_glyph_ids = HZ_MALLOC(sizeof(uint16_t) * (ligature->component_count - 1));
    hz_stream_read16_n(table, ligature->component_count - 1, ligature->component_glyph_ids);
    hz_stream_destroy(table);
}

static const hz_byte *
hz_ot_layout_choose_lang_sys(hz_face_t *face,
                             hz_byte *data,
                             hz_tag_t script,
                             hz_tag_t language)
{
    hz_byte                buffer[1024];
    hz_bump_allocator_t allocator;

    hz_bump_allocator_init(&allocator, buffer, sizeof(buffer));

    hz_stream_t *subtable = hz_stream_create(data, 0, 0);
    uint16_t script_count = 0;
    uint16_t index = 0;
    hz_rec16_t *script_records = NULL;
    uint16_t found_script = 0;
    const hz_byte *found_addr;

    hz_stream_read16(subtable, &script_count);
    HZ_LOG("script count: %d\n", script_count);
    script_records = hz_bump_allocator_alloc(&allocator, sizeof(hz_rec16_t) * script_count);

    while (index < script_count) {
        hz_tag_t curr_tag;
        uint16_t curr_offset;

        hz_stream_read32(subtable, &curr_tag);
        hz_stream_read16(subtable, &curr_offset);

        HZ_LOG("[%u] = \"%c%c%c%c\" (%u)\n", index, HZ_UNTAG(curr_tag), curr_offset);

        script_records[index].offset = curr_offset;
        script_records[index].tag = curr_tag;

        if (script == curr_tag) {
            found_script = index;
            break;
        }

        ++index;
    }

    /* Found script */
    uint16_t script_offset = script_records[found_script].offset;
    hz_stream_t *script_stream = hz_stream_create(data + script_offset, 0, 0);
    hz_offset16_t default_lang_sys_offset;
    uint16_t lang_sys_count;
    hz_stream_read16(script_stream, &default_lang_sys_offset);
    hz_stream_read16(script_stream, &lang_sys_count);
    found_addr = script_stream->data + default_lang_sys_offset;

    HZ_LOG("default lang sys: %u\n", default_lang_sys_offset);
    HZ_LOG("lang sys count: %u\n", lang_sys_count);


    uint16_t langSysIndex = 0;
    while (langSysIndex < lang_sys_count) {
        hz_rec16_t lang_sys_rec;
        hz_stream_read32(script_stream, &lang_sys_rec.tag);
        hz_stream_read16(script_stream, &lang_sys_rec.offset);

        HZ_LOG("[%u] = \"%c%c%c%c\" %u\n", langSysIndex, HZ_UNTAG(lang_sys_rec.tag), lang_sys_rec.offset);

        if (lang_sys_rec.tag == language) {
            /* Found language system */
            found_addr = script_stream->data + lang_sys_rec.offset;
            break;
        }

        ++langSysIndex;
    }

    /* Couldn't find alterior language system, return default. */
    hz_stream_destroy(script_stream);
    hz_stream_destroy(subtable);
    return found_addr;
}

static void
hz_ot_layout_parse_lang_sys() {

}



void
hz_ot_layout_feature_get_lookups(const uint8_t *data,
                                 hz_array_t *lookup_indices)
{

    hz_stream_t *table = hz_stream_create(data,0,0);
    hz_feature_table_t feature_table;
    hz_stream_read16(table, &feature_table.feature_params);
    hz_stream_read16(table, &feature_table.lookup_index_count);

//    HZ_LOG("feature_params: 0x%04X\n", feature_table.feature_params);
//    HZ_LOG("lookup_index_count: %u\n", feature_table.lookup_index_count);

    int i = 0;
    while (i < feature_table.lookup_index_count) {
        uint16_t lookup_index;
        hz_stream_read16(table, &lookup_index);
        hz_array_push_back(lookup_indices, lookup_index);
        ++i;
    }
}


void
hz_ot_layout_gather_feature_glyphs(hz_face_t *face,
                                   hz_stream_t *table,
                                   hz_set_t *glyphs)
{
    uint16_t lookup_type;
    uint16_t lookup_flags;
    uint16_t subtable_count;
    hz_stream_read16(table, &lookup_type);
    hz_stream_read16(table, &lookup_flags);
    hz_stream_read16(table, &subtable_count);
    uint16_t subtable_index = 0;

    while (subtable_index < subtable_count) {
        hz_offset16_t offset;
        hz_stream_read16(table, &offset);
        hz_stream_t *subtable = hz_stream_create(table->data + offset, 0, 0);
        uint16_t format;
        hz_stream_read16(subtable, &format);

        switch (lookup_type) {
            case HZ_GSUB_LOOKUP_TYPE_SINGLE_SUBSTITUTION: {
                if (format == 1) {
                    hz_offset16_t coverage_offset;
                    int16_t id_delta;
                    hz_stream_read16(subtable, &coverage_offset);
                    hz_stream_read16(subtable, (uint16_t *) &id_delta);
                    /* NOTE: Implement */
                }
                else if (format == 2) {
                    hz_offset16_t coverage_offset;
                    uint16_t glyph_count;
                    hz_stream_read16(subtable, &coverage_offset);
                    hz_stream_read16(subtable, &glyph_count);

                    /* Get destination glyph indices */
                    uint16_t i;
                    for (i = 0; i < glyph_count; ++i) {
                        hz_index_t substitute_glyph;
                        hz_stream_read16(subtable, &substitute_glyph);
                        hz_set_add_no_duplicate(glyphs, substitute_glyph);
                    }
                }
                break;
            }

            case HZ_GSUB_LOOKUP_TYPE_MULTIPLE_SUBSTITUTION: {
                break;
            }

            case HZ_GSUB_LOOKUP_TYPE_ALTERNATE_SUBSTITUTION: {
                break;
            }

            case HZ_GSUB_LOOKUP_TYPE_LIGATURE_SUBSTITUTION: {
                if (format == 1) {
                    hz_offset16_t coverage_offset;
                    uint16_t ligature_set_count;
                    hz_offset16_t *ligature_set_offsets;

                    hz_stream_read16(subtable, &coverage_offset);
                    hz_stream_read16(subtable, &ligature_set_count);
                    ligature_set_offsets = malloc(ligature_set_count * sizeof(uint16_t));
                    hz_stream_read16_n(subtable, ligature_set_count, ligature_set_offsets);

                    /* loop over every ligature set, then ligatures, and collect final glyph ids */
                    uint16_t ls, l;
                    for (ls = 0; ls < ligature_set_count; ++ls) {
                        hz_offset16_t os = ligature_set_offsets[ ls ];
                        uint16_t ligature_count;
                        hz_stream_t *ligature_set = hz_stream_create(subtable->data + os, 0, 0);
                        hz_stream_read16(ligature_set, &ligature_count);

                        for (l = 0; l < ligature_count; ++l) {
                            hz_offset16_t ligature_offset;
                            hz_stream_read16(ligature_set, &ligature_offset);
                            hz_ligature_t *ligature = hz_ligature_create();
                            hz_ligature_decode(ligature, ligature_set->data + ligature_offset);
                            hz_set_add_no_duplicate(glyphs, ligature->ligature_glyph);
                            hz_ligature_destroy(ligature);
                        }
                    }

                    free(ligature_set_offsets);
                } else {
                    /* error */
                }
                break;
            }

            case HZ_GSUB_LOOKUP_TYPE_CONTEXTUAL_SUBSTITUTION: {
                break;
            }

            case HZ_GSUB_LOOKUP_TYPE_CHAINED_CONTEXTS_SUBSTITUTION: {
                break;
            }

            case HZ_GSUB_LOOKUP_TYPE_EXTENSION_SUBSTITUTION: {
                break;
            }

            case HZ_GSUB_LOOKUP_TYPE_REVERSE_CHAINING_CONTEXTUAL_SINGLE_SUBSTITUTION: {
                break;
            }
        }

        ++subtable_index;
    }
}

hz_bool
hz_ot_layout_gather_glyphs(hz_face_t *face,
                           hz_tag_t script,
                           hz_tag_t language,
                           const hz_array_t *wanted_features,
                           hz_set_t *glyphs)
{
    HZ_ASSERT(face != NULL);
    HZ_ASSERT(wanted_features != NULL);


    hz_stream_t *table = hz_stream_create(hz_face_get_ot_tables(face)->GSUB_table, 0, 0);
    uint32_t version;
    uint16_t script_list_offset;
    uint16_t feature_list_offset;
    uint16_t lookup_list_offset;
    uint32_t feature_variations_offset;

    hz_stream_read32(table, &version);

    switch (version) {
        case 0x00010000: /* 1.0 */
            hz_stream_read16(table, &script_list_offset);
            hz_stream_read16(table, &feature_list_offset);
            hz_stream_read16(table, &lookup_list_offset);
            break;

        case 0x00010001: /* 1.1 */
            hz_stream_read16(table, &script_list_offset);
            hz_stream_read16(table, &feature_list_offset);
            hz_stream_read16(table, &lookup_list_offset);
            hz_stream_read32(table, &feature_variations_offset);
            break;

        default:
            /* error */
            break;
    }

    void *lsaddr = hz_ot_layout_choose_lang_sys(face,
                                                table->data + script_list_offset,
                                                script, language);

    if (lsaddr == NULL) {
        /* Language system was not found */
        HZ_ERROR("Language system was not found!\n");
        return HZ_FALSE;
    }

    HZ_LOG("Found language system!\n");

//    hz_array_t *lang_feature_indices = hz_array_create();
    hz_stream_t *lsbuf = hz_stream_create(lsaddr, 0, 0);

    hz_lang_sys_t langSys;
    hz_stream_read16(lsbuf, &langSys.lookupOrder);
    hz_stream_read16(lsbuf, &langSys.requiredFeatureIndex);
    hz_stream_read16(lsbuf, &langSys.featureIndexCount);

    /* lookupOrder should be (nil) */
    HZ_LOG("lookupOrder: %p\n", (void *) langSys.lookupOrder);
    HZ_LOG("requiredFeatureIndex: %u\n", langSys.requiredFeatureIndex);
    HZ_LOG("featureIndexCount: %u\n", langSys.featureIndexCount);

    if (langSys.requiredFeatureIndex == 0xFFFF) {
        HZ_LOG("No required features!\n");
    }

//    uint16_t loopIndex = 0;
//    while (loopIndex < langSys.featureIndexCount) {
//        uint16_t featureIndex;
//        hz_stream_read16(lsbuf, &featureIndex);
//        HZ_LOG("[%u] = %u\n", loopIndex, featureIndex);
//        hz_array_push_back(lang_feature_indices, featureIndex);
//        ++loopIndex;
//    }

    hz_stream_t *lookup_list = hz_stream_create(table->data + lookup_list_offset, 0, 0);
    hz_array_t *lookup_offsets = hz_array_create();
    {
        /* Read lookup offets to table */
        uint16_t lookup_count;
        uint16_t lookup_index = 0;
        hz_stream_read16(lookup_list, &lookup_count);
        while (lookup_index < lookup_count) {
            uint16_t lookup_offset;
            hz_stream_read16(lookup_list, &lookup_offset);
            hz_array_push_back(lookup_offsets, lookup_offset);
            ++lookup_index;
        }
    }


    hz_stream_t *feature_list = hz_stream_create(table->data + feature_list_offset, 0, 0);


    {
        /* Parsing the FeatureList and applying selected Features */
        uint16_t feature_count;
        uint16_t feature_index = 0;
        hz_stream_read16(feature_list, &feature_count);
        HZ_LOG("feature_count: %u\n", feature_count);

        hz_map_t *feature_map = hz_map_create();

        /* fill map from feature type to offset */
        while (feature_index < feature_count) {
            hz_tag_t tag;
            uint16_t offset;
            hz_stream_read32(feature_list, &tag);
            hz_stream_read16(feature_list, &offset);
            hz_feature_t feature = hz_ot_feature_from_tag(tag);
            hz_map_set_value(feature_map, feature, offset);
            ++feature_index;
        }

        uint16_t wanted_feature_count = hz_array_size(wanted_features);
        uint16_t wanted_feature_index = 0;
        while (wanted_feature_index < wanted_feature_count) {
            hz_feature_t wanted_feature = hz_array_at(wanted_features, wanted_feature_index);

            if ( hz_map_value_exists(feature_map, wanted_feature) ) {
                /* feature is wanted and exists */
                hz_offset16_t feature_offset = hz_map_get_value(feature_map, wanted_feature);
                hz_array_t *lookup_indices = hz_array_create();
                hz_ot_layout_feature_get_lookups(feature_list->data + feature_offset, lookup_indices);

                int i = 0;
                while (i < hz_array_size(lookup_indices)) {
                    uint16_t lookup_offset = hz_array_at(lookup_offsets, hz_array_at(lookup_indices, i));
                    hz_stream_t *lookup_table = hz_stream_create(lookup_list->data + lookup_offset, 0, 0);
                    hz_ot_layout_gather_feature_glyphs(face, lookup_table, glyphs);
                    ++i;
                }

                hz_array_destroy(lookup_indices);
            }

            ++wanted_feature_index;
        }

        hz_map_destroy(feature_map);
    }


    hz_array_destroy(lookup_offsets);
    return HZ_TRUE;
}



/*
 * nested_index is set to an integer between 0 and the length of the sequence
 * if it is set to -1, then apply lookup to entire sequence.
 * */
void
hz_ot_layout_apply_gsub_lookup(hz_face_t *face,
                               hz_feature_t feature,
                               hz_sequence_t *sequence,
                               hz_sequence_node_t *nested,
                               hz_stream_t *lookup)
{
    uint16_t lookup_type, lookup_flags, subtable_count;

    hz_stream_read16(lookup, &lookup_type);
    hz_stream_read16(lookup, &lookup_flags);
    hz_stream_read16(lookup, &subtable_count);

//    hz_glyph_class_t class_ignore = hz_ignored_classes_from_lookup_flags(lookup_flags);

    uint16_t i;
    for (i=0; i<subtable_count; ++i) {
        hz_offset16_t offset;
        hz_stream_read16(lookup, &offset);
        hz_stream_t *subtable = hz_stream_create(lookup->data + offset, 0, 0);
        hz_ot_layout_apply_gsub_subtable(face, subtable, lookup_type, lookup_flags, feature, sequence, nested);
        hz_stream_destroy(subtable);
    }
}

hz_bool
hz_ot_layout_apply_lookup(hz_face_t *face,
                          hz_feature_t feature,
                          hz_tag_t table_tag,
                          hz_sequence_t *sequence,
                          hz_sequence_node_t *nested,
                          uint16_t lookup_index)
{
    hz_stream_t *table;
    uint32_t version;
    uint16_t script_list_offset;
    uint16_t feature_list_offset;
    uint16_t lookup_list_offset;
    uint32_t feature_variations_offset;

    if (table_tag == HZ_OT_TAG_GSUB) {
        table = hz_stream_create(hz_face_get_ot_tables(face)->GSUB_table, 0, 0);
    } else if (table_tag == HZ_OT_TAG_GPOS) {
        table = hz_stream_create(hz_face_get_ot_tables(face)->GPOS_table, 0, 0);
    } else {
        /* error */
        return HZ_FALSE;
    }

    hz_stream_read32(table, &version);

    switch (version) {
        case 0x00010000: /* 1.0 */
            hz_stream_read16(table, &script_list_offset);
            hz_stream_read16(table, &feature_list_offset);
            hz_stream_read16(table, &lookup_list_offset);
            break;
        case 0x00010001: /* 1.1 */
            hz_stream_read16(table, &script_list_offset);
            hz_stream_read16(table, &feature_list_offset);
            hz_stream_read16(table, &lookup_list_offset);
            hz_stream_read32(table, &feature_variations_offset);
            break;

        default:
            /* error */
            return HZ_FALSE;
    }

    uint8_t *lookup_list = table->data  + lookup_list_offset;
    uint16_t lookup_count = bswap16(*(uint16_t*)&lookup_list[0]);

    if (lookup_index >= lookup_count)
        return HZ_FALSE;

    uint16_t lookup_offset = bswap16(((uint16_t*)lookup_list)[1+lookup_index]);
    hz_stream_t *lookup_table = hz_stream_create(lookup_list + lookup_offset, 0, 0);
    hz_ot_layout_apply_gsub_lookup(face, feature, sequence, nested, lookup_table);
    hz_stream_destroy(lookup_table);

    return HZ_TRUE;
}

hz_bool
hz_ot_layout_apply_gsub_features(hz_face_t *face,
                                 hz_tag_t script,
                                 hz_tag_t language,
                                 const hz_array_t *wanted_features,
                                 hz_sequence_t *sect)
{
    HZ_ASSERT(face != NULL);
    HZ_ASSERT(wanted_features != NULL);

    hz_stream_t *table = hz_stream_create(hz_face_get_ot_tables(face)->GSUB_table, 0, 0);
    uint32_t version;
    uint16_t script_list_offset;
    uint16_t feature_list_offset;
    uint16_t lookup_list_offset;
    uint32_t feature_variations_offset;

    hz_stream_read32(table, &version);

    switch (version) {
        case 0x00010000: /* 1.0 */
            hz_stream_read16(table, &script_list_offset);
            hz_stream_read16(table, &feature_list_offset);
            hz_stream_read16(table, &lookup_list_offset);
            break;
        case 0x00010001: /* 1.1 */
            hz_stream_read16(table, &script_list_offset);
            hz_stream_read16(table, &feature_list_offset);
            hz_stream_read16(table, &lookup_list_offset);
            hz_stream_read32(table, &feature_variations_offset);
            break;

        default:
            /* error */
            break;
    }

    void *lsaddr = hz_ot_layout_choose_lang_sys(face,
                                                table->data + script_list_offset,
                                                script, language);

    if (lsaddr == NULL) {
        /* Language system was not found */
        HZ_ERROR("Language system was not found!\n");
        return HZ_FALSE;
    }

    HZ_LOG("Found language system!\n");

    hz_lang_sys_t langSys;

    {
        hz_stream_t *lsbuf = hz_stream_create(lsaddr, 0, 0);

        hz_stream_read16(lsbuf, &langSys.lookupOrder);
        hz_stream_read16(lsbuf, &langSys.requiredFeatureIndex);
        hz_stream_read16(lsbuf, &langSys.featureIndexCount);

        hz_stream_destroy(lsbuf);
    }

    /* lookupOrder should be (nil) */
    HZ_LOG("lookupOrder: %p\n", (void *) langSys.lookupOrder);
    HZ_LOG("requiredFeatureIndex: %u\n", langSys.requiredFeatureIndex);
    HZ_LOG("featureIndexCount: %u\n", langSys.featureIndexCount);

    if (langSys.requiredFeatureIndex == 0xFFFF) {
        HZ_LOG("No required features!\n");
    }

//    uint16_t loopIndex = 0;
//    while (loopIndex < langSys.featureIndexCount) {
//        uint16_t featureIndex;
//        hz_stream_read16(lsbuf, &featureIndex);
//        HZ_LOG("[%u] = %u\n", loopIndex, featureIndex);
//        hz_array_push_back(lang_feature_indices, featureIndex);
//        ++loopIndex;
//    }

    hz_array_t *lookup_offsets = hz_array_create();
    hz_stream_t *lookup_list = hz_stream_create(table->data + lookup_list_offset, 0, 0);
    {
        {
            /* Read lookup offets to table */
            uint16_t lookup_count;
            uint16_t lookup_index = 0;
            hz_stream_read16(lookup_list, &lookup_count);
            while (lookup_index < lookup_count) {
                uint16_t lookup_offset;
                hz_stream_read16(lookup_list, &lookup_offset);
                hz_array_push_back(lookup_offsets, lookup_offset);
                ++lookup_index;
            }
        }
    }


    hz_stream_t *feature_list = hz_stream_create(table->data + feature_list_offset, 0, 0);


    {
        /* Parsing the FeatureList and applying selected Features */
        uint16_t feature_count;
        uint16_t feature_index = 0;
        hz_stream_read16(feature_list, &feature_count);
        HZ_LOG("feature_count: %u\n", feature_count);

        hz_map_t *feature_map = hz_map_create();

        /* fill map from feature type to offset */
        while (feature_index < feature_count) {
            hz_tag_t tag;
            uint16_t offset;
            hz_stream_read32(feature_list, &tag);
            hz_stream_read16(feature_list, &offset);
            hz_feature_t feature = hz_ot_feature_from_tag(tag);
            hz_map_set_value(feature_map, feature, offset);
            ++feature_index;
        }

        uint16_t wanted_feature_count = hz_array_size(wanted_features);
        uint16_t wanted_feature_index = 0;
        while (wanted_feature_index < wanted_feature_count) {
            hz_feature_t wanted_feature = hz_array_at(wanted_features, wanted_feature_index);

            if ( hz_map_value_exists(feature_map, wanted_feature) ) {
                /* feature is wanted and exists */
                hz_offset16_t feature_offset = hz_map_get_value(feature_map, wanted_feature);
                hz_array_t *lookup_indices = hz_array_create();
                hz_ot_layout_feature_get_lookups(feature_list->data + feature_offset, lookup_indices);

                int i = 0;
                while (i < hz_array_size(lookup_indices)) {
                    uint16_t lookup_offset = hz_array_at(lookup_offsets, hz_array_at(lookup_indices, i));
                    hz_stream_t *lookup_table = hz_stream_create(lookup_list->data + lookup_offset, 0, 0);
                    hz_ot_layout_apply_gsub_feature(face, lookup_table, wanted_feature, sect);
                    hz_stream_destroy(lookup_table);
                    ++i;
                }

                hz_array_destroy(lookup_indices);
            }

            ++wanted_feature_index;
        }


        hz_map_destroy(feature_map);
    }

    hz_stream_destroy(lookup_list);
    hz_stream_destroy(feature_list);
    hz_array_destroy(lookup_offsets);
    hz_stream_destroy(table);
    return HZ_TRUE;
}

hz_bool
hz_ot_layout_apply_gpos_features(hz_face_t *face,
                                 hz_tag_t script,
                                 hz_tag_t language,
                                 const hz_array_t *wanted_features,
                                 hz_sequence_t *sequence)
{
    HZ_ASSERT(face != NULL);
    HZ_ASSERT(wanted_features != NULL);

    hz_stream_t *table = hz_stream_create(hz_face_get_ot_tables(face)->GPOS_table, 0,0);
    uint32_t version;
    uint16_t script_list_offset;
    uint16_t feature_list_offset;
    uint16_t lookup_list_offset;
    uint32_t feature_variations_offset;

    hz_stream_read32(table, &version);

    switch (version) {
        case 0x00010000: /* 1.0 */
            hz_stream_read16(table, &script_list_offset);
            hz_stream_read16(table, &feature_list_offset);
            hz_stream_read16(table, &lookup_list_offset);
            break;
        case 0x00010001: /* 1.1 */
            hz_stream_read16(table, &script_list_offset);
            hz_stream_read16(table, &feature_list_offset);
            hz_stream_read16(table, &lookup_list_offset);
            hz_stream_read32(table, &feature_variations_offset);
            break;
        default: /* error */
            break;
    }

    void *lsaddr = hz_ot_layout_choose_lang_sys(face,
                                                table->data + script_list_offset,
                                                script, language);

    if (lsaddr == NULL) {
        /* Language system was not found */
        HZ_ERROR("Language system was not found!\n");
        return HZ_FALSE;
    }

    HZ_LOG("Found language system!\n");

    hz_array_t *lang_feature_indices = hz_array_create();
    hz_stream_t *lsbuf = hz_stream_create(lsaddr, 0, 0);

    hz_lang_sys_t langSys;
    hz_stream_read16(lsbuf, &langSys.lookupOrder);
    hz_stream_read16(lsbuf, &langSys.requiredFeatureIndex);
    hz_stream_read16(lsbuf, &langSys.featureIndexCount);

    /* lookupOrder should be (nil) */
    HZ_LOG("lookupOrder: %p\n", (void *) langSys.lookupOrder);
    HZ_LOG("requiredFeatureIndex: %u\n", langSys.requiredFeatureIndex);
    HZ_LOG("featureIndexCount: %u\n", langSys.featureIndexCount);

    if (langSys.requiredFeatureIndex == 0xFFFF) {
        HZ_LOG("No required features!\n");
    }

    uint16_t loopIndex = 0;
    while (loopIndex < langSys.featureIndexCount) {
        uint16_t featureIndex;
        hz_stream_read16(lsbuf, &featureIndex);
        HZ_LOG("[%u] = %u\n", loopIndex, featureIndex);
        hz_array_push_back(lang_feature_indices, featureIndex);
        ++loopIndex;
    }

    hz_stream_t *lookup_list = hz_stream_create(table->data + lookup_list_offset, 0, 0);
    hz_array_t *lookup_offsets = hz_array_create();

    {
        /* Read lookup offets to table */
        uint16_t lookup_count;
        uint16_t lookup_index = 0;
        hz_stream_read16(lookup_list, &lookup_count);
        while (lookup_index < lookup_count) {
            uint16_t lookup_offset;
            hz_stream_read16(lookup_list, &lookup_offset);
            hz_array_push_back(lookup_offsets, lookup_offset);
            ++lookup_index;
        }
    }


    hz_stream_t *feature_list = hz_stream_create(table->data + feature_list_offset, 0, 0);


    {
        /* Parsing the FeatureList and applying selected Features */
        uint16_t feature_count;
        uint16_t feature_index = 0;
        hz_stream_read16(feature_list, &feature_count);
        HZ_LOG("feature_count: %u\n", feature_count);

        hz_map_t *feature_map = hz_map_create();

        /* fill map from feature type to offset */
        while (feature_index < feature_count) {
            hz_tag_t tag;
            uint16_t offset;
            hz_stream_read32(feature_list, &tag);
            hz_stream_read16(feature_list, &offset);
            hz_feature_t feature = hz_ot_feature_from_tag(tag);
            hz_map_set_value(feature_map, feature, offset);
            ++feature_index;
        }

        uint16_t wanted_feature_count = hz_array_size(wanted_features);
        uint16_t wanted_feature_index = 0;
        while (wanted_feature_index < wanted_feature_count) {
            hz_feature_t wanted_feature = hz_array_at(wanted_features, wanted_feature_index);

            if ( hz_map_value_exists(feature_map, wanted_feature) ) {
                /* feature is wanted and exists */
                hz_offset16_t feature_offset = hz_map_get_value(feature_map, wanted_feature);
                hz_array_t *lookup_indices = hz_array_create();
                hz_ot_layout_feature_get_lookups(feature_list->data + feature_offset, lookup_indices);

                int i = 0;
                while (i < hz_array_size(lookup_indices)) {
                    uint16_t lookup_offset = hz_array_at(lookup_offsets, hz_array_at(lookup_indices, i));
                    hz_stream_t *lookup_table = hz_stream_create(lookup_list->data + lookup_offset, 0, 0);
                    hz_ot_layout_apply_gpos_feature(face, lookup_table, wanted_feature, sequence);
                    hz_stream_destroy(lookup_table);
                    ++i;
                }

                hz_array_destroy(lookup_indices);
            }

            ++wanted_feature_index;
        }


        hz_map_destroy(feature_map);
    }

    hz_array_destroy(lookup_offsets);
    hz_stream_destroy(lookup_list);
    hz_stream_destroy(table);
    return HZ_TRUE;
}


void
hz_ot_layout_lookups_substitute_closure(hz_face_t *face,
                                          const hz_set_t *lookups,
                                          hz_set_t *glyphs)
{

}

hz_bool
hz_ot_layout_lookup_would_substitute(hz_face_t *face,
                                     unsigned int lookup_index,
                                     const hz_index_t *glyphs,
                                     unsigned int glyph_count,
                                     hz_bool zero_context)
{

}

hz_bool
hz_decode_coverage(const uint8_t *data,
                   hz_map_t *map,
                   hz_array_t *id_arr)
{
    uint16_t coverage_format = 0;
    hz_stream_t *table = hz_stream_create(data,0,0);

    hz_stream_read16(table, &coverage_format);

    switch (coverage_format) {
        case 1: {
            uint16_t coverage_idx = 0;
            uint16_t coverage_glyph_count;
            hz_stream_read16(table, &coverage_glyph_count);
            while (coverage_idx < coverage_glyph_count) {
                uint16_t glyph_index;
                hz_stream_read16(table, &glyph_index);
//                hz_array_push_back(coverage_glyphs, glyph_index);
                if (id_arr != NULL)
                    hz_map_set_value(map, glyph_index, hz_array_at(id_arr, coverage_idx));
                else
                    hz_map_set_value(map, glyph_index, coverage_idx);

                ++coverage_idx;
            }

            break;
        }

        case 2: {
            uint16_t range_index = 0, range_count;
            hz_stream_read16(table, &range_count);

            /* Assuming ranges are ordered from 0 to glyph_count in order */
            while (range_index < range_count) {
                hz_index_t from, to;
                hz_range_rec_t range;
                uint16_t range_offset;
                uint32_t range_end;

                hz_stream_read16(table, &range.start_glyph_id);
                hz_stream_read16(table, &range.end_glyph_id);
                hz_stream_read16(table, &range.start_coverage_index);

                range_offset = 0;
                range_end = (range.end_glyph_id - range.start_glyph_id);
                while (range_offset <= range_end) {
                    from = range.start_glyph_id + range_offset;

                    if (id_arr != NULL)
                        to = hz_array_at(id_arr, range.start_coverage_index + range_offset);
                    else
                        to = range.start_coverage_index + range_offset;

                    hz_map_set_value(map, from, to);

//                    HZ_LOG("%d -> %d\n", from, to);
                    ++range_offset;
                }

                ++range_index;
            }
            break;
        }

        default:
            /* error */
            break;
    }

    hz_stream_destroy(table);
    return HZ_TRUE;
}

hz_sequence_node_t *
hz_prev_node_not_of_class(hz_sequence_node_t *g,
                          hz_glyph_class_t gcignore,
                          int *skip)
{
    if (gcignore == HZ_GLYPH_CLASS_ZERO) {
        /* if no ignored classes, just give the next glyph directly as an optimization */
        if (skip != NULL) (* skip) --;
        return g->prev;
    }

    while (1) {
        if (skip != NULL) (* skip) --;
        g = g->prev;

        /* break if prev node is NULL, cannot keep searching */
        if (g == NULL)
            break;

        /* if not any of the class flags set, break, as we found what we want */
        if (~g->gc & gcignore)
            break;
    }

    return g;
}

hz_sequence_node_t *
hz_next_node_not_of_class(hz_sequence_node_t *g,
                          hz_glyph_class_t gcignore,
                          int64_t *skip)
{
    if (gcignore == HZ_GLYPH_CLASS_ZERO) {
        /* if no ignored classes, just give the next glyph directly as an optimization */
        if (skip != NULL) (* skip) ++;
        return g->next;
    }

    while (1) {
        if (skip != NULL) (* skip) ++;
        g = g->next;

        /* break if next node is NULL, cannot keep searching */
        if (g == NULL)
            break;

        /* if not any of the class flags set, break, as we found what we want */
        if (~g->gc & gcignore)
            break;
    }

    return g;
}

hz_sequence_node_t *
hz_next_node_of_class(hz_sequence_node_t *node,
                      hz_glyph_class_t gc,
                      int *skip)
{
    if (gc == HZ_GLYPH_CLASS_ZERO) {
        /* if no wanted classes, just give the next glyph directly as an optimization */
        if (skip != NULL) (* skip) ++;
        return node->next;
    }

    while (1) {
        if (skip != NULL) (* skip) ++;
        node = node->next;

        /* break if next node is NULL, cannot keep searching */
        if (node == NULL)
            break;

        /* if not any of the class flags set, break, as we found what we want */
        if (node->gc & gc)
            break;
    }

    return node;
}

hz_sequence_node_t *
hz_next_node_of_class_bound(hz_sequence_node_t *node,
                            hz_glyph_class_t gc,
                            int *skip,
                            int max_skip)
{
    if (gc == HZ_GLYPH_CLASS_ZERO) {
        /* if no wanted classes, just give the next glyph directly as an optimization */
        if (skip != NULL) (* skip) ++;
        return node->next;
    }

    while (*skip < max_skip) {
        if (skip != NULL) (* skip) ++;
        node = node->next;

        /* break if next node is NULL, cannot keep searching */
        if (node == NULL)
            break;

        /* if not any of the class flags set, break, as we found what we want */
        if (node->gc & gc)
            break;
    }

    return node;
}

/* assumes node is not NULL */
hz_sequence_node_t *
hz_last_node_of_class(hz_sequence_node_t *node,
                      hz_glyph_class_t gc,
                      int *skip)
{
    if (gc == HZ_GLYPH_CLASS_ZERO)
        return node;

    while(1) {
        if (node == NULL) break;
        if (node->next == NULL) break;
        if (~node->next->gc & gc) break;
        node = node->next;
        if (skip != NULL) (* skip) ++;
    }

    return node;
}

hz_sequence_node_t *
hz_prev_node_of_class(hz_sequence_node_t *node,
                      hz_glyph_class_t gc,
                      uint16_t *skip_count)
{
    if (gc == HZ_GLYPH_CLASS_ZERO) {
        /* if no wanted classes, just give the prev glyph directly as an optimization */
        if (skip_count != NULL) (* skip_count) --;
        return node->prev;
    }

    while (1) {
        if (skip_count != NULL) (* skip_count) --;
        node = node->prev;

        /* break if next node is NULL, cannot keep searching */
        if (node == NULL)
            break;

        /* if not any of the class flags set, break, as we found what we want */
        if (node->gc & gc)
            break;
    }

    return node;
}


#define HZ_MAX(x, y) (((x) > (y)) ? (x) : (y))

/* if possible, apply ligature fit for sequence of glyphs
 * returns true if replacement occurred
 * */
void
hz_ot_layout_apply_fit_ligature(hz_ligature_t **ligatures,
                                uint16_t ligature_count,
                                hz_glyph_class_t ignore_flags,
                                hz_sequence_node_t *start_node)
{
    uint16_t i, j;
    hz_sequence_node_cache_t *node_cache = hz_sequence_node_cache_create();



    for (i=0; i<ligature_count; ++i) {
        int64_t skip_count = 0;
        hz_sequence_node_t *step_node = start_node;
        hz_ligature_t *ligature = ligatures[i];
        hz_bool did_match = HZ_TRUE;

        /* go over sequence and compare with current ligature */
        for (j=0; j<ligature->component_count-1; ++j) {
            step_node = hz_next_node_not_of_class(step_node, ignore_flags, &skip_count);

            if (step_node == NULL) {
                did_match = HZ_FALSE;
                break;
            }

            hz_index_t g1 = step_node->id;
            hz_index_t g2 = ligature->component_glyph_ids[j];
            if (g1 != g2) {
                did_match = HZ_FALSE;
                break;
            }
        }

//        if (did_match) {
//            start_node->id = ligature->ligature_glyph;
//            start_node->gc |= HZ_GLYPH_CLASS_LIGATURE;
//            break;
//        }

        if (did_match) {
            /* pattern matches, replace it and tag following mark glyphs with
             * a component id, link all following marks together after the ligature which were attached
             * to the ligature.
             * */
            int comp_count = ligature->component_count;

            if (comp_count - 1) {
                if (ignore_flags == HZ_GLYPH_CLASS_ZERO)
                    hz_sequence_rem_next_n_nodes(start_node, comp_count-1);
                else {
                    hz_sequence_node_t *n1, *n2 = start_node;

                    int comp_index = 0;
                    while (comp_index < comp_count) {
                        hz_sequence_node_t *child;
                        int skip = 0;
                        n1 = n2;
                        n2 = hz_next_node_of_class_bound(n1, ignore_flags, &skip, comp_count);

                        if ((comp_index + skip) > comp_count) {
                            int rem = HZ_MAX((comp_index + skip) - comp_count, 0);
                            if (rem)
                                hz_sequence_rem_next_n_nodes(n1, rem);
                        } else {
                            hz_sequence_rem_node_range(n1, n2);
                        }

                        if (n2 == NULL) break;

                        n1 = n2;
                        n2 = hz_last_node_of_class(n1, HZ_GLYPH_CLASS_MARK, NULL);

                        for (child = n1; child != n2->next; child = child->next) {
                            child->cid = comp_index ? comp_index : skip-1;
                        }

                        comp_index += comp_index ? skip-1 : skip;
                    }
                }
            }

            /* insert ligature glyph */
            start_node->id = ligature->ligature_glyph;
            start_node->gc |= HZ_GLYPH_CLASS_LIGATURE;
            break;
        }
    }

    hz_sequence_node_cache_destroy(node_cache);
}

hz_sequence_rule_t
hz_sequence_rule_create(void)
{
    hz_sequence_rule_t rule;

    rule.glyph_count = 0;
    rule.seq_lookup_count = 0;
    rule.input_sequence = NULL;
    rule.seq_lookup_records = NULL;

    return rule;
}

void
hz_sequence_rule_destroy(hz_sequence_rule_t *rule)
{
    if (rule->input_sequence != NULL) HZ_FREE(rule->input_sequence);
    if (rule->seq_lookup_records != NULL) HZ_FREE(rule->seq_lookup_records);
}

void
hz_ot_read_sequence_lookup_record(hz_stream_t *stream,
                                  hz_sequence_lookup_record_t *record)
{
    hz_stream_read16(stream, &record->sequence_index);
    hz_stream_read16(stream, &record->lookup_list_index);
}

void
hz_ot_parse_sequence_rule(const hz_byte *data, hz_sequence_rule_t *rule) {
    size_t i;
    hz_stream_t *stream = hz_stream_create(data,0,0);

    hz_stream_read16(stream, &rule->glyph_count);
    hz_stream_read16(stream, &rule->seq_lookup_count);

    rule->input_sequence = HZ_MALLOC(rule->glyph_count-1);
    hz_stream_read16_n(stream, rule->glyph_count-1, rule->input_sequence);

    rule->seq_lookup_records = HZ_MALLOC(sizeof(hz_sequence_lookup_record_t)
            * rule->seq_lookup_count);

    for (i = 0; i < rule->seq_lookup_count; ++i)
        hz_ot_read_sequence_lookup_record(stream, &rule->seq_lookup_records[i]);

    hz_stream_destroy(stream);
}

void
hz_ot_parse_sequence_rule_set(const hz_byte *data, hz_sequence_rule_set_t *rule_set)
{
    hz_stream_t *stream = hz_stream_create(data,0,0);
    hz_stream_read16(stream, &rule_set->rule_count);

    HZ_HEAPARR(rule_offsets, rule_set->rule_count, uint16_t);

    if (rule_set->rule_count) {
        uint16_t rule_index;
        hz_stream_read16_n(stream, rule_set->rule_count, rule_offsets);

        rule_set->rules = HZ_MALLOC(sizeof(hz_sequence_rule_t) * rule_set->rule_count);

        for (rule_index = 0; rule_index < rule_set->rule_count; ++rule_index) {
            uint16_t rule_offset = rule_offsets[rule_index];
            if (rule_offset) {
                /* If the offset is not NULL */
                hz_ot_parse_sequence_rule(stream->data + rule_offset, &rule_set->rules[rule_index]);
            }
        }
    }

    HZ_FREE(rule_offsets);
    hz_stream_destroy(stream);
}

void
hz_ot_parse_chained_sequence_rule(hz_byte *data, hz_chained_sequence_rule_t *rule) {
    hz_byte                  buffer[1024];
    hz_bump_allocator_t        allocator;
    hz_stream_t                *stream;
    size_t                     i;

    hz_bump_allocator_init(&allocator, buffer, sizeof(buffer));
    stream = hz_stream_create(data,0,0);

    hz_stream_read16(stream, &rule->backtrack_glyph_count);
    rule->backtrack_sequence = HZ_MALLOC(sizeof(uint16_t) * rule->backtrack_glyph_count);
    hz_stream_read16_n(stream, rule->backtrack_glyph_count, rule->backtrack_sequence);

    hz_stream_read16(stream, &rule->input_glyph_count);
    rule->input_sequence = HZ_MALLOC(sizeof(uint16_t) * (rule->input_glyph_count - 1));
    hz_stream_read16_n(stream, rule->input_glyph_count - 1, rule->input_sequence);

    hz_stream_read16(stream, &rule->lookahead_glyph_count);
    rule->lookahead_sequence = HZ_MALLOC(sizeof(uint16_t) * rule->lookahead_glyph_count);
    hz_stream_read16_n(stream, rule->lookahead_glyph_count, rule->lookahead_sequence);

    hz_stream_read16(stream, &rule->seq_lookup_count);
    rule->seq_lookup_records = HZ_MALLOC(sizeof(hz_sequence_lookup_record_t)
            * rule->seq_lookup_count);

    for (i = 0; i < rule->seq_lookup_count; ++i) {
        hz_sequence_lookup_record_t *record = &rule->seq_lookup_records[i];
        hz_ot_read_sequence_lookup_record(stream, record);
    }

    hz_stream_destroy(stream);
}

void
hz_ot_parse_chained_sequence_rule_set(hz_byte *data, hz_chained_sequence_rule_set_t *rule_set) {
    size_t                      i;
    hz_offset16_t               *offsets;
    hz_stream_t                 *stream;

    stream = hz_stream_create(data, 0, 0);

    hz_stream_read16(stream, &rule_set->count);
    offsets = HZ_MALLOC(sizeof(uint16_t) * rule_set->count);

    rule_set->rules = HZ_MALLOC(sizeof(hz_chained_sequence_rule_t)
            * rule_set->count);

    for (i = 0; i < rule_set->count; ++i) {
        hz_chained_sequence_rule_t *rule = &rule_set->rules[i];
        hz_ot_parse_chained_sequence_rule(stream->data + offsets[i], rule);
    }

    HZ_FREE(offsets);
    hz_stream_destroy(stream);
}


typedef struct hz_chained_sequence_context_format3_t {
    uint16_t prefix_count;
    hz_map_t **prefix_maps; /* prefix coverages */
    uint16_t input_count;
    hz_map_t **input_maps; /* input coverages */
    uint16_t suffix_count;
    hz_map_t **suffix_maps; /* suffix coverages */
    uint16_t lookup_count;
    hz_sequence_lookup_record_t *lookup_records;
} hz_chained_sequence_context_format3_t;

void
hz_ot_read_chained_sequence_context_format3_table(hz_stream_t *stream,
                                                  hz_chained_sequence_context_format3_t *table)
{
    hz_byte                    buffer[1024];
    hz_bump_allocator_t          allocator;
    hz_offset16_t               *backtrack_offsets;
    hz_offset16_t               *input_offsets;
    hz_offset16_t               *lookahead_offsets;

    hz_bump_allocator_init(&allocator, buffer, sizeof buffer);

    {
        /* read backtrack glyph offsets */
        size_t i, n;
        hz_stream_read16(stream, &table->prefix_count);
        n = table->prefix_count;
        backtrack_offsets = hz_bump_allocator_alloc(&allocator, n * sizeof(hz_offset16_t));
        hz_stream_read16_n(stream, n, backtrack_offsets);

        table->prefix_maps = HZ_MALLOC(sizeof(hz_map_t *) * n);

        for (i=0; i<n; ++i) {
            table->prefix_maps[i] = hz_map_create();
            hz_decode_coverage(stream->data + backtrack_offsets[i],
                               table->prefix_maps[i],
                               NULL);
        }
    }

    {
        /* read input glyph offsets */
        size_t i, n;
        hz_stream_read16(stream, &table->input_count);
        n = table->input_count;
        input_offsets = hz_bump_allocator_alloc(&allocator, n * sizeof(hz_offset16_t));
        hz_stream_read16_n(stream, n, input_offsets);

        table->input_maps = HZ_MALLOC(sizeof(hz_map_t *) * n);

        for (i=0; i<n; ++i) {
            table->input_maps[i] = hz_map_create();
            hz_decode_coverage(stream->data + input_offsets[i],
                               table->input_maps[i],
                               NULL);
        }
    }

    {
        /* read lookahead glyph offsets */
        size_t i, n;
        hz_stream_read16(stream, &table->suffix_count);
        n = table->suffix_count;
        lookahead_offsets = hz_bump_allocator_alloc(&allocator, n * sizeof(hz_offset16_t));
        hz_stream_read16_n(stream, n, lookahead_offsets);

        table->suffix_maps = HZ_MALLOC(sizeof(hz_map_t *) * n);

        for (i=0; i<n; ++i) {
            table->suffix_maps[i] = hz_map_create();
            hz_decode_coverage(stream->data + lookahead_offsets[i],
                               table->suffix_maps[i],
                               NULL);
        }
    }

    {
        /* read lookup records */
        size_t i, n;
        hz_stream_read16(stream, &table->lookup_count);
        n = table->lookup_count;
        table->lookup_records = HZ_MALLOC(sizeof(hz_sequence_lookup_record_t) * n);

        for (i=0;i<n;++i) {
            hz_sequence_lookup_record_t *record = table->lookup_records + i;
            hz_ot_read_sequence_lookup_record(stream, record);
        }
    }

    hz_bump_allocator_free(&allocator, backtrack_offsets);
    hz_bump_allocator_free(&allocator, input_offsets);
    hz_bump_allocator_free(&allocator, lookahead_offsets);
    hz_bump_allocator_release(&allocator);
}

void
hz_ot_clear_chained_sequence_context_format3_table(hz_chained_sequence_context_format3_t *table) {
    size_t i;

    for (i=0; i<table->prefix_count; ++i) {
        hz_map_destroy(table->prefix_maps[i]);
    }

    for (i=0; i<table->input_count; ++i) {
        hz_map_destroy(table->input_maps[i]);
    }

    for (i=0; i<table->suffix_count; ++i) {
        hz_map_destroy(table->suffix_maps[i]);
    }

    HZ_FREE(table->prefix_maps);
    HZ_FREE(table->input_maps);
    HZ_FREE(table->suffix_maps);
    HZ_FREE(table->lookup_records);
}

void
hz_ot_layout_apply_gsub_lookup_to_glyph(hz_sequence_node_t *g) {

}

typedef struct hz_sequence_table_t {
    uint16_t glyph_count;
    uint16_t *glyphs;
} hz_sequence_table_t;

void
hz_ot_read_sequence_table(hz_byte *data, hz_sequence_table_t *table) {
    hz_stream_t *stream = hz_stream_create(data,0,0);
    hz_stream_read16(stream, &table->glyph_count);
    table->glyphs = HZ_MALLOC(sizeof(uint16_t) * table->glyph_count);
    hz_stream_read16_n(stream, table->glyph_count, table->glyphs);
    hz_stream_destroy(stream);
}

void
hz_ot_clear_sequence_table(hz_sequence_table_t *table) {
    HZ_FREE(table->glyphs);
}

typedef struct hz_multiple_subst_format1_t {
    hz_map_t *coverage;
    uint16_t sequence_count;
    hz_sequence_table_t *sequences;
} hz_multiple_subst_format1_t;

void
hz_ot_read_multiple_subst_format1(hz_stream_t *stream,
                                  hz_multiple_subst_format1_t *table)
{
    hz_byte           buffer[8192];
    hz_bump_allocator_t allocator;
    hz_offset16_t       coverage_offset;
    hz_offset16_t       *sequence_offsets;
    size_t              i;

    hz_bump_allocator_init(&allocator, buffer, sizeof buffer);

    /* read coverage table */
    hz_stream_read16(stream, &coverage_offset);
    table->coverage = hz_map_create();
    hz_decode_coverage(stream->data + coverage_offset, table->coverage, NULL);

    /* read and decode sequences */
    hz_stream_read16(stream, &table->sequence_count);
    sequence_offsets = hz_bump_allocator_alloc(&allocator, sizeof(uint16_t) * table->sequence_count);
    table->sequences = HZ_MALLOC(sizeof(hz_sequence_table_t) * table->sequence_count);

    hz_stream_read16_n(stream, table->sequence_count, sequence_offsets);

    for (i = 0; i < table->sequence_count; ++i) {
        hz_ot_read_sequence_table(stream->data + sequence_offsets[i], table->sequences + i);
    }

    /* release memory resources */
    hz_bump_allocator_free(&allocator, sequence_offsets);
    hz_bump_allocator_release(&allocator);
}

void
hz_ot_clear_multiple_subst_format1(hz_multiple_subst_format1_t *table) {
    size_t i;
    hz_map_destroy(table->coverage);

    for (i = 0; i < table->sequence_count; ++i) {
        hz_ot_clear_sequence_table(table->sequences + i);
    }

    HZ_FREE(table->sequences);
}

typedef struct hz_reverse_chain_single_subst_format1_t {
    hz_map_t *coverage;
    uint16_t prefix_count;
    hz_map_t **prefix_maps; /* prefix coverages */
    uint16_t suffix_count;
    hz_map_t **suffix_maps; /* suffix coverages */
    uint16_t glyph_count;
    uint16_t *glyphs;
} hz_reverse_chain_single_subst_format1_t;

static void
hz_read_reverse_chain_single_subst_format1(hz_stream_t *stream,
        hz_reverse_chain_single_subst_format1_t *subst)
{
    hz_byte             buffer[4096];
    hz_bump_allocator_t allocator;
    hz_offset16_t       coverage_offset;
    hz_offset16_t       *prefix_coverage_offsets;
    hz_offset16_t       *suffix_coverage_offsets;
    uint16_t            i;

    hz_bump_allocator_init(&allocator, buffer, sizeof buffer);

    hz_stream_read16(stream, &coverage_offset);
    subst->coverage = hz_map_create();
    hz_decode_coverage(stream->data + coverage_offset, subst->coverage, NULL);

    hz_stream_read16(stream, &subst->prefix_count);
    prefix_coverage_offsets = hz_bump_allocator_alloc(&allocator, subst->prefix_count * sizeof(uint16_t));
    hz_stream_read16_n(stream, subst->prefix_count, prefix_coverage_offsets);
    subst->prefix_maps = HZ_MALLOC(sizeof(hz_map_t *) * subst->prefix_count);
    for (i = 0; i < subst->prefix_count; ++i) {
        subst->prefix_maps[i] = hz_map_create();
        hz_decode_coverage(stream->data + prefix_coverage_offsets[i],
                           subst->prefix_maps[i],
                           NULL);
    }

    hz_stream_read16(stream, &subst->suffix_count);
    suffix_coverage_offsets = hz_bump_allocator_alloc(&allocator, subst->suffix_count * sizeof(uint16_t));
    hz_stream_read16_n(stream, subst->suffix_count, suffix_coverage_offsets);
    subst->suffix_maps = HZ_MALLOC(sizeof(hz_map_t *) * subst->suffix_count);
    for (i = 0; i < subst->suffix_count; ++i) {
        subst->suffix_maps[i] = hz_map_create();
        hz_decode_coverage(stream->data + suffix_coverage_offsets[i],
                           subst->suffix_maps[i],
                           NULL);
    }

    hz_stream_read16(stream, &subst->glyph_count);
    subst->glyphs = HZ_MALLOC(subst->glyph_count * sizeof(uint16_t));
    hz_stream_read16_n(stream, subst->glyph_count, subst->glyphs);

    hz_bump_allocator_free(&allocator, prefix_coverage_offsets);
    hz_bump_allocator_free(&allocator, suffix_coverage_offsets);
    hz_bump_allocator_release(&allocator);
}

void
hz_free_reverse_chain_single_subst_format1(hz_reverse_chain_single_subst_format1_t *subst)
{
    size_t i;
    hz_map_destroy(subst->coverage);

    for(i=0; i<subst->prefix_count; ++i) {
        hz_map_destroy(subst->prefix_maps[i]);
    }

    for(i=0; i<subst->suffix_count; ++i) {
        hz_map_destroy(subst->suffix_maps[i]);
    }

    HZ_FREE(subst->prefix_maps);
    HZ_FREE(subst->suffix_maps);
    HZ_FREE(subst->glyphs);
}

hz_bool
hz_ot_layout_pattern_match(hz_sequence_node_t *node,
                           hz_map_t **maps,
                           uint16_t map_count,
                           uint16_t ignore_flags,
                           hz_sequence_node_cache_t *cache,
                           hz_bool reverse)
{
    size_t i;
    hz_bool does_match = HZ_TRUE;

    for (i = 0; i < map_count; ++i) {

        if (node == NULL) {
            does_match = HZ_FALSE;
            break;
        }

        if (!hz_map_value_exists(maps[i], node->id)) {
            does_match = HZ_FALSE;
            break;
        }

        if (cache != NULL)
            hz_sequence_node_cache_add(cache, node);


        if (reverse) {
            node = hz_prev_node_not_of_class(node,
                                             ignore_flags,
                                             NULL);
        } else {
            node = hz_next_node_not_of_class(node,
                                             ignore_flags,
                                             NULL);
        }
    }

    return does_match;
}

void
hz_ot_layout_apply_gsub_subtable(hz_face_t *face,
                                 hz_stream_t *subtable,
                                 uint16_t lookup_type,
                                 uint16_t lookup_flags,
                                 hz_feature_t feature,
                                 hz_sequence_t *sequence,
                                 hz_sequence_node_t *nested)
{
    hz_glyph_class_t ignore_flags = hz_ignored_classes_from_lookup_flags(lookup_flags);
    uint16_t format;
    hz_stream_read16(subtable, &format);

//    if ((feature == HZ_FEATURE_MEDI || feature == HZ_FEATURE_INIT || feature == HZ_FEATURE_FINA
//         || feature == HZ_FEATURE_ISOL)
//        && lookup_type != 7) {
//        printf("%d.%d\n", lookup_type, format);
//    }

//    if (feature == HZ_FEATURE_RLIG && lookup_type != 7) {
//        printf("%d.%d\n", lookup_type, format);
//
//    }

    switch (lookup_type) {
        case HZ_GSUB_LOOKUP_TYPE_SINGLE_SUBSTITUTION: {
            if (format == 1) {
                hz_map_t *coverage = hz_map_create();
                hz_offset16_t coverage_offset;
                int16_t id_delta;
                hz_stream_read16(subtable, &coverage_offset);
                hz_stream_read16(subtable, (uint16_t *) &id_delta);
                hz_sequence_node_t *g;

                hz_decode_coverage(subtable->data + coverage_offset, coverage, NULL);

                if (nested) {
                    if (hz_map_value_exists(coverage, nested->id)) {
                        nested->id += id_delta;
                    }
                } else {
                    for (g=sequence->root; g != NULL; g = hz_next_node_not_of_class(g, ignore_flags, NULL)) {
                        if (hz_map_value_exists(coverage, g->id)) {
                            switch (feature) {
                                case HZ_FEATURE_ISOL:
                                case HZ_FEATURE_MEDI:
                                case HZ_FEATURE_MED2:
                                case HZ_FEATURE_INIT:
                                case HZ_FEATURE_FINA:
                                case HZ_FEATURE_FIN2:
                                case HZ_FEATURE_FIN3:
                                    if (hz_ot_shape_complex_arabic_join(feature, g)) {
                                        g->id += id_delta;
                                    }
                                    break;
                                default:
                                    g->id += id_delta;
                                    break;
                            }
                        }
                    }
                }

                hz_map_destroy(coverage);
            } else if (format == 2) {
                hz_map_t *map_subst = hz_map_create();
                hz_offset16_t coverage_offset;
                uint16_t glyph_count;
                hz_array_t *subst = hz_array_create();
                hz_stream_read16(subtable, &coverage_offset);
                hz_stream_read16(subtable, &glyph_count);

                /* Get destination glyph indices */
                uint16_t dst_gidx;
                for (dst_gidx = 0; dst_gidx < glyph_count; ++dst_gidx) {
                    hz_index_t substitute_glyph;
                    hz_stream_read16(subtable, &substitute_glyph);
                    hz_array_push_back(subst, substitute_glyph);
                }

                /* Read coverage offset */
                hz_decode_coverage(subtable->data + coverage_offset, map_subst, subst);

                /* Substitute glyphs */
                hz_sequence_node_t *g;

                if (nested) {
                    if (hz_map_value_exists(map_subst, nested->id)) {
                        nested->id = hz_map_get_value(map_subst, nested->id);
                    }
                } else {
                    for(g = sequence->root; g!=NULL; g=hz_next_node_not_of_class(g, ignore_flags, NULL)) {
                        if (hz_map_value_exists(map_subst, g->id)) {
                            switch (feature) {
                                case HZ_FEATURE_ISOL:
                                case HZ_FEATURE_MEDI:
                                case HZ_FEATURE_MED2:
                                case HZ_FEATURE_INIT:
                                case HZ_FEATURE_FINA:
                                case HZ_FEATURE_FIN2:
                                case HZ_FEATURE_FIN3:
                                    if (hz_ot_shape_complex_arabic_join(feature, g)) {
                                        g->id = hz_map_get_value(map_subst, g->id);
                                    }
                                    break;
                                default:
                                    g->id = hz_map_get_value(map_subst, g->id);
                                    break;
                            }
                        }
                    }
                }

                hz_array_destroy(subst);
                hz_map_destroy(map_subst);
            } else {
                /* error */
            }

            break;
        }

        case HZ_GSUB_LOOKUP_TYPE_MULTIPLE_SUBSTITUTION: {
            if (format == 1) {
                hz_multiple_subst_format1_t table_cache;
                hz_ot_read_multiple_subst_format1(subtable, &table_cache);

                hz_sequence_node_t *g, *h;

                if (nested) {
                    if (hz_map_value_exists(table_cache.coverage, nested->id)) {
                        /* glyph is covered, replace with appropriate sequence */
                        uint16_t i, j;
                        i = hz_map_get_value(table_cache.coverage, nested->id);
                        hz_sequence_table_t *sequence_table = &table_cache.sequences[i];
                        hz_sequence_node_t *t = nested;
                        for (j=0; j<sequence_table->glyph_count; ++j) {
                            hz_sequence_node_t *node = HZ_ALLOC(hz_sequence_node_t);
                            node->id = sequence_table->glyphs[j];
                            node->prev = NULL;
                            node->next = NULL;
                            node->gc = HZ_GLYPH_CLASS_ZERO;//t->gc;
                            node->codepoint = 0;//t->codepoint;
                            hz_sequence_insert_node(t, node);
                            t = t->next;
                        }

                        hz_sequence_pop_node(sequence, nested);
                    }
                } else {
                    g = hz_sequence_last_node(sequence->root);
                    while (g!=NULL) {
                        if (hz_map_value_exists(table_cache.coverage, g->id)) {
                            /* glyph is covered, replace with appropriate sequence */
                            uint16_t i, j;
                            i = hz_map_get_value(table_cache.coverage, g->id);
                            hz_sequence_table_t *sequence_table = &table_cache.sequences[i];

                            if ((feature == HZ_FEATURE_MEDI || feature == HZ_FEATURE_INIT || feature == HZ_FEATURE_FINA
                                 || feature == HZ_FEATURE_ISOL)) {
                                if (hz_ot_shape_complex_arabic_join(feature, g)) {
                                    hz_sequence_node_t *t = g;
                                    for (j=0; j<sequence_table->glyph_count; ++j) {
                                        hz_sequence_node_t *node = HZ_ALLOC(hz_sequence_node_t);
                                        node->id = sequence_table->glyphs[j];
                                        node->prev = NULL;
                                        node->next = NULL;
                                        node->gc = t->gc;
                                        node->codepoint = t->codepoint;
                                        hz_sequence_insert_node(t, node);
                                        t = t->next;
                                    }

                                    hz_sequence_node_t *tmp_node = g;
                                    g = hz_prev_node_not_of_class(g, ignore_flags, NULL);
                                    hz_sequence_pop_node(sequence, tmp_node);
                                    continue;
                                }
                            } else {
                                hz_sequence_node_t *t = g;
                                for (j=0; j<sequence_table->glyph_count; ++j) {
                                    hz_sequence_node_t *node = HZ_ALLOC(hz_sequence_node_t);
                                    node->id = sequence_table->glyphs[j];
                                    node->prev = NULL;
                                    node->next = NULL;
                                    node->gc = HZ_GLYPH_CLASS_ZERO;//t->gc;
                                    node->codepoint = 0;//t->codepoint;
                                    hz_sequence_insert_node(t, node);
                                    t = t->next;
                                }

                                hz_sequence_node_t *tmp_node = g;
                                g = hz_prev_node_not_of_class(g, ignore_flags, NULL);
                                hz_sequence_pop_node(sequence, tmp_node);
                                continue;
                            }
                        }

                        g = hz_prev_node_not_of_class(g, ignore_flags, NULL);
                    }
                }

                hz_ot_clear_multiple_subst_format1(&table_cache);
            } else {
                /* error */
            }
            break;
        }

        case HZ_GSUB_LOOKUP_TYPE_ALTERNATE_SUBSTITUTION: {
            break;
        }

        case HZ_GSUB_LOOKUP_TYPE_LIGATURE_SUBSTITUTION: {
            if (format == 1) {
                hz_offset16_t coverage_offset;
                uint16_t ligature_set_count;
                hz_offset16_t *ligature_set_offsets;
                hz_map_t *coverage_map = hz_map_create();

                hz_stream_read16(subtable, &coverage_offset);
                hz_stream_read16(subtable, &ligature_set_count);
                ligature_set_offsets = HZ_MALLOC(ligature_set_count * sizeof(uint16_t));
                hz_stream_read16_n(subtable, ligature_set_count, ligature_set_offsets);
                hz_decode_coverage(subtable->data + coverage_offset, coverage_map, NULL);


                /* loop over every non-ignored glyph in the section */
                hz_sequence_node_t *g;
                for (g = sequence->root; g != NULL; g = hz_next_node_not_of_class(g, ignore_flags, NULL)) {
                    if (hz_map_value_exists(coverage_map, g->id)) {
                        /* current glyph is covered, check pattern and replace */
                        hz_offset16_t ligature_set_offset = ligature_set_offsets[ hz_map_get_value(coverage_map, g->id) ];
                        uint16_t ligature_count;
                        hz_stream_t *ligature_set = hz_stream_create(subtable->data + ligature_set_offset, 0, 0);
                        hz_stream_read16(ligature_set, &ligature_count);
                        hz_ligature_t **ligatures = HZ_MALLOC(sizeof(hz_ligature_t*) * ligature_count);
                        uint16_t ligature_index = 0;
                        for (ligature_index = 0; ligature_index < ligature_count; ++ligature_index) {
                            hz_offset16_t ligature_offset;
                            hz_stream_read16(ligature_set, &ligature_offset);

                            ligatures[ligature_index] = hz_ligature_create();
                            hz_ligature_decode(ligatures[ligature_index], ligature_set->data + ligature_offset);
                        }

//                        hz_ot_layout_apply_fit_ligature(ligatures, ligature_count, ignore_flags, g);

                        for (ligature_index = 0; ligature_index < ligature_count; ++ligature_index) {
                            hz_ligature_destroy(ligatures[ligature_index]);
                        }

                        HZ_FREE(ligatures);
                        hz_stream_destroy(ligature_set);
                    }
                }

                HZ_FREE(ligature_set_offsets);
                hz_map_destroy(coverage_map);
            } else {
                /* error */
            }

            break;
        }

        case HZ_GSUB_LOOKUP_TYPE_CONTEXTUAL_SUBSTITUTION: {
            if (format == 1) {
                hz_sequence_node_t *g;
                hz_map_t *coverage;
                hz_offset16_t coverage_offset;
                uint16_t seq_rule_set_count;
                hz_offset16_t *seq_rule_set_offsets;

                /* remaining glyphs in sequence, useful in knowing
                * when to stop looking at rule sequences if they are too long.
                * */
                size_t nrem, i;
                hz_sequence_rule_set_t *seq_rule_set_array;

                hz_stream_read16(subtable, &coverage_offset);
                hz_stream_read16(subtable, &seq_rule_set_count);

                seq_rule_set_offsets = HZ_MALLOC(sizeof(hz_offset16_t) * seq_rule_set_count);
                seq_rule_set_array = HZ_MALLOC(sizeof(hz_sequence_rule_set_t) * seq_rule_set_count);
                hz_stream_read16_n(subtable, seq_rule_set_count, seq_rule_set_offsets);

                /* read rule set array */
                for (i = 0; i < seq_rule_set_count; ++i) {
                    uint16_t rule_set_offset = seq_rule_set_offsets[i];
                    hz_ot_parse_sequence_rule_set(subtable->data + rule_set_offset,
                                                  &seq_rule_set_array[i]);
                }

                /* parse coverage map */
                coverage = hz_map_create();
                hz_decode_coverage(subtable->data + coverage_offset,
                                   coverage, NULL);

                for (g = sequence->root, nrem = hz_sequence_node_count(g);
                     g != NULL; g = hz_next_node_not_of_class(g, ignore_flags, NULL), --nrem) {
                    if (hz_map_value_exists(coverage, g->id)) {
                        /* get rule set for current glyph index */
                        uint16_t rule_set_index = hz_map_get_value(coverage, g->id);
                        hz_sequence_rule_set_t *rule_set = seq_rule_set_array + rule_set_index;
                        hz_index_t start_id = g->id;
                        uint16_t rule_index;

                        /* go through all rule sets, find rule set sequence which is shorter than
                         * remaining glyph count in input sequence
                         * */
                        for (rule_index = 0; rule_index < rule_set->rule_count; ++rule_index) {
                            hz_sequence_rule_t *rule = &rule_set->rules[rule_index];

                            if (rule->glyph_count + 1 <= nrem) {
                                /* rule is shorter than remaining glyph count, compare with
                                 * original sequence
                                 * */


                            }
                        }

                    }
                }

                HZ_FREE(seq_rule_set_array);
                HZ_FREE(seq_rule_set_offsets);
                hz_map_destroy(coverage);
            } else if (format == 2) {

            }


            break;
        }

        case HZ_GSUB_LOOKUP_TYPE_CHAINED_CONTEXTS_SUBSTITUTION: {
            if (format == 1) {
                hz_byte                       buffer[4096];
                hz_bump_allocator_t             allocator;
                hz_offset16_t                   coverage_offset;
                uint16_t                        chained_seq_rule_set_count;
                hz_offset16_t                   *chained_seq_rule_set_offsets;
                hz_map_t                        *coverage;
                hz_sequence_node_t              *p, *q;
                hz_chained_sequence_rule_set_t  *chained_seq_rule_sets;
                size_t                          i;

                hz_bump_allocator_init(&allocator, buffer, sizeof(buffer));

                hz_stream_read16(subtable, &coverage_offset);
                hz_stream_read16(subtable, &chained_seq_rule_set_count);

                chained_seq_rule_set_offsets = hz_bump_allocator_alloc(&allocator,
                                                                       sizeof(hz_offset16_t) *
                                                                       chained_seq_rule_set_count);

                chained_seq_rule_sets = hz_bump_allocator_alloc(&allocator,
                                                                sizeof(hz_chained_sequence_rule_set_t) *
                                                                chained_seq_rule_set_count);

                hz_stream_read16_n(subtable, chained_seq_rule_set_count, chained_seq_rule_set_offsets);

                for (i = 0; i < chained_seq_rule_set_count; ++i) {
                    hz_offset16_t rule_set_offset = chained_seq_rule_set_offsets[i];
                    hz_chained_sequence_rule_set_t *rule_set = chained_seq_rule_sets + i;
//                        hz_ot_parse_chained_sequence_rule_set(subtable->data + rule_set_offset, rule_set);
                }

                coverage = hz_map_create();
                hz_decode_coverage(subtable->data + coverage_offset, coverage, NULL);

                for (p = sequence->root; p != NULL;
                     p = hz_next_node_not_of_class(p, ignore_flags, NULL)) {
                    if (hz_map_value_exists(coverage, p->id)) {
                        uint16_t rule_index = hz_map_get_value(coverage, p->id);

                    }
                }

                hz_map_destroy(coverage);
                hz_bump_allocator_free(&allocator, chained_seq_rule_set_offsets);
                hz_bump_allocator_release(&allocator);
            } else if (format == 2) {

            } else if (format == 3) {
                hz_chained_sequence_context_format3_t table_cache;
                hz_sequence_node_t *g;
                hz_ot_read_chained_sequence_context_format3_table(subtable, &table_cache);

                for (g = sequence->root; g != NULL;
                g = hz_next_node_not_of_class(g, ignore_flags, NULL)) {

                    hz_sequence_node_cache_t *i_seq = hz_sequence_node_cache_create();

                    if (hz_ot_layout_pattern_match(g,
                                                   table_cache.input_maps,
                                                   table_cache.input_count,
                                                   ignore_flags,
                                                   i_seq,
                                                   HZ_FALSE)) {

                        hz_sequence_node_t *n1, *n2;
                        n1 = hz_prev_node_not_of_class(i_seq->nodes[0], ignore_flags, NULL);
                        n2 = hz_next_node_not_of_class(i_seq->nodes[i_seq->node_count-1],ignore_flags,NULL);

                        hz_bool prefix_match = hz_ot_layout_pattern_match(n1,
                                                        table_cache.prefix_maps,
                                                        table_cache.prefix_count,
                                                        ignore_flags,
                                                        NULL,
                                                        HZ_TRUE);

                        hz_bool suffix_match = hz_ot_layout_pattern_match(n2,
                                                   table_cache.suffix_maps,
                                                   table_cache.suffix_count,
                                                   ignore_flags,
                                                   NULL,
                                                   HZ_FALSE);

                        if (prefix_match && suffix_match) {
                            size_t k;

                            /* apply lookups to input, within context */
                            for (k=0; k<table_cache.lookup_count; ++k) {
                                hz_sequence_lookup_record_t *record = table_cache.lookup_records + k;
                                hz_sequence_node_t *new_nested = i_seq->nodes[record->sequence_index];
                                hz_ot_layout_apply_lookup(face,
                                                          feature,
                                                          HZ_OT_TAG_GSUB,
                                                          sequence,
                                                          new_nested,
                                                          record->lookup_list_index);
                            }

                            g = i_seq->nodes[i_seq->node_count-1];
                        }

                    }

                    hz_sequence_node_cache_destroy(i_seq);
                }

                hz_ot_clear_chained_sequence_context_format3_table(&table_cache);

            } else {
                /* error */
            }

            break;
        }

        case HZ_GSUB_LOOKUP_TYPE_EXTENSION_SUBSTITUTION: {
            if (format == 1) {
                uint16_t extension_lookup_type;
                hz_offset32_t extension_offset;

                hz_stream_read16(subtable, &extension_lookup_type);
                hz_stream_read32(subtable, &extension_offset);

                HZ_ASSERT(extension_lookup_type < 7);

                subtable->data += extension_offset;
                subtable->offset = 0;

                hz_ot_layout_apply_gsub_subtable(face, subtable, extension_lookup_type, lookup_flags,
                                                 feature, sequence, nested);
            } else {
                /* error */
            }

            break;
        }

        case HZ_GSUB_LOOKUP_TYPE_REVERSE_CHAINING_CONTEXTUAL_SINGLE_SUBSTITUTION: {
            if (format == 1) {
                hz_reverse_chain_single_subst_format1_t subst;
                hz_read_reverse_chain_single_subst_format1(subtable, &subst);
                hz_sequence_node_t *g;

                /* loop in reverse order, check if glyph found in coverage */
                for (g = hz_sequence_last_node(sequence->root);
                g!=NULL;
                g=hz_prev_node_not_of_class(g,ignore_flags,NULL)) {
                    if (hz_map_value_exists(subst.coverage,g->id)) {
                        /* glyph covered, check prefix & suffix context */

                        hz_bool prefix_match = hz_ot_layout_pattern_match(
                                hz_prev_node_not_of_class(g, ignore_flags, NULL),
                                subst.prefix_maps,
                                subst.prefix_count,
                                ignore_flags,
                                NULL,
                                HZ_TRUE);

                        hz_bool suffix_match = hz_ot_layout_pattern_match(
                                hz_next_node_not_of_class(g, ignore_flags, NULL),
                                subst.suffix_maps,
                                subst.suffix_count,
                                ignore_flags,
                                NULL,
                                HZ_FALSE);

                        if ((prefix_match || !subst.prefix_count) && (suffix_match || !subst.suffix_count)) {
                            size_t k;
                            hz_sequence_node_t *t = g;

                            /* replace glyph with substitutes */
                            for (k=0; k<subst.glyph_count; ++k) {
                                hz_sequence_node_t *node = HZ_ALLOC(hz_sequence_node_t);
                                node->id = subst.glyphs[k];
                                node->prev = NULL;
                                node->next = NULL;
                                node->gc = t->gc;
                                node->codepoint = t->codepoint;
                                hz_sequence_insert_node(t, node);
                                t = t->next;
                            }
                        }
                    }
                }

                hz_free_reverse_chain_single_subst_format1(&subst);
            } else {
                /* error */
            }
            break;
        }

        default:
            HZ_LOG("Invalid GSUB lookup type!\n");
            break;
    }

}

void
hz_ot_layout_apply_gsub_feature(hz_face_t *face,
                                hz_stream_t *table,
                                hz_feature_t feature,
                                hz_sequence_t *sequence)
{
    uint16_t lookup_type;
    uint16_t lookup_flags;
    uint16_t subtable_count;
    hz_stream_read16(table, &lookup_type);
    hz_stream_read16(table, &lookup_flags);
    hz_stream_read16(table, &subtable_count);

    hz_glyph_class_t class_ignore = hz_ignored_classes_from_lookup_flags(lookup_flags);

    /* NOTE: cache sequence nodes which have valid unignored glyph classes as an optimization, so will not use functions
     that have to go through ignored glyphs over and over.
     */

    uint16_t subtable_index = 0;
    while (subtable_index < subtable_count) {
        hz_offset16_t offset;
        hz_stream_read16(table, &offset);
        hz_stream_t *subtable = hz_stream_create(table->data + offset, 0, 0);
        hz_ot_layout_apply_gsub_subtable(face, subtable, lookup_type, lookup_flags, feature, sequence, NULL);
        hz_stream_destroy(subtable);
        ++subtable_index;
    }
}

typedef struct hz_entry_exit_record_t {
    hz_offset16_t entry_anchor_offset, exit_anchor_offset;
} hz_entry_exit_record_t;

typedef struct hz_anchor_t {
    int16_t x_coord, y_coord;
} hz_anchor_t;

typedef struct hz_anchor_pair_t {
    hz_bool has_entry, has_exit;
    hz_anchor_t entry, exit;
} hz_anchor_pair_t;


hz_anchor_t
hz_ot_layout_read_anchor(const uint8_t *data) {
    hz_stream_t *stream = hz_stream_create(data, 0,0);
    hz_anchor_t anchor;

    uint16_t format;
    hz_stream_read16(stream, &format);

    HZ_ASSERT(format >= 1 && format <= 3);
    hz_stream_read16(stream, (uint16_t *) &anchor.x_coord);
    hz_stream_read16(stream, (uint16_t *) &anchor.y_coord);

    hz_stream_destroy(stream);
    return anchor;
}

hz_anchor_pair_t
hz_ot_layout_read_anchor_pair(const uint8_t *subtable, const hz_entry_exit_record_t *rec) {
    hz_anchor_pair_t anchor_pair;

    anchor_pair.has_entry = rec->entry_anchor_offset ? HZ_TRUE : HZ_FALSE;
    anchor_pair.has_exit = rec->exit_anchor_offset ? HZ_TRUE : HZ_FALSE;

    if (anchor_pair.has_entry)
        anchor_pair.entry = hz_ot_layout_read_anchor(subtable + rec->entry_anchor_offset);

    if (anchor_pair.has_exit)
        anchor_pair.exit = hz_ot_layout_read_anchor(subtable + rec->exit_anchor_offset);

    return anchor_pair;
}

typedef struct hz_mark_record_t {
    uint16_t mark_class;
    hz_offset16_t mark_anchor_offset;
} hz_mark_record_t;

typedef struct hz_mark2_record_t {
    uint16_t mark_class;
    hz_offset16_t mark2_anchor_offets;
} hz_mark2_record_t;

hz_sequence_node_t *
hz_ot_layout_find_prev_with_class(hz_sequence_node_t *node, hz_glyph_class_t gc)
{
    node = node->prev;
    while (node != NULL) {
        if (node->gc == gc) {
            /* found node with required class */
            break;
        }

        node = node->prev;
    }

    return node;
}

hz_sequence_node_t *
hz_ot_layout_find_next_with_class(hz_sequence_node_t *node, hz_glyph_class_t gc)
{
    node = node->next;
    while (node != NULL) {
        if (node->gc == gc) {
            /* found node with required class */
            break;
        }

        node = node->next;
    }

    return node;
}

void
hz_ot_layout_apply_gpos_subtable(hz_face_t *face,
                                 hz_stream_t *subtable,
                                 uint16_t lookup_type,
                                 uint16_t lookup_flags,
                                 hz_feature_t feature,
                                 hz_sequence_t *sequence)
{
    uint16_t class_ignore = hz_ignored_classes_from_lookup_flags(lookup_flags);
    uint16_t format;
    hz_stream_read16(subtable, &format);

//    if (lookup_type != 9 && lookup_type != 8) {
//        printf("%d.%d\n", lookup_type, format);
//    }

    switch (lookup_type) {
        case HZ_GPOS_LOOKUP_TYPE_SINGLE_ADJUSTMENT: {
            break;
        }
        case HZ_GPOS_LOOKUP_TYPE_PAIR_ADJUSTMENT: {
            break;
        }
        case HZ_GPOS_LOOKUP_TYPE_CURSIVE_ATTACHMENT: {
            if (format == 1) {
                /* 4k stack buffer */
                hz_byte           buffer[4096];
                hz_bump_allocator_t allocator;
                hz_bump_allocator_init(&allocator, buffer, sizeof buffer);

                hz_offset16_t coverage_offset;
                uint16_t record_count, record_index = 0;
                hz_entry_exit_record_t *records;
                hz_map_t *coverage_map = hz_map_create();

                hz_stream_read16(subtable, &coverage_offset);
                hz_stream_read16(subtable, &record_count);

                records = hz_bump_allocator_alloc(&allocator, sizeof(hz_entry_exit_record_t) * record_count);

                while (record_index < record_count) {
                    hz_entry_exit_record_t *rec = &records[record_index];
                    hz_stream_read16(subtable, &rec->entry_anchor_offset);
                    hz_stream_read16(subtable, &rec->exit_anchor_offset);
                    ++record_index;
                }

                /* get coverage glyph to index map */
                hz_decode_coverage(subtable->data + coverage_offset, coverage_map, NULL);


                /* position glyphs */
                hz_sequence_node_t *g;

                for (g = sequence->root;
                    g != NULL;
                    g = hz_next_node_not_of_class(g, class_ignore, NULL))
                {
                    if (hz_map_value_exists(coverage_map, g->id)) {
                        uint16_t curr_idx = hz_map_get_value(coverage_map, g->id);
                        const hz_entry_exit_record_t *curr_rec = records + curr_idx;
                        hz_anchor_pair_t curr_pair = hz_ot_layout_read_anchor_pair(subtable->data, curr_rec);

                        if (curr_pair.has_exit && g->next != NULL) {
                            uint16_t next_idx = hz_map_get_value(coverage_map, g->next->id);
                            const hz_entry_exit_record_t *next_rec = records + next_idx;
                            hz_anchor_pair_t next_pair = hz_ot_layout_read_anchor_pair(subtable->data, next_rec);

                            int16_t y_delta = next_pair.entry.y_coord - curr_pair.exit.y_coord;
                            int16_t x_delta = next_pair.entry.x_coord - curr_pair.exit.x_coord;

                            /* TODO: implement */
                        }
                    }
                }

                /* release resources */
                hz_bump_allocator_free(&allocator, records);
                hz_bump_allocator_release(&allocator);
                hz_map_destroy(coverage_map);
            } else {
                /* error */
            }

            break;
        }
        case HZ_GPOS_LOOKUP_TYPE_MARK_TO_BASE_ATTACHMENT: {
            /* attach mark to base glyph point */
            if (format == 1) {
                hz_byte           buffer[4096];
                hz_bump_allocator_t allocator;
                hz_bump_allocator_init(&allocator, buffer, sizeof buffer);

                hz_offset16_t mark_coverage_offset;
                hz_offset16_t base_coverage_offset;
                uint16_t mark_class_count;
                hz_offset16_t mark_array_offset;
                hz_offset16_t base_array_offset;
                hz_map_t *mark_map = hz_map_create();
                hz_map_t *base_map = hz_map_create();
                hz_sequence_node_t *g;
                hz_mark_record_t *mark_records;
                uint16_t *base_anchor_offsets;

                hz_stream_read16(subtable, &mark_coverage_offset);
                hz_stream_read16(subtable, &base_coverage_offset);
                hz_stream_read16(subtable, &mark_class_count);
                hz_stream_read16(subtable, &mark_array_offset);
                hz_stream_read16(subtable, &base_array_offset);

                /* parse coverages */
                hz_decode_coverage(subtable->data + mark_coverage_offset, mark_map, NULL);
                hz_decode_coverage(subtable->data + base_coverage_offset, base_map, NULL);

                /* parse arrays */
                uint16_t mark_count;
                uint16_t base_count;

                {
                    /* parsing mark array */
                    hz_stream_t *marks = hz_stream_create(subtable->data + mark_array_offset, 0, 0);
                    hz_stream_read16(marks, &mark_count);
                    mark_records = hz_bump_allocator_alloc(&allocator, sizeof(hz_mark_record_t) * mark_count);
                    uint16_t mark_index = 0;

                    while (mark_index < mark_count) {
                        hz_mark_record_t *mark = &mark_records[mark_index];

                        hz_stream_read16(marks, &mark->mark_class);
                        hz_stream_read16(marks, &mark->mark_anchor_offset);

                        ++mark_index;
                    }

                    hz_stream_destroy(marks);
                }

                {
                    /* parsing base array */
                    hz_stream_t *bases = hz_stream_create(subtable->data + base_array_offset, 0, 0);
                    hz_stream_read16(bases, &base_count);
                    base_anchor_offsets = malloc(base_count * mark_class_count * sizeof(uint32_t));
                    hz_stream_read16_n(bases, base_count * mark_class_count, base_anchor_offsets);
                    hz_stream_destroy(bases);
                }


                /* go over every glyph and position marks in relation to their base */
                for (g = sequence->root; g != NULL; g = hz_next_node_not_of_class(g, class_ignore, NULL)) {

                    /* position mark in relation to previous base if it exists */
                    hz_sequence_node_t *prev_base = hz_ot_layout_find_prev_with_class(g, HZ_GLYPH_CLASS_BASE);

                    if (prev_base != NULL) {
                        /* there actually is a previous base in the section */
                        if (hz_map_value_exists(base_map, prev_base->id) &&
                            hz_map_value_exists(mark_map, g->id)) {
                            /* both the mark and base are covered by the table
                             * position mark in relation to base glyph
                             * */
                            uint16_t mark_index = hz_map_get_value(mark_map, g->id);
                            HZ_ASSERT(mark_index < mark_count);

                            hz_mark_record_t *mark = &mark_records[ mark_index ];
                            uint16_t base_index = hz_map_get_value(base_map, prev_base->id);

                            HZ_ASSERT(mark->mark_class < mark_class_count);
                            uint16_t base_anchor_offset = base_anchor_offsets[ base_index * mark_class_count + mark->mark_class ];

                            /* check if the base anchor is NULL */
                            if (base_anchor_offset != 0) {
                                hz_anchor_t base_anchor = hz_ot_layout_read_anchor(subtable->data
                                                                                   + base_array_offset + base_anchor_offset);
                                hz_anchor_t mark_anchor = hz_ot_layout_read_anchor(subtable->data
                                                                                   + mark_array_offset + mark->mark_anchor_offset);

                                hz_metrics_t *base_metric = hz_face_get_glyph_metrics(face, prev_base->id);
                                hz_metrics_t *mark_metric = hz_face_get_glyph_metrics(face, g->id);

                                int32_t x1 = mark_anchor.x_coord;
                                int32_t y1 = mark_anchor.y_coord;
                                int32_t x2 = base_anchor.x_coord;
                                int32_t y2 = base_anchor.y_coord;

                                g->x_offset = x2 - x1;
                                g->y_offset = y2 - y1;
                            }
                        }
                    }
                }

                HZ_FREE(base_anchor_offsets);
                hz_bump_allocator_free(&allocator, mark_records);
                hz_bump_allocator_release(&allocator);
                hz_map_destroy(mark_map);
                hz_map_destroy(base_map);
            } else {
                /* error */
            }

            break;
        }
        case HZ_GPOS_LOOKUP_TYPE_MARK_TO_LIGATURE_ATTACHMENT: {
            if (format == 1) {
                hz_offset16_t mark_coverage_offset;
                hz_offset16_t ligature_coverage_offset;
                uint16_t mark_class_count;
                hz_offset16_t mark_array_offset;
                hz_offset16_t ligature_array_offset;

                hz_mark_record_t *mark_records;
                uint16_t mark_count;
                hz_offset16_t *ligature_attach_offsets;
                uint16_t ligature_count;

                /* mark and base pointers */
                hz_sequence_node_t *m, *l;

                hz_map_t *mark_map = hz_map_create();
                hz_map_t *ligature_map = hz_map_create();

                hz_stream_read16(subtable, (uint16_t *)&mark_coverage_offset);
                hz_stream_read16(subtable, (uint16_t *)&ligature_coverage_offset);
                hz_stream_read16(subtable, &mark_class_count);
                hz_stream_read16(subtable, (uint16_t *)&mark_array_offset);
                hz_stream_read16(subtable, (uint16_t *)&ligature_array_offset);

                /* parse coverages */
                hz_decode_coverage(subtable->data + mark_coverage_offset, mark_map, NULL);
                hz_decode_coverage(subtable->data + ligature_coverage_offset, ligature_map, NULL);

                {
                    /* parse mark array */
                    uint16_t mark_index;
                    hz_stream_t *marks = hz_stream_create(subtable->data + mark_array_offset,
                                                          0, 0);
                    hz_stream_read16(marks, &mark_count);
                    mark_records = HZ_MALLOC(sizeof(hz_mark_record_t) * mark_count);

                    for (mark_index = 0; mark_index < mark_count; ++mark_index) {
                        hz_mark_record_t *mark = &mark_records[mark_index];

                        hz_stream_read16(marks, &mark->mark_class);
                        hz_stream_read16(marks, &mark->mark_anchor_offset);
                    }

                    hz_stream_destroy(marks);
                }

                {
                    /* parse ligature array */
                    uint16_t ligature_index;
                    hz_stream_t *ligatures = hz_stream_create(subtable->data + ligature_array_offset,
                                                              0, 0);
                    hz_stream_read16(ligatures, &ligature_count);
                    ligature_attach_offsets = HZ_MALLOC(ligature_count * sizeof(uint16_t));
                    hz_stream_read16_n(ligatures, ligature_count, ligature_attach_offsets);

                    hz_stream_destroy(ligatures);
                }

                /* go through section glyphs and adjust marks */
                for (m = sequence->root; m != NULL; m = m->next) {
                    if (m->gc & HZ_GLYPH_CLASS_MARK) {
                        if (hz_map_value_exists(mark_map, m->id)) {
                            l = hz_prev_node_not_of_class(m, HZ_GLYPH_CLASS_MARK, NULL);

                            if (l->gc & HZ_GLYPH_CLASS_LIGATURE) {
                                if (hz_map_value_exists(ligature_map, l->id)) {
                                    uint16_t mark_index = hz_map_get_value(mark_map, m->id);
                                    hz_mark_record_t *mark_record = mark_records + mark_index;

                                    uint16_t ligature_index = hz_map_get_value(ligature_map, l->id);
                                    hz_offset16_t ligature_attach_offset = ligature_attach_offsets[ligature_index];

                                    hz_stream_t *ligature_attach_table = hz_stream_create(
                                            subtable->data + ligature_array_offset + ligature_attach_offset,
                                            0, 0);

                                    uint16_t component_count;
                                    hz_stream_read16(ligature_attach_table, &component_count);

                                    hz_offset16_t *anchor_offsets = (hz_offset16_t *)
                                            (ligature_attach_table->data + ligature_attach_table->offset);

                                    hz_offset16_t ligature_anchor_offset = bswap16(
                                            anchor_offsets[m->cid * mark_class_count + mark_record->mark_class]);

                                    if (mark_record->mark_anchor_offset && ligature_anchor_offset) {
                                        hz_anchor_t mark_anchor = hz_ot_layout_read_anchor(subtable->data
                                                                                           + mark_array_offset
                                                                                           + mark_record->mark_anchor_offset);

                                        hz_anchor_t ligature_anchor = hz_ot_layout_read_anchor(subtable->data
                                                                                               + ligature_array_offset
                                                                                               + ligature_attach_offset
                                                                                               + ligature_anchor_offset);

                                        int32_t x1 = mark_anchor.x_coord;
                                        int32_t y1 = mark_anchor.y_coord;
                                        int32_t x2 = ligature_anchor.x_coord;
                                        int32_t y2 = ligature_anchor.y_coord;

                                        m->x_offset = x2 - x1;
                                        m->y_offset = y2 - y1;
                                    }

                                    hz_stream_destroy(ligature_attach_table);
                                }
                            }
                        }
                    }
                }

                /* destroy */
                HZ_FREE(mark_records);
                HZ_FREE(ligature_attach_offsets);
                hz_map_destroy(mark_map);
                hz_map_destroy(ligature_map);
            } else {
                /* error */
            }
            break;
        }
        case HZ_GPOS_LOOKUP_TYPE_MARK_TO_MARK_ATTACHMENT: {
            if (format == 1) {
                hz_offset16_t mark1_coverage_offset;
                hz_offset16_t mark2_coverage_offset;
                uint16_t mark_class_count;
                hz_offset16_t mark1_array_offset;
                hz_offset16_t mark2_array_offset;
                hz_map_t *mark1_map = hz_map_create();
                hz_map_t *mark2_map = hz_map_create();
                hz_mark_record_t *mark1_records;
                hz_offset16_t *mark2_anchor_offsets;
                uint16_t mark1_count, mark2_count;
                hz_sequence_node_t *node;

                hz_stream_read16(subtable, &mark1_coverage_offset);
                hz_stream_read16(subtable, &mark2_coverage_offset);
                hz_stream_read16(subtable, &mark_class_count);
                hz_stream_read16(subtable, &mark1_array_offset);
                hz_stream_read16(subtable, &mark2_array_offset);

                /* parse coverages */
                hz_decode_coverage(subtable->data + mark1_coverage_offset, mark1_map, NULL);
                hz_decode_coverage(subtable->data + mark2_coverage_offset, mark2_map, NULL);

                /* parse mark arrays */
                {
                    /* parse mark1 array */
                    uint16_t mark_index;
                    hz_stream_t *mark_array = hz_stream_create(subtable->data + mark1_array_offset,0,0);
                    hz_stream_read16(mark_array, &mark1_count);
                    mark1_records = HZ_MALLOC(sizeof(hz_mark_record_t) * mark1_count);
                    for (mark_index = 0; mark_index < mark1_count; ++mark_index) {
                        hz_mark_record_t *record = mark1_records + mark_index;
                        hz_stream_read16(mark_array, &record->mark_class);
                        hz_stream_read16(mark_array, &record->mark_anchor_offset);
                    }

                    hz_stream_destroy(mark_array);
                }

                {
                    /* parse mark2 array */
                    hz_stream_t *mark_array = hz_stream_create(subtable->data + mark2_array_offset,0,0);
                    hz_stream_read16(mark_array, &mark2_count);
                    mark2_anchor_offsets = HZ_MALLOC(sizeof(uint16_t) * mark_class_count * mark2_count);
                    hz_stream_read16_n(mark_array, mark_class_count * mark2_count, mark2_anchor_offsets);
                    hz_stream_destroy(mark_array);
                }

                /* go over every glyph and position marks in relation to their base mark */
                for (node = sequence->root; node != NULL; node = node->next) {
                    if (node->gc & HZ_GLYPH_CLASS_MARK) {
                        /* glyph is of mark class, position in relation to last mark */
                        hz_sequence_node_t *prev_node = hz_prev_node_not_of_class(node, class_ignore, NULL);
                        if (prev_node != NULL) {
                            /* previous mark found, check if both glyph's ids are found in the
                             * coverage maps.
                             * */
                            if (hz_map_value_exists(mark1_map, node->id) &&
                                hz_map_value_exists(mark2_map, prev_node->id)) {
                                /* both marks glyphs are covered */
                                uint16_t mark1_index = hz_map_get_value(mark1_map, node->id);
                                HZ_ASSERT(mark1_index < mark1_count);
                                hz_mark_record_t *mark1 = &mark1_records[ mark1_index ];

                                uint16_t mark2_index = hz_map_get_value(mark2_map, prev_node->id);
                                HZ_ASSERT(mark1->mark_class < mark_class_count);
                                uint16_t mark2_anchor_offset = mark2_anchor_offsets[ mark2_index * mark_class_count
                                                                                     + mark1->mark_class ];

                                /* check if the base anchor is NULL */
                                if (mark2_anchor_offset != 0) {
                                    hz_anchor_t mark2_anchor = hz_ot_layout_read_anchor(
                                            subtable->data + mark2_array_offset + mark2_anchor_offset);
                                    hz_anchor_t mark1_anchor = hz_ot_layout_read_anchor(
                                            subtable->data + mark1_array_offset + mark1->mark_anchor_offset);

                                    hz_metrics_t *base_metric = hz_face_get_glyph_metrics(face, prev_node->id);
                                    hz_metrics_t *mark_metric = hz_face_get_glyph_metrics(face, node->id);

                                    int32_t x1 = mark1_anchor.x_coord;
                                    int32_t y1 = mark1_anchor.y_coord;
                                    int32_t x2 = mark2_anchor.x_coord;
                                    int32_t y2 = mark2_anchor.y_coord;

                                    node->x_offset += x2 - x1;
                                    node->y_offset += y2 - y1;
                                }
                            }
                        }
                    }
                }


                HZ_FREE(mark1_records);
                HZ_FREE(mark2_anchor_offsets);
                hz_map_destroy(mark1_map);
                hz_map_destroy(mark2_map);

            } else {
                /* error */
            }
            break;
        }
        case HZ_GPOS_LOOKUP_TYPE_CONTEXT_POSITIONING: {
            break;
        }
        case HZ_GPOS_LOOKUP_TYPE_CHAINED_CONTEXT_POSITIONING: {
            break;
        }
        case HZ_GPOS_LOOKUP_TYPE_EXTENSION_POSITIONING: {
            if (format == 1) {
                uint16_t extension_lookup_type;
                hz_offset32_t extension_offset;

                hz_stream_read16(subtable, &extension_lookup_type);
                hz_stream_read32(subtable, &extension_offset);

                HZ_ASSERT(extension_lookup_type < 9);

                subtable->data += extension_offset;
                subtable->offset = 0;

                hz_ot_layout_apply_gpos_subtable(face, subtable, extension_lookup_type, lookup_flags,
                                                 feature, sequence);
            } else {
                /* error */
            }
            break;
        }
        default: {
            break;
        }
    }
}

void
hz_ot_layout_apply_gpos_feature(hz_face_t *face,
                                hz_stream_t *table,
                                hz_feature_t feature,
                                hz_sequence_t *sequence)
{
    uint16_t lookup_type;
    uint16_t lookup_flags;
    uint16_t subtable_count;
    hz_stream_read16(table, &lookup_type);
    hz_stream_read16(table, &lookup_flags);
    hz_stream_read16(table, &subtable_count);

    HZ_LOG("lookup_type: %d\n", lookup.lookup_type);
    HZ_LOG("lookup_flag: %d\n", lookup.lookup_flags);
    HZ_LOG("subtable_count: %d\n", lookup.subtable_count);
    hz_glyph_class_t ic = hz_ignored_classes_from_lookup_flags(lookup_flags);

    uint16_t subtable_index = 0;
    while (subtable_index < subtable_count) {
        hz_offset16_t offset;
        hz_stream_read16(table, &offset);
        hz_stream_t *subtable = hz_stream_create(table->data + offset, 0, 0);
        hz_ot_layout_apply_gpos_subtable(face, subtable, lookup_type, lookup_flags, feature, sequence);
        hz_stream_destroy(subtable);
        ++subtable_index;
    }
}

hz_tag_t
hz_ot_script_to_tag(hz_script_t script)
{
    switch (script) {
        case HZ_SCRIPT_ARABIC: return HZ_TAG('a','r','a','b');
        case HZ_SCRIPT_LATIN: return HZ_TAG('l','a','t','n');
        case HZ_SCRIPT_HAN: return HZ_TAG('h','a','n','i');
    }

    return 0;
}

hz_tag_t
hz_ot_language_to_tag(hz_language_t language)
{
    switch (language) {
        case HZ_LANGUAGE_ARABIC: return HZ_TAG('A','R','A',' ');
        case HZ_LANGUAGE_ENGLISH: return HZ_TAG('E','N','G',' ');
        case HZ_LANGUAGE_FRENCH: return HZ_TAG('F','R','A',' ');
        case HZ_LANGUAGE_SPANISH: return HZ_TAG('E', 'S', 'P', ' ');
        case HZ_LANGUAGE_GERMAN: return HZ_TAG('D','E','U',' ');
        case HZ_LANGUAGE_JAPANESE: return HZ_TAG('J','A','N',' ');
        case HZ_LANGUAGE_URDU: return HZ_TAG('U','R','D',' ');
    }

    return 0;
}
