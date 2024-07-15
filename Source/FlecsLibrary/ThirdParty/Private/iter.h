/**
 * @file iter.h
 * @brief Iterator utilities.
 */

#ifndef FLECS_ITER_H
#define FLECS_ITER_H

void flecs_iter_init(
    const ecs_world_t *world,
    ecs_iter_t *it,
    ecs_flags8_t fields);

void flecs_iter_validate(
    ecs_iter_t *it);

bool flecs_iter_next_row(
    ecs_iter_t *it);

bool flecs_iter_next_instanced(
    ecs_iter_t *it,
    bool result);

void* flecs_iter_calloc(
    ecs_iter_t *it,
    ecs_size_t size,
    ecs_size_t align);

#define flecs_iter_calloc_t(it, T)\
    flecs_iter_calloc(it, ECS_SIZEOF(T), ECS_ALIGNOF(T))

#define flecs_iter_calloc_n(it, T, count)\
    flecs_iter_calloc(it, ECS_SIZEOF(T) * count, ECS_ALIGNOF(T))

void flecs_iter_free(
    void *ptr,
    ecs_size_t size);

#define flecs_iter_free_t(ptr, T)\
    flecs_iter_free(ptr, ECS_SIZEOF(T))

#define flecs_iter_free_n(ptr, T, count)\
    flecs_iter_free(ptr, ECS_SIZEOF(T) * count)

#endif
