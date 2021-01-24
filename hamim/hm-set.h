#ifndef HM_SET_H
#define HM_SET_H

#include "hm-base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hm_set_t {
    uint32_t *values;
    size_t count;
} hm_set_t;

hm_set_t *
hm_set_create(void);

void
hm_set_add(hm_set_t *set, uint32_t val);

void
hm_set_add_range(hm_set_t *set, uint32_t v1, uint32_t v2);

void
hm_set_clear(hm_set_t *set);

void
hm_set_del(hm_set_t *set, uint32_t val);

void
hm_set_del_range(hm_set_t *set, uint32_t v1, uint32_t v2);

void
hm_set_destroy(hm_set_t *set);

hm_bool
hm_set_has(const hm_set_t *set, uint32_t val);

hm_bool
hm_set_is_empty(const hm_set_t *set);

#ifdef __cplusplus
}
#endif

#endif /* HM_SET_H */
