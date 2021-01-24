#include "guru-ot.h"

guru_feature_t
guru_ot_feature_from_tag(guru_tag tag) {
    /* loop over feature tag lookup table for it's id */
    int i;
    const guru_feature_info_t *feature_info = NULL;

    for (i = 0; i < GURU_FEATURE_COUNT; ++i) {
        feature_info = &GURU_FEATURE_INFO_LUT[i];
        if (feature_info->tag == tag) {
            return feature_info->feature;
        }
    }

    return -1;
}


guru_tag
guru_ot_tag_from_feature(guru_feature_t feature) {
    /* loop over feature tag lookup table for it's id */
    int i;
    const guru_feature_info_t *feature_info = NULL;

    for (i = 0; i < GURU_FEATURE_COUNT; ++i) {
        feature_info = &GURU_FEATURE_INFO_LUT[i];
        if (feature_info->feature == feature) {
            return feature_info->tag;
        }
    }

    return 0;
}











static void *
guru_ot_layout_choose_lang_sys(guru_face_t *face,
                             guru_byte *data,
                             guru_tag script,
                             guru_tag language) {
    guru_stream_t *subtable = guru_stream_create(data, 0, 0);
    uint16_t script_count = 0;
    uint16_t index = 0;

    guru_stream_read16(subtable, &script_count);
    GURU_LOG("script count: %d\n", script_count);

    while (index < script_count) {
        guru_tag currTag;
        uint16_t currOffset;

        guru_stream_read32(subtable, &currTag);
        guru_stream_read16(subtable, &currOffset);

        GURU_LOG("[%u] = \"%c%c%c%c\" (%u)\n", index, GURU_UNTAG(currTag), currOffset);

        if (script == currTag) {
            /* Found script */
            uint16_t scriptOffset = currOffset;
            unsigned char *scriptData = data + scriptOffset;
            guru_stream_t *scriptStream = guru_stream_create(scriptData, 0, 0);
            guru_offset16 defaultLangSysOffset;
            uint16_t langSysCount;
            guru_stream_read16(scriptStream, &defaultLangSysOffset);
            guru_stream_read16(scriptStream, &langSysCount);

            GURU_LOG("default lang sys: %u\n", defaultLangSysOffset);
            GURU_LOG("lang sys count: %u\n", langSysCount);

            uint16_t langSysIndex = 0;
            while (langSysIndex < langSysCount) {
                guru_rec16_t langSysRec;
                guru_stream_read32(scriptStream, &langSysRec.tag);
                guru_stream_read16(scriptStream, &langSysRec.offset);

                GURU_LOG("[%u] = \"%c%c%c%c\" %u\n", langSysIndex, GURU_UNTAG(langSysRec.tag), langSysRec.offset);

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
guru_ot_layout_parse_lang_sys() {

}



void
guru_ot_layout_feature_get_lookups(guru_face_t *face,
                                   guru_tag table_tag,
                                   uint16_t feature_index,
                                   uint16_t *lookup_count,
                                   uint16_t *lookup_indices) {
    guru_stream_t *table;
    uint8_t *table_data;
    uint32_t ver;
    uint16_t script_list_offset;
    uint16_t feature_list_offset;
    uint16_t lookup_list_offset;
    guru_offset32 feature_variations_offset;

    GURU_ASSERT(table_tag == GURU_OT_TAG_GSUB || table_tag == GURU_OT_TAG_GPOS);

    if (table_tag == GURU_OT_TAG_GSUB)
        table_data = face->gsub_table;
    else if (table_tag == GURU_OT_TAG_GPOS)
        table_data = face->gpos_table;

    table = guru_stream_create(table_data,0,0);
    guru_stream_read32(table, &ver);

    if (ver == 0x00010000) {
        /* version 1.0 */
        guru_stream_read16(table, &script_list_offset);
        guru_stream_read16(table, &feature_list_offset);
        guru_stream_read16(table, &lookup_list_offset);
    } else if (ver == 0x00010001) {
        /* version 1.1 */
        guru_stream_read16(table, &script_list_offset);
        guru_stream_read16(table, &feature_list_offset);
        guru_stream_read16(table, &lookup_list_offset);
        guru_stream_read32(table, &feature_variations_offset);
    }

    /* get feature at feature_index */
    uint16_t feature_count;
    guru_stream_t *feature_list = guru_stream_create(table_data + feature_list_offset,
                                                     0,0);
    guru_stream_read16(feature_list, &feature_count);
    guru_stream_seek(feature_list, 6 * feature_index); /* sizeof(guru_rec16_t) * feature_index */

    guru_tag tag;
    uint16_t feature_offset;
    guru_stream_read32(feature_list, &tag);
    guru_stream_read16(feature_list, &feature_offset);
    GURU_LOG("TAG: %c%c%c%c\n", GURU_UNTAG(tag));

    guru_stream_t *subtable = guru_stream_create(feature_list->data + feature_offset,
                                                             0, 0);
    guru_feature_table_t feature_table;
    guru_stream_read16(subtable, &feature_table.feature_params);
    guru_stream_read16(subtable, &feature_table.lookup_index_count);

    GURU_LOG("feature_params: 0x%04X\n", feature_table.feature_params);
    GURU_LOG("lookup_index_count: %u\n", feature_table.lookup_index_count);

    if (lookup_indices == NULL) {
        if (lookup_count != NULL)
            *lookup_count = feature_table.lookup_index_count;
    } else {
        int i = 0;
        while (i < feature_table.lookup_index_count) {
            guru_stream_read16(subtable, &lookup_indices[i]);
            ++i;
        }
    }

}


void
guru_ot_layout_collect_lookups(guru_face_t *face,
                               guru_tag table_tag,
                               guru_tag script,
                               guru_tag language,
                               const guru_bitset_t *feature_bits,
                               guru_set_t *lookup_indices)
{
    GURU_ASSERT(face != NULL);
    GURU_ASSERT(lookup_indices != NULL);

    guru_byte *data = (guru_byte *) face->gsub_table;
    guru_stream_t *table = guru_stream_create(data, 0, 0);
    uint32_t ver;
    uint16_t script_list_offset;
    uint16_t feature_list_offset;
    uint16_t lookup_list_offset;
    uint32_t feature_variations_offset;

    guru_stream_read32(table, &ver);

    GURU_LOG("GSUB version: %u.%u\n", ver >> 16, ver & 0xFFFF);

    if (ver == 0x00010000) {
        guru_stream_read16(table, &script_list_offset);
        guru_stream_read16(table, &feature_list_offset);
        guru_stream_read16(table, &lookup_list_offset);
        GURU_LOG("script_list_offset: %u\n", script_list_offset);
        GURU_LOG("feature_list_offset: %u\n", feature_list_offset);
        GURU_LOG("lookup_list_offset: %u\n", lookup_list_offset);
    } else if (ver == 0x00010001) {
        guru_stream_read16(table, &script_list_offset);
        guru_stream_read16(table, &feature_list_offset);
        guru_stream_read16(table, &lookup_list_offset);
        guru_stream_read32(table, &feature_variations_offset);
        GURU_LOG("script_list_offset: %u\n", script_list_offset);
        GURU_LOG("feature_list_offset: %u\n", feature_list_offset);
        GURU_LOG("lookup_list_offset: %u\n", lookup_list_offset);
        GURU_LOG("feature_variations_offset: %p\n", (void *) feature_variations_offset);
    }

    void *lsaddr = guru_ot_layout_choose_lang_sys(face,
                                                  data + script_list_offset,
                                                  script, language);
    if (lsaddr == NULL) {
        /* Language system was not found */
        GURU_ERROR("Language system was not found!\n");
        return;
    }

    GURU_LOG("Found language system!\n");

    guru_set_t *lang_feature_indices = guru_set_create();
    guru_stream_t *lsbuf = guru_stream_create(lsaddr, 0, 0);

    guru_lang_sys_t langSys;
    guru_stream_read16(lsbuf, &langSys.lookupOrder);
    guru_stream_read16(lsbuf, &langSys.requiredFeatureIndex);
    guru_stream_read16(lsbuf, &langSys.featureIndexCount);

    /* lookupOrder should be (nil) */
    GURU_LOG("lookupOrder: %p\n", (void *) langSys.lookupOrder);
    GURU_LOG("requiredFeatureIndex: %u\n", langSys.requiredFeatureIndex);
    GURU_LOG("featureIndexCount: %u\n", langSys.featureIndexCount);

    if (langSys.requiredFeatureIndex == 0xFFFF) {
        GURU_LOG("No required features!\n");
    }

    uint16_t loopIndex = 0;
    while (loopIndex < langSys.featureIndexCount) {
        uint16_t featureIndex;
        guru_stream_read16(lsbuf, &featureIndex);
        GURU_LOG("[%u] = %u\n", loopIndex, featureIndex);
        guru_set_add(lang_feature_indices, featureIndex);
        ++loopIndex;
    }


    guru_set_t *feature_indices = guru_set_create();

    {
        /* Parsing the FeatureList and accumulating selected Features */
        guru_stream_t *subtable = guru_stream_create(data + feature_list_offset, 0, 0);

        uint16_t feature_count;
        uint16_t feature_index = 0;
        guru_stream_read16(subtable, &feature_count);
        GURU_LOG("feature_count: %u\n", feature_count);

        while (feature_index < feature_count) {
            guru_rec16_t rec;
            guru_stream_read32(subtable, &rec.tag);
            guru_stream_read16(subtable, &rec.offset);
            GURU_LOG("[%u] = { \"%c%c%c%c\", %u }\n", feature_index,
                     GURU_UNTAG(rec.tag), rec.offset);


            guru_feature_t feature = guru_ot_feature_from_tag(rec.tag);

            if (guru_bitset_check(feature_bits, feature) == GURU_TRUE) {
                /* Feature is requested and exists */
                guru_set_add(feature_indices, feature_index);
            }

            ++feature_index;
        }
    }

    /* Get lookups for selected features */
    GURU_LOG("feature count: %lu\n", feature_indices->count);

    /* Gather all lookups for selected features */
    int fii = 0;
    while (fii < feature_indices->count) {
        uint16_t lookup_count;
        guru_ot_layout_feature_get_lookups(face, table_tag, feature_indices->values[fii],
                                           &lookup_count, NULL);

        uint16_t *feature_lookup_indices = (uint16_t *) GURU_MALLOC(lookup_count);
        guru_ot_layout_feature_get_lookups(face, table_tag, feature_indices->values[fii],
                                           &lookup_count, feature_lookup_indices);

        int lii = 0;
        while (lii < lookup_count) {
            guru_set_add(lookup_indices, feature_lookup_indices[lii]);
            ++ lii;
        }

        GURU_FREE(feature_lookup_indices);
        ++fii;
    }
}

void
guru_ot_layout_lookups_substitute_closure(guru_face_t *face,
                                          const guru_set_t *lookups,
                                          guru_set_t *glyphs)
{

}

guru_bool
guru_ot_layout_lookup_would_substitute(guru_face_t *face,
                                       unsigned int lookup_index,
                                       const guru_id *glyphs,
                                       unsigned int glyph_count,
                                       guru_bool zero_context)
{

}


void
guru_ot_layout_apply_lookup(guru_face_t *face,
                            guru_tag table_tag,
                            uint16_t lookup_index,
                            guru_array_t *glyph_array)
{
    guru_stream_t *table;
    uint8_t *table_data;
    uint32_t ver;
    uint16_t script_list_offset;
    uint16_t feature_list_offset;
    uint16_t lookup_list_offset;
    guru_offset32 feature_variations_offset;

    GURU_ASSERT(table_tag == GURU_OT_TAG_GSUB || table_tag == GURU_OT_TAG_GPOS);

    if (table_tag == GURU_OT_TAG_GSUB)
        table_data = face->gsub_table;
    else if (table_tag == GURU_OT_TAG_GPOS)
        table_data = face->gpos_table;

    table = guru_stream_create(table_data,0,0);
    guru_stream_read32(table, &ver);

    if (ver == 0x00010000) {
        /* version 1.0 */
        guru_stream_read16(table, &script_list_offset);
        guru_stream_read16(table, &feature_list_offset);
        guru_stream_read16(table, &lookup_list_offset);
    } else if (ver == 0x00010001) {
        /* version 1.1 */
        guru_stream_read16(table, &script_list_offset);
        guru_stream_read16(table, &feature_list_offset);
        guru_stream_read16(table, &lookup_list_offset);
        guru_stream_read32(table, &feature_variations_offset);
    }

    uint8_t *lookup_list_addr = table_data + lookup_list_offset;
    uint8_t *lookup_addr = lookup_list_addr + 2 + lookup_index * 2;
    guru_offset16 lookup_offset = bswap16(*(uint16_t *)lookup_addr);

//    GURU_LOG("%d\n", lookup_offset);

    guru_stream_t *lookup_table_stream = guru_stream_create(lookup_list_addr + lookup_offset, 0,0);
    guru_lookup_table_t lookup_table;
    guru_stream_read16(lookup_table_stream, &lookup_table.lookup_type);
    guru_stream_read16(lookup_table_stream, &lookup_table.lookup_flag);
    guru_stream_read16(lookup_table_stream, &lookup_table.subtable_count);

    GURU_LOG("lookup_type: %d\n", lookup_table.lookup_type);
    GURU_LOG("lookup_flag: %d\n", lookup_table.lookup_flag);
    GURU_LOG("subtable_count: %d\n", lookup_table.subtable_count);

    switch (lookup_table.lookup_type) {
        case GURU_GSUB_LOOKUP_TYPE_SINGLE_SUBSTITUTION: {
            int i = 0;
            while (i < lookup_table.subtable_count) {
                guru_offset16 offset;
                guru_stream_read16(lookup_table_stream, &offset);

                uint8_t *subtable_data = lookup_list_addr + lookup_offset + offset;
                guru_stream_t *subtable = guru_stream_create(subtable_data, 0, 0);
                uint16_t format;
                guru_stream_read16(subtable, &format);

                if (format == 1) {
                    guru_offset16 coverage_offset;
                    int16_t id_delta;
                    guru_stream_read16(subtable, &coverage_offset);
                    guru_stream_read16(subtable, (uint16_t *) &id_delta);



                } else if (format == 2) {
                    guru_offset16 coverage_offset;
                    uint16_t glyph_count;
                    guru_array_t *from = guru_array_create();
                    guru_array_t *to = guru_array_create();
                    uint16_t glyph_index;
                    guru_stream_read16(subtable, &coverage_offset);
                    guru_stream_read16(subtable, &glyph_count);

                    /* Read coverage offset */
                    uint16_t coverage_format;
                    guru_stream_t *coverage = guru_stream_create(subtable->data + coverage_offset,0,0);
                    guru_stream_read16(coverage, &coverage_format);

                    if (coverage_offset == 1) {
                        uint16_t coverage_idx = 0;
                        guru_id start_id;
                        uint16_t coverage_glyph_count;
                        guru_stream_read16(coverage, &start_id);
                        guru_stream_read16(coverage, &coverage_glyph_count);
                        while (coverage_idx < coverage_glyph_count) {
                            guru_array_push_back(from, start_id + coverage_idx);
                            ++coverage_idx;
                        }
                    } else if (coverage_format == 2) {

                    }

                    /* Get destination glyph indices */
                    for (glyph_index = 0; glyph_index < glyph_count; ++glyph_index) {
                        guru_id substitute_glyph;
                        guru_stream_read16(subtable, &substitute_glyph);
                        guru_array_push_back(to, substitute_glyph);
                    }

                    /* Substitute glyphs */
                    size_t gidx = 0;
                    while (gidx < guru_array_size(glyph_array)) {
                        guru_id curr_id = guru_array_at(glyph_array, gidx);
                        size_t val_idx;
                        if (guru_array_has(from, curr_id, &val_idx)) {
                            guru_array_set(glyph_array, gidx, guru_array_at(to, val_idx));
                        }

                        ++gidx;
                    }

                    guru_array_destroy(from);
                    guru_array_destroy(to);
                } else {
                    GURU_LOG("Invalid substitution subtable format!\n");
                    break;
                }

                ++i;
            }
            break;
        }

        case GURU_GSUB_LOOKUP_TYPE_MULTIPLE_SUBSTITUTION: {
            break;
        }

        case GURU_GSUB_LOOKUP_TYPE_ALTERNATE_SUBSTITUTION: {
            break;
        }

        case GURU_GSUB_LOOKUP_TYPE_LIGATURE_SUBSTITUTION: {
            break;
        }

        case GURU_GSUB_LOOKUP_TYPE_CONTEXTUAL_SUBSTITUTION: {
            break;
        }

        case GURU_GSUB_LOOKUP_TYPE_CHAINED_CONTEXTS_SUBSTITUTION: {
            break;
        }

        case GURU_GSUB_LOOKUP_TYPE_EXTENSION_SUBSTITUTION: {
            break;
        }

        case GURU_GSUB_LOOKUP_TYPE_REVERSE_CHAINING_CONTEXTUAL_SINGLE_SUBSTITUTION: {
            break;
        }

        default:
            GURU_LOG("Invalid GSUB lookup type!\n");
            break;
    }
}

guru_tag
guru_ot_script_to_tag(guru_script_t script)
{
    switch (script) {
        case GURU_SCRIPT_ARABIC: return GURU_TAG('a','r','a','b');
        case GURU_SCRIPT_LATIN: return GURU_TAG('l','a','t','n');
    }

    return 0;
}

guru_tag
guru_ot_language_to_tag(guru_language_t language)
{
    switch (language) {
        case GURU_LANGUAGE_ARABIC: return GURU_TAG('A','R','A',' ');
        case GURU_LANGUAGE_ENGLISH: return GURU_TAG('E','N','G',' ');
        case GURU_LANGUAGE_FRENCH: return GURU_TAG('F','R','A',' ');
    }

    return 0;
}