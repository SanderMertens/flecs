#include <multi_world.h>

typedef struct {
    int32_t x, y;
} Position;

static void test_world(int32_t x, int32_t y) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {x, y});

    const Position *p = ecs_get(world, e, Position);
    assert(p != NULL);
    assert(p->x == x);
    assert(p->y == y);

    ecs_fini(world);
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    test_world(1, 2);
    test_world(3, 4);

    return 0;
}
