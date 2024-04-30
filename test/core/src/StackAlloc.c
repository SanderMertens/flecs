#include <core.h>
#include "../../../src/private_api.h"

void StackAlloc_init_fini(void) {
    ecs_world_t *world = ecs_mini();

    ecs_stack_t stack = {0};
    flecs_stack_init(&stack);

    test_int(stack.tail_page->sp, 0);
    test_assert(stack.tail_page == &stack.first);

    flecs_stack_fini(&stack);
    ecs_fini(world);
}

/// @brief Utililty for creating a mock allocation on the stack allocator
/// @param stack Stack to use
/// @param allocSizes Array of sizes to alloc. All of alignment 1. Ends with a size of 0.
/// @return cursor to start of allocations
static 
ecs_stack_cursor_t* init_some_object(ecs_stack_t* stack, int32_t* allocSizes) {
    ecs_stack_cursor_t *cursor = flecs_stack_get_cursor(stack);
    while (*allocSizes > 0)
    {
        flecs_stack_calloc(stack, *allocSizes++, 1);
    }
    return cursor;
}

void StackAlloc_multiple_overlapping_cursors(void) {
    ecs_world_t *world = ecs_mini();
    ecs_stack_t stack = {0};
    flecs_stack_init(&stack);

    ecs_stack_cursor_t *cursor1 = init_some_object(&stack, (int32_t[]){64, 64, 0});
    int16_t spAfter1 = stack.tail_page->sp;
    ecs_stack_cursor_t *cursor2 = init_some_object(&stack, (int32_t[]){64, 64, 0});
    ecs_stack_cursor_t *cursor3 = init_some_object(&stack, (int32_t[]){64, 64, 0});
    int16_t spAfter3 = stack.tail_page->sp;
    ecs_stack_cursor_t *cursor4 = init_some_object(&stack, (int32_t[]){64, 64, 0});
    int16_t spAfter4 = stack.tail_page->sp;

    // [] == in use, {} == free
    // [1] [2] [3] [4]
    flecs_stack_restore_cursor(&stack, cursor2);
    // [1] {2} [3] [4]
    test_int(stack.tail_page->sp, spAfter4);

    flecs_stack_restore_cursor(&stack, cursor4);
    // [1] {2} [3]
    test_int(stack.tail_page->sp, spAfter3);

    flecs_stack_restore_cursor(&stack, cursor3);
    // [1]
    test_int(stack.tail_page->sp, spAfter1);

    flecs_stack_restore_cursor(&stack, cursor1);
    // Empty
    test_int(stack.tail_page->sp, 0);

    flecs_stack_fini(&stack);
    ecs_fini(world);

}
