#include "hz-set.h"

hz_set_t *
hz_set_create(void)
{
    hz_set_t *set = HZ_ALLOC(hz_set_t);
    set->count = 0;
    set->values = NULL;
    return set;
}

void
hz_set_add(hz_set_t *set, uint32_t val)
{
    size_t new_count = set->count + 1;

    if (hz_set_is_empty(set))
        set->values = (uint32_t *) HZ_MALLOC(sizeof(uint32_t) * new_count);
    else
        set->values = (uint32_t *) HZ_REALLOC((void *)set->values,
                                     new_count * sizeof(uint32_t));

    set->values[new_count - 1] = val;
    set->count = new_count;
}

hz_bool_t
hz_set_add_no_duplicate(hz_set_t *set, uint32_t val)
{
    hz_bool_t has_copy = 0;
    int i = 0;
    while (i < set->count) {
        if (set->values[i] == val) {
            has_copy = 1;
            break;
        }

        ++i;
    }

    if (!has_copy)
        hz_set_add(set, val);

    return has_copy;
}

void
hz_set_add_range(hz_set_t *set, uint32_t v1, uint32_t v2)
{
    int32_t del = (int32_t)v2 - (int32_t)v1;
    size_t len = del >= 0 ? del : -del;
    size_t new_count = set->count + len;
    size_t i = 0;

    if (hz_set_is_empty(set))
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
hz_set_add_range_no_duplicate(hz_set_t *set, uint32_t v1, uint32_t v2)
{
    uint32_t val = v1;

    while (val <= v2) {
        hz_set_add_no_duplicate(set, val);
        ++ val;
    }
}

void
hz_set_clear (hz_set_t *set)
{

}

void
hz_set_del(hz_set_t *set, uint32_t val)
{

}

void
hz_set_del_range(hz_set_t *set, uint32_t v1, uint32_t v2)
{

}

void
hz_set_destroy(hz_set_t *set)
{

}

hz_bool_t
hz_set_has(const hz_set_t *set, uint32_t val)
{
    size_t i = 0;
    if (hz_set_is_empty(set))
        return HZ_FALSE;

    while (i < set->count) {
        if (set->values[i] == val) return HZ_TRUE;
        ++i;
    }

    return HZ_FALSE;
}

hz_bool_t
hz_set_is_empty(const hz_set_t *set)
{
    return !set->count || set->values == NULL;
}