#include <api.h>

void Deinit(ecs_iter_t *it) {
    ECS_COLUMN(it, Position, p, 1);

    Velocity *v = NULL;
    if (it->column_count >= 2) {
        v = ecs_column(it, Velocity, 2);
    }

    probe_system(it);

    /* Write to validate columns point to valid memory */
    int i;
    for (i = 0; i < it->count; i ++) {
        p[i].x = 0;
        p[i].y = 0;

        if (v) {
            v[i].x = 0;
            v[i].y = 0;
        }
    }
}

static
void Remove_from_current(ecs_iter_t *it) {
    IterData *ctx = ecs_get_context(it->world);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];

        if (ctx->component) {
            ecs_remove_entity(it->world, e, ctx->component);
        }

        if (ctx->component_2) {
            ecs_remove_entity(it->world, e, ctx->component_2);
        }

        ctx->entity_count ++;
    }
}

void TriggerOnRemove_remove_match_1_of_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TRIGGER(world, Deinit, EcsOnRemove, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_remove(world, e, Position);

    test_int(ctx.count, 1);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}

void TriggerOnRemove_remove_match_1_of_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_TRIGGER(world, Deinit, EcsOnRemove, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_remove(world, e, Type);

    test_int(ctx.count, 1);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}

void TriggerOnRemove_remove_no_match_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TRIGGER(world, Deinit, EcsOnRemove, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_remove(world, e, Velocity);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void TriggerOnRemove_delete_match_1_of_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TRIGGER(world, Deinit, EcsOnRemove, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_delete(world, e);

    test_int(ctx.count, 1);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}

void TriggerOnRemove_delete_match_1_of_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_TRIGGER(world, Deinit, EcsOnRemove, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_delete(world, e);

    test_int(ctx.count, 1);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}

void TriggerOnRemove_delete_no_match_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TRIGGER(world, Deinit, EcsOnRemove, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Velocity);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_delete(world, e);

    test_int(ctx.count, 0); 

    ecs_fini(world);
}

static Position old_position = {0};

static
void RemovePosition(ecs_iter_t *it) {
    ECS_COLUMN(it, Position, p, 1);

    test_assert(it->count == 1);

    old_position = p[0];
}

void TriggerOnRemove_remove_watched() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_TRIGGER(world, RemovePosition, EcsOnRemove, Position);

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);

    /* Make parent entity watched */
    ecs_new_w_entity(world, ECS_CHILDOF | e);

    ecs_remove(world, e, Position);

    test_int(old_position.x, 10);
    test_int(old_position.y, 20);

    ecs_fini(world);
}


void TriggerOnRemove_delete_watched() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_TRIGGER(world, RemovePosition, EcsOnRemove, Position);

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);

    /* Make parent entity watched */
    ecs_new_w_entity(world, ECS_CHILDOF | e);

    ecs_delete(world, e);

    test_int(old_position.x, 10);
    test_int(old_position.y, 20);

    ecs_fini(world);
}

static bool dummy_called = false;

static
void Dummy(ecs_iter_t *it) {
    dummy_called = true;
}

void TriggerOnRemove_on_remove_in_on_update() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, Remove_from_current, EcsOnUpdate, Position);
    ECS_TRIGGER(world, Dummy, EcsOnRemove, Velocity);

    IterData ctx = {.component = ecs_entity(Velocity)};
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