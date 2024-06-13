#include <monitor_rest.h>

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_IMPORT(world, FlecsStats);
    ECS_IMPORT(world, FlecsRest);

    return ecs_fini(world);
}
