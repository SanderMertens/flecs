#include <core.h>

void Count_count_empty(void) {
    ecs_world_t *world = ecs_mini();

    test_int(ecs_count_id(world, 0), 0);

    ecs_fini(world);
}

void Count_count_w_entity_0(void) {
    ecs_world_t *world = ecs_mini();

    test_int(ecs_count_id(world, 0), 0);

    ecs_fini(world);
}

void Count_count_1_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add(world, e1, Position);
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_add(world, e2, Position);

    test_int(ecs_count(world, Position), 2);

    ecs_fini(world);
}

void Count_count_disabled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_new_w(world, Position);

    test_int(ecs_count(world, Position), 2);

    ecs_enable(world, e1, false);

    test_int(ecs_count(world, Position), 2);

    ecs_fini(world);
}

void Count_count_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_new_w(world, Position);

    test_int(ecs_count(world, Position), 2);

    ecs_add_id(world, e1, EcsPrefab);

    test_int(ecs_count(world, Position), 2);

    ecs_fini(world);
}

void Count_count_sparse_pair_rel_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_add_id(world, Rel, EcsSparse);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_add_pair(world, e1, Rel, TgtB);
    ecs_new_w_pair(world, Rel, TgtA);

    test_int(ecs_count_id(world, ecs_pair(Rel, EcsWildcard)), 3);

    ecs_fini(world);
}

void Count_count_dont_fragment_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_new_w(world, Position);
    ecs_new_w(world, Position);

    test_int(ecs_count(world, Position), 2);

    ecs_fini(world);
}

void Count_count_dont_fragment_pair_rel_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_add_id(world, Rel, EcsDontFragment);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_add_pair(world, e1, Rel, TgtB);
    ecs_new_w_pair(world, Rel, TgtA);

    test_int(ecs_count_id(world, ecs_pair(Rel, EcsWildcard)), 3);

    ecs_fini(world);
}
