/**
 * @file allocator.h
 * @brief Allocator that returns memory objects of any size. 
 */

#ifndef FLECS_ALLOCATOR_H
#define FLECS_ALLOCATOR_H

#include "api_defines.h"

typedef struct ecs_allocator_t {
    struct ecs_map_t sizes; /* <size, block_allocator_t> */
} ecs_allocator_t;

void flecs_allocator_init(
    ecs_allocator_t *a);

void flecs_allocator_fini(
    ecs_allocator_t *a);

ecs_block_allocator_t* flecs_allocator_get(
    ecs_allocator_t *a, 
    size_t size);

#define flecs_allocator(obj) (&obj->allocators.dyn)

#define flecs_alloc(a, size) flecs_balloc(flecs_allocator_get(a, size))
#define flecs_alloc_n(a, T, count) flecs_alloc(a, ECS_SIZEOF(T) * (count))

#define flecs_calloc(a, size) flecs_bcalloc(flecs_allocator_get(a, size))
#define flecs_calloc_n(a, T, count) flecs_calloc(a, ECS_SIZEOF(T) * (count))

#define flecs_free(a, size, ptr) flecs_bfree(flecs_allocator_get(a, size), ptr)
#define flecs_free_n(a, T, count, ptr) flecs_free(a, ECS_SIZEOF(T) * (count), ptr)

#define flecs_realloc(a, size_dst, size_src, ptr)\
    flecs_brealloc(flecs_allocator_get(a, size_dst),\
    flecs_allocator_get(a, size_src),\
    ptr)
#define flecs_realloc_n(a, T, count_dst, count_src, ptr)\
    flecs_realloc(a, ECS_SIZEOF(T) * (count_dst), ECS_SIZEOF(T) * (count_src), ptr)

#define flecs_dup(a, size, ptr) flecs_bdup(flecs_allocator_get(a, size), ptr)
#define flecs_dup_n(a, T, count, ptr) flecs_dup(a, ECS_SIZEOF(T) * (count), ptr)

#endif
