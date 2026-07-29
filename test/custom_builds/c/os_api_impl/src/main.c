#include <os_api_impl.h>

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    assert(ecs_os_has_heap());
    assert(ecs_os_has_time());
    assert(ecs_os_has_threading());

    return ecs_fini(world);
}
