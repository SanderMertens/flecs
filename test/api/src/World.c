#include <api.h>

static
void install_test_abort() {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.abort = test_abort;
    ecs_os_set_api(&os_api);
}

void Move(ecs_rows_t *rows) {
    ProbeSystem(rows);

    int row;
    for (row = 0; row < rows->count; row ++) {
        Position *p = ecs_field(rows, Position, 1, row);
        Velocity *v = ecs_field(rows, Velocity, 2, row);
        p->x += v->x * rows->delta_time;
        p->y += v->y * rows->delta_time;
    }
}

void World_progress_w_0() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e_1, Position, Velocity);

    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_set(world, e_1, Position, {0, 0});
    ecs_set(world, e_1, Velocity, {1, 2});

    ecs_progress(world, 0);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Move);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.c[0][1], ecs_entity(Velocity));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.s[0][1], 0);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_assert(p->x != 0);
    test_assert(p->y != 0);

    ecs_fini(world);
}

void World_progress_w_t() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e_1, Position, Velocity);

    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_set(world, e_1, Position, {0, 0});
    ecs_set(world, e_1, Velocity, {1, 2});

    ecs_progress(world, 2);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Move);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.c[0][1], ecs_entity(Velocity));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.s[0][1], 0);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 4);

    ecs_fini(world);
}

void World_entity_range_offset() {
    ecs_world_t *world = ecs_init();

    ecs_set_entity_range(world, 50, 0);

    ecs_entity_t e = ecs_new(world, 0);
    test_int(e, 50);

    ecs_fini(world);
}

void World_entity_range_offset_out_of_range() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_set_entity_range(world, 50, 0);

    test_expect_abort();

    ecs_add(world, 20, Position);

    ecs_fini(world);
}

void World_entity_range_limit_out_of_range() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_set_entity_range(world, 0, 50);

    test_expect_abort();

    ecs_add(world, 60, Position);

    ecs_fini(world);
}

void World_entity_range_out_of_range_check_disabled() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_enable_range_check(world, false);
    ecs_set_entity_range(world, 50, 100);

    /* Validate that range is being used when issuing new ids */
    ecs_entity_t e = ecs_new(world, 0);
    test_int(e, 50);

    /* Validate that application does not abort when changing out of range */
    ecs_entity_t e2 = ecs_set(world, 49, Position, {10, 20});
    test_int(e2, 49);
    test_assert( ecs_has(world, e2, Position));
    
    Position *p = ecs_get_ptr(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void AddToExisting(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, Velocity, 2);

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_add(rows->world, rows->entities[i], Velocity);
        test_assert( ecs_has(rows->world, rows->entities[i], Velocity));
    }
}

void World_entity_range_add_existing_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, AddToExisting, EcsOnUpdate, Position, .Velocity);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(e < 500);

    ecs_set_entity_range(world, 500, 1000);

    ecs_progress(world, 1);

    ecs_fini(world);
}

void World_entity_range_add_in_range_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, AddToExisting, EcsOnUpdate, Position, .Velocity);

    ecs_set_entity_range(world, 500, 1000);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e == 500);

    ecs_progress(world, 1);

    ecs_fini(world);
}

void AddOutOfRange(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, Velocity, 2);

    int i;
    for (i = 0; i < rows->count; i ++) {
        test_expect_abort();
        ecs_add(rows->world, 1001, Velocity);
    }
}

void World_entity_range_add_out_of_range_in_progress() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, AddOutOfRange, EcsOnUpdate, Position, .Velocity);

    ecs_set_entity_range(world, 500, 1000);

    /* Dummy entity to invoke the system */
    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e == 500);

    ecs_progress(world, 1);

    ecs_fini(world);
}

void World_get_tick() {
    ecs_world_t *world = ecs_init();

    test_int(ecs_get_tick(world), 0);

    ecs_progress(world, 1);

    test_int(ecs_get_tick(world), 1);

    ecs_progress(world, 1);

    test_int(ecs_get_tick(world), 2);

    ecs_fini(world);
}

static int32_t malloc_count;

static
void *test_malloc(size_t size) {
    malloc_count ++;
    return malloc(size);
}

static
void *test_calloc(size_t size, size_t n) {
    malloc_count ++;
    return calloc(size, n);
}

static
void *test_realloc(void *old_ptr, size_t size) {
    malloc_count ++;
    return realloc(old_ptr, size);
}

void World_dim() {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.malloc = test_malloc;
    os_api.calloc = test_calloc;
    os_api.realloc = test_realloc;
    ecs_os_set_api(&os_api);    

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    /* Create single entity so that the table exists. This makes the allocation
     * counts more predictable, as new_w_count won't trigger table creation */
    ecs_new(world, Position);

    ecs_dim(world, 1100);

    malloc_count = 0;

    ecs_new_w_count(world, Position, 500);

    test_int(malloc_count, 2);

    malloc_count = 0;

    ecs_new_w_count(world, Position, 500);

    test_int(malloc_count, 2);

    ecs_fini(world);
}

void World_dim_type() {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.malloc = test_malloc;
    os_api.calloc = test_calloc;
    os_api.realloc = test_realloc;
    ecs_os_set_api(&os_api);    

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_dim_type(world, Position, 1000);

    malloc_count = 0;

    ecs_new_w_count(world, Position, 500);

    test_int(malloc_count, 2);

    malloc_count = 0;

    ecs_new_w_count(world, Position, 400);

    test_int(malloc_count, 2);

    ecs_fini(world);
}

void World_dim_dim_type() {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.malloc = test_malloc;
    os_api.calloc = test_calloc;
    os_api.realloc = test_realloc;
    ecs_os_set_api(&os_api);    

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_dim(world, 1000);
    ecs_dim_type(world, Position, 1000);

    malloc_count = 0;

    ecs_new_w_count(world, Position, 500);

    test_int(malloc_count, 0);

    malloc_count = 0;

    ecs_new_w_count(world, Position, 400);

    test_int(malloc_count, 0);

    ecs_fini(world);
}

static
void TOnLoad(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    int i;
    for (i = 0; i < rows->count; i ++) {
        test_int(p[i].x, 0);
        p[i].x ++;
    }
}

static
void TPostLoad(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    int i;
    for (i = 0; i < rows->count; i ++) {
        test_int(p[i].x, 1);
        p[i].x ++;
    }
}

static
void TPreUpdate(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    int i;
    for (i = 0; i < rows->count; i ++) {
        test_int(p[i].x, 2);
        p[i].x ++;
    }
}

static
void TOnUpdate(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    int i;
    for (i = 0; i < rows->count; i ++) {
        test_int(p[i].x, 3);
        p[i].x ++;
    }
}

static
void TOnValidate(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    int i;
    for (i = 0; i < rows->count; i ++) {
        test_int(p[i].x, 4);
        p[i].x ++;
    }
}

static
void TPostUpdate(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    int i;
    for (i = 0; i < rows->count; i ++) {
        test_int(p[i].x, 5);
        p[i].x ++;
    }
}

static
void TPreStore(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    int i;
    for (i = 0; i < rows->count; i ++) {
        test_int(p[i].x, 6);
        p[i].x ++;
    }
}

static
void TOnStore(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    int i;
    for (i = 0; i < rows->count; i ++) {
        test_int(p[i].x, 7);
        p[i].x ++;
    }
}

static
void TManual(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    int i;
    for (i = 0; i < rows->count; i ++) {
        test_int(p[i].x, 8);
        p[i].x ++;
    }
}

void World_phases() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, TOnLoad, EcsOnLoad, Position);
    ECS_SYSTEM(world, TPostLoad, EcsPostLoad, Position);
    ECS_SYSTEM(world, TPreUpdate, EcsPreUpdate, Position);
    ECS_SYSTEM(world, TOnUpdate, EcsOnUpdate, Position);
    ECS_SYSTEM(world, TOnValidate, EcsOnValidate, Position);
    ECS_SYSTEM(world, TPostUpdate, EcsPostUpdate, Position);
    ECS_SYSTEM(world, TPreStore, EcsPreStore, Position);
    ECS_SYSTEM(world, TOnStore, EcsOnStore, Position);
    ECS_SYSTEM(world, TManual, EcsManual, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_set(world, e, Position, {0, 0});

    ecs_progress(world, 1);

    Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 8);

    ecs_run(world, TManual, 0, NULL);

    test_int(p->x, 9);

    ecs_fini(world);
}

void World_phases_match_in_create() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_set(world, e, Position, {0, 0});

    ECS_SYSTEM(world, TOnLoad, EcsOnLoad, Position);
    ECS_SYSTEM(world, TPostLoad, EcsPostLoad, Position);
    ECS_SYSTEM(world, TPreUpdate, EcsPreUpdate, Position);
    ECS_SYSTEM(world, TOnUpdate, EcsOnUpdate, Position);
    ECS_SYSTEM(world, TOnValidate, EcsOnValidate, Position);
    ECS_SYSTEM(world, TPostUpdate, EcsPostUpdate, Position);
    ECS_SYSTEM(world, TPreStore, EcsPreStore, Position);
    ECS_SYSTEM(world, TOnStore, EcsOnStore, Position);
    ECS_SYSTEM(world, TManual, EcsManual, Position);

    ecs_progress(world, 1);

    Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 8);

    ecs_run(world, TManual, 0, NULL);

    test_int(p->x, 9);

    ecs_fini(world);
}

static
void TMergeOnLoad(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    ECS_COLUMN_COMPONENT(rows, Position, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        test_int(p[i].x, 0);
        ecs_set(rows->world, rows->entities[i], Position, {p[i].x + 1, 0});
    }
}

static
void TMergePostLoad(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    ECS_COLUMN_COMPONENT(rows, Position, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        test_int(p[i].x, 1);
        ecs_set(rows->world, rows->entities[i], Position, {p[i].x + 1, 0});
    }
}

static
void TMergePreUpdate(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    ECS_COLUMN_COMPONENT(rows, Position, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        test_int(p[i].x, 2);
        ecs_set(rows->world, rows->entities[i], Position, {p[i].x + 1, 0});
    }
}

static
void TMergeOnUpdate(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    ECS_COLUMN_COMPONENT(rows, Position, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        test_int(p[i].x, 3);
        ecs_set(rows->world, rows->entities[i], Position, {p[i].x + 1, 0});
    }
}

static
void TMergeOnValidate(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    ECS_COLUMN_COMPONENT(rows, Position, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        test_int(p[i].x, 4);
        ecs_set(rows->world, rows->entities[i], Position, {p[i].x + 1, 0});
    }
}

static
void TMergePostUpdate(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    ECS_COLUMN_COMPONENT(rows, Position, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        test_int(p[i].x, 5);
        ecs_set(rows->world, rows->entities[i], Position, {p[i].x + 1, 0});
    }
}

static
void TMergePreStore(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    ECS_COLUMN_COMPONENT(rows, Position, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        test_int(p[i].x, 6);
        ecs_set(rows->world, rows->entities[i], Position, {p[i].x + 1, 0});
    }
}

static
void TMergeOnStore(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    ECS_COLUMN_COMPONENT(rows, Position, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        test_int(p[i].x, 7);
        ecs_set(rows->world, rows->entities[i], Position, {p[i].x + 1, 0});
    }
}

static
void TMergeManual(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    ECS_COLUMN_COMPONENT(rows, Position, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        test_int(p[i].x, 8);
        ecs_set(rows->world, rows->entities[i], Position, {p[i].x + 1, 0});
    }
}

void World_phases_w_merging() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, TMergeOnLoad, EcsOnLoad, Position);
    ECS_SYSTEM(world, TMergePostLoad, EcsPostLoad, Position);
    ECS_SYSTEM(world, TMergePreUpdate, EcsPreUpdate, Position);
    ECS_SYSTEM(world, TMergeOnUpdate, EcsOnUpdate, Position);
    ECS_SYSTEM(world, TMergeOnValidate, EcsOnValidate, Position);
    ECS_SYSTEM(world, TMergePostUpdate, EcsPostUpdate, Position);
    ECS_SYSTEM(world, TMergePreStore, EcsPreStore, Position);
    ECS_SYSTEM(world, TMergeOnStore, EcsOnStore, Position);
    ECS_SYSTEM(world, TMergeManual, EcsManual, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_set(world, e, Position, {0, 0});

    ecs_progress(world, 1);

    Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 8);

    ecs_run(world, TMergeManual, 0, NULL);

    test_int(p->x, 9);

    ecs_fini(world);
}

static
void TimeCheck(ecs_rows_t *rows) {
    test_assert(rows->delta_time > 0);
}

void World_measure_time() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, TimeCheck, EcsOnLoad, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    int i = 0;
    for (i = 0; i < 1000; i ++) {
        ecs_progress(world, 0);
    }

    ecs_fini(world);
}

void World_control_fps() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, TimeCheck, EcsOnLoad, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    double start, now = 0;
    ecs_set_target_fps(world, 60);

    /* Run for one second */
    int count = 0;
    do {    
        ecs_progress(world, 0);
        if (!count) {
            start = ecs_get_delta_time(world);
        }

        now += ecs_get_delta_time(world);
        count ++;
    } while ((now - start) < 1.0);

    test_assert(count >= 50);
    test_assert(count < 65);

    ecs_fini(world);
}

static
void busy_wait(float wait_time) {
    ecs_time_t start, t;
    ecs_os_get_time(&start);

    do {
        t = start;
    } while (ecs_time_measure(&t) < wait_time);
}

static
void BusySystem(ecs_rows_t *rows) {
    /* Spend 14msec doing something */
    busy_wait(0.014);
}

void World_control_fps_busy_system() {
    ecs_world_t *world = ecs_init();

    ECS_SYSTEM(world, BusySystem, EcsOnUpdate, 0);

    double start, now = 0;
    ecs_set_target_fps(world, 60);

    /* Run for one second */
    int count = 0;
    do {    
        ecs_progress(world, 0);
        if (!count) {
            start = ecs_get_delta_time(world);
        }

        now += ecs_get_delta_time(world);
        count ++;
    } while ((now - start) < 1.0);

    /* FPS control relies on sleep, which relies on the OS scheduler. Therefore
     * pick a wide enough range to avoid tests failing at random. */
    test_assert(count >= 50);
    test_assert(count < 65);

    ecs_fini(world);
}

void World_control_fps_busy_app() {
    ecs_world_t *world = ecs_init();

    double start, now = 0;
    ecs_set_target_fps(world, 60);

    /* Run for one second */
    int count = 0;
    do {    
        ecs_progress(world, 0);
        if (!count) {
            start = ecs_get_delta_time(world);
        }

        now += ecs_get_delta_time(world);
        count ++;

        busy_wait(0.014);
    } while ((now - start) < 1.0);

    /* FPS control relies on sleep, which relies on the OS scheduler. Therefore
     * pick a wide enough range to avoid tests failing at random. */
    test_assert(count >= 50);
    test_assert(count < 65);

    ecs_fini(world);
}

static
void RandomSystem(ecs_rows_t *rows) {
    /* wait at most 16msec */
    float rnd_time = ((float)rand() / (float)RAND_MAX) * 0.016;
    busy_wait(rnd_time);
}

void World_control_fps_random_system() {
    ecs_world_t *world = ecs_init();

    ECS_SYSTEM(world, RandomSystem, EcsOnUpdate, 0);

    double start, now = 0;
    ecs_set_target_fps(world, 60);

    /* Run for one second */
    int count = 0;
    do {    
        ecs_progress(world, 0);
        if (!count) {
            start = ecs_get_delta_time(world);
        }

        now += ecs_get_delta_time(world);
        count ++;
    } while ((now - start) < 1.0);

    /* FPS control relies on sleep, which relies on the OS scheduler. Therefore
     * pick a wide enough range to avoid tests failing at random. */
    test_assert(count >= 50);
    test_assert(count < 65);

    ecs_fini(world);
}

void World_control_fps_random_app() {
    ecs_world_t *world = ecs_init();

    double start, now = 0;
    ecs_set_target_fps(world, 60);

    /* Run for one second */
    int count = 0;
    do {    
        ecs_progress(world, 0);
        if (!count) {
            start = ecs_get_delta_time(world);
        }

        now += ecs_get_delta_time(world);
        count ++;

        float rnd_time = ((float)rand() / (float)RAND_MAX) * 0.016;
        busy_wait(rnd_time);
    } while ((now - start) < 1.0);

    /* FPS control relies on sleep, which relies on the OS scheduler. Therefore
     * pick a wide enough range to avoid tests failing at random. */
    test_assert(count >= 55);
    test_assert(count < 65);

    ecs_fini(world);
}

static
void Dummy(ecs_rows_t *rows) { }

void World_world_stats() {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsStats, 0);

    ECS_COMPONENT(world, Velocity);

    /* Make sure that stats are collected by requiring EcsWorldStats */
    ecs_new_system(world, "CollectWorldStats", EcsManual, "[in] EcsWorldStats", NULL);

    EcsWorldStats stats = {0};

    ecs_progress(world, 1);
    ecs_progress(world, 1);
    ecs_progress(world, 1);

    stats = ecs_get(world, EcsWorld, EcsWorldStats);

    int init_system_count = stats.col_systems_count;
    int init_table_count = stats.tables_count;
    int init_component_count = stats.components_count;
    int init_entity_count = stats.entities_count;

    ecs_progress(world, 1);

    stats = ecs_get(world, EcsWorld, EcsWorldStats);

    test_int(stats.col_systems_count - init_system_count, 0);
    test_int(stats.tables_count - init_table_count, 0);
    test_int(stats.components_count - init_component_count, 0);
    test_int(stats.entities_count - init_entity_count, 0);

    ECS_COMPONENT(world, Position);

    ecs_progress(world, 1);
    stats = ecs_get(world, EcsWorld, EcsWorldStats);

    test_int(stats.col_systems_count - init_system_count, 0);
    test_int(stats.tables_count - init_table_count, 0);
    test_int(stats.components_count - init_component_count, 1);
    test_int(stats.entities_count - init_entity_count, 1);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_progress(world, 1);
    stats = ecs_get(world, EcsWorld, EcsWorldStats);

    test_int(stats.col_systems_count - init_system_count, 0);
    test_int(stats.tables_count - init_table_count, 1);
    test_int(stats.components_count - init_component_count, 1);
    test_int(stats.entities_count - init_entity_count, 2);

    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);

    ecs_progress(world, 1);
    stats = ecs_get(world, EcsWorld, EcsWorldStats);

    test_int(stats.col_systems_count - init_system_count, 1);
    test_int(stats.tables_count - init_table_count, 1);
    test_int(stats.components_count - init_component_count, 1);
    test_int(stats.entities_count - init_entity_count, 3);

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position);

    ecs_progress(world, 1);
    stats = ecs_get(world, EcsWorld, EcsWorldStats);

    test_int(stats.col_systems_count - init_system_count, 2);
    test_int(stats.tables_count - init_table_count, 1);
    test_int(stats.components_count - init_component_count, 1);
    test_int(stats.entities_count - init_entity_count, 4);

    ECS_TYPE(world, Type, Position);

    ecs_progress(world, 1);
    stats = ecs_get(world, EcsWorld, EcsWorldStats);

    test_int(stats.col_systems_count - init_system_count, 2);
    test_int(stats.tables_count - init_table_count, 2);
    test_int(stats.components_count - init_component_count, 1);
    test_int(stats.entities_count - init_entity_count, 5);

    ECS_TYPE(world, Feature, Move, Dummy);

    ecs_progress(world, 1);
    stats = ecs_get(world, EcsWorld, EcsWorldStats);

    test_int(stats.col_systems_count - init_system_count, 2);
    test_int(stats.tables_count - init_table_count, 3);
    test_int(stats.components_count - init_component_count, 1);
    test_int(stats.entities_count - init_entity_count, 6);

    ecs_progress(world, 0);
    stats = ecs_get(world, EcsWorld, EcsWorldStats);

    test_assert(stats.system_seconds_total != 0);
    test_assert(stats.frame_seconds_total != 0);

    ecs_fini(world);
}


void World_quit() {
    ecs_world_t *world = ecs_init();

    uint32_t count = 0;

    while (ecs_progress(world, 0)) {
        test_int(count, 0);
        ecs_quit(world);
        count ++;
    }

    ecs_fini(world);
}

void World_get_delta_time() {
    ecs_world_t *world = ecs_init();

    test_int(ecs_get_delta_time(world), 0);

    ecs_progress(world, 1.0);

    test_flt(ecs_get_delta_time(world), 1.0);

    ecs_fini(world);
}

void World_get_delta_time_auto() {
    ecs_world_t *world = ecs_init();

    test_int(ecs_get_delta_time(world), 0);

    ecs_progress(world, 0);

    test_assert(ecs_get_delta_time(world) != 0);

    ecs_fini(world);
}

void World_recreate_world() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_fini(world) == 0);

    world = ecs_init();

    test_assert(ecs_fini(world) == 0);
}

void World_init_w_args_set_threads() {
    ecs_world_t *world = ecs_init_w_args(3, ((char*[]){
        "test",
        "--threads", "4",
        NULL
    }));

    test_assert(world != NULL);

    test_int(ecs_get_threads(world), 4);

    ecs_fini(world);
}

void World_init_w_args_set_fps() {
    ecs_world_t *world = ecs_init_w_args(3, ((char*[]){
        "test",
        "--fps", "60",
        NULL
    }));

    test_assert(world != NULL);

    test_int(ecs_get_target_fps(world), 60);

    ecs_fini(world);
}

void World_init_w_args_enable_dbg() {
    ecs_world_t *world = ecs_init_w_args(2, ((char*[]){
        "test",
        "--debug",
        NULL
    }));

    test_assert(world != NULL);

    test_assert(ecs_os_dbg_enabled() == true);

    ecs_fini(world);
}


void World_no_threading() {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.mutex_new = NULL;
    ecs_os_set_api(&os_api);

    ecs_world_t *world = ecs_init();
    test_assert(world != NULL);
    ecs_fini(world);
}

void World_no_time() {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.get_time = NULL;
    ecs_os_set_api(&os_api);

    ecs_world_t *world = ecs_init();
    test_assert(world != NULL);
    ecs_fini(world);
}

void World_is_entity_enabled() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);

    test_assert( ecs_is_enabled(world, e) == true);

    ecs_fini(world);
}
