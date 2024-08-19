#include <core.h>

void Sparse_has(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    test_bool(false, ecs_has(world, e, Position));

    ecs_add(world, e, Position);
    test_bool(true, ecs_has(world, e, Position));

    ecs_fini(world);
}

void Sparse_owns(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    test_bool(false, ecs_owns(world, e, Position));

    ecs_add(world, e, Position);
    test_bool(true, ecs_owns(world, e, Position));

    ecs_fini(world);
}

void Sparse_get(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    test_assert(NULL == ecs_get(world, e, Position));

    ecs_add(world, e, Position);
    test_assert(NULL != ecs_get(world, e, Position));

    ecs_fini(world);
}

void Sparse_get_mut(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    test_assert(NULL == ecs_get_mut(world, e, Position));

    ecs_add(world, e, Position);
    test_assert(NULL != ecs_get_mut(world, e, Position));

    ecs_fini(world);
}

void Sparse_ensure(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    Position *p = ecs_ensure(world, e, Position);
    test_assert(NULL != p);

    ecs_add(world, e, Position);
    test_assert(p == ecs_get_mut(world, e, Position));

    ecs_fini(world);
}

void Sparse_emplace(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    bool is_new;
    ecs_entity_t e = ecs_new(world);
    Position *p = ecs_emplace(world, e, Position, &is_new);
    test_assert(NULL != p);
    test_bool(true, is_new);

    test_assert(p == ecs_emplace(world, e, Position, &is_new));
    test_bool(false, is_new);

    ecs_add(world, e, Foo);

    test_assert(p == ecs_emplace(world, e, Position, &is_new));
    test_bool(false, is_new);

    ecs_fini(world);
}

void Sparse_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));
    
    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Sparse_modified_no_on_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    Position *p = ecs_ensure(world, e, Position);
    test_assert(NULL != p);

    ecs_modified(world, e, Position);

    test_assert(true); // no crash

    ecs_fini(world);
}

void Sparse_insert_1(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    test_assert(ecs_has(world, e, Position));
    
    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Sparse_insert_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);

    ecs_entity_t e = ecs_insert(world, 
        ecs_value(Position, {10, 20}),
        ecs_value(Velocity, {1, 2}));

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

    ecs_fini(world);
}

void Sparse_get_ref(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});
    
    ecs_ref_t ref = ecs_ref_init(world, e, Position);

    {
        const Position *p = ecs_ref_get(world, &ref, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_add(world, e, Foo);

    {
        const Position *p = ecs_ref_get(world, &ref, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

void Sparse_update_ref(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});
    
    ecs_ref_t ref = ecs_ref_init(world, e, Position);

    {
        const Position *p = ecs_ref_get(world, &ref, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_add(world, e, Foo);
    ecs_ref_update(world, &ref);

    {
        const Position *p = ecs_ref_get(world, &ref, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

void Sparse_get_recycled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e0 = ecs_new_w(world, Position);
    ecs_delete(world, e0);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != e0);
    test_assert(e0 == (uint32_t)e);

    test_assert(NULL == ecs_get(world, e, Position));

    ecs_add(world, e, Position);
    test_assert(NULL != ecs_get(world, e, Position));

    ecs_fini(world);
}

void Sparse_get_mut_recycled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e0 = ecs_new_w(world, Position);
    ecs_delete(world, e0);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != e0);
    test_assert(e0 == (uint32_t)e);

    test_assert(NULL == ecs_get_mut(world, e, Position));

    ecs_add(world, e, Position);
    test_assert(NULL != ecs_get_mut(world, e, Position));

    ecs_fini(world);
}

void Sparse_ensure_recycled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e0 = ecs_new_w(world, Position);
    ecs_delete(world, e0);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != e0);
    test_assert(e0 == (uint32_t)e);

    Position *p = ecs_ensure(world, e, Position);
    test_assert(NULL != p);

    ecs_add(world, e, Position);
    test_assert(p == ecs_get_mut(world, e, Position));

    ecs_fini(world);
}

void Sparse_emplace_recycled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e0 = ecs_new_w(world, Position);
    ecs_delete(world, e0);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != e0);
    test_assert(e0 == (uint32_t)e);

    bool is_new;
    Position *p = ecs_emplace(world, e, Position, &is_new);
    test_assert(NULL != p);
    test_bool(true, is_new);

    test_assert(p == ecs_emplace(world, e, Position, &is_new));
    test_bool(false, is_new);

    ecs_add(world, e, Foo);

    test_assert(p == ecs_emplace(world, e, Position, &is_new));
    test_bool(false, is_new);

    ecs_fini(world);
}

void Sparse_set_recycled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e0 = ecs_new_w(world, Position);
    ecs_delete(world, e0);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != e0);
    test_assert(e0 == (uint32_t)e);

    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));
    
    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Sparse_get_ref_recycled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e0 = ecs_new_w(world, Position);
    ecs_delete(world, e0);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != e0);
    test_assert(e0 == (uint32_t)e);

    ecs_set(world, e, Position, {10, 20});
    
    ecs_ref_t ref = ecs_ref_init(world, e, Position);

    {
        const Position *p = ecs_ref_get(world, &ref, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_add(world, e, Foo);

    {
        const Position *p = ecs_ref_get(world, &ref, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

void Sparse_test_stable_ptr(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Foo);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    test_assert(NULL == ecs_get(world, e, Position));

    ecs_add(world, e, Position);
    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);

    const Velocity *v = ecs_ensure(world, e, Velocity);
    test_assert(v != NULL);
    test_assert(p == ecs_get(world, e, Position));

    ecs_add(world, e, Foo);
    test_assert(p == ecs_get(world, e, Position));
    test_assert(v != ecs_get(world, e, Velocity));

    ecs_fini(world);
}

void Sparse_has_after_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    test_bool(false, ecs_has(world, e, Position));

    ecs_add(world, e, Position);
    test_bool(true, ecs_has(world, e, Position));

    ecs_remove(world, e, Position);
    test_bool(false, ecs_has(world, e, Position));

    ecs_fini(world);
}

void Sparse_get_after_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    test_assert(NULL == ecs_get(world, e, Position));

    ecs_add(world, e, Position);
    test_assert(NULL != ecs_get(world, e, Position));

    ecs_remove(world, e, Position);
    test_assert(NULL == ecs_get(world, e, Position));

    ecs_fini(world);
}

void Sparse_get_mut_after_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    test_assert(NULL == ecs_get_mut(world, e, Position));

    ecs_add(world, e, Position);
    test_assert(NULL != ecs_get_mut(world, e, Position));

    ecs_remove(world, e, Position);
    test_assert(NULL == ecs_get_mut(world, e, Position));

    ecs_fini(world);
}

void Sparse_sparse_w_hole(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add(world, e1, Velocity);
    ecs_add(world, e1, Position);

    ecs_entity_t e2 = ecs_new(world);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e2, Position);

    ecs_delete(world, e1);
    ecs_delete(world, e2);

    e1 = ecs_new(world); // create hole in sparse set

    e2 = ecs_new(world);
    ecs_add(world, e2, Velocity);

    test_assert(!ecs_has(world, e1, Velocity));
    test_assert(ecs_has(world, e2, Velocity));

    ecs_fini(world);
}

void Sparse_record_get(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Tag);

    {
        const ecs_record_t *r = ecs_read_begin(world, e);
        test_assert(r != NULL);
        test_assert(NULL == ecs_record_get(world, r, Position));
        ecs_read_end(r);
    }

    ecs_add(world, e, Position);

    {
        const ecs_record_t *r = ecs_read_begin(world, e);
        test_assert(r != NULL);
        const Position *p = ecs_record_get(world, r, Position);
        test_assert(p != NULL);
        ecs_read_end(r);
    }

    ecs_fini(world);
}

void Sparse_has_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t b = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, b);
    test_bool(false, ecs_has(world, e, Position));

    ecs_set(world, b, Position, {10, 20});
    test_bool(true, ecs_has(world, e, Position));

    ecs_fini(world);
}

void Sparse_owns_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t b = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, b);
    test_bool(false, ecs_has(world, e, Position));
    test_bool(false, ecs_owns(world, e, Position));

    ecs_set(world, b, Position, {10, 20});
    test_bool(true, ecs_has(world, e, Position));
    test_bool(false, ecs_owns(world, e, Position));

    ecs_fini(world);
}

void Sparse_get_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t b = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, b);
    test_assert(NULL == ecs_get(world, e, Position));

    ecs_set(world, b, Position, {10, 20});

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    test_assert(p == ecs_get(world, b, Position));

    ecs_fini(world);
}

void Sparse_get_mut_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t b = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, b);
    test_assert(NULL == ecs_get_mut(world, e, Position));

    ecs_set(world, b, Position, {10, 20});
    test_assert(NULL == ecs_get_mut(world, e, Position));

    ecs_fini(world);
}

void Sparse_ensure_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t b = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, b);
    test_assert(NULL == ecs_get(world, e, Position));

    ecs_set(world, b, Position, {10, 20});

    Position *p = ecs_ensure(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    test_assert(p != ecs_get(world, b, Position));

    ecs_fini(world);
}

void Sparse_emplace_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t b = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, b);
    test_assert(NULL == ecs_get(world, e, Position));

    ecs_set(world, b, Position, {10, 20});

    bool is_new;
    Position *p = ecs_emplace(world, e, Position, &is_new);
    test_bool(is_new, true);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    test_assert(p != ecs_get(world, b, Position));

    ecs_fini(world);
}

static int position_ctor_invoked = 0;
static int position_dtor_invoked = 0;
static int position_on_add_invoked = 0;
static int position_on_remove_invoked = 0;
static int position_on_set_invoked = 0;

static ECS_CTOR(Position, ptr, {
    position_ctor_invoked ++;
    ptr->x = 10;
    ptr->y = 20;
})

static ECS_DTOR(Position, ptr, {
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
    position_dtor_invoked ++;
})

static void Position_on_add(ecs_iter_t *it) {
    test_int(1, position_ctor_invoked);
    Position *p = ecs_field_at(it, Position, 0, 0);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    test_uint(it->event, EcsOnAdd);

    position_on_add_invoked ++;
}

static void Position_on_remove(ecs_iter_t *it) {
    test_int(0, position_dtor_invoked);
    Position *p = ecs_field_at(it, Position, 0, 0);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    test_uint(it->event, EcsOnRemove);

    position_on_remove_invoked ++;
}

static void Position_on_set(ecs_iter_t *it) {
    test_int(1, position_ctor_invoked);
    test_int(0, position_dtor_invoked);
    Position *p = ecs_field_at(it, Position, 0, 0);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 40);
    test_uint(it->event, EcsOnSet);

    position_on_set_invoked ++;
}

void Sparse_ctor_after_emplace(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
    });

    ecs_entity_t e = ecs_new(world);

    bool is_new;
    ecs_emplace(world, e, Position, &is_new);
    test_bool(true, is_new);

    test_int(0, position_ctor_invoked);
    test_int(0, position_dtor_invoked);

    ecs_fini(world);
}

void Sparse_ctor_dtor_after_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .dtor = ecs_dtor(Position)
    });

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);

    test_int(1, position_ctor_invoked);
    test_int(0, position_dtor_invoked);

    {
        const Position *ptr = ecs_get(world, e, Position);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }

    ecs_remove(world, e, Position);

    test_int(1, position_ctor_invoked);
    test_int(1, position_dtor_invoked);

    ecs_fini(world);
}

void Sparse_ctor_dtor_after_clear(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .dtor = ecs_dtor(Position)
    });

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);

    test_int(1, position_ctor_invoked);
    test_int(0, position_dtor_invoked);

    {
        const Position *ptr = ecs_get(world, e, Position);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }

    ecs_clear(world, e);

    test_int(1, position_ctor_invoked);
    test_int(1, position_dtor_invoked);

    ecs_fini(world);
}

void Sparse_ctor_dtor_after_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .dtor = ecs_dtor(Position)
    });

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);

    test_int(1, position_ctor_invoked);
    test_int(0, position_dtor_invoked);

    {
        const Position *ptr = ecs_get(world, e, Position);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }

    ecs_delete(world, e);

    test_int(1, position_ctor_invoked);
    test_int(1, position_dtor_invoked);

    ecs_fini(world);
}

void Sparse_ctor_dtor_after_fini(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .dtor = ecs_dtor(Position)
    });

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);

    test_int(1, position_ctor_invoked);
    test_int(0, position_dtor_invoked);

    {
        const Position *ptr = ecs_get(world, e, Position);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }

    ecs_fini(world);

    test_int(1, position_ctor_invoked);
    test_int(1, position_dtor_invoked);
}

void Sparse_on_add_remove_after_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .dtor = ecs_dtor(Position),
        .on_add = Position_on_add,
        .on_remove = Position_on_remove,
    });

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);

    test_int(1, position_ctor_invoked);
    test_int(1, position_on_add_invoked);
    test_int(0, position_dtor_invoked);
    test_int(0, position_on_remove_invoked);

    {
        const Position *ptr = ecs_get(world, e, Position);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }

    ecs_remove(world, e, Position);

    test_int(1, position_ctor_invoked);
    test_int(1, position_on_add_invoked);
    test_int(1, position_dtor_invoked);
    test_int(1, position_on_remove_invoked);

    ecs_fini(world);
}

void Sparse_on_add_remove_after_clear(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .dtor = ecs_dtor(Position),
        .on_add = Position_on_add,
        .on_remove = Position_on_remove,
    });

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);

    test_int(1, position_ctor_invoked);
    test_int(1, position_on_add_invoked);
    test_int(0, position_dtor_invoked);
    test_int(0, position_on_remove_invoked);

    {
        const Position *ptr = ecs_get(world, e, Position);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }

    ecs_clear(world, e);

    test_int(1, position_ctor_invoked);
    test_int(1, position_on_add_invoked);
    test_int(1, position_dtor_invoked);
    test_int(1, position_on_remove_invoked);

    ecs_fini(world);
}

void Sparse_on_add_remove_after_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .dtor = ecs_dtor(Position),
        .on_add = Position_on_add,
        .on_remove = Position_on_remove,
    });

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);

    test_int(1, position_ctor_invoked);
    test_int(1, position_on_add_invoked);
    test_int(0, position_dtor_invoked);
    test_int(0, position_on_remove_invoked);

    {
        const Position *ptr = ecs_get(world, e, Position);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }

    ecs_delete(world, e);

    test_int(1, position_ctor_invoked);
    test_int(1, position_on_add_invoked);
    test_int(1, position_dtor_invoked);
    test_int(1, position_on_remove_invoked);

    ecs_fini(world);
}

void Sparse_on_add_remove_after_fini(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .dtor = ecs_dtor(Position),
        .on_add = Position_on_add,
        .on_remove = Position_on_remove,
    });

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);

    test_int(1, position_ctor_invoked);
    test_int(1, position_on_add_invoked);
    test_int(0, position_dtor_invoked);
    test_int(0, position_on_remove_invoked);

    {
        const Position *ptr = ecs_get(world, e, Position);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }

    ecs_fini(world);

    test_int(1, position_ctor_invoked);
    test_int(1, position_on_add_invoked);
    test_int(1, position_dtor_invoked);
    test_int(1, position_on_remove_invoked);
}

void Sparse_on_set_after_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .on_add = Position_on_add,
        .on_set = Position_on_set
    });

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {30, 40});

    test_int(1, position_ctor_invoked);
    test_int(1, position_on_add_invoked);
    test_int(1, position_on_set_invoked);

    {
        const Position *ptr = ecs_get(world, e, Position);
        test_int(ptr->x, 30);
        test_int(ptr->y, 40);
    }

    ecs_fini(world);
}

void Sparse_on_set_after_modified(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .on_add = Position_on_add,
        .on_set = Position_on_set
    });

    ecs_entity_t e = ecs_new(world);

    Position *p = ecs_ensure(world, e, Position);
    test_assert(p != NULL);
    p->x = 30;
    p->y = 40;
    ecs_modified(world, e, Position);

    test_int(1, position_ctor_invoked);
    test_int(1, position_on_add_invoked);
    test_int(1, position_on_set_invoked);

    {
        const Position *ptr = ecs_get(world, e, Position);
        test_int(ptr->x, 30);
        test_int(ptr->y, 40);
    }

    ecs_fini(world);
}

void Sparse_sparse_relationship(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Tgt);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    test_assert(NULL == ecs_get_pair(world, e, Position, Tgt));

    ecs_set_pair(world, e, Position, Tgt, {10, 20});
    const Position *ptr = ecs_get_pair(world, e, Position, Tgt);
    test_assert(ptr != NULL);

    ecs_add(world, e, Foo);
    test_assert(ptr == ecs_get_pair(world, e, Position, Tgt));

    ecs_fini(world);
}

void Sparse_defer_ensure(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);

    {
        Position *p = ecs_ensure(world, e, Position);
        test_assert(p != NULL);
        p->x = 10;
        p->y = 20;
    }

    test_assert(!ecs_has(world, e, Position));
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

void Sparse_defer_ensure_w_modified(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);

    {
        Position *p = ecs_ensure(world, e, Position);
        test_assert(p != NULL);
        p->x = 10;
        p->y = 20;
    }

    ecs_modified(world, e, Position);

    test_assert(!ecs_has(world, e, Position));
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

void Sparse_defer_ensure_modified(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);

    {
        Position *p = ecs_ensure_modified_id(world, e, ecs_id(Position));
        test_assert(p != NULL);
        p->x = 10;
        p->y = 20;
    }

    test_assert(!ecs_has(world, e, Position));
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

void Sparse_defer_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);

    ecs_set(world, e, Position, {10, 20});

    test_assert(!ecs_has(world, e, Position));
    ecs_defer_end(world);
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);

    ecs_fini(world);
}

void Sparse_defer_emplace(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);

    {
        bool is_new = false;
        Position *p = ecs_emplace(world, e, Position, &is_new);
        test_assert(p != NULL);
        test_bool(is_new, true);
        p->x = 10;
        p->y = 20;
    }

    test_assert(!ecs_has(world, e, Position));
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

void Sparse_defer_emplace_w_modified(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);

    {
        bool is_new = false;
        Position *p = ecs_emplace(world, e, Position, &is_new);
        test_assert(p != NULL);
        test_bool(is_new, true);
        p->x = 10;
        p->y = 20;
    }

    ecs_modified(world, e, Position);

    test_assert(!ecs_has(world, e, Position));
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

void Sparse_defer_ensure_existing(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);

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

void Sparse_defer_ensure_existing_twice(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);

    ecs_defer_begin(world);

    {
        Position *p = ecs_ensure(world, e, Position);
        test_assert(p != NULL);
        p->x = 10;
        p->y = 20;
    }

    {
        Position *p = ecs_ensure(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
        p->x = 30;
        p->y = 40;
    }

    ecs_defer_end(world);

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }

    ecs_fini(world);
}

void Sparse_defer_ensure_w_modified_existing(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);

    ecs_defer_begin(world);

    {
        Position *p = ecs_ensure(world, e, Position);
        test_assert(p != NULL);
        p->x = 10;
        p->y = 20;
    }

    ecs_modified(world, e, Position);

    ecs_defer_end(world);

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

void Sparse_defer_ensure_modified_existing(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);

    ecs_defer_begin(world);

    {
        Position *p = ecs_ensure_modified_id(world, e, ecs_id(Position));
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

void Sparse_defer_set_existing(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);

    ecs_defer_begin(world);

    ecs_set(world, e, Position, {10, 20});

    ecs_defer_end(world);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);

    ecs_fini(world);
}

void Sparse_defer_emplace_existing(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);

    ecs_defer_begin(world);

    {
        bool is_new = false;
        Position *p = ecs_emplace(world, e, Position, &is_new);
        test_assert(p != NULL);
        test_bool(is_new, false);
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

void Sparse_defer_emplace_w_modified_existing(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);

    ecs_defer_begin(world);

    {
        bool is_new = false;
        Position *p = ecs_emplace(world, e, Position, &is_new);
        test_assert(p != NULL);
        test_bool(is_new, false);
        p->x = 10;
        p->y = 20;
    }

    ecs_modified(world, e, Position);

    ecs_defer_end(world);

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

void Sparse_defer_batched_ensure(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);

    {
        Position *p = ecs_ensure(world, e, Position);
        test_assert(p != NULL);
        p->x = 10;
        p->y = 20;
    }
    {
        Velocity *p = ecs_ensure(world, e, Velocity);
        test_assert(p != NULL);
        p->x = 1;
        p->y = 2;
    }

    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    ecs_defer_end(world);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Velocity *p = ecs_get(world, e, Velocity);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
    }

    ecs_fini(world);
}

void Sparse_defer_batched_ensure_w_modified(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);

    {
        Position *p = ecs_ensure(world, e, Position);
        test_assert(p != NULL);
        p->x = 10;
        p->y = 20;
    }
    ecs_modified(world, e, Position);

    {
        Velocity *p = ecs_ensure(world, e, Velocity);
        test_assert(p != NULL);
        p->x = 1;
        p->y = 2;
    }
    ecs_modified(world, e, Velocity);

    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    ecs_defer_end(world);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Velocity *p = ecs_get(world, e, Velocity);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
    }

    ecs_fini(world);
}

void Sparse_defer_batched_ensure_modified(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);

    {
        Position *p = ecs_ensure_modified_id(world, e, ecs_id(Position));
        test_assert(p != NULL);
        p->x = 10;
        p->y = 20;
    }
    {
        Velocity *p = ecs_ensure_modified_id(world, e, ecs_id(Velocity));
        test_assert(p != NULL);
        p->x = 1;
        p->y = 2;
    }

    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    ecs_defer_end(world);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Velocity *p = ecs_get(world, e, Velocity);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
    }

    ecs_fini(world);
}

void Sparse_defer_batched_emplace(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);

    {
        bool is_new = false;
        Position *p = ecs_emplace(world, e, Position, &is_new);
        test_assert(p != NULL);
        test_bool(is_new, true);
        p->x = 10;
        p->y = 20;
    }

    {
        bool is_new = false;
        Velocity *p = ecs_emplace(world, e, Velocity, &is_new);
        test_assert(p != NULL);
        test_bool(is_new, true);
        p->x = 1;
        p->y = 2;
    }

    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    ecs_defer_end(world);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Velocity *p = ecs_get(world, e, Velocity);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
    }

    ecs_fini(world);
}

void Sparse_defer_batched_emplace_w_modified(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);

    {
        bool is_new = false;
        Position *p = ecs_emplace(world, e, Position, &is_new);
        test_assert(p != NULL);
        test_bool(is_new, true);
        p->x = 10;
        p->y = 20;
    }
    ecs_modified(world, e, Position);

    {
        bool is_new = false;
        Velocity *p = ecs_emplace(world, e, Velocity, &is_new);
        test_assert(p != NULL);
        test_bool(is_new, true);
        p->x = 1;
        p->y = 2;
    }
    ecs_modified(world, e, Velocity);

    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    ecs_defer_end(world);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Velocity *p = ecs_get(world, e, Velocity);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
    }

    ecs_fini(world);
}

void Sparse_defer_batched_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);

    ecs_set(world, e, Position, {10, 20});
    ecs_set(world, e, Velocity, {1, 2});

    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    ecs_defer_end(world);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Velocity *p = ecs_get(world, e, Velocity);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
    }

    ecs_fini(world);
}

void Sparse_defer_batched_ensure_existing(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);

    ecs_defer_begin(world);

    {
        Position *p = ecs_ensure(world, e, Position);
        test_assert(p != NULL);
        p->x = 10;
        p->y = 20;
    }
    {
        Velocity *p = ecs_ensure(world, e, Velocity);
        test_assert(p != NULL);
        p->x = 1;
        p->y = 2;
    }

    ecs_defer_end(world);

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Velocity *p = ecs_get(world, e, Velocity);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
    }

    ecs_fini(world);
}

void Sparse_defer_batched_ensure_existing_twice(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);

    ecs_defer_begin(world);

    {
        Position *p = ecs_ensure(world, e, Position);
        test_assert(p != NULL);
        p->x = 10;
        p->y = 20;
    }
    {
        Velocity *p = ecs_ensure(world, e, Velocity);
        test_assert(p != NULL);
        p->x = 1;
        p->y = 2;
    }
    {
        Position *p = ecs_ensure(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
        p->x = 30;
        p->y = 40;
    }
    {
        Velocity *p = ecs_ensure(world, e, Velocity);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
        p->x = 3;
        p->y = 4;
    }

    ecs_defer_end(world);

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }
    {
        const Velocity *p = ecs_get(world, e, Velocity);
        test_assert(p != NULL);
        test_int(p->x, 3);
        test_int(p->y, 4);
    }

    ecs_fini(world);
}

void Sparse_defer_batched_ensure_w_modified_existing(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);

    ecs_defer_begin(world);

    {
        Position *p = ecs_ensure(world, e, Position);
        test_assert(p != NULL);
        p->x = 10;
        p->y = 20;
    }
    ecs_modified(world, e, Position);

    {
        Velocity *p = ecs_ensure(world, e, Velocity);
        test_assert(p != NULL);
        p->x = 1;
        p->y = 2;
    }
    ecs_modified(world, e, Velocity);

    ecs_defer_end(world);

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Velocity *p = ecs_get(world, e, Velocity);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
    }

    ecs_fini(world);
}

void Sparse_defer_batched_ensure_modified_existing(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);

    ecs_defer_begin(world);

    {
        Position *p = ecs_ensure_modified_id(world, e, ecs_id(Position));
        test_assert(p != NULL);
        p->x = 10;
        p->y = 20;
    }
    {
        Velocity *p = ecs_ensure_modified_id(world, e, ecs_id(Velocity));
        test_assert(p != NULL);
        p->x = 1;
        p->y = 2;
    }

    ecs_defer_end(world);

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Velocity *p = ecs_get(world, e, Velocity);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
    }

    ecs_fini(world);
}

void Sparse_defer_batched_emplace_existing(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);

    ecs_defer_begin(world);

    {
        bool is_new = false;
        Position *p = ecs_emplace(world, e, Position, &is_new);
        test_assert(p != NULL);
        test_bool(is_new, false);
        p->x = 10;
        p->y = 20;
    }

    {
        bool is_new = false;
        Velocity *p = ecs_emplace(world, e, Velocity, &is_new);
        test_assert(p != NULL);
        test_bool(is_new, false);
        p->x = 1;
        p->y = 2;
    }

    ecs_defer_end(world);

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Velocity *p = ecs_get(world, e, Velocity);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
    }

    ecs_fini(world);
}

void Sparse_defer_batched_emplace_w_modified_existing(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);

    ecs_defer_begin(world);

    {
        bool is_new = false;
        Position *p = ecs_emplace(world, e, Position, &is_new);
        test_assert(p != NULL);
        test_bool(is_new, false);
        p->x = 10;
        p->y = 20;
    }
    ecs_modified(world, e, Position);

    {
        bool is_new = false;
        Velocity *p = ecs_emplace(world, e, Velocity, &is_new);
        test_assert(p != NULL);
        test_bool(is_new, false);
        p->x = 1;
        p->y = 2;
    }
    ecs_modified(world, e, Velocity);

    ecs_defer_end(world);

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Velocity *p = ecs_get(world, e, Velocity);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
    }

    ecs_fini(world);
}

void Sparse_defer_batched_set_existing(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);

    ecs_defer_begin(world);

    ecs_set(world, e, Position, {10, 20});
    ecs_set(world, e, Velocity, {1, 2});

    ecs_defer_end(world);

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Velocity *p = ecs_get(world, e, Velocity);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
    }

    ecs_fini(world);
}

void Sparse_defer_batched_set_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);

    ecs_set(world, e, Position, {10, 20});
    ecs_remove(world, e, Position);

    ecs_defer_end(world);

    test_assert(!ecs_has(world, e, Position));

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p == NULL);
    }

    ecs_fini(world);
}

void Sparse_defer_batched_set_remove_existing(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);

    ecs_defer_begin(world);

    ecs_set(world, e, Position, {10, 20});
    ecs_remove(world, e, Position);

    ecs_defer_end(world);

    test_assert(!ecs_has(world, e, Position));

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p == NULL);
    }

    ecs_fini(world);
}
