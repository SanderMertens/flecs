/**
 * @file allocator.h
 * @brief Allocator that returns memory objects of any size.
 */

#ifndef FLECS_ALLOCATOR_H
#define FLECS_ALLOCATOR_H

#include "../private/api_defines.h"

FLECS_DBG_API extern int64_t ecs_block_allocator_alloc_count; /**< Block allocator allocation count. */
FLECS_DBG_API extern int64_t ecs_block_allocator_free_count; /**< Block allocator free count. */
FLECS_DBG_API extern int64_t ecs_stack_allocator_alloc_count; /**< Stack allocator allocation count. */
FLECS_DBG_API extern int64_t ecs_stack_allocator_free_count; /**< Stack allocator free count. */

/** General purpose allocator that manages block allocators for different sizes. */
struct ecs_allocator_t {
#ifndef FLECS_USE_OS_ALLOC
    ecs_block_allocator_t chunks; /**< Block allocator for chunk storage. */
    struct ecs_sparse_t sizes; /**< Sparse set mapping size to block allocator. */
#else
    bool dummy; /**< Unused member for OS allocator fallback. */
#endif
};

/** Initialize an allocator.
 *
 * @param a The allocator to initialize.
 */
FLECS_API
void flecs_allocator_init(
    ecs_allocator_t *a);

/** Deinitialize an allocator.
 *
 * @param a The allocator to deinitialize.
 */
FLECS_API
void flecs_allocator_fini(
    ecs_allocator_t *a);

/** Get or create a block allocator for the specified size.
 *
 * @param a The allocator.
 * @param size The allocation size.
 * @return The block allocator for the given size.
 */
FLECS_API
ecs_block_allocator_t* flecs_allocator_get(
    ecs_allocator_t *a,
    ecs_size_t size);

/** Duplicate a string using the allocator.
 *
 * @param a The allocator.
 * @param str The string to duplicate.
 * @return The duplicated string.
 */
FLECS_API
char* flecs_strdup(
    ecs_allocator_t *a,
    const char* str);

/** Free a string previously allocated with flecs_strdup().
 *
 * @param a The allocator.
 * @param str The string to free.
 */
FLECS_API
void flecs_strfree(
    ecs_allocator_t *a,
    char* str);

/** Duplicate a memory block using the allocator.
 *
 * @param a The allocator.
 * @param size The size of the memory block.
 * @param src The source memory to duplicate.
 * @return Pointer to the duplicated memory.
 */
FLECS_API
void* flecs_dup(
    ecs_allocator_t *a,
    ecs_size_t size,
    const void *src);

#ifndef FLECS_USE_OS_ALLOC

/** Get the dynamic allocator from an object. */
#define flecs_allocator(obj) (&obj->allocators.dyn)

/** Allocate memory of a given size. */
#define flecs_alloc(a, size) flecs_balloc(flecs_allocator_get(a, size))

/** Allocate memory with debug type name info. */
#define flecs_alloc_w_dbg_info(a, size, type_name) flecs_balloc_w_dbg_info(flecs_allocator_get(a, size), type_name)

/** Allocate memory for a single element of type T. */
#define flecs_alloc_t(a, T) flecs_alloc_w_dbg_info(a, ECS_SIZEOF(T), #T)

/** Allocate memory for count elements of type T. */
#define flecs_alloc_n(a, T, count) flecs_alloc_w_dbg_info(a, ECS_SIZEOF(T) * (count), #T)

/** Allocate zeroed memory of a given size. */
#define flecs_calloc(a, size) flecs_bcalloc(flecs_allocator_get(a, size))

/** Allocate zeroed memory with debug type name info. */
#define flecs_calloc_w_dbg_info(a, size, type_name) flecs_bcalloc_w_dbg_info(flecs_allocator_get(a, size), type_name)

/** Allocate zeroed memory for a single element of type T. */
#define flecs_calloc_t(a, T) flecs_calloc_w_dbg_info(a, ECS_SIZEOF(T), #T)

/** Allocate zeroed memory for count elements of type T. */
#define flecs_calloc_n(a, T, count) flecs_calloc_w_dbg_info(a, ECS_SIZEOF(T) * (count), #T)

/** Free memory of a given size. */
#define flecs_free(a, size, ptr)\
    flecs_bfree((ptr) ? flecs_allocator_get(a, size) : NULL, ptr)

/** Free memory for a single element of type T. */
#define flecs_free_t(a, T, ptr)\
    flecs_bfree_w_dbg_info((ptr) ? flecs_allocator_get(a, ECS_SIZEOF(T)) : NULL, ptr, #T)

/** Free memory for count elements of type T. */
#define flecs_free_n(a, T, count, ptr)\
    flecs_bfree_w_dbg_info((ptr) ? flecs_allocator_get(a, ECS_SIZEOF(T) * (count)) : NULL\
        , ptr, #T)

/** Reallocate memory from one size to another. */
#define flecs_realloc(a, size_dst, size_src, ptr)\
    flecs_brealloc(flecs_allocator_get(a, size_dst),\
    flecs_allocator_get(a, size_src),\
    ptr)

/** Reallocate memory with debug type name info. */
#define flecs_realloc_w_dbg_info(a, size_dst, size_src, ptr, type_name)\
    flecs_brealloc_w_dbg_info(flecs_allocator_get(a, size_dst),\
    flecs_allocator_get(a, size_src),\
    ptr,\
    type_name)

/** Reallocate memory for count elements of type T. */
#define flecs_realloc_n(a, T, count_dst, count_src, ptr)\
    flecs_realloc(a, ECS_SIZEOF(T) * (count_dst), ECS_SIZEOF(T) * (count_src), ptr)

/** Duplicate count elements of type T. */
#define flecs_dup_n(a, T, count, ptr) flecs_dup(a, ECS_SIZEOF(T) * (count), ptr)

#else

/** Allocate memory of a given size (OS allocator fallback).
 *
 * @param a The allocator (unused).
 * @param size The allocation size.
 * @return Pointer to the allocated memory.
 */
void* flecs_alloc(
    ecs_allocator_t *a,
    ecs_size_t size);

/** Allocate zeroed memory of a given size (OS allocator fallback).
 *
 * @param a The allocator (unused).
 * @param size The allocation size.
 * @return Pointer to the zeroed memory.
 */
void* flecs_calloc(
    ecs_allocator_t *a,
    ecs_size_t size);

/** Reallocate memory from one size to another (OS allocator fallback).
 *
 * @param a The allocator (unused).
 * @param dst_size The new allocation size.
 * @param src_size The old allocation size.
 * @param ptr The pointer to reallocate.
 * @return Pointer to the reallocated memory.
 */
void* flecs_realloc(
    ecs_allocator_t *a,
    ecs_size_t dst_size,
    ecs_size_t src_size,
    void *ptr);

/** Free memory of a given size (OS allocator fallback).
 *
 * @param a The allocator (unused).
 * @param size The allocation size.
 * @param ptr The pointer to free.
 */
void flecs_free(
    ecs_allocator_t *a,
    ecs_size_t size,
    void *ptr);

/** Allocate memory with debug type name info (OS allocator fallback). */
#define flecs_alloc_w_dbg_info(a, size, type_name) flecs_alloc(a, size)

/** Allocate memory for a single element of type T (OS allocator fallback). */
#define flecs_alloc_t(a, T) flecs_alloc(a, ECS_SIZEOF(T))

/** Allocate memory for count elements of type T (OS allocator fallback). */
#define flecs_alloc_n(a, T, count) flecs_alloc(a, ECS_SIZEOF(T) * (count))

/** Allocate zeroed memory with debug type name info (OS allocator fallback). */
#define flecs_calloc_w_dbg_info(a, size, type_name) flecs_calloc(a, size)

/** Allocate zeroed memory for a single element of type T (OS allocator fallback). */
#define flecs_calloc_t(a, T) flecs_calloc(a, ECS_SIZEOF(T))

/** Allocate zeroed memory for count elements of type T (OS allocator fallback). */
#define flecs_calloc_n(a, T, count) flecs_calloc(a, ECS_SIZEOF(T) * (count))

/** Free memory for a single element of type T (OS allocator fallback). */
#define flecs_free_t(a, T, ptr) flecs_free(a, ECS_SIZEOF(T), ptr)

/** Free memory for count elements of type T (OS allocator fallback). */
#define flecs_free_n(a, T, count, ptr) flecs_free(a, ECS_SIZEOF(T) * (count), ptr)

/** Reallocate memory with debug type name info (OS allocator fallback). */
#define flecs_realloc_w_dbg_info(a, size_dst, size_src, ptr, type_name)\
    flecs_realloc(a, size_dst, size_src, ptr)

/** Reallocate memory for count elements of type T (OS allocator fallback). */
#define flecs_realloc_n(a, T, count_dst, count_src, ptr)\
    flecs_realloc(a, ECS_SIZEOF(T) * count_dst, ECS_SIZEOF(T) * count_src, ptr)

/** Duplicate count elements of type T (OS allocator fallback). */
#define flecs_dup_n(a, T, count, ptr) flecs_dup(a, ECS_SIZEOF(T) * (count), ptr)

#endif

#endif
