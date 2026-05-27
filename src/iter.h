/**
 * @file iter.h
 * @brief Iterator utilities.
 */

#ifndef FLECS_ITER_H
#define FLECS_ITER_H

/* Initialize iterator. */
void flecs_iter_init(
    const ecs_world_t *world,
    ecs_iter_t *it,
    bool alloc_resources);

/* Slow path for ecs_field when field is not a $this column. */
void* flecs_field_shared(
    const ecs_iter_t *it,
    size_t size,
    int8_t index);

/* Free iterator memory block. */
void flecs_iter_free(
    void *ptr,
    ecs_size_t size);

/* Allocate zero initialized memory from iterator allocator. */
void* flecs_iter_calloc(
    ecs_iter_t *it,
    ecs_size_t size,
    ecs_size_t align);

#define flecs_iter_calloc_t(it, T)\
    flecs_iter_calloc(it, ECS_SIZEOF(T), ECS_ALIGNOF(T))

#define flecs_iter_calloc_n(it, T, count)\
    flecs_iter_calloc(it, ECS_SIZEOF(T) * count, ECS_ALIGNOF(T))

#define flecs_iter_free_t(ptr, T)\
    flecs_iter_free(ptr, ECS_SIZEOF(T))

#define flecs_iter_free_n(ptr, T, count)\
    flecs_iter_free(ptr, ECS_SIZEOF(T) * count)

#endif
