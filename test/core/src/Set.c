#include <core.h>

void Set_set_empty(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    
    ecs_fini(world);
}

void Set_set_nonempty(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new_w(world, Position);
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

void Set_set_non_empty_override(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);

    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    
    ecs_fini(world);
}

void Set_set_again(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);
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

void Set_set_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world);
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

void Set_add_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);
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

void Set_set_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);
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

void Set_set_add_other(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world);
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

void Set_set_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);
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

void Set_set_remove_other(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new_w(world, Velocity);
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

void Set_set_remove_twice(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);
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

void Set_set_and_new(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Position));
    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Set_set_null(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);

    test_expect_abort();
    ecs_set_ptr(world, e, Position, NULL);
}

void Set_ensure_new(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    Position *p = ecs_ensure(world, e, Position);
    test_assert(p != NULL);
    test_assert( ecs_has(world, e, Position));
    test_assert(p == ecs_get(world, e, Position));
    
    ecs_fini(world);
}

void Set_ensure_existing(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));
    const Position *p_prev = ecs_get(world, e, Position);

    Position *p = ecs_ensure(world, e, Position);
    test_assert(p != NULL);
    test_assert( ecs_has(world, e, Position));
    test_assert(p == p_prev);
    
    ecs_fini(world);
}

void Set_ensure_tag_new(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, MyTag);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    test_expect_abort();

    ecs_ensure_id(world, e, MyTag);
}

void Set_ensure_tag_existing(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, MyTag);

    ecs_entity_t e = ecs_new_w(world, MyTag);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, MyTag));

    test_expect_abort();

    ecs_ensure_id(world, e, MyTag);
}

void Set_ensure_tag_new_w_comp(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, MyTag);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);

    test_expect_abort();

    ecs_ensure_id(world, e, MyTag);
}

void Set_ensure_tag_existing_w_comp(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, MyTag);

    ecs_entity_t e = ecs_new_w(world, MyTag);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, MyTag));
    ecs_add(world, e, Position);

    test_expect_abort();

    ecs_ensure_id(world, e, MyTag);
}

void Set_ensure_tag_new_w_pair(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Pair);
    ECS_TAG(world, MyTag);

    ecs_entity_t e = ecs_new_w_id(world, ecs_pair(ecs_id(Position), Pair));
    test_assert(e != 0);

    test_expect_abort();

    ecs_ensure_id(world, e, MyTag);
}

void Set_ensure_tag_existing_w_pair(void) {
    install_test_abort();
    
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Pair);
    ECS_TAG(world, MyTag);

    ecs_entity_t e = ecs_new_w(world, MyTag);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, MyTag));
    ecs_add_pair(world, e, Pair, ecs_id(Position));

    test_expect_abort();

    ecs_ensure_id(world, e, MyTag);
}

void Set_get_mut_not_existing(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);
    Position *p = ecs_get_mut(world, e, Position);
    test_assert(p == NULL);
    
    ecs_fini(world);
}

void Set_get_mut_existing(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    Position *p = ecs_get_mut(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    
    ecs_fini(world);
}

void Set_get_mut_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t e = ecs_new_w(world, Foo);
    test_assert(NULL == ecs_get_mut_id(world, e, Foo));
    
    ecs_fini(world);
}

void Set_get_mut_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tgt);

    ecs_entity_t e = ecs_insert(world, ecs_value_pair(Position, Tgt, {10, 20}));
    Position *p = ecs_get_mut_pair(world, e, Position, Tgt);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Set_get_mut_pair_second(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tgt);

    ecs_entity_t e = ecs_insert(world, ecs_value_pair_2nd(Tgt, Position, {10, 20}));
    Position *p = ecs_get_mut_pair_second(world, e, Tgt, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    
    ecs_fini(world);
}

static bool is_invoked = false;

void OnSetPosition(ecs_iter_t *it) {
    is_invoked = true;
}

void Set_modified_w_on_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, OnSetPosition, EcsOnSet, Position);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    Position *p = ecs_ensure(world, e, Position);
    test_assert(p != NULL);
    test_assert( ecs_has(world, e, Position));
    test_assert(p == ecs_get(world, e, Position));

    test_assert(is_invoked == false);
    ecs_modified(world, e, Position);
    test_assert(is_invoked == true);
    
    ecs_fini(world);
}

void Set_modified_no_component(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, OnSetPosition, EcsOnSet, Position);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    test_expect_abort();

    /* This should trigger an assert */
    ecs_modified(world, e, Position);

    ecs_fini(world);
}

static ECS_COMPONENT_DECLARE(Position);
static ECS_COMPONENT_DECLARE(Velocity);

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

void Set_ensure_w_add_in_on_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);

    ECS_OBSERVER(world, OnAdd, EcsOnAdd, Position);

    ecs_entity_t e = ecs_new(world);

    Position *p = ecs_ensure(world, e, Position);
    test_assert(p != NULL);
    p->x = 10;
    p->y = 20;

    const Position *pc = ecs_get(world, e, Position);
    test_int(pc->x, 10);
    test_int(pc->y, 20);

    ecs_fini(world);
}

void Set_ensure_w_remove_in_on_add(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_OBSERVER(world, OnAddRemove, EcsOnAdd, Position);

    ecs_entity_t e = ecs_new(world);

    test_expect_abort();

    /* ensure is guaranteed to always return a valid pointer, so removing the
     * component from the OnAdd trigger is not allowed */
    ecs_ensure(world, e, Position);
}

static
void OnAddRealloc(ecs_iter_t *it) {
    ecs_entity_t *entities = it->ctx;

    int i;
    for (i = 0; i < 1000; i ++) {
        ecs_set(it->world, entities[i], Velocity, {i, i * 2});
    }
}

void Set_ensure_w_realloc_in_on_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);

    ecs_entity_t *entities = ecs_os_malloc_n(ecs_entity_t, 1000);
    for (int i = 0; i < 1000; i ++) {
        entities[i] = ecs_new_w(world, Position);
    }

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .callback = OnAddRealloc,
        .events = {EcsOnAdd},
        .query.terms[0].id = ecs_id(Position),
        .ctx = entities
    });

    ecs_entity_t e = ecs_new_w(world, Velocity);
    const Position *ptr = ecs_ensure(world, e, Position);
    test_assert(ptr == ecs_get(world, e, Position));

    for (int i = 0; i < 1000; i ++) {
        test_assert( ecs_has(world, entities[i], Velocity));
        const Velocity *vptr = ecs_get(world, entities[i], Velocity);
        test_assert(vptr != NULL);
        // printf("e = {%f, %f}\n", vptr->x, vptr->y);
        test_int(vptr->x, i);
        test_int(vptr->y, i * 2);
        test_assert(ecs_get_table(world, e) == 
            ecs_get_table(world, entities[i]));
    }

    ecs_fini(world);

    ecs_os_free(entities);
}

void Set_emplace(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    Position *p = ecs_emplace(world, e, Position, NULL);
    test_assert(p != NULL);
    test_assert(ecs_has(world, e, Position));
    test_assert(p == ecs_get(world, e, Position));

    ecs_fini(world);
}

void Set_emplace_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    Position *p = ecs_emplace(world, e, Position, NULL);
    test_assert(p != NULL);
    test_assert(ecs_has(world, e, Position));
    test_assert(p == ecs_get(world, e, Position));

    Velocity *v = ecs_emplace(world, e, Velocity, NULL);
    test_assert(v != NULL);
    test_assert(ecs_has(world, e, Velocity));
    test_assert(v == ecs_get(world, e, Velocity));  

    ecs_fini(world);
}

void Set_emplace_existing(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(ecs_has(world, e, Position));
    test_assert(e != 0);

    test_expect_abort();
    ecs_emplace(world, e, Position, NULL);
}

void Set_emplace_w_move(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    // Create entity with name, as name is component that is moved
    ecs_entity_t e = ecs_set_name(world, 0, "Foo");
    test_assert(e != 0);
    test_str("Foo", ecs_get_name(world, e));

    Position *p = ecs_emplace(world, e, Position, NULL);
    test_assert(p != NULL);
    test_assert(ecs_has(world, e, Position));
    test_assert(p == ecs_get(world, e, Position));

    test_str("Foo", ecs_get_name(world, e));

    ecs_fini(world);
}

static
void OnAddMove(ecs_iter_t *it) {
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], Velocity, {1, 2});
    }
}

void Set_emplace_w_observer_w_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .callback = OnAddMove,
        .events = {EcsOnAdd},
        .query.terms[0].id = ecs_id(Position)
    });

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    Position *p = ecs_emplace(world, e, Position, NULL);
    test_assert(p != NULL);
    p->x = 10;
    p->y = 20;

    test_assert(ecs_has(world, e, Position));

    {
        const Position *pc = ecs_get(world, e, Position);
        test_assert(p == pc);
        test_int(pc->x, 10);
        test_int(pc->y, 20);

        const Velocity *vc = ecs_get(world, e, Velocity);
        test_assert(vc != NULL);
        test_int(vc->x, 1);
        test_int(vc->y, 2);
    }

    ecs_fini(world);
}

void Set_emplace_existing_w_check(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    bool is_new;
    ecs_emplace(world, e, Position, &is_new);
    test_bool(is_new, true);
    ecs_emplace(world, e, Position, &is_new);
    test_bool(is_new, false);

    ecs_fini(world);
}
