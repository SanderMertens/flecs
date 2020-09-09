#include <api.h>

static
void install_test_abort() {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.abort_ = test_abort;
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

static
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

static
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

static
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

static
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

    ecs_entity_t base = ecs_new(world, Position);
    test_int(ctx.copy.invoked, 0);

    ecs_entity_t e = ecs_new_w_entity(world, ECS_INSTANCEOF | base);
    test_int(ctx.copy.invoked, 0);

    ecs_add(world, e, Position);
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

    const ecs_entity_t *ids = ecs_bulk_new_w_data(world, 2, 
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
    test_int(ctx.copy.entity, ids[0]);
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

void ComponentLifecycle_ctor_on_bulk_add() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    const ecs_entity_t *ids = ecs_bulk_new(world, Position, 10);
    test_assert(ids != NULL);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Velocity), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, ids[i], Position));
        ecs_set(world, ids[i], Position, {i, i * 2});
    }

    test_int(ctx.ctor.invoked, 0);

    ecs_bulk_add(world, Velocity, &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    test_int(ctx.ctor.invoked, 1);
    test_assert(ctx.ctor.world == world);
    test_int(ctx.ctor.component, ecs_entity(Velocity));
    test_int(ctx.ctor.entity, ids[0]);
    test_int(ctx.ctor.size, sizeof(Velocity));
    test_int(ctx.ctor.count, 10);

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, ids[i], Position));
        test_assert( ecs_has(world, ids[i], Velocity));

        const Position *p = ecs_get(world, ids[i], Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2);

        const Velocity *v = ecs_get(world, ids[i], Velocity);
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

    const ecs_entity_t *ids = ecs_bulk_new(world, Type, 10);
    test_assert(ids != NULL);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Velocity), EcsComponentLifecycle, {
        .dtor = comp_dtor,
        .ctx = &ctx
    });

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, ids[i], Position));
        test_assert( ecs_has(world, ids[i], Velocity));
        ecs_set(world, ids[i], Position, {i, i * 2});
    }

    test_int(ctx.dtor.invoked, 0);

    ecs_bulk_remove(world, Velocity, &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    test_int(ctx.dtor.invoked, 1);
    test_assert(ctx.dtor.world == world);
    test_int(ctx.dtor.component, ecs_entity(Velocity));
    test_int(ctx.dtor.entity, ids[0]);
    test_int(ctx.dtor.size, sizeof(Velocity));
    test_int(ctx.dtor.count, 10);

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, ids[i], Position));
        test_assert( !ecs_has(world, ids[i], Velocity));

        const Position *p = ecs_get(world, ids[i], Position);
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

    const ecs_entity_t *ids = ecs_bulk_new(world, Position, 10);
    test_assert(ids != NULL);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Velocity), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, ids[i], Position));
        ecs_set(world, ids[i], Position, {i, i * 2});
    }

    test_int(ctx.ctor.invoked, 0);

    ecs_bulk_add_entity(world, ecs_entity(Velocity), &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    test_int(ctx.ctor.invoked, 1);
    test_assert(ctx.ctor.world == world);
    test_int(ctx.ctor.component, ecs_entity(Velocity));
    test_int(ctx.ctor.entity, ids[0]);
    test_int(ctx.ctor.size, sizeof(Velocity));
    test_int(ctx.ctor.count, 10);

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, ids[i], Position));
        test_assert( ecs_has(world, ids[i], Velocity));

        const Position *p = ecs_get(world, ids[i], Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2);

        const Velocity *v = ecs_get(world, ids[i], Velocity);
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

    const ecs_entity_t *ids = ecs_bulk_new(world, Type, 10);
    test_assert(ids != NULL);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Velocity), EcsComponentLifecycle, {
        .dtor = comp_dtor,
        .ctx = &ctx
    });

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, ids[i], Position));
        test_assert( ecs_has(world, ids[i], Velocity));
        ecs_set(world, ids[i], Position, {i, i * 2});
    }

    test_int(ctx.dtor.invoked, 0);

    ecs_bulk_remove_entity(world, ecs_entity(Velocity), &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    test_int(ctx.dtor.invoked, 1);
    test_assert(ctx.dtor.world == world);
    test_int(ctx.dtor.component, ecs_entity(Velocity));
    test_int(ctx.dtor.entity, ids[0]);
    test_int(ctx.dtor.size, sizeof(Velocity));
    test_int(ctx.dtor.count, 10);

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, ids[i], Position));
        test_assert( !ecs_has(world, ids[i], Velocity));

        const Position *p = ecs_get(world, ids[i], Position);
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

    const ecs_entity_t *ids = ecs_bulk_new(world, Type, 10);
    test_assert(ids != NULL);

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
        test_assert( ecs_has(world, ids[i], Position));
        test_assert( ecs_has(world, ids[i], Mass));
        ecs_set(world, ids[i], Position, {i, i * 2});
    }

    test_int(ctx_ctor.ctor.invoked, 0);
    test_int(ctx_dtor.dtor.invoked, 0);

    ecs_bulk_add_remove(world, Velocity, Mass, &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    test_int(ctx_ctor.ctor.invoked, 1);
    test_assert(ctx_ctor.ctor.world == world);
    test_int(ctx_ctor.ctor.component, ecs_entity(Velocity));
    test_int(ctx_ctor.ctor.entity, ids[0]);
    test_int(ctx_ctor.ctor.size, sizeof(Velocity));
    test_int(ctx_ctor.ctor.count, 10);

    test_int(ctx_dtor.dtor.invoked, 1);
    test_assert(ctx_dtor.dtor.world == world);
    test_int(ctx_dtor.dtor.component, ecs_entity(Mass));
    test_int(ctx_dtor.dtor.entity, ids[0]);
    test_int(ctx_dtor.dtor.size, sizeof(Mass));
    test_int(ctx_dtor.dtor.count, 10);    

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, ids[i], Position));
        test_assert( ecs_has(world, ids[i], Velocity));
        test_assert( !ecs_has(world, ids[i], Mass));

        const Position *p = ecs_get(world, ids[i], Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2); 

        const Velocity *v = ecs_get(world, ids[i], Velocity);
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

    const ecs_entity_t *ids = ecs_bulk_new(world, Position, 10);
    test_assert(ids != NULL);

    int32_t i;
    for (i = 0; i < 10; i ++) {
        test_assert(ecs_has(world, ids[i], Position));
        ecs_set(world, ids[i], Position, {i, i * 2});
    }

    test_int(ctx.copy.invoked, 10);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_entity(Position));
    test_int(ctx.copy.entity, ids[i - 1]);
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 1);

    ctx = (cl_ctx){ { 0 } };

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    test_int(ctx.copy.invoked, 1);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_entity(Position));
    test_int(ctx.copy.entity, ids[0]);
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 10);

    ecs_snapshot_restore(world, s);

    for (i = 0; i < 10; i ++) {
        test_assert(ecs_has(world, ids[i], Position));
        const Position *p = ecs_get(world, ids[i], Position);
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

    const ecs_entity_t *ids = ecs_bulk_new(world, Position, 10);
    test_assert(ids != NULL);

    int32_t i;
    for (i = 0; i < 10; i ++) {
        test_assert(ecs_has(world, ids[i], Position));
        ecs_set(world, ids[i], Position, {i, i * 2});
    }

    test_int(ctx.copy.invoked, 10);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_entity(Position));
    test_int(ctx.copy.entity, ids[i - 1]);
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 1);

    ctx = (cl_ctx){ { 0 } };

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    test_int(ctx.ctor.invoked, 1);
    test_assert(ctx.ctor.world == world);
    test_int(ctx.ctor.component, ecs_entity(Position));
    test_int(ctx.ctor.entity, ids[0]);
    test_int(ctx.ctor.size, sizeof(Position));
    test_int(ctx.ctor.count, 10);

    test_int(ctx.copy.invoked, 1);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_entity(Position));
    test_int(ctx.copy.entity, ids[0]);
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 10);

    ecs_snapshot_restore(world, s);

    for (i = 0; i < 10; i ++) {
        test_assert(ecs_has(world, ids[i], Position));
        const Position *p = ecs_get(world, ids[i], Position);
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

    const ecs_entity_t *temp_ids = ecs_bulk_new(world, Position, 10);
    test_assert(temp_ids != NULL);
    ecs_entity_t ids[10];
    memcpy(ids, temp_ids, sizeof(ecs_entity_t) * 10);

    int32_t i;
    for (i = 0; i < 10; i ++) {
        test_assert(ecs_has(world, ids[i], Position));
        ecs_set(world, ids[i], Position, {i, i * 2});
    }

    test_int(ctx.dtor.invoked, 0);

    ctx = (cl_ctx){ { 0 } };

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    test_int(ctx.dtor.invoked, 0);

    /* Delete one entity, so we have more confidence we're destructing the right
     * entities */
    ecs_delete(world, ids[0]);
    
    test_int(ctx.dtor.invoked, 1);
    ctx = (cl_ctx){ { 0 } };

    ecs_snapshot_restore(world, s);

    test_int(ctx.dtor.invoked, 1);
    test_assert(ctx.dtor.world == world);
    test_int(ctx.dtor.component, ecs_entity(Position));
    test_int(ctx.dtor.entity, ids[9]);
    test_int(ctx.dtor.size, sizeof(Position));
    test_int(ctx.dtor.count, 9);

    for (i = 0; i < 10; i ++) {
        test_assert(ecs_has(world, ids[i], Position));
        const Position *p = ecs_get(world, ids[i], Position);
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

/* Position */

static int ctor_position = 0;
static
ECS_CTOR(Position, ptr, {
    ctor_position ++;
});

static int dtor_position = 0;
static
ECS_DTOR(Position, ptr, {
    dtor_position ++;
});

static int copy_position = 0;
static
ECS_COPY(Position, dst, src, {
    copy_position ++;
});

static int move_position = 0;
static
ECS_MOVE(Position, dst, src, {
    move_position ++;
});

/* Velocity */

static int ctor_velocity = 0;
static
ECS_CTOR(Velocity, ptr, {
    ctor_velocity ++;
});

static int dtor_velocity = 0;
static
ECS_DTOR(Velocity, ptr, {
    dtor_velocity ++;
});

static int copy_velocity = 0;
static
ECS_COPY(Velocity, dst, src, {
    copy_velocity ++;
});

static int move_velocity = 0;
static
ECS_MOVE(Velocity, dst, src, {
    move_velocity ++;
});

/* Mass */

static int ctor_mass = 0;
static
ECS_CTOR(Mass, ptr, {
    ctor_mass ++;
});

static int dtor_mass = 0;
static
ECS_DTOR(Mass, ptr, {
    dtor_mass ++;
});

static int copy_mass = 0;
static
ECS_COPY(Mass, dst, src, {
    copy_mass ++;
});

static int move_mass = 0;
static
ECS_MOVE(Mass, dst, src, {
    move_mass ++;
});

/* Rotation */

static int ctor_rotation = 0;
static
ECS_CTOR(Rotation, ptr, {
    ctor_rotation ++;
});

static int dtor_rotation = 0;
static
ECS_DTOR(Rotation, ptr, {
    dtor_rotation ++;
});

static int copy_rotation = 0;
static
ECS_COPY(Rotation, dst, src, {
    copy_rotation ++;
});

static int move_rotation = 0;
static
ECS_MOVE(Rotation, dst, src, {
    move_rotation ++;
});

void ComponentLifecycle_merge_to_different_table() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e, Position, Velocity, Rotation);

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .ctor = ecs_ctor(Position),
        .dtor = ecs_dtor(Position),
        .copy = ecs_copy(Position),
        .move = ecs_move(Position)
    });

    ecs_set(world, ecs_entity(Velocity), EcsComponentLifecycle, {
        .ctor = ecs_ctor(Velocity),
        .dtor = ecs_dtor(Velocity),
        .copy = ecs_copy(Velocity),
        .move = ecs_move(Velocity)
    });   

    ecs_set(world, ecs_entity(Mass), EcsComponentLifecycle, {
        .ctor = ecs_ctor(Mass),
        .dtor = ecs_dtor(Mass),
        .copy = ecs_copy(Mass),
        .move = ecs_move(Mass)
    });

    ecs_set(world, ecs_entity(Rotation), EcsComponentLifecycle, {
        .ctor = ecs_ctor(Rotation),
        .dtor = ecs_dtor(Rotation),
        .copy = ecs_copy(Rotation),
        .move = ecs_move(Rotation)
    });    

    ecs_staging_begin(world);

    /* Position is not destroyed here. It was never copied to the stage, and it
     * still exists in the main stage, so it will only be destroyed after the
     * data is merged from stage to main stage. */
    ecs_remove(world, e, Position);
    test_int(ctor_position, 0);
    test_int(dtor_position, 0);
    test_int(copy_position, 0);
    test_int(move_position, 0);

    /* Velocity, Rotation is constructed and copied to stage */
    test_int(ctor_velocity, 1);
    test_int(copy_velocity, 1);
    test_int(move_velocity, 0);

    test_int(ctor_rotation, 1);
    test_int(copy_rotation, 1);
    test_int(move_rotation, 0);

    /* Mass is constructed in stage */
    ecs_add(world, e, Mass);
    test_int(ctor_mass, 1);

    /* Rotation and Velocity are constructed & moved */
    test_int(ctor_velocity, 2);
    test_int(copy_velocity, 1);
    test_int(move_velocity, 1);
    test_int(ctor_rotation, 2);
    test_int(copy_rotation, 1);
    test_int(move_rotation, 1);

    /* Destroy Rotation in stage. This should trigger the destructor, as it
     * was copied to the stage. */
    ecs_remove(world, e, Rotation);
    test_int(dtor_rotation, 1);
    test_int(ctor_rotation, 2);
    test_int(copy_rotation, 1);
    test_int(move_rotation, 1);

    test_int(ctor_velocity, 3);
    test_int(copy_velocity, 1);
    test_int(move_velocity, 2);

    ecs_staging_end(world, false);

    test_assert(!ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(ecs_has(world, e, Mass));
    test_assert(!ecs_has(world, e, Rotation));

    /* Position is destroyed in main stage */
    test_int(ctor_position, 0);
    test_int(dtor_position, 1);
    test_int(copy_position, 0);
    test_int(move_position, 0);

    /* Velocity should have been moved */ 
    test_int(ctor_velocity, 5); /* 1 because constructed in stage */
    test_int(dtor_velocity, 0);
    test_int(copy_velocity, 1); /* 1 because copied to stage */
    test_int(move_velocity, 4);

    /* Rotation is also destroyed in the main stage. A copy of Rotation was made
     * to the stage, and thus it needs to be destructed twice. */
    test_int(ctor_rotation, 2); /* 1 because constructed in stage */
    test_int(dtor_rotation, 2); /* 2 because destroyed in stage */
    test_int(copy_rotation, 1); /* 1 because copied to stage */
    test_int(move_rotation, 1);

    /* Mass is moved to main stage */
    test_int(ctor_mass, 4); /* Component constructed in stage and main stage */
    test_int(dtor_mass, 0);
    test_int(copy_mass, 0);
    test_int(move_mass, 2);

    ecs_fini(world);
}

void ComponentLifecycle_merge_to_new_table() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .ctor = ecs_ctor(Position),
        .dtor = ecs_dtor(Position),
        .copy = ecs_copy(Position),
        .move = ecs_move(Position)
    });

    ecs_staging_begin(world);

    ecs_add(world, e, Position);

    ecs_staging_end(world, false);

    test_int(ctor_position, 2);
    test_int(move_position, 1);

    ecs_fini(world);
}

void ComponentLifecycle_delete_in_stage() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e, Position, Velocity, Mass);

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .ctor = ecs_ctor(Position),
        .dtor = ecs_dtor(Position),
        .copy = ecs_copy(Position),
        .move = ecs_move(Position)
    });

    ecs_set(world, ecs_entity(Velocity), EcsComponentLifecycle, {
        .ctor = ecs_ctor(Velocity),
        .dtor = ecs_dtor(Velocity),
        .copy = ecs_copy(Velocity),
        .move = ecs_move(Velocity)
    });   

    ecs_set(world, ecs_entity(Mass), EcsComponentLifecycle, {
        .ctor = ecs_ctor(Mass),
        .dtor = ecs_dtor(Mass),
        .copy = ecs_copy(Mass),
        .move = ecs_move(Mass)
    });

    ecs_staging_begin(world);

    /* None of the components should be destructed while in the stage as they
     * were never copied to the stage */
    ecs_delete(world, e);
    test_int(dtor_position, 0);
    test_int(dtor_velocity, 0);
    test_int(dtor_mass, 0);

    ecs_staging_end(world, false);

    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    test_assert(!ecs_has(world, e, Mass));

    /* Position is destroyed in main stage */
    test_int(ctor_position, 0);
    test_int(dtor_position, 1);
    test_int(copy_position, 0);
    test_int(move_position, 0);

    /* Velocity is destroyed in main stage */
    test_int(ctor_velocity, 0);
    test_int(dtor_velocity, 1);
    test_int(copy_velocity, 0);
    test_int(move_velocity, 0);

    /* Mass is destroyed in main stage */
    test_int(ctor_mass, 0);
    test_int(dtor_mass, 1);
    test_int(copy_mass, 0);
    test_int(move_mass, 0); 

    ecs_fini(world);
}

typedef struct Trait {
    float value_1;
    float value_2;
} Trait;

void ComponentLifecycle_ctor_on_add_trait() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Trait);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Trait), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.ctor.invoked, 0);

    ecs_add_trait(world, e, ecs_entity(Position), ecs_entity(Trait));
    test_int(ctx.ctor.invoked, 1);
    test_assert(ctx.ctor.world == world);
    test_int(ctx.ctor.component, ecs_entity(Trait));
    test_int(ctx.ctor.entity, e);
    test_int(ctx.ctor.size, sizeof(Trait));
    test_int(ctx.ctor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_ctor_on_add_trait_set_ctor_after_table() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Trait);

    cl_ctx ctx = { { 0 } };

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.ctor.invoked, 0);
    ecs_add_trait(world, e, ecs_entity(Position), ecs_entity(Trait));

    /* Remove trait so we can add it again after registering the ctor */
    ecs_remove_trait(world, e, ecs_entity(Position), ecs_entity(Trait));

    /* Register component after table has been created */
    ecs_set(world, ecs_entity(Trait), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    /* Re-add */
    ecs_add_trait(world, e, ecs_entity(Position), ecs_entity(Trait));    

    test_int(ctx.ctor.invoked, 1);
    test_assert(ctx.ctor.world == world);
    test_int(ctx.ctor.component, ecs_entity(Trait));
    test_int(ctx.ctor.entity, e);
    test_int(ctx.ctor.size, sizeof(Trait));
    test_int(ctx.ctor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_ctor_on_add_trait_tag() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Trait);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.ctor.invoked, 0);

    ecs_add_trait(world, e, ecs_entity(Position), Trait);

    test_int(ctx.ctor.invoked, 1);
    test_assert(ctx.ctor.world == world);
    test_int(ctx.ctor.component, ecs_entity(Position));
    test_int(ctx.ctor.entity, e);
    test_int(ctx.ctor.size, sizeof(Position));
    test_int(ctx.ctor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_ctor_on_add_trait_tag_set_ctor_after_table() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Trait);

    cl_ctx ctx = { { 0 } };

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.ctor.invoked, 0);
    ecs_add_trait(world, e, ecs_entity(Position), Trait);

    /* Remove trait so we can add it again after registering the ctor */
    ecs_remove_trait(world, e, ecs_entity(Position), Trait);

    /* Register component after table has been created */
    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    /* Re-add */
    ecs_add_trait(world, e, ecs_entity(Position), Trait); 

    test_int(ctx.ctor.invoked, 1);
    test_assert(ctx.ctor.world == world);
    test_int(ctx.ctor.component, ecs_entity(Position));
    test_int(ctx.ctor.entity, e);
    test_int(ctx.ctor.size, sizeof(Position));
    test_int(ctx.ctor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_ctor_on_move_trait() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Trait);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Trait), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    /* Create entity in existing table */
    ecs_entity_t e = ecs_new(world, Position);
    test_int(ctx.ctor.invoked, 0);

    /* Add trait to existing table */
    ecs_add_trait(world, e, ecs_entity(Position), ecs_entity(Trait));

    test_int(ctx.ctor.invoked, 1);
    test_assert(ctx.ctor.world == world);
    test_int(ctx.ctor.component, ecs_entity(Trait));
    test_int(ctx.ctor.entity, e);
    test_int(ctx.ctor.size, sizeof(Trait));
    test_int(ctx.ctor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_move_on_realloc() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .move = comp_move,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.ctor.invoked, 0);

    ecs_add(world, e, Position);
    test_int(ctx.ctor.invoked, 1);
    test_int(ctx.move.invoked, 0);
    test_assert(ctx.ctor.world == world);
    test_int(ctx.ctor.component, ecs_entity(Position));
    test_int(ctx.ctor.entity, e);
    test_int(ctx.ctor.size, sizeof(Position));
    test_int(ctx.ctor.count, 1);

    ctx = (cl_ctx){ { 0 } };

    /* Trigger realloc & move */
    ecs_new(world, Position);
    ecs_new(world, Position);
    test_int(ctx.ctor.invoked, 2);
    test_int(ctx.move.invoked, 1);

    ecs_fini(world);
}

void ComponentLifecycle_move_on_dim() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .move = comp_move,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.ctor.invoked, 0);

    ecs_add(world, e, Position);
    test_int(ctx.ctor.invoked, 1);
    test_int(ctx.move.invoked, 0);
    test_assert(ctx.ctor.world == world);
    test_int(ctx.ctor.component, ecs_entity(Position));
    test_int(ctx.ctor.entity, e);
    test_int(ctx.ctor.size, sizeof(Position));
    test_int(ctx.ctor.count, 1);

    ctx = (cl_ctx){ { 0 } };

    /* Trigger realloc & move */
    ecs_dim_type(world, ecs_type(Position), 1000);
    test_int(ctx.ctor.invoked, 1);
    test_int(ctx.move.invoked, 1);

    ecs_fini(world);
}

void ComponentLifecycle_move_on_bulk_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .move = comp_move,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.ctor.invoked, 0);

    ecs_add(world, e, Position);
    test_int(ctx.ctor.invoked, 1);
    test_int(ctx.move.invoked, 0);
    test_assert(ctx.ctor.world == world);
    test_int(ctx.ctor.component, ecs_entity(Position));
    test_int(ctx.ctor.entity, e);
    test_int(ctx.ctor.size, sizeof(Position));
    test_int(ctx.ctor.count, 1);

    ctx = (cl_ctx){ { 0 } };

    /* Trigger realloc & move */
    ecs_bulk_new(world, Position, 1000);
    test_int(ctx.ctor.invoked, 1);
    test_int(ctx.move.invoked, 1);

    ecs_fini(world);
}

void ComponentLifecycle_move_on_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .move = comp_move,
        .ctx = &ctx
    });

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);
    test_int(ctx.ctor.invoked, 2);

    ctx = (cl_ctx){ { 0 } };

    ecs_delete(world, e1); /* Should trigger move of e2 */

    test_int(ctx.ctor.invoked, 0);
    test_int(ctx.move.invoked, 1);
    test_assert(ctx.move.world == world);
    test_int(ctx.move.component, ecs_entity(Position));
    test_int(ctx.move.entity, e2);
    test_int(ctx.move.size, sizeof(Position));
    test_int(ctx.move.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_override_trait() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Trait);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Trait), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new(world, 0);
    ecs_add_trait(world, base, ecs_entity(Position), ecs_entity(Trait));
    test_int(ctx.ctor.invoked, 1);
    test_int(ctx.copy.invoked, 0);

    ecs_entity_t instance = ecs_new_w_entity(world, ECS_INSTANCEOF | base);
    test_int(ctx.ctor.invoked, 1); /* No change */
    test_int(ctx.copy.invoked, 0);

    ctx = (cl_ctx){ { 0 } };

    /* Override */
    ecs_add_trait(world, instance, ecs_entity(Position), ecs_entity(Trait));

    test_int(ctx.ctor.invoked, 1);
    test_int(ctx.copy.invoked, 1);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_entity(Trait));
    test_int(ctx.copy.entity, instance);
    test_int(ctx.copy.size, sizeof(Trait));
    test_int(ctx.copy.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_override_trait_tag() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Trait);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new(world, 0);
    ecs_add_trait(world, base, ecs_entity(Position), Trait);
    test_int(ctx.ctor.invoked, 1);
    test_int(ctx.copy.invoked, 0);

    ecs_entity_t instance = ecs_new_w_entity(world, ECS_INSTANCEOF | base);
    test_int(ctx.ctor.invoked, 1); /* No change */
    test_int(ctx.copy.invoked, 0);

    ctx = (cl_ctx){ { 0 } };

    /* Override */
    ecs_add_trait(world, instance, ecs_entity(Position), Trait);

    test_int(ctx.ctor.invoked, 1);
    test_int(ctx.copy.invoked, 1);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_entity(Position));
    test_int(ctx.copy.entity, instance);
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_set_trait() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Trait);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Trait), EcsComponentLifecycle, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.copy.invoked, 0);
    
    ecs_set_trait(world, e, Position, Trait, {0, 0});
    test_int(ctx.copy.invoked, 1);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_entity(Trait));
    test_int(ctx.copy.entity, e);
    test_int(ctx.copy.size, sizeof(Trait));
    test_int(ctx.copy.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_set_trait_tag() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Trait);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_entity(Position), EcsComponentLifecycle, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.copy.invoked, 0);
    
    ecs_set_trait_tag(world, e, Trait, Position, {0, 0});
    test_int(ctx.copy.invoked, 1);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_entity(Position));
    test_int(ctx.copy.entity, e);
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 1);

    ecs_fini(world);
}
