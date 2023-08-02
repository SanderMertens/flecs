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

#define FLECS_STACK_PAGE_OFFSET ECS_ALIGN(ECS_SIZEOF(ecs_stack_page_t), 16)

int64_t ecs_stack_allocator_alloc_count = 0;
int64_t ecs_stack_allocator_free_count = 0;

static
ecs_stack_page_t* flecs_stack_page_new(uint32_t page_id) {
    ecs_stack_page_t *result = ecs_os_malloc(
        FLECS_STACK_PAGE_OFFSET + ECS_STACK_PAGE_SIZE);
    result->data = ECS_OFFSET(result, FLECS_STACK_PAGE_OFFSET);
    result->next = NULL;
    result->id = page_id + 1;
    ecs_os_linc(&ecs_stack_allocator_alloc_count);
    return result;
}

void* flecs_stack_alloc(
    ecs_stack_t *stack, 
    ecs_size_t size,
    ecs_size_t align)
{
    ecs_stack_page_t *page = stack->cur;
    if (page == &stack->first && !page->data) {
        page->data = ecs_os_malloc(ECS_STACK_PAGE_SIZE);
        ecs_os_linc(&ecs_stack_allocator_alloc_count);
    }

    int16_t sp = flecs_ito(int16_t, ECS_ALIGN(page->sp, align));
    int16_t next_sp = flecs_ito(int16_t, sp + size);
    void *result = NULL;

    if (next_sp > ECS_STACK_PAGE_SIZE) {
        if (size > ECS_STACK_PAGE_SIZE) {
            result = ecs_os_malloc(size); /* Too large for page */
            goto done;
        }

        if (page->next) {
            page = page->next;
        } else {
            page = page->next = flecs_stack_page_new(page->id);
        }
        sp = 0;
        next_sp = flecs_ito(int16_t, size);
        stack->cur = page;
    }

    page->sp = next_sp;
    result = ECS_OFFSET(page->data, sp);

done:
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
    if (size > ECS_STACK_PAGE_SIZE) {
        ecs_os_free(ptr);
    }
}

ecs_stack_cursor_t* flecs_stack_get_cursor(
    ecs_stack_t *stack)
{
    ecs_stack_page_t *cur = stack->cur;
    int16_t sp = stack->cur->sp;
    ecs_stack_cursor_t *result = flecs_stack_alloc_t(stack, ecs_stack_cursor_t);
    result->cur = cur;
    result->sp = sp;
    result->isFree = false;

#ifdef FLECS_DEBUG
    ++stack->cursorCount;
    result->owner = stack;
#endif

    result->prev = stack->tailCursor;
    stack->tailCursor = result;
    return result;
}

void flecs_stack_restore_cursor(
    ecs_stack_t *stack,
    ecs_stack_cursor_t *cursor)
{
    if (!cursor) {
        return;
    }

    ecs_assert(stack == cursor->owner, ECS_INVALID_OPERATION, NULL);
    ecs_assert(cursor->isFree == false, ECS_DOUBLE_FREE, NULL);
    ecs_assert(stack->cursorCount > 0, ECS_DOUBLE_FREE, NULL);
#ifdef FLECS_DEBUG    
    --stack->cursorCount;
#endif
    cursor->isFree = true;

    // Check if the cursor is the last one on the stack
    if (cursor != stack->tailCursor)
    {
        // we're done here
        return;
    }

    // This is the tail cursor
    // Walk the prev list until we find one not yet freed
    while (cursor)
    {
        ecs_stack_cursor_t* prev = cursor->prev;
        if (!prev || !prev->isFree)
        {
            break;      // marker is now the best marker to free
        }
        cursor = prev;
    }
    stack->tailCursor = cursor->prev; // now pointing to tail-most cursor not yet freed
    stack->cur = cursor->cur;
    stack->cur->sp = cursor->sp;

    // if the cursor count is zero, stack should be empty
    // if the cursor count is non-zero, stack should not be empty
    ecs_assert((stack->cursorCount == 0) == (stack->cur == &stack->first && stack->cur->sp == 0), ECS_LEAK_DETECTED, NULL);
}

void flecs_stack_reset(
    ecs_stack_t *stack)
{
    ecs_assert(stack->cursorCount == 0, ECS_LEAK_DETECTED, NULL);
    stack->cur = &stack->first;
    stack->first.sp = 0;
    stack->tailCursor = NULL;
}

void flecs_stack_init(
    ecs_stack_t *stack)
{
    ecs_os_zeromem(stack);
    stack->cur = &stack->first;
    stack->first.data = NULL;
}

void flecs_stack_fini(
    ecs_stack_t *stack)
{
    ecs_stack_page_t *next, *cur = &stack->first;
    ecs_assert(stack->cursorCount == 0, ECS_LEAK_DETECTED, NULL);
    ecs_assert(stack->cur == &stack->first, ECS_LEAK_DETECTED, NULL);
    ecs_assert(stack->cur->sp == 0, ECS_LEAK_DETECTED, NULL);
    do {
        next = cur->next;
        if (cur == &stack->first) {
            if (cur->data) {
                ecs_os_linc(&ecs_stack_allocator_free_count);
            }
            ecs_os_free(cur->data);
        } else {
            ecs_os_linc(&ecs_stack_allocator_free_count);
            ecs_os_free(cur);
        }
    } while ((cur = next));
}
