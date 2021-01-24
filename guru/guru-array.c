#include "guru-array.h"

struct guru_array_t {
    uint32_t *data;
    size_t size;
};

guru_array_t *
guru_array_create(void)
{
    guru_array_t *array = GURU_MALLOC(sizeof(guru_array_t));
    array->data = NULL;
    array->size = 0;
    return array;
}

void
guru_array_push_back(guru_array_t *array, uint32_t val)
{
    size_t new_size = array->size + 1;

    if (array->data == NULL)
        array->data = GURU_MALLOC(new_size * sizeof(uint32_t));
    else
        array->data = GURU_REALLOC(array->data, new_size * sizeof(uint32_t));

    array->data[new_size - 1] = val;
    array->size = new_size;
}

void
guru_array_pop_back(guru_array_t *array)
{

}

size_t
guru_array_size(guru_array_t *array)
{
    return array->size;
}

void
guru_array_pop_at(guru_array_t *array, size_t index)
{

}

void
guru_array_pop_range_at(guru_array_t *array, size_t index, size_t count)
{

}

void
guru_array_insert(guru_array_t *array, size_t index, uint32_t val)
{

}

void
guru_array_destroy(guru_array_t *array)
{
    free(array->data);
    free(array);
}

uint32_t
guru_array_at(guru_array_t *array, size_t index)
{
    return array->data[index];
}

guru_bool
guru_array_has(guru_array_t *array, uint32_t val, size_t *val_idx)
{
    size_t index = 0;
    while (index < array->size) {
        if (array->data[index] == val) {
            if (val_idx != NULL)
                *val_idx = index;
            return GURU_TRUE;
        }
        ++index;
    }

    return GURU_FALSE;
}

void
guru_array_set(guru_array_t *array, size_t index, uint32_t val)
{
    array->data[index] = val;
}