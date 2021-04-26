#ifndef CMAS_H
#define CMAS_H

#include <inttypes.h>

typedef struct hz_bump_allocator_t {
    uint8_t *data;
    uint16_t size;
    uint16_t bump_ptr;
    uint8_t flags;
} hz_bump_allocator_t;

static void hz_bump_allocator_init(hz_bump_allocator_t *ma, uint8_t *data, size_t size);
static void *hz_bump_allocator_alloc(hz_bump_allocator_t *ma, size_t n);
static void hz_bump_allocator_free(hz_bump_allocator_t *ma, void *p);
static void hz_bump_allocator_release(hz_bump_allocator_t *ma);

#ifdef CMAS_IMPL
void hz_bump_allocator_init(hz_bump_allocator_t *ma, uint8_t *data, size_t size) {
    ma->data = data;
    ma->size = size;
    ma->bump_ptr = 0;
    ma->flags = 0;
}

static void *hz_bump_allocator_alloc(hz_bump_allocator_t *ma, size_t n) {
    void *addr;
    if (ma->bump_ptr + n > ma->size) return NULL;

    addr = ma->data + ma->bump_ptr;
    ma->bump_ptr += n;
    return addr;
}

static void hz_bump_allocator_free(hz_bump_allocator_t *ma, void *p) {
    /* no-op */
}

static void hz_bump_allocator_release(hz_bump_allocator_t *ma) {
    /* no-op */
}


#define hz_bump_allocator_stack_init(A, B) hz_bump_allocator_init(A, &B[0], sizeof(B))

#endif

#endif /* CMAS_H */