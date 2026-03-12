/**
 * @file datastructures/stack_allocator.h
 * @brief Stack allocator.
 */

#ifndef FLECS_STACK_ALLOCATOR_H
#define FLECS_STACK_ALLOCATOR_H

/** A page of memory in the stack allocator. */
typedef struct ecs_stack_page_t {
    void *data; /**< Pointer to the page data. */
    struct ecs_stack_page_t *next; /**< Next page in the list. */
    int16_t sp; /**< Current stack pointer within the page. */
    uint32_t id; /**< Page identifier. */
} ecs_stack_page_t;

/** Cursor that marks a position in the stack allocator for later restoration. */
typedef struct ecs_stack_cursor_t {
    struct ecs_stack_cursor_t *prev; /**< Previous cursor in the stack. */
    struct ecs_stack_page_t *page; /**< Page at the cursor position. */
    int16_t sp; /**< Stack pointer at the cursor position. */
    bool is_free; /**< Whether this cursor has been freed. */
#ifdef FLECS_DEBUG
    struct ecs_stack_t *owner; /**< Stack allocator that owns this cursor (debug only). */
#endif
} ecs_stack_cursor_t;

/** Stack allocator for quick allocation of small temporary values. */
typedef struct ecs_stack_t {
    ecs_stack_page_t *first; /**< First page in the stack. */
    ecs_stack_page_t *tail_page; /**< Current tail page. */
    ecs_stack_cursor_t *tail_cursor; /**< Current tail cursor. */
#ifdef FLECS_DEBUG
    int32_t cursor_count; /**< Number of active cursors (debug only). */
#endif
} ecs_stack_t;

/** Offset of usable data within a stack page (aligned to 16 bytes). */
#define FLECS_STACK_PAGE_OFFSET ECS_ALIGN(ECS_SIZEOF(ecs_stack_page_t), 16)

/** Size of usable data within a stack page. */
#define FLECS_STACK_PAGE_SIZE (1024 - FLECS_STACK_PAGE_OFFSET)

/** Initialize a stack allocator.
 *
 * @param stack The stack allocator to initialize.
 */
FLECS_DBG_API
void flecs_stack_init(
    ecs_stack_t *stack);

/** Deinitialize a stack allocator.
 *
 * @param stack The stack allocator to deinitialize.
 */
FLECS_DBG_API
void flecs_stack_fini(
    ecs_stack_t *stack);

/** Allocate memory from the stack.
 *
 * @param stack The stack allocator.
 * @param size The allocation size.
 * @param align The required alignment.
 * @return Pointer to the allocated memory.
 */
FLECS_DBG_API
void* flecs_stack_alloc(
    ecs_stack_t *stack,
    ecs_size_t size,
    ecs_size_t align);

/** Allocate memory for a single element of type T from the stack. */
#define flecs_stack_alloc_t(stack, T)\
    flecs_stack_alloc(stack, ECS_SIZEOF(T), ECS_ALIGNOF(T))

/** Allocate memory for count elements of type T from the stack. */
#define flecs_stack_alloc_n(stack, T, count)\
    flecs_stack_alloc(stack, ECS_SIZEOF(T) * count, ECS_ALIGNOF(T))

/** Allocate zeroed memory from the stack.
 *
 * @param stack The stack allocator.
 * @param size The allocation size.
 * @param align The required alignment.
 * @return Pointer to the zeroed memory.
 */
FLECS_DBG_API
void* flecs_stack_calloc(
    ecs_stack_t *stack,
    ecs_size_t size,
    ecs_size_t align);

/** Allocate zeroed memory for a single element of type T from the stack. */
#define flecs_stack_calloc_t(stack, T)\
    flecs_stack_calloc(stack, ECS_SIZEOF(T), ECS_ALIGNOF(T))

/** Allocate zeroed memory for count elements of type T from the stack. */
#define flecs_stack_calloc_n(stack, T, count)\
    flecs_stack_calloc(stack, ECS_SIZEOF(T) * count, ECS_ALIGNOF(T))

/** Free memory allocated from the stack.
 *
 * @param ptr The pointer to free.
 * @param size The size of the allocation.
 */
FLECS_DBG_API
void flecs_stack_free(
    void *ptr,
    ecs_size_t size);

/** Free memory for a single element of type T from the stack. */
#define flecs_stack_free_t(ptr, T)\
    flecs_stack_free(ptr, ECS_SIZEOF(T))

/** Free memory for count elements of type T from the stack. */
#define flecs_stack_free_n(ptr, T, count)\
    flecs_stack_free(ptr, ECS_SIZEOF(T) * count)

/** Reset the stack allocator.
 *
 * @param stack The stack allocator to reset.
 */
void flecs_stack_reset(
    ecs_stack_t *stack);

/** Get a cursor marking the current position in the stack.
 *
 * @param stack The stack allocator.
 * @return A cursor that can be used to restore the stack.
 */
FLECS_DBG_API
ecs_stack_cursor_t* flecs_stack_get_cursor(
    ecs_stack_t *stack);

/** Restore the stack to a previously saved cursor position.
 *
 * @param stack The stack allocator.
 * @param cursor The cursor to restore to.
 */
FLECS_DBG_API
void flecs_stack_restore_cursor(
    ecs_stack_t *stack,
    ecs_stack_cursor_t *cursor);

#endif
