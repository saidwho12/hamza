#include "hm-ot.h"
#include "hm-ot-shape-complex-arabic.h"
#include "hm.h"

#define HM_ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

hm_bool
hm_ot_shape_complex_arabic_char_joining(hm_unicode codepoint,
                                        hm_arabic_joining_entry_t *entry)
{
    const hm_arabic_joining_entry_t *curr_entry = NULL;
    size_t index = 0;

    while (index < HM_ARRAY_SIZE(hm_arabic_joining_list)) {
        curr_entry = &hm_arabic_joining_list[index];

        if (curr_entry->codepoint == codepoint) {
            /* Found entry for current harf */
            *entry = *curr_entry;
            return HM_TRUE;
        }

        ++index;
    }

    return HM_FALSE;
}

typedef enum hm_joining_dir_t {
    JOINING_NEXT,
    JOINING_PREV
} hm_joining_dir_t;

hm_section_node_t *
hm_ot_shape_complex_arabic_adj_base_c(hm_section_node_t *node, hm_joining_dir_t dir)
{
    if (dir == JOINING_PREV)
        return node->prev;
    else if (dir == JOINING_NEXT)
        return node->next;

    return NULL;
}

uint16_t
hm_ot_shape_complex_arabic_adj_joining(hm_section_node_t *node, hm_joining_dir_t dir)
{
    hm_arabic_joining_entry_t entry;
    hm_unicode codepoint;
    hm_section_node_t *adj = hm_ot_shape_complex_arabic_adj_base_c(node, dir);

    if (adj == NULL)
        goto no_adjacent;

    codepoint = adj->data.codepoint;
    if (hm_ot_shape_complex_arabic_char_joining(codepoint, &entry))
        return entry.joining;

    no_adjacent:
    /* No adjacent harf, return non-joining */
    return NO_JOINING_GROUP | JOINING_TYPE_T;
}

hm_bool
hm_ot_shape_complex_arabic_join(hm_feature_t feature, hm_section_node_t *node)
{
    uint16_t curr;
    hm_arabic_joining_entry_t curr_entry;

    if (hm_ot_shape_complex_arabic_char_joining(node->data.codepoint, &curr_entry)) {
        uint16_t prev, next;
        curr = curr_entry.joining;
        prev = hm_ot_shape_complex_arabic_adj_joining(node, JOINING_PREV);
        next = hm_ot_shape_complex_arabic_adj_joining(node, JOINING_NEXT);

        /* Conditions for substitution */
        hm_bool fina = curr & (JOINING_TYPE_R | JOINING_TYPE_D)
                       && prev & (JOINING_TYPE_L | JOINING_TYPE_D | JOINING_TYPE_C);

        hm_bool medi = curr & JOINING_TYPE_D
                       && prev & (JOINING_TYPE_L | JOINING_TYPE_D | JOINING_TYPE_C)
                       && next & (JOINING_TYPE_R | JOINING_TYPE_D | JOINING_TYPE_C);

        hm_bool init = curr & (JOINING_TYPE_L | JOINING_TYPE_D)
                       && next & (JOINING_TYPE_R | JOINING_TYPE_D | JOINING_TYPE_C);

        if (feature == HM_FEATURE_FINA) {
            return fina && !(medi || init);
        }
        else if (feature == HM_FEATURE_MEDI) {
            return medi;
        }
        else if (feature == HM_FEATURE_INIT) {
            return init && !(fina || medi);
        }
    }

    return HM_FALSE;
}