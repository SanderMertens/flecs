/**
 * @file datastructures/stack_allocator.h
 * @brief Stack allocator.
 */

#ifndef FLECS_STACK_ALLOCATOR_H
#define FLECS_STACK_ALLOCATOR_H

/** Stack allocator for quick allocation of small temporary values */
#define ECS_STACK_PAGE_SIZE (4096)

typedef struct ecs_stack_page_t {
    void *data;
    struct ecs_stack_page_t *next;
    int16_t sp;
    uint32_t id;
} ecs_stack_page_t;

typedef struct ecs_stack_t {
    ecs_stack_page_t first;
    ecs_stack_page_t *cur;
} ecs_stack_t;

void flecs_stack_init(
    ecs_stack_t *stack);

void flecs_stack_fini(
    ecs_stack_t *stack);

void* flecs_stack_alloc(
    ecs_stack_t *stack, 
    ecs_size_t size,
    ecs_size_t align);

#define flecs_stack_alloc_t(stack, T)\
    flecs_stack_alloc(stack, ECS_SIZEOF(T), ECS_ALIGNOF(T))

#define flecs_stack_alloc_n(stack, T, count)\
    flecs_stack_alloc(stack, ECS_SIZEOF(T) * count, ECS_ALIGNOF(T))

void* flecs_stack_calloc(
    ecs_stack_t *stack, 
    ecs_size_t size,
    ecs_size_t align);

#define flecs_stack_calloc_t(stack, T)\
    flecs_stack_calloc(stack, ECS_SIZEOF(T), ECS_ALIGNOF(T))

#define flecs_stack_calloc_n(stack, T, count)\
    flecs_stack_calloc(stack, ECS_SIZEOF(T) * count, ECS_ALIGNOF(T))

void flecs_stack_free(
    void *ptr,
    ecs_size_t size);

#define flecs_stack_free_t(ptr, T)\
    flecs_stack_free(ptr, ECS_SIZEOF(T))

#define flecs_stack_free_n(ptr, T, count)\
    flecs_stack_free(ptr, ECS_SIZEOF(T) * count)

void flecs_stack_reset(
    ecs_stack_t *stack);

ecs_stack_cursor_t flecs_stack_get_cursor(
    ecs_stack_t *stack);

void flecs_stack_restore_cursor(
    ecs_stack_t *stack,
    const ecs_stack_cursor_t *cursor);

#endif
