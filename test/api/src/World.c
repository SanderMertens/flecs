#include <api.h>
#include <stdlib.h>

void World_setup() {
    ecs_log_set_level(-3);
}

static
void Move(ecs_iter_t *it) {
    Position *pos = ecs_term(it, Position, 1);
    Velocity *vel = ecs_term(it, Velocity, 2);
    probe_iter(it);

    int row;
    for (row = 0; row < it->count; row ++) {
        Position *p = &pos[row];
        Velocity *v = &vel[row];
        p->x += v->x * it->delta_time;
        p->y += v->y * it->delta_time;
    }
}

void World_progress_w_0() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e1, Position, Velocity);

    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_set(world, e1, Position, {0, 0});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_progress(world, 0);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Move);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_assert(p->x != 0);
    test_assert(p->y != 0);

    ecs_fini(world);
}

void World_progress_w_t() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e1, Position, Velocity);

    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_set(world, e1, Position, {0, 0});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_progress(world, 2);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Move);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 4);

    ecs_fini(world);
}

void World_entity_range_offset() {
    ecs_world_t *world = ecs_init();

    ecs_set_entity_range(world, 5000, 0);

    ecs_entity_t e = ecs_new(world, 0);
    test_int(e, 5000);

    ecs_fini(world);
}

void World_entity_range_offset_out_of_range() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_enable_range_check(world, true);
    ecs_set_entity_range(world, 2000, 0);

    test_expect_abort();

    ecs_add(world, 1500, Position);

    ecs_fini(world);
}

void World_entity_range_limit_out_of_range() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_enable_range_check(world, true);
    ecs_set_entity_range(world, 0, 2000);

    test_expect_abort();

    ecs_add(world, 2500, Position);

    ecs_fini(world);
}

void World_entity_range_out_of_range_check_disabled() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_enable_range_check(world, false);
    ecs_set_entity_range(world, 5000, 10000);

    /* Validate that range is being used when issuing new ids */
    ecs_entity_t e = ecs_new(world, 0);
    test_int(e, 5000);

    /* Validate that application does not abort when changing out of range */
    ecs_entity_t e2 = ecs_set(world, 4999, Position, {10, 20});
    test_int(e2, 4999);
    test_assert( ecs_has(world, e2, Position));
    
    const Position *p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void World_entity_range_check_after_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_enable_range_check(world, true);
    ecs_set_entity_range(world, 5000, 10000);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);
    test_assert(e == 5000);

    ecs_delete(world, e);

    e = ecs_new(world, 0);
    test_assert(e != 0);
    test_assert((uint32_t)e == 5000);

    ecs_fini(world);
}

void World_entity_range_add_existing_staged() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(e < 1000);

    ecs_set_entity_range(world, 1000, 1500);

    ecs_readonly_begin(world);
    ecs_world_t *stage = ecs_get_stage(world, 0);
    ecs_add(stage, e, Velocity);
    ecs_readonly_end(world);

    ecs_fini(world);
}

void World_entity_range_add_in_range_staged() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_set_entity_range(world, 500, 1000);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e == 500);

    ecs_readonly_begin(world);
    ecs_world_t *stage = ecs_get_stage(world, 0);
    ecs_add(stage, e, Velocity);
    ecs_readonly_end(world);

    ecs_fini(world);
}

void AddOutOfRange(ecs_iter_t *it) {
    ecs_id_t ecs_id(Velocity) = ecs_term_id(it, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        test_expect_abort();
        ecs_add(it->world, 1001, Velocity);
    }
}

void World_entity_range_add_out_of_range_staged() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_enable_range_check(world, true);
    ecs_set_entity_range(world, 500, 1000);

    /* Dummy entity to invoke the system */
    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e == 500);

    ecs_readonly_begin(world);
    ecs_world_t *stage = ecs_get_stage(world, 0);
    ecs_add(stage, e, Velocity);
    ecs_readonly_end(world);

    ecs_fini(world);
}

void World_get_tick() {
    ecs_world_t *world = ecs_init();

    const ecs_world_info_t *stats = ecs_get_world_info(world);
    test_int(stats->frame_count_total, 0);

    ecs_progress(world, 1);

    test_int(stats->frame_count_total, 1);

    ecs_progress(world, 1);

    test_int(stats->frame_count_total, 2);

    ecs_fini(world);
}

static int32_t malloc_count;

static
void *test_malloc(ecs_size_t size) {
    malloc_count ++;
    return malloc(size);
}

static
void *test_calloc(ecs_size_t size) {
    malloc_count ++;
    return calloc(size, 1);
}

static
void *test_realloc(void *old_ptr, ecs_size_t size) {
    malloc_count ++;
    return realloc(old_ptr, size);
}

void World_dim() {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.malloc_ = test_malloc;
    os_api.calloc_ = test_calloc;
    os_api.realloc_ = test_realloc;
    ecs_os_set_api(&os_api);

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    /* Create single entity so that the table exists. This makes the allocation
     * counts more predictable, as new_w_count won't trigger table creation */
    ecs_new(world, Position);

    ecs_dim(world, 1100);

    malloc_count = 0;

    ecs_bulk_new(world, Position, 500);

    test_int(malloc_count, 3);

    malloc_count = 0;

    ecs_bulk_new(world, Position, 500);

    test_int(malloc_count, 3);

    ecs_fini(world);
}

static
void TOnLoad(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    int i;
    for (i = 0; i < it->count; i ++) {
        test_int(p[i].x, 0);
        p[i].x ++;
    }
}

static
void TPostLoad(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    int i;
    for (i = 0; i < it->count; i ++) {
        test_int(p[i].x, 1);
        p[i].x ++;
    }
}

static
void TPreUpdate(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    int i;
    for (i = 0; i < it->count; i ++) {
        test_int(p[i].x, 2);
        p[i].x ++;
    }
}

static
void TOnUpdate(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    int i;
    for (i = 0; i < it->count; i ++) {
        test_int(p[i].x, 3);
        p[i].x ++;
    }
}

static
void TOnValidate(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    int i;
    for (i = 0; i < it->count; i ++) {
        test_int(p[i].x, 4);
        p[i].x ++;
    }
}

static
void TPostUpdate(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    int i;
    for (i = 0; i < it->count; i ++) {
        test_int(p[i].x, 5);
        p[i].x ++;
    }
}

static
void TPreStore(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    int i;
    for (i = 0; i < it->count; i ++) {
        test_int(p[i].x, 6);
        p[i].x ++;
    }
}

static
void TOnStore(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    int i;
    for (i = 0; i < it->count; i ++) {
        test_int(p[i].x, 7);
        p[i].x ++;
    }
}

static
void TManual(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    int i;
    for (i = 0; i < it->count; i ++) {
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
    ECS_SYSTEM(world, TManual, 0, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_set(world, e, Position, {0, 0});

    ecs_progress(world, 1);

    const Position *p = ecs_get(world, e, Position);
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
    ECS_SYSTEM(world, TManual, 0, Position);

    ecs_progress(world, 1);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 8);

    ecs_run(world, TManual, 0, NULL);

    test_int(p->x, 9);

    ecs_fini(world);
}

static
void TMergeOnLoad(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    ecs_id_t ecs_id(Position) = ecs_term_id(it, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        test_int(p[i].x, 0);
        ecs_set(it->world, it->entities[i], Position, {p[i].x + 1, 0});
    }
}

static
void TMergePostLoad(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    ecs_id_t ecs_id(Position) = ecs_term_id(it, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        test_int(p[i].x, 1);
        ecs_set(it->world, it->entities[i], Position, {p[i].x + 1, 0});
    }
}

static
void TMergePreUpdate(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    ecs_id_t ecs_id(Position) = ecs_term_id(it, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        test_int(p[i].x, 2);
        ecs_set(it->world, it->entities[i], Position, {p[i].x + 1, 0});
    }
}

static
void TMergeOnUpdate(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    ecs_id_t ecs_id(Position) = ecs_term_id(it, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        test_int(p[i].x, 3);
        ecs_set(it->world, it->entities[i], Position, {p[i].x + 1, 0});
    }
}

static
void TMergeOnValidate(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    ecs_id_t ecs_id(Position) = ecs_term_id(it, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        test_int(p[i].x, 4);
        ecs_set(it->world, it->entities[i], Position, {p[i].x + 1, 0});
    }
}

static
void TMergePostUpdate(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    ecs_id_t ecs_id(Position) = ecs_term_id(it, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        test_int(p[i].x, 5);
        ecs_set(it->world, it->entities[i], Position, {p[i].x + 1, 0});
    }
}

static
void TMergePreStore(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    ecs_id_t ecs_id(Position) = ecs_term_id(it, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        test_int(p[i].x, 6);
        ecs_set(it->world, it->entities[i], Position, {p[i].x + 1, 0});
    }
}

static
void TMergeOnStore(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    ecs_id_t ecs_id(Position) = ecs_term_id(it, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        test_int(p[i].x, 7);
        ecs_set(it->world, it->entities[i], Position, {p[i].x + 1, 0});
    }
}

static
void TMergeManual(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    ecs_id_t ecs_id(Position) = ecs_term_id(it, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        test_int(p[i].x, 8);
        ecs_set(it->world, it->entities[i], Position, {p[i].x + 1, 0});
    }
}

void World_phases_w_merging() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, TMergeOnLoad, EcsOnLoad, Position, [out] Position());
    ECS_SYSTEM(world, TMergePostLoad, EcsPostLoad, Position, [out] Position());
    ECS_SYSTEM(world, TMergePreUpdate, EcsPreUpdate, Position, [out] Position());
    ECS_SYSTEM(world, TMergeOnUpdate, EcsOnUpdate, Position, [out] Position());
    ECS_SYSTEM(world, TMergeOnValidate, EcsOnValidate, Position, [out] Position());
    ECS_SYSTEM(world, TMergePostUpdate, EcsPostUpdate, Position, [out] Position());
    ECS_SYSTEM(world, TMergePreStore, EcsPreStore, Position, [out] Position());
    ECS_SYSTEM(world, TMergeOnStore, EcsOnStore, Position, [out] Position());
    ECS_SYSTEM(world, TMergeManual, 0, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_set(world, e, Position, {0, 0});

    ecs_progress(world, 1);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 8);

    ecs_run(world, TMergeManual, 0, NULL);

    p = ecs_get(world, e, Position);
    test_int(p->x, 9);

    ecs_fini(world);
}

static
void TimeCheck(ecs_iter_t *it) {
    test_assert(it->delta_time > 0);
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
    test_is_flaky();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, TimeCheck, EcsOnLoad, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    double start, now = 0;
    ecs_set_target_fps(world, 20);

    /* Run a few times to give the code an opportunity to calibrate */
    ecs_progress(world, 0);
    ecs_progress(world, 0);
    ecs_progress(world, 0);
    ecs_progress(world, 0);
    ecs_progress(world, 0);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    /* Run for one second */
    int count = 0;
    do {    
        ecs_progress(world, 0);
        if (!count) {
            start = stats->delta_time;
        }

        now += stats->delta_time;
        count ++;
    } while ((now - start) < 1.0);

    /* CI can be unpredictable, just make sure it's in the right ballpark */
    test_assert(count >= 15);
    test_assert(count < 25);

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
void BusySystem(ecs_iter_t *it) {
    /* Spend 14msec doing something */
    busy_wait(0.014);
}

void World_control_fps_busy_system() {
    test_is_flaky();

    ecs_world_t *world = ecs_init();

    ECS_SYSTEM(world, BusySystem, EcsOnUpdate, 0);

    double start, now = 0;
    ecs_set_target_fps(world, 20);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    /* Run for one second */
    int count = 0;
    do {    
        ecs_progress(world, 0);
        if (!count) {
            start = stats->delta_time;
        }

        now += stats->delta_time;
        count ++;
    } while ((now - start) < 1.0);

    /* FPS control relies on sleep, which relies on the OS scheduler. Therefore
     * pick a wide enough range to avoid tests failing at random. */
    test_assert(count >= 15);
    test_assert(count < 25);

    ecs_fini(world);
}

void World_control_fps_busy_app() {
    test_is_flaky();

    ecs_world_t *world = ecs_init();

    double start, now = 0;
    ecs_set_target_fps(world, 20);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    /* Run for one second */
    int count = 0;
    do {    
        ecs_progress(world, 0);
        if (!count) {
            start = stats->delta_time;
        }

        now += stats->delta_time;
        count ++;

        busy_wait(0.014);
    } while ((now - start) < 1.0);

    /* FPS control relies on sleep, which relies on the OS scheduler. Therefore
     * pick a wide enough range to avoid tests failing at random. */
    test_assert(count >= 15);
    test_assert(count < 25);

    ecs_fini(world);
}

void World_measure_fps_vs_actual() {
    test_is_flaky();

    ecs_world_t *world = ecs_init();

    ecs_set_target_fps(world, 60);

    /* Run 10 times, test if one second has passed */
    ecs_time_t t;
    ecs_os_get_time(&t);
    int32_t i;
    for (i = 0; i < 60; i ++) {
        ecs_progress(world, 0);
    }

    float elapsed = ecs_time_measure(&t);
    test_assert(elapsed >= 0.9);

    ecs_fini(world);
}

void World_measure_delta_time_vs_actual() {
    test_is_flaky();

    ecs_world_t *world = ecs_init();

    ecs_set_target_fps(world, 60);
    const ecs_world_info_t *stats = ecs_get_world_info(world);

    /* Run 10 times, test if one second has passed */
    ecs_time_t t;
    float delta_time = 0;
    ecs_os_get_time(&t);
    int32_t i;
    for (i = 0; i < 60; i ++) {
        ecs_progress(world, 0);
        delta_time += stats->delta_time;
    }

    float elapsed = ecs_time_measure(&t);
    test_assert(delta_time - elapsed < 0.1);
    test_assert(elapsed >= 0.9);    

    ecs_fini(world);
}

static
void RandomSystem(ecs_iter_t *it) {
    /* wait at most 16msec */
    float rnd_time = ((float)rand() / (float)RAND_MAX) * 0.016;
    busy_wait(rnd_time);
}

void World_control_fps_random_system() {
    test_is_flaky();

    ecs_world_t *world = ecs_init();

    ECS_SYSTEM(world, RandomSystem, EcsOnUpdate, 0);

    double start, now = 0;
    ecs_set_target_fps(world, 20);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    /* Run for one second */
    int count = 0;
    do {    
        ecs_progress(world, 0);
        if (!count) {
            start = stats->delta_time;
        }

        now += stats->delta_time;
        count ++;
    } while ((now - start) < 1.0);

    /* FPS control relies on sleep, which relies on the OS scheduler. Therefore
     * pick a wide enough range to avoid tests failing at random. */
    test_assert(count >= 15);
    test_assert(count < 25);

    ecs_fini(world);
}

void World_control_fps_random_app() {
    test_is_flaky();
    
    ecs_world_t *world = ecs_init();

    double start, now = 0;
    ecs_set_target_fps(world, 20);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    /* Run for one second */
    int count = 0;
    do {    
        ecs_progress(world, 0);
        if (!count) {
            start = stats->delta_time;
        }

        now += stats->delta_time;
        count ++;

        float rnd_time = ((float)rand() / (float)RAND_MAX) * 0.016;
        busy_wait(rnd_time);
    } while ((now - start) < 1.0);

    /* FPS control relies on sleep, which relies on the OS scheduler. Therefore
     * pick a wide enough range to avoid tests failing at random. */
    test_assert(count >= 15);
    test_assert(count < 25);

    ecs_fini(world);
}

void World_quit() {
    ecs_world_t *world = ecs_init();

    int32_t count = 0;

    while (ecs_progress(world, 0)) {
        test_int(count, 0);
        ecs_quit(world);
        count ++;
    }

    test_int(count, 1);

    ecs_fini(world);
}

void World_get_delta_time() {
    ecs_world_t *world = ecs_init();

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    test_int(stats->delta_time, 0);

    ecs_progress(world, 1.0);

    test_flt(stats->delta_time, 1.0);

    ecs_fini(world);
}

void World_get_delta_time_auto() {
    ecs_world_t *world = ecs_init();

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    test_int(stats->delta_time, 0);

    ecs_progress(world, 0);

    test_assert(stats->delta_time != 0);

    ecs_fini(world);
}

void World_recreate_world() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_fini(world) == 0);

    world = ecs_init();

    test_assert(ecs_fini(world) == 0);
}

void World_recreate_world_w_component() {
    ecs_world_t *world = ecs_init();
    test_assert(world != NULL);

    {
        ECS_COMPONENT(world, Position);
        test_assert(ecs_id(Position) != 0);
    }

    test_assert(ecs_fini(world) == 0);

    {
        world = ecs_init();
        test_assert(world != NULL);

        ECS_COMPONENT(world, Position);
        test_assert(ecs_id(Position) != 0);

        test_assert(ecs_fini(world) == 0);
    }
}

void World_no_threading() {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.mutex_new_ = NULL;
    ecs_os_set_api(&os_api);

    ecs_world_t *world = ecs_init();
    test_assert(world != NULL);
    ecs_fini(world);
}

void World_no_time() {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.get_time_ = NULL;
    ecs_os_set_api(&os_api);

    ecs_world_t *world = ecs_init();
    test_assert(world != NULL);
    ecs_fini(world);
}

void World_is_entity_enabled() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);

    test_assert( ecs_has_id(world, e, EcsDisabled) == false);

    ecs_fini(world);
}

static int zero_time_scale_invoked = 0;

void ZeroTimeScale(ecs_iter_t *it) {
    test_assert(it->delta_time == 0.0);
    zero_time_scale_invoked ++;
}

void World_system_time_scale() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_new_w_id(world, Tag);

    ecs_set_time_scale(world, 0);

    ECS_SYSTEM(world, ZeroTimeScale, EcsOnUpdate, Tag);

    ecs_progress(world, 0);
    ecs_progress(world, 0);

    const ecs_world_info_t *info = ecs_get_world_info(world);
    test_assert(info->delta_time == 0.0);

    test_int(zero_time_scale_invoked, 2);

    ecs_fini(world);
}

void World_ensure_empty_root() {
    ecs_world_t *world = ecs_init();

    ecs_query_t *q = ecs_query_new(world, "!(ChildOf, *)");
    ecs_iter_t it = ecs_query_iter(world, q);

    /* Make sure that the only entity in the root is the flecs module */
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_assert(it.entities[0] == EcsFlecs);

    /* Entity for the query */
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_assert(ecs_has_id(world, it.entities[0], EcsQuery));

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void World_register_alias_twice_same_entity() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_id(world);

    ecs_set_alias(world, e, "Foo");
    ecs_set_alias(world, e, "Foo");

    ecs_entity_t f = ecs_lookup(world, "Foo");
    test_assert(f == e);

    ecs_fini(world);
}

void World_register_alias_twice_different_entity() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_id(world);
    ecs_entity_t f = ecs_new_id(world);

    ecs_set_alias(world, e, "Foo");
    
    test_expect_abort();
    ecs_set_alias(world, f, "Foo");
}

void World_redefine_component() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t c = ecs_component_init(world, &(ecs_component_desc_t){
        .entity.name = "flecs.core.Component",
        .entity.symbol = "EcsComponent",
        .type.size = ECS_SIZEOF(EcsComponent),
        .type.alignment = ECS_ALIGNOF(EcsComponent)
    });

    test_assert(c == ecs_id(EcsComponent));

    ecs_fini(world);
}

void World_delete_empty_tables_after_mini() {
    ecs_world_t *world = ecs_mini();

    const ecs_world_info_t *info = ecs_get_world_info(world);
    int32_t old_empty_table_count = info->empty_table_count;

    int32_t deleted;
    deleted = ecs_delete_empty_tables(world, 0, 0, 1, 0, 0); /* Increase to 1 */
    test_int(deleted, 0);

    deleted = ecs_delete_empty_tables(world, 0, 0, 1, 0, 0); /* Delete */
    test_assert(deleted != 0);
    test_int(info->empty_table_count + deleted, old_empty_table_count);

    ecs_fini(world);
}

void World_delete_empty_tables_after_init() {
    ecs_world_t *world = ecs_init();

    const ecs_world_info_t *info = ecs_get_world_info(world);
    int32_t old_empty_table_count = info->empty_table_count;

    int32_t deleted;
    deleted = ecs_delete_empty_tables(world, 0, 0, 1, 0, 0); /* Increase to 1 */
    test_int(deleted, 0);

    deleted = ecs_delete_empty_tables(world, 0, 0, 1, 0, 0); /* Delete */
    test_assert(deleted != 0);
    test_int(info->empty_table_count + deleted, old_empty_table_count);

    ecs_fini(world);
}

void World_delete_1000_empty_tables() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ecs_run_aperiodic(world, 0);

    const ecs_world_info_t *info = ecs_get_world_info(world);
    int32_t old_empty_table_count = info->empty_table_count;

    ecs_entity_t e = ecs_new(world, Tag);
    for (int i = 0; i < 1000; i ++) {
        ecs_add_id(world, e, ecs_new_id(world));
    }

    ecs_run_aperiodic(world, 0);
    test_int(info->empty_table_count, old_empty_table_count + 1000);

    int32_t deleted;
    deleted = ecs_delete_empty_tables(world, 0, 0, 1, 0, 0); /* Increase to 1 */
    test_int(deleted, 0);

    deleted = ecs_delete_empty_tables(world, 0, 0, 1, 0, 0); /* Delete */
    test_assert(deleted != 0);
    test_assert(deleted >= 1000);

    test_assert(info->empty_table_count <= old_empty_table_count);

    ecs_fini(world);
}

void World_delete_empty_tables_for_id() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ecs_run_aperiodic(world, 0);

    const ecs_world_info_t *info = ecs_get_world_info(world);
    int32_t old_empty_table_count = info->empty_table_count;

    ecs_entity_t e1 = ecs_new(world, TagA);
    for (int i = 0; i < 500; i ++) {
        ecs_add_id(world, e1, ecs_new_id(world));
    }

    ecs_entity_t e2 = ecs_new(world, TagB);
    for (int i = 0; i < 500; i ++) {
        ecs_add_id(world, e2, ecs_new_id(world));
    }

    ecs_run_aperiodic(world, 0);
    test_int(info->empty_table_count, old_empty_table_count + 1000);

    int32_t deleted;
    deleted = ecs_delete_empty_tables(world, TagA, 0, 1, 0, 0); /* Increase to 1 */
    test_int(deleted, 0);

    deleted = ecs_delete_empty_tables(world, TagA, 0, 1, 0, 0); /* Delete */
    test_assert(deleted != 0);
    test_assert(deleted >= 500);
    test_assert(deleted < 1000);

    test_assert((info->empty_table_count - 500) <= old_empty_table_count);

    ecs_fini(world);
}

void World_use_after_delete_empty() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add(world, e, TagA);
    ecs_add(world, e, TagB);

    ecs_remove(world, e, TagA);
    int32_t deleted;
    deleted = ecs_delete_empty_tables(world, 0, 0, 1, 0, 0);
    test_assert(deleted == 0);
    deleted = ecs_delete_empty_tables(world, 0, 0, 1, 0, 0);
    test_assert(deleted != 0);
    ecs_add(world, e, TagA);

    test_assert( ecs_has(world, e, TagA));
    test_assert( ecs_has(world, e, TagB));

    ecs_fini(world);
}

void World_use_after_clear_empty() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add(world, e, TagA);
    ecs_add(world, e, TagB);

    ecs_remove(world, e, TagA);
    int32_t deleted;
    deleted = ecs_delete_empty_tables(world, 0, 1, 0, 0, 0);
    test_assert(deleted == 0);
    deleted = ecs_delete_empty_tables(world, 0, 1, 0, 0, 0);
    test_assert(deleted == 0);
    ecs_add(world, e, TagA);

    test_assert( ecs_has(world, e, TagA));
    test_assert( ecs_has(world, e, TagB));

    ecs_fini(world);
}

void World_use_after_delete_empty_w_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);

    test_assert( ecs_has(world, e, Position));

    ecs_remove(world, e, Velocity);
    test_assert( ecs_has(world, e, Position));
    test_assert( !ecs_has(world, e, Velocity));

    int32_t deleted;
    deleted = ecs_delete_empty_tables(world, 0, 0, 1, 0, 0);
    test_assert(deleted == 0);
    test_bool(true, ecs_is_alive(world, ecs_id(Position)));
    test_bool(true, ecs_is_alive(world, ecs_id(Velocity)));
    test_assert( ecs_has(world, e, Position));

    deleted = ecs_delete_empty_tables(world, 0, 0, 1, 0, 0);
    test_assert(deleted != 0);
    test_bool(true, ecs_is_alive(world, ecs_id(Position)));
    test_bool(true, ecs_is_alive(world, ecs_id(Velocity)));
    test_assert( ecs_has(world, e, Position));
    
    ecs_add(world, e, Velocity);

    test_assert( ecs_has(world, e, Position));
    test_assert( ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void World_use_after_clear_empty_w_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);

    test_assert( ecs_has(world, e, Position));

    ecs_remove(world, e, Velocity);
    test_assert( ecs_has(world, e, Position));
    test_assert( !ecs_has(world, e, Velocity));

    int32_t deleted;
    deleted = ecs_delete_empty_tables(world, 0, 1, 0, 0, 0);
    test_assert(deleted == 0);
    test_bool(true, ecs_is_alive(world, ecs_id(Position)));
    test_bool(true, ecs_is_alive(world, ecs_id(Velocity)));
    test_assert( ecs_has(world, e, Position));

    deleted = ecs_delete_empty_tables(world, 0, 1, 0, 0, 0);
    test_assert(deleted == 0);
    test_bool(true, ecs_is_alive(world, ecs_id(Position)));
    test_bool(true, ecs_is_alive(world, ecs_id(Velocity)));
    test_assert( ecs_has(world, e, Position));
    
    ecs_add(world, e, Velocity);

    test_assert( ecs_has(world, e, Position));
    test_assert( ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void World_use_after_clear_empty_w_component_w_lifecycle() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_set_hooks(world, Position, { 
        .ctor = ecs_default_ctor 
    });
    ecs_set_hooks(world, Velocity, { 
        .ctor = ecs_default_ctor 
    });

    ecs_entity_t e = ecs_new_id(world);
    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);

    test_assert( ecs_has(world, e, Position));

    ecs_remove(world, e, Velocity);
    test_assert( ecs_has(world, e, Position));
    test_assert( !ecs_has(world, e, Velocity));

    int32_t deleted;
    deleted = ecs_delete_empty_tables(world, 0, 1, 0, 0, 0);
    test_assert(deleted == 0);
    test_bool(true, ecs_is_alive(world, ecs_id(Position)));
    test_bool(true, ecs_is_alive(world, ecs_id(Velocity)));
    test_assert( ecs_has(world, e, Position));

    deleted = ecs_delete_empty_tables(world, 0, 1, 0, 0, 0);
    test_assert(deleted == 0);
    test_bool(true, ecs_is_alive(world, ecs_id(Position)));
    test_bool(true, ecs_is_alive(world, ecs_id(Velocity)));
    test_assert( ecs_has(world, e, Position));
    
    ecs_add(world, e, Velocity);

    test_assert( ecs_has(world, e, Position));
    test_assert( ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void World_use_after_clear_unused() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    int32_t deleted;
    deleted = ecs_delete_empty_tables(world, 0, 1, 0, 0, 0);
    test_assert(deleted == 0);
    deleted = ecs_delete_empty_tables(world, 0, 1, 0, 0, 0);
    test_assert(deleted == 0);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add(world, e, TagA);
    ecs_add(world, e, TagB);

    test_assert(ecs_has(world, e, TagA));
    test_assert(ecs_has(world, e, TagB));

    ecs_fini(world);
}

static ECS_COMPONENT_DECLARE(Test);

typedef struct Test {
    uint32_t value;
} Test;

static int at_fini_test_invoked = 0;

static 
void at_fini_test(
    ecs_world_t* world,
    void* context) 
{
    test_assert(ecs_singleton_get_mut(world, Test) != NULL);
    at_fini_test_invoked = 1;
}

void World_get_mut_in_at_fini() {
    ecs_world_t* world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Test);
    ecs_singleton_set(world, Test, { 42 });

    ecs_atfini(world, at_fini_test, NULL);

    ecs_fini(world);

    test_int(at_fini_test_invoked, 1);
}

void World_get_type_info() {
    ecs_world_t* world = ecs_mini();

    ECS_COMPONENT(world, Position);

    const ecs_type_info_t *ti = ecs_get_type_info(world, ecs_id(Position));
    test_assert(ti != NULL);
    test_int(ti->size, ECS_SIZEOF(Position));
    test_int(ti->alignment, ECS_ALIGNOF(Position));
    test_uint(ti->component, ecs_id(Position));

    ecs_fini(world);
}

void World_get_type_info_after_delete_with() {
    ecs_world_t* world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_delete_with(world, ecs_id(Position));

    const ecs_type_info_t *ti = ecs_get_type_info(world, ecs_id(Position));
    test_assert(ti != NULL);
    test_int(ti->size, ECS_SIZEOF(Position));
    test_int(ti->alignment, ECS_ALIGNOF(Position));
    test_uint(ti->component, ecs_id(Position));

    ecs_fini(world);
}

void World_get_type_info_after_reuse() {
    ecs_world_t* world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_delete_with(world, ecs_id(Position));

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    const ecs_type_info_t *ti = ecs_get_type_info(world, ecs_id(Position));
    test_assert(ti != NULL);
    test_int(ti->size, ECS_SIZEOF(Position));
    test_int(ti->alignment, ECS_ALIGNOF(Position));
    test_uint(ti->component, ecs_id(Position));

    ecs_fini(world);
}
