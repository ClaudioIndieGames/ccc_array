# ccc_array.h
## Description
Claudio's C Collections Array, a header-only library that implements a universal interface for *dynamic* and *static* arrays.

## How to use it
Since it's a header-only library, all you need to do is use `#include "ccc_array.h"` in your project files.
Only remember to define `CCC_ARRAY_IMPLEMENTATION`, in exactly ***one*** translation unit (e.g., main.c) before the inclusion:
```c
#define CCC_ARRAY_IMPLEMENTATION
#include "ccc_array.h"
```
Optionally, you can define `CCC_ARRAY_NO_NAMESPACE` to avoid the `ccc_` prefix in the API.

### Header and container
The library uses a header (the `ccc_array` type, including all the array metadata) and a container (the actual data).
Header and container can be provided by the user using the arguments `header` and `container` or created by the library.

### Array creation
1. dynamic array (header and container created by the library)
```c
// assuming CCC_ARRAY_NO_NAMESPACE is defined
array* a = array_create(sizeof(int));
array_destroy(a);
```

2. static array (header and container provided by the user)
```c
// assuming CCC_ARRAY_NO_NAMESPACE is defined
array header;
int container[10];
array* a = array_create(sizeof(int), .header = &header, .container = &container, .elements_capacity = 10);
// or array* a = array_create(sizeof(int), .header = &(array){0}, .container = &(int[10]){0}, .elements_capacity = 10);
array_destroy(a);  // optional
```

## API
```c
ccc_array* ccc_array_create(size_t element_size, ccc_array* header = NULL, void* container = NULL, \
    size_t elements_capacity = 4, ccc_allocator* allocator = &ccc_array_default_allocator, bool zeroed = false, bool no_realloc = false);
void ccc_array_destroy(ccc_array* this)
void* ccc_array_at(ccc_array* this, size_t index)
void* ccc_array_insert(ccc_array* this, size_t index)
void ccc_array_remove(ccc_array* this, size_t index)
void ccc_array_clear(ccc_array* this)
void* ccc_array_append(ccc_array* this)
void ccc_array_pop(ccc_array* this)
void* ccc_array_front(ccc_array* this)
void* ccc_array_back(ccc_array* this)
size_t ccc_array_size(ccc_array* this)
bool ccc_array_empty(ccc_array* this)
void ccc_array_sort(ccc_array* this, int (*compare_func)(const void*, const void*))
size_t ccc_array_find(ccc_array* this, const void* value, int (*compare_func)(const void*, const void*))
```