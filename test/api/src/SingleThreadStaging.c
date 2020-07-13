#include <api.h>

static
void NewEmpty(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);

    int i;
    for (i = 0; i < it->count; i ++) {
        ctx->new_entities[ctx->entity_count] = ecs_new(it->world, 0);
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
    ECS_SYSTEM(world, NewEmpty, EcsOnUpdate, Position);

    IterData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !ecs_get_type(world, ctx.new_entities[i]));
    }

    ecs_fini(world);
}

static
void New_w_component(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_type_t type = ecs_type_from_entity(it->world, ctx->component);
        ecs_entity_t e = ecs_new_w_type(it->world, type);
        ctx->new_entities[ctx->entity_count] = e;
        test_assert( !!ecs_get_type(it->world, e));
        test_assert( ecs_has_type(it->world, e, type));

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
    ECS_SYSTEM(world, New_w_component, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Velocity)};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !!ecs_get_type(world, ctx.new_entities[i]));
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
    ECS_SYSTEM(world, New_w_component, EcsOnUpdate, Position);

    IterData ctx = {.component = Type};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !!ecs_get_type(world, ctx.new_entities[i]));
        test_assert( ecs_has(world, ctx.new_entities[i], Position));
        test_assert( ecs_has(world, ctx.new_entities[i], Velocity));
    }

    ecs_fini(world);
}

static
void NewEmpty_w_count(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);

    ctx->new_entities[ctx->entity_count] = ecs_bulk_new(it->world, 0, 1000);
    ctx->entity_count ++;
}

void SingleThreadStaging_new_empty_w_count() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, e_1, Position);
    ECS_SYSTEM(world, NewEmpty_w_count, EcsOnUpdate, Position);

    IterData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 1);
    test_assert(ctx.new_entities[0] != 0);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert( !ecs_get_type(world, ctx.new_entities[0] + i));
    }

    ecs_fini(world);
}

static
void New_w_component_w_count(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    
    ecs_type_t type = ecs_type_from_entity(it->world, ctx->component);
    ctx->new_entities[ctx->entity_count] = ecs_bulk_new_w_type(it->world, type, 1000);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert( 
            ecs_has_type(it->world, ctx->new_entities[ctx->entity_count] + i, 
                type));
    }

    ctx->entity_count ++;
}

void SingleThreadStaging_new_component_w_count() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, e_1, Position);
    ECS_SYSTEM(world, New_w_component_w_count, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Position)};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 1);
    test_assert(ctx.new_entities[0] != 0);
    test_int(ecs_count(world, Position), 1001);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert( ecs_has(world, ctx.new_entities[0] + i, Position));
    }

    ecs_fini(world);
}

void SingleThreadStaging_new_type_w_count() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_ENTITY(world, e_1, Position);
    ECS_SYSTEM(world, New_w_component_w_count, EcsOnUpdate, Position);

    IterData ctx = {.component = Type};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 1);
    test_assert(ctx.new_entities[0] != 0);
    test_int(ecs_count(world, Type), 1000);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert( ecs_has(world, ctx.new_entities[0] + i, Type));
    }

    ecs_fini(world);
}

static
void Add_to_new_empty(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = ecs_new(it->world, 0);
        if (ctx->component) {
            ecs_add_entity(it->world, e, ctx->component);
            
            test_assert( !!ecs_get_type(it->world, e));
            test_assert( ecs_has_entity(it->world, e, ctx->component));
            test_assert( ecs_get_w_entity(it->world, e, ctx->component) != NULL);
        }
        if (ctx->component_2) {
            ecs_add_entity(it->world, e, ctx->component_2);

            test_assert( !!ecs_get_type(it->world, e));
            test_assert( ecs_has_entity(it->world, e, ctx->component_2)); 
            test_assert( ecs_get_w_entity(it->world, e, ctx->component_2) != NULL);           
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
    ECS_SYSTEM(world, Add_to_new_empty, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Velocity)};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !!ecs_get_type(world, ctx.new_entities[i]));
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
    ECS_SYSTEM(world, Add_to_new_empty, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Velocity), .component_2 = ecs_entity(Rotation)};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !!ecs_get_type(world, ctx.new_entities[i]));
        test_assert( ecs_has(world, ctx.new_entities[i], Velocity));
        test_assert( ecs_has(world, ctx.new_entities[i], Rotation));
    }

    ecs_fini(world);
}

static
void Add_remove_same_from_new(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = ecs_new(it->world, 0);

        test_assert( !ecs_get_type(it->world, e));

        if (ctx->component) {
            ecs_add_entity(it->world, e, ctx->component);
            test_assert( ecs_has_entity(it->world, e, ctx->component));
            test_assert( ecs_get_w_entity(it->world, e, ctx->component) != NULL);

            ecs_remove_entity(it->world, e, ctx->component);
            test_assert( !ecs_has_entity(it->world, e, ctx->component));
            test_assert( ecs_get_w_entity(it->world, e, ctx->component) == NULL);
        }

        if (ctx->component_2) {
            ecs_add_entity(it->world, e, ctx->component_2);
            test_assert( ecs_has_entity(it->world, e, ctx->component_2));
            test_assert( ecs_get_w_entity(it->world, e, ctx->component_2) != NULL);

            ecs_remove_entity(it->world, e, ctx->component_2);
            test_assert( !ecs_has_entity(it->world, e, ctx->component_2));
            test_assert( ecs_get_w_entity(it->world, e, ctx->component_2) == NULL);
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
    ECS_SYSTEM(world, Add_remove_same_from_new, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Velocity)};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !ecs_get_type(world, ctx.new_entities[i]));
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
    ECS_SYSTEM(world, Add_remove_same_from_new, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Position), .component_2 = ecs_entity(Velocity)};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !ecs_get_type(world, ctx.new_entities[i]));
    }

    ecs_fini(world);
}

static
void Add_remove_same_from_new_w_component(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_type_t type = ecs_type_from_entity(it->world, ctx->component);
        ecs_entity_t e = ecs_new_w_type(it->world, type);
        test_assert( ecs_has_entity(it->world, e, ctx->component));
        test_assert( ecs_get_w_entity(it->world, e, ctx->component) != NULL);

        if (ctx->component_2) {
            ecs_add_entity(it->world, e, ctx->component_2);
            test_assert( ecs_has_entity(it->world, e, ctx->component_2));
            test_assert( ecs_get_w_entity(it->world, e, ctx->component_2) != NULL);

            ecs_remove_entity(it->world, e, ctx->component_2);
            test_assert( !ecs_has_entity(it->world, e, ctx->component_2));
            test_assert( ecs_get_w_entity(it->world, e, ctx->component_2) == NULL);
            test_assert( ecs_get_w_entity(it->world, e, ctx->component) != NULL);
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
    ECS_SYSTEM(world, Add_remove_same_from_new_w_component, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Position), .component_2 = ecs_entity(Velocity)};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !!ecs_get_type(world, ctx.new_entities[i]));
        test_assert( ecs_has(world, ctx.new_entities[i], Position));
        test_assert( !ecs_has(world, ctx.new_entities[i], Velocity));
    }

    ecs_fini(world);
}

static
void Add_remove_different_from_new_empty(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = ecs_new(it->world, 0);

        if (ctx->component_3) {
            ecs_add_entity(it->world, e, ctx->component_3);
            test_assert( ecs_has_entity(it->world, e, ctx->component_3));
            test_assert( ecs_get_w_entity(it->world, e, ctx->component_3) != NULL);
        }

        if (ctx->component_2) {
            ecs_remove_entity(it->world, e, ctx->component_2);
            test_assert( !ecs_has_entity(it->world, e, ctx->component_2));
            test_assert( ecs_get_w_entity(it->world, e, ctx->component_2) == NULL);
        }

        if (ctx->component) {
            ecs_add_entity(it->world, e, ctx->component);
            test_assert( ecs_has_entity(it->world, e, ctx->component));  
            test_assert( ecs_get_w_entity(it->world, e, ctx->component) != NULL);      
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
    ECS_SYSTEM(world, Add_remove_different_from_new_empty, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Rotation), .component_2 = ecs_entity(Position), .component_3 = ecs_entity(Mass)};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !!ecs_get_type(world, ctx.new_entities[i]));
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
    ECS_SYSTEM(world, Add_remove_different_from_new_empty, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Rotation), .component_2 = ecs_entity(Position), .component_3 = ecs_entity(Position)};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !!ecs_get_type(world, ctx.new_entities[i]));
        test_assert( ecs_has(world, ctx.new_entities[i], Rotation));
        test_assert( !ecs_has(world, ctx.new_entities[i], Position));
    }

    ecs_fini(world);
}

static
void Clone_current(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    int i;
    for (i = 0; i < it->count; i ++) {
        ctx->new_entities[ctx->entity_count] = ecs_clone(it->world, 0, it->entities[i], false);
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
    ECS_SYSTEM(world, Clone_current, EcsOnUpdate, Position);

    IterData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert(ctx.new_entities[0] != 0);
    test_assert( !!ecs_get_type(world, ctx.new_entities[0]));
    test_assert( ecs_has(world, ctx.new_entities[0], Position));

    test_assert(ctx.new_entities[1] != 0);
    test_assert( !!ecs_get_type(world, ctx.new_entities[1]));
    test_assert( ecs_has(world, ctx.new_entities[1], Position));
    test_assert( ecs_has(world, ctx.new_entities[1], Velocity));

    test_assert(ctx.new_entities[2] != 0);
    test_assert( !!ecs_get_type(world, ctx.new_entities[2]));
    test_assert( ecs_has(world, ctx.new_entities[2], Position));
    test_assert( ecs_has(world, ctx.new_entities[2], Mass));

    ecs_fini(world);
}

static
void Clone_current_w_value(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    int i;
    for (i = 0; i < it->count; i ++) {
        ctx->new_entities[ctx->entity_count] = ecs_clone(it->world, 0, it->entities[i], true);
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
    ECS_SYSTEM(world, Clone_current_w_value, EcsOnUpdate, Position);

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
    test_assert( !!ecs_get_type(world, ctx.new_entities[0]));
    test_assert( ecs_has(world, ctx.new_entities[0], Position));
    test_int( ecs_get(world, ctx.new_entities[0], Position)->x, 10);
    test_int( ecs_get(world, ctx.new_entities[0], Position)->y, 20);

    test_assert(ctx.new_entities[1] != 0);
    test_assert( !!ecs_get_type(world, ctx.new_entities[1]));
    test_assert( ecs_has(world, ctx.new_entities[1], Position));
    test_assert( ecs_has(world, ctx.new_entities[1], Velocity));
    test_int( ecs_get(world, ctx.new_entities[1], Position)->x, 11);
    test_int( ecs_get(world, ctx.new_entities[1], Position)->y, 21);
    test_int( ecs_get(world, ctx.new_entities[1], Velocity)->x, 30);
    test_int( ecs_get(world, ctx.new_entities[1], Velocity)->y, 40);

    test_assert(ctx.new_entities[2] != 0);
    test_assert( !!ecs_get_type(world, ctx.new_entities[2]));
    test_assert( ecs_has(world, ctx.new_entities[2], Position));
    test_assert( ecs_has(world, ctx.new_entities[2], Mass));
    test_int( ecs_get(world, ctx.new_entities[2], Position)->x, 12);
    test_int( ecs_get(world, ctx.new_entities[2], Position)->y, 22);
    test_int( *ecs_get(world, ctx.new_entities[2], Mass), 50);

    ecs_fini(world);
}

static
void Add_to_current(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);

    int i;
    for (i = 0; i < it->count; i ++) {
        if (ctx->component) {
            ecs_add_entity(it->world, it->entities[i], ctx->component);

            test_assert( !!ecs_get_type(it->world, it->entities[i]));
            test_assert( ecs_has_entity(it->world,  it->entities[i], ctx->component));
            test_assert( ecs_get_w_entity(it->world, it->entities[i], ctx->component) != NULL);
        }

        if (ctx->component_2) {
            ecs_add_entity(it->world, it->entities[i], ctx->component_2);

            test_assert( !!ecs_get_type(it->world, it->entities[i]));
            test_assert( ecs_has_entity(it->world, it->entities[i], ctx->component_2)); 
            test_assert( ecs_get_w_entity(it->world, it->entities[i], ctx->component_2) != NULL);           
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
    ECS_SYSTEM(world, Add_to_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Rotation)};
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
    ECS_SYSTEM(world, Add_to_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Velocity), .component_2 = ecs_entity(Rotation)};
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
void Remove_from_current(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);

    int i;
    for (i = it->count - 1; i >= 0; i --) {
        ecs_entity_t e = it->entities[i];

        if (ctx->component) {
            ecs_remove_entity(it->world, e, ctx->component);
            test_assert( !ecs_has_entity(it->world,  e, ctx->component));
            test_assert( ecs_get_w_entity(it->world, e, ctx->component) == NULL);
        }

        if (ctx->component_2) {
            ecs_remove_entity(it->world, e, ctx->component_2);
            test_assert( !ecs_has_entity(it->world, e, ctx->component_2));
            test_assert( ecs_get_w_entity(it->world, e, ctx->component_2) == NULL);
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
    ECS_SYSTEM(world, Remove_from_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
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
    test_assert( !ecs_get_type(world, e_1));

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
    ECS_SYSTEM(world, Remove_from_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Position), .component_2 = ecs_entity(Velocity)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
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
    test_assert( !ecs_get_type(world, e_1));

    test_assert( !ecs_has(world, e_2, Position));
    test_assert( !ecs_has(world, e_2, Velocity));
    test_assert( !ecs_get_type(world, e_2));

    test_assert( !ecs_has(world, e_3, Position));
    test_assert( !ecs_has(world, e_3, Velocity));
    test_assert( ecs_has(world, e_3, Mass));

    ecs_fini(world);
}

static
void Add_remove_same_from_current(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    int i;
    for (i = 0; i < it->count; i ++) {
        if (ctx->component) {
            ecs_add_entity(it->world, it->entities[i], ctx->component);
            test_assert( ecs_has_entity(it->world,  it->entities[i], ctx->component));
            test_assert( ecs_get_w_entity(it->world, it->entities[i], ctx->component) != NULL);

            ecs_remove_entity(it->world, it->entities[i], ctx->component);
            test_assert( !ecs_has_entity(it->world,  it->entities[i], ctx->component));
            test_assert( ecs_get_w_entity(it->world, it->entities[i], ctx->component) == NULL);
        }

        if (ctx->component_2) {
            ecs_add_entity(it->world, it->entities[i], ctx->component_2);
            test_assert( ecs_has_entity(it->world,  it->entities[i], ctx->component_2));
            test_assert( ecs_get_w_entity(it->world, it->entities[i], ctx->component_2) != NULL);

            ecs_remove_entity(it->world, it->entities[i], ctx->component_2);
            test_assert( !ecs_has_entity(it->world,  it->entities[i], ctx->component_2));
            test_assert( ecs_get_w_entity(it->world, it->entities[i], ctx->component_2) == NULL);
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
    ECS_SYSTEM(world, Add_remove_same_from_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Rotation)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
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
    ECS_SYSTEM(world, Add_remove_same_from_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
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
    test_assert( !ecs_get_type(world, e_1));

    test_assert( !ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_2, Velocity));

    test_assert( !ecs_has(world, e_3, Position));
    test_assert( ecs_has(world, e_3, Mass));

    ecs_fini(world);
}

static
void Remove_add_same_from_current(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    int i;
    for (i = 0; i < it->count; i ++) {
        if (ctx->component) {
            ecs_remove_entity(it->world, it->entities[i], ctx->component);
            test_assert( !ecs_has_entity(it->world,  it->entities[i], ctx->component));
            test_assert( ecs_get_w_entity(it->world, it->entities[i], ctx->component) == NULL);

            ecs_add_entity(it->world, it->entities[i], ctx->component);
            test_assert( ecs_has_entity(it->world,  it->entities[i], ctx->component));
            test_assert( ecs_get_w_entity(it->world, it->entities[i], ctx->component) != NULL);
        }

        if (ctx->component_2) {
            ecs_remove_entity(it->world, it->entities[i], ctx->component_2);
            test_assert( !ecs_has_entity(it->world,  it->entities[i], ctx->component_2));
            test_assert( ecs_get_w_entity(it->world, it->entities[i], ctx->component_2) == NULL);

            ecs_add_entity(it->world, it->entities[i], ctx->component_2);
            test_assert( ecs_has_entity(it->world,  it->entities[i], ctx->component_2));
            test_assert( ecs_get_w_entity(it->world, it->entities[i], ctx->component_2) != NULL);
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
    ECS_SYSTEM(world, Remove_add_same_from_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Rotation)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
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
    ECS_SYSTEM(world, Remove_add_same_from_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
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
    ECS_SYSTEM(world, Add_remove_same_from_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Rotation), .component_2 = ecs_entity(Mass)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
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
    ECS_SYSTEM(world, Add_remove_same_from_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Position), .component_2 = ecs_entity(Velocity)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
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
    test_assert( !ecs_get_type(world, e_1));

    test_assert( !ecs_has(world, e_2, Position));
    test_assert( !ecs_has(world, e_2, Velocity));
    test_assert( !ecs_get_type(world, e_2));

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
    ECS_SYSTEM(world, Remove_add_same_from_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Rotation), .component_2 = ecs_entity(Mass)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
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

static
void AddRemoveAdd(ecs_iter_t *it) {
    ECS_COLUMN(it, Velocity, v, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add(it->world, it->entities[i], Velocity);
        ecs_remove(it->world, it->entities[i], Velocity);
        ecs_add(it->world, it->entities[i], Velocity);
    }
}

void SingleThreadStaging_add_remove_add_same_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, AddRemoveAdd, EcsOnUpdate, Position, :Velocity);

    /* Create entities from scratch so they don't have the EcsName component */
    ecs_entity_t e_1 = ecs_new(world, Position);
    ecs_entity_t e_2 = ecs_new(world, Position);
    ecs_entity_t e_3 = ecs_new(world, Position);

    ecs_progress(world, 1);

    test_assert( ecs_has(world, e_1, Velocity));
    test_assert( ecs_has(world, e_2, Velocity));
    test_assert( ecs_has(world, e_3, Velocity));

    ecs_fini(world);
}

void SingleThreadStaging_remove_add_2_same_existing_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Remove_add_same_from_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Position), .component_2 = ecs_entity(Velocity)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
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
void Add_remove_different_from_current(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    int i;
    for (i = 0; i < it->count; i ++) {
        if (ctx->component_3) {
            ecs_add_entity(it->world, it->entities[i], ctx->component_3);
            test_assert( ecs_has_entity(it->world, it->entities[i], ctx->component_3));
            test_assert( ecs_get_w_entity(it->world, it->entities[i], ctx->component_3) != NULL);
        }

        if (ctx->component_2) {
            ecs_remove_entity(it->world, it->entities[i], ctx->component_2);
            test_assert( !ecs_has_entity(it->world,  it->entities[i], ctx->component_2));
            test_assert( ecs_get_w_entity(it->world, it->entities[i], ctx->component_2) == NULL);
        }

        if (ctx->component) {
            ecs_add_entity(it->world, it->entities[i], ctx->component);
            test_assert( ecs_has_entity(it->world,  it->entities[i], ctx->component));     
            test_assert( ecs_get_w_entity(it->world, it->entities[i], ctx->component) != NULL);   
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
    ECS_SYSTEM(world, Add_remove_different_from_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Rotation), .component_2 = ecs_entity(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
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
    ECS_SYSTEM(world, Add_remove_different_from_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Rotation), .component_2 = ecs_entity(Position), .component_3 = ecs_entity(Mass)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
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
void Add_1_remove_2_different_from_current(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    int i;
    for (i = 0; i < it->count; i ++) {
        if (ctx->component) {
            ecs_add_entity(it->world, it->entities[i], ctx->component);
            test_assert( ecs_has_entity(it->world,  it->entities[i], ctx->component)); 
            test_assert( ecs_get_w_entity(it->world, it->entities[i], ctx->component) != NULL);       
        }

        if (ctx->component_2) {
            ecs_remove_entity(it->world, it->entities[i], ctx->component_2);
            test_assert( !ecs_has_entity(it->world,  it->entities[i], ctx->component_2));
            test_assert( ecs_get_w_entity(it->world, it->entities[i], ctx->component_2) == NULL);
        }

        if (ctx->component_3) {
            ecs_remove_entity(it->world, it->entities[i], ctx->component_3);
            test_assert( !ecs_has_entity(it->world, it->entities[i], ctx->component_3));
            test_assert( ecs_get_w_entity(it->world, it->entities[i], ctx->component_3) == NULL);
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
    ECS_SYSTEM(world, Add_1_remove_2_different_from_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Rotation), .component_2 = ecs_entity(Position), .component_3 = ecs_entity(Velocity)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
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
void Delete_current(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_delete(it->world, it->entities[i]);
        test_assert( !ecs_get_type(it->world, it->entities[i]));
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
    ECS_SYSTEM(world, Delete_current, EcsOnUpdate, Position);

    IterData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( !ecs_get_type(world, e_1));
    test_assert( !ecs_get_type(world, e_2));
    test_assert( !ecs_get_type(world, e_3));

    ecs_fini(world);
}

static
void Delete_even(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    int i;
    for (i = 0; i < it->count; i ++) {
        if (!(it->entities[i] % 2)) {
            ecs_delete(it->world, it->entities[i]);
            test_assert( !ecs_get_type(it->world, it->entities[i]));
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
    ECS_SYSTEM(world, Delete_even, EcsOnUpdate, Position);

    IterData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 8);

    if (!(e_1 % 2))
        test_assert( !ecs_get_type(world, e_1));
    else
        test_assert ( !!ecs_get_type(world, e_1));

    if (!(e_2 % 2))
        test_assert( !ecs_get_type(world, e_2));
    else
        test_assert ( !!ecs_get_type(world, e_2));

    if (!(e_3 % 2))
        test_assert( !ecs_get_type(world, e_3));
    else
        test_assert ( !!ecs_get_type(world, e_3));

    if (!(e_4 % 2))
        test_assert( !ecs_get_type(world, e_4));
    else
        test_assert ( !!ecs_get_type(world, e_4));

    if (!(e_5 % 2))
        test_assert( !ecs_get_type(world, e_5));
    else
        test_assert ( !!ecs_get_type(world, e_5));

    if (!(e_6 % 2))
        test_assert( !ecs_get_type(world, e_6));
    else
        test_assert ( !!ecs_get_type(world, e_6));

    if (!(e_7 % 2))
        test_assert( !ecs_get_type(world, e_7));
    else
        test_assert ( !!ecs_get_type(world, e_7));

    if (!(e_8 % 2))
        test_assert( !ecs_get_type(world, e_8));
    else
        test_assert ( !!ecs_get_type(world, e_8));

    ecs_fini(world);
}

static
void Delete_new_empty(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = ecs_new(it->world, 0);
        ecs_delete(it->world, e);
        test_assert( !ecs_get_type(it->world, e));

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
    ECS_SYSTEM(world, Delete_new_empty, EcsOnUpdate, Position);

    IterData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !ecs_get_type(world, ctx.new_entities[i]));
    }

    ecs_fini(world);
}

static
void Delete_new_w_component(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_type_t type = ecs_type_from_entity(it->world, ctx->component);
        ecs_entity_t e = ecs_new_w_type(it->world, type);
        test_assert( !!ecs_get_type(it->world, e));

        ecs_delete(it->world, e);
        test_assert( !ecs_get_type(it->world, e));

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
    ECS_SYSTEM(world, Delete_new_w_component, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Position)};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !ecs_get_type(world, ctx.new_entities[i]));
    }

    ecs_fini(world);
}

static
void Set_current(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    
    ecs_entity_t ecs_entity(Rotation) = ctx->component;
    ecs_type_t ecs_type(Rotation) = ecs_type_from_entity(
            it->world, ecs_entity(Rotation));

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], Rotation, {10 + it->entities[i]});
        test_assert( ecs_has(it->world, it->entities[i], Rotation));
        test_assert( ecs_get(it->world, it->entities[i], Rotation) != NULL);
        test_int( *ecs_get(it->world, it->entities[i], Rotation), 10 + it->entities[i]);

        ctx->entity_count ++;
    }
}

void SingleThreadStaging_set_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Set_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Rotation)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
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
    test_int( *ecs_get(world, e_1, Rotation), e_1 + 10);

    test_assert( ecs_has(world, e_2, Position));
    test_assert( ecs_has(world, e_2, Velocity));
    test_assert( ecs_has(world, e_2, Rotation));
    test_int( *ecs_get(world, e_2, Rotation), e_2 + 10);

    test_assert( ecs_has(world, e_3, Position));
    test_assert( ecs_has(world, e_3, Velocity));
    test_assert( ecs_has(world, e_3, Mass));
    test_assert( ecs_has(world, e_3, Rotation));
    test_int( *ecs_get(world, e_3, Rotation), e_3 + 10);

    ecs_fini(world);
}

static
void Set_new_empty(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    
    ecs_entity_t ecs_entity(Rotation) = ctx->component;
    ecs_type_t ecs_type(Rotation) = ecs_type_from_entity(
            it->world, ecs_entity(Rotation));
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = ecs_new(it->world, 0);
        test_assert( !ecs_get_type(it->world, e));

        ecs_set(it->world, e, Rotation, {10 + e});
        test_assert( ecs_has(it->world, e, Rotation));
        test_assert( ecs_get(it->world, e, Rotation) != NULL);
        test_int( *ecs_get(it->world, e, Rotation), 10 + e);

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
    ECS_SYSTEM(world, Set_new_empty, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Rotation)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
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
        test_assert( !!ecs_get_type(world, ctx.new_entities[i]));
        test_assert( ecs_has(world, ctx.new_entities[i], Rotation));
        test_int( *ecs_get(world, ctx.new_entities[i], Rotation), ctx.new_entities[i] + 10);
    }

    ecs_fini(world);
}

static
void Set_new_w_component(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    
    ecs_entity_t ecs_entity(Position) = ctx->component;
    ecs_type_t ecs_type(Position) = ecs_type_from_entity(
            it->world, ecs_entity(Position));

    ecs_entity_t ecs_entity(Rotation) = ctx->component_2;
    ecs_type_t ecs_type(Rotation) = ecs_type_from_entity(
            it->world, ecs_entity(Rotation));

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = ecs_new(it->world, Position);
        test_assert( ecs_has(it->world, e, Position));

        ecs_set(it->world, e, Rotation, {10 + e});
        test_assert( ecs_has(it->world, e, Position));
        test_assert( ecs_has(it->world, e, Rotation));
        test_assert( ecs_get(it->world, e, Rotation) != NULL);
        test_int( *ecs_get(it->world, e, Rotation), 10 + e);

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
    ECS_SYSTEM(world, Set_new_w_component, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Position), .component_2 = ecs_entity(Rotation)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
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
        test_assert( !!ecs_get_type(world, ctx.new_entities[i]));
        test_assert( ecs_has(world, ctx.new_entities[i], Position));
        test_assert( ecs_has(world, ctx.new_entities[i], Rotation));
        test_int( *ecs_get(world, ctx.new_entities[i], Rotation), ctx.new_entities[i] + 10);
    }

    ecs_fini(world);
}

static
void Set_existing_new_w_component(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    
    ecs_entity_t ecs_entity(Position) = ctx->component;
    ecs_type_t ecs_type(Position) = 
        ecs_type_from_entity(it->world, ecs_entity(Position));

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = ecs_new(it->world, Position);
        test_assert( ecs_has(it->world, e, Position));

        ecs_set(it->world, e, Position, {10 + e, 20 + e});
        test_assert( ecs_has(it->world, e, Position));

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
    ECS_SYSTEM(world, Set_existing_new_w_component, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
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
        test_assert( !!ecs_get_type(world, ctx.new_entities[i]));
        test_assert( ecs_has(world, ctx.new_entities[i], Position));
        test_int( ecs_get(world, ctx.new_entities[i], Position)->x, ctx.new_entities[i] + 10);
        test_int( ecs_get(world, ctx.new_entities[i], Position)->y, ctx.new_entities[i] + 20);
    }

    ecs_fini(world);
}

static
void Set_new_after_add(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    
    ecs_entity_t ecs_entity(Position) = ctx->component;
    ecs_type_t ecs_type(Position) = 
        ecs_type_from_entity(it->world, ecs_entity(Position));

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = ecs_new(it->world, 0);
        test_assert( !ecs_get_type(it->world, e));

        ecs_add(it->world, e, Position);
        test_assert( ecs_has(it->world, e, Position));

        ecs_set(it->world, e, Position, {10 + e, 20 + e});
        test_assert( ecs_has(it->world, e, Position));

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
    ECS_SYSTEM(world, Set_new_after_add, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
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
        test_assert( !!ecs_get_type(world, ctx.new_entities[i]));
        test_assert( ecs_has(world, ctx.new_entities[i], Position));
        test_int( ecs_get(world, ctx.new_entities[i], Position)->x, ctx.new_entities[i] + 10);
        test_int( ecs_get(world, ctx.new_entities[i], Position)->y, ctx.new_entities[i] + 20);
    }

    ecs_fini(world);
}

static
void Remove_after_set(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    
    ecs_entity_t ecs_entity(Position) = ctx->component;
    ecs_type_t ecs_type(Position) = 
        ecs_type_from_entity(it->world, ecs_entity(Position));

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = ecs_new(it->world, 0);
        test_assert( !ecs_get_type(it->world, e));

        ecs_set(it->world, e, Position, {10 + e, 20 + e});
        test_assert( ecs_has(it->world, e, Position));
        test_assert( ecs_get(it->world, e, Position) != NULL);
        test_int( ecs_get(it->world, e, Position)->x, 10 + e);
        test_int( ecs_get(it->world, e, Position)->y, 20 + e);

        ecs_remove(it->world, e, Position);
        test_assert( !ecs_get_type(it->world, e));
        test_assert( ecs_get(it->world, e, Position) == NULL);
        
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
    ECS_SYSTEM(world, Remove_after_set, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
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
        test_assert( !ecs_get_type(world, ctx.new_entities[i]));
    }

    ecs_fini(world);
}

static
void Delete_after_set(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    
    ecs_entity_t ecs_entity(Position) = ctx->component;
    ecs_type_t ecs_type(Position) = 
        ecs_type_from_entity(it->world, ecs_entity(Position));

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = ecs_new(it->world, 0);
        test_assert( !ecs_get_type(it->world, e));

        ecs_set(it->world, e, Position, {10 + e, 20 + e});
        test_assert( ecs_has(it->world, e, Position));
        test_assert( ecs_get(it->world, e, Position) != NULL);
        test_int( ecs_get(it->world, e, Position)->x, 10 + e);
        test_int( ecs_get(it->world, e, Position)->y, 20 + e);

        ecs_delete(it->world, e);
        test_assert( !ecs_get_type(it->world, e));
        test_assert( ecs_get(it->world, e, Position) == NULL);

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
    ECS_SYSTEM(world, Delete_after_set, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_entity(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
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
        test_assert( !ecs_get_type(world, ctx.new_entities[i]));
    }

    ecs_fini(world);
}


void SingleThreadStaging_add_to_current_in_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TRIGGER(world, Add_to_current, EcsOnAdd, Position);

    IterData ctx = {.component = ecs_entity(Velocity)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
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
    ECS_TRIGGER(world, Remove_from_current, EcsOnAdd, Position);

    IterData ctx = {.component = ecs_entity(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
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

void SingleThreadStaging_remove_added_component_in_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_TRIGGER(world, Remove_from_current, EcsOnAdd, Position);

    IterData ctx = {.component = ecs_entity(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
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

static Probe pv_probe;

static
void On_PV(ecs_iter_t *it) {
    ECS_COLUMN(it, Position, p, 1);
    ECS_COLUMN(it, Velocity, v, 2);

    probe_system_w_ctx(it, &pv_probe);

    int i;
    for (i = 0; i < it->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}

void SingleThreadStaging_match_table_created_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_SYSTEM(world, Add_to_current, EcsOnUpdate, Position, !Velocity);
    ECS_SYSTEM(world, On_PV, EcsOnUpdate, Position, Velocity);

    IterData add_ctx = {.component = ecs_entity(Velocity)};
    ecs_set_context(world, &add_ctx);

    ecs_entity_t e_1 = ecs_new(world, Position);
    ecs_entity_t e_2 = ecs_new(world, Position);
    ecs_entity_t e_3 = ecs_new(world, Position);

    ecs_progress(world, 1);

    test_int(pv_probe.count, 3);
    test_int(pv_probe.invoked, 1);
    test_int(pv_probe.system, On_PV);
    test_int(pv_probe.column_count, 2);
    test_null(pv_probe.param);

    test_int(pv_probe.e[0], e_1);
    test_int(pv_probe.e[1], e_2);
    test_int(pv_probe.e[2], e_3);
    test_int(pv_probe.c[0][0], ecs_entity(Position));
    test_int(pv_probe.s[0][0], 0);
    test_int(pv_probe.c[0][1], ecs_entity(Velocity));
    test_int(pv_probe.s[0][1], 0);

    ecs_fini(world);
}

static
void Set_velocity_on_new(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);

    ecs_entity_t ecs_entity(Velocity) = ctx->component;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, 0, Velocity, {10, 20});
    }
}

static
void On_V(ecs_iter_t *it) {
    ECS_COLUMN(it, Velocity, v, 1);

    probe_system(it);

    int i;
    for (i = 0; i < it->count; i ++) {
        v[i].x = v[i].y;
    }
}

void SingleThreadStaging_match_table_created_w_new_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, Set_velocity_on_new, EcsOnUpdate, Position);
    ECS_SYSTEM(world, On_V, EcsOnUpdate, Velocity);

    IterData add_ctx = {.component = ecs_entity(Velocity)};
    ecs_set_context(world, &add_ctx);

    ecs_set(world, 0, Position, {10, 20});

    ecs_progress(world, 1);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);
    ecs_enable(world, Set_velocity_on_new, false);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, On_V);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.c[0][0], ecs_entity(Velocity));
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}


void SingleThreadStaging_match_table_created_w_new_in_on_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, Set_velocity_on_new, EcsOnSet, Position);
    ECS_SYSTEM(world, On_V, EcsOnUpdate, Velocity);

    IterData add_ctx = {.component = ecs_entity(Velocity)};
    ecs_set_context(world, &add_ctx);

    ecs_set(world, 0, Position, {10, 20});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);
    ecs_enable(world, Set_velocity_on_new, false);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, On_V);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.c[0][0], ecs_entity(Velocity));
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}

static
void Create_container(ecs_iter_t *it) {
    probe_system(it);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_new_w_entity(it->world, ECS_CHILDOF | it->entities[i]);
    }
}

void SingleThreadStaging_merge_table_w_container_added_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Create_container, EcsOnUpdate, Position);

    ECS_ENTITY(world, e_1, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Create_container);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.c[0][0], ecs_entity(Position));
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

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_set(world, e_1, Position, {10, 20});

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Create_container);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}

/* Hacky way to test if entities are properly merged by system */
static ecs_entity_t g_child = 0;
static ecs_entity_t g_parent = 0;

static
void Create_container_reverse(ecs_iter_t *it) {

    probe_system(it);

    ecs_world_t *world = it->world;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t child = ecs_new(world, 0);
        ecs_entity_t parent = ecs_new(world, 0);
        test_assert(child != 0);
        test_assert(parent != 0);

        ecs_add_entity(world, child, ECS_CHILDOF | parent);

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

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_set(world, e_1, Position, {10, 20});

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Create_container_reverse);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    test_assert(g_parent != 0);
    test_assert(g_child != 0);

    test_assert( ecs_has_entity(world, g_child, ECS_CHILDOF | g_parent));

    ecs_fini(world);
}

static
void Task(ecs_iter_t *it) {
    ECS_COLUMN(it, Position, p, 1);

    ecs_entity_t *e = ecs_get_context(it->world);

    ecs_add(it->world, *e, Position);
}

void SingleThreadStaging_merge_after_tasks() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);

    ECS_SYSTEM(world, Task, EcsOnUpdate, :Position);

    ecs_set_context(world, &e);

    ecs_progress(world, 1);

    test_assert( ecs_has(world, e, Position));

    ecs_fini(world);
}

static
void OverrideAfterRemove(ecs_iter_t *it) {
    ECS_COLUMN(it, Position, p, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        test_int(p[i].x, 30);
        test_int(p[i].y, 40);

        ecs_remove(it->world, it->entities[i], Position);

        /* The entity still has Position, because it inherits it from base */
        test_assert(ecs_has(it->world, it->entities[i], Position));

        /* The entity does not own Position */
        test_assert(!ecs_owns(it->world, it->entities[i], Position, true));

        ecs_add(it->world, it->entities[i], Position);
        test_assert(ecs_has(it->world, it->entities[i], Position));

        const Position *p_ptr = ecs_get(it->world, it->entities[i], Position);
        test_assert(p_ptr != NULL);
        test_int(p_ptr->x, 10);
        test_int(p_ptr->y, 20);

        /* Main stage at this point still has old values */
        test_int(p[i].x, 30);
        test_int(p[i].y, 40);        
    }
}

void SingleThreadStaging_override_after_remove_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, base, Position, EcsDisabled);

    ECS_ENTITY(world, e, Position, INSTANCEOF | base);

    ECS_SYSTEM(world, OverrideAfterRemove, EcsOnUpdate, Position);

    ecs_set(world, base, Position, {10, 20});

    ecs_set(world, e, Position, {30, 40});

    ecs_progress(world, 1);

    test_assert( ecs_has(world, e, Position));
    const Position *p_ptr = ecs_get(world, e, Position);
    test_assert(p_ptr != NULL);
    test_int(p_ptr->x, 10);
    test_int(p_ptr->y, 20);    

    ecs_fini(world);
}

static
void GetParentInProgress(ecs_iter_t *it) {
    ECS_COLUMN(it, Velocity, v, 2);
    
    ecs_world_t *world = it->world;

    /* Create parent */
    ecs_entity_t parent = ecs_new(world, Velocity);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];

        ecs_add_entity(world, e, ECS_CHILDOF | parent);
        test_assert( ecs_has_entity(world, e, ECS_CHILDOF | parent));

        ecs_entity_t test_parent = ecs_get_parent(world, e, Velocity);
        test_assert(test_parent != 0);
    }
}

void SingleThreadStaging_get_parent_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, GetParentInProgress, EcsOnUpdate, Position, :Velocity);

    ecs_entity_t e = ecs_new(world, Position);

    ecs_progress(world, 1);

    ecs_entity_t parent = ecs_get_parent(world, e, Velocity);
    test_assert(parent != 0);

    ecs_fini(world);
}

static
void AddInProgress(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    ECS_COLUMN(it, Position, p, 1);
    ECS_COLUMN(it, Velocity, v, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(world, it->entities[i], Position, {1, 1});
        ecs_set(world, it->entities[i], Velocity, {2, 2});

        /* Make sure we can now access velocity from stage */
        test_assert( ecs_has(world, it->entities[i], Velocity));
        const Velocity *v_ptr = ecs_get(world, it->entities[i], Velocity);
        test_assert(v_ptr != NULL);
        test_int(v_ptr->x, 2);
        test_int(v_ptr->y, 2);

        /* Make sure main stage hasn't been updated yet (this happens in the 
         * merge) */
        test_int(p[i].x, 0);
        test_int(p[i].y, 0);
    }
}

static
void Move(ecs_iter_t *it) {
    ECS_COLUMN(it, Position, p, 1);
    ECS_COLUMN(it, Velocity, v, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }    
}

void SingleThreadStaging_merge_once() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, AddInProgress, EcsOnUpdate, Position, !Velocity);
    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);

    ecs_entity_t e = ecs_set(world, 0, Position, {0, 0});
    ecs_progress(world, 1);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 3);
    test_int(p->y, 3);

    const Velocity *v = ecs_get(world, e, Velocity);
    test_int(v->x, 2);
    test_int(v->y, 2);

    ecs_progress(world, 1);
    ecs_progress(world, 1);

    p = ecs_get(world, e, Position);
    test_int(p->x, 7);
    test_int(p->y, 7);

    v = ecs_get(world, e, Velocity);
    test_int(v->x, 2);
    test_int(v->y, 2);

    ecs_fini(world);
}

void MutableTest(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    ECS_COLUMN(it, Velocity, v, 2);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        bool is_added;
        Velocity *v_mut = ecs_get_mut(
            world, it->entities[i], Velocity, &is_added);

        test_assert(v_mut != NULL);
        test_assert(v_mut != v);

        // Even though component is added to stage, is_added should only be true
        // if the component is added for the first time, which requires the app
        // to init the component value.
        if (!v) {
            test_bool(is_added, true);
        } else {
            test_bool(is_added, false);
        }

        if (is_added) {
            v_mut->x = 0;
            v_mut->y = 0;
        }

        v_mut->x ++;
        v_mut->y ++;

        // Make sure we didn't update the main stage
        if (v) {
            test_assert(v->x == v_mut->x - 1);
            test_assert(v->y == v_mut->y - 1);
        }
    }
}

void SingleThreadStaging_get_mutable() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_SYSTEM(world, MutableTest, EcsOnUpdate, Position, ?Velocity);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_progress(world, 0);
    test_assert( ecs_has(world, e, Velocity));
    
    const Velocity *v = ecs_get(world, e, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 1);

    ecs_progress(world, 0);
    test_assert( ecs_has(world, e, Velocity));

    v = ecs_get(world, e, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 2);
    test_int(v->y, 2);
    
    ecs_fini(world);
}

void SingleThreadStaging_get_mutable_from_main() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_SYSTEM(world, MutableTest, EcsOnUpdate, Position, ?Velocity);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    // Add velocity on main stage
    ecs_set(world, e, Velocity, {1, 1});

    ecs_progress(world, 0);
    test_assert( ecs_has(world, e, Velocity));
    
    const Velocity *v = ecs_get(world, e, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 2);
    test_int(v->y, 2);

    ecs_progress(world, 0);
    test_assert( ecs_has(world, e, Velocity));

    v = ecs_get(world, e, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 3);
    test_int(v->y, 3);
    
    ecs_fini(world);
}

void MutableTest_w_Add(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    ECS_COLUMN(it, Velocity, v, 2);
    ECS_COLUMN_COMPONENT(it, MyTag, 3);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_add(world, it->entities[i], MyTag);

        bool is_added;
        Velocity *v_mut = ecs_get_mut(
            world, it->entities[i], Velocity, &is_added);

        // Even though component is added to stage, is_added should only be true
        // if the component is added for the first time, which requires the app
        // to init the component value.
        if (!v) {
            test_bool(is_added, true);
        } else {
            test_bool(is_added, false);
        }

        if (is_added) {
            v_mut->x = 0;
            v_mut->y = 0;
        }

        v_mut->x ++;
        v_mut->y ++;

        // Make sure we didn't update the main stage
        if (v) {
            test_assert(v->x == v_mut->x - 1);
            test_assert(v->y == v_mut->y - 1);
        }
    }
}

typedef bool MyBool;

void SingleThreadStaging_get_mutable_w_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, MyBool);
    ECS_SYSTEM(world, MutableTest_w_Add, EcsOnUpdate, Position, ?Velocity, :MyBool);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_progress(world, 0);
    test_assert( ecs_has(world, e, Velocity));
    
    const Velocity *v = ecs_get(world, e, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 1);

    ecs_progress(world, 0);
    test_assert( ecs_has(world, e, Velocity));

    v = ecs_get(world, e, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 2);
    test_int(v->y, 2);

    ecs_progress(world, 0);
    test_assert( ecs_has(world, e, Velocity));

    v = ecs_get(world, e, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 3);
    test_int(v->y, 3);
    
    ecs_fini(world);
}

void OnAdd(ecs_iter_t *it) {
    ECS_COLUMN(it, Velocity, v, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        v->x = 1;
        v->y = 2;
    }
}

void AddInProgress2(ecs_iter_t *it) {
    ECS_COLUMN(it, Velocity, v, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add(it->world, it->entities[i], Velocity);
    }
}

void SingleThreadStaging_on_add_after_new_type_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_SYSTEM(world, AddInProgress2, EcsOnUpdate, Position, :Velocity);
    ECS_TRIGGER(world, OnAdd, EcsOnAdd, Velocity);

    ecs_entity_t e = ecs_new(world, Position);

    ecs_progress(world, 1);

    test_assert( ecs_has(world, e, Velocity));
    const Velocity *v = ecs_get(world, e, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 2);

    ecs_fini(world);
}
