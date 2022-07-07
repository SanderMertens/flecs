#include <api.h>

static
void Observer(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);
}

static
void Observer_w_value(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);

    test_int(it->count, 1);
    test_assert(it->entities != NULL);
    test_assert(it->entities[0] != 0);

    Position *p = ecs_term(it, Position, 1);
    test_int(p->x, 10);
    test_int(p->y, 20);

    if (it->term_count > 1) {
        Velocity *v = ecs_term(it, Velocity, 2);
        test_int(v->x, 1);
        test_int(v->y, 2);
    }
}

static
void Observer_w_1_value(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);

    test_int(it->count, 1);
    test_assert(it->entities != NULL);
    test_assert(it->entities[0] != 0);

    Position *p = ecs_term(it, Position, 1);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

static
void Observer_w_filter_term(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);

    test_int(it->count, 1);
    test_assert(it->entities != NULL);
    test_assert(it->entities[0] != 0);

    test_assert(it->ptrs == NULL);
}

static
void Observer_w_1_filter_term(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);

    test_int(it->count, 1);
    test_assert(it->entities != NULL);
    test_assert(it->entities[0] != 0);

    test_assert(it->ptrs != NULL);
    test_assert(it->ptrs[0] == NULL);
    test_assert(it->ptrs[1] != NULL);

    Velocity *v = ecs_term(it, Velocity, 2);
    test_int(v->x, 1);
    test_int(v->y, 2);
}

static
void Observer_w_2_filter_terms(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);

    test_int(it->count, 1);
    test_assert(it->entities != NULL);
    test_assert(it->entities[0] != 0);

    test_assert(it->ptrs != NULL);
    test_assert(it->sizes != NULL);

    test_assert(it->ptrs[0] == NULL);
    test_assert(it->ptrs[1] == NULL);
    test_assert(it->ptrs[2] != NULL);

    Mass *m = ecs_term(it, Mass, 3);
    test_int(m[0], 100);
}

static bool dummy_called = false;

static
void Dummy(ecs_iter_t *it) {
    dummy_called = true;
}

void Observer_2_terms_w_on_add() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{TagA}, {TagB}},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new_id(world);

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

void Observer_2_terms_w_on_remove() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{TagA}, {TagB}},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new_id(world);

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

void Observer_2_terms_w_on_set_value() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ecs_id(Position)}, {ecs_id(Velocity)}},
        .events = {EcsOnSet},
        .callback = Observer_w_value,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new_id(world);

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

void Observer_2_terms_w_on_remove_value() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ecs_id(Position)}, {ecs_id(Velocity)}},
        .events = {EcsOnRemove},
        .callback = Observer_w_value,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new_id(world);

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


void Observer_2_terms_w_on_add_2nd() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{TagA}, {TagB}},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new_id(world);

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

void Observer_2_terms_w_on_remove_2nd() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{TagA}, {TagB}},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new_id(world);

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

void Observer_2_pair_terms_w_on_add() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_id_t pair_a = ecs_pair(RelA, ObjA);
    ecs_id_t pair_b = ecs_pair(RelB, ObjB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{pair_a}, {pair_b}},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new_id(world);

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

void Observer_2_pair_terms_w_on_remove() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_id_t pair_a = ecs_pair(RelA, ObjA);
    ecs_id_t pair_b = ecs_pair(RelB, ObjB);    

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{pair_a}, {pair_b}},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new_id(world);

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

void Observer_2_wildcard_pair_terms_w_on_add() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ecs_pair(RelA, ObjA)}, {ecs_pair(RelB, EcsWildcard)}},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new_id(world);

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

void Observer_2_wildcard_pair_terms_w_on_add_2_matching() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ecs_pair(RelA, ObjA)}, {ecs_pair(RelB, EcsWildcard)}},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new_id(world);

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

void Observer_2_wildcard_pair_terms_w_on_add_3_matching() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);
    ECS_TAG(world, ObjC);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ecs_pair(RelA, ObjA)}, {ecs_pair(RelB, EcsWildcard)}},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new_id(world);

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


void Observer_2_wildcard_pair_terms_w_on_remove() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ecs_pair(RelA, ObjA)}, {ecs_pair(RelB, EcsWildcard)}},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new_id(world);

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

void Observer_on_set_n_entities() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ecs_id(Self)}},
        .events = {EcsOnSet},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e_1 = ecs_new_id(world);
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

    ecs_entity_t e_2 = ecs_new_id(world);
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

    ecs_entity_t e_3 = ecs_new_id(world);
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

void Observer_on_set_n_entities_2_comp() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Obj);
    ECS_COMPONENT(world, Self);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ecs_id(Self)}, {ecs_pair(ecs_id(Self), EcsWildcard)}},
        .events = {EcsOnSet},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e_1 = ecs_new_id(world);
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

    ecs_entity_t e_2 = ecs_new_id(world);
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

void Observer_wildcard_pair_w_pred_component() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);
    ECS_COMPONENT(world, Position);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ecs_pair(ecs_id(Position), EcsWildcard)}},
        .events = {EcsOnSet},
        .callback = Observer_w_value,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new_id(world);
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
    Position *p = ecs_get_mut_pair(world, e, Position, ObjA);
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

void Observer_wildcard_pair_w_obj_component() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_COMPONENT(world, Position);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ ecs_pair(EcsWildcard, ecs_id(Position)) }},
        .events = {EcsOnSet},
        .callback = Observer_w_value,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new_id(world);
    test_int(ctx.invoked, 0);

    ecs_set_pair_object(world, e, RelA, Position, {10, 20});

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
    Position *p = ecs_get_mut_pair_second(world, e, RelA, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    p->x = 30;
    p->y = 40;

    ecs_os_zeromem(&ctx);

    ecs_set_pair_object(world, e, RelB, Position, {10, 20});

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

void Observer_2_terms_1_not_w_on_add() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{TagA}, {TagB, .oper = EcsNot}},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new_id(world);

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

void Observer_2_terms_1_not_w_on_remove() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{TagA}, {TagB, .oper = EcsNot}},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new_id(world);

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

void Observer_2_terms_w_on_set() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ecs_id(Position)}, {TagB}},
        .events = {EcsOnSet},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new_id(world);

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

void Observer_2_terms_w_un_set() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ecs_id(Position)}, {TagB}},
        .events = {EcsUnSet},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new_id(world);

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
    test_int(ctx.event, EcsUnSet);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.c[0][1], TagB);

    ecs_fini(world);
}

void Observer_3_terms_2_or_on_add() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{TagA}, {TagB, .oper = EcsOr}, {TagC, .oper = EcsOr}},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new_id(world);

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

void Observer_3_terms_2_or_on_remove() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{TagA}, {TagB, .oper = EcsOr}, {TagC, .oper = EcsOr}},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new_id(world);

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

void Observer_2_terms_w_from_entity_on_add() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    
    ecs_entity_t x = ecs_new_id(world);
    
    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{TagA}, {TagB, .oper = EcsOr, .subj.entity = x}},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new_id(world);

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

void Observer_2_terms_on_remove_on_clear() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{TagA}, {TagB}},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new_id(world);

    ecs_add_id(world, e, TagA);
    test_int(ctx.invoked, 0);

    ecs_add_id(world, e, TagB);
    test_int(ctx.invoked, 0);

    ecs_clear(world, e);
    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void Observer_2_terms_on_remove_on_delete() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{TagA}, {TagB}},
        .events = {EcsOnRemove},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new_id(world);

    ecs_add_id(world, e, TagA);
    test_int(ctx.invoked, 0);

    ecs_add_id(world, e, TagB);
    test_int(ctx.invoked, 0);

    ecs_delete(world, e);
    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void Observer_add_after_delete_observer() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t observer = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{.id = ecs_id(Position) }},
        .events = {EcsOnAdd},
        .callback = Dummy
    });

    ecs_entity_t e1 = ecs_new(world, Position);
    test_assert(e1 != 0);
    test_assert(ecs_has(world, e1, Position));
    test_int(dummy_called, 1);

    dummy_called = 0;

    ecs_delete(world, observer);
    test_int(dummy_called, 0);

    ecs_entity_t e2 = ecs_new(world, Position);
    test_assert(e2 != 0);
    test_assert(ecs_has(world, e2, Position));
    test_int(dummy_called, 0);

    ecs_fini(world);
}

void Observer_remove_after_delete_observer() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t observer = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{.id = ecs_id(Position) }},
        .events = {EcsOnRemove},
        .callback = Dummy
    });

    ecs_entity_t e1 = ecs_new(world, Position);
    test_assert(e1 != 0);
    test_assert(ecs_has(world, e1, Position));
    test_int(dummy_called, 0);

    ecs_remove(world, e1, Position);
    test_int(dummy_called, 1);

    dummy_called = 0;

    ecs_delete(world, observer);
    test_int(dummy_called, 0);

    ecs_entity_t e2 = ecs_new(world, Position);
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

void Observer_delete_observer_w_ctx() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms =  {{ Tag }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx_value,
        .ctx_free = ctx_free,
        .binding_ctx = &binding_ctx_value,
        .binding_ctx_free = binding_ctx_free
    });
    test_assert(o != 0);

    test_assert(ecs_get_observer_ctx(world, o) == &ctx_value);
    test_assert(ecs_get_observer_binding_ctx(world, o) == &binding_ctx_value);

    ecs_delete(world, o);

    test_int(ctx_value, 1);
    test_int(binding_ctx_value, 1);

    ecs_fini(world);
}

void Observer_filter_w_strings() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{.pred.name = "TagA"}, {.pred.name = "TagB"}},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new_id(world);

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

void Observer_iter_type_set() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ Tag }},
        .events = {EcsOnAdd},
        .callback = TypeObserver,
        .ctx = &ctx
    });
    test_assert(t != 0);

    ecs_new(world, Tag);

    test_assert(trigger_table != NULL);
    test_assert(trigger_table != NULL);
    const ecs_type_t *type = ecs_table_get_type(trigger_table);
    test_int(type->count, 1);
    test_int(type->array[0], Tag);
    
    ecs_fini(world);
}

void ObserverReadonly(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);
    test_bool(ecs_term_is_readonly(it, 1), true);
}

void Observer_readonly_term() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ TagA, .inout = EcsIn }},
        .events = {EcsOnAdd},
        .callback = ObserverReadonly,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new_id(world);
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

void Observer_trigger_on_prefab() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx_1 = {0};
    Probe ctx_2 = {0};

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ TagA }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx_1
    });

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ TagA }, { EcsPrefab, .oper = EcsOptional}},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx_2
    });

    ecs_new(world, TagA);
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

void Observer_trigger_on_disabled() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx_1 = {0};
    Probe ctx_2 = {0};

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ TagA }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx_1
    });

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ TagA }, { EcsDisabled, .oper = EcsOptional}},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx_2
    });

    ecs_new(world, TagA);
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

void Observer_unset_1_of_1() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, UnSet, EcsUnSet, Position);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);
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

void Observer_unset_1_of_2() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, UnSet, EcsUnSet, Position, Velocity);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);
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

void Observer_unset_1_of_3() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_OBSERVER(world, UnSet, EcsUnSet, Position, Velocity, Mass);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);
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

void Observer_unset_on_delete_1() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, UnSet, EcsUnSet, Position);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);    

    ecs_new(world, Position);
    test_int(ctx.invoked, 0);

    ecs_new(world, Position);
    test_int(ctx.invoked, 0);

    ecs_entity_t e = ecs_new(world, Position);
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

void Observer_unset_on_delete_2() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, UnSet, EcsUnSet, Position, Velocity);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);    

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_add(world, e1, Velocity);
    test_int(ctx.invoked, 0);

    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_add(world, e2, Velocity);
    test_int(ctx.invoked, 0);

    ecs_entity_t e3 = ecs_new(world, Position);
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

void Observer_unset_on_delete_3() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_OBSERVER(world, UnSet, EcsUnSet, Position, Velocity, Mass);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);    

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_add(world, e1, Velocity);
    ecs_add(world, e1, Mass);
    test_int(ctx.invoked, 0);

    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e2, Mass);
    test_int(ctx.invoked, 0);

    ecs_entity_t e3 = ecs_new(world, Position);
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

void Observer_unset_on_fini_1() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, UnSet, EcsUnSet, Position);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);    

    ecs_entity_t e1 = ecs_new(world, Position);
    test_int(ctx.invoked, 0);

    ecs_entity_t e2 = ecs_new(world, Position);
    test_int(ctx.invoked, 0);

    ecs_entity_t e3 = ecs_new(world, Position);
    test_int(ctx.invoked, 0);

    ecs_new(world, Velocity);
    test_int(ctx.invoked, 0);

    ecs_fini(world);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 3);
    test_int(ctx.system, UnSet);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);

    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);      
}

void Observer_unset_on_fini_2() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, UnSet, EcsUnSet, Position, Velocity);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);    

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_add(world, e1, Velocity);
    test_int(ctx.invoked, 0);

    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_add(world, e2, Velocity);
    test_int(ctx.invoked, 0);

    ecs_entity_t e3 = ecs_new(world, Position);
    ecs_add(world, e3, Velocity);
    test_int(ctx.invoked, 0);

    ecs_new(world, Position);
    test_int(ctx.invoked, 0);

    ecs_new(world, Velocity);
    test_int(ctx.invoked, 0);    

    ecs_fini(world);

    test_int(ctx.count, 3);
    test_int(ctx.system, UnSet);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);

    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0); 
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0);  
}

void Observer_unset_on_fini_3() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_OBSERVER(world, UnSet, EcsUnSet, Position, Velocity, Mass);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);    

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_add(world, e1, Velocity);
    ecs_add(world, e1, Mass);
    test_int(ctx.invoked, 0);

    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e2, Mass);
    test_int(ctx.invoked, 0);

    ecs_entity_t e3 = ecs_new(world, Position);
    ecs_add(world, e3, Velocity);
    ecs_add(world, e3, Mass);
    test_int(ctx.invoked, 0);

    ecs_new(world, Position);
    test_int(ctx.invoked, 0);

    ecs_new(world, Velocity);
    test_int(ctx.invoked, 0); 

    ecs_new(world, Mass);
    test_int(ctx.invoked, 0);        

    ecs_fini(world);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 3);
    test_int(ctx.system, UnSet);
    test_int(ctx.term_count, 3);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);

    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0); 
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0); 
    test_int(ctx.c[0][2], ecs_id(Mass));
    test_int(ctx.s[0][2], 0);         
}

void Observer_overlapping_unset_systems() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_OBSERVER(world, UnSetA, EcsUnSet, Position);
    ECS_OBSERVER(world, UnSetB, EcsUnSet, Position, Velocity);
    ECS_OBSERVER(world, UnSet, EcsUnSet, Position, Velocity);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);  

    ecs_entity_t e = ecs_new(world, Position);
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
    if (!ecs_get_context(it->world)) {
        return;
    }

    probe_iter(it);

    test_int(it->count, 1);

    Position *p = ecs_term(it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Observer_unset_move_to_nonempty_table() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_OBSERVER(world, UnSet_TestComp, EcsUnSet, Position);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);  

    ECS_ENTITY(world, DummyA, Position, Velocity);
    test_int(ctx.invoked, 0);

    ECS_ENTITY(world, DummyB, Position, Velocity);
    test_int(ctx.invoked, 0);

    ECS_ENTITY(world, e, Position, Velocity);
    ecs_set(world, e, Position, {10, 20});
    ecs_set(world, e, Velocity, {20, 10});
    test_int(ctx.invoked, 0);

    ecs_entity_t e2 = ecs_new(world, Velocity);
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
    ecs_set_context(world, NULL);

    ecs_fini(world);
}

static
void UnSet_WriteComp(ecs_iter_t *it) {
    if (!ecs_get_context(it->world)) {
        return;
    }

    probe_iter(it);

    test_int(it->count, 1);

    Position *p = ecs_term(it, Position, 1);
    test_assert(p != NULL);

    Velocity *v = ecs_term(it, Velocity, 2);

    test_int(p->x, 10);
    test_int(p->y, 20);

    test_int(v->x, 1);
    test_int(v->y, 2);

    v->x = 2;
    v->y = 3;
}

void Observer_write_in_unset() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_OBSERVER(world, UnSet_WriteComp, EcsUnSet, Position, Velocity);

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    ecs_set(world, e, Velocity, {1, 2});

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

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
    ecs_set_context(world, NULL);    

    const Velocity *v = ecs_get(world, e, Velocity);
    test_int(v->x, 2);
    test_int(v->y, 3);

    ecs_fini(world);
}

void Observer_filter_term() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ .id = ecs_id(Position), .inout = EcsInOutFilter }},
        .events = {EcsOnSet},
        .callback = Observer_w_filter_term,
        .ctx = &ctx
    });

    /* Create entities before trigger */
    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
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

void Observer_2_terms_1_filter() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {
            { .id = ecs_id(Position), .inout = EcsInOutFilter },
            { .id = ecs_id(Velocity) }
        },
        .events = {EcsOnSet},
        .callback = Observer_w_1_filter_term,
        .ctx = &ctx
    });

    /* Create entities before trigger */
    ecs_entity_t e1 = ecs_set(world, 0, Velocity, {1, 2});
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

void Observer_3_terms_2_filter() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {
            { .id = ecs_id(Position), .inout = EcsInOutFilter },
            { .id = ecs_id(Velocity), .inout = EcsInOutFilter },
            { .id = ecs_id(Mass) }
        },
        .events = {EcsOnSet},
        .callback = Observer_w_2_filter_terms,
        .ctx = &ctx
    });

    /* Create entities before trigger */
    ecs_entity_t e1 = ecs_set(world, 0, Velocity, {1, 2});
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

void Observer_and_from() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_PREFAB(world, Type, TagA, TagB);

    Probe ctx = {0};
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {
            { ECS_AND | Type}
        },
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
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

void Observer_or_from() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_PREFAB(world, Type, TagA, TagB);

    Probe ctx = {0};
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {
            { ECS_OR | Type}
        },
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
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

void Observer_notify_propagated_twice() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ TagA }},
        .events = {EcsOnAdd},
        .callback = TriggerTwice,
        .ctx = &ctx
    });
    test_assert(t1 != 0);

    base_ent = ecs_new_id(world);
    inst_ent_a = ecs_new_w_pair(world, EcsIsA, base_ent);
    ecs_add(world, inst_ent_a, TagB);
    inst_ent_b = ecs_new_w_pair(world, EcsIsA, base_ent);

    test_int(ctx.invoked, 0);

    ecs_add(world, base_ent, TagA);

    test_int(ctx.invoked, 3);
    test_int(invoke_count, 3);

    ecs_fini(world);
}

void Observer_on_add_yield_existing() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    /* Create entities before trigger */
    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ Tag }},
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
    ecs_new(world, Tag);
    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void Observer_on_add_yield_existing_2_tables() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    /* Create entities before trigger */
    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    ecs_add(world, e3, TagB);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ TagA }},
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
    ecs_new(world, TagA);
    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void Observer_on_add_yield_existing_2_terms() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    /* Create entities before trigger */
    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);
    ecs_entity_t e4 = ecs_new(world, TagA); /* no match */
    ecs_entity_t e5 = ecs_new(world, TagB); /* no match */

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
        .filter.terms = {{ TagA }, { TagB }},
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
    ecs_entity_t e6 = ecs_new(world, TagA);
    test_int(ctx.invoked, 0);
    ecs_add(world, e6, TagB);
    test_int(ctx.invoked, 1);

    ecs_fini(world);
}

void Observer_observer_superset_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_entity_t base = ecs_new_id(world);
    ecs_entity_t inst = ecs_new_id(world);
    ecs_add_pair(world, inst, EcsIsA, base);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ .id = ecs_pair(Rel, EcsWildcard), .subj.set.mask = EcsSuperSet }},
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

void Observer_observer_superset_wildcard_add_isa() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_entity_t base = ecs_new_id(world);
    ecs_add_pair(world, base, Rel, ObjA);
    ecs_add_pair(world, base, Rel, ObjB);

    ecs_entity_t inst = ecs_new_id(world);
    
    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ .id = ecs_pair(Rel, EcsWildcard), .subj.set.mask = EcsSuperSet }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(t != 0);

    test_int(ctx.invoked, 0);

    ecs_add_pair(world, inst, EcsIsA, base);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, ecs_pair(Rel, EcsWildcard));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], inst);
    test_int(ctx.s[0][0], base);

    ecs_fini(world);
}

void Observer_observer_superset_wildcard_add_isa_at_offset() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_entity_t base = ecs_new_id(world);
    ecs_add(world, base, Tag);
    ecs_add_pair(world, base, Rel, ObjA);
    ecs_add_pair(world, base, Rel, ObjB);

    ecs_entity_t inst = ecs_new_id(world);
    
    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ .id = ecs_pair(Rel, EcsWildcard), .subj.set.mask = EcsSuperSet }},
        .events = {EcsOnAdd},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(t != 0);

    test_int(ctx.invoked, 0);

    ecs_add_pair(world, inst, EcsIsA, base);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, ecs_pair(Rel, EcsWildcard));
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], inst);
    test_int(ctx.s[0][0], base);

    ecs_fini(world);
}

void Observer_on_set_w_tag() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ TagA }},
        .events = {EcsOnSet},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(t1 != 0);

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);

    ecs_add(world, e, TagA);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, TagA);

    ecs_fini(world);
}

void Observer_mixed_on_set_w_tag() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_COMPONENT(world, Position);

    Probe ctx = {0};
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ TagA }, { ecs_id(Position) }},
        .events = {EcsOnSet},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(t1 != 0);

    ecs_entity_t e1 = ecs_new_id(world);
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

    ecs_entity_t e2 = ecs_new_id(world);
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

void Observer_mixed_un_set_w_tag() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_COMPONENT(world, Position);

    Probe ctx = {0};
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ TagA }, { ecs_id(Position) }},
        .events = {EcsUnSet},
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(t1 != 0);

    ecs_entity_t e1 = ecs_new_id(world);
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

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_set(world, e2, Position, {10, 20});
    ecs_add(world, e2, TagA);
    test_int(ctx.invoked, 0);

    ecs_remove(world, e2, Position);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e2);
    test_int(ctx.event, EcsUnSet);
    test_int(ctx.event_id, ecs_id(Position));

    ecs_fini(world);
}

void Observer_match_base_w_id_at_offset() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    Probe ctx_1 = {0};
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ ecs_id(Position) }},
        .events = {EcsOnSet},
        .callback = Observer,
        .ctx = &ctx_1
    });
    test_assert(t1 != 0);

    Probe ctx_2 = {0};
    ecs_entity_t t2 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ ecs_id(Velocity) }},
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
    if (ecs_observer_default_run_action(it)) {
        run_invoked_matched ++;
    }
    run_invoked ++;
}

static void Run_w_iter_next(ecs_iter_t *it) {
    run_invoked ++;
    
    test_assert(it != NULL);
    test_assert(it->next != NULL);
    test_assert(it->callback != NULL);

    while (ecs_iter_next(it)) {
        it->callback(it);
    }
}

void Observer_custom_run_action() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ TagA }},
        .events = {EcsOnAdd},
        .run = Run,
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(t1 != 0);

    ecs_entity_t e = ecs_new(world, TagA);

    test_int(run_invoked, 1);
    test_int(run_invoked_matched, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e);
    test_int(ctx.event, EcsOnAdd);
    
    ecs_fini(world);
}

void Observer_custom_run_action_w_iter_next() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    Probe ctx = {0};
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ TagA }},
        .events = {EcsOnAdd},
        .run = Run_w_iter_next,
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(t1 != 0);

    ecs_entity_t e = ecs_new(world, TagA);

    test_int(run_invoked, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e);
    test_int(ctx.event, EcsOnAdd);
    
    ecs_fini(world);
}

void Observer_custom_run_action_2_terms() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ TagA }, { TagB }},
        .events = {EcsOnAdd},
        .run = Run,
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(t1 != 0);

    ecs_entity_t e = ecs_new(world, TagA);
    test_int(run_invoked, 1);
    run_invoked = 0;

    ecs_add(world, e, TagB);
    test_int(run_invoked, 1);
    test_int(run_invoked_matched, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e);
    test_int(ctx.event, EcsOnAdd);
    
    ecs_fini(world);
}

void Observer_custom_run_action_w_iter_next_2_terms() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    Probe ctx = {0};
    ecs_entity_t t1 = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ TagA }, { TagB }},
        .events = {EcsOnAdd},
        .run = Run_w_iter_next,
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(t1 != 0);

    ecs_entity_t e = ecs_new(world, TagA);
    test_int(run_invoked, 1);
    run_invoked = 0;

    ecs_add(world, e, TagB);
    test_int(run_invoked, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e);
    test_int(ctx.event, EcsOnAdd);
    
    ecs_fini(world);
}

void Observer_read_in_on_remove_after_add_other_w_not() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);
    ECS_TAG(world, O);

    Probe ctx = {0};
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {
            { .id = ecs_pair(ecs_id(Position), O)},
            { .id = Tag, .oper = EcsNot }
        },
        .events = {EcsOnRemove},
        .callback = Observer_w_1_value,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_set_pair(world, 0, Position, O, { 10, 20 });
    test_int(ctx.invoked, 0);

    ecs_add(world, e, Tag);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.e[0], e);
    test_int(ctx.event, EcsOnRemove);

    ecs_fini(world);
}
