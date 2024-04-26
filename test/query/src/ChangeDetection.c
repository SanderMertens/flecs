#include <query.h>

void ChangeDetection_query_changed_after_new(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    
    ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);
    test_bool(true, ecs_query_changed(q));
    test_bool(true, ecs_query_changed(q));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_changed(q));
    while (ecs_query_next(&it)) { }
    test_bool(false, ecs_query_changed(q));

    ecs_new_w(world, Position);
    test_bool(true, ecs_query_changed(q));

    it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_changed(q));

    while (ecs_query_next(&it)) { }
    test_bool(false, ecs_query_changed(q));

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_query_changed_after_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e1 = ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);
    test_bool(true, ecs_query_changed(q));
    test_bool(true, ecs_query_changed(q));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_changed(q));

    while (ecs_query_next(&it)) { }
    test_bool(false, ecs_query_changed(q));

    ecs_delete(world, e1);
    test_bool(true, ecs_query_changed(q));

    it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_changed(q));
    while (ecs_query_next(&it)) { }
    test_bool(false, ecs_query_changed(q));

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_query_changed_after_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e1 = ecs_new(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);
    test_bool(true, ecs_query_changed(q));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_changed(q));
    while (ecs_query_next(&it)) { }
    test_bool(false, ecs_query_changed(q));

    ecs_add(world, e1, Position);
    test_bool(true, ecs_query_changed(q));

    it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_changed(q));
    while (ecs_query_next(&it)) { }

    test_bool(false, ecs_query_changed(q));

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_query_changed_after_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e1 = ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);
    test_bool(true, ecs_query_changed(q));
    test_bool(true, ecs_query_changed(q));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_changed(q));
    while (ecs_query_next(&it)) { }
    test_bool(false, ecs_query_changed(q));

    ecs_remove(world, e1, Position);
    test_bool(true, ecs_query_changed(q));

    it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_changed(q));
    while (ecs_query_next(&it)) { }
    test_bool(false, ecs_query_changed(q));

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_query_changed_after_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e1 = ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);
    test_bool(true, ecs_query_changed(q));
    test_bool(true, ecs_query_changed(q));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_changed(q));
    while (ecs_query_next(&it)) { }
    test_bool(false, ecs_query_changed(q));

    ecs_set(world, e1, Position, {10, 20});
    test_bool(true, ecs_query_changed(q));

    it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_changed(q));
    while (ecs_query_next(&it)) { }
    test_bool(false, ecs_query_changed(q));

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_query_change_after_modified(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e1 = ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);
    test_bool(true, ecs_query_changed(q));
    test_bool(true, ecs_query_changed(q));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_changed(q));
    while (ecs_query_next(&it)) { }
    test_bool(false, ecs_query_changed(q));

    ecs_modified(world, e1, Position);
    test_bool(true, ecs_query_changed(q));

    it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_changed(q));
    while (ecs_query_next(&it)) { }
    test_bool(false, ecs_query_changed(q));

    ecs_query_fini(q);

    ecs_fini(world);
}

static int sys_invoked = 0;
void Sys(ecs_iter_t *it) { 
    sys_invoked ++;
}

void ChangeDetection_query_change_after_out_system(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Sys, EcsOnUpdate, [out] Position);

    ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);
    test_bool(true, ecs_query_changed(q));
    test_bool(true, ecs_query_changed(q));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_changed(q));
    while (ecs_query_next(&it)) { }
    test_bool(false, ecs_query_changed(q));

    ecs_progress(world, 0);
    test_int(sys_invoked, 1);
    test_bool(true, ecs_query_changed(q));

    it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_changed(q));
    while (ecs_query_next(&it)) { }
    test_bool(false, ecs_query_changed(q));

    ecs_query_fini(q);

    ecs_fini(world);

    test_int(sys_invoked, 1);
}

void ChangeDetection_query_change_after_in_system(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Sys, EcsOnUpdate, [in] Position);
    
    ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);
    test_bool(true, ecs_query_changed(q));
    test_bool(true, ecs_query_changed(q));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_changed(q));
    while (ecs_query_next(&it)) { }
    test_bool(false, ecs_query_changed(q));

    ecs_progress(world, 0);
    test_bool(false, ecs_query_changed(q));

    it = ecs_query_iter(world, q);
    test_bool(false, ecs_query_changed(q));
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_query_change_after_modified_out_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_entity_t e = ecs_new_w(world, Position);
    ecs_add(world, e, Velocity);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position, [out] Velocity",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);
    test_bool(true, ecs_query_changed(q));
    test_bool(true, ecs_query_changed(q));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_changed(q));
    while (ecs_query_next(&it)) { }
    test_bool(false, ecs_query_changed(q));

    ecs_set(world, e, Position, {10, 20});
    test_bool(true, ecs_query_changed(q));

    it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_changed(q));
    while (ecs_query_next(&it)) { }
    test_bool(false, ecs_query_changed(q));

    ecs_set(world, e, Velocity, {1, 2});
    test_bool(false, ecs_query_changed(q));

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_query_change_check_iter(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    
    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_entity_t e3 = ecs_new_w(world, Position);

    ecs_add(world, e1, TagA);
    ecs_add(world, e2, TagB);
    ecs_add(world, e3, TagC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(q != NULL);
    test_bool(true, ecs_query_changed(q));
    test_bool(true, ecs_query_changed(q));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_changed(q));
    while (ecs_query_next(&it)) { 
        test_bool(ecs_iter_changed(&it), true);
    }
    test_bool(false, ecs_query_changed(q));

    ecs_modified(world, e1, Position);
    test_bool(true, ecs_query_changed(q));

    it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_changed(q));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_bool(ecs_iter_changed(&it), true);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e2);
    test_bool(ecs_iter_changed(&it), false);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    test_bool(ecs_iter_changed(&it), false);
    test_bool(ecs_query_changed(q), false);
    test_bool(ecs_query_next(&it), false);

    it = ecs_query_iter(world, q);
    test_bool(false, ecs_query_changed(q));
    while (ecs_query_next(&it)) { 
        test_bool(ecs_iter_changed(&it), false);
    }
    test_bool(false, ecs_query_changed(q));

    ecs_modified(world, e2, Position);

    it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_changed(q));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_bool(ecs_iter_changed(&it), false);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e2);
    test_bool(ecs_iter_changed(&it), true);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    test_bool(ecs_iter_changed(&it), false);
    test_bool(ecs_query_changed(q), false);
    test_bool(ecs_query_next(&it), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_query_change_check_iter_after_skip_read(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);
    test_bool(true, ecs_query_changed(q));
    test_bool(true, ecs_query_changed(q));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_changed(q));
    while (ecs_query_next(&it)) { }
    test_bool(false, ecs_query_changed(q));

    ecs_set(world, e, Position, {10, 20});
    test_bool(true, ecs_query_changed(q));

    it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), true);
    test_assert(ecs_iter_changed(&it) == true);
    ecs_iter_skip(&it);
    test_bool(ecs_query_next(&it), false);

    test_bool(true, ecs_query_changed(q));

    it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), true);
    test_assert(ecs_iter_changed(&it) == true);
    test_bool(ecs_query_next(&it), false);
    test_bool(false, ecs_query_changed(q));
    
    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_query_change_check_iter_after_skip_write(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    
    ecs_new_w(world, Position);

    ecs_query_t *qw = ecs_query(world, {
        .expr = "[out] Position",
        .cache_kind = EcsQueryCacheAuto
    });

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);
    test_bool(true, ecs_query_changed(q));
    test_bool(true, ecs_query_changed(q));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_changed(q));
    while (ecs_query_next(&it)) { }
    test_bool(false, ecs_query_changed(q));

    it = ecs_query_iter(world, qw);
    test_bool(false, ecs_query_changed(q));
    test_bool(ecs_query_next(&it), true);
    test_bool(false, ecs_query_changed(q));
    ecs_iter_skip(&it);
    test_bool(ecs_query_next(&it), false);
    test_bool(false, ecs_query_changed(q));

    test_bool(false, ecs_query_changed(q));

    it = ecs_query_iter(world, qw);
    test_bool(false, ecs_query_changed(q));
    test_bool(ecs_query_next(&it), true);
    test_bool(false, ecs_query_changed(q));
    test_bool(ecs_query_next(&it), false);
    test_bool(true, ecs_query_changed(q));

    it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), true);
    test_assert(ecs_iter_changed(&it) == true);
    test_bool(ecs_query_next(&it), false);
    test_bool(false, ecs_query_changed(q));

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_query_change_parent_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t parent = ecs_new_w(world, Position);
    ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position(up)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(q != NULL);

    test_bool(true, ecs_query_changed(q));
    test_bool(true, ecs_query_changed(q));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_changed(q));
    while (ecs_query_next(&it)) { }
    test_bool(false, ecs_query_changed(q));

    ecs_set(world, parent, Position, {10, 20});

    test_bool(true, ecs_query_changed(q));

    it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), true);
    test_assert(ecs_iter_changed(&it) == true);
    test_bool(ecs_query_next(&it), false);
    test_bool(false, ecs_query_changed(q));

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_query_change_prefab_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t base = ecs_new_w(world, Position);
    ecs_new_w_pair(world, EcsIsA, base);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position(up IsA)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);
    test_bool(true, ecs_query_changed(q));
    test_bool(true, ecs_query_changed(q));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_changed(q));
    while (ecs_query_next(&it)) { }
    test_bool(false, ecs_query_changed(q));

    ecs_set(world, base, Position, {10, 20});

    test_bool(true, ecs_query_changed(q));

    it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), true);
    test_assert(ecs_iter_changed(&it) == true);
    test_bool(ecs_query_next(&it), false);
    test_bool(false, ecs_query_changed(q));

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_query_change_parent_term_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t parent = ecs_new_w(world, Position);
    ecs_add_id(world, parent, EcsPrefab);
    ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position(up), ?Prefab",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);
    test_bool(true, ecs_query_changed(q));
    test_bool(true, ecs_query_changed(q));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_changed(q));
    while (ecs_query_next(&it)) { }
    test_bool(false, ecs_query_changed(q));

    ecs_set(world, parent, Position, {10, 20});

    test_bool(true, ecs_query_changed(q));

    it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), true);
    test_assert(ecs_iter_changed(&it) == true);
    test_bool(ecs_query_next(&it), false);
    test_bool(false, ecs_query_changed(q));

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_query_change_prefab_term_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t base = ecs_new_w(world, Position);
    ecs_add_id(world, base, EcsPrefab);
    ecs_new_w_pair(world, EcsIsA, base);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position(up IsA)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);
    test_bool(true, ecs_query_changed(q));
    test_bool(true, ecs_query_changed(q));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_changed(q));
    while (ecs_query_next(&it)) { }
    test_bool(false, ecs_query_changed(q));

    ecs_set(world, base, Position, {10, 20});

    test_bool(true, ecs_query_changed(q));

    it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), true);
    test_assert(ecs_iter_changed(&it) == true);
    test_bool(ecs_query_next(&it), false);
    test_bool(false, ecs_query_changed(q));

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_query_change_skip_non_instanced(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Tag);
    
    ecs_entity_t base = ecs_new_w(world, Position);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsIsA, base);

    ecs_add(world, e1, Velocity);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e3, Velocity);
    ecs_add(world, e4, Velocity);

    ecs_add(world, e3, Tag);
    ecs_add(world, e4, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(up IsA), [out] Velocity",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_query_t *q_r = ecs_query(world, {
        .expr = "Position(up IsA), [in] Velocity",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q_r != NULL);

    test_assert(ecs_query_changed(q_r) == true);
    ecs_iter_t it = ecs_query_iter(world, q_r);
    while (ecs_query_next(&it)) { }
    test_assert(ecs_query_changed(q_r) == false);

    it = ecs_query_iter(world, q);
    int32_t count = 0;

    while (ecs_query_next(&it)) {
        test_int(it.count, 1);

        if (it.entities[0] == e1) {
            ecs_iter_skip(&it); /* Skip one entity in table 1 */
        }
        if (it.entities[0] == e3 || it.entities[0] == e4) {
            ecs_iter_skip(&it); /* Skip both entities in table 1 */
        }

        count ++;
    }

    test_int(count, 4);

    test_bool(ecs_query_changed(q_r), true);
    
    it = ecs_query_iter(world, q_r);
    count = 0;

    while (ecs_query_next(&it)) {
        test_int(it.count, 1);

        if (it.entities[0] == e1 || it.entities[0] == e2) {
            /* Table changed, not all entities were skipped */
            test_bool( ecs_iter_changed(&it), true );
        }
        if (it.entities[0] == e3 || it.entities[0] == e4) {
            /* Table did not change, all entities were skipped */
            test_bool( ecs_iter_changed(&it), false );
        }

        count ++;
    }

    test_int(count, 4);

    test_bool(ecs_query_changed(q_r), false);

    ecs_query_fini(q);
    ecs_query_fini(q_r);

    ecs_fini(world);
}

void ChangeDetection_query_changed_w_or(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);
    ecs_add(world, e, TagA);
    ecs_add(world, e, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position, TagA || TagB, [in] Velocity",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);
    
    test_bool(true, ecs_query_changed(q));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_bool(false, ecs_query_next(&it));
    test_bool(false, ecs_query_changed(q));

    ecs_set(world, e, Position, {10, 20});
    test_bool(true, ecs_query_changed(q));

    it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_bool(false, ecs_query_next(&it));
    test_bool(false, ecs_query_changed(q));

    ecs_set(world, e, Velocity, {10, 20});
    test_bool(true, ecs_query_changed(q));

    it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_bool(false, ecs_query_next(&it));
    test_bool(false, ecs_query_changed(q));

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_query_changed_or(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add(world, e1, Position);
    ecs_add(world, e1, TagA);
    ecs_add(world, e1, TagB);

    ecs_entity_t e2 = ecs_new(world);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e2, TagA);
    ecs_add(world, e2, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, [in] Position || [in] Velocity, TagB",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    test_bool(true, ecs_query_changed(q));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_bool(true, ecs_query_next(&it));
    test_bool(false, ecs_query_next(&it));
    test_bool(false, ecs_query_changed(q));

    ecs_set(world, e1, Position, {10, 20});
    test_bool(true, ecs_query_changed(q));

    it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_bool(true, ecs_query_next(&it));
    test_bool(false, ecs_query_next(&it));
    test_bool(false, ecs_query_changed(q));

    ecs_set(world, e2, Velocity, {10, 20});
    test_bool(true, ecs_query_changed(q));

    it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_bool(true, ecs_query_next(&it));
    test_bool(false, ecs_query_next(&it));
    test_bool(false, ecs_query_changed(q));

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_query_changed_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add(world, e1, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);
    
    test_bool(true, ecs_query_changed(q));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_bool(false, ecs_query_next(&it));
    test_bool(false, ecs_query_changed(q));

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_query_changed_no_source(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add(world, e1, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, [out] TagB()",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);
    
    test_bool(true, ecs_query_changed(q));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_bool(false, ecs_query_next(&it));
    test_bool(false, ecs_query_changed(q));

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_query_changed_no_source_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add(world, e1, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position, [out] Velocity()",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);
    
    test_bool(true, ecs_query_changed(q));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_bool(false, ecs_query_next(&it));
    test_bool(false, ecs_query_changed(q));

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_query_changed_w_not_out(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add(world, e1, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position, [out] !Velocity",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);
    
    test_bool(true, ecs_query_changed(q));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_bool(false, ecs_query_next(&it));
    test_bool(false, ecs_query_changed(q));

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_query_changed_w_singleton(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_singleton_set(world, Velocity, {1, 2});

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position, Velocity($)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_bool(true, ecs_query_changed(q));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it)) { }
        test_bool(false, ecs_query_changed(q));
    }

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));

    test_bool(true, ecs_query_changed(q));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_bool(true, ecs_iter_changed(&it));
        test_uint(e, it.entities[0]);
        Position *p = ecs_field(&it, Position, 0);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_int(p->x, 10);
        test_int(p->y, 20);
        test_int(v->x, 1);
        test_int(v->y, 2);
        test_bool(false, ecs_query_next(&it));
    }

    test_bool(false, ecs_query_changed(q));

    ecs_singleton_set(world, Velocity, {2, 3});

    test_bool(true, ecs_query_changed(q));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_bool(true, ecs_query_changed(q));
        test_bool(true, ecs_iter_changed(&it));
        test_uint(e, it.entities[0]);
        Position *p = ecs_field(&it, Position, 0);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_int(p->x, 10);
        test_int(p->y, 20);
        test_int(v->x, 2);
        test_int(v->y, 3);
        test_bool(false, ecs_query_next(&it));
    }

    test_bool(false, ecs_query_changed(q));

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_query_changed_w_only_singleton(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_singleton_set(world, Position, {1, 2});

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position($)",
        .cache_kind = EcsQueryCacheAuto
    });

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_bool(true, ecs_iter_changed(&it));
        test_int(0, it.count);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 1);
        test_int(p->y, 2);
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_bool(false, ecs_iter_changed(&it));
        test_int(0, it.count);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 1);
        test_int(p->y, 2);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_query_changed_w_only_singleton_after_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_singleton_set(world, Position, {1, 2});

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position($)",
        .cache_kind = EcsQueryCacheAuto
    });

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_bool(true, ecs_iter_changed(&it));
        test_int(0, it.count);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 1);
        test_int(p->y, 2);
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_bool(false, ecs_iter_changed(&it));
        test_int(0, it.count);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 1);
        test_int(p->y, 2);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_singleton_set(world, Position, {3, 4});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_bool(true, ecs_iter_changed(&it));
        test_int(0, it.count);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 3);
        test_int(p->y, 4);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_query_changed_w_only_singleton_after_out_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_singleton_set(world, Position, {1, 2});

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position($)",
        .cache_kind = EcsQueryCacheAuto
    });

    ecs_query_t *q_write = ecs_query(world, {
        .expr = "[out] Position"
    });

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_bool(true, ecs_iter_changed(&it));
        test_int(0, it.count);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 1);
        test_int(p->y, 2);
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_bool(false, ecs_iter_changed(&it));
        test_int(0, it.count);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 1);
        test_int(p->y, 2);
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q_write);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        Position *p = ecs_field(&it, Position, 0);
        p->x = 3;
        p->y = 4;
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_bool(true, ecs_iter_changed(&it));
        test_int(0, it.count);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 3);
        test_int(p->y, 4);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);
    ecs_query_fini(q_write);

    ecs_fini(world);
}

void ChangeDetection_query_changed_w_only_singleton_after_singleton_out_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_singleton_set(world, Position, {1, 2});

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position($)",
        .cache_kind = EcsQueryCacheAuto
    });

    ecs_query_t *q_write = ecs_query(world, {
        .expr = "[out] Position($)"
    });

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_bool(true, ecs_iter_changed(&it));
        test_int(0, it.count);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 1);
        test_int(p->y, 2);
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_bool(false, ecs_iter_changed(&it));
        test_int(0, it.count);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 1);
        test_int(p->y, 2);
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q_write);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        Position *p = ecs_field(&it, Position, 0);
        p->x = 3;
        p->y = 4;
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_bool(true, ecs_iter_changed(&it));
        test_int(0, it.count);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 3);
        test_int(p->y, 4);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);
    ecs_query_fini(q_write);

    ecs_fini(world);
}

void ChangeDetection_query_changed_w_only_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t parent = ecs_insert(world, ecs_value(Position, {1, 2}));
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(up)",
        .cache_kind = EcsQueryCacheAuto
    });

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_bool(true, ecs_iter_changed(&it));
        test_int(1, it.count);
        test_uint(child, it.entities[0]);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 1);
        test_int(p->y, 2);
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_bool(false, ecs_iter_changed(&it));
        test_int(1, it.count);
        test_uint(child, it.entities[0]);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 1);
        test_int(p->y, 2);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_query_changed_w_only_parent_after_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t parent = ecs_insert(world, ecs_value(Position, {1, 2}));
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(up)",
        .cache_kind = EcsQueryCacheAuto
    });

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_bool(true, ecs_iter_changed(&it));
        test_int(1, it.count);
        test_uint(child, it.entities[0]);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 1);
        test_int(p->y, 2);
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_bool(false, ecs_iter_changed(&it));
        test_int(1, it.count);
        test_uint(child, it.entities[0]);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 1);
        test_int(p->y, 2);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, parent, Position, {3, 4});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_bool(true, ecs_iter_changed(&it));
        test_int(1, it.count);
        test_uint(child, it.entities[0]);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 3);
        test_int(p->y, 4);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_query_changed_w_only_parent_after_out_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t parent = ecs_insert(world, ecs_value(Position, {1, 2}));
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(up)",
        .cache_kind = EcsQueryCacheAuto
    });

    ecs_query_t *q_write = ecs_query(world, {
        .expr = "[out] Position"
    });

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_bool(true, ecs_iter_changed(&it));
        test_int(1, it.count);
        test_uint(child, it.entities[0]);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 1);
        test_int(p->y, 2);
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_bool(false, ecs_iter_changed(&it));
        test_int(1, it.count);
        test_uint(child, it.entities[0]);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 1);
        test_int(p->y, 2);
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q_write);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(parent, it.entities[0]);
        Position *p = ecs_field(&it, Position, 0);
        p->x = 3;
        p->y = 4;
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_bool(true, ecs_iter_changed(&it));
        test_int(1, it.count);
        test_uint(child, it.entities[0]);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 3);
        test_int(p->y, 4);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);
    ecs_query_fini(q_write);

    ecs_fini(world);
}

void ChangeDetection_query_changed_w_only_parent_after_parent_out_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t parent = ecs_insert(world, ecs_value(Position, {1, 2}));
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(up)",
        .cache_kind = EcsQueryCacheAuto
    });

    ecs_query_t *q_write = ecs_query(world, {
        .expr = "[out] Position(up)"
    });

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_bool(true, ecs_iter_changed(&it));
        test_int(1, it.count);
        test_uint(child, it.entities[0]);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 1);
        test_int(p->y, 2);
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_bool(false, ecs_iter_changed(&it));
        test_int(1, it.count);
        test_uint(child, it.entities[0]);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 1);
        test_int(p->y, 2);
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q_write);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(child, it.entities[0]);
        Position *p = ecs_field(&it, Position, 0);
        p->x = 3;
        p->y = 4;
        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_bool(true, ecs_iter_changed(&it));
        test_int(1, it.count);
        test_uint(child, it.entities[0]);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 3);
        test_int(p->y, 4);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);
    ecs_query_fini(q_write);

    ecs_fini(world);
}
