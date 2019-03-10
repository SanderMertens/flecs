#include <include/api.h>

typedef struct IterData {
    EcsType component;
    EcsType component_2;
    EcsType component_3;
    EcsEntity new_entities[MAX_ENTITIES];
    uint32_t entity_count;
} IterData;

static
void NewEmpty(EcsRows *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = 0; i < rows->limit; i ++) {
        ctx->new_entities[ctx->entity_count] = ecs_new(rows->world, 0);
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_new_empty() {
    EcsWorld *world = ecs_init();

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
void New_w_component(EcsRows *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = 0; i < rows->limit; i ++) {
        EcsEntity e = _ecs_new(rows->world, ctx->component);
        ctx->new_entities[ctx->entity_count] = e;

        test_assert( !ecs_empty(rows->world, e));
        test_assert( _ecs_has(rows->world, e, ctx->component));

        ctx->entity_count ++;
    }
}

void SingleThreadStaging_new_w_component() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, New_w_component, EcsOnFrame, Position);

    IterData ctx = {.component = TVelocity};
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
    EcsWorld *world = ecs_init();

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
void Add_to_new_empty(EcsRows *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = 0; i < rows->limit; i ++) {
        EcsEntity e = ecs_new(rows->world, 0);
        if (ctx->component) {
            _ecs_add(rows->world, e, ctx->component);
            
            test_assert( !ecs_empty(rows->world, e));
            test_assert( _ecs_has(rows->world, e, ctx->component));
        }
        if (ctx->component_2) {
            _ecs_add(rows->world, e, ctx->component_2);

            test_assert( !ecs_empty(rows->world, e));
            test_assert( _ecs_has(rows->world, e, ctx->component_2));            
        }
        ctx->new_entities[ctx->entity_count] = e;
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_add_to_new_empty() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, Add_to_new_empty, EcsOnFrame, Position);

    IterData ctx = {.component = TVelocity};
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
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, Add_to_new_empty, EcsOnFrame, Position);

    IterData ctx = {.component = TVelocity, .component_2 = TRotation};
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
void Add_remove_same_from_new(EcsRows *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = 0; i < rows->limit; i ++) {
        EcsEntity e = ecs_new(rows->world, 0);

        test_assert( ecs_empty(rows->world, e));

        if (ctx->component) {
            _ecs_add(rows->world, e, ctx->component);

            test_assert( _ecs_has(rows->world, e, ctx->component));

            _ecs_remove(rows->world, e, ctx->component);

            test_assert( !_ecs_has(rows->world, e, ctx->component));
        }

        if (ctx->component_2) {
            _ecs_add(rows->world, e, ctx->component_2);

            test_assert( _ecs_has(rows->world, e, ctx->component_2));

            _ecs_remove(rows->world, e, ctx->component_2);

            test_assert( !_ecs_has(rows->world, e, ctx->component_2));
        }

        ctx->new_entities[ctx->entity_count] = e;
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_add_remove_same_to_new_empty() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, Add_remove_same_from_new, EcsOnFrame, Position);

    IterData ctx = {.component = TVelocity};
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
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, Add_remove_same_from_new, EcsOnFrame, Position);

    IterData ctx = {.component = TPosition, .component_2 = TVelocity};
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
void Add_remove_same_from_new_w_component(EcsRows *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = 0; i < rows->limit; i ++) {
        EcsEntity e = _ecs_new(rows->world, ctx->component);

        test_assert( _ecs_has(rows->world, e, ctx->component));

        if (ctx->component_2) {
            _ecs_add(rows->world, e, ctx->component_2);

            test_assert( _ecs_has(rows->world, e, ctx->component_2));

            _ecs_remove(rows->world, e, ctx->component_2);

            test_assert( !_ecs_has(rows->world, e, ctx->component_2));
        }

        ctx->new_entities[ctx->entity_count] = e;
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_add_remove_same_to_new_w_component() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, Add_remove_same_from_new_w_component, EcsOnFrame, Position);

    IterData ctx = {.component = TPosition, .component_2 = TVelocity};
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
void Add_remove_different_from_new_empty(EcsRows *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = 0; i < rows->limit; i ++) {
        EcsEntity e = ecs_new(rows->world, 0);

        if (ctx->component_3) {
            _ecs_add(rows->world, e, ctx->component_3);
            test_assert( _ecs_has(rows->world, e, ctx->component_3));
        }

        if (ctx->component_2) {
            _ecs_remove(rows->world, e, ctx->component_2);
            test_assert( !_ecs_has(rows->world, e, ctx->component_2));
        }

        if (ctx->component) {
            _ecs_add(rows->world, e, ctx->component);
            test_assert( _ecs_has(rows->world, e, ctx->component));        
        }

        ctx->new_entities[ctx->entity_count] = e;
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_2_add_1_remove_to_new_empty() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, Add_remove_different_from_new_empty, EcsOnFrame, Position);

    IterData ctx = {.component = TRotation, .component_2 = TPosition, .component_3 = TMass};
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
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, Add_remove_different_from_new_empty, EcsOnFrame, Position);

    IterData ctx = {.component = TRotation, .component_2 = TPosition, .component_3 = TPosition};
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
void Clone_current(EcsRows *rows) {
    EcsEntity *entities = ecs_column(rows, EcsEntity, 0);
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = 0; i < rows->limit; i ++) {
        ctx->new_entities[ctx->entity_count] = ecs_clone(rows->world, entities[i], false);
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_clone() {
    EcsWorld *world = ecs_init();

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
void Clone_current_w_value(EcsRows *rows) {
    EcsEntity *entities = ecs_column(rows, EcsEntity, 0);
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = 0; i < rows->limit; i ++) {
        ctx->new_entities[ctx->entity_count] = ecs_clone(rows->world, entities[i], true);
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_clone_w_value() {
    EcsWorld *world = ecs_init();

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
void Add_to_current(EcsRows *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    EcsEntity *entities = ecs_column(rows, EcsEntity, 0);
    int i;
    for (i = 0; i < rows->limit; i ++) {
        if (ctx->component) {
            _ecs_add(rows->world, entities[i], ctx->component);

            test_assert( !ecs_empty(rows->world, entities[i]));
            test_assert( _ecs_has(rows->world, entities[i], ctx->component));
        }

        if (ctx->component_2) {
            _ecs_add(rows->world, entities[i], ctx->component_2);

            test_assert( !ecs_empty(rows->world, entities[i]));
            test_assert( _ecs_has(rows->world, entities[i], ctx->component_2));            
        }

        ctx->entity_count ++;
    }
}

void SingleThreadStaging_add_to_current() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, Add_to_current, EcsOnFrame, Position);

    IterData ctx = {.component = TRotation};
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
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, Add_to_current, EcsOnFrame, Position);

    IterData ctx = {.component = TVelocity, .component_2 = TRotation};
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
void Remove_from_current(EcsRows *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    EcsEntity *entities = ecs_column(rows, EcsEntity, 0);
    int i;
    for (i = 0; i < rows->limit; i ++) {
        if (ctx->component) {
            _ecs_remove(rows->world, entities[i], ctx->component);

            test_assert( !_ecs_has(rows->world, entities[i], ctx->component));
        }

        if (ctx->component_2) {
            _ecs_remove(rows->world, entities[i], ctx->component_2);
            
            test_assert( !_ecs_has(rows->world, entities[i], ctx->component_2));
        }

        ctx->entity_count ++;
    }
}

void SingleThreadStaging_remove_from_current() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Remove_from_current, EcsOnFrame, Position);

    IterData ctx = {.component = TPosition};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    EcsEntity e_1 = ecs_new(world, 0);
    EcsEntity e_2 = ecs_new(world, 0);
    EcsEntity e_3 = ecs_new(world, 0);

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
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Remove_from_current, EcsOnFrame, Position);

    IterData ctx = {.component = TPosition, .component_2 = TVelocity};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    EcsEntity e_1 = ecs_new(world, 0);
    EcsEntity e_2 = ecs_new(world, 0);
    EcsEntity e_3 = ecs_new(world, 0);

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
void Add_remove_same_from_current(EcsRows *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    EcsEntity *entities = ecs_column(rows, EcsEntity, 0);
    int i;
    for (i = 0; i < rows->limit; i ++) {
        if (ctx->component) {
            _ecs_add(rows->world, entities[i], ctx->component);

            test_assert( _ecs_has(rows->world, entities[i], ctx->component));

            _ecs_remove(rows->world, entities[i], ctx->component);

            test_assert( !_ecs_has(rows->world, entities[i], ctx->component));
        }

        if (ctx->component_2) {
            _ecs_add(rows->world, entities[i], ctx->component_2);

            test_assert( _ecs_has(rows->world, entities[i], ctx->component_2));

            _ecs_remove(rows->world, entities[i], ctx->component_2);

            test_assert( !_ecs_has(rows->world, entities[i], ctx->component_2));
        }

        ctx->entity_count ++;
    }
}

void SingleThreadStaging_add_remove_same_to_current() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Add_remove_same_from_current, EcsOnFrame, Position);

    IterData ctx = {.component = TRotation};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    EcsEntity e_1 = ecs_new(world, 0);
    EcsEntity e_2 = ecs_new(world, 0);
    EcsEntity e_3 = ecs_new(world, 0);

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
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Add_remove_same_from_current, EcsOnFrame, Position);

    IterData ctx = {.component = TPosition};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    EcsEntity e_1 = ecs_new(world, 0);
    EcsEntity e_2 = ecs_new(world, 0);
    EcsEntity e_3 = ecs_new(world, 0);

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
void Remove_add_same_from_current(EcsRows *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    EcsEntity *entities = ecs_column(rows, EcsEntity, 0);
    int i;
    for (i = 0; i < rows->limit; i ++) {
        if (ctx->component) {
            _ecs_remove(rows->world, entities[i], ctx->component);

            test_assert( !_ecs_has(rows->world, entities[i], ctx->component));

            _ecs_add(rows->world, entities[i], ctx->component);

            test_assert( _ecs_has(rows->world, entities[i], ctx->component));
        }

        if (ctx->component_2) {
            _ecs_remove(rows->world, entities[i], ctx->component_2);

            test_assert( !_ecs_has(rows->world, entities[i], ctx->component_2));

            _ecs_add(rows->world, entities[i], ctx->component_2);

            test_assert( _ecs_has(rows->world, entities[i], ctx->component_2));
        }

        ctx->entity_count ++;
    }
}

void SingleThreadStaging_remove_add_same_to_current() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Remove_add_same_from_current, EcsOnFrame, Position);

    IterData ctx = {.component = TRotation};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    EcsEntity e_1 = ecs_new(world, 0);
    EcsEntity e_2 = ecs_new(world, 0);
    EcsEntity e_3 = ecs_new(world, 0);

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
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Remove_add_same_from_current, EcsOnFrame, Position);

    IterData ctx = {.component = TPosition};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    EcsEntity e_1 = ecs_new(world, 0);
    EcsEntity e_2 = ecs_new(world, 0);
    EcsEntity e_3 = ecs_new(world, 0);

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
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Add_remove_same_from_current, EcsOnFrame, Position);

    IterData ctx = {.component = TRotation, .component_2 = TMass};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    EcsEntity e_1 = ecs_new(world, 0);
    EcsEntity e_2 = ecs_new(world, 0);
    EcsEntity e_3 = ecs_new(world, 0);

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
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Add_remove_same_from_current, EcsOnFrame, Position);

    IterData ctx = {.component = TPosition, .component_2 = TVelocity};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    EcsEntity e_1 = ecs_new(world, 0);
    EcsEntity e_2 = ecs_new(world, 0);
    EcsEntity e_3 = ecs_new(world, 0);

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
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Remove_add_same_from_current, EcsOnFrame, Position);

    IterData ctx = {.component = TRotation, .component_2 = TMass};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    EcsEntity e_1 = ecs_new(world, 0);
    EcsEntity e_2 = ecs_new(world, 0);
    EcsEntity e_3 = ecs_new(world, 0);

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
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Remove_add_same_from_current, EcsOnFrame, Position);

    IterData ctx = {.component = TPosition, .component_2 = TVelocity};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    EcsEntity e_1 = ecs_new(world, 0);
    EcsEntity e_2 = ecs_new(world, 0);
    EcsEntity e_3 = ecs_new(world, 0);

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
void Add_remove_different_from_current(EcsRows *rows) {
    EcsEntity *entities = ecs_column(rows, EcsEntity, 0);
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = 0; i < rows->limit; i ++) {
        if (ctx->component_3) {
            _ecs_add(rows->world, entities[i], ctx->component_3);
            test_assert( _ecs_has(rows->world, entities[i], ctx->component_3));
        }

        if (ctx->component_2) {
            _ecs_remove(rows->world, entities[i], ctx->component_2);
            test_assert( !_ecs_has(rows->world, entities[i], ctx->component_2));
        }

        if (ctx->component) {
            _ecs_add(rows->world, entities[i], ctx->component);
            test_assert( _ecs_has(rows->world, entities[i], ctx->component));        
        }

        ctx->entity_count ++;
    }
}

void SingleThreadStaging_add_remove_different_to_current() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Add_remove_different_from_current, EcsOnFrame, Position);

    IterData ctx = {.component = TRotation, .component_2 = TPosition};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    EcsEntity e_1 = ecs_new(world, 0);
    EcsEntity e_2 = ecs_new(world, 0);
    EcsEntity e_3 = ecs_new(world, 0);

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
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Add_remove_different_from_current, EcsOnFrame, Position);

    IterData ctx = {.component = TRotation, .component_2 = TPosition, .component_3 = TMass};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    EcsEntity e_1 = ecs_new(world, 0);
    EcsEntity e_2 = ecs_new(world, 0);
    EcsEntity e_3 = ecs_new(world, 0);

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
void Add_1_remove_2_different_from_current(EcsRows *rows) {
    EcsEntity *entities = ecs_column(rows, EcsEntity, 0);
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = 0; i < rows->limit; i ++) {
        if (ctx->component) {
            _ecs_add(rows->world, entities[i], ctx->component);
            test_assert( _ecs_has(rows->world, entities[i], ctx->component));        
        }

        if (ctx->component_2) {
            _ecs_remove(rows->world, entities[i], ctx->component_2);
            test_assert( !_ecs_has(rows->world, entities[i], ctx->component_2));
        }

        if (ctx->component_3) {
            _ecs_remove(rows->world, entities[i], ctx->component_3);
            test_assert( !_ecs_has(rows->world, entities[i], ctx->component_3));
        }

        ctx->entity_count ++;
    }
}

void SingleThreadStaging_1_add_2_remove_to_current() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Add_1_remove_2_different_from_current, EcsOnFrame, Position);

    IterData ctx = {.component = TRotation, .component_2 = TPosition, .component_3 = TVelocity};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsId component */
    EcsEntity e_1 = ecs_new(world, 0);
    EcsEntity e_2 = ecs_new(world, 0);
    EcsEntity e_3 = ecs_new(world, 0);

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
void Delete_current(EcsRows *rows) {
    EcsEntity *entities = ecs_column(rows, EcsEntity, 0);
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = 0; i < rows->limit; i ++) {
        ecs_delete(rows->world, entities[i]);
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_delete_current() {
    EcsWorld *world = ecs_init();

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

void SingleThreadStaging_delete_even() {
    // Implement testcase
}

void SingleThreadStaging_delete_new_empty() {
    // Implement testcase
}

void SingleThreadStaging_delete_new_w_component() {
    // Implement testcase
}

void SingleThreadStaging_set_current() {
    // Implement testcase
}

void SingleThreadStaging_set_new_empty() {
    // Implement testcase
}

void SingleThreadStaging_set_new_w_component() {
    // Implement testcase
}

void SingleThreadStaging_set_new_after_add() {
    // Implement testcase
}

void SingleThreadStaging_remove_after_set() {
    // Implement testcase
}

void SingleThreadStaging_delete_after_set() {
    // Implement testcase
}

void SingleThreadStaging_get_after_add() {
    // Implement testcase
}

void SingleThreadStaging_get_after_2_add() {
    // Implement testcase
}

void SingleThreadStaging_get_after_set() {
    // Implement testcase
}

void SingleThreadStaging_get_after_2_set() {
    // Implement testcase
}
