#ifndef HM_MAP_H
#define HM_MAP_H

#include "hm-base.h"

typedef struct hm_map_t hm_map_t;

hm_map_t *
hm_map_create(void);

void
hm_map_destroy(hm_map_t *map);

/*
 * @return if true if value exists or false if was created
 * */
hm_bool
hm_map_set_value(hm_map_t *map, uint32_t key, uint32_t value);

uint32_t
hm_map_get_value(hm_map_t *map, uint32_t key);

void
hm_map_remove(hm_map_t *map, uint32_t key);

hm_bool
hm_map_value_exists(hm_map_t *map, uint32_t key);


#endif /* HM_MAP_H */
