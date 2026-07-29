#include <os_api_impl.h>

int main(int, char *[]) {
    flecs::world ecs;

    assert(ecs_os_has_heap());
    assert(ecs_os_has_time());
    assert(ecs_os_has_threading());

    return 0;
}
