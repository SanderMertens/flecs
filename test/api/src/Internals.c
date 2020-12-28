#include <api.h>

void Internals_setup() {
    ecs_tracing_enable(-3);
}

static
void Iter(ecs_iter_t *it) {
    ECS_COLUMN(it, Position, p, 1);
    ECS_COLUMN(it, Velocity, v, 2);
    ECS_COLUMN(it, Mass, m, 3);

    probe_system(it);

    int i;
    for (i = 0; i < it->count; i ++) {
        p[i].x = 10;
        p[i].y = 20;

        if (v) {
            v[i].x = 30;
            v[i].y = 40;
        }

        if (m) {
            m[i] = 50;
        }
    }
}

void Internals_deactivate_table() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Position);

    /* System is now matched with archetype of entities. Delete entities to
     * deactivate table for system */
    ecs_delete(world, e_1);
    ecs_delete(world, e_2);

    test_assert(true);

    ecs_fini(world);
}

void Internals_activate_table() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Position);

    /* Add entities after system definition to trigger table activation */
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);

    test_assert(true);

    ecs_fini(world);
}

void Internals_activate_deactivate_table() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Position);

    /* Add entities after system definition to trigger table activation */
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);

    /* System is now matched with archetype of entities. Delete entities to
     * deactivate table for system */
    ecs_delete(world, e_1);
    ecs_delete(world, e_2);

    test_assert(true);

    ecs_fini(world);
}

void Internals_activate_deactivate_reactive() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Position);

    /* Add entities after system definition to trigger table activation */
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);

    /* System is now matched with archetype of entities. Delete entities to
     * deactivate table for system */
    ecs_delete(world, e_1);
    ecs_delete(world, e_2);

    /* Add entities of same type to trigger table reactivation */
    ECS_ENTITY(world, e_3, Position);
    ECS_ENTITY(world, e_4, Position);

    test_assert(true);

    ecs_fini(world);
}

void Internals_activate_deactivate_activate_other() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Position);

    /* Add entities after system definition to trigger table activation */
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);

    /* System is now matched with archetype of entities. Delete entities to
     * deactivate table for system */
    ecs_delete(world, e_1);
    ecs_delete(world, e_2);

    /* Add entities of different type type to trigger new table activation */
    ECS_ENTITY(world, e_3, Position, Velocity);
    ECS_ENTITY(world, e_4, Position, Velocity);

    test_assert(true);

    ecs_fini(world);
}

static int invoked = 0;

static
void CreateNewTable(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, Velocity, 2);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_add(it->world, it->entities[i], Velocity);
    }
}

static
void ManualSystem(ecs_iter_t *it) {
    invoked ++;
}

void Internals_no_double_system_table_after_merge() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e, Position);

    ECS_SYSTEM(world, CreateNewTable, EcsOnUpdate, Position, :Velocity);
    ECS_SYSTEM(world, ManualSystem, 0, Position, Velocity);

    /* CreateNewTable system created a new, non-empty table. This will be merged
     * which will trigger activation of ManualSystem. This will cause the system
     * to go from the inactive_systems array to the manual_systems array. This
     * happens as systems are notified of new tables (during the merge). Because
     * the manual_systems array was evaluated after the inactive_systems array,
     * a table could be added to a system twice. */
    ecs_progress(world, 0);

    /* Validate that the CreateNewTable system has ran */
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    /* Now run the ManualSystem, and make sure it is only invoked once. If it is
     * invoked twice, the table has been registered with the system twice, which
     * is wrong. */
    ecs_run(world, ManualSystem, 0, NULL);

    test_int(invoked, 1);

    ecs_fini(world);
}

void Internals_recreate_deleted_table() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t parent_A = ecs_new(world, 0);
    ecs_entity_t child_A = ecs_new_w_entity(world, ECS_CHILDOF | parent_A);
    test_assert(parent_A != 0);
    test_assert(child_A != 0);

    ecs_delete(world, parent_A); // Deletes table

    ecs_entity_t parent_B = ecs_new(world, 0);
    ecs_entity_t child_B = ecs_new_w_entity(world, ECS_CHILDOF | parent_B);
    test_assert(parent_B != 0);
    test_assert(child_B != 0);
    
    ecs_fini(world);
}

void Internals_create_65k_tables() {
    ecs_world_t *world = ecs_init();

    int32_t i;
    for (i = 0; i <= 65536; i ++) {
        ecs_entity_t e = ecs_new_id(world);
        ecs_add_entity(world, e, e);
        test_assert(ecs_has_entity(world, e, e));
        test_int(ecs_vector_count(ecs_get_type(world, e)), 1);
    }
    
    ecs_fini(world);
}
