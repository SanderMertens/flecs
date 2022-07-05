#include <api.h>

void TriggerOnAdd_setup() {
    ecs_log_set_level(-3);
}

void Init(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    
    Velocity *v = NULL;
    if (it->term_count >= 2) {
        v = ecs_term(it, Velocity, 2);
    }

    probe_iter(it);

    int i;
    for (i = 0; i < it->count; i ++) {
        p[i].x = 10;
        p[i].y = 20;

        if (v) {
            v[i].x = 30;
            v[i].y = 40;
        }
    }
}

static
void Add_to_current(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);

    int i;
    for (i = 0; i < it->count; i ++) {
        if (ctx->component) {
            ecs_add_id(it->world, it->entities[i], ctx->component);

            test_assert( !!ecs_get_type(it->world, it->entities[i]));
        }

        if (ctx->component_2) {
            ecs_add_id(it->world, it->entities[i], ctx->component_2);
        }

        ctx->entity_count ++;
    }
}

static
void Remove_from_current(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];

        if (ctx->component) {
            ecs_remove_id(it->world, e, ctx->component);
        }

        if (ctx->component_2) {
            ecs_remove_id(it->world, e, ctx->component_2);
        }

        ctx->entity_count ++;
    }
}

static
void Set_current(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    
    ecs_entity_t ecs_id(Rotation) = ctx->component;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], Rotation, {10 + it->entities[i]});
        ctx->entity_count ++;
    }
}

static bool dummy_called = false;

static
void Dummy(ecs_iter_t *it) {
    dummy_called = true;
}

void TriggerOnAdd_new_match_1_of_1() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, Init, EcsOnAdd, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void TriggerOnAdd_new_match_1_of_2() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, Init, EcsOnAdd, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ECS_ENTITY(world, e, Position, Velocity);
    test_assert(e != 0);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void TriggerOnAdd_new_no_match_1() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, Init, EcsOnAdd, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Velocity);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void TriggerOnAdd_add_match_1_of_1() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, Init, EcsOnAdd, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_add(world, e, Position);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void TriggerOnAdd_add_match_1_of_2() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, Init, EcsOnAdd, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void TriggerOnAdd_add_no_match_1() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, Init, EcsOnAdd, Velocity);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_add(world, e, Position);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void TriggerOnAdd_set_match_1_of_1() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, Init, EcsOnAdd, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_set(world, e, Position, {1, 2});

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void TriggerOnAdd_set_no_match_1() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, Init, EcsOnAdd, Velocity);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_set(world, e, Position, {1, 2});

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void TriggerOnAdd_clone_match_1_of_1() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, Init, EcsOnAdd, Position);

    ecs_entity_t e1 = ecs_new(world, Position);
    test_assert(e1 != 0);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e2 = ecs_clone(world, 0, e1, false);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e2);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e2, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void TriggerOnAdd_clone_match_1_of_2() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, Init, EcsOnAdd, Position);

    ECS_ENTITY(world, e1, Position, Velocity);
    test_assert(e1 != 0);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e2 = ecs_clone(world, 0, e1, false);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e2);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e2, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void TriggerOnAdd_add_again_1() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, Init, EcsOnAdd, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_add(world, e, Position);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void TriggerOnAdd_set_again_1() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, Init, EcsOnAdd, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_set(world, e, Position, {10, 20});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_set(world, e, Position, {30, 40});

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void TriggerOnAdd_add_again_2() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, Init, EcsOnAdd, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void TriggerOnAdd_new_w_count_match_1_of_1() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, Init, EcsOnAdd, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    const ecs_entity_t *ids = ecs_bulk_new(world, Position, 3);
    test_assert(ids != NULL);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], ids[0]);
    test_int(ctx.e[1], ids[1]);
    test_int(ctx.e[2], ids[2]);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    int i;
    for (i = 0; i < 3; i ++) {
        const Position *p = ecs_get(world, ids[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

static
void AddVelocity(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);

    ecs_id_t v = 0;
    if (it->ctx) {
        v = *(ecs_id_t*)it->ctx;
    }
    
    if (!v) {
        v = ecs_term_id(it, 2);
    }

    probe_iter(it);

    int i;
    for (i = 0; i < it->count; i ++) {
        p[i].x = 1;
        p[i].y = 2;
        ecs_add_id(it->world, it->entities[i], v);
    }
}

void TriggerOnAdd_override_after_add_in_on_add() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_PREFAB(world, Prefab, Position);
    ecs_set(world, Prefab, Position, {1, 2});

    ECS_OBSERVER(world, AddVelocity, EcsOnAdd, Position(self));

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .entity = { .entity = AddVelocity}, .ctx = &ecs_id(Velocity)
    });

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, Prefab);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_add(world, e, Position);
    test_assert( ecs_has(world, e, Position));
    test_assert( ecs_has(world, e, Velocity));

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, AddVelocity);
    test_int(ctx.term_count, 1);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

static
void OnSetPosition(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        p[i].x ++;
        p[i].y ++;
    }
}

void TriggerOnAdd_set_after_add_in_on_add() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_OBSERVER(world, AddVelocity, EcsOnAdd, Position);
    ECS_OBSERVER(world, OnSetPosition, EcsOnSet, Position);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .entity = {.entity = AddVelocity}, .ctx = &ecs_id(Velocity)
    });

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_add(world, e, Position);
    test_assert( ecs_has(world, e, Position));
    test_assert( ecs_has(world, e, Velocity));

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, AddVelocity);
    test_int(ctx.term_count, 2);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 2);
    test_int(p->y, 3);

    ecs_fini(world);
}

static
void AddAgain(ecs_iter_t *it) {
    ecs_id_t ecs_id(Position) = ecs_term_id(it, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add(it->world, it->entities[i], Position);
    }
}

void TriggerOnAdd_add_again_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, Init, EcsOnAdd, Position);
    ECS_SYSTEM(world, AddAgain, EcsOnUpdate, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

static
void AddMass(ecs_iter_t *it) {
    Mass *m = ecs_term(it, Mass, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        m[i] = 10;
    }
}

void TriggerOnAdd_add_in_progress_before_system_def() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_SYSTEM(world, AddVelocity, EcsOnUpdate, Position, Velocity());

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));
    test_assert( !ecs_has(world, e, Velocity));

    ecs_progress(world, 1);

    test_assert( ecs_has(world, e, Position));
    test_assert( ecs_has(world, e, Velocity));

    ECS_COMPONENT(world, Mass);
    ECS_OBSERVER(world, AddMass, EcsOnAdd, Mass);

    ecs_add(world, e, Mass);
    test_assert( ecs_has(world, e, Mass));
    
    const Mass *m = ecs_get(world, e, Mass);
    test_assert(m != NULL);
    test_int(*m, 10);

    ecs_fini(world);
}

void SystemA(ecs_iter_t *it) {
    int i, tag;
    for (i = 0; i < it->count; i ++) {
        for (tag = 1000; tag < 1100; tag ++) {
            ecs_add_id(it->world, it->entities[i], tag);
        }
    }
}

void SystemB(ecs_iter_t *it) {
    ecs_id_t ecs_id(Position) = ecs_term_id(it, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_has(it->world, it->entities[i], Position);
    }
}

void TriggerOnAdd_2_systems_w_table_creation() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, SystemA, EcsOnAdd, Position);
    ECS_OBSERVER(world, SystemB, EcsOnAdd, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);    

    ecs_fini(world);
}

void NewWithPosition(ecs_iter_t *it) {
    ecs_id_t ecs_id(Position) = ecs_term_id(it, 1);

    ecs_entity_t e = ecs_new(it->world, Position);
    test_assert(e != 0); 
}

void TriggerOnAdd_2_systems_w_table_creation_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, SystemA, EcsOnAdd, Position);
    ECS_OBSERVER(world, SystemB, EcsOnAdd, Position);
    ECS_SYSTEM(world, NewWithPosition, EcsOnUpdate, Position());

    ecs_progress(world, 1);   

    ecs_fini(world);
}

static
void TestContext(ecs_iter_t *it) {
    void *world_ctx = ecs_get_context(it->world);
    test_assert(world_ctx == it->ctx);
    int32_t *param = it->ctx;
    (*param) ++;
}

void TriggerOnAdd_sys_context() {
    ecs_world_t *world = ecs_mini();
    int32_t param = 0;

    ECS_COMPONENT(world, Position);

    ECS_OBSERVER(world, TestContext, EcsOnAdd, Position);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .entity = {.entity = TestContext}, .ctx = &param
    });

    test_assert(ecs_get_observer_ctx(world, TestContext) == &param);

    ecs_fini(world);
}

void TriggerOnAdd_get_sys_context_from_param() {
    ecs_world_t *world = ecs_mini();
    int32_t param = 0;

    ECS_COMPONENT(world, Position);

    ECS_OBSERVER(world, TestContext, EcsOnAdd, Position);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .entity = {.entity = TestContext}, .ctx = &param
    });

    /* Set world context so system can compare if pointer is correct */
    ecs_set_context(world, &param);

    /* Trigger system */
    ecs_new(world, Position);

    test_int(param, 1);

    ecs_fini(world);
}

void TriggerOnAdd_remove_added_component_in_on_add_w_set() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, Remove_from_current, EcsOnAdd, Position);

    IterData ctx = {.component = ecs_id(Position)};
    ecs_set_context(world, &ctx);

    test_expect_abort();

    ecs_set(world, 0, Position, {0, 0});
}

void Add_3_to_current(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    int i;
    for (i = 0; i < it->count; i ++) {
        if (ctx->component_3) {
            ecs_add_id(it->world, it->entities[i], ctx->component_3);
        }
        ctx->entity_count ++;
    }
}

void TriggerOnAdd_on_add_in_on_add() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_OBSERVER(world, Add_to_current, EcsOnAdd, Position);
    ECS_OBSERVER(world, Add_3_to_current, EcsOnAdd, Velocity);

    IterData ctx = {.component = ecs_id(Velocity), .component_3 = ecs_id(Mass)};
    ecs_set_context(world, &ctx);

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_entity_t e3 = ecs_new(world, Position);

    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e3, Position));

    test_assert( ecs_has(world, e1, Velocity));
    test_assert( ecs_has(world, e2, Velocity));
    test_assert( ecs_has(world, e3, Velocity));

    test_assert( ecs_has(world, e1, Mass));
    test_assert( ecs_has(world, e2, Mass));
    test_assert( ecs_has(world, e3, Mass));

    ecs_fini(world);
}

void TriggerOnAdd_on_remove_in_on_add() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_OBSERVER(world, Remove_from_current, EcsOnAdd, Position);
    ECS_OBSERVER(world, Dummy, EcsOnRemove, Velocity);

    IterData ctx = {.component = ecs_id(Velocity)};
    ecs_set_context(world, &ctx);

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);

    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e3, Position));

    test_assert( !ecs_has(world, e1, Velocity));
    test_assert( !ecs_has(world, e2, Velocity));
    test_assert( !ecs_has(world, e3, Velocity));

    test_assert(dummy_called);

    ecs_fini(world);
}

void TriggerOnAdd_on_set_in_on_add() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Rotation);
    ECS_COMPONENT(world, Mass);

    ECS_OBSERVER(world, Set_current, EcsOnAdd, Position);
    ECS_OBSERVER(world, Add_3_to_current, EcsOnSet, Rotation);

    IterData ctx = {.component = ecs_id(Rotation), .component_3 = ecs_id(Mass)};
    ecs_set_context(world, &ctx);

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_entity_t e3 = ecs_new(world, Position);

    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e3, Position));

    test_assert( ecs_has(world, e1, Rotation));
    test_assert( ecs_has(world, e2, Rotation));
    test_assert( ecs_has(world, e3, Rotation));

    test_assert( ecs_has(world, e1, Mass));
    test_assert( ecs_has(world, e2, Mass));
    test_assert( ecs_has(world, e3, Mass));

    const Rotation *r = ecs_get(world, e1, Rotation);
    test_assert(r != NULL);
    test_int(*r, 10 + e1);

    r = ecs_get(world, e2, Rotation);
    test_assert(r != NULL);
    test_int(*r, 10 + e2);

    r = ecs_get(world, e3, Rotation);
    test_assert(r != NULL);
    test_int(*r, 10 + e3);

    ecs_fini(world);
}

void TriggerOnAdd_on_add_in_on_update() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_SYSTEM(world, Add_to_current, EcsOnUpdate, Position);
    ECS_OBSERVER(world, Add_3_to_current, EcsOnAdd, Velocity);

    IterData ctx = {.component = ecs_id(Velocity), .component_3 = ecs_id(Mass)};
    ecs_set_context(world, &ctx);

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_entity_t e3 = ecs_new(world, Position);

    ecs_progress(world, 1);

    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e3, Position));

    test_assert( ecs_has(world, e1, Velocity));
    test_assert( ecs_has(world, e2, Velocity));
    test_assert( ecs_has(world, e3, Velocity));

    test_assert( ecs_has(world, e1, Mass));
    test_assert( ecs_has(world, e2, Mass));
    test_assert( ecs_has(world, e3, Mass));

    ecs_fini(world);
}

void TriggerOnAdd_emplace() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_id(Position),
        .events = {EcsOnAdd},
        .callback = Dummy
    });    

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);
    test_int(dummy_called, 0);

    Position *p = ecs_emplace(world, e, Position);
    test_assert(p != NULL);
    test_bool(dummy_called, true);

    ecs_fini(world);
}

void TriggerOnAdd_add_after_delete_trigger() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t trigger = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = ecs_id(Position),
        .events = {EcsOnAdd},
        .callback = Dummy
    });

    ecs_entity_t e1 = ecs_new(world, Position);
    test_assert(e1 != 0);
    test_assert(ecs_has(world, e1, Position));
    test_int(dummy_called, 1);

    dummy_called = 0;

    ecs_delete(world, trigger);
    test_int(dummy_called, 0);

    ecs_entity_t e2 = ecs_new(world, Position);
    test_assert(e2 != 0);
    test_assert(ecs_has(world, e2, Position));
    test_int(dummy_called, 0);

    ecs_fini(world);
}

void TriggerOnAdd_add_after_delete_wildcard_id_trigger() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t trigger = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = EcsWildcard,
        .events = {EcsOnAdd},
        .callback = Dummy
    });

    ecs_entity_t e1 = ecs_new(world, Position);
    test_assert(e1 != 0);
    test_assert(ecs_has(world, e1, Position));
    test_int(dummy_called, 1);

    dummy_called = 0;

    ecs_delete(world, trigger);
    test_int(dummy_called, 0);

    ecs_entity_t e2 = ecs_new(world, Position);
    test_assert(e2 != 0);
    test_assert(ecs_has(world, e2, Position));
    test_int(dummy_called, 0);

    ecs_fini(world);
}
