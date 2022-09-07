/**
 * @file vec.h
 * @brief Vector with allocator support.
 */

#ifndef FLECS_VEC_H
#define FLECS_VEC_H

/** A component column. */
typedef struct ecs_vec_t {
    void *array;
    int32_t count;
    int32_t size;
} ecs_vec_t;

ecs_vec_t* ecs_vec_init(
    ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

#define ecs_vec_init_t(allocator, vec, T, elem_count) \
    ecs_vec_init(allocator, vec, ECS_SIZEOF(T), elem_count)

void ecs_vec_fini(
    ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size);

#define ecs_vec_fini_t(allocator, vec, T) \
    ecs_vec_fini(allocator, vec, ECS_SIZEOF(T))

void* ecs_vec_append(
    ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size);

#define ecs_vec_append_t(allocator, vec, T) \
    ECS_CAST(T*, ecs_vec_append(allocator, vec, ECS_SIZEOF(T)))

void ecs_vec_remove(
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem);

#define ecs_vec_remove_t(vec, T) \
    ECS_CAST(T*, ecs_vec_remove(allocator, vec, ECS_SIZEOF(T)))

void ecs_vec_remove_last(
    ecs_vec_t *vec);

ecs_vec_t ecs_vec_copy(
    ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size);

#define ecs_vec_copy_t(allocator, vec, T) \
    ecs_vec_copy(allocator, vec, ECS_SIZEOF(T))

void ecs_vec_reclaim(
    ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size);

#define ecs_vec_reclaim_t(allocator, vec, T) \
    ecs_vec_reclaim(allocator, vec, ECS_SIZEOF(T))

void ecs_vec_set_size(
    ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

#define ecs_vec_set_size_t(allocator, vec, T, elem_count) \
    ecs_vec_set_size(allocator, vec, ECS_SIZEOF(T), elem_count)

void ecs_vec_set_count(
    ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

#define ecs_vec_set_count_t(allocator, vec, T, elem_count) \
    ecs_vec_set_count(allocator, vec, ECS_SIZEOF(T), elem_count)

void* ecs_vec_grow(
    ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

#define ecs_vec_grow_t(allocator, vec, T, elem_count) \
    ecs_vec_grow(allocator, vec, ECS_SIZEOF(T), elem_count)

int32_t ecs_vec_count(
    ecs_vec_t *vec);

int32_t ecs_vec_size(
    ecs_vec_t *vec);

void* ecs_vec_get(
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t index);

#define ecs_vec_get_t(vec, T, index) \
    ECS_CAST(T*, ecs_vec_get(vec, ECS_SIZEOF(T), index))

void* ecs_vec_first(
    ecs_vec_t *vec);

#define ecs_vec_first_t(vec, T) \
    ECS_CAST(T*, ecs_vec_first(vec))

void* ecs_vec_last(
    ecs_vec_t *vec,
    ecs_size_t size);

#define ecs_vec_last_t(vec, T) \
    ECS_CAST(T*, ecs_vec_last(vec, ECS_SIZEOF(T)))

#endif 
