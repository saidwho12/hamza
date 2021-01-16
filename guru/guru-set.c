#include "guru-set.h"

struct guru_set_t {
    guru_uint64 *values;
    size_t count;
};

guru_set_t *
guru_set_create(void)
{
    guru_set_t *set = GURU_ALLOC(guru_set_t);
    set->count = 0;
    set->values = NULL;
    return set;
}

void
guru_set_add(guru_set_t *set, guru_uint64 val)
{
    size_t new_count = set->count + 1;

    if (guru_set_is_empty(set))
        set->values = (guru_uint64 *) GURU_MALLOC(sizeof(guru_uint64) * new_count);
    else
        set->values = (guru_uint64 *) GURU_REALLOC((void *)set->values,
                                     new_count * sizeof(guru_uint64));

    set->values[new_count - 1] = val;
    set->count = new_count;
}

void
guru_set_add_range(guru_set_t *set, guru_uint64 v1, guru_uint64 v2)
{
    guru_int64 del = (guru_int64)v2 - (guru_int64)v1;
    size_t len = del >= 0 ? del : -del;
    size_t new_count = set->count + len;
    size_t i = 0;

    if (guru_set_is_empty(set))
        set->values = (guru_uint64 *) malloc(sizeof(guru_uint64) * new_count);
    else
        set->values = (guru_uint64 *) realloc((void *)set->values,
                                     new_count * sizeof(guru_uint64));

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
guru_set_del(guru_set_t *set, guru_uint64 val)
{

}

void
guru_set_del_range(guru_set_t *set, guru_uint64 v1, guru_uint64 v2)
{

}

void
guru_set_destroy(guru_set_t *set)
{

}

guru_bool
guru_set_has(const guru_set_t *set, guru_uint64 val)
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