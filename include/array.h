#ifndef REFLECS_ARRAY_H
#define REFLECS_ARRAY_H

#include <reflecs/reflecs.h>
#include <reflecs/iter.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EcsArray EcsArray;

typedef struct EcsArrayParams {
    uint32_t element_size; /* Size of an element */
    EcsComparator compare_action; /* Comparator function */
} EcsArrayParams;

typedef struct EcsArrayIter {
    EcsArrayParams *params;
    uint32_t index;
} EcsArrayIter;

EcsArray* ecs_array_new(
    uint32_t size,
    EcsArrayParams *params);

void ecs_array_free(
    EcsArray *array);

EcsArray* ecs_array_add(
    EcsArray *array,
    EcsArrayParams *params,
    void *elem_out);

void* ecs_array_get(
    EcsArray *array,
    EcsArrayParams *params,
    uint32_t index);

EcsArray* ecs_array_remove(
    EcsArray *array,
    EcsArrayParams *params,
    uint32_t index);

uint32_t ecs_array_count(
    EcsArray *array);

uint32_t ecs_array_size(
    EcsArray *array);

EcsIter _ecs_array_iter(
    EcsArray *array,
    EcsArrayParams *params,
    EcsArrayIter *iter_data);

#define ecs_array_iter(me, params)\
    _ecs_array_iter(me, params, alloca(sizeof(EcsArrayIter)))

void ecs_array_sort(
    EcsArray *array,
    EcsArrayParams *params);

#ifdef __cplusplus
}
#endif

#endif
