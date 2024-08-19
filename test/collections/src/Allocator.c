#include <collections.h>

void Allocator_setup(void) {
    ecs_os_set_api_defaults();
}

void Allocator_init_fini_empty(void) {
    ecs_allocator_t a;
    flecs_allocator_init(&a);
    flecs_allocator_fini(&a);
    test_assert(true); // make sure there are no leaks, crashses
}
