#include "hm-map.h"

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

struct hm_map_t {
    struct map_bucket *buckets;
    size_t bucket_count;
};

hm_map_t *
hm_map_create(void)
{
    hm_map_t *map = HM_MALLOC(sizeof(hm_map_t));
    map->bucket_count = BUCKET_COUNT;
    map->buckets = HM_MALLOC(sizeof(struct map_bucket) * map->bucket_count);
    size_t i = 0;

    while (i < BUCKET_COUNT) {
        bucket_init(&map->buckets[i]);
        ++i;
    }

    return map;
}

void
hm_map_destroy(hm_map_t *map)
{

}

hm_bool
hm_map_set_value(hm_map_t *map, uint32_t key, uint32_t value)
{
    uint32_t hash = hash_fnv1a(key);
    size_t index = hash % map->bucket_count;

    struct map_bucket *bucket = &map->buckets[index];

    if (bucket->root == NULL) {
        struct map_bucket_node *new_node = HM_MALLOC(sizeof(struct map_bucket_node));
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
                return HM_TRUE;
            }

            curr_node = curr_node->next;
        }

        struct map_bucket_node *new_node = HM_MALLOC(sizeof(struct map_bucket_node));
        new_node->prev = curr_node;
        new_node->value = value;
        new_node->key = key;
        new_node->next = NULL;
        curr_node->next = new_node;
    }

    return HM_FALSE;
}

uint32_t
hm_map_get_value(hm_map_t *map, uint32_t key)
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
hm_map_remove(hm_map_t *map, uint32_t key)
{

}

hm_bool
hm_map_value_exists(hm_map_t *map, uint32_t key)
{
    uint32_t hash = hash_fnv1a(key);
    size_t index = hash % map->bucket_count;
    struct map_bucket *bucket = &map->buckets[index];

    if (bucket->root != NULL) {
        struct map_bucket_node *curr_node = bucket->root;

        do {
            if (curr_node->key == key) {
                return HM_TRUE;
            }

            curr_node = curr_node->next;
        } while (curr_node != NULL);
    }

    return HM_FALSE;
}


hm_bool
hm_map_set_value_for_keys(hm_map_t *map, uint32_t k0, uint32_t k1, uint32_t value)
{
    hm_bool any_set = HM_FALSE;
    uint32_t k = k0;

    while (k <= k1) {
        if (hm_map_set_value(map, k, value))
            any_set = HM_TRUE;

        ++k;
    }

    return any_set;
}








