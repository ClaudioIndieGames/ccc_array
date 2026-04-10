/*
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

    ### CUDA Support
    When compiling with CUDA (nvcc), the library uses CUDA allocators for device memory.

    ## API
    ```c
    ccc_array* ccc_array_create(size_t element_size, ccc_array* header = NULL, void* container = NULL, \
        size_t elements_capacity = 4, ccc_allocator* allocator = &ccc_array_default_allocator, bool zeroed = false, bool no_realloc = false);
    void ccc_array_destroy(ccc_array* self)
    void* ccc_array_at(ccc_array* self, size_t index)
    void* ccc_array_insert(ccc_array* self, size_t index)
    void ccc_array_remove(ccc_array* self, size_t index)
    void ccc_array_clear(ccc_array* self)
    void* ccc_array_append(ccc_array* self)
    void ccc_array_pop(ccc_array* self)
    void* ccc_array_front(ccc_array* self)
    void* ccc_array_back(ccc_array* self)
    size_t ccc_array_size(ccc_array* self)
    bool ccc_array_empty(ccc_array* self)
    void ccc_array_sort(ccc_array* self, int (*compare_func)(const void*, const void*))
    size_t ccc_array_find(ccc_array* self, const void* value, int (*compare_func)(const void*, const void*))
    ```

    Claudio Raccomandato, March 28 2026
*/

#ifndef __CCC_ARRAY_H__
#define __CCC_ARRAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>         // size_t
#include <stdbool.h>        // bool

#ifdef __CUDACC__
#include <cuda_runtime.h>
#endif

#ifdef CCC_ALLOCATOR_NO_NAMESPACE
#define allocator                   ccc_allocator
#endif

typedef struct {
    void* (*alloc)(size_t size);
    void* (*realloc)(void* ptr, size_t old_size, size_t new_size);
    void (*free)(void* ptr);
} ccc_allocator;

extern ccc_allocator ccc_array_default_allocator;

#ifdef CCC_ARRAY_NO_NAMESPACE
#define array_default_allocator     ccc_array_default_allocator
#define array                       ccc_array
#define array_create_opt_args       ccc_array_create_opt_args
#define array_create                ccc_array_create
#define array_create_with_args      ccc_array_create_with_args
#define array_destroy               ccc_array_destroy
#define array_at                    ccc_array_at
#define array_insert                ccc_array_insert
#define array_remove                ccc_array_remove
#define array_clear                 ccc_array_clear
#define array_append                ccc_array_append
#define array_pop                   ccc_array_pop
#define array_front                 ccc_array_front
#define array_back                  ccc_array_back
#define array_size                  ccc_array_size
#define array_empty                 ccc_array_empty
#define array_sort                  ccc_array_sort
#define array_find                  ccc_array_find
#endif

#ifndef CCC_ARRAY_DEF
#define CCC_ARRAY_DEF
#endif

enum ccc_array_attributes_e {
    CCC_ARRAY_ZEROED = 1 << 0,
    CCC_ARRAY_NO_REALLOC = 1 << 1,
    CCC_ARRAY_EXT_HEADER = 1 << 2,
    CCC_ARRAY_EXT_CONTAINER = 1 << 3 | CCC_ARRAY_NO_REALLOC,
};

typedef struct {
    void* container;
    size_t element_size;
    size_t count;
    size_t capacity;
    ccc_allocator* allocator;
    unsigned int attributes;
} ccc_array;

typedef struct {
    ccc_array* header;
    void* container;
    size_t elements_capacity;
    ccc_allocator* allocator;
    bool zeroed;
    bool no_realloc;
} ccc_array_create_opt_args;

#define ccc_array_create(element_size, ...) ccc_array_create_with_args(element_size, (ccc_array_create_opt_args){ \
    .header = NULL, \
    .container = NULL, \
    .elements_capacity = 4, \
    .allocator = &ccc_array_default_allocator, \
    .zeroed = false, \
    .no_realloc = false, \
    __VA_ARGS__ \
})
#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF ccc_array* ccc_array_create_with_args(size_t element_size, ccc_array_create_opt_args opt_args);

#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF void ccc_array_destroy(ccc_array* self);
#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF void* ccc_array_at(ccc_array* self, size_t index);
#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF void* ccc_array_insert(ccc_array* self, size_t index);
#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF void ccc_array_remove(ccc_array* self, size_t index);
#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF void ccc_array_clear(ccc_array* self);
#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF void* ccc_array_append(ccc_array* self);
#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF void ccc_array_pop(ccc_array* self);
#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF void* ccc_array_front(ccc_array* self);
#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF void* ccc_array_back(ccc_array* self);
#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF size_t ccc_array_size(ccc_array* self);
#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF bool ccc_array_empty(ccc_array* self);
#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF void ccc_array_sort(ccc_array* self, int (*compare_func)(const void*, const void*));
#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF size_t ccc_array_find(ccc_array* self, const void* value, int (*compare_func)(const void*, const void*));

#ifdef CCC_ARRAY_IMPLEMENTATION

#ifdef __CUDACC__

static void* ccc_array_cuda_alloc(size_t size) {
    void* ptr;
    cudaMallocManaged(&ptr, size);
    //cudaMallocHost(&ptr, size);
    return ptr;
}

static void* ccc_array_cuda_realloc(void* ptr, size_t old_size, size_t new_size) {
    void* new_ptr;
    cudaMallocManaged(&new_ptr, new_size);
    //cudaMallocHost(&new_ptr, new_size);
    if (ptr && old_size > 0) {
        cudaMemcpy(new_ptr, ptr, old_size < new_size ? old_size : new_size, cudaMemcpyDefault);
        cudaFree(ptr);
        //cudaFreeHost(ptr);
    }
    return new_ptr;
}

static void ccc_array_cuda_free(void* ptr) {
    cudaFree(ptr);
    //cudaFreeHost(ptr);
}

ccc_allocator ccc_array_default_allocator = {ccc_array_cuda_alloc, ccc_array_cuda_realloc, ccc_array_cuda_free};

#else

#ifndef CCC_ARRAY_NO_STDLIB

#include <stdlib.h>         // malloc, realloc, free

static void* ccc_array_stdlib_realloc(void* ptr, size_t old_size, size_t new_size) {
    (void)old_size; // ignore
    return realloc(ptr, new_size);
}

ccc_allocator ccc_array_default_allocator = {malloc, ccc_array_stdlib_realloc, free};

#endif  // CCC_ARRAY_NO_STDLIB

#endif  // __CUDACC__

#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF ccc_array* ccc_array_create_with_args(size_t element_size, ccc_array_create_opt_args opt_args) {
    if (element_size <= 0) return NULL;

    // check the allocator
    if (!opt_args.allocator) return NULL;
    if (!opt_args.header) {
        if (opt_args.allocator->alloc == NULL || opt_args.allocator->free == NULL) return NULL;
    }
    if (!opt_args.container) {
        if (opt_args.allocator->alloc == NULL || opt_args.allocator->free == NULL) return NULL;
        if (!opt_args.no_realloc && opt_args.allocator->realloc == NULL) return NULL;
    }

    // create the array header
    ccc_array* header = opt_args.header;
    if (!header) {
        header = (ccc_array*)opt_args.allocator->alloc(sizeof(ccc_array));
        if (!header) return NULL;
    }

    // create the container
    if (opt_args.elements_capacity <= 0) return NULL;
    header->container = opt_args.container;
    if (!header->container) {
        header->container = opt_args.allocator->alloc(element_size * opt_args.elements_capacity);
        if (!header->container) return NULL;
    }

    // initialize the header
    header->element_size = element_size;
    header->count = 0;
    header->capacity = opt_args.elements_capacity * element_size;
    header->allocator = opt_args.allocator;
    header->attributes = 0;
    if (opt_args.header) header->attributes |= CCC_ARRAY_EXT_HEADER;
    if (opt_args.container) header->attributes |= CCC_ARRAY_EXT_CONTAINER;
    if (opt_args.no_realloc) header->attributes |= CCC_ARRAY_NO_REALLOC;
    if (opt_args.zeroed) {
        header->attributes |= CCC_ARRAY_ZEROED;
        for (size_t i = 0; i < header->capacity; ++i) {
            ((char*)header->container)[i] = 0;
        }
    }

    return header;
}

#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF void ccc_array_destroy(ccc_array* self) {
    if (!self) return;

    if (!(self->attributes & CCC_ARRAY_EXT_CONTAINER)) {
        self->allocator->free(self->container);
    }
    if (!(self->attributes & CCC_ARRAY_EXT_HEADER)) {
        self->allocator->free(self);
    }
}

#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF void* ccc_array_at(ccc_array* self, size_t index) {
    if (!self) return NULL;
    if (index >= self->count) return NULL;
    return (char*)(self->container) + (self->element_size * index);
}

CCC_ARRAY_DEF void* ccc_array_insert(ccc_array* self, size_t index) {
    if (!self) return NULL;
    if (index > self->count) return NULL;

    // if there is not enough capacity, then double the capacity
    if (self->count * self->element_size >= self->capacity) {
        if (self->attributes & CCC_ARRAY_NO_REALLOC) return NULL;
        size_t old_capacity = self->capacity;
        self->capacity *= 2;
        self->container = self->allocator->realloc(self->container, old_capacity, self->capacity);
        if (!self->container) return NULL;
        if (self->attributes & CCC_ARRAY_ZEROED) {
            for (size_t i = self->count * self->element_size; i < self->capacity; ++i) {
                ((char*)self->container)[i] = 0;
            }
        }
    }

    // if not appending, shift the rest of the elements to the right
    if (index < self->count) {
        for (size_t i = self->count; i > index; --i) {
            for (size_t j = 0; j < self->element_size; ++j) {
                ((char*)self->container)[i * self->element_size + j] = ((char*)self->container)[(i - 1) * self->element_size + j];
            }
        }
        if (self->attributes & CCC_ARRAY_ZEROED) {
            for (size_t i = 0; i < self->element_size; ++i) {
                ((char*)self->container)[(index * self->element_size) + i] = 0;
            }
        }
    }

    // increase the count and return the pointer to the new element
    self->count += 1;
    return ccc_array_at(self, index);
}

#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF void ccc_array_remove(ccc_array* self, size_t index) {
    if (!self) return;
    if (index >= self->count) return;

    // if 1/4 of the capacity is not used, then reduce it by half
    // NOTE: don't reduce the capacity if the count is less than 4, to avoid too much reallocations when the array is small
    if (self->count > 4 && self->count * self->element_size <= self->capacity / 4) {
        if (!(self->attributes & CCC_ARRAY_NO_REALLOC)) {
            size_t old_capacity = self->capacity;
            self->capacity = self->count * self->element_size * 2;
            self->container = self->allocator->realloc(self->container, old_capacity, self->capacity);
            if (!self->container) return;
        }
    }

    // decrease the count
    self->count -= 1;

    // if not removing from the end, shift the rest of the elements to the left
    if (index < self->count) {
        for (size_t i = index; i < self->count; ++i) {
            for (size_t j = 0; j < self->element_size; ++j) {
                ((char*)self->container)[i * self->element_size + j] = ((char*)self->container)[(i + 1) * self->element_size + j];
            }
        }
    }
}

#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF void ccc_array_clear(ccc_array* self) {
    if (!self) return;
    if (self->attributes & CCC_ARRAY_ZEROED) {
        for (size_t i = 0; i < self->count * self->element_size; ++i) {
            ((char*)self->container)[i] = 0;
        }
    }
    self->count = 0;
}

#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF void* ccc_array_append(ccc_array* self) {
    return ccc_array_insert(self, self->count);
}

#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF void ccc_array_pop(ccc_array* self) {
    ccc_array_remove(self, 0);
}

#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF void* ccc_array_front(ccc_array* self) {
    return ccc_array_at(self, 0);
}

#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF void* ccc_array_back(ccc_array* self) {
    return ccc_array_at(self, self->count - 1);
}

#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF size_t ccc_array_size(ccc_array* self) {
    if (!self) return 0;
    return self->count;
}

#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF bool ccc_array_empty(ccc_array* self) {
    return (ccc_array_size(self) == 0);
}

#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF void ccc_array_sort(ccc_array* self, int (*compare_func)(const void*, const void*)) {
    if (!self) return;
    if (self->count <= 1) return;
    if (!compare_func) return;

    // Small manual stack (log2(n) is enough if optimized)
    int stack[64];
    int top = -1;

    stack[++top] = 0;
    stack[++top] = self->count - 1;

    //char pivot[self->element_size];
    char* pivot = (char*)self->allocator->alloc(self->element_size);

    while (top >= 0) {
        int right = stack[top--];
        int left  = stack[top--];

        while (left < right) {
            int i = left;
            int j = right;

            // Median-of-three pivot (better than middle)
            int mid = left + (right - left) / 2;

            char* a = ((char*)self->container) + left * self->element_size;
            char* b = ((char*)self->container) + mid * self->element_size;
            char* c = ((char*)self->container) + right * self->element_size;

            // Order left, mid, right
            if (compare_func(a, b) > 0) {
                for (int k = 0; k < (int)self->element_size; ++k) {
                    char tmp = a[k];
                    a[k] = b[k];
                    b[k] = tmp;
                }
            }
            if (compare_func(a, c) > 0) {
                for (int k = 0; k < (int)self->element_size; ++k) {
                    char tmp = a[k];
                    a[k] = c[k];
                    c[k] = tmp;
                }
            }
            if (compare_func(b, c) > 0) {
                for (int k = 0; k < (int)self->element_size; ++k) {
                    char tmp = b[k];
                    b[k] = c[k];
                    c[k] = tmp;
                }
            }

            // Use middle as pivot (copy it!)
            for (int k = 0; k < (int)self->element_size; ++k) pivot[k] = b[k];

            // Partition
            while (i <= j) {
                while (compare_func(((char*)self->container) + i * self->element_size, pivot) < 0) i++;
                while (compare_func(((char*)self->container) + j * self->element_size, pivot) > 0) j--;

                if (i <= j) {
                    // swap elements at i and j
                    char* ptr_i = ((char*)self->container) + i * self->element_size;
                    char* ptr_j = ((char*)self->container) + j * self->element_size;
                    for (int k = 0; k < (int)self->element_size; ++k) {
                        char tmp = ptr_i[k];
                        ptr_i[k] = ptr_j[k];
                        ptr_j[k] = tmp;
                    }
                    i++;
                    j--;
                }
            }

            // Tail-call elimination:
            // Process smaller partition first, push larger one
            if ((j - left) < (right - i)) {
                if (i < right) {
                    stack[++top] = i;
                    stack[++top] = right;
                }
                right = j;
            } else {
                if (left < j) {
                    stack[++top] = left;
                    stack[++top] = j;
                }
                left = i;
            }
        }
    }

    self->allocator->free(pivot);
}

#ifdef __CUDACC__
__host__ __device__
#endif
CCC_ARRAY_DEF size_t ccc_array_find(ccc_array* self, const void* value, int (*compare_func)(const void*, const void*)) {
    if (!self) return 0;
    if (!compare_func) return self->count;

    size_t index = 0;
    while (index < self->count) {
        void* e = ccc_array_at(self, index);
        if (compare_func(e, value) == 0) return index;
        index += 1;
    }
    return index;
}

#endif  // CCC_ARRAY_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif  // __CCC_ARRAY_H__