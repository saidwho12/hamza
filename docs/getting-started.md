# Getting Started - Quickstart Guide
## Compilation
=== "CMake"
    Hi
=== "Plain C/C++"
    Copy and paste `hz.c` and `hz.h` files into your project, and a UCD file.

## Initialize library
First, include `hz.h` into your file:
```c
#include <hz/hz.h>
```

```c
hz_error err = hz_init(&(struct hz_init_params){
    .avx2_optimize = HZ_TRUE,
});
```
