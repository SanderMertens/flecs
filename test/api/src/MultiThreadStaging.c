#include <include/api.h>

static
void Add_to_current(EcsRows *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    EcsEntity *entities = ecs_column(rows, EcsEntity, 0);
    int i;
    for (i = 0; i < rows->count; i ++) {
        if (ctx->component) {
            _ecs_add(rows->world, entities[i], ctx->component);
            
            test_assert( !ecs_empty(rows->world, entities[i]));
            test_assert( _ecs_has(rows->world, entities[i], ctx->component));
            test_assert( _ecs_get_ptr(rows->world, entities[i], ctx->component) != NULL);
        }

        if (ctx->component_2) {
            _ecs_add(rows->world, entities[i], ctx->component_2);

            test_assert( !ecs_empty(rows->world, entities[i]));
            test_assert( _ecs_has(rows->world, entities[i], ctx->component_2)); 
            test_assert( _ecs_get_ptr(rows->world, entities[i], ctx->component_2) != NULL);
        }

        ctx->entity_count ++;
    }
}

void MultiThreadStaging_2_threads_add_to_current() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rotation);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, Add_to_current, EcsOnFrame, Position);

    IterData ctx = {.component = TRotation};
    ecs_set_context(world, &ctx);

    EcsEntity start_1 = ecs_new_w_count(world, Position, 500, NULL);
    EcsEntity start_2 = ecs_new_w_count(world, Type, 500, NULL);

    ecs_set_threads(world, 2);

    ecs_progress(world, 1);

    int i;
    for (i = 0; i < 500; i ++) {
        test_assert( ecs_has(world, start_1 + i, Position));
        test_assert( ecs_has(world, start_1 + i, Rotation));
        test_assert( !ecs_has(world, start_1 + i, Velocity));
    }

    for (i = 0; i < 500; i ++) {
        test_assert( ecs_has(world, start_2 + i, Position));
        test_assert( ecs_has(world, start_2 + i, Rotation));
        test_assert( ecs_has(world, start_2 + i, Velocity));
    }

    ecs_fini(world);
}

void MultiThreadStaging_3_threads_add_to_current() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rotation);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, Add_to_current, EcsOnFrame, Position);

    IterData ctx = {.component = TRotation};
    ecs_set_context(world, &ctx);

    EcsEntity start_1 = ecs_new_w_count(world, Position, 500, NULL);
    EcsEntity start_2 = ecs_new_w_count(world, Type, 500, NULL);

    ecs_set_threads(world, 3);

    ecs_progress(world, 1);

    int i;
    for (i = 0; i < 500; i ++) {
        test_assert( ecs_has(world, start_1 + i, Position));
        test_assert( ecs_has(world, start_1 + i, Rotation));
        test_assert( !ecs_has(world, start_1 + i, Velocity));
    }

    for (i = 0; i < 500; i ++) {
        test_assert( ecs_has(world, start_2 + i, Position));
        test_assert( ecs_has(world, start_2 + i, Rotation));
        test_assert( ecs_has(world, start_2 + i, Velocity));
    }

    ecs_fini(world);
}

void MultiThreadStaging_4_threads_add_to_current() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rotation);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, Add_to_current, EcsOnFrame, Position);

    IterData ctx = {.component = TRotation};
    ecs_set_context(world, &ctx);

    EcsEntity start_1 = ecs_new_w_count(world, Position, 500, NULL);
    EcsEntity start_2 = ecs_new_w_count(world, Type, 500, NULL);

    ecs_set_threads(world, 4);

    ecs_progress(world, 1);

    int i;
    for (i = 0; i < 500; i ++) {
        test_assert( ecs_has(world, start_1 + i, Position));
        test_assert( ecs_has(world, start_1 + i, Rotation));
        test_assert( !ecs_has(world, start_1 + i, Velocity));
    }

    for (i = 0; i < 500; i ++) {
        test_assert( ecs_has(world, start_2 + i, Position));
        test_assert( ecs_has(world, start_2 + i, Rotation));
        test_assert( ecs_has(world, start_2 + i, Velocity));
    }

    ecs_fini(world);
}

void MultiThreadStaging_5_threads_add_to_current() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rotation);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, Add_to_current, EcsOnFrame, Position);

    IterData ctx = {.component = TRotation};
    ecs_set_context(world, &ctx);

    EcsEntity start_1 = ecs_new_w_count(world, Position, 500, NULL);
    EcsEntity start_2 = ecs_new_w_count(world, Type, 500, NULL);

    ecs_set_threads(world, 5);

    ecs_progress(world, 1);

    int i;
    for (i = 0; i < 500; i ++) {
        test_assert( ecs_has(world, start_1 + i, Position));
        test_assert( ecs_has(world, start_1 + i, Rotation));
        test_assert( !ecs_has(world, start_1 + i, Velocity));
    }

    for (i = 0; i < 500; i ++) {
        test_assert( ecs_has(world, start_2 + i, Position));
        test_assert( ecs_has(world, start_2 + i, Rotation));
        test_assert( ecs_has(world, start_2 + i, Velocity));
    }

    ecs_fini(world);
}

void MultiThreadStaging_6_threads_add_to_current() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rotation);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, Add_to_current, EcsOnFrame, Position);

    IterData ctx = {.component = TRotation};
    ecs_set_context(world, &ctx);

    EcsEntity start_1 = ecs_new_w_count(world, Position, 500, NULL);
    EcsEntity start_2 = ecs_new_w_count(world, Type, 500, NULL);

    ecs_set_threads(world, 6);

    ecs_progress(world, 1);

    int i;
    for (i = 0; i < 500; i ++) {
        test_assert( ecs_has(world, start_1 + i, Position));
        test_assert( ecs_has(world, start_1 + i, Rotation));
        test_assert( !ecs_has(world, start_1 + i, Velocity));
    }

    for (i = 0; i < 500; i ++) {
        test_assert( ecs_has(world, start_2 + i, Position));
        test_assert( ecs_has(world, start_2 + i, Rotation));
        test_assert( ecs_has(world, start_2 + i, Velocity));
    }

    ecs_fini(world);
}

