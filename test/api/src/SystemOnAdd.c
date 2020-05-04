#include <api.h>

static
void install_test_abort() {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.abort = test_abort;
    ecs_os_set_api(&os_api);
}

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

void SystemOnAdd_new_match_1_of_1() {
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

void SystemOnAdd_new_match_1_of_2() {
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

void SystemOnAdd_new_no_match_1() {
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

void SystemOnAdd_add_match_1_of_1() {
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

void SystemOnAdd_add_match_1_of_2() {
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

void SystemOnAdd_add_no_match_1() {
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

void SystemOnAdd_set_match_1_of_1() {
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

void SystemOnAdd_set_no_match_1() {
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

void SystemOnAdd_clone_match_1_of_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TRIGGER(world, Init, EcsOnAdd, Position, NULL);

    ecs_entity_t e_1 = ecs_new(world, Position);
    test_assert(e_1 != 0);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e_2 = ecs_copy(world, 0, e_1, false);

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

void SystemOnAdd_clone_match_1_of_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_TRIGGER(world, Init, EcsOnAdd, Position, NULL);

    ecs_entity_t e_1 = ecs_new(world, Type);
    test_assert(e_1 != 0);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e_2 = ecs_copy(world, 0, e_1, false);

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

void SystemOnAdd_add_again_1() {
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

void SystemOnAdd_set_again_1() {
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

void SystemOnAdd_add_again_2() {
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

void SystemOnAdd_new_w_count_match_1_of_1() {
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

    probe_system(rows);

    int i;
    for (i = 0; i < rows->count; i ++) {
        p[i].x = 1;
        p[i].y = 2;
        ecs_add_type(rows->world, rows->entities[i], v);
    }
}

void SystemOnAdd_override_after_add_in_on_add() {
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
    test_int(ctx.column_count, 2);

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

void SystemOnAdd_set_after_add_in_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TRIGGER(world, AddVelocity, EcsOnAdd, Position, &ecs_entity(Velocity));
    ECS_TRIGGER(world, OnSetPosition, EcsOnSet, Position, NULL);

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
    test_int(ctx.system, Init);
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

void SystemOnAdd_add_again_in_progress() {
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

void SystemOnAdd_add_in_progress_before_system_def() {
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

static int is_invoked;

static
void IsInvoked(ecs_rows_t *rows) {
    is_invoked ++;
}

void SystemOnAdd_disabled_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TRIGGER(world, IsInvoked, EcsOnAdd, Position, 0);

    ecs_enable(world, IsInvoked, false);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    test_int(is_invoked, 0);

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

void SystemOnAdd_2_systems_w_table_creation() {
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

void SystemOnAdd_2_systems_w_table_creation_in_progress() {
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
    uint32_t *param = rows->param;
    (*param) ++;
}


void SystemOnAdd_sys_context() {
    ecs_world_t *world = ecs_init();
    uint32_t param = 0;

    ECS_COMPONENT(world, Position);

    ECS_TRIGGER(world, TestContext, EcsOnAdd, Position, NULL);

    ecs_set_system_context(world, TestContext, &param);

    test_assert(ecs_get_system_context(world, TestContext) == &param);

    ecs_fini(world);
}

void SystemOnAdd_get_sys_context_from_param() {
    ecs_world_t *world = ecs_init();
    uint32_t param = 0;

    ECS_COMPONENT(world, Position);

    ECS_TRIGGER(world, TestContext, EcsOnAdd, Position, NULL);

    /* Set world context so system can compare if pointer is correct */
    ecs_set_context(world, &param);
    ecs_set_system_context(world, TestContext, &param);

    /* Trigger system */
    ecs_new(world, Position);

    test_int(param, 1);

    ecs_fini(world);
}
