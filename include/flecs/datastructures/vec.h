/**
 * @file vec.h
 * @brief Vector with allocator support.
 */

#ifndef FLECS_VEC_H
#define FLECS_VEC_H

#include "../private/api_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A component column. */
typedef struct ecs_vec_t {
    void *array;              /**< Pointer to the element array. */
    int32_t count;            /**< Number of elements in the vector. */
    int32_t size;             /**< Allocated capacity in number of elements. */
#ifdef FLECS_SANITIZE
    ecs_size_t elem_size;     /**< Size of each element in bytes (sanitize only). */
    const char *type_name;    /**< Type name string for debugging (sanitize only). */
#endif
} ecs_vec_t;

/** Initialize a vector.
 *
 * @param allocator Allocator to use for memory management.
 * @param vec The vector to initialize.
 * @param size Size of each element in bytes.
 * @param elem_count Initial number of elements to allocate.
 */
FLECS_API
void ecs_vec_init(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

/** Initialize a vector with debug info.
 *
 * @param allocator Allocator to use for memory management.
 * @param vec The vector to initialize.
 * @param size Size of each element in bytes.
 * @param elem_count Initial number of elements to allocate.
 * @param type_name Type name string for debugging.
 */
FLECS_API
void ecs_vec_init_w_dbg_info(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count,
    const char *type_name);

/** Type-safe vector initialization.
 *
 * @param allocator Allocator to use for memory management.
 * @param vec The vector to initialize.
 * @param T The element type.
 * @param elem_count Initial number of elements to allocate.
 */
#define ecs_vec_init_t(allocator, vec, T, elem_count) \
    ecs_vec_init_w_dbg_info(allocator, vec, ECS_SIZEOF(T), elem_count, "vec<"#T">")

/** Initialize a vector if it is not already initialized.
 *
 * @param vec The vector to initialize.
 * @param size Size of each element in bytes.
 */
FLECS_API
void ecs_vec_init_if(
    ecs_vec_t *vec,
    ecs_size_t size);

/** Type-safe conditional vector initialization.
 *
 * @param vec The vector to initialize.
 * @param T The element type.
 */
#define ecs_vec_init_if_t(vec, T) \
    ecs_vec_init_if(vec, ECS_SIZEOF(T))

/** Deinitialize a vector.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to deinitialize.
 * @param size Size of each element in bytes.
 */
FLECS_API
void ecs_vec_fini(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size);

/** Type-safe vector deinitialization.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to deinitialize.
 * @param T The element type.
 */
#define ecs_vec_fini_t(allocator, vec, T) \
    ecs_vec_fini(allocator, vec, ECS_SIZEOF(T))

/** Reset a vector. Keeps allocated memory for reuse.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to reset.
 * @param size Size of each element in bytes.
 * @return Pointer to the reset vector.
 */
FLECS_API
ecs_vec_t* ecs_vec_reset(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size);

/** Type-safe vector reset.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to reset.
 * @param T The element type.
 */
#define ecs_vec_reset_t(allocator, vec, T) \
    ecs_vec_reset(allocator, vec, ECS_SIZEOF(T))

/** Clear a vector. Sets count to zero without freeing memory.
 *
 * @param vec The vector to clear.
 */
FLECS_API
void ecs_vec_clear(
    ecs_vec_t *vec);

/** Append a new element to the vector.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to append to.
 * @param size Size of each element in bytes.
 * @return Pointer to the newly appended element.
 */
FLECS_API
void* ecs_vec_append(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size);

/** Type-safe element append.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to append to.
 * @param T The element type.
 * @return Typed pointer to the newly appended element.
 */
#define ecs_vec_append_t(allocator, vec, T) \
    ECS_CAST(T*, ecs_vec_append(allocator, vec, ECS_SIZEOF(T)))

/** Remove an element by swapping with the last element.
 *
 * @param vec The vector to remove from.
 * @param size Size of each element in bytes.
 * @param elem Index of the element to remove.
 */
FLECS_API
void ecs_vec_remove(
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem);

/** Type-safe element removal (swap with last).
 *
 * @param vec The vector to remove from.
 * @param T The element type.
 * @param elem Index of the element to remove.
 */
#define ecs_vec_remove_t(vec, T, elem) \
    ecs_vec_remove(vec, ECS_SIZEOF(T), elem)

/** Remove an element while preserving order.
 *
 * @param v The vector to remove from.
 * @param size Size of each element in bytes.
 * @param index Index of the element to remove.
 */
FLECS_API
void ecs_vec_remove_ordered(
    ecs_vec_t *v,
    ecs_size_t size,
    int32_t index);

/** Type-safe ordered element removal.
 *
 * @param vec The vector to remove from.
 * @param T The element type.
 * @param elem Index of the element to remove.
 */
#define ecs_vec_remove_ordered_t(vec, T, elem) \
    ecs_vec_remove_ordered(vec, ECS_SIZEOF(T), elem)

/** Remove the last element from the vector.
 *
 * @param vec The vector to remove from.
 */
FLECS_API
void ecs_vec_remove_last(
    ecs_vec_t *vec);

/** Copy a vector.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The source vector to copy.
 * @param size Size of each element in bytes.
 * @return A new vector containing copies of all elements.
 */
FLECS_API
ecs_vec_t ecs_vec_copy(
    struct ecs_allocator_t *allocator,
    const ecs_vec_t *vec,
    ecs_size_t size);

/** Type-safe vector copy.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The source vector to copy.
 * @param T The element type.
 */
#define ecs_vec_copy_t(allocator, vec, T) \
    ecs_vec_copy(allocator, vec, ECS_SIZEOF(T))

/** Copy a vector and shrink to fit.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The source vector to copy.
 * @param size Size of each element in bytes.
 * @return A new vector with capacity shrunk to its count.
 */
FLECS_API
ecs_vec_t ecs_vec_copy_shrink(
    struct ecs_allocator_t *allocator,
    const ecs_vec_t *vec,
    ecs_size_t size);

/** Type-safe vector copy and shrink.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The source vector to copy.
 * @param T The element type.
 */
#define ecs_vec_copy_shrink_t(allocator, vec, T) \
    ecs_vec_copy_shrink(allocator, vec, ECS_SIZEOF(T))

/** Reclaim unused memory. Shrinks the vector's allocation to fit its count.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to reclaim memory from.
 * @param size Size of each element in bytes.
 */
FLECS_API
void ecs_vec_reclaim(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size);

/** Type-safe memory reclaim.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to reclaim memory from.
 * @param T The element type.
 */
#define ecs_vec_reclaim_t(allocator, vec, T) \
    ecs_vec_reclaim(allocator, vec, ECS_SIZEOF(T))

/** Set the capacity of a vector.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to resize.
 * @param size Size of each element in bytes.
 * @param elem_count Desired capacity in number of elements.
 */
FLECS_API
void ecs_vec_set_size(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

/** Type-safe set capacity.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to resize.
 * @param T The element type.
 * @param elem_count Desired capacity in number of elements.
 */
#define ecs_vec_set_size_t(allocator, vec, T, elem_count) \
    ecs_vec_set_size(allocator, vec, ECS_SIZEOF(T), elem_count)

/** Set the minimum capacity of a vector. Does not shrink.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to resize.
 * @param size Size of each element in bytes.
 * @param elem_count Minimum capacity in number of elements.
 */
FLECS_API
void ecs_vec_set_min_size(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

/** Type-safe set minimum capacity.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to resize.
 * @param T The element type.
 * @param elem_count Minimum capacity in number of elements.
 */
#define ecs_vec_set_min_size_t(allocator, vec, T, elem_count) \
    ecs_vec_set_min_size(allocator, vec, ECS_SIZEOF(T), elem_count)

/** Set the minimum capacity using type info for lifecycle management.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to resize.
 * @param size Size of each element in bytes.
 * @param elem_count Minimum capacity in number of elements.
 * @param ti Type info for lifecycle callbacks.
 */
FLECS_API
void ecs_vec_set_min_size_w_type_info(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count,
    const ecs_type_info_t *ti);

/** Set the minimum count. Increases count if smaller than elem_count.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to modify.
 * @param size Size of each element in bytes.
 * @param elem_count Minimum element count.
 */
FLECS_API
void ecs_vec_set_min_count(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

/** Type-safe set minimum count.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to modify.
 * @param T The element type.
 * @param elem_count Minimum element count.
 */
#define ecs_vec_set_min_count_t(allocator, vec, T, elem_count) \
    ecs_vec_set_min_count(allocator, vec, ECS_SIZEOF(T), elem_count)

/** Set the minimum count and zero-initialize new elements.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to modify.
 * @param size Size of each element in bytes.
 * @param elem_count Minimum element count.
 */
FLECS_API
void ecs_vec_set_min_count_zeromem(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

/** Type-safe set minimum count with zero-initialization.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to modify.
 * @param T The element type.
 * @param elem_count Minimum element count.
 */
#define ecs_vec_set_min_count_zeromem_t(allocator, vec, T, elem_count) \
    ecs_vec_set_min_count_zeromem(allocator, vec, ECS_SIZEOF(T), elem_count)

/** Set the element count of a vector.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to modify.
 * @param size Size of each element in bytes.
 * @param elem_count Desired element count.
 */
FLECS_API
void ecs_vec_set_count(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

/** Type-safe set element count.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to modify.
 * @param T The element type.
 * @param elem_count Desired element count.
 */
#define ecs_vec_set_count_t(allocator, vec, T, elem_count) \
    ecs_vec_set_count(allocator, vec, ECS_SIZEOF(T), elem_count)

/** Set the element count using type info for lifecycle management.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to modify.
 * @param size Size of each element in bytes.
 * @param elem_count Desired element count.
 * @param ti Type info for lifecycle callbacks.
 */
FLECS_API
void ecs_vec_set_count_w_type_info(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count,
    const ecs_type_info_t *ti);

/** Set the minimum count using type info for lifecycle management.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to modify.
 * @param size Size of each element in bytes.
 * @param elem_count Minimum element count.
 * @param ti Type info for lifecycle callbacks.
 */
FLECS_API
void ecs_vec_set_min_count_w_type_info(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count,
    const ecs_type_info_t *ti);

/** Grow the vector by a number of elements.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to grow.
 * @param size Size of each element in bytes.
 * @param elem_count Number of elements to grow by.
 * @return Pointer to the first newly added element.
 */
FLECS_API
void* ecs_vec_grow(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    int32_t elem_count);

/** Type-safe vector grow.
 *
 * @param allocator Allocator used for memory management.
 * @param vec The vector to grow.
 * @param T The element type.
 * @param elem_count Number of elements to grow by.
 */
#define ecs_vec_grow_t(allocator, vec, T, elem_count) \
    ecs_vec_grow(allocator, vec, ECS_SIZEOF(T), elem_count)

/** Return the number of elements in the vector.
 *
 * @param vec The vector.
 * @return The number of elements.
 */
FLECS_API
int32_t ecs_vec_count(
    const ecs_vec_t *vec);

/** Return the allocated capacity of the vector.
 *
 * @param vec The vector.
 * @return The allocated capacity in number of elements.
 */
FLECS_API
int32_t ecs_vec_size(
    const ecs_vec_t *vec);

/** Get a pointer to an element at the given index.
 *
 * @param vec The vector.
 * @param size Size of each element in bytes.
 * @param index Index of the element to retrieve.
 * @return Pointer to the element.
 */
FLECS_API
void* ecs_vec_get(
    const ecs_vec_t *vec,
    ecs_size_t size,
    int32_t index);

/** Type-safe element access by index.
 *
 * @param vec The vector.
 * @param T The element type.
 * @param index Index of the element to retrieve.
 * @return Typed pointer to the element.
 */
#define ecs_vec_get_t(vec, T, index) \
    ECS_CAST(T*, ecs_vec_get(vec, ECS_SIZEOF(T), index))

/** Get a pointer to the first element.
 *
 * @param vec The vector.
 * @return Pointer to the first element, or NULL if empty.
 */
FLECS_API
void* ecs_vec_first(
    const ecs_vec_t *vec);

/** Type-safe access to the first element.
 *
 * @param vec The vector.
 * @param T The element type.
 * @return Typed pointer to the first element, or NULL if empty.
 */
#define ecs_vec_first_t(vec, T) \
    ECS_CAST(T*, ecs_vec_first(vec))

/** Get a pointer to the last element.
 *
 * @param vec The vector.
 * @param size Size of each element in bytes.
 * @return Pointer to the last element, or NULL if empty.
 */
FLECS_API
void* ecs_vec_last(
    const ecs_vec_t *vec,
    ecs_size_t size);

/** Type-safe access to the last element.
 *
 * @param vec The vector.
 * @param T The element type.
 * @return Typed pointer to the last element, or NULL if empty.
 */
#define ecs_vec_last_t(vec, T) \
    ECS_CAST(T*, ecs_vec_last(vec, ECS_SIZEOF(T)))

#ifdef __cplusplus
}
#endif

#endif
