#include "hm-array.h"

struct hm_array_t {
    uint32_t *data;
    size_t size;
};

hm_array_t *
hm_array_create(void)
{
    hm_array_t *array = HM_MALLOC(sizeof(hm_array_t));
    array->data = NULL;
    array->size = 0;
    return array;
}

void
hm_array_push_back(hm_array_t *array, uint32_t val)
{
    size_t new_size = array->size + 1;

    if (array->data == NULL)
        array->data = HM_MALLOC(new_size * sizeof(uint32_t));
    else
        array->data = HM_REALLOC(array->data, new_size * sizeof(uint32_t));

    array->data[new_size - 1] = val;
    array->size = new_size;
}

void
hm_array_pop_back(hm_array_t *array)
{

}

size_t
hm_array_size(hm_array_t *array)
{
    return array->size;
}

void
hm_array_pop_at(hm_array_t *array, size_t index)
{

}

void
hm_array_pop_range_at(hm_array_t *array, size_t index, size_t count)
{

}

void
hm_array_insert(hm_array_t *array, size_t index, uint32_t val)
{

}

void
hm_array_destroy(hm_array_t *array)
{
    free(array->data);
    free(array);
}

uint32_t
hm_array_at(hm_array_t *array, size_t index)
{
    return array->data[index];
}

hm_bool
hm_array_has(hm_array_t *array, uint32_t val, size_t *val_idx)
{
    size_t index = 0;
    while (index < array->size) {
        if (array->data[index] == val) {
            if (val_idx != NULL)
                *val_idx = index;
            return HM_TRUE;
        }
        ++index;
    }

    return HM_FALSE;
}

void
hm_array_set(hm_array_t *array, size_t index, uint32_t val)
{
    array->data[index] = val;
}