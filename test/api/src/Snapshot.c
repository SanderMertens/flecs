#include <api.h>

void Snapshot_simple_snapshot() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_snapshot_t *s = ecs_snapshot_take(world, NULL);

    Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p->x ++;
    p->y ++;

    ecs_snapshot_restore(world, s);

    test_assert(ecs_has(world, e, Position));
    p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);    

    ecs_fini(world);
}

void Snapshot_snapshot_after_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_snapshot_t *s = ecs_snapshot_take(world, NULL);

    ecs_entity_t e2 = ecs_new(world, Position);
    test_assert(e2 != 0);
    test_assert(ecs_has(world, e2, Position));

    ecs_snapshot_restore(world, s);

    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e2, Position));
    test_assert(ecs_new(world, 0) == e2);

    ecs_fini(world);
}

void Snapshot_snapshot_after_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_snapshot_t *s = ecs_snapshot_take(world, NULL);

    ecs_delete(world, e);
    test_assert(!ecs_has(world, e, Position));

    ecs_snapshot_restore(world, s);    

    test_assert(ecs_has(world, e, Position));
    Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Snapshot_snapshot_after_new_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_snapshot_t *s = ecs_snapshot_take(world, NULL);

    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_add(world, e2, Velocity);

    ecs_snapshot_restore(world, s);    

    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e2, Position));
    test_assert(!ecs_has(world, e2, Velocity));

    ecs_fini(world);
}

void Snapshot_snapshot_after_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_snapshot_t *s = ecs_snapshot_take(world, NULL);

    ecs_add(world, e, Velocity);
    test_assert(ecs_has(world, e, Velocity));

    ecs_snapshot_restore(world, s);

    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Snapshot_snapshot_after_remove() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_add(world, e, Velocity);
    test_assert(ecs_has(world, e, Velocity));

    ecs_snapshot_t *s = ecs_snapshot_take(world, NULL);

    ecs_remove(world, e, Velocity);
    test_assert(!ecs_has(world, e, Velocity));

    ecs_snapshot_restore(world, s);

    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void Snapshot_snapshot_w_include_filter() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    test_assert(e1 != 0);
    test_assert(ecs_has(world, e1, Position));

    ecs_entity_t e2 = ecs_set(world, 0, Position, {15, 25});
    test_assert(e2 != 0);
    ecs_add(world, e2, Velocity);
    test_assert(ecs_has(world, e2, Position));
    test_assert(ecs_has(world, e2, Velocity));

    ecs_entity_t e3 = ecs_set(world, 0, Velocity, {25, 35});
    test_assert(e3 != 0);
    test_assert(ecs_has(world, e3, Velocity));

    ecs_snapshot_t *s = ecs_snapshot_take(world, &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    Position *p = ecs_get_ptr(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p->x ++;
    p->y ++;

    p = ecs_get_ptr(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 15);
    test_int(p->y, 25);

    p->x ++;
    p->y ++;

    Velocity *v = ecs_get_ptr(world, e3, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 25); 
    test_int(v->y, 35);

    v->x ++;
    v->y ++;

    ecs_snapshot_restore(world, s);

    /* Restored */
    p = ecs_get_ptr(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    /* Restored */
    p = ecs_get_ptr(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 15);
    test_int(p->y, 25);

    /* Not restored */
    v = ecs_get_ptr(world, e3, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 26); 
    test_int(v->y, 36);

    test_assert(ecs_new(world, 0) > e3);

    ecs_fini(world);
}

void Snapshot_snapshot_w_exclude_filter() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    test_assert(e1 != 0);
    test_assert(ecs_has(world, e1, Position));

    ecs_entity_t e2 = ecs_set(world, 0, Position, {15, 25});
    test_assert(e2 != 0);
    ecs_add(world, e2, Velocity);
    test_assert(ecs_has(world, e2, Position));
    test_assert(ecs_has(world, e2, Velocity));

    ecs_entity_t e3 = ecs_set(world, 0, Velocity, {25, 35});
    test_assert(e3 != 0);
    test_assert(ecs_has(world, e3, Velocity));

    ecs_snapshot_t *s = ecs_snapshot_take(world, &(ecs_filter_t){
        .exclude = ecs_type(Position)
    });

    Position *p = ecs_get_ptr(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p->x ++;
    p->y ++;

    p = ecs_get_ptr(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 15);
    test_int(p->y, 25);

    p->x ++;
    p->y ++;

    Velocity *v = ecs_get_ptr(world, e3, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 25); 
    test_int(v->y, 35);

    v->x ++;
    v->y ++;

    ecs_snapshot_restore(world, s);

    /* Not restored */
    p = ecs_get_ptr(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 21);

    /* Not restored */
    p = ecs_get_ptr(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 16);
    test_int(p->y, 26);

    /* Restored */
    v = ecs_get_ptr(world, e3, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 25); 
    test_int(v->y, 35);

    test_assert(ecs_new(world, 0) > e3);

    ecs_fini(world);
}

void Snapshot_snapshot_w_filter_after_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_entity_t e1 = ecs_set(world, 0, Position, {1, 2});
    test_assert(e1 != 0);
    test_assert(ecs_has(world, e1, Position));

    ecs_entity_t e2 = ecs_set(world, 0, Velocity, {3, 4});
    test_assert(e2 != 0);
    test_assert(ecs_has(world, e2, Velocity));

    ecs_snapshot_t *s = ecs_snapshot_take(world, &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    ecs_set(world, e1, Position, {5, 6});
    ecs_set(world, e2, Velocity, {7, 8});

    ecs_entity_t e3 = ecs_set(world, 0, Position, {33, 44});
    test_assert(e3 != 0);
    test_assert(ecs_has(world, e3, Position));

    ecs_entity_t e4 = ecs_set(world, 0, Velocity, {33, 44});
    test_assert(e4 != 0);
    test_assert(ecs_has(world, e4, Velocity));

    ecs_snapshot_restore(world, s);

    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Velocity));
    test_assert(ecs_is_empty(world, e3));
    test_assert(ecs_has(world, e4, Velocity));

    

    ecs_fini(world);
}

void Snapshot_snapshot_w_filter_after_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_entity_t e1 = ecs_set(world, 0, Position, {1, 2});
    test_assert(e1 != 0);
    test_assert(ecs_has(world, e1, Position));

    ecs_entity_t e2 = ecs_set(world, 0, Velocity, {3, 4});
    test_assert(e2 != 0);
    test_assert(ecs_has(world, e2, Velocity));

    ecs_entity_t e3 = ecs_set(world, 0, Position, {1, 2});
    test_assert(e3 != 0);
    test_assert(ecs_has(world, e3, Position)); 

    ecs_entity_t e4 = ecs_set(world, 0, Velocity, {3, 4});
    test_assert(e4 != 0);
    test_assert(ecs_has(world, e4, Velocity));

    ecs_snapshot_t *s = ecs_snapshot_take(world, &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    ecs_delete(world, e3);
    ecs_delete(world, e4);

    test_assert( ecs_is_empty(world, e3));
    test_assert( ecs_is_empty(world, e4));

    ecs_snapshot_restore(world, s);

    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Velocity));
    test_assert(ecs_has(world, e3, Position));
    test_assert(ecs_is_empty(world, e4));

    ecs_fini(world);
}

void Snapshot_snapshot_free_empty() {
    ecs_world_t *world = ecs_init();

    ecs_snapshot_t *s = ecs_snapshot_take(world, NULL);
    test_assert(s != NULL);

    ecs_snapshot_free(world, s);

    ecs_fini(world);
}

void Snapshot_snapshot_free() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    test_assert( ecs_new(world, Position) != 0);
    test_assert( ecs_new(world, Velocity) != 0);

    ecs_snapshot_t *s = ecs_snapshot_take(world, NULL);
    test_assert(s != NULL);

    ecs_snapshot_free(world, s);

    ecs_fini(world);
}

void Snapshot_snapshot_free_filtered() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    test_assert( ecs_new(world, Position) != 0);
    test_assert( ecs_new(world, Velocity) != 0);

    ecs_snapshot_t *s = ecs_snapshot_take(world, &(ecs_filter_t){
        .include = ecs_type(Position)
    });
    test_assert(s != NULL);

    ecs_snapshot_free(world, s);

    ecs_fini(world);
}

static bool invoked = false;

static
void Dummy(ecs_rows_t *rows) {
    invoked = true;
}

void Snapshot_snapshot_activate_table_w_filter() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position);

    ecs_entity_t e = ecs_set(world, 0, Position, {0, 0});
    test_assert(e != 0);

    ecs_snapshot_t *s = ecs_snapshot_take(world, &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    ecs_snapshot_restore(world, s);

    test_assert(ecs_has(world, e, Position));
    
    ecs_progress(world, 0);
    test_bool(invoked, true);

    /* Cleanup */
    ecs_fini(world);
}

void Snapshot_snapshot_copy() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_snapshot_t *s = ecs_snapshot_take(world, NULL);
    ecs_snapshot_t *s_copy = ecs_snapshot_copy(world, s, NULL);
    ecs_snapshot_free(world, s);

    Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p->x ++;
    p->y ++;

    ecs_snapshot_restore(world, s_copy);

    test_assert(ecs_has(world, e, Position));
    p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);    

    ecs_fini(world);
}

void Snapshot_snapshot_copy_filtered() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    test_assert(e1 != 0);
    test_assert(ecs_has(world, e1, Position));

    ecs_entity_t e2 = ecs_set(world, 0, Velocity, {10, 20});
    test_assert(e2 != 0);
    test_assert(ecs_has(world, e2, Velocity));

    ecs_snapshot_t *s = ecs_snapshot_take(world, &(ecs_filter_t) {
        .include = ecs_type(Position)
    });

    ecs_snapshot_t *s_copy = ecs_snapshot_copy(world, s, NULL);
    ecs_snapshot_free(world, s);

    Position *p = ecs_get_ptr(world, e1, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p->x ++;
    p->y ++;

    ecs_delete(world, e2);

    ecs_snapshot_restore(world, s_copy);

    test_int( ecs_count(world, Position), 1);
    test_int( ecs_count(world, Velocity), 0);
    test_assert( !ecs_is_empty(world, e1));
    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_is_empty(world, e2));    

    p = ecs_get_ptr(world, e1, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);    

    ecs_fini(world);
}

void Snapshot_snapshot_copy_w_filter() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    test_assert(e1 != 0);
    test_assert(ecs_has(world, e1, Position));

    ecs_entity_t e2 = ecs_set(world, 0, Velocity, {10, 20});
    test_assert(e2 != 0);
    test_assert(ecs_has(world, e2, Velocity));

    ecs_snapshot_t *s = ecs_snapshot_take(world, NULL);
    ecs_snapshot_t *s_copy = ecs_snapshot_copy(world, s, &(ecs_filter_t) {
        .include = ecs_type(Position)
    });
    ecs_snapshot_free(world, s);

    Position *p = ecs_get_ptr(world, e1, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p->x ++;
    p->y ++;

    ecs_delete(world, e2);

    ecs_snapshot_restore(world, s_copy);

    test_int( ecs_count(world, Position), 1);
    test_int( ecs_count(world, Velocity), 0);
    test_assert( !ecs_is_empty(world, e1));
    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_is_empty(world, e2));    

    p = ecs_get_ptr(world, e1, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);    

    ecs_fini(world);
}
