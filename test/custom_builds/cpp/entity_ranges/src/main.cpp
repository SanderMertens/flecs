#include <entity_ranges.h>

int main(int, char *[]) {
    flecs::world ecs;

    const ecs_entity_range_t *range = ecs_entity_range_new(
        ecs, 10000, 20000);
    assert(range != NULL);

    ecs_entity_range_set(ecs, range);

    auto e = ecs.entity();
    assert(e.id() >= 10000);
    assert(e.id() <= 20000);

    return 0;
}
