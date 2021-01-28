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
    size_t new_size = array->size - count;

    size_t remaining_len = array->size - (index + count);
    uint32_t *remaining = HM_MALLOC(remaining_len * sizeof(uint32_t));
    memcpy(remaining, &array->data[index + count], remaining_len * sizeof(uint32_t));

    array->data = HM_REALLOC(array->data, new_size * sizeof(uint32_t));
    array->size = new_size;

    memcpy(&array->data[index], remaining, remaining_len * sizeof(uint32_t));
    HM_FREE(remaining);
}

void
hm_array_insert(hm_array_t *array, size_t index, uint32_t val)
{
    if (index > array->size) return;

    if (index == array->size) {
        hm_array_push_back(array, val);
    } else {
        size_t new_size = array->size + 1;

        size_t sec_len = array->size - (index + 1);
        uint32_t *sec = HM_MALLOC(sec_len * sizeof(uint32_t));
        memcpy(sec, &array->data[index], sec_len);

        array->data = HM_REALLOC(array->data, new_size * sizeof(uint32_t));
        array->size = new_size;
        memcpy(&array->data[index + 1], sec, sec_len);
        array->data[index] = val;

        HM_FREE(sec);
    }
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

hm_bool
hm_array_range_eq(const hm_array_t *a_arr, size_t a_index,
                  const hm_array_t *b_arr, size_t b_index,
                  size_t len)
{
    size_t offset = 0;
    HM_ASSERT(a_arr != NULL && b_arr != NULL);
    HM_ASSERT(len > 0);

    if(a_index + len > a_arr->size) return HM_FALSE;
    if(b_index + len > b_arr->size) return HM_FALSE;

    while (offset < len) {
        if (a_arr->data[a_index + offset] != b_arr->data[b_index + offset]) return HM_FALSE;
        ++offset;
    }

    return HM_TRUE;
}