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
#ifdef FLECS_DEBUG
    ecs_size_t elem_size;
#endif
} ecs_vec_t;

FLECS_API
ecs_vec_t* ecs_vec_init(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

#define ecs_vec_init_t(allocator, vec, T, elem_count) \
    ecs_vec_init(allocator, vec, ECS_SIZEOF(T), elem_count)

FLECS_API
void ecs_vec_fini(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size);

#define ecs_vec_fini_t(allocator, vec, T) \
    ecs_vec_fini(allocator, vec, ECS_SIZEOF(T))

FLECS_API
ecs_vec_t* ecs_vec_reset(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size);

#define ecs_vec_reset_t(allocator, vec, T) \
    ecs_vec_reset(allocator, vec, ECS_SIZEOF(T))

FLECS_API
void ecs_vec_clear(
    ecs_vec_t *vec);

FLECS_API
void* ecs_vec_append(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size);

#define ecs_vec_append_t(allocator, vec, T) \
    ECS_CAST(T*, ecs_vec_append(allocator, vec, ECS_SIZEOF(T)))

FLECS_API
void ecs_vec_remove(
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem);

#define ecs_vec_remove_t(vec, T, elem) \
    ecs_vec_remove(vec, ECS_SIZEOF(T), elem)

FLECS_API
void ecs_vec_remove_last(
    ecs_vec_t *vec);

FLECS_API
ecs_vec_t ecs_vec_copy(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size);

#define ecs_vec_copy_t(allocator, vec, T) \
    ecs_vec_copy(allocator, vec, ECS_SIZEOF(T))

FLECS_API
void ecs_vec_reclaim(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size);

#define ecs_vec_reclaim_t(allocator, vec, T) \
    ecs_vec_reclaim(allocator, vec, ECS_SIZEOF(T))

FLECS_API
void ecs_vec_set_size(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

#define ecs_vec_set_size_t(allocator, vec, T, elem_count) \
    ecs_vec_set_size(allocator, vec, ECS_SIZEOF(T), elem_count)

FLECS_API
void ecs_vec_set_count(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

#define ecs_vec_set_count_t(allocator, vec, T, elem_count) \
    ecs_vec_set_count(allocator, vec, ECS_SIZEOF(T), elem_count)

FLECS_API
void* ecs_vec_grow(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

#define ecs_vec_grow_t(allocator, vec, T, elem_count) \
    ecs_vec_grow(allocator, vec, ECS_SIZEOF(T), elem_count)

FLECS_API
int32_t ecs_vec_count(
    const ecs_vec_t *vec);

FLECS_API
int32_t ecs_vec_size(
    const ecs_vec_t *vec);

FLECS_API
void* ecs_vec_get(
    const ecs_vec_t *vec,
    ecs_size_t size,
    int32_t index);

#define ecs_vec_get_t(vec, T, index) \
    ECS_CAST(T*, ecs_vec_get(vec, ECS_SIZEOF(T), index))

FLECS_API
void* ecs_vec_first(
    const ecs_vec_t *vec);

#define ecs_vec_first_t(vec, T) \
    ECS_CAST(T*, ecs_vec_first(vec))

FLECS_API
void* ecs_vec_last(
    const ecs_vec_t *vec,
    ecs_size_t size);

#define ecs_vec_last_t(vec, T) \
    ECS_CAST(T*, ecs_vec_last(vec, ECS_SIZEOF(T)))

#endif 
