#include <plecs_w_parser.h>

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    int ret = ecs_plecs_from_str(world, NULL, "Foo");
    assert(ret == 0);
    (void)ret;

    ecs_fini(world);
}
