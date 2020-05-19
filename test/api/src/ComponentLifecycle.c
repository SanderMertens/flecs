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
    
    Position *p = ptr;
    int i;
    for (i = 0; i < count; i ++) {
        p[i].x = 10;
        p[i].y = 20;
    }
}

void comp_dtor(
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

void comp_move(
    ecs_world_t *world,
    ecs_entity_t component,    
    const ecs_entity_t *dst_entity,
    const ecs_entity_t *src_entity,
    void *dst_ptr,
    void *src_ptr,
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

void ComponentLifecycle_no_copy_on_move() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    xtor_ctx ctx = { 0 };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_int(ctx.invoked, 1);
    memset(&ctx, 0, sizeof(ctx));

    ecs_add(world, e, Velocity);
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_stage() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    xtor_ctx ctx = { 0 };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_int(ctx.invoked, 1);
    memset(&ctx, 0, sizeof(ctx));

    ecs_staging_begin(world);

    ecs_add(world, e, Velocity);
    test_int(ctx.invoked, 1);
    test_assert(ctx.world == world);
    test_int(ctx.component, ecs_entity(Position));
    test_int(ctx.entity, e);
    test_int(ctx.src_entity, e);
    test_int(ctx.size, sizeof(Position));
    test_int(ctx.count, 1);

    ecs_staging_end(world, false);

    ecs_fini(world);
}

void ComponentLifecycle_move_on_merge() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    xtor_ctx ctx = { 0 };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .move = comp_move,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_int(ctx.invoked, 0);

    ecs_staging_begin(world);

    ecs_add(world, e, Velocity);
    test_int(ctx.invoked, 0);

    ecs_staging_end(world, false);
    test_int(ctx.invoked, 1);
    test_assert(ctx.world == world);
    test_int(ctx.component, ecs_entity(Position));
    test_int(ctx.entity, e);
    test_int(ctx.src_entity, e);
    test_int(ctx.size, sizeof(Position));
    test_int(ctx.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_ctor_on_bulk_add() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_bulk_new(world, Position, 10);
    test_assert(e != 0);

    xtor_ctx ctx = { 0 };

    ecs_set(world, ecs_entity(Velocity), EcsComponentLifecycle, {
        .ctor = comp_xtor,
        .ctx = &ctx
    });

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    test_int(ctx.invoked, 0);

    ecs_bulk_add(world, Velocity, &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    test_int(ctx.invoked, 1);
    test_assert(ctx.world == world);
    test_int(ctx.component, ecs_entity(Velocity));
    test_int(ctx.entity, e);
    test_int(ctx.size, sizeof(Velocity));
    test_int(ctx.count, 10);

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( ecs_has(world, e + i, Velocity));

        const Position *p = ecs_get_ptr(world, e + i, Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2);

        const Velocity *v = ecs_get_ptr(world, e + i, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 10);
        test_int(v->y, 20);        
    }    
}

void ComponentLifecycle_dtor_on_bulk_remove() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t e = ecs_bulk_new(world, Type, 10);
    test_assert(e != 0);

    xtor_ctx ctx = { 0 };

    ecs_set(world, ecs_entity(Velocity), EcsComponentLifecycle, {
        .dtor = comp_xtor,
        .ctx = &ctx
    });

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( ecs_has(world, e + i, Velocity));
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    test_int(ctx.invoked, 0);

    ecs_bulk_remove(world, Velocity, &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    test_int(ctx.invoked, 1);
    test_assert(ctx.world == world);
    test_int(ctx.component, ecs_entity(Velocity));
    test_int(ctx.entity, e);
    test_int(ctx.size, sizeof(Velocity));
    test_int(ctx.count, 10);

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( !ecs_has(world, e + i, Velocity));

        const Position *p = ecs_get_ptr(world, e + i, Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2);      
    } 
}

void ComponentLifecycle_ctor_on_bulk_add_entity() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_bulk_new(world, Position, 10);
    test_assert(e != 0);

    xtor_ctx ctx = { 0 };

    ecs_set(world, ecs_entity(Velocity), EcsComponentLifecycle, {
        .ctor = comp_xtor,
        .ctx = &ctx
    });

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    test_int(ctx.invoked, 0);

    ecs_bulk_add_entity(world, ecs_entity(Velocity), &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    test_int(ctx.invoked, 1);
    test_assert(ctx.world == world);
    test_int(ctx.component, ecs_entity(Velocity));
    test_int(ctx.entity, e);
    test_int(ctx.size, sizeof(Velocity));
    test_int(ctx.count, 10);

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( ecs_has(world, e + i, Velocity));

        const Position *p = ecs_get_ptr(world, e + i, Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2);

        const Velocity *v = ecs_get_ptr(world, e + i, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 10);
        test_int(v->y, 20);        
    }  
}

void ComponentLifecycle_dtor_on_bulk_remove_entity() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t e = ecs_bulk_new(world, Type, 10);
    test_assert(e != 0);

    xtor_ctx ctx = { 0 };

    ecs_set(world, ecs_entity(Velocity), EcsComponentLifecycle, {
        .dtor = comp_xtor,
        .ctx = &ctx
    });

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( ecs_has(world, e + i, Velocity));
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    test_int(ctx.invoked, 0);

    ecs_bulk_remove_entity(world, ecs_entity(Velocity), &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    test_int(ctx.invoked, 1);
    test_assert(ctx.world == world);
    test_int(ctx.component, ecs_entity(Velocity));
    test_int(ctx.entity, e);
    test_int(ctx.size, sizeof(Velocity));
    test_int(ctx.count, 10);

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( !ecs_has(world, e + i, Velocity));

        const Position *p = ecs_get_ptr(world, e + i, Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2);      
    } 
}

void ComponentLifecycle_ctor_dtor_on_bulk_add_remove() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type, Position, Mass);

    ecs_entity_t e = ecs_bulk_new(world, Type, 10);
    test_assert(e != 0);

    xtor_ctx ctx_ctor = { 0 };
    xtor_ctx ctx_dtor = { 0 };

    ecs_set(world, ecs_entity(Velocity), EcsComponentLifecycle, {
        .ctor = comp_xtor,
        .ctx = &ctx_ctor
    });

    ecs_set(world, ecs_entity(Mass), EcsComponentLifecycle, {
        .dtor = comp_dtor,
        .ctx = &ctx_dtor
    });

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( ecs_has(world, e + i, Mass));
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    test_int(ctx_ctor.invoked, 0);
    test_int(ctx_dtor.invoked, 0);

    ecs_bulk_add_remove(world, Velocity, Mass, &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    test_int(ctx_ctor.invoked, 1);
    test_assert(ctx_ctor.world == world);
    test_int(ctx_ctor.component, ecs_entity(Velocity));
    test_int(ctx_ctor.entity, e);
    test_int(ctx_ctor.size, sizeof(Velocity));
    test_int(ctx_ctor.count, 10);

    test_int(ctx_dtor.invoked, 1);
    test_assert(ctx_dtor.world == world);
    test_int(ctx_dtor.component, ecs_entity(Mass));
    test_int(ctx_dtor.entity, e);
    test_int(ctx_dtor.size, sizeof(Mass));
    test_int(ctx_dtor.count, 10);    

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( ecs_has(world, e + i, Velocity));
        test_assert( !ecs_has(world, e + i, Mass));

        const Position *p = ecs_get_ptr(world, e + i, Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2); 

        const Velocity *v = ecs_get_ptr(world, e + i, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 10);
        test_int(v->y, 20);
    } 
}
