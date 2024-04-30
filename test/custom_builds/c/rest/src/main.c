#include <rest.h>

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ecs_insert(world, ecs_value(EcsRest, {0}));

    return ecs_fini(world);
}
