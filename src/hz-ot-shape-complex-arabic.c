#include "hz-ot.h"
#include "hz-ot-shape-complex-arabic.h"
#include "hz.h"

hz_bool
hz_ot_shape_complex_arabic_char_joining(hz_unicode_t codepoint,
                                        uint16_t *joining)
{
    const hz_arabic_joining_entry_t *curr_entry = NULL;
    size_t index = 0;

    while (index < HZ_ARRLEN(hz_arabic_joining_list)) {
        curr_entry = &hz_arabic_joining_list[index];

        if (curr_entry->codepoint == codepoint) {
            /* Found entry for current harf */
            *joining = curr_entry->joining;
            return HZ_TRUE;
        }

        ++index;
    }

    if (codepoint == 0x0640) {
        *joining = NO_JOINING_GROUP | JOINING_TYPE_L | JOINING_TYPE_R;
        return HZ_TRUE;
    }

    return HZ_FALSE;
}

typedef enum hz_joining_dir_t {
    JOINING_NEXT,
    JOINING_PREV
} hz_joining_dir_t;

hz_sequence_node_t *
hz_ot_shape_complex_arabic_adjacent_char(const hz_sequence_node_t *node, hz_bool do_reverse)
{
    hz_sequence_node_t *curr_node = do_reverse ? node->prev : node->next;
    while (curr_node != NULL) {
        hz_unicode_t code; 
        hz_glyph_class_t glyph_class;
        hz_bool is_arabic_code;

        code = curr_node->codepoint;
        glyph_class = curr_node->gc;

        /*
        is_arabic_code = (code >= 0x0600 && code <= 0x06FF) ||
                (code >= 0x0750 && code <= 0x077F) ||
                (code >= 0x08A0 && code <= 0x08FF);
        */
        
        if (glyph_class & HZ_GLYPH_CLASS_BASE && curr_node->codepoint != 0) {
            /* glyph is anything else than a mark, return NULL */
            break;
        }

        curr_node = do_reverse ? curr_node->prev : curr_node->next;
    }

    return curr_node;
}

uint16_t
hz_ot_shape_complex_arabic_joining(const hz_sequence_node_t *node, hz_bool do_reverse)
{
    uint16_t joining;
    hz_unicode_t codepoint;
    hz_sequence_node_t *adj = hz_ot_shape_complex_arabic_adjacent_char(node, do_reverse);

    if (adj == NULL)
        goto no_adjacent;

    codepoint = adj->codepoint;
    if (hz_ot_shape_complex_arabic_char_joining(codepoint, &joining))
        return joining;

    no_adjacent:
    /* No adjacent char, return non-joining */
    return NO_JOINING_GROUP | JOINING_TYPE_T;
}

hz_bool
hz_ot_shape_complex_arabic_join(hz_feature_t feature, const hz_sequence_node_t *node)
{
    uint16_t curr;

    if (hz_ot_shape_complex_arabic_char_joining(node->codepoint, &curr)) {
        uint16_t prev, next;
        prev = hz_ot_shape_complex_arabic_joining(node, HZ_TRUE);
        next = hz_ot_shape_complex_arabic_joining(node, HZ_FALSE);

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
        else if (feature == HZ_FEATURE_ISOL) {
            return !init && !fina && !medi;
        }
    }

    return HZ_FALSE;
}

hz_bool
hz_ot_shape_complex_arabic_isol(const hz_sequence_node_t *g)
{
    uint16_t curr;

    if (hz_ot_shape_complex_arabic_char_joining(g->codepoint, &curr)) {
        uint16_t prev, next;
        prev = hz_ot_shape_complex_arabic_joining(g, HZ_TRUE);
        next = hz_ot_shape_complex_arabic_joining(g, HZ_FALSE);

        /* Conditions for substitution */
        hz_bool fina = curr & (JOINING_TYPE_R | JOINING_TYPE_D)
                       && prev & (JOINING_TYPE_L | JOINING_TYPE_D | JOINING_TYPE_C);

        hz_bool medi = curr & JOINING_TYPE_D
                       && prev & (JOINING_TYPE_L | JOINING_TYPE_D | JOINING_TYPE_C)
                       && next & (JOINING_TYPE_R | JOINING_TYPE_D | JOINING_TYPE_C);

        hz_bool init = curr & (JOINING_TYPE_L | JOINING_TYPE_D)
                       && next & (JOINING_TYPE_R | JOINING_TYPE_D | JOINING_TYPE_C);

        return !init && !fina && !medi;
    }

    return HZ_FALSE;
}

hz_bool
hz_ot_shape_complex_arabic_init(const hz_sequence_node_t *g)
{
    uint16_t curr;

    if (hz_ot_shape_complex_arabic_char_joining(g->codepoint, &curr)) {
        uint16_t prev, next;
        prev = hz_ot_shape_complex_arabic_joining(g, HZ_TRUE);
        next = hz_ot_shape_complex_arabic_joining(g, HZ_FALSE);

        /* Conditions for substitution */
        hz_bool fina = curr & (JOINING_TYPE_R | JOINING_TYPE_D)
                       && prev & (JOINING_TYPE_L | JOINING_TYPE_D | JOINING_TYPE_C);

        hz_bool medi = curr & JOINING_TYPE_D
                       && prev & (JOINING_TYPE_L | JOINING_TYPE_D | JOINING_TYPE_C)
                       && next & (JOINING_TYPE_R | JOINING_TYPE_D | JOINING_TYPE_C);

        hz_bool init = curr & (JOINING_TYPE_L | JOINING_TYPE_D)
                       && next & (JOINING_TYPE_R | JOINING_TYPE_D | JOINING_TYPE_C);

        return init && !(fina || medi);
    }

    return HZ_FALSE;
}

hz_bool
hz_ot_shape_complex_arabic_medi(const hz_sequence_node_t *g)
{
    uint16_t curr;

    if (hz_ot_shape_complex_arabic_char_joining(g->codepoint, &curr)) {
        uint16_t prev, next;
        prev = hz_ot_shape_complex_arabic_joining(g, HZ_TRUE);
        next = hz_ot_shape_complex_arabic_joining(g, HZ_FALSE);

        /* Conditions for substitution */
        hz_bool medi = curr & JOINING_TYPE_D
                       && prev & (JOINING_TYPE_L | JOINING_TYPE_D | JOINING_TYPE_C)
                       && next & (JOINING_TYPE_R | JOINING_TYPE_D | JOINING_TYPE_C);

        return medi;
    }

    return HZ_FALSE;
}

hz_bool
hz_ot_shape_complex_arabic_fina(const hz_sequence_node_t *g)
{
    uint16_t curr;

    if (hz_ot_shape_complex_arabic_char_joining(g->codepoint, &curr)) {
        uint16_t prev, next;
        prev = hz_ot_shape_complex_arabic_joining(g, HZ_TRUE);
        next = hz_ot_shape_complex_arabic_joining(g, HZ_FALSE);

        /* Conditions for substitution */
        hz_bool fina = curr & (JOINING_TYPE_R | JOINING_TYPE_D)
                       && prev & (JOINING_TYPE_L | JOINING_TYPE_D | JOINING_TYPE_C);

        hz_bool medi = curr & JOINING_TYPE_D
                       && prev & (JOINING_TYPE_L | JOINING_TYPE_D | JOINING_TYPE_C)
                       && next & (JOINING_TYPE_R | JOINING_TYPE_D | JOINING_TYPE_C);

        hz_bool init = curr & (JOINING_TYPE_L | JOINING_TYPE_D)
                       && next & (JOINING_TYPE_R | JOINING_TYPE_D | JOINING_TYPE_C);

        return fina && !(medi || init);
    }

    return HZ_FALSE;
}
