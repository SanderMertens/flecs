#ifdef FLECS_QUEUE

#ifndef FLECS_QUEUE_H_
#define FLECS_QUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_queue_t ecs_queue_t;

FLECS_EXPORT
ecs_queue_t* _ecs_queue_new(
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count);

#define ecs_queue_new(T, elem_count)\
    _ecs_queue_new(ECS_VECTOR_T(T), elem_count)

FLECS_EXPORT
ecs_queue_t* _ecs_queue_from_array(
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count,
    void *array);

#define ecs_queue_from_array(T, elem_count, array)\
    _ecs_queue_from_array(ECS_VECTOR_T(T), elem_count, array)

FLECS_EXPORT
void* _ecs_queue_push(
    ecs_queue_t *queue,
    ecs_size_t elem_size,
    int16_t offset);

#define ecs_queue_push(queue, T)\
    (T*)_ecs_queue_push(queue, ECS_VECTOR_T(T))

FLECS_EXPORT
void* _ecs_queue_get(
    ecs_queue_t *queue,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t index);

#define ecs_queue_get(queue, T, index)\
    (T*)_ecs_queue_get(queue, ECS_VECTOR_T(T), index)

#define ecs_queue_get_t(vector, size, alignment, index) \
    _ecs_queue_get(vector, ECS_VECTOR_U(size, alignment), index)

FLECS_EXPORT
void* _ecs_queue_last(
    ecs_queue_t *queue,
    ecs_size_t elem_size,
    int16_t offset);

#define ecs_queue_last(queue, T)\
    (T*)_ecs_queue_last(queue, ECS_VECTOR_T(T))

FLECS_EXPORT
int32_t ecs_queue_index(
    ecs_queue_t *queue);

FLECS_EXPORT
int32_t ecs_queue_count(
    ecs_queue_t *queue);

FLECS_EXPORT
void ecs_queue_free(
    ecs_queue_t *queue);

#ifdef __cplusplus
}
#endif

#endif

#endif
