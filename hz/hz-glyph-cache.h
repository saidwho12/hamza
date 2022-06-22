#ifndef HZ_GLYPH_CACHE_H
#define HZ_GLYPH_CACHE_H

#include "hz.h"

typedef enum {
    HZ_CACHE_REPLACEMENT_POLICY_2Q,
    HZ_CACHE_REPLACEMENT_POLICY_LRU,
    HZ_CACHE_REPLACEMENT_POLICY_PSEUDO_LRU,
    HZ_CACHE_REPLACEMENT_POLICY_LFU,
    HZ_CACHE_REPLACEMENT_POLICY_FIFO
} HzCacheReplacementPolicy;

typedef struct {
    HzCacheReplacementPolicy policy;
} HzGlyphCache;

HZDECL HzGlyphCache *
hzGlyphCacheCreate();

HZDECL void
hzGlyphCacheDestroy(HzGlyphCache *glyph_cache);

typedef struct hz_glyph_cache_config_t {
    int width;
    int height;
    int grid_divisions;
    float distance_scale;
    float margin;
} hz_glyph_cache_config_t;

#endif /* HZ_GLYPH_CACHE_H */