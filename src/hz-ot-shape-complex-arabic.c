#include "hz-ot.h"
#include "hz-ot-shape-complex-arabic.h"
#include "hz.h"

hz_bool_t
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

static hz_bool_t
is_arabic_codepoint(hz_unicode_t cp) {
    return (cp >= 0x0600u && cp <= 0x06FFu) || /* Arabic (0600–06FF) */
           (cp >= 0x0750u && cp <= 0x077Fu) || /* Arabic Supplement (0750–077F) */
           (cp >= 0x08A0u && cp <= 0x08FFu) || /* Arabic Extended-A (08A0–08FF) */
           (cp >= 0xFB50u && cp <= 0xFDFFu) || /* Arabic Presentation Forms-B (FE70–FEFF) */
           (cp >= 0xFE70u && cp <= 0xFEFFu) || /* Arabic Presentation Forms-B (FE70–FEFF) */
           (cp >= 0x1EE00u && cp <= 0x1EEFFu); /* Arabic Mathematical Alphabetic Symbols (1EE00–1EEFF) */
}

hz_sequence_node_t *
hz_ot_shape_complex_arabic_adjacent_char(const hz_sequence_node_t *node, hz_bool_t do_reverse)
{
    hz_sequence_node_t *curr_node = do_reverse ? node->prev : node->next;
    while (curr_node != NULL) {
        if (!is_arabic_codepoint(curr_node->codepoint)) {
            return NULL;
        }

        if (curr_node->gc & ~HZ_GLYPH_CLASS_MARK) {
            /* glyph is anything else than a mark, return NULL */
            break;
        }

        curr_node = do_reverse ? curr_node->prev : curr_node->next;
    }

    return curr_node;
}

uint16_t
hz_ot_shape_complex_arabic_joining(const hz_sequence_node_t *node, hz_bool_t do_reverse)
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

hz_bool_t
hz_ot_shape_complex_arabic_isol(const hz_sequence_node_t *g)
{
    uint16_t curr;

    if (hz_ot_shape_complex_arabic_char_joining(g->codepoint, &curr)) {
        uint16_t prev, next;
        prev = hz_ot_shape_complex_arabic_joining(g, HZ_TRUE);
        next = hz_ot_shape_complex_arabic_joining(g, HZ_FALSE);

        /* Conditions for substitution */
        hz_bool_t fina = curr & (JOINING_TYPE_R | JOINING_TYPE_D)
                         && prev & (JOINING_TYPE_L | JOINING_TYPE_D | JOINING_TYPE_C);

        hz_bool_t medi = curr & JOINING_TYPE_D
                         && prev & (JOINING_TYPE_L | JOINING_TYPE_D | JOINING_TYPE_C)
                         && next & (JOINING_TYPE_R | JOINING_TYPE_D | JOINING_TYPE_C);

        hz_bool_t init = curr & (JOINING_TYPE_L | JOINING_TYPE_D)
                         && next & (JOINING_TYPE_R | JOINING_TYPE_D | JOINING_TYPE_C);

        return !init && !fina && !medi;
    }

    return HZ_FALSE;
}

hz_bool_t
hz_ot_shape_complex_arabic_init(const hz_sequence_node_t *g)
{
    uint16_t curr;

    if (hz_ot_shape_complex_arabic_char_joining(g->codepoint, &curr)) {
        uint16_t prev, next;
        prev = hz_ot_shape_complex_arabic_joining(g, HZ_TRUE);
        next = hz_ot_shape_complex_arabic_joining(g, HZ_FALSE);

        /* Conditions for substitution */
        hz_bool_t fina = curr & (JOINING_TYPE_R | JOINING_TYPE_D)
                         && prev & (JOINING_TYPE_L | JOINING_TYPE_D | JOINING_TYPE_C);

        hz_bool_t medi = curr & JOINING_TYPE_D
                         && prev & (JOINING_TYPE_L | JOINING_TYPE_D | JOINING_TYPE_C)
                         && next & (JOINING_TYPE_R | JOINING_TYPE_D | JOINING_TYPE_C);

        hz_bool_t init = curr & (JOINING_TYPE_L | JOINING_TYPE_D)
                         && next & (JOINING_TYPE_R | JOINING_TYPE_D | JOINING_TYPE_C);

        return init && !medi;//(fina || medi);
    }

    return HZ_FALSE;
}

hz_bool_t
hz_ot_shape_complex_arabic_medi(const hz_sequence_node_t *g)
{
    uint16_t curr;

    if (hz_ot_shape_complex_arabic_char_joining(g->codepoint, &curr)) {
        uint16_t prev, next;
        prev = hz_ot_shape_complex_arabic_joining(g, HZ_TRUE);
        next = hz_ot_shape_complex_arabic_joining(g, HZ_FALSE);

        /* Conditions for substitution */
        hz_bool_t medi = curr & JOINING_TYPE_D
                         && prev & (JOINING_TYPE_L | JOINING_TYPE_D | JOINING_TYPE_C)
                         && next & (JOINING_TYPE_R | JOINING_TYPE_D | JOINING_TYPE_C);

        return medi;
    }

    return HZ_FALSE;
}

hz_bool_t
hz_ot_shape_complex_arabic_fina(const hz_sequence_node_t *g)
{
    uint16_t curr;

    if (hz_ot_shape_complex_arabic_char_joining(g->codepoint, &curr)) {
        uint16_t prev, next;
        prev = hz_ot_shape_complex_arabic_joining(g, HZ_TRUE);
        next = hz_ot_shape_complex_arabic_joining(g, HZ_FALSE);

        /* Conditions for substitution */
        hz_bool_t fina = curr & (JOINING_TYPE_R | JOINING_TYPE_D)
                         && prev & (JOINING_TYPE_L | JOINING_TYPE_D | JOINING_TYPE_C);

        hz_bool_t medi = curr & JOINING_TYPE_D
                         && prev & (JOINING_TYPE_L | JOINING_TYPE_D | JOINING_TYPE_C)
                         && next & (JOINING_TYPE_R | JOINING_TYPE_D | JOINING_TYPE_C);

        hz_bool_t init = curr & (JOINING_TYPE_L | JOINING_TYPE_D)
                         && next & (JOINING_TYPE_R | JOINING_TYPE_D | JOINING_TYPE_C);

        return fina && !(medi || init);
    }

    return HZ_FALSE;
}
