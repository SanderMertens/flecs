#include <log_level_3.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    return ecs_fini(world);
}
