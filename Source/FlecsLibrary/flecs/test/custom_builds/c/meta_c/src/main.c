#include <meta_c.h>

ECS_STRUCT(Position, {
    float x;
    float y;
});

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_META_COMPONENT(world, Position);

    return ecs_fini(world);
}
