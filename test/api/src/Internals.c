#include <api.h>

void Internals_setup() {
    ecs_log_set_level(-3);
}

static
void Iter(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 1);
    Velocity *v = ecs_field(it, Velocity, 2);
    Mass *m = ecs_field(it, Mass, 3);

    probe_iter(it);

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

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Position);

    /* System is now matched with archetype of entities. Delete entities to
     * deactivate table for system */
    ecs_delete(world, e1);
    ecs_delete(world, e2);

    test_assert(true);

    ecs_fini(world);
}

void Internals_activate_table() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Position);

    /* Add entities after system definition to trigger table activation */
    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);

    test_assert(true);

    ecs_fini(world);
}

void Internals_activate_deactivate_table() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Position);

    /* Add entities after system definition to trigger table activation */
    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);

    /* System is now matched with archetype of entities. Delete entities to
     * deactivate table for system */
    ecs_delete(world, e1);
    ecs_delete(world, e2);

    test_assert(true);

    ecs_fini(world);
}

void Internals_activate_deactivate_reactive() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Position);

    /* Add entities after system definition to trigger table activation */
    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);

    /* System is now matched with archetype of entities. Delete entities to
     * deactivate table for system */
    ecs_delete(world, e1);
    ecs_delete(world, e2);

    /* Add entities of same type to trigger table reactivation */
    ECS_ENTITY(world, e3, Position);
    ECS_ENTITY(world, e4, Position);

    test_assert(true);

    ecs_fini(world);
}

void Internals_activate_deactivate_activate_other() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Position);

    /* Add entities after system definition to trigger table activation */
    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);

    /* System is now matched with archetype of entities. Delete entities to
     * deactivate table for system */
    ecs_delete(world, e1);
    ecs_delete(world, e2);

    /* Add entities of different type type to trigger new table activation */
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity);

    test_assert(true);

    ecs_fini(world);
}

static int invoked = 0;

static
void CreateNewTable(ecs_iter_t *it) {
    ecs_id_t ecs_id(Velocity) = ecs_field_id(it, 2);

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

    ECS_SYSTEM(world, CreateNewTable, EcsOnUpdate, Position, Velocity());
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
    ecs_entity_t child_A = ecs_new_w_pair(world, EcsChildOf, parent_A);
    test_assert(parent_A != 0);
    test_assert(child_A != 0);

    ecs_delete(world, parent_A); // Deletes table
    test_assert( !ecs_is_alive(world, parent_A));
    test_assert( !ecs_is_alive(world, child_A));

    ecs_entity_t parent_B = ecs_new(world, 0);
    ecs_entity_t child_B = ecs_new_w_pair(world, EcsChildOf, parent_B);
    test_assert(parent_B != 0);
    test_assert(child_B != 0);
    
    ecs_fini(world);
}

void Internals_create_65k_tables() {
    ecs_world_t *world = ecs_init();

    int32_t i;
    for (i = 0; i <= 65536; i ++) {
        ecs_entity_t e = ecs_new_id(world);
        ecs_add_id(world, e, e);
        test_assert(ecs_has_id(world, e, e));
        test_int(ecs_get_type(world, e)->count, 1);
    }
    
    ecs_fini(world);
}

void Internals_no_duplicate_root_table_id() {
    ecs_world_t *world = ecs_init();

    /* This scenario triggered a bug where the first table registered in the
     * world would get id 0, which is the same id as the root table. This caused
     * the table cache to assert as it saw the wrong table for an id. */

    int32_t i;
    for (i = 0; i <= 50; i ++) {
        ecs_entity_t e = ecs_new_id(world);
        ecs_add_id(world, e, i + 1000);
        test_assert(e != 0);
        test_assert(ecs_has_id(world, e, i + 1000));
    }

    ecs_entity_t f = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Foo"
    });
    
    test_assert(f != 0);
    test_str(ecs_get_name(world, f), "Foo");
    
    ecs_fini(world);
}

void Internals_override_os_api_w_addon() {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;

    ecs_os_set_api(&os_api);

    test_assert(ecs_os_has_threading());
    test_assert(ecs_os_has_time());
    test_assert(ecs_os_has_logging());
    test_assert(ecs_os_has_heap());

    ecs_world_t *world = ecs_init();
    ecs_fini(world);
}

void Internals_records_resize_on_override() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t TagA = ecs_new_id(world);
    ecs_entity_t TagB = ecs_new_id(world);
    ecs_entity_t TagC = ecs_new_id(world);
    ecs_entity_t TagD = ecs_new_id(world);
    ecs_entity_t TagE = ecs_new_id(world);
    ecs_entity_t TagF = ecs_new_id(world);
    ecs_entity_t TagG = ecs_new_id(world);
    ecs_entity_t TagH = ecs_new_id(world);

    ecs_entity_t RelA = ecs_new_id(world);
    ecs_entity_t RelB = ecs_new_id(world);
    ecs_entity_t RelC = ecs_new_id(world);
    ecs_entity_t RelD = ecs_new_id(world);
    ecs_entity_t RelE = ecs_new_id(world);
    ecs_entity_t RelF = ecs_new_id(world);
    ecs_entity_t RelG = ecs_new_id(world);
    ecs_entity_t TgtA = ecs_new_id(world);
    ecs_entity_t TgtB = ecs_new_id(world);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_id(world, e, TagA);
    ecs_add_id(world, e, TagB);
    ecs_add_id(world, e, TagC);
    ecs_add_id(world, e, TagD);
    ecs_add_id(world, e, TagE);
    ecs_add_id(world, e, TagF);
    ecs_add_id(world, e, TagG);
    ecs_add_id(world, e, TagH);
    ecs_add_pair(world, e, RelA, TgtA);
    ecs_add_pair(world, e, RelB, TgtA);
    ecs_add_pair(world, e, RelC, TgtA);
    ecs_add_pair(world, e, RelD, TgtA);
    ecs_add_pair(world, e, RelE, TgtA);
    ecs_add_pair(world, e, RelF, TgtA);
    ecs_override_pair(world, e, RelG, TgtB);

    test_assert(ecs_has_id(world, e, TagA));
    test_assert(ecs_has_id(world, e, TagB));
    test_assert(ecs_has_id(world, e, TagC));
    test_assert(ecs_has_id(world, e, TagD));
    test_assert(ecs_has_id(world, e, TagE));
    test_assert(ecs_has_id(world, e, TagF));
    test_assert(ecs_has_id(world, e, TagG));
    test_assert(ecs_has_id(world, e, TagH));
    test_assert(ecs_has_pair(world, e, RelA, TgtA));
    test_assert(ecs_has_pair(world, e, RelB, TgtA));
    test_assert(ecs_has_pair(world, e, RelC, TgtA));
    test_assert(ecs_has_pair(world, e, RelD, TgtA));
    test_assert(ecs_has_pair(world, e, RelE, TgtA));
    test_assert(ecs_has_pair(world, e, RelF, TgtA));

    ecs_fini(world);
}

void Internals_table_observed_after_add() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t p = ecs_new_id(world);
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_table_t *pt = ecs_get_table(world, p);
    ecs_table_t *ct = ecs_get_table(world, c);
    test_assert(pt == NULL);
    test_assert(ct != NULL);
    test_int(flecs_table_observed_count(ct), 0);

    ecs_add(world, p, TagA);
    ecs_table_t *pt_a = ecs_get_table(world, p);
    test_assert(pt_a != NULL);
    test_int(flecs_table_observed_count(pt_a), 1);

    ecs_add(world, p, TagB);
    ecs_table_t *pt_b = ecs_get_table(world, p);
    test_assert(pt_b != NULL);
    test_int(flecs_table_observed_count(pt_a), 0);
    test_int(flecs_table_observed_count(pt_b), 1);

    ecs_fini(world);
}

void Internals_table_observed_after_remove() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t p = ecs_new_id(world);
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_table_t *pt = ecs_get_table(world, p);
    ecs_table_t *ct = ecs_get_table(world, c);
    test_assert(pt == NULL);
    test_assert(ct != NULL);
    test_int(flecs_table_observed_count(ct), 0);

    ecs_add(world, p, TagA);
    ecs_table_t *pt_a = ecs_get_table(world, p);
    test_assert(pt_a != NULL);
    test_int(flecs_table_observed_count(pt_a), 1);

    ecs_add(world, p, TagB);
    ecs_table_t *pt_b = ecs_get_table(world, p);
    test_assert(pt_b != NULL);
    test_int(flecs_table_observed_count(pt_a), 0);
    test_int(flecs_table_observed_count(pt_b), 1);

    ecs_remove(world, p, TagB);
    test_int(flecs_table_observed_count(pt_a), 1);
    test_int(flecs_table_observed_count(pt_b), 0);

    ecs_remove(world, p, TagA);
    test_int(flecs_table_observed_count(pt_a), 0);
    test_int(flecs_table_observed_count(pt_b), 0);

    ecs_fini(world);
}

void Internals_table_observed_after_clear() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t p = ecs_new_id(world);
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_table_t *pt = ecs_get_table(world, p);
    ecs_table_t *ct = ecs_get_table(world, c);
    test_assert(pt == NULL);
    test_assert(ct != NULL);
    test_int(flecs_table_observed_count(ct), 0);

    ecs_add(world, p, TagA);
    ecs_table_t *pt_a = ecs_get_table(world, p);
    test_assert(pt_a != NULL);
    test_int(flecs_table_observed_count(pt_a), 1);

    ecs_clear(world, p);
    test_int(flecs_table_observed_count(pt_a), 0);

    ecs_add(world, p, TagA);
    test_int(flecs_table_observed_count(pt_a), 1);

    ecs_fini(world);
}

void Internals_table_observed_after_delete() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t p = ecs_new_id(world);
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_table_t *pt = ecs_get_table(world, p);
    ecs_table_t *ct = ecs_get_table(world, c);
    test_assert(pt == NULL);
    test_assert(ct != NULL);
    test_int(flecs_table_observed_count(ct), 0);

    ecs_add(world, p, TagA);
    ecs_table_t *pt_a = ecs_get_table(world, p);
    test_assert(pt_a != NULL);
    test_int(flecs_table_observed_count(pt_a), 1);

    ecs_delete(world, p);
    test_int(flecs_table_observed_count(pt_a), 0);

    ecs_fini(world);
}

void Internals_table_observed_after_on_remove() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t p = ecs_new_id(world);
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_table_t *pt = ecs_get_table(world, p);
    ecs_table_t *ct = ecs_get_table(world, c);
    test_assert(pt == NULL);
    test_assert(ct != NULL);
    test_int(flecs_table_observed_count(ct), 0);

    ecs_entity_t t = ecs_new_id(world);
    ecs_add_id(world, p, t);
    ecs_table_t *pt_t = ecs_get_table(world, p);
    test_assert(pt_t != NULL);
    test_int(flecs_table_observed_count(pt_t), 1);

    ecs_add(world, p, TagA);
    ecs_table_t *pt_ta = ecs_get_table(world, p);
    test_assert(pt_ta != NULL);
    test_int(flecs_table_observed_count(pt_t), 0);
    test_int(flecs_table_observed_count(pt_ta), 1);

    ecs_delete(world, t);
    test_assert(!ecs_has_id(world, p, t));
    test_assert(ecs_has(world, p, TagA));

    ecs_table_t *p_a = ecs_get_table(world, p);
    test_assert(p_a != NULL);
    test_int(flecs_table_observed_count(p_a), 1);

    ecs_fini(world);
}

void Internals_table_observed_after_entity_flag() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t p = ecs_new_id(world);

    ecs_add(world, p, TagA);
    ecs_table_t *p_a = ecs_get_table(world, p);
    test_assert(p_a != NULL);
    test_int(flecs_table_observed_count(p_a), 0);

    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_table_t *ct = ecs_get_table(world, c);
    test_assert(ct != NULL);
    test_int(flecs_table_observed_count(ct), 0);
    test_int(flecs_table_observed_count(p_a), 1);

    ecs_fini(world);

}

void Internals_table_create_leak_check() {
    ecs_world_t *world = ecs_mini();

    int64_t max_block_count;

    ecs_entity_t tag = ecs_new_id(world);
    ecs_entity_t e = ecs_new_w_id(world, tag);
    max_block_count = ecs_block_allocator_alloc_count - 
        ecs_block_allocator_free_count;
    ecs_delete(world, tag);

    for (int i = 0; i < 25000; i ++) {
        tag = ecs_new_id(world);
        ecs_add_id(world, e, tag);
        ecs_delete(world, tag);
    }

    test_int(max_block_count, ecs_block_allocator_alloc_count - 
        ecs_block_allocator_free_count);

    ecs_fini(world);
}
