#include <addons.h>

void Snapshot_simple_snapshot() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    Position *p = ecs_get_mut(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p->x ++;
    p->y ++;

    ecs_snapshot_restore(world, s);

    test_assert(ecs_has(world, e, Position));
    p = ecs_get_mut(world, e, Position);
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

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    ecs_entity_t e2 = ecs_new(world, Position);
    test_assert(e2 != 0);
    test_assert(ecs_has(world, e2, Position));

    ecs_snapshot_restore(world, s);

    test_assert(ecs_is_alive(world, e));
    test_assert(!ecs_is_alive(world, e2));

    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_new(world, 0) == e2);

    ecs_fini(world);
}

void Snapshot_snapshot_after_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));

    ecs_snapshot_restore(world, s);    

    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
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

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_add(world, e2, Velocity);

    ecs_snapshot_restore(world, s);    

    test_assert(ecs_is_alive(world, e));
    test_assert(!ecs_is_alive(world, e2));

    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void Snapshot_snapshot_after_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_snapshot_t *s = ecs_snapshot_take(world);

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

    ecs_snapshot_t *s = ecs_snapshot_take(world);

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

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ ecs_id(Position) }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);
    ecs_snapshot_t *s = ecs_snapshot_take_w_iter(&it);

    Position *p = ecs_get_mut(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p->x ++;
    p->y ++;

    p = ecs_get_mut(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 15);
    test_int(p->y, 25);

    p->x ++;
    p->y ++;

    Velocity *v = ecs_get_mut(world, e3, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 25); 
    test_int(v->y, 35);

    v->x ++;
    v->y ++;

    ecs_snapshot_restore(world, s);

    /* Restored */
    p = ecs_get_mut(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    /* Restored */
    p = ecs_get_mut(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 15);
    test_int(p->y, 25);

    /* Not restored */
    v = ecs_get_mut(world, e3, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 26); 
    test_int(v->y, 36);

    test_assert(ecs_new(world, 0) > e3);

    ecs_filter_fini(&f);

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

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ ecs_id(Position), .oper = EcsNot }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);
    ecs_snapshot_t *s = ecs_snapshot_take_w_iter(&it);

    Position *p = ecs_get_mut(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p->x ++;
    p->y ++;

    p = ecs_get_mut(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 15);
    test_int(p->y, 25);

    p->x ++;
    p->y ++;

    Velocity *v = ecs_get_mut(world, e3, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 25); 
    test_int(v->y, 35);

    v->x ++;
    v->y ++;

    ecs_snapshot_restore(world, s);

    /* Not restored */
    p = ecs_get_mut(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 21);

    /* Not restored */
    p = ecs_get_mut(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 16);
    test_int(p->y, 26);

    /* Restored */
    v = ecs_get_mut(world, e3, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 25); 
    test_int(v->y, 35);

    test_assert(ecs_new(world, 0) > e3);

    ecs_filter_fini(&f);

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

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ ecs_id(Position) }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);
    ecs_snapshot_t *s = ecs_snapshot_take_w_iter(&it);

    ecs_set(world, e1, Position, {5, 6});
    ecs_set(world, e2, Velocity, {7, 8});

    ecs_entity_t e3 = ecs_set(world, 0, Position, {33, 44});
    test_assert(e3 != 0);
    test_assert(ecs_has(world, e3, Position));

    ecs_entity_t e4 = ecs_set(world, 0, Velocity, {34, 45});
    test_assert(e4 != 0);
    test_assert(ecs_has(world, e4, Velocity));

    ecs_snapshot_restore(world, s);

    test_assert(ecs_has(world, e1, Position));
    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    test_assert(ecs_has(world, e2, Velocity));
    const Velocity *v = ecs_get(world, e2, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 7);
    test_int(v->y, 8);

    test_assert(ecs_has(world, e3, Position));
    p = ecs_get(world, e3, Position);
    test_assert(p != NULL);
    test_int(p->x, 33);
    test_int(p->y, 44);

    test_assert(ecs_has(world, e4, Velocity));
    v = ecs_get(world, e4, Velocity);
    test_assert(p != NULL);
    test_int(v->x, 34);
    test_int(v->y, 45);

    ecs_filter_fini(&f);

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

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ ecs_id(Position) }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);
    ecs_snapshot_t *s = ecs_snapshot_take_w_iter(&it);

    ecs_delete(world, e3);
    ecs_delete(world, e4);

    test_assert(!ecs_is_alive(world, e3));
    test_assert(!ecs_is_alive(world, e4));

    ecs_snapshot_restore(world, s);

    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_is_alive(world, e2));
    test_assert(ecs_is_alive(world, e3));
    test_assert(!ecs_is_alive(world, e4));

    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Velocity));
    test_assert(ecs_has(world, e3, Position));

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Snapshot_snapshot_free_empty() {
    ecs_world_t *world = ecs_init();

    ecs_snapshot_t *s = ecs_snapshot_take(world);
    test_assert(s != NULL);

    ecs_snapshot_free(s);

    ecs_fini(world);
}

void Snapshot_snapshot_free() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    test_assert( ecs_new(world, Position) != 0);
    test_assert( ecs_new(world, Velocity) != 0);

    ecs_snapshot_t *s = ecs_snapshot_take(world);
    test_assert(s != NULL);

    ecs_snapshot_free(s);

    ecs_fini(world);
}

void Snapshot_snapshot_free_filtered() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    test_assert( ecs_new(world, Position) != 0);
    test_assert( ecs_new(world, Velocity) != 0);

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ ecs_id(Position) }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);
    ecs_snapshot_t *s = ecs_snapshot_take_w_iter(&it);
    test_assert(s != NULL);

    ecs_snapshot_free(s);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Snapshot_snapshot_free_filtered_w_dtor() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "e1"
    });

    ecs_entity_t e2 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "e2"
    });

    ecs_entity_t e3 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "e3"
    });

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);
    ecs_add(world, e3, Position);

    ecs_add(world, e1, Velocity);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e3, Velocity);

    ecs_add(world, e3, Mass);

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ ecs_id(Position) }, { ecs_id(Velocity) }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);
    ecs_snapshot_t *s = ecs_snapshot_take_w_iter(&it);
    test_assert(s != NULL);

    ecs_snapshot_free(s);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

static bool invoked = false;

static
void Dummy(ecs_iter_t *it) {
    invoked = true;
}

void Snapshot_snapshot_activate_table_w_filter() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position);

    ecs_entity_t e = ecs_set(world, 0, Position, {0, 0});
    test_assert(e != 0);

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ ecs_id(Position) }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);
    ecs_snapshot_t *s = ecs_snapshot_take_w_iter(&it);

    ecs_snapshot_restore(world, s);

    test_assert(ecs_has(world, e, Position));
    
    ecs_progress(world, 0);
    test_bool(invoked, true);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Snapshot_snapshot_copy() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_snapshot_t *s = ecs_snapshot_take(world);
    ecs_iter_t it = ecs_snapshot_iter(s);
    ecs_snapshot_t *s_copy = ecs_snapshot_take_w_iter(&it);
    ecs_snapshot_free(s);

    Position *p = ecs_get_mut(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p->x ++;
    p->y ++;

    ecs_snapshot_restore(world, s_copy);

    test_assert(ecs_has(world, e, Position));
    p = ecs_get_mut(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);    

    ecs_fini(world);
}

void Snapshot_snapshot_get_ref_after_restore() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_ref_t ref = ecs_ref_init(world, e, Position);
    const Position *p = ecs_ref_get(world, &ref, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    Position *p_mut = ecs_get_mut(world, e, Position);
    test_int(p_mut->x, 10);
    test_int(p_mut->y, 20);

    p_mut->x ++;
    p_mut->y ++;

    ecs_snapshot_restore(world, s);

    test_assert(ecs_has(world, e, Position));
    p = ecs_ref_get(world, &ref, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Snapshot_new_after_snapshot() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    ecs_snapshot_restore(world, s);

    ecs_entity_t e2 = ecs_new(world, Position);
    test_assert(e2 != 0);
    test_assert(ecs_has(world, e2, Position));

    ecs_add(world, e2, Velocity);
    test_assert(ecs_has(world, e2, Velocity));

    ecs_fini(world);
}

void Snapshot_add_after_snapshot() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    ecs_snapshot_restore(world, s);

    ecs_add(world, e, Velocity);
    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Snapshot_delete_after_snapshot() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    ecs_snapshot_restore(world, s);

    ecs_delete(world, e);

    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void Snapshot_new_empty_after_snapshot() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    ecs_snapshot_restore(world, s);

    ecs_entity_t e2 = ecs_new(world, 0);
    test_assert(e2 != 0);

    ecs_fini(world);
}

void Snapshot_set_after_snapshot() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    ecs_snapshot_restore(world, s);

    ecs_entity_t e2 = ecs_new(world, 0);
    test_assert(e2 != 0);

    ecs_set_name(world, e2, "e2");
    test_assert(ecs_has_pair(world, e2, ecs_id(EcsIdentifier), EcsName));
    test_str(ecs_get_name(world, e2), "e2");

    ecs_fini(world);
}

void Snapshot_restore_recycled() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    ecs_delete(world, e);

    ecs_snapshot_restore(world, s);

    ecs_entity_t e2 = ecs_new(world, 0);
    test_assert(e2 != 0);
    test_assert(e != e2);

    ecs_fini(world);
}

static
void SetP(ecs_iter_t *it) {
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_set_name(it->world, 0, "e2");
    }
}

void Snapshot_snapshot_w_new_in_onset() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ECS_OBSERVER(world, SetP, EcsOnSet, Position);

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    ecs_snapshot_restore(world, s);

    ecs_entity_t e2 = ecs_lookup(world, "e2");
    test_assert(e2 != 0);

    ecs_entity_t e3 = ecs_set_name(world, 0, "e3");
    test_assert(e3 != 0);
    test_assert(e3 > e2);
    test_str(ecs_get_name(world, e3), "e3");

    ecs_fini(world);
}

static ecs_entity_t v_entity = 0;

static
void CreateV(ecs_iter_t *it) {
    ecs_entity_t ecs_id(Velocity) = ecs_field_id(it, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        v_entity = ecs_set(it->world, 0, Velocity, {3, 4});
    }    
}

void Snapshot_snapshot_w_new_in_onset_in_snapshot_table() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Velocity, {1, 2});

    ECS_OBSERVER(world, CreateV, EcsOnSet, Position, Velocity());

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    ecs_snapshot_restore(world, s);

    const Velocity *v = ecs_get(world, e2, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 2);

    v = ecs_get(world, v_entity, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 3);
    test_int(v->y, 4);

    ecs_fini(world);
}

void Snapshot_snapshot_from_stage() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_entity_t e1 = ecs_set(world, 0, Position, {1, 2});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {3, 4});

    ecs_readonly_begin(world);

    ecs_world_t *stage = ecs_get_stage(world, 0);

    ecs_snapshot_t *s = ecs_snapshot_take(stage);

    ecs_delete(stage, e1);
    ecs_delete(stage, e2);

    ecs_readonly_end(world);

    test_assert(!ecs_is_alive(world, e1));
    test_assert(!ecs_is_alive(world, e2));

    ecs_snapshot_restore(world, s);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);

    ecs_fini(world);
}
