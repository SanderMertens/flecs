#include <api.h>

void Set_set_empty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    
    ecs_fini(world);
}

void Set_set_nonempty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_set(world, e, Velocity, {10, 20});
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    const Velocity *v = ecs_get(world, e, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 10);
    test_int(v->y, 20);
    
    ecs_fini(world);
}

void Set_set_non_empty_override() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    
    ecs_fini(world);
}

void Set_set_again() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_set(world, e, Position, {30, 40});
    test_assert(ecs_has(world, e, Position));

    p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 40);
    
    ecs_fini(world);
}

void Set_set_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_set(world, e, Velocity, {30, 40});
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Velocity *v = ecs_get(world, e, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);
    
    ecs_fini(world);
}

void Set_add_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));

    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    
    ecs_fini(world);
}

void Set_set_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));

    p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    
    ecs_fini(world);
}

void Set_set_add_other() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_add(world, e, Velocity);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    
    ecs_fini(world);
}

void Set_set_remove() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));

    p = ecs_get(world, e, Position);
    test_assert(p == NULL);
    
    ecs_fini(world);
}

void Set_set_remove_other() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, Velocity);
    test_assert(e != 0);

    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_remove(world, e, Velocity);
    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));

    p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    
    ecs_fini(world);
}

void Set_set_remove_twice() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));

    p = ecs_get(world, e, Position);
    test_assert(p == NULL);

    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));

    p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    
    ecs_fini(world);
}

void Set_set_and_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Position));
    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Set_set_null() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_set_ptr(world, e, Position, NULL);
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Position));
    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 0);
    test_int(p->y, 0);

    ecs_fini(world);
}

void Set_get_mut_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    bool is_added = false;
    Position *p = ecs_get_mut(world, e, Position, &is_added);
    test_assert(p != NULL);
    test_bool(is_added, true);
    test_assert( ecs_has(world, e, Position));
    test_assert(p == ecs_get(world, e, Position));
    
    ecs_fini(world);
}

void Set_get_mut_existing() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));
    const Position *p_prev = ecs_get(world, e, Position);

    bool is_added = false;
    Position *p = ecs_get_mut(world, e, Position, &is_added);
    test_assert(p != NULL);
    test_bool(is_added, false);
    test_assert( ecs_has(world, e, Position));
    test_assert(p == p_prev);
    
    ecs_fini(world);
}

void Set_get_mut_tag_new() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_TAG(world, MyTag);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_expect_abort();

    bool is_added = false;
    ecs_get_mut_id(world, e, MyTag, &is_added);
}

void Set_get_mut_tag_existing() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_TAG(world, MyTag);

    ecs_entity_t e = ecs_new(world, MyTag);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, MyTag));

    test_expect_abort();

    bool is_added = false;
    ecs_get_mut_id(world, e, MyTag, &is_added);
}

void Set_get_mut_tag_new_w_comp() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, MyTag);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    test_expect_abort();

    bool is_added = false;
    ecs_get_mut_id(world, e, MyTag, &is_added);
}

void Set_get_mut_tag_existing_w_comp() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, MyTag);

    ecs_entity_t e = ecs_new(world, MyTag);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, MyTag));
    ecs_add(world, e, Position);

    test_expect_abort();

    bool is_added = false;
    ecs_get_mut_id(world, e, MyTag, &is_added);
}

void Set_get_mut_tag_new_w_pair() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Pair);
    ECS_TAG(world, MyTag);

    ecs_entity_t e = ecs_new_w_id(world, ecs_pair(ecs_id(Position), Pair));
    test_assert(e != 0);

    test_expect_abort();

    bool is_added = false;
    ecs_get_mut_id(world, e, MyTag, &is_added);
}

void Set_get_mut_tag_existing_w_pair() {
    install_test_abort();
    
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Pair);
    ECS_TAG(world, MyTag);

    ecs_entity_t e = ecs_new(world, MyTag);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, MyTag));
    ecs_add_pair(world, e, Pair, ecs_id(Position));

    test_expect_abort();

    bool is_added = false;
    ecs_get_mut_id(world, e, MyTag, &is_added);
}

static bool is_invoked = false;

void OnSetPosition(ecs_iter_t *it) {
    is_invoked = true;
}

void Set_modified_w_on_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, OnSetPosition, EcsOnSet, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    bool is_added = false;
    Position *p = ecs_get_mut(world, e, Position, &is_added);
    test_assert(p != NULL);
    test_bool(is_added, true);
    test_assert( ecs_has(world, e, Position));
    test_assert(p == ecs_get(world, e, Position));

    test_assert(is_invoked == false);
    ecs_modified(world, e, Position);
    test_assert(is_invoked == true);
    
    ecs_fini(world);
}

void Set_modified_no_component() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, OnSetPosition, EcsOnSet, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_expect_abort();

    /* This should trigger an assert */
    ecs_modified(world, e, Position);

    ecs_fini(world);
}

extern ECS_COMPONENT_DECLARE(Position);
extern ECS_COMPONENT_DECLARE(Velocity);

static
void OnAdd(ecs_iter_t *it) {
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add(it->world, it->entities[i], Velocity);
    }
}

static
void OnAddRemove(ecs_iter_t *it) {
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_remove(it->world, it->entities[i], Position);
    }
}

void Set_get_mut_w_add_in_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);

    ECS_TRIGGER(world, OnAdd, EcsOnAdd, Position);

    ecs_entity_t e = ecs_new_id(world);

    bool added = false;
    Position *p = ecs_get_mut(world, e, Position, &added);
    test_assert(p != NULL);
    test_bool(added, true);
    p->x = 10;
    p->y = 20;

    const Position *pc = ecs_get(world, e, Position);
    test_int(pc->x, 10);
    test_int(pc->y, 20);

    ecs_fini(world);
}

void Set_get_mut_w_remove_in_on_add() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TRIGGER(world, OnAddRemove, EcsOnAdd, Position);

    ecs_entity_t e = ecs_new_id(world);

    test_expect_abort();

    /* get_mut is guaranteed to always return a valid pointer, so removing the
     * component from the OnAdd trigger is not allowed */
    bool added = false;
    ecs_get_mut(world, e, Position, &added);
}

void Set_emplace() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);

    Position *p = ecs_emplace(world, e, Position);
    test_assert(p != NULL);
    test_assert(ecs_has(world, e, Position));
    test_assert(p == ecs_get(world, e, Position));

    ecs_fini(world);
}

void Set_emplace_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);

    Position *p = ecs_emplace(world, e, Position);
    test_assert(p != NULL);
    test_assert(ecs_has(world, e, Position));
    test_assert(p == ecs_get(world, e, Position));

    Velocity *v = ecs_emplace(world, e, Velocity);
    test_assert(v != NULL);
    test_assert(ecs_has(world, e, Velocity));
    test_assert(v == ecs_get(world, e, Velocity));  

    ecs_fini(world);
}

void Set_emplace_existing() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(ecs_has(world, e, Position));
    test_assert(e != 0);

    test_expect_abort();
    ecs_emplace(world, e, Position);
}

void Set_emplace_w_move() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    // Create entity with name, as name is component that is moved
    ecs_entity_t e = ecs_set_name(world, 0, "Foo");
    test_assert(e != 0);
    test_str("Foo", ecs_get_name(world, e));

    Position *p = ecs_emplace(world, e, Position);
    test_assert(p != NULL);
    test_assert(ecs_has(world, e, Position));
    test_assert(p == ecs_get(world, e, Position));

    test_str("Foo", ecs_get_name(world, e));

    ecs_fini(world);
}
