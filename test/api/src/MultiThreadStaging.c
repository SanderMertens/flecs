#include <api.h>

void MultiThreadStaging_setup() {
    bake_set_os_api();
    ecs_tracing_enable(-3);
}

static
void Add_to_current(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);

    int i;
    for (i = 0; i < it->count; i ++) {
        if (ctx->component) {
            ecs_add_entity(it->world, it->entities[i], ctx->component);
        }

        if (ctx->component_2) {
            ecs_add_entity(it->world, it->entities[i], ctx->component_2);
        }

        ctx->entity_count ++;
    }
}

void MultiThreadStaging_2_threads_add_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rotation);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, Add_to_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Rotation)};
    ecs_set_context(world, &ctx);

    ecs_entity_t ids_1[100];
    const ecs_entity_t *temp_ids_1 = ecs_bulk_new(world, Position, 100);
    memcpy(ids_1, temp_ids_1, sizeof(ecs_entity_t) * 100);

    const ecs_entity_t *ids_2 = ecs_bulk_new(world, Type, 100);

    ecs_set_threads(world, 2);

    ecs_progress(world, 1);

    int i;
    for (i = 0; i < 100; i ++) {
        test_assert( ecs_has(world, ids_1[i], Position));
        test_assert( ecs_has(world, ids_1[i], Rotation));
        test_assert( !ecs_has(world, ids_1[i], Velocity));
    }

    for (i = 0; i < 100; i ++) {
        test_assert( ecs_has(world, ids_2[i], Position));
        test_assert( ecs_has(world, ids_2[i], Rotation));
        test_assert( ecs_has(world, ids_2[i], Velocity));
    }

    ecs_fini(world);
}

void MultiThreadStaging_3_threads_add_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rotation);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, Add_to_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Rotation)};
    ecs_set_context(world, &ctx);

    ecs_entity_t ids_1[100];
    const ecs_entity_t *temp_ids_1 = ecs_bulk_new(world, Position, 100);
    memcpy(ids_1, temp_ids_1, sizeof(ecs_entity_t) * 100);

    const ecs_entity_t *ids_2 = ecs_bulk_new(world, Type, 100);

    ecs_set_threads(world, 3);

    ecs_progress(world, 1);

    int i;
    for (i = 0; i < 100; i ++) {
        test_assert( ecs_has(world, ids_1[i], Position));
        test_assert( ecs_has(world, ids_1[i], Rotation));
        test_assert( !ecs_has(world, ids_1[i], Velocity));
    }

    for (i = 0; i < 100; i ++) {
        test_assert( ecs_has(world, ids_2[i], Position));
        test_assert( ecs_has(world, ids_2[i], Rotation));
        test_assert( ecs_has(world, ids_2[i], Velocity));
    }

    ecs_fini(world);
}

void MultiThreadStaging_4_threads_add_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rotation);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, Add_to_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Rotation)};
    ecs_set_context(world, &ctx);

    ecs_entity_t ids_1[100];
    const ecs_entity_t *temp_ids_1 = ecs_bulk_new(world, Position, 100);
    memcpy(ids_1, temp_ids_1, sizeof(ecs_entity_t) * 100);

    const ecs_entity_t *ids_2 = ecs_bulk_new(world, Type, 100);

    ecs_set_threads(world, 4);

    ecs_progress(world, 1);

    int i;
    for (i = 0; i < 100; i ++) {
        test_assert( ecs_has(world, ids_1[i], Position));
        test_assert( ecs_has(world, ids_1[i], Rotation));
        test_assert( !ecs_has(world, ids_1[i], Velocity));
    }

    for (i = 0; i < 100; i ++) {
        test_assert( ecs_has(world, ids_2[i], Position));
        test_assert( ecs_has(world, ids_2[i], Rotation));
        test_assert( ecs_has(world, ids_2[i], Velocity));
    }

    ecs_fini(world);
}

void MultiThreadStaging_5_threads_add_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rotation);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, Add_to_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Rotation)};
    ecs_set_context(world, &ctx);

    ecs_entity_t ids_1[100];
    const ecs_entity_t *temp_ids_1 = ecs_bulk_new(world, Position, 100);
    memcpy(ids_1, temp_ids_1, sizeof(ecs_entity_t) * 100);

    const ecs_entity_t *ids_2 = ecs_bulk_new(world, Type, 100);

    ecs_set_threads(world, 5);

    ecs_progress(world, 1);

    int i;
    for (i = 0; i < 100; i ++) {
        test_assert( ecs_has(world, ids_1[i], Position));
        test_assert( ecs_has(world, ids_1[i], Rotation));
        test_assert( !ecs_has(world, ids_1[i], Velocity));
    }

    for (i = 0; i < 100; i ++) {
        test_assert( ecs_has(world, ids_2[i], Position));
        test_assert( ecs_has(world, ids_2[i], Rotation));
        test_assert( ecs_has(world, ids_2[i], Velocity));
    }

    ecs_fini(world);
}

void MultiThreadStaging_6_threads_add_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rotation);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, Add_to_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Rotation)};
    ecs_set_context(world, &ctx);

    ecs_entity_t ids_1[100];
    const ecs_entity_t *temp_ids_1 = ecs_bulk_new(world, Position, 100);
    memcpy(ids_1, temp_ids_1, sizeof(ecs_entity_t) * 100);

    const ecs_entity_t *ids_2 = ecs_bulk_new(world, Type, 100);

    ecs_set_threads(world, 6);

    ecs_progress(world, 1);

    int i;
    for (i = 0; i < 100; i ++) {
        test_assert( ecs_has(world, ids_1[i], Position));
        test_assert( ecs_has(world, ids_1[i], Rotation));
        test_assert( !ecs_has(world, ids_1[i], Velocity));
    }

    for (i = 0; i < 100; i ++) {
        test_assert( ecs_has(world, ids_2[i], Position));
        test_assert( ecs_has(world, ids_2[i], Rotation));
        test_assert( ecs_has(world, ids_2[i], Velocity));
    }

    ecs_fini(world);
}

static
void InitVelocity(ecs_iter_t *it) {
    ECS_COLUMN(it, Velocity, v, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        v[i].x = 10;
        v[i].y = 20;
    }
}

static
void AddVelocity(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, Velocity, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add(it->world, it->entities[i], Velocity);
    }
}

void MultiThreadStaging_2_threads_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TRIGGER(world, InitVelocity, EcsOnAdd, Velocity);
    ECS_SYSTEM(world, AddVelocity, EcsOnUpdate, Position, :Velocity);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    const ecs_entity_t *ids = ecs_bulk_new(world, Position, 10);
    test_assert(ids != NULL);

    ecs_set_threads(world, 2);

    ecs_progress(world, 0);

    int i;
    for (i = 0; i < 10; i ++) {
        ecs_entity_t e = ids[i];
        test_assert( ecs_has(world, e, Velocity));
        const Velocity *v = ecs_get(world, e, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 10);
        test_int(v->y, 20);
    }

    ecs_fini(world);
}

static
void New_w_count(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, Position, 1);

    ecs_bulk_new(it->world, Position, 10);
}

void MultiThreadStaging_new_w_count() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, New_w_count, EcsOnUpdate, :Position);

    ecs_set_threads(world, 2);

    ecs_progress(world, 0);

    test_int( ecs_count(world, Position), 10);

    ecs_fini(world);
}
