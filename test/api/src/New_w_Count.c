#include <api.h>

void New_w_Count_empty() {
    ecs_world_t *world = ecs_init();

    const ecs_entity_t *ids = ecs_bulk_new(world, 0, 1000);
    test_assert(ids != NULL);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(ids[i] != 0);
    }

    ecs_fini(world);
}

void New_w_Count_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    const ecs_entity_t *ids = ecs_bulk_new(world, Position, 1000);
    test_assert(ids != NULL);
    
    int i;
    for (i = 0; i < 1000; i ++) {
        ecs_entity_t e = ids[i];
        test_assert(e != 0);
        test_assert(ecs_has(world, e, Position));
    }

    test_int(ecs_count(world, Position), 1000);

    ecs_fini(world);
}

void New_w_Count_tag() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Tag, 0);

    const ecs_entity_t *ids = ecs_bulk_new_w_id(world, Tag, 1000);
    test_assert(ids != NULL);
    test_int(ecs_count_id(world, Tag), 1000);

    int i;
    for (i = 0; i < 1000; i ++) {
        ecs_entity_t e = ids[i];
        test_assert(e != 0);
        test_assert(ecs_has_id(world, e, Tag));
    }

    ecs_fini(world);
}
