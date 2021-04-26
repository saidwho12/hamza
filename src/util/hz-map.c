#include "hz-map.h"

#define BUCKET_COUNT 64

static
uint32_t hash_fnv1a(uint32_t val) {
    static const uint32_t FNV_offset_basis = 0x811c9dc5;
    static const uint32_t FNV_prime = 0x01000193;

    uint32_t hash = FNV_offset_basis;
    size_t byte_index = 0;
    uint8_t byte_of_data, bytes[4] = { val & 0xFF, (val >> 8) & 0xFF, (val >> 16) & 0xFF, val >> 24 };

    while (byte_index < 4) {
        byte_of_data = bytes[byte_index];
        hash ^= byte_of_data;
        hash *= FNV_prime;
        ++byte_index;
    }

    return hash;
}

typedef struct hz_map_bucket_node_t {
    struct hz_map_bucket_node_t *prev, *next;
    uint32_t key;
    uint32_t value;
} hz_map_bucket_node_t;

typedef struct hz_map_bucket_t {
    struct hz_map_bucket_node_t *root;
} hz_map_bucket_t;

static void bucket_init(hz_map_bucket_t *b) {
    b->root = NULL;
}

struct hz_map_t {
    hz_map_bucket_t *buckets;
    size_t bucket_count;
};

hz_map_t *
hz_map_create(void)
{
    hz_map_t *map = HZ_MALLOC(sizeof(hz_map_t));
    map->bucket_count = BUCKET_COUNT;
    map->buckets = HZ_MALLOC(sizeof(hz_map_bucket_t) * map->bucket_count);
    size_t i = 0;

    while (i < BUCKET_COUNT) {
        bucket_init(&map->buckets[i]);
        ++i;
    }

    return map;
}

void
hz_map_destroy(hz_map_t *map)
{
    size_t i;

    for (i = 0; i < map->bucket_count; ++i) {
        hz_map_bucket_t *bucket = &map->buckets[i];
        if (bucket != NULL) {
            hz_map_bucket_node_t *node = bucket->root;

            while (node != NULL) {
                hz_map_bucket_node_t *tmp = node;
                node = node->next;
                HZ_FREE(tmp);
            }

            bucket->root = NULL;
        }
    }

    HZ_FREE(map->buckets);
    HZ_FREE(map);
}

hz_bool
hz_map_set_value(hz_map_t *map, uint32_t key, uint32_t value)
{
    uint32_t hash = hash_fnv1a(key);
    size_t index = hash % map->bucket_count;

    hz_map_bucket_t *bucket = &map->buckets[index];

    if (bucket->root == NULL) {
        hz_map_bucket_node_t *new_node = HZ_MALLOC(sizeof(hz_map_bucket_node_t));
        new_node->prev = NULL;
        new_node->next = NULL;
        new_node->value = value;
        new_node->key = key;
        bucket->root = new_node;
    } else {
        /* loop over nodes, if one with equal key is found set value, otherwise insert node */
        hz_map_bucket_node_t *curr_node = bucket->root;
        while (curr_node->next != NULL) {
            if (curr_node->key == key) {
                curr_node->value = value;
                return HZ_TRUE;
            }

            curr_node = curr_node->next;
        }

        hz_map_bucket_node_t *new_node = HZ_MALLOC(sizeof(hz_map_bucket_node_t));
        new_node->prev = curr_node;
        new_node->value = value;
        new_node->key = key;
        new_node->next = NULL;
        curr_node->next = new_node;
    }

    return HZ_FALSE;
}

uint32_t
hz_map_get_value(hz_map_t *map, uint32_t key)
{
    uint32_t hash = hash_fnv1a(key);
    size_t index = hash % map->bucket_count;
    hz_map_bucket_t *bucket = &map->buckets[index];

    if (bucket->root != NULL) {
        hz_map_bucket_node_t *curr_node = bucket->root;

        /* if only single node is the root of bucket, no need to compare keys  */
//        if (curr_node->next == NULL && curr_node->key == key) {
//            return curr_node->value;
//        }

        while (curr_node != NULL) {
            if (curr_node->key == key) {
                return curr_node->value;
            }

            curr_node = curr_node->next;
        }
    }

    return 0;
}

void
hz_map_remove(hz_map_t *map, uint32_t key)
{

}

hz_bool
hz_map_value_exists(hz_map_t *map, uint32_t key)
{
    uint32_t hash = hash_fnv1a(key);
    size_t index = hash % map->bucket_count;
    hz_map_bucket_t *bucket = &map->buckets[index];

    if (bucket->root != NULL) {
        hz_map_bucket_node_t *curr_node = bucket->root;

        do {
            if (curr_node->key == key) {
                return HZ_TRUE;
            }

            curr_node = curr_node->next;
        } while (curr_node != NULL);
    }

    return HZ_FALSE;
}


hz_bool
hz_map_set_value_for_keys(hz_map_t *map, uint32_t k0, uint32_t k1, uint32_t value)
{
    hz_bool any_set = HZ_FALSE;
    uint32_t k = k0;

    while (k <= k1) {
        if (hz_map_set_value(map, k, value))
            any_set = HZ_TRUE;

        ++k;
    }

    return any_set;
}








