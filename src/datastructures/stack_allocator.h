/**
 * @file stack_allocator.h
 * @brief Data structure used for temporary small allocations.
 */

#ifndef FLECS_STACK_ALLOCATOR_H
#define FLECS_STACK_ALLOCATOR_H

void flecs_stack_init(
    ecs_stack_t *stack);

void flecs_stack_fini(
    ecs_stack_t *stack);

void* flecs_stack_alloc(
    ecs_stack_t *stack, 
    ecs_size_t size,
    ecs_size_t align);

void* flecs_stack_allocn(
    ecs_stack_t *stack, 
    ecs_size_t size,
    ecs_size_t align,
    int32_t count);

void* flecs_stack_callocn(
    ecs_stack_t *stack, 
    ecs_size_t size,
    ecs_size_t align,
    int32_t count);

void flecs_stack_free(
    void *ptr,
    ecs_size_t size);

void flecs_stack_reset(
    ecs_stack_t *stack);

ecs_stack_cursor_t flecs_stack_get_cursor(
    ecs_stack_t *stack);

void flecs_stack_restore_cursor(
    ecs_stack_t *stack,
    const ecs_stack_cursor_t *cursor);

#endif
