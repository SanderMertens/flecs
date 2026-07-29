#include <prefab.h>

typedef struct {
    int32_t x, y;
} Position;

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_set(world, p, Position, {10, 20});

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    const Position *pos = ecs_get(world, i, Position);
    assert(pos != NULL);
    assert(pos->x == 10);
    assert(pos->y == 20);

    return ecs_fini(world);
}
