#include <api.h>

static
void Add_to_current(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);

    int i;
    for (i = 0; i < rows->count; i ++) {
        if (ctx->component) {
            _ecs_add(rows->world, rows->entities[i], ctx->component);
            
            test_assert( !ecs_is_empty(rows->world, rows->entities[i]));
            test_assert( _ecs_has(rows->world, rows->entities[i], ctx->component));
            test_assert( _ecs_get_ptr(rows->world, rows->entities[i], ctx->component) != NULL);
        }

        if (ctx->component_2) {
            _ecs_add(rows->world, rows->entities[i], ctx->component_2);

            test_assert( !ecs_is_empty(rows->world, rows->entities[i]));
            test_assert( _ecs_has(rows->world, rows->entities[i], ctx->component_2)); 
            test_assert( _ecs_get_ptr(rows->world, rows->entities[i], ctx->component_2) != NULL);
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

    IterData ctx = {.component = ecs_type(Rotation)};
    ecs_set_context(world, &ctx);

    ecs_entity_t start_1 = ecs_new_w_count(world, Position, 100);
    ecs_entity_t start_2 = ecs_new_w_count(world, Type, 100);

    ecs_set_threads(world, 2);

    ecs_progress(world, 1);

    int i;
    for (i = 0; i < 100; i ++) {
        test_assert( ecs_has(world, start_1 + i, Position));
        test_assert( ecs_has(world, start_1 + i, Rotation));
        test_assert( !ecs_has(world, start_1 + i, Velocity));
    }

    for (i = 0; i < 100; i ++) {
        test_assert( ecs_has(world, start_2 + i, Position));
        test_assert( ecs_has(world, start_2 + i, Rotation));
        test_assert( ecs_has(world, start_2 + i, Velocity));
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

    IterData ctx = {.component = ecs_type(Rotation)};
    ecs_set_context(world, &ctx);

    ecs_entity_t start_1 = ecs_new_w_count(world, Position, 100);
    ecs_entity_t start_2 = ecs_new_w_count(world, Type, 100);

    ecs_set_threads(world, 3);

    ecs_progress(world, 1);

    int i;
    for (i = 0; i < 100; i ++) {
        test_assert( ecs_has(world, start_1 + i, Position));
        test_assert( ecs_has(world, start_1 + i, Rotation));
        test_assert( !ecs_has(world, start_1 + i, Velocity));
    }

    for (i = 0; i < 100; i ++) {
        test_assert( ecs_has(world, start_2 + i, Position));
        test_assert( ecs_has(world, start_2 + i, Rotation));
        test_assert( ecs_has(world, start_2 + i, Velocity));
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

    IterData ctx = {.component = ecs_type(Rotation)};
    ecs_set_context(world, &ctx);

    ecs_entity_t start_1 = ecs_new_w_count(world, Position, 100);
    ecs_entity_t start_2 = ecs_new_w_count(world, Type, 100);

    ecs_set_threads(world, 4);

    ecs_progress(world, 1);

    int i;
    for (i = 0; i < 100; i ++) {
        test_assert( ecs_has(world, start_1 + i, Position));
        test_assert( ecs_has(world, start_1 + i, Rotation));
        test_assert( !ecs_has(world, start_1 + i, Velocity));
    }

    for (i = 0; i < 100; i ++) {
        test_assert( ecs_has(world, start_2 + i, Position));
        test_assert( ecs_has(world, start_2 + i, Rotation));
        test_assert( ecs_has(world, start_2 + i, Velocity));
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

    IterData ctx = {.component = ecs_type(Rotation)};
    ecs_set_context(world, &ctx);

    ecs_entity_t start_1 = ecs_new_w_count(world, Position, 100);
    ecs_entity_t start_2 = ecs_new_w_count(world, Type, 100);

    ecs_set_threads(world, 5);

    ecs_progress(world, 1);

    int i;
    for (i = 0; i < 100; i ++) {
        test_assert( ecs_has(world, start_1 + i, Position));
        test_assert( ecs_has(world, start_1 + i, Rotation));
        test_assert( !ecs_has(world, start_1 + i, Velocity));
    }

    for (i = 0; i < 100; i ++) {
        test_assert( ecs_has(world, start_2 + i, Position));
        test_assert( ecs_has(world, start_2 + i, Rotation));
        test_assert( ecs_has(world, start_2 + i, Velocity));
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

    IterData ctx = {.component = ecs_type(Rotation)};
    ecs_set_context(world, &ctx);

    ecs_entity_t start_1 = ecs_new_w_count(world, Position, 100);
    ecs_entity_t start_2 = ecs_new_w_count(world, Type, 100);

    ecs_set_threads(world, 6);

    ecs_progress(world, 1);

    int i;
    for (i = 0; i < 100; i ++) {
        test_assert( ecs_has(world, start_1 + i, Position));
        test_assert( ecs_has(world, start_1 + i, Rotation));
        test_assert( !ecs_has(world, start_1 + i, Velocity));
    }

    for (i = 0; i < 100; i ++) {
        test_assert( ecs_has(world, start_2 + i, Position));
        test_assert( ecs_has(world, start_2 + i, Rotation));
        test_assert( ecs_has(world, start_2 + i, Velocity));
    }

    ecs_fini(world);
}

static
void Delete_above_1000(ecs_rows_t *rows) {
    int i;

    for (i = 0; i < rows->count; i ++) {
        if ((i + rows->frame_offset) > 1000) {
            ecs_delete(rows->world, rows->entities[i]);
        }
    }
}

static
void Add_random(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);

    int i;
    for (i = 0; i < rows->count; i ++) {
        if (rand() % 2) {
            ecs_entity_t e = _ecs_new(rows->world, ctx->component);
            test_assert( !ecs_is_empty(rows->world, e));
            test_assert( _ecs_has(rows->world, e, ctx->component));
            test_assert( _ecs_get_ptr(rows->world, e, ctx->component) != NULL);
        }
        
        if (rand() % 2) {
            _ecs_add(rows->world, rows->entities[i], ctx->component_2);
            
            test_assert( !ecs_is_empty(rows->world, rows->entities[i]));
            test_assert( _ecs_has(rows->world, rows->entities[i], ctx->component));

            test_assert( _ecs_get_ptr(rows->world, rows->entities[i], ctx->component) != NULL);
        }

        if (rand() % 2) {
            _ecs_add(rows->world, rows->entities[i], ctx->component_3);

            test_assert( !ecs_is_empty(rows->world, rows->entities[i]));
            test_assert( _ecs_has(rows->world, rows->entities[i], ctx->component_3)); 
            test_assert( _ecs_get_ptr(rows->world, rows->entities[i], ctx->component_3) != NULL);
        }

        ctx->entity_count ++;
    }
}

void MultiThreadStaging_stress_create_delete_entity_random_components() {
    test_quarantine("07-09-2019");

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rotation);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, Add_random, EcsOnUpdate, Position);
    ECS_SYSTEM(world, Delete_above_1000, EcsPostUpdate, Position);

    IterData ctx = {.component = ecs_type(Position), .component_2 = ecs_type(Velocity), .component_3 = ecs_type(Rotation)};
    ecs_set_context(world, &ctx);

    ecs_new_w_count(world, Position, 500);
    ecs_new_w_count(world, Type, 500);

    ecs_set_threads(world, 6);

    uint64_t i;
    for (i = 0; i < 100; i ++) {
        ecs_progress(world, 1);
    }

    ecs_fini(world);
}

static
void Set_velocity_callback(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Velocity, v, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        v->x ++;
        v->y ++;
    }
}

static
void Set_random(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);

    ecs_type_t ecs_type(Position) = ctx->component;
    ecs_entity_t ecs_entity(Position) = ecs_type_to_entity(rows->world, ecs_type(Position));

    ecs_type_t ecs_type(Velocity) = ctx->component_2;
    ecs_entity_t ecs_entity(Velocity) = ecs_type_to_entity(rows->world, ecs_type(Velocity));
 
    ecs_type_t ecs_type(Rotation) = ctx->component_3;
    ecs_entity_t ecs_entity(Rotation) = ecs_type_to_entity(rows->world, ecs_type(Rotation));

    int i;
    for (i = 0; i < rows->count; i ++) {
        if (rand() % 2) {
            ecs_entity_t e = ecs_set(rows->world, 0, Position, {10, 20});
            test_assert( !ecs_is_empty(rows->world, e));
            test_assert( ecs_has(rows->world, e, Position));
            Position *p = ecs_get_ptr(rows->world, e, Position);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        
        if (rand() % 2) {
            ecs_set(rows->world, rows->entities[i], Velocity, {30, 40});
            
            test_assert( !ecs_is_empty(rows->world, rows->entities[i]));
            test_assert( ecs_has(rows->world, rows->entities[i], Velocity));
            Velocity *v = ecs_get_ptr(rows->world, rows->entities[i], Velocity);            
            test_assert(v != NULL);
            test_int(v->x, 31);
            test_int(v->y, 41);
        }

        if (rand() % 2) {
            ecs_set(rows->world, rows->entities[i], Rotation, {50});

            test_assert( !ecs_is_empty(rows->world, rows->entities[i]));
            test_assert( ecs_has(rows->world, rows->entities[i], Rotation));
            Rotation *r = ecs_get_ptr(rows->world, rows->entities[i], Rotation);            
            test_assert(r != NULL);
            test_int(*r, 50);
        }

        ctx->entity_count ++;
    }
}

void MultiThreadStaging_stress_set_entity_random_components() {
    test_quarantine("07-09-2019");
    
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rotation);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, Set_random, EcsOnUpdate, Position);
    ECS_SYSTEM(world, Set_velocity_callback, EcsOnSet, Velocity);
    ECS_SYSTEM(world, Delete_above_1000, EcsPostUpdate, Position);

    IterData ctx = {.component = ecs_type(Position), .component_2 = ecs_type(Velocity), .component_3 = ecs_type(Rotation)};
    ecs_set_context(world, &ctx);

    ecs_new_w_count(world, Position, 500);
    ecs_new_w_count(world, Type, 500);

    ecs_set_threads(world, 6);

    int i;
    for (i = 0; i < 100; i ++) {
        ecs_progress(world, 1);
    }

    ecs_fini(world);
}

static
void InitVelocity(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Velocity, v, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        v[i].x = 10;
        v[i].y = 20;
    }
}

static
void AddVelocity(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, Velocity, 2);

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_add(rows->world, rows->entities[i], Velocity);
    }
}

void MultiThreadStaging_2_threads_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, InitVelocity, EcsOnAdd, Velocity);
    ECS_SYSTEM(world, AddVelocity, EcsOnUpdate, Position, .Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new_w_count(world, Position, 10);
    test_assert(e != 0);

    ecs_set_threads(world, 2);

    ecs_progress(world, 0);

    ecs_entity_t i;
    for (i = e; i < e + 10; i ++) {
        test_assert( ecs_has(world, i, Velocity));
        Velocity *v = ecs_get_ptr(world, i, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 10);
        test_int(v->y, 20);
    }

    ecs_fini(world);
}

static
void New_w_count(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, Position, 1);

    ecs_new_w_count(rows->world, Position, 10);
}


void MultiThreadStaging_new_w_count() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, New_w_count, EcsOnUpdate, .Position);

    ecs_set_threads(world, 2);

    ecs_progress(world, 0);

    test_int( ecs_count(world, Position), 10);

    ecs_fini(world);
}
