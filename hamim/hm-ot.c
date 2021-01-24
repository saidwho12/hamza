#include "hm-ot.h"

hm_feature_t
hm_ot_feature_from_tag(hm_tag tag) {
    /* loop over feature tag lookup table for it's id */
    int i;
    const hm_feature_info_t *feature_info = NULL;

    for (i = 0; i < HM_FEATURE_COUNT; ++i) {
        feature_info = &HM_FEATURE_INFO_LUT[i];
        if (feature_info->tag == tag) {
            return feature_info->feature;
        }
    }

    return -1;
}


hm_tag
hm_ot_tag_from_feature(hm_feature_t feature) {
    /* loop over feature tag lookup table for it's id */
    int i;
    const hm_feature_info_t *feature_info = NULL;

    for (i = 0; i < HM_FEATURE_COUNT; ++i) {
        feature_info = &HM_FEATURE_INFO_LUT[i];
        if (feature_info->feature == feature) {
            return feature_info->tag;
        }
    }

    return 0;
}











static void *
hm_ot_layout_choose_lang_sys(hm_face_t *face,
                             hm_byte *data,
                             hm_tag script,
                             hm_tag language) {
    hm_stream_t *subtable = hm_stream_create(data, 0, 0);
    uint16_t script_count = 0;
    uint16_t index = 0;

    hm_stream_read16(subtable, &script_count);
    HM_LOG("script count: %d\n", script_count);

    while (index < script_count) {
        hm_tag currTag;
        uint16_t currOffset;

        hm_stream_read32(subtable, &currTag);
        hm_stream_read16(subtable, &currOffset);

        HM_LOG("[%u] = \"%c%c%c%c\" (%u)\n", index, HM_UNTAG(currTag), currOffset);

        if (script == currTag) {
            /* Found script */
            uint16_t scriptOffset = currOffset;
            unsigned char *scriptData = data + scriptOffset;
            hm_stream_t *scriptStream = hm_stream_create(scriptData, 0, 0);
            hm_offset16 defaultLangSysOffset;
            uint16_t langSysCount;
            hm_stream_read16(scriptStream, &defaultLangSysOffset);
            hm_stream_read16(scriptStream, &langSysCount);

            HM_LOG("default lang sys: %u\n", defaultLangSysOffset);
            HM_LOG("lang sys count: %u\n", langSysCount);

            uint16_t langSysIndex = 0;
            while (langSysIndex < langSysCount) {
                hm_rec16_t langSysRec;
                hm_stream_read32(scriptStream, &langSysRec.tag);
                hm_stream_read16(scriptStream, &langSysRec.offset);

                HM_LOG("[%u] = \"%c%c%c%c\" %u\n", langSysIndex, HM_UNTAG(langSysRec.tag), langSysRec.offset);

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
hm_ot_layout_parse_lang_sys() {

}



void
hm_ot_layout_feature_get_lookups(hm_face_t *face,
                                   hm_tag table_tag,
                                   uint16_t feature_index,
                                   uint16_t *lookup_count,
                                   uint16_t *lookup_indices) {
    hm_stream_t *table;
    uint8_t *table_data;
    uint32_t ver;
    uint16_t script_list_offset;
    uint16_t feature_list_offset;
    uint16_t lookup_list_offset;
    hm_offset32 feature_variations_offset;

    HM_ASSERT(table_tag == HM_OT_TAG_GSUB || table_tag == HM_OT_TAG_GPOS);

    if (table_tag == HM_OT_TAG_GSUB)
        table_data = face->gsub_table;
    else if (table_tag == HM_OT_TAG_GPOS)
        table_data = face->gpos_table;

    table = hm_stream_create(table_data,0,0);
    hm_stream_read32(table, &ver);

    if (ver == 0x00010000) {
        /* version 1.0 */
        hm_stream_read16(table, &script_list_offset);
        hm_stream_read16(table, &feature_list_offset);
        hm_stream_read16(table, &lookup_list_offset);
    } else if (ver == 0x00010001) {
        /* version 1.1 */
        hm_stream_read16(table, &script_list_offset);
        hm_stream_read16(table, &feature_list_offset);
        hm_stream_read16(table, &lookup_list_offset);
        hm_stream_read32(table, &feature_variations_offset);
    }

    /* get feature at feature_index */
    uint16_t feature_count;
    hm_stream_t *feature_list = hm_stream_create(table_data + feature_list_offset,
                                                     0,0);
    hm_stream_read16(feature_list, &feature_count);
    hm_stream_seek(feature_list, 6 * feature_index); /* sizeof(hm_rec16_t) * feature_index */

    hm_tag tag;
    uint16_t feature_offset;
    hm_stream_read32(feature_list, &tag);
    hm_stream_read16(feature_list, &feature_offset);
    HM_LOG("TAG: %c%c%c%c\n", HM_UNTAG(tag));

    hm_stream_t *subtable = hm_stream_create(feature_list->data + feature_offset,
                                                             0, 0);
    hm_feature_table_t feature_table;
    hm_stream_read16(subtable, &feature_table.feature_params);
    hm_stream_read16(subtable, &feature_table.lookup_index_count);

    HM_LOG("feature_params: 0x%04X\n", feature_table.feature_params);
    HM_LOG("lookup_index_count: %u\n", feature_table.lookup_index_count);

    if (lookup_indices == NULL) {
        if (lookup_count != NULL)
            *lookup_count = feature_table.lookup_index_count;
    } else {
        int i = 0;
        while (i < feature_table.lookup_index_count) {
            hm_stream_read16(subtable, &lookup_indices[i]);
            ++i;
        }
    }

}


void
hm_ot_layout_collect_lookups(hm_face_t *face,
                               hm_tag table_tag,
                               hm_tag script,
                               hm_tag language,
                               const hm_bitset_t *feature_bits,
                               hm_set_t *lookup_indices)
{
    HM_ASSERT(face != NULL);
    HM_ASSERT(lookup_indices != NULL);

    hm_byte *data = (hm_byte *) face->gsub_table;
    hm_stream_t *table = hm_stream_create(data, 0, 0);
    uint32_t ver;
    uint16_t script_list_offset;
    uint16_t feature_list_offset;
    uint16_t lookup_list_offset;
    uint32_t feature_variations_offset;

    hm_stream_read32(table, &ver);

    HM_LOG("GSUB version: %u.%u\n", ver >> 16, ver & 0xFFFF);

    if (ver == 0x00010000) {
        hm_stream_read16(table, &script_list_offset);
        hm_stream_read16(table, &feature_list_offset);
        hm_stream_read16(table, &lookup_list_offset);
        HM_LOG("script_list_offset: %u\n", script_list_offset);
        HM_LOG("feature_list_offset: %u\n", feature_list_offset);
        HM_LOG("lookup_list_offset: %u\n", lookup_list_offset);
    } else if (ver == 0x00010001) {
        hm_stream_read16(table, &script_list_offset);
        hm_stream_read16(table, &feature_list_offset);
        hm_stream_read16(table, &lookup_list_offset);
        hm_stream_read32(table, &feature_variations_offset);
        HM_LOG("script_list_offset: %u\n", script_list_offset);
        HM_LOG("feature_list_offset: %u\n", feature_list_offset);
        HM_LOG("lookup_list_offset: %u\n", lookup_list_offset);
        HM_LOG("feature_variations_offset: %p\n", (void *) feature_variations_offset);
    }

    void *lsaddr = hm_ot_layout_choose_lang_sys(face,
                                                  data + script_list_offset,
                                                  script, language);
    if (lsaddr == NULL) {
        /* Language system was not found */
        HM_ERROR("Language system was not found!\n");
        return;
    }

    HM_LOG("Found language system!\n");

    hm_set_t *lang_feature_indices = hm_set_create();
    hm_stream_t *lsbuf = hm_stream_create(lsaddr, 0, 0);

    hm_lang_sys_t langSys;
    hm_stream_read16(lsbuf, &langSys.lookupOrder);
    hm_stream_read16(lsbuf, &langSys.requiredFeatureIndex);
    hm_stream_read16(lsbuf, &langSys.featureIndexCount);

    /* lookupOrder should be (nil) */
    HM_LOG("lookupOrder: %p\n", (void *) langSys.lookupOrder);
    HM_LOG("requiredFeatureIndex: %u\n", langSys.requiredFeatureIndex);
    HM_LOG("featureIndexCount: %u\n", langSys.featureIndexCount);

    if (langSys.requiredFeatureIndex == 0xFFFF) {
        HM_LOG("No required features!\n");
    }

    uint16_t loopIndex = 0;
    while (loopIndex < langSys.featureIndexCount) {
        uint16_t featureIndex;
        hm_stream_read16(lsbuf, &featureIndex);
        HM_LOG("[%u] = %u\n", loopIndex, featureIndex);
        hm_set_add(lang_feature_indices, featureIndex);
        ++loopIndex;
    }


    hm_set_t *feature_indices = hm_set_create();

    {
        /* Parsing the FeatureList and accumulating selected Features */
        hm_stream_t *subtable = hm_stream_create(data + feature_list_offset, 0, 0);

        uint16_t feature_count;
        uint16_t feature_index = 0;
        hm_stream_read16(subtable, &feature_count);
        HM_LOG("feature_count: %u\n", feature_count);

        while (feature_index < feature_count) {
            hm_rec16_t rec;
            hm_stream_read32(subtable, &rec.tag);
            hm_stream_read16(subtable, &rec.offset);
            HM_LOG("[%u] = { \"%c%c%c%c\", %u }\n", feature_index,
                     HM_UNTAG(rec.tag), rec.offset);


            hm_feature_t feature = hm_ot_feature_from_tag(rec.tag);

            if (hm_bitset_check(feature_bits, feature) == HM_TRUE) {
                /* Feature is requested and exists */
                hm_set_add(feature_indices, feature_index);
            }

            ++feature_index;
        }
    }

    /* Get lookups for selected features */
    HM_LOG("feature count: %lu\n", feature_indices->count);

    /* Gather all lookups for selected features */
    int fii = 0;
    while (fii < feature_indices->count) {
        uint16_t lookup_count;
        hm_ot_layout_feature_get_lookups(face, table_tag, feature_indices->values[fii],
                                           &lookup_count, NULL);

        uint16_t *feature_lookup_indices = (uint16_t *) HM_MALLOC(lookup_count);
        hm_ot_layout_feature_get_lookups(face, table_tag, feature_indices->values[fii],
                                           &lookup_count, feature_lookup_indices);

        int lii = 0;
        while (lii < lookup_count) {
            hm_set_add(lookup_indices, feature_lookup_indices[lii]);
            ++ lii;
        }

        HM_FREE(feature_lookup_indices);
        ++fii;
    }
}

void
hm_ot_layout_lookups_substitute_closure(hm_face_t *face,
                                          const hm_set_t *lookups,
                                          hm_set_t *glyphs)
{

}

hm_bool
hm_ot_layout_lookup_would_substitute(hm_face_t *face,
                                       unsigned int lookup_index,
                                       const hm_id *glyphs,
                                       unsigned int glyph_count,
                                       hm_bool zero_context)
{

}


void
hm_ot_layout_apply_lookup(hm_face_t *face,
                            hm_tag table_tag,
                            uint16_t lookup_index,
                            hm_array_t *glyph_array)
{
    hm_stream_t *table;
    uint8_t *table_data;
    uint32_t ver;
    uint16_t script_list_offset;
    uint16_t feature_list_offset;
    uint16_t lookup_list_offset;
    hm_offset32 feature_variations_offset;

    HM_ASSERT(table_tag == HM_OT_TAG_GSUB || table_tag == HM_OT_TAG_GPOS);

    if (table_tag == HM_OT_TAG_GSUB)
        table_data = face->gsub_table;
    else if (table_tag == HM_OT_TAG_GPOS)
        table_data = face->gpos_table;

    table = hm_stream_create(table_data,0,0);
    hm_stream_read32(table, &ver);

    if (ver == 0x00010000) {
        /* version 1.0 */
        hm_stream_read16(table, &script_list_offset);
        hm_stream_read16(table, &feature_list_offset);
        hm_stream_read16(table, &lookup_list_offset);
    } else if (ver == 0x00010001) {
        /* version 1.1 */
        hm_stream_read16(table, &script_list_offset);
        hm_stream_read16(table, &feature_list_offset);
        hm_stream_read16(table, &lookup_list_offset);
        hm_stream_read32(table, &feature_variations_offset);
    }

    uint8_t *lookup_list_addr = table_data + lookup_list_offset;
    uint8_t *lookup_addr = lookup_list_addr + 2 + lookup_index * 2;
    hm_offset16 lookup_offset = bswap16(*(uint16_t *)lookup_addr);

//    HM_LOG("%d\n", lookup_offset);

    hm_stream_t *lookup_table_stream = hm_stream_create(lookup_list_addr + lookup_offset, 0,0);
    hm_lookup_table_t lookup_table;
    hm_stream_read16(lookup_table_stream, &lookup_table.lookup_type);
    hm_stream_read16(lookup_table_stream, &lookup_table.lookup_flag);
    hm_stream_read16(lookup_table_stream, &lookup_table.subtable_count);

    HM_LOG("lookup_type: %d\n", lookup_table.lookup_type);
    HM_LOG("lookup_flag: %d\n", lookup_table.lookup_flag);
    HM_LOG("subtable_count: %d\n", lookup_table.subtable_count);

    switch (lookup_table.lookup_type) {
        case HM_GSUB_LOOKUP_TYPE_SINGLE_SUBSTITUTION: {
            int i = 0;
            while (i < lookup_table.subtable_count) {
                hm_offset16 offset;
                hm_stream_read16(lookup_table_stream, &offset);

                uint8_t *subtable_data = lookup_list_addr + lookup_offset + offset;
                hm_stream_t *subtable = hm_stream_create(subtable_data, 0, 0);
                uint16_t format;
                hm_stream_read16(subtable, &format);

                if (format == 1) {
                    hm_offset16 coverage_offset;
                    int16_t id_delta;
                    hm_stream_read16(subtable, &coverage_offset);
                    hm_stream_read16(subtable, (uint16_t *) &id_delta);



                } else if (format == 2) {
                    hm_offset16 coverage_offset;
                    uint16_t glyph_count;
                    hm_array_t *from = hm_array_create();
                    hm_array_t *to = hm_array_create();
                    uint16_t glyph_index;
                    hm_stream_read16(subtable, &coverage_offset);
                    hm_stream_read16(subtable, &glyph_count);

                    /* Read coverage offset */
                    uint16_t coverage_format;
                    hm_stream_t *coverage = hm_stream_create(subtable->data + coverage_offset,0,0);
                    hm_stream_read16(coverage, &coverage_format);

                    if (coverage_offset == 1) {
                        uint16_t coverage_idx = 0;
                        hm_id start_id;
                        uint16_t coverage_glyph_count;
                        hm_stream_read16(coverage, &start_id);
                        hm_stream_read16(coverage, &coverage_glyph_count);
                        while (coverage_idx < coverage_glyph_count) {
                            hm_array_push_back(from, start_id + coverage_idx);
                            ++coverage_idx;
                        }
                    } else if (coverage_format == 2) {

                    }

                    /* Get destination glyph indices */
                    for (glyph_index = 0; glyph_index < glyph_count; ++glyph_index) {
                        hm_id substitute_glyph;
                        hm_stream_read16(subtable, &substitute_glyph);
                        hm_array_push_back(to, substitute_glyph);
                    }

                    /* Substitute glyphs */
                    size_t gidx = 0;
                    while (gidx < hm_array_size(glyph_array)) {
                        hm_id curr_id = hm_array_at(glyph_array, gidx);
                        size_t val_idx;
                        if (hm_array_has(from, curr_id, &val_idx)) {
                            hm_array_set(glyph_array, gidx, hm_array_at(to, val_idx));
                        }

                        ++gidx;
                    }

                    hm_array_destroy(from);
                    hm_array_destroy(to);
                } else {
                    HM_LOG("Invalid substitution subtable format!\n");
                    break;
                }

                ++i;
            }
            break;
        }

        case HM_GSUB_LOOKUP_TYPE_MULTIPLE_SUBSTITUTION: {
            break;
        }

        case HM_GSUB_LOOKUP_TYPE_ALTERNATE_SUBSTITUTION: {
            break;
        }

        case HM_GSUB_LOOKUP_TYPE_LIGATURE_SUBSTITUTION: {
            break;
        }

        case HM_GSUB_LOOKUP_TYPE_CONTEXTUAL_SUBSTITUTION: {
            break;
        }

        case HM_GSUB_LOOKUP_TYPE_CHAINED_CONTEXTS_SUBSTITUTION: {
            break;
        }

        case HM_GSUB_LOOKUP_TYPE_EXTENSION_SUBSTITUTION: {
            break;
        }

        case HM_GSUB_LOOKUP_TYPE_REVERSE_CHAINING_CONTEXTUAL_SINGLE_SUBSTITUTION: {
            break;
        }

        default:
            HM_LOG("Invalid GSUB lookup type!\n");
            break;
    }
}

hm_tag
hm_ot_script_to_tag(hm_script_t script)
{
    switch (script) {
        case HM_SCRIPT_ARABIC: return HM_TAG('a','r','a','b');
        case HM_SCRIPT_LATIN: return HM_TAG('l','a','t','n');
    }

    return 0;
}

hm_tag
hm_ot_language_to_tag(hm_language_t language)
{
    switch (language) {
        case HM_LANGUAGE_ARABIC: return HM_TAG('A','R','A',' ');
        case HM_LANGUAGE_ENGLISH: return HM_TAG('E','N','G',' ');
        case HM_LANGUAGE_FRENCH: return HM_TAG('F','R','A',' ');
    }

    return 0;
}