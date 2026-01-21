#include <core.h>

void Internals_setup(void) {
    ecs_log_set_level(-3);
}

static
void Iter(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 0);
    Velocity *v = ecs_field(it, Velocity, 1);
    Mass *m = ecs_field(it, Mass, 2);

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

void Internals_deactivate_table(void) {
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

void Internals_activate_table(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Position);

    /* Add entities after system definition to trigger table activation */
    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);

    test_assert(true);

    ecs_fini(world);
}

void Internals_activate_deactivate_table(void) {
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

void Internals_activate_deactivate_reactive(void) {
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

void Internals_activate_deactivate_activate_other(void) {
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

    /* Add entities of different type to trigger new table activation */
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity);

    test_assert(true);

    ecs_fini(world);
}

static int invoked = 0;

static
void CreateNewTable(ecs_iter_t *it) {
    ecs_id_t ecs_id(Velocity) = ecs_field_id(it, 1);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_add(it->world, it->entities[i], Velocity);
    }
}

static
void ManualSystem(ecs_iter_t *it) {
    invoked ++;
}

void Internals_no_double_system_table_after_merge(void) {
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

void Internals_recreate_deleted_table(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t parent_A = ecs_new(world);
    ecs_entity_t child_A = ecs_new_w_pair(world, EcsChildOf, parent_A);
    test_assert(parent_A != 0);
    test_assert(child_A != 0);

    ecs_delete(world, parent_A); // Deletes table
    test_assert( !ecs_is_alive(world, parent_A));
    test_assert( !ecs_is_alive(world, child_A));

    ecs_entity_t parent_B = ecs_new(world);
    ecs_entity_t child_B = ecs_new_w_pair(world, EcsChildOf, parent_B);
    test_assert(parent_B != 0);
    test_assert(child_B != 0);
    
    ecs_fini(world);
}

void Internals_create_65k_tables(void) {
    ecs_world_t *world = ecs_init();

    int32_t i;
    for (i = 0; i <= 65536; i ++) {
        ecs_entity_t e = ecs_new(world);
        ecs_add_id(world, e, e);
        test_assert(ecs_has_id(world, e, e));
        test_int(ecs_get_type(world, e)->count, 1);
    }
    
    ecs_fini(world);
}

void Internals_no_duplicate_root_table_id(void) {
    ecs_world_t *world = ecs_init();

    /* This scenario triggered a bug where the first table registered in the
     * world would get id 0, which is the same id as the root table. This caused
     * the table cache to assert as it saw the wrong table for an id. */

    int32_t i;
    for (i = 0; i <= 50; i ++) {
        ecs_entity_t e = ecs_new(world);
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

void Internals_override_os_api_w_addon(void) {
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

void Internals_records_resize_on_override(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t TagA = ecs_new(world);
    ecs_entity_t TagB = ecs_new(world);
    ecs_entity_t TagC = ecs_new(world);
    ecs_entity_t TagD = ecs_new(world);
    ecs_entity_t TagE = ecs_new(world);
    ecs_entity_t TagF = ecs_new(world);
    ecs_entity_t TagG = ecs_new(world);
    ecs_entity_t TagH = ecs_new(world);

    ecs_entity_t RelA = ecs_new(world);
    ecs_entity_t RelB = ecs_new(world);
    ecs_entity_t RelC = ecs_new(world);
    ecs_entity_t RelD = ecs_new(world);
    ecs_entity_t RelE = ecs_new(world);
    ecs_entity_t RelF = ecs_new(world);
    ecs_entity_t RelG = ecs_new(world);
    ecs_entity_t TgtA = ecs_new(world);
    ecs_entity_t TgtB = ecs_new(world);

    ecs_entity_t e = ecs_new(world);
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
    ecs_auto_override_pair(world, e, RelG, TgtB);

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

void Internals_table_observed_after_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_table_t *pt = ecs_get_table(world, p);
    ecs_table_t *ct = ecs_get_table(world, c);
    test_assert(pt != NULL);
    test_int(ecs_table_get_type(pt)->count, 0);
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

void Internals_table_observed_after_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_table_t *pt = ecs_get_table(world, p);
    ecs_table_t *ct = ecs_get_table(world, c);
    test_assert(pt != NULL);
    test_int(ecs_table_get_type(pt)->count, 0);
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

void Internals_table_observed_after_clear(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_table_t *pt = ecs_get_table(world, p);
    ecs_table_t *ct = ecs_get_table(world, c);
    test_assert(pt != NULL);
    test_int(ecs_table_get_type(pt)->count, 0);
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

void Internals_table_observed_after_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_table_t *pt = ecs_get_table(world, p);
    ecs_table_t *ct = ecs_get_table(world, c);
    test_assert(pt != NULL);
    test_int(ecs_table_get_type(pt)->count, 0);
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

void Internals_table_observed_after_on_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_table_t *pt = ecs_get_table(world, p);
    ecs_table_t *ct = ecs_get_table(world, c);
    test_assert(pt != NULL);
    test_int(ecs_table_get_type(pt)->count, 0);
    test_assert(ct != NULL);
    test_int(flecs_table_observed_count(ct), 0);

    ecs_entity_t t = ecs_new(world);
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

void Internals_table_observed_after_entity_flag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t p = ecs_new(world);

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

void Internals_table_create_leak_check(void) {
    ecs_world_t *world = ecs_mini();

    int64_t max_block_count;

    ecs_entity_t tag = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, tag);
    ecs_delete(world, tag);
    
    test_assert(ecs_get_table(world, e) != NULL);

    max_block_count = ecs_block_allocator_alloc_count - 
        ecs_block_allocator_free_count;

    for (int i = 0; i < 25000; i ++) {
        tag = ecs_new(world);
        ecs_add_id(world, e, tag);
        ecs_delete(world, tag);
    }

    test_int(max_block_count, ecs_block_allocator_alloc_count - 
        ecs_block_allocator_free_count);

    ecs_fini(world);
}

void Internals_component_record_has_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Tag);

    ecs_table_t *table_1 = ecs_table_add_id(world, NULL, ecs_id(Position));
    test_assert(table_1 != NULL);
    ecs_table_t *table_2 = ecs_table_add_id(world, table_1, ecs_id(Velocity));
    test_assert(table_2 != NULL);
    ecs_table_t *table_3 = ecs_table_add_id(world, table_2, Tag);
    test_assert(table_3 != NULL);

    ecs_component_record_t *pos_cr = flecs_components_get(world, ecs_id(Position));
    ecs_component_record_t *vel_cr = flecs_components_get(world, ecs_id(Velocity));
    ecs_component_record_t *tag_cr = flecs_components_get(world, Tag);
    test_assert(pos_cr != NULL);
    test_assert(vel_cr != NULL);
    test_assert(tag_cr != NULL);

    {
        const ecs_table_record_t *tr = flecs_component_get_table(pos_cr, table_1);
        test_assert(tr != NULL);
        test_int(tr->column, 0);
        test_int(tr->index, 0);
        test_int(tr->count, 1);
        test_assert(tr->hdr.table == table_1);
    } {
        const ecs_table_record_t *tr = flecs_component_get_table(vel_cr, table_1);
        test_assert(tr == NULL);
    } {
        const ecs_table_record_t *tr = flecs_component_get_table(vel_cr, table_1);
        test_assert(tr == NULL);
    }

    {
        const ecs_table_record_t *tr = flecs_component_get_table(pos_cr, table_2);
        test_assert(tr != NULL);
        test_int(tr->column, 0);
        test_int(tr->index, 0);
        test_int(tr->count, 1);
        test_assert(tr->hdr.table == table_2);
    } {
        const ecs_table_record_t *tr = flecs_component_get_table(vel_cr, table_2);
        test_assert(tr != NULL);
        test_int(tr->column, 1);
        test_int(tr->index, 1);
        test_int(tr->count, 1);
        test_assert(tr->hdr.table == table_2);
    } {
        const ecs_table_record_t *tr = flecs_component_get_table(tag_cr, table_2);
        test_assert(tr == NULL);
    }

    {
        const ecs_table_record_t *tr = flecs_component_get_table(pos_cr, table_3);
        test_assert(tr != NULL);
        test_int(tr->column, 0);
        test_int(tr->index, 0);
        test_int(tr->count, 1);
        test_assert(tr->hdr.table == table_3);
    } {
        const ecs_table_record_t *tr = flecs_component_get_table(vel_cr, table_3);
        test_assert(tr != NULL);
        test_int(tr->column, 1);
        test_int(tr->index, 1);
        test_int(tr->count, 1);
        test_assert(tr->hdr.table == table_3);
    } {
        const ecs_table_record_t *tr = flecs_component_get_table(tag_cr, table_3);
        test_assert(tr != NULL);
        test_int(tr->column, -1);
        test_int(tr->index, 2);
        test_int(tr->count, 1);
        test_assert(tr->hdr.table == table_3);
    }

    ecs_fini(world);
}

void Internals_component_record_iter_tables(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Tag);

    ecs_table_t *table_1 = ecs_table_add_id(world, NULL, ecs_id(Position));
    test_assert(table_1 != NULL);
    ecs_table_t *table_2 = ecs_table_add_id(world, table_1, ecs_id(Velocity));
    test_assert(table_2 != NULL);
    ecs_table_t *table_3 = ecs_table_add_id(world, table_2, Tag);
    test_assert(table_3 != NULL);

    ecs_component_record_t *pos_cr = flecs_components_get(world, ecs_id(Position));
    test_assert(pos_cr != NULL);

    ecs_table_cache_iter_t it;
    if (flecs_component_iter(pos_cr, &it)) {
        const ecs_table_record_t *tr;

        tr = flecs_component_next(&it);
        test_assert(tr != NULL);
        test_assert(tr->hdr.table == table_1);

        tr = flecs_component_next(&it);
        test_assert(tr != NULL);
        test_assert(tr->hdr.table == table_2);

        tr = flecs_component_next(&it);
        test_assert(tr != NULL);
        test_assert(tr->hdr.table == table_3);

        tr = flecs_component_next(&it);
        test_assert(tr == NULL);
    }

    ecs_fini(world);
}

void Internals_table_get_records(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Tag);

    ecs_table_t *table = ecs_table_add_id(world, NULL, ecs_id(Position));
    test_assert(table != NULL);
    table = ecs_table_add_id(world, table, ecs_id(Velocity));
    test_assert(table != NULL);
    table = ecs_table_add_id(world, table, Tag);
    test_assert(table != NULL);

    ecs_table_records_t r = flecs_table_records(table);
    test_int(r.count, 5); /* Including *, (ChildOf, 0) */

    test_int(r.array[0].column, 0);
    test_int(r.array[0].index, 0);
    test_int(r.array[0].count, 1);

    test_int(r.array[1].column, 1);
    test_int(r.array[1].index, 1);
    test_int(r.array[1].count, 1);

    test_int(r.array[2].column, -1);
    test_int(r.array[2].index, 2);
    test_int(r.array[2].count, 1);

    ecs_fini(world);
}

void Internals_childof_tgt_exists_after_query(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_childof(parent) }}
    });

    test_assert(q != NULL);

    /* Make sure just querying for a pair doesn't create a component record */
    test_assert(flecs_components_get(world, ecs_childof(parent)) == NULL);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Internals_component_record_depth_root(void) {
    ecs_world_t *world = ecs_mini();

    ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(0));
    test_assert(cr != NULL);

    test_int(0, flecs_component_get_childof_depth(cr));

    ecs_fini(world);
}

void Internals_component_record_depth_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_childof(parent));
    test_assert(cr != NULL);

    test_int(1, flecs_component_get_childof_depth(cr));

    ecs_fini(world);
}

void Internals_component_record_depth_nested_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_new_w_pair(world, EcsChildOf, child);

    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_childof(child));
    test_assert(cr != NULL);

    test_int(2, flecs_component_get_childof_depth(cr));

    ecs_fini(world);
}

void Internals_component_record_depth_after_reparent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t parent_2 = ecs_new(world);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_new_w_pair(world, EcsChildOf, child);

    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_childof(child));
    test_assert(cr != NULL);

    test_int(2, flecs_component_get_childof_depth(cr));

    ecs_add_pair(world, child, EcsChildOf, parent_2);

    test_int(2, flecs_component_get_childof_depth(cr));

    ecs_fini(world);
}

void Internals_component_record_depth_after_reparent_different_depth(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t child_2 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_new_w_pair(world, EcsChildOf, child);

    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_childof(child));
    test_assert(cr != NULL);

    test_int(2, flecs_component_get_childof_depth(cr));

    ecs_add_pair(world, child, EcsChildOf, child_2);

    test_int(3, flecs_component_get_childof_depth(cr));

    ecs_fini(world);
}

void Internals_component_record_depth_nested_after_reparent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t parent_2 = ecs_new(world);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t grand_child = ecs_new_w_pair(world, EcsChildOf, child);
    ecs_new_w_pair(world, EcsChildOf, grand_child);

    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_childof(grand_child));
    test_assert(cr != NULL);

    test_int(3, flecs_component_get_childof_depth(cr));

    ecs_add_pair(world, child, EcsChildOf, parent_2);

    test_int(3, flecs_component_get_childof_depth(cr));

    ecs_fini(world);
}

void Internals_component_record_depth_nested_after_reparent_different_depth(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t child_2 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t grand_child = ecs_new_w_pair(world, EcsChildOf, child);
    ecs_new_w_pair(world, EcsChildOf, grand_child);

    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_childof(grand_child));
    test_assert(cr != NULL);

    test_int(3, flecs_component_get_childof_depth(cr));

    ecs_add_pair(world, child, EcsChildOf, child_2);

    test_int(4, flecs_component_get_childof_depth(cr));

    ecs_fini(world);
}

void Internals_component_record_depth_after_parent_remove(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_new_w_pair(world, EcsChildOf, child);

    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_childof(child));
    test_assert(cr != NULL);

    test_int(2, flecs_component_get_childof_depth(cr));

    ecs_remove_pair(world, child, EcsChildOf, parent);

    test_int(1, flecs_component_get_childof_depth(cr));

    ecs_fini(world);
}

void Internals_component_record_depth_nested_after_parent_remove(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t grand_child = ecs_new_w_pair(world, EcsChildOf, child);
    ecs_new_w_pair(world, EcsChildOf, grand_child);

    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_childof(grand_child));
    test_assert(cr != NULL);

    test_int(3, flecs_component_get_childof_depth(cr));

    ecs_remove_pair(world, child, EcsChildOf, parent);

    test_int(2, flecs_component_get_childof_depth(cr));

    ecs_fini(world);
}

void Internals_component_record_depth_after_parent_add(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t child = ecs_new(world);
    ecs_new_w_pair(world, EcsChildOf, child);

    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_childof(child));
    test_assert(cr != NULL);

    test_int(1, flecs_component_get_childof_depth(cr));

    ecs_add_pair(world, child, EcsChildOf, parent);

    test_int(2, flecs_component_get_childof_depth(cr));

    ecs_fini(world);
}

void Internals_component_record_depth_nested_after_parent_add(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t child = ecs_new(world);
    ecs_entity_t grand_child = ecs_new_w_pair(world, EcsChildOf, child);
    ecs_new_w_pair(world, EcsChildOf, grand_child);

    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_childof(grand_child));
    test_assert(cr != NULL);

    test_int(2, flecs_component_get_childof_depth(cr));

    ecs_add_pair(world, child, EcsChildOf, parent);

    test_int(3, flecs_component_get_childof_depth(cr));

    ecs_fini(world);
}

void Internals_non_fragmenting_component_record_depth_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_childof(parent));
    test_assert(cr != NULL);

    test_int(1, flecs_component_get_childof_depth(cr));

    ecs_fini(world);
}

void Internals_non_fragmenting_component_record_depth_nested_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_add_id(world, child, EcsOrderedChildren);

    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_childof(child));
    test_assert(cr != NULL);

    test_int(2, flecs_component_get_childof_depth(cr));

    ecs_fini(world);
}

void Internals_non_fragmenting_component_record_depth_after_reparent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);
    ecs_entity_t parent_2 = ecs_new(world);
    ecs_add_id(world, parent_2, EcsOrderedChildren);

    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_add_id(world, child, EcsOrderedChildren);

    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_childof(child));
    test_assert(cr != NULL);

    test_int(2, flecs_component_get_childof_depth(cr));

    ecs_set(world, child, EcsParent, {parent_2});

    test_int(2, flecs_component_get_childof_depth(cr));

    ecs_fini(world);
}

void Internals_non_fragmenting_component_record_depth_after_reparent_different_depth(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_add_id(world, child, EcsOrderedChildren);
    ecs_entity_t child_2 = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_add_id(world, child_2, EcsOrderedChildren);

    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_childof(child));
    test_assert(cr != NULL);

    test_int(2, flecs_component_get_childof_depth(cr));

    ecs_set(world, child, EcsParent, {child_2});

    test_int(3, flecs_component_get_childof_depth(cr));

    ecs_fini(world);
}

void Internals_non_fragmenting_component_record_depth_nested_after_reparent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);
    ecs_entity_t parent_2 = ecs_new(world);
    ecs_add_id(world, parent_2, EcsOrderedChildren);
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_add_id(world, child, EcsOrderedChildren);
    ecs_entity_t grand_child = ecs_insert(world, ecs_value(EcsParent, {child}));
    ecs_add_id(world, grand_child, EcsOrderedChildren);

    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_childof(grand_child));
    test_assert(cr != NULL);

    test_int(3, flecs_component_get_childof_depth(cr));

    ecs_set(world, child, EcsParent, {parent_2});

    test_int(3, flecs_component_get_childof_depth(cr));

    ecs_fini(world);
}

void Internals_non_fragmenting_component_record_depth_nested_after_reparent_different_depth(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);
    ecs_entity_t child_2 = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_add_id(world, child_2, EcsOrderedChildren);
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_add_id(world, child, EcsOrderedChildren);
    ecs_entity_t grand_child = ecs_insert(world, ecs_value(EcsParent, {child}));
    ecs_add_id(world, grand_child, EcsOrderedChildren);

    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_childof(grand_child));
    test_assert(cr != NULL);

    test_int(3, flecs_component_get_childof_depth(cr));

    ecs_set(world, child, EcsParent, {child_2});

    test_int(4, flecs_component_get_childof_depth(cr));

    ecs_fini(world);
}

void Internals_non_fragmenting_component_record_depth_after_parent_remove(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_add_id(world, child, EcsOrderedChildren);

    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_childof(child));
    test_assert(cr != NULL);

    test_int(2, flecs_component_get_childof_depth(cr));

    ecs_remove(world, child, EcsParent);

    test_int(1, flecs_component_get_childof_depth(cr));

    ecs_fini(world);
}

void Internals_non_fragmenting_component_record_depth_nested_after_parent_remove(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_add_id(world, child, EcsOrderedChildren);
    ecs_entity_t grand_child = ecs_insert(world, ecs_value(EcsParent, {child}));
    ecs_add_id(world, grand_child, EcsOrderedChildren);

    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_childof(grand_child));
    test_assert(cr != NULL);

    test_int(3, flecs_component_get_childof_depth(cr));

    ecs_remove(world, child, EcsParent);

    test_int(2, flecs_component_get_childof_depth(cr));

    ecs_fini(world);
}

void Internals_non_fragmenting_component_record_depth_after_parent_add(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);
    ecs_entity_t child = ecs_new(world);
    ecs_add_id(world, child, EcsOrderedChildren);
    ecs_new_w_pair(world, EcsChildOf, child);

    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_childof(child));
    test_assert(cr != NULL);

    test_int(1, flecs_component_get_childof_depth(cr));

    ecs_set(world, child, EcsParent, {parent});

    test_int(2, flecs_component_get_childof_depth(cr));

    ecs_fini(world);
}

void Internals_non_fragmenting_component_record_depth_nested_after_parent_add(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);
    ecs_entity_t child = ecs_new(world);
    ecs_add_id(world, child, EcsOrderedChildren);
    ecs_entity_t grand_child = ecs_insert(world, ecs_value(EcsParent, {child}));
    ecs_add_id(world, grand_child, EcsOrderedChildren);

    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_childof(grand_child));
    test_assert(cr != NULL);

    test_int(2, flecs_component_get_childof_depth(cr));

    ecs_set(world, child, EcsParent, {parent});

    test_int(3, flecs_component_get_childof_depth(cr));

    ecs_fini(world);
}

void Internals_component_record_for_value_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_entity_t e = ecs_new(world);

    ecs_add_id(world, e, ecs_value_pair(Rel, 10));

    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_value_pair(Rel, 10));
    test_assert(cr != NULL);
    test_uint(ecs_value_pair(Rel, 10), flecs_component_get_id(cr));

    test_assert(flecs_components_get(world, ecs_pair(Rel, 10)) == NULL);
    test_assert(flecs_components_get(world, ecs_value_pair(Rel, EcsWildcard)) == NULL);
    test_assert(flecs_components_get(world, ecs_pair(EcsWildcard, 10)) == NULL);
    test_assert(flecs_components_get(world, ecs_value_pair(EcsWildcard, 10)) == NULL);

    ecs_component_record_t *parent = flecs_components_get(
        world, ecs_pair(Rel, EcsWildcard));
    test_assert(parent != NULL);
    test_uint(ecs_pair(Rel, EcsWildcard), flecs_component_get_id(parent));

    ecs_fini(world);
}
