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
    for (i = rows->begin; i < rows->end; i ++) {
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
    for (i = rows->begin; i < rows->end; i ++) {
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
    for (i = rows->begin; i < rows->end; i ++) {
        EcsEntity e = ecs_new(rows->world, 0);
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
    for (i = rows->begin; i < rows->end; i ++) {
        EcsEntity e = ecs_new(rows->world, 0);

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
    for (i = rows->begin; i < rows->end; i ++) {
        EcsEntity e = _ecs_new(rows->world, ctx->component);
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
    for (i = rows->begin; i < rows->end; i ++) {
        EcsEntity e = ecs_new(rows->world, 0);

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
    for (i = rows->begin; i < rows->end; i ++) {
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
    for (i = rows->begin; i < rows->end; i ++) {
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
    for (i = rows->begin; i < rows->end; i ++) {
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
    for (i = rows->begin; i < rows->end; i ++) {
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
    for (i = rows->begin; i < rows->end; i ++) {
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
    for (i = rows->begin; i < rows->end; i ++) {
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
    for (i = rows->begin; i < rows->end; i ++) {
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
    for (i = rows->begin; i < rows->end; i ++) {
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
    for (i = rows->begin; i < rows->end; i ++) {
        ecs_delete(rows->world, entities[i]);
        test_assert( ecs_empty(rows->world, entities[i]));
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

static
void Delete_even(EcsRows *rows) {
    EcsEntity *entities = ecs_column(rows, EcsEntity, 0);
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = rows->begin; i < rows->end; i ++) {
        if (!(entities[i] % 2)) {
            ecs_delete(rows->world, entities[i]);
            test_assert( ecs_empty(rows->world, entities[i]));
        }
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_delete_even() {
    EcsWorld *world = ecs_init();

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
void Delete_new_empty(EcsRows *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = rows->begin; i < rows->end; i ++) {
        EcsEntity e = ecs_new(rows->world, 0);
        ecs_delete(rows->world, e);
        test_assert( ecs_empty(rows->world, e));

        ctx->new_entities[ctx->entity_count] = e;
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_delete_new_empty() {
    EcsWorld *world = ecs_init();

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
void Delete_new_w_component(EcsRows *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    int i;
    for (i = rows->begin; i < rows->end; i ++) {
        EcsEntity e = _ecs_new(rows->world, ctx->component);
        test_assert( !ecs_empty(rows->world, e));

        ecs_delete(rows->world, e);
        test_assert( ecs_empty(rows->world, e));

        ctx->new_entities[ctx->entity_count] = e;
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_delete_new_w_component() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_SYSTEM(world, Delete_new_w_component, EcsOnFrame, Position);

    IterData ctx = {.component = TPosition};
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
void Set_current(EcsRows *rows) {
    EcsEntity *entities = ecs_column(rows, EcsEntity, 0);
    IterData *ctx = ecs_get_context(rows->world);
    
    EcsType TRotation = ctx->component;

    int i;
    for (i = rows->begin; i < rows->end; i ++) {
        ecs_set(rows->world, entities[i], Rotation, {10 + entities[i]});
        test_assert( ecs_has(rows->world, entities[i], Rotation));
        test_assert( ecs_get_ptr(rows->world, entities[i], Rotation) != NULL);
        test_int( ecs_get(rows->world, entities[i], Rotation), 10 + entities[i]);

        ctx->entity_count ++;
    }
}

void SingleThreadStaging_set_current() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Set_current, EcsOnFrame, Position);

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
void Set_new_empty(EcsRows *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    
    EcsType TRotation = ctx->component;

    int i;
    for (i = rows->begin; i < rows->end; i ++) {
        EcsEntity e = ecs_new(rows->world, 0);
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
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Set_new_empty, EcsOnFrame, Position);

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
void Set_new_w_component(EcsRows *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    
    EcsType TPosition = ctx->component;
    EcsType TRotation = ctx->component_2;

    int i;
    for (i = rows->begin; i < rows->end; i ++) {
        EcsEntity e = ecs_new(rows->world, Position);
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
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Set_new_w_component, EcsOnFrame, Position);

    IterData ctx = {.component = TPosition, .component_2 = TRotation};
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
void Set_existing_new_w_component(EcsRows *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    
    EcsType TPosition = ctx->component;

    int i;
    for (i = rows->begin; i < rows->end; i ++) {
        EcsEntity e = ecs_new(rows->world, Position);
        test_assert( ecs_has(rows->world, e, Position));

        ecs_set(rows->world, e, Position, {10 + e, 20 + e});
        test_assert( ecs_has(rows->world, e, Position));

        ctx->new_entities[ctx->entity_count] = e;
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_set_existing_new_w_component() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Set_existing_new_w_component, EcsOnFrame, Position);

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
void Set_new_after_add(EcsRows *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    
    EcsType TPosition = ctx->component;

    int i;
    for (i = rows->begin; i < rows->end; i ++) {
        EcsEntity e = ecs_new(rows->world, 0);
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
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Set_new_after_add, EcsOnFrame, Position);

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
void Remove_after_set(EcsRows *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    
    EcsType TPosition = ctx->component;

    int i;
    for (i = rows->begin; i < rows->end; i ++) {
        EcsEntity e = ecs_new(rows->world, 0);
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
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Remove_after_set, EcsOnFrame, Position);

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
void Delete_after_set(EcsRows *rows) {
    IterData *ctx = ecs_get_context(rows->world);
    
    EcsType TPosition = ctx->component;

    int i;
    for (i = rows->begin; i < rows->end; i ++) {
        EcsEntity e = ecs_new(rows->world, 0);
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
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Delete_after_set, EcsOnFrame, Position);

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

void SingleThreadStaging_add_in_on_add() {
    // Implement testcase
}

void SingleThreadStaging_remove_in_on_add() {
    // Implement testcase
}

void SingleThreadStaging_add_in_on_remove() {
    // Implement testcase
}

void SingleThreadStaging_remove_in_on_remove() {
    // Implement testcase
}

void SingleThreadStaging_add_in_on_set() {
    // Implement testcase
}

void SingleThreadStaging_remove_in_on_set() {
    // Implement testcase
}
