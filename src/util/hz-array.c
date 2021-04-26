#include "hz-array.h"

struct hz_array_t {
    uint32_t *data;
    size_t size;
};

hz_array_t *
hz_array_create(void)
{
    hz_array_t *array = HZ_MALLOC(sizeof(hz_array_t));
    array->data = NULL;
    array->size = 0;
    return array;
}

void
hz_array_push_back(hz_array_t *array, uint32_t val)
{
    size_t new_size = array->size + 1;

    if (array->data == NULL)
        array->data = HZ_MALLOC(new_size * sizeof(uint32_t));
    else
        array->data = HZ_REALLOC(array->data, new_size * sizeof(uint32_t));

    array->data[new_size - 1] = val;
    array->size = new_size;
}

void
hz_array_pop_back(hz_array_t *array)
{

}

size_t
hz_array_size(const hz_array_t *array)
{
    return array->size;
}

hz_bool
hz_array_is_empty(const hz_array_t *array)
{
    return array->data == NULL || !array->size;
}

void
hz_array_resize(hz_array_t *array, size_t new_size)
{
    if (hz_array_is_empty(array))
        array->data = HZ_MALLOC(new_size * sizeof(uint32_t));
    else
        array->data = HZ_REALLOC(array->data, new_size * sizeof(uint32_t));

    array->size = new_size;
}

void
hz_array_pop_at(hz_array_t *array, size_t index)
{

}

void
hz_array_pop_range_at(hz_array_t *array, size_t index, size_t count)
{
    size_t new_size = array->size - count;

    size_t remaining_len = array->size - (index + count);
    uint32_t *remaining = HZ_MALLOC(remaining_len * sizeof(uint32_t));
    memcpy(remaining, &array->data[index + count], remaining_len * sizeof(uint32_t));

    array->data = HZ_REALLOC(array->data, new_size * sizeof(uint32_t));
    array->size = new_size;

    memcpy(&array->data[index], remaining, remaining_len * sizeof(uint32_t));
    HZ_FREE(remaining);
}

void
hz_array_insert(hz_array_t *array, size_t index, uint32_t val)
{
    if (index > array->size) return;

    if (index == array->size) {
        hz_array_push_back(array, val);
    } else {
        size_t new_size = array->size + 1;

        size_t sec_len = array->size - (index + 1);
        uint32_t *sec = HZ_MALLOC(sec_len * sizeof(uint32_t));
        memcpy(sec, &array->data[index], sec_len);

        array->data = HZ_REALLOC(array->data, new_size * sizeof(uint32_t));
        array->size = new_size;
        memcpy(&array->data[index + 1], sec, sec_len);
        array->data[index] = val;

        HZ_FREE(sec);
    }
}

void
hz_array_destroy(hz_array_t *array)
{
    HZ_FREE(array->data);
    HZ_FREE(array);
}

uint32_t
hz_array_at(const hz_array_t *array, size_t index)
{
    return array->data[index];
}

hz_bool
hz_array_has(hz_array_t *array, uint32_t val, size_t *val_idx)
{
    size_t index = 0;
    while (index < array->size) {
        if (array->data[index] == val) {
            if (val_idx != NULL)
                *val_idx = index;
            return HZ_TRUE;
        }
        ++index;
    }

    return HZ_FALSE;
}

void
hz_array_set(hz_array_t *array, size_t index, uint32_t val)
{
    array->data[index] = val;
}

hz_bool
hz_array_range_eq(const hz_array_t *a_arr, size_t a_index,
                  const hz_array_t *b_arr, size_t b_index,
                  size_t len)
{
    size_t offset = 0;
    HZ_ASSERT(a_arr != NULL && b_arr != NULL);
    HZ_ASSERT(len > 0);

    if(a_index + len > a_arr->size) return HZ_FALSE;
    if(b_index + len > b_arr->size) return HZ_FALSE;

    while (offset < len) {
        if (a_arr->data[a_index + offset] != b_arr->data[b_index + offset]) return HZ_FALSE;
        ++offset;
    }

    return HZ_TRUE;
}