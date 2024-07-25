#include <core.h>

void Commands_defer_new(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_defer_begin(world);
    
    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);

    test_assert(!ecs_has(world, e, Position));

    ecs_defer_end(world);

    test_assert(!ecs_has(world, e, Position));    

    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Position));

    ecs_frame_end(world);

    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void Commands_defer_bulk_new(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_defer_begin(world);
    
    const ecs_entity_t *temp_ids = ecs_bulk_new(world, Position, 3);
    ecs_entity_t ids[3];
    memcpy(ids, temp_ids, sizeof(ecs_entity_t) * 3);

    test_assert(!ecs_has(world, ids[0], Position));
    test_assert(!ecs_has(world, ids[1], Position));
    test_assert(!ecs_has(world, ids[2], Position));   

    ecs_defer_end(world);

    test_assert(!ecs_has(world, ids[0], Position));
    test_assert(!ecs_has(world, ids[1], Position));
    test_assert(!ecs_has(world, ids[2], Position));

    ecs_defer_end(world);

    test_assert(ecs_has(world, ids[0], Position));
    test_assert(ecs_has(world, ids[1], Position));
    test_assert(ecs_has(world, ids[2], Position));  

    ecs_frame_end(world);

    test_assert(ecs_has(world, ids[0], Position));
    test_assert(ecs_has(world, ids[1], Position));
    test_assert(ecs_has(world, ids[2], Position));

    ecs_fini(world);
}

void Commands_defer_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_defer_begin(world);
    
    ecs_add(world, e, Position);

    test_assert(!ecs_has(world, e, Position));

    ecs_defer_end(world);

    test_assert(!ecs_has(world, e, Position));    

    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Position));

    ecs_frame_end(world);

    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void Commands_defer_add_two(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_defer_begin(world);
    
    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);

    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));

    ecs_defer_end(world);

    test_assert(!ecs_has(world, e, Position));    
    test_assert(!ecs_has(world, e, Velocity));

    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_frame_end(world);

    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Commands_defer_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_defer_begin(world);
    
    ecs_remove(world, e, Position);

    test_assert(ecs_has(world, e, Position));

    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Position));    

    ecs_defer_end(world);

    test_assert(!ecs_has(world, e, Position));

    ecs_frame_end(world);

    test_assert(!ecs_has(world, e, Position));

    ecs_fini(world);
}

void Commands_defer_remove_two(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_defer_begin(world);
    
    ecs_remove(world, e, Position);
    ecs_remove(world, e, Velocity);

    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Position));   
    test_assert(ecs_has(world, e, Velocity)); 

    ecs_defer_end(world);

    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));

    ecs_frame_end(world);

    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Commands_defer_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w(world, Position);
    ecs_set(world, e, Position, {1, 2});

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_defer_begin(world);
    
    /* set will not be deferred if the component exists */
    ecs_set(world, e, Position, {3, 4});

    test_assert(ecs_has(world, e, Position));
    p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);

    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Position));  
    p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);  

    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Position));
    p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);

    ecs_frame_end(world);

    test_assert(ecs_has(world, e, Position));
    p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);

    ecs_fini(world);
}

void Commands_defer_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_defer_begin(world);
    
    ecs_delete(world, e);

    test_assert(ecs_is_alive(world, e));

    ecs_defer_end(world);

    test_assert(ecs_is_alive(world, e));  

    ecs_defer_end(world);

    test_assert(!ecs_is_alive(world, e));

    ecs_frame_end(world);

    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void Commands_defer_twice(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_defer_begin(world);
    ecs_defer_begin(world);

    ecs_set(world, e, Velocity, {1, 2});

    ecs_defer_end(world);
    test_assert(!ecs_has(world, e, Velocity));
    ecs_defer_end(world);
    test_assert(ecs_has(world, e, Velocity));

    const Velocity *v = ecs_get(world, e, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 2);

    ecs_fini(world);
}

void Commands_defer_twice_in_progress(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_frame_begin(world, 0);
    ecs_readonly_begin(world, false);
    ecs_world_t *stage = ecs_get_stage(world, 0);

    ecs_defer_begin(stage);
    ecs_defer_begin(stage);

    ecs_set(stage, e, Velocity, {1, 2});

    ecs_defer_end(stage);
    test_assert(!ecs_has(stage, e, Velocity));
    ecs_defer_end(stage);
    test_assert(!ecs_has(stage, e, Velocity));

    ecs_readonly_end(world);
    test_assert(ecs_has(world, e, Velocity));
    ecs_frame_end(world);
    test_assert(ecs_has(world, e, Velocity));

    const Velocity *v = ecs_get(world, e, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 2);

    ecs_fini(world);
}

static
void AddVelocity(ecs_iter_t *it) {
    ecs_id_t ecs_id(Velocity) = ecs_field_id(it, 1);

    ecs_defer_begin(it->world);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add(it->world, it->entities[i], Velocity);
    }

    ecs_defer_end(it->world);
}

void Commands_run_w_defer(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, AddVelocity, 0, Position, [in] Velocity());

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_run(world, AddVelocity, 0, NULL);

    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Commands_system_in_progress_w_defer(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, AddVelocity, EcsOnUpdate, Position, [in] Velocity());

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_progress(world, 0);

    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Commands_defer_ensure(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);
    {
        Position *p = ecs_ensure(world, e, Position);
        test_assert(p != NULL);
        p->x = 10;
        p->y = 20;
    }
    ecs_defer_end(world);

    {
        Position *p = ecs_get_mut(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

static bool on_set_invoked = 0;

static
void OnSetTestInvoked(ecs_iter_t *it) {
    on_set_invoked = 1;
}

void Commands_defer_ensure_no_modify(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_OBSERVER(world, OnSetTestInvoked, EcsOnSet, Velocity);

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_defer_begin(world);

    Velocity *v = ecs_ensure(world, e, Velocity);
    v->x = 1;
    v->y = 2;

    test_assert(!on_set_invoked);

    ecs_defer_end(world);

    test_assert(!on_set_invoked);

    test_assert(ecs_has(world, e, Velocity));
    const Velocity *vptr = ecs_get(world, e, Velocity);
    test_int(vptr->x, 1);
    test_int(vptr->y, 2);

    ecs_fini(world);
}

void Commands_defer_ensure_w_modify(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_OBSERVER(world, OnSetTestInvoked, EcsOnSet, Velocity);

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_defer_begin(world);

    Velocity *v = ecs_ensure(world, e, Velocity);
    v->x = 1;
    v->y = 2;
    test_assert(!on_set_invoked);

    ecs_modified(world, e, Velocity);

    test_assert(!on_set_invoked);

    ecs_defer_end(world);

    test_assert(on_set_invoked);

    test_assert(ecs_has(world, e, Velocity));
    const Velocity *vptr = ecs_get(world, e, Velocity);
    test_int(vptr->x, 1);
    test_int(vptr->y, 2);

    ecs_fini(world);
}

void Commands_defer_modify(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Velocity);

    ECS_OBSERVER(world, OnSetTestInvoked, EcsOnSet, Velocity);

    ecs_entity_t e = ecs_new_w(world, Velocity);

    ecs_defer_begin(world);

    ecs_modified(world, e, Velocity);

    test_assert(!on_set_invoked);

    ecs_defer_end(world);

    test_assert(on_set_invoked);

    ecs_fini(world);
}

void Commands_defer_set_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);

    ecs_set_pair(world, e, Velocity, ecs_id(Position), {1, 2});

    ecs_defer_end(world);

    test_assert(ecs_has_pair(world, e, ecs_id(Velocity), ecs_id(Position)));

    ecs_fini(world);    
}

void Commands_defer_clear(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_defer_begin(world);
    
    ecs_clear(world, e);

    test_assert(ecs_has(world, e, Position));

    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Position));

    ecs_defer_end(world);

    test_assert(!ecs_has(world, e, Position));

    ecs_frame_end(world);

    test_assert(!ecs_has(world, e, Position));

    ecs_fini(world);    
}

void Commands_defer_add_after_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_delete(world, e);
    ecs_add(world, e, Velocity);

    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    test_assert(ecs_is_alive(world, e));

    ecs_defer_end(world);

    test_assert(!ecs_is_alive(world, e));

    ecs_frame_end(world);

    test_assert(!ecs_is_alive(world, e));

    test_int(ecs_count(world, Position), 0);
    test_int(ecs_count(world, Velocity), 0);

    ecs_fini(world);  
}

void Commands_defer_set_after_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_delete(world, e);
    ecs_set(world, e, Velocity, {1, 2});

    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    test_assert(ecs_is_alive(world, e));

    ecs_defer_end(world);

    test_assert(!ecs_is_alive(world, e));

    ecs_frame_end(world);

    test_assert(!ecs_is_alive(world, e));

    test_int(ecs_count(world, Position), 0);
    test_int(ecs_count(world, Velocity), 0);

    ecs_fini(world);  
}

void Commands_defer_ensure_after_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_delete(world, e);
    Velocity *v = ecs_ensure(world, e, Velocity);
    v->x = 1;
    v->y = 2;

    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    test_assert(ecs_is_alive(world, e));

    ecs_defer_end(world);

    test_assert(!ecs_is_alive(world, e));

    ecs_frame_end(world);

    test_assert(!ecs_is_alive(world, e));

    test_int(ecs_count(world, Position), 0);
    test_int(ecs_count(world, Velocity), 0);

    ecs_fini(world);  
}

void Commands_defer_ensure_after_delete_2nd_to_last(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new_w(world, Position);

    /* Create 2nd position. This will cause deletion of the entity in the sparse
     * set to take a different path since it's not the last. */
    ecs_new_w(world, Position);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_delete(world, e);
    Velocity *v = ecs_ensure(world, e, Velocity);
    v->x = 1;
    v->y = 2;

    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    test_assert(ecs_is_alive(world, e));

    ecs_defer_end(world);

    test_assert(!ecs_is_alive(world, e));

    ecs_frame_end(world);

    test_assert(!ecs_is_alive(world, e));

    test_int(ecs_count(world, Position), 1);
    test_int(ecs_count(world, Velocity), 0);

    ecs_fini(world);  
}

void Commands_defer_add_child_to_deleted_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t parent = ecs_new(world);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_add(world, child, Velocity);
    ecs_delete(world, parent);

    ecs_defer_end(world);  

    ecs_frame_end(world);    

    test_assert(!ecs_is_alive(world, parent));
    test_assert(!ecs_is_alive(world, child));

    ecs_fini(world);    
}

void Commands_recreate_deleted_entity_while_deferred(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e_old = ecs_new(world);
    test_assert(e_old != 0);

    ecs_delete(world, e_old);
    test_assert(!ecs_is_alive(world, e_old));

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_defer_begin(world);
    
    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);
    test_assert(e != e_old);
    test_assert((e & ECS_ENTITY_MASK) == (e_old & ECS_ENTITY_MASK));

    test_assert(!ecs_has(world, e, Position));

    ecs_defer_end(world);

    test_assert(!ecs_has(world, e, Position));    

    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Position));

    ecs_frame_end(world);

    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void Commands_defer_add_to_recycled_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t id = ecs_new(world);
    test_assert(id != 0);

    ecs_delete(world, id);

    ecs_entity_t id_2 = ecs_new(world);
    test_assert(id != id_2);
    test_assert((int32_t)id == (int32_t)id_2);
    
    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_entity_t child = ecs_new_w_id(world, id_2);
    ecs_add(world, child, Velocity);

    ecs_defer_end(world);  

    ecs_frame_end(world);    

    test_assert(!ecs_is_alive(world, id));
    test_assert(ecs_is_alive(world, id_2));
    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_has_id(world, child, id_2));

    ecs_fini(world); 
}

void Commands_defer_add_to_recycled_id_w_role(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t id = ecs_new(world);
    test_assert(id != 0);

    ecs_delete(world, id);

    ecs_entity_t id_2 = ecs_new(world);
    test_assert(id != id_2);
    test_assert((int32_t)id == (int32_t)id_2);
    
    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_entity_t child = ecs_new_w_id(world, ECS_TOGGLE | id_2);
    ecs_add(world, child, Velocity);

    ecs_defer_end(world);  

    ecs_frame_end(world);    

    test_assert(!ecs_is_alive(world, id));
    test_assert(ecs_is_alive(world, id_2));
    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_has_id(world, child, ECS_TOGGLE | id_2));

    ecs_fini(world); 
}

void Commands_defer_add_to_recycled_relation(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t target = ecs_new(world);
    test_assert(target != 0);

    ecs_entity_t rel = ecs_new(world);
    test_assert(rel != 0);

    ecs_delete(world, rel);

    ecs_entity_t rel_2 = ecs_new(world);
    test_assert(rel != rel_2);
    test_assert((int32_t)rel == (int32_t)rel_2);
    
    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_entity_t child = ecs_new_w_pair(world, rel_2, target);
    ecs_add(world, child, Velocity);

    ecs_defer_end(world);  

    ecs_frame_end(world);    

    test_assert(ecs_is_alive(world, target));
    test_assert(!ecs_is_alive(world, rel));
    test_assert(ecs_is_alive(world, rel_2));
    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_has_pair(world, child, rel_2, target));

    ecs_fini(world); 
}

void Commands_defer_add_to_recycled_object(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Rel);

    ecs_entity_t target = ecs_new(world);
    test_assert(target != 0);

    ecs_delete(world, target);

    ecs_entity_t object_2 = ecs_new(world);
    test_assert(target != object_2);
    test_assert((int32_t)target == (int32_t)object_2);
    
    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_entity_t child = ecs_new_w_pair(world, Rel, object_2);
    ecs_add(world, child, Velocity);

    ecs_defer_end(world);  

    ecs_frame_end(world);    

    test_assert(!ecs_is_alive(world, target));
    test_assert(ecs_is_alive(world, object_2));
    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_has_pair(world, child, Rel, object_2));

    ecs_fini(world); 
}

void Commands_defer_add_to_recycled_object_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t parent = ecs_new(world);
    test_assert(parent != 0);

    ecs_delete(world, parent);

    ecs_entity_t parent_2 = ecs_new(world);
    test_assert(parent != parent_2);
    test_assert((int32_t)parent == (int32_t)parent_2);
    
    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent_2);
    ecs_add(world, child, Velocity);

    ecs_defer_end(world);  

    ecs_frame_end(world);    

    test_assert(!ecs_is_alive(world, parent));
    test_assert(ecs_is_alive(world, parent_2));
    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_has_pair(world, child, EcsChildOf, parent_2));

    ecs_fini(world); 
}

void Commands_defer_add_to_deleted_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t id = ecs_new(world);
    test_assert(id != 0);
    
    ecs_frame_begin(world, 1);
    ecs_defer_begin(world);

    ecs_delete(world, id);
    ecs_entity_t child = ecs_new_w_id(world, id);
    ecs_add(world, child, Velocity);

    ecs_defer_end(world);
    ecs_frame_end(world);    

    test_assert(!ecs_is_alive(world, id));
    test_assert(ecs_is_alive(world, child));
    test_assert(!ecs_has_id(world, child, id));

    ecs_fini(world); 
}

void Commands_defer_add_to_deleted_id_w_role(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t id = ecs_new(world);
    test_assert(id != 0);
    
    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_delete(world, id);
    ecs_entity_t child = ecs_new_w_id(world, ECS_TOGGLE | id);
    ecs_add(world, child, Velocity);

    ecs_defer_end(world);  

    ecs_frame_end(world);    

    test_assert(!ecs_is_alive(world, id));
    test_assert(ecs_is_alive(world, child));
    test_assert(!ecs_has_id(world, child, ECS_TOGGLE | id));

    ecs_fini(world); 
}

void Commands_defer_add_to_deleted_relation(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t target = ecs_new(world);
    test_assert(target != 0);

    ecs_entity_t rel = ecs_new(world);
    test_assert(rel != 0);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_delete(world, rel);
    ecs_entity_t child = ecs_new_w_pair(world, rel, target);
    ecs_add(world, child, Velocity);

    ecs_defer_end(world);  

    ecs_frame_end(world);    

    test_assert(ecs_is_alive(world, target));
    test_assert(!ecs_is_alive(world, rel));
    test_assert(ecs_is_alive(world, child));
    test_assert(!ecs_has_pair(world, child, rel, target));

    ecs_fini(world); 
}

void Commands_defer_add_to_deleted_object(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TAG(world, Rel);

    ecs_entity_t target = ecs_new(world);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_delete(world, target);
    ecs_entity_t child = ecs_new_w_pair(world, Rel, target);
    ecs_add(world, child, Velocity);

    ecs_defer_end(world);  

    ecs_frame_end(world);    

    test_assert(!ecs_is_alive(world, target));
    test_assert(ecs_is_alive(world, child));
    test_assert(!ecs_has_pair(world, child, Rel, target));

    ecs_fini(world); 
}

void Commands_defer_add_to_deleted_object_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t parent = ecs_new(world);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_delete(world, parent);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_add(world, child, Velocity);

    ecs_defer_end(world);  

    ecs_frame_end(world);    

    test_assert(!ecs_is_alive(world, parent));
    test_assert(!ecs_is_alive(world, child));

    ecs_fini(world); 
}

void Commands_defer_delete_added_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t id = ecs_new(world);
    test_assert(id != 0);
    
    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_entity_t child = ecs_new_w_id(world, id);
    ecs_add(world, child, Velocity);
    ecs_delete(world, id);

    ecs_defer_end(world);  

    ecs_frame_end(world);

    test_assert(!ecs_is_alive(world, id));
    test_assert(ecs_is_alive(world, child));
    test_assert(!ecs_has_id(world, child, id));

    ecs_fini(world); 
}

void Commands_defer_delete_added_id_w_role(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t id = ecs_new(world);
    test_assert(id != 0);
    
    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_entity_t child = ecs_new_w_id(world, ECS_TOGGLE | id);
    ecs_add(world, child, Velocity);
    ecs_delete(world, id);

    ecs_defer_end(world);  

    ecs_frame_end(world);    

    test_assert(!ecs_is_alive(world, id));
    test_assert(ecs_is_alive(world, child));
    test_assert(!ecs_has_id(world, child, ECS_TOGGLE | id));

    ecs_fini(world);
}

void Commands_defer_delete_added_relation(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t target = ecs_new(world);
    test_assert(target != 0);

    ecs_entity_t rel = ecs_new(world);
    test_assert(rel != 0);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_entity_t child = ecs_new_w_pair(world, rel, target);
    ecs_add(world, child, Velocity);
    ecs_delete(world, rel);

    ecs_defer_end(world);

    ecs_frame_end(world);    

    test_assert(ecs_is_alive(world, target));
    test_assert(!ecs_is_alive(world, rel));
    test_assert(ecs_is_alive(world, child));
    test_assert(!ecs_has_pair(world, child, rel, target));

    ecs_fini(world); 
}

void Commands_defer_delete_added_object(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TAG(world, Rel);

    ecs_entity_t target = ecs_new(world);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_entity_t child = ecs_new_w_pair(world, Rel, target);
    ecs_add(world, child, Velocity);
    ecs_delete(world, target);

    ecs_defer_end(world);  

    ecs_frame_end(world);    

    test_assert(!ecs_is_alive(world, target));
    test_assert(ecs_is_alive(world, child));
    test_assert(!ecs_has_pair(world, child, Rel, target));

    ecs_fini(world);
}

void Commands_defer_delete_added_object_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t parent = ecs_new(world);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_add(world, child, Velocity);
    ecs_delete(world, parent);

    ecs_defer_end(world);  

    ecs_frame_end(world);    

    test_assert(!ecs_is_alive(world, parent));
    test_assert(!ecs_is_alive(world, child));

    ecs_fini(world); 
}

void Commands_discard_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_defer_begin(world);

    ecs_delete(world, e);
    test_assert(ecs_is_alive(world, e));
    
    ecs_add(world, e, Position);

    test_assert(ecs_is_alive(world, e));
    test_assert(!ecs_has(world, e, Position));

    ecs_defer_end(world);

    test_assert(ecs_is_alive(world, e));
    test_assert(!ecs_has(world, e, Position));    

    ecs_defer_end(world);

    test_assert(!ecs_is_alive(world, e));

    ecs_frame_end(world);

    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void Commands_discard_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_defer_begin(world);

    ecs_delete(world, e);
    test_assert(ecs_is_alive(world, e));
    
    ecs_remove(world, e, Position);

    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_has(world, e, Position));

    ecs_defer_end(world);

    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_has(world, e, Position));    

    ecs_defer_end(world);

    test_assert(!ecs_is_alive(world, e));

    ecs_frame_end(world);

    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void Commands_discard_add_two(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_defer_begin(world);

    ecs_delete(world, e);
    test_assert(ecs_is_alive(world, e));
    
    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);

    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    test_assert(ecs_is_alive(world, e));

    ecs_defer_end(world);

    test_assert(!ecs_has(world, e, Position));    
    test_assert(!ecs_has(world, e, Velocity));
    test_assert(ecs_is_alive(world, e));

    ecs_defer_end(world);

    test_assert(!ecs_is_alive(world, e));

    ecs_frame_end(world);

    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void Commands_discard_remove_two(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_defer_begin(world);

    ecs_delete(world, e);
    test_assert(ecs_is_alive(world, e));
    
    ecs_remove(world, e, Position);
    ecs_remove(world, e, Velocity);

    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_defer_end(world);

    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_has(world, e, Position));    
    test_assert(ecs_has(world, e, Velocity));

    ecs_defer_end(world);

    test_assert(!ecs_is_alive(world, e));

    ecs_frame_end(world);

    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void Commands_discard_child(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_defer_begin(world);
    
    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);
    test_assert(!ecs_has(world, e, Position));
    ecs_delete(world, e);
    test_assert(ecs_is_alive(world, e));    

    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, e);
    test_assert(child != 0);
    test_assert(!ecs_has_pair(world, child, EcsChildOf, e));

    ecs_defer_end(world);

    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_is_alive(world, child));
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has_pair(world, child, EcsChildOf, e));

    ecs_defer_end(world);

    test_assert(!ecs_is_alive(world, e));
    test_assert(!ecs_is_alive(world, child));

    ecs_frame_end(world);

    test_assert(!ecs_is_alive(world, e));
    test_assert(!ecs_is_alive(world, child));

    ecs_fini(world);
}

void Commands_discard_child_w_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_defer_begin(world);
    
    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);
    test_assert(!ecs_has(world, e, Position));
    ecs_delete(world, e);
    test_assert(ecs_is_alive(world, e));    

    ecs_entity_t child = ecs_new(world);
    test_assert(child != 0);

    ecs_add_pair(world, child, EcsChildOf, e);
    test_assert(!ecs_has_pair(world, child, EcsChildOf, e));

    ecs_defer_end(world);

    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_is_alive(world, child));

    ecs_defer_end(world);

    test_assert(!ecs_is_alive(world, e));
    test_assert(!ecs_is_alive(world, child));

    ecs_frame_end(world);

    test_assert(!ecs_is_alive(world, e));
    test_assert(!ecs_is_alive(world, child));

    ecs_fini(world);
}

void Commands_defer_return_value(void) {
    ecs_world_t *world = ecs_mini();

    test_bool(ecs_defer_begin(world), true);

    test_bool(ecs_defer_begin(world), false);

    test_bool(ecs_defer_end(world), false);

    test_bool(ecs_defer_end(world), true);

    ecs_fini(world);
}

void Commands_defer_ensure_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Pair);

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);

    Position *p = ecs_ensure_pair(world, e, Position, Pair);
    test_assert(p != NULL);
    p->x = 10;
    p->y = 20;

    ecs_defer_end(world);

    const Position *pc = ecs_get_pair(world, e, Position, Pair);
    test_assert(pc != NULL);
    test_int(pc->x, 10);
    test_int(pc->y, 20);

    ecs_fini(world);
}

void Commands_async_stage_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);

    ecs_world_t *async = ecs_stage_new(world);
    ecs_add(async, e, Position);
    test_assert(!ecs_has(world, e, Position));
    ecs_merge(async);
    test_assert(ecs_has(world, e, Position));

    ecs_stage_free(async);

    ecs_fini(world);
}

void Commands_async_stage_add_twice(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world);

    ecs_world_t *async = ecs_stage_new(world);
    ecs_add(async, e, Position);
    test_assert(!ecs_has(world, e, Position));
    ecs_merge(async);
    test_assert(ecs_has(world, e, Position));

    ecs_add(async, e, Velocity);
    test_assert(!ecs_has(world, e, Velocity));
    ecs_merge(async);
    test_assert(ecs_has(world, e, Velocity));

    ecs_stage_free(async);

    ecs_fini(world);
}


void Commands_async_stage_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_world_t *async = ecs_stage_new(world);
    ecs_remove(async, e, Position);
    test_assert(ecs_has(world, e, Position));
    ecs_merge(async);
    test_assert(!ecs_has(world, e, Position));

    ecs_stage_free(async);

    ecs_fini(world);
}

void Commands_async_stage_clear(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new_w(world, Position);
    ecs_add(world, e, Velocity);

    ecs_world_t *async = ecs_stage_new(world);
    ecs_clear(async, e);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    ecs_merge(async);
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));

    ecs_stage_free(async);

    ecs_fini(world);
}

void Commands_async_stage_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new_w(world, Position);
    ecs_add(world, e, Velocity);

    ecs_world_t *async = ecs_stage_new(world);
    ecs_delete(async, e);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(ecs_is_alive(world, e));
    ecs_merge(async);
    test_assert(!ecs_is_alive(world, e));

    ecs_stage_free(async);

    ecs_fini(world);
}

void Commands_async_stage_new(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_world_t *async = ecs_stage_new(world);
    ecs_entity_t e = ecs_new(async);
    ecs_add(async, e, Position);
    ecs_add(async, e, Velocity);
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    ecs_merge(async);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity)); 
    test_assert(ecs_is_alive(world, e));

    ecs_stage_free(async);

    ecs_fini(world);
}

void Commands_async_stage_readonly(void) {
    ecs_world_t *world = ecs_mini();

    ecs_world_t *async = ecs_stage_new(world);
    test_assert(!ecs_stage_is_readonly(async));

    ecs_stage_free(async);
    ecs_fini(world);
}

static
void RegisterComponent(ecs_iter_t *it) {
    ecs_entity_t ecs_id(Position) = ecs_component_init(it->world, 
        &(ecs_component_desc_t){
            .entity = ecs_entity(it->world, {.name = "Position"}),
            .type.size = ECS_SIZEOF(Position),
            .type.alignment = ECS_ALIGNOF(Position)
        });    

    test_assert(ecs_id(Position) != 0);
}

void Commands_register_component_while_in_progress(void) {
    ecs_world_t *world = ecs_init();

    ECS_SYSTEM(world, RegisterComponent, EcsOnUpdate, 0);

    ecs_progress(world, 0);

    ecs_entity_t ecs_id(Position) = ecs_lookup(world, "Position");
    test_assert(ecs_id(Position) != 0);
    test_assert(ecs_has(world, ecs_id(Position), EcsComponent));

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has_id(world, e, ecs_id(Position)));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Commands_register_component_while_staged(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world);
    ecs_entity_t canary = ecs_new(world);

    ecs_readonly_begin(world, false);

    ecs_world_t *stage = ecs_get_stage(world, 0);

    ecs_add_id(stage, canary, Tag);

    ecs_entity_t ecs_id(Position) = ecs_component_init(stage, 
        &(ecs_component_desc_t){
            .entity = ecs_entity(stage, {.name = "Position"}),
            .type.size = ECS_SIZEOF(Position),
            .type.alignment = ECS_ALIGNOF(Position)
        });

    test_assert(ecs_id(Position) != 0);

    ecs_set(stage, e, Position, {10, 20});
    test_assert(!ecs_has_id(world, e, ecs_id(Position)));
    test_assert(!ecs_has_id(world, canary, Tag));

    ecs_readonly_end(world);

    test_assert(ecs_has_id(world, e, ecs_id(Position)));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Commands_register_component_while_deferred(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world);
    ecs_entity_t canary = ecs_new(world);

    ecs_defer_begin(world);

    ecs_add_id(world, canary, Tag);

    ecs_entity_t ecs_id(Position) = ecs_component_init(world, 
        &(ecs_component_desc_t){
            .entity = ecs_entity(world, {.name = "Position"}),
            .type.size = ECS_SIZEOF(Position),
            .type.alignment = ECS_ALIGNOF(Position)
        });

    test_assert(ecs_id(Position) != 0);

    ecs_set(world, e, Position, {10, 20});
    test_assert(!ecs_has_id(world, e, ecs_id(Position)));
    test_assert(!ecs_has_id(world, canary, Tag));

    ecs_defer_end(world);

    ecs_fini(world);
}

void Commands_defer_enable(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);
    ecs_enable(world, e, false);
    test_assert(!ecs_has_id(world, e, EcsDisabled));
    ecs_defer_end(world);

    test_assert(ecs_has_id(world, e, EcsDisabled));

    ecs_fini(world);
}

void Commands_defer_disable(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new_w_id(world, EcsDisabled);

    ecs_defer_begin(world);
    ecs_enable(world, e, true);
    test_assert(ecs_has_id(world, e, EcsDisabled));
    ecs_defer_end(world);

    test_assert(!ecs_has_id(world, e, EcsDisabled));

    ecs_fini(world);
}

void Commands_defer_enable_from_stage(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);

    ecs_world_t *stage = ecs_get_stage(world, 0);

    ecs_defer_begin(world);
    ecs_enable(stage, e, false);
    test_assert(!ecs_has_id(world, e, EcsDisabled));
    ecs_defer_end(world);

    test_assert(ecs_has_id(world, e, EcsDisabled));

    ecs_fini(world);
}

void Commands_defer_toggle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ecs_add_id(world, Foo, EcsCanToggle);

    ecs_entity_t e = ecs_new_w(world, Foo);

    ecs_defer_begin(world);
    ecs_enable_id(world, e, Foo, false);
    test_assert(ecs_is_enabled_id(world, e, Foo));
    ecs_defer_end(world);

    test_assert(!ecs_is_enabled_id(world, e, Foo));

    ecs_fini(world);
}

void Commands_defer_toggle_from_stage(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ecs_add_id(world, Foo, EcsCanToggle);

    ecs_entity_t e = ecs_new_w(world, Foo);

    ecs_world_t *stage = ecs_get_stage(world, 0);

    ecs_defer_begin(world);
    ecs_enable_id(stage, e, Foo, false);
    test_assert(ecs_is_enabled_id(stage, e, Foo));
    ecs_defer_end(world);

    test_assert(!ecs_is_enabled_id(world, e, Foo));

    ecs_fini(world);
}

void Commands_defer_delete_with(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e_1 = ecs_new_w(world, TagA);
    ecs_entity_t e_2 = ecs_new_w(world, TagA);
    ecs_entity_t e_3 = ecs_new_w(world, TagA);
    ecs_add(world, e_1, TagB);

    ecs_defer_begin(world);
    ecs_delete_with(world, TagA);
    test_assert(ecs_is_alive(world, e_1));
    test_assert(ecs_is_alive(world, e_2));
    test_assert(ecs_is_alive(world, e_3));
    ecs_defer_end(world);

    test_assert(!ecs_is_alive(world, e_1));
    test_assert(!ecs_is_alive(world, e_2));
    test_assert(!ecs_is_alive(world, e_3));

    ecs_fini(world);
}

void Commands_defer_remove_all(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e_1 = ecs_new_w(world, TagA);
    ecs_entity_t e_2 = ecs_new_w(world, TagA);
    ecs_entity_t e_3 = ecs_new_w(world, TagA);
    ecs_add(world, e_1, TagB);

    ecs_defer_begin(world);
    ecs_remove_all(world, TagA);
    test_assert(ecs_is_alive(world, e_1));
    test_assert(ecs_is_alive(world, e_2));
    test_assert(ecs_is_alive(world, e_3));
    ecs_defer_end(world);

    test_assert(ecs_is_alive(world, e_1));
    test_assert(ecs_is_alive(world, e_2));
    test_assert(ecs_is_alive(world, e_3));

    test_assert(!ecs_has_id(world, e_1, TagA));
    test_assert(!ecs_has_id(world, e_2, TagA));
    test_assert(!ecs_has_id(world, e_3, TagA));
    test_assert(ecs_has_id(world, e_1, TagB));

    ecs_fini(world);
}

void Commands_deferred_modified_after_remove(void) {
    ecs_world_t *world = ecs_mini();
    
    ECS_COMPONENT(world, Position);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = ecs_id(Position),
        .events = { EcsOnSet },
        .callback = OnSetTestInvoked
    });

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_modified(world, e, Position);

    test_int(on_set_invoked, 1);
    on_set_invoked = 0;

    ecs_defer_begin(world);

    ecs_remove(world, e, Position);
    test_assert(ecs_has(world, e, Position));

    ecs_modified(world, e, Position);
    ecs_defer_end(world);

    test_int(on_set_invoked, 0);

    ecs_fini(world);
}

typedef struct {
    int value;
} Counter;

ECS_COMPONENT_DECLARE(Counter);

static int update_counter_invoked = 0;
static int remove_counter_invoked = 0;

static void update_counter(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    for (int i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_entity_t parent = ecs_get_target(world, e, EcsChildOf, 0);
        test_assert(parent != 0);

        Counter *ptr = ecs_ensure(world, parent, Counter);
        test_assert(ptr != NULL);

        if (it->event == EcsOnAdd) {
            ptr->value ++;
        } else if (it->event == EcsOnRemove) {
            ptr->value --;
        }

        update_counter_invoked ++;
    }
}

static void remove_counter(ecs_iter_t *it) {
    Counter *ptr = ecs_field(it, Counter, 0);

    for (int i = 0; i < it->count; i ++) {
        test_int(ptr[i].value, 0);
        remove_counter_invoked ++;
    }
}

void Commands_merge_cleanup_ops_before_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Counter);
    ECS_TAG(world, Tag);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = Tag,
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = update_counter
    });

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0] = {
            .id = ecs_id(Counter),
            .src.id = EcsSelf
        },
        .events = {EcsOnRemove},
        .callback = remove_counter
    });

    ecs_entity_t parent = ecs_new(world);
    ecs_set(world, parent, Counter, {0});

    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_add(world, child, Tag);

    test_int(update_counter_invoked, 1);

    const Counter *ptr = ecs_get(world, parent, Counter);
    test_assert(ptr != NULL);
    test_assert(ptr->value == 1);

    ecs_delete(world, parent);

    test_int(update_counter_invoked, 2);
    test_int(remove_counter_invoked, 1);

    ecs_fini(world);
}

void Commands_merge_nested_cleanup_ops_before_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Counter);
    ECS_TAG(world, Tag);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0] = {
            .id = Tag,
            .src.id = EcsSelf
        },
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = update_counter
    });

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0] = {
            .id = ecs_id(Counter),
            .src.id = EcsSelf
        },
        .events = {EcsOnRemove},
        .callback = remove_counter
    });

    ecs_entity_t parent = ecs_new(world);
    ecs_set(world, parent, Counter, {0});

    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_add(world, child, Tag);
    test_int(update_counter_invoked, 1);
    ecs_set(world, child, Counter, {0});

    ecs_entity_t grand_child = ecs_new_w_pair(world, EcsChildOf, child);
    ecs_add(world, grand_child, Tag);
    test_int(update_counter_invoked, 2);

    const Counter *ptr = ecs_get(world, parent, Counter);
    test_assert(ptr != NULL);
    test_assert(ptr->value == 1);

    ptr = ecs_get(world, child, Counter);
    test_assert(ptr != NULL);
    test_assert(ptr->value == 1);

    ecs_delete(world, parent);

    test_int(update_counter_invoked, 4);
    test_int(remove_counter_invoked, 2);

    ecs_fini(world);
}

void Commands_defer_suspend_resume(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e = ecs_new(world);
    
    ecs_defer_begin(world);
    ecs_add(world, e, TagA);
    test_assert(!ecs_has(world, e, TagA));

    ecs_defer_suspend(world);
    ecs_add(world, e, TagB);
    test_assert(!ecs_has(world, e, TagA));
    test_assert(ecs_has(world, e, TagB));
    ecs_defer_resume(world);

    test_assert(!ecs_has(world, e, TagA));
    test_assert(ecs_has(world, e, TagB));

    ecs_defer_end(world);

    test_assert(ecs_has(world, e, TagA));
    test_assert(ecs_has(world, e, TagB));

    ecs_fini(world);
}

static
void System(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);
}

static int system_2_invoked = 0;

static
void System2(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);
    system_2_invoked ++;
}

void Commands_create_observer_while_deferred(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_defer_begin(world);
    Probe ctx = {0};
    ecs_entity_t observer = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ .id = TagA }},
        .events = {EcsOnAdd},
        .callback = System,
        .ctx = &ctx
    });
    ecs_defer_end(world);
    test_assert(observer != 0);
    test_int(ctx.invoked, 0);

    ecs_new_w(world, TagA);
    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void Commands_create_query_while_deferred(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_defer_begin(world);
    ecs_query_t *query = ecs_query(world, {
        .terms = {{ .id = TagA }}
    });
    ecs_defer_end(world);
    test_assert(query != 0);

    ecs_entity_t e = ecs_new_w(world, TagA);

    ecs_iter_t it = ecs_query_iter(world, query);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(query);

    ecs_fini(world);
}

void Commands_update_observer_while_deferred(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_defer_begin(world);
    Probe ctx = {0};
    ecs_entity_t observer = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ .id = TagA }},
        .events = {EcsOnAdd},
        .callback = System,
        .ctx = &ctx
    });
    ecs_defer_end(world);
    test_assert(observer != 0);
    test_int(ctx.invoked, 0);

    ecs_new_w(world, TagA);
    test_int(ctx.invoked, 1);

    test_int(system_2_invoked, 0);

    ecs_defer_begin(world);
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .entity = observer,
        .callback = System2
    });
    ecs_defer_end(world);

    ecs_new_w(world, TagA);
    test_int(ctx.invoked, 2);
    test_int(system_2_invoked, 1);

    ecs_fini(world);
}

typedef struct {
    int16_t arr[8096];
} LargeComponent;

void Commands_defer_set_large_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, LargeComponent);

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);
    LargeComponent *ptr = ecs_ensure(world, e, LargeComponent);
    test_assert(ptr != NULL);
    for (int i = 0; i < 8096; i ++) {
        ptr->arr[i] = i;
    }
    test_assert(!ecs_has(world, e, LargeComponent));
    ecs_defer_end(world);

    ptr = ecs_ensure(world, e, LargeComponent);
    test_assert(ptr != NULL);
    for (int i = 0; i < 8096; i ++) {
        test_int(ptr->arr[i], i);
    }

    ecs_fini(world);
}

static ECS_COMPONENT_DECLARE(Position);

static void CreatePosition(ecs_iter_t *it) {
    ecs_entity_t e = ecs_new(it->world);
    ecs_set(it->world, e, Position, {999, 1000});
}

void Commands_defer_while_suspend_readonly(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);

    /* Create observer on EcsComponent which will defer a command while readonly
     * mode is suspended */
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = ecs_id(EcsComponent),
        .events = { EcsOnAdd },
        .callback = CreatePosition
    });

    ecs_world_t *s = ecs_get_stage(world, 0);

    ecs_readonly_begin(world, false);

    /* Component creation suspends readonly mode so that they can be used right
     * after they have been registered */
    ECS_COMPONENT(s, Velocity);

    ecs_entity_t e = ecs_insert(world, 
        ecs_value(Position, {10, 20}),
        ecs_value(Velocity, {1, 2}));
    test_assert(!ecs_has(s, e, Position));
    test_assert(!ecs_has(s, e, Velocity));

    ecs_readonly_end(world);

    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Velocity *v = ecs_get(world, e, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 2);

    test_int(2, ecs_count(world, Position));

    ecs_fini(world);
}

void Commands_defer_while_suspend_readonly_w_existing_commands(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);

    /* Create observer on EcsComponent which will defer a command while readonly
     * mode is suspended */
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = ecs_id(EcsComponent),
        .events = { EcsOnAdd },
        .callback = CreatePosition
    });

    ecs_world_t *s = ecs_get_stage(world, 0);

    ecs_readonly_begin(world, false);

    ecs_entity_t e1 = ecs_insert(s, ecs_value(Position, {10, 20}));
    test_assert(!ecs_has(s, e1, Position));

    /* Component creation suspends readonly mode so that they can be used right
     * after they have been registered */
    ECS_COMPONENT(s, Velocity);

    ecs_entity_t e2 = ecs_insert(world,
        ecs_value(Position, {20, 30}),
        ecs_value(Velocity, {1, 2}));
    test_assert(!ecs_has(s, e2, Position));
    test_assert(!ecs_has(s, e2, Velocity));

    ecs_readonly_end(world);

    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));
    test_assert(ecs_has(world, e2, Velocity));

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 30);

    const Velocity *v = ecs_get(world, e2, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 2);

    test_int(3, ecs_count(world, Position));

    ecs_fini(world);
}

void Commands_defer_remove_after_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);
    ecs_set(world, e, Position, {10, 20});
    test_assert(!ecs_has(world, e, Position));
    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));
    ecs_defer_end(world);

    test_assert(!ecs_has(world, e, Position));

    ecs_fini(world);
}

static int position_observer_invoked = 0;

static
void PositionObserver(ecs_iter_t *it) {
    position_observer_invoked ++;
}

void Commands_defer_remove_after_set_w_observer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, PositionObserver, EcsOnSet, Position);

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);
    ecs_set(world, e, Position, {10, 20});
    test_assert(!ecs_has(world, e, Position));
    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));
    ecs_defer_end(world);

    test_int(position_observer_invoked, 0);
    test_assert(!ecs_has(world, e, Position));

    ecs_fini(world);
}

void Commands_defer_override_after_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    ecs_set(world, inst, Position, {20, 30});

    ecs_defer_begin(world);
    ecs_remove(world, inst, Position);
    ecs_add(world, inst, Position);
    ecs_defer_end(world);

    const Position *p = ecs_get(world, inst, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    test_assert(p != ecs_get(world, base, Position));

    ecs_fini(world);
}

void Commands_defer_override_after_remove_3_ops(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    ecs_set(world, inst, Position, {20, 30});

    ecs_defer_begin(world);
    ecs_remove(world, inst, Position);
    ecs_add(world, inst, Position);
    ecs_defer_end(world);

    const Position *p = ecs_get(world, inst, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    test_assert(p != ecs_get(world, base, Position));

    ecs_fini(world);
}

void Commands_flush_stage_to_deferred_world(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world);

    ecs_world_t *async = ecs_stage_new(world);
    ecs_add(async, e, Tag);
    test_assert(!ecs_has(world, e, Tag));

    ecs_defer_begin(world);
    ecs_merge(async);
    test_assert(!ecs_has(world, e, Tag));
    ecs_defer_end(world);
    test_assert(ecs_has(world, e, Tag));

    ecs_stage_free(async);

    ecs_fini(world);
}

static
void AddPosition(ecs_iter_t *it) {
    test_int(it->count, 1);
    ecs_set(it->world, it->entities[0], Position, {10, 20});
}

void Commands_add_in_observer_during_merge(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG(world, TagA);

    ECS_OBSERVER(world, AddPosition, EcsOnAdd, TagA);

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);
    ecs_add(world, e, TagA);
    test_assert(!ecs_has(world, e, TagA));
    test_assert(!ecs_has(world, e, Position));
    ecs_defer_end(world);

    test_assert(ecs_has(world, e, TagA));
    test_assert(ecs_has(world, e, Position));

    const Position *ptr = ecs_get(world, e, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_fini(world);
}

void Commands_add_in_observer_during_merge_2_commands(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ECS_OBSERVER(world, AddPosition, EcsOnAdd, TagB);

    ecs_entity_t e = ecs_new(world);

    ecs_log_set_level(0);

    ecs_defer_begin(world);
    ecs_add(world, e, TagA);
    ecs_add(world, e, TagB);
    test_assert(!ecs_has(world, e, TagA));
    test_assert(!ecs_has(world, e, TagB));
    test_assert(!ecs_has(world, e, Position));
    ecs_defer_end(world);

    test_assert(ecs_has(world, e, TagA));
    test_assert(ecs_has(world, e, TagB));
    test_assert(ecs_has(world, e, Position));

    const Position *ptr = ecs_get(world, e, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_log_set_level(-1);

    ecs_fini(world);
}

static ECS_DECLARE(TagB);
static ECS_DECLARE(TagC);

static
void AddTwoTags(ecs_iter_t *it) {
    test_int(it->count, 1);
    ecs_add(it->world, it->entities[0], TagB);
    ecs_add(it->world, it->entities[0], TagC);
}

void Commands_add_2_in_observer_while_on_remove_for_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG_DEFINE(world, TagB);
    ECS_TAG_DEFINE(world, TagC);

    ECS_OBSERVER(world, AddTwoTags, EcsOnRemove, TagA);

    ecs_entity_t e = ecs_new_w(world, TagA);
    ecs_delete(world, e);

    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void Commands_add_2_in_observer_while_on_remove_for_delete_child(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG_DEFINE(world, TagB);
    ECS_TAG_DEFINE(world, TagC);

    ECS_OBSERVER(world, AddTwoTags, EcsOnRemove, TagA);

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t e = ecs_new_w(world, TagA);
    ecs_add_pair(world, e, EcsChildOf, parent);
    ecs_delete(world, e);

    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void Commands_add_2_in_observer_while_on_remove_for_delete_recycled_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG_DEFINE(world, TagB);
    ECS_TAG_DEFINE(world, TagC);

    ECS_OBSERVER(world, AddTwoTags, EcsOnRemove, TagA);

    ecs_delete(world, ecs_new(world));

    ecs_entity_t e = ecs_new_w(world, TagA);
    ecs_delete(world, e);

    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void Commands_add_2_in_observer_while_on_remove_for_deferred_delete_recycled_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG_DEFINE(world, TagB);
    ECS_TAG_DEFINE(world, TagC);

    ECS_OBSERVER(world, AddTwoTags, EcsOnRemove, TagA);

    ecs_delete(world, ecs_new(world));

    ecs_entity_t e = ecs_new_w(world, TagA);
    ecs_defer_begin(world);
    ecs_delete(world, e);
    ecs_defer_end(world);

    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void Commands_defer_add_after_clear(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);
    ecs_clear(world, e);
    ecs_add(world, e, Position);
    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void Commands_defer_cmd_after_modified(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    
    ecs_defer_begin(world);
    ecs_set(world, e1, Position, {10, 20});
    ecs_modified(world, e2, Position);
    ecs_set(world, e2, Position, {20, 30});

    test_assert(!ecs_has(world, e1, Position));
    test_assert(!ecs_has(world, e2, Position));
    ecs_defer_end(world);
    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 30);

    ecs_fini(world);
}

void Commands_defer_remove_after_emplace_different_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Tag);

    ecs_defer_begin(world);
    Position *p = ecs_emplace(world, e, Position, NULL);
    p->x = 10;
    p->y = 20;
    ecs_remove(world, e, Tag);
    test_assert(!ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Tag));
    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Tag));

    const Position *ptr = ecs_get(world, e, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_fini(world);
}

void Commands_defer_remove_after_set_and_emplace_different_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Tag);

    ecs_defer_begin(world);
    Position *p = ecs_emplace(world, e, Position, NULL);
    p->x = 10;
    p->y = 20;
    ecs_set(world, e, Velocity, {1, 2});
    ecs_remove(world, e, Tag);
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    test_assert(ecs_has(world, e, Tag));
    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(!ecs_has(world, e, Tag));

    const Position *ptr = ecs_get(world, e, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    const Velocity *v = ecs_get(world, e, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 2);

    ecs_fini(world);
}

void Commands_clear_after_add_to_nonempty(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e = ecs_new_w(world, TagA);

    ecs_defer_begin(world);
    ecs_add(world, e, TagB);
    ecs_clear(world, e);

    test_assert(ecs_has(world, e, TagA));
    test_assert(!ecs_has(world, e, TagB));
    ecs_defer_end(world);

    test_assert(!ecs_has(world, e, TagA));
    test_assert(!ecs_has(world, e, TagB));

    ecs_fini(world);
}

void Commands_remove_after_add_to_nonempty(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e = ecs_new_w(world, TagA);

    ecs_defer_begin(world);
    ecs_add(world, e, TagB);
    ecs_remove(world, e, TagA);
    ecs_remove(world, e, TagB);

    test_assert(ecs_has(world, e, TagA));
    test_assert(!ecs_has(world, e, TagB));
    ecs_defer_end(world);

    test_assert(!ecs_has(world, e, TagA));
    test_assert(!ecs_has(world, e, TagB));

    ecs_fini(world);
}

void Commands_register_while_deferred_with_n_stages(void) {
    ecs_world_t *world = ecs_mini();

    ecs_set_stage_count(world, 2);

    ecs_defer_begin(world);
    ECS_COMPONENT(world, Position);
    ecs_defer_end(world);

    test_assert(ecs_id(Position));

    ecs_fini(world);
}

static int position_velocity_observer_invoked = 0;

static
void PositionVelocityObserver(ecs_iter_t *it) {
    position_velocity_observer_invoked ++;
    test_int(it->count, 1);
    
    Position *p = ecs_field(it, Position, 0);
    Velocity *v = ecs_field(it, Velocity, 1);

    test_assert(p != NULL);
    test_assert(v != NULL);

    test_int(p->x, 10);
    test_int(p->y, 20);

    test_int(v->x, 1);
    test_int(v->y, 2);
}

void Commands_defer_2_sets_w_multi_observer(void) {
    ecs_world_t *world = ecs_mini();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_observer(world, {
        .query.terms = {{ ecs_id(Position) }, { ecs_id(Velocity) }},
        .callback = PositionVelocityObserver,
        .events = { EcsOnSet }
    });

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);
    ecs_set(world, e, Position, {10, 20});
    ecs_set(world, e, Velocity, {1, 2});
    ecs_defer_end(world);

    test_int(position_velocity_observer_invoked, 2);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);
    const Velocity *v = ecs_get(world, e, Velocity);
    test_int(v->x, 1);
    test_int(v->y, 2);

    ecs_fini(world);
}

void Commands_defer_2_ensures_w_multi_observer(void) {
    ecs_world_t *world = ecs_mini();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_observer(world, {
        .query.terms = {{ ecs_id(Position) }, { ecs_id(Velocity) }},
        .callback = PositionVelocityObserver,
        .events = { EcsOnSet }
    });

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);
    {
        Position *p = ecs_ensure(world, e, Position);
        p->x = 10;
        p->y = 20;
        ecs_modified(world, e, Position);
        Velocity *v = ecs_ensure(world, e, Velocity);
        v->x = 1;
        v->y = 2;
        ecs_modified(world, e, Velocity);
    }
    ecs_defer_end(world);

    test_int(position_velocity_observer_invoked, 2);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);
    const Velocity *v = ecs_get(world, e, Velocity);
    test_int(v->x, 1);
    test_int(v->y, 2);

    ecs_fini(world);
}

void Commands_defer_2_ensures_no_modified_w_multi_observer(void) {
    ecs_world_t *world = ecs_mini();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_observer(world, {
        .query.terms = {{ ecs_id(Position) }, { ecs_id(Velocity) }},
        .callback = PositionVelocityObserver,
        .events = { EcsOnSet }
    });

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);
    {
        Position *p = ecs_ensure(world, e, Position);
        p->x = 10;
        p->y = 20;
        Velocity *v = ecs_ensure(world, e, Velocity);
        v->x = 1;
        v->y = 2;
    }
    ecs_defer_end(world);

    test_int(position_velocity_observer_invoked, 0);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);
    const Velocity *v = ecs_get(world, e, Velocity);
    test_int(v->x, 1);
    test_int(v->y, 2);

    ecs_fini(world);
}

void Commands_exists_remove_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {1, 2});

    ecs_defer_begin(world);
    ecs_remove(world, e, Position);
    ecs_set(world, e, Position, {5, 6});
    ecs_defer_end(world);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 5);
    test_int(p->y, 6);

    ecs_fini(world);
}

void Commands_absent_remove_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);
    ecs_remove(world, e, Position);
    ecs_set(world, e, Position, {5, 6});
    ecs_defer_end(world);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 5);
    test_int(p->y, 6);

    ecs_fini(world);
}

void Commands_exists_set_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {1, 2});

    ecs_defer_begin(world);
    ecs_set(world, e, Position, {5, 6});
    ecs_remove(world, e, Position);
    ecs_defer_end(world);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p == NULL);

    ecs_fini(world);
}

void Commands_absent_set_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);
    ecs_set(world, e, Position, {5, 6});
    ecs_remove(world, e, Position);
    ecs_defer_end(world);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p == NULL);

    ecs_fini(world);
}

void Commands_exists_set_w_ensure(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {1, 2});

    ecs_defer_begin(world);
    {
        ecs_set(world, e, Position, {5, 6});
        Position *p = ecs_ensure(world, e, Position);
        p->x = 11;
    }
    ecs_defer_end(world);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 6);

    ecs_fini(world);
}

void Commands_exists_remove_ensure(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {1, 2});

    ecs_defer_begin(world);
    ecs_remove(world, e, Position);
    {
        Position *p = ecs_ensure(world, e, Position);
        test_assert(p != NULL);
        p->x = 5;
        p->y = 6;
    }
    ecs_defer_end(world);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 5);
    test_int(p->y, 6);

    ecs_fini(world);
}

void Commands_absent_remove_ensure(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);
    ecs_remove(world, e, Position);
    {
        Position *p = ecs_ensure(world, e, Position);
        test_assert(p != NULL);
        p->x = 5;
        p->y = 6;
    }
    test_assert(!ecs_has(world, e, Position));
    ecs_defer_end(world);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 5);
    test_int(p->y, 6);

    ecs_fini(world);
}

void Commands_exists_ensure_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {1, 2});

    ecs_defer_begin(world);
    {
        Position *p = ecs_ensure(world, e, Position);
        test_assert(p != NULL);
        p->x = 5;
        p->y = 6;
    }
    ecs_remove(world, e, Position);
    ecs_defer_end(world);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p == NULL);

    ecs_fini(world);
}

void Commands_absent_ensure_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);
    {
        Position *p = ecs_ensure(world, e, Position);
        test_assert(p != NULL);
        p->x = 5;
        p->y = 6;
    }
    test_assert(!ecs_has(world, e, Position));
    ecs_remove(world, e, Position);
    ecs_defer_end(world);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p == NULL);

    ecs_fini(world);
}

void Commands_absent_set_invoke_on_set(void) {
    ecs_world_t *world = ecs_mini();
    on_set_invoked = 0;

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, OnSetTestInvoked, EcsOnSet, Position);

    ecs_entity_t e = ecs_new(world);

    /* create the component */
    ecs_defer_begin(world);
    ecs_set(world, e, Position, {1, 2});

    /* OnSet should not be run until defer has completed */
    test_int(on_set_invoked, 0);
    ecs_defer_end(world);

    const Position *p = ecs_get(world, e, Position);
    test_int(on_set_invoked, 1);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Commands_exists_set_invoke_on_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, OnSetTestInvoked, EcsOnSet, Position);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {1, 2});
    on_set_invoked = 0;

    /* create the component */
    ecs_defer_begin(world);
    ecs_set(world, e, Position, {5, 6});

    /* OnSet should not be run until defer has completed */
    test_int(on_set_invoked, 0);
    ecs_defer_end(world);

    const Position *p = ecs_get(world, e, Position);
    test_int(on_set_invoked, 1);
    test_int(p->x, 5);
    test_int(p->y, 6);

    ecs_fini(world);
}

void Commands_defer_ensure_no_on_set(void) {
    ecs_world_t *world = ecs_mini();
    on_set_invoked = 0;

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, OnSetTestInvoked, EcsOnSet, Position);

    ecs_entity_t e = ecs_new(world);

    /* create the component */
    ecs_defer_begin(world);
    {
        Position *p = ecs_ensure(world, e, Position);
        p->x = 1;
        p->y = 2;
    }
    test_assert(!ecs_has(world, e, Position));
    test_int(on_set_invoked, 0);
    ecs_defer_end(world);
    test_int(on_set_invoked, 0);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Commands_defer_existing_ensure_no_on_set(void) {
    ecs_world_t *world = ecs_mini();
    on_set_invoked = 0;

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, OnSetTestInvoked, EcsOnSet, Position);

    ecs_entity_t e = ecs_new_w(world, Position);

    /* create the component */
    ecs_defer_begin(world);
    {
        Position *p = ecs_ensure(world, e, Position);
        p->x = 1;
        p->y = 2;
    }
    test_assert(ecs_has(world, e, Position));
    test_int(on_set_invoked, 0);
    ecs_defer_end(world);
    test_int(on_set_invoked, 0);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Commands_ensure_override(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_insert(world, ecs_value(Position, {1, 2}));
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);

    ecs_defer_begin(world);
    {
        Position *p = ecs_ensure(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
        p->x = 5;
        p->y = 6;
    }

    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_owns(world, e, Position));
    ecs_defer_end(world);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_owns(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 5);
    test_int(p->y, 6);

    const Position *bp = ecs_get(world, base, Position);
    test_assert(p != bp);
    test_int(bp->x, 1);
    test_int(bp->y, 2);

    ecs_fini(world);
}

void Commands_set_override(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_insert(world, ecs_value(Position, {1, 2}));
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);

    ecs_defer_begin(world);
    {
        ecs_set(world, e, Position, {5, 6});
    }

    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_owns(world, e, Position));
    ecs_defer_end(world);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_owns(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 5);
    test_int(p->y, 6);

    const Position *bp = ecs_get(world, base, Position);
    test_assert(p != bp);
    test_int(bp->x, 1);
    test_int(bp->y, 2);

    ecs_fini(world);
}

void Commands_absent_ensure_for_entity_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w(world, Tag);
    test_assert(e != 0);
    
    ecs_defer_begin(world);
    {
        Position *p = ecs_ensure(world, e, Position);
        test_assert(p != NULL);
        p->x = 10;
        p->y = 20;
    }
    ecs_defer_end(world);

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

static int set_position_invoked = 0;
static int set_velocity_invoked = 0;
static int add_tag_invoked = 0;

static void set_position_hook(ecs_iter_t *it) {
    set_position_invoked ++;
}

static void set_velocity_hook(ecs_iter_t *it) {
    set_velocity_invoked ++;
}

static void add_tag(ecs_iter_t *it) {
    test_int(set_position_invoked, 1);
    add_tag_invoked ++;
}

void Commands_on_set_hook_before_on_add_for_existing_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_set_hooks(world, Position, {
        .on_set = set_position_hook
    });

    ecs_observer(world, {
        .query.terms[0].id = TagA,
        .events = { EcsOnAdd },
        .callback = add_tag
    });

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_defer_begin(world);
    ecs_add(world, e, TagB); /* 2 add commands, to trigger batching */
    ecs_modified(world, e, Position);
    ecs_add(world, e, TagA);

    test_int(set_position_invoked, 0);
    test_int(add_tag_invoked, 0);
    ecs_defer_end(world);

    test_assert(set_position_invoked != 0);
    test_int(add_tag_invoked, 1);

    ecs_fini(world);
}

void Commands_defer_2_sets_w_observer_same_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_observer(world, {
        .query.terms[0].id = ecs_id(Position),
        .events = { EcsOnSet },
        .callback = set_position_hook
    });

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_defer_begin(world);
    ecs_set(world, e, Position, {10, 20});
    ecs_set(world, e, Position, {20, 30});
    ecs_defer_end(world);

    test_assert(set_position_invoked != 0);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 30);

    ecs_fini(world);
}

void Commands_defer_2_sets_w_observer_other_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_observer(world, {
        .query.terms[0].id = ecs_id(Position),
        .events = { EcsOnSet },
        .callback = set_position_hook
    });

    ecs_observer(world, {
        .query.terms[0].id = ecs_id(Velocity),
        .events = { EcsOnSet },
        .callback = set_velocity_hook
    });

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_defer_begin(world);
    ecs_set(world, e, Position, {10, 20});
    ecs_set(world, e, Velocity, {1, 2});
    ecs_defer_end(world);

    test_assert(set_position_invoked != 0);
    test_assert(set_velocity_invoked != 0);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Velocity *v = ecs_get(world, e, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 2);

    ecs_fini(world);
}

static int remove_tag_invoked = 0;
static void remove_tag(ecs_iter_t *it) {
    remove_tag_invoked ++;
}

void Commands_on_remove_after_deferred_clear_and_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_observer(world, {
        .query.terms[0].id = TagA,
        .events = { EcsOnRemove },
        .callback = remove_tag
    });

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, TagA);
    ecs_add(world, e, TagB);

    ecs_defer_begin(world);
    ecs_clear(world, e);
    ecs_add(world, e, TagC);
    test_int(remove_tag_invoked, 0);
    ecs_defer_end(world);
    test_int(remove_tag_invoked, 1);

    test_assert(!ecs_has(world, e, TagA));
    test_assert(!ecs_has(world, e, TagB));
    test_assert(ecs_has(world, e, TagC));

    ecs_fini(world);
}

static void Recycle(ecs_iter_t *it) {
    ecs_new(it->world);
    *(ecs_entity_t*)it->ctx = ecs_new(it->world);
}

void Commands_defer_delete_recycle_same_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e2 = ecs_new(world);

    ecs_observer(world, {
        .query.terms = {
            { .id = ecs_id(Foo) }
        },
        .events = { EcsOnAdd },
        .callback = Recycle,
        .ctx = &e1
    });

    ecs_defer_begin(world);
    ecs_delete(world, parent);
    ecs_add(world, e2, Foo);
    ecs_add(world, e1, Bar);
    ecs_defer_end(world);

    test_assert(!ecs_is_alive(world, parent));
    test_assert(ecs_is_alive(world, e2));
    test_assert(ecs_is_alive(world, e1));
    test_assert((uint32_t)e1 != e1);
    test_assert(ecs_has(world, e2, Foo));

    ecs_fini(world);
}

static ECS_COMPONENT_DECLARE(Velocity);

static
void AddWhileSuspended(ecs_iter_t *it) {
    for (int i = 0; i < it->count; i ++) {
        ecs_add(it->world, it->entities[i], Velocity);
    }
}

void Commands_observer_while_defer_suspended(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);

    ecs_observer(world, {
        .query.terms = {
            { .id = ecs_id(Position) }
        },
        .events = { EcsOnAdd },
        .callback = AddWhileSuspended,
    });

    ecs_defer_begin(world);
    ecs_defer_suspend(world);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));

    ecs_defer_resume(world);
    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Commands_on_add_hook_while_defer_suspended(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);

    ecs_set_hooks(world, Position, {
        .on_add = AddWhileSuspended
    });

    ecs_defer_begin(world);
    ecs_defer_suspend(world);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));

    ecs_defer_resume(world);
    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Commands_on_set_hook_while_defer_suspended(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);

    ecs_set_hooks(world, Position, {
        .on_set = AddWhileSuspended
    });

    ecs_defer_begin(world);
    ecs_defer_suspend(world);

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));

    ecs_defer_resume(world);
    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Commands_on_remove_hook_while_defer_suspended(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);

    ecs_set_hooks(world, Position, {
        .on_remove = AddWhileSuspended
    });

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));

    ecs_defer_begin(world);
    ecs_defer_suspend(world);

    test_assert(ecs_has(world, e, Position));
    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));

    ecs_defer_resume(world);
    ecs_defer_end(world);

    test_assert(!ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

static int on_set_is_deferred_invoked = 0;
static int dummy_observer_invoked = 0;
static ECS_TAG_DECLARE(Foo);
static ECS_TAG_DECLARE(Bar);

static
void OnSetIsDeferred(ecs_iter_t *it) {
    on_set_is_deferred_invoked ++;
    test_int(it->count, 1);
    ecs_table_t *t = ecs_get_table(it->world, it->entities[0]);
    ecs_add(it->world, it->entities[0], Bar);
    ecs_remove(it->world, it->entities[0], Foo);
    test_assert(t == ecs_get_table(it->world, it->entities[0]));
}

static
void DummyObserver(ecs_iter_t *it) {
    dummy_observer_invoked ++;
}

void Commands_on_set_hook_batched_is_deferred(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG_DEFINE(world, Foo);
    ECS_TAG_DEFINE(world, Bar);
    ECS_TAG(world, Tag);

    ecs_set_hooks(world, Position, {
        .on_set = OnSetIsDeferred
    });

    ECS_OBSERVER(world, DummyObserver, EcsOnSet, Position);

    ecs_entity_t e = ecs_new_w(world, Bar);
    ecs_add(world, e, Position);

    test_int(on_set_is_deferred_invoked, 0);
    test_int(dummy_observer_invoked, 0);

    ecs_defer_begin(world);
    ecs_add(world, e, Tag);
    ecs_modified(world, e, Position);
    test_int(on_set_is_deferred_invoked, 0);
    test_int(dummy_observer_invoked, 0);
    ecs_defer_end(world);

    test_int(on_set_is_deferred_invoked, 1);
    test_int(dummy_observer_invoked, 1);

    test_assert(ecs_has(world, e, Tag));
    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Foo));
    test_assert(ecs_has(world, e, Bar));

    ecs_fini(world);
}

void Commands_add_path(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_entity(world, { .name = "p" });
    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_defer_begin(world);
    ecs_add_path(world, e, p, "foo");
    ecs_defer_end(world);

    ecs_entity_t foo = ecs_lookup(world, "p.foo");
    test_assert(foo == e);

    char *path = ecs_get_path(world, e);
    test_assert(path != NULL);
    test_str(path, "p.foo");
    ecs_os_free(path);

    ecs_fini(world);
}

void Commands_add_path_to_deleted_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_entity(world, { .name = "p" });
    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_defer_begin(world);
    ecs_delete(world, p);
    ecs_add_path(world, e, p, "foo");
    ecs_defer_end(world);

    ecs_entity_t foo = ecs_lookup(world, "p.foo");
    test_assert(foo == 0);

    test_assert(!ecs_is_alive(world, p));
    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void Commands_add_path_nested(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_entity(world, { .name = "p" });
    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_defer_begin(world);
    ecs_add_path(world, e, p, "foo.bar");
    ecs_defer_end(world);

    ecs_entity_t foo = ecs_lookup(world, "p.foo");
    test_assert(foo != 0);

    char *path = ecs_get_path(world, e);
    test_assert(path != NULL);
    test_str(path, "p.foo.bar");
    ecs_os_free(path);

    ecs_fini(world);
}

void Commands_add_path_nested_to_deleted_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_entity(world, { .name = "p" });
    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_defer_begin(world);
    ecs_delete(world, p);
    ecs_add_path(world, e, p, "foo.bar");
    ecs_defer_end(world);

    ecs_entity_t foo = ecs_lookup(world, "p.foo");
    test_assert(foo == 0);

    test_assert(!ecs_is_alive(world, p));
    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void Commands_add_path_nested_to_created_deleted_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_entity(world, { .name = "p" });
    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_defer_begin(world);
    ecs_entity_t foo = ecs_entity(world, { .name = "p.foo" });
    ecs_delete(world, foo);
    ecs_add_path(world, e, p, "foo.bar");
    ecs_defer_end(world);

    test_assert(ecs_is_alive(world, p));
    test_assert(!ecs_is_alive(world, e));
    test_assert(!ecs_is_alive(world, foo));

    ecs_fini(world);
}

void Commands_add_path_w_stage(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_entity(world, { .name = "p" });
    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_world_t *s = ecs_get_stage(world, 0);

    ecs_defer_begin(world);
    ecs_add_path(s, e, p, "foo");
    ecs_defer_end(world);

    ecs_entity_t foo = ecs_lookup(world, "p.foo");
    test_assert(foo == e);

    char *path = ecs_get_path(world, e);
    test_assert(path != NULL);
    test_str(path, "p.foo");
    ecs_os_free(path);

    ecs_fini(world);
}

void Commands_add_path_to_deleted_parent_w_stage(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_entity(world, { .name = "p" });
    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_world_t *s = ecs_get_stage(world, 0);

    ecs_defer_begin(world);
    ecs_delete(world, p);
    ecs_add_path(s, e, p, "foo");
    ecs_defer_end(world);

    ecs_entity_t foo = ecs_lookup(world, "p.foo");
    test_assert(foo == 0);

    test_assert(!ecs_is_alive(world, p));
    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void Commands_add_path_nested_w_stage(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_entity(world, { .name = "p" });
    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_world_t *s = ecs_get_stage(world, 0);

    ecs_defer_begin(world);
    ecs_add_path(s, e, p, "foo.bar");
    ecs_defer_end(world);

    ecs_entity_t foo = ecs_lookup(world, "p.foo");
    test_assert(foo != 0);

    char *path = ecs_get_path(world, e);
    test_assert(path != NULL);
    test_str(path, "p.foo.bar");
    ecs_os_free(path);

    ecs_fini(world);
}

void Commands_add_path_nested_to_deleted_parent_w_stage(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_entity(world, { .name = "p" });
    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_world_t *s = ecs_get_stage(world, 0);

    ecs_defer_begin(world);
    ecs_delete(world, p);
    ecs_add_path(s, e, p, "foo.bar");
    ecs_defer_end(world);

    ecs_entity_t foo = ecs_lookup(world, "p.foo");
    test_assert(foo == 0);

    test_assert(!ecs_is_alive(world, p));
    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void Commands_add_path_nested_to_created_deleted_parent_w_stage(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_entity(world, { .name = "p" });
    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_world_t *s = ecs_get_stage(world, 0);

    ecs_defer_begin(world);
    ecs_entity_t foo = ecs_entity(s, { .name = "p.foo" });
    ecs_delete(s, foo);
    ecs_add_path(s, e, p, "foo.bar");
    ecs_defer_end(world);

    test_assert(ecs_is_alive(world, p));
    test_assert(!ecs_is_alive(world, e));
    test_assert(!ecs_is_alive(world, foo));

    ecs_fini(world);
}

void Commands_defer_emplace_w_arg(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);
    {
        bool is_new = false;
        Position *p = ecs_emplace(world, e, Position, &is_new);
        p->x = 1; p->y = 2;
        test_bool(is_new, true);
    }
    {
        bool is_new = false;
        Position *p = ecs_emplace(world, e, Position, &is_new);
        p->x = 10; p->y = 20;
        test_bool(is_new, true); /* Might change after cmd queue temp storage refactor */
    }
    test_assert(!ecs_has(world, e, Position));
    ecs_defer_end(world);
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Commands_defer_emplace_existing_w_arg(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_defer_begin(world);
    {
        bool is_new = false;
        Position *p = ecs_emplace(world, e, Position, &is_new);
        p->x = 1; p->y = 2;
        test_bool(is_new, false);
    }
    {
        bool is_new = false;
        Position *p = ecs_emplace(world, e, Position, &is_new);
        p->x = 10; p->y = 20;
        test_bool(is_new, false); /* Might change after cmd queue temp storage refactor */
    }
    test_assert(ecs_has(world, e, Position));
    ecs_defer_end(world);
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

static int mixed_observer_invoked = 0;
static int mixed_observer_on_add_invoked = 0;
static int mixed_observer_on_set_invoked = 0;

static void MixedObserver(ecs_iter_t *it) {
    test_int(it->count, 1);

    Position *p = ecs_field(it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    mixed_observer_invoked ++;

    if (it->event == EcsOnAdd) {
        mixed_observer_on_add_invoked ++;
    } else if (it->event == EcsOnSet) {
        mixed_observer_on_set_invoked ++;
    }
}

void Commands_mixed_on_add_on_set_w_set_w_batching(void) {
    ecs_world_t *world = ecs_mini();
    
    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_observer(world, {
        .query.terms = {{ ecs_id(Position) }, { Foo }},
        .callback = MixedObserver,
        .events = { EcsOnSet }
    });

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);
    ecs_add(world, e, Foo);
    ecs_set(world, e, Position, {10, 20});
    ecs_defer_end(world);

    test_int(mixed_observer_invoked, 2);
    test_int(mixed_observer_on_add_invoked, 1);
    test_int(mixed_observer_on_set_invoked, 1);

    ecs_fini(world);
}

void Commands_mixed_on_add_on_set_w_emplace(void) {
    ecs_world_t *world = ecs_mini();
    
    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_observer(world, {
        .query.terms = {{ ecs_id(Position) }, { Foo }},
        .callback = MixedObserver,
        .events = { EcsOnSet }
    });

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);
    ecs_add(world, e, Foo);
    Position *p = ecs_emplace(world, e, Position, NULL);
    p->x = 10;
    p->y = 20;
    ecs_modified(world, e, Position);
    ecs_defer_end(world);

    test_int(mixed_observer_invoked, 1);
    test_int(mixed_observer_on_add_invoked, 0); /* Entity didn't have component yet */
    test_int(mixed_observer_on_set_invoked, 1);

    ecs_fini(world);
}

void Commands_add_isa_set_w_override_batched(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_insert(world, ecs_value(Position, {10, 20}));

    ecs_defer_begin(world);
    ecs_entity_t i = ecs_new(world);
    ecs_add_pair(world, i, EcsIsA, p);
    ecs_set(world, i, Position, {20, 30});
    test_assert(!ecs_has_pair(world, i, EcsIsA, p));
    test_assert(!ecs_has(world, i, Position));
    ecs_defer_end(world);
    test_assert(ecs_has_pair(world, i, EcsIsA, p));
    test_assert(ecs_has(world, i, Position));
    
    const Position *ptr = ecs_get(world, i, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 20);
    test_int(ptr->y, 30);

    ecs_fini(world);
}

void Commands_add_set_isa_w_override_batched(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_insert(world, ecs_value(Position, {10, 20}));

    ecs_defer_begin(world);
    ecs_entity_t i = ecs_new(world);
    ecs_set(world, i, Position, {20, 30});
    ecs_add_pair(world, i, EcsIsA, p);
    test_assert(!ecs_has_pair(world, i, EcsIsA, p));
    test_assert(!ecs_has(world, i, Position));
    ecs_defer_end(world);
    test_assert(ecs_has_pair(world, i, EcsIsA, p));
    test_assert(ecs_has(world, i, Position));
    
    const Position *ptr = ecs_get(world, i, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 20);
    test_int(ptr->y, 30);

    ecs_fini(world);
}

void Commands_add_batched_set_with(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsWith, ecs_id(Velocity));

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);
    ecs_add(world, e, Foo);
    ecs_set(world, e, Position, {10, 20});
    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Foo));
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Commands_defer_emplace_after_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {0, 0}));

    ecs_defer_begin(world);
    ecs_remove(world, e, Position);
    {
        Position *p = ecs_emplace(world, e, Position, NULL);
        test_assert(p != NULL);
        p->x = 10;
        p->y = 20;
    }
    ecs_defer_end(world);
    
    test_assert(ecs_has(world, e, Position));
    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}
