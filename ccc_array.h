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

    ## API
    ```c
    ccc_array* ccc_array_create(size_t element_size, ccc_array* header = NULL, void* container = NULL, \
        size_t elements_capacity = 4, ccc_array_allocator allocator = CCC_ARRAY_DEFAULT_ALLOCATOR, bool zeroed = false, bool no_realloc = false);
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

    Claudio Raccomandato, March 28 2026
*/

#ifndef __CCC_ARRAY_H__
#define __CCC_ARRAY_H__

#include <stddef.h>         // size_t
#include <stdbool.h>        // bool

#ifndef CCC_ARRAY_NO_STDLIB
#include <stdlib.h>         // malloc, realloc, free
#define CCC_ARRAY_DEFAULT_ALLOCATOR ((ccc_array_allocator) {malloc, realloc, free})
#else
#ifndef CCC_ARRAY_DEFAULT_ALLOCATOR
#error "You must define CCC_ARRAY_DEFAULT_ALLOCATOR if you don't want to include stdlib.h"
#endif
#endif

#ifdef CCC_ARRAY_NO_NAMESPACE
#define array_allocator     ccc_array_allocator
#define array               ccc_array
#define array_create        ccc_array_create
#define array_destroy       ccc_array_destroy
#define array_at            ccc_array_at
#define array_insert        ccc_array_insert
#define array_remove        ccc_array_remove
#define array_clear         ccc_array_clear
#define array_append        ccc_array_append
#define array_pop           ccc_array_pop
#define array_front         ccc_array_front
#define array_back          ccc_array_back
#define array_size          ccc_array_size
#define array_empty         ccc_array_empty
#define array_sort          ccc_array_sort
#define array_find          ccc_array_find
#endif

typedef struct {
    void* (*alloc)(size_t size);
    void* (*realloc)(void* ptr, size_t new_size);
    void (*free)(void* ptr);
} ccc_array_allocator;

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
    ccc_array_allocator allocator;
    unsigned int attributes;
} ccc_array;

typedef struct {
    ccc_array* header;
    void* container;
    size_t elements_capacity;
    ccc_array_allocator allocator;
    bool zeroed;
    bool no_realloc;
} ccc_array_create_opt_args;

#define ccc_array_create(element_size, ...) ccc_array_create_(element_size, (ccc_array_create_opt_args){ \
    .header = NULL, \
    .container = NULL, \
    .elements_capacity = 4, \
    .allocator = CCC_ARRAY_DEFAULT_ALLOCATOR, \
    .zeroed = false, \
    .no_realloc = false, \
    __VA_ARGS__ \
})
ccc_array* ccc_array_create_(size_t element_size, ccc_array_create_opt_args opt_args);

void ccc_array_destroy(ccc_array* this);
void* ccc_array_at(ccc_array* this, size_t index);
void* ccc_array_insert(ccc_array* this, size_t index);
void ccc_array_remove(ccc_array* this, size_t index);
void ccc_array_clear(ccc_array* this);
void* ccc_array_append(ccc_array* this);
void ccc_array_pop(ccc_array* this);
void* ccc_array_front(ccc_array* this);
void* ccc_array_back(ccc_array* this);
size_t ccc_array_size(ccc_array* this);
bool ccc_array_empty(ccc_array* this);
void ccc_array_sort(ccc_array* this, int (*compare_func)(const void*, const void*));
size_t ccc_array_find(ccc_array* this, const void* value, int (*compare_func)(const void*, const void*));

#ifdef CCC_ARRAY_IMPLEMENTATION

ccc_array* ccc_array_create_(size_t element_size, ccc_array_create_opt_args opt_args) {
    if (element_size <= 0) return NULL;

    // check the allocator
    if (!opt_args.header) {
        if (opt_args.allocator.alloc == NULL || opt_args.allocator.free == NULL) return NULL;
    }
    if (!opt_args.container) {
        if (opt_args.allocator.alloc == NULL || opt_args.allocator.free == NULL) return NULL;
        if (!opt_args.no_realloc && opt_args.allocator.realloc == NULL) return NULL;
    }

    // create the array header
    ccc_array* header = opt_args.header;
    if (!header) {
        header = opt_args.allocator.alloc(sizeof(ccc_array));
        if (!header) return NULL;
    }

    // create the container
    if (opt_args.elements_capacity <= 0) return NULL;
    header->container = opt_args.container;
    if (!header->container) {
        header->container = opt_args.allocator.alloc(element_size * opt_args.elements_capacity);
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

void ccc_array_destroy(ccc_array* this) {
    if (!this) return;

    if (!(this->attributes & CCC_ARRAY_EXT_CONTAINER)) {
        this->allocator.free(this->container);
    }
    if (!(this->attributes & CCC_ARRAY_EXT_HEADER)) {
        this->allocator.free(this);
    }
}

void* ccc_array_at(ccc_array* this, size_t index) {
    if (!this) return NULL;
    if (index >= this->count) return NULL;
    return (char*)(this->container) + (this->element_size * index);
}

void* ccc_array_insert(ccc_array* this, size_t index) {
    if (!this) return NULL;
    if (index > this->count) return NULL;

    // if there is not enough capacity, then double the capacity
    if (this->count * this->element_size >= this->capacity) {
        if (this->attributes & CCC_ARRAY_NO_REALLOC) return NULL;
        this->capacity *= 2;
        this->container = this->allocator.realloc(this->container, this->capacity);
        if (!this->container) return NULL;
        if (this->attributes & CCC_ARRAY_ZEROED) {
            for (size_t i = this->count * this->element_size; i < this->capacity; ++i) {
                ((char*)this->container)[i] = 0;
            }
        }
    }

    // if not appending, shift the rest of the elements to the right
    if (index < this->count) {
        for (size_t i = this->count; i > index; --i) {
            for (size_t j = 0; j < this->element_size; ++j) {
                ((char*)this->container)[i * this->element_size + j] = ((char*)this->container)[(i - 1) * this->element_size + j];
            }
        }
        if (this->attributes & CCC_ARRAY_ZEROED) {
            for (size_t i = 0; i < this->element_size; ++i) {
                ((char*)this->container)[(index * this->element_size) + i] = 0;
            }
        }
    }

    // increase the count and return the pointer to the new element
    this->count += 1;
    return ccc_array_at(this, index);
}

void ccc_array_remove(ccc_array* this, size_t index) {
    if (!this) return;
    if (index >= this->count) return;

    // if 1/4 of the capacity is not used, then reduce it by half
    // NOTE: don't reduce the capacity if the count is less than 4, to avoid too much reallocations when the array is small
    if (this->count > 4 && this->count * this->element_size <= this->capacity / 4) {
        if (!(this->attributes & CCC_ARRAY_NO_REALLOC)) {
            this->capacity = this->count * this->element_size * 2;
            this->container = this->allocator.realloc(this->container, this->capacity);
            if (!this->container) return;
        }
    }

    // decrease the count
    this->count -= 1;

    // if not removing from the end, shift the rest of the elements to the left
    if (index < this->count) {
        for (size_t i = index; i < this->count; ++i) {
            for (size_t j = 0; j < this->element_size; ++j) {
                ((char*)this->container)[i * this->element_size + j] = ((char*)this->container)[(i + 1) * this->element_size + j];
            }
        }
    }
}

void ccc_array_clear(ccc_array* this) {
    if (!this) return;
    if (this->attributes & CCC_ARRAY_ZEROED) {
        for (size_t i = 0; i < this->count * this->element_size; ++i) {
            ((char*)this->container)[i] = 0;
        }
    }
    this->count = 0;
}

void* ccc_array_append(ccc_array* this) {
    return ccc_array_insert(this, this->count);
}

void ccc_array_pop(ccc_array* this) {
    ccc_array_remove(this, 0);
}

void* ccc_array_front(ccc_array* this) {
    return ccc_array_at(this, 0);
}

void* ccc_array_back(ccc_array* this) {
    return ccc_array_at(this, this->count - 1);
}

size_t ccc_array_size(ccc_array* this) {
    if (!this) return 0;
    return this->count;
}

bool ccc_array_empty(ccc_array* this) {
    return (ccc_array_size(this) == 0);
}

void ccc_array_sort(ccc_array* this, int (*compare_func)(const void*, const void*)) {

    // Small manual stack (log2(n) is enough if optimized)
    int stack[64];
    int top = -1;

    stack[++top] = 0;
    stack[++top] = this->count - 1;

    while (top >= 0) {
        int right = stack[top--];
        int left  = stack[top--];

        while (left < right) {
            int i = left;
            int j = right;

            // Median-of-three pivot (better than middle)
            int mid = left + (right - left) / 2;

            char* a = ((char*)this->container) + left * this->element_size;
            char* b = ((char*)this->container) + mid * this->element_size;
            char* c = ((char*)this->container) + right * this->element_size;

            // Order left, mid, right
            if (compare_func(a, b) > 0) {
                for (int k = 0; k < (int)this->element_size; ++k) {
                    char tmp = a[k];
                    a[k] = b[k];
                    b[k] = tmp;
                }
            }
            if (compare_func(a, c) > 0) {
                for (int k = 0; k < (int)this->element_size; ++k) {
                    char tmp = a[k];
                    a[k] = c[k];
                    c[k] = tmp;
                }
            }
            if (compare_func(b, c) > 0) {
                for (int k = 0; k < (int)this->element_size; ++k) {
                    char tmp = b[k];
                    b[k] = c[k];
                    c[k] = tmp;
                }
            }

            // Use middle as pivot (copy it!)
            char pivot[this->element_size];
            for (int k = 0; k < (int)this->element_size; ++k) pivot[k] = b[k];

            // Partition
            while (i <= j) {
                while (compare_func(((char*)this->container) + i * this->element_size, pivot) < 0) i++;
                while (compare_func(((char*)this->container) + j * this->element_size, pivot) > 0) j--;

                if (i <= j) {
                    // swap elements at i and j
                    char* ptr_i = ((char*)this->container) + i * this->element_size;
                    char* ptr_j = ((char*)this->container) + j * this->element_size;
                    for (int k = 0; k < (int)this->element_size; ++k) {
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
}

size_t ccc_array_find(ccc_array* this, const void* value, int (*compare_func)(const void*, const void*)) {
    size_t index = 0;
    while (index < this->count) {
        void* e = ccc_array_at(this, index);
        if (compare_func(e, value) == 0) return index;
        index += 1;
    }
    return index;
}

#endif  // CCC_ARRAY_IMPLEMENTATION

#endif  // __CCC_ARRAY_H__