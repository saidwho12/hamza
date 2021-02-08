#ifndef CMAS_H
#define CMAS_H

#include <inttypes.h>

typedef struct cmas_mono_ma_t {
    uint8_t *data;
    uint16_t size;
    uint16_t bump_ptr;
    uint8_t flags;
} cmas_mono_ma_t;

static cmas_mono_ma_t cmas_mono_ma_create(uint8_t *data, size_t size);
static void *cmas_mono_ma_alloc(cmas_mono_ma_t *ma, size_t n);
static void cmas_mono_ma_free(cmas_mono_ma_t *ma, void *p);
static void cmas_mono_ma_release(cmas_mono_ma_t *ma);

#ifdef CMAS_IMPL
static cmas_mono_ma_t cmas_mono_ma_create(uint8_t *data, size_t size) {
    cmas_mono_ma_t ma;
    ma.data = data;
    ma.size = size;
    ma.bump_ptr = 0;
    ma.flags = 0;
    return ma;
}

static void *cmas_mono_ma_alloc(cmas_mono_ma_t *ma, size_t n) {
    void *addr;
    if (ma->bump_ptr + n > ma->size) return NULL;

    addr = ma->data + ma->bump_ptr;
    ma->bump_ptr += n;
    return addr;
}

static void cmas_mono_ma_free(cmas_mono_ma_t *ma, void *p) {
    /* no-op */
}

static void cmas_mono_ma_release(cmas_mono_ma_t *ma) {
    /* no-op */
}

#endif

#endif /* CMAS_H */