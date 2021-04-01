#ifndef CMAS_H
#define CMAS_H

#include <inttypes.h>

typedef struct cmas_mono_ma_t {
    uint8_t *data;
    uint16_t size;
    uint16_t bump_ptr;
    uint8_t flags;
} hz_monotonic_allocator_t;

static hz_monotonic_allocator_t hz_monotonic_allocator_create(uint8_t *data, size_t size);
static void *hz_monotonic_allocator_alloc(hz_monotonic_allocator_t *ma, size_t n);
static void hz_monotonic_allocator_free(hz_monotonic_allocator_t *ma, void *p);
static void hz_monotonic_allocator_release(hz_monotonic_allocator_t *ma);

#ifdef CMAS_IMPL
static hz_monotonic_allocator_t hz_monotonic_allocator_create(uint8_t *data, size_t size) {
    hz_monotonic_allocator_t ma;
    ma.data = data;
    ma.size = size;
    ma.bump_ptr = 0;
    ma.flags = 0;
    return ma;
}

static void *hz_monotonic_allocator_alloc(hz_monotonic_allocator_t *ma, size_t n) {
    void *addr;
    if (ma->bump_ptr + n > ma->size) return NULL;

    addr = ma->data + ma->bump_ptr;
    ma->bump_ptr += n;
    return addr;
}

static void hz_monotonic_allocator_free(hz_monotonic_allocator_t *ma, void *p) {
    /* no-op */
}

static void hz_monotonic_allocator_release(hz_monotonic_allocator_t *ma) {
    /* no-op */
}

#endif

#endif /* CMAS_H */