#include <api.h>

void DeferredActions_defer_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_defer_begin(world);
    
    ecs_entity_t e = ecs_new(world, Position);
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

void DeferredActions_defer_bulk_new() {
    ecs_world_t *world = ecs_init();

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

void DeferredActions_defer_bulk_new_w_data() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_defer_begin(world);
    
    const ecs_entity_t *temp_ids = ecs_bulk_new_w_data(world, 3, 
        &(ecs_ids_t){
            .array = (ecs_entity_t[]){ecs_id(Position)},
            .count = 1
        },
        (void*[]){
            (Position[]){
                {10, 20},
                {30, 40},
                {50, 60}
            }
        });

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

    const Position *
    p = ecs_get(world, ids[0], Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get(world, ids[1], Position);
    test_int(p->x, 30);
    test_int(p->y, 40);

    p = ecs_get(world, ids[2], Position);
    test_int(p->x, 50);
    test_int(p->y, 60);

    ecs_fini(world);
}

void DeferredActions_defer_bulk_new_w_data_pair() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Pair);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_defer_begin(world);

    ecs_entity_t pair_id = ecs_pair(Pair, ecs_id(Position));
    
    const ecs_entity_t *temp_ids = ecs_bulk_new_w_data(world, 3, 
        &(ecs_ids_t){
            .array = (ecs_entity_t[]){pair_id},
            .count = 1
        },
        (void*[]){
            (Position[]){
                {10, 20},
                {30, 40},
                {50, 60}
            }
        });

    ecs_entity_t ids[3];
    memcpy(ids, temp_ids, sizeof(ecs_entity_t) * 3);

    test_assert(!ecs_has_id(world, ids[0], pair_id));
    test_assert(!ecs_has_id(world, ids[1], pair_id));
    test_assert(!ecs_has_id(world, ids[2], pair_id)); 

    ecs_defer_end(world);

    test_assert(!ecs_has_id(world, ids[0], pair_id));
    test_assert(!ecs_has_id(world, ids[1], pair_id));
    test_assert(!ecs_has_id(world, ids[2], pair_id));

    ecs_defer_end(world);

    test_assert(ecs_has_id(world, ids[0], pair_id));
    test_assert(ecs_has_id(world, ids[1], pair_id));
    test_assert(ecs_has_id(world, ids[2], pair_id));

    ecs_frame_end(world);

    test_assert(ecs_has_id(world, ids[0], pair_id));
    test_assert(ecs_has_id(world, ids[1], pair_id));
    test_assert(ecs_has_id(world, ids[2], pair_id));

    const Position *
    p = ecs_get_id(world, ids[0], pair_id);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_id(world, ids[1], pair_id);
    test_int(p->x, 30);
    test_int(p->y, 40);

    p = ecs_get_id(world, ids[2], pair_id);
    test_int(p->x, 50);
    test_int(p->y, 60);

    ecs_fini(world);
}

void DeferredActions_defer_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);

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

void DeferredActions_defer_add_two() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, 0);

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

void DeferredActions_defer_remove() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);

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

void DeferredActions_defer_remove_two() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new_id(world);
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

void DeferredActions_defer_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    ecs_set(world, e, Position, {1, 2});

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_defer_begin(world);
    
    ecs_set(world, e, Position, {3, 4});

    test_assert(ecs_has(world, e, Position));
    p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Position));  
    p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);  

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

void DeferredActions_defer_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);

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

void DeferredActions_defer_twice() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, Position);

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

void DeferredActions_defer_twice_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, Position);

    ecs_frame_begin(world, 0);
    ecs_staging_begin(world);
    ecs_world_t *stage = ecs_get_stage(world, 0);

    ecs_defer_begin(stage);
    ecs_defer_begin(stage);

    ecs_set(stage, e, Velocity, {1, 2});

    ecs_defer_end(stage);
    test_assert(!ecs_has(stage, e, Velocity));
    ecs_defer_end(stage);
    test_assert(!ecs_has(stage, e, Velocity));

    ecs_staging_end(world);
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
    ecs_id_t ecs_id(Velocity) = ecs_term_id(it, 2);

    ecs_defer_begin(it->world);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add(it->world, it->entities[i], Velocity);
    }

    ecs_defer_end(it->world);
}

void DeferredActions_run_w_defer() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, AddVelocity, 0, Position, [in] :Velocity);

    ecs_entity_t e = ecs_new(world, Position);

    ecs_run(world, AddVelocity, 0, NULL);

    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void DeferredActions_system_in_progress_w_defer() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, AddVelocity, EcsOnUpdate, Position, [in] :Velocity);

    ecs_entity_t e = ecs_new(world, Position);

    ecs_progress(world, 0);

    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

static bool on_set_invoked = 0;

static
void OnSetVelocity(ecs_iter_t *it) {
    on_set_invoked = 1;
}

void DeferredActions_defer_get_mut_no_modify() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, OnSetVelocity, EcsOnSet, Velocity);

    ecs_entity_t e = ecs_new(world, Position);

    ecs_defer_begin(world);

    Velocity *v = ecs_get_mut(world, e, Velocity, NULL);
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

void DeferredActions_defer_get_mut_w_modify() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, OnSetVelocity, EcsOnSet, Velocity);

    ecs_entity_t e = ecs_new(world, Position);

    ecs_defer_begin(world);

    Velocity *v = ecs_get_mut(world, e, Velocity, NULL);
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

void DeferredActions_defer_modify() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, OnSetVelocity, EcsOnSet, Velocity);

    ecs_entity_t e = ecs_new(world, Velocity);

    ecs_defer_begin(world);

    ecs_modified(world, e, Velocity);

    test_assert(!on_set_invoked);

    ecs_defer_end(world);

    test_assert(on_set_invoked);

    ecs_fini(world);
}

void DeferredActions_defer_set_pair() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, 0);

    ecs_defer_begin(world);

    ecs_set_pair(world, e, Velocity, ecs_id(Position), {1, 2});

    ecs_defer_end(world);

    test_assert(ecs_has_pair(world, e, ecs_id(Velocity), ecs_id(Position)));

    ecs_fini(world);    
}

void DeferredActions_defer_clear() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);

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

void DeferredActions_defer_add_after_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, Position);

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

void DeferredActions_defer_set_after_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, Position);

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

void DeferredActions_defer_get_mut_after_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, Position);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_delete(world, e);
    Velocity *v = ecs_get_mut(world, e, Velocity, NULL);
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

void DeferredActions_defer_get_mut_after_delete_2nd_to_last() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, Position);

    /* Create 2nd position. This will cause deletion of the entity in the sparse
     * set to take a different path since it's not the last. */
    ecs_new(world, Position);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_delete(world, e);
    Velocity *v = ecs_get_mut(world, e, Velocity, NULL);
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

void DeferredActions_defer_add_child_to_deleted_parent() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t parent = ecs_new(world, 0);

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

void DeferredActions_recreate_deleted_entity_while_deferred() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e_old = ecs_new(world, 0);
    test_assert(e_old != 0);

    ecs_delete(world, e_old);
    test_assert(!ecs_is_alive(world, e_old));

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_defer_begin(world);
    
    ecs_entity_t e = ecs_new(world, Position);
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

void DeferredActions_defer_add_to_recycled_id() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t id = ecs_new_id(world);
    test_assert(id != 0);

    ecs_delete(world, id);

    ecs_entity_t id_2 = ecs_new_id(world);
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

void DeferredActions_defer_add_to_recycled_id_w_role() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t id = ecs_new_id(world);
    test_assert(id != 0);

    ecs_delete(world, id);

    ecs_entity_t id_2 = ecs_new_id(world);
    test_assert(id != id_2);
    test_assert((int32_t)id == (int32_t)id_2);
    
    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_entity_t child = ecs_new_w_id(world, ECS_DISABLED | id_2);
    ecs_add(world, child, Velocity);

    ecs_defer_end(world);  

    ecs_frame_end(world);    

    test_assert(!ecs_is_alive(world, id));
    test_assert(ecs_is_alive(world, id_2));
    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_has_id(world, child, ECS_DISABLED | id_2));

    ecs_fini(world); 
}

void DeferredActions_defer_add_to_recycled_relation() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t object = ecs_new_id(world);
    test_assert(object != 0);

    ecs_entity_t rel = ecs_new_id(world);
    test_assert(rel != 0);

    ecs_delete(world, rel);

    ecs_entity_t rel_2 = ecs_new_id(world);
    test_assert(rel != rel_2);
    test_assert((int32_t)rel == (int32_t)rel_2);
    
    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_entity_t child = ecs_new_w_pair(world, rel_2, object);
    ecs_add(world, child, Velocity);

    ecs_defer_end(world);  

    ecs_frame_end(world);    

    test_assert(ecs_is_alive(world, object));
    test_assert(!ecs_is_alive(world, rel));
    test_assert(ecs_is_alive(world, rel_2));
    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_has_pair(world, child, rel_2, object));

    ecs_fini(world); 
}

void DeferredActions_defer_add_to_recycled_object() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Rel);

    ecs_entity_t object = ecs_new(world, 0);
    test_assert(object != 0);

    ecs_delete(world, object);

    ecs_entity_t object_2 = ecs_new_id(world);
    test_assert(object != object_2);
    test_assert((int32_t)object == (int32_t)object_2);
    
    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_entity_t child = ecs_new_w_pair(world, Rel, object_2);
    ecs_add(world, child, Velocity);

    ecs_defer_end(world);  

    ecs_frame_end(world);    

    test_assert(!ecs_is_alive(world, object));
    test_assert(ecs_is_alive(world, object_2));
    test_assert(ecs_is_alive(world, child));
    test_assert(ecs_has_pair(world, child, Rel, object_2));

    ecs_fini(world); 
}

void DeferredActions_defer_add_to_recycled_object_childof() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t parent = ecs_new(world, 0);
    test_assert(parent != 0);

    ecs_delete(world, parent);

    ecs_entity_t parent_2 = ecs_new_id(world);
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

void DeferredActions_defer_add_to_deleted_id() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t id = ecs_new_id(world);
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

void DeferredActions_defer_add_to_deleted_id_w_role() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t id = ecs_new_id(world);
    test_assert(id != 0);
    
    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_delete(world, id);
    ecs_entity_t child = ecs_new_w_id(world, ECS_DISABLED | id);
    ecs_add(world, child, Velocity);

    ecs_defer_end(world);  

    ecs_frame_end(world);    

    test_assert(!ecs_is_alive(world, id));
    test_assert(ecs_is_alive(world, child));
    test_assert(!ecs_has_id(world, child, ECS_DISABLED | id));

    ecs_fini(world); 
}

void DeferredActions_defer_add_to_deleted_relation() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t object = ecs_new_id(world);
    test_assert(object != 0);

    ecs_entity_t rel = ecs_new_id(world);
    test_assert(rel != 0);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_delete(world, rel);
    ecs_entity_t child = ecs_new_w_pair(world, rel, object);
    ecs_add(world, child, Velocity);

    ecs_defer_end(world);  

    ecs_frame_end(world);    

    test_assert(ecs_is_alive(world, object));
    test_assert(!ecs_is_alive(world, rel));
    test_assert(ecs_is_alive(world, child));
    test_assert(!ecs_has_pair(world, child, rel, object));

    ecs_fini(world); 
}

void DeferredActions_defer_add_to_deleted_object() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TAG(world, Rel);

    ecs_entity_t object = ecs_new(world, 0);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_delete(world, object);
    ecs_entity_t child = ecs_new_w_pair(world, Rel, object);
    ecs_add(world, child, Velocity);

    ecs_defer_end(world);  

    ecs_frame_end(world);    

    test_assert(!ecs_is_alive(world, object));
    test_assert(ecs_is_alive(world, child));
    test_assert(!ecs_has_pair(world, child, Rel, object));

    ecs_fini(world); 
}

void DeferredActions_defer_add_to_deleted_object_childof() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t parent = ecs_new(world, 0);

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

void DeferredActions_defer_delete_added_id() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t id = ecs_new_id(world);
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

void DeferredActions_defer_delete_added_id_w_role() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t id = ecs_new_id(world);
    test_assert(id != 0);
    
    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_entity_t child = ecs_new_w_id(world, ECS_DISABLED | id);
    ecs_add(world, child, Velocity);
    ecs_delete(world, id);

    ecs_defer_end(world);  

    ecs_frame_end(world);    

    test_assert(!ecs_is_alive(world, id));
    test_assert(ecs_is_alive(world, child));
    test_assert(!ecs_has_id(world, child, ECS_DISABLED | id));

    ecs_fini(world);
}

void DeferredActions_defer_delete_added_relation() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t object = ecs_new_id(world);
    test_assert(object != 0);

    ecs_entity_t rel = ecs_new_id(world);
    test_assert(rel != 0);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_entity_t child = ecs_new_w_pair(world, rel, object);
    ecs_add(world, child, Velocity);
    ecs_delete(world, rel);

    ecs_defer_end(world);  

    ecs_frame_end(world);    

    test_assert(ecs_is_alive(world, object));
    test_assert(!ecs_is_alive(world, rel));
    test_assert(ecs_is_alive(world, child));
    test_assert(!ecs_has_pair(world, child, rel, object));

    ecs_fini(world); 
}

void DeferredActions_defer_delete_added_object() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TAG(world, Rel);

    ecs_entity_t object = ecs_new(world, 0);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_entity_t child = ecs_new_w_pair(world, Rel, object);
    ecs_add(world, child, Velocity);
    ecs_delete(world, object);

    ecs_defer_end(world);  

    ecs_frame_end(world);    

    test_assert(!ecs_is_alive(world, object));
    test_assert(ecs_is_alive(world, child));
    test_assert(!ecs_has_pair(world, child, Rel, object));

    ecs_fini(world);
}

void DeferredActions_defer_delete_added_object_childof() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t parent = ecs_new(world, 0);

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

void DeferredActions_discard_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);

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

void DeferredActions_discard_remove() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);

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

void DeferredActions_discard_add_two() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new_id(world);

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

void DeferredActions_discard_remove_two() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new_id(world);
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

void DeferredActions_discard_child() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_defer_begin(world);
    
    ecs_entity_t e = ecs_new(world, 0);
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

void DeferredActions_discard_child_w_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_frame_begin(world, 1);

    ecs_defer_begin(world);

    ecs_defer_begin(world);
    
    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);
    test_assert(!ecs_has(world, e, Position));
    ecs_delete(world, e);
    test_assert(ecs_is_alive(world, e));    

    ecs_entity_t child = ecs_new(world, 0);
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

void DeferredActions_defer_return_value() {
    ecs_world_t *world = ecs_init();

    test_bool(ecs_defer_begin(world), true);

    test_bool(ecs_defer_begin(world), false);

    test_bool(ecs_defer_end(world), false);

    test_bool(ecs_defer_end(world), true);

    ecs_fini(world);
}
 
void DeferredActions_defer_get_mut_pair() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Pair);

    ecs_entity_t e = ecs_new_id(world);

    ecs_defer_begin(world);

    Position *p = ecs_get_mut_pair(world, e, Position, Pair, NULL);
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

void DeferredActions_async_stage_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_id(world);

    ecs_world_t *async = ecs_async_stage_new(world);
    ecs_add(async, e, Position);
    test_assert(!ecs_has(world, e, Position));
    ecs_merge(async);
    test_assert(ecs_has(world, e, Position));

    ecs_async_stage_free(async);

    ecs_fini(world);
}

void DeferredActions_async_stage_add_twice() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new_id(world);

    ecs_world_t *async = ecs_async_stage_new(world);
    ecs_add(async, e, Position);
    test_assert(!ecs_has(world, e, Position));
    ecs_merge(async);
    test_assert(ecs_has(world, e, Position));

    ecs_add(async, e, Velocity);
    test_assert(!ecs_has(world, e, Velocity));
    ecs_merge(async);
    test_assert(ecs_has(world, e, Velocity));

    ecs_async_stage_free(async);

    ecs_fini(world);
}


void DeferredActions_async_stage_remove() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);

    ecs_world_t *async = ecs_async_stage_new(world);
    ecs_remove(async, e, Position);
    test_assert(ecs_has(world, e, Position));
    ecs_merge(async);
    test_assert(!ecs_has(world, e, Position));

    ecs_async_stage_free(async);

    ecs_fini(world);
}

void DeferredActions_async_stage_clear() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, Position);
    ecs_add(world, e, Velocity);

    ecs_world_t *async = ecs_async_stage_new(world);
    ecs_clear(async, e);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    ecs_merge(async);
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));

    ecs_async_stage_free(async);

    ecs_fini(world);
}

void DeferredActions_async_stage_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, Position);
    ecs_add(world, e, Velocity);

    ecs_world_t *async = ecs_async_stage_new(world);
    ecs_delete(async, e);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(ecs_is_alive(world, e));
    ecs_merge(async);
    test_assert(!ecs_is_alive(world, e));

    ecs_async_stage_free(async);

    ecs_fini(world);
}

void DeferredActions_async_stage_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_world_t *async = ecs_async_stage_new(world);
    ecs_entity_t e = ecs_new(async, 0);
    ecs_add(async, e, Position);
    ecs_add(async, e, Velocity);
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    ecs_merge(async);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity)); 
    test_assert(ecs_is_alive(world, e));

    ecs_async_stage_free(async);

    ecs_fini(world);
}

void DeferredActions_async_stage_no_get() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);

    ecs_world_t *async = ecs_async_stage_new(world);
    
    test_expect_abort();
    ecs_get(async, e, Position);
}

void DeferredActions_async_stage_readonly() {
    ecs_world_t *world = ecs_init();

    ecs_world_t *async = ecs_async_stage_new(world);
    test_assert(!ecs_stage_is_readonly(async));

    ecs_async_stage_free(async);
    ecs_fini(world);
}

void DeferredActions_async_stage_is_async() {
    ecs_world_t *world = ecs_init();

    ecs_world_t *async = ecs_async_stage_new(world);
    test_assert(ecs_stage_is_async(async));

    ecs_async_stage_free(async);
    ecs_fini(world);
}

static
void RegisterComponent(ecs_iter_t *it) {
    ecs_entity_t ecs_id(Position) = ecs_component_init(it->world, 
        &(ecs_component_desc_t){
            .entity = {.name = "Position"},
            .size = sizeof(Position),
            .alignment = ECS_ALIGNOF(Position)
        });    

    test_assert(ecs_id(Position) != 0);
}

void DeferredActions_register_component_while_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_SYSTEM(world, RegisterComponent, EcsOnUpdate, 0);

    ecs_progress(world, 0);

    ecs_entity_t ecs_id(Position) = ecs_lookup(world, "Position");
    test_assert(ecs_id(Position) != 0);
    test_assert(ecs_has(world, ecs_id(Position), EcsComponent));

    ecs_entity_t e = ecs_new_id(world);
    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has_id(world, e, ecs_id(Position)));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void DeferredActions_register_component_while_staged() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_id(world);
    ecs_entity_t canary = ecs_new_id(world);

    ecs_staging_begin(world);

    ecs_world_t *stage = ecs_get_stage(world, 0);

    ecs_add_id(stage, canary, Tag);

    ecs_entity_t ecs_id(Position) = ecs_component_init(stage, 
        &(ecs_component_desc_t){
            .entity = {.name = "Position"},
            .size = sizeof(Position),
            .alignment = ECS_ALIGNOF(Position)
        });

    test_assert(ecs_id(Position) != 0);

    ecs_set(stage, e, Position, {10, 20});
    test_assert(!ecs_has_id(world, e, ecs_id(Position)));
    test_assert(!ecs_has_id(world, canary, Tag));

    ecs_staging_end(world);

    test_assert(ecs_has_id(world, e, ecs_id(Position)));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void DeferredActions_register_component_while_deferred() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_id(world);
    ecs_entity_t canary = ecs_new_id(world);

    ecs_defer_begin(world);

    ecs_add_id(world, canary, Tag);

    ecs_entity_t ecs_id(Position) = ecs_component_init(world, 
        &(ecs_component_desc_t){
            .entity = {.name = "Position"},
            .size = sizeof(Position),
            .alignment = ECS_ALIGNOF(Position)
        });

    test_assert(ecs_id(Position) != 0);

    ecs_set(world, e, Position, {10, 20});
    test_assert(!ecs_has_id(world, e, ecs_id(Position)));
    test_assert(!ecs_has_id(world, canary, Tag));

    ecs_defer_end(world);

    ecs_fini(world);
}

void DeferredActions_defer_enable() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_id(world);

    ecs_defer_begin(world);
    ecs_enable(world, e, false);
    test_assert(!ecs_has_id(world, e, EcsDisabled));
    ecs_defer_end(world);

    test_assert(ecs_has_id(world, e, EcsDisabled));

    ecs_fini(world);
}

void DeferredActions_defer_disable() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_w_id(world, EcsDisabled);

    ecs_defer_begin(world);
    ecs_enable(world, e, true);
    test_assert(ecs_has_id(world, e, EcsDisabled));
    ecs_defer_end(world);

    test_assert(!ecs_has_id(world, e, EcsDisabled));

    ecs_fini(world);
}
