#include <entity_ranges.h>

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    const ecs_entity_range_t *range = ecs_entity_range_new(
        world, 10000, 20000);
    assert(range != NULL);

    ecs_entity_range_set(world, range);
    assert(ecs_entity_range_get(world) == range);

    ecs_entity_t e = ecs_new(world);
    assert((uint32_t)e >= 10000);
    assert((uint32_t)e <= 20000);

    return ecs_fini(world);
}
