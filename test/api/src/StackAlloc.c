// #include <api.h>
// #include "../../../src/private_api.h"

// void StackAlloc_init_fini() {
//     ecs_world_t *world = ecs_mini();

//     ecs_stack_t stack = {0};
//     flecs_stack_init(&stack);

//     test_int(stack.cur->sp, 0);

//     flecs_stack_fini(&stack);
//     ecs_fini(world);
// }

// /// @brief Utililty for creating a mock allocation on the stack allocator
// /// @param stack Stack to use
// /// @param allocSizes Array of sizes to alloc. All of alignment 1. Ends with a size of 0.
// /// @return cursor to start of allocations
// static ecs_stack_cursor_t init_some_object(ecs_stack_t* stack, int32_t* allocSizes) {
//     ecs_stack_cursor_t cursor = flecs_stack_get_cursor(stack);
//     while (*allocSizes > 0)
//     {
//         flecs_stack_calloc(stack, *allocSizes++, 1);
//     }
//     return cursor;
// }

// static bool compare_cursors_equal(ecs_stack_cursor_t c1, ecs_stack_cursor_t c2) {
//     return (c1.cur == c2.cur) && (c1.sp == c2.sp);
// }

// void StackAlloc_round_trip_cursor_marker() {
//     ecs_world_t *world = ecs_mini();
//     ecs_stack_t stack = {0};
//     flecs_stack_init(&stack);

//     ecs_stack_cursor_t cursor1 = init_some_object(&stack, (int32_t[]){64, 64, 0});
//     test_true(compare_cursors_equal(cursor1, stack.tailMarker->cursor));

//     ecs_stack_cursor_t cursor2 = init_some_object(&stack, (int32_t[]){64, 64, 0});
//     test_true(compare_cursors_equal(cursor2, stack.tailMarker->cursor));
//     ecs_stack_cursor_marker_t* marker2 = stack.tailMarker;

//     int64_t sizeToAlmostFillPage = ECS_STACK_PAGE_SIZE - stack.cur->sp - 
//         (3 * ECS_SIZEOF(ecs_stack_cursor_marker_t) / 2);
//     ecs_stack_cursor_t cursor3 = init_some_object(&stack, (int32_t[]){sizeToAlmostFillPage, 0});
//     test_true(compare_cursors_equal(cursor3, stack.tailMarker->cursor));

//     // cursor 4 marker allocates on new page, restores to current page
//     int16_t spBefore4 = stack.cur->sp;
//     ecs_stack_cursor_t cursor4 = init_some_object(&stack, (int32_t[]){64, 64, 0});
//     test_true(compare_cursors_equal(cursor4, stack.tailMarker->cursor));
//     test_true(cursor1.cur == cursor2.cur);
//     test_true(cursor2.cur == cursor3.cur);
//     test_true(cursor3.cur != cursor4.cur); // should have been allocated on new page
//     ecs_stack_cursor_marker_t* marker4 = stack.tailMarker;
//     test_true(marker4->restoreTo.cur == cursor3.cur);   // will restore back to prior page
//     test_int(marker4->restoreTo.sp, spBefore4);

//     test_true(marker2 == flecs_stack_cursor_to_marker(&stack, &cursor2));    // on prior page
//     test_true(marker4 == flecs_stack_cursor_to_marker(&stack, &cursor4));    // on current page

//     flecs_stack_restore_cursor(&stack, &cursor1);
//     flecs_stack_restore_cursor(&stack, &cursor2);
//     flecs_stack_restore_cursor(&stack, &cursor3);
//     flecs_stack_restore_cursor(&stack, &cursor4);

//     flecs_stack_fini(&stack);
//     ecs_fini(world);
// }

// void StackAlloc_nested_cursors_freed_in_order() {
//     ecs_world_t *world = ecs_mini();
//     ecs_stack_t stack = {0};
//     flecs_stack_init(&stack);

//     int32_t sizeofCursorMarker = ECS_SIZEOF(ecs_stack_cursor_marker_t);
//     int32_t alignOfCursorMarker = (int32_t) ECS_ALIGNOF(ecs_stack_cursor_marker_t);
//     int32_t sizeofObj1 = sizeofCursorMarker + 16 + 12 + 3;
//     int32_t expectedSpAfterCursor1 = sizeofObj1;
//     int32_t expectedSpCursor2 = ECS_ALIGN(sizeofObj1, alignOfCursorMarker);
//     int32_t expectedSpAfterCursor2 = expectedSpCursor2 + sizeofCursorMarker + 4 + 8 + 12;

//     ecs_stack_cursor_t cursor1 = init_some_object(&stack, (int32_t[]){16, 12, 3, 0});   // odd number of bytes, force adjustment for next cursor
//     test_int(cursor1.sp, 0);
//     test_int(stack.cur->sp, expectedSpAfterCursor1);

//     ecs_stack_cursor_t cursor2 = init_some_object(&stack, (int32_t[]){4, 8, 12, 0});
//     test_int(cursor2.sp, expectedSpCursor2);  // check for cursor alignment
//     test_int(stack.cur->sp, expectedSpAfterCursor2);

//     flecs_stack_restore_cursor(&stack, &cursor2);
//     test_int(stack.cur->sp, expectedSpAfterCursor1);

//     flecs_stack_restore_cursor(&stack, &cursor1);
//     test_int(stack.cur->sp, 0);

//     flecs_stack_fini(&stack);
//     ecs_fini(world);
// }

// void StackAlloc_overlapping_cursors() {
//     ecs_world_t *world = ecs_mini();
//     ecs_stack_t stack = {0};
//     flecs_stack_init(&stack);

//     int32_t sizeofCursorMarker = ECS_SIZEOF(ecs_stack_cursor_marker_t);
//     int32_t alignOfCursorMarker = (int32_t) ECS_ALIGNOF(ecs_stack_cursor_marker_t);
//     int32_t sizeofObj1 = sizeofCursorMarker + 21 + 43;
//     int32_t expectedSpAfterCursor1 = sizeofObj1;
//     int32_t expectedSpCursor2 = ECS_ALIGN(sizeofObj1, alignOfCursorMarker);
//     int32_t expectedSpAfterCursor2 = expectedSpCursor2 + sizeofCursorMarker + 16 + 16 + 24;

//     ecs_stack_cursor_t cursor1 = init_some_object(&stack, (int32_t[]){21, 43, 0});   // two odd allocations that yield 64 bytes
//     test_int(cursor1.sp, 0);
//     test_int(stack.cur->sp, expectedSpAfterCursor1);

//     ecs_stack_cursor_t cursor2 = init_some_object(&stack, (int32_t[]){16, 16, 24, 0});
//     test_int(cursor2.sp, expectedSpCursor2);  // check for cursor alignment
//     test_int(stack.cur->sp, expectedSpAfterCursor2);

//     flecs_stack_restore_cursor(&stack, &cursor1);
//     test_int(stack.cur->sp, expectedSpAfterCursor2);
//     ecs_stack_cursor_marker_t* marker1 = flecs_stack_cursor_to_marker(&stack, &cursor1);
//     test_true(marker1->isFree);

//     flecs_stack_restore_cursor(&stack, &cursor2);
//     test_int(stack.cur->sp, 0);

//     flecs_stack_fini(&stack);
//     ecs_fini(world);
// }

// void StackAlloc_multiple_overlapping_cursors() {
//     ecs_world_t *world = ecs_mini();
//     ecs_stack_t stack = {0};
//     flecs_stack_init(&stack);

//     ecs_stack_cursor_t cursor1 = init_some_object(&stack, (int32_t[]){64, 64, 0});
//     int16_t spAfter1 = stack.cur->sp;
//     ecs_stack_cursor_t cursor2 = init_some_object(&stack, (int32_t[]){64, 64, 0});
//     ecs_stack_cursor_t cursor3 = init_some_object(&stack, (int32_t[]){64, 64, 0});
//     int16_t spAfter3 = stack.cur->sp;
//     ecs_stack_cursor_t cursor4 = init_some_object(&stack, (int32_t[]){64, 64, 0});
//     int16_t spAfter4 = stack.cur->sp;

//     // [] == in use, {} == free
//     // [1] [2] [3] [4]
//     flecs_stack_restore_cursor(&stack, &cursor2);
//     // [1] {2} [3] [4]
//     test_int(stack.cur->sp, spAfter4);

//     flecs_stack_restore_cursor(&stack, &cursor4);
//     // [1] {2} [3]
//     test_int(stack.cur->sp, spAfter3);

//     flecs_stack_restore_cursor(&stack, &cursor3);
//     // [1]
//     test_int(stack.cur->sp, spAfter1);

//     flecs_stack_restore_cursor(&stack, &cursor1);
//     // Empty
//     test_int(stack.cur->sp, 0);

//     flecs_stack_fini(&stack);
//     ecs_fini(world);

// }

void StackAlloc_init_fini() {
    // Implement testcase
}

void StackAlloc_round_trip_cursor_marker() {
    // Implement testcase
}

void StackAlloc_nested_cursors_freed_in_order() {
    // Implement testcase
}

void StackAlloc_overlapping_cursors() {
    // Implement testcase
}

void StackAlloc_multiple_overlapping_cursors() {
    // Implement testcase
}
