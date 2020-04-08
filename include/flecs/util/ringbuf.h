

#ifndef FLECS_RINGBUF_H_
#define FLECS_RINGBUF_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_ringbuf_t ecs_ringbuf_t;

FLECS_EXPORT
ecs_ringbuf_t* _ecs_ringbuf_new(
    size_t elem_size,
    int32_t elem_count);

#define ecs_ringbuf_new(T, elem_count)\
    _ecs_ringbuf_new(sizeof(T), elem_count)

FLECS_EXPORT
void* _ecs_ringbuf_push(
    ecs_ringbuf_t *buffer,
    size_t elem_size);

#define ecs_ringbuf_push(buffer, T)\
    (T*)_ecs_ringbuf_push(buffer, sizeof(T))

FLECS_EXPORT
void* _ecs_ringbuf_get(
    ecs_ringbuf_t *buffer,
    size_t elem_size,
    int32_t index);

#define ecs_ringbuf_get(buffer, T, index)\
    (T*)_ecs_ringbuf_get(buffer, sizeof(T), index)

FLECS_EXPORT
void* _ecs_ringbuf_last(
    ecs_ringbuf_t *buffer,
    size_t elem_size);

#define ecs_ringbuf_last(buffer, T)\
    (T*)_ecs_ringbuf_last(buffer, elem_size)

FLECS_EXPORT
int32_t ecs_ringbuf_index(
    ecs_ringbuf_t *buffer);

FLECS_EXPORT
int32_t ecs_ringbuf_count(
    ecs_ringbuf_t *buffer);

FLECS_EXPORT
void ecs_ringbuf_free(
    ecs_ringbuf_t *buffer);

#ifdef __cplusplus
}
#endif

#endif
