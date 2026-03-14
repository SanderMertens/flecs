/**
 * @file sparse.h
 * @brief Sparse set data structure.
 */

#ifndef FLECS_SPARSE_H
#define FLECS_SPARSE_H

#include "../private/api_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/** The number of elements in a single page. */
#define FLECS_SPARSE_PAGE_SIZE (1 << FLECS_SPARSE_PAGE_BITS)

/** Compute the page index from an ID by stripping the first 12 bits. */
#define FLECS_SPARSE_PAGE(index) ((int32_t)((uint32_t)index >> FLECS_SPARSE_PAGE_BITS))

/** Compute the offset of an index inside a page. */
#define FLECS_SPARSE_OFFSET(index) ((int32_t)index & (FLECS_SPARSE_PAGE_SIZE - 1))

/** A page in the sparse set containing a sparse-to-dense mapping and data. */
typedef struct ecs_sparse_page_t {
    int32_t *sparse;            /**< Sparse array with indices to dense array. */
    void *data;                 /**< Store data in sparse array to reduce
                                 *   indirection and provide stable pointers. */
} ecs_sparse_page_t;

/** A sparse set data structure for O(1) access with stable pointers. */
typedef struct ecs_sparse_t {
    ecs_vec_t dense;            /**< Dense array with indices to sparse array. The
                                 *   dense array stores both alive and not alive
                                 *   sparse indices. The 'count' member keeps
                                 *   track of which indices are alive. */
    ecs_vec_t pages;            /**< Chunks with sparse arrays and data. */
    ecs_size_t size;            /**< Element size in bytes. */
    int32_t count;              /**< Number of alive entries. */
    uint64_t max_id;            /**< Local max index (if no global is set). */
    struct ecs_allocator_t *allocator;       /**< Allocator for general allocations. */
    struct ecs_block_allocator_t *page_allocator; /**< Allocator for page allocations. */
} ecs_sparse_t;

/** Initialize a sparse set.
 *
 * @param result The sparse set to initialize.
 * @param allocator Allocator for general memory management.
 * @param page_allocator Block allocator for page allocations.
 * @param size Size of each element in bytes.
 */
FLECS_DBG_API
void flecs_sparse_init(
    ecs_sparse_t *result,
    struct ecs_allocator_t *allocator,
    struct ecs_block_allocator_t *page_allocator,
    ecs_size_t size);

/** Typed sparse set initialization.
 *
 * @param result The sparse set to initialize.
 * @param allocator Allocator for general memory management.
 * @param page_allocator Block allocator for page allocations.
 * @param T The element type.
 */
#define flecs_sparse_init_t(result, allocator, page_allocator, T)\
    flecs_sparse_init(result, allocator, page_allocator, ECS_SIZEOF(T))

/** Deinitialize a sparse set.
 *
 * @param sparse The sparse set to deinitialize.
 */
FLECS_DBG_API
void flecs_sparse_fini(
    ecs_sparse_t *sparse);

/** Remove all elements from a sparse set.
 *
 * @param sparse The sparse set to clear.
 */
FLECS_DBG_API
void flecs_sparse_clear(
    ecs_sparse_t *sparse);

/** Add an element to a sparse set. This generates or recycles an ID.
 *
 * @param sparse The sparse set to add to.
 * @param elem_size Size of each element in bytes.
 * @return Pointer to the newly added element.
 */
FLECS_DBG_API
void* flecs_sparse_add(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size);

/** Typed element add.
 *
 * @param sparse The sparse set to add to.
 * @param T The element type.
 * @return Typed pointer to the newly added element.
 */
#define flecs_sparse_add_t(sparse, T)\
    ECS_CAST(T*, flecs_sparse_add(sparse, ECS_SIZEOF(T)))

/** Get the last issued ID.
 *
 * @param sparse The sparse set.
 * @return The last issued ID.
 */
FLECS_DBG_API
uint64_t flecs_sparse_last_id(
    const ecs_sparse_t *sparse);

/** Generate or recycle a new ID.
 *
 * @param sparse The sparse set.
 * @return A new or recycled ID.
 */
FLECS_DBG_API
uint64_t flecs_sparse_new_id(
    ecs_sparse_t *sparse);

/** Remove an element.
 *
 * @param sparse The sparse set to remove from.
 * @param size Size of each element in bytes.
 * @param id The ID of the element to remove.
 * @return True if the element was found and removed.
 */
FLECS_DBG_API
bool flecs_sparse_remove(
    ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t id);

/** Typed element removal.
 *
 * @param sparse The sparse set to remove from.
 * @param T The element type.
 * @param id The ID of the element to remove.
 */
#define flecs_sparse_remove_t(sparse, T, id)\
    flecs_sparse_remove(sparse, ECS_SIZEOF(T), id)

/** Remove an element and increase the generation.
 *
 * @param sparse The sparse set to remove from.
 * @param size Size of each element in bytes.
 * @param id The ID of the element to remove.
 * @return True if the element was found and removed.
 */
bool flecs_sparse_remove_w_gen(
    ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t id);

/** Typed element removal with generation increase.
 *
 * @param sparse The sparse set to remove from.
 * @param T The element type.
 * @param id The ID of the element to remove.
 */
#define flecs_sparse_remove_w_gen_t(sparse, T, id)\
    flecs_sparse_remove_w_gen(sparse, ECS_SIZEOF(T), id)

/** Test if an ID is alive, which requires the generation count to match.
 *
 * @param sparse The sparse set to check.
 * @param id The ID to test for liveness.
 * @return True if the ID is alive.
 */
FLECS_DBG_API
bool flecs_sparse_is_alive(
    const ecs_sparse_t *sparse,
    uint64_t id);

/** Get a value from a sparse set by dense ID. This function is useful in
 * combination with flecs_sparse_count() for iterating all values in the set.
 *
 * @param sparse The sparse set to retrieve from.
 * @param elem_size Size of each element in bytes.
 * @param index Dense index of the element.
 * @return Pointer to the element at the given dense index.
 */
FLECS_DBG_API
void* flecs_sparse_get_dense(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    int32_t index);

/** Typed get by dense index.
 *
 * @param sparse The sparse set to retrieve from.
 * @param T The element type.
 * @param index Dense index of the element.
 * @return Typed pointer to the element.
 */
#define flecs_sparse_get_dense_t(sparse, T, index)\
    ECS_CAST(T*, flecs_sparse_get_dense(sparse, ECS_SIZEOF(T), index))

/** Get the number of alive elements in the sparse set.
 *
 * @param sparse The sparse set.
 * @return The number of alive elements.
 */
FLECS_DBG_API
int32_t flecs_sparse_count(
    const ecs_sparse_t *sparse);

/** Check if a sparse set has an ID.
 *
 * @param sparse The sparse set to check.
 * @param id The ID to look for.
 * @return True if the sparse set contains the ID.
 */
bool flecs_sparse_has(
    const ecs_sparse_t *sparse,
    uint64_t id);

/** Get element by sparse ID, regardless of whether the element is alive or not.
 *
 * @param sparse The sparse set to retrieve from.
 * @param elem_size Size of each element in bytes.
 * @param id The sparse ID of the element.
 * @return Pointer to the element, regardless of liveness.
 */
FLECS_DBG_API
void* flecs_sparse_get(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

/** Typed get by sparse ID.
 *
 * @param sparse The sparse set to retrieve from.
 * @param T The element type.
 * @param index The sparse ID of the element.
 * @return Typed pointer to the element.
 */
#define flecs_sparse_get_t(sparse, T, index)\
    ECS_CAST(T*, flecs_sparse_get(sparse, ECS_SIZEOF(T), index))

/** Create an element by (sparse) ID.
 *
 * @param sparse The sparse set to insert into.
 * @param elem_size Size of each element in bytes.
 * @param id The sparse ID for the new element.
 * @return Pointer to the newly created element.
 */
FLECS_DBG_API
void* flecs_sparse_insert(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

/** Typed insert by sparse ID.
 *
 * @param sparse The sparse set to insert into.
 * @param T The element type.
 * @param index The sparse ID for the new element.
 * @return Typed pointer to the newly created element.
 */
#define flecs_sparse_insert_t(sparse, T, index)\
    ECS_CAST(T*, flecs_sparse_insert(sparse, ECS_SIZEOF(T), index))

/** Get or create an element by (sparse) ID.
 *
 * @param sparse The sparse set.
 * @param elem_size Size of each element in bytes.
 * @param id The sparse ID to get or create.
 * @param is_new Output parameter set to true if a new element was created.
 * @return Pointer to the existing or newly created element.
 */
FLECS_DBG_API
void* flecs_sparse_ensure(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id,
    bool *is_new);

/** Typed get or create by sparse ID.
 *
 * @param sparse The sparse set.
 * @param T The element type.
 * @param index The sparse ID to get or create.
 * @param is_new Output parameter set to true if a new element was created.
 * @return Typed pointer to the element.
 */
#define flecs_sparse_ensure_t(sparse, T, index, is_new)\
    ECS_CAST(T*, flecs_sparse_ensure(sparse, ECS_SIZEOF(T), index, is_new))

/** Fast version of ensure with no liveness checking.
 *
 * @param sparse The sparse set.
 * @param elem_size Size of each element in bytes.
 * @param id The sparse ID to get or create.
 * @return Pointer to the element.
 */
FLECS_DBG_API
void* flecs_sparse_ensure_fast(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

/** Typed fast ensure without liveness checking.
 *
 * @param sparse The sparse set.
 * @param T The element type.
 * @param index The sparse ID to get or create.
 * @return Typed pointer to the element.
 */
#define flecs_sparse_ensure_fast_t(sparse, T, index)\
    ECS_CAST(T*, flecs_sparse_ensure_fast(sparse, ECS_SIZEOF(T), index))

/** Get a pointer to IDs (alive and not alive). Use with flecs_sparse_count().
 *
 * @param sparse The sparse set.
 * @return Pointer to the dense array of IDs.
 */
FLECS_DBG_API
const uint64_t* flecs_sparse_ids(
    const ecs_sparse_t *sparse);

/** Shrink sparse set memory to fit current usage.
 *
 * @param sparse The sparse set to shrink.
 */
FLECS_DBG_API
void flecs_sparse_shrink(
    ecs_sparse_t *sparse);

/** Exposed but unstable APIs.
 * These APIs are visible in the header but not part of the stable public API,
 * and as a result may change without notice. */

/** Initialize a public sparse set.
 *
 * @param sparse The sparse set to initialize.
 * @param elem_size Size of each element in bytes.
 */
FLECS_API
void ecs_sparse_init(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size);

/** Typed public sparse set initialization.
 *
 * @param sparse The sparse set to initialize.
 * @param T The element type.
 */
#define ecs_sparse_init_t(sparse, T)\
    ecs_sparse_init(sparse, ECS_SIZEOF(T))

/** Add an element to a public sparse set.
 *
 * @param sparse The sparse set to add to.
 * @param elem_size Size of each element in bytes.
 * @return Pointer to the newly added element.
 */
FLECS_API
void* ecs_sparse_add(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size);

/** Typed public sparse set add.
 *
 * @param sparse The sparse set to add to.
 * @param T The element type.
 * @return Typed pointer to the newly added element.
 */
#define ecs_sparse_add_t(sparse, T)\
    ECS_CAST(T*, ecs_sparse_add(sparse, ECS_SIZEOF(T)))

/** Get the last issued ID from a public sparse set.
 *
 * @param sparse The sparse set.
 * @return The last issued ID.
 */
FLECS_API
uint64_t ecs_sparse_last_id(
    const ecs_sparse_t *sparse);

/** Get the number of alive elements in a public sparse set.
 *
 * @param sparse The sparse set.
 * @return The number of alive elements.
 */
FLECS_API
int32_t ecs_sparse_count(
    const ecs_sparse_t *sparse);

/** Get a value from a public sparse set by dense index.
 *
 * @param sparse The sparse set.
 * @param elem_size Size of each element in bytes.
 * @param index Dense index of the element.
 * @return Pointer to the element.
 */
FLECS_API
void* ecs_sparse_get_dense(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    int32_t index);

/** Typed public get by dense index.
 *
 * @param sparse The sparse set.
 * @param T The element type.
 * @param index Dense index of the element.
 * @return Typed pointer to the element.
 */
#define ecs_sparse_get_dense_t(sparse, T, index)\
    ECS_CAST(T*, ecs_sparse_get_dense(sparse, ECS_SIZEOF(T), index))

/** Get a value from a public sparse set by sparse ID.
 *
 * @param sparse The sparse set.
 * @param elem_size Size of each element in bytes.
 * @param id The sparse ID of the element.
 * @return Pointer to the element.
 */
FLECS_API
void* ecs_sparse_get(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

/** Typed public get by sparse ID.
 *
 * @param sparse The sparse set.
 * @param T The element type.
 * @param index The sparse ID of the element.
 * @return Typed pointer to the element.
 */
#define ecs_sparse_get_t(sparse, T, index)\
    ECS_CAST(T*, ecs_sparse_get(sparse, ECS_SIZEOF(T), index))

#ifdef __cplusplus
}
#endif

#endif
