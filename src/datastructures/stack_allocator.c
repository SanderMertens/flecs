#include "../private_api.h"

#define FLECS_STACK_PAGE_OFFSET ECS_ALIGN(ECS_SIZEOF(ecs_stack_page_t), 16)

static
ecs_stack_page_t* flecs_stack_page_new(void) {
    ecs_stack_page_t *result = ecs_os_malloc(
        FLECS_STACK_PAGE_OFFSET + ECS_STACK_PAGE_SIZE);
    result->data = ECS_OFFSET(result, FLECS_STACK_PAGE_OFFSET);
    result->next = NULL;
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
    }

    ecs_size_t sp = ECS_ALIGN(page->sp, align);
    ecs_size_t next_sp = sp + size;

    if (next_sp > ECS_STACK_PAGE_SIZE) {
        if (size > ECS_STACK_PAGE_SIZE) {
            return ecs_os_malloc(size); /* Too large for page */
        }

        if (page->next) {
            page = page->next;
        } else {
            page = page->next = flecs_stack_page_new();
        }
        sp = 0;
        next_sp = size;
        stack->cur = page;
    }

    page->sp = next_sp;

    return ECS_OFFSET(page->data, sp);
}

void* flecs_stack_allocn(
    ecs_stack_t *stack, 
    ecs_size_t size,
    ecs_size_t align,
    int32_t count)
{
    return flecs_stack_alloc(stack, size * count, align);
}

void* flecs_stack_callocn(
    ecs_stack_t *stack, 
    ecs_size_t size,
    ecs_size_t align,
    int32_t count)
{
    void *ptr = flecs_stack_alloc(stack, size * count, align);
    ecs_os_memset(ptr, 0, size * count);
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
    return (ecs_stack_cursor_t){
        .cur = stack->cur, .sp = stack->cur->sp
    };
}

void flecs_stack_restore_cursor(
    ecs_stack_t *stack,
    const ecs_stack_cursor_t *cursor)
{
    stack->cur = cursor->cur;
    stack->cur->sp = cursor->sp;
}

void flecs_stack_reset(
    ecs_stack_t *stack)
{
    stack->cur = &stack->first;
    stack->first.sp = 0;
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
    do {
        next = cur->next;
        if (cur == &stack->first) {
            ecs_os_free(cur->data);
        } else {
            ecs_os_free(cur);
        }
    } while ((cur = next));
}
