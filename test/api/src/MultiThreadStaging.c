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

static
void Delete_above_1000(EcsRows *rows) {
    int i;
    for (i = 0; i < rows->count; i ++) {
        if ((i + rows->frame_offset) > 1000) {
            ecs_delete(rows->world, rows->entities[i]);
        }
    }
}

static
void Add_random(EcsRows *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    EcsEntity *entities = ecs_column(rows, EcsEntity, 0);

    int i;
    for (i = 0; i < rows->count; i ++) {
        if (rand() % 2) {
            EcsEntity e = _ecs_new(rows->world, ctx->component);
            test_assert( !ecs_empty(rows->world, e));
            test_assert( _ecs_has(rows->world, e, ctx->component));
            test_assert( _ecs_get_ptr(rows->world, e, ctx->component) != NULL);
        }
        
        if (rand() % 2) {
            _ecs_add(rows->world, entities[i], ctx->component_2);
            
            test_assert( !ecs_empty(rows->world, entities[i]));
            test_assert( _ecs_has(rows->world, entities[i], ctx->component));
            test_assert( _ecs_get_ptr(rows->world, entities[i], ctx->component) != NULL);
        }

        if (rand() % 2) {
            _ecs_add(rows->world, entities[i], ctx->component_3);

            test_assert( !ecs_empty(rows->world, entities[i]));
            test_assert( _ecs_has(rows->world, entities[i], ctx->component_3)); 
            test_assert( _ecs_get_ptr(rows->world, entities[i], ctx->component_3) != NULL);
        }

        ctx->entity_count ++;
    }
}

void MultiThreadStaging_stress_create_delete_entity_random_components() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rotation);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, Add_random, EcsOnFrame, Position);
    ECS_SYSTEM(world, Delete_above_1000, EcsPostFrame, Position);

    IterData ctx = {.component = TPosition, .component_2 = TVelocity, .component_3 = TRotation};
    ecs_set_context(world, &ctx);

    ecs_new_w_count(world, Position, 500, NULL);
    ecs_new_w_count(world, Type, 500, NULL);

    ecs_set_threads(world, 6);


    uint64_t i;
    for (i = 0; i < 1000; i ++) {
        ecs_progress(world, 1);
    }

    ecs_fini(world);
}

static
void Set_velocity_callback(EcsRows *rows) {
    Velocity *v = ecs_column(rows, Velocity, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        v->x ++;
        v->y ++;
    }
}

static
void Set_random(EcsRows *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    EcsEntity *entities = ecs_column(rows, EcsEntity, 0);

    EcsType TPosition = ctx->component;
    EcsType TVelocity = ctx->component_2;
    EcsType TRotation = ctx->component_3;

    int i;
    for (i = 0; i < rows->count; i ++) {
        if (rand() % 2) {
            EcsEntity e = ecs_set(rows->world, 0, Position, {10, 20});
            test_assert( !ecs_empty(rows->world, e));
            test_assert( ecs_has(rows->world, e, Position));
            Position *p = ecs_get_ptr(rows->world, e, Position);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        
        if (rand() % 2) {
            ecs_set(rows->world, entities[i], Velocity, {30, 40});
            
            test_assert( !ecs_empty(rows->world, entities[i]));
            test_assert( ecs_has(rows->world, entities[i], Velocity));
            Velocity *v = ecs_get_ptr(rows->world, entities[i], Velocity);            
            test_assert(v != NULL);
            test_int(v->x, 31);
            test_int(v->y, 41);
        }

        if (rand() % 2) {
            ecs_set(rows->world, entities[i], Rotation, {50});

            test_assert( !ecs_empty(rows->world, entities[i]));
            test_assert( ecs_has(rows->world, entities[i], Rotation));
            Rotation *r = ecs_get_ptr(rows->world, entities[i], Rotation);            
            test_assert(r != NULL);
            test_int(*r, 50);
        }

        ctx->entity_count ++;
    }
}

void MultiThreadStaging_stress_set_entity_random_components() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rotation);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, Set_random, EcsOnFrame, Position);
    ECS_SYSTEM(world, Set_velocity_callback, EcsOnSet, Velocity);

    IterData ctx = {.component = TPosition, .component_2 = TVelocity, .component_3 = TRotation};
    ecs_set_context(world, &ctx);

    ecs_new_w_count(world, Position, 500, NULL);
    ecs_new_w_count(world, Type, 500, NULL);

    ecs_set_threads(world, 6);

    int i;
    for (i = 0; i < 1000; i ++) {
        ecs_progress(world, 1);
    }

    ecs_fini(world);
}
