#include <stdio.h>
#define CCC_ARRAY_IMPLEMENTATION
#define CCC_ARRAY_NO_NAMESPACE
#include "ccc_array.h"

int compare_ints(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

int compare_ints_rev(const void* a, const void* b) {
    return (*(int*)b - *(int*)a);
}

void print_ints(ccc_array* a) {
    for (size_t i = 0; i < array_size(a); ++i) {
        int* element = array_at(a, i);
        printf("%d ", *element);
    }
    printf("\n");
}

int main() {

    // dynamic array example
    array* da = array_create(sizeof(int), .elements_capacity = 10);
    if (!da) {
        fprintf(stderr, "Failed to create array\n");
        return 1;
    }
    for (int i = 0; i < 15; ++i) {
        int* element = array_append(da);
        if (!element) {
            fprintf(stderr, "Failed to append to array\n");
            array_destroy(da);
            return 1;
        }
        *element = i;
        printf("Appended element: ");
        print_ints(da);
    }
    array_sort(da, compare_ints_rev);
    printf("Sorted elements:  ");
    print_ints(da);
    for (int i = 0; i < 15; ++i) {
        array_pop(da);
        printf("Popped element:   ");
        print_ints(da);
    }
    array_destroy(da);

    // static array example
    array* sa = array_create(sizeof(int), .header = &(array){0}, .container = &(int [10]){0}, .elements_capacity = 10);
    if (!sa) {
        fprintf(stderr, "Failed to create static array\n");
        return 1;
    }
    for (int i = 0; i < 10; ++i) {
        int* element = array_insert(sa, 0);
        if (!element) {
            fprintf(stderr, "Failed to insert into static array\n");
            array_destroy(sa);
            return 1;
        }
        *element = i;
        printf("Inserted element: ");
        print_ints(sa);
    }
    array_sort(sa, compare_ints);
    printf("Sorted elements:  ");
    print_ints(sa);
    for (int i = 9; i >= 0; --i) {
        array_remove(sa, array_size(sa) - 1);
        printf("Removed element:  ");
        print_ints(sa);
    }
    array_destroy(sa);

    return 0;
}