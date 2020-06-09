#include <api.h>

void Lookup_lookup() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, MyEntity, 0);

    ecs_entity_t lookup = ecs_lookup(world, "MyEntity");
    test_assert(lookup != 0);
    test_assert(lookup == MyEntity);

    ecs_fini(world);
}

void Lookup_lookup_w_null_name() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, MyEntity, 0);

    /* Ensure this doesn't crash the lookup function */
    ecs_set(world, 0, EcsName, {NULL});

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

    ecs_entity_t e1 = ecs_set(world, 0, EcsName, {"Child"});
    ecs_entity_t e2 = ecs_set(world, 0, EcsName, {"Child"});

    ecs_add_entity(world, e1, ECS_CHILDOF | Parent1);
    ecs_add_entity(world, e2, ECS_CHILDOF | Parent2);

    ecs_entity_t lookup = ecs_lookup_child(world, Parent1, "Child");
    test_assert(lookup != 0);
    test_assert(lookup == e1);

    lookup = ecs_lookup_child(world, Parent2, "Child");
    test_assert(lookup != 0);
    test_assert(lookup == e2);

    ecs_fini(world);
}

void LookupSystem(ecs_iter_t *it) {
    ecs_entity_t e = ecs_new(it->world, 0);
    test_assert(e != 0);

    ecs_set(it->world, e, EcsName, {"Foo"});
    test_assert( ecs_has(it->world, e, EcsName));

    ecs_entity_t found = ecs_lookup(it->world, "Foo");
    test_assert(found != 0);
    test_assert(found == e);
}

void LookupChildSystem(ecs_iter_t *it) {
    ecs_entity_t parent = ecs_new(it->world, 0);
    test_assert(parent != 0);

    ecs_entity_t e = ecs_new(it->world, 0);
    test_assert(e != 0);

    ecs_add_entity(it->world, e, ECS_CHILDOF | parent);
    test_assert( ecs_has_entity(it->world, e, ECS_CHILDOF | parent));

    ecs_set(it->world, e, EcsName, {"Foo"});
    test_assert( ecs_has(it->world, e, EcsName));

    ecs_entity_t found = ecs_lookup_child(it->world, parent, "Foo");
    test_assert(found != 0);
    test_assert(found == e);
}

void Lookup_lookup_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_SYSTEM(world, LookupSystem, EcsOnUpdate, 0);

    ecs_progress(world, 1);

    ecs_fini(world);
}

void Lookup_lookup_child_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_SYSTEM(world, LookupChildSystem, EcsOnUpdate, 0);

    ecs_progress(world, 1);

    ecs_fini(world);
}

void Lookup_get_name() {
    ecs_world_t *world = ecs_init();

    /* Ensure this doesn't crash the lookup function */
    ecs_entity_t e = ecs_set(world, 0, EcsName, {"Entity"});
    const char *id = ecs_get_name(world, e);
    test_assert(id != NULL);
    test_str(id, "Entity");

    ecs_fini(world);
}

void Lookup_get_name_no_name() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    /* Ensure this doesn't crash the lookup function */
    ecs_entity_t e = ecs_new(world, Position);
    const char *id = ecs_get_name(world, e);
    test_assert(id == NULL);

    ecs_fini(world);
}

void Lookup_get_name_from_empty() {
    ecs_world_t *world = ecs_init();

    /* Ensure this doesn't crash the lookup function */
    ecs_entity_t e = ecs_new(world, 0);
    const char *id = ecs_get_name(world, e);
    test_assert(id == NULL);

    ecs_fini(world);
}

void Lookup_lookup_by_id() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_lookup(world, "1000");
    test_int(e, 1000);

    ecs_fini(world);
}
