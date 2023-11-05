#include <define_debug_and_sanitize.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);
    return ecs_fini(world);
}
