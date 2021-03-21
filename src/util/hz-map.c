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

struct map_bucket_node {
    struct map_bucket_node *prev, *next;
    uint32_t key;
    uint32_t value;
};

struct map_bucket {
    struct map_bucket_node *root;
};

static void bucket_init(struct map_bucket *b) {
    b->root = NULL;
}

struct hz_map_t {
    struct map_bucket *buckets;
    size_t bucket_count;
};

hz_map_t *
hz_map_create(void)
{
    hz_map_t *map = HZ_MALLOC(sizeof(hz_map_t));
    map->bucket_count = BUCKET_COUNT;
    map->buckets = HZ_MALLOC(sizeof(struct map_bucket) * map->bucket_count);
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

}

hz_bool_t
hz_map_set_value(hz_map_t *map, uint32_t key, uint32_t value)
{
    uint32_t hash = hash_fnv1a(key);
    size_t index = hash % map->bucket_count;

    struct map_bucket *bucket = &map->buckets[index];

    if (bucket->root == NULL) {
        struct map_bucket_node *new_node = HZ_MALLOC(sizeof(struct map_bucket_node));
        new_node->prev = NULL;
        new_node->next = NULL;
        new_node->value = value;
        new_node->key = key;
        bucket->root = new_node;
    } else {
        /* loop over nodes, if one with equal key is found set value, otherwise insert node */
        struct map_bucket_node *curr_node = bucket->root;
        while (curr_node->next != NULL) {
            if (curr_node->key == key) {
                curr_node->value = value;
                return HZ_TRUE;
            }

            curr_node = curr_node->next;
        }

        struct map_bucket_node *new_node = HZ_MALLOC(sizeof(struct map_bucket_node));
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
    struct map_bucket *bucket = &map->buckets[index];

    if (bucket->root != NULL) {
        struct map_bucket_node *curr_node = bucket->root;

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

hz_bool_t
hz_map_value_exists(hz_map_t *map, uint32_t key)
{
    uint32_t hash = hash_fnv1a(key);
    size_t index = hash % map->bucket_count;
    struct map_bucket *bucket = &map->buckets[index];

    if (bucket->root != NULL) {
        struct map_bucket_node *curr_node = bucket->root;

        do {
            if (curr_node->key == key) {
                return HZ_TRUE;
            }

            curr_node = curr_node->next;
        } while (curr_node != NULL);
    }

    return HZ_FALSE;
}


hz_bool_t
hz_map_set_value_for_keys(hz_map_t *map, uint32_t k0, uint32_t k1, uint32_t value)
{
    hz_bool_t any_set = HZ_FALSE;
    uint32_t k = k0;

    while (k <= k1) {
        if (hz_map_set_value(map, k, value))
            any_set = HZ_TRUE;

        ++k;
    }

    return any_set;
}








