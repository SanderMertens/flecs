/**
 * @file datastructures/stack_allocator.c
 * @brief Stack allocator.
 * 
 * The stack allocator enables pushing and popping values to a stack, and has
 * a lower overhead when compared to block allocators. A stack allocator is a
 * good fit for small temporary allocations.
 * 
 * The stack allocator allocates memory in pages. If the requested size of an
 * allocation exceeds the page size, a regular allocator is used instead.
 */

#include "../private_api.h"

int64_t ecs_stack_allocator_alloc_count = 0;
int64_t ecs_stack_allocator_free_count = 0;

static ecs_stack_page_t* flecs_stack_page_new(uint32_t page_id) {
    ecs_stack_page_t *result = ecs_os_malloc(
        FLECS_STACK_PAGE_OFFSET + FLECS_STACK_PAGE_SIZE);
    result->data = ECS_OFFSET(result, FLECS_STACK_PAGE_OFFSET);
    result->next = NULL;
    result->id = page_id + 1;
    result->sp = 0;
    ecs_os_linc(&ecs_stack_allocator_alloc_count);
    return result;
}

#define FLECS_STACK_OVERSIZED_MSG \
    "an allocation that exceeds FLECS_STACK_PAGE_SIZE was not freed: callers " \
    "must pair flecs_stack_alloc with flecs_stack_free"

#ifdef FLECS_DEBUG
/* Allocations that are too large for a page are prefixed with the stack that
 * allocated them, so that flecs_stack_free can account for them and the
 * allocator can assert that callers honor the pairing contract. */
typedef struct ecs_stack_oversized_t {
    ecs_stack_t *owner;
} ecs_stack_oversized_t;

#define FLECS_STACK_OVERSIZED_OFFSET \
    ECS_ALIGN(ECS_SIZEOF(ecs_stack_oversized_t), 16)
#endif

void* flecs_stack_alloc(
    ecs_stack_t *stack, 
    ecs_size_t size,
    ecs_size_t align)
{
    ecs_assert(size > 0, ECS_INTERNAL_ERROR, NULL);
    void *result = NULL;

    if (size > FLECS_STACK_PAGE_SIZE) {
        /* Too large for page */
#ifdef FLECS_DEBUG
        ecs_stack_oversized_t *hdr = ecs_os_malloc(
            FLECS_STACK_OVERSIZED_OFFSET + size);
        hdr->owner = stack;
        ++ stack->oversized_count;
        result = ECS_OFFSET(hdr, FLECS_STACK_OVERSIZED_OFFSET);
#else
        result = ecs_os_malloc(size);
#endif
        goto done;
    }

    ecs_stack_page_t *page = stack->tail_page;
    if (!page) {
        page = stack->first = flecs_stack_page_new(0);
        stack->tail_page = page;
    }

    ecs_assert(page->data != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_assert(align != 0, ECS_INTERNAL_ERROR, NULL);
    int16_t sp = flecs_ito(int16_t, ECS_ALIGN(page->sp, align));
    int16_t next_sp = flecs_ito(int16_t, sp + size);

    if (next_sp > FLECS_STACK_PAGE_SIZE) {
        if (page->next) {
            page = page->next;
        } else {
            page = page->next = flecs_stack_page_new(page->id);
        }
        sp = 0;
        next_sp = flecs_ito(int16_t, size);
        stack->tail_page = page;
    }

    page->sp = next_sp;
    result = ECS_OFFSET(page->data, sp);

done:
    ecs_assert(result != NULL, ECS_INTERNAL_ERROR, NULL);
#ifdef FLECS_SANITIZE
    ecs_os_memset(result, 0xAA, size);
#endif
    return result;
}

void* flecs_stack_calloc(
    ecs_stack_t *stack, 
    ecs_size_t size,
    ecs_size_t align)
{
    void *ptr = flecs_stack_alloc(stack, size, align);
    ecs_os_memset(ptr, 0, size);
    return ptr;
}

void flecs_stack_free(
    void *ptr,
    ecs_size_t size)
{
    if (size > FLECS_STACK_PAGE_SIZE) {
#ifdef FLECS_DEBUG
        ecs_stack_oversized_t *hdr = ECS_OFFSET(
            ptr, -FLECS_STACK_OVERSIZED_OFFSET);
        ecs_dbg_assert(hdr->owner->oversized_count > 0, ECS_DOUBLE_FREE,
            "double free detected in stack allocator");
        -- hdr->owner->oversized_count;
        ecs_os_free(hdr);
#else
        ecs_os_free(ptr);
#endif
    }
}

ecs_stack_cursor_t* flecs_stack_get_cursor(
    ecs_stack_t *stack)
{
    ecs_assert(stack != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_stack_page_t *page = stack->tail_page;
    if (!page) {
        page = stack->first = flecs_stack_page_new(0);
        stack->tail_page = page;
    }

    int16_t sp = stack->tail_page->sp;
    ecs_stack_cursor_t *result = flecs_stack_alloc_t(stack, ecs_stack_cursor_t);
    result->page = page;
    result->sp = sp;
    result->is_free = false;

#ifdef FLECS_DEBUG
    ++ stack->cursor_count;
    result->owner = stack;
#endif

    result->prev = stack->tail_cursor;
    stack->tail_cursor = result;
    return result;
}

#define FLECS_STACK_LEAK_MSG \
    "a stack allocator leak is most likely due to an unterminated " \
    "iteration: call ecs_iter_fini to fix"

void flecs_stack_restore_cursor(
    ecs_stack_t *stack,
    ecs_stack_cursor_t *cursor)
{
    if (!cursor) {
        return;
    }

    ecs_dbg_assert(stack == cursor->owner, ECS_INVALID_OPERATION, 
        "attempting to restore a cursor for the wrong stack");
    ecs_dbg_assert(stack->cursor_count > 0, ECS_DOUBLE_FREE, 
        "double free detected in stack allocator");
    ecs_assert(cursor->is_free == false, ECS_DOUBLE_FREE,
        "double free detected in stack allocator");

    cursor->is_free = true;

#ifdef FLECS_DEBUG    
    -- stack->cursor_count;
#endif

    /* If cursor is not the last on the stack, no memory should be freed */
    if (cursor != stack->tail_cursor) {
        return;
    }

    /* Iterate freed cursors to know how much memory we can free */
    do {
        ecs_stack_cursor_t* prev = cursor->prev;
        if (!prev || !prev->is_free) {
            break; /* Found active cursor, free up until this point */
        }
        cursor = prev;
    } while (cursor);

    stack->tail_cursor = cursor->prev;
    stack->tail_page = cursor->page;
    stack->tail_page->sp = cursor->sp;

    /* If the cursor count is zero, the stack should be empty.
     * If the cursor count is non-zero, the stack should not be empty. */
    ecs_dbg_assert((stack->cursor_count == 0) == 
        (stack->tail_page == stack->first && stack->tail_page->sp == 0), 
            ECS_LEAK_DETECTED, FLECS_STACK_LEAK_MSG);

#ifdef FLECS_DEBUG
    if (!stack->cursor_count) {
        ecs_dbg_assert(stack->oversized_count == 0, ECS_LEAK_DETECTED,
            FLECS_STACK_OVERSIZED_MSG);
    }
#endif
}

void flecs_stack_reset(
    ecs_stack_t *stack)
{
    ecs_dbg_assert(stack->cursor_count == 0, ECS_LEAK_DETECTED, 
        FLECS_STACK_LEAK_MSG);
    ecs_dbg_assert(stack->oversized_count == 0, ECS_LEAK_DETECTED,
        FLECS_STACK_OVERSIZED_MSG);
    stack->tail_page = stack->first;
    if (stack->first) {
        stack->first->sp = 0;
    }
    stack->tail_cursor = NULL;
}

void flecs_stack_init(
    ecs_stack_t *stack)
{
    ecs_os_zeromem(stack);
    stack->first = NULL;
    stack->tail_page = NULL;
}

void flecs_stack_fini(
    ecs_stack_t *stack)
{
    ecs_stack_page_t *next, *cur = stack->first;
    ecs_dbg_assert(stack->cursor_count == 0, ECS_LEAK_DETECTED, 
        FLECS_STACK_LEAK_MSG);
    ecs_assert(stack->tail_page == stack->first, ECS_LEAK_DETECTED, 
        FLECS_STACK_LEAK_MSG);
    ecs_assert(!stack->tail_page || stack->tail_page->sp == 0, ECS_LEAK_DETECTED, 
        FLECS_STACK_LEAK_MSG);
    ecs_dbg_assert(stack->oversized_count == 0, ECS_LEAK_DETECTED,
        FLECS_STACK_OVERSIZED_MSG);

    if (cur) {
        do {
            next = cur->next;
            ecs_os_linc(&ecs_stack_allocator_free_count);
            ecs_os_free(cur);
        } while ((cur = next));
    }
}
