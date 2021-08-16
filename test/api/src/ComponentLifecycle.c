#include <api.h>

void ComponentLifecycle_setup() {
    ecs_tracing_enable(-3);
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
    data->ctor.count += count;
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
    data->dtor.count += count;
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
    data->copy.count += count;
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

    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.ctor.invoked, 0);

    ecs_add(world, e, Position);
    test_assert(ctx.ctor.invoked != 0);
    test_assert(ctx.ctor.world == world);
    test_int(ctx.ctor.component, ecs_id(Position));
    test_int(ctx.ctor.entity, e);
    test_int(ctx.ctor.size, sizeof(Position));
    test_int(ctx.ctor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_ctor_on_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(ctx.ctor.invoked != 0);
    test_assert(ctx.ctor.world == world);
    test_int(ctx.ctor.component, ecs_id(Position));
    test_int(ctx.ctor.entity, e);
    test_int(ctx.ctor.size, sizeof(Position));
    test_int(ctx.ctor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_dtor_on_remove() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .dtor = comp_dtor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, Position);
    test_int(ctx.dtor.invoked, 0);

    ecs_remove(world, e, Position);
    test_assert(ctx.dtor.invoked != 0);
    test_assert(ctx.dtor.world == world);
    test_int(ctx.dtor.component, ecs_id(Position));
    test_int(ctx.dtor.entity, e);
    test_int(ctx.dtor.size, sizeof(Position));
    test_int(ctx.dtor.count, 1);
    
    ecs_fini(world);
}

void ComponentLifecycle_dtor_on_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .dtor = comp_dtor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, Position);
    test_int(ctx.dtor.invoked, 0);

    ecs_delete(world, e);
    test_assert(ctx.dtor.invoked != 0);
    test_assert(ctx.dtor.world == world);
    test_int(ctx.dtor.component, ecs_id(Position));
    test_int(ctx.dtor.entity, e);
    test_int(ctx.dtor.size, sizeof(Position));
    test_int(ctx.dtor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.copy.invoked, 0);
    
    ecs_set(world, e, Position, {0, 0});
    test_assert(ctx.copy.invoked != 0);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_id(Position));
    test_int(ctx.copy.entity, e);
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_override() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new(world, Position);
    test_int(ctx.copy.invoked, 0);

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);
    test_int(ctx.copy.invoked, 0);

    ecs_add(world, e, Position);
    test_assert(ctx.copy.invoked != 0);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_id(Position));
    test_int(ctx.copy.entity, e);
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_clone() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(ctx.copy.invoked != 0);
    memset(&ctx, 0, sizeof(ctx));

    ecs_entity_t clone = ecs_clone(world, 0, e, true);
    test_assert(ctx.copy.invoked != 0);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_id(Position));
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

    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(ctx.copy.invoked != 0);
    memset(&ctx, 0, sizeof(ctx));

    ecs_add(world, e, Velocity);
    test_int(ctx.copy.invoked, 0);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_snapshot() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    cl_ctx ctx = { { 0 } };
    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
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

    test_assert(ctx.copy.invoked != 0);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_id(Position));
    test_int(ctx.copy.entity, ids[i - 1]);
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 10);

    ctx = (cl_ctx){ { 0 } };

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    test_assert(ctx.copy.invoked != 0);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_id(Position));
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
    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
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

    test_assert(ctx.copy.invoked != 0);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_id(Position));
    test_int(ctx.copy.entity, ids[i - 1]);
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 10);

    ctx = (cl_ctx){ { 0 } };

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    test_assert(ctx.ctor.invoked != 0);
    test_assert(ctx.ctor.world == world);
    test_int(ctx.ctor.component, ecs_id(Position));
    test_int(ctx.ctor.entity, ids[0]);
    test_int(ctx.ctor.size, sizeof(Position));
    test_int(ctx.ctor.count, 10);

    test_assert(ctx.copy.invoked != 0);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_id(Position));
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
    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
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
    
    test_assert(ctx.dtor.invoked != 0);
    ctx = (cl_ctx){ { 0 } };

    ecs_snapshot_restore(world, s);

    test_assert(ctx.dtor.invoked != 0);
    test_assert(ctx.dtor.world == world);
    test_int(ctx.dtor.component, ecs_id(Position));
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
    ptr->x = 0;
    ptr->y = 0;
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
    ecs_world_t *world = ecs_mini();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e, Position, Velocity, Rotation);

    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .ctor = ecs_ctor(Position),
        .dtor = ecs_dtor(Position),
        .copy = ecs_copy(Position),
        .move = ecs_move(Position)
    });

    ecs_set(world, ecs_id(Velocity), EcsComponentLifecycle, {
        .ctor = ecs_ctor(Velocity),
        .dtor = ecs_dtor(Velocity),
        .copy = ecs_copy(Velocity),
        .move = ecs_move(Velocity)
    });   

    ecs_set(world, ecs_id(Mass), EcsComponentLifecycle, {
        .ctor = ecs_ctor(Mass),
        .dtor = ecs_dtor(Mass),
        .copy = ecs_copy(Mass),
        .move = ecs_move(Mass)
    });

    ecs_set(world, ecs_id(Rotation), EcsComponentLifecycle, {
        .ctor = ecs_ctor(Rotation),
        .dtor = ecs_dtor(Rotation),
        .copy = ecs_copy(Rotation),
        .move = ecs_move(Rotation)
    });    

    ecs_defer_begin(world);

    ecs_remove(world, e, Position);
    test_int(ctor_position, 0);
    test_int(dtor_position, 0);
    test_int(copy_position, 0);
    test_int(move_position, 0);

    test_int(ctor_velocity, 0);
    test_int(dtor_velocity, 0);
    test_int(copy_velocity, 0);
    test_int(move_velocity, 0);

    test_int(ctor_rotation, 0);
    test_int(dtor_rotation, 0);
    test_int(copy_rotation, 0);
    test_int(move_rotation, 0);

    ecs_add(world, e, Mass);
    test_int(ctor_mass, 0);

    test_int(ctor_velocity, 0);
    test_int(dtor_velocity, 0);
    test_int(copy_velocity, 0);
    test_int(move_velocity, 0);

    test_int(ctor_rotation, 0);
    test_int(dtor_rotation, 0);
    test_int(copy_rotation, 0);
    test_int(move_rotation, 0);

    ecs_remove(world, e, Rotation);
    test_int(ctor_rotation, 0);
    test_int(dtor_rotation, 0);
    test_int(copy_rotation, 0);
    test_int(move_rotation, 0);

    test_int(ctor_velocity, 0);
    test_int(dtor_velocity, 0);
    test_int(copy_velocity, 0);
    test_int(move_velocity, 0);

    ecs_defer_end(world);

    test_assert(!ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(ecs_has(world, e, Mass));
    test_assert(!ecs_has(world, e, Rotation));

    test_int(ctor_position, 0);
    test_int(dtor_position, 1); // removed first, no moves
    test_int(copy_position, 0);
    test_int(move_position, 0);

    test_int(ctor_velocity, 3); // got moved 3 times
    test_int(dtor_velocity, 3);
    test_int(copy_velocity, 0);
    test_int(move_velocity, 3);

    test_int(ctor_rotation, 2); // got moved 2 times, then removed
    test_int(dtor_rotation, 3);
    test_int(copy_rotation, 0);
    test_int(move_rotation, 2);

    test_int(ctor_mass, 2); // got added, moved once
    test_int(dtor_mass, 1);
    test_int(copy_mass, 0);
    test_int(move_mass, 1);

    ecs_fini(world);
}

void ComponentLifecycle_merge_to_new_table() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);

    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .ctor = ecs_ctor(Position),
        .dtor = ecs_dtor(Position),
        .copy = ecs_copy(Position),
        .move = ecs_move(Position)
    });

    ecs_defer_begin(world);

    ecs_add(world, e, Position);

    ecs_defer_end(world);

    test_int(ctor_position, 1);
    test_int(move_position, 0);

    ecs_fini(world);
}

void ComponentLifecycle_delete_in_stage() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e, Position, Velocity, Mass);

    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .ctor = ecs_ctor(Position),
        .dtor = ecs_dtor(Position),
        .copy = ecs_copy(Position),
        .move = ecs_move(Position)
    });

    ecs_set(world, ecs_id(Velocity), EcsComponentLifecycle, {
        .ctor = ecs_ctor(Velocity),
        .dtor = ecs_dtor(Velocity),
        .copy = ecs_copy(Velocity),
        .move = ecs_move(Velocity)
    });   

    ecs_set(world, ecs_id(Mass), EcsComponentLifecycle, {
        .ctor = ecs_ctor(Mass),
        .dtor = ecs_dtor(Mass),
        .copy = ecs_copy(Mass),
        .move = ecs_move(Mass)
    });

    ecs_defer_begin(world);

    /* None of the components should be destructed while in the stage as they
     * were never copied to the stage */
    ecs_delete(world, e);
    test_int(dtor_position, 0);
    test_int(dtor_velocity, 0);
    test_int(dtor_mass, 0);

    ecs_defer_end(world);

    test_assert(ecs_exists(world, e));
    test_assert(!ecs_is_alive(world, e));

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

typedef struct Pair {
    float value_1;
    float value_2;
} Pair;

void ComponentLifecycle_ctor_on_add_pair() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Pair);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_id(Pair), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.ctor.invoked, 0);

    ecs_add_pair(world, e, ecs_id(Pair), ecs_id(Position));
    test_assert(ctx.ctor.invoked != 0);
    test_assert(ctx.ctor.world == world);
    test_int(ctx.ctor.component, ecs_id(Pair));
    test_int(ctx.ctor.entity, e);
    test_int(ctx.ctor.size, sizeof(Pair));
    test_int(ctx.ctor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_ctor_on_add_pair_set_ctor_after_table() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Pair);

    cl_ctx ctx = { { 0 } };

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.ctor.invoked, 0);
    ecs_add_pair(world, e, ecs_id(Pair), ecs_id(Position));

    /* Remove pair so we can add it again after registering the ctor */
    ecs_remove_pair(world, e, ecs_id(Pair), ecs_id(Position));

    /* Register component after table has been created */
    ecs_set(world, ecs_id(Pair), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    /* Re-add */
    ecs_add_pair(world, e, ecs_id(Pair), ecs_id(Position));

    test_assert(ctx.ctor.invoked != 0);
    test_assert(ctx.ctor.world == world);
    test_int(ctx.ctor.component, ecs_id(Pair));
    test_int(ctx.ctor.entity, e);
    test_int(ctx.ctor.size, sizeof(Pair));
    test_int(ctx.ctor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_ctor_on_add_pair_tag() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Pair);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.ctor.invoked, 0);

    ecs_add_pair(world, e, Pair, ecs_id(Position));

    test_assert(ctx.ctor.invoked != 0);
    test_assert(ctx.ctor.world == world);
    test_int(ctx.ctor.component, ecs_id(Position));
    test_int(ctx.ctor.entity, e);
    test_int(ctx.ctor.size, sizeof(Position));
    test_int(ctx.ctor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_ctor_on_add_pair_tag_set_ctor_after_table() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Pair);

    cl_ctx ctx = { { 0 } };

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.ctor.invoked, 0);
    ecs_add_pair(world, e, Pair, ecs_id(Position));

    /* Remove pair so we can add it again after registering the ctor */
    ecs_remove_pair(world, e, Pair, ecs_id(Position));

    /* Register component after table has been created */
    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    /* Re-add */
    ecs_add_pair(world, e, Pair, ecs_id(Position)); 

    test_assert(ctx.ctor.invoked != 0);
    test_assert(ctx.ctor.world == world);
    test_int(ctx.ctor.component, ecs_id(Position));
    test_int(ctx.ctor.entity, e);
    test_int(ctx.ctor.size, sizeof(Position));
    test_int(ctx.ctor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_ctor_on_move_pair() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Pair);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_id(Pair), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    /* Create entity in existing table */
    ecs_entity_t e = ecs_new(world, Position);
    test_int(ctx.ctor.invoked, 0);

    /* Add pair to existing table */
    ecs_add_pair(world, e, ecs_id(Pair), ecs_id(Position));

    test_assert(ctx.ctor.invoked != 0);
    test_assert(ctx.ctor.world == world);
    test_int(ctx.ctor.component, ecs_id(Pair));
    test_int(ctx.ctor.entity, e);
    test_int(ctx.ctor.size, sizeof(Pair));
    test_int(ctx.ctor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_move_on_realloc() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .move = comp_move,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.ctor.invoked, 0);

    ecs_add(world, e, Position);
    test_assert(ctx.ctor.invoked != 0);
    test_int(ctx.move.invoked, 0);
    test_assert(ctx.ctor.world == world);
    test_int(ctx.ctor.component, ecs_id(Position));
    test_int(ctx.ctor.entity, e);
    test_int(ctx.ctor.size, sizeof(Position));
    test_int(ctx.ctor.count, 1);

    ctx = (cl_ctx){ { 0 } };

    /* Trigger realloc & move */
    ecs_new(world, Position);
    ecs_new(world, Position);
    test_assert(ctx.ctor.invoked != 0);
    test_assert(ctx.move.invoked != 0);

    ecs_fini(world);
}

void ComponentLifecycle_move_on_bulk_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .move = comp_move,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.ctor.invoked, 0);

    ecs_add(world, e, Position);
    test_assert(ctx.ctor.invoked != 0);
    test_int(ctx.move.invoked, 0);
    test_assert(ctx.ctor.world == world);
    test_int(ctx.ctor.component, ecs_id(Position));
    test_int(ctx.ctor.entity, e);
    test_int(ctx.ctor.size, sizeof(Position));
    test_int(ctx.ctor.count, 1);

    ctx = (cl_ctx){ { 0 } };

    /* Trigger realloc & move */
    ecs_bulk_new(world, Position, 1000);
    test_assert(ctx.ctor.invoked != 0);
    test_assert(ctx.move.invoked != 0);

    ecs_fini(world);
}

void ComponentLifecycle_move_on_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .move = comp_move,
        .ctx = &ctx
    });

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);
    test_assert(ctx.ctor.invoked != 0);

    ctx = (cl_ctx){ { 0 } };

    ecs_delete(world, e1); /* Should trigger move of e2 */

    test_int(ctx.ctor.invoked, 0);
    test_assert(ctx.move.invoked != 0);
    test_assert(ctx.move.world == world);
    test_int(ctx.move.component, ecs_id(Position));
    test_int(ctx.move.entity, e2);
    test_int(ctx.move.size, sizeof(Position));
    test_int(ctx.move.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_move_dtor_on_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .dtor = comp_dtor,
        .move = comp_move,
        .ctx = &ctx
    });

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);

    ctx = (cl_ctx){ { 0 } };

    ecs_delete(world, e1); /* Should trigger move of e2 */

    test_assert(ctx.dtor.invoked != 0);
    test_assert(ctx.move.invoked != 0);
    test_assert(ctx.move.world == world);

    test_int(ctx.dtor.component, ecs_id(Position));
    test_int(ctx.dtor.entity, e1);
    test_int(ctx.dtor.size, sizeof(Position));
    test_int(ctx.dtor.count, 1);

    test_int(ctx.move.component, ecs_id(Position));
    test_int(ctx.move.entity, e2);
    test_int(ctx.move.size, sizeof(Position));
    test_int(ctx.move.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_override_pair() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Pair);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_id(Pair), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new(world, 0);
    ecs_add_pair(world, base, ecs_id(Pair), ecs_id(Position));
    test_assert(ctx.ctor.invoked != 0);
    test_int(ctx.copy.invoked, 0);

    ecs_entity_t instance = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(ctx.ctor.invoked != 0); /* No change */
    test_int(ctx.copy.invoked, 0);

    ctx = (cl_ctx){ { 0 } };

    /* Override */
    ecs_add_pair(world, instance, ecs_id(Pair), ecs_id(Position));

    test_assert(ctx.ctor.invoked != 0);
    test_assert(ctx.copy.invoked != 0);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_id(Pair));
    test_int(ctx.copy.entity, instance);
    test_int(ctx.copy.size, sizeof(Pair));
    test_int(ctx.copy.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_override_pair_tag() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Pair);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new(world, 0);
    ecs_add_pair(world, base, Pair, ecs_id(Position));
    test_assert(ctx.ctor.invoked != 0);
    test_int(ctx.copy.invoked, 0);

    ecs_entity_t instance = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(ctx.ctor.invoked != 0); /* No change */
    test_int(ctx.copy.invoked, 0);

    ctx = (cl_ctx){ { 0 } };

    /* Override */
    ecs_add_pair(world, instance, Pair, ecs_id(Position));

    test_assert(ctx.ctor.invoked != 0);
    test_assert(ctx.copy.invoked != 0);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_id(Position));
    test_int(ctx.copy.entity, instance);
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_set_pair() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Pair);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_id(Pair), EcsComponentLifecycle, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.copy.invoked, 0);
    
    ecs_set_pair(world, e, Pair, ecs_id(Position), {0, 0});
    test_assert(ctx.copy.invoked != 0);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_id(Pair));
    test_int(ctx.copy.entity, e);
    test_int(ctx.copy.size, sizeof(Pair));
    test_int(ctx.copy.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_set_pair_tag() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Pair);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.copy.invoked, 0);
    
    ecs_set_pair_object(world, e, Pair, Position, {0, 0});
    test_assert(ctx.copy.invoked != 0);
    test_assert(ctx.copy.world == world);
    test_int(ctx.copy.component, ecs_id(Position));
    test_int(ctx.copy.entity, e);
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_prevent_lifecycle_overwrite() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();

    ECS_COMPONENT(world, Position);

    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .ctor = comp_ctor
    });

    /* Should trigger assert */
    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .ctor = ecs_ctor(Position)
    });

    ecs_fini(world);    
}

void ComponentLifecycle_prevent_lifecycle_overwrite_null_callbacks() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();

    ECS_COMPONENT(world, Position);

    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        NULL
    });

    /* Should trigger assert */
    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .ctor = ecs_ctor(Position)
    });

    ecs_fini(world);  
}

void ComponentLifecycle_allow_lifecycle_overwrite_equal_callbacks() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .ctor = ecs_ctor(Position)
    });

    /* Same actions, should be ok */
    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .ctor = ecs_ctor(Position)
    });

    ecs_new(world, Position);

    test_int(ctor_position, 1);

    ecs_fini(world); 
}

static
void AddPosition(ecs_iter_t *it) { }

void ComponentLifecycle_set_lifecycle_after_trigger() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TRIGGER(world, AddPosition, EcsOnAdd, Position);

    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .ctor = ecs_ctor(Position)
    });

    ecs_new(world, Position);

    test_int(ctor_position, 1);

    ecs_fini(world);  
}

static int dummy_dtor_invoked = 0;

typedef struct Dummy {
    int value;
} Dummy;

static
void dummy_dtor(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_entity_t *entity_ptr,
    void *ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    int i;
    for (i = 0; i < count; i ++) {
        test_assert(ecs_is_valid(world, entity_ptr[i]));
        test_assert(ecs_is_alive(world, entity_ptr[i]));
    }

    dummy_dtor_invoked ++;
}

void ComponentLifecycle_valid_entity_in_dtor_after_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Dummy);

    ecs_set(world, ecs_id(Dummy), EcsComponentLifecycle, {
        .dtor = dummy_dtor
    });

    ecs_entity_t e = ecs_new(world, Dummy);
    test_assert(e != 0);

    ecs_delete(world, e);
    test_assert(!ecs_is_valid(world, e));
    test_assert(!ecs_is_alive(world, e));

    test_int(dummy_dtor_invoked, 1);

    ecs_fini(world);  
}

void ComponentLifecycle_ctor_w_emplace() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);

    const Position *ptr = ecs_emplace(world, e, Position);
    test_assert(ptr != NULL);
    test_int(ctx.ctor.invoked, 0);

    ecs_fini(world); 
}

void ComponentLifecycle_dtor_on_fini() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Dummy);

    ecs_set(world, ecs_id(Dummy), EcsComponentLifecycle, {
        .dtor = dummy_dtor
    });

    ecs_entity_t e = ecs_new(world, Dummy);
    test_assert(e != 0);

    test_int(dummy_dtor_invoked, 0);

    ecs_fini(world); 

    test_int(dummy_dtor_invoked, 1);
}

static
void type_dtor(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_entity_t *entity_ptr,
    void *ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    test_int(count, 1);
    test_assert(entity_ptr != NULL);

    ecs_entity_t e = entity_ptr[0];
    test_assert(e != 0);

    ecs_type_t type = ecs_get_type(world, e);
    test_assert(type != NULL);

    test_int(ecs_vector_count(type), 3);

    dummy_dtor_invoked = 1;
}

typedef struct {
    ecs_entity_t e;
} Entity;

typedef struct {
    char *str;
} String;

static int other_dtor_invoked;
static int other_dtor_valid_entity;

static
void other_type_dtor(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_entity_t *entity_ptr,
    void *ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    test_int(count, 1);
    test_assert(entity_ptr != NULL);
    test_assert(ptr != NULL);

    ecs_entity_t e = entity_ptr[0];
    test_assert(e != 0);
    test_assert(ecs_is_valid(world, e));

    Entity *comp = ptr;
    test_assert(comp->e != 0);
    
    if (ecs_is_valid(world, comp->e)) {
        ecs_type_t type = ecs_get_type(world, comp->e);
        test_assert(type != NULL);
        test_int(ecs_vector_count(type), 2);
        other_dtor_valid_entity ++;
    }

    other_dtor_invoked ++;
}

ECS_COMPONENT_DECLARE(String);
ECS_COMPONENT_DECLARE(Entity);

static
void other_comp_dtor(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_entity_t *entity_ptr,
    void *ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    test_int(count, 1);
    test_assert(entity_ptr != NULL);
    test_assert(ptr != NULL);

    Entity *comp = ptr;

    ecs_entity_t e = entity_ptr[0];
    test_assert(e != 0);
    test_assert(ecs_is_valid(world, e));

    test_assert(comp->e != 0);
    
    if (ecs_is_valid(world, comp->e)) {
        if (ecs_has(world, comp->e, String)) {
            const String *str_ptr = ecs_get(world, comp->e, String);
            test_assert(str_ptr != NULL);
            test_assert(str_ptr->str != NULL);
            test_str(str_ptr->str, "Foo");
            other_dtor_valid_entity ++;
        } else {
            test_assert(ecs_get(world, comp->e, String) == NULL);
        }
    }

    other_dtor_invoked ++;
}

static
void other_delete_dtor(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_entity_t *entity_ptr,
    void *ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    test_int(count, 1);
    test_assert(entity_ptr != NULL);
    test_assert(ptr != NULL);

    Entity *comp = ptr;

    ecs_entity_t e = entity_ptr[0];
    test_assert(e != 0);
    test_assert(ecs_is_valid(world, e));

    test_assert(comp->e != 0);
    
    if (ecs_is_valid(world, comp->e)) {
        ecs_delete(world, comp->e);
        other_dtor_valid_entity ++;

        test_assert(ecs_is_valid(world, e));
        test_assert(entity_ptr[0] == e);
        test_assert(ecs_get(world, e, Entity) == comp);
        test_assert(ecs_is_valid(world, comp->e));
    }

    other_dtor_invoked ++;
}

static
void self_delete_dtor(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_entity_t *entity_ptr,
    void *ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    test_int(count, 1);
    test_assert(entity_ptr != NULL);
    test_assert(ptr != NULL);

    ecs_entity_t e = entity_ptr[0];
    test_assert(e != 0);
    
    if (ecs_is_valid(world, e)) {
        ecs_delete(world, e);

        // Delete should be deferred
        test_assert(ecs_is_valid(world, e));
    }

    dummy_dtor_invoked ++;
}

static
void string_dtor(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_entity_t *entity_ptr,
    void *ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    test_int(count, 1);
    test_assert(entity_ptr != NULL);
    test_assert(ptr != NULL);

    String *comp = ptr;
    ecs_entity_t e = entity_ptr[0];
    test_assert(e != 0);
    test_assert(ecs_is_valid(world, e));

    test_assert(comp->str != NULL);
    ecs_os_free(comp->str);
}

void ComponentLifecycle_valid_type_in_dtor_on_fini() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Dummy);

    ecs_set(world, ecs_id(Dummy), EcsComponentLifecycle, {
        .dtor = type_dtor
    });

    ecs_entity_t e = ecs_new(world, Dummy);
    test_assert(e != 0);
    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);

    test_int(dummy_dtor_invoked, 0);

    ecs_fini(world); 

    test_int(dummy_dtor_invoked, 1);
}

void ComponentLifecycle_valid_other_type_of_entity_in_dtor_on_fini() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT_DEFINE(world, Entity);

    ecs_set(world, ecs_id(Entity), EcsComponentLifecycle, {
        .dtor = other_type_dtor
    });

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_set(world, e2, Entity, {e1});

    ecs_add(world, e1, Velocity);
    ecs_set(world, e1, Entity, {e2});

    test_int(other_dtor_invoked, 0);

    ecs_fini(world); 

    test_int(other_dtor_invoked, 2);
    test_int(other_dtor_valid_entity, 1);
}

void ComponentLifecycle_valid_same_type_comp_of_entity_in_dtor_on_fini() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, String);
    ECS_COMPONENT_DEFINE(world, Entity);

    ecs_set(world, ecs_id(String), EcsComponentLifecycle, {
        .dtor = string_dtor
    });

    ecs_set(world, ecs_id(Entity), EcsComponentLifecycle, {
        .dtor = other_comp_dtor
    });

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);

    ecs_set(world, e2, Entity, {e1});
    ecs_set(world, e1, Entity, {e2});

    ecs_set(world, e1, String, {ecs_os_strdup("Foo")});
    ecs_set(world, e2, String, {ecs_os_strdup("Foo")});

    test_int(other_dtor_invoked, 0);

    ecs_fini(world); 

    test_int(other_dtor_invoked, 2);
    test_int(other_dtor_valid_entity, 1);
}

void ComponentLifecycle_valid_same_type_comp_of_entity_in_dtor_on_delete_parent() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, String);
    ECS_COMPONENT_DEFINE(world, Entity);

    ecs_set(world, ecs_id(String), EcsComponentLifecycle, {
        .dtor = string_dtor
    });

    ecs_set(world, ecs_id(Entity), EcsComponentLifecycle, {
        .dtor = other_comp_dtor
    });

    ecs_entity_t parent = ecs_new_id(world);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    
    ecs_add_pair(world, e1, EcsChildOf, parent);
    ecs_add_pair(world, e2, EcsChildOf, parent);

    ecs_set(world, e2, Entity, {e1});
    ecs_set(world, e1, Entity, {e2});

    ecs_set(world, e1, String, {ecs_os_strdup("Foo")});
    ecs_set(world, e2, String, {ecs_os_strdup("Foo")});

    test_int(other_dtor_invoked, 0);

    ecs_delete(world, parent);

    test_int(other_dtor_invoked, 2);
    test_int(other_dtor_valid_entity, 1);

    ecs_fini(world);
}

void ComponentLifecycle_delete_in_dtor_same_type_on_fini() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, String);
    ECS_COMPONENT_DEFINE(world, Entity);

    ecs_set(world, ecs_id(String), EcsComponentLifecycle, {
        .dtor = string_dtor
    });

    ecs_set(world, ecs_id(Entity), EcsComponentLifecycle, {
        .dtor = other_delete_dtor
    });

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);

    ecs_set(world, e2, Entity, {e1});
    ecs_set(world, e1, Entity, {e2});

    ecs_set(world, e1, String, {ecs_os_strdup("Foo")});
    ecs_set(world, e2, String, {ecs_os_strdup("Foo")});

    test_int(other_dtor_invoked, 0);

    ecs_fini(world);

    test_int(other_dtor_invoked, 2);
    test_int(other_dtor_valid_entity, 1);
}

void ComponentLifecycle_delete_in_dtor_other_type_on_fini() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, String);
    ECS_COMPONENT_DEFINE(world, Entity);
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_set(world, ecs_id(String), EcsComponentLifecycle, {
        .dtor = string_dtor
    });

    ecs_set(world, ecs_id(Entity), EcsComponentLifecycle, {
        .dtor = other_delete_dtor
    });

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Velocity);

    ecs_set(world, e2, Entity, {e1});
    ecs_set(world, e1, Entity, {e2});

    ecs_set(world, e1, String, {ecs_os_strdup("Foo")});
    ecs_set(world, e2, String, {ecs_os_strdup("Foo")});

    test_int(other_dtor_invoked, 0);

    ecs_fini(world);

    test_int(other_dtor_invoked, 2);
    test_int(other_dtor_valid_entity, 1);
}

void ComponentLifecycle_delete_self_in_dtor_on_fini() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Dummy);

    ecs_set(world, ecs_id(Dummy), EcsComponentLifecycle, {
        .dtor = self_delete_dtor
    });

    ecs_new(world, Dummy);
    ecs_new(world, Dummy);
    ecs_new(world, Dummy);

    test_int(dummy_dtor_invoked, 0);

    ecs_fini(world);

    test_int(dummy_dtor_invoked, 3);
}

void ComponentLifecycle_delete_in_dtor_same_type_on_delete_parent() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, String);
    ECS_COMPONENT_DEFINE(world, Entity);

    ecs_set(world, ecs_id(String), EcsComponentLifecycle, {
        .dtor = string_dtor
    });

    ecs_set(world, ecs_id(Entity), EcsComponentLifecycle, {
        .dtor = other_delete_dtor
    });

    ecs_entity_t parent = ecs_new_id(world);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);

    ecs_add_pair(world, e1, EcsChildOf, parent);
    ecs_add_pair(world, e2, EcsChildOf, parent);

    ecs_set(world, e2, Entity, {e1});
    ecs_set(world, e1, Entity, {e2});

    ecs_set(world, e1, String, {ecs_os_strdup("Foo")});
    ecs_set(world, e2, String, {ecs_os_strdup("Foo")});

    test_int(other_dtor_invoked, 0);

    ecs_delete(world, parent);

    test_int(other_dtor_invoked, 2);
    test_int(other_dtor_valid_entity, 1);

    test_assert(!ecs_is_alive(world, e1));
    test_assert(!ecs_is_alive(world, e2));

    ecs_fini(world);
}

void ComponentLifecycle_delete_in_dtor_other_type_on_delete_parent() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, String);
    ECS_COMPONENT_DEFINE(world, Entity);
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_set(world, ecs_id(String), EcsComponentLifecycle, {
        .dtor = string_dtor
    });

    ecs_set(world, ecs_id(Entity), EcsComponentLifecycle, {
        .dtor = other_delete_dtor
    });

    ecs_entity_t parent = ecs_new_id(world);

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Velocity);

    ecs_add_pair(world, e1, EcsChildOf, parent);
    ecs_add_pair(world, e2, EcsChildOf, parent);

    ecs_set(world, e2, Entity, {e1});
    ecs_set(world, e1, Entity, {e2});

    ecs_set(world, e1, String, {ecs_os_strdup("Foo")});
    ecs_set(world, e2, String, {ecs_os_strdup("Foo")});

    test_int(other_dtor_invoked, 0);

    ecs_delete(world, parent);

    test_int(other_dtor_invoked, 2);
    test_int(other_dtor_valid_entity, 1);

    test_assert(!ecs_is_alive(world, e1));
    test_assert(!ecs_is_alive(world, e2));    

    ecs_fini(world);
}

void ComponentLifecycle_delete_self_in_dtor_on_delete_parent() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Dummy);

    ecs_set(world, ecs_id(Dummy), EcsComponentLifecycle, {
        .dtor = self_delete_dtor
    });

    ecs_entity_t parent = ecs_new_id(world);

    ecs_entity_t e1 = ecs_new(world, Dummy);
    ecs_entity_t e2 = ecs_new(world, Dummy);
    ecs_entity_t e3 = ecs_new(world, Dummy);

    ecs_add_pair(world, e1, EcsChildOf, parent);
    ecs_add_pair(world, e2, EcsChildOf, parent);
    ecs_add_pair(world, e3, EcsChildOf, parent);

    test_int(dummy_dtor_invoked, 0);

    ecs_delete(world, parent);

    test_int(dummy_dtor_invoked, 3);
    
    test_assert(!ecs_is_alive(world, e1));
    test_assert(!ecs_is_alive(world, e2));
    test_assert(!ecs_is_alive(world, e3));

    ecs_fini(world);
}

void ComponentLifecycle_delete_in_dtor_same_type_on_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, String);
    ECS_COMPONENT_DEFINE(world, Entity);

    ecs_set(world, ecs_id(String), EcsComponentLifecycle, {
        .dtor = string_dtor
    });

    ecs_set(world, ecs_id(Entity), EcsComponentLifecycle, {
        .dtor = other_delete_dtor
    });

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);

    ecs_set(world, e1, Entity, {e2});
    ecs_set(world, e2, Entity, {e3});
    ecs_set(world, e3, Entity, {e1});

    ecs_set(world, e1, String, {ecs_os_strdup("Foo")});
    ecs_set(world, e2, String, {ecs_os_strdup("Foo")});
    ecs_set(world, e3, String, {ecs_os_strdup("Foo")});

    test_int(other_dtor_invoked, 0);

    ecs_delete(world, e3);
    test_assert(!ecs_is_alive(world, e1));
    test_assert(!ecs_is_alive(world, e2));
    test_assert(!ecs_is_alive(world, e3));

    ecs_delete(world, e2);
    ecs_delete(world, e1);
    
    test_int(other_dtor_invoked, 3);
    test_int(other_dtor_valid_entity, 2);

    ecs_fini(world);
}

void ComponentLifecycle_delete_in_dtor_other_type_on_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, String);
    ECS_COMPONENT_DEFINE(world, Entity);
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_set(world, ecs_id(String), EcsComponentLifecycle, {
        .dtor = string_dtor
    });

    ecs_set(world, ecs_id(Entity), EcsComponentLifecycle, {
        .dtor = other_delete_dtor
    });

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Velocity);

    ecs_set(world, e2, Entity, {e1});
    ecs_set(world, e1, Entity, {e2});

    ecs_set(world, e1, String, {ecs_os_strdup("Foo")});
    ecs_set(world, e2, String, {ecs_os_strdup("Foo")});

    test_int(other_dtor_invoked, 0);

    ecs_delete(world, e1);
    ecs_delete(world, e2);

    test_int(other_dtor_invoked, 2);
    test_int(other_dtor_valid_entity, 1);

    ecs_fini(world);
}

void ComponentLifecycle_delete_self_in_dtor_on_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Dummy);

    ecs_set(world, ecs_id(Dummy), EcsComponentLifecycle, {
        .dtor = self_delete_dtor
    });

    ecs_entity_t e1 = ecs_new(world, Dummy);
    ecs_entity_t e2 = ecs_new(world, Dummy);
    ecs_entity_t e3 = ecs_new(world, Dummy);

    test_int(dummy_dtor_invoked, 0);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);

    test_int(dummy_dtor_invoked, 3);

    ecs_fini(world);
}

static int position_on_set_invoked;
static int velocity_on_set_invoked;

ECS_ON_SET(Position, ptr, {
    ptr->x += 1;
    ptr->y += 2;
    position_on_set_invoked ++;
})

ECS_ON_SET(Velocity, ptr, {
    ptr->x += 1;
    ptr->y += 2;
    velocity_on_set_invoked ++;
})

void ComponentLifecycle_on_set_after_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_set(world, ecs_id(Position), EcsComponentLifecycle, {
        .ctor = ecs_ctor(Position),
        .on_set = ecs_on_set(Position)
    });

    ecs_entity_t e = ecs_new(world, Position);
    test_int(ctor_position, 1);
    test_int(position_on_set_invoked, 0);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 0);
    test_int(p->y, 0);

    ecs_set(world, e, Position, {10, 20});
    test_int(ctor_position, 1);
    test_int(position_on_set_invoked, 1);

    test_assert(p == ecs_get(world, e, Position));
    test_int(p->x, 11);
    test_int(p->y, 22);

    ecs_fini(world);
}
