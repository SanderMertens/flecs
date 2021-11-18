#include <rest.h>

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ecs_set(world, 0, EcsRest, {0});

    return ecs_fini(world);
}
