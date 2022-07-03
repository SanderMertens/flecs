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

void flecs_stack_free(
    void *ptr,
    ecs_size_t size);

void flecs_stack_reset(
    ecs_stack_t *stack);

#endif
