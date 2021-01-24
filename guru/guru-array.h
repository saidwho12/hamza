#ifndef GURU_ARRAY_H
#define GURU_ARRAY_H

#include "guru-base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct guru_array_t guru_array_t;

guru_array_t *
guru_array_create(void);

void
guru_array_push_back(guru_array_t *array, uint32_t val);

void
guru_array_pop_back(guru_array_t *array);

size_t
guru_array_size(guru_array_t *array);

void
guru_array_pop_at(guru_array_t *array, size_t index);

void
guru_array_pop_range_at(guru_array_t *array, size_t index, size_t count);

void
guru_array_insert(guru_array_t *array, size_t index, uint32_t val);

void
guru_array_destroy(guru_array_t *array);

uint32_t
guru_array_at(guru_array_t *array, size_t index);

guru_bool
guru_array_has(guru_array_t *array, uint32_t val, size_t *val_idx);

void
guru_array_set(guru_array_t *array, size_t index, uint32_t val);

#ifdef __cplusplus
};
#endif

#endif /* GURU_ARRAY_H */