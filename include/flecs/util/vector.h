#ifndef FLECS_VECTOR_H
#define FLECS_VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_vector_t ecs_vector_t;
typedef struct ecs_vector_params_t ecs_vector_params_t;

typedef int (*EcsComparator)(
    const void* p1,
    const void *p2);

typedef void (*EcsMove)(
    ecs_vector_t *array,
    const ecs_vector_params_t *params,
    void *to,
    void *from,
    void *ctx);

struct ecs_vector_params_t {
    EcsMove move_action; /* Invoked when moving elements */
    void *move_ctx;
    void *ctx;
    uint32_t element_size; /* Size of an element */
};

FLECS_EXPORT
ecs_vector_t* ecs_vector_new(
    const ecs_vector_params_t *params,
    uint32_t size);

FLECS_EXPORT
ecs_vector_t* ecs_vector_new_from_buffer(
    const ecs_vector_params_t *params,
    uint32_t size,
    void *buffer);

FLECS_EXPORT
void ecs_vector_free(
    ecs_vector_t *array);

FLECS_EXPORT
void ecs_vector_clear(
    ecs_vector_t *array);

FLECS_EXPORT
void* ecs_vector_add(
    ecs_vector_t **array_inout,
    const ecs_vector_params_t *params);

FLECS_EXPORT
void* ecs_vector_addn(
    ecs_vector_t **array_inout,
    const ecs_vector_params_t *params,
    uint32_t count);

FLECS_EXPORT
void* ecs_vector_get(
    const ecs_vector_t *array,
    const ecs_vector_params_t *params,
    uint32_t index);

FLECS_EXPORT
uint32_t ecs_vector_get_index(
    const ecs_vector_t *array,
    const ecs_vector_params_t *params,
    void *elem);

FLECS_EXPORT
void* ecs_vector_last(
    const ecs_vector_t *array,
    const ecs_vector_params_t *params);

FLECS_EXPORT
uint32_t ecs_vector_remove(
    ecs_vector_t *array,
    const ecs_vector_params_t *params,
    void *elem);

FLECS_EXPORT
void ecs_vector_remove_last(
    ecs_vector_t *array);

FLECS_EXPORT
bool ecs_vector_pop(
    ecs_vector_t *array,
    const ecs_vector_params_t *params,
    void *value);

FLECS_EXPORT
uint32_t ecs_vector_move_index(
    ecs_vector_t **dst_array,
    ecs_vector_t *src_array,
    const ecs_vector_params_t *params,
    uint32_t index);

FLECS_EXPORT
uint32_t ecs_vector_remove_index(
    ecs_vector_t *array,
    const ecs_vector_params_t *params,
    uint32_t index);

FLECS_EXPORT
void ecs_vector_reclaim(
    ecs_vector_t **array,
    const ecs_vector_params_t *params);

FLECS_EXPORT
uint32_t ecs_vector_set_size(
    ecs_vector_t **array,
    const ecs_vector_params_t *params,
    uint32_t size);

FLECS_EXPORT
uint32_t ecs_vector_set_count(
    ecs_vector_t **array,
    const ecs_vector_params_t *params,
    uint32_t size);

FLECS_EXPORT
uint32_t ecs_vector_count(
    const ecs_vector_t *array);

FLECS_EXPORT
uint32_t ecs_vector_size(
    const ecs_vector_t *array);

FLECS_EXPORT
void* ecs_vector_first(
    const ecs_vector_t *array);

FLECS_EXPORT
void ecs_vector_sort(
    ecs_vector_t *array,
    const ecs_vector_params_t *params,
    EcsComparator compare_action);

FLECS_EXPORT
void ecs_vector_memory(
    const ecs_vector_t *array,
    const ecs_vector_params_t *params,
    uint32_t *allocd,
    uint32_t *used);

#ifdef __cplusplus
}
#endif

#endif
