#include "hm-ot.h"
#include "hm-ot-shape-complex-arabic.h"
#include "hm.h"

#define HM_ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

hm_bool
hm_ot_shape_complex_arabic_char_joining(hm_face_t *face,
                                        hm_id char_id,
                                        hm_arabic_joining_entry_t *entry)
{
    const hm_arabic_joining_entry_t *curr_entry = NULL;
    size_t index = 0;

    while (index < HM_ARRAY_SIZE(hm_arabic_joining_list)) {
        curr_entry = &hm_arabic_joining_list[index];
        hm_id entry_id = hm_face_cmap_unicode_to_id(face, curr_entry->codepoint);

        if (entry_id == char_id) {
            /* Found entry for current harf */
            *entry = *curr_entry;
            return HM_TRUE;
        }

        ++index;
    }

    return HM_FALSE;
}

uint16_t
hm_ot_shape_complex_arabic_prev(hm_face_t *face, hm_array_t *glyphs, size_t index)
{
    hm_arabic_joining_entry_t entry;
    hm_id char_id;

    if (index == 0) {
        goto no_prev;
    }

    char_id = hm_array_at(glyphs, index - 1);
    if (hm_ot_shape_complex_arabic_char_joining(face, char_id, &entry)) {
        return entry.joining;
    }

    no_prev:
    /* No previous harf, return non-joining */
    return NO_JOINING_GROUP | JOINING_TYPE_T;
}

uint16_t
hm_ot_shape_complex_arabic_next(hm_face_t *face, hm_array_t *glyphs, size_t index)
{
    hm_arabic_joining_entry_t entry;
    hm_id char_id;
    size_t len = hm_array_size(glyphs);

    if (index >= len - 1) {
        goto no_next;
    }

    char_id = hm_array_at(glyphs, index + 1);
    if (hm_ot_shape_complex_arabic_char_joining(face, char_id, &entry)) {
        return entry.joining;
    }

    no_next:
    /* No previous harf, return non-joining */
    return NO_JOINING_GROUP | JOINING_TYPE_T;
}

hm_bool
hm_ot_shape_complex_arabic_medi_cond(hm_face_t *face, hm_array_t *glyphs, size_t index)
{
    uint16_t prev, curr, next;
    hm_id curr_id;
    hm_arabic_joining_entry_t curr_entry;

    curr_id = hm_array_at(glyphs, index);
    if (hm_ot_shape_complex_arabic_char_joining(face, curr_id, &curr_entry)) {
        curr = curr_entry.joining;

        prev = hm_ot_shape_complex_arabic_prev(face, glyphs, index);
        next = hm_ot_shape_complex_arabic_next(face, glyphs, index);

        /* Condition for substitution */
        if (curr & JOINING_TYPE_D
        && prev & (JOINING_TYPE_L | JOINING_TYPE_D | JOINING_TYPE_C)
        && next & (JOINING_TYPE_R | JOINING_TYPE_D | JOINING_TYPE_C)) {
            return HM_TRUE;
        }
    }

    return HM_FALSE;
}

hm_bool
hm_ot_shape_complex_arabic_init_cond(hm_face_t *face, hm_array_t *glyphs, size_t index)
{
    uint16_t curr, next;
    hm_id curr_id;
    hm_arabic_joining_entry_t curr_entry;

    curr_id = hm_array_at(glyphs, index);
    if (hm_ot_shape_complex_arabic_char_joining(face, curr_id, &curr_entry)) {
        curr = curr_entry.joining;

        next = hm_ot_shape_complex_arabic_next(face, glyphs, index);

        /* Condition for substitution */
        if (curr & (JOINING_TYPE_L | JOINING_TYPE_C)
            && (next & (JOINING_TYPE_R | JOINING_TYPE_D | JOINING_TYPE_C))) {
            return HM_TRUE;
        }
    }

    return HM_FALSE;
}

hm_bool
hm_ot_shape_complex_arabic_fina_cond(hm_face_t *face, hm_array_t *glyphs, size_t index)
{
    uint16_t curr, prev;
    hm_id curr_id;
    hm_arabic_joining_entry_t curr_entry;

    curr_id = hm_array_at(glyphs, index);
    if (hm_ot_shape_complex_arabic_char_joining(face, curr_id, &curr_entry)) {
        curr = curr_entry.joining;

        prev = hm_ot_shape_complex_arabic_prev(face, glyphs, index);

        /* Condition for substitution */
        if (curr & (JOINING_TYPE_R | JOINING_TYPE_C)
            && (prev & (JOINING_TYPE_L | JOINING_TYPE_D | JOINING_TYPE_C))) {
            return HM_TRUE;
        }
    }

    return HM_FALSE;
}

hm_bool
hm_ot_shape_complex_arabic_isol_cond(hm_face_t *face, hm_array_t *glyphs, size_t index)
{
    uint16_t prev, curr, next;
    hm_id curr_id;
    hm_arabic_joining_entry_t curr_entry;

    curr_id = hm_array_at(glyphs, index);
    if (hm_ot_shape_complex_arabic_char_joining(face, curr_id, &curr_entry)) {
        curr = curr_entry.joining;

        prev = hm_ot_shape_complex_arabic_prev(face, glyphs, index);
        next = hm_ot_shape_complex_arabic_next(face, glyphs, index);

        /* Condition for substitution */
        if (curr & JOINING_TYPE_D
            && (prev & (JOINING_TYPE_L | JOINING_TYPE_D | JOINING_TYPE_C))
            && (next & (JOINING_TYPE_R | JOINING_TYPE_D | JOINING_TYPE_C))) {
            return HM_TRUE;
        }
    }

    return HM_FALSE;
}