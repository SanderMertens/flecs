#ifndef REFLECS_ARRAY_H
#define REFLECS_ARRAY_H

#include <reflecs/platform.h>
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
    const EcsArrayParams *params);

EcsArray* ecs_array_new_from_buffer(
    uint32_t size,
    const EcsArrayParams *params,
    void *buffer);

void ecs_array_free(
    EcsArray *array);

EcsArray* ecs_array_add(
    EcsArray *array,
    const EcsArrayParams *params,
    void *elem_out);

void* ecs_array_get(
    EcsArray *array,
    const EcsArrayParams *params,
    uint32_t index);

EcsArray* ecs_array_remove(
    EcsArray *array,
    const EcsArrayParams *params,
    uint32_t index);

uint32_t ecs_array_count(
    EcsArray *array);

uint32_t ecs_array_size(
    EcsArray *array);

void* ecs_array_buffer(
    EcsArray *array);

EcsIter _ecs_array_iter(
    EcsArray *array,
    const EcsArrayParams *params,
    EcsArrayIter *iter_data);

#define ecs_array_iter(me, params)\
    _ecs_array_iter(me, params, alloca(sizeof(EcsArrayIter)))

void ecs_array_sort(
    EcsArray *array,
    const EcsArrayParams *params);

#ifdef __cplusplus
}
#endif

#endif
