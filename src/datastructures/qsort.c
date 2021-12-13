#include "../private_api.h"
#include "qsort.h"

#define ELEM(base, i, size) \
    ECS_OFFSET(base, (size) * (i))

void ecs_qsort(
    void *base, 
    size_t nitems, 
    size_t size, 
    int (*compar)(const void *, const void*))
{
    void *tmp = ecs_os_alloca(size); /* For swap */

    #define LESS(i, j) \
        compar(ELEM(base, i, size), ELEM(base, j, size)) < 0

    #define SWAP(i, j) \
        ecs_os_memcpy(tmp, ELEM(base, i, size), size),\
        ecs_os_memcpy(ELEM(base, i, size), ELEM(base, j, size), size),\
        ecs_os_memcpy(ELEM(base, j, size), tmp, size)

    QSORT(nitems, LESS, SWAP);
}
