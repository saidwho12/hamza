#include "hz-ot.h"
#include "hz-ot-shape-complex-arabic.h"
#include "hz.h"

hz_bool
hz_ot_shape_complex_arabic_char_joining(hz_unicode codepoint,
                                        hz_arabic_joining_entry_t *entry)
{
    const hz_arabic_joining_entry_t *curr_entry = NULL;
    size_t index = 0;

    while (index < HZ_ARRAY_SIZE(hz_arabic_joining_list)) {
        curr_entry = &hz_arabic_joining_list[index];

        if (curr_entry->codepoint == codepoint) {
            /* Found entry for current harf */
            *entry = *curr_entry;
            return HZ_TRUE;
        }

        ++index;
    }

    return HZ_FALSE;
}

typedef enum hz_joining_dir_t {
    JOINING_NEXT,
    JOINING_PREV
} hz_joining_dir_t;

hz_section_node_t *
hz_ot_shape_complex_arabic_adj_harf(hz_section_node_t *node, hz_joining_dir_t dir)
{
    if (dir == JOINING_PREV) {
        hz_section_node_t *curr_node = node->prev;
        while (curr_node != NULL) {
            if (curr_node->data.g_class & (HZ_GLYPH_CLASS_BASE_BIT | HZ_GLYPH_CLASS_LIGATURE_BIT)
            || curr_node->data.codepoint == 0x20)
                break;

            curr_node = curr_node->prev;
        }

        return curr_node;
    }
    else if (dir == JOINING_NEXT) {
        hz_section_node_t *curr_node = node->next;
        while (curr_node != NULL) {
            if (curr_node->data.g_class & (HZ_GLYPH_CLASS_BASE_BIT | HZ_GLYPH_CLASS_LIGATURE_BIT)
                || curr_node->data.codepoint == 0x20)
                break;

            curr_node = curr_node->next;
        }

        return curr_node;
    }

    return NULL;
}

uint16_t
hz_ot_shape_complex_arabic_adj_joining(hz_section_node_t *node, hz_joining_dir_t dir)
{
    hz_arabic_joining_entry_t entry;
    hz_unicode codepoint;
    hz_section_node_t *adj = hz_ot_shape_complex_arabic_adj_harf(node, dir);

    if (adj == NULL)
        goto no_adjacent;

    codepoint = adj->data.codepoint;
    if (hz_ot_shape_complex_arabic_char_joining(codepoint, &entry))
        return entry.joining;

    no_adjacent:
    /* No adjacent harf, return non-joining */
    return NO_JOINING_GROUP | JOINING_TYPE_T;
}

hz_bool
hz_ot_shape_complex_arabic_join(hz_feature_t feature, hz_section_node_t *node)
{
    uint16_t curr;
    hz_arabic_joining_entry_t curr_entry;

    if (hz_ot_shape_complex_arabic_char_joining(node->data.codepoint, &curr_entry)) {
        uint16_t prev, next;
        curr = curr_entry.joining;
        prev = hz_ot_shape_complex_arabic_adj_joining(node, JOINING_PREV);
        next = hz_ot_shape_complex_arabic_adj_joining(node, JOINING_NEXT);

        /* Conditions for substitution */
        hz_bool fina = curr & (JOINING_TYPE_R | JOINING_TYPE_D)
                       && prev & (JOINING_TYPE_L | JOINING_TYPE_D | JOINING_TYPE_C);

        hz_bool medi = curr & JOINING_TYPE_D
                       && prev & (JOINING_TYPE_L | JOINING_TYPE_D | JOINING_TYPE_C)
                       && next & (JOINING_TYPE_R | JOINING_TYPE_D | JOINING_TYPE_C);

        hz_bool init = curr & (JOINING_TYPE_L | JOINING_TYPE_D)
                       && next & (JOINING_TYPE_R | JOINING_TYPE_D | JOINING_TYPE_C);

        if (feature == HZ_FEATURE_FINA) {
            return fina && !(medi || init);
        }
        else if (feature == HZ_FEATURE_MEDI) {
            return medi;
        }
        else if (feature == HZ_FEATURE_INIT) {
            return init && !(fina || medi);
        }
    }

    return HZ_FALSE;
}