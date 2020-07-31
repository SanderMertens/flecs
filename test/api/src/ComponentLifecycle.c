#include <api.h>

static
void install_test_abort() {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.abort = test_abort;
    ecs_os_set_api(&os_api);
    ecs_tracing_enable(-2);
}

typedef struct xtor_ctx {
    ecs_world_t *world;
    ecs_entity_t component;
    ecs_entity_t entity;
    size_t size;
    int32_t count;
    int32_t invoked;
} xtor_ctx;

typedef struct copy_ctx {
    ecs_world_t *world;
    ecs_entity_t component;
    ecs_entity_t entity;
    ecs_entity_t src_entity;
    size_t size;
    int32_t count;
    int32_t invoked;
} copy_ctx;

typedef struct cl_ctx {
    xtor_ctx ctor;
    xtor_ctx dtor;
    copy_ctx copy;
    copy_ctx move;
} cl_ctx;

void comp_ctor(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_entity_t *entity_ptr,
    void *ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    cl_ctx *data = ctx;
    data->ctor.world = world;
    data->ctor.component = component;
    data->ctor.entity = entity_ptr[0],
    data->ctor.size = size;
    data->ctor.count = count;
    data->ctor.invoked ++;
    
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
    cl_ctx *data = ctx;
    data->dtor.world = world;
    data->dtor.component = component;
    data->dtor.entity = entity_ptr[0],
    data->dtor.size = size;
    data->dtor.count = count;
    data->dtor.invoked ++;
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
    cl_ctx *data = ctx;
    data->copy.world = world;
    data->copy.component = component;
    data->copy.entity = dst_entity[0],
    data->copy.src_entity = src_entity[0],
    data->copy.size = size;
    data->copy.count = count;
    data->copy.invoked ++;
    
    memcpy(dst_ptr, src_ptr, size * count);
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
    cl_ctx *data = ctx;
    data->move.world = world;
    data->move.component = component;
    data->move.entity = dst_entity[0],
    data->move.src_entity = src_entity[0],
    data->move.size = size;
    data->move.count = count;
    data->move.invoked ++;
    
    memcpy(dst_ptr, src_ptr, size * count);
}

void ComponentLifecycle_ctor_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.ctor.invoked, 0);

    ecs_add(world, e, Position);
    test_int(ctx.ctor.invoked, 1);
    test_assert(ctx.ctor.world == world);
    test_int(ctx.ctor.component, ecs_entity(Position));
    test_int(ctx.ctor.entity, e);
    test_int(ctx.ctor.size, sizeof(Position));
    test_int(ctx.ctor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_ctor_on_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, Position);
    test_int(ctx.ctor.invoked, 1);
    test_assert(ctx.ctor.world == world);
    test_int(ctx.ctor.component, ecs_entity(Position));
    test_int(ctx.ctor.entity, e);
    test_int(ctx.ctor.size, sizeof(Position));
    test_int(ctx.ctor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_dtor_on_remove() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .dtor = comp_dtor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, Position);
    test_int(ctx.dtor.invoked, 0);

    ecs_remove(world, e, Position);
    test_int(ctx.dtor.invoked, 1);
    test_assert(ctx.dtor.world == world);
    test_int(ctx.dtor.component, ecs_entity(Position));
    test_int(ctx.dtor.entity, e);
    test_int(ctx.dtor.size, sizeof(Position));
    test_int(ctx.dtor.count, 1);
    
    ecs_fini(world);
}

void ComponentLifecycle_dtor_on_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .dtor = comp_dtor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, Position);
    test_int(ctx.dtor.invoked, 0);

    ecs_delete(world, e);
    test_int(ctx.dtor.invoked, 1);
    test_assert(ctx.dtor.world == world);
    test_int(ctx.dtor.component, ecs_entity(Position));
    test_int(ctx.dtor.entity, e);
    test_int(ctx.dtor.size, sizeof(Position));
    test_int(ctx.dtor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.copy.invoked, 0);
    
    ecs_set(world, e, Position, {0, 0});
    test_int(ctx.copy.invoked, 1);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_entity(Position));
    test_int(ctx.copy.entity, e);
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_override() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, Position);
    test_int(ctx.copy.invoked, 0);

    ecs_entity_t base = ecs_new_w_entity(world, ECS_INSTANCEOF | e);
    test_int(ctx.copy.invoked, 0);

    ecs_add(world, base, Position);
    test_int(ctx.copy.invoked, 1);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_entity(Position));
    test_int(ctx.copy.entity, e);
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_new_w_data() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_bulk_new_w_data(world, 2, 
        &(ecs_entities_t){
            .array = (ecs_entity_t[]){
                ecs_entity(Position)
            }, 
            .count = 1
        },
        (void*[]){
            (Position[]){
                {10, 20},
                {30, 40}
            }
        });

    test_int(ctx.copy.invoked, 1);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_entity(Position));
    test_int(ctx.copy.entity, e);
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 2);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_clone() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_int(ctx.copy.invoked, 1);
    memset(&ctx, 0, sizeof(ctx));

    ecs_entity_t clone = ecs_clone(world, 0, e, true);
    test_int(ctx.copy.invoked, 1);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_entity(Position));
    test_int(ctx.copy.entity, clone);
    test_int(ctx.copy.src_entity, e);
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_no_copy_on_move() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_int(ctx.copy.invoked, 1);
    memset(&ctx, 0, sizeof(ctx));

    ecs_add(world, e, Velocity);
    test_int(ctx.copy.invoked, 0);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_stage() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_int(ctx.copy.invoked, 1);
    memset(&ctx, 0, sizeof(ctx));

    ecs_staging_begin(world);

    ecs_add(world, e, Velocity);
    test_int(ctx.copy.invoked, 1);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_entity(Position));
    test_int(ctx.copy.entity, e);
    test_int(ctx.copy.src_entity, e);
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 1);

    ecs_staging_end(world, false);

    ecs_fini(world);
}

void ComponentLifecycle_move_on_merge() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .move = comp_move,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_int(ctx.move.invoked, 0);

    ecs_staging_begin(world);

    ecs_add(world, e, Velocity);
    test_int(ctx.ctor.invoked, 1);
    test_int(ctx.move.invoked, 0);

    ecs_staging_end(world, false);
    test_int(ctx.ctor.invoked, 2);
    test_int(ctx.move.invoked, 1);
    test_assert(ctx.move.world == world);
    test_int(ctx.move.component, ecs_entity(Position));
    test_int(ctx.move.entity, e);
    test_int(ctx.move.src_entity, e);
    test_int(ctx.move.size, sizeof(Position));
    test_int(ctx.move.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_ctor_on_bulk_add() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_bulk_new(world, Position, 10);
    test_assert(e != 0);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Velocity), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    test_int(ctx.ctor.invoked, 0);

    ecs_bulk_add(world, Velocity, &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    test_int(ctx.ctor.invoked, 1);
    test_assert(ctx.ctor.world == world);
    test_int(ctx.ctor.component, ecs_entity(Velocity));
    test_int(ctx.ctor.entity, e);
    test_int(ctx.ctor.size, sizeof(Velocity));
    test_int(ctx.ctor.count, 10);

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( ecs_has(world, e + i, Velocity));

        const Position *p = ecs_get(world, e + i, Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2);

        const Velocity *v = ecs_get(world, e + i, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 10);
        test_int(v->y, 20);        
    }    

    ecs_fini(world);
}

void ComponentLifecycle_dtor_on_bulk_remove() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t e = ecs_bulk_new(world, Type, 10);
    test_assert(e != 0);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Velocity), EcsComponentLifecycle, {
        .dtor = comp_dtor,
        .ctx = &ctx
    });

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( ecs_has(world, e + i, Velocity));
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    test_int(ctx.dtor.invoked, 0);

    ecs_bulk_remove(world, Velocity, &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    test_int(ctx.dtor.invoked, 1);
    test_assert(ctx.dtor.world == world);
    test_int(ctx.dtor.component, ecs_entity(Velocity));
    test_int(ctx.dtor.entity, e);
    test_int(ctx.dtor.size, sizeof(Velocity));
    test_int(ctx.dtor.count, 10);

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( !ecs_has(world, e + i, Velocity));

        const Position *p = ecs_get(world, e + i, Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2);      
    } 

    ecs_fini(world);
}

void ComponentLifecycle_ctor_on_bulk_add_entity() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_bulk_new(world, Position, 10);
    test_assert(e != 0);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Velocity), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    test_int(ctx.ctor.invoked, 0);

    ecs_bulk_add_entity(world, ecs_entity(Velocity), &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    test_int(ctx.ctor.invoked, 1);
    test_assert(ctx.ctor.world == world);
    test_int(ctx.ctor.component, ecs_entity(Velocity));
    test_int(ctx.ctor.entity, e);
    test_int(ctx.ctor.size, sizeof(Velocity));
    test_int(ctx.ctor.count, 10);

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( ecs_has(world, e + i, Velocity));

        const Position *p = ecs_get(world, e + i, Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2);

        const Velocity *v = ecs_get(world, e + i, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 10);
        test_int(v->y, 20);        
    }  

    ecs_fini(world);
}

void ComponentLifecycle_dtor_on_bulk_remove_entity() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t e = ecs_bulk_new(world, Type, 10);
    test_assert(e != 0);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Velocity), EcsComponentLifecycle, {
        .dtor = comp_dtor,
        .ctx = &ctx
    });

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( ecs_has(world, e + i, Velocity));
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    test_int(ctx.dtor.invoked, 0);

    ecs_bulk_remove_entity(world, ecs_entity(Velocity), &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    test_int(ctx.dtor.invoked, 1);
    test_assert(ctx.dtor.world == world);
    test_int(ctx.dtor.component, ecs_entity(Velocity));
    test_int(ctx.dtor.entity, e);
    test_int(ctx.dtor.size, sizeof(Velocity));
    test_int(ctx.dtor.count, 10);

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( !ecs_has(world, e + i, Velocity));

        const Position *p = ecs_get(world, e + i, Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2);
    } 

    ecs_fini(world);
}

void ComponentLifecycle_ctor_dtor_on_bulk_add_remove() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type, Position, Mass);

    ecs_entity_t e = ecs_bulk_new(world, Type, 10);
    test_assert(e != 0);

    cl_ctx ctx_ctor = { { 0 } };
    cl_ctx ctx_dtor = { { 0 } };

    ecs_set(world, ecs_entity(Velocity), EcsComponentLifecycle, {
        .ctor = comp_ctor,
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

    test_int(ctx_ctor.ctor.invoked, 0);
    test_int(ctx_dtor.dtor.invoked, 0);

    ecs_bulk_add_remove(world, Velocity, Mass, &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    test_int(ctx_ctor.ctor.invoked, 1);
    test_assert(ctx_ctor.ctor.world == world);
    test_int(ctx_ctor.ctor.component, ecs_entity(Velocity));
    test_int(ctx_ctor.ctor.entity, e);
    test_int(ctx_ctor.ctor.size, sizeof(Velocity));
    test_int(ctx_ctor.ctor.count, 10);

    test_int(ctx_dtor.dtor.invoked, 1);
    test_assert(ctx_dtor.dtor.world == world);
    test_int(ctx_dtor.dtor.component, ecs_entity(Mass));
    test_int(ctx_dtor.dtor.entity, e);
    test_int(ctx_dtor.dtor.size, sizeof(Mass));
    test_int(ctx_dtor.dtor.count, 10);    

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( ecs_has(world, e + i, Velocity));
        test_assert( !ecs_has(world, e + i, Mass));

        const Position *p = ecs_get(world, e + i, Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2); 

        const Velocity *v = ecs_get(world, e + i, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 10);
        test_int(v->y, 20);
    } 

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_snapshot() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    cl_ctx ctx = { { 0 } };
    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_bulk_new(world, Position, 10);
    test_assert(e != 0);

    int32_t i;
    for (i = 0; i < 10; i ++) {
        test_assert(ecs_has(world, e + i, Position));
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    test_int(ctx.copy.invoked, 10);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_entity(Position));
    test_int(ctx.copy.entity, e + i - 1);
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 1);

    ctx = (cl_ctx){ { 0 } };

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    test_int(ctx.copy.invoked, 1);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_entity(Position));
    test_int(ctx.copy.entity, e);
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 10);

    ecs_snapshot_restore(world, s);

    for (i = 0; i < 10; i ++) {
        test_assert(ecs_has(world, e + i, Position));
        const Position *p = ecs_get(world, e + i, Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2);
    }   

    ecs_fini(world);
}

void ComponentLifecycle_ctor_copy_on_snapshot() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    cl_ctx ctx = { { 0 } };
    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_bulk_new(world, Position, 10);
    test_assert(e != 0);

    int32_t i;
    for (i = 0; i < 10; i ++) {
        test_assert(ecs_has(world, e + i, Position));
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    test_int(ctx.copy.invoked, 10);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_entity(Position));
    test_int(ctx.copy.entity, e + i - 1);
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 1);

    ctx = (cl_ctx){ { 0 } };

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    test_int(ctx.ctor.invoked, 1);
    test_assert(ctx.ctor.world == world);
    test_int(ctx.ctor.component, ecs_entity(Position));
    test_int(ctx.ctor.entity, e);
    test_int(ctx.ctor.size, sizeof(Position));
    test_int(ctx.ctor.count, 10);

    test_int(ctx.copy.invoked, 1);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_entity(Position));
    test_int(ctx.copy.entity, e);
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 10);

    ecs_snapshot_restore(world, s);

    for (i = 0; i < 10; i ++) {
        test_assert(ecs_has(world, e + i, Position));
        const Position *p = ecs_get(world, e + i, Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2);
    }   

    ecs_fini(world);
}

void ComponentLifecycle_dtor_on_restore() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    cl_ctx ctx = { { 0 } };
    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .dtor = comp_dtor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_bulk_new(world, Position, 10);
    test_assert(e != 0);

    int32_t i;
    for (i = 0; i < 10; i ++) {
        test_assert(ecs_has(world, e + i, Position));
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    test_int(ctx.dtor.invoked, 0);

    ctx = (cl_ctx){ { 0 } };

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    test_int(ctx.dtor.invoked, 0);

    /* Delete one entity, so we have more confidence we're destructing the right
     * entities */
    ecs_delete(world, e);
    
    test_int(ctx.dtor.invoked, 1);
    ctx = (cl_ctx){ { 0 } };

    ecs_snapshot_restore(world, s);

    test_int(ctx.dtor.invoked, 1);
    test_assert(ctx.dtor.world == world);
    test_int(ctx.dtor.component, ecs_entity(Position));
    test_int(ctx.dtor.entity, e + 9);
    test_int(ctx.dtor.size, sizeof(Position));
    test_int(ctx.dtor.count, 9);

    for (i = 0; i < 10; i ++) {
        test_assert(ecs_has(world, e + i, Position));
        const Position *p = ecs_get(world, e + i, Position);
        test_assert(p != NULL);

        test_int(p->x, i);
        test_int(p->y, i * 2);
    }   

    ecs_fini(world);
}

void ComponentLifecycle_ctor_on_tag() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    test_expect_abort();

    ecs_set(world, Tag, EcsComponentLifecycle, {
        .ctor = comp_ctor
    });

    ecs_fini(world);
}

void ComponentLifecycle_dtor_on_tag() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    test_expect_abort();

    ecs_set(world, Tag, EcsComponentLifecycle, {
        .dtor = comp_dtor
    });

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_tag() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    test_expect_abort();

    ecs_set(world, Tag, EcsComponentLifecycle, {
        .copy = comp_copy
    });

    ecs_fini(world);
}

void ComponentLifecycle_move_on_tag() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    test_expect_abort();

    ecs_set(world, Tag, EcsComponentLifecycle, {
        .move = comp_move
    });

    ecs_fini(world);
}
