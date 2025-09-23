/*
 * Tests generated from: ../../../docs/Quickstart.md
 * Contains 25 test(s) from documentation code snippets
 */

#include <core.h>

void quickstart_flecs_quickstart_concepts_world_01(void) {
    ecs_world_t *world = ecs_init();

    // Do the ECS stuff

    ecs_fini(world);

    test_assert(true);
}

void quickstart_flecs_quickstart_concepts_entity_02(void) {
    ecs_world_t *world = ecs_init();
    ecs_entity_t e = ecs_new(world);
    ecs_is_alive(world, e); // true!

    ecs_delete(world, e);
    ecs_is_alive(world, e); // false!
    ecs_fini(world);

    test_assert(true);
}

void quickstart_flecs_quickstart_concepts_entity_03(void) {
    ecs_world_t *world = ecs_init();
    ecs_entity_t e = ecs_entity(world, { .name = "Bob" });

    printf("Entity name: %s\n", ecs_get_name(world, e));
    ecs_fini(world);

    test_assert(true);
}

void quickstart_flecs_quickstart_concepts_entity_04(void) {
    ecs_world_t *world = ecs_init();
    ecs_entity_t bob = ecs_entity(world, { .name = "Bob" });
    ecs_entity_t e = ecs_lookup(world, "Bob");
    ecs_fini(world);

    test_assert(true);
}

void quickstart_flecs_quickstart_concepts_component_05(void) {
    ecs_world_t *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world);

    // Add a component. This creates the component in the ECS storage, but does not
    // assign it with a value.
    ecs_add(world, e, Velocity);

    // Set the value for the Position & Velocity components. A component will be
    // added if the entity doesn't have it yet.
    ecs_set(world, e, Position, {10, 20});
    ecs_set(world, e, Velocity, {1, 2});

    // Get a component
    const Position *p = ecs_get(world, e, Position);

    // Remove component
    ecs_remove(world, e, Position);
    ecs_fini(world);

    test_assert(true);
}

void quickstart_flecs_quickstart_concepts_component_06(void) {
    ecs_world_t *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Serializable);

    ecs_entity_t pos_e = ecs_id(Position);
    printf("Name: %s\n", ecs_get_name(world, pos_e)); // outputs 'Name: Position'

    // It's possible to add components like you would for any entity
    ecs_add(world, pos_e, Serializable);
    ecs_fini(world);

    test_assert(true);
}

void quickstart_flecs_quickstart_concepts_component_07(void) {
    ecs_world_t *world = ecs_init();
    ECS_COMPONENT(world, Position);

    ecs_entity_t pos_e = ecs_id(Position);

    const EcsComponent *c = ecs_get(world, pos_e, EcsComponent);
    printf("Component size: %u\n", c->size);
    ecs_fini(world);

    test_assert(true);
}

void quickstart_flecs_quickstart_concepts_tag_08(void) {
    ecs_world_t *world = ecs_init();
    // Create Enemy tag
    ecs_entity_t Enemy = ecs_new(world);

    // Create entity, add Enemy tag
    ecs_entity_t e = ecs_new(world);

    ecs_add_id(world, e, Enemy);
    ecs_has_id(world, e, Enemy); // true!

    ecs_remove_id(world, e, Enemy);
    ecs_has_id(world, e, Enemy); // false!
    ecs_fini(world);

    test_assert(true);
}

void quickstart_flecs_quickstart_concepts_pair_09(void) {
    ecs_world_t *world = ecs_init();
    // Create Likes relationship
    ecs_entity_t Likes = ecs_new(world);

    // Create a small graph with two entities that like each other
    ecs_entity_t Bob = ecs_new(world);
    ecs_entity_t Alice = ecs_new(world);

    ecs_add_pair(world, Bob, Likes, Alice); // Bob likes Alice
    ecs_add_pair(world, Alice, Likes, Bob); // Alice likes Bob
    ecs_has_pair(world, Bob, Likes, Alice); // true!

    ecs_remove_pair(world, Bob, Likes, Alice);
    ecs_has_pair(world, Bob, Likes, Alice); // false!
    ecs_fini(world);

    test_assert(true);
}

void quickstart_flecs_quickstart_concepts_pair_10(void) {
    ecs_world_t *world = ecs_init();
    ecs_entity_t Likes = ecs_new(world);
    ecs_entity_t Bob = ecs_new(world);
    ecs_id_t id = ecs_pair(Likes, Bob);
    ecs_fini(world);

    test_assert(true);
}

void quickstart_flecs_quickstart_concepts_pair_11(void) {
    ecs_world_t *world = ecs_init();
    ecs_entity_t Likes = ecs_new(world);
    ecs_entity_t Bob = ecs_new(world);
    ecs_id_t id = ecs_pair(Likes, Bob);
    if (ecs_id_is_pair(id)) {
        ecs_entity_t relationship = ecs_pair_first(world, id);
        ecs_entity_t target = ecs_pair_second(world, id);
    }
    ecs_fini(world);

    test_assert(true);
}

void quickstart_flecs_quickstart_concepts_pair_12(void) {
    ecs_world_t *world = ecs_init();
    ecs_entity_t Bob = ecs_new(world);
    ecs_entity_t Eats = ecs_new(world);
    ecs_entity_t Grows = ecs_new(world);
    ecs_entity_t Apples = ecs_new(world);
    ecs_entity_t Pears = ecs_new(world);
    ecs_add_pair(world, Bob, Eats, Apples);
    ecs_add_pair(world, Bob, Eats, Pears);
    ecs_add_pair(world, Bob, Grows, Pears);

    ecs_has_pair(world, Bob, Eats, Apples); // true!
    ecs_has_pair(world, Bob, Eats, Pears);  // true!
    ecs_has_pair(world, Bob, Grows, Pears); // true!
    ecs_fini(world);

    test_assert(true);
}

void quickstart_flecs_quickstart_concepts_pair_13(void) {
    ecs_world_t *world = ecs_init();
    ecs_entity_t Likes = ecs_new(world);
    ecs_entity_t Alice = ecs_new(world);
    ecs_entity_t Bob = ecs_new(world);
    ecs_add_pair(world, Alice, Likes, Bob);
    ecs_entity_t o = ecs_get_target(world, Alice, Likes, 0); // Returns Bob
    ecs_fini(world);

    test_assert(true);
}

void quickstart_flecs_quickstart_concepts_hierarchies_14(void) {
    ecs_world_t *world = ecs_init();
    ecs_entity_t parent = ecs_new(world);

    // ecs_new_w_pair is the same as ecs_new_id + ecs_add_pair
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);

    // Deleting the parent also deletes its children
    ecs_delete(world, parent);
    ecs_fini(world);

    test_assert(true);
}

void quickstart_flecs_quickstart_concepts_hierarchies_15(void) {
    ecs_world_t *world = ecs_init();
    ecs_entity_t parent = ecs_entity(world, {
        .name = "parent"
    });

    ecs_entity_t child = ecs_entity(world, {
        .name = "child"
    });

    ecs_add_pair(world, child, EcsChildOf, parent);

    char *path = ecs_get_path(world, child);
    printf("%s\n", path); // output: 'parent.child'
    ecs_os_free(path);

    ecs_lookup(world, "parent.child");         // returns child
    ecs_lookup_from(world, parent, "child");   // returns child
    ecs_fini(world);

    test_assert(true);
}

void quickstart_flecs_quickstart_concepts_hierarchies_16(void) {
    ecs_world_t *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), 
                .trav = EcsChildOf, // Use ChildOf relationship for traversal
                .src.id = EcsCascade // Breadth-first order
            }}
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        Position *p = ecs_field(&it, Position, 0);
        Position *p_parent = ecs_field(&it, Position, 1);
        for (int i = 0; i < it.count; i++) {
            // Do the thing
        }
    }
    ecs_fini(world);

    test_assert(true);
}

void quickstart_flecs_quickstart_concepts_type_17(void) {
    ecs_world_t *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);

    const ecs_type_t *type = ecs_get_type(world, e);
    char *type_str = ecs_type_str(world, type);
    printf("Type: %s\n", type_str); // output: 'Position,Velocity'
    ecs_os_free(type_str);
    ecs_fini(world);

    test_assert(true);
}

void quickstart_flecs_quickstart_concepts_type_18(void) {
    ecs_world_t *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ecs_entity_t e = ecs_new(world);
    const ecs_type_t *type = ecs_get_type(world, e);
    for (int i = 0; i < type->count; i++) {
        if (type->array[i] == ecs_id(Position)) {
            // Found Position component!
        }
    }
    ecs_fini(world);

    test_assert(true);
}

void quickstart_flecs_quickstart_concepts_type_19(void) {
    ecs_world_t *world = ecs_init();
    ECS_COMPONENT(world, Gravity);
    // Register singleton component
    ecs_add_id(world, ecs_id(Gravity), EcsSingleton);

    // Set singleton component
    ecs_singleton_set(world, Gravity, { 9.81 });

    // Get singleton component
    const Gravity *g = ecs_singleton_get(world, Gravity);
    ecs_fini(world);

    test_assert(true);
}

void quickstart_flecs_quickstart_concepts_type_20(void) {
    ecs_world_t *world = ecs_init();
    ECS_COMPONENT(world, Gravity);
    ecs_set(world, ecs_id(Gravity), Gravity, {9.81});

    const Gravity *g = ecs_get(world, ecs_id(Gravity), Gravity);
    ecs_fini(world);

    test_assert(true);
}

void quickstart_flecs_quickstart_concepts_query_21(void) {
    ecs_world_t *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ecs_entity_t parent = ecs_new(world);
    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_pair(EcsChildOf, parent) }
        }
    });

    // Iterate the query results. Because entities are grouped by their type there
    // are two loops: an outer loop for the type, and an inner loop for the entities
    // for that type.
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        // Each type has its own set of component arrays
        Position *p = ecs_field(&it, Position, 0);

        // Iterate all entities for the type
        for (int i = 0; i < it.count; i++) {
            printf("%s: {%f, %f}\n", ecs_get_name(world, it.entities[i]),
                p[i].x, p[i].y);
        }
    }

    ecs_query_fini(q);
    ecs_fini(world);

    test_assert(true);
}

void quickstart_flecs_quickstart_concepts_query_22(void) {
    ecs_world_t *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_pair(EcsChildOf, EcsWildcard) },
            { ecs_id(Position), .oper = EcsNot },
        }
    });

    // Iteration code is the same
    ecs_fini(world);

    test_assert(true);
}

void quickstart_flecs_quickstart_concepts_system_23(void) {
    ecs_world_t *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    float delta_time = 0.016f;

    {
    // Option 1, use the ECS_SYSTEM convenience macro
    ECS_SYSTEM(world, Move, 0, Position, Velocity);
    ecs_run(world, Move, delta_time, NULL); // Run system
    }
    // Option 2, use the ecs_system_init function/ecs_system macro
    ecs_entity_t move_sys = ecs_system(world, {
        .query.terms = {
            { ecs_id(Position) },
            { ecs_id(Velocity) },
        },
        .callback = Move
    });

    ecs_run(world, move_sys, delta_time, NULL); // Run system

    // The callback code (same for both options)
    /*
    void Move(ecs_iter_t *it) {
        Position *p = ecs_field(it, Position, 0);
        Velocity *v = ecs_field(it, Velocity, 1);

        for (int i = 0; i < it->count; i++) {
            p[i].x += v[i].x * it->delta_time;
            p[i].y += v[i].y * it->delta_time;
        }
    }
    */
    ecs_fini(world);

    test_assert(true);
}

void quickstart_flecs_quickstart_concepts_pipeline_24(void) {
    ecs_world_t *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);
    ecs_remove_id(world, Move, EcsOnUpdate);
    ecs_add_id(world, Move, EcsPostUpdate);
    ecs_fini(world);

    test_assert(true);
}

void quickstart_flecs_quickstart_concepts_observer_25(void) {
    ecs_world_t *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ecs_observer(world, {
        .query.terms = { { ecs_id(Position) }, { ecs_id(Velocity) }},
        .events = { EcsOnSet },
        .callback = OnSetPosition
    });

    // Callback code is same as system

    ecs_entity_t e = ecs_new(world);    // Doesn't invoke the observer
    ecs_set(world, e, Position, {10, 20}); // Doesn't invoke the observer
    ecs_set(world, e, Velocity, {1, 2});   // Invokes the observer
    ecs_set(world, e, Position, {20, 40}); // Invokes the observer
    ecs_fini(world);

    test_assert(true);
}