#include <api.h>

void Queries_query_changed_after_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_new(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_new(world, Position);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_fini(world);
}

void Queries_query_changed_after_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e1 = ecs_new(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_delete(world, e1);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_fini(world);
}

void Queries_query_changed_after_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e1 = ecs_new(world, 0);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_add(world, e1, Position);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_fini(world);
}

void Queries_query_changed_after_remove() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e1 = ecs_new(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_remove(world, e1, Position);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_fini(world);
}

void Queries_query_changed_after_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e1 = ecs_new(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_set(world, e1, Position, {10, 20});
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_fini(world);
}

void Sys(ecs_iter_t *it) { }

void Queries_query_change_after_out_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Sys, EcsOnUpdate, [out] Position);
    
    ecs_new(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_progress(world, 0);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_fini(world);
}

void Queries_query_change_after_in_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Sys, EcsOnUpdate, [in] Position);
    
    ecs_new(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_progress(world, 0);
    test_assert(ecs_query_changed(q) == false);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_fini(world);
}

void Queries_subquery_match_existing() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_bulk_new(world, Position, 3);
    ecs_bulk_new(world, Velocity, 3);
    ecs_bulk_new(world, Type, 3);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);

    ecs_query_t *sq = ecs_subquery_new(world, q, "Velocity");
    test_assert(sq != NULL);

    int32_t table_count = 0, entity_count = 0;
    ecs_iter_t it = ecs_query_iter(q);
    while (ecs_query_next(&it)) {
        table_count ++;
        entity_count += it.count;

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_has(world, it.entities[i], Position));
        }
    }

    test_int(table_count, 2);
    test_int(entity_count, 6);

    table_count = 0, entity_count = 0;
    it = ecs_query_iter(sq);
    while (ecs_query_next(&it)) {
        table_count ++;
        entity_count += it.count;

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_has(world, it.entities[i], Position));
            test_assert(ecs_has(world, it.entities[i], Velocity));
        }
    } 

    test_int(table_count, 1);
    test_int(entity_count, 3); 

    ecs_query_free(sq); 

    ecs_fini(world);
}

void Queries_subquery_match_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);

    ecs_query_t *sq = ecs_subquery_new(world, q, "Velocity");
    test_assert(sq != NULL);

    ECS_TYPE(world, Type, Position, Velocity);
    ecs_bulk_new(world, Position, 3);
    ecs_bulk_new(world, Velocity, 3);
    ecs_bulk_new(world, Type, 3);

    int32_t table_count = 0, entity_count = 0;
    ecs_iter_t it = ecs_query_iter(q);
    while (ecs_query_next(&it)) {
        table_count ++;
        entity_count += it.count;

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_has(world, it.entities[i], Position));
        }
    }

    test_int(table_count, 2);
    test_int(entity_count, 6);

    table_count = 0, entity_count = 0;
    it = ecs_query_iter(sq);
    while (ecs_query_next(&it)) {
        table_count ++;
        entity_count += it.count;

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_has(world, it.entities[i], Position));
            test_assert(ecs_has(world, it.entities[i], Velocity));
        }
    } 

    test_int(table_count, 1);
    test_int(entity_count, 3);  

    ecs_query_free(sq);

    ecs_fini(world);
}

void Queries_subquery_inactive() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type, Position, Velocity);
    ecs_bulk_new(world, Position, 3);
    ecs_bulk_new(world, Velocity, 3);
    ecs_entity_t e = ecs_new(world, Type);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);

    ecs_query_t *sq = ecs_subquery_new(world, q, "Velocity");
    test_assert(sq != NULL);
    
    /* Create an empty table which should deactivate it for both queries */
    ecs_delete(world, e);

    int32_t table_count = 0, entity_count = 0;
    ecs_iter_t it = ecs_query_iter(q);
    while (ecs_query_next(&it)) {
        table_count ++;
        entity_count += it.count;

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_has(world, it.entities[i], Position));
        }
    }

    test_int(table_count, 1);
    test_int(entity_count, 3);

    table_count = 0, entity_count = 0;
    it = ecs_query_iter(sq);
    test_int(it.table_count, 0);
    test_int(it.inactive_table_count, 1);

    table_count = 0, entity_count = 0;
    it = ecs_query_iter(sq);
    while (ecs_query_next(&it)) {
        table_count ++;
    }

    test_int(table_count, 0);

    ecs_query_free(sq);

    ecs_fini(world);
}

void Queries_subquery_rematch() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_add(world, parent, Position);

    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_add(world, e1, Position);
    ecs_add(world, e1, Velocity);
    ecs_add_entity(world, e1, ECS_CHILDOF | parent);

    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_add(world, e2, Position);
    ecs_add_entity(world, e2, ECS_CHILDOF | parent);

    ecs_query_t *q = ecs_query_new(world, "Position, PARENT:Position");
    test_assert(q != NULL);

    ecs_query_t *sq = ecs_subquery_new(world, q, "Velocity");
    test_assert(sq != NULL);

    int32_t table_count = 0, entity_count = 0;
    ecs_iter_t it = ecs_query_iter(q);
    while (ecs_query_next(&it)) {
        table_count ++;
        entity_count += it.count;

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_has(world, it.entities[i], Position));
        }
    }

    test_int(table_count, 2);  
    test_int(entity_count, 2);

    table_count = 0, entity_count = 0;
    it = ecs_query_iter(sq);
    while (ecs_query_next(&it)) {
        table_count ++;
        entity_count += it.count;

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_has(world, it.entities[i], Position));
            test_assert(ecs_has(world, it.entities[i], Velocity));
        }
    }

    test_int(table_count, 1);
    test_int(entity_count, 1);

    /* Queries now no longer match */
    ecs_remove(world, parent, Position);

    /* Force rematching */
    ecs_progress(world, 0);

    /* Test if tables have been unmatched */
    it = ecs_query_iter(q);
    test_int(it.table_count, 0);

    it = ecs_query_iter(sq);
    test_int(it.table_count, 0);

    ecs_query_free(sq);

    ecs_fini(world);
}

void Queries_query_single_trait() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Trait);

    ECS_ENTITY(world, e1, TRAIT|Trait>Position);
    ECS_ENTITY(world, e2, TRAIT|Trait>Velocity);
    ECS_ENTITY(world, e3, TRAIT|Trait);
    ECS_ENTITY(world, e4, Position);
    ECS_ENTITY(world, e5, Velocity);

    int32_t table_count = 0, entity_count = 0;

    ecs_query_t *q = ecs_query_new(world, "TRAIT | Trait > Velocity");
    ecs_iter_t it = ecs_query_iter(q);
    while (ecs_query_next(&it)) {
        table_count ++;

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] == e2);
            entity_count ++;
        }
    }

    test_int(table_count, 1);
    test_int(entity_count, 1);

    ecs_fini(world);
}

void Queries_query_single_instanceof() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, BaseA, 0);
    ECS_ENTITY(world, BaseB, 0);
    ECS_ENTITY(world, e1, INSTANCEOF | BaseB);
    ECS_ENTITY(world, e2, INSTANCEOF | BaseA);

    int32_t table_count = 0, entity_count = 0;

    ecs_query_t *q = ecs_query_new(world, "INSTANCEOF | BaseA");
    ecs_iter_t it = ecs_query_iter(q);
    while (ecs_query_next(&it)) {
        table_count ++;

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] == e2);
            entity_count ++;
        }
    }

    test_int(table_count, 1);
    test_int(entity_count, 1);

    ecs_fini(world);
}

void Queries_query_single_childof() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, BaseA, 0);
    ECS_ENTITY(world, BaseB, 0);
    ECS_ENTITY(world, e1, CHILDOF | BaseB);
    ECS_ENTITY(world, e2, CHILDOF | BaseA);

    int32_t table_count = 0, entity_count = 0;

    ecs_query_t *q = ecs_query_new(world, "CHILDOF | BaseA");
    ecs_iter_t it = ecs_query_iter(q);
    while (ecs_query_next(&it)) {
        table_count ++;

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] == e2);
            entity_count ++;
        }
    }

    test_int(table_count, 1);
    test_int(entity_count, 1);

    ecs_fini(world);
}

void Queries_query_w_filter() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_new(world, Position);
    ecs_add(world, ecs_new(world, 
        Position), 
        Velocity);

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_filter_t f = {
        .include = ecs_type(Velocity)
    };

    ecs_iter_t it = ecs_query_iter(q);
    int32_t table_count = 0, entity_count = 0;
    while (ecs_query_next_w_filter(&it, &f)) {
        table_count ++;

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            ecs_entity_t e = it.entities[i];
            test_assert(ecs_has(world, e, Position));
            test_assert(ecs_has(world, e, Velocity));
            entity_count ++;
        }
    }

    test_int(table_count, 1);
    test_int(entity_count, 1);

    ecs_fini(world);
}

void Queries_query_optional_owned() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t base = ecs_new(world, Velocity);
    
    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_add_entity(world, e1, ECS_INSTANCEOF | base);

    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_add(world, e2, Velocity);

    ecs_entity_t e3 = ecs_new(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position, ?Velocity");
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(q);
    int32_t count = 0;
    
    while (ecs_query_next(&it)) {
        Position *p = ecs_column(&it, Position, 1);
        Velocity *v = ecs_column(&it, Velocity, 2);
        
        test_int(it.count, 1);
        test_assert(p != NULL);

        if (it.entities[0] == e1) {
            test_assert(v == NULL);
        } else if (it.entities[0] == e2) {
            test_assert(v != NULL);
        } else if (it.entities[0] == e3) {
            test_assert(v == NULL);
        }

        count ++;
    }

    test_int(count, 3);

    ecs_fini(world);
}

void Queries_query_optional_shared() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t base = ecs_new(world, Velocity);
    
    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_add_entity(world, e1, ECS_INSTANCEOF | base);

    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_add(world, e2, Velocity);

    ecs_entity_t e3 = ecs_new(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position, ?SHARED:Velocity");
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(q);
    int32_t count = 0;
    
    while (ecs_query_next(&it)) {
        Position *p = ecs_column(&it, Position, 1);
        Velocity *v = ecs_column(&it, Velocity, 2);
        
        test_int(it.count, 1);
        test_assert(p != NULL);
        
        if (it.entities[0] == e1) {
            test_assert(v != NULL);
        } else if (it.entities[0] == e2) {
            test_assert(v == NULL);
        } else if (it.entities[0] == e3) {
            test_assert(v == NULL);
        }

        count ++;
    }

    test_int(count, 3);

    ecs_fini(world);
}

void Queries_query_optional_shared_nested() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t base_base = ecs_new(world, Velocity);
    ecs_entity_t base = ecs_new(world, 0);
    ecs_add_entity(world, base, ECS_INSTANCEOF | base_base);
    
    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_add_entity(world, e1, ECS_INSTANCEOF | base);

    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_add(world, e2, Velocity);

    ecs_entity_t e3 = ecs_new(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position, ?SHARED:Velocity");
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(q);
    int32_t count = 0;
    
    while (ecs_query_next(&it)) {
        Position *p = ecs_column(&it, Position, 1);
        Velocity *v = ecs_column(&it, Velocity, 2);
        
        test_int(it.count, 1);
        test_assert(p != NULL);
        
        if (it.entities[0] == e1) {
            test_assert(v != NULL);
        } else if (it.entities[0] == e2) {
            test_assert(v == NULL);
        } else if (it.entities[0] == e3) {
            test_assert(v == NULL);
        }

        count ++;
    }

    test_int(count, 3);

    ecs_fini(world);
}

void Queries_query_optional_any() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t base = ecs_new(world, Velocity);
    
    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_add_entity(world, e1, ECS_INSTANCEOF | base);

    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_add(world, e2, Velocity);

    ecs_entity_t e3 = ecs_new(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position, ?ANY:Velocity");
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(q);
    int32_t count = 0;
    
    while (ecs_query_next(&it)) {
        Position *p = ecs_column(&it, Position, 1);
        Velocity *v = ecs_column(&it, Velocity, 2);
        
        test_int(it.count, 1);
        test_assert(p != NULL);
        
        if (it.entities[0] == e1) {
            test_assert(v != NULL);
            test_assert(!ecs_is_owned(&it, 2));
        } else if (it.entities[0] == e2) {
            test_assert(v != NULL);
            test_assert(ecs_is_owned(&it, 2));
        } else if (it.entities[0] == e3) {
            test_assert(v == NULL);
        }

        count ++;
    }

    test_int(count, 3);

    ecs_fini(world);
}

void Queries_query_rematch_optional_after_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t base = ecs_new(world, 0);
    
    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_add_entity(world, e1, ECS_INSTANCEOF | base);
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_add(world, e2, Velocity);
    ecs_entity_t e3 = ecs_new(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position, ?SHARED:Velocity");
    test_assert(q != NULL);

    /* First iteration, base doesn't have Velocity but query should match with
     * entity anyway since the component is optional */
    ecs_iter_t it = ecs_query_iter(q);
    int32_t count = 0;
    
    while (ecs_query_next(&it)) {
        Position *p = ecs_column(&it, Position, 1);
        Velocity *v = ecs_column(&it, Velocity, 2);
        
        test_int(it.count, 1);
        test_assert(p != NULL);
        
        if (it.entities[0] == e1) {
            test_assert(v == NULL);
        } else if (it.entities[0] == e2) {
            test_assert(v == NULL);
        } else if (it.entities[0] == e3) {
            test_assert(v == NULL);
        }

        count ++;
    }
    test_int(count, 3);

    /* Add Velocity to base, should trigger rematch */
    ecs_add(world, base, Velocity);

    /* Trigger a merge, which triggers the rematch */
    ecs_staging_begin(world);
    ecs_staging_end(world);

    /* Second iteration, base has Velocity and entity should be able to access
     * the shared component. */
    it = ecs_query_iter(q);
    count = 0;
    
    while (ecs_query_next(&it)) {
        Position *p = ecs_column(&it, Position, 1);
        Velocity *v = ecs_column(&it, Velocity, 2);
        
        test_int(it.count, 1);
        test_assert(p != NULL);
        
        if (it.entities[0] == e1) {
            test_assert(v != NULL);
        } else if (it.entities[0] == e2) {
            test_assert(v == NULL);
        } else if (it.entities[0] == e3) {
            test_assert(v == NULL);
        }

        count ++;
    }
    test_int(count, 3);    

    ecs_fini(world);    
}
