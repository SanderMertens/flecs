#ifndef FLECS_ARRAY_H
#define FLECS_ARRAY_H

#include "../flecs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_array_t ecs_array_t;
typedef struct ecs_array_params_t ecs_array_params_t;

typedef int (*EcsComparator)(
    const void* p1,
    const void *p2);

typedef void (*EcsMove)(
    ecs_array_t *array,
    const ecs_array_params_t *params,
    void *to,
    void *from,
    void *ctx);

struct ecs_array_params_t {
    EcsMove move_action; /* Invoked when moving elements */
    void *move_ctx;
    void *ctx;
    uint32_t element_size; /* Size of an element */
};

typedef struct EcsArrayIter {
    ecs_array_params_t *params;
    uint32_t index;
} EcsArrayIter;

FLECS_EXPORT
ecs_array_t* ecs_array_new(
    const ecs_array_params_t *params,
    uint32_t size);

FLECS_EXPORT
ecs_array_t* ecs_array_new_from_buffer(
    const ecs_array_params_t *params,
    uint32_t size,
    void *buffer);

FLECS_EXPORT
void ecs_array_free(
    ecs_array_t *array);

FLECS_EXPORT
void ecs_array_clear(
    ecs_array_t *array);

FLECS_EXPORT
void* ecs_array_add(
    ecs_array_t **array_inout,
    const ecs_array_params_t *params);

FLECS_EXPORT
void* ecs_array_addn(
    ecs_array_t **array_inout,
    const ecs_array_params_t *params,
    uint32_t count);

FLECS_EXPORT
void* ecs_array_get(
    ecs_array_t *array,
    const ecs_array_params_t *params,
    uint32_t index);

FLECS_EXPORT
uint32_t ecs_array_get_index(
    ecs_array_t *array,
    const ecs_array_params_t *params,
    void *elem);

FLECS_EXPORT
void* ecs_array_last(
    ecs_array_t *array,
    const ecs_array_params_t *params);

FLECS_EXPORT
uint32_t ecs_array_remove(
    ecs_array_t *array,
    const ecs_array_params_t *params,
    void *elem);

FLECS_EXPORT
void ecs_array_remove_last(
    ecs_array_t *array);

FLECS_EXPORT
uint32_t ecs_array_move_index(
    ecs_array_t **dst_array,
    ecs_array_t *src_array,
    const ecs_array_params_t *params,
    uint32_t index);

FLECS_EXPORT
uint32_t ecs_array_remove_index(
    ecs_array_t *array,
    const ecs_array_params_t *params,
    uint32_t index);

FLECS_EXPORT
void ecs_array_reclaim(
    ecs_array_t **array,
    const ecs_array_params_t *params);

FLECS_EXPORT
uint32_t ecs_array_set_size(
    ecs_array_t **array,
    const ecs_array_params_t *params,
    uint32_t size);

FLECS_EXPORT
uint32_t ecs_array_set_count(
    ecs_array_t **array,
    const ecs_array_params_t *params,
    uint32_t size);

FLECS_EXPORT
uint32_t ecs_array_count(
    ecs_array_t *array);

FLECS_EXPORT
uint32_t ecs_array_size(
    ecs_array_t *array);

FLECS_EXPORT
void* ecs_array_buffer(
    ecs_array_t *array);

FLECS_EXPORT
void ecs_array_sort(
    ecs_array_t *array,
    const ecs_array_params_t *params,
    EcsComparator compare_action);

FLECS_EXPORT
void ecs_array_memory(
    ecs_array_t *array,
    const ecs_array_params_t *params,
    uint32_t *allocd,
    uint32_t *used);

#ifdef __cplusplus
}
#endif

#endif
