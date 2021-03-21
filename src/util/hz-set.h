#ifndef HZ_SET_H
#define HZ_SET_H

#include "../hz-base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hz_set_t {
    uint32_t *values;
    size_t count;
} hz_set_t;

hz_set_t *
hz_set_create(void);

void
hz_set_add(hz_set_t *set, uint32_t val);

hz_bool_t
hz_set_add_no_duplicate(hz_set_t *set, uint32_t val);

void
hz_set_add_range(hz_set_t *set, uint32_t v1, uint32_t v2);

void
hz_set_add_range_no_duplicate(hz_set_t *set, uint32_t v1, uint32_t v2);

void
hz_set_clear(hz_set_t *set);

void
hz_set_del(hz_set_t *set, uint32_t val);

void
hz_set_del_range(hz_set_t *set, uint32_t v1, uint32_t v2);

void
hz_set_destroy(hz_set_t *set);

hz_bool_t
hz_set_has(const hz_set_t *set, uint32_t val);

hz_bool_t
hz_set_is_empty(const hz_set_t *set);

#ifdef __cplusplus
}
#endif

#endif /* HZ_SET_H */
