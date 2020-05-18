#include <api.h>

typedef struct xtor_ctx {
    ecs_world_t *world;
    ecs_entity_t component;
    ecs_entity_t entity;
    ecs_entity_t src_entity;
    size_t size;
    int32_t count;
    int32_t invoked;
} xtor_ctx;

void comp_xtor(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_entity_t *entity_ptr,
    void *ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    xtor_ctx *data = ctx;
    data->world = world;
    data->component = component;
    data->entity = entity_ptr[0],
    data->size = size;
    data->count = count;
    data->invoked ++;
}

void comp_copy(
    ecs_world_t *world,
    ecs_entity_t component,    
    const ecs_entity_t *dst_entity,
    const ecs_entity_t *src_entity,
    void *dst_ptr,
    const void *src_ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    xtor_ctx *data = ctx;
    data->world = world;
    data->component = component;
    data->entity = dst_entity[0],
    data->src_entity = src_entity[0],
    data->size = size;
    data->count = count;
    data->invoked ++;
}

void ComponentLifecycle_ctor_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    xtor_ctx ctx = { 0 };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .ctor = comp_xtor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.invoked, 0);

    ecs_add(world, e, Position);
    test_int(ctx.invoked, 1);
    test_assert(ctx.world == world);
    test_int(ctx.component, ecs_entity(Position));
    test_int(ctx.entity, e);
    test_int(ctx.size, sizeof(Position));
    test_int(ctx.count, 1);
}

void ComponentLifecycle_ctor_on_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    xtor_ctx ctx = { 0 };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .ctor = comp_xtor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, Position);
    test_int(ctx.invoked, 1);
    test_assert(ctx.world == world);
    test_int(ctx.component, ecs_entity(Position));
    test_int(ctx.entity, e);
    test_int(ctx.size, sizeof(Position));
    test_int(ctx.count, 1);
}

void ComponentLifecycle_dtor_on_remove() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    xtor_ctx ctx = { 0 };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .dtor = comp_xtor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, Position);
    test_int(ctx.invoked, 0);

    ecs_remove(world, e, Position);
    test_int(ctx.invoked, 1);
    test_assert(ctx.world == world);
    test_int(ctx.component, ecs_entity(Position));
    test_int(ctx.entity, e);
    test_int(ctx.size, sizeof(Position));
    test_int(ctx.count, 1);
}

void ComponentLifecycle_dtor_on_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    xtor_ctx ctx = { 0 };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .dtor = comp_xtor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, Position);
    test_int(ctx.invoked, 0);

    ecs_delete(world, e);
    test_int(ctx.invoked, 1);
    test_assert(ctx.world == world);
    test_int(ctx.component, ecs_entity(Position));
    test_int(ctx.entity, e);
    test_int(ctx.size, sizeof(Position));
    test_int(ctx.count, 1);
}

void ComponentLifecycle_copy_on_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    xtor_ctx ctx = { 0 };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.invoked, 0);
    
    ecs_set(world, e, Position, {0, 0});
    test_int(ctx.invoked, 1);
    test_assert(ctx.world == world);
    test_int(ctx.component, ecs_entity(Position));
    test_int(ctx.entity, e);
    test_int(ctx.size, sizeof(Position));
    test_int(ctx.count, 1);
}

void ComponentLifecycle_copy_on_override() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    xtor_ctx ctx = { 0 };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, Position);
    test_int(ctx.invoked, 0);

    ecs_entity_t base = ecs_new_w_entity(world, ECS_INSTANCEOF | e);
    test_int(ctx.invoked, 0);

    ecs_add(world, base, Position);
    test_int(ctx.invoked, 1);
    test_assert(ctx.world == world);
    test_int(ctx.component, ecs_entity(Position));
    test_int(ctx.entity, e);
    test_int(ctx.size, sizeof(Position));
    test_int(ctx.count, 1);
}

void ComponentLifecycle_copy_on_new_w_data() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    xtor_ctx ctx = { 0 };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_bulk_new_w_type(world, ecs_type(Position), 2, (void*[]){
        (Position[]){
            {10, 20},
            {30, 40}
        }
    });

    test_int(ctx.invoked, 1);
    test_assert(ctx.world == world);
    test_int(ctx.component, ecs_entity(Position));
    test_int(ctx.entity, e);
    test_int(ctx.size, sizeof(Position));
    test_int(ctx.count, 2);
}

void ComponentLifecycle_copy_on_clone() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    xtor_ctx ctx = { 0 };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_int(ctx.invoked, 1);
    memset(&ctx, 0, sizeof(ctx));

    ecs_entity_t clone = ecs_clone(world, 0, e, true);
    test_int(ctx.invoked, 1);
    test_assert(ctx.world == world);
    test_int(ctx.component, ecs_entity(Position));
    test_int(ctx.entity, clone);
    test_int(ctx.src_entity, e);
    test_int(ctx.size, sizeof(Position));
    test_int(ctx.count, 1);
}

void ComponentLifecycle_copy_on_stage() {
    // Implement testcase
}

void ComponentLifecycle_move_on_merge() {
    // Implement testcase
}
