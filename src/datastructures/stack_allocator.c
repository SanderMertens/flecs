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

ecs_stack_cursor_t flecs_stack_get_cursor(
    ecs_stack_t *stack)
{
    // Capture current stack info
    ecs_stack_cursor_t result = { 
        .cur = stack->cur,
        .sp = stack->cur->sp 
        };

    ecs_stack_cursor_marker_t* marker = flecs_stack_calloc_t(stack, ecs_stack_cursor_marker_t);
    marker->restoreTo = result;
    if (stack->cur != result.cur)
    {
        // marker was allocated on new page
        result.cur = stack->cur;
        result.sp = (int16_t) (stack->cur->sp - sizeof(ecs_stack_cursor_marker_t));
        ecs_assert(result.sp >= 0, ECS_INTERNAL_ERROR, NULL);
    }
    marker->cursor = result;
    ++stack->cursorCount;
#ifdef FLECS_DEBUG
    marker->cursorId = ++stack->cursorLastId;
    marker->owner = stack;
    *ecs_vec_append_t(NULL, &stack->cursorIds, uint32_t) = marker->cursorId;
#endif

    marker->prev = stack->tailMarker;
    stack->tailMarker = marker;
    return result;
}

ecs_stack_cursor_marker_t* flecs_stack_get_cursor_marker(
    ecs_stack_t *stack, 
    const ecs_stack_cursor_t *cursor)
{
    ecs_assert(stack, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(cursor, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(cursor->cur, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cursor->cur->data, ECS_INTERNAL_ERROR, NULL);

    // (char*) cast is to prevent warning of pointer arithmetic on void*
    // (void*) cast is to prevent warning of larger alignment required
    ecs_stack_cursor_marker_t* marker = (ecs_stack_cursor_marker_t*)(void*)((char*)cursor->cur->data + cursor->sp);
#ifdef FLECS_DEBUG
    // validate this cursor belongs to this stack
    bool found = false;
    for (ecs_stack_page_t *page = &stack->first; !found && page; page = page->next)
    {
        found = (page == cursor->cur);
    }
    ecs_assert(found, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(marker->cursor.cur == cursor->cur, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(marker->cursor.sp == cursor->sp, ECS_INTERNAL_ERROR, NULL);
#endif
    return marker;
}

#ifdef FLECS_DEBUG
static bool flecs_remove_cursor_from_active_ids(
    ecs_stack_t *stack,
    const ecs_stack_cursor_t *cursor)
{
    ecs_stack_cursor_marker_t* marker = flecs_stack_get_cursor_marker(stack, cursor); 
    const uint32_t cursorId = marker->cursorId;
    const uint32_t* const vec = ecs_vec_first_t(&stack->cursorIds, uint32_t);
    const uint32_t* p = &vec[stack->cursorIds.count - 1];
    while (p >= vec)
    {
        if (*p == cursorId)
        {
            int32_t index = (int32_t)(p - vec);
            ecs_vec_remove_t(&stack->cursorIds, uint32_t, index);
            return true;
        }
        --p;
    }
    return false;
}
#endif
void flecs_stack_restore_cursor(
    ecs_stack_t *stack,
    const ecs_stack_cursor_t *cursor)
{
    if (!cursor->cur)
    {
        return;     // cursor not initialized
    }
    ecs_stack_cursor_marker_t* marker = flecs_stack_get_cursor_marker(stack, cursor); 
#ifdef FLECS_DEBUG
    ecs_assert(stack == marker->owner, ECS_INTERNAL_ERROR, NULL);
    bool found = flecs_remove_cursor_from_active_ids(stack, cursor);
    ecs_assert(found || marker->isFree, ECS_INTERNAL_ERROR, NULL); // Probably a double fini of an iterator
#endif
    if (marker->isFree)
    {
        return;     // already freed
    }

    ecs_assert(stack->cursorCount > 0, ECS_INTERNAL_ERROR, NULL); // count should not go negative
    --stack->cursorCount;
    marker->isFree = true;

    // Check if the cursor is the last one on the stack
    if (marker != stack->tailMarker)
    {
        // we're done here
        return;
    }

    // This is the tail cursor
    // Walk the prev list until we find one not yet freed
    while (marker)
    {
        ecs_stack_cursor_marker_t* prev = marker->prev;
        if (!prev || !prev->isFree)
        {
            break;      // marker is now the best marker to free
        }
        marker = prev;
    }
    stack->tailMarker = marker->prev;     // now pointing to tail-most cursor not yet freed
    stack->cur = marker->restoreTo.cur;
    stack->cur->sp = marker->restoreTo.sp;

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
    stack->tailMarker = NULL;
#ifdef FLECS_DEBUG
    ecs_vec_clear(&stack->cursorIds);
#endif
}

void flecs_stack_init(
    ecs_stack_t *stack)
{
    ecs_os_zeromem(stack);
    stack->cur = &stack->first;
    stack->first.data = NULL;
#ifdef FLECS_DEBUG
    ecs_vec_init_t(NULL, &stack->cursorIds, uint32_t, 2);
#endif
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
#ifdef FLECS_DEBUG
    ecs_vec_fini_t(NULL, &stack->cursorIds, uint32_t);
#endif
}
