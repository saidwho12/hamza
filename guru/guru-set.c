#include "guru-set.h"

guru_set_t *
guru_set_create(void)
{
    guru_set_t *set = GURU_ALLOC(guru_set_t);
    set->count = 0;
    set->values = NULL;
    return set;
}

void
guru_set_add(guru_set_t *set, uint32_t val)
{
    size_t new_count = set->count + 1;

    if (guru_set_is_empty(set))
        set->values = (uint32_t *) GURU_MALLOC(sizeof(uint32_t) * new_count);
    else
        set->values = (uint32_t *) GURU_REALLOC((void *)set->values,
                                     new_count * sizeof(uint32_t));

    set->values[new_count - 1] = val;
    set->count = new_count;
}

void
guru_set_add_range(guru_set_t *set, uint32_t v1, uint32_t v2)
{
    int32_t del = (int32_t)v2 - (int32_t)v1;
    size_t len = del >= 0 ? del : -del;
    size_t new_count = set->count + len;
    size_t i = 0;

    if (guru_set_is_empty(set))
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
guru_set_clear (guru_set_t *set)
{

}

void
guru_set_del(guru_set_t *set, uint32_t val)
{

}

void
guru_set_del_range(guru_set_t *set, uint32_t v1, uint32_t v2)
{

}

void
guru_set_destroy(guru_set_t *set)
{

}

guru_bool
guru_set_has(const guru_set_t *set, uint32_t val)
{
    size_t i = 0;
    if (guru_set_is_empty(set))
        return GURU_FALSE;

    while (i < set->count) {
        if (set->values[i] == val) return GURU_TRUE;
        ++i;
    }

    return GURU_FALSE;
}

guru_bool
guru_set_is_empty(const guru_set_t *set)
{
    return !set->count || set->values == NULL;
}