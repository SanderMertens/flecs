#include <api.h>

void Lookup_lookup() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, MyEntity, 0);

    ecs_entity_t lookup = ecs_lookup(world, "MyEntity");
    test_assert(lookup != 0);
    test_assert(lookup == MyEntity);

    ecs_fini(world);
}

void Lookup_lookup_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t lookup = ecs_lookup(world, "Position");
    test_assert(lookup != 0);
    test_assert(lookup == ecs_entity(Position));

    ecs_fini(world);
}

void Lookup_lookup_not_found() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t lookup = ecs_lookup(world, "foo");
    test_assert(lookup == 0);

    ecs_fini(world);
}

void Lookup_lookup_child() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent1, 0);
    ECS_ENTITY(world, Parent2, 0);

    ecs_entity_t e1 = ecs_set(world, 0, EcsId, {"Child"});
    ecs_entity_t e2 = ecs_set(world, 0, EcsId, {"Child"});

    ecs_adopt(world, e1, Parent1);
    ecs_adopt(world, e2, Parent2);

    ecs_entity_t lookup = ecs_lookup_child(world, Parent1, "Child");
    test_assert(lookup != 0);
    test_assert(lookup == e1);

    lookup = ecs_lookup_child(world, Parent2, "Child");
    test_assert(lookup != 0);
    test_assert(lookup == e2);

    ecs_fini(world);
}

void Lookup_lookup_child_w_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_set(world, 0, EcsId, {"Child"});
    ecs_entity_t e2 = ecs_set(world, 0, EcsId, {"Child"});

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Velocity);

    ecs_entity_t lookup = ecs_lookup_child(world, ecs_entity(Position), "Child");
    test_assert(lookup != 0);
    test_assert(lookup == e1);

    lookup = ecs_lookup_child(world, ecs_entity(Velocity), "Child");
    test_assert(lookup != 0);
    test_assert(lookup == e2);

    ecs_fini(world);
}
