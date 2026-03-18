/**
 * @file datastructures/block_allocator.h
 * @brief Block allocator.
 */

#ifndef FLECS_BLOCK_ALLOCATOR_H
#define FLECS_BLOCK_ALLOCATOR_H

#include "../private/api_defines.h"

/** Forward declaration of map type. */
typedef struct ecs_map_t ecs_map_t;

/** A block of memory managed by the block allocator. */
typedef struct ecs_block_allocator_block_t {
    void *memory; /**< Pointer to the block memory. */
    struct ecs_block_allocator_block_t *next; /**< Next block in the list. */
} ecs_block_allocator_block_t;

/** Header for a free chunk in the block allocator free list. */
typedef struct ecs_block_allocator_chunk_header_t {
    struct ecs_block_allocator_chunk_header_t *next; /**< Next free chunk. */
} ecs_block_allocator_chunk_header_t;

/** Block allocator that returns fixed-size memory blocks. */
typedef struct ecs_block_allocator_t {
    int32_t data_size; /**< Size of each allocation. */
#ifndef FLECS_USE_OS_ALLOC
    int32_t chunk_size; /**< Aligned chunk size including header. */
    int32_t chunks_per_block; /**< Number of chunks per block. */
    int32_t block_size; /**< Total size of each allocated block. */
    ecs_block_allocator_chunk_header_t *head; /**< Head of the free chunk list. */
    ecs_block_allocator_block_t *block_head; /**< Head of the allocated block list. */
#ifdef FLECS_SANITIZE
    int32_t alloc_count; /**< Number of outstanding allocations (sanitizer only). */
    ecs_map_t *outstanding; /**< Map of outstanding allocations (sanitizer only). */
#endif
#endif
} ecs_block_allocator_t;

/** Initialize a block allocator.
 *
 * @param ba The block allocator to initialize.
 * @param size The size of each allocation.
 */
FLECS_API
void flecs_ballocator_init(
    ecs_block_allocator_t *ba,
    ecs_size_t size);

/** Initialize a block allocator for type T. */
#define flecs_ballocator_init_t(ba, T)\
    flecs_ballocator_init(ba, ECS_SIZEOF(T))

/** Initialize a block allocator for count elements of type T. */
#define flecs_ballocator_init_n(ba, T, count)\
    flecs_ballocator_init(ba, ECS_SIZEOF(T) * count)

/** Create a new block allocator on the heap.
 *
 * @param size The size of each allocation.
 * @return The new block allocator.
 */
FLECS_API
ecs_block_allocator_t* flecs_ballocator_new(
    ecs_size_t size);

/** Create a new block allocator for type T. */
#define flecs_ballocator_new_t(T)\
    flecs_ballocator_new(ECS_SIZEOF(T))

/** Create a new block allocator for count elements of type T. */
#define flecs_ballocator_new_n(T, count)\
    flecs_ballocator_new(ECS_SIZEOF(T) * count)

/** Deinitialize a block allocator.
 *
 * @param ba The block allocator to deinitialize.
 */
FLECS_API
void flecs_ballocator_fini(
    ecs_block_allocator_t *ba);

/** Free a block allocator created with flecs_ballocator_new().
 *
 * @param ba The block allocator to free.
 */
FLECS_API
void flecs_ballocator_free(
    ecs_block_allocator_t *ba);

/** Allocate a block of memory.
 *
 * @param allocator The block allocator.
 * @return Pointer to the allocated memory.
 */
FLECS_API
void* flecs_balloc(
    ecs_block_allocator_t *allocator);

/** Allocate a block of memory with debug type name info.
 *
 * @param allocator The block allocator.
 * @param type_name The type name for debug tracking.
 * @return Pointer to the allocated memory.
 */
FLECS_API
void* flecs_balloc_w_dbg_info(
    ecs_block_allocator_t *allocator,
    const char *type_name);

/** Allocate a zeroed block of memory.
 *
 * @param allocator The block allocator.
 * @return Pointer to the zeroed memory.
 */
FLECS_API
void* flecs_bcalloc(
    ecs_block_allocator_t *allocator);

/** Allocate a zeroed block of memory with debug type name info.
 *
 * @param allocator The block allocator.
 * @param type_name The type name for debug tracking.
 * @return Pointer to the zeroed memory.
 */
FLECS_API
void* flecs_bcalloc_w_dbg_info(
    ecs_block_allocator_t *allocator,
    const char *type_name);

/** Free a block of memory.
 *
 * @param allocator The block allocator.
 * @param memory The memory to free.
 */
FLECS_API
void flecs_bfree(
    ecs_block_allocator_t *allocator,
    void *memory);

/** Free a block of memory with debug type name info.
 *
 * @param allocator The block allocator.
 * @param memory The memory to free.
 * @param type_name The type name for debug tracking.
 */
FLECS_API
void flecs_bfree_w_dbg_info(
    ecs_block_allocator_t *allocator,
    void *memory,
    const char *type_name);

/** Reallocate a block from one block allocator to another.
 *
 * @param dst The destination block allocator.
 * @param src The source block allocator.
 * @param memory The memory to reallocate.
 * @return Pointer to the reallocated memory.
 */
FLECS_API
void* flecs_brealloc(
    ecs_block_allocator_t *dst,
    ecs_block_allocator_t *src,
    void *memory);

/** Reallocate a block with debug type name info.
 *
 * @param dst The destination block allocator.
 * @param src The source block allocator.
 * @param memory The memory to reallocate.
 * @param type_name The type name for debug tracking.
 * @return Pointer to the reallocated memory.
 */
FLECS_API
void* flecs_brealloc_w_dbg_info(
    ecs_block_allocator_t *dst,
    ecs_block_allocator_t *src,
    void *memory,
    const char *type_name);

/** Duplicate a block of memory.
 *
 * @param ba The block allocator.
 * @param memory The memory to duplicate.
 * @return Pointer to the duplicated memory.
 */
FLECS_API
void* flecs_bdup(
    ecs_block_allocator_t *ba,
    void *memory);

#endif
