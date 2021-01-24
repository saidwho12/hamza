#include "hm-set.h"

hm_set_t *
hm_set_create(void)
{
    hm_set_t *set = HM_ALLOC(hm_set_t);
    set->count = 0;
    set->values = NULL;
    return set;
}

void
hm_set_add(hm_set_t *set, uint32_t val)
{
    size_t new_count = set->count + 1;

    if (hm_set_is_empty(set))
        set->values = (uint32_t *) HM_MALLOC(sizeof(uint32_t) * new_count);
    else
        set->values = (uint32_t *) HM_REALLOC((void *)set->values,
                                     new_count * sizeof(uint32_t));

    set->values[new_count - 1] = val;
    set->count = new_count;
}

void
hm_set_add_range(hm_set_t *set, uint32_t v1, uint32_t v2)
{
    int32_t del = (int32_t)v2 - (int32_t)v1;
    size_t len = del >= 0 ? del : -del;
    size_t new_count = set->count + len;
    size_t i = 0;

    if (hm_set_is_empty(set))
        set->values = (uint32_t *) malloc(sizeof(uint32_t) * new_count);
    else
        set->values = (uint32_t *) realloc((void *)set->values,
                                     new_count * sizeof(uint32_t));

    while (i < len) {
        set->values[set->count + i] = v1 + i;
        ++i;
    }

    set->count = new_count;
}

void
hm_set_clear (hm_set_t *set)
{

}

void
hm_set_del(hm_set_t *set, uint32_t val)
{

}

void
hm_set_del_range(hm_set_t *set, uint32_t v1, uint32_t v2)
{

}

void
hm_set_destroy(hm_set_t *set)
{

}

hm_bool
hm_set_has(const hm_set_t *set, uint32_t val)
{
    size_t i = 0;
    if (hm_set_is_empty(set))
        return HM_FALSE;

    while (i < set->count) {
        if (set->values[i] == val) return HM_TRUE;
        ++i;
    }

    return HM_FALSE;
}

hm_bool
hm_set_is_empty(const hm_set_t *set)
{
    return !set->count || set->values == NULL;
}