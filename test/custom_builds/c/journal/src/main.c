#include <journal.h>

typedef struct {
    int32_t x, y;
} Position;

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});
    ecs_remove(world, e, Position);
    ecs_delete(world, e);

    return ecs_fini(world);
}
