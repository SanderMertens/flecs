/**
 * @file datastructures/qsort.c
 * @brief Quicksort implementation.
 */

#include "../private_api.h"
#include "qsort.h"

void ecs_qsort(
    void *base, 
    ecs_size_t nitems, 
    ecs_size_t size, 
    int (*compar)(const void *, const void*))
{
    void *tmp = ecs_os_alloca(size); /* For swap */

    #define LESS(i, j) \
        compar(ECS_ELEM(base, size, i), ECS_ELEM(base, size, j)) < 0

    #define SWAP(i, j) \
        ecs_os_memcpy(tmp, ECS_ELEM(base, size, i), size),\
        ecs_os_memcpy(ECS_ELEM(base, size, i), ECS_ELEM(base, size, j), size),\
        ecs_os_memcpy(ECS_ELEM(base, size, j), tmp, size)

    QSORT(nitems, LESS, SWAP);
}
