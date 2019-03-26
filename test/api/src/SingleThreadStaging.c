#include <include/api.h>

static
void NewEmpty(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = 0; i < rows->count; i ++) {
        ctx->new_entities[ctx->entity_count] = ecs_new(rows->world, 0);
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_new_empty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, NewEmpty, EcsOnFrame, Position);

    IterData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( ecs_empty(world, ctx.new_entities[i]));
    }

    ecs_fini(world);
}

static
void New_w_component(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t e = _ecs_new(rows->world, ctx->component);
        ctx->new_entities[ctx->entity_count] = e;

        test_assert( !ecs_empty(rows->world, e));
        test_assert( _ecs_has(rows->world, e, ctx->component));

        ctx->entity_count ++;
    }
}

void SingleThreadStaging_new_w_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, New_w_component, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Velocity)};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !ecs_empty(world, ctx.new_entities[i]));
        test_assert( ecs_has(world, ctx.new_entities[i], Velocity));
    }

    ecs_fini(world);
}

void SingleThreadStaging_new_w_type_of_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, New_w_component, EcsOnFrame, Position);

    IterData ctx = {.component = TType};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !ecs_empty(world, ctx.new_entities[i]));
        test_assert( ecs_has(world, ctx.new_entities[i], Position));
        test_assert( ecs_has(world, ctx.new_entities[i], Velocity));
    }

    ecs_fini(world);
}

static
void Add_to_new_empty(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t e = ecs_new(rows->world, 0);
        if (ctx->component) {
            _ecs_add(rows->world, e, ctx->component);
            
            test_assert( !ecs_empty(rows->world, e));
            test_assert( _ecs_has(rows->world, e, ctx->component));
            test_assert( _ecs_get_ptr(rows->world, e, ctx->component) != NULL);
        }
        if (ctx->component_2) {
            _ecs_add(rows->world, e, ctx->component_2);

            test_assert( !ecs_empty(rows->world, e));
            test_assert( _ecs_has(rows->world, e, ctx->component_2)); 
            test_assert( _ecs_get_ptr(rows->world, e, ctx->component_2) != NULL);           
        }
        ctx->new_entities[ctx->entity_count] = e;
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_add_to_new_empty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, Add_to_new_empty, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Velocity)};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !ecs_empty(world, ctx.new_entities[i]));
        test_assert( ecs_has(world, ctx.new_entities[i], Velocity));
    }

    ecs_fini(world);
}

void SingleThreadStaging_2_add_to_new_empty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, Add_to_new_empty, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Velocity), .component_2 = ecs_to_type(Rotation)};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !ecs_empty(world, ctx.new_entities[i]));
        test_assert( ecs_has(world, ctx.new_entities[i], Velocity));
        test_assert( ecs_has(world, ctx.new_entities[i], Rotation));
    }

    ecs_fini(world);
}

static
void Add_remove_same_from_new(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t e = ecs_new(rows->world, 0);

        test_assert( ecs_empty(rows->world, e));

        if (ctx->component) {
            _ecs_add(rows->world, e, ctx->component);
            test_assert( _ecs_has(rows->world, e, ctx->component));
            test_assert( _ecs_get_ptr(rows->world, e, ctx->component) != NULL);

            _ecs_remove(rows->world, e, ctx->component);
            test_assert( !_ecs_has(rows->world, e, ctx->component));
            test_assert( _ecs_get_ptr(rows->world, e, ctx->component) == NULL);
        }

        if (ctx->component_2) {
            _ecs_add(rows->world, e, ctx->component_2);
            test_assert( _ecs_has(rows->world, e, ctx->component_2));
            test_assert( _ecs_get_ptr(rows->world, e, ctx->component_2) != NULL);

            _ecs_remove(rows->world, e, ctx->component_2);
            test_assert( !_ecs_has(rows->world, e, ctx->component_2));
            test_assert( _ecs_get_ptr(rows->world, e, ctx->component_2) == NULL);
        }

        ctx->new_entities[ctx->entity_count] = e;
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_add_remove_same_to_new_empty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, Add_remove_same_from_new, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Velocity)};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( ecs_empty(world, ctx.new_entities[i]));
    }

    ecs_fini(world);
}

void SingleThreadStaging_add_remove_2_same_to_new_empty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, Add_remove_same_from_new, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Position), .component_2 = ecs_to_type(Velocity)};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( ecs_empty(world, ctx.new_entities[i]));
    }

    ecs_fini(world);
}

static
void Add_remove_same_from_new_w_component(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t e = _ecs_new(rows->world, ctx->component);
        test_assert( _ecs_has(rows->world, e, ctx->component));
        test_assert( _ecs_get_ptr(rows->world, e, ctx->component) != NULL);

        if (ctx->component_2) {
            _ecs_add(rows->world, e, ctx->component_2);
            test_assert( _ecs_has(rows->world, e, ctx->component_2));
            test_assert( _ecs_get_ptr(rows->world, e, ctx->component_2) != NULL);

            _ecs_remove(rows->world, e, ctx->component_2);
            test_assert( !_ecs_has(rows->world, e, ctx->component_2));
            test_assert( _ecs_get_ptr(rows->world, e, ctx->component_2) == NULL);
            test_assert( _ecs_get_ptr(rows->world, e, ctx->component) != NULL);
        }

        ctx->new_entities[ctx->entity_count] = e;
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_add_remove_same_to_new_w_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, Add_remove_same_from_new_w_component, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Position), .component_2 = ecs_to_type(Velocity)};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !ecs_empty(world, ctx.new_entities[i]));
        test_assert( ecs_has(world, ctx.new_entities[i], Position));
        test_assert( !ecs_has(world, ctx.new_entities[i], Velocity));
    }

    ecs_fini(world);
}

static
void Add_remove_different_from_new_empty(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t e = ecs_new(rows->world, 0);

        if (ctx->component_3) {
            _ecs_add(rows->world, e, ctx->component_3);
            test_assert( _ecs_has(rows->world, e, ctx->component_3));
            test_assert( _ecs_get_ptr(rows->world, e, ctx->component_3) != NULL);
        }

        if (ctx->component_2) {
            _ecs_remove(rows->world, e, ctx->component_2);
            test_assert( !_ecs_has(rows->world, e, ctx->component_2));
            test_assert( _ecs_get_ptr(rows->world, e, ctx->component_2) == NULL);
        }

        if (ctx->component) {
            _ecs_add(rows->world, e, ctx->component);
            test_assert( _ecs_has(rows->world, e, ctx->component));  
            test_assert( _ecs_get_ptr(rows->world, e, ctx->component) != NULL);      
        }

        ctx->new_entities[ctx->entity_count] = e;
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_2_add_1_remove_to_new_empty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, Add_remove_different_from_new_empty, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Rotation), .component_2 = ecs_to_type(Position), .component_3 = ecs_to_type(Mass)};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !ecs_empty(world, ctx.new_entities[i]));
        test_assert( ecs_has(world, ctx.new_entities[i], Rotation));
        test_assert( ecs_has(world, ctx.new_entities[i], Mass));
        test_assert( !ecs_has(world, ctx.new_entities[i], Position));
    }

    ecs_fini(world);
}

void SingleThreadStaging_2_add_1_remove_same_to_new_empty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, Add_remove_different_from_new_empty, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Rotation), .component_2 = ecs_to_type(Position), .component_3 = ecs_to_type(Position)};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !ecs_empty(world, ctx.new_entities[i]));
        test_assert( ecs_has(world, ctx.new_entities[i], Rotation));
        test_assert( !ecs_has(world, ctx.new_entities[i], Position));
    }

    ecs_fini(world);
}

static
void Clone_current(ecs_rows_t *rows) {
    ecs_entity_t *entities = ecs_column(rows, ecs_entity_t, 0);
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = 0; i < rows->count; i ++) {
        ctx->new_entities[ctx->entity_count] = ecs_clone(rows->world, entities[i], false);
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_clone() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, Clone_current, EcsOnFrame, Position);

    IterData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert(ctx.new_entities[0] != 0);
    test_assert( !ecs_empty(world, ctx.new_entities[0]));
    test_assert( ecs_has(world, ctx.new_entities[0], Position));

    test_assert(ctx.new_entities[1] != 0);
    test_assert( !ecs_empty(world, ctx.new_entities[1]));
    test_assert( ecs_has(world, ctx.new_entities[1], Position));
    test_assert( ecs_has(world, ctx.new_entities[1], Velocity));

    test_assert(ctx.new_entities[2] != 0);
    test_assert( !ecs_empty(world, ctx.new_entities[2]));
    test_assert( ecs_has(world, ctx.new_entities[2], Position));
    test_assert( ecs_has(world, ctx.new_entities[2], Mass));

    ecs_fini(world);
}

static
void Clone_current_w_value(ecs_rows_t *rows) {
    ecs_entity_t *entities = ecs_column(rows, ecs_entity_t, 0);
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = 0; i < rows->count; i ++) {
        ctx->new_entities[ctx->entity_count] = ecs_clone(rows->world, entities[i], true);
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_clone_w_value() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, Clone_current_w_value, EcsOnFrame, Position);

    ecs_set(world, e_1, Position, {10, 20});

    ecs_set(world, e_2, Position, {11, 21});
    ecs_set(world, e_2, Velocity, {30, 40});

    ecs_set(world, e_3, Position, {12, 22});
    ecs_set(world, e_3, Mass, {50});

    IterData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert(ctx.new_entities[0] != 0);
    test_assert( !ecs_empty(world, ctx.new_entities[0]));
    test_assert( ecs_has(world, ctx.new_entities[0], Position));
    test_int( ecs_get(world, ctx.new_entities[0], Position).x, 10);
    test_int( ecs_get(world, ctx.new_entities[0], Position).y, 20);

    test_assert(ctx.new_entities[1] != 0);
    test_assert( !ecs_empty(world, ctx.new_entities[1]));
    test_assert( ecs_has(world, ctx.new_entities[1], Position));
    test_assert( ecs_has(world, ctx.new_entities[1], Velocity));
    test_int( ecs_get(world, ctx.new_entities[1], Position).x, 11);
    test_int( ecs_get(world, ctx.new_entities[1], Position).y, 21);
    test_int( ecs_get(world, ctx.new_entities[1], Velocity).x, 30);
    test_int( ecs_get(world, ctx.new_entities[1], Velocity).y, 40);

    test_assert(ctx.new_entities[2] != 0);
    test_assert( !ecs_empty(world, ctx.new_entities[2]));
    test_assert( ecs_has(world, ctx.new_entities[2], Position));
    test_assert( ecs_has(world, ctx.new_entities[2], Mass));
    test_int( ecs_get(world, ctx.new_entities[2], Position).x, 12);
    test_int( ecs_get(world, ctx.new_entities[2], Position).y, 22);
    test_int( ecs_get(world, ctx.new_entities[2], Mass), 50);

    ecs_fini(world);
}

static
void Add_to_current(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    ecs_entity_t *entities = ecs_column(rows, ecs_entity_t, 0);
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

void SingleThreadStaging_add_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, Add_to_current, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Rotation)};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_1, Rotation));

    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_2, Velocity));
    test_assert( ecs_has(world, e_2, Rotation));

    test_assert( ecs_has(world, e_3, Position));
    test_assert( ecs_has(world, e_3, Mass));
    test_assert( ecs_has(world, e_3, Rotation));

    ecs_fini(world);
}

void SingleThreadStaging_2_add_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, Add_to_current, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Velocity), .component_2 = ecs_to_type(Rotation)};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_1, Velocity));
    test_assert( ecs_has(world, e_1, Rotation));

    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_2, Velocity));
    test_assert( ecs_has(world, e_2, Rotation));

    test_assert( ecs_has(world, e_3, Position));
    test_assert( ecs_has(world, e_1, Velocity));
    test_assert( ecs_has(world, e_3, Mass));
    test_assert( ecs_has(world, e_3, Rotation));

    ecs_fini(world);    
}

static
void Remove_from_current(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    ecs_entity_t *entities = ecs_column(rows, ecs_entity_t, 0);
    int i;
    for (i = 0; i < rows->count; i ++) {
        if (ctx->component) {
            _ecs_remove(rows->world, entities[i], ctx->component);
            test_assert( !_ecs_has(rows->world, entities[i], ctx->component));
            test_assert( _ecs_get_ptr(rows->world, entities[i], ctx->component) == NULL);
        }

        if (ctx->component_2) {
            _ecs_remove(rows->world, entities[i], ctx->component_2);
            test_assert( !_ecs_has(rows->world, entities[i], ctx->component_2));
            test_assert( _ecs_get_ptr(rows->world, entities[i], ctx->component_2) == NULL);
        }

        ctx->entity_count ++;
    }
}

void SingleThreadStaging_remove_from_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Remove_from_current, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    ecs_entity_t e_1 = ecs_new(world, 0);
    ecs_entity_t e_2 = ecs_new(world, 0);
    ecs_entity_t e_3 = ecs_new(world, 0);

    ecs_add(world, e_1, Position);
    ecs_add(world, e_2, Position);
    ecs_add(world, e_2, Velocity);
    ecs_add(world, e_3, Position);
    ecs_add(world, e_3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( !ecs_has(world, e_1, Position));
    test_assert( ecs_empty(world, e_1));

    test_assert( !ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_2, Velocity));

    test_assert( !ecs_has(world, e_3, Position));
    test_assert( ecs_has(world, e_3, Mass));

    ecs_fini(world);
}

void SingleThreadStaging_remove_2_from_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Remove_from_current, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Position), .component_2 = ecs_to_type(Velocity)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    ecs_entity_t e_1 = ecs_new(world, 0);
    ecs_entity_t e_2 = ecs_new(world, 0);
    ecs_entity_t e_3 = ecs_new(world, 0);

    ecs_add(world, e_1, Position);
    ecs_add(world, e_1, Velocity);
    ecs_add(world, e_2, Position);
    ecs_add(world, e_2, Velocity);
    ecs_add(world, e_3, Position);
    ecs_add(world, e_3, Velocity);
    ecs_add(world, e_3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( !ecs_has(world, e_1, Position));
    test_assert( !ecs_has(world, e_1, Velocity));
    test_assert( ecs_empty(world, e_1));

    test_assert( !ecs_has(world, e_2, Position));
    test_assert( !ecs_has(world, e_2, Velocity));
    test_assert( ecs_empty(world, e_2));

    test_assert( !ecs_has(world, e_3, Position));
    test_assert( !ecs_has(world, e_3, Velocity));
    test_assert( ecs_has(world, e_3, Mass));

    ecs_fini(world);
}

static
void Add_remove_same_from_current(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    ecs_entity_t *entities = ecs_column(rows, ecs_entity_t, 0);
    int i;
    for (i = 0; i < rows->count; i ++) {
        if (ctx->component) {
            _ecs_add(rows->world, entities[i], ctx->component);
            test_assert( _ecs_has(rows->world, entities[i], ctx->component));
            test_assert( _ecs_get_ptr(rows->world, entities[i], ctx->component) != NULL);

            _ecs_remove(rows->world, entities[i], ctx->component);
            test_assert( !_ecs_has(rows->world, entities[i], ctx->component));
            test_assert( _ecs_get_ptr(rows->world, entities[i], ctx->component) == NULL);
        }

        if (ctx->component_2) {
            _ecs_add(rows->world, entities[i], ctx->component_2);
            test_assert( _ecs_has(rows->world, entities[i], ctx->component_2));
            test_assert( _ecs_get_ptr(rows->world, entities[i], ctx->component_2) != NULL);

            _ecs_remove(rows->world, entities[i], ctx->component_2);
            test_assert( !_ecs_has(rows->world, entities[i], ctx->component_2));
            test_assert( _ecs_get_ptr(rows->world, entities[i], ctx->component_2) == NULL);
        }

        ctx->entity_count ++;
    }
}

void SingleThreadStaging_add_remove_same_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Add_remove_same_from_current, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Rotation)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    ecs_entity_t e_1 = ecs_new(world, 0);
    ecs_entity_t e_2 = ecs_new(world, 0);
    ecs_entity_t e_3 = ecs_new(world, 0);

    ecs_add(world, e_1, Position);
    ecs_add(world, e_2, Position);
    ecs_add(world, e_2, Velocity);
    ecs_add(world, e_3, Position);
    ecs_add(world, e_3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( ecs_has(world, e_1, Position));
    test_assert( !ecs_has(world, e_1, Rotation));

    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_2, Velocity));
    test_assert( !ecs_has(world, e_2, Rotation));

    test_assert( ecs_has(world, e_3, Position));
    test_assert( ecs_has(world, e_3, Mass));
    test_assert( !ecs_has(world, e_3, Rotation));

    ecs_fini(world);
}

void SingleThreadStaging_add_remove_same_existing_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Add_remove_same_from_current, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    ecs_entity_t e_1 = ecs_new(world, 0);
    ecs_entity_t e_2 = ecs_new(world, 0);
    ecs_entity_t e_3 = ecs_new(world, 0);

    ecs_add(world, e_1, Position);
    ecs_add(world, e_2, Position);
    ecs_add(world, e_2, Velocity);
    ecs_add(world, e_3, Position);
    ecs_add(world, e_3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( !ecs_has(world, e_1, Position));
    test_assert( ecs_empty(world, e_1));

    test_assert( !ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_2, Velocity));

    test_assert( !ecs_has(world, e_3, Position));
    test_assert( ecs_has(world, e_3, Mass));

    ecs_fini(world);
}

static
void Remove_add_same_from_current(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    ecs_entity_t *entities = ecs_column(rows, ecs_entity_t, 0);
    int i;
    for (i = 0; i < rows->count; i ++) {
        if (ctx->component) {
            _ecs_remove(rows->world, entities[i], ctx->component);
            test_assert( !_ecs_has(rows->world, entities[i], ctx->component));
            test_assert( _ecs_get_ptr(rows->world, entities[i], ctx->component) == NULL);

            _ecs_add(rows->world, entities[i], ctx->component);
            test_assert( _ecs_has(rows->world, entities[i], ctx->component));
            test_assert( _ecs_get_ptr(rows->world, entities[i], ctx->component) != NULL);
        }

        if (ctx->component_2) {
            _ecs_remove(rows->world, entities[i], ctx->component_2);
            test_assert( !_ecs_has(rows->world, entities[i], ctx->component_2));
            test_assert( _ecs_get_ptr(rows->world, entities[i], ctx->component_2) == NULL);

            _ecs_add(rows->world, entities[i], ctx->component_2);
            test_assert( _ecs_has(rows->world, entities[i], ctx->component_2));
            test_assert( _ecs_get_ptr(rows->world, entities[i], ctx->component_2) != NULL);
        }

        ctx->entity_count ++;
    }
}

void SingleThreadStaging_remove_add_same_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Remove_add_same_from_current, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Rotation)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    ecs_entity_t e_1 = ecs_new(world, 0);
    ecs_entity_t e_2 = ecs_new(world, 0);
    ecs_entity_t e_3 = ecs_new(world, 0);

    ecs_add(world, e_1, Position);
    ecs_add(world, e_2, Position);
    ecs_add(world, e_2, Velocity);
    ecs_add(world, e_3, Position);
    ecs_add(world, e_3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_1, Rotation));

    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_2, Velocity));
    test_assert( ecs_has(world, e_2, Rotation));

    test_assert( ecs_has(world, e_3, Position));
    test_assert( ecs_has(world, e_3, Mass));
    test_assert( ecs_has(world, e_3, Rotation));

    ecs_fini(world);
}

void SingleThreadStaging_remove_add_same_existing_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Remove_add_same_from_current, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    ecs_entity_t e_1 = ecs_new(world, 0);
    ecs_entity_t e_2 = ecs_new(world, 0);
    ecs_entity_t e_3 = ecs_new(world, 0);

    ecs_add(world, e_1, Position);
    ecs_add(world, e_2, Position);
    ecs_add(world, e_2, Velocity);
    ecs_add(world, e_3, Position);
    ecs_add(world, e_3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( ecs_has(world, e_1, Position));

    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_2, Velocity));

    test_assert( ecs_has(world, e_3, Position));
    test_assert( ecs_has(world, e_3, Mass));

    ecs_fini(world);
}

void SingleThreadStaging_add_remove_2_same_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Add_remove_same_from_current, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Rotation), .component_2 = ecs_to_type(Mass)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    ecs_entity_t e_1 = ecs_new(world, 0);
    ecs_entity_t e_2 = ecs_new(world, 0);
    ecs_entity_t e_3 = ecs_new(world, 0);

    ecs_add(world, e_1, Position);
    ecs_add(world, e_2, Position);
    ecs_add(world, e_2, Velocity);
    ecs_add(world, e_3, Position);
    ecs_add(world, e_3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( ecs_has(world, e_1, Position));
    test_assert( !ecs_has(world, e_1, Rotation));
    test_assert( !ecs_has(world, e_1, Mass));

    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_2, Velocity));
    test_assert( !ecs_has(world, e_2, Rotation));
    test_assert( !ecs_has(world, e_2, Mass));

    test_assert( ecs_has(world, e_3, Position));
    test_assert( !ecs_has(world, e_3, Mass));
    test_assert( !ecs_has(world, e_3, Rotation));

    ecs_fini(world);
}

void SingleThreadStaging_add_remove_2_same_existing_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Add_remove_same_from_current, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Position), .component_2 = ecs_to_type(Velocity)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    ecs_entity_t e_1 = ecs_new(world, 0);
    ecs_entity_t e_2 = ecs_new(world, 0);
    ecs_entity_t e_3 = ecs_new(world, 0);

    ecs_add(world, e_1, Position);
    ecs_add(world, e_1, Velocity);
    ecs_add(world, e_2, Position);
    ecs_add(world, e_2, Velocity);
    ecs_add(world, e_3, Position);
    ecs_add(world, e_3, Velocity);
    ecs_add(world, e_3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( !ecs_has(world, e_1, Position));
    test_assert( !ecs_has(world, e_1, Velocity));
    test_assert( ecs_empty(world, e_1));

    test_assert( !ecs_has(world, e_2, Position));
    test_assert( !ecs_has(world, e_2, Velocity));
    test_assert( ecs_empty(world, e_2));

    test_assert( !ecs_has(world, e_3, Position));
    test_assert( !ecs_has(world, e_3, Velocity));
    test_assert( ecs_has(world, e_3, Mass));

    ecs_fini(world);
}

void SingleThreadStaging_remove_add_2_same_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Remove_add_same_from_current, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Rotation), .component_2 = ecs_to_type(Mass)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    ecs_entity_t e_1 = ecs_new(world, 0);
    ecs_entity_t e_2 = ecs_new(world, 0);
    ecs_entity_t e_3 = ecs_new(world, 0);

    ecs_add(world, e_1, Position);
    ecs_add(world, e_2, Position);
    ecs_add(world, e_2, Velocity);
    ecs_add(world, e_3, Position);
    ecs_add(world, e_3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_1, Rotation));
    test_assert( ecs_has(world, e_1, Mass));

    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_2, Velocity));
    test_assert( ecs_has(world, e_2, Rotation));
    test_assert( ecs_has(world, e_2, Mass));

    test_assert( ecs_has(world, e_3, Position));
    test_assert( ecs_has(world, e_3, Mass));
    test_assert( ecs_has(world, e_3, Rotation));

    ecs_fini(world);
}

void SingleThreadStaging_remove_add_2_same_existing_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Remove_add_same_from_current, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Position), .component_2 = ecs_to_type(Velocity)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    ecs_entity_t e_1 = ecs_new(world, 0);
    ecs_entity_t e_2 = ecs_new(world, 0);
    ecs_entity_t e_3 = ecs_new(world, 0);

    ecs_add(world, e_1, Position);
    ecs_add(world, e_2, Position);
    ecs_add(world, e_2, Velocity);
    ecs_add(world, e_3, Position);
    ecs_add(world, e_3, Velocity);
    ecs_add(world, e_3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_1, Velocity));

    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_2, Velocity));

    test_assert( ecs_has(world, e_3, Position));
    test_assert( ecs_has(world, e_3, Velocity));
    test_assert( ecs_has(world, e_3, Mass));

    ecs_fini(world);
}

static
void Add_remove_different_from_current(ecs_rows_t *rows) {
    ecs_entity_t *entities = ecs_column(rows, ecs_entity_t, 0);
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = 0; i < rows->count; i ++) {
        if (ctx->component_3) {
            _ecs_add(rows->world, entities[i], ctx->component_3);
            test_assert( _ecs_has(rows->world, entities[i], ctx->component_3));
            test_assert( _ecs_get_ptr(rows->world, entities[i], ctx->component_3) != NULL);
        }

        if (ctx->component_2) {
            _ecs_remove(rows->world, entities[i], ctx->component_2);
            test_assert( !_ecs_has(rows->world, entities[i], ctx->component_2));
            test_assert( _ecs_get_ptr(rows->world, entities[i], ctx->component_2) == NULL);
        }

        if (ctx->component) {
            _ecs_add(rows->world, entities[i], ctx->component);
            test_assert( _ecs_has(rows->world, entities[i], ctx->component));     
            test_assert( _ecs_get_ptr(rows->world, entities[i], ctx->component) != NULL);   
        }

        ctx->entity_count ++;
    }
}

void SingleThreadStaging_add_remove_different_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Add_remove_different_from_current, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Rotation), .component_2 = ecs_to_type(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    ecs_entity_t e_1 = ecs_new(world, 0);
    ecs_entity_t e_2 = ecs_new(world, 0);
    ecs_entity_t e_3 = ecs_new(world, 0);

    ecs_add(world, e_1, Position);
    ecs_add(world, e_2, Position);
    ecs_add(world, e_2, Velocity);
    ecs_add(world, e_3, Position);
    ecs_add(world, e_3, Velocity);
    ecs_add(world, e_3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( !ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_1, Rotation));

    test_assert( !ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_2, Velocity));
    test_assert( ecs_has(world, e_2, Rotation));

    test_assert( !ecs_has(world, e_3, Position));
    test_assert( ecs_has(world, e_3, Velocity));
    test_assert( ecs_has(world, e_3, Mass));
    test_assert( ecs_has(world, e_3, Rotation));

    ecs_fini(world);
}

void SingleThreadStaging_2_add_1_remove_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Add_remove_different_from_current, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Rotation), .component_2 = ecs_to_type(Position), .component_3 = ecs_to_type(Mass)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    ecs_entity_t e_1 = ecs_new(world, 0);
    ecs_entity_t e_2 = ecs_new(world, 0);
    ecs_entity_t e_3 = ecs_new(world, 0);

    ecs_add(world, e_1, Position);
    ecs_add(world, e_2, Position);
    ecs_add(world, e_2, Velocity);
    ecs_add(world, e_3, Position);
    ecs_add(world, e_3, Velocity);
    ecs_add(world, e_3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( !ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_1, Mass));
    test_assert( ecs_has(world, e_1, Rotation));

    test_assert( !ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_2, Velocity));
    test_assert( ecs_has(world, e_2, Mass));
    test_assert( ecs_has(world, e_2, Rotation));

    test_assert( !ecs_has(world, e_3, Position));
    test_assert( ecs_has(world, e_3, Velocity));
    test_assert( ecs_has(world, e_3, Mass));
    test_assert( ecs_has(world, e_3, Rotation));

    ecs_fini(world);
}

static
void Add_1_remove_2_different_from_current(ecs_rows_t *rows) {
    ecs_entity_t *entities = ecs_column(rows, ecs_entity_t, 0);
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = 0; i < rows->count; i ++) {
        if (ctx->component) {
            _ecs_add(rows->world, entities[i], ctx->component);
            test_assert( _ecs_has(rows->world, entities[i], ctx->component)); 
            test_assert( _ecs_get_ptr(rows->world, entities[i], ctx->component) != NULL);       
        }

        if (ctx->component_2) {
            _ecs_remove(rows->world, entities[i], ctx->component_2);
            test_assert( !_ecs_has(rows->world, entities[i], ctx->component_2));
            test_assert( _ecs_get_ptr(rows->world, entities[i], ctx->component_2) == NULL);
        }

        if (ctx->component_3) {
            _ecs_remove(rows->world, entities[i], ctx->component_3);
            test_assert( !_ecs_has(rows->world, entities[i], ctx->component_3));
            test_assert( _ecs_get_ptr(rows->world, entities[i], ctx->component_3) == NULL);
        }

        ctx->entity_count ++;
    }
}

void SingleThreadStaging_1_add_2_remove_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Add_1_remove_2_different_from_current, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Rotation), .component_2 = ecs_to_type(Position), .component_3 = ecs_to_type(Velocity)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    ecs_entity_t e_1 = ecs_new(world, 0);
    ecs_entity_t e_2 = ecs_new(world, 0);
    ecs_entity_t e_3 = ecs_new(world, 0);

    ecs_add(world, e_1, Position);
    ecs_add(world, e_2, Position);
    ecs_add(world, e_2, Velocity);
    ecs_add(world, e_3, Position);
    ecs_add(world, e_3, Velocity);
    ecs_add(world, e_3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( !ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_1, Rotation));

    test_assert( !ecs_has(world, e_2, Position));
    test_assert( !ecs_has(world, e_2, Velocity));
    test_assert( ecs_has(world, e_2, Rotation));

    test_assert( !ecs_has(world, e_3, Position));
    test_assert( !ecs_has(world, e_3, Velocity));
    test_assert( ecs_has(world, e_3, Mass));
    test_assert( ecs_has(world, e_3, Rotation));

    ecs_fini(world);
}

static
void Delete_current(ecs_rows_t *rows) {
    ecs_entity_t *entities = ecs_column(rows, ecs_entity_t, 0);
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_delete(rows->world, entities[i]);
        test_assert( ecs_empty(rows->world, entities[i]));
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_delete_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, Delete_current, EcsOnFrame, Position);

    IterData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( ecs_empty(world, e_1));
    test_assert( ecs_empty(world, e_2));
    test_assert( ecs_empty(world, e_3));

    ecs_fini(world);
}

static
void Delete_even(ecs_rows_t *rows) {
    ecs_entity_t *entities = ecs_column(rows, ecs_entity_t, 0);
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = 0; i < rows->count; i ++) {
        if (!(entities[i] % 2)) {
            ecs_delete(rows->world, entities[i]);
            test_assert( ecs_empty(rows->world, entities[i]));
        }
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_delete_even() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);
    ECS_ENTITY(world, e_3, Position, Velocity);
    ECS_ENTITY(world, e_4, Position, Velocity);
    ECS_ENTITY(world, e_5, Position, Mass);
    ECS_ENTITY(world, e_6, Position, Mass);
    ECS_ENTITY(world, e_7, Position, Rotation);
    ECS_ENTITY(world, e_8, Position, Rotation);
    ECS_SYSTEM(world, Delete_even, EcsOnFrame, Position);

    IterData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 8);

    if (!(e_1 % 2))
        test_assert( ecs_empty(world, e_1));
    else
        test_assert ( !ecs_empty(world, e_1));

    if (!(e_2 % 2))
        test_assert( ecs_empty(world, e_2));
    else
        test_assert ( !ecs_empty(world, e_2));

    if (!(e_3 % 2))
        test_assert( ecs_empty(world, e_3));
    else
        test_assert ( !ecs_empty(world, e_3));

    if (!(e_4 % 2))
        test_assert( ecs_empty(world, e_4));
    else
        test_assert ( !ecs_empty(world, e_4));

    if (!(e_5 % 2))
        test_assert( ecs_empty(world, e_5));
    else
        test_assert ( !ecs_empty(world, e_5));

    if (!(e_6 % 2))
        test_assert( ecs_empty(world, e_6));
    else
        test_assert ( !ecs_empty(world, e_6));

    if (!(e_7 % 2))
        test_assert( ecs_empty(world, e_7));
    else
        test_assert ( !ecs_empty(world, e_7));

    if (!(e_8 % 2))
        test_assert( ecs_empty(world, e_8));
    else
        test_assert ( !ecs_empty(world, e_8));

    ecs_fini(world);
}

static
void Delete_new_empty(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t e = ecs_new(rows->world, 0);
        ecs_delete(rows->world, e);
        test_assert( ecs_empty(rows->world, e));

        ctx->new_entities[ctx->entity_count] = e;
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_delete_new_empty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, Delete_new_empty, EcsOnFrame, Position);

    IterData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( ecs_empty(world, ctx.new_entities[i]));
    }

    ecs_fini(world);
}

static
void Delete_new_w_component(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t e = _ecs_new(rows->world, ctx->component);
        test_assert( !ecs_empty(rows->world, e));

        ecs_delete(rows->world, e);
        test_assert( ecs_empty(rows->world, e));

        ctx->new_entities[ctx->entity_count] = e;
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_delete_new_w_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, Delete_new_w_component, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Position)};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( ecs_empty(world, ctx.new_entities[i]));
    }

    ecs_fini(world);
}

static
void Set_current(ecs_rows_t *rows) {
    ecs_entity_t *entities = ecs_column(rows, ecs_entity_t, 0);
    IterData *ctx = ecs_get_context(rows->world);
    
    ecs_type_t ecs_to_type(Rotation) = ctx->component;

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_set(rows->world, entities[i], Rotation, {10 + entities[i]});
        test_assert( ecs_has(rows->world, entities[i], Rotation));
        test_assert( ecs_get_ptr(rows->world, entities[i], Rotation) != NULL);
        test_int( ecs_get(rows->world, entities[i], Rotation), 10 + entities[i]);

        ctx->entity_count ++;
    }
}

void SingleThreadStaging_set_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Set_current, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Rotation)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    ecs_entity_t e_1 = ecs_new(world, 0);
    ecs_entity_t e_2 = ecs_new(world, 0);
    ecs_entity_t e_3 = ecs_new(world, 0);

    ecs_add(world, e_1, Position);
    ecs_add(world, e_2, Position);
    ecs_add(world, e_2, Velocity);
    ecs_add(world, e_3, Position);
    ecs_add(world, e_3, Velocity);
    ecs_add(world, e_3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_1, Rotation));
    test_int( ecs_get(world, e_1, Rotation), e_1 + 10);

    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_2, Velocity));
    test_assert( ecs_has(world, e_2, Rotation));
    test_int( ecs_get(world, e_2, Rotation), e_2 + 10);

    test_assert( ecs_has(world, e_3, Position));
    test_assert( ecs_has(world, e_3, Velocity));
    test_assert( ecs_has(world, e_3, Mass));
    test_assert( ecs_has(world, e_3, Rotation));
    test_int( ecs_get(world, e_3, Rotation), e_3 + 10);

    ecs_fini(world);
}

static
void Set_new_empty(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    
    ecs_type_t ecs_to_type(Rotation) = ctx->component;

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t e = ecs_new(rows->world, 0);
        test_assert( ecs_empty(rows->world, e));

        ecs_set(rows->world, e, Rotation, {10 + e});
        test_assert( ecs_has(rows->world, e, Rotation));
        test_assert( ecs_get_ptr(rows->world, e, Rotation) != NULL);
        test_int( ecs_get(rows->world, e, Rotation), 10 + e);

        ctx->new_entities[ctx->entity_count] = e;
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_set_new_empty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Set_new_empty, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Rotation)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    ecs_entity_t e_1 = ecs_new(world, 0);
    ecs_entity_t e_2 = ecs_new(world, 0);
    ecs_entity_t e_3 = ecs_new(world, 0);

    ecs_add(world, e_1, Position);
    ecs_add(world, e_2, Position);
    ecs_add(world, e_2, Velocity);
    ecs_add(world, e_3, Position);
    ecs_add(world, e_3, Velocity);
    ecs_add(world, e_3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !ecs_empty(world, ctx.new_entities[i]));
        test_assert( ecs_has(world, ctx.new_entities[i], Rotation));
        test_int( ecs_get(world, ctx.new_entities[i], Rotation), ctx.new_entities[i] + 10);
    }

    ecs_fini(world);
}

static
void Set_new_w_component(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    
    ecs_type_t ecs_to_type(Position) = ctx->component;
    ecs_type_t ecs_to_type(Rotation) = ctx->component_2;

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t e = ecs_new(rows->world, Position);
        test_assert( ecs_has(rows->world, e, Position));

        ecs_set(rows->world, e, Rotation, {10 + e});
        test_assert( ecs_has(rows->world, e, Position));
        test_assert( ecs_has(rows->world, e, Rotation));
        test_assert( ecs_get_ptr(rows->world, e, Rotation) != NULL);
        test_int( ecs_get(rows->world, e, Rotation), 10 + e);

        ctx->new_entities[ctx->entity_count] = e;
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_set_new_w_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Set_new_w_component, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Position), .component_2 = ecs_to_type(Rotation)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    ecs_entity_t e_1 = ecs_new(world, 0);
    ecs_entity_t e_2 = ecs_new(world, 0);
    ecs_entity_t e_3 = ecs_new(world, 0);

    ecs_add(world, e_1, Position);
    ecs_add(world, e_2, Position);
    ecs_add(world, e_2, Velocity);
    ecs_add(world, e_3, Position);
    ecs_add(world, e_3, Velocity);
    ecs_add(world, e_3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !ecs_empty(world, ctx.new_entities[i]));
        test_assert( ecs_has(world, ctx.new_entities[i], Position));
        test_assert( ecs_has(world, ctx.new_entities[i], Rotation));
        test_int( ecs_get(world, ctx.new_entities[i], Rotation), ctx.new_entities[i] + 10);
    }

    ecs_fini(world);
}

static
void Set_existing_new_w_component(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    
    ecs_type_t ecs_to_type(Position) = ctx->component;

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t e = ecs_new(rows->world, Position);
        test_assert( ecs_has(rows->world, e, Position));

        ecs_set(rows->world, e, Position, {10 + e, 20 + e});
        test_assert( ecs_has(rows->world, e, Position));

        ctx->new_entities[ctx->entity_count] = e;
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_set_existing_new_w_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Set_existing_new_w_component, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    ecs_entity_t e_1 = ecs_new(world, 0);
    ecs_entity_t e_2 = ecs_new(world, 0);
    ecs_entity_t e_3 = ecs_new(world, 0);

    ecs_add(world, e_1, Position);
    ecs_add(world, e_2, Position);
    ecs_add(world, e_2, Velocity);
    ecs_add(world, e_3, Position);
    ecs_add(world, e_3, Velocity);
    ecs_add(world, e_3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !ecs_empty(world, ctx.new_entities[i]));
        test_assert( ecs_has(world, ctx.new_entities[i], Position));
        test_int( ecs_get(world, ctx.new_entities[i], Position).x, ctx.new_entities[i] + 10);
        test_int( ecs_get(world, ctx.new_entities[i], Position).y, ctx.new_entities[i] + 20);
    }

    ecs_fini(world);
}

static
void Set_new_after_add(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    
    ecs_type_t ecs_to_type(Position) = ctx->component;

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t e = ecs_new(rows->world, 0);
        test_assert( ecs_empty(rows->world, e));

        ecs_add(rows->world, e, Position);
        test_assert( ecs_has(rows->world, e, Position));

        ecs_set(rows->world, e, Position, {10 + e, 20 + e});
        test_assert( ecs_has(rows->world, e, Position));

        ctx->new_entities[ctx->entity_count] = e;
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_set_new_after_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Set_new_after_add, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    ecs_entity_t e_1 = ecs_new(world, 0);
    ecs_entity_t e_2 = ecs_new(world, 0);
    ecs_entity_t e_3 = ecs_new(world, 0);

    ecs_add(world, e_1, Position);
    ecs_add(world, e_2, Position);
    ecs_add(world, e_2, Velocity);
    ecs_add(world, e_3, Position);
    ecs_add(world, e_3, Velocity);
    ecs_add(world, e_3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !ecs_empty(world, ctx.new_entities[i]));
        test_assert( ecs_has(world, ctx.new_entities[i], Position));
        test_int( ecs_get(world, ctx.new_entities[i], Position).x, ctx.new_entities[i] + 10);
        test_int( ecs_get(world, ctx.new_entities[i], Position).y, ctx.new_entities[i] + 20);
    }

    ecs_fini(world);
}

static
void Remove_after_set(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    
    ecs_type_t ecs_to_type(Position) = ctx->component;

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t e = ecs_new(rows->world, 0);
        test_assert( ecs_empty(rows->world, e));

        ecs_set(rows->world, e, Position, {10 + e, 20 + e});
        test_assert( ecs_has(rows->world, e, Position));
        test_assert( ecs_get_ptr(rows->world, e, Position) != NULL);
        test_int( ecs_get(rows->world, e, Position).x, 10 + e);
        test_int( ecs_get(rows->world, e, Position).y, 20 + e);

        ecs_remove(rows->world, e, Position);
        test_assert( ecs_empty(rows->world, e));
        test_assert( ecs_get_ptr(rows->world, e, Position) == NULL);
        
        ctx->new_entities[ctx->entity_count] = e;
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_remove_after_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Remove_after_set, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    ecs_entity_t e_1 = ecs_new(world, 0);
    ecs_entity_t e_2 = ecs_new(world, 0);
    ecs_entity_t e_3 = ecs_new(world, 0);

    ecs_add(world, e_1, Position);
    ecs_add(world, e_2, Position);
    ecs_add(world, e_2, Velocity);
    ecs_add(world, e_3, Position);
    ecs_add(world, e_3, Velocity);
    ecs_add(world, e_3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !ecs_has(world, ctx.new_entities[i], Position));
        test_assert( ecs_empty(world, ctx.new_entities[i]));
    }

    ecs_fini(world);
}

static
void Delete_after_set(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    
    ecs_type_t ecs_to_type(Position) = ctx->component;

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t e = ecs_new(rows->world, 0);
        test_assert( ecs_empty(rows->world, e));

        ecs_set(rows->world, e, Position, {10 + e, 20 + e});
        test_assert( ecs_has(rows->world, e, Position));
        test_assert( ecs_get_ptr(rows->world, e, Position) != NULL);
        test_int( ecs_get(rows->world, e, Position).x, 10 + e);
        test_int( ecs_get(rows->world, e, Position).y, 20 + e);

        ecs_delete(rows->world, e);
        test_assert( ecs_empty(rows->world, e));
        test_assert( ecs_get_ptr(rows->world, e, Position) == NULL);

        ctx->new_entities[ctx->entity_count] = e;
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_delete_after_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Delete_after_set, EcsOnFrame, Position);

    IterData ctx = {.component = ecs_to_type(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    ecs_entity_t e_1 = ecs_new(world, 0);
    ecs_entity_t e_2 = ecs_new(world, 0);
    ecs_entity_t e_3 = ecs_new(world, 0);

    ecs_add(world, e_1, Position);
    ecs_add(world, e_2, Position);
    ecs_add(world, e_2, Velocity);
    ecs_add(world, e_3, Position);
    ecs_add(world, e_3, Velocity);
    ecs_add(world, e_3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !ecs_has(world, ctx.new_entities[i], Position));
        test_assert( ecs_empty(world, ctx.new_entities[i]));
    }

    ecs_fini(world);
}


void SingleThreadStaging_add_to_current_in_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_SYSTEM(world, Add_to_current, EcsOnAdd, Position);

    IterData ctx = {.component = ecs_to_type(Velocity)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    ecs_entity_t e_1 = ecs_new(world, Position);
    ecs_entity_t e_2 = ecs_new(world, Position);
    ecs_entity_t e_3 = ecs_new(world, Position);

    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_3, Position));

    test_assert( ecs_has(world, e_1, Velocity));
    test_assert( ecs_has(world, e_2, Velocity));
    test_assert( ecs_has(world, e_3, Velocity));

    ecs_fini(world);
}

void SingleThreadStaging_remove_from_current_in_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_SYSTEM(world, Remove_from_current, EcsOnAdd, Position);

    IterData ctx = {.component = ecs_to_type(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    ecs_entity_t e_1 = ecs_new(world, Type);
    ecs_entity_t e_2 = ecs_new(world, Type);
    ecs_entity_t e_3 = ecs_new(world, Type);

    test_assert( !ecs_has(world, e_1, Position));
    test_assert( !ecs_has(world, e_2, Position));
    test_assert( !ecs_has(world, e_3, Position));

    test_assert( ecs_has(world, e_1, Velocity));
    test_assert( ecs_has(world, e_2, Velocity));
    test_assert( ecs_has(world, e_3, Velocity));

    ecs_fini(world);
}

void SingleThreadStaging_add_to_current_in_on_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_SYSTEM(world, Add_to_current, EcsOnSet, Position);

    IterData ctx = {.component = ecs_to_type(Velocity)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    ecs_entity_t e_1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e_2 = ecs_set(world, 0, Position, {11, 21});
    ecs_entity_t e_3 = ecs_set(world, 0, Position, {12, 22});

    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_3, Position));

    test_assert( ecs_has(world, e_1, Velocity));
    test_assert( ecs_has(world, e_2, Velocity));
    test_assert( ecs_has(world, e_3, Velocity));

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_2, Position);
    test_int(p->x, 11);
    test_int(p->y, 21);

    p = ecs_get_ptr(world, e_3, Position);
    test_int(p->x, 12);
    test_int(p->y, 22);

    ecs_fini(world);
}

void SingleThreadStaging_remove_from_current_in_on_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_SYSTEM(world, Remove_from_current, EcsOnSet, Position);

    IterData ctx = {.component = ecs_to_type(Velocity)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    ecs_entity_t e_1 = ecs_new(world, Type);
    ecs_entity_t e_2 = ecs_new(world, Type);
    ecs_entity_t e_3 = ecs_new(world, Type);

    e_1 = ecs_set(world, e_1, Position, {10, 20});
    e_2 = ecs_set(world, e_2, Position, {11, 21});
    e_3 = ecs_set(world, e_3, Position, {12, 22});

    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_3, Position));

    test_assert( !ecs_has(world, e_1, Velocity));
    test_assert( !ecs_has(world, e_2, Velocity));
    test_assert( !ecs_has(world, e_3, Velocity));

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_2, Position);
    test_int(p->x, 11);
    test_int(p->y, 21);

    p = ecs_get_ptr(world, e_3, Position);
    test_int(p->x, 12);
    test_int(p->y, 22);

    ecs_fini(world);
}

void SingleThreadStaging_remove_set_component_in_on_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_SYSTEM(world, Remove_from_current, EcsOnSet, Position);

    IterData ctx = {.component = ecs_to_type(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    ecs_entity_t e_1 = ecs_new(world, Type);
    ecs_entity_t e_2 = ecs_new(world, Type);
    ecs_entity_t e_3 = ecs_new(world, Type);

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

void SingleThreadStaging_remove_added_component_in_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_SYSTEM(world, Remove_from_current, EcsOnAdd, Position);

    IterData ctx = {.component = ecs_to_type(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    ecs_entity_t e_1 = ecs_new(world, Type);
    ecs_entity_t e_2 = ecs_new(world, Type);
    ecs_entity_t e_3 = ecs_new(world, Type);
    
    test_assert( !ecs_has(world, e_1, Position));
    test_assert( !ecs_has(world, e_2, Position));
    test_assert( !ecs_has(world, e_3, Position));

    test_assert( ecs_has(world, e_1, Velocity));
    test_assert( ecs_has(world, e_2, Velocity));
    test_assert( ecs_has(world, e_3, Velocity));

    ecs_fini(world);
}

void SingleThreadStaging_remove_added_component_in_on_add_w_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_SYSTEM(world, Remove_from_current, EcsOnAdd, Position);

    IterData ctx = {.component = ecs_to_type(Position)};
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
    ecs_entity_t *entities = ecs_column(rows, ecs_entity_t, 0);
    int i;

    for (i = 0; i < rows->count; i ++) {
        if (ctx->component_3) {
            _ecs_add(rows->world, entities[i], ctx->component_3);

            test_assert( !ecs_empty(rows->world, entities[i]));
            test_assert( _ecs_has(rows->world, entities[i], ctx->component_3)); 
            test_assert( _ecs_get_ptr(rows->world, entities[i], ctx->component_3) != NULL);           
        }
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_on_add_in_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_SYSTEM(world, Add_to_current, EcsOnAdd, Position);
    ECS_SYSTEM(world, Add_3_to_current, EcsOnAdd, Velocity);

    IterData ctx = {.component = ecs_to_type(Velocity), .component_3 = ecs_to_type(Mass)};
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

void SingleThreadStaging_on_remove_in_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, Remove_from_current, EcsOnAdd, Position);
    ECS_SYSTEM(world, Dummy, EcsOnRemove, Velocity);

    IterData ctx = {.component = ecs_to_type(Velocity)};
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

void SingleThreadStaging_on_set_in_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Rotation);
    ECS_COMPONENT(world, Mass);

    ECS_SYSTEM(world, Set_current, EcsOnAdd, Position);
    ECS_SYSTEM(world, Add_3_to_current, EcsOnSet, Rotation);

    IterData ctx = {.component = ecs_to_type(Rotation), .component_3 = ecs_to_type(Mass)};
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

    Rotation *r = ecs_get_ptr(world, e_1, Rotation);
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

void SingleThreadStaging_on_add_in_on_frame() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_SYSTEM(world, Add_to_current, EcsOnFrame, Position);
    ECS_SYSTEM(world, Add_3_to_current, EcsOnAdd, Velocity);

    IterData ctx = {.component = ecs_to_type(Velocity), .component_3 = ecs_to_type(Mass)};
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

void SingleThreadStaging_on_remove_in_on_frame() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, Remove_from_current, EcsOnFrame, Position);
    ECS_SYSTEM(world, Dummy, EcsOnRemove, Velocity);

    IterData ctx = {.component = ecs_to_type(Velocity)};
    ecs_set_context(world, &ctx);

    ecs_entity_t e_1 = ecs_new(world, Type);
    ecs_entity_t e_2 = ecs_new(world, Type);
    ecs_entity_t e_3 = ecs_new(world, Type);

    ecs_progress(world, 1);

    test_assert( ecs_has(world, e_1, Position));
    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_3, Position));

    test_assert( !ecs_has(world, e_1, Velocity));
    test_assert( !ecs_has(world, e_2, Velocity));
    test_assert( !ecs_has(world, e_3, Velocity));

    test_assert(dummy_called);

    ecs_fini(world);
}

static
void On_PV(ecs_rows_t *rows) {
    Position *p = ecs_column(rows, Position, 1);
    Velocity *v = ecs_column(rows, Velocity, 2);

    ProbeSystem(rows);

    int i;
    for (i = 0; i < rows->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}

void SingleThreadStaging_match_table_created_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_SYSTEM(world, Add_to_current, EcsOnFrame, Position, !Velocity);
    ECS_SYSTEM(world, On_PV, EcsOnFrame, Position, Velocity);

    IterData add_ctx = {.component = ecs_to_type(Velocity)};
    ecs_set_context(world, &add_ctx);

    ecs_entity_t e_1 = ecs_new(world, Position);
    ecs_entity_t e_2 = ecs_new(world, Position);
    ecs_entity_t e_3 = ecs_new(world, Position);

    ecs_progress(world, 1);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, On_PV);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_2);
    test_int(ctx.e[1], e_3);
    test_int(ctx.e[2], e_1);
    test_int(ctx.c[0][0], ecs_to_entity(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_to_entity(Velocity));
    test_int(ctx.s[0][1], 0);

    ecs_fini(world);
}

void SingleThreadStaging_match_table_created_w_add_in_on_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_SYSTEM(world, Add_to_current, EcsOnSet, Position);
    ECS_SYSTEM(world, On_PV, EcsOnFrame, Position, Velocity);

    IterData add_ctx = {.component = ecs_to_type(Velocity)};
    ecs_set_context(world, &add_ctx);

    ecs_entity_t e_1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e_2 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e_3 = ecs_set(world, 0, Position, {10, 20});

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, On_PV);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_3);
    test_int(ctx.c[0][0], ecs_to_entity(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_to_entity(Velocity));
    test_int(ctx.s[0][1], 0);

    ecs_fini(world);
}

static
void Set_velocity(ecs_rows_t *rows) {
    ecs_entity_t *entities = ecs_column(rows, ecs_entity_t, 0);

    IterData *ctx = ecs_get_context(rows->world);
    ecs_type_t ecs_to_type(Velocity) = ctx->component;

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_set(rows->world, entities[i], Velocity, {10, 20});
    }
}

void SingleThreadStaging_match_table_created_w_set_in_on_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, Set_velocity, EcsOnSet, Position);
    ECS_SYSTEM(world, On_PV, EcsOnFrame, Position, Velocity);

    IterData add_ctx = {.component = ecs_to_type(Velocity)};
    ecs_set_context(world, &add_ctx);

    ecs_entity_t e_1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e_2 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e_3 = ecs_set(world, 0, Position, {10, 20});

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, On_PV);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_3);
    test_int(ctx.c[0][0], ecs_to_entity(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_to_entity(Velocity));
    test_int(ctx.s[0][1], 0);

    ecs_fini(world);
}

static
void Set_velocity_on_new(ecs_rows_t *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    ecs_type_t ecs_to_type(Velocity) = ctx->component;

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_set(rows->world, 0, Velocity, {10, 20});
    }
}

static
void On_V(ecs_rows_t *rows) {
    Velocity *v = ecs_column(rows, Velocity, 1);

    ProbeSystem(rows);

    int i;
    for (i = 0; i < rows->count; i ++) {
        v[i].x = v[i].y;
    }
}

void SingleThreadStaging_match_table_created_w_new_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, Set_velocity_on_new, EcsOnFrame, Position);
    ECS_SYSTEM(world, On_V, EcsOnFrame, Velocity);

    IterData add_ctx = {.component = ecs_to_type(Velocity)};
    ecs_set_context(world, &add_ctx);

    ecs_set(world, 0, Position, {10, 20});

    ecs_progress(world, 1);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);
    ecs_enable(world, Set_velocity_on_new, false);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, On_V);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.c[0][0], ecs_to_entity(Velocity));
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}


void SingleThreadStaging_match_table_created_w_new_in_on_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, Set_velocity_on_new, EcsOnSet, Position);
    ECS_SYSTEM(world, On_V, EcsOnFrame, Velocity);

    IterData add_ctx = {.component = ecs_to_type(Velocity)};
    ecs_set_context(world, &add_ctx);

    ecs_set(world, 0, Position, {10, 20});

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);
    ecs_enable(world, Set_velocity_on_new, false);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, On_V);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.c[0][0], ecs_to_entity(Velocity));
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}

static
void Create_container(ecs_rows_t *rows) {
    ecs_entity_t *entities = ecs_column(rows, ecs_entity_t, 0);

    ProbeSystem(rows);

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_new_child(rows->world, entities[i], NULL, 0);
    }
}

void SingleThreadStaging_merge_table_w_container_added_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Create_container, EcsOnFrame, Position);

    /* Entity is not yet a container. Adding this entity to another entity would
     * cause an error */
    ECS_ENTITY(world, e_1, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    /* Entity will become a container. A child component is stored in a new
     * archetype that includes e_1 in its component list. However, e_1 doesn't
     * have the EcsContainer tag yet in the main stage. This test ensures that
     * the table can be created. */
    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Create_container);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.c[0][0], ecs_to_entity(Position));
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}

void SingleThreadStaging_merge_table_w_container_added_on_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Create_container, EcsOnSet, Position);

    /* Entity is not yet a container. Adding this entity to another entity would
     * cause an error */
    ECS_ENTITY(world, e_1, 0);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_set(world, e_1, Position, {10, 20});

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Create_container);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.c[0][0], ecs_to_entity(Position));
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}

/* Hacky way to test if entities are properly merged by system */
static ecs_entity_t g_child = 0;
static ecs_entity_t g_parent = 0;

static
void Create_container_reverse(ecs_rows_t *rows) {

    ProbeSystem(rows);

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t child = ecs_new(rows->world, 0);
        ecs_entity_t parent = ecs_new(rows->world, 0);
        test_assert(child != 0);
        test_assert(parent != 0);

        ecs_adopt(rows->world, parent, child);
        test_assert( ecs_contains(rows->world, parent, child));

        g_parent = parent;
        g_child = child;
    }
}

void SingleThreadStaging_merge_table_w_container_added_on_set_reverse() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Create_container_reverse, EcsOnSet, Position);

    /* Entity is not yet a container. Adding this entity to another entity would
     * cause an error */
    ECS_ENTITY(world, e_1, 0);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_set(world, e_1, Position, {10, 20});

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Create_container_reverse);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.c[0][0], ecs_to_entity(Position));
    test_int(ctx.s[0][0], 0);

    test_assert(g_parent != 0);
    test_assert(g_child != 0);

    test_assert( ecs_contains(world, g_parent, g_child));
    test_assert( ecs_has(world, g_parent, EcsContainer));

    ecs_fini(world);
}
