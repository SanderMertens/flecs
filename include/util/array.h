#ifndef FLECS_ARRAY_H
#define FLECS_ARRAY_H

#include <include/flecs.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EcsArray EcsArray;
typedef struct EcsArrayParams EcsArrayParams;

typedef int (*EcsComparator)(
    const void* p1,
    const void *p2);

typedef void (*EcsMove)(
    EcsArray *array,
    const EcsArrayParams *params,
    void *to,
    void *from,
    void *ctx);

struct EcsArrayParams {
    EcsMove move_action; /* Invoked when moving elements */
    void *move_ctx;
    void *ctx;
    uint32_t element_size; /* Size of an element */
};

typedef struct EcsArrayIter {
    EcsArrayParams *params;
    uint32_t index;
} EcsArrayIter;

FLECS_EXPORT
EcsArray* ecs_array_new(
    const EcsArrayParams *params,
    uint32_t size);

FLECS_EXPORT
EcsArray* ecs_array_new_from_buffer(
    const EcsArrayParams *params,
    uint32_t size,
    void *buffer);

FLECS_EXPORT
void ecs_array_free(
    EcsArray *array);

FLECS_EXPORT
void ecs_array_clear(
    EcsArray *array);

FLECS_EXPORT
void* ecs_array_add(
    EcsArray **array_inout,
    const EcsArrayParams *params);

FLECS_EXPORT
void* ecs_array_addn(
    EcsArray **array_inout,
    const EcsArrayParams *params,
    uint32_t count);

FLECS_EXPORT
void* ecs_array_get(
    EcsArray *array,
    const EcsArrayParams *params,
    uint32_t index);

FLECS_EXPORT
uint32_t ecs_array_get_index(
    EcsArray *array,
    const EcsArrayParams *params,
    void *elem);

FLECS_EXPORT
void* ecs_array_last(
    EcsArray *array,
    const EcsArrayParams *params);

FLECS_EXPORT
uint32_t ecs_array_remove(
    EcsArray *array,
    const EcsArrayParams *params,
    void *elem);

FLECS_EXPORT
void ecs_array_remove_last(
    EcsArray *array);

FLECS_EXPORT
uint32_t ecs_array_move_index(
    EcsArray **dst_array,
    EcsArray *src_array,
    const EcsArrayParams *params,
    uint32_t index);

FLECS_EXPORT
uint32_t ecs_array_remove_index(
    EcsArray *array,
    const EcsArrayParams *params,
    uint32_t index);

FLECS_EXPORT
void ecs_array_reclaim(
    EcsArray **array,
    const EcsArrayParams *params);

FLECS_EXPORT
uint32_t ecs_array_set_size(
    EcsArray **array,
    const EcsArrayParams *params,
    uint32_t size);

FLECS_EXPORT
uint32_t ecs_array_set_count(
    EcsArray **array,
    const EcsArrayParams *params,
    uint32_t size);

FLECS_EXPORT
uint32_t ecs_array_count(
    EcsArray *array);

FLECS_EXPORT
uint32_t ecs_array_size(
    EcsArray *array);

FLECS_EXPORT
void* ecs_array_buffer(
    EcsArray *array);

FLECS_EXPORT
EcsIter _ecs_array_iter(
    EcsArray *array,
    const EcsArrayParams *params,
    EcsArrayIter *iter_data);

#define ecs_array_iter(me, params)\
    _ecs_array_iter(me, params, alloca(sizeof(EcsArrayIter)))

FLECS_EXPORT
void ecs_array_sort(
    EcsArray *array,
    const EcsArrayParams *params,
    EcsComparator compare_action);

FLECS_EXPORT
void ecs_array_memory(
    EcsArray *array,
    const EcsArrayParams *params,
    uint32_t *allocd,
    uint32_t *used);

#ifdef __cplusplus
}
#endif

#endif
