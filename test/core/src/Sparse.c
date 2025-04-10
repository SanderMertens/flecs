#include <core.h>

static bool fragment = true;

void Sparse_setup(void) {
    const char *fragment_param = test_param("fragment");
    if (fragment_param) {
        if (!strcmp(fragment_param, "yes")) {
            // already set to default
        } else if (!strcmp(fragment_param, "no")) {
            fragment = false;
        } else {
            printf("unexpected value for fragment '%s'\n", fragment_param);
        }
    }
}

static int position_ctor_invoked = 0;
static int position_dtor_invoked = 0;
static int position_on_add_invoked = 0;
static int position_on_remove_invoked = 0;
static int position_on_set_invoked = 0;
static Position position_on_set_value = {0, 0};
static int Position_x = 10;
static int Position_y = 20;

static ECS_CTOR(Position, ptr, {
    position_ctor_invoked ++;
    ptr->x = 10;
    ptr->y = 20;
})

static ECS_DTOR(Position, ptr, {
    test_int(ptr->x, Position_x);
    test_int(ptr->y, Position_y);
    position_dtor_invoked ++;
})

static void Position_on_add(ecs_iter_t *it) {
    test_int(1, position_ctor_invoked - position_on_add_invoked);
    Position *p = ecs_field_at(it, Position, 0, 0);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    test_uint(it->event, EcsOnAdd);
    test_assert(it->entities != NULL);
    test_assert(ecs_is_alive(it->world, it->entities[0]));

    position_on_add_invoked ++;
}

static void Position_on_add_no_ctor(ecs_iter_t *it) {
    Position *p = ecs_field_at(it, Position, 0, 0);
    test_assert(p != NULL);
    test_uint(it->event, EcsOnAdd);

    position_on_add_invoked ++;
}

static void Position_on_remove(ecs_iter_t *it) {
    test_int(0, position_dtor_invoked);
    Position *p = ecs_field_at(it, Position, 0, 0);
    test_assert(p != NULL);
    test_int(p->x, Position_x);
    test_int(p->y, Position_y);
    test_uint(it->event, EcsOnRemove);
    test_assert(it->entities != NULL);
    test_assert(ecs_is_alive(it->world, it->entities[0]));

    position_on_remove_invoked ++;
}

static void Position_on_set(ecs_iter_t *it) {
    test_int(1, position_ctor_invoked);
    test_int(0, position_dtor_invoked);
    Position *p = ecs_field_at(it, Position, 0, 0);
    test_assert(p != NULL);
    position_on_set_value = *p;
    test_uint(it->event, EcsOnSet);
    test_assert(it->entities != NULL);
    test_assert(ecs_is_alive(it->world, it->entities[0]));

    position_on_set_invoked ++;
}

static void Position_on_set_bulk(ecs_iter_t *it) {
    test_assert(position_ctor_invoked != 0);
    test_int(0, position_dtor_invoked);
    Position *p = ecs_field_at(it, Position, 0, 0);
    test_assert(p != NULL);
    position_on_set_value = *p;
    test_uint(it->event, EcsOnSet);
    test_assert(it->entities != NULL);
    test_assert(ecs_is_alive(it->world, it->entities[0]));

    position_on_set_invoked ++;
}

void Sparse_has(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t e = ecs_new(world);
    test_bool(false, ecs_has(world, e, Position));

    ecs_add(world, e, Position);
    test_bool(true, ecs_has(world, e, Position));

    ecs_fini(world);
}

void Sparse_has_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t e = ecs_new(world);
    test_bool(false, ecs_has_pair(world, e, ecs_id(Position), TgtA));
    test_bool(false, ecs_has_pair(world, e, ecs_id(Position), TgtB));

    ecs_add_pair(world, e, ecs_id(Position), TgtA);
    test_bool(true, ecs_has_pair(world, e, ecs_id(Position), TgtA));
    test_bool(false, ecs_has_pair(world, e, ecs_id(Position), TgtB));

    ecs_fini(world);
}

void Sparse_has_pair_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t e = ecs_new(world);
    test_bool(false, ecs_has_pair(world, e, ecs_id(Position), EcsWildcard));
    test_bool(false, ecs_has_pair(world, e, EcsWildcard, TgtA));
    test_bool(false, ecs_has_pair(world, e, EcsWildcard, TgtB));

    ecs_add_pair(world, e, ecs_id(Position), TgtA);
    test_bool(true, ecs_has_pair(world, e, ecs_id(Position), EcsWildcard));
    test_bool(true, ecs_has_pair(world, e, EcsWildcard, TgtA));
    test_bool(false, ecs_has_pair(world, e, EcsWildcard, TgtB));

    ecs_fini(world);
}

void Sparse_owns(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t e = ecs_new(world);
    test_bool(false, ecs_owns(world, e, Position));

    ecs_add(world, e, Position);
    test_bool(true, ecs_owns(world, e, Position));

    ecs_fini(world);
}

void Sparse_owns_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t e = ecs_new(world);
    test_bool(false, ecs_owns_pair(world, e, ecs_id(Position), TgtA));
    test_bool(false, ecs_owns_pair(world, e, ecs_id(Position), TgtB));

    ecs_add_pair(world, e, ecs_id(Position), TgtA);
    test_bool(true, ecs_owns_pair(world, e, ecs_id(Position), TgtA));
    test_bool(false, ecs_owns_pair(world, e, ecs_id(Position), TgtB));

    ecs_fini(world);
}

void Sparse_owns_pair_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t e = ecs_new(world);
    test_bool(false, ecs_owns_pair(world, e, ecs_id(Position), EcsWildcard));
    test_bool(false, ecs_owns_pair(world, e, EcsWildcard, TgtA));
    test_bool(false, ecs_owns_pair(world, e, EcsWildcard, TgtB));

    ecs_add_pair(world, e, ecs_id(Position), TgtA);
    test_bool(true, ecs_owns_pair(world, e, ecs_id(Position), EcsWildcard));
    test_bool(true, ecs_owns_pair(world, e, EcsWildcard, TgtA));
    test_bool(false, ecs_owns_pair(world, e, EcsWildcard, TgtB));

    ecs_fini(world);
}

void Sparse_add_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));
    test_assert(NULL != ecs_get(world, e, Position));

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));
    test_assert(NULL != ecs_get(world, e, Position));

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));
    test_assert(NULL == ecs_get(world, e, Position));

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));
    test_assert(NULL == ecs_get(world, e, Position));

    ecs_fini(world);
}

void Sparse_add_remove_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, Sparse);
    if (!fragment) ecs_add_id(world, Foo, EcsDontFragment);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Foo);
    test_assert(ecs_has(world, e, Foo));

    ecs_add(world, e, Foo);
    test_assert(ecs_has(world, e, Foo));

    ecs_remove(world, e, Foo);
    test_assert(!ecs_has(world, e, Foo));

    ecs_remove(world, e, Foo);
    test_assert(!ecs_has(world, e, Foo));

    ecs_fini(world);
}

void Sparse_add_remove_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, ecs_id(Position), TgtA);
    test_assert(ecs_has_pair(world, e, ecs_id(Position), TgtA));

    ecs_add_pair(world, e, ecs_id(Position), TgtA);
    test_assert(ecs_has_pair(world, e, ecs_id(Position), TgtA));

    ecs_remove_pair(world, e, ecs_id(Position), TgtA);
    test_assert(!ecs_has_pair(world, e, ecs_id(Position), TgtA));

    ecs_remove_pair(world, e, ecs_id(Position), TgtA);
    test_assert(!ecs_has_pair(world, e, ecs_id(Position), TgtA));

    ecs_fini(world);
}

void Sparse_add_remove_pair_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, Sparse);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    if (!fragment) ecs_add_id(world, Foo, EcsDontFragment);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Foo, TgtA);
    test_assert(ecs_has_pair(world, e, Foo, TgtA));

    ecs_add_pair(world, e, Foo, TgtA);
    test_assert(ecs_has_pair(world, e, Foo, TgtA));

    ecs_remove_pair(world, e, Foo, TgtA);
    test_assert(!ecs_has_pair(world, e, Foo, TgtA));

    ecs_remove_pair(world, e, Foo, TgtA);
    test_assert(!ecs_has_pair(world, e, Foo, TgtA));

    ecs_fini(world);
}

void Sparse_add_remove_twice_w_hooks(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .on_add = ecs_on_add(Position),
        .dtor = ecs_dtor(Position),
        .on_remove = ecs_on_remove(Position)
    });

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    test_int(position_ctor_invoked, 0);
    test_int(position_dtor_invoked, 0);
    test_int(position_on_add_invoked, 0);
    test_int(position_on_remove_invoked, 0);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));
    test_int(position_ctor_invoked, 1);
    test_int(position_dtor_invoked, 0);
    test_int(position_on_add_invoked, 1);
    test_int(position_on_remove_invoked, 0);

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));
    test_int(position_ctor_invoked, 1);
    test_int(position_dtor_invoked, 0);
    test_int(position_on_add_invoked, 1);
    test_int(position_on_remove_invoked, 0);

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));
    test_int(position_ctor_invoked, 1);
    test_int(position_dtor_invoked, 1);
    test_int(position_on_add_invoked, 1);
    test_int(position_on_remove_invoked, 1);

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));
    test_int(position_ctor_invoked, 1);
    test_int(position_dtor_invoked, 1);
    test_int(position_on_add_invoked, 1);
    test_int(position_on_remove_invoked, 1);

    ecs_fini(world);
}

void Sparse_get(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t e = ecs_new(world);
    Position *p = ecs_ensure(world, e, Position);
    test_assert(NULL != p);

    ecs_add(world, e, Position);
    test_assert(p == ecs_get_mut(world, e, Position));

    ecs_fini(world);
}

void Sparse_ensure_twice_w_hooks(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .on_add = ecs_on_add(Position),
        .dtor = ecs_dtor(Position),
        .on_remove = ecs_on_remove(Position)
    });

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    test_int(position_ctor_invoked, 0);
    test_int(position_dtor_invoked, 0);
    test_int(position_on_add_invoked, 0);
    test_int(position_on_remove_invoked, 0);

    ecs_entity_t e = ecs_new(world);
    void *p = ecs_ensure(world, e, Position);
    test_assert(p != NULL);
    test_assert(ecs_has(world, e, Position));
    test_assert(p == ecs_get(world, e, Position));
    test_int(position_ctor_invoked, 1);
    test_int(position_dtor_invoked, 0);
    test_int(position_on_add_invoked, 1);
    test_int(position_on_remove_invoked, 0);

    test_assert(p == ecs_ensure(world, e, Position));
    test_assert(ecs_has(world, e, Position));
    test_assert(p == ecs_get(world, e, Position));
    test_int(position_ctor_invoked, 1);
    test_int(position_dtor_invoked, 0);
    test_int(position_on_add_invoked, 1);
    test_int(position_on_remove_invoked, 0);

    ecs_fini(world);
}

void Sparse_emplace(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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

void Sparse_emplace_twice_w_hooks(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .on_add = Position_on_add_no_ctor,
        .dtor = ecs_dtor(Position),
        .on_remove = ecs_on_remove(Position)
    });

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    test_int(position_ctor_invoked, 0);
    test_int(position_dtor_invoked, 0);
    test_int(position_on_add_invoked, 0);
    test_int(position_on_remove_invoked, 0);

    bool is_new = false;

    ecs_entity_t e = ecs_new(world);
    Position *p = ecs_emplace(world, e, Position, &is_new);
    test_assert(p != NULL);
    test_assert(ecs_has(world, e, Position));
    test_assert(p == ecs_get(world, e, Position));
    test_bool(true, is_new);
    test_int(position_ctor_invoked, 0);
    test_int(position_dtor_invoked, 0);
    test_int(position_on_add_invoked, 1);
    test_int(position_on_remove_invoked, 0);

    test_assert(p == ecs_emplace(world, e, Position, &is_new));
    test_assert(ecs_has(world, e, Position));
    test_assert(p == ecs_get(world, e, Position));
    test_bool(false, is_new);
    test_int(position_ctor_invoked, 0);
    test_int(position_dtor_invoked, 0);
    test_int(position_on_add_invoked, 1);
    test_int(position_on_remove_invoked, 0);

    p->x = 10;
    p->y = 20;
    position_ctor_invoked = 1;

    ecs_fini(world);

    test_int(position_ctor_invoked, 1);
    test_int(position_dtor_invoked, 1);
    test_int(position_on_add_invoked, 1);
    test_int(position_on_remove_invoked, 1);
}

void Sparse_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));
    
    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Sparse_clone(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));

    ecs_entity_t c = ecs_clone(world, 0, e, false);
    test_assert(c != 0);
    test_assert(ecs_has(world, c, Position));

    ecs_fini(world);
}

void Sparse_clone_w_value(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));

    ecs_entity_t c = ecs_clone(world, 0, e, true);
    test_assert(c != 0);
    test_assert(ecs_has(world, c, Position));

    const Position *p = ecs_get(world, c, Position);
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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Velocity), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t e = ecs_new(world);
    test_bool(false, ecs_has(world, e, Position));

    ecs_add(world, e, Position);
    test_bool(true, ecs_has(world, e, Position));

    ecs_remove(world, e, Position);
    test_bool(false, ecs_has(world, e, Position));

    ecs_fini(world);
}

void Sparse_has_after_clear(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t e = ecs_new(world);
    test_bool(false, ecs_has(world, e, Position));

    ecs_add(world, e, Position);
    test_bool(true, ecs_has(world, e, Position));

    ecs_clear(world, e);
    test_bool(false, ecs_has(world, e, Position));

    ecs_fini(world);
}

void Sparse_get_after_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Velocity), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
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

void Sparse_override_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t base = ecs_new(world);
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsIsA, base);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_owns(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_assert(p != ecs_get(world, base, Position));
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_remove(world, e, Position);
    test_assert(!ecs_owns(world, e, Position));
    test_assert(!ecs_has(world, e, Position));

    ecs_fini(world);
}

void Sparse_override_component_2_lvls(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t base = ecs_new(world);
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t base_base = ecs_new_w_pair(world, EcsIsA, base);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsIsA, base_base);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_owns(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_assert(p != ecs_get(world, base, Position));
    test_assert(p != ecs_get(world, base_base, Position));
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Sparse_override_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_add_id(world, Foo, EcsSparse);
    if (!fragment) ecs_add_id(world, Foo, EcsDontFragment);

    ecs_entity_t base = ecs_new(world);
    ecs_add(world, base, Foo);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsIsA, base);
    test_assert(ecs_has(world, e, Foo));
    test_assert(ecs_owns(world, e, Foo));

    ecs_remove(world, e, Foo);
    test_assert(!ecs_owns(world, e, Foo));
    test_assert(!ecs_has(world, e, Foo));

    ecs_fini(world);
}

void Sparse_override_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);

    ecs_add_id(world, Rel, EcsSparse);
    if (!fragment) ecs_add_id(world, Rel, EcsDontFragment);

    ecs_entity_t base = ecs_new(world);
    ecs_add_pair(world, base, Rel, TgtA);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsIsA, base);
    test_assert(ecs_has_pair(world, e, Rel, TgtA));
    test_assert(ecs_owns_pair(world, e, Rel, TgtA));

    ecs_remove_pair(world, e, Rel, TgtA);
    test_assert(!ecs_owns_pair(world, e, Rel, TgtA));
    test_assert(!ecs_has_pair(world, e, Rel, TgtA));

    ecs_fini(world);
}

void Sparse_dont_override_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new(world);
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsIsA, base);
    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_owns(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_assert(p == ecs_get(world, base, Position));
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Sparse_delete_w_override_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t base = ecs_new(world);
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsIsA, base);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_owns(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_assert(p != ecs_get(world, base, Position));
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_delete(world, e);

    ecs_fini(world);
}

static int on_remove_isa_invoked = 0;

static
void on_remove_isa(ecs_iter_t *it) {
    on_remove_isa_invoked ++;
}

void Sparse_delete_w_override_on_remove_isa(void) {
    ecs_world_t *world = ecs_mini();

    ecs_observer(world, {
        .query.terms = {{ ecs_pair(EcsIsA, EcsWildcard) }},
        .events = { EcsOnRemove },
        .callback = on_remove_isa
    });

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t base = ecs_new(world);
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsIsA, base);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_owns(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_assert(p != ecs_get(world, base, Position));
    test_int(p->x, 10);
    test_int(p->y, 20);

    test_int(0, on_remove_isa_invoked);

    ecs_delete(world, e);

    test_int(1, on_remove_isa_invoked);

    ecs_fini(world);
}

void Sparse_manual_override_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t base = ecs_new(world);
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsIsA, base);
    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_owns(world, e, Position));

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_assert(p == ecs_get(world, base, Position));
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_owns(world, e, Position));

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_assert(p != ecs_get(world, base, Position));
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_remove(world, e, Position);
    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_owns(world, e, Position));

    ecs_fini(world);
}

void Sparse_auto_override_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t base = ecs_new(world);
    ecs_set(world, base, Position, {10, 20});
    ecs_add_id(world, base, ECS_AUTO_OVERRIDE | ecs_id(Position));

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsIsA, base);

    if (!fragment) {
        test_int(ecs_get_type(world, e)->count, 1);
    } else {
        test_int(ecs_get_type(world, e)->count, 2);
    }
    
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_owns(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_assert(p != ecs_get(world, base, Position));
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_remove(world, e, Position);
    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_owns(world, e, Position));

    ecs_fini(world);
}

void Sparse_auto_override_component_no_value(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t base = ecs_new(world);
    ecs_add_id(world, base, ECS_AUTO_OVERRIDE | ecs_id(Position));

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsIsA, base);

    if (!fragment) {
        test_int(ecs_get_type(world, e)->count, 1);
    } else {
        test_int(ecs_get_type(world, e)->count, 2);
    }
    
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_owns(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_assert(p != ecs_get(world, base, Position));

    ecs_remove(world, e, Position);
    test_assert(!ecs_owns(world, e, Position));
    test_assert(!ecs_has(world, e, Position));

    ecs_fini(world);
}

void Sparse_bulk_init(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    Position p[] = {
        {10, 20},
        {30, 40},
        {50, 60}
    };

    void *data[] = {p};

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 3,
        .ids = {ecs_id(Position)},
        .data = data
    });

    test_assert(ecs_has(world, entities[0], Position));
    test_assert(ecs_has(world, entities[1], Position));
    test_assert(ecs_has(world, entities[2], Position));

    {
        const Position *p = ecs_get(world, entities[0], Position);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    } {
        const Position *p = ecs_get(world, entities[1], Position);
        test_assert(p != NULL);
        test_int(p->x, 30); test_int(p->y, 40);
    } {
        const Position *p = ecs_get(world, entities[2], Position);
        test_assert(p != NULL);
        test_int(p->x, 50); test_int(p->y, 60);
    }

    ecs_fini(world);
}

void Sparse_bulk_init_w_non_sparse(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    Position p[] = {
        {10, 20},
        {30, 40},
        {50, 60}
    };

    Velocity v[] = {
        {1, 2},
        {3, 4},
        {5, 6}
    };

    void *data[] = {p, v};

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 3,
        .ids = {ecs_id(Position), ecs_id(Velocity)},
        .data = data
    });

    test_assert(ecs_has(world, entities[0], Position));
    test_assert(ecs_has(world, entities[1], Position));
    test_assert(ecs_has(world, entities[2], Position));

    test_assert(ecs_has(world, entities[0], Velocity));
    test_assert(ecs_has(world, entities[1], Velocity));
    test_assert(ecs_has(world, entities[2], Velocity));

    {
        const Position *p = ecs_get(world, entities[0], Position);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    } {
        const Position *p = ecs_get(world, entities[1], Position);
        test_assert(p != NULL);
        test_int(p->x, 30); test_int(p->y, 40);
    } {
        const Position *p = ecs_get(world, entities[2], Position);
        test_assert(p != NULL);
        test_int(p->x, 50); test_int(p->y, 60);
    }

    {
        const Velocity *p = ecs_get(world, entities[0], Velocity);
        test_assert(p != NULL);
        test_int(p->x, 1); test_int(p->y, 2);
    } {
        const Velocity *p = ecs_get(world, entities[1], Velocity);
        test_assert(p != NULL);
        test_int(p->x, 3); test_int(p->y, 4);
    } {
        const Velocity *p = ecs_get(world, entities[2], Velocity);
        test_assert(p != NULL);
        test_int(p->x, 5); test_int(p->y, 6);
    }

    ecs_fini(world);
}

void Sparse_ctor_after_emplace(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    test_int(30, position_on_set_value.x);
    test_int(40, position_on_set_value.y);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    test_int(30, position_on_set_value.x);
    test_int(40, position_on_set_value.y);

    {
        const Position *ptr = ecs_get(world, e, Position);
        test_int(ptr->x, 30);
        test_int(ptr->y, 40);
    }

    ecs_fini(world);
}

void Sparse_on_set_at_offset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .on_add = Position_on_add,
        .on_set = Position_on_set
    });

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});

    test_int(1, position_ctor_invoked);
    test_int(1, position_on_add_invoked);
    test_int(1, position_on_set_invoked);
    test_int(10, position_on_set_value.x);
    test_int(20, position_on_set_value.y);
    
    position_ctor_invoked  = 0;
    position_on_add_invoked = 0;
    position_on_set_invoked = 0;

    ecs_entity_t e2 = ecs_new(world);
    ecs_set(world, e2, Position, {30, 40});

    test_int(1, position_ctor_invoked);
    test_int(1, position_on_add_invoked);
    test_int(1, position_on_set_invoked);
    test_int(30, position_on_set_value.x);
    test_int(40, position_on_set_value.y);

    ecs_fini(world);
}

void Sparse_on_set_after_clone(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .on_add = Position_on_add,
        .on_set = Position_on_set
    });

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));

    position_ctor_invoked = 0;
    position_on_add_invoked = 0;
    position_on_set_invoked = 0;

    ecs_entity_t c = ecs_clone(world, 0, e, true);
    test_assert(c != 0);
    test_assert(ecs_has(world, c, Position));

    test_int(1, position_ctor_invoked);
    test_int(1, position_on_add_invoked);
    test_int(1, position_on_set_invoked);

    const Position *p = ecs_get(world, c, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Sparse_on_set_after_bulk_init(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .on_add = Position_on_add,
        .on_set = Position_on_set_bulk
    });

    Position p[] = {
        {10, 20},
        {30, 40},
        {50, 60}
    };

    void *data[] = {p};

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 3,
        .ids = {ecs_id(Position)},
        .data = data
    });

    test_assert(ecs_has(world, entities[0], Position));
    test_assert(ecs_has(world, entities[1], Position));
    test_assert(ecs_has(world, entities[2], Position));

    {
        const Position *p = ecs_get(world, entities[0], Position);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    } {
        const Position *p = ecs_get(world, entities[1], Position);
        test_assert(p != NULL);
        test_int(p->x, 30); test_int(p->y, 40);
    } {
        const Position *p = ecs_get(world, entities[2], Position);
        test_assert(p != NULL);
        test_int(p->x, 50); test_int(p->y, 60);
    }

    test_int(3, position_ctor_invoked);
    test_int(3, position_on_add_invoked);
    test_int(3, position_on_set_invoked);

    ecs_fini(world);
}

static
void Position_add_observer(ecs_iter_t *it) {
    probe_iter(it);
    test_int(it->count, 1);
    Position *p = ecs_field_at(it, Position, 0, 0);
    test_assert(p != NULL);
}

static
void Position_set_observer(ecs_iter_t *it) {
    probe_iter(it);
    test_int(it->count, 1);
    Position *p = ecs_field_at(it, Position, 0, 0);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Sparse_on_add_observer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    Probe ctx = {0};

    ecs_observer(world, {
        .query.terms = {{ .id = ecs_id(Position) }},
        .events = {EcsOnAdd},
        .callback = Position_add_observer,
        .ctx = &ctx
    });

    test_int(ctx.invoked, 0);
    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    test_assert(e != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_uint(ctx.event, EcsOnAdd);
    test_uint(ctx.e[0], e);
    test_uint(ctx.c[0][0], ecs_id(Position));

    ecs_fini(world);
}

void Sparse_on_set_observer_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    Probe ctx = {0};
    
    ecs_observer(world, {
        .query.terms = {{ .id = ecs_id(Position) }},
        .events = {EcsOnSet},
        .callback = Position_set_observer,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world);

    test_int(ctx.invoked, 0);

    ecs_set(world, e, Position, {10, 20});

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_uint(ctx.event, EcsOnSet);
    test_uint(ctx.e[0], e);
    test_uint(ctx.c[0][0], ecs_id(Position));

    ecs_fini(world);
}

void Sparse_on_set_observer_modified(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    Probe ctx = {0};
    
    ecs_observer(world, {
        .query.terms = {{ .id = ecs_id(Position) }},
        .events = {EcsOnSet},
        .callback = Position_set_observer,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world);

    Position *p = ecs_ensure(world, e, Position);
    test_assert(p != NULL);
    p->x = 10;
    p->y = 20;

    test_int(ctx.invoked, 0);

    ecs_modified(world, e, Position);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_uint(ctx.event, EcsOnSet);
    test_uint(ctx.e[0], e);
    test_uint(ctx.c[0][0], ecs_id(Position));

    ecs_fini(world);
}

void Sparse_on_set_observer_insert(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    Probe ctx = {0};
    
    ecs_observer(world, {
        .query.terms = {{ .id = ecs_id(Position) }},
        .events = {EcsOnSet},
        .callback = Position_set_observer,
        .ctx = &ctx
    });
    
    test_int(ctx.invoked, 0);

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    test_assert(e != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_uint(ctx.event, EcsOnSet);
    test_uint(ctx.e[0], e);
    test_uint(ctx.c[0][0], ecs_id(Position));

    ecs_fini(world);
}

void Sparse_on_remove_observer_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    Probe ctx = {0};
    
    ecs_observer(world, {
        .query.terms = {{ .id = ecs_id(Position) }},
        .events = {EcsOnRemove},
        .callback = Position_set_observer,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    test_assert(e != 0);

    test_int(ctx.invoked, 0);

    ecs_remove(world, e, Position);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_uint(ctx.event, EcsOnRemove);
    test_uint(ctx.e[0], e);
    test_uint(ctx.c[0][0], ecs_id(Position));

    ecs_fini(world);
}

void Sparse_on_remove_observer_clear(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    Probe ctx = {0};
    
    ecs_observer(world, {
        .query.terms = {{ .id = ecs_id(Position) }},
        .events = {EcsOnRemove},
        .callback = Position_set_observer,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    test_assert(e != 0);

    test_int(ctx.invoked, 0);

    ecs_clear(world, e);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_uint(ctx.event, EcsOnRemove);
    test_uint(ctx.e[0], e);
    test_uint(ctx.c[0][0], ecs_id(Position));

    ecs_fini(world);
}

void Sparse_on_remove_observer_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    Probe ctx = {0};
    
    ecs_observer(world, {
        .query.terms = {{ .id = ecs_id(Position) }},
        .events = {EcsOnRemove},
        .callback = Position_set_observer,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    test_assert(e != 0);

    test_int(ctx.invoked, 0);

    ecs_delete(world, e);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_uint(ctx.event, EcsOnRemove);
    test_uint(ctx.e[0], e);
    test_uint(ctx.c[0][0], ecs_id(Position));

    ecs_fini(world);
}

void Sparse_on_remove_observer_fini(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    Probe ctx = {0};
    
    ecs_observer(world, {
        .query.terms = {{ .id = ecs_id(Position) }},
        .events = {EcsOnRemove},
        .callback = Position_set_observer,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    test_assert(e != 0);

    test_int(ctx.invoked, 0);

    ecs_fini(world);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_uint(ctx.event, EcsOnRemove);
    test_uint(ctx.e[0], e);
    test_uint(ctx.c[0][0], ecs_id(Position));
}

void Sparse_on_set_after_remove_override(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new(world);
    ecs_set(world, base, Position, {10, 20});
    ecs_auto_override(world, base, Position);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsIsA, base);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_owns(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_assert(p != ecs_get(world, base, Position));
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_set(world, e, Position, {30, 40});

    Probe ctx = {0};

    ecs_observer(world, {
        .query.terms = {{ ecs_id(Position) }},
        .events = { EcsOnSet },
        .callback = Position_set_observer,
        .ctx = &ctx
    });

    ecs_remove(world, e, Position);

    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_owns(world, e, Position));

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_uint(ctx.event, EcsOnRemove);
    test_uint(ctx.e[0], e);
    test_uint(ctx.c[0][0], ecs_id(Position));

    ecs_fini(world);
}

static
void Position_Velocity_add_observer(ecs_iter_t *it) {
    probe_iter(it);
    test_int(it->count, 1);
    Position *p = ecs_field_at(it, Position, 0, 0);
    test_assert(p != NULL);
    Velocity *v = ecs_field_at(it, Velocity, 1, 0);
    test_assert(v != NULL);
}

static
void Position_Velocity_set_observer(ecs_iter_t *it) {
    probe_iter(it);
    test_int(it->count, 1);
    Position *p = ecs_field_at(it, Position, 0, 0);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    Velocity *v = ecs_field_at(it, Velocity, 1, 0);
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 2);
}

void Sparse_on_add_observer_2_terms(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Velocity), EcsDontFragment);

    Probe ctx = {0};

    ecs_observer(world, {
        .query.terms = {{ .id = ecs_id(Position) }, { .id = ecs_id(Velocity) }},
        .events = {EcsOnAdd},
        .callback = Position_Velocity_add_observer,
        .ctx = &ctx
    });

    test_int(ctx.invoked, 0);
    ecs_entity_t e = ecs_insert(world, 
        ecs_value(Position, {10, 20}),
        ecs_value(Velocity, {1, 2}));
    test_assert(e != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_uint(ctx.event, EcsOnAdd);
    test_uint(ctx.e[0], e);
    test_uint(ctx.c[0][0], ecs_id(Position));
    test_uint(ctx.c[0][1], ecs_id(Velocity));

    ecs_fini(world);
}

void Sparse_on_set_observer_2_terms(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Velocity), EcsDontFragment);

    Probe ctx = {0};

    ecs_observer(world, {
        .query.terms = {{ .id = ecs_id(Position) }, { .id = ecs_id(Velocity) }},
        .events = {EcsOnSet},
        .callback = Position_Velocity_set_observer,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});

    test_int(ctx.invoked, 0);

    ecs_set(world, e, Velocity, {1, 2});

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_uint(ctx.event, EcsOnSet);
    test_uint(ctx.e[0], e);
    test_uint(ctx.c[0][0], ecs_id(Position));
    test_uint(ctx.c[0][1], ecs_id(Velocity));

    ecs_fini(world);
}

void Sparse_on_remove_observer_2_terms(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Velocity), EcsDontFragment);

    Probe ctx = {0};

    ecs_observer(world, {
        .query.terms = {{ .id = ecs_id(Position) }, { .id = ecs_id(Velocity) }},
        .events = {EcsOnRemove},
        .callback = Position_Velocity_set_observer,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});

    test_int(ctx.invoked, 0);

    ecs_set(world, e, Velocity, {1, 2});

    test_int(ctx.invoked, 0);

    ecs_remove(world, e, Position);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_uint(ctx.event, EcsOnRemove);
    test_uint(ctx.e[0], e);
    test_uint(ctx.c[0][0], ecs_id(Position));
    test_uint(ctx.c[0][1], ecs_id(Velocity));

    ecs_fini(world);

    test_int(ctx.invoked, 1);
}

void Sparse_sparse_relationship(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Tgt);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t e = ecs_new(world);
    test_assert(NULL == ecs_get_pair(world, e, Position, Tgt));

    ecs_set_pair(world, e, Position, Tgt, {10, 20});
    const Position *ptr = ecs_get_pair(world, e, Position, Tgt);
    test_assert(ptr != NULL);

    ecs_add(world, e, Foo);
    test_assert(ptr == ecs_get_pair(world, e, Position, Tgt));

    ecs_fini(world);
}

void Sparse_sparse_relationship_second(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Rel);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t e = ecs_new(world);
    test_assert(NULL == ecs_get_pair_second(world, e, Rel, Position));

    ecs_set_pair_second(world, e, Rel, Position, {10, 20});
    const Position *ptr = ecs_get_pair_second(world, e, Rel, Position);
    test_assert(ptr != NULL);

    ecs_add(world, e, Foo);
    test_assert(ptr == ecs_get_pair_second(world, e, Rel, Position));

    ecs_fini(world);
}

void Sparse_exclusive_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_add_id(world, Rel, EcsSparse);
    ecs_add_id(world, Rel, EcsExclusive);
    if (!fragment) ecs_add_id(world, Rel, EcsDontFragment);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Rel, TgtA);
    test_assert(ecs_has_pair(world, e, Rel, TgtA));
    test_assert(!ecs_has_pair(world, e, Rel, TgtB));

    ecs_add_pair(world, e, Rel, TgtB);
    test_assert(!ecs_has_pair(world, e, Rel, TgtA));
    test_assert(ecs_has_pair(world, e, Rel, TgtB));

    ecs_remove_pair(world, e, Rel, TgtB);
    test_assert(!ecs_has_pair(world, e, Rel, TgtA));
    test_assert(!ecs_has_pair(world, e, Rel, TgtB));

    ecs_fini(world);
}

void Sparse_exclusive_pair_w_data(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    ecs_add_id(world, ecs_id(Position), EcsExclusive);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t e = ecs_new(world);
    ecs_set_pair(world, e, Position, TgtA, {10, 20});
    test_assert(ecs_has_pair(world, e, ecs_id(Position), TgtA));
    test_assert(!ecs_has_pair(world, e, ecs_id(Position), TgtB));
    {
        const Position *p = ecs_get_pair(world, e, Position, TgtA);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Position *p = ecs_get_pair(world, e, Position, TgtB);
        test_assert(p == NULL);
    }

    ecs_set_pair(world, e, Position, TgtB, {30, 40});
    test_assert(!ecs_has_pair(world, e, ecs_id(Position), TgtA));
    test_assert(ecs_has_pair(world, e, ecs_id(Position), TgtB));
    {
        const Position *p = ecs_get_pair(world, e, Position, TgtA);
        test_assert(p == NULL);
    }
    {
        const Position *p = ecs_get_pair(world, e, Position, TgtB);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }

    ecs_remove_pair(world, e, ecs_id(Position), TgtB);
    test_assert(!ecs_has_pair(world, e, ecs_id(Position), TgtA));
    test_assert(!ecs_has_pair(world, e, ecs_id(Position), TgtB));
    {
        const Position *p = ecs_get_pair(world, e, Position, TgtA);
        test_assert(p == NULL);
    }
    {
        const Position *p = ecs_get_pair(world, e, Position, TgtB);
        test_assert(p == NULL);
    }

    ecs_fini(world);
}

void Sparse_exclusive_pair_w_hooks(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    ecs_add_id(world, ecs_id(Position), EcsExclusive);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .on_add = ecs_on_add(Position),
        .dtor = ecs_dtor(Position),
        .on_remove = ecs_on_remove(Position)
    });

    ecs_entity_t e = ecs_new(world);
    ecs_set_pair(world, e, Position, TgtA, {10, 20});
    test_assert(ecs_has_pair(world, e, ecs_id(Position), TgtA));
    test_assert(!ecs_has_pair(world, e, ecs_id(Position), TgtB));
    {
        const Position *p = ecs_get_pair(world, e, Position, TgtA);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Position *p = ecs_get_pair(world, e, Position, TgtB);
        test_assert(p == NULL);
    }

    test_int(position_ctor_invoked, 1);
    test_int(position_dtor_invoked, 0);
    test_int(position_on_add_invoked, 1);
    test_int(position_on_remove_invoked, 0);

    position_ctor_invoked = 0;
    position_dtor_invoked = 0;
    position_on_add_invoked = 0;
    position_on_remove_invoked = 0;

    ecs_set_pair(world, e, Position, TgtB, {30, 40});
    test_assert(!ecs_has_pair(world, e, ecs_id(Position), TgtA));
    test_assert(ecs_has_pair(world, e, ecs_id(Position), TgtB));
    {
        const Position *p = ecs_get_pair(world, e, Position, TgtA);
        test_assert(p == NULL);
    }
    {
        const Position *p = ecs_get_pair(world, e, Position, TgtB);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }

    test_int(position_ctor_invoked, 1);
    test_int(position_dtor_invoked, 1);
    test_int(position_on_add_invoked, 1);
    test_int(position_on_remove_invoked, 1);

    position_ctor_invoked = 0;
    position_dtor_invoked = 0;
    position_on_add_invoked = 0;
    position_on_remove_invoked = 0;

    Position_x = 30;
    Position_y = 40;

    ecs_remove_pair(world, e, ecs_id(Position), TgtB);
    test_assert(!ecs_has_pair(world, e, ecs_id(Position), TgtA));
    test_assert(!ecs_has_pair(world, e, ecs_id(Position), TgtB));
    {
        const Position *p = ecs_get_pair(world, e, Position, TgtA);
        test_assert(p == NULL);
    }
    {
        const Position *p = ecs_get_pair(world, e, Position, TgtB);
        test_assert(p == NULL);
    }

    test_int(position_ctor_invoked, 0);
    test_int(position_dtor_invoked, 1);
    test_int(position_on_add_invoked, 0);
    test_int(position_on_remove_invoked, 1);

    ecs_fini(world);
}

void Sparse_target_1_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_add_id(world, Rel, EcsSparse);
    if (!fragment) ecs_add_id(world, Rel, EcsDontFragment);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TgtB);

    test_uint(TgtA, ecs_get_target(world, e1, Rel, 0));
    test_uint(0, ecs_get_target(world, e1, Rel, 1));

    test_uint(TgtB, ecs_get_target(world, e2, Rel, 0));
    test_uint(0, ecs_get_target(world, e2, Rel, 1));

    ecs_remove_pair(world, e1, Rel, TgtB);
    test_uint(TgtA, ecs_get_target(world, e1, Rel, 0));
    test_uint(0, ecs_get_target(world, e1, Rel, 1));
    test_assert(ecs_has_pair(world, e1, Rel, TgtA));

    ecs_remove_pair(world, e1, Rel, TgtA);
    test_uint(0, ecs_get_target(world, e1, Rel, 0));
    test_assert(!ecs_has_pair(world, e1, Rel, TgtA));

    ecs_remove_pair(world, e2, Rel, TgtA);
    test_uint(TgtB, ecs_get_target(world, e2, Rel, 0));
    test_uint(0, ecs_get_target(world, e2, Rel, 1));
    test_assert(ecs_has_pair(world, e2, Rel, TgtB));

    ecs_remove_pair(world, e2, Rel, TgtB);
    test_uint(0, ecs_get_target(world, e2, Rel, 0));
    test_assert(!ecs_has_pair(world, e2, Rel, TgtB));

    ecs_fini(world);
}

void Sparse_target_2_pairs(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);
    ECS_TAG(world, TgtD);

    ecs_add_id(world, Rel, EcsSparse);
    if (!fragment) ecs_add_id(world, Rel, EcsDontFragment);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_add_pair(world, e1, Rel, TgtC);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_add_pair(world, e2, Rel, TgtD);

    test_uint(TgtA, ecs_get_target(world, e1, Rel, 0));
    test_uint(TgtC, ecs_get_target(world, e1, Rel, 1));
    test_uint(0, ecs_get_target(world, e1, Rel, 2));

    test_uint(TgtB, ecs_get_target(world, e2, Rel, 0));
    test_uint(TgtD, ecs_get_target(world, e2, Rel, 1));
    test_uint(0, ecs_get_target(world, e2, Rel, 2));

    ecs_fini(world);
}

void Sparse_target_exclusive_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_add_id(world, Rel, EcsSparse);
    ecs_add_id(world, Rel, EcsExclusive);
    if (!fragment) ecs_add_id(world, Rel, EcsDontFragment);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TgtB);

    test_uint(TgtA, ecs_get_target(world, e1, Rel, 0));
    test_uint(0, ecs_get_target(world, e1, Rel, 1));

    test_uint(TgtB, ecs_get_target(world, e2, Rel, 0));
    test_uint(0, ecs_get_target(world, e2, Rel, 1));

    ecs_fini(world);
}

void Sparse_target_from_base(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);
    ECS_TAG(world, TgtD);

    ecs_add_id(world, Rel, EcsSparse);
    if (!fragment) ecs_add_id(world, Rel, EcsDontFragment);
    ecs_add_pair(world, Rel, EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new_w_pair(world, Rel, TgtA);
    ecs_add_pair(world, base, Rel, TgtC);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, base);

    test_uint(TgtA, ecs_get_target(world, e1, Rel, 0));
    test_uint(TgtC, ecs_get_target(world, e1, Rel, 1));
    test_uint(0, ecs_get_target(world, e1, Rel, 2));

    ecs_fini(world);
}

void Sparse_exclusive_target_from_base(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);
    ECS_TAG(world, TgtD);

    ecs_add_id(world, Rel, EcsSparse);
    if (!fragment) ecs_add_id(world, Rel, EcsDontFragment);
    ecs_add_pair(world, Rel, EcsOnInstantiate, EcsInherit);
    ecs_add_id(world, Rel, EcsExclusive);

    ecs_entity_t base = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, base);

    test_uint(TgtA, ecs_get_target(world, e1, Rel, 0));
    test_uint(0, ecs_get_target(world, e1, Rel, 1));

    ecs_fini(world);
}

void Sparse_target_for_base(void) {
    if (!fragment) test_quarantine("April 7 2025");
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Hello);

    ecs_add_id(world, Rel, EcsSparse);
    if (!fragment) ecs_add_id(world, Rel, EcsDontFragment);

    ecs_entity_t base_1 = ecs_new_w(world, Foo);
    ecs_entity_t base_2 = ecs_new_w(world, Bar);
    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, base_1);
    ecs_add_pair(world, e1, Rel, base_2);

    test_uint(base_1, ecs_get_target_for(world, e1, Rel, Foo));
    test_uint(base_2, ecs_get_target_for(world, e1, Rel, Bar));
    test_uint(0, ecs_get_target_for(world, e1, Rel, Hello));

    ecs_fini(world);
}

void Sparse_target_for_self_sparse_component(void) {
    if (!fragment) test_quarantine("April 7 2025");
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Hello);

    ecs_add_id(world, Foo, EcsSparse);
    if (!fragment) ecs_add_id(world, Foo, EcsDontFragment);
    ecs_add_id(world, Bar, EcsSparse);
    if (!fragment) ecs_add_id(world, Bar, EcsDontFragment);
    ecs_add_id(world, Hello, EcsSparse);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_add(world, e1, Bar);

    test_uint(e1, ecs_get_target_for(world, e1, Rel, Foo));
    test_uint(e1, ecs_get_target_for(world, e1, Rel, Bar));
    test_uint(0, ecs_get_target_for(world, e1, Rel, Hello));

    ecs_fini(world);
}

void Sparse_target_for_base_sparse_component(void) {
    if (!fragment) test_quarantine("April 7 2025");
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Hello);

    ecs_add_id(world, Foo, EcsSparse);
    if (!fragment) ecs_add_id(world, Foo, EcsDontFragment);
    ecs_add_id(world, Bar, EcsSparse);
    if (!fragment) ecs_add_id(world, Bar, EcsDontFragment);
    ecs_add_id(world, Hello, EcsSparse);

    ecs_entity_t base_1 = ecs_new_w(world, Foo);
    ecs_entity_t base_2 = ecs_new_w(world, Bar);
    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, base_1);
    ecs_add_pair(world, e1, Rel, base_2);

    test_uint(base_1, ecs_get_target_for(world, e1, Rel, Foo));
    test_uint(base_2, ecs_get_target_for(world, e1, Rel, Bar));
    test_uint(0, ecs_get_target_for(world, e1, Rel, Hello));

    ecs_fini(world);
}

void Sparse_defer_ensure(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Velocity), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Velocity), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Velocity), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Velocity), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Velocity), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Velocity), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Velocity), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Velocity), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Velocity), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Velocity), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Velocity), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Velocity), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ecs_add_id(world, ecs_id(Velocity), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Velocity), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);

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

void Sparse_dont_fragment_trait_without_sparse_trait(void) {
    if (fragment) {
        test_assert(true);
        return;
    }

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t e = ecs_new(world);
    test_assert(NULL == ecs_get(world, e, Position));

    ecs_add(world, e, Position);
    test_assert(NULL != ecs_get(world, e, Position));

    ecs_fini(world);
}

void Sparse_on_delete_sparse_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ecs_add_pair(world, ecs_id(Position), EcsOnDelete, EcsRemove);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(ecs_has(world, e, Position));

    ecs_delete(world, ecs_id(Position));

    test_assert(ecs_is_alive(world, e));
    test_assert(!ecs_has(world, e, Position));

    ecs_fini(world);
}

void Sparse_on_delete_sparse_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ecs_add_pair(world, ecs_id(Position), EcsOnDelete, EcsDelete);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(ecs_has(world, e, Position));

    ecs_delete(world, ecs_id(Position));

    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void Sparse_on_delete_sparse_panic(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsSparse);
    if (!fragment) ecs_add_id(world, ecs_id(Position), EcsDontFragment);
    ecs_add_pair(world, ecs_id(Position), EcsOnDelete, EcsPanic);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(ecs_has(world, e, Position));

    test_expect_abort();
    ecs_delete(world, ecs_id(Position));
}

void Sparse_on_delete_target_sparse_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    

    ecs_add_id(world, Rel, EcsSparse);
    if (!fragment) ecs_add_id(world, Rel, EcsDontFragment);
    ecs_add_pair(world, Rel, EcsOnDeleteTarget, EcsRemove);

    ecs_entity_t tgt = ecs_new(world);

    ecs_entity_t e = ecs_new_w_pair(world, Rel, tgt);
    test_assert(ecs_has_pair(world, e, Rel, tgt));

    ecs_delete(world, tgt);

    test_assert(ecs_is_alive(world, e));
    test_assert(!ecs_has_pair(world, e, Rel, tgt));

    ecs_fini(world);
}

void Sparse_on_delete_target_sparse_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    

    ecs_add_id(world, Rel, EcsSparse);
    if (!fragment) ecs_add_id(world, Rel, EcsDontFragment);
    ecs_add_pair(world, Rel, EcsOnDeleteTarget, EcsDelete);

    ecs_entity_t tgt = ecs_new(world);

    ecs_entity_t e = ecs_new_w_pair(world, Rel, tgt);
    test_assert(ecs_has_pair(world, e, Rel, tgt));

    ecs_delete(world, tgt);

    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void Sparse_on_delete_target_sparse_panic(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    

    ecs_add_id(world, Rel, EcsSparse);
    if (!fragment) ecs_add_id(world, Rel, EcsDontFragment);
    ecs_add_pair(world, Rel, EcsOnDeleteTarget, EcsPanic);

    ecs_entity_t tgt = ecs_new(world);

    ecs_entity_t e = ecs_new_w_pair(world, Rel, tgt);
    test_assert(ecs_has_pair(world, e, Rel, tgt));

    test_expect_abort();
    ecs_delete(world, tgt);
}
