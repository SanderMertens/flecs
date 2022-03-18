#include <no_log.h>

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    assert(ecs_should_log(-1) == false);
    assert(ecs_should_log(0) == false);
    assert(ecs_should_log(1) == false);
    assert(ecs_should_log(2) == false);

    return ecs_fini(world);
}
