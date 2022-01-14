#ifndef HZ_GLYPH_CACHE_H
#define HZ_GLYPH_CACHE_H

#include "hz.h"

typedef enum hz_cache_replacement_policy_t {
    HZ_CACHE_REPLACEMENT_POLICY_2Q,
    HZ_CACHE_REPLACEMENT_POLICY_LRU,
    HZ_CACHE_REPLACEMENT_POLICY_PSEUDO_LRU,
    HZ_CACHE_REPLACEMENT_POLICY_LFU,
    HZ_CACHE_REPLACEMENT_POLICY_FIFO
} hz_cache_replacement_policy_t;

typedef struct hz_glyph_cache_t {
    hz_cache_replacement_policy_t policy;
} hz_glyph_cache_t;

HZ_API hz_glyph_cache_t *
hz_glyph_cache_create();

HZ_API void
hz_glyph_cache_destroy(hz_glyph_cache_t *glyph_cache);


#endif /* HZ_GLYPH_CACHE_H */