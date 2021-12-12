#include <plecs.h>

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init();

    int ret = ecs_plecs_from_str(world, NULL, "Foo");
    assert(ret == 0);
    (void)ret;

    ecs_fini(world);
}
