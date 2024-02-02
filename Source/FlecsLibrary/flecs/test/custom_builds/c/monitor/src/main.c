#include <monitor.h>

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_IMPORT(world, FlecsMonitor);

    return ecs_fini(world);
}
