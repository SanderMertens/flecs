#include <api.h>

void Move(ecs_rows_t *rows) {
    ProbeSystem(rows);

    int row;
    for (row = 0; row < rows->count; row ++) {
        Position *p = ecs_field(rows, Position, row, 1);
        Velocity *v = ecs_field(rows, Velocity, row, 2);
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
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_set_entity_range(world, 50, 0);

    test_expect_abort();

    ecs_add(world, 20, Position);

    ecs_fini(world);
}

void World_entity_range_limit_out_of_range() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_set_entity_range(world, 0, 50);

    test_expect_abort();

    ecs_add(world, 60, Position);

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

    ecs_dim(world, 1000);

    malloc_count = 0;

    ecs_new_w_count(world, Position, 500);

    test_int(malloc_count, 6);

    malloc_count = 0;

    ecs_new_w_count(world, Position, 500);

    test_int(malloc_count, 5);

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

    test_int(malloc_count, 3);

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

    test_int(malloc_count, 1);

    malloc_count = 0;

    ecs_new_w_count(world, Position, 400);

    test_int(malloc_count, 2);

    ecs_fini(world);
}
