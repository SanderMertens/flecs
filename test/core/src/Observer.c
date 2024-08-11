#include <core.h>

static
void Observer(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);
}

static
void Observer_w_value_1(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);

    test_int(it->count, 1);
    test_assert(it->entities != NULL);
    test_assert(it->entities[0] != 0);

    Position *p = ecs_field(it, Position, 0);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

static
void Observer_w_value_2(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);

    test_int(it->count, 1);
    test_assert(it->entities != NULL);
    test_assert(it->entities[0] != 0);

    Position *p = ecs_field(it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    Velocity *v = ecs_field(it, Velocity, 1);
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 2);
}

static
void Observer_w_1_value(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);

    test_int(it->count, 1);
    test_assert(it->entities != NULL);
    test_assert(it->entities[0] != 0);

    Position *p = ecs_field(it, Position, 0);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

static
void Observer_w_filter_term(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);

    test_int(it->count, 1);
    test_assert(it->entities != NULL);
    test_assert(it->entities[0] != 0);
}

static
void Observer_w_1_filter_term(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);

    test_int(it->count, 1);
    test_assert(it->entities != NULL);
    test_assert(it->entities[0] != 0);

    Velocity *v = ecs_field(it, Velocity, 1);
    test_int(v->x, 1);
    test_int(v->y, 2);
}

static
void Observer_w_2_filter_terms(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);

    test_int(it->count, 1);
    test_assert(it->entities != NULL);
    test_assert(it->entities[0] != 0);
    test_assert(it->sizes != NULL);

    Mass *m = ecs_field(it, Mass, 2);
    test_int(m[0], 100);
}

static bool dummy_called = false;

static
void Dummy(ecs_iter_t *it) {
    dummy_called = true;
}

void Observer_2_terms_w_on_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{TagA}, {TagB}},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);

    ecs_add_id(world, e, TagB);
    test_int(ctx.invoked, 0);

    ecs_add_id(world, e, TagA);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.c[0][1], TagB);

    ecs_fini(world);
}

void Observer_2_terms_w_on_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{TagA}, {TagB}},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);

    ecs_add_id(world, e, TagA);
    test_int(ctx.invoked, 0);

    ecs_add_id(world, e, TagB);
    test_int(ctx.invoked, 0);

    ecs_remove_id(world, e, TagA);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.c[0][1], TagB);

    ecs_fini(world);
}

void Observer_2_terms_w_on_set_value(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ecs_id(Position)}, {ecs_id(Velocity)}},
        .events = {EcsOnSet},
        .callback = Observer_w_value_2,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);

    ecs_set(world, e, Velocity, {1, 2});
    test_int(ctx.invoked, 0);

    ecs_set(world, e, Position, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.c[0][1], ecs_id(Velocity));

    ecs_fini(world);
}

void Observer_2_terms_w_on_remove_value(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ecs_id(Position)}, {ecs_id(Velocity)}},
        .events = {EcsOnRemove},
        .callback = Observer_w_value_2,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);

    ecs_set(world, e, Velocity, {1, 2});
    test_int(ctx.invoked, 0);

    ecs_set(world, e, Position, {10, 20});
    test_int(ctx.invoked, 0);

    ecs_remove(world, e, Position);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.c[0][1], ecs_id(Velocity));

    ecs_fini(world);
}

void Observer_2_terms_w_on_set_value_self_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {
            {ecs_id(Position), .src.id = EcsSelf|EcsUp}, 
            {ecs_id(Velocity), .src.id = EcsSelf|EcsUp}
        },
        .events = {EcsOnSet},
        .callback = Observer_w_value_2,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);

    ecs_set(world, e, Velocity, {1, 2});
    test_int(ctx.invoked, 0);

    ecs_set(world, e, Position, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.c[0][1], ecs_id(Velocity));

    ecs_fini(world);
}

void Observer_2_terms_w_on_remove_value_self_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {
            {ecs_id(Position), .src.id = EcsSelf|EcsUp}, 
            {ecs_id(Velocity), .src.id = EcsSelf|EcsUp}
        },
        .events = {EcsOnRemove},
        .callback = Observer_w_value_2,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);

    ecs_set(world, e, Velocity, {1, 2});
    test_int(ctx.invoked, 0);

    ecs_set(world, e, Position, {10, 20});
    test_int(ctx.invoked, 0);

    ecs_remove(world, e, Position);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.c[0][1], ecs_id(Velocity));

    ecs_fini(world);
}

void Observer_2_terms_w_on_add_2nd(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{TagA}, {TagB}},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);

    ecs_add_id(world, e, TagA);
    test_int(ctx.invoked, 0);

    ecs_add_id(world, e, TagB);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.c[0][1], TagB);

    ecs_fini(world);
}

void Observer_2_terms_w_on_remove_2nd(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{TagA}, {TagB}},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);

    ecs_add_id(world, e, TagA);
    test_int(ctx.invoked, 0);

    ecs_add_id(world, e, TagB);
    test_int(ctx.invoked, 0);

    ecs_remove_id(world, e, TagB);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.c[0][1], TagB);

    ecs_fini(world);
}

void Observer_2_pair_terms_w_on_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_id_t pair_a = ecs_pair(RelA, ObjA);
    ecs_id_t pair_b = ecs_pair(RelB, ObjB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{pair_a}, {pair_b}},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);

    ecs_add_pair(world, e, RelA, ObjA);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, RelB, ObjB);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], pair_a);
    test_int(ctx.c[0][1], pair_b);

    ecs_fini(world);
}

void Observer_2_pair_terms_w_on_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_id_t pair_a = ecs_pair(RelA, ObjA);
    ecs_id_t pair_b = ecs_pair(RelB, ObjB);    

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{pair_a}, {pair_b}},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);

    ecs_add_pair(world, e, RelA, ObjA);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, RelB, ObjB);
    test_int(ctx.invoked, 0);

    ecs_remove_pair(world, e, RelB, ObjB);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], pair_a);
    test_int(ctx.c[0][1], pair_b);

    ecs_fini(world);
}

void Observer_2_wildcard_pair_terms_w_on_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ecs_pair(RelA, ObjA)}, {ecs_pair(RelB, EcsWildcard)}},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);

    ecs_add_pair(world, e, RelA, ObjA);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, RelB, ObjB);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(RelA, ObjA));
    test_int(ctx.c[0][1], ecs_pair(RelB, ObjB));

    ecs_fini(world);
}

void Observer_2_wildcard_pair_terms_w_on_add_2_matching(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ecs_pair(RelA, ObjA)}, {ecs_pair(RelB, EcsWildcard)}},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);

    ecs_add_pair(world, e, RelA, ObjA);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, RelB, ObjA);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(RelA, ObjA));
    test_int(ctx.c[0][1], ecs_pair(RelB, ObjA));

    ecs_os_zeromem(&ctx);
    ecs_add_pair(world, e, RelB, ObjB);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(RelA, ObjA));
    test_int(ctx.c[0][1], ecs_pair(RelB, ObjB));

    ecs_fini(world);
}

void Observer_2_wildcard_pair_terms_w_on_add_3_matching(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);
    ECS_TAG(world, ObjC);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ecs_pair(RelA, ObjA)}, {ecs_pair(RelB, EcsWildcard)}},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);

    ecs_add_pair(world, e, RelA, ObjA);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, RelB, ObjA);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(RelA, ObjA));
    test_int(ctx.c[0][1], ecs_pair(RelB, ObjA));

    ecs_os_zeromem(&ctx);
    ecs_add_pair(world, e, RelB, ObjC);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(RelA, ObjA));
    test_int(ctx.c[0][1], ecs_pair(RelB, ObjC));

    ecs_os_zeromem(&ctx);
    ecs_add_pair(world, e, RelB, ObjB);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(RelA, ObjA));
    test_int(ctx.c[0][1], ecs_pair(RelB, ObjB));

    ecs_fini(world);
}

void Observer_2_wildcard_pair_terms_w_on_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ecs_pair(RelA, ObjA)}, {ecs_pair(RelB, EcsWildcard)}},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);

    ecs_add_pair(world, e, RelA, ObjA);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, RelB, ObjB);
    test_int(ctx.invoked, 0);

    ecs_remove_pair(world, e, RelB, ObjB);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(RelA, ObjA));
    test_int(ctx.c[0][1], ecs_pair(RelB, ObjB));

    ecs_fini(world);
}

void Observer_on_set_n_entities(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ecs_id(Self)}},
        .events = {EcsOnSet},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e_1 = ecs_new(world);
    test_int(ctx.invoked, 0);
    ecs_set(world, e_1, Self, {e_1});

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e_1);
    test_int(ctx.c[0][0], ecs_id(Self));

    ecs_os_zeromem(&ctx);

    ecs_entity_t e_2 = ecs_new(world);
    test_int(ctx.invoked, 0);
    ecs_set(world, e_2, Self, {e_2});

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e_2);
    test_int(ctx.c[0][0], ecs_id(Self));

    ecs_os_zeromem(&ctx);

    ecs_entity_t e_3 = ecs_new(world);
    test_int(ctx.invoked, 0);
    ecs_set(world, e_3, Self, {e_3});

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e_3);
    test_int(ctx.c[0][0], ecs_id(Self));

    ecs_fini(world);
}

void Observer_on_set_n_entities_2_comp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Obj);
    ECS_COMPONENT(world, Self);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ecs_id(Self)}, {ecs_pair(ecs_id(Self), EcsWildcard)}},
        .events = {EcsOnSet},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e_1 = ecs_new(world);
    ecs_set(world, e_1, Self, {e_1});
    test_int(ctx.invoked, 0);
    ecs_set_pair(world, e_1, Self, Obj, {e_1});

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);
    test_int(ctx.e[0], e_1);
    test_int(ctx.c[0][0], ecs_id(Self));
    test_int(ctx.c[0][1], ecs_pair(ecs_id(Self), Obj));

    ecs_os_zeromem(&ctx);

    ecs_entity_t e_2 = ecs_new(world);
    ecs_set(world, e_2, Self, {e_2});
    test_int(ctx.invoked, 0);
    ecs_set_pair(world, e_2, Self, Obj, {e_2});

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);
    test_int(ctx.e[0], e_2);
    test_int(ctx.c[0][0], ecs_id(Self));
    test_int(ctx.c[0][1], ecs_pair(ecs_id(Self), Obj));

    ecs_fini(world);
}

void Observer_wildcard_pair_w_pred_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);
    ECS_COMPONENT(world, Position);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ecs_pair(ecs_id(Position), EcsWildcard)}},
        .events = {EcsOnSet},
        .callback = Observer_w_value_1,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);
    test_int(ctx.invoked, 0);

    ecs_set_pair(world, e, Position, ObjA, {10, 20});

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(ecs_id(Position), ObjA));

    /* Change existing component without triggering OnSet as the callback
     * expects value {10, 20}, then add a new component with {10, 20} */
    Position *p = ecs_ensure_pair(world, e, Position, ObjA);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    p->x = 30;
    p->y = 40;

    ecs_os_zeromem(&ctx);

    ecs_set_pair(world, e, Position, ObjB, {10, 20});

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(ecs_id(Position), ObjB));

    ecs_fini(world);
}

void Observer_wildcard_pair_w_obj_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_COMPONENT(world, Position);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ ecs_pair(EcsWildcard, ecs_id(Position)) }},
        .events = {EcsOnSet},
        .callback = Observer_w_value_1,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);
    test_int(ctx.invoked, 0);

    ecs_set_pair_second(world, e, RelA, Position, {10, 20});

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(RelA, ecs_id(Position)));

    /* Change existing component without triggering OnSet as the callback
     * expects value {10, 20}, then add a new component with {10, 20} */
    Position *p = ecs_ensure_pair_second(world, e, RelA, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    p->x = 30;
    p->y = 40;

    ecs_os_zeromem(&ctx);

    ecs_set_pair_second(world, e, RelB, Position, {10, 20});

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_pair(RelB, ecs_id(Position)));

    ecs_fini(world);
}

void Observer_2_terms_1_not_w_on_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{TagA}, {TagB, .oper = EcsNot}},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);

    ecs_add_id(world, e, TagB);
    test_int(ctx.invoked, 0);

    ecs_add_id(world, e, TagA);
    test_int(ctx.invoked, 0);

    ecs_clear(world, e);
    test_int(ctx.invoked, 0);
    test_assert(!ecs_has(world, e, TagB));

    ecs_add_id(world, e, TagA);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.c[0][1], TagB);

    ecs_fini(world);
}

void Observer_2_terms_1_not_w_on_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{TagA}, {TagB, .oper = EcsNot}},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);

    ecs_add_id(world, e, TagB);
    test_int(ctx.invoked, 0);

    ecs_add_id(world, e, TagA);
    test_int(ctx.invoked, 0);

    ecs_remove_id(world, e, TagA);
    test_int(ctx.invoked, 0);

    ecs_clear(world, e);
    test_assert(!ecs_has(world, e, TagB));

    ecs_add_id(world, e, TagA);
    test_int(ctx.invoked, 0); 

    ecs_remove_id(world, e, TagA);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.c[0][1], TagB);

    ecs_fini(world);
}

void Observer_2_terms_w_on_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ecs_id(Position)}, {TagB}},
        .events = {EcsOnSet},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);

    ecs_add_id(world, e, TagB);
    test_int(ctx.invoked, 0);

    ecs_add(world, e, Position);
    test_int(ctx.invoked, 0);

    ecs_set(world, e, Position, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.c[0][1], TagB);

    ecs_fini(world);
}

void Observer_2_terms_w_un_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ecs_id(Position)}, {TagB}},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);

    ecs_add_id(world, e, TagB);
    test_int(ctx.invoked, 0);

    ecs_add(world, e, Position);
    test_int(ctx.invoked, 0);

    ecs_set(world, e, Position, {10, 20});
    test_int(ctx.invoked, 0);

    ecs_remove(world, e, Position);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.c[0][1], TagB);

    ecs_fini(world);
}

void Observer_3_terms_2_or_on_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {
            {TagA, .src.id = EcsSelf|EcsUp}, 
            {TagB, .oper = EcsOr, .src.id = EcsSelf|EcsUp}, 
            {TagC, .src.id = EcsSelf|EcsUp}
        },
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);

    ecs_add_id(world, e, TagA);
    test_int(ctx.invoked, 0);

    ecs_add_id(world, e, TagB);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.c[0][1], TagB);

    ecs_os_zeromem(&ctx);

    ecs_add_id(world, e, TagC);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.c[0][1], TagC);

    ecs_fini(world);
}

void Observer_3_terms_2_or_on_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{TagA}, {TagB, .oper = EcsOr}, {TagC }},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);

    ecs_add_id(world, e, TagA);
    test_int(ctx.invoked, 0);

    ecs_add_id(world, e, TagB);
    test_int(ctx.invoked, 0);

    ecs_add_id(world, e, TagC);
    test_int(ctx.invoked, 0);

    ecs_remove_id(world, e, TagC);
    test_int(ctx.invoked, 1);

    ecs_remove_id(world, e, TagB);
    test_int(ctx.invoked, 2);

    ecs_remove_id(world, e, TagA);
    test_int(ctx.invoked, 2);

    ecs_fini(world);
}

void Observer_2_terms_w_from_entity_on_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    
    ecs_entity_t x = ecs_new(world);
    
    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{TagA}, {TagB, .src.id = x}},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);

    ecs_add_id(world, e, TagA);
    test_int(ctx.invoked, 0);

    ecs_add_id(world, e, TagB);
    test_int(ctx.invoked, 0);

    ecs_clear(world, e);
    test_int(ctx.invoked, 0);

    ecs_add_id(world, x, TagB);
    test_int(ctx.invoked, 0);

    ecs_add_id(world, e, TagA);
    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void Observer_2_terms_on_remove_on_clear(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{TagA}, {TagB}},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);

    ecs_add_id(world, e, TagA);
    test_int(ctx.invoked, 0);

    ecs_add_id(world, e, TagB);
    test_int(ctx.invoked, 0);

    ecs_clear(world, e);
    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void Observer_2_terms_on_remove_on_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{TagA}, {TagB}},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);

    ecs_add_id(world, e, TagA);
    test_int(ctx.invoked, 0);

    ecs_add_id(world, e, TagB);
    test_int(ctx.invoked, 0);

    ecs_delete(world, e);
    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void Observer_add_after_delete_observer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t observer = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{.id = ecs_id(Position) }},
        .events = {EcsOnAdd},
        .callback = Dummy
    });

    ecs_entity_t e1 = ecs_new_w(world, Position);
    test_assert(e1 != 0);
    test_assert(ecs_has(world, e1, Position));
    test_int(dummy_called, 1);

    dummy_called = 0;

    ecs_delete(world, observer);
    test_int(dummy_called, 0);

    ecs_entity_t e2 = ecs_new_w(world, Position);
    test_assert(e2 != 0);
    test_assert(ecs_has(world, e2, Position));
    test_int(dummy_called, 0);

    ecs_fini(world);
}

void Observer_remove_after_delete_observer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t observer = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{.id = ecs_id(Position) }},
        .events = {EcsOnRemove},
        .callback = Dummy
    });

    ecs_entity_t e1 = ecs_new_w(world, Position);
    test_assert(e1 != 0);
    test_assert(ecs_has(world, e1, Position));
    test_int(dummy_called, 0);

    ecs_remove(world, e1, Position);
    test_int(dummy_called, 1);

    dummy_called = 0;

    ecs_delete(world, observer);
    test_int(dummy_called, 0);

    ecs_entity_t e2 = ecs_new_w(world, Position);
    test_assert(e2 != 0);
    test_assert(ecs_has(world, e2, Position));
    test_int(dummy_called, 0);

    ecs_remove(world, e2, Position);
    test_int(dummy_called, 0);

    ecs_fini(world);
}

static int ctx_value;
static
void ctx_free(void *ctx) {
    test_assert(&ctx_value == ctx);
    ctx_value ++;
}

static int binding_ctx_value;
static
void binding_ctx_free(void *ctx) {
    test_assert(&binding_ctx_value == ctx);
    binding_ctx_value ++;
}

static int ctx_value_2;
static
void ctx_free_2(void *ctx) {
    test_assert(&ctx_value_2 == ctx);
    ctx_value_2 ++;
}

static int binding_ctx_value_2;
static
void binding_ctx_free_2(void *ctx) {
    test_assert(&binding_ctx_value_2 == ctx);
    binding_ctx_value_2 ++;
}

void Observer_delete_observer_w_ctx(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms =  {{ Tag }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx_value,
        .ctx_free = ctx_free,
        .callback_ctx = &binding_ctx_value,
        .callback_ctx_free = binding_ctx_free
    });
    test_assert(o != 0);

    test_assert(ecs_observer_get(world, o)->ctx == &ctx_value);
    test_assert(ecs_observer_get(world, o)->callback_ctx == &binding_ctx_value);

    ecs_delete(world, o);

    test_int(ctx_value, 1);
    test_int(binding_ctx_value, 1);

    ecs_fini(world);
}

void Observer_update_ctx(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t system = ecs_observer(world, {
        .query.terms = {{.id = Tag}},
        .callback = Dummy,
        .events = {EcsOnAdd},
        .ctx = &ctx_value,
        .ctx_free = ctx_free,
        .callback_ctx = &binding_ctx_value,
        .callback_ctx_free = binding_ctx_free
    });
    test_assert(system != 0);

    test_assert(ecs_observer_get(world, system)->ctx == &ctx_value);
    test_assert(ecs_observer_get(world, system)->callback_ctx 
        == &binding_ctx_value);

    ecs_observer(world, {
        .entity = system,
        .ctx = &ctx_value,
        .ctx_free = ctx_free,
        .callback_ctx = &binding_ctx_value,
        .callback_ctx_free = binding_ctx_free
    });

    test_int(ctx_value, 0);
    test_int(binding_ctx_value, 0);
    test_int(ctx_value_2, 0);
    test_int(binding_ctx_value_2, 0);

    ecs_observer(world, {
        .entity = system,
        .ctx = &ctx_value_2,
        .ctx_free = ctx_free_2,
        .callback_ctx = &binding_ctx_value_2,
        .callback_ctx_free = binding_ctx_free_2
    });

    test_int(ctx_value, 1);
    test_int(binding_ctx_value, 1);
    test_int(ctx_value_2, 0);
    test_int(binding_ctx_value_2, 0);

    ecs_delete(world, system);

    test_int(ctx_value, 1);
    test_int(binding_ctx_value, 1);
    test_int(ctx_value_2, 1);
    test_int(binding_ctx_value_2, 1);

    ecs_fini(world);
}

void Observer_filter_w_strings(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{.first.name = "TagA"}, {.first.name = "TagB"}},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);

    ecs_add_id(world, e, TagB);
    test_int(ctx.invoked, 0);

    ecs_add_id(world, e, TagA);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.c[0][1], TagB);

    ecs_fini(world);
}

static ecs_table_t *trigger_table;

void TypeObserver(ecs_iter_t *it) {
    trigger_table = it->table;
}

void Observer_iter_type_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ Tag }},
        .events = {EcsOnAdd},
        .callback = TypeObserver,
        .ctx = &ctx
    });
    test_assert(t != 0);

    ecs_new_w(world, Tag);

    test_assert(trigger_table != NULL);
    test_assert(trigger_table != NULL);
    const ecs_type_t *type = ecs_table_get_type(trigger_table);
    test_int(type->count, 1);
    test_int(type->array[0], Tag);
    
    ecs_fini(world);
}

void ObserverReadonly(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);
    test_bool(ecs_field_is_readonly(it, 0), true);
}

void Observer_readonly_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ TagA, .inout = EcsIn }},
        .events = {EcsOnAdd},
        .callback = ObserverReadonly,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);
    ecs_add(world, e, TagA);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], TagA);

    ecs_fini(world);
}

void Observer_trigger_on_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx_1 = {0};
    Probe ctx_2 = {0};

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ TagA }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx_1
    });

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ TagA }, { EcsPrefab, .oper = EcsOptional}},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx_2
    });

    ecs_new_w(world, TagA);
    test_int(ctx_1.invoked, 1);
    test_int(ctx_2.invoked, 1);
    ecs_os_zeromem(&ctx_1);
    ecs_os_zeromem(&ctx_2);

    ecs_entity_t e = ecs_new_w_id(world, EcsPrefab);
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 0);
    ecs_add(world, e, TagA);
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 1);
    ecs_os_zeromem(&ctx_2);

    e = ecs_new_w_id(world, EcsDisabled);
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 0);
    ecs_add(world, e, TagA);
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 0);

    ecs_fini(world);
}

void Observer_trigger_on_disabled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx_1 = {0};
    Probe ctx_2 = {0};

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ TagA }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx_1
    });

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ TagA }, { EcsDisabled, .oper = EcsOptional}},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx_2
    });

    ecs_new_w(world, TagA);
    test_int(ctx_1.invoked, 1);
    test_int(ctx_2.invoked, 1);
    ecs_os_zeromem(&ctx_1);
    ecs_os_zeromem(&ctx_2);

    ecs_entity_t e = ecs_new_w_id(world, EcsPrefab);
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 0);
    ecs_add(world, e, TagA);
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 0);
    ecs_os_zeromem(&ctx_2);

    e = ecs_new_w_id(world, EcsDisabled);
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 0);
    ecs_add(world, e, TagA);
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 1);

    ecs_fini(world);
}

static
void UnSet(ecs_iter_t *it) {
    probe_iter(it);
}

static
void UnSetA(ecs_iter_t *it) { }

static
void UnSetB(ecs_iter_t *it) {
}

void Observer_unset_1_of_1(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, UnSet, EcsOnRemove, Position);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_int(ctx.invoked, 0);

    ecs_set(world, e, Position, {10, 20});
    test_int(ctx.invoked, 0);

    ecs_set(world, e, Velocity, {1, 2});
    test_int(ctx.invoked, 0);

    ecs_remove(world, e, Velocity);
    test_int(ctx.invoked, 0);
    
    ecs_remove(world, e, Position);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, UnSet);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    ecs_os_zeromem(&ctx);

    ecs_remove(world, e, Position);
    test_int(ctx.invoked, 0);

    ecs_os_zeromem(&ctx);

    ecs_fini(world);
}

void Observer_unset_1_of_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, UnSet, EcsOnRemove, Position, Velocity);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_int(ctx.invoked, 0);

    ecs_set(world, e, Position, {10, 20});
    test_int(ctx.invoked, 0);

    ecs_set(world, e, Velocity, {1, 2});
    test_int(ctx.invoked, 0);
    
    ecs_remove(world, e, Position);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, UnSet);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0);    

    ecs_os_zeromem(&ctx);

    ecs_remove(world, e, Velocity);
    test_int(ctx.invoked, 0);

    ecs_os_zeromem(&ctx);

    ecs_remove(world, e, Position);
    test_int(ctx.invoked, 0);

    ecs_os_zeromem(&ctx);

    ecs_fini(world);

    test_int(ctx.invoked, 0);
}

void Observer_unset_1_of_3(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_OBSERVER(world, UnSet, EcsOnRemove, Position, Velocity, Mass);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_int(ctx.invoked, 0);

    ecs_set(world, e, Position, {10, 20});
    test_int(ctx.invoked, 0);

    ecs_set(world, e, Velocity, {1, 2});
    test_int(ctx.invoked, 0);

    ecs_set(world, e, Mass, {1});
    test_int(ctx.invoked, 0);    
    
    ecs_remove(world, e, Position);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, UnSet);
    test_int(ctx.term_count, 3);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0);  
    test_int(ctx.c[0][2], ecs_id(Mass));
    test_int(ctx.s[0][2], 0);        

    ecs_os_zeromem(&ctx);

    ecs_remove(world, e, Mass);
    test_int(ctx.invoked, 0);

    ecs_os_zeromem(&ctx);

    ecs_remove(world, e, Velocity);
    test_int(ctx.invoked, 0);

    ecs_os_zeromem(&ctx);

    ecs_remove(world, e, Position);
    test_int(ctx.invoked, 0);

    ecs_os_zeromem(&ctx);

    ecs_fini(world);

    test_int(ctx.invoked, 0);
}

void Observer_unset_on_delete_1(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, UnSet, EcsOnRemove, Position);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);    

    ecs_new_w(world, Position);
    test_int(ctx.invoked, 0);

    ecs_new_w(world, Position);
    test_int(ctx.invoked, 0);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_int(ctx.invoked, 0);

    ecs_delete(world, e);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, UnSet);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);

    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0); 

    ecs_fini(world);
}

void Observer_unset_on_delete_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, UnSet, EcsOnRemove, Position, Velocity);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);    

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_add(world, e1, Velocity);
    test_int(ctx.invoked, 0);

    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_add(world, e2, Velocity);
    test_int(ctx.invoked, 0);

    ecs_entity_t e3 = ecs_new_w(world, Position);
    ecs_add(world, e3, Velocity);
    test_int(ctx.invoked, 0);

    ecs_delete(world, e3);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, UnSet);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e3);

    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0); 
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0);  

    ecs_fini(world);   
}

void Observer_unset_on_delete_3(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_OBSERVER(world, UnSet, EcsOnRemove, Position, Velocity, Mass);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);    

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_add(world, e1, Velocity);
    ecs_add(world, e1, Mass);
    test_int(ctx.invoked, 0);

    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e2, Mass);
    test_int(ctx.invoked, 0);

    ecs_entity_t e3 = ecs_new_w(world, Position);
    ecs_add(world, e3, Velocity);
    ecs_add(world, e3, Mass);
    test_int(ctx.invoked, 0);

    ecs_delete(world, e3);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, UnSet);
    test_int(ctx.term_count, 3);
    test_null(ctx.param);

    test_int(ctx.e[0], e3);

    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0); 
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0); 
    test_int(ctx.c[0][2], ecs_id(Mass));
    test_int(ctx.s[0][2], 0);  
    
    ecs_fini(world);
}

void Observer_unset_on_fini_1(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, UnSet, EcsOnRemove, Position);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);    

    ecs_entity_t e1 = ecs_new_w(world, Position);
    test_int(ctx.invoked, 0);

    ecs_entity_t e2 = ecs_new_w(world, Position);
    test_int(ctx.invoked, 0);

    ecs_entity_t e3 = ecs_new_w(world, Position);
    test_int(ctx.invoked, 0);

    ecs_new_w(world, Velocity);
    test_int(ctx.invoked, 0);

    ecs_fini(world);

    test_int(ctx.count, 3);
    test_int(ctx.system, UnSet);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e3);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e1);

    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);      
}

void Observer_unset_on_fini_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, UnSet, EcsOnRemove, Position, Velocity);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);    

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_add(world, e1, Velocity);
    test_int(ctx.invoked, 0);

    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_add(world, e2, Velocity);
    test_int(ctx.invoked, 0);

    ecs_entity_t e3 = ecs_new_w(world, Position);
    ecs_add(world, e3, Velocity);
    test_int(ctx.invoked, 0);

    ecs_new_w(world, Position);
    test_int(ctx.invoked, 0);

    ecs_new_w(world, Velocity);
    test_int(ctx.invoked, 0);    

    ecs_fini(world);

    test_int(ctx.count, 3);
    test_int(ctx.system, UnSet);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e3);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e1);

    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0); 
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0);  
}

void Observer_unset_on_fini_3(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_OBSERVER(world, UnSet, EcsOnRemove, Position, Velocity, Mass);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);    

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_add(world, e1, Velocity);
    ecs_add(world, e1, Mass);
    test_int(ctx.invoked, 0);

    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e2, Mass);
    test_int(ctx.invoked, 0);

    ecs_entity_t e3 = ecs_new_w(world, Position);
    ecs_add(world, e3, Velocity);
    ecs_add(world, e3, Mass);
    test_int(ctx.invoked, 0);

    ecs_new_w(world, Position);
    test_int(ctx.invoked, 0);

    ecs_new_w(world, Velocity);
    test_int(ctx.invoked, 0); 

    ecs_new_w(world, Mass);
    test_int(ctx.invoked, 0);        

    ecs_fini(world);

    test_int(ctx.count, 3);
    test_int(ctx.system, UnSet);
    test_int(ctx.term_count, 3);
    test_null(ctx.param);

    test_int(ctx.e[0], e3);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e1);

    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0); 
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0); 
    test_int(ctx.c[0][2], ecs_id(Mass));
    test_int(ctx.s[0][2], 0);         
}

void Observer_overlapping_unset_systems(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_OBSERVER(world, UnSetA, EcsOnRemove, Position);
    ECS_OBSERVER(world, UnSetB, EcsOnRemove, Position, Velocity);
    ECS_OBSERVER(world, UnSet, EcsOnRemove, Position, Velocity);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);  

    ecs_entity_t e = ecs_new_w(world, Position);
    ecs_add(world, e, Velocity);
    test_int(ctx.invoked, 0);

    ecs_remove(world, e, Velocity);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, UnSet);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);

    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0); 
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0);   

    ecs_fini(world);
}

static
void UnSet_TestComp(ecs_iter_t *it) {
    if (!ecs_get_ctx(it->world)) {
        return;
    }

    probe_iter(it);

    test_int(it->count, 1);

    Position *p = ecs_field(it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Observer_unset_move_to_nonempty_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_OBSERVER(world, UnSet_TestComp, EcsOnRemove, Position);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);  

    ECS_ENTITY(world, DummyA, Position, Velocity);
    test_int(ctx.invoked, 0);

    ECS_ENTITY(world, DummyB, Position, Velocity);
    test_int(ctx.invoked, 0);

    ECS_ENTITY(world, e, Position, Velocity);
    ecs_set(world, e, Position, {10, 20});
    ecs_set(world, e, Velocity, {20, 10});
    test_int(ctx.invoked, 0);

    ecs_entity_t e2 = ecs_new_w(world, Velocity);
    ecs_set(world, e2, Velocity, {30, 40});
    test_int(ctx.invoked, 0);

    ecs_remove(world, e, Position);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, UnSet_TestComp);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);

    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    /* Prevent system from getting called by fini */
    ecs_set_ctx(world, NULL, NULL);

    ecs_fini(world);
}

static
void UnSet_WriteComp(ecs_iter_t *it) {
    if (!ecs_get_ctx(it->world)) {
        return;
    }

    probe_iter(it);

    test_int(it->count, 1);

    Position *p = ecs_field(it, Position, 0);
    test_assert(p != NULL);

    Velocity *v = ecs_field(it, Velocity, 1);

    test_int(p->x, 10);
    test_int(p->y, 20);

    test_int(v->x, 1);
    test_int(v->y, 2);

    v->x = 2;
    v->y = 3;
}

void Observer_write_in_unset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_OBSERVER(world, UnSet_WriteComp, EcsOnRemove, Position, Velocity);

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_set(world, e, Velocity, {1, 2});

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_remove(world, e, Position);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, UnSet_WriteComp);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);

    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0);    

    /* Prevent system from getting called by fini */
    ecs_set_ctx(world, NULL, NULL);    

    const Velocity *v = ecs_get(world, e, Velocity);
    test_int(v->x, 2);
    test_int(v->y, 3);

    ecs_fini(world);
}

void Observer_filter_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ .id = ecs_id(Position), .inout = EcsInOutNone }},
        .events = {EcsOnSet},
        .callback = Observer_w_filter_term,
        .ctx = &ctx
    });

    /* Create entities before trigger */
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    test_assert(e1 != 0);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.event_id, ecs_id(Position));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.c[0][0], ecs_id(Position));

    ecs_fini(world);
}

void Observer_2_terms_1_filter(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {
            { .id = ecs_id(Position), .inout = EcsInOutNone },
            { .id = ecs_id(Velocity) }
        },
        .events = {EcsOnSet},
        .callback = Observer_w_1_filter_term,
        .ctx = &ctx
    });

    /* Create entities before trigger */
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Velocity, {1, 2}));
    test_assert(e1 != 0);
    test_int(ctx.invoked, 0);

    ecs_set(world, e1, Position, {10, 20});

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.event_id, ecs_id(Position));
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.c[0][1], ecs_id(Velocity));

    ecs_fini(world);
}

void Observer_3_terms_2_filter(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {
            { .id = ecs_id(Position), .inout = EcsInOutNone },
            { .id = ecs_id(Velocity), .inout = EcsInOutNone },
            { .id = ecs_id(Mass) }
        },
        .events = {EcsOnSet},
        .callback = Observer_w_2_filter_terms,
        .ctx = &ctx
    });

    /* Create entities before trigger */
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Velocity, {1, 2}));
    test_assert(e1 != 0);
    test_int(ctx.invoked, 0);

    ecs_set(world, e1, Mass, {100});
    ecs_set(world, e1, Position, {10, 20});

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.event_id, ecs_id(Position));
    test_int(ctx.term_count, 3);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.c[0][2], ecs_id(Mass));

    ecs_fini(world);
}

void Observer_and_from(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_PREFAB(world, Type, TagA, TagB);

    Probe ctx = {0};
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {
            { Type, .oper = EcsAndFrom }
        },
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world);
    test_int(ctx.invoked, 0);

    ecs_add(world, e, TagA);
    test_int(ctx.invoked, 0);

    ecs_add(world, e, TagB);
    test_int(ctx.invoked, 1);

    ecs_remove(world, e, TagA);
    test_int(ctx.invoked, 1);

    ecs_add(world, e, TagA);
    test_int(ctx.invoked, 2);

    ecs_fini(world);
}

void Observer_or_from(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_PREFAB(world, Type, TagA, TagB);

    Probe ctx = {0};
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {
            { Type, .oper = EcsOrFrom }
        },
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world);
    test_int(ctx.invoked, 0);

    ecs_add(world, e, TagA);
    test_int(ctx.invoked, 1);

    ecs_add(world, e, TagB);
    test_int(ctx.invoked, 2);

    ecs_remove(world, e, TagA);
    test_int(ctx.invoked, 2);

    ecs_add(world, e, TagA);
    test_int(ctx.invoked, 3);

    ecs_fini(world);
}

void Observer_and_from_empty(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Type = ecs_new(world);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {
            { Type, .oper = EcsAndFrom }
        },
        .events = {EcsOnAdd, EcsOnRemove, EcsOnSet},
        .callback = Observer,
        .ctx = &ctx
    });

    test_assert(o != 0);

    ecs_fini(world);

    test_int(ctx.invoked, 0);
}

void Observer_or_from_empty(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Type = ecs_new(world);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {
            { Type, .oper = EcsOrFrom }
        },
        .events = {EcsOnAdd, EcsOnRemove, EcsOnSet},
        .callback = Observer,
        .ctx = &ctx
    });

    test_assert(o != 0);

    ecs_fini(world);

    test_int(ctx.invoked, 0);
}

void Observer_and_from_empty_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ecs_entity_t Type = ecs_new(world);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {
            { Type, .oper = EcsAndFrom },
            { TagA }
        },
        .events = {EcsOnAdd, EcsOnRemove, EcsOnSet},
        .callback = Observer,
        .ctx = &ctx
    });

    test_assert(o != 0);

    ecs_entity_t e = ecs_new_w(world, TagA);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][1], TagA);

    ecs_fini(world);
}

void Observer_or_from_empty_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ecs_entity_t Type = ecs_new(world);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {
            { Type, .oper = EcsOrFrom },
            { TagA }
        },
        .events = {EcsOnAdd, EcsOnRemove, EcsOnSet},
        .callback = Observer,
        .ctx = &ctx
    });

    test_assert(o != 0);

    ecs_entity_t e = ecs_new_w(world, TagA);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][1], TagA);

    ecs_fini(world);
}

static int invoke_count = 0;
static ecs_entity_t base_ent = 0;
static ecs_entity_t inst_ent_a = 0;
static ecs_entity_t inst_ent_b = 0;

static void TriggerTwice(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);

    test_assert(base_ent != 0);
    test_assert(inst_ent_a != 0);
    test_assert(inst_ent_b != 0);
    test_int(it->count, 1);

    if (invoke_count == 0) {
        test_assert(it->entities[0] == base_ent);
        invoke_count ++;
    } else if (invoke_count == 1) {
        test_assert(it->entities[0] == inst_ent_b);
        invoke_count ++;
    } else {
        test_int(invoke_count, 2);
        test_assert(it->entities[0] == inst_ent_a);
        invoke_count ++;
    }
}

void Observer_notify_propagated_twice(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_add_pair(world, TagA, EcsOnInstantiate, EcsInherit);

    Probe ctx = {0};
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ TagA }},
        .events = {EcsOnAdd},
        .callback = TriggerTwice,
        .ctx = &ctx
    });
    test_assert(t1 != 0);

    base_ent = ecs_new(world);
    inst_ent_a = ecs_new_w_pair(world, EcsIsA, base_ent);
    ecs_add(world, inst_ent_a, TagB);
    inst_ent_b = ecs_new_w_pair(world, EcsIsA, base_ent);

    test_int(ctx.invoked, 0);

    ecs_add(world, base_ent, TagA);

    test_int(ctx.invoked, 3);
    test_int(invoke_count, 3);

    ecs_fini(world);
}

void Observer_on_add_yield_existing(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    /* Create entities before trigger */
    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ Tag }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx,
        .yield_existing = true
    });

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 3);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, Tag);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);
    test_int(ctx.c[0][0], Tag);

    // Ensure normal triggering also still works
    ecs_os_zeromem(&ctx);
    ecs_new_w(world, Tag);
    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void Observer_on_add_yield_existing_2_tables(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    /* Create entities before trigger */
    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    ecs_add(world, e3, TagB);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ TagA }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx,
        .yield_existing = true
    });

    test_int(ctx.invoked, 2);
    test_int(ctx.count, 3);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);
    test_int(ctx.c[0][0], TagA);

    // Ensure normal triggering also still works
    ecs_os_zeromem(&ctx);
    ecs_new_w(world, TagA);
    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void Observer_on_add_yield_existing_2_terms(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    /* Create entities before trigger */
    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_entity_t e4 = ecs_new_w(world, TagA); /* no match */
    ecs_entity_t e5 = ecs_new_w(world, TagB); /* no match */

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);

    ecs_add(world, e1, TagB);
    ecs_add(world, e2, TagB);
    ecs_add(world, e3, TagB);

    ecs_add(world, e3, TagC);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ TagA }, { TagB }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx,
        .yield_existing = true
    });

    test_int(ctx.invoked, 2);
    test_int(ctx.count, 3);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.c[0][1], TagB);
    test_int(ctx.c[1][0], TagA);
    test_int(ctx.c[1][1], TagB);

    // Ensure normal triggering also still works
    ecs_os_zeromem(&ctx);
    ecs_entity_t e6 = ecs_new_w(world, TagA);
    test_int(ctx.invoked, 0);
    ecs_add(world, e6, TagB);
    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void Observer_on_add_yield_existing_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TgtB);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ ecs_pair(Rel, EcsWildcard ) }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx,
        .yield_existing = true
    });

    test_int(ctx.invoked, 2);
    test_int(ctx.count, 2);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.c[0][0], ecs_pair(Rel, TgtA));
    test_int(ctx.c[1][0], ecs_pair(Rel, TgtB));

    ecs_fini(world);
}

void Observer_on_add_yield_existing_wildcard_multi(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_new_w_pair(world, Rel, TgtB);

    ecs_add(world, e1, Tag);
    ecs_add(world, e2, Tag);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ ecs_pair(Rel, EcsWildcard ) }, { Tag }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx,
        .yield_existing = true
    });

    test_int(ctx.invoked, 2);
    test_int(ctx.count, 2);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.c[0][0], ecs_pair(Rel, TgtA));
    test_int(ctx.c[0][1], Tag);
    test_int(ctx.c[1][0], ecs_pair(Rel, TgtB));
    test_int(ctx.c[1][1], Tag);

    ecs_fini(world);
}

void Observer_on_add_yield_existing_wildcard_multi_w_wildcard_pivot(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_add(world, e1, Tag);
    ecs_add(world, e2, Tag);
    ecs_new_w(world, Tag);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ ecs_pair(Rel, EcsWildcard ) }, { Tag }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx,
        .yield_existing = true
    });

    test_int(ctx.invoked, 2);
    test_int(ctx.count, 2);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.c[0][0], ecs_pair(Rel, TgtA));
    test_int(ctx.c[0][1], Tag);
    test_int(ctx.c[1][0], ecs_pair(Rel, TgtB));
    test_int(ctx.c[1][1], Tag);

    ecs_fini(world);
}

void Observer_observer_superset_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_add_pair(world, Rel, EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new(world);
    ecs_entity_t inst = ecs_new(world);
    ecs_add_pair(world, inst, EcsIsA, base);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ 
            .id = ecs_pair(Rel, EcsWildcard), 
            .src.id = EcsUp, 
            .trav = EcsIsA
        }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(t != 0);

    test_int(ctx.invoked, 0);

    ecs_add_pair(world, base, Rel, ObjA);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, ecs_pair(Rel, ObjA));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], inst);
    test_int(ctx.s[0][0], base);

    ecs_os_zeromem(&ctx);

    ecs_add_pair(world, base, Rel, ObjB);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, ecs_pair(Rel, ObjB));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], inst);
    test_int(ctx.s[0][0], base);

    ecs_fini(world);
}

void Observer_observer_superset_wildcard_add_isa(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_add_pair(world, Rel, EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new(world);
    ecs_add_pair(world, base, Rel, ObjA);
    ecs_add_pair(world, base, Rel, ObjB);

    ecs_entity_t inst = ecs_new(world);
    
    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ 
            .id = ecs_pair(Rel, EcsWildcard), 
            .src.id = EcsUp,
            .trav = EcsIsA
        }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(t != 0);

    test_int(ctx.invoked, 0);

    ecs_add_pair(world, inst, EcsIsA, base);

    test_int(ctx.invoked, 2);
    test_int(ctx.count, 2);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], inst);
    test_int(ctx.s[0][0], base);

    ecs_fini(world);
}

void Observer_observer_superset_wildcard_add_isa_at_offset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_add_pair(world, Rel, EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new(world);
    ecs_add(world, base, Tag);
    ecs_add_pair(world, base, Rel, ObjA);
    ecs_add_pair(world, base, Rel, ObjB);

    ecs_entity_t inst = ecs_new(world);
    
    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ 
            .id = ecs_pair(Rel, EcsWildcard), 
            .src.id = EcsUp,
            .trav = EcsIsA 
        }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(t != 0);

    test_int(ctx.invoked, 0);

    ecs_add_pair(world, inst, EcsIsA, base);

    test_int(ctx.invoked, 2);
    test_int(ctx.count, 2);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], inst);
    test_int(ctx.s[0][0], base);

    ecs_fini(world);
}

void Observer_on_set_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ TagA }},
        .events = {EcsOnSet},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(t1 != 0);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    ecs_add(world, e, TagA);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);

    ecs_fini(world);
}

void Observer_mixed_on_set_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_COMPONENT(world, Position);

    Probe ctx = {0};
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ TagA }, { ecs_id(Position) }},
        .events = {EcsOnSet},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(t1 != 0);

    ecs_entity_t e1 = ecs_new(world);
    test_assert(e1 != 0);

    ecs_add(world, e1, TagA);
    test_int(ctx.invoked, 0);

    ecs_add(world, e1, Position);
    test_int(ctx.invoked, 0);

    ecs_set(world, e1, Position, {10, 20});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e1);
    test_int(ctx.event, EcsOnSet);
    test_int(ctx.event_id, ecs_id(Position));

    ctx = (Probe){0};

    ecs_entity_t e2 = ecs_new(world);
    test_assert(e2 != 0);

    ecs_set(world, e2, Position, {10, 20});
    test_int(ctx.invoked, 0);

    ecs_add(world, e2, TagA);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e2);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);

    ecs_fini(world);
}

void Observer_mixed_un_set_w_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_COMPONENT(world, Position);

    Probe ctx = {0};
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ TagA }, { ecs_id(Position) }},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(t1 != 0);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add(world, e1, TagA);
    ecs_set(world, e1, Position, {10, 20});
    test_int(ctx.invoked, 0);

    ecs_remove(world, e1, TagA);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e1);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.event_id, TagA);

    ctx = (Probe){0};

    ecs_entity_t e2 = ecs_new(world);
    ecs_set(world, e2, Position, {10, 20});
    ecs_add(world, e2, TagA);
    test_int(ctx.invoked, 0);

    ecs_remove(world, e2, Position);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e2);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.event_id, ecs_id(Position));

    ecs_fini(world);
}

void Observer_match_base_w_id_at_offset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    Probe ctx_1 = {0};
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ ecs_id(Position) }},
        .events = {EcsOnSet},
        .callback = Observer,
        .ctx = &ctx_1
    });
    test_assert(t1 != 0);

    Probe ctx_2 = {0};
    ecs_entity_t t2 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ ecs_id(Velocity) }},
        .events = {EcsOnSet},
        .callback = Observer,
        .ctx = &ctx_2
    });
    test_assert(t2 != 0);

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_set(world, base, Position, {10, 20});
    ecs_set(world, base, Velocity, {1, 2});
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 0);

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_int(ctx_1.invoked, 1);
    test_int(ctx_1.count, 1);
    test_int(ctx_1.e[0], inst);
    test_int(ctx_1.event, EcsOnSet);
    test_int(ctx_1.event_id, ecs_id(Position));

    test_int(ctx_2.invoked, 1);
    test_int(ctx_2.count, 1);
    test_int(ctx_2.e[0], inst);
    test_int(ctx_2.event, EcsOnSet);
    test_int(ctx_2.event_id, ecs_id(Velocity));

    ecs_fini(world);
}

static int run_invoked = 0;
static int run_invoked_matched = 0;

static void Run(ecs_iter_t *it) {
    run_invoked ++;
    
    test_assert(it != NULL);
    test_assert(it->next != NULL);
    test_assert(it->callback != NULL);

    while (ecs_iter_next(it)) {
        it->callback(it);
        run_invoked_matched ++;
    }
}

static void Run_w_1_field(ecs_iter_t *it) {    
    run_invoked ++;
    
    test_assert(it != NULL);
    test_assert(it->next != NULL);

    while (ecs_iter_next(it)) {
        test_int(it->count, 1);
        Position *p = ecs_field(it, Position, 0);
        test_int(p->x, 10);
        test_int(p->y, 20);
        
        run_invoked_matched ++;
    }
}

static void Run_w_1_field_w_callback(ecs_iter_t *it) {    
    run_invoked ++;
    
    test_assert(it != NULL);
    test_assert(it->next != NULL);

    while (ecs_iter_next(it)) {
        test_int(it->count, 1);
        Position *p = ecs_field(it, Position, 0);
        test_int(p->x, 10);
        test_int(p->y, 20);

        it->callback(it);
        
        run_invoked_matched ++;
    }
}

static void Run_w_2_fields(ecs_iter_t *it) {
    run_invoked ++;

    test_assert(it != NULL);
    test_assert(it->next != NULL);

    while (ecs_iter_next(it)) {
        test_int(it->count, 1);
        Position *p = ecs_field(it, Position, 0);
        test_int(p->x, 10);
        test_int(p->y, 20);

        Velocity *v = ecs_field(it, Velocity, 1);
        test_int(v->x, 1);
        test_int(v->y, 2);

        run_invoked_matched ++;
    }
}

static void Run_w_2_fields_w_callback(ecs_iter_t *it) {
    run_invoked ++;

    test_assert(it != NULL);
    test_assert(it->next != NULL);

    while (ecs_iter_next(it)) {
        test_int(it->count, 1);
        Position *p = ecs_field(it, Position, 0);
        test_int(p->x, 10);
        test_int(p->y, 20);

        Velocity *v = ecs_field(it, Velocity, 1);
        test_int(v->x, 1);
        test_int(v->y, 2);

        it->callback(it);

        run_invoked_matched ++;
    }
}

void Observer_custom_run_action(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ TagA }},
        .events = {EcsOnAdd},
        .run = Run,
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(t1 != 0);

    ecs_entity_t e = ecs_new_w(world, TagA);

    test_int(run_invoked, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e);
    test_int(ctx.event, EcsOnAdd);
    
    ecs_fini(world);
}

void Observer_custom_run_action_w_field(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ ecs_id(Position) }},
        .events = {EcsOnSet},
        .run = Run_w_1_field,
    });
    test_assert(t1 != 0);

    ecs_insert(world, ecs_value(Position, {10, 20}));

    test_int(run_invoked, 1);
    test_int(run_invoked_matched, 1);
    
    ecs_fini(world);
}

void Observer_custom_run_action_w_2_fields(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ ecs_id(Position) }, { ecs_id(Velocity) }},
        .events = {EcsOnSet},
        .run = Run_w_2_fields,
    });
    test_assert(t1 != 0);

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    test_int(run_invoked, 0);

    ecs_set(world, e, Velocity, {1, 2});

    test_int(run_invoked, 1);
    test_int(run_invoked_matched, 1);
    
    ecs_fini(world);
}

void Observer_custom_run_w_yield_existing(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));

    Probe ctx = {0};
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ ecs_id(Position) }},
        .events = {EcsOnSet},
        .run = Run,
        .callback = Observer,
        .yield_existing = true,
        .ctx = &ctx
    });
    test_assert(t1 != 0);

    test_int(run_invoked, 1);
    test_int(run_invoked_matched, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e1);
    test_int(ctx.event, EcsOnSet);

    run_invoked = 0;
    run_invoked_matched = 0;
    ecs_os_memset(&ctx, 0, sizeof(Probe));

    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {10, 20}));
    test_int(run_invoked, 1);
    test_int(run_invoked_matched, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e2);
    test_int(ctx.event, EcsOnSet);

    ecs_fini(world);
}

void Observer_custom_run_w_yield_existing_1_field(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_insert(world, ecs_value(Position, {10, 20}));

    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ ecs_id(Position) }},
        .events = {EcsOnSet},
        .run = Run_w_1_field,
        .yield_existing = true
    });
    test_assert(t1 != 0);

    test_int(run_invoked, 1);
    test_int(run_invoked_matched, 1);

    run_invoked = 0;
    run_invoked_matched = 0;

    ecs_insert(world, ecs_value(Position, {10, 20}));
    test_int(run_invoked, 1);
    test_int(run_invoked_matched, 1);

    ecs_fini(world);
}

void Observer_custom_run_w_yield_existing_1_field_w_callback(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));

    Probe ctx = {0};
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ ecs_id(Position) }},
        .events = {EcsOnSet},
        .run = Run_w_1_field_w_callback,
        .callback = Observer_w_value_1,
        .yield_existing = true,
        .ctx = &ctx
    });
    test_assert(t1 != 0);

    test_int(run_invoked, 1);
    test_int(run_invoked_matched, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e1);
    test_int(ctx.event, EcsOnSet);

    run_invoked = 0;
    run_invoked_matched = 0;
    ecs_os_memset(&ctx, 0, sizeof(Probe));

    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {10, 20}));
    test_int(run_invoked, 1);
    test_int(run_invoked_matched, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e2);
    test_int(ctx.event, EcsOnSet);

    ecs_fini(world);
}

void Observer_custom_run_w_yield_existing_2_fields(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_insert(world, 
        ecs_value(Position, {10, 20}),
        ecs_value(Velocity, {1, 2}));

    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ ecs_id(Position) }, { ecs_id(Velocity) }},
        .events = {EcsOnSet},
        .run = Run_w_2_fields,
        .callback = Observer,
        .yield_existing = true,
    });
    test_assert(t1 != 0);

    test_int(run_invoked, 1);
    test_int(run_invoked_matched, 1);

    run_invoked = 0;
    run_invoked_matched = 0;

    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {10, 20}));
    test_int(run_invoked, 0);

    ecs_set(world, e2, Velocity, {1, 2});

    test_int(run_invoked, 1);
    test_int(run_invoked_matched, 1);

    ecs_fini(world);
}

void Observer_custom_run_w_yield_existing_2_fields_w_callback(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_insert(world, 
        ecs_value(Position, {10, 20}),
        ecs_value(Velocity, {1, 2}));

    Probe ctx = {0};
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ ecs_id(Position) }, { ecs_id(Velocity) }},
        .events = {EcsOnSet},
        .run = Run_w_2_fields_w_callback,
        .callback = Observer_w_value_2,
        .yield_existing = true,
        .ctx = &ctx
    });
    test_assert(t1 != 0);

    test_int(run_invoked, 1);
    test_int(run_invoked_matched, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e1);
    test_int(ctx.event, EcsOnSet);

    run_invoked = 0;
    run_invoked_matched = 0;
    ecs_os_memset(&ctx, 0, sizeof(Probe));

    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {10, 20}));
    test_int(run_invoked, 0);

    ecs_set(world, e2, Velocity, {1, 2});

    test_int(run_invoked, 1);
    test_int(run_invoked_matched, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e2);
    test_int(ctx.event, EcsOnSet);

    ecs_fini(world);
}

void Observer_custom_run_action_2_terms(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ TagA }, { TagB }},
        .events = {EcsOnAdd},
        .run = Run,
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(t1 != 0);

    ecs_entity_t e = ecs_new_w(world, TagA);
    test_int(run_invoked, 0);

    ecs_add(world, e, TagB);

    test_int(run_invoked, 1);
    test_int(run_invoked_matched, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e);
    test_int(ctx.event, EcsOnAdd);
    
    ecs_fini(world);
}

void Observer_custom_run_action_w_iter_next_2_terms(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ TagA }, { TagB }},
        .events = {EcsOnAdd},
        .run = Run,
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(t1 != 0);

    ecs_entity_t e = ecs_new_w(world, TagA);
    test_int(run_invoked, 0);

    ecs_add(world, e, TagB);
    test_int(run_invoked, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e);
    test_int(ctx.event, EcsOnAdd);
    
    ecs_fini(world);
}

void Observer_read_in_on_remove_after_add_other_w_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);
    ECS_TAG(world, Tgt);

    Probe ctx = {0};
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {
            { .id = ecs_pair_t(Position, Tgt)},
            { .id = Tag, .oper = EcsNot }
        },
        .events = {EcsOnRemove},
        .callback = Observer_w_1_value,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_insert(world, ecs_value_pair(Position, Tgt, { 10, 20 }));
    test_int(ctx.invoked, 0);

    ecs_add(world, e, Tag);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e);
    test_int(ctx.event, EcsOnRemove);

    ecs_fini(world);
}

void Observer_observer_w_short_notation(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer(world, {
        .query.terms = {{
            .id = Foo
        }},
        .events = { EcsOnAdd },
        .callback = Observer,
        .ctx = &ctx
    });

    test_assert(o != 0);

    ecs_entity_t e = ecs_new_w(world, Foo);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e);
    test_int(ctx.event, EcsOnAdd);

    ecs_fini(world);
}

void Observer_observer_w_filter_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms = {{ .id = TagA }, { .id = TagB, .inout = EcsInOutFilter }},
        .events = { EcsOnAdd },
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world);
    test_int(ctx.invoked, 0);

    ecs_add(world, e, TagB);
    test_int(ctx.invoked, 0);

    ecs_add(world, e, TagA);
    test_int(ctx.invoked, 1);
    
    ecs_remove(world, e, TagB);
    ecs_add(world, e, TagB);
    test_int(ctx.invoked, 1);

    ecs_clear(world, e);
    test_int(ctx.invoked, 1);

    ecs_add(world, e, TagA);
    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

static int free_ctx_invoked = 0;

static
void free_ctx(void *ctx) {
    free_ctx_invoked ++;
    ecs_os_free(ctx);
}

static int observer_w_ctx_invoked = 0;

static void Observer_w_ctx(ecs_iter_t *it) {
    test_assert(it->ctx != NULL);
    test_int(*(int*)it->ctx, 10);
    observer_w_ctx_invoked ++;
}

void Observer_multi_observer_w_ctx_free(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    int *ctx = ecs_os_malloc_t(int);
    *ctx = 10;

    ecs_entity_t o = ecs_observer(world, {
        .query.terms = {{ .id = TagA }, { .id = TagB }},
        .events = { EcsOnAdd },
        .callback = Observer_w_ctx,
        .ctx = ctx,
        .ctx_free = free_ctx
    });

    test_int(observer_w_ctx_invoked, 0);
    test_int(free_ctx_invoked, 0);

    ecs_entity_t e = ecs_new_w(world, TagA);
    test_int(observer_w_ctx_invoked, 0);
    test_int(free_ctx_invoked, 0);
    ecs_add(world, e, TagB);
    test_int(observer_w_ctx_invoked, 1);
    test_int(free_ctx_invoked, 0);

    ecs_delete(world, o);

    test_int(free_ctx_invoked, 1);

    ecs_fini(world);

    test_int(free_ctx_invoked, 1);
}

void Observer_propagate_after_on_delete_clear_action(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_entity_t tgt = ecs_new(world);
    ecs_entity_t parent = ecs_new(world);
    ecs_add_pair(world, parent, Rel, tgt);

    Probe ctx = {0};

    ecs_observer(world, {
        .query.terms = {{ 
            .id = ecs_pair(Rel, EcsWildcard), 
            .src.id = EcsUp, 
            .trav = EcsChildOf 
        }},
        .events = { EcsOnRemove },
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    test_int(ctx.invoked, 0);

    ecs_delete(world, tgt);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], child);
    test_int(ctx.s[0][0], parent);
    test_int(ctx.event, EcsOnRemove);

    ecs_fini(world);
}

void Observer_on_add_after_batch_w_exclusive_adds(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, Exclusive);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    Probe ctx = {0};

    ecs_observer(world, {
        .query.terms = {{ 
            ecs_pair(Rel, EcsWildcard), 
            .src.id = EcsSelf
        }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });
     

    ecs_entity_t e = ecs_new(world);

    ecs_defer_begin(world);
    ecs_add_pair(world, e, Rel, TgtA);
    ecs_add_pair(world, e, Rel, TgtB);
    ecs_defer_end(world);

    test_assert(!ecs_has_pair(world, e, Rel, TgtA));
    test_assert(ecs_has_pair(world, e, Rel, TgtB));

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][0], ecs_pair(Rel, TgtB));
    test_int(ctx.event, EcsOnAdd);
    
    ecs_fini(world);
}

void Observer_propagate_match_relationship_w_self_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_add_pair(world, TagA, EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, TagB, EcsOnInstantiate, EcsInherit);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms = {{ TagB, .src.id = EcsSelf|EcsUp, .trav = EcsIsA }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_entity_t parent = ecs_new(world);
    ecs_add(world, parent, TagA);
    ecs_new_w_pair(world, EcsChildOf, parent);
    test_int(ctx.invoked, 0);

    ecs_add(world, parent, TagB);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], parent);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][0], TagB);
    test_int(ctx.event, EcsOnAdd);

    ecs_fini(world);
}

void Observer_propagate_match_relationship_w_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_add_pair(world, TagA, EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, TagB, EcsOnInstantiate, EcsInherit);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms = {{ TagB, .src.id = EcsUp, .trav = EcsIsA }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_entity_t parent = ecs_new(world);
    ecs_add(world, parent, TagA);
    ecs_new_w_pair(world, EcsChildOf, parent);
    test_int(ctx.invoked, 0);

    ecs_add(world, parent, TagB);
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Observer_propagate_isa_of_parent_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_add_pair(world, TagA, EcsOnInstantiate, EcsInherit);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms = {{ TagA, .src.id = EcsUp, .trav = EcsChildOf }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new(world);
    ecs_entity_t parent = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);

    test_int(ctx.invoked, 0);

    ecs_add(world, base, TagA);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], child);
    test_int(ctx.s[0][0], base);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.event, EcsOnAdd);

    ecs_fini(world);
}

void Observer_propagate_isa_of_parent_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_add_pair(world, TagA, EcsOnInstantiate, EcsInherit);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms = {{ TagA, .src.id = EcsUp, .trav = EcsChildOf }},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new(world);
    ecs_add(world, base, TagA);
    ecs_entity_t parent = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);

    test_int(ctx.invoked, 0);

    ecs_remove(world, base, TagA);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], child);
    test_int(ctx.s[0][0], base);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.event, EcsOnRemove);

    ecs_fini(world);
}

void Observer_propagate_isa_of_parent_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms = {{ ecs_id(Position), .src.id = EcsUp, .trav = EcsChildOf }},
        .events = {EcsOnSet},
        .callback = Observer_w_1_value,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_insert(world, ecs_value(Position, {0, 0}));
    ecs_entity_t parent = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);

    test_int(ctx.invoked, 0);

    ecs_set(world, base, Position, {10, 20});

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], child);
    test_int(ctx.s[0][0], base);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.event, EcsOnSet);

    ecs_fini(world);
}

void Observer_propagate_add_childof_of_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms = {{ TagA, .src.id = EcsUp, .trav = EcsChildOf }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_entity_t grandparent = ecs_new(world);
    ecs_add(world, grandparent, TagA);
    ecs_entity_t parent = ecs_new(world);
    ecs_new_w_pair(world, EcsChildOf, parent);

    test_int(ctx.invoked, 0);

    ecs_add_pair(world, parent, EcsChildOf, grandparent);

    test_int(ctx.invoked, 2);
    test_int(ctx.count, 2);
    test_int(ctx.s[0][0], grandparent);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.event, EcsOnAdd);

    ecs_fini(world);
}

void Observer_propagate_add_childof_of_parent_w_siblings(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms = {{ TagA, .src.id = EcsUp, .trav = EcsChildOf }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_entity_t grandparent = ecs_new(world);
    ecs_add(world, grandparent, TagA);

    {
        ecs_entity_t parent = ecs_new_w_pair(world, EcsChildOf, grandparent);
        ecs_new_w_pair(world, EcsChildOf, parent);

        test_int(ctx.invoked, 2);
        test_int(ctx.count, 2);
        test_int(ctx.e[0], parent);
        test_int(ctx.s[0][0], grandparent);
        test_int(ctx.c[0][0], TagA);
        test_int(ctx.event, EcsOnAdd);
    }

    ecs_os_zeromem(&ctx);

    {
        ecs_entity_t parent = ecs_new_w_pair(world, EcsChildOf, grandparent);
        ecs_new_w_pair(world, EcsChildOf, parent);

        test_int(ctx.invoked, 2);
        test_int(ctx.count, 2);
        test_int(ctx.e[0], parent);
        test_int(ctx.s[0][0], grandparent);
        test_int(ctx.c[0][0], TagA);
        test_int(ctx.event, EcsOnAdd);
    }

    ecs_os_zeromem(&ctx);

    {
        ecs_entity_t parent = ecs_new_w_pair(world, EcsChildOf, grandparent);
        ecs_new_w_pair(world, EcsChildOf, parent);

        test_int(ctx.invoked, 2);
        test_int(ctx.count, 2);
        test_int(ctx.e[0], parent);
        test_int(ctx.s[0][0], grandparent);
        test_int(ctx.c[0][0], TagA);
        test_int(ctx.event, EcsOnAdd);
    }

    ecs_fini(world);
}

void Observer_propagate_add_childof_w_parent_w_base(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_add_pair(world, TagA, EcsOnInstantiate, EcsInherit);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms = {{ TagA, .src.id = EcsUp, .trav = EcsChildOf }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new_w(world, TagA);
    ecs_entity_t parent = ecs_new_w_pair(world, EcsIsA, base);
    
    test_int(ctx.invoked, 0);
    
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], child);
    test_int(ctx.s[0][0], base);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.event, EcsOnAdd);

    ecs_fini(world);
}

void Observer_propagate_remove_childof_w_parent_w_base(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_add_pair(world, TagA, EcsOnInstantiate, EcsInherit);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms = {{ TagA, .src.id = EcsUp, .trav = EcsChildOf }},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new_w(world, TagA);
    ecs_entity_t parent = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);

    test_int(ctx.invoked, 0);

    ecs_remove_pair(world, child, EcsChildOf, parent);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], child);
    test_int(ctx.s[0][0], base);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.event, EcsOnRemove);

    ecs_fini(world);
}

void Observer_propagate_remove_childof_of_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms = {{ TagA, .src.id = EcsUp, .trav = EcsChildOf }},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_entity_t grandparent = ecs_new(world);
    ecs_add(world, grandparent, TagA);
    ecs_entity_t parent = ecs_new_w_pair(world, EcsChildOf, grandparent);
    ecs_new_w_pair(world, EcsChildOf, parent);

    test_int(ctx.invoked, 0);

    ecs_remove_pair(world, parent, EcsChildOf, grandparent);

    test_int(ctx.invoked, 2);
    test_int(ctx.count, 2);
    test_int(ctx.s[0][0], grandparent);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.event, EcsOnRemove);

    ecs_fini(world);
}

void Observer_propagate_add_isa_of_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_add_pair(world, TagA, EcsOnInstantiate, EcsInherit);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms = {{ TagA, .src.id = EcsUp, .trav = EcsChildOf }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new(world);
    ecs_add(world, base, TagA);
    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);

    test_int(ctx.invoked, 0);

    ecs_add_pair(world, parent, EcsIsA, base);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], child);
    test_int(ctx.s[0][0], base);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.event, EcsOnAdd);

    ecs_fini(world);
}

void Observer_propagate_remove_isa_of_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_add_pair(world, TagA, EcsOnInstantiate, EcsInherit);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms = {{ TagA, .src.id = EcsUp, .trav = EcsChildOf }},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new(world);
    ecs_add(world, base, TagA);
    ecs_entity_t parent = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);

    test_int(ctx.invoked, 0);

    ecs_remove_pair(world, parent, EcsIsA, base);    

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], child);
    test_int(ctx.s[0][0], base);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.event, EcsOnRemove);

    ecs_fini(world);
}

void Observer_propagate_add_childof_of_base(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_add_pair(world, TagA, EcsOnInstantiate, EcsInherit);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms = {{ TagA, .src.id = EcsUp, .trav = EcsChildOf }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new(world);
    ecs_add(world, base, TagA);
    ecs_entity_t parent = ecs_new(world);
    ecs_new_w_pair(world, EcsIsA, parent);

    test_int(ctx.invoked, 0);

    ecs_add_pair(world, parent, EcsChildOf, base);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], parent);
    test_int(ctx.s[0][0], base);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.event, EcsOnAdd);

    ecs_fini(world);
}

void Observer_propagate_remove_childof_of_base(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_add_pair(world, TagA, EcsOnInstantiate, EcsInherit);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms = {{ TagA, .src.id = EcsUp, .trav = EcsChildOf }},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new(world);
    ecs_add(world, base, TagA);
    ecs_entity_t parent = ecs_new_w_pair(world, EcsChildOf, base);
    ecs_new_w_pair(world, EcsIsA, parent);

    test_int(ctx.invoked, 0);

    ecs_remove_pair(world, parent, EcsChildOf, base);    

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], parent);
    test_int(ctx.s[0][0], base);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.event, EcsOnRemove);

    ecs_fini(world);
}

void Observer_emit_for_parent_w_prefab_child_and_instance(void) {
    ecs_world_t* world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms = {
            { .id = ecs_id(Position) }
        },
        .events = { EcsOnSet },
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world);
    ecs_entity_t p = ecs_new_w_pair(world, EcsChildOf, e);
    ecs_entity_t child_1 = ecs_new_w_pair(world, EcsChildOf, e);
    ecs_add_pair(world, child_1, EcsIsA, p);

    test_int(ctx.invoked, 0);

    ecs_set(world, e, Position, {10, 20});

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.event, EcsOnSet);

    ecs_fini(world);
}

void Observer_observer_w_2_fixed_src(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t Foo  = ecs_new(world);
    ecs_entity_t Bar = ecs_new(world);

    ecs_entity_t e1  = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);

    Probe ctx = {0};
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {
            { Foo, .src.id = e1 },
            { Bar, .src.id = e2 },
        },
        .events = { EcsOnAdd },
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_add_id(world, e1, Foo);
    test_int(ctx.invoked, 0);

    ecs_add_id(world, e2, Bar);
    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void Observer_emit_for_recreated_id_after_remove_all(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e1, Rel, Tgt);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms = {
            { TagA },
            { ecs_pair(Rel, Tgt) }
        },
        .events = { EcsOnAdd },
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_remove_all(world, ecs_pair(Rel, Tgt));
    test_assert(ecs_id_is_valid(world, ecs_pair(Rel, Tgt)));
    test_assert(ecs_is_alive(world, e1));

    test_int(0, ctx.invoked);

    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e2, Rel, Tgt);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e2);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.c[0][1], ecs_pair(Rel, Tgt));
    test_int(ctx.event, EcsOnAdd);

    ecs_fini(world);
}

void Observer_emit_for_recreated_id_after_remove_all_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e1, Rel, Tgt);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms = {
            { TagA },
            { ecs_pair(Rel, Tgt) }
        },
        .events = { EcsOnAdd },
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_remove_all(world, ecs_pair(Rel, EcsWildcard));
    test_assert(ecs_id_is_valid(world, ecs_pair(Rel, Tgt)));
    test_assert(ecs_is_alive(world, e1));

    test_int(0, ctx.invoked);

    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e2, Rel, Tgt);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e2);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.c[0][1], ecs_pair(Rel, Tgt));
    test_int(ctx.event, EcsOnAdd);

    ecs_fini(world);
}

void Observer_emit_for_recreated_id_after_delete_with(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e1, Rel, Tgt);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms = {
            { TagA },
            { ecs_pair(Rel, Tgt) }
        },
        .events = { EcsOnAdd },
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_delete_with(world, ecs_pair(Rel, Tgt));
    test_assert(ecs_id_is_valid(world, ecs_pair(Rel, Tgt)));
    test_assert(!ecs_is_alive(world, e1));

    test_int(0, ctx.invoked);

    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e2, Rel, Tgt);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e2);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.c[0][1], ecs_pair(Rel, Tgt));
    test_int(ctx.event, EcsOnAdd);

    ecs_fini(world);
}

void Observer_emit_for_recreated_id_after_delete_with_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e1, Rel, Tgt);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms = {
            { TagA },
            { ecs_pair(Rel, Tgt) }
        },
        .events = { EcsOnAdd },
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_delete_with(world, ecs_pair(Rel, EcsWildcard));
    test_assert(ecs_id_is_valid(world, ecs_pair(Rel, Tgt)));
    test_assert(!ecs_is_alive(world, e1));

    test_int(0, ctx.invoked);

    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e2, Rel, Tgt);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e2);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.c[0][1], ecs_pair(Rel, Tgt));
    test_int(ctx.event, EcsOnAdd);

    ecs_fini(world);
}

void Observer_delete_observed_id(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e1, Rel, Tgt);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms = {
            { TagA },
            { ecs_pair(Rel, Tgt) }
        },
        .events = { EcsOnAdd },
        .callback = Observer,
        .ctx = &ctx
    });

    test_expect_abort();
    ecs_delete(world, TagA);
}

void Observer_delete_observed_rel(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e1, Rel, Tgt);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms = {
            { TagA },
            { ecs_pair(Rel, Tgt) }
        },
        .events = { EcsOnAdd },
        .callback = Observer,
        .ctx = &ctx
    });

    test_expect_abort();
    ecs_delete(world, Rel);
}

void Observer_delete_observed_tgt(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e1, Rel, Tgt);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms = {
            { TagA },
            { ecs_pair(Rel, Tgt) }
        },
        .events = { EcsOnAdd },
        .callback = Observer,
        .ctx = &ctx
    });

    test_expect_abort();
    ecs_delete(world, Tgt);
}

static int pair_x = 0;
static int32_t pair_column = 0;

static
void OnTagPair(ecs_iter_t *it) {
    test_int(it->count, 1);
    pair_column = it->trs[0]->index;
    probe_iter(it);
}

static
void OnPair(ecs_iter_t *it) {
    test_int(it->count, 1);
    Position *p = ecs_field(it, Position, 0);
    pair_x = p->x;
    pair_column = it->trs[0]->index;
    probe_iter(it);
}

void Observer_on_add_2_pairs_w_uni_observer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);
    ECS_TAG(world, Tag);
    ECS_OBSERVER(world, OnTagPair, EcsOnAdd, (Rel, *));

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Tag);
    ecs_add_pair(world, e, Rel, TgtA);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnTagPair);
    test_int(ctx.term_count, 1);
    test_int(ctx.event_id, ecs_pair(Rel, TgtA));
    test_int(pair_column, 1);

    ecs_os_zeromem(&ctx);
    ecs_add_pair(world, e, Rel, TgtB);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnTagPair);
    test_int(ctx.term_count, 1);
    test_int(ctx.event_id, ecs_pair(Rel, TgtB));
    test_int(pair_column, 2);

    ecs_os_zeromem(&ctx);
    ecs_add_pair(world, e, Rel, TgtC);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnTagPair);
    test_int(ctx.term_count, 1);
    test_int(ctx.event_id, ecs_pair(Rel, TgtC));
    test_int(pair_column, 3);

    ecs_fini(world);
}

void Observer_on_add_2_pairs_w_multi_observer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);
    ECS_TAG(world, Tag);
    ECS_OBSERVER(world, OnTagPair, EcsOnAdd, (Rel, *), Tag);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Tag);
    ecs_add_pair(world, e, Rel, TgtA);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnTagPair);
    test_int(ctx.term_count, 2);
    test_int(ctx.event_id, ecs_pair(Rel, TgtA));
    test_int(pair_column, 1);

    ecs_os_zeromem(&ctx);
    ecs_add_pair(world, e, Rel, TgtB);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnTagPair);
    test_int(ctx.term_count, 2);
    test_int(ctx.event_id, ecs_pair(Rel, TgtB));
    test_int(pair_column, 2);

    ecs_os_zeromem(&ctx);
    ecs_add_pair(world, e, Rel, TgtC);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnTagPair);
    test_int(ctx.term_count, 2);
    test_int(ctx.event_id, ecs_pair(Rel, TgtC));
    test_int(pair_column, 3);

    ecs_fini(world);
}

void Observer_on_set_2_pairs_w_uni_observer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);
    ECS_TAG(world, Tag);
    ECS_OBSERVER(world, OnPair, EcsOnSet, (Position, *));

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Tag);
    ecs_set_pair(world, e, Position, TgtA, {1});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPair);
    test_int(ctx.term_count, 1);
    test_int(ctx.event_id, ecs_pair(ecs_id(Position), TgtA));
    test_int(pair_column, 1);
    test_int(pair_x, 1);

    ecs_os_zeromem(&ctx);
    ecs_set_pair(world, e, Position, TgtB, {2});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPair);
    test_int(ctx.term_count, 1);
    test_int(ctx.event_id, ecs_pair(ecs_id(Position), TgtB));
    test_int(pair_column, 2);
    test_int(pair_x, 2);

    ecs_os_zeromem(&ctx);
    ecs_set_pair(world, e, Position, TgtC, {3});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPair);
    test_int(ctx.term_count, 1);
    test_int(ctx.event_id, ecs_pair(ecs_id(Position), TgtC));
    test_int(pair_column, 3);
    test_int(pair_x, 3);

    ecs_fini(world);
}

void Observer_on_set_2_pairs_w_multi_observer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);
    ECS_TAG(world, Tag);
    ECS_OBSERVER(world, OnPair, EcsOnSet, (Position, *), Tag);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Tag);
    ecs_set_pair(world, e, Position, TgtA, {1});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPair);
    test_int(ctx.term_count, 2);
    test_int(ctx.event_id, ecs_pair(ecs_id(Position), TgtA));
    test_int(pair_column, 1);
    test_int(pair_x, 1);

    ecs_os_zeromem(&ctx);
    ecs_set_pair(world, e, Position, TgtB, {2});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPair);
    test_int(ctx.term_count, 2);
    test_int(ctx.event_id, ecs_pair(ecs_id(Position), TgtB));
    test_int(pair_column, 2);
    test_int(pair_x, 2);

    ecs_os_zeromem(&ctx);
    ecs_set_pair(world, e, Position, TgtC, {3});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPair);
    test_int(ctx.term_count, 2);
    test_int(ctx.event_id, ecs_pair(ecs_id(Position), TgtC));
    test_int(pair_column, 3);
    test_int(pair_x, 3);

    ecs_fini(world);
}

void Observer_on_remove_target_from_base_at_offset(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t R = ecs_new(world);
    ecs_entity_t T1 = ecs_new(world);
    ecs_entity_t T2 = ecs_new(world);
    ecs_entity_t C = ecs_new(world);

    ecs_add_pair(world, R, EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, C, EcsOnInstantiate, EcsInherit);

    Probe ctx = { 0 };
    ecs_entity_t o = ecs_observer(world, {
        .query.terms = {
            { .id = ecs_pair(R, EcsWildcard), .src.id = EcsUp, .trav = EcsIsA },
            { .id = C },
        },
        .events = { EcsOnRemove },
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new(world);
    ecs_add_pair(world, base, R, T1);
    ecs_add_pair(world, base, R, T2);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsIsA, base);
    ecs_add_id(world, e, C);

    test_int(ctx.invoked, 0);
    ecs_delete(world, T2);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.term_count, 2);
    test_int(ctx.event_id, ecs_pair(R, T2));
    test_int(ctx.event, EcsOnRemove);

    ecs_fini(world);
}

static void Observer_base_component(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);

    Position *p = ecs_field(it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 40);
}

void Observer_on_remove_target_component_from_base_at_offset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t T1 = ecs_new(world);
    ecs_entity_t T2 = ecs_new(world);
    ecs_entity_t C = ecs_new(world);

    ecs_add_pair(world, C, EcsOnInstantiate, EcsInherit);

    Probe ctx = { 0 };
    ecs_entity_t o = ecs_observer(world, {
        .query.terms = {
            { .id = ecs_pair(ecs_id(Position), EcsWildcard), .src.id = EcsUp, .trav = EcsIsA },
            { .id = C },
        },
        .events = { EcsOnRemove },
        .callback = Observer_base_component,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new(world);
    ecs_set_pair(world, base, Position, T1, {10, 20});
    ecs_set_pair(world, base, Position, T2, {30, 40});

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsIsA, base);
    ecs_add_id(world, e, C);

    test_int(ctx.invoked, 0);
    ecs_delete(world, T2);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, o);
    test_int(ctx.term_count, 2);
    test_int(ctx.e[0], e);
    test_int(ctx.s[0][0], base);
    test_int(ctx.s[0][1], 0);
    test_int(ctx.event_id, ecs_pair(ecs_id(Position), T2));
    test_int(ctx.event, EcsOnRemove);

    ecs_delete(world, o);

    ecs_fini(world);
}

static void Observer_w_other_table(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);
    test_assert(it->table != NULL);
    test_assert(it->other_table != NULL);
}

static void Observer_dummy(ecs_iter_t *it) {}

void Observer_wildcard_propagate_w_other_table(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t tag = ecs_new(world);
    ecs_entity_t rel = ecs_new(world);
    ecs_entity_t tgt_1 = ecs_new(world);
    
    ecs_entity_t parent = ecs_new_w_id(world, tag);
    ecs_new_w_pair(world, EcsChildOf, parent);

    Probe ctx_parent = {0};

    ecs_observer(world, {
        .query.terms = {{ ecs_pair(rel, EcsWildcard) }},
        .events = {EcsOnAdd},
        .callback = Observer_dummy
    });

    ecs_observer(world, {
        .query.terms = {{ ecs_pair(rel, EcsWildcard) }},
        .events = {EcsWildcard},
        .callback = Observer_w_other_table,
        .ctx = &ctx_parent
    });

    ecs_observer(world, {
        .query.terms = {{ ecs_pair(rel, EcsWildcard), .src.id = EcsUp }},
        .events = {EcsWildcard},
        .callback = Observer_dummy
    });

    ecs_add_pair(world, parent, rel, tgt_1);

    test_int(ctx_parent.invoked, 1);

    ecs_fini(world);
}

void Observer_disable_observer(void) {
    ecs_world_t *ecs = ecs_mini();

    ECS_COMPONENT(ecs, Position);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer(ecs, {
        .query.terms = {
            { .id = ecs_id(Position) }
        },
        .events = { EcsOnAdd },
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_new_w(ecs, Position);
    test_int(ctx.invoked, 1);
    ecs_os_zeromem(&ctx);

    ecs_enable(ecs, o, false);

    ecs_new_w(ecs, Position);
    test_int(ctx.invoked, 0);
    ecs_os_zeromem(&ctx);

    ecs_enable(ecs, o, true);

    ecs_new_w(ecs, Position);
    test_int(ctx.invoked, 1);
    ecs_os_zeromem(&ctx);

    ecs_fini(ecs);
}

void Observer_disable_observer_module(void) {
    ecs_world_t *ecs = ecs_mini();

    ECS_COMPONENT(ecs, Position);

    ecs_entity_t module = ecs_new_w_id(ecs, EcsModule);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer(ecs, {
        .entity = ecs_entity(ecs, { .parent = module }),
        .query.terms = {
            { .id = ecs_id(Position) }
        },
        .events = { EcsOnAdd },
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_new_w(ecs, Position);
    test_int(ctx.invoked, 1);
    ecs_os_zeromem(&ctx);

    ecs_enable(ecs, module, false);
    test_assert(!ecs_has_id(ecs, o, EcsDisabled));

    ecs_new_w(ecs, Position);
    test_int(ctx.invoked, 0);
    ecs_os_zeromem(&ctx);

    ecs_enable(ecs, module, true);
    test_assert(!ecs_has_id(ecs, o, EcsDisabled));

    ecs_new_w(ecs, Position);
    test_int(ctx.invoked, 1);
    ecs_os_zeromem(&ctx);

    ecs_fini(ecs);
}

void Observer_disable_observer_module_nested(void) {
    ecs_world_t *ecs = ecs_mini();

    ECS_COMPONENT(ecs, Position);

    ecs_entity_t module = ecs_new_w_id(ecs, EcsModule);
    ecs_entity_t module_child = ecs_new_w_id(ecs, EcsModule);
    ecs_add_pair(ecs, module_child, EcsChildOf, module);

    Probe ctx = {0};
    ecs_observer(ecs, {
        .entity = ecs_entity(ecs, { .parent = module_child }),
        .query.terms = {
            { .id = ecs_id(Position) }
        },
        .events = { EcsOnAdd },
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_new_w(ecs, Position);
    test_int(ctx.invoked, 1);
    ecs_os_zeromem(&ctx);

    ecs_enable(ecs, module, false);
    test_assert(!ecs_has_id(ecs, module_child, EcsDisabled));

    ecs_new_w(ecs, Position);
    test_int(ctx.invoked, 0);
    ecs_os_zeromem(&ctx);

    ecs_enable(ecs, module_child, false);
    test_assert(ecs_has_id(ecs, module_child, EcsDisabled));

    ecs_new_w(ecs, Position);
    test_int(ctx.invoked, 0);
    ecs_os_zeromem(&ctx);

    ecs_enable(ecs, module, true);
    test_assert(ecs_has_id(ecs, module_child, EcsDisabled));

    ecs_new_w(ecs, Position);
    test_int(ctx.invoked, 0);
    ecs_os_zeromem(&ctx);

    ecs_enable(ecs, module_child, true);
    test_assert(!ecs_has_id(ecs, module_child, EcsDisabled));

    ecs_new_w(ecs, Position);
    test_int(ctx.invoked, 1);
    ecs_os_zeromem(&ctx);

    ecs_fini(ecs);
}

void Observer_disable_observer_and_module(void) {
    ecs_world_t *ecs = ecs_mini();

    ECS_COMPONENT(ecs, Position);

    ecs_entity_t module = ecs_new_w_id(ecs, EcsModule);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer(ecs, {
        .entity = ecs_entity(ecs, { .parent = module }),
        .query.terms = {
            { .id = ecs_id(Position) }
        },
        .events = { EcsOnAdd },
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_new_w(ecs, Position);
    test_int(ctx.invoked, 1);
    ecs_os_zeromem(&ctx);

    ecs_enable(ecs, module, false);
    test_assert(!ecs_has_id(ecs, o, EcsDisabled));

    ecs_new_w(ecs, Position);
    test_int(ctx.invoked, 0);
    ecs_os_zeromem(&ctx);

    ecs_enable(ecs, o, false);
    test_assert(ecs_has_id(ecs, o, EcsDisabled));

    ecs_new_w(ecs, Position);
    test_int(ctx.invoked, 0);
    ecs_os_zeromem(&ctx);

    ecs_enable(ecs, module, true);
    test_assert(ecs_has_id(ecs, o, EcsDisabled));

    ecs_new_w(ecs, Position);
    test_int(ctx.invoked, 0);
    ecs_os_zeromem(&ctx);

    ecs_enable(ecs, o, true);
    test_assert(!ecs_has_id(ecs, o, EcsDisabled));

    ecs_new_w(ecs, Position);
    test_int(ctx.invoked, 1);
    ecs_os_zeromem(&ctx);

    ecs_fini(ecs);
}

void Observer_disable_multi_observer(void) {
    ecs_world_t *ecs = ecs_mini();

    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Velocity);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer(ecs, {
        .query.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Velocity) },
        },
        .events = { EcsOnAdd },
        .callback = Observer,
        .ctx = &ctx
    });

    {
        ecs_entity_t e = ecs_new_w(ecs, Position);
        ecs_add(ecs, e, Velocity);
    }
    test_int(ctx.invoked, 1);
    ecs_os_zeromem(&ctx);

    ecs_enable(ecs, o, false);

    {
        ecs_entity_t e = ecs_new_w(ecs, Position);
        ecs_add(ecs, e, Velocity);
    }
    test_int(ctx.invoked, 0);
    ecs_os_zeromem(&ctx);

    ecs_enable(ecs, o, true);

    {
        ecs_entity_t e = ecs_new_w(ecs, Position);
        ecs_add(ecs, e, Velocity);
    }
    test_int(ctx.invoked, 1);
    ecs_os_zeromem(&ctx);

    ecs_fini(ecs);
}

void Observer_disable_multi_observer_module(void) {
    ecs_world_t *ecs = ecs_mini();

    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Velocity);

    ecs_entity_t module = ecs_new_w_id(ecs, EcsModule);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer(ecs, {
        .entity = ecs_entity(ecs, { .parent = module }),
        .query.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Velocity) },
        },
        .events = { EcsOnAdd },
        .callback = Observer,
        .ctx = &ctx
    });

    {
        ecs_entity_t e = ecs_new_w(ecs, Position);
        ecs_add(ecs, e, Velocity);
    }
    test_int(ctx.invoked, 1);
    ecs_os_zeromem(&ctx);

    ecs_enable(ecs, module, false);
    test_assert(!ecs_has_id(ecs, o, EcsDisabled));

    {
        ecs_entity_t e = ecs_new_w(ecs, Position);
        ecs_add(ecs, e, Velocity);
    }
    test_int(ctx.invoked, 0);
    ecs_os_zeromem(&ctx);

    ecs_enable(ecs, module, true);
    test_assert(!ecs_has_id(ecs, o, EcsDisabled));

    {
        ecs_entity_t e = ecs_new_w(ecs, Position);
        ecs_add(ecs, e, Velocity);
    }
    test_int(ctx.invoked, 1);
    ecs_os_zeromem(&ctx);

    ecs_fini(ecs);
}

void Observer_disable_multi_observer_module_nested(void) {
    ecs_world_t *ecs = ecs_mini();

    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Velocity);

    ecs_entity_t module = ecs_new_w_id(ecs, EcsModule);
    ecs_entity_t module_child = ecs_new_w_id(ecs, EcsModule);
    ecs_add_pair(ecs, module_child, EcsChildOf, module);

    Probe ctx = {0};
    ecs_observer(ecs, {
        .entity = ecs_entity(ecs, { .parent = module_child }),
        .query.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Velocity) }
        },
        .events = { EcsOnAdd },
        .callback = Observer,
        .ctx = &ctx
    });

    {
        ecs_entity_t e = ecs_new_w(ecs, Position);
        ecs_add(ecs, e, Velocity);
    }
    test_int(ctx.invoked, 1);
    ecs_os_zeromem(&ctx);

    ecs_enable(ecs, module, false);
    test_assert(!ecs_has_id(ecs, module_child, EcsDisabled));

    {
        ecs_entity_t e = ecs_new_w(ecs, Position);
        ecs_add(ecs, e, Velocity);
    }
    test_int(ctx.invoked, 0);
    ecs_os_zeromem(&ctx);

    ecs_enable(ecs, module_child, false);
    test_assert(ecs_has_id(ecs, module_child, EcsDisabled));

    {
        ecs_entity_t e = ecs_new_w(ecs, Position);
        ecs_add(ecs, e, Velocity);
    }
    test_int(ctx.invoked, 0);
    ecs_os_zeromem(&ctx);

    ecs_enable(ecs, module, true);
    test_assert(ecs_has_id(ecs, module_child, EcsDisabled));

    {
        ecs_entity_t e = ecs_new_w(ecs, Position);
        ecs_add(ecs, e, Velocity);
    }
    test_int(ctx.invoked, 0);
    ecs_os_zeromem(&ctx);

    ecs_enable(ecs, module_child, true);
    test_assert(!ecs_has_id(ecs, module_child, EcsDisabled));

    {
        ecs_entity_t e = ecs_new_w(ecs, Position);
        ecs_add(ecs, e, Velocity);
    }
    test_int(ctx.invoked, 1);
    ecs_os_zeromem(&ctx);

    ecs_fini(ecs);
}

void Observer_disable_multi_observer_and_module(void) {
    ecs_world_t *ecs = ecs_mini();

    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Velocity);

    ecs_entity_t module = ecs_new_w_id(ecs, EcsModule);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer(ecs, {
        .entity = ecs_entity(ecs, { .parent = module }),
        .query.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Velocity) }
        },
        .events = { EcsOnAdd },
        .callback = Observer,
        .ctx = &ctx
    });

    {
        ecs_entity_t e = ecs_new_w(ecs, Position);
        ecs_add(ecs, e, Velocity);
    }
    test_int(ctx.invoked, 1);
    ecs_os_zeromem(&ctx);

    ecs_enable(ecs, module, false);
    test_assert(!ecs_has_id(ecs, o, EcsDisabled));

    {
        ecs_entity_t e = ecs_new_w(ecs, Position);
        ecs_add(ecs, e, Velocity);
    }
    test_int(ctx.invoked, 0);
    ecs_os_zeromem(&ctx);

    ecs_enable(ecs, o, false);
    test_assert(ecs_has_id(ecs, o, EcsDisabled));

    {
        ecs_entity_t e = ecs_new_w(ecs, Position);
        ecs_add(ecs, e, Velocity);
    }
    test_int(ctx.invoked, 0);
    ecs_os_zeromem(&ctx);

    ecs_enable(ecs, module, true);
    test_assert(ecs_has_id(ecs, o, EcsDisabled));

    {
        ecs_entity_t e = ecs_new_w(ecs, Position);
        ecs_add(ecs, e, Velocity);
    }
    test_int(ctx.invoked, 0);
    ecs_os_zeromem(&ctx);

    ecs_enable(ecs, o, true);
    test_assert(!ecs_has_id(ecs, o, EcsDisabled));

    {
        ecs_entity_t e = ecs_new_w(ecs, Position);
        ecs_add(ecs, e, Velocity);
    }
    test_int(ctx.invoked, 1);
    ecs_os_zeromem(&ctx);

    ecs_fini(ecs);
}

void Observer_tag_w_on_set_and_on_add(void) {
    ecs_world_t *world = ecs_mini();
    
    ecs_entity_t tag = ecs_new(world);
    ecs_entity_t e1 = ecs_new(world);

    Probe ctx = {0};

    ecs_entity_t o = ecs_observer(world, {
        .query.terms = {{ tag }},
        .events = {EcsOnAdd, EcsOnSet},
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_add_id(world, e1, tag);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e1);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][0], tag);
    test_int(ctx.event, EcsOnAdd);

    ecs_delete(world, o);
    ecs_os_zeromem(&ctx);

    ecs_entity_t e2 = ecs_new(world);
    ecs_add_id(world, e2, tag);
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Observer_tag_w_on_set_and_on_add_reverse(void) {
    ecs_world_t *world = ecs_mini();
    
    ecs_entity_t tag = ecs_new(world);
    ecs_entity_t e1 = ecs_new(world);

    Probe ctx = {0};

    ecs_entity_t o = ecs_observer(world, {
        .query.terms = {{ tag }},
        .events = {EcsOnSet, EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_add_id(world, e1, tag);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e1);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][0], tag);
    test_int(ctx.event, EcsOnAdd);

    ecs_delete(world, o);
    ecs_os_zeromem(&ctx);

    ecs_entity_t e2 = ecs_new(world);
    ecs_add_id(world, e2, tag);
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Observer_on_add_pair_w_rel_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t e1 = ecs_new(world);

    Probe ctx = {0};

    ecs_observer(world, {
        .query.terms = {{ ecs_pair(EcsAny, Tgt) }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });

    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e1, Rel, Tgt);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e1);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][0], ecs_pair(Rel, Tgt));
    test_int(ctx.event, EcsOnAdd);

    ecs_fini(world);
}

void Observer_on_remove_pair_w_rel_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t e1 = ecs_new(world);

    Probe ctx = {0};

    ecs_observer(world, {
        .query.terms = {{ ecs_pair(EcsAny, Tgt) }},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });

    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e1, Rel, Tgt);

    test_int(ctx.invoked, 0);

    ecs_remove_pair(world, e1, Rel, Tgt);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e1);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][0], ecs_pair(Rel, Tgt));
    test_int(ctx.event, EcsOnRemove);

    ecs_fini(world);
}

void Observer_on_add_pair_w_tgt_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t e1 = ecs_new(world);

    Probe ctx = {0};

    ecs_observer(world, {
        .query.terms = {{ ecs_pair(Rel, EcsAny) }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });

    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e1, Rel, Tgt);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e1);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][0], ecs_pair(Rel, Tgt));
    test_int(ctx.event, EcsOnAdd);

    ecs_fini(world);
}

void Observer_on_remove_pair_w_tgt_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t e1 = ecs_new(world);

    Probe ctx = {0};

    ecs_observer(world, {
        .query.terms = {{ ecs_pair(Rel, EcsAny) }},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });

    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e1, Rel, Tgt);

    test_int(ctx.invoked, 0);

    ecs_remove_pair(world, e1, Rel, Tgt);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e1);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][0], ecs_pair(Rel, Tgt));
    test_int(ctx.event, EcsOnRemove);

    ecs_fini(world);
}

void Observer_on_add_pair_w_any_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t e1 = ecs_new(world);

    Probe ctx = {0};

    ecs_observer(world, {
        .query.terms = {{ ecs_pair(EcsAny, EcsAny) }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });

    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e1, Rel, Tgt);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e1);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][0], ecs_pair(Rel, Tgt));
    test_int(ctx.event, EcsOnAdd);

    ecs_fini(world);
}

void Observer_on_remove_pair_w_any_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t e1 = ecs_new(world);

    Probe ctx = {0};

    ecs_observer(world, {
        .query.terms = {{ ecs_pair(EcsAny, EcsAny) }},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });

    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e1, Rel, Tgt);

    test_int(ctx.invoked, 0);

    ecs_remove_pair(world, e1, Rel, Tgt);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e1);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][0], ecs_pair(Rel, Tgt));
    test_int(ctx.event, EcsOnRemove);

    ecs_fini(world);
}

void Observer_on_add_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new(world);

    Probe ctx = {0};

    ecs_observer(world, {
        .query.terms = {{ EcsAny }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });

    test_int(ctx.invoked, 0);

    ecs_add(world, e1, TagA);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e1);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.event, EcsOnAdd);

    ecs_fini(world);
}

void Observer_on_remove_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_w(world, TagA);

    Probe ctx = {0};

    ecs_observer(world, {
        .query.terms = {{ EcsAny }},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });

    test_int(ctx.invoked, 0);

    ecs_remove(world, e1, TagA);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e1);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.event, EcsOnRemove);

    ecs_fini(world);
}

void Observer_yield_existing_uni_no_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e1 = ecs_new_w(world, TagA);

    Probe ctx = {0};

    ecs_observer(world, {
        .query.terms = {
            { TagA, .src.id = e1 },
        },
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx,
        .yield_existing = true
    });

    test_int(ctx.invoked, 1);
    test_int(ctx.e[0], 0);
    test_int(ctx.s[0][0], e1);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.event, EcsOnAdd);

    ecs_fini(world);
}

void Observer_yield_existing_multi_no_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_add(world, e1, TagB);

    Probe ctx = {0};

    ecs_observer(world, {
        .query.terms = {
            { TagA, .src.id = e1 },
            { TagB, .src.id = e1 },
        },
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx,
        .yield_existing = true
    });

    test_int(ctx.invoked, 1);
    test_int(ctx.e[0], 0);
    test_int(ctx.s[0][0], e1);
    test_int(ctx.c[0][0], TagA);
    test_int(ctx.event, EcsOnAdd);

    ecs_fini(world);
}

void Observer_observer_no_id_in_scope(void) {
    ecs_world_t* world = ecs_mini();
    
    ECS_TAG(world, TagA);

    ecs_entity_t parent = ecs_new(world);

    ecs_set_scope(world, parent);

    ecs_entity_t o = ecs_observer(world, {
        .query.terms = {
            { .id = TagA },
        },
        .callback = Observer,
        .events = { EcsOnAdd },
    });

    ecs_set_scope(world, 0);

    test_assert(ecs_has_pair(world, o, EcsChildOf, parent));

    ecs_fini(world);
}

static
void ObserverRegisterComp(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);

    ecs_component(it->world, {
        .entity = ecs_entity(it->world, { .name = "Position", .symbol = "Position" }),
        .type.size = 8,
        .type.alignment = 4,
    });
}

void Observer_register_comp_in_emit_named_entity(void) {
    ecs_world_t* world = ecs_mini();
    
    ECS_TAG(world, TagA);
    ECS_TAG(world, Event);

    Probe ctx;

    ecs_observer(world, {
        .query.terms = {
            { .id = EcsAny },
        },
        .callback = ObserverRegisterComp,
        .events = { Event },
        .ctx = &ctx,
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e1" });

    ecs_emit(world, &(ecs_event_desc_t){
        .event = Event,
        .entity = e
    });

    test_int(ctx.invoked, 1);

    ecs_entity_t pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);
    const EcsComponent *ptr = ecs_get(world, pos, EcsComponent);
    test_assert(ptr != NULL);
    test_int(ptr->size, 8);
    test_int(ptr->alignment, 4);

    ecs_fini(world);
}

static
void ObserverRegisterCompMacro(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);

    ECS_COMPONENT(it->world, Position);
}

void Observer_register_comp_w_macro_in_emit_named_entity(void) {
    ecs_world_t* world = ecs_mini();
    
    ECS_TAG(world, TagA);
    ECS_TAG(world, Event);

    Probe ctx;

    ecs_observer(world, {
        .query.terms = {
            { .id = EcsAny },
        },
        .callback = ObserverRegisterCompMacro,
        .events = { Event },
        .ctx = &ctx,
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e1" });

    ecs_emit(world, &(ecs_event_desc_t){
        .event = Event,
        .entity = e
    });

    test_int(ctx.invoked, 1);

    ecs_entity_t pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);
    const EcsComponent *ptr = ecs_get(world, pos, EcsComponent);
    test_assert(ptr != NULL);
    test_int(ptr->size, 8);
    test_int(ptr->alignment, 4);

    ecs_fini(world);
}

static ECS_TAG_DECLARE(Foo);

static
void ObserverAddToSelf(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);

    test_int(it->count, 1);
    ecs_add(it->world, it->entities[0], Foo);
}

void Observer_add_to_self_in_emit_entity(void) {
    ecs_world_t* world = ecs_mini();

    ECS_TAG(world, Event);
    ECS_TAG_DEFINE(world, Foo);

    Probe ctx;

    ecs_observer(world, {
        .query.terms = {
            { .id = EcsAny },
        },
        .callback = ObserverAddToSelf,
        .events = { Event },
        .ctx = &ctx,
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e1" });

    ecs_emit(world, &(ecs_event_desc_t){
        .event = Event,
        .entity = e
    });

    test_int(ctx.invoked, 1);
    test_assert(ecs_has(world, e, Foo));

    ecs_fini(world);
}

void Observer_on_set_w_not_tag(void) {
    ecs_world_t* world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    Probe ctx;

    ecs_observer(world, {
        .query.terms = {
            { .id = ecs_id(Position) },
            { .id = Foo, .oper = EcsNot },
        },
        .callback = Observer_w_1_value,
        .events = { EcsOnSet },
        .ctx = &ctx,
    });

    ecs_entity_t e = ecs_new(world);

    ecs_set(world, e, Position, {10, 20});
    test_int(ctx.invoked, 1);

    ecs_os_zeromem(&ctx);

    ecs_add(world, e, Foo);
    test_int(ctx.invoked, 0);

    ecs_remove(world, e, Foo);
    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void Observer_on_set_w_not_component(void) {
    ecs_world_t* world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    Probe ctx;

    ecs_observer(world, {
        .query.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Velocity), .oper = EcsNot },
        },
        .callback = Observer_w_1_value,
        .events = { EcsOnSet },
        .ctx = &ctx,
    });

    ecs_entity_t e = ecs_new(world);

    ecs_set(world, e, Position, {10, 20});
    test_int(ctx.invoked, 1);

    ecs_os_zeromem(&ctx);

    ecs_set(world, e, Velocity, {1, 2});
    test_int(ctx.invoked, 0);

    ecs_remove(world, e, Velocity);
    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void Observer_wildcard_event(void) {
    ecs_world_t* world = ecs_init();

    ECS_COMPONENT(world, Position);

    Probe ctx = {0};

    ecs_observer(world, {
        .query.terms = {
            { .id = ecs_id(Position) },
        },
        .callback = Observer,
        .events = { EcsWildcard },
        .ctx = &ctx,
    });

    test_int(ctx.invoked, 0);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);

    test_int(ctx.invoked, 2);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e);
    test_uint(ctx.event, EcsOnAdd);

    ecs_os_zeromem(&ctx);
    test_int(ctx.invoked, 0);

    ecs_set(world, e, Position, {10, 20});

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e);
    test_uint(ctx.event, EcsOnSet);

    ecs_os_zeromem(&ctx);
    test_int(ctx.invoked, 0);

    ecs_remove(world, e, Position);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e);
    test_uint(ctx.event, EcsOnRemove);

    ecs_fini(world);
}

static int callback_callback_invoked = 0;
static int callback_run_invoked = 0;

static
void callback_callback(ecs_iter_t *it) {
    callback_callback_invoked ++;
}

static
void callback_run(ecs_iter_t *it) {
    callback_run_invoked ++;
}

void Observer_register_callback_after_run(void) {
    ecs_world_t* world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t s = ecs_observer(world, {
        .query.terms = {{ .id = ecs_id(Position) }},
        .events = {EcsOnSet},
        .callback = callback_callback
    });
    
    ecs_entity_t e = ecs_new(world);

    ecs_set(world, e, Position, {10, 20});
    test_int(callback_callback_invoked, 1);
    test_int(callback_run_invoked, 0);

    ecs_observer(world, {
        .entity = s,
        .run = callback_run
    });

    ecs_set(world, e, Position, {10, 20});
    test_int(callback_callback_invoked, 1);
    test_int(callback_run_invoked, 1);

    ecs_fini(world);
}

void Observer_register_run_after_callback(void) {
    ecs_world_t* world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t s = ecs_observer(world, {
        .query.terms = {{ .id = ecs_id(Position) }},
        .events = {EcsOnSet},
        .run = callback_run
    });
    
    ecs_entity_t e = ecs_new(world);

    ecs_set(world, e, Position, {10, 20});
    test_int(callback_run_invoked, 1);
    test_int(callback_callback_invoked, 0);

    ecs_observer(world, {
        .entity = s,
        .callback = callback_callback
    });

    ecs_set(world, e, Position, {10, 20});
    test_int(callback_run_invoked, 1);
    test_int(callback_callback_invoked, 1);

    ecs_fini(world);
}

static
void ctx_free_3(void *ptr) {
    int32_t *ctx = ptr;
    ctx[0]++;
}

void Observer_register_callback_after_run_ctx(void) {
    ecs_world_t* world = ecs_mini();

    ECS_COMPONENT(world, Position);

    int32_t callback_ctx = 0, run_ctx = 0;

    ecs_entity_t s = ecs_observer(world, {
        .query.terms = {{ .id = ecs_id(Position) }},
        .events = {EcsOnSet},
        .callback = callback_callback,
        .callback_ctx = &callback_ctx,
        .callback_ctx_free = ctx_free_3
    });

    test_int(callback_ctx, 0);

    ecs_observer(world, {
        .entity = s,
        .query.terms = {{ .id = ecs_id(Position) }},
        .events = {EcsOnSet},
        .run = callback_run,
        .run_ctx = &run_ctx,
        .run_ctx_free = ctx_free_3
    });

    test_int(callback_ctx, 1);

    ecs_fini(world);

    test_int(callback_ctx, 1);
    test_int(run_ctx, 1);
}

void Observer_register_run_after_callback_ctx(void) {
    ecs_world_t* world = ecs_mini();

    ECS_COMPONENT(world, Position);

    int32_t callback_ctx = 0, run_ctx = 0;

    ecs_entity_t s = ecs_observer(world, {
        .query.terms = {{ .id = ecs_id(Position) }},
        .events = {EcsOnSet},
        .run = callback_run,
        .run_ctx = &run_ctx,
        .run_ctx_free = ctx_free_3
    });

    test_int(run_ctx, 0);

    ecs_observer(world, {
        .entity = s,
        .query.terms = {{ .id = ecs_id(Position) }},
        .events = {EcsOnSet},
        .callback = callback_callback,
        .callback_ctx = &callback_ctx,
        .callback_ctx_free = ctx_free_3
    });

    test_int(run_ctx, 1);
    test_int(callback_ctx, 0);

    ecs_fini(world);

    test_int(callback_ctx, 1);
    test_int(run_ctx, 1);
}

void Observer_cache_test_1(void) {
    ecs_world_t *world = ecs_mini();
    
    ecs_entity_t tag = ecs_new(world);
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);

    Probe ctx = {0};

    ecs_observer(world, {
        .query.terms = {{ tag, .src.id = EcsUp, .trav = EcsChildOf }},
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });
    
    ecs_add_id(world, e2, ecs_pair(EcsChildOf, e1));
    ecs_add_id(world, e1, tag);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e2);
    test_int(ctx.s[0][0], e1);
    test_int(ctx.c[0][0], tag);
    test_int(ctx.event, EcsOnAdd);
    ecs_os_zeromem(&ctx);

    ecs_entity_t e3 = ecs_new(world);
    ecs_add_id(world, e3, ecs_pair(EcsChildOf, e2));

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e3);
    test_int(ctx.s[0][0], e1);
    test_int(ctx.c[0][0], tag);
    test_int(ctx.event, EcsOnAdd);
    ecs_os_zeromem(&ctx);

    ecs_delete(world, e3);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e3);
    test_int(ctx.s[0][0], e1);
    test_int(ctx.c[0][0], tag);
    test_int(ctx.event, EcsOnRemove);
    ecs_os_zeromem(&ctx);
    
    ecs_remove_id(world, e1, tag);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e2);
    test_int(ctx.s[0][0], e1);
    test_int(ctx.c[0][0], tag);
    test_int(ctx.event, EcsOnRemove);
    ecs_os_zeromem(&ctx);

    e3 = ecs_new(world);
    ecs_add_id(world, e3, ecs_pair(EcsChildOf, e2));

    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Observer_cache_test_2(void) {
    ecs_world_t *world = ecs_mini();
    
    ecs_entity_t tag_1 = ecs_new(world);
    ecs_entity_t tag_2 = ecs_new(world);
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    Probe ctx_1 = {0};
    Probe ctx_2 = {0};

    ecs_observer(world, {
        .query.terms = {{ tag_1, .src.id = EcsUp, .trav = EcsChildOf }},
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx_1
    });
    ecs_observer(world, {
        .query.terms = {{ tag_2, .src.id = EcsUp, .trav = EcsChildOf }},
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx_2
    });

    ecs_add_id(world, e1, tag_1);
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 0);

    ecs_add_id(world, e2, ecs_pair(EcsChildOf, e1));
    test_int(ctx_1.invoked, 1);
    test_int(ctx_1.count, 1);
    test_int(ctx_1.e[0], e2);
    test_int(ctx_1.s[0][0], e1);
    test_int(ctx_1.c[0][0], tag_1);
    test_int(ctx_1.event, EcsOnAdd);
    ecs_os_zeromem(&ctx_1);
    test_int(ctx_2.invoked, 0);

    ecs_add_id(world, e2, tag_2);
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 0);

    ecs_add_id(world, e3, ecs_pair(EcsChildOf, e2));
    test_int(ctx_1.invoked, 1);
    test_int(ctx_1.count, 1);
    test_int(ctx_1.e[0], e3);
    test_int(ctx_1.s[0][0], e1);
    test_int(ctx_1.c[0][0], tag_1);
    test_int(ctx_1.event, EcsOnAdd);
    ecs_os_zeromem(&ctx_1);
    test_int(ctx_2.invoked, 1);
    test_int(ctx_2.count, 1);
    test_int(ctx_2.e[0], e3);
    test_int(ctx_2.s[0][0], e2);
    test_int(ctx_2.c[0][0], tag_2);
    test_int(ctx_2.event, EcsOnAdd);
    ecs_os_zeromem(&ctx_2);

    ecs_fini(world);
}

void Observer_cache_test_3(void) {
    ecs_world_t *world = ecs_mini();
    
    ecs_entity_t tag_1 = ecs_new(world);
    ecs_entity_t tag_2 = ecs_new(world);
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    Probe ctx_1 = {0};
    Probe ctx_2 = {0};

    ecs_observer(world, {
        .query.terms = {{ tag_1, .src.id = EcsUp, .trav = EcsChildOf }},
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx_1
    });
    ecs_observer(world, {
        .query.terms = {{ tag_2, .src.id = EcsUp, .trav = EcsChildOf }},
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx_2
    });

    ecs_add_id(world, e1, tag_1);
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 0);

    ecs_add_id(world, e2, ecs_pair(EcsChildOf, e1));
    test_int(ctx_1.invoked, 1);
    test_int(ctx_1.count, 1);
    test_int(ctx_1.e[0], e2);
    test_int(ctx_1.s[0][0], e1);
    test_int(ctx_1.c[0][0], tag_1);
    test_int(ctx_1.event, EcsOnAdd);
    ecs_os_zeromem(&ctx_1);
    test_int(ctx_2.invoked, 0);

    ecs_add_id(world, e2, tag_1);
    ecs_add_id(world, e2, tag_2);
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 0);

    ecs_add_id(world, e3, ecs_pair(EcsChildOf, e2));
    test_int(ctx_1.invoked, 1);
    test_int(ctx_1.count, 1);
    test_int(ctx_1.e[0], e3);
    test_int(ctx_1.s[0][0], e2);
    test_int(ctx_1.c[0][0], tag_1);
    test_int(ctx_1.event, EcsOnAdd);
    ecs_os_zeromem(&ctx_1);
    test_int(ctx_2.invoked, 1);
    test_int(ctx_2.count, 1);
    test_int(ctx_2.e[0], e3);
    test_int(ctx_2.s[0][0], e2);
    test_int(ctx_2.c[0][0], tag_2);
    test_int(ctx_2.event, EcsOnAdd);
    ecs_os_zeromem(&ctx_2);

    ecs_fini(world);
}

void Observer_cache_test_4(void) {
    ecs_world_t *world = ecs_mini();
    
    ecs_entity_t tag_0 = ecs_new(world);
    ecs_entity_t tag_1 = ecs_new(world);
    ecs_entity_t tag_2 = ecs_new(world);
    ecs_entity_t e0 = ecs_new(world);
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    Probe *ctx_0 = ecs_os_calloc_t(Probe);
    Probe *ctx_1 = ecs_os_calloc_t(Probe);
    Probe *ctx_2 = ecs_os_calloc_t(Probe);

    ecs_observer(world, {
        .query.terms = {{ tag_0, .src.id = EcsUp, .trav = EcsChildOf }},
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = Observer,
        .ctx = ctx_0
    });
    ecs_observer(world, {
        .query.terms = {{ tag_1, .src.id = EcsUp, .trav = EcsChildOf }},
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = Observer,
        .ctx = ctx_1
    });
    ecs_observer(world, {
        .query.terms = {{ tag_2, .src.id = EcsUp, .trav = EcsChildOf }},
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = Observer,
        .ctx = ctx_2
    });

    ecs_add_id(world, e0, tag_0);
    test_int(ctx_0->invoked, 0);
    test_int(ctx_1->invoked, 0);
    test_int(ctx_2->invoked, 0);

    ecs_add_id(world, e1, ecs_pair(EcsChildOf, e0));
    test_int(ctx_0->invoked, 1);
    test_int(ctx_0->count, 1);
    test_int(ctx_0->e[0], e1);
    test_int(ctx_0->s[0][0], e0);
    test_int(ctx_0->c[0][0], tag_0);
    test_int(ctx_0->event, EcsOnAdd);
    ecs_os_zeromem(ctx_0);
    test_int(ctx_1->invoked, 0);
    test_int(ctx_2->invoked, 0);

    ecs_add_id(world, e1, tag_1);
    test_int(ctx_0->invoked, 0);
    test_int(ctx_1->invoked, 0);
    test_int(ctx_2->invoked, 0);

    ecs_add_id(world, e2, ecs_pair(EcsChildOf, e1));
    test_int(ctx_0->invoked, 1);
    test_int(ctx_0->count, 1);
    test_int(ctx_0->e[0], e2);
    test_int(ctx_0->s[0][0], e0);
    test_int(ctx_0->c[0][0], tag_0);
    test_int(ctx_0->event, EcsOnAdd);
    ecs_os_zeromem(ctx_0);
    test_int(ctx_1->invoked, 1);
    test_int(ctx_1->count, 1);
    test_int(ctx_1->e[0], e2);
    test_int(ctx_1->s[0][0], e1);
    test_int(ctx_1->c[0][0], tag_1);
    test_int(ctx_1->event, EcsOnAdd);
    ecs_os_zeromem(ctx_1);
    test_int(ctx_2->invoked, 0);

    ecs_add_id(world, e2, tag_1);
    ecs_add_id(world, e2, tag_2);
    test_int(ctx_0->invoked, 0);
    test_int(ctx_1->invoked, 0);
    test_int(ctx_2->invoked, 0);

    ecs_remove_id(world, e0, tag_0);
    test_int(ctx_0->invoked, 2);
    ecs_os_zeromem(ctx_0);
    test_int(ctx_1->invoked, 0);
    test_int(ctx_2->invoked, 0);

    ecs_add_id(world, e3, ecs_pair(EcsChildOf, e2));
    test_int(ctx_0->invoked, 0);
    test_int(ctx_1->invoked, 1);
    test_int(ctx_1->count, 1);
    test_int(ctx_1->e[0], e3);
    test_int(ctx_1->s[0][0], e2);
    test_int(ctx_1->c[0][0], tag_1);
    test_int(ctx_1->event, EcsOnAdd);
    ecs_os_zeromem(ctx_1);
    test_int(ctx_2->invoked, 1);
    test_int(ctx_2->count, 1);
    test_int(ctx_2->e[0], e3);
    test_int(ctx_2->s[0][0], e2);
    test_int(ctx_2->c[0][0], tag_2);
    test_int(ctx_2->event, EcsOnAdd);
    ecs_os_zeromem(ctx_2);

    ecs_fini(world);

    ecs_os_free(ctx_0);
    ecs_os_free(ctx_1);
    ecs_os_free(ctx_2);
}

void Observer_cache_test_5(void) {
    ecs_world_t *world = ecs_mini();
    
    ecs_entity_t tag_0 = ecs_new(world);
    ecs_entity_t tag_1 = ecs_new(world);
    ecs_entity_t tag_2 = ecs_new(world);
    ecs_entity_t e0 = ecs_new(world);
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_entity_t e4 = ecs_new(world);

    Probe ctx_1 = {0};
    Probe ctx_2 = {0};

    ecs_observer(world, {
        .query.terms = {{ tag_1, .src.id = EcsUp, .trav = EcsChildOf }},
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx_1
    });
    ecs_observer(world, {
        .query.terms = {{ tag_2, .src.id = EcsUp, .trav = EcsChildOf }},
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx_2
    });

    ecs_add_id(world, e0, tag_0);
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 0);

    ecs_add_id(world, e1, ecs_pair(EcsChildOf, e0));
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 0);

    ecs_add_id(world, e1, tag_1);
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 0);

    ecs_add_id(world, e2, ecs_pair(EcsChildOf, e1));
    test_int(ctx_1.invoked, 1);
    test_int(ctx_1.count, 1);
    test_int(ctx_1.e[0], e2);
    test_int(ctx_1.s[0][0], e1);
    test_int(ctx_1.c[0][0], tag_1);
    test_int(ctx_1.event, EcsOnAdd);
    ecs_os_zeromem(&ctx_1);
    test_int(ctx_2.invoked, 0);

    ecs_add_id(world, e2, tag_1);
    ecs_add_id(world, e2, tag_2);
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 0);

    ecs_remove_id(world, e0, tag_0);
    test_int(ctx_1.invoked, 0);
    test_int(ctx_2.invoked, 0);

    ecs_add_id(world, e3, ecs_pair(EcsChildOf, e2));
    test_int(ctx_1.invoked, 1);
    test_int(ctx_1.count, 1);
    test_int(ctx_1.e[0], e3);
    test_int(ctx_1.s[0][0], e2);
    test_int(ctx_1.c[0][0], tag_1);
    test_int(ctx_1.event, EcsOnAdd);
    ecs_os_zeromem(&ctx_1);
    test_int(ctx_2.invoked, 1);
    test_int(ctx_2.count, 1);
    test_int(ctx_2.e[0], e3);
    test_int(ctx_2.s[0][0], e2);
    test_int(ctx_2.c[0][0], tag_2);
    test_int(ctx_2.event, EcsOnAdd);
    ecs_os_zeromem(&ctx_2);

    ecs_add_id(world, e4, ecs_pair(EcsChildOf, e1));
    test_int(ctx_1.invoked, 1);
    test_int(ctx_1.count, 1);
    test_int(ctx_1.e[0], e4);
    test_int(ctx_1.s[0][0], e1);
    test_int(ctx_1.c[0][0], tag_1);
    test_int(ctx_1.event, EcsOnAdd);
    ecs_os_zeromem(&ctx_1);
    test_int(ctx_2.invoked, 0);

    ecs_fini(world);
}

void Observer_cache_test_6(void) {
    ecs_world_t *world = ecs_mini();
    
    ecs_entity_t tag_1 = ecs_new(world);
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    Probe ctx_1 = {0};
    ecs_observer(world, {
        .query.terms = {{ tag_1, .src.id = EcsUp, .trav = EcsChildOf }},
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx_1
    });

    ecs_add_id(world, e1, tag_1);
    test_int(ctx_1.invoked, 0);
    
    ecs_add_id(world, e2, ecs_pair(EcsChildOf, e1));
    test_int(ctx_1.invoked, 1);
    test_int(ctx_1.count, 1);
    test_int(ctx_1.e[0], e2);
    test_int(ctx_1.s[0][0], e1);
    test_int(ctx_1.c[0][0], tag_1);
    test_int(ctx_1.event, EcsOnAdd);
    ecs_os_zeromem(&ctx_1);

    ecs_remove_id(world, e1, tag_1);
    test_int(ctx_1.invoked, 1);
    test_int(ctx_1.count, 1);
    test_int(ctx_1.e[0], e2);
    test_int(ctx_1.s[0][0], e1);
    test_int(ctx_1.c[0][0], tag_1);
    test_int(ctx_1.event, EcsOnRemove);
    ecs_os_zeromem(&ctx_1);

    ecs_add_id(world, e3, ecs_pair(EcsChildOf, e1));
    test_int(ctx_1.invoked, 0);

    ecs_fini(world);
}

void Observer_cache_test_7(void) {
    ecs_world_t *world = ecs_mini();
    
    ecs_entity_t tag_1 = ecs_new(world);
    ecs_entity_t tag_2 = ecs_new(world);
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    Probe ctx_1 = {0};
    ecs_observer(world, {
        .query.terms = {{ tag_1, .src.id = EcsUp, .trav = EcsChildOf }},
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx_1
    });

    ecs_add_id(world, e1, tag_1);
    ecs_add_id(world, e1, tag_2);
    test_int(ctx_1.invoked, 0);
    
    ecs_add_id(world, e2, ecs_pair(EcsChildOf, e1));
    test_int(ctx_1.invoked, 1);
    test_int(ctx_1.count, 1);
    test_int(ctx_1.e[0], e2);
    test_int(ctx_1.s[0][0], e1);
    test_int(ctx_1.c[0][0], tag_1);
    test_int(ctx_1.event, EcsOnAdd);
    ecs_os_zeromem(&ctx_1);

    ecs_remove_id(world, e1, tag_1);
    test_int(ctx_1.invoked, 1);
    test_int(ctx_1.count, 1);
    test_int(ctx_1.e[0], e2);
    test_int(ctx_1.s[0][0], e1);
    test_int(ctx_1.c[0][0], tag_1);
    test_int(ctx_1.event, EcsOnRemove);
    ecs_os_zeromem(&ctx_1);

    ecs_add_id(world, e3, ecs_pair(EcsChildOf, e1));
    test_int(ctx_1.invoked, 0);

    ecs_fini(world);
}

void Observer_cache_test_8(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms = {{ TagB }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_entity_t parent = ecs_new(world);
    ecs_add(world, parent, TagA);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_new_w_pair(world, EcsChildOf, child);
    test_int(ctx.invoked, 0);

    ecs_add(world, parent, TagB);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], parent);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][0], TagB);
    test_int(ctx.event, EcsOnAdd);

    ecs_fini(world);
}

void Observer_cache_test_9(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms = {{ ecs_id(Position), .src.id = EcsUp, .trav = EcsIsA }},
        .events = { EcsOnRemove },
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t base_2 = ecs_new_w_id(world, EcsPrefab);
    ecs_add_pair(world, base_2, EcsIsA, base);
    ecs_set(world, base_2, Position, {20, 30});

    ecs_entity_t base_3 = ecs_new_w_id(world, EcsPrefab);
    ecs_add_pair(world, base_3, EcsIsA, base);
    ecs_set(world, base_3, Position, {40, 50});

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base_2);
    ecs_add_pair(world, inst, EcsIsA, base_3);

    ecs_clear(world, base_2);

    /* Once for Position of base_2, twice because Position is reachable through
     * two paths. */
    test_int(ctx.invoked, 2);
    test_int(ctx.count, 2);
    test_int(ctx.e[0], inst);
    test_int(ctx.s[0][0], base_2);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.event, EcsOnRemove);

    ecs_os_zeromem(&ctx);

    ecs_clear(world, base_3);

    /* Once for Position of base_2, twice because Position is reachable through
     * two paths. */
    test_int(ctx.invoked, 2);
    test_int(ctx.count, 2);
    test_int(ctx.e[0], inst);
    test_int(ctx.s[0][0], base_3);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.event, EcsOnRemove);

    ecs_fini(world);
}

void Observer_cache_test_10(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t tag = ecs_new(world);
    ecs_entity_t ns = ecs_new(world);
    ecs_add_id(world, ns, tag);

    ecs_entity_t foo_parent = ecs_new_w_pair(world, EcsChildOf, ns);
    ecs_entity_t foo = ecs_new_w_pair(world, EcsChildOf, foo_parent);
    ecs_entity_t bar = ecs_new(world);

    ecs_entity_t parent = ecs_new_w_id(world, foo);
    
    ecs_entity_t e1 = ecs_new(world);
    ecs_add_id(world, e1, foo);
    ecs_add_pair(world, e1, EcsChildOf, parent);
    ecs_add_id(world, e1, bar);

    ecs_entity_t e2 = ecs_new(world);
    ecs_add_id(world, e2, foo);
    ecs_add_pair(world, e2, EcsChildOf, parent);
    ecs_add_id(world, e2, bar);

    ecs_entity_t c1 = ecs_new(world);
    ecs_add_id(world, c1, foo);
    ecs_add_pair(world, c1, EcsChildOf, e1);

    ecs_entity_t c2 = ecs_new(world);
    ecs_add_id(world, c2, foo);
    ecs_add_pair(world, c2, EcsChildOf, e2);

    ecs_delete(world, ns);
    test_assert(!ecs_is_alive(world, ns));
    test_assert(!ecs_is_alive(world, foo));
    test_assert(!ecs_is_alive(world, foo_parent));
    test_assert(!ecs_has_id(world, parent, foo));
    test_assert(!ecs_has_id(world, e1, foo));
    test_assert(!ecs_has_id(world, e2, foo));
    test_assert(!ecs_has_id(world, c1, foo));
    test_assert(!ecs_has_id(world, c2, foo));

    ecs_delete(world, parent);
    test_assert(!ecs_is_alive(world, parent));
    test_assert(!ecs_is_alive(world, e1));
    test_assert(!ecs_is_alive(world, e2));
    test_assert(!ecs_is_alive(world, c1));
    test_assert(!ecs_is_alive(world, c2));

    ecs_fini(world);
}

void Observer_cache_test_11(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t ns = ecs_new(world);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, ns);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsDependsOn, e1);
    ecs_add_pair(world, e2, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, ns);
    ecs_add_id(world, e3, EcsModule);

    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, e3);
    ecs_add_pair(world, e4, EcsDependsOn, e2);

    ecs_add_id(world, e1, ecs_new(world));

    ecs_run_aperiodic(world, 0);

    test_assert(true); // ensure cache revalidation didn't assert

    ecs_fini(world);
}

void Observer_cache_test_12(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t t0 = ecs_new(world); 
    ecs_entity_t t1 = ecs_new(world); 
    ecs_entity_t t2 = ecs_new(world); 

    ecs_entity_t e0 = ecs_new(world); 
    ecs_add_id(world, e0, t0); 
    ecs_add_id(world, e0, t1); 
    ecs_add_id(world, e0, t2); 

    ecs_entity_t e1 = ecs_new(world); 
    ecs_add_id(world, e1, t0); 
    ecs_add_id(world, e1, t1); 
    ecs_add_id(world, e1, t2); 

    ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e0); 
    ecs_add_id(world, e3, t0); 

    ecs_entity_t e4 = ecs_new(world); 
    ecs_new_w_pair(world, EcsChildOf, e4); 
    ecs_remove_all(world, t1); 

    ecs_new_w_pair(world, EcsChildOf, e3); 

    ecs_run_aperiodic(world, 0);

    ecs_remove_all(world, t0); 

    test_assert(true); // ensure cache revalidation didn't assert

    ecs_fini(world);
}

void Observer_cache_test_13(void) {
    install_test_abort();
    ecs_world_t *world = ecs_mini();

    ecs_entity_t t0 = ecs_new(world);
    ecs_entity_t e0 = ecs_new(world);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add_id(world, e1, t0);
    ecs_add_id(world, e1, ecs_pair(EcsChildOf, e0));

    ecs_entity_t e2 = ecs_new(world);
    ecs_add_id(world, e2, t0);

    ecs_entity_t e3 = ecs_new(world);
    ecs_add_id(world, e3, ecs_pair(EcsChildOf, e2));

    ecs_delete_with(world, t0);
    test_assert(!ecs_is_alive(world, e1));
    test_assert(!ecs_is_alive(world, e2));
    test_assert(!ecs_is_alive(world, e3));

    ecs_entity_t e4 = ecs_new(world);
    test_expect_abort();
    ecs_add_id(world, e4, ecs_pair(EcsChildOf, e2));
}

void Observer_cache_test_14(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    {
        ecs_entity_t e0 = ecs_new(world);
        ecs_add(world, e0, Tag);

        ecs_entity_t e1 = ecs_new(world);
        ecs_add_pair(world, e1, EcsChildOf, e0);

        ecs_entity_t e2 = ecs_new(world);
        ecs_add_pair(world, e2, EcsChildOf, e1);

        ecs_delete_with(world, Tag);

        test_assert(!ecs_is_alive(world, e0));
        test_assert(!ecs_is_alive(world, e1));
        test_assert(!ecs_is_alive(world, e2));
    }

    {
        ecs_entity_t e0 = ecs_new(world);
        ecs_add(world, e0, Tag);

        ecs_entity_t e1 = ecs_new(world);
        ecs_add_pair(world, e1, EcsChildOf, e0);
        ecs_add(world, e1, Tag);

        ecs_entity_t e2 = ecs_new(world);
        ecs_add_pair(world, e2, EcsChildOf, e1);

        ecs_remove_all(world, Tag);

        test_assert(ecs_is_alive(world, e0));
        test_assert(ecs_is_alive(world, e1));
        test_assert(ecs_is_alive(world, e2));

        test_assert(!ecs_has(world, e0, Tag));
    }

    ecs_fini(world);
}

void Observer_cache_test_15(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r = ecs_new(world);
    ecs_entity_t e0 = ecs_new(world);
    ecs_entity_t e1 = ecs_new(world);

    ecs_add_pair(world, e1, r, e0);
    ecs_add_pair(world, e0, EcsChildOf, e1);

    ecs_run_aperiodic(world, 0);

    ecs_delete(world, e1);

    test_assert(!ecs_is_alive(world, e1));
    test_assert(!ecs_is_alive(world, e0));
    test_assert(ecs_is_alive(world, r));

    ecs_fini(world);
}

static
void Observer_w_run_aperiodic(ecs_iter_t *it) {
    test_int(it->count, 1);
    ecs_run_aperiodic(it->world, 0);
}

void Observer_cache_test_16(void) {
    ecs_world_t* world = ecs_init();

    ECS_TAG(world, Foo);

    ecs_add_pair(world, Foo, EcsOnInstantiate, EcsInherit);

    ECS_OBSERVER(world, Observer_w_run_aperiodic, EcsOnAdd, Foo);

    ecs_entity_t p1 = ecs_new(world);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, p1);

    ecs_run_aperiodic(world, 0);
    
    ecs_entity_t e2 = ecs_new(world);
    ecs_add_pair(world, e2, EcsIsA, p1);
    ecs_add_pair(world, e2, EcsChildOf, e1);
    ecs_add(world, e1, Foo);

    ecs_fini(world);
}

static int Observer_a_invoked = 0;
static int Observer_b_invoked = 0;

static void Observer_a(ecs_iter_t *it) {
    Observer_a_invoked += it->count;
}

static void Observer_b(ecs_iter_t *it) {
    Observer_b_invoked += it->count;
}

void Observer_filter_observer_after_observer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_observer(world, {
        .query.terms = {
            { .id = ecs_id(Position) },
            { .id = Tag }
        },
        .callback = Observer_a,
        .events = { EcsOnAdd }
    });

    ecs_observer(world, {
        .query.terms = {
            { .id = ecs_id(Position), .inout = EcsInOutNone },
            { .id = Tag, .inout = EcsInOutNone}
        },
        .callback = Observer_b,
        .events = { EcsOnAdd }
    });

    ecs_entity_t e = ecs_new_w(world, Tag);
    ecs_add(world, e, Position);

    test_int(Observer_a_invoked, 1);
    test_int(Observer_b_invoked, 1);

    ecs_fini(world);
}

void Observer_notify_after_defer_batched(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    Probe ctx = {0};

    ecs_observer(world, {
        .query.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Velocity) }
        },
        .callback = Observer,
        .events = { EcsOnAdd },
        .ctx = &ctx
    });

    ecs_defer_begin(world);
    ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});
    test_int(ctx.invoked, 0);
    ecs_set(world, e1, Velocity, {1, 2});
    ecs_defer_end(world);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e1);

    ecs_fini(world);
}

void Observer_notify_after_defer_batched_2_entities_in_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    Probe ctx = {0};

    ecs_observer(world, {
        .query.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Velocity) }
        },
        .callback = Observer,
        .events = { EcsOnAdd },
        .ctx = &ctx
    });

    ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});
    test_int(ctx.invoked, 0);
    ecs_set(world, e1, Velocity, {1, 2});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e1);

    ecs_os_zeromem(&ctx);
    
    ecs_entity_t e2 = ecs_new(world);

    ecs_defer_begin(world);
    ecs_set(world, e2, Position, {30, 40});
    ecs_set(world, e2, Velocity, {3, 4});
    test_int(ctx.count, 0);
    ecs_defer_end(world);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e2);

    ecs_fini(world);
}

void Observer_notify_after_defer_batched_2_entities_in_table_w_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    Probe ctx = {0};

    ecs_observer(world, {
        .query.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Velocity) }
        },
        .callback = Observer,
        .events = { EcsOnAdd },
        .ctx = &ctx
    });

    ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});
    test_int(ctx.invoked, 0);
    ecs_set(world, e1, Velocity, {1, 2});
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e1);

    ecs_os_zeromem(&ctx);
    
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);
    ecs_add_pair(world, e3, EcsChildOf, e2);

    ecs_defer_begin(world);
    ecs_set(world, e2, Position, {30, 40});
    ecs_set(world, e2, Velocity, {3, 4});
    test_int(ctx.count, 0);
    ecs_defer_end(world);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e2);

    ecs_fini(world);
}

void Observer_multi_observer_table_fill_w_singleton(void) {
    ecs_world_t* world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    Probe ctx = {0};

    ecs_observer(world, {
        .query.terms = {
            { .id = ecs_id(Position), .src.id = ecs_id(Position)|EcsIsEntity },
            { .id = ecs_id(Velocity) },
        },
        .callback = Observer,
        .events = { EcsOnTableFill },
        .ctx = &ctx
    });

    ecs_singleton_add(world, Position);
    test_int(ctx.invoked, 0);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Velocity);
    test_int(ctx.invoked, 0);

    ecs_run_aperiodic(world, 0);
    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

ECS_COMPONENT_DECLARE(Velocity);

static
void AddVelocity(ecs_iter_t *it) {
    for (int i = 0; i < it->count; i ++) {
        ecs_add(it->world, it->entities[i], Velocity);
        test_assert(!ecs_has(it->world, it->entities[i], Velocity));
    }
}

void Observer_add_in_yield_existing(void) {
    ecs_world_t* world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_entity_t e3 = ecs_new_w(world, Position);

    ecs_observer(world, {
        .query.terms = {
            { .id = ecs_id(Position) },
        },
        .callback = AddVelocity,
        .events = { EcsOnAdd },
        .yield_existing = true,
    });

    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e1, Velocity));

    test_assert(ecs_has(world, e2, Position));
    test_assert(ecs_has(world, e2, Velocity));

    test_assert(ecs_has(world, e3, Position));
    test_assert(ecs_has(world, e3, Velocity));

    ecs_fini(world);
}

void Observer_add_in_yield_existing_multi(void) {
    ecs_world_t* world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT_DEFINE(world, Velocity);

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_entity_t e3 = ecs_new_w(world, Position);

    ecs_add(world, e1, Mass);
    ecs_add(world, e2, Mass);
    ecs_add(world, e3, Mass);

    ecs_observer(world, {
        .query.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Mass) },
        },
        .callback = AddVelocity,
        .events = { EcsOnAdd },
        .yield_existing = true,
    });

    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e1, Mass));
    test_assert(ecs_has(world, e1, Velocity));

    test_assert(ecs_has(world, e2, Position));
    test_assert(ecs_has(world, e2, Mass));
    test_assert(ecs_has(world, e2, Velocity));

    test_assert(ecs_has(world, e3, Position));
    test_assert(ecs_has(world, e3, Mass));
    test_assert(ecs_has(world, e3, Velocity));

    ecs_fini(world);
}

void Observer_get_filter(void) {
    ecs_world_t* world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t o = ecs_observer(world, {
        .query.terms = {
            { .id = ecs_id(Position) }
        },
        .callback = Observer,
        .events = { EcsOnAdd },
    });

    const ecs_query_t *f = ecs_observer_get(world, o)->query;
    test_assert(f != NULL);
    test_int(f->term_count, 1);
    test_uint(ecs_id(Position), f->terms[0].id);
    
    ecs_fini(world);
}

void Observer_uni_observer_eval_count(void) {
    ecs_world_t* world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t o = ecs_observer(world, {
        .query.terms = {
            { .id = ecs_id(Position) },
        },
        .callback = Observer,
        .events = { EcsOnAdd },
    });

    const ecs_query_t *q = ecs_observer_get(world, o)->query;
    test_assert(q != NULL);
    test_int(q->eval_count, 0);

    ecs_new_w(world, Position);

    test_int(q->eval_count, 1);

    ecs_fini(world);
}

void Observer_multi_observer_eval_count(void) {
    ecs_world_t* world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t o = ecs_observer(world, {
        .query.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Velocity) },
        },
        .callback = Observer,
        .events = { EcsOnAdd },
    });

    const ecs_query_t *q = ecs_observer_get(world, o)->query;
    test_assert(q != NULL);
    test_int(q->eval_count, 0);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_int(q->eval_count, 0);

    ecs_add(world, e, Velocity);
    test_int(q->eval_count, 1);

    ecs_fini(world);
}

