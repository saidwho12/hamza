#ifndef HZ_ALLOCATOR_H
#define HZ_ALLOCATOR_H

typedef void (* hz_alloc_func_t)(void *user, size_t size);
typedef void (* hz_free_func_t)(void *user, void *p);

typedef struct hz_global_allocator_t {
    hz_malloc_func_t allocfn;
    hz_free_func_t freefn;
    void *user;
} hz_global_allocator_t;


static hz_global_allocator_t global_allocator = { malloc, free, NULL };




#endif /* HZ_ALLOCATOR_H */
