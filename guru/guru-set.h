#ifndef GURU_SET_H
#define GURU_SET_H

#include "guru-base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct guru_set_t {
    uint32_t *values;
    size_t count;
} guru_set_t;

guru_set_t *
guru_set_create(void);

void
guru_set_add(guru_set_t *set, uint32_t val);

void
guru_set_add_range(guru_set_t *set, uint32_t v1, uint32_t v2);

void
guru_set_clear(guru_set_t *set);

void
guru_set_del(guru_set_t *set, uint32_t val);

void
guru_set_del_range(guru_set_t *set, uint32_t v1, uint32_t v2);

void
guru_set_destroy(guru_set_t *set);

guru_bool
guru_set_has(const guru_set_t *set, uint32_t val);

guru_bool
guru_set_is_empty(const guru_set_t *set);

#ifdef __cplusplus
}
#endif

#endif /* GURU_SET_H */
