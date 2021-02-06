#include "hz-ot.h"
#include "hz-ot-shape-complex-arabic.h"
#include "hz-map.h"

hz_feature_t
hz_ot_feature_from_tag(hz_tag tag) {
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

hz_tag
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











static void *
hz_ot_layout_choose_lang_sys(hz_face_t *face,
                             hz_byte *data,
                             hz_tag script,
                             hz_tag language) {
    hz_stream_t *subtable = hz_stream_create(data, 0, 0);
    uint16_t script_count = 0;
    uint16_t index = 0;

    hz_stream_read16(subtable, &script_count);
    HZ_LOG("script count: %d\n", script_count);

    while (index < script_count) {
        hz_tag currTag;
        uint16_t currOffset;

        hz_stream_read32(subtable, &currTag);
        hz_stream_read16(subtable, &currOffset);

        HZ_LOG("[%u] = \"%c%c%c%c\" (%u)\n", index, HZ_UNTAG(currTag), currOffset);

        if (script == currTag) {
            /* Found script */
            uint16_t scriptOffset = currOffset;
            unsigned char *scriptData = data + scriptOffset;
            hz_stream_t *scriptStream = hz_stream_create(scriptData, 0, 0);
            hz_offset16 defaultLangSysOffset;
            uint16_t langSysCount;
            hz_stream_read16(scriptStream, &defaultLangSysOffset);
            hz_stream_read16(scriptStream, &langSysCount);

            HZ_LOG("default lang sys: %u\n", defaultLangSysOffset);
            HZ_LOG("lang sys count: %u\n", langSysCount);

            uint16_t langSysIndex = 0;
            while (langSysIndex < langSysCount) {
                hz_rec16_t langSysRec;
                hz_stream_read32(scriptStream, &langSysRec.tag);
                hz_stream_read16(scriptStream, &langSysRec.offset);

                HZ_LOG("[%u] = \"%c%c%c%c\" %u\n", langSysIndex, HZ_UNTAG(langSysRec.tag), langSysRec.offset);

                if (langSysRec.tag == language) {
                    /* Found language system */
                    return scriptData + langSysRec.offset;
                }

                ++langSysIndex;
            }

            /* Couldn't find alterior language system, return default. */
            return scriptData + defaultLangSysOffset;
        }

        ++index;
    }

    return NULL;
}

static void
hz_ot_layout_parse_lang_sys() {

}



void
hz_ot_layout_feature_get_lookups(uint8_t *data,
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


hz_bool
hz_ot_layout_apply_gsub_features(hz_face_t *face,
                                 hz_tag script,
                                 hz_tag language,
                                 const hz_bitset_t *feature_bits,
                                 hz_section_t *sect)
{
    HZ_ASSERT(face != NULL);
    HZ_ASSERT(feature_bits != NULL);

    hz_byte *data = (hz_byte *) face->gsub_table;
    hz_stream_t *table = hz_stream_create(data, 0, 0);
    uint32_t ver;
    uint16_t script_list_offset;
    uint16_t feature_list_offset;
    uint16_t lookup_list_offset;
    uint32_t feature_variations_offset;

    hz_stream_read32(table, &ver);

    HZ_LOG("GSUB version: %u.%u\n", ver >> 16, ver & 0xFFFF);

    if (ver == 0x00010000) {
        hz_stream_read16(table, &script_list_offset);
        hz_stream_read16(table, &feature_list_offset);
        hz_stream_read16(table, &lookup_list_offset);
        HZ_LOG("script_list_offset: %u\n", script_list_offset);
        HZ_LOG("feature_list_offset: %u\n", feature_list_offset);
        HZ_LOG("lookup_list_offset: %u\n", lookup_list_offset);
    }
    else if (ver == 0x00010001) {
        hz_stream_read16(table, &script_list_offset);
        hz_stream_read16(table, &feature_list_offset);
        hz_stream_read16(table, &lookup_list_offset);
        hz_stream_read32(table, &feature_variations_offset);
        HZ_LOG("script_list_offset: %u\n", script_list_offset);
        HZ_LOG("feature_list_offset: %u\n", feature_list_offset);
        HZ_LOG("lookup_list_offset: %u\n", lookup_list_offset);
        HZ_LOG("feature_variations_offset: %p\n", (void *) feature_variations_offset);
    }

    void *lsaddr = hz_ot_layout_choose_lang_sys(face,
                                                data + script_list_offset,
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

    hz_stream_t *lookup_list = hz_stream_create(data + lookup_list_offset, 0, 0);
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


    hz_stream_t *feature_list = hz_stream_create(data + feature_list_offset, 0, 0);


    {
        /* Parsing the FeatureList and applying selected Features */
        uint16_t feature_count;
        uint16_t feature_index = 0;
        hz_stream_read16(feature_list, &feature_count);
        HZ_LOG("feature_count: %u\n", feature_count);

        while (feature_index < feature_count) {
            hz_rec16_t rec;
            hz_stream_read32(feature_list, &rec.tag);
            hz_stream_read16(feature_list, &rec.offset);
            HZ_LOG("[%u] = { \"%c%c%c%c\", %u }\n", feature_index,
                     HZ_UNTAG(rec.tag), rec.offset);

            hz_feature_t feature = hz_ot_feature_from_tag(rec.tag);

            if (hz_bitset_check(feature_bits, feature) == HZ_TRUE) {
                /* Feature is requested and exists */
                hz_array_t *lookup_indices = hz_array_create();
                hz_ot_layout_feature_get_lookups(feature_list->data + rec.offset, lookup_indices);

                int i = 0;
                while (i < hz_array_size(lookup_indices)) {
                    uint16_t lookup_offset = hz_array_at(lookup_offsets, hz_array_at(lookup_indices, i));
                    hz_stream_t *lookup_table = hz_stream_create(lookup_list->data + lookup_offset, 0, 0);
                    hz_ot_layout_apply_gsub_lookup(face, lookup_table, feature, sect);
                    ++i;
                }

                hz_array_destroy(lookup_indices);
            }

            ++feature_index;
        }
    }

    return HZ_TRUE;
}

hz_bool
hz_ot_layout_apply_gpos_features(hz_face_t *face,
                                 hz_tag script,
                                 hz_tag language,
                                 const hz_bitset_t *feature_bits,
                                 hz_section_t *sect)
{
    HZ_ASSERT(face != NULL);
    HZ_ASSERT(feature_bits != NULL);

    hz_byte *data = (hz_byte *) face->gpos_table;
    hz_stream_t *table = hz_stream_create(data, 0, 0);
    uint32_t ver;
    uint16_t script_list_offset;
    uint16_t feature_list_offset;
    uint16_t lookup_list_offset;
    uint32_t feature_variations_offset;

    hz_stream_read32(table, &ver);

    HZ_LOG("GPOS version: %u.%u\n", ver >> 16, ver & 0xFFFF);
    switch (ver) {
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
            break;
    }

    void *lsaddr = hz_ot_layout_choose_lang_sys(face,
                                                data + script_list_offset,
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

    hz_stream_t *lookup_list = hz_stream_create(data + lookup_list_offset, 0, 0);
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


    hz_stream_t *feature_list = hz_stream_create(data + feature_list_offset, 0, 0);


    {
        /* Parsing the FeatureList and applying selected Features */
        uint16_t feature_count;
        uint16_t feature_index = 0;
        hz_stream_read16(feature_list, &feature_count);
        HZ_LOG("feature_count: %u\n", feature_count);

        while (feature_index < feature_count) {
            hz_rec16_t rec;
            hz_stream_read32(feature_list, &rec.tag);
            hz_stream_read16(feature_list, &rec.offset);
            HZ_LOG("[%u] = { \"%c%c%c%c\", %u }\n", feature_index,
                   HZ_UNTAG(rec.tag), rec.offset);

            hz_feature_t feature = hz_ot_feature_from_tag(rec.tag);

            if (hz_bitset_check(feature_bits, feature) == HZ_TRUE) {
                /* Feature is requested and exists */
                hz_array_t *lookup_indices = hz_array_create();
                hz_ot_layout_feature_get_lookups(feature_list->data + rec.offset, lookup_indices);

                int i = 0;
                while (i < hz_array_size(lookup_indices)) {
                    uint16_t lookup_offset = hz_array_at(lookup_offsets, hz_array_at(lookup_indices, i));
                    hz_stream_t *lookup_table = hz_stream_create(lookup_list->data + lookup_offset, 0, 0);
                    hz_ot_layout_apply_gpos_lookup(face, lookup_table, feature, sect);
                    ++i;
                }

                hz_array_destroy(lookup_indices);
            }

            ++feature_index;
        }
    }

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
                                     const hz_id *glyphs,
                                     unsigned int glyph_count,
                                     hz_bool zero_context)
{

}


hz_bool
hz_ot_layout_parse_coverage(uint8_t *data,
                            hz_map_t *map_subst,
                            hz_array_t *subst)
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
                hz_map_set_value(map_subst, glyph_index, hz_array_at(subst, coverage_idx));
                ++coverage_idx;
            }

            return HZ_TRUE;
        }

        case 2: {
            uint16_t range_index = 0, range_count;
            hz_stream_read16(table, &range_count);

            /* Assuming ranges are ordered from 0 to glyph_count in order */
            while (range_index < range_count) {
                hz_id from, to;
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
                    to = hz_array_at(subst, range.start_coverage_index + range_offset);
                    hz_map_set_value(map_subst, from, to);
                    HZ_LOG("%d -> %d\n", from, to);
                    ++range_offset;
                }

                ++range_index;
            }

            return HZ_TRUE;
        }

        default: return HZ_FALSE;
    }
}



void
hz_ot_layout_apply_gsub_lookup(hz_face_t *face,
                               hz_stream_t *table,
                               hz_feature_t feature,
                               hz_section_t *sect)
{
    hz_lookup_table_t lookup;
    hz_stream_read16(table, &lookup.lookup_type);
    hz_stream_read16(table, &lookup.lookup_flag);
    hz_stream_read16(table, &lookup.subtable_count);

    HZ_LOG("lookup_type: %d\n", lookup.lookup_type);
    HZ_LOG("lookup_flag: %d\n", lookup.lookup_flag);
    HZ_LOG("subtable_count: %d\n", lookup.subtable_count);

    uint16_t subtable_index = 0;
    while (subtable_index < lookup.subtable_count) {
        hz_offset16 offset;
        hz_stream_read16(table, &offset);
        hz_stream_t *subtable = hz_stream_create(table->data + offset, 0, 0);
        uint16_t subst_format;
        hz_stream_read16(subtable, &subst_format);

        switch (lookup.lookup_type) {
            case HZ_GSUB_LOOKUP_TYPE_SINGLE_SUBSTITUTION: {
                if (subst_format == 1) {
                    hz_offset16 coverage_offset;
                    int16_t id_delta;
                    hz_stream_read16(subtable, &coverage_offset);
                    hz_stream_read16(subtable, (uint16_t *) &id_delta);
                    /* NOTE: Implement */
                }
                else if (subst_format == 2) {
                    hz_map_t *map_subst = hz_map_create();
                    hz_offset16 coverage_offset;
                    uint16_t glyph_count;
                    hz_array_t *subst = hz_array_create();
                    hz_stream_read16(subtable, &coverage_offset);
                    hz_stream_read16(subtable, &glyph_count);

                    /* Get destination glyph indices */
                    uint16_t dst_gidx;
                    for (dst_gidx = 0; dst_gidx < glyph_count; ++dst_gidx) {
                        hz_id substitute_glyph;
                        hz_stream_read16(subtable, &substitute_glyph);
                        hz_array_push_back(subst, substitute_glyph);
                    }

                    /* Read coverage offset */
                    hz_ot_layout_parse_coverage(subtable->data + coverage_offset, map_subst, subst);

                    /* Substitute glyphs */
                    hz_section_node_t *curr_node = sect->root;

                    while (curr_node != NULL) {
                        hz_id curr_id = curr_node->data.id;

                        if (hz_map_value_exists(map_subst, curr_id)) {
                            switch (feature) {
                                case HZ_FEATURE_ISOL:
                                case HZ_FEATURE_MEDI:
                                case HZ_FEATURE_MED2:
                                case HZ_FEATURE_INIT:
                                case HZ_FEATURE_FINA:
                                case HZ_FEATURE_FIN2:
                                case HZ_FEATURE_FIN3:
                                    if (hz_ot_shape_complex_arabic_join(feature, curr_node)) {
                                        curr_node->data.id = hz_map_get_value(map_subst, curr_id);
                                    }
                                    break;
                            }
                        }
                        curr_node = curr_node->next;
                    }

                    hz_array_destroy(subst);
                    hz_map_destroy(map_subst);
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
                hz_offset16 coverage_offset;
                uint16_t ligature_set_count;
                hz_array_t *ligature_offsets;
                hz_map_t *ligature_map;
                HZ_ASSERT(subst_format == 1);

                ligature_offsets = hz_array_create();
                ligature_map = hz_map_create();
                hz_stream_read16(subtable, &coverage_offset);
                hz_stream_read16(subtable, &ligature_set_count);

                /* Read all ligature set table offsets */
                uint16_t ligature_set_index = 0;
                while (ligature_set_index < ligature_set_count) {
                    uint16_t val = bswap16(*(uint16_t *)(subtable->data + subtable->offset + ligature_set_index * 2));
                    hz_array_push_back(ligature_offsets, val);
                    ++ligature_set_index;
                }

                /* Read coverage offset */
                hz_ot_layout_parse_coverage(subtable->data + coverage_offset, ligature_map, ligature_offsets);

                /* Substitute glyphs */
                hz_section_node_t *curr_node = sect->root;
                while (curr_node != NULL) {
                    hz_id curr_id = curr_node->data.id;
                    if (hz_map_value_exists(ligature_map, curr_id)) {
                        HZ_LOG("curr_id: 0x%04x\n", curr_id);

                        uint16_t ligsetoff = hz_map_get_value(ligature_map, curr_id);
                        /* LigatureSet table (all ligatures beginning with the same glyph) */
                        hz_stream_t *ligature_set = hz_stream_create(subtable->data + ligsetoff,0,0);
                        uint16_t ligcount;
                        hz_stream_read16(ligature_set, &ligcount);
                        /* Check each ligature and if any matches replace */
                        uint16_t ligidx = 0;
                        while (ligidx < ligcount) {
                            hz_offset16 ligoff;
                            hz_stream_read16(ligature_set, &ligoff);
                            /* Ligature table */
                            hz_stream_t *ligature = hz_stream_create(ligature_set->data + ligoff,0,0);
                            uint16_t ligature_glyph;
                            uint16_t component_count;
                            hz_stream_read16(ligature, &ligature_glyph);
                            hz_stream_read16(ligature, &component_count);

                            /* Check if ligature is applicable */
                            size_t rem = hz_section_node_count(curr_node);
                            if (rem >= component_count) {
                                hz_array_t *ligarr = hz_array_create();
                                hz_array_push_back(ligarr, curr_id);

                                /* Gather all components */
                                uint16_t component_index = 0;
                                while (component_index < component_count) {
                                    uint16_t glyph;
                                    hz_stream_read16(ligature, &glyph);
                                    hz_array_push_back(ligarr, glyph);
                                    ++component_index;
                                }

                                hz_bool is_eq = HZ_TRUE;
                                size_t comp_offset = 0;
                                hz_section_node_t *tmp_node = curr_node;
                                while (comp_offset < component_count) {
                                    hz_id ligid = hz_array_at(ligarr, comp_offset);

                                    if (tmp_node->data.id != ligid) {
                                        is_eq = HZ_FALSE;
                                        break;
                                    }

                                    tmp_node = tmp_node->next;
                                    ++comp_offset;
                                }

                                if (is_eq) {
                                    /* Substitute nodes for single node */
                                    hz_section_rem_n_next_nodes(curr_node, component_count-1);
                                    curr_node->data.id = ligature_glyph;
                                    goto skip_done_lig;
                                }

                                hz_array_destroy(ligarr);
                            }

                            ++ligidx;
                        }
                    }

                    skip_done_lig:
                    curr_node = curr_node->next;
                }

                hz_array_destroy(ligature_offsets);
                hz_map_destroy(ligature_map);
                break;
            }

            case HZ_GSUB_LOOKUP_TYPE_CONTEXTUAL_SUBSTITUTION: {
                break;
            }

            case HZ_GSUB_LOOKUP_TYPE_CHAINED_CONTEXTS_SUBSTITUTION: {
                break;
            }

            case HZ_GSUB_LOOKUP_TYPE_EXTENSION_SUBSTITUTION: {
                switch (subst_format) {
                    case 1:

                        break;
                    default:
                        break;
                }
                break;
            }

            case HZ_GSUB_LOOKUP_TYPE_REVERSE_CHAINING_CONTEXTUAL_SINGLE_SUBSTITUTION: {
                break;
            }

            default:
                HZ_LOG("Invalid GSUB lookup type!\n");
                break;
        }

        ++subtable_index;
    }
}

typedef struct hz_entry_exit_record_t {
    hz_offset16 entry_anchor_offset, exit_anchor_offset;
} hz_entry_exit_record_t;

typedef struct hz_anchor_t {
    int16_t x_coord, y_coord;
} hz_anchor_t;

void
hz_ot_layout_apply_gpos_lookup(hz_face_t *face,
                               hz_stream_t *table,
                               hz_feature_t feature,
                               hz_section_t *sect)
{
    hz_lookup_table_t lookup;
    hz_stream_read16(table, &lookup.lookup_type);
    hz_stream_read16(table, &lookup.lookup_flag);
    hz_stream_read16(table, &lookup.subtable_count);

    HZ_LOG("lookup_type: %d\n", lookup.lookup_type);
    HZ_LOG("lookup_flag: %d\n", lookup.lookup_flag);
    HZ_LOG("subtable_count: %d\n", lookup.subtable_count);

    uint16_t subtable_index = 0;
    while (subtable_index < lookup.subtable_count) {
        hz_offset16 offset;
        hz_stream_read16(table, &offset);
        hz_stream_t *subtable = hz_stream_create(table->data + offset, 0, 0);
        uint16_t format;
        hz_stream_read16(subtable, &format);

        switch (lookup.lookup_type) {
            case HZ_GPOS_LOOKUP_TYPE_SINGLE_ADJUSTMENT: {
                break;
            }
            case HZ_GPOS_LOOKUP_TYPE_PAIR_ADJUSTMENT: {
                break;
            }
            case HZ_GPOS_LOOKUP_TYPE_CURSIVE_ATTACHMENT: {
                if (format == 1) {
                    hz_offset16 coverage_offset;
                    uint16_t entry_exit_count, entry_exit_index;

                    hz_stream_read16(subtable, &coverage_offset);
                    hz_stream_read16(subtable, &entry_exit_count);

                    entry_exit_index = 0;
                    while (entry_exit_index < entry_exit_count) {
                        hz_entry_exit_record_t rec;
                        hz_stream_read16(subtable, &rec.entry_anchor_offset);
                        hz_stream_read16(subtable, &rec.exit_anchor_offset);
                        ++entry_exit_index;
                    }

                    hz_anchor_t entry, exit;

                } else {
                    /* error */
                }

                break;
            }
            case HZ_GPOS_LOOKUP_TYPE_MARK_TO_BASE_ATTACHMENT: {
                break;
            }
            case HZ_GPOS_LOOKUP_TYPE_MARK_TO_LIGATURE_ATTACHMENT: {
                break;
            }
            case HZ_GPOS_LOOKUP_TYPE_MARK_TO_MARK_ATTACHMENT: {
                break;
            }
            case HZ_GPOS_LOOKUP_TYPE_CONTEXT_POSITIONING: {
                break;
            }
            case HZ_GPOS_LOOKUP_TYPE_CHAINED_CONTEXT_POSITIONING: {
                break;
            }
            case HZ_GPOS_LOOKUP_TYPE_EXTENSION_POSITIONING: {
                break;
            }
            default: {
                break;
            }
        }

        ++subtable_index;
    }
}

hz_tag
hz_ot_script_to_tag(hz_script_t script)
{
    switch (script) {
        case HZ_SCRIPT_ARABIC: return HZ_TAG('a','r','a','b');
        case HZ_SCRIPT_LATIN: return HZ_TAG('l','a','t','n');
    }

    return 0;
}

hz_tag
hz_ot_language_to_tag(hz_language_t language)
{
    switch (language) {
        case HZ_LANGUAGE_ARABIC: return HZ_TAG('A','R','A',' ');
        case HZ_LANGUAGE_ENGLISH: return HZ_TAG('E','N','G',' ');
        case HZ_LANGUAGE_FRENCH: return HZ_TAG('F','R','A',' ');
    }

    return 0;
}