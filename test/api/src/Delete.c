#include <api.h>

void Delete_setup() {
    ecs_log_set_level(-3);
}

void Delete_delete_1() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_delete(world, e);

    test_assert(ecs_exists(world, e));
    test_assert(!ecs_is_alive(world, e));
    
    ecs_fini(world);
}

void Delete_delete_1_again() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_delete(world, e);
    test_assert(ecs_exists(world, e));
    test_assert(!ecs_is_alive(world, e));

    ecs_delete(world, e);
    test_assert(ecs_exists(world, e));
    test_assert(!ecs_is_alive(world, e));
    
    ecs_fini(world);
}

void Delete_delete_recycled_tag_again() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);

    ecs_delete(world, e);
    test_assert(ecs_exists(world, e));
    test_assert(!ecs_is_alive(world, e));

    ecs_entity_t t = ecs_new_id(world);
    test_assert(t != 0);
    test_assert((uint32_t)t == (uint32_t)e);
    test_assert(ecs_exists(world, e));
    test_assert(!ecs_is_alive(world, e));
    test_assert(ecs_exists(world, t));
    test_assert(ecs_is_alive(world, t));

    ecs_entity_t f = ecs_new_id(world);
    ecs_add_id(world, f, t);

    ecs_delete(world, e);
    test_assert(ecs_exists(world, e));
    test_assert(!ecs_is_alive(world, e));
    test_assert(ecs_exists(world, t));
    test_assert(ecs_is_alive(world, t));

    test_assert(ecs_has_id(world, f, t));

    ecs_fini(world);
}

void Delete_delete_empty() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_delete(world, e);
    test_assert(ecs_exists(world, e));
    test_assert(!ecs_is_alive(world, e));
    
    ecs_fini(world);
}

void Delete_delete_nonexist() {
    ecs_world_t *world = ecs_mini();

    ecs_delete(world, 100);
    test_assert(!ecs_exists(world, 100));
    test_assert(!ecs_is_alive(world, 100));
    
    ecs_fini(world);
}

void Delete_delete_1st_of_3() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_entity_t e3 = ecs_new(world, Position);
    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    ecs_delete(world, e1);
    test_assert(ecs_exists(world, e1));
    test_assert(!ecs_is_alive(world, e1));

    test_assert(ecs_exists(world, e2));
    test_assert(ecs_is_alive(world, e2));

    test_assert(ecs_exists(world, e3));
    test_assert(ecs_is_alive(world, e3));

    ecs_fini(world);
}

void Delete_delete_2nd_of_3() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_entity_t e3 = ecs_new(world, Position);
    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    ecs_delete(world, e2);

    test_assert(ecs_exists(world, e1));
    test_assert(ecs_is_alive(world, e1));

    test_assert(ecs_exists(world, e2));
    test_assert(!ecs_is_alive(world, e2));

    test_assert(ecs_exists(world, e3));
    test_assert(ecs_is_alive(world, e3));
    
    ecs_fini(world);
}

void Delete_delete_3rd_of_3() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_entity_t e3 = ecs_new(world, Position);
    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    ecs_delete(world, e3);

    test_assert(ecs_exists(world, e1));
    test_assert(ecs_is_alive(world, e1));

    test_assert(ecs_exists(world, e2));
    test_assert(ecs_is_alive(world, e2));

    test_assert(ecs_exists(world, e3));
    test_assert(!ecs_is_alive(world, e3));
    
    ecs_fini(world);
}

void Delete_delete_2_of_3() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_entity_t e3 = ecs_new(world, Position);
    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    ecs_delete(world, e2);
    ecs_delete(world, e3);

    test_assert(ecs_exists(world, e1));
    test_assert(ecs_is_alive(world, e1));

    test_assert(ecs_exists(world, e2));
    test_assert(!ecs_is_alive(world, e2));

    test_assert(ecs_exists(world, e3));
    test_assert(!ecs_is_alive(world, e3));
    
    ecs_fini(world);
}

void Delete_delete_3_of_3() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_entity_t e3 = ecs_new(world, Position);
    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);

    test_assert(ecs_exists(world, e1));
    test_assert(!ecs_is_alive(world, e1));

    test_assert(ecs_exists(world, e2));
    test_assert(!ecs_is_alive(world, e2));

    test_assert(ecs_exists(world, e3));
    test_assert(!ecs_is_alive(world, e3));

    ecs_fini(world);
}

static
void CreateEntity(ecs_iter_t *it) {
    ecs_id_t ecs_id(Position) = ecs_term_id(it, 1);
    const ecs_entity_t *ids = ecs_bulk_new(it->world, Position, 10);
    test_assert(ids != NULL);
}

static
void DeleteEntity(ecs_iter_t *it) {
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_delete(it->world, it->entities[i]);
    }
}

static int on_remove_system_invoked;

static
void OnRemoveSystem(ecs_iter_t *it) {
    int i;
    for (i = 0; i < it->count; i ++) {
        on_remove_system_invoked ++;
    }
}

void Delete_delete_w_on_remove() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, CreateEntity, EcsOnUpdate, [out] Position());
    ECS_SYSTEM(world, DeleteEntity, EcsOnStore, Position);
    ECS_OBSERVER(world, OnRemoveSystem, EcsOnRemove, Position);

    ecs_progress(world, 0);

    test_int( ecs_count(world, Position), 0);
    test_int(on_remove_system_invoked, 10);
    
    ecs_fini(world);
}

void Delete_clear_1_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_clear(world, e);
    test_assert(!ecs_get_type(world, e));

    ecs_entity_t e2 = ecs_new(world, 0);
    test_assert(e2 > e);
    
    ecs_fini(world);
}

void Delete_clear_2_components() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, Position);
    ecs_add(world, e, Velocity);
    test_assert(e != 0);

    ecs_clear(world, e);
    test_assert(!ecs_get_type(world, e));

    ecs_entity_t e2 = ecs_new(world, 0);
    test_assert(e2 > e);
    
    ecs_fini(world);
}

void Delete_alive_after_delete() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));

    ecs_delete(world, e);
    
    test_assert(!ecs_is_alive(world, e));
    test_assert(ecs_exists(world, e));
    
    ecs_fini(world);
}

void Delete_alive_after_clear() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));

    ecs_clear(world, e);

    test_assert(!ecs_get_type(world, e));
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_exists(world, e));
    
    ecs_fini(world);
}

void Delete_alive_after_staged_delete() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));

    ecs_defer_begin(world);
    ecs_delete(world, e);
    ecs_defer_end(world);
    
    test_assert(ecs_exists(world, e));
    test_assert(!ecs_is_alive(world, e));
    
    ecs_fini(world);
}

void Delete_alive_while_staged() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));

    ecs_defer_begin(world);
    test_assert(ecs_is_alive(world, e));
    ecs_defer_end(world);
    
    ecs_fini(world);
}

void Delete_alive_while_staged_w_delete() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));

    ecs_defer_begin(world);
    test_assert(ecs_is_alive(world, e));
    ecs_delete(world, e);
    test_assert(ecs_is_alive(world, e));
    ecs_defer_end(world);

    test_assert(!ecs_is_alive(world, e));
    test_assert(ecs_exists(world, e));
    
    ecs_fini(world);
}

void Delete_alive_while_staged_w_delete_recycled_id() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));

    ecs_delete(world, e);
    e = ecs_new(world, 0);
    test_assert(ecs_is_alive(world, e));

    ecs_defer_begin(world);
    test_assert(ecs_is_alive(world, e));
    ecs_delete(world, e);
    test_assert(ecs_is_alive(world, e));
    ecs_defer_end(world);

    test_assert(!ecs_is_alive(world, e));
    test_assert(ecs_exists(world, e));
    
    ecs_fini(world);
}

void Delete_alive_after_recycle() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));

    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));
    test_assert(ecs_exists(world, e));

    ecs_entity_t e2 = ecs_new(world, 0);
    test_assert(e2 != 0);
    test_assert(ecs_is_alive(world, e2));
    test_assert(e != e2);
    test_assert((ECS_ENTITY_MASK & e) == (ECS_ENTITY_MASK & e2));
    
    ecs_fini(world);
}

void Delete_delete_recycled() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world, 0);
    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));
    test_assert(ecs_exists(world, e));

    ecs_entity_t e2 = ecs_new(world, 0);
    test_assert(e != e2);
    test_assert((e2 & ECS_ENTITY_MASK) == (e & ECS_ENTITY_MASK));
    test_assert(!ecs_is_alive(world, e));
    test_assert(ecs_is_alive(world, e2));

    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));
    test_assert(ecs_is_alive(world, e2));

    ecs_fini(world);
}

void Delete_get_alive_for_alive() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_exists(world, e));

    ecs_entity_t a = ecs_get_alive(world, e);
    test_assert(a != 0);
    test_assert(e == a);

    ecs_fini(world);
}

void Delete_get_alive_for_recycled() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);
    
    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));
    test_assert(ecs_exists(world, e));

    e = ecs_new(world, 0);
    test_assert(ecs_entity_t_lo(e) != e); // Ensure id is recycled

    ecs_entity_t a = ecs_get_alive(world, ecs_entity_t_lo(e));
    test_assert(a != 0);
    test_assert(e == a);

    ecs_fini(world);
}

void Delete_get_alive_for_not_alive() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);
    
    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));
    test_assert(ecs_exists(world, e));

    // Ensure id has no generation
    test_assert(ecs_entity_t_lo(e) == e);

    ecs_entity_t a = ecs_get_alive(world, e);
    test_assert(a == 0);

    ecs_fini(world);
}

void Delete_get_alive_w_generation_for_recycled_alive() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);
    
    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));
    test_assert(ecs_exists(world, e));

    e = ecs_new(world, 0);
    test_assert(ecs_entity_t_lo(e) != e);

    ecs_entity_t a = ecs_get_alive(world, e);
    test_assert(a != 0);
    test_assert(e == a);

    ecs_fini(world);
}

void Delete_get_alive_w_generation_for_recycled_not_alive() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);
    
    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));
    test_assert(ecs_exists(world, e));

    e = ecs_new(world, 0);
    test_assert(ecs_entity_t_lo(e) != e);

    ecs_delete(world, e);

    test_expect_abort();

    // Will assert, because function is called with generation that is not alive
    ecs_get_alive(world, e);
}

void Delete_get_alive_for_0() {
    ecs_world_t *world = ecs_mini();

    test_assert(0 == ecs_get_alive(world, 0));

    ecs_fini(world);
}

void Delete_get_alive_for_nonexistent() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_get_alive(world, 1000);
    test_assert(e == 0);

    ecs_fini(world);
}

static int move_invoked = 0;
static int dtor_invoked = 0;

static int dtor_x;
static int dtor_y;

static int move_dst_x;
static int move_dst_y;

static int move_src_x;
static int move_src_y;

static ECS_DTOR(Position, ptr, {
    dtor_invoked ++;
    dtor_x = ptr->x;
    dtor_y = ptr->y;
})

static ECS_MOVE(Position, dst, src, {
    move_dst_x = dst->x;
    move_dst_y = dst->y; 
    move_src_x = src->x;
    move_src_y = src->y;         
    *dst = *src;
    move_invoked ++;
})

void Delete_move_w_dtor_move() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .dtor = ecs_dtor(Position),
        .move = ecs_move(Position),
    });

    ecs_entity_t e_1 = ecs_new_id(world);
    test_assert(e_1 != 0);

    ecs_entity_t e_2 = ecs_new_id(world);
    test_assert(e_2 != 0);


    ecs_set(world, e_1, Position, {10, 20});
    ecs_set(world, e_2, Position, {30, 40}); // append after e_1

    test_int(move_invoked, 0);

    ecs_delete(world, e_1); // move e_2 to e_1

    test_assert(!ecs_is_alive(world, e_1));
    test_assert(ecs_is_alive(world, e_2));

    // counter intuitive, but this is because in this case the move is
    // responsible for cleaning up e_1, because e_2 is moved into e_1
    test_int(dtor_invoked, 1);
    test_int(dtor_x, 30);
    test_int(dtor_y, 40);

    test_int(move_invoked, 1);
    test_int(move_dst_x, 10);
    test_int(move_dst_y, 20);
    test_int(move_src_x, 30);
    test_int(move_src_y, 40);

    const Position *p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 40);

    ecs_fini(world);
}

void Delete_move_w_dtor_no_move() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .dtor = ecs_dtor(Position)
    });

    ecs_entity_t e_1 = ecs_new_id(world);
    test_assert(e_1 != 0);

    ecs_entity_t e_2 = ecs_new_id(world);
    test_assert(e_2 != 0);

    ecs_set(world, e_1, Position, {10, 20});
    ecs_set(world, e_2, Position, {30, 40}); // append after e_1

    ecs_delete(world, e_1); // move e_2 to e_1

    test_assert(!ecs_is_alive(world, e_1));
    test_assert(ecs_is_alive(world, e_2));

    test_int(dtor_invoked, 1);
    test_int(dtor_x, 10);
    test_int(dtor_y, 20);

    const Position *p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 40);

    ecs_fini(world);
}

void Delete_move_w_no_dtor_move() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .move = ecs_move(Position)
    });

    ecs_entity_t e_1 = ecs_new_id(world);
    test_assert(e_1 != 0);

    ecs_entity_t e_2 = ecs_new_id(world);
    test_assert(e_2 != 0);

    ecs_set(world, e_1, Position, {10, 20});
    ecs_set(world, e_2, Position, {30, 40}); // append after e_1

    ecs_delete(world, e_1); // move e_2 to e_1

    test_assert(!ecs_is_alive(world, e_1));
    test_assert(ecs_is_alive(world, e_2));

    test_int(move_invoked, 1);
    test_int(move_dst_x, 10);
    test_int(move_dst_y, 20);
    test_int(move_src_x, 30);
    test_int(move_src_y, 40);    

    const Position *p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 40);

    ecs_fini(world);
}

void Delete_wrap_generation_count() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t start = ecs_new_id(world);
    ecs_entity_t e = start;

    for (int i = 0; i < 65535; i ++) {
        ecs_delete(world, e);
        e = ecs_new_id(world);
        test_assert(e != start);
        test_assert((uint32_t)e == start);
    }

    ecs_delete(world, e);
    e = ecs_new_id(world);
    test_assert(e == start);

    for (int i = 0; i < 65535; i ++) {
        ecs_delete(world, e);
        e = ecs_new_id(world);
        test_assert(e != start);
        test_assert((uint32_t)e == start);
    }

    ecs_delete(world, e);
    e = ecs_new_id(world);
    test_assert(e == start);

    ecs_fini(world);
}
