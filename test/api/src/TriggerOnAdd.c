#include <api.h>

void Init(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    
    Velocity *v = NULL;
    if (rows->column_count >= 2) {
        v = ecs_column(rows, Velocity, 2);
    }

    probe_system(rows);

    int i;
    for (i = 0; i < rows->count; i ++) {
        p[i].x = 10;
        p[i].y = 20;

        if (v) {
            v[i].x = 30;
            v[i].y = 40;
        }
    }
}

static
void Add_to_current(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);

    int i;
    for (i = 0; i < rows->count; i ++) {
        if (ctx->component) {
            ecs_add_entity(rows->world, rows->entities[i], ctx->component);

            test_assert( !!ecs_get_type(rows->world, rows->entities[i]));
        }

        if (ctx->component_2) {
            ecs_add_entity(rows->world, rows->entities[i], ctx->component_2);
        }

        ctx->entity_count ++;
    }
}

static
void Remove_from_current(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t e = rows->entities[i];

        if (ctx->component) {
            ecs_remove_entity(rows->world, e, ctx->component);
        }

        if (ctx->component_2) {
            ecs_remove_entity(rows->world, e, ctx->component_2);
        }

        ctx->entity_count ++;
    }
}

static
void Set_current(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    
    ecs_entity_t ecs_entity(Rotation) = ctx->component;

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_set(rows->world, rows->entities[i], Rotation, {10 + rows->entities[i]});
        ctx->entity_count ++;
    }
}

void TriggerOnAdd_new_match_1_of_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TRIGGER(world, Init, EcsOnAdd, Position, NULL);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void TriggerOnAdd_new_match_1_of_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_TRIGGER(world, Init, EcsOnAdd, Position, NULL);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void TriggerOnAdd_new_no_match_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TRIGGER(world, Init, EcsOnAdd, Position, NULL);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Velocity);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void TriggerOnAdd_add_match_1_of_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TRIGGER(world, Init, EcsOnAdd, Position, NULL);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_add(world, e, Position);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void TriggerOnAdd_add_match_1_of_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_TRIGGER(world, Init, EcsOnAdd, Position, NULL);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_add(world, e, Type);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void TriggerOnAdd_add_no_match_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TRIGGER(world, Init, EcsOnAdd, Velocity, NULL);

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
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TRIGGER(world, Init, EcsOnAdd, Position, NULL);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_set(world, e, Position, {1, 2});

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void TriggerOnAdd_set_no_match_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TRIGGER(world, Init, EcsOnAdd, Velocity, NULL);

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
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TRIGGER(world, Init, EcsOnAdd, Position, NULL);

    ecs_entity_t e_1 = ecs_new(world, Position);
    test_assert(e_1 != 0);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e_2 = ecs_clone(world, 0, e_1, false);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e_2);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get_ptr(world, e_2, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void TriggerOnAdd_clone_match_1_of_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_TRIGGER(world, Init, EcsOnAdd, Position, NULL);

    ecs_entity_t e_1 = ecs_new(world, Type);
    test_assert(e_1 != 0);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e_2 = ecs_clone(world, 0, e_1, false);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e_2);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get_ptr(world, e_2, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void TriggerOnAdd_add_again_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TRIGGER(world, Init, EcsOnAdd, Position, NULL);

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
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TRIGGER(world, Init, EcsOnAdd, Position, NULL);

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
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_TRIGGER(world, Init, EcsOnAdd, Position, NULL);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Type);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_add(world, e, Type);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void TriggerOnAdd_new_w_count_match_1_of_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TRIGGER(world, Init, EcsOnAdd, Position, NULL);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_bulk_new(world, Position, 3);
    test_assert(e != 0);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.e[1], e + 1);
    test_int(ctx.e[2], e + 2);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

static
void AddVelocity(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    ecs_type_t v = rows->param;
    if (!v) {
        ecs_entity_t e = ecs_column_entity(rows, 2);
        v = ecs_type_from_entity(rows->world, e);
    }

    probe_system(rows);

    int i;
    for (i = 0; i < rows->count; i ++) {
        p[i].x = 1;
        p[i].y = 2;
        ecs_add_type(rows->world, rows->entities[i], v);
    }
}

void TriggerOnAdd_override_after_add_in_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_PREFAB(world, Prefab, Position);
    ecs_set(world, Prefab, Position, {1, 2});

    ECS_TRIGGER(world, AddVelocity, EcsOnAdd, Position, ecs_type(Velocity));

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new_w_entity(world, ECS_INSTANCEOF | Prefab);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_add(world, e, Position);
    test_assert( ecs_has(world, e, Position));
    test_assert( ecs_has(world, e, Velocity));

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, AddVelocity);
    test_int(ctx.column_count, 1);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

static
void OnSetPosition(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        p[i].x ++;
        p[i].y ++;
    }
}

void TriggerOnAdd_set_after_add_in_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TRIGGER(world, AddVelocity, EcsOnAdd, Position, &ecs_entity(Velocity));
    ECS_SYSTEM(world, OnSetPosition, EcsOnSet, Position);

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
    test_int(ctx.column_count, 2);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 2);
    test_int(p->y, 3);

    ecs_fini(world);
}

static
void AddAgain(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, Position, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_add(rows->world, rows->entities[i], Position);
    }
}

void TriggerOnAdd_add_again_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TRIGGER(world, Init, EcsOnAdd, Position, NULL);
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
void AddMass(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Mass, m, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        m[i] = 10;
    }
}

void TriggerOnAdd_add_in_progress_before_system_def() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_SYSTEM(world, AddVelocity, EcsOnUpdate, Position, .Velocity);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));
    test_assert( !ecs_has(world, e, Velocity));

    ecs_progress(world, 1);

    test_assert( ecs_has(world, e, Position));
    test_assert( ecs_has(world, e, Velocity));

    ECS_COMPONENT(world, Mass);
    ECS_TRIGGER(world, AddMass, EcsOnAdd, Mass, NULL);

    ecs_add(world, e, Mass);
    test_assert( ecs_has(world, e, Mass));
    
    const Mass *m = ecs_get_ptr(world, e, Mass);
    test_assert(m != NULL);
    test_int(*m, 10);

    ecs_fini(world);
}

void SystemA(ecs_rows_t *rows) {
    int i, tag;
    for (i = 0; i < rows->count; i ++) {
        for (tag = 1000; tag < 1100; tag ++) {
            ecs_add_type(
                rows->world, 
                rows->entities[i], 
                ecs_type_from_entity(rows->world, tag));
        }
    }
}

void SystemB(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, Position, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_has(rows->world, rows->entities[i], Position);
    }
}

void TriggerOnAdd_2_systems_w_table_creation() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TRIGGER(world, SystemA, EcsOnAdd, Position, NULL);
    ECS_TRIGGER(world, SystemB, EcsOnAdd, Position, NULL);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);    

    ecs_fini(world);
}

void NewWithPosition(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, Position, 1);

    ecs_entity_t e = ecs_new(rows->world, Position);
    test_assert(e != 0); 
}

void TriggerOnAdd_2_systems_w_table_creation_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TRIGGER(world, SystemA, EcsOnAdd, Position, NULL);
    ECS_TRIGGER(world, SystemB, EcsOnAdd, Position, NULL);
    ECS_SYSTEM(world, NewWithPosition, EcsOnUpdate, .Position);

    ecs_progress(world, 1);   

    ecs_fini(world);
}

static
void TestContext(ecs_rows_t *rows) {
    void *world_ctx = ecs_get_context(rows->world);
    test_assert(world_ctx == rows->param);
    int32_t *param = rows->param;
    (*param) ++;
}

void TriggerOnAdd_sys_context() {
    ecs_world_t *world = ecs_init();
    int32_t param = 0;

    ECS_COMPONENT(world, Position);

    ECS_TRIGGER(world, TestContext, EcsOnAdd, Position, NULL);

    ecs_set(world, TestContext, EcsContext, {&param});

    const EcsContext *ctx = ecs_get_ptr(world, TestContext, EcsContext);
    test_assert(ctx != NULL);
    test_assert(ctx->ctx == &param);

    ecs_fini(world);
}

void TriggerOnAdd_get_sys_context_from_param() {
    ecs_world_t *world = ecs_init();
    int32_t param = 0;

    ECS_COMPONENT(world, Position);

    ECS_TRIGGER(world, TestContext, EcsOnAdd, Position, &param);

    /* Set world context so system can compare if pointer is correct */
    ecs_set_context(world, &param);
    ecs_set(world, TestContext, EcsContext, {&param});

    /* Trigger system */
    ecs_new(world, Position);

    test_int(param, 1);

    ecs_fini(world);
}

void TriggerOnAdd_remove_added_component_in_on_add_w_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_TRIGGER(world, Remove_from_current, EcsOnAdd, Position, NULL);

    IterData ctx = {.component = ecs_entity(Position)};
    ecs_set_context(world, &ctx);

    ecs_entity_t e_1 = ecs_new(world, Velocity);
    ecs_entity_t e_2 = ecs_new(world, Velocity);
    ecs_entity_t e_3 = ecs_new(world, Velocity);

    e_1 = ecs_set(world, e_1, Position, {10, 20});
    e_2 = ecs_set(world, e_2, Position, {11, 21});
    e_3 = ecs_set(world, e_3, Position, {12, 22});

    test_assert( !ecs_has(world, e_1, Position));
    test_assert( !ecs_has(world, e_2, Position));
    test_assert( !ecs_has(world, e_3, Position));

    test_assert( ecs_has(world, e_1, Velocity));
    test_assert( ecs_has(world, e_2, Velocity));
    test_assert( ecs_has(world, e_3, Velocity));

    ecs_fini(world);
}

void Add_3_to_current(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = 0; i < rows->count; i ++) {
        if (ctx->component_3) {
            ecs_add_entity(rows->world, rows->entities[i], ctx->component_3);
        }
        ctx->entity_count ++;
    }
}

void TriggerOnAdd_on_add_in_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_TRIGGER(world, Add_to_current, EcsOnAdd, Position, NULL);
    ECS_TRIGGER(world, Add_3_to_current, EcsOnAdd, Velocity, NULL);

    IterData ctx = {.component = ecs_entity(Velocity), .component_3 = ecs_entity(Mass)};
    ecs_set_context(world, &ctx);

    ecs_entity_t e_1 = ecs_new(world, Position);
    ecs_entity_t e_2 = ecs_new(world, Position);
    ecs_entity_t e_3 = ecs_new(world, Position);

    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_3, Position));

    test_assert( ecs_has(world, e_1, Velocity));
    test_assert( ecs_has(world, e_2, Velocity));
    test_assert( ecs_has(world, e_3, Velocity));

    test_assert( ecs_has(world, e_1, Mass));
    test_assert( ecs_has(world, e_2, Mass));
    test_assert( ecs_has(world, e_3, Mass));

    ecs_fini(world);
}

static bool dummy_called = false;

static
void Dummy(ecs_rows_t *rows) {
    dummy_called = true;
}

void TriggerOnAdd_on_remove_in_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_TRIGGER(world, Remove_from_current, EcsOnAdd, Position, NULL);
    ECS_TRIGGER(world, Dummy, EcsOnRemove, Velocity, NULL);

    IterData ctx = {.component = ecs_entity(Velocity)};
    ecs_set_context(world, &ctx);

    ecs_entity_t e_1 = ecs_new(world, Type);
    ecs_entity_t e_2 = ecs_new(world, Type);
    ecs_entity_t e_3 = ecs_new(world, Type);

    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_3, Position));

    test_assert( !ecs_has(world, e_1, Velocity));
    test_assert( !ecs_has(world, e_2, Velocity));
    test_assert( !ecs_has(world, e_3, Velocity));

    test_assert(dummy_called);

    ecs_fini(world);
}

void TriggerOnAdd_on_set_in_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Rotation);
    ECS_COMPONENT(world, Mass);

    ECS_TRIGGER(world, Set_current, EcsOnAdd, Position, NULL);
    ECS_SYSTEM(world, Add_3_to_current, EcsOnSet, Rotation);

    IterData ctx = {.component = ecs_entity(Rotation), .component_3 = ecs_entity(Mass)};
    ecs_set_context(world, &ctx);

    ecs_entity_t e_1 = ecs_new(world, Position);
    ecs_entity_t e_2 = ecs_new(world, Position);
    ecs_entity_t e_3 = ecs_new(world, Position);

    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_3, Position));

    test_assert( ecs_has(world, e_1, Rotation));
    test_assert( ecs_has(world, e_2, Rotation));
    test_assert( ecs_has(world, e_3, Rotation));

    test_assert( ecs_has(world, e_1, Mass));
    test_assert( ecs_has(world, e_2, Mass));
    test_assert( ecs_has(world, e_3, Mass));

    const Rotation *r = ecs_get_ptr(world, e_1, Rotation);
    test_assert(r != NULL);
    test_int(*r, 10 + e_1);

    r = ecs_get_ptr(world, e_2, Rotation);
    test_assert(r != NULL);
    test_int(*r, 10 + e_2);

    r = ecs_get_ptr(world, e_3, Rotation);
    test_assert(r != NULL);
    test_int(*r, 10 + e_3);

    ecs_fini(world);
}

void TriggerOnAdd_on_add_in_on_update() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_SYSTEM(world, Add_to_current, EcsOnUpdate, Position);
    ECS_TRIGGER(world, Add_3_to_current, EcsOnAdd, Velocity, NULL);

    IterData ctx = {.component = ecs_entity(Velocity), .component_3 = ecs_entity(Mass)};
    ecs_set_context(world, &ctx);

    ecs_entity_t e_1 = ecs_new(world, Position);
    ecs_entity_t e_2 = ecs_new(world, Position);
    ecs_entity_t e_3 = ecs_new(world, Position);

    ecs_progress(world, 1);

    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_3, Position));

    test_assert( ecs_has(world, e_1, Velocity));
    test_assert( ecs_has(world, e_2, Velocity));
    test_assert( ecs_has(world, e_3, Velocity));

    test_assert( ecs_has(world, e_1, Mass));
    test_assert( ecs_has(world, e_2, Mass));
    test_assert( ecs_has(world, e_3, Mass));

    ecs_fini(world);
}
