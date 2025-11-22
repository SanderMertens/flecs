#include <query.h>

void ChangeDetection_query_changed_after_new(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    
    ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position",
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
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
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
    });
    test_assert(q != NULL);
    test_bool(true, ecs_query_changed(q));
    test_bool(true, ecs_query_changed(q));

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) { }
    test_bool(false, ecs_query_changed(q));

    ecs_delete(world, e1);
    test_bool(true, ecs_query_changed(q));

    it = ecs_query_iter(world, q);
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
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
    });
    test_assert(q != NULL);
    test_bool(true, ecs_query_changed(q));

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) { }
    test_bool(false, ecs_query_changed(q));

    ecs_add(world, e1, Position);
    test_bool(true, ecs_query_changed(q));

    it = ecs_query_iter(world, q);
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
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
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
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
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
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
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
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
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

void ChangeDetection_query_change_after_out_query_no_data_flag(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_new_w(world, Position);

    ecs_query_t *q_write = ecs_query(world, {
        .expr = "[inout] Position",
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
    });
    test_assert(q_write != NULL);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position",
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
    });
    test_assert(q != NULL);
    test_bool(true, ecs_query_changed(q));
    test_bool(true, ecs_query_changed(q));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_changed(q));
        while (ecs_query_next(&it)) { }
        test_bool(false, ecs_query_changed(q));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q_write);
        it.flags |= EcsIterNoData;
        while (ecs_query_next(&it)) { }
        test_bool(false, ecs_query_changed(q));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_changed(q));
        while (ecs_query_next(&it)) {
            test_bool(false, ecs_iter_changed(&it));
        }
        test_bool(false, ecs_query_changed(q));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_query_change_after_in_system(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Sys, EcsOnUpdate, [in] Position);
    
    ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position",
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
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
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
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
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
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
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
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
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
    });

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position",
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
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
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
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
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    
    ecs_entity_t base = ecs_new_w(world, Position);
    ecs_new_w_pair(world, EcsIsA, base);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position(up IsA)",
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
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
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
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
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new_w(world, Position);
    ecs_add_id(world, base, EcsPrefab);
    ecs_new_w_pair(world, EcsIsA, base);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position(up IsA)",
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
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
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
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
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
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
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
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
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
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
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
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
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
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

    ecs_add_id(world, ecs_id(Velocity), EcsSingleton);

    ecs_singleton_set(world, Velocity, {1, 2});

    ecs_query_t *q = ecs_query(world, {
        .expr = "[in] Position, Velocity",
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
    });

    test_assert(q != NULL);

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

    ecs_add_id(world, ecs_id(Position), EcsSingleton);

    ecs_singleton_set(world, Position, {1, 2});

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
    });

    test_assert(q != NULL);

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

    ecs_add_id(world, ecs_id(Position), EcsSingleton);

    ecs_singleton_set(world, Position, {1, 2});

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
    });

    test_assert(q != NULL);

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

    ecs_add_id(world, ecs_id(Position), EcsSingleton);

    ecs_singleton_set(world, Position, {1, 2});

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
    });

    test_assert(q != NULL);

    ecs_query_t *q_write = ecs_query(world, {
        .expr = "[out] Position($this)"
    });

    test_assert(q_write != NULL);

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

    ecs_add_id(world, ecs_id(Position), EcsSingleton);

    ecs_singleton_set(world, Position, {1, 2});

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
    });

    test_assert(q != NULL);

    ecs_query_t *q_write = ecs_query(world, {
        .expr = "[out] Position"
    });

    test_assert(q_write != NULL);

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
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
    });

    test_assert(q != NULL);

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
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
    });
    
    test_assert(q != NULL);

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
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
    });

    test_assert(q != NULL);

    ecs_query_t *q_write = ecs_query(world, {
        .expr = "[out] Position"
    });

    test_assert(q_write != NULL);

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
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
    });

    test_assert(q != NULL);

    ecs_query_t *q_write = ecs_query(world, {
        .expr = "[out] Position(up)"
    });

    test_assert(q_write != NULL);

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

void ChangeDetection_query_change_w_optional(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_query_t *q_read = ecs_query(world, {
        .expr = "[in] Position, [in] Velocity",
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
    });
    test_assert(q_read != NULL);

    ecs_query_t *q_write = ecs_query(world, {
        .expr = "[in] Position, [in] Velocity, [out] ?Mass",
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
    });
    test_assert(q_write != NULL);

    ecs_insert(world, 
        ecs_value(Position, {10, 20}),
        ecs_value(Velocity, {1, 2}));

    test_bool(true, ecs_query_changed(q_read));
    test_bool(true, ecs_query_changed(q_read));

    {
        ecs_iter_t it = ecs_query_iter(world, q_read);
        test_bool(true, ecs_query_changed(q_read));
        while (ecs_query_next(&it)) { }
        test_bool(false, ecs_query_changed(q_read));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q_write);
        while (ecs_query_next(&it)) { }
    }

    test_bool(false, ecs_query_changed(q_read));

    ecs_query_fini(q_read);
    ecs_query_fini(q_write);

    ecs_fini(world);
}

void ChangeDetection_query_changed_after_count(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t foo = ecs_new(world);

    // This makes the query not entirely cacheable, which prevents query_count
    // from just getting the numbers from the cache.
    ecs_add_id(world, foo, EcsCanToggle);

    ecs_entity_t bar = ecs_new(world);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add_id(world, e1, foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = foo, .inout = EcsIn }},
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
    });
    test_assert(q != NULL);

    ecs_add_id(world, e1, bar);
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_bool(true, ecs_iter_changed(&it));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove_id(world, e1, bar);
    ecs_add_id(world, e1, bar);

    ecs_query_count(q); // Make sure this doesn't affect change detection

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_bool(true, ecs_iter_changed(&it));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}


void ChangeDetection_staged_query_w_shared_inout_field(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_world_t *stage = ecs_get_stage(world, 0);

    ecs_query_t *qr = ecs_query(world, {
        .terms = {{ ecs_id(Position), .inout = EcsIn }},
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
    });
    test_assert(qr != NULL);

    {
        test_bool(true, ecs_query_changed(qr));
        ecs_iter_t it = ecs_query_iter(world, qr);
        while (ecs_query_next(&it)) { }
        test_bool(false, ecs_query_changed(qr));
    }

    ecs_query_t *q = ecs_query(stage, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp, .inout = EcsInOut }}
    });
    test_assert(q != NULL);

    ecs_entity_t p = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p);

    {
        test_bool(true, ecs_query_changed(qr));
        ecs_iter_t it = ecs_query_iter(world, qr);
        while (ecs_query_next(&it)) { }
        test_bool(false, ecs_query_changed(qr));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(c, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        test_bool(true, ecs_query_changed(qr));
        ecs_iter_t it = ecs_query_iter(world, qr);
        while (ecs_query_next(&it)) { }
        test_bool(false, ecs_query_changed(qr));
    }

    ecs_query_fini(q);
    ecs_query_fini(qr);

    ecs_fini(world);
}

void ChangeDetection_staged_query_w_fixed_inout_field(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_world_t *stage = ecs_get_stage(world, 0);

    ecs_query_t *qr = ecs_query(world, {
        .terms = {{ ecs_id(Position), .inout = EcsIn }},
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
    });
    test_assert(qr != NULL);

    {
        test_bool(true, ecs_query_changed(qr));
        ecs_iter_t it = ecs_query_iter(world, qr);
        while (ecs_query_next(&it)) { }
        test_bool(false, ecs_query_changed(qr));
    }

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));

    ecs_query_t *q = ecs_query(stage, {
        .terms = {{ ecs_id(Position), .src.id = e, .inout = EcsInOut }}
    });
    test_assert(q != NULL);

    {
        test_bool(true, ecs_query_changed(qr));
        ecs_iter_t it = ecs_query_iter(world, qr);
        while (ecs_query_next(&it)) { }
        test_bool(false, ecs_query_changed(qr));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e, ecs_field_src(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        test_bool(true, ecs_query_changed(qr));
        ecs_iter_t it = ecs_query_iter(world, qr);
        while (ecs_query_next(&it)) { }
        test_bool(false, ecs_query_changed(qr));
    }

    ecs_query_fini(q);
    ecs_query_fini(qr);

    ecs_fini(world);
}

void ChangeDetection_staged_query_w_fixed_inout_field_read(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_world_t *stage = ecs_get_stage(world, 0);

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));

    ecs_query_t *qr = ecs_query(stage, {
        .terms = {
            { ecs_id(Position), .src.id = e, .inout = EcsIn },
            { ecs_id(Position), .inout = EcsIn }
        },
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
    });
    test_assert(qr != NULL);

    ecs_query_t *q = ecs_query(stage, {
        .terms = {{ ecs_id(Position), .src.id = e, .inout = EcsInOut }}
    });
    test_assert(q != NULL);

    {
        test_bool(true, ecs_query_changed(qr));
        ecs_iter_t it = ecs_query_iter(world, qr);
        while (ecs_query_next(&it)) {
            test_bool(true, ecs_iter_changed(&it));
        }
        test_bool(false, ecs_query_changed(qr));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e, ecs_field_src(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        test_bool(true, ecs_query_changed(qr));
        ecs_iter_t it = ecs_query_iter(world, qr);
        while (ecs_query_next(&it)) {
            test_bool(true, ecs_iter_changed(&it));
        }
        test_bool(false, ecs_query_changed(qr));
    }

    ecs_query_fini(q);
    ecs_query_fini(qr);

    ecs_fini(world);
}

void ChangeDetection_simple_write_query(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));

    ecs_query_t *qr = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .src.id = e, .inout = EcsIn },
            { ecs_id(Position), .inout = EcsIn }
        },
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
    });
    test_assert(qr != NULL);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position) }}
    });
    test_assert(q != NULL);

    {
        test_bool(true, ecs_query_changed(qr));
        ecs_iter_t it = ecs_query_iter(world, qr);
        while (ecs_query_next(&it)) {
            test_bool(true, ecs_iter_changed(&it));
        }
        test_bool(false, ecs_query_changed(qr));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        test_bool(false, ecs_query_next(&it));
    }

    {
        test_bool(true, ecs_query_changed(qr));
    }

    ecs_query_fini(q);
    ecs_query_fini(qr);

    ecs_fini(world);
}

void ChangeDetection_change_detection_w_early_out(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));

    ecs_query_t *qr = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .src.id = e, .inout = EcsIn },
            { ecs_id(Position), .inout = EcsIn }
        },
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
    });
    test_assert(qr != NULL);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position) }}
    });
    test_assert(q != NULL);

    {
        test_bool(true, ecs_query_changed(qr));
        ecs_iter_t it = ecs_query_iter(world, qr);
        while (ecs_query_next(&it)) {
            test_bool(true, ecs_iter_changed(&it));
        }
        test_bool(false, ecs_query_changed(qr));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }

        ecs_iter_fini(&it); // should flag current result as changed
    }

    {
        test_bool(true, ecs_query_changed(qr));
    }

    ecs_query_fini(q);
    ecs_query_fini(qr);

    ecs_fini(world);
}

void ChangeDetection_change_detection_w_early_out_skip(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));

    ecs_query_t *qr = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .src.id = e, .inout = EcsIn },
            { ecs_id(Position), .inout = EcsIn }
        },
        .cache_kind = EcsQueryCacheAuto,
        .flags = EcsQueryDetectChanges
    });
    test_assert(qr != NULL);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position) }}
    });
    test_assert(q != NULL);

    {
        test_bool(true, ecs_query_changed(qr));
        ecs_iter_t it = ecs_query_iter(world, qr);
        while (ecs_query_next(&it)) {
            test_bool(true, ecs_iter_changed(&it));
        }
        test_bool(false, ecs_query_changed(qr));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }

        ecs_iter_skip(&it);

        ecs_iter_fini(&it); // should not flag current result as changed
    }

    {
        test_bool(false, ecs_query_changed(qr));
    }

    ecs_query_fini(q);
    ecs_query_fini(qr);

    ecs_fini(world);
}

void ChangeDetection_mark_dirty_w_uncached_trivial_search(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position) }
        }
    });

    test_assert(q != NULL);

    ecs_query_t *q_r = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .inout = EcsIn }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q_r != NULL);

    ecs_new_w(world, Position);

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_assert(it.flags & EcsIterTrivialSearch);
        while (ecs_query_next(&it));
    }

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    ecs_query_fini(q);
    ecs_query_fini(q_r);

    ecs_fini(world);
}

void ChangeDetection_mark_dirty_w_uncached_trivial_test(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position) }
        }
    });

    test_assert(q != NULL);

    ecs_query_t *q_r = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .inout = EcsIn }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q_r != NULL);

    ecs_entity_t e = ecs_new_w(world, Position);

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e);
        test_assert(it.flags & EcsIterTrivialTest);
        while (ecs_query_next(&it));
    }

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    ecs_query_fini(q);
    ecs_query_fini(q_r);

    ecs_fini(world);
}

void ChangeDetection_mark_dirty_w_uncached_w_up_search(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .src.id = EcsUp, .inout = EcsOut }
        }
    });

    test_assert(q != NULL);

    ecs_query_t *q_r = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .inout = EcsIn }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q_r != NULL);

    ecs_entity_t parent = ecs_new_w(world, Position);
    ecs_new_w_pair(world, EcsChildOf, parent);

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_assert(!(it.flags & EcsIterTrivialSearch));
        while (ecs_query_next(&it));
    }

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    ecs_query_fini(q);
    ecs_query_fini(q_r);

    ecs_fini(world);
}

void ChangeDetection_mark_dirty_w_uncached_w_up_test(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .src.id = EcsUp, .inout = EcsOut }
        }
    });

    test_assert(q != NULL);

    ecs_query_t *q_r = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .inout = EcsIn }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q_r != NULL);

    ecs_entity_t parent = ecs_new_w(world, Position);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, parent);

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e);
        test_assert(!(it.flags & EcsIterTrivialTest));
        while (ecs_query_next(&it));
    }

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    ecs_query_fini(q);
    ecs_query_fini(q_r);

    ecs_fini(world);
}

void ChangeDetection_mark_dirty_w_uncached_w_fixed_search(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_entity_t fixed = ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .src.id = fixed },
            { ecs_id(Velocity) },
        }
    });

    test_assert(q != NULL);

    ecs_query_t *q_r_1 = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .inout = EcsIn }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q_r_1 != NULL);

    ecs_query_t *q_r_2 = ecs_query(world, {
        .terms = {
            { ecs_id(Velocity), .inout = EcsIn }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q_r_2 != NULL);

    ecs_new_w(world, Velocity);

    test_assert(ecs_query_changed(q_r_1));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r_1);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r_1));

    test_assert(ecs_query_changed(q_r_2));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r_2);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r_2));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_assert(!(it.flags & EcsIterTrivialSearch));
        while (ecs_query_next(&it));
    }

    test_assert(ecs_query_changed(q_r_1));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r_1);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r_1));

    test_assert(ecs_query_changed(q_r_2));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r_2);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r_2));

    ecs_query_fini(q);
    ecs_query_fini(q_r_1);
    ecs_query_fini(q_r_2);

    ecs_fini(world);
}

void ChangeDetection_mark_dirty_w_uncached_w_fixed_test(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_entity_t fixed = ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .src.id = fixed },
            { ecs_id(Velocity) },
        }
    });

    test_assert(q != NULL);

    ecs_query_t *q_r_1 = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .inout = EcsIn }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q_r_1 != NULL);

    ecs_query_t *q_r_2 = ecs_query(world, {
        .terms = {
            { ecs_id(Velocity), .inout = EcsIn }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q_r_2 != NULL);

    ecs_entity_t e = ecs_new_w(world, Velocity);

    test_assert(ecs_query_changed(q_r_1));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r_1);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r_1));

    test_assert(ecs_query_changed(q_r_2));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r_2);
        ecs_iter_set_var(&it, 0, e);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r_2));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_assert(!(it.flags & EcsIterTrivialSearch));
        ecs_iter_set_var(&it, 0, e);
        while (ecs_query_next(&it));
    }

    test_assert(ecs_query_changed(q_r_1));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r_1);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r_1));

    test_assert(ecs_query_changed(q_r_2));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r_2);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r_2));

    ecs_query_fini(q);
    ecs_query_fini(q_r_1);
    ecs_query_fini(q_r_2);

    ecs_fini(world);
}

void ChangeDetection_mark_dirty_w_uncached_w_wildcard_search(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tgt);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_pair_t(Position, EcsWildcard) }
        }
    });

    test_assert(q != NULL);

    ecs_query_t *q_r = ecs_query(world, {
        .terms = {
            { ecs_pair_t(Position, EcsWildcard), .inout = EcsIn }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q_r != NULL);

    ecs_new_w_pair(world, ecs_id(Position), Tgt);

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_assert(!(it.flags & EcsIterTrivialSearch));
        while (ecs_query_next(&it));
    }

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    ecs_query_fini(q);
    ecs_query_fini(q_r);

    ecs_fini(world);
}

void ChangeDetection_mark_dirty_w_uncached_w_wildcard_test(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tgt);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_pair_t(Position, EcsWildcard) }
        }
    });

    test_assert(q != NULL);

    ecs_query_t *q_r = ecs_query(world, {
        .terms = {
            { ecs_pair_t(Position, EcsWildcard), .inout = EcsIn }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q_r != NULL);

    ecs_entity_t e = ecs_new_w_pair(world, ecs_id(Position), Tgt);

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e);
        test_assert(!(it.flags & EcsIterTrivialTest));
        while (ecs_query_next(&it));
    }

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    ecs_query_fini(q);
    ecs_query_fini(q_r);

    ecs_fini(world);
}

void ChangeDetection_mark_dirty_w_cached_trivial_search(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position) }
        },
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(q != NULL);

    ecs_query_t *q_r = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .inout = EcsIn }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q_r != NULL);

    ecs_new_w(world, Position);

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_assert(it.flags & EcsIterTrivialSearch);
        while (ecs_query_next(&it));
    }

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    ecs_query_fini(q);
    ecs_query_fini(q_r);

    ecs_fini(world);
}

void ChangeDetection_mark_dirty_w_cached_trivial_test(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position) }
        },
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(q != NULL);

    ecs_query_t *q_r = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .inout = EcsIn }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q_r != NULL);

    ecs_entity_t e = ecs_new_w(world, Position);

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e);
        test_assert(it.flags & EcsIterTrivialTest);
        while (ecs_query_next(&it));
    }

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    ecs_query_fini(q);
    ecs_query_fini(q_r);

    ecs_fini(world);
}

void ChangeDetection_mark_dirty_w_cached_w_up_search(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, Rel, Traversable);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .src.id = EcsUp, .inout = EcsOut, .trav = Rel }
        },
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(q != NULL);

    ecs_query_t *q_r = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .inout = EcsIn }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q_r != NULL);

    ecs_entity_t parent = ecs_new_w(world, Position);
    ecs_new_w_pair(world, Rel, parent);

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_assert(it.flags & EcsIterTrivialSearch);
        while (ecs_query_next(&it));
    }

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    ecs_query_fini(q);
    ecs_query_fini(q_r);

    ecs_fini(world);
}

void ChangeDetection_mark_dirty_w_cached_w_up_test(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, Rel, Traversable);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .src.id = EcsUp, .inout = EcsOut, .trav = Rel }
        },
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(q != NULL);

    ecs_query_t *q_r = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .inout = EcsIn }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q_r != NULL);

    ecs_entity_t parent = ecs_new_w(world, Position);
    ecs_entity_t e = ecs_new_w_pair(world, Rel, parent);

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e);
        test_assert(it.flags & EcsIterTrivialTest);
        while (ecs_query_next(&it));
    }

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    ecs_query_fini(q);
    ecs_query_fini(q_r);

    ecs_fini(world);
}

void ChangeDetection_mark_dirty_w_cached_w_up_search_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .src.id = EcsUp, .inout = EcsOut }
        },
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(q != NULL);

    ecs_query_t *q_r = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .inout = EcsIn }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q_r != NULL);

    ecs_entity_t parent = ecs_new_w(world, Position);
    ecs_new_w_pair(world, EcsChildOf, parent);

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_assert(!(it.flags & EcsIterTrivialSearch));
        while (ecs_query_next(&it));
    }

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    ecs_query_fini(q);
    ecs_query_fini(q_r);

    ecs_fini(world);
}

void ChangeDetection_mark_dirty_w_cached_w_up_test_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .src.id = EcsUp, .inout = EcsOut }
        },
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(q != NULL);

    ecs_query_t *q_r = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .inout = EcsIn }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q_r != NULL);

    ecs_entity_t parent = ecs_new_w(world, Position);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, parent);

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e);
        test_assert(!(it.flags & EcsIterTrivialTest));
        while (ecs_query_next(&it));
    }

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    ecs_query_fini(q);
    ecs_query_fini(q_r);

    ecs_fini(world);
}

void ChangeDetection_mark_dirty_w_cached_w_fixed_search(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_entity_t fixed = ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .src.id = fixed },
            { ecs_id(Velocity) },
        },
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(q != NULL);

    ecs_query_t *q_r_1 = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .inout = EcsIn }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q_r_1 != NULL);

    ecs_query_t *q_r_2 = ecs_query(world, {
        .terms = {
            { ecs_id(Velocity), .inout = EcsIn }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q_r_2 != NULL);

    ecs_new_w(world, Velocity);

    test_assert(ecs_query_changed(q_r_1));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r_1);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r_1));

    test_assert(ecs_query_changed(q_r_2));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r_2);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r_2));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_assert(!(it.flags & EcsIterTrivialSearch));
        while (ecs_query_next(&it));
    }

    test_assert(ecs_query_changed(q_r_1));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r_1);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r_1));

    test_assert(ecs_query_changed(q_r_2));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r_2);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r_2));

    ecs_query_fini(q);
    ecs_query_fini(q_r_1);
    ecs_query_fini(q_r_2);

    ecs_fini(world);
}

void ChangeDetection_mark_dirty_w_cached_w_fixed_test(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_entity_t fixed = ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .src.id = fixed },
            { ecs_id(Velocity) },
        },
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(q != NULL);

    ecs_query_t *q_r_1 = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .inout = EcsIn }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q_r_1 != NULL);

    ecs_query_t *q_r_2 = ecs_query(world, {
        .terms = {
            { ecs_id(Velocity), .inout = EcsIn }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q_r_2 != NULL);

    ecs_entity_t e = ecs_new_w(world, Velocity);

    test_assert(ecs_query_changed(q_r_1));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r_1);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r_1));

    test_assert(ecs_query_changed(q_r_2));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r_2);
        ecs_iter_set_var(&it, 0, e);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r_2));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_assert(!(it.flags & EcsIterTrivialSearch));
        ecs_iter_set_var(&it, 0, e);
        while (ecs_query_next(&it));
    }

    test_assert(ecs_query_changed(q_r_1));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r_1);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r_1));

    test_assert(ecs_query_changed(q_r_2));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r_2);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r_2));

    ecs_query_fini(q);
    ecs_query_fini(q_r_1);
    ecs_query_fini(q_r_2);

    ecs_fini(world);
}

void ChangeDetection_mark_dirty_w_cached_w_wildcard_search(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tgt);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_pair_t(Position, EcsWildcard) }
        },
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(q != NULL);

    ecs_query_t *q_r = ecs_query(world, {
        .terms = {
            { ecs_pair_t(Position, EcsWildcard), .inout = EcsIn }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q_r != NULL);

    ecs_new_w_pair(world, ecs_id(Position), Tgt);

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_assert(it.flags & EcsIterTrivialSearch);
        while (ecs_query_next(&it));
    }

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    ecs_query_fini(q);
    ecs_query_fini(q_r);

    ecs_fini(world);
}

void ChangeDetection_mark_dirty_w_cached_w_wildcard_test(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tgt);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_pair_t(Position, EcsWildcard) }
        },
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(q != NULL);

    ecs_query_t *q_r = ecs_query(world, {
        .terms = {
            { ecs_pair_t(Position, EcsWildcard), .inout = EcsIn }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q_r != NULL);

    ecs_entity_t e = ecs_new_w_pair(world, ecs_id(Position), Tgt);

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e);
        test_assert(it.flags & EcsIterTrivialTest);
        while (ecs_query_next(&it));
    }

    test_assert(ecs_query_changed(q_r));
    {
        ecs_iter_t it = ecs_query_iter(world, q_r);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q_r));

    ecs_query_fini(q);
    ecs_query_fini(q_r);

    ecs_fini(world);
}

void ChangeDetection_detect_w_trivial_search(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position) }},
        .flags = EcsQueryDetectChanges
    });

    test_assert(q != NULL);

    test_assert(ecs_query_changed(q));
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q));

    ecs_new_w(world, Position);

    test_assert(ecs_query_changed(q));
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q));

    ecs_fini(world);
}

void ChangeDetection_detect_w_trivial_test(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position) }},
        .flags = EcsQueryDetectChanges
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_add(world, e2, Velocity);

    test_assert(ecs_query_changed(q));
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q));

    ecs_set(world, e2, Position, {10, 20});

    test_assert(ecs_query_changed(q));
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        while (ecs_query_next(&it));
    }
    test_assert(ecs_query_changed(q));

    test_assert(ecs_query_changed(q));
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q));

    ecs_fini(world);
}

void ChangeDetection_detect_w_up_search(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }},
        .flags = EcsQueryDetectChanges
    });

    test_assert(q != NULL);

    test_assert(ecs_query_changed(q));
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q));

    ecs_entity_t parent = ecs_new_w(world, Position);

    test_assert(!ecs_query_changed(q));

    ecs_new_w_pair(world, EcsChildOf, parent);

    test_assert(ecs_query_changed(q));
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q));

    ecs_fini(world);
}

void ChangeDetection_detect_w_up_test(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position), .src.id = EcsUp }},
        .flags = EcsQueryDetectChanges
    });

    test_assert(q != NULL);

    ecs_entity_t p1 = ecs_new_w(world, Position);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t p2 = ecs_new_w(world, Position);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p2);

    test_assert(ecs_query_changed(q));
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q));

    ecs_set(world, p2, Position, {10, 20});

    test_assert(ecs_query_changed(q));
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        while (ecs_query_next(&it));
    }
    test_assert(ecs_query_changed(q));

    test_assert(ecs_query_changed(q));
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q));

    ecs_fini(world);
}

void ChangeDetection_detect_w_fixed_search(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t fixed = ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .src.id = fixed },
            { ecs_id(Velocity) }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q != NULL);

    ecs_entity_t e = ecs_new_w(world, Velocity);

    test_assert(ecs_query_changed(q));
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q));

    ecs_set(world, fixed, Position, {10, 20});

    test_assert(ecs_query_changed(q));
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q));

    ecs_set(world, e, Velocity, {1, 2});

    test_assert(ecs_query_changed(q));
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q));

    ecs_fini(world);
}

void ChangeDetection_detect_w_fixed_test(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Foo);

    ecs_entity_t fixed = ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position), .src.id = fixed },
            { ecs_id(Velocity) }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w(world, Velocity);
    ecs_entity_t e2 = ecs_new_w(world, Velocity);
    ecs_add(world, e2, Foo);

    test_assert(ecs_query_changed(q));
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        while (ecs_query_next(&it));
    }
    test_assert(ecs_query_changed(q));
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q));

    ecs_set(world, fixed, Position, {10, 20});

    test_assert(ecs_query_changed(q));
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q));

    ecs_set(world, e2, Velocity, {1, 2});

    test_assert(ecs_query_changed(q));
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        while (ecs_query_next(&it));
    }
    test_assert(ecs_query_changed(q));

    test_assert(ecs_query_changed(q));
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q));    

    ecs_fini(world);
}

void ChangeDetection_detect_w_wildcard_search(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tgt);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_pair_t(Position, EcsWildcard) }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q != NULL);

    ecs_entity_t e = ecs_new_w_pair(world, ecs_id(Position), Tgt);

    test_assert(ecs_query_changed(q));
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q));

    ecs_set_pair(world, e, Position, Tgt, {10, 20});

    test_assert(ecs_query_changed(q));
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q));

    ecs_fini(world);
}

void ChangeDetection_detect_w_wildcard_test(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_pair_t(Position, EcsWildcard) }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, ecs_id(Position), TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, ecs_id(Position), TgtB);

    test_assert(ecs_query_changed(q));
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        while (ecs_query_next(&it));
    }
    test_assert(ecs_query_changed(q));
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q));

    ecs_set_pair(world, e1, Position, TgtA, {10, 20});

    test_assert(ecs_query_changed(q));
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q));

    ecs_set_pair(world, e2, Position, TgtB, {10, 20});

    test_assert(ecs_query_changed(q));
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        while (ecs_query_next(&it));
    }
    test_assert(ecs_query_changed(q));
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        while (ecs_query_next(&it));
    }
    test_assert(!ecs_query_changed(q));

    ecs_fini(world);
}

void ChangeDetection_detect_w_group_by(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Position) }},
        .group_by = Rel,
        .flags = EcsQueryDetectChanges
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_set(world, e1, Position, {10, 20});

    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_set(world, e2, Position, {30, 40});

    ecs_entity_t e3 = ecs_new_w_pair(world, Rel, TgtC);
    ecs_set(world, e3, Position, {50, 60});

    test_bool(ecs_query_changed(q), true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it));
    }

    test_bool(ecs_query_changed(q), false);

    ecs_set(world, e1, Position, {1, 2});

    test_bool(ecs_query_changed(q), true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it)) {
            test_assert(ecs_iter_get_group(&it) == TgtA || !ecs_iter_changed(&it));
        }
    }

    test_bool(ecs_query_changed(q), false);

    ecs_set(world, e2, Position, {1, 2});

    test_bool(ecs_query_changed(q), true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it)) {
            test_assert(ecs_iter_get_group(&it) == TgtB || !ecs_iter_changed(&it));
        }
    }

    test_bool(ecs_query_changed(q), false);

    ecs_set(world, e3, Position, {1, 2});

    test_bool(ecs_query_changed(q), true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it)) {
            test_assert(ecs_iter_get_group(&it) == TgtC || !ecs_iter_changed(&it));
        }
    }

    test_bool(ecs_query_changed(q), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_detect_w_childof_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position) }, 
            { ecs_childof(EcsAny), .src.id = EcsSelf }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});

    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_set(world, e2, Position, {30, 40});

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_set(world, e3, Position, {50, 60});

    test_bool(ecs_query_changed(q), true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it));
    }

    test_bool(ecs_query_changed(q), false);

    ecs_set(world, e1, Position, {1, 2});

    test_bool(ecs_query_changed(q), false);

    ecs_set(world, e2, Position, {1, 2});

    test_bool(ecs_query_changed(q), true);

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_detect_w_childof_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position) }, 
            { ecs_childof(EcsAny), .src.id = EcsUp }
        },
        .flags = EcsQueryDetectChanges,
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});

    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_set(world, e2, Position, {30, 40});

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_set(world, e3, Position, {50, 60});

    test_bool(ecs_query_changed(q), true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it));
    }

    test_bool(ecs_query_changed(q), false);

    ecs_set(world, e1, Position, {1, 2});

    test_bool(ecs_query_changed(q), false);

    ecs_set(world, e2, Position, {1, 2});

    test_bool(ecs_query_changed(q), false);

    ecs_set(world, e3, Position, {1, 2});

    test_bool(ecs_query_changed(q), true);

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_detect_w_childof_self_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position) }, 
            { ecs_childof(EcsAny), .src.id = EcsSelf | EcsUp }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});

    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_set(world, e2, Position, {30, 40});

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_set(world, e3, Position, {50, 60});

    test_bool(ecs_query_changed(q), true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it));
    }

    test_bool(ecs_query_changed(q), false);

    ecs_set(world, e1, Position, {1, 2});

    test_bool(ecs_query_changed(q), false);

    ecs_set(world, e2, Position, {1, 2});

    test_bool(ecs_query_changed(q), true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it));
    }

    test_bool(ecs_query_changed(q), false);


    ecs_set(world, e3, Position, {1, 2});

    test_bool(ecs_query_changed(q), true);

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_detect_w_cascade(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position) }, 
            { ecs_childof(EcsAny), .src.id = EcsSelf|EcsCascade }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});

    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_set(world, e2, Position, {30, 40});

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_set(world, e3, Position, {50, 60});

    test_bool(ecs_query_changed(q), true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it));
    }

    test_bool(ecs_query_changed(q), false);

    ecs_set(world, e1, Position, {1, 2});

    test_bool(ecs_query_changed(q), false);

    ecs_set(world, e2, Position, {1, 2});

    test_bool(ecs_query_changed(q), true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it)) {
            test_assert(ecs_iter_get_group(&it) == 1 || !ecs_iter_changed(&it));
        }
    }

    test_bool(ecs_query_changed(q), false);

    ecs_set(world, e3, Position, {1, 2});

    test_bool(ecs_query_changed(q), true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it)) {
            test_assert(ecs_iter_get_group(&it) == 2 || !ecs_iter_changed(&it));
        }
    }

    test_bool(ecs_query_changed(q), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_detect_w_cascade_desc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position) }, 
            { ecs_childof(EcsAny), .src.id = EcsSelf|EcsCascade|EcsDesc }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});

    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_set(world, e2, Position, {30, 40});

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_set(world, e3, Position, {50, 60});

    test_bool(ecs_query_changed(q), true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it));
    }

    test_bool(ecs_query_changed(q), false);

    ecs_set(world, e1, Position, {1, 2});

    test_bool(ecs_query_changed(q), false);

    ecs_set(world, e2, Position, {1, 2});

    test_bool(ecs_query_changed(q), true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it)) {
            test_assert(ecs_iter_get_group(&it) == 1 || !ecs_iter_changed(&it));
        }
    }

    test_bool(ecs_query_changed(q), false);

    ecs_set(world, e3, Position, {1, 2});

    test_bool(ecs_query_changed(q), true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it)) {
            test_assert(ecs_iter_get_group(&it) == 2 || !ecs_iter_changed(&it));
        }
    }

    test_bool(ecs_query_changed(q), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void ChangeDetection_detect_partially_cached(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Velocity), EcsDontFragment);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position) }, 
            { ecs_id(Velocity), .src.id = EcsSelf }
        },
        .flags = EcsQueryDetectChanges
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});

    ecs_entity_t e2 = ecs_new_w(world, Velocity);
    ecs_set(world, e2, Position, {30, 40});

    test_bool(ecs_query_changed(q), true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it));
    }

    test_bool(ecs_query_changed(q), false);

    ecs_set(world, e1, Position, {1, 2});

    test_bool(ecs_query_changed(q), true); // False positive because partially cached

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it));
    }

    test_bool(ecs_query_changed(q), false);

    ecs_set(world, e2, Position, {1, 2});

    test_bool(ecs_query_changed(q), true);

    ecs_query_fini(q);

    ecs_fini(world);
}
