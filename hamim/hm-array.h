#ifndef HM_ARRAY_H
#define HM_ARRAY_H

#include "hm-base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hm_array_t hm_array_t;

hm_array_t *
hm_array_create(void);

void
hm_array_push_back(hm_array_t *array, uint32_t val);

void
hm_array_pop_back(hm_array_t *array);

size_t
hm_array_size(hm_array_t *array);

void
hm_array_pop_at(hm_array_t *array, size_t index);

void
hm_array_pop_range_at(hm_array_t *array, size_t index, size_t count);

void
hm_array_insert(hm_array_t *array, size_t index, uint32_t val);

void
hm_array_destroy(hm_array_t *array);

uint32_t
hm_array_at(hm_array_t *array, size_t index);

hm_bool
hm_array_has(hm_array_t *array, uint32_t val, size_t *val_idx);

void
hm_array_set(hm_array_t *array, size_t index, uint32_t val);

#ifdef __cplusplus
};
#endif

#endif /* HM_ARRAY_H */