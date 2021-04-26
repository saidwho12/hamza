#ifndef HZ_ARRAY_H
#define HZ_ARRAY_H

#include "../hz-base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hz_array_t hz_array_t;

hz_array_t *
hz_array_create(void);

void
hz_array_push_back(hz_array_t *array, uint32_t val);

void
hz_array_pop_back(hz_array_t *array);

size_t
hz_array_size(const hz_array_t *array);

hz_bool
hz_array_is_empty(const hz_array_t *array);

void
hz_array_resize(hz_array_t *array, size_t new_size);

void
hz_array_pop_at(hz_array_t *array, size_t index);

void
hz_array_pop_range_at(hz_array_t *array, size_t index, size_t count);

void
hz_array_insert(hz_array_t *array, size_t index, uint32_t val);

void
hz_array_destroy(hz_array_t *array);

uint32_t
hz_array_at(const hz_array_t *array, size_t index);

hz_bool
hz_array_has(hz_array_t *array, uint32_t val, size_t *val_idx);

void
hz_array_set(hz_array_t *array, size_t index, uint32_t val);


hz_bool
hz_array_range_eq(const hz_array_t *a_arr, size_t a_index,
                  const hz_array_t *b_arr, size_t b_index,
                  size_t len);

#ifdef __cplusplus
}
#endif

#endif /* HZ_ARRAY_H */