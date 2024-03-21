#include <int_time_type.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ecs_progress(world, 0);

    return ecs_fini(world);
}
