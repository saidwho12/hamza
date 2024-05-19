# Advanced Usage

## Memory Management
### Custom Allocator
For those wanting to control the memory allocation of their programs, compiling without the CRT, or working within an embedded environment, you can provide a callback function with the following signature:

```c
void* (*)(void* userptr, hz_allocator_cmd_t cmd, void* ptr, size_t size, size_t alignment);
```

Below is how a very simple allocator is written using the CRT's `malloc`, `free`, and `realloc`:
```c
void *allocfn(void *userptr, hz_allocator_cmd_t cmd, void *ptr, size_t size, size_t alignment) {
    HZ_IGNORE_ARG(userptr);
    HZ_IGNORE_ARG(alignment);
    
    switch (cmd) {
    case HZ_CMD_ALLOC:
        return malloc(size);
    case HZ_CMD_REALLOC:
        return realloc(ptr, size);
    case HZ_CMD_FREE:
        free(ptr);
        return (void*)0;
    }
}
```
Here, the `alignment` argument is ignored for simplicity, you are however _required_ to implement aligned allocation ::when SIMD is enabled:: .
After defining an allocation function, you would set Hamza's internal allocator in the following way:
```c
hz_set_allocator((hz_allocator_t){.userptr=NULL,.allocfn=&allocfn});
``` 