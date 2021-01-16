#include "guru-ot.h"

static void *
guru_ot_layout_choose_lang_sys(guru_face_t *face,
                             guru_byte *data,
                             guru_tag script,
                             guru_tag language) {
    guru_stream_t subtable = guru_stream_create(data, 0, 0);
    guru_uint16 script_count = 0;
    guru_uint16 index = 0;

    guru_stream_read16(&subtable, &script_count);
    GURU_LOG("script count: %d\n", script_count);

    while (index < script_count) {
        guru_tag currTag;
        guru_uint16 currOffset;

        guru_stream_read32(&subtable, &currTag);
        guru_stream_read16(&subtable, &currOffset);

        GURU_LOG("[%u] = \"%c%c%c%c\" (%u)\n", index, GURU_UNTAG(currTag), currOffset);

        if (script == currTag) {
            /* Found script */
            guru_uint16 scriptOffset = currOffset;
            unsigned char *scriptData = data + scriptOffset;
            guru_stream_t scriptStream = guru_stream_create(scriptData, 0, 0);
            guru_offset16 defaultLangSysOffset;
            guru_uint16 langSysCount;
            guru_stream_read16(&scriptStream, &defaultLangSysOffset);
            guru_stream_read16(&scriptStream, &langSysCount);

            GURU_LOG("default lang sys: %u\n", defaultLangSysOffset);
            GURU_LOG("lang sys count: %u\n", langSysCount);

            guru_uint16 langSysIndex = 0;
            while (langSysIndex < langSysCount) {
                guru_rec16_t langSysRec;
                guru_stream_read32(&scriptStream, &langSysRec.tag);
                guru_stream_read16(&scriptStream, &langSysRec.offset);

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
guru_ot_layout_collect_lookups(guru_face_t *face,
                             guru_tag table_tag,
                             guru_tag script,
                             guru_tag language,
                             const guru_bitset_t *feature_bits,
                             guru_set_t *lookup_indices)
{
    GURU_ASSERT(face != NULL);
    GURU_ASSERT(lookup_indices != NULL);

    if(table_tag == GURU_OT_TAG_GSUB) {
        guru_byte *data = (guru_byte *) face->GSUB;
        guru_stream_t table = guru_stream_create(data, 0, 0);
        guru_uint16 version_major, version_minor;

        guru_stream_read16(&table, &version_major);
        guru_stream_read16(&table, &version_minor);

        GURU_LOG("GSUB version: %d.%d\n", version_major, version_minor);

        if (version_major == 1 && version_minor == 0) {
            guru_uint16 script_list_offset;
            guru_uint16 feature_list_offset;
            guru_uint16 lookup_list_offset;
            guru_stream_read16(&table, &script_list_offset);
            guru_stream_read16(&table, &feature_list_offset);
            guru_stream_read16(&table, &lookup_list_offset);

            GURU_LOG("script_list_offset: %u\n", script_list_offset);
            GURU_LOG("feature_list_offset: %u\n", feature_list_offset);
            GURU_LOG("lookup_list_offset: %u\n", lookup_list_offset);

            void *lsaddr = guru_ot_layout_choose_lang_sys(face,
                                               data + script_list_offset,
                                                            script, language);
            if (lsaddr == NULL) {
                /* Language system was not found */
                GURU_ERROR("Language system was not found!\n");
                return;
            } else {
                GURU_LOG("Found language system!\n");

                guru_set_t *feature_indices = guru_set_create();
                guru_stream_t lsbuf = guru_stream_create(lsaddr, 0, 0 );

                guru_lang_sys_t langSys;
                guru_stream_read16(&lsbuf, &langSys.lookupOrder);
                guru_stream_read16(&lsbuf, &langSys.requiredFeatureIndex);
                guru_stream_read16(&lsbuf, &langSys.featureIndexCount);

                /* lookupOrder should be (nil) */
                GURU_LOG("lookupOrder: %p\n", (void *)langSys.lookupOrder);
                GURU_LOG("requiredFeatureIndex: %u\n", langSys.requiredFeatureIndex);
                GURU_LOG("featureIndexCount: %u\n", langSys.featureIndexCount);

                if (langSys.requiredFeatureIndex == 0xFFFF) {
                    GURU_LOG("No required features!\n");
                }

                guru_uint16 loopIndex = 0;
                while (loopIndex < langSys.featureIndexCount) {
                    guru_uint16 featureIndex;
                    guru_stream_read16(&lsbuf, &featureIndex);
                    GURU_LOG("[%u] = %u\n", loopIndex, featureIndex);
                    guru_set_add(feature_indices, featureIndex);
                    ++loopIndex;
                }
            }

            {
                /* Parsing the FeatureList and accumulating selected Features */
                guru_stream_t subtable = guru_stream_create(data + feature_list_offset, 0, 0);

                guru_uint16 feature_count;
                guru_uint16 feature_index = 0;
                guru_stream_read16(&subtable, &feature_count);
                GURU_LOG("feature_count: %u\n", feature_count);

                while (feature_index < feature_count) {
                    guru_rec16_t rec;
                    guru_stream_read32(&subtable, &rec.tag);
                    guru_stream_read16(&subtable, &rec.offset);

                    GURU_LOG("[%u] = { \"%c%c%c%c\", %u }\n", feature_index,
                             GURU_UNTAG(rec.tag), rec.offset);

                    ++feature_index;
                }
            }

        }
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