#include <api.h>

static ECS_COMPONENT_DECLARE(Velocity);

void SingleThreadStaging_setup() {
    ecs_log_set_level(-3);
}

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
    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Mass);
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
        ecs_entity_t e = 0;
        if (ctx->component) {
            e = ecs_new_w_id(it->world, ctx->component);
        }
        ctx->new_entities[ctx->entity_count] = e;
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_new_w_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Mass);
    ECS_SYSTEM(world, New_w_component, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_id(Velocity)};
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

static
void NewEmpty_w_count(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);

    ctx->new_entities[ctx->entity_count] = ecs_bulk_new(it->world, 0, 1000)[0];
    ctx->entity_count ++;
}

void SingleThreadStaging_new_empty_w_count() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, e1, Position);
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
    const ecs_entity_t *ids = NULL;
    if (ctx->component) {
        ids = ecs_bulk_new_w_id(it->world, ctx->component, 1000);
    }
    ctx->new_entities[ctx->entity_count] = ids[0];
    ctx->entity_count ++;
}

void SingleThreadStaging_new_component_w_count() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, e1, Position);
    ECS_SYSTEM(world, New_w_component_w_count, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_id(Position)};
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

static
void Add_to_new_empty(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = ecs_new(it->world, 0);
        if (ctx->component) {
            ecs_add_id(it->world, e, ctx->component);
        }
        if (ctx->component_2) {
            ecs_add_id(it->world, e, ctx->component_2);
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
    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Mass);
    ECS_SYSTEM(world, Add_to_new_empty, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_id(Velocity)};
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
    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Mass);
    ECS_SYSTEM(world, Add_to_new_empty, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_id(Velocity), .component_2 = ecs_id(Rotation)};
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
            ecs_add_id(it->world, e, ctx->component);
            ecs_remove_id(it->world, e, ctx->component);
        }

        if (ctx->component_2) {
            ecs_add_id(it->world, e, ctx->component_2);
            ecs_remove_id(it->world, e, ctx->component_2);
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
    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Mass);
    ECS_SYSTEM(world, Add_remove_same_from_new, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_id(Velocity)};
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
    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Mass);
    ECS_SYSTEM(world, Add_remove_same_from_new, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_id(Position), .component_2 = ecs_id(Velocity)};
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
        ecs_entity_t e = ecs_new_w_id(it->world, ctx->component);

        if (ctx->component_2) {
            ecs_add_id(it->world, e, ctx->component_2);
            ecs_remove_id(it->world, e, ctx->component_2);
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
    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Mass);
    ECS_SYSTEM(world, Add_remove_same_from_new_w_component, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_id(Position), .component_2 = ecs_id(Velocity)};
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
            ecs_add_id(it->world, e, ctx->component_3);
        }

        if (ctx->component_2) {
            ecs_remove_id(it->world, e, ctx->component_2);
        }

        if (ctx->component) {
            ecs_add_id(it->world, e, ctx->component);
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
    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Mass);
    ECS_SYSTEM(world, Add_remove_different_from_new_empty, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_id(Rotation), .component_2 = ecs_id(Position), .component_3 = ecs_id(Mass)};
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
    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Mass);
    ECS_SYSTEM(world, Add_remove_different_from_new_empty, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_id(Rotation), .component_2 = ecs_id(Position), .component_3 = ecs_id(Position)};
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
    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Mass);
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
    ECS_SYSTEM(world, Clone_current_w_value, EcsOnUpdate, Position);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);

    ecs_set(world, e1, Position, {10, 20});

    ecs_set(world, e2, Position, {11, 21});
    ecs_set(world, e2, Velocity, {30, 40});

    ecs_set(world, e3, Position, {12, 22});
    ecs_set(world, e3, Mass, {50});

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
            ecs_add_id(it->world, it->entities[i], ctx->component);
        }
        if (ctx->component_2) {
            ecs_add_id(it->world, it->entities[i], ctx->component_2);
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
    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Mass);
    ECS_SYSTEM(world, Add_to_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_id(Rotation)};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e1, Rotation));

    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e2, Velocity));
    test_assert( ecs_has(world, e2, Rotation));

    test_assert( ecs_has(world, e3, Position));
    test_assert( ecs_has(world, e3, Mass));
    test_assert( ecs_has(world, e3, Rotation));

    ecs_fini(world);
}

void SingleThreadStaging_2_add_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Mass);
    ECS_SYSTEM(world, Add_to_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_id(Velocity), .component_2 = ecs_id(Rotation)};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e1, Velocity));
    test_assert( ecs_has(world, e1, Rotation));

    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e2, Velocity));
    test_assert( ecs_has(world, e2, Rotation));

    test_assert( ecs_has(world, e3, Position));
    test_assert( ecs_has(world, e1, Velocity));
    test_assert( ecs_has(world, e3, Mass));
    test_assert( ecs_has(world, e3, Rotation));

    ecs_fini(world);    
}

static
void Remove_from_current(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);

    int i;
    for (i = it->count - 1; i >= 0; i --) {
        ecs_entity_t e = it->entities[i];

        if (ctx->component) {
            ecs_remove_id(it->world, e, ctx->component);
        }

        if (ctx->component_2) {
            ecs_remove_id(it->world, e, ctx->component_2);
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

    IterData ctx = {.component = ecs_id(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_entity_t e3 = ecs_new(world, 0);

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( !ecs_has(world, e1, Position));
    test_assert( !ecs_get_type(world, e1));

    test_assert( !ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e2, Velocity));

    test_assert( !ecs_has(world, e3, Position));
    test_assert( ecs_has(world, e3, Mass));

    ecs_fini(world);
}

void SingleThreadStaging_remove_2_from_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Remove_from_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_id(Position), .component_2 = ecs_id(Velocity)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_entity_t e3 = ecs_new(world, 0);

    ecs_add(world, e1, Position);
    ecs_add(world, e1, Velocity);
    ecs_add(world, e2, Position);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Velocity);
    ecs_add(world, e3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( !ecs_has(world, e1, Position));
    test_assert( !ecs_has(world, e1, Velocity));
    test_assert( !ecs_get_type(world, e1));

    test_assert( !ecs_has(world, e2, Position));
    test_assert( !ecs_has(world, e2, Velocity));
    test_assert( !ecs_get_type(world, e2));

    test_assert( !ecs_has(world, e3, Position));
    test_assert( !ecs_has(world, e3, Velocity));
    test_assert( ecs_has(world, e3, Mass));

    ecs_fini(world);
}

static
void Add_remove_same_from_current(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    int i;
    for (i = 0; i < it->count; i ++) {
        if (ctx->component) {
            ecs_add_id(it->world, it->entities[i], ctx->component);
            ecs_remove_id(it->world, it->entities[i], ctx->component);
        }

        if (ctx->component_2) {
            ecs_add_id(it->world, it->entities[i], ctx->component_2);
            ecs_remove_id(it->world, it->entities[i], ctx->component_2);
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

    IterData ctx = {.component = ecs_id(Rotation)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_entity_t e3 = ecs_new(world, 0);

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( ecs_has(world, e1, Position));
    test_assert( !ecs_has(world, e1, Rotation));

    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e2, Velocity));
    test_assert( !ecs_has(world, e2, Rotation));

    test_assert( ecs_has(world, e3, Position));
    test_assert( ecs_has(world, e3, Mass));
    test_assert( !ecs_has(world, e3, Rotation));

    ecs_fini(world);
}

void SingleThreadStaging_add_remove_same_existing_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Add_remove_same_from_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_id(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_entity_t e3 = ecs_new(world, 0);

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( !ecs_has(world, e1, Position));
    test_assert( !ecs_get_type(world, e1));

    test_assert( !ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e2, Velocity));

    test_assert( !ecs_has(world, e3, Position));
    test_assert( ecs_has(world, e3, Mass));

    ecs_fini(world);
}

static
void Remove_add_same_from_current(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    int i;
    for (i = 0; i < it->count; i ++) {
        if (ctx->component) {
            ecs_remove_id(it->world, it->entities[i], ctx->component);
            ecs_add_id(it->world, it->entities[i], ctx->component);
        }

        if (ctx->component_2) {
            ecs_remove_id(it->world, it->entities[i], ctx->component_2);
            ecs_add_id(it->world, it->entities[i], ctx->component_2);
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

    IterData ctx = {.component = ecs_id(Rotation)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_entity_t e3 = ecs_new(world, 0);

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e1, Rotation));

    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e2, Velocity));
    test_assert( ecs_has(world, e2, Rotation));

    test_assert( ecs_has(world, e3, Position));
    test_assert( ecs_has(world, e3, Mass));
    test_assert( ecs_has(world, e3, Rotation));

    ecs_fini(world);
}

void SingleThreadStaging_remove_add_same_existing_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Remove_add_same_from_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_id(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_entity_t e3 = ecs_new(world, 0);

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( ecs_has(world, e1, Position));

    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e2, Velocity));

    test_assert( ecs_has(world, e3, Position));
    test_assert( ecs_has(world, e3, Mass));

    ecs_fini(world);
}

void SingleThreadStaging_add_remove_2_same_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Add_remove_same_from_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_id(Rotation), .component_2 = ecs_id(Mass)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_entity_t e3 = ecs_new(world, 0);

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( ecs_has(world, e1, Position));
    test_assert( !ecs_has(world, e1, Rotation));
    test_assert( !ecs_has(world, e1, Mass));

    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e2, Velocity));
    test_assert( !ecs_has(world, e2, Rotation));
    test_assert( !ecs_has(world, e2, Mass));

    test_assert( ecs_has(world, e3, Position));
    test_assert( !ecs_has(world, e3, Mass));
    test_assert( !ecs_has(world, e3, Rotation));

    ecs_fini(world);
}

void SingleThreadStaging_add_remove_2_same_existing_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Add_remove_same_from_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_id(Position), .component_2 = ecs_id(Velocity)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_entity_t e3 = ecs_new(world, 0);

    ecs_add(world, e1, Position);
    ecs_add(world, e1, Velocity);
    ecs_add(world, e2, Position);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Velocity);
    ecs_add(world, e3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( !ecs_has(world, e1, Position));
    test_assert( !ecs_has(world, e1, Velocity));
    test_assert( !ecs_get_type(world, e1));

    test_assert( !ecs_has(world, e2, Position));
    test_assert( !ecs_has(world, e2, Velocity));
    test_assert( !ecs_get_type(world, e2));

    test_assert( !ecs_has(world, e3, Position));
    test_assert( !ecs_has(world, e3, Velocity));
    test_assert( ecs_has(world, e3, Mass));

    ecs_fini(world);
}

void SingleThreadStaging_remove_add_2_same_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Remove_add_same_from_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_id(Rotation), .component_2 = ecs_id(Mass)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_entity_t e3 = ecs_new(world, 0);

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e1, Rotation));
    test_assert( ecs_has(world, e1, Mass));

    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e2, Velocity));
    test_assert( ecs_has(world, e2, Rotation));
    test_assert( ecs_has(world, e2, Mass));

    test_assert( ecs_has(world, e3, Position));
    test_assert( ecs_has(world, e3, Mass));
    test_assert( ecs_has(world, e3, Rotation));

    ecs_fini(world);
}

static
void AddRemoveAdd(ecs_iter_t *it) {
    ecs_id_t ecs_id(Velocity) = ecs_field_id(it, 2);

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

    ECS_SYSTEM(world, AddRemoveAdd, EcsOnUpdate, Position, Velocity());

    /* Create entities from scratch so they don't have the EcsName component */
    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_entity_t e3 = ecs_new(world, Position);

    ecs_progress(world, 1);

    test_assert( ecs_has(world, e1, Velocity));
    test_assert( ecs_has(world, e2, Velocity));
    test_assert( ecs_has(world, e3, Velocity));

    ecs_fini(world);
}

void SingleThreadStaging_remove_add_2_same_existing_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Remove_add_same_from_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_id(Position), .component_2 = ecs_id(Velocity)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_entity_t e3 = ecs_new(world, 0);

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Velocity);
    ecs_add(world, e3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e1, Velocity));

    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e2, Velocity));

    test_assert( ecs_has(world, e3, Position));
    test_assert( ecs_has(world, e3, Velocity));
    test_assert( ecs_has(world, e3, Mass));

    ecs_fini(world);
}

static
void Add_remove_different_from_current(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    int i;
    for (i = 0; i < it->count; i ++) {
        if (ctx->component_3) {
            ecs_add_id(it->world, it->entities[i], ctx->component_3);
        }

        if (ctx->component_2) {
            ecs_remove_id(it->world, it->entities[i], ctx->component_2);
        }

        if (ctx->component) {
            ecs_add_id(it->world, it->entities[i], ctx->component);
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

    IterData ctx = {.component = ecs_id(Rotation), .component_2 = ecs_id(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_entity_t e3 = ecs_new(world, 0);

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Velocity);
    ecs_add(world, e3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( !ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e1, Rotation));

    test_assert( !ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e2, Velocity));
    test_assert( ecs_has(world, e2, Rotation));

    test_assert( !ecs_has(world, e3, Position));
    test_assert( ecs_has(world, e3, Velocity));
    test_assert( ecs_has(world, e3, Mass));
    test_assert( ecs_has(world, e3, Rotation));

    ecs_fini(world);
}

void SingleThreadStaging_2_add_1_remove_to_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_SYSTEM(world, Add_remove_different_from_current, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_id(Rotation), .component_2 = ecs_id(Position), .component_3 = ecs_id(Mass)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_entity_t e3 = ecs_new(world, 0);

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Velocity);
    ecs_add(world, e3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( !ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e1, Mass));
    test_assert( ecs_has(world, e1, Rotation));

    test_assert( !ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e2, Velocity));
    test_assert( ecs_has(world, e2, Mass));
    test_assert( ecs_has(world, e2, Rotation));

    test_assert( !ecs_has(world, e3, Position));
    test_assert( ecs_has(world, e3, Velocity));
    test_assert( ecs_has(world, e3, Mass));
    test_assert( ecs_has(world, e3, Rotation));

    ecs_fini(world);
}

static
void Add_1_remove_2_different_from_current(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    int i;
    for (i = 0; i < it->count; i ++) {
        if (ctx->component) {
            ecs_add_id(it->world, it->entities[i], ctx->component);
        }

        if (ctx->component_2) {
            ecs_remove_id(it->world, it->entities[i], ctx->component_2);
        }

        if (ctx->component_3) {
            ecs_remove_id(it->world, it->entities[i], ctx->component_3);
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

    IterData ctx = {.component = ecs_id(Rotation), .component_2 = ecs_id(Position), .component_3 = ecs_id(Velocity)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_entity_t e3 = ecs_new(world, 0);

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Velocity);
    ecs_add(world, e3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( !ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e1, Rotation));

    test_assert( !ecs_has(world, e2, Position));
    test_assert( !ecs_has(world, e2, Velocity));
    test_assert( ecs_has(world, e2, Rotation));

    test_assert( !ecs_has(world, e3, Position));
    test_assert( !ecs_has(world, e3, Velocity));
    test_assert( ecs_has(world, e3, Mass));
    test_assert( ecs_has(world, e3, Rotation));

    ecs_fini(world);
}

static
void Delete_current(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_delete(it->world, it->entities[i]);
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_delete_current() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Mass);
    ECS_SYSTEM(world, Delete_current, EcsOnUpdate, Position);

    IterData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);
    
    test_assert(!ecs_is_alive(world, e1));
    test_assert(!ecs_is_alive(world, e2));
    test_assert(!ecs_is_alive(world, e3));

    ecs_fini(world);
}

static
void Delete_even(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    int i;
    for (i = 0; i < it->count; i ++) {
        if (!(it->entities[i] % 2)) {
            ecs_delete(it->world, it->entities[i]);
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
    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity);
    ECS_ENTITY(world, e5, Position, Mass);
    ECS_ENTITY(world, e6, Position, Mass);
    ECS_ENTITY(world, e7, Position, Rotation);
    ECS_ENTITY(world, e8, Position, Rotation);
    ECS_SYSTEM(world, Delete_even, EcsOnUpdate, Position);

    IterData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 8);

    if (!(e1 % 2))
        test_assert( !ecs_is_alive(world, e1));
    else
        test_assert ( ecs_is_alive(world, e1));

    if (!(e2 % 2))
        test_assert( !ecs_is_alive(world, e2));
    else
        test_assert ( ecs_is_alive(world, e2));

    if (!(e3 % 2))
        test_assert( !ecs_is_alive(world, e3));
    else
        test_assert ( ecs_is_alive(world, e3));

    if (!(e4 % 2))
        test_assert( !ecs_is_alive(world, e4));
    else
        test_assert ( ecs_is_alive(world, e4));

    if (!(e5 % 2))
        test_assert( !ecs_is_alive(world, e5));
    else
        test_assert ( ecs_is_alive(world, e5));

    if (!(e6 % 2))
        test_assert( !ecs_is_alive(world, e6));
    else
        test_assert ( ecs_is_alive(world, e6));

    if (!(e7 % 2))
        test_assert( !ecs_is_alive(world, e7));
    else
        test_assert ( ecs_is_alive(world, e7));

    if (!(e8 % 2))
        test_assert( !ecs_is_alive(world, e8));
    else
        test_assert ( ecs_is_alive(world, e8));

    ecs_fini(world);
}

static
void Delete_new_empty(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = ecs_new(it->world, 0);
        ecs_delete(it->world, e);
        ctx->new_entities[ctx->entity_count] = e;
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_delete_new_empty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Mass);
    ECS_SYSTEM(world, Delete_new_empty, EcsOnUpdate, Position);

    IterData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !ecs_is_alive(world, ctx.new_entities[i]));
    }

    ecs_fini(world);
}

static
void Delete_new_w_component(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = ecs_new_w_id(it->world, ctx->component);
        ecs_delete(it->world, e);
        ctx->new_entities[ctx->entity_count] = e;
        ctx->entity_count ++;
    }
}

void SingleThreadStaging_delete_new_w_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Mass);
    ECS_SYSTEM(world, Delete_new_w_component, EcsOnUpdate, Position);

    IterData ctx = {.component = ecs_id(Position)};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !ecs_is_alive(world, ctx.new_entities[i]));
    }

    ecs_fini(world);
}

static
void Set_current(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
        ecs_entity_t ecs_id(Rotation) = ctx->component;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], Rotation, {10 + it->entities[i]});
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

    IterData ctx = {.component = ecs_id(Rotation)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_entity_t e3 = ecs_new(world, 0);

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Velocity);
    ecs_add(world, e3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e1, Rotation));
    test_int( *ecs_get(world, e1, Rotation), e1 + 10);

    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e2, Velocity));
    test_assert( ecs_has(world, e2, Rotation));
    test_int( *ecs_get(world, e2, Rotation), e2 + 10);

    test_assert( ecs_has(world, e3, Position));
    test_assert( ecs_has(world, e3, Velocity));
    test_assert( ecs_has(world, e3, Mass));
    test_assert( ecs_has(world, e3, Rotation));
    test_int( *ecs_get(world, e3, Rotation), e3 + 10);

    ecs_fini(world);
}

static
void Set_new_empty(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);
    ecs_entity_t ecs_id(Rotation) = ctx->component;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = ecs_new(it->world, 0);
        ecs_set(it->world, e, Rotation, {10 + e});
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

    IterData ctx = {.component = ecs_id(Rotation)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_entity_t e3 = ecs_new(world, 0);

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Velocity);
    ecs_add(world, e3, Mass);

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

    ecs_entity_t ecs_id(Rotation) = ctx->component_2;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = ecs_new_w_id(it->world, ctx->component);
        ecs_set(it->world, e, Rotation, {10 + e});
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

    IterData ctx = {.component = ecs_id(Position), .component_2 = ecs_id(Rotation)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_entity_t e3 = ecs_new(world, 0);

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Velocity);
    ecs_add(world, e3, Mass);

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
    
    ecs_entity_t ecs_id(Position) = ctx->component;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = ecs_new_w_id(it->world, ctx->component);
        ecs_set(it->world, e, Position, {10 + e, 20 + e});
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

    IterData ctx = {.component = ecs_id(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_entity_t e3 = ecs_new(world, 0);

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Velocity);
    ecs_add(world, e3, Mass);

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
    
    ecs_entity_t ecs_id(Position) = ctx->component;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = ecs_new(it->world, 0);
        ecs_add_id(it->world, e, ctx->component);
        ecs_set(it->world, e, Position, {10 + e, 20 + e});
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

    IterData ctx = {.component = ecs_id(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_entity_t e3 = ecs_new(world, 0);

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Velocity);
    ecs_add(world, e3, Mass);

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
    
    ecs_entity_t ecs_id(Position) = ctx->component;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = ecs_new(it->world, 0);
        ecs_set(it->world, e, Position, {10 + e, 20 + e});
        ecs_remove_id(it->world, e, ctx->component);

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

    IterData ctx = {.component = ecs_id(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_entity_t e3 = ecs_new(world, 0);

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Velocity);
    ecs_add(world, e3, Mass);

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
    
    ecs_entity_t ecs_id(Position) = ctx->component;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = ecs_new(it->world, 0);
        test_assert( !ecs_get_type(it->world, e));

        ecs_set(it->world, e, Position, {10 + e, 20 + e});
        ecs_delete(it->world, e);

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

    IterData ctx = {.component = ecs_id(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_entity_t e3 = ecs_new(world, 0);

    ecs_add(world, e1, Position);
    ecs_add(world, e2, Position);
    ecs_add(world, e2, Velocity);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Velocity);
    ecs_add(world, e3, Mass);

    ecs_progress(world, 1);

    test_int(ctx.entity_count, 3);

    int i;
    for (i = 0; i < ctx.entity_count; i ++) {
        test_assert(ctx.new_entities[i] != 0);
        test_assert( !ecs_is_alive(world, ctx.new_entities[i]));
    }

    ecs_fini(world);
}

void SingleThreadStaging_add_to_current_in_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, Add_to_current, EcsOnAdd, Position);

    IterData ctx = {.component = ecs_id(Velocity)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_entity_t e3 = ecs_new(world, Position);

    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e2, Position));
    test_assert( ecs_has(world, e3, Position));

    test_assert( ecs_has(world, e1, Velocity));
    test_assert( ecs_has(world, e2, Velocity));
    test_assert( ecs_has(world, e3, Velocity));

    ecs_fini(world);
}

void SingleThreadStaging_remove_from_current_in_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, Remove_from_current, EcsOnAdd, Position);

    IterData ctx = {.component = ecs_id(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);

    test_assert( !ecs_has(world, e1, Position));
    test_assert( !ecs_has(world, e2, Position));
    test_assert( !ecs_has(world, e3, Position));

    test_assert( ecs_has(world, e1, Velocity));
    test_assert( ecs_has(world, e2, Velocity));
    test_assert( ecs_has(world, e3, Velocity));

    ecs_fini(world);
}

void SingleThreadStaging_remove_added_component_in_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, Remove_from_current, EcsOnAdd, Position);

    IterData ctx = {.component = ecs_id(Position)};
    ecs_set_context(world, &ctx);

    /* Create entities from scratch so they don't have the EcsName component */
    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    
    test_assert( !ecs_has(world, e1, Position));
    test_assert( !ecs_has(world, e2, Position));
    test_assert( !ecs_has(world, e3, Position));

    test_assert( ecs_has(world, e1, Velocity));
    test_assert( ecs_has(world, e2, Velocity));
    test_assert( ecs_has(world, e3, Velocity));

    ecs_fini(world);
}

static Probe pv_probe;

static
void On_PV(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 1);
    Velocity *v = ecs_field(it, Velocity, 2);

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

    ECS_SYSTEM(world, Add_to_current, EcsOnUpdate, Position, [out] !Velocity);
    ECS_SYSTEM(world, On_PV, EcsOnUpdate, Position, Velocity);

    IterData add_ctx = {.component = ecs_id(Velocity)};
    ecs_set_context(world, &add_ctx);

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_entity_t e3 = ecs_new(world, Position);

    ecs_progress(world, 1);

    test_int(pv_probe.count, 3);
    test_int(pv_probe.invoked, 1);
    test_int(pv_probe.system, On_PV);
    test_int(pv_probe.term_count, 2);
    test_null(pv_probe.param);

    test_int(pv_probe.e[0], e1);
    test_int(pv_probe.e[1], e2);
    test_int(pv_probe.e[2], e3);
    test_int(pv_probe.c[0][0], ecs_id(Position));
    test_int(pv_probe.s[0][0], 0);
    test_int(pv_probe.c[0][1], ecs_id(Velocity));
    test_int(pv_probe.s[0][1], 0);

    ecs_fini(world);
}

static
void Set_velocity_on_new(ecs_iter_t *it) {
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, 0, Velocity, {10, 20});
    }
}

static
void On_V(ecs_iter_t *it) {
    Velocity *v = ecs_field(it, Velocity, 1);

    probe_iter(it);

    int i;
    for (i = 0; i < it->count; i ++) {
        v[i].x = v[i].y;
    }
}

void SingleThreadStaging_match_table_created_w_new_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);

    ECS_SYSTEM(world, Set_velocity_on_new, EcsOnUpdate, Position);
    ECS_SYSTEM(world, On_V, EcsOnUpdate, Velocity);

    ecs_set(world, 0, Position, {10, 20});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);
    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, On_V);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.c[0][0], ecs_id(Velocity));
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}

void SingleThreadStaging_match_table_created_w_new_in_on_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);

    ECS_OBSERVER(world, Set_velocity_on_new, EcsOnSet, Position);
    ECS_SYSTEM(world, On_V, EcsOnUpdate, Velocity);

    IterData add_ctx = {.component = ecs_id(Velocity)};
    ecs_set_context(world, &add_ctx);

    ecs_set(world, 0, Position, {10, 20});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);
    ecs_enable(world, Set_velocity_on_new, false);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, On_V);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.c[0][0], ecs_id(Velocity));
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}

static
void Create_container(ecs_iter_t *it) {
    probe_iter(it);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_new_w_pair(it->world, EcsChildOf, it->entities[i]);
    }
}

void SingleThreadStaging_merge_table_w_container_added_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Create_container, EcsOnUpdate, Position);

    ECS_ENTITY(world, e1, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Create_container);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}

void SingleThreadStaging_merge_table_w_container_added_on_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_OBSERVER(world, Create_container, EcsOnSet, Position);

    /* Entity is not yet a container. Adding this entity to another entity would
     * cause an error */
    ECS_ENTITY(world, e1, 0);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_set(world, e1, Position, {10, 20});

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Create_container);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}

/* Hacky way to test if entities are properly merged by system */
static ecs_entity_t g_child = 0;
static ecs_entity_t g_parent = 0;

static
void Create_container_reverse(ecs_iter_t *it) {

    probe_iter(it);

    ecs_world_t *world = it->world;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t child = ecs_new(world, 0);
        ecs_entity_t parent = ecs_new(world, 0);
        test_assert(child != 0);
        test_assert(parent != 0);

        ecs_add_pair(world, child, EcsChildOf, parent);

        g_parent = parent;
        g_child = child;
    }
}

void SingleThreadStaging_merge_table_w_container_added_on_set_reverse() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_OBSERVER(world, Create_container_reverse, EcsOnSet, Position);

    /* Entity is not yet a container. Adding this entity to another entity would
     * cause an error */
    ECS_ENTITY(world, e1, 0);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_set(world, e1, Position, {10, 20});

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Create_container_reverse);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    test_assert(g_parent != 0);
    test_assert(g_child != 0);

    test_assert( ecs_has_pair(world, g_child, EcsChildOf, g_parent));

    ecs_fini(world);
}

static
void Task(ecs_iter_t *it) {
    ecs_id_t ecs_id(Position) = ecs_field_id(it, 1);

    ecs_entity_t *e = ecs_get_context(it->world);

    ecs_add(it->world, *e, Position);
}

void SingleThreadStaging_merge_after_tasks() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);

    ECS_SYSTEM(world, Task, EcsOnUpdate, Position());

    ecs_set_context(world, &e);

    ecs_progress(world, 1);

    test_assert( ecs_has(world, e, Position));

    ecs_fini(world);
}

static
void OverrideAfterRemove(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 1);
    ecs_id_t ecs_id(Position) = ecs_field_id(it, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        test_int(p[i].x, 30);
        test_int(p[i].y, 40);
        ecs_remove(it->world, it->entities[i], Position);
        ecs_add(it->world, it->entities[i], Position);
    }
}

void SingleThreadStaging_override_after_remove_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, base, Position, Disabled);

    ECS_ENTITY(world, e, Position, (IsA, base));

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
    ecs_id_t ecs_id(Velocity) = ecs_field_id(it, 2);
    
    ecs_world_t *world = it->world;

    /* Create parent */
    ecs_entity_t parent = ecs_new(world, Velocity);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_add_pair(world, e, EcsChildOf, parent);
    }
}

void SingleThreadStaging_get_parent_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, GetParentInProgress, EcsOnUpdate, Position, Velocity());

    ecs_entity_t e = ecs_new(world, Position);

    ecs_progress(world, 1);

    ecs_entity_t parent = ecs_get_target(world, e, EcsChildOf, 0);
    test_assert(parent != 0);

    ecs_fini(world);
}

static
void AddInProgress(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    ecs_id_t ecs_id(Position) = ecs_field_id(it, 1);
    ecs_id_t ecs_id(Velocity) = ecs_field_id(it, 2);
    
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(world, it->entities[i], Position, {1, 1});
        ecs_set(world, it->entities[i], Velocity, {2, 2});
    }
}

static
void Move(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 1);
    Velocity *v = ecs_field(it, Velocity, 2);

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

    ECS_SYSTEM(world, AddInProgress, EcsOnUpdate, Position, [out] !Velocity);
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

static int move_position = 0;
static
ECS_MOVE(Position, dst, src, {
    move_position ++;
    dst->x = src->x;
    dst->y = src->y;
})

void SingleThreadStaging_clear_stage_after_merge() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .move = ecs_move(Position)
    });

    ecs_entity_t e = ecs_new(world, 0);

    ecs_defer_begin(world);
    ecs_set(world, e, Position, {10, 20});
    ecs_defer_end(world);
    
    test_int(move_position, 1);
    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_defer_begin(world);
    ecs_set(world, e, Position, {30, 40});
    ecs_defer_end(world);  

    /* move will only happen the first time ecs_set() is called when the
     * component is created while deferred */
    test_int(move_position, 1);
    p = ecs_get(world, e, Position);
    test_int(p->x, 30);
    test_int(p->y, 40);      

    ecs_fini(world);
}

void MutableTest(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    Velocity *v = ecs_field(it, Velocity, 2);
    ecs_id_t ecs_id(Velocity) = ecs_field_id(it, 2);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        bool is_added = !ecs_has(world, it->entities[i], Velocity);
        Velocity *v_mut = ecs_get_mut(world, it->entities[i], Velocity);

        test_assert(v_mut != NULL);

        // Even though component is added to stage, is_added should only be true
        // if the component is added for the first time, which requires the app
        // to init the component value.
        if (!v) {
            test_bool(is_added, true);
        } else {
            test_bool(is_added, false);
            test_assert(v_mut == v);
        }

        if (is_added) {
            v_mut->x = 0;
            v_mut->y = 0;
        }

        v_mut->x ++;
        v_mut->y ++;
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

    Velocity *v = ecs_field(it, Velocity, 2);
    ecs_id_t ecs_id(Velocity) = ecs_field_id(it, 2);
    ecs_id_t ecs_id(MyTag) = ecs_field_id(it, 3);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_add(world, it->entities[i], MyTag);

        bool is_added = !ecs_has(world, it->entities[i], Velocity);
        Velocity *v_mut = ecs_get_mut(world, it->entities[i], Velocity);

        test_assert(v_mut != NULL);

        // Even though component is added to stage, is_added should only be true
        // if the component is added for the first time, which requires the app
        // to init the component value.
        if (!v) {
            test_bool(is_added, true);
        } else {
            test_bool(is_added, false);
            test_assert(v_mut == v);
        }

        if (is_added) {
            v_mut->x = 0;
            v_mut->y = 0;
        }

        v_mut->x ++;
        v_mut->y ++;
    }
}

typedef bool MyBool;

void SingleThreadStaging_get_mutable_w_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, MyBool);
    ECS_SYSTEM(world, MutableTest_w_Add, EcsOnUpdate, Position, ?Velocity, MyBool());

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

static
void OnAdd(ecs_iter_t *it) {
    Velocity *v = ecs_field(it, Velocity, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        v->x = 1;
        v->y = 2;
    }
}

static
void AddInProgress2(ecs_iter_t *it) {
    ecs_id_t ecs_id(Velocity) = ecs_field_id(it, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add(it->world, it->entities[i], Velocity);
    }
}

void SingleThreadStaging_on_add_after_new_type_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_SYSTEM(world, AddInProgress2, EcsOnUpdate, Position, Velocity());
    ECS_OBSERVER(world, OnAdd, EcsOnAdd, Velocity);

    ecs_entity_t e = ecs_new(world, Position);

    ecs_progress(world, 1);

    test_assert( ecs_has(world, e, Velocity));
    const Velocity *v = ecs_get(world, e, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 2);

    ecs_fini(world);
}

void SingleThreadStaging_lock_table() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.terms = {{ecs_id(Position)}}
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        ecs_table_t *table = it.table;
        test_assert(table != NULL);
        
        ecs_table_lock(world, table);

        test_expect_abort();
        ecs_remove(world, e, Position);
    }

    test_assert(false); // Should never get here
}

void SingleThreadStaging_recursive_lock_table() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.terms = {{ecs_id(Position)}}
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        ecs_table_t *table = it.table;
        test_assert(table != NULL);
        
        ecs_table_lock(world, table);
        ecs_table_lock(world, table);
        ecs_table_unlock(world, table);

        test_expect_abort();
        ecs_remove(world, e, Position);
    }

    test_assert(false); // Should never get here
}

void SingleThreadStaging_modify_after_lock() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.terms = {{ecs_id(Position)}}
    });

    int32_t count = 0;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {    
        ecs_table_t *table = it.table;
        test_assert(table != NULL);
                    
        ecs_table_lock(world, table);
        count ++;
        ecs_table_unlock(world, table);
    }

    test_int(count, 1);

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));

    ecs_fini(world);
}

void SingleThreadStaging_get_case_from_stage() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Switch, Union);
    ECS_TAG(world, CaseOne);
    ECS_TAG(world, CaseTwo);

    ecs_entity_t e = ecs_new_w_pair(world, Switch, CaseOne);

    ecs_frame_begin(world, 1);

    ecs_readonly_begin(world);

    ecs_world_t *stage = ecs_get_stage(world, 0);

    ecs_entity_t c = ecs_get_target(world, e, Switch, 0);
    test_assert(c == CaseOne);

    c = ecs_get_target(stage, e, Switch, 0);
    test_assert(c == CaseOne);

    ecs_readonly_end(world);

    ecs_frame_end(world);

    ecs_fini(world);
}

void SingleThreadStaging_get_object_from_stage() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_new_id(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_readonly_begin(world);

    ecs_world_t *stage = ecs_get_stage(world, 0);

    test_assert(parent == ecs_get_target(stage, e, EcsChildOf, 0));

    ecs_readonly_end(world);

    ecs_fini(world);
}

void SingleThreadStaging_add_to_world_while_readonly() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_id(world);

    ecs_readonly_begin(world);
    ecs_add(world, e, Tag);
    test_assert(!ecs_has(world, e, Tag));
    ecs_readonly_end(world);

    test_assert(ecs_has(world, e, Tag));

    ecs_fini(world);
}

void SingleThreadStaging_add_to_world_and_stage_while_readonly() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e = ecs_new_id(world);
    ecs_world_t *stage = ecs_get_stage(world, 0);

    ecs_readonly_begin(world);
    ecs_add(world, e, TagA);
    ecs_add(stage, e, TagB);
    test_assert(!ecs_has(world, e, TagA));
    test_assert(!ecs_has(world, e, TagB));
    ecs_readonly_end(world);

    test_assert(ecs_has(world, e, TagA));
    test_assert(ecs_has(world, e, TagB));

    ecs_fini(world);
}

void SingleThreadStaging_lookup_after_stage_count_change() {
    ecs_world_t *world = ecs_mini();

    ecs_set_stage_count(world, 2);

    /* Make sure we can still lookup entities from flecs.core */
    test_assert(ecs_lookup_fullpath(world, "$") != 0);

    ecs_fini(world);
}

void SingleThreadStaging_lookup_w_scope_after_stage_count_change() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new_entity(world, "parent");
    ecs_entity_t child = ecs_new_entity(world, "child");
    ecs_add_pair(world, child, EcsChildOf, parent);

    test_assert(ecs_lookup_fullpath(world, "parent.child") != 0);
    ecs_set_scope(world, parent);
    test_assert(ecs_lookup_fullpath(world, "parent.child") != 0);
    test_assert(ecs_lookup_fullpath(world, "child") != 0);

    ecs_set_stage_count(world, 2);

    test_assert(ecs_lookup_fullpath(world, "parent.child") != 0);
    test_assert(ecs_lookup_fullpath(world, "child") != 0);

    ecs_fini(world);
}

void SingleThreadStaging_with_after_stage_count_change() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t tag = ecs_new_id(world);
    ecs_set_with(world, tag);

    ecs_entity_t e1 = ecs_new(world, 0);
    test_assert(ecs_has_id(world, e1, tag));

    ecs_set_stage_count(world, 2);

    ecs_entity_t e2 = ecs_new(world, 0);
    test_assert(ecs_has_id(world, e2, tag));

    ecs_fini(world);
}
