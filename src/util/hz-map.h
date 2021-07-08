#ifndef HZ_MAP_H
#define HZ_MAP_H

#include "../hz-base.h"

typedef struct hz_map_t hz_map_t;

hz_map_t *
hz_map_create(void);

void
hz_map_destroy(hz_map_t *map);

hz_bool_t
hz_map_set_value(hz_map_t *map, uint32_t key, uint32_t value);

uint32_t
hz_map_get_value(hz_map_t *map, uint32_t key);

void
hz_map_remove(hz_map_t *map, uint32_t key);

hz_bool_t
hz_map_value_exists(hz_map_t *map, uint32_t key);

hz_bool_t
hz_map_set_value_for_keys(hz_map_t *map, uint32_t k0, uint32_t k1, uint32_t value);


#endif /* HZ_MAP_H */
