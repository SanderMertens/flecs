#include <api.h>

static ECS_COMPONENT_DECLARE(Position);
static ECS_COMPONENT_DECLARE(String);
static ECS_COMPONENT_DECLARE(Entity);

void ComponentLifecycle_setup() {
    ecs_log_set_level(-3);
}

typedef struct xtor_ctx {
    ecs_entity_t component;
    size_t size;
    int32_t count;
    int32_t invoked;
} xtor_ctx;

typedef struct copy_ctx {
    ecs_entity_t component;
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
    void *ptr,
    int32_t count,
    const ecs_type_info_t *info)
{
    cl_ctx *data = info->hooks.ctx;
    data->ctor.component = info->component;
    data->ctor.size = info->size;
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
    void *ptr,
    int32_t count,
    const ecs_type_info_t *info)
{
    cl_ctx *data = info->hooks.ctx;
    data->dtor.component = info->component;
    data->dtor.size = info->size;
    data->dtor.count += count;
    data->dtor.invoked ++;
}

static
void comp_copy(
    void *dst_ptr,
    const void *src_ptr,
    int32_t count,
    const ecs_type_info_t *info)
{
    cl_ctx *data = info->hooks.ctx;
    data->copy.component = info->component;
    data->copy.size = info->size;
    data->copy.count += count;
    data->copy.invoked ++;
    
    memcpy(dst_ptr, src_ptr, info->size * count);
}

static
void comp_move(
    void *dst_ptr,
    void *src_ptr,
    int32_t count,
    const ecs_type_info_t *info)
{
    cl_ctx *data = info->hooks.ctx;
    data->move.component = info->component;
    data->move.size = info->size;
    data->move.count = count;
    data->move.invoked ++;
    
    memcpy(dst_ptr, src_ptr, info->size * count);
}

void ComponentLifecycle_ctor_on_add() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.ctor.invoked, 0);

    ecs_add(world, e, Position);
    test_assert(ctx.ctor.invoked != 0);
    test_int(ctx.ctor.component, ecs_id(Position));
    test_int(ctx.ctor.size, sizeof(Position));
    test_int(ctx.ctor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_ctor_on_new() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    ecs_new(world, Position);
    test_assert(ctx.ctor.invoked != 0);
    test_int(ctx.ctor.component, ecs_id(Position));
    test_int(ctx.ctor.size, sizeof(Position));
    test_int(ctx.ctor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_dtor_on_remove() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .dtor = comp_dtor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, Position);
    test_int(ctx.dtor.invoked, 0);

    ecs_remove(world, e, Position);
    test_assert(ctx.dtor.invoked != 0);
    test_int(ctx.dtor.component, ecs_id(Position));
    test_int(ctx.dtor.size, sizeof(Position));
    test_int(ctx.dtor.count, 1);
    
    ecs_fini(world);
}

void ComponentLifecycle_dtor_on_delete() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .dtor = comp_dtor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, Position);
    test_int(ctx.dtor.invoked, 0);

    ecs_delete(world, e);
    test_assert(ctx.dtor.invoked != 0);
    test_int(ctx.dtor.component, ecs_id(Position));
    test_int(ctx.dtor.size, sizeof(Position));
    test_int(ctx.dtor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_set() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.copy.invoked, 0);
    
    ecs_set(world, e, Position, {0, 0});
    test_assert(ctx.copy.invoked != 0);
    test_int(ctx.copy.component, ecs_id(Position));
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_override() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new(world, Position);
    test_int(ctx.copy.invoked, 0);

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);
    test_int(ctx.copy.invoked, 0);

    ecs_add(world, e, Position);
    test_assert(ctx.copy.invoked != 0);
    test_int(ctx.copy.component, ecs_id(Position));
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_clone() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(ctx.copy.invoked != 0);
    memset(&ctx, 0, sizeof(ctx));

    ecs_clone(world, 0, e, true);
    test_assert(ctx.copy.invoked != 0);
    test_int(ctx.copy.component, ecs_id(Position));
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_no_copy_on_move() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
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
    test_quarantine("13 March 2022");
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    
    cl_ctx ctx = { { 0 } };
    ecs_set_hooks(world, Position, {
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
    test_int(ctx.copy.component, ecs_id(Position));
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 10);

    ctx = (cl_ctx){ { 0 } };

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    test_assert(ctx.copy.invoked != 0);
    test_int(ctx.copy.component, ecs_id(Position));
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 10);

    ecs_snapshot_restore(world, s);
    ecs_snapshot_free(s);

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
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    
    cl_ctx ctx = { { 0 } };
    ecs_set_hooks(world, Position, {
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
    test_int(ctx.copy.component, ecs_id(Position));
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 10);

    ctx = (cl_ctx){ { 0 } };

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    test_assert(ctx.ctor.invoked != 0);
    test_int(ctx.ctor.component, ecs_id(Position));
    test_int(ctx.ctor.size, sizeof(Position));
    test_int(ctx.ctor.count, 10);

    test_assert(ctx.copy.invoked != 0);
    test_int(ctx.copy.component, ecs_id(Position));
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
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    
    cl_ctx ctx = { { 0 } };
    ecs_set_hooks(world, Position, {
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

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    test_expect_abort();

    ecs_set_hooks_id(world, Tag, &(ecs_type_hooks_t){
        .ctor = comp_ctor
    });

    ecs_fini(world);
}

void ComponentLifecycle_dtor_on_tag() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    test_expect_abort();

    ecs_set_hooks_id(world, Tag, &(ecs_type_hooks_t){
        .dtor = comp_dtor
    });

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_tag() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    test_expect_abort();

    ecs_set_hooks_id(world, Tag, &(ecs_type_hooks_t){
        .copy = comp_copy
    });

    ecs_fini(world);
}

void ComponentLifecycle_move_on_tag() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    test_expect_abort();

    ecs_set_hooks_id(world, Tag, &(ecs_type_hooks_t){
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
})

static int dtor_position = 0;
static
ECS_DTOR(Position, ptr, {
    dtor_position ++;
})

static int copy_position = 0;
static
ECS_COPY(Position, dst, src, {
    copy_position ++;
})

static int move_position = 0;
static
ECS_MOVE(Position, dst, src, {
    move_position ++;
})

/* Velocity */

static int ctor_velocity = 0;
static
ECS_CTOR(Velocity, ptr, {
    ctor_velocity ++;
})

static int dtor_velocity = 0;
static
ECS_DTOR(Velocity, ptr, {
    dtor_velocity ++;
})

static int copy_velocity = 0;
static
ECS_COPY(Velocity, dst, src, {
    copy_velocity ++;
})

static int move_velocity = 0;
static
ECS_MOVE(Velocity, dst, src, {
    move_velocity ++;
})

/* Mass */

static int ctor_mass = 0;
static
ECS_CTOR(Mass, ptr, {
    ctor_mass ++;
})

static int dtor_mass = 0;
static
ECS_DTOR(Mass, ptr, {
    dtor_mass ++;
})

static int copy_mass = 0;
static
ECS_COPY(Mass, dst, src, {
    copy_mass ++;
})

static int move_mass = 0;
static
ECS_MOVE(Mass, dst, src, {
    move_mass ++;
})

/* Rotation */

static int ctor_rotation = 0;
static
ECS_CTOR(Rotation, ptr, {
    ctor_rotation ++;
})

static int dtor_rotation = 0;
static
ECS_DTOR(Rotation, ptr, {
    dtor_rotation ++;
})

static int copy_rotation = 0;
static
ECS_COPY(Rotation, dst, src, {
    copy_rotation ++;
})

static int move_rotation = 0;
static
ECS_MOVE(Rotation, dst, src, {
    move_rotation ++;
})

void ComponentLifecycle_merge_to_different_table() {
    ecs_world_t *world = ecs_mini();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .dtor = ecs_dtor(Position),
        .copy = ecs_copy(Position),
        .move = ecs_move(Position)
    });

    ecs_set_hooks(world, Velocity, {
        .ctor = ecs_ctor(Velocity),
        .dtor = ecs_dtor(Velocity),
        .copy = ecs_copy(Velocity),
        .move = ecs_move(Velocity)
    });   

    ecs_set_hooks(world, Mass, {
        .ctor = ecs_ctor(Mass),
        .dtor = ecs_dtor(Mass),
        .copy = ecs_copy(Mass),
        .move = ecs_move(Mass)
    });

    ecs_set_hooks(world, Rotation, {
        .ctor = ecs_ctor(Rotation),
        .dtor = ecs_dtor(Rotation),
        .copy = ecs_copy(Rotation),
        .move = ecs_move(Rotation)
    });

    ECS_ENTITY(world, e, Position, Velocity, Rotation);

    ctor_position = 0;
    dtor_position = 0;
    copy_position = 0;
    move_position = 0;

    ctor_velocity = 0;
    dtor_velocity = 0;
    copy_velocity = 0;
    move_velocity = 0;

    ctor_rotation = 0;
    dtor_rotation = 0;
    copy_rotation = 0;
    move_rotation = 0;

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
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);

    ecs_set_hooks(world, Position, {
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
    ecs_world_t *world = ecs_mini();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .dtor = ecs_dtor(Position),
        .copy = ecs_copy(Position),
        .move = ecs_move(Position)
    });

    ecs_set_hooks(world, Velocity, {
        .ctor = ecs_ctor(Velocity),
        .dtor = ecs_dtor(Velocity),
        .copy = ecs_copy(Velocity),
        .move = ecs_move(Velocity)
    });   

    ecs_set_hooks(world, Mass, {
        .ctor = ecs_ctor(Mass),
        .dtor = ecs_dtor(Mass),
        .copy = ecs_copy(Mass),
        .move = ecs_move(Mass)
    });

    ECS_ENTITY(world, e, Position, Velocity, Mass);

    ctor_position = 0;
    dtor_position = 0;
    copy_position = 0;
    move_position = 0;

    ctor_velocity = 0;
    dtor_velocity = 0;
    copy_velocity = 0;
    move_velocity = 0;

    ctor_mass = 0;
    dtor_mass = 0;
    copy_mass = 0;
    move_mass = 0;

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
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Pair);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Pair, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.ctor.invoked, 0);

    ecs_add_pair(world, e, ecs_id(Pair), ecs_id(Position));
    test_assert(ctx.ctor.invoked != 0);
    test_int(ctx.ctor.component, ecs_id(Pair));
    test_int(ctx.ctor.size, sizeof(Pair));
    test_int(ctx.ctor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_ctor_on_add_pair_tag() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Pair);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.ctor.invoked, 0);

    ecs_add_pair(world, e, Pair, ecs_id(Position));

    test_assert(ctx.ctor.invoked != 0);
    test_int(ctx.ctor.component, ecs_id(Position));
    test_int(ctx.ctor.size, sizeof(Position));
    test_int(ctx.ctor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_ctor_on_move_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Pair);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Pair, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    /* Create entity in existing table */
    ecs_entity_t e = ecs_new(world, Position);
    test_int(ctx.ctor.invoked, 0);

    /* Add pair to existing table */
    ecs_add_pair(world, e, ecs_id(Pair), ecs_id(Position));

    test_assert(ctx.ctor.invoked != 0);
    test_int(ctx.ctor.component, ecs_id(Pair));
    test_int(ctx.ctor.size, sizeof(Pair));
    test_int(ctx.ctor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_move_on_realloc() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .ctor = comp_ctor,
        .move = comp_move,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.ctor.invoked, 0);

    ecs_add(world, e, Position);
    test_assert(ctx.ctor.invoked != 0);
    test_int(ctx.move.invoked, 0);
    test_int(ctx.ctor.component, ecs_id(Position));
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
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .ctor = comp_ctor,
        .move = comp_move,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.ctor.invoked, 0);

    ecs_add(world, e, Position);
    test_assert(ctx.ctor.invoked != 0);
    test_int(ctx.move.invoked, 0);
    test_int(ctx.ctor.component, ecs_id(Position));
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
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .ctor = comp_ctor,
        .move = comp_move,
        .ctx = &ctx
    });

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_new(world, Position);
    test_assert(ctx.ctor.invoked != 0);

    ctx = (cl_ctx){ { 0 } };

    ecs_delete(world, e1); /* Should trigger move of e2 */

    test_int(ctx.ctor.invoked, 0);
    test_assert(ctx.move.invoked != 0);
    test_int(ctx.move.component, ecs_id(Position));
    test_int(ctx.move.size, sizeof(Position));
    test_int(ctx.move.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_move_dtor_on_delete() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .dtor = comp_dtor,
        .move = comp_move,
        .ctx = &ctx
    });

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_new(world, Position);

    ctx = (cl_ctx){ { 0 } };

    ecs_delete(world, e1); /* Should trigger move of e2 */

    test_assert(ctx.dtor.invoked != 0);
    test_assert(ctx.move.invoked != 0);

    test_int(ctx.dtor.component, ecs_id(Position));
    test_int(ctx.dtor.size, sizeof(Position));
    test_int(ctx.dtor.count, 1);

    test_int(ctx.move.component, ecs_id(Position));
    test_int(ctx.move.size, sizeof(Position));
    test_int(ctx.move.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_override_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Pair);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Pair, {
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
    test_int(ctx.copy.component, ecs_id(Pair));
    test_int(ctx.copy.size, sizeof(Pair));
    test_int(ctx.copy.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_override_pair_tag() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Pair);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
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
    test_int(ctx.copy.component, ecs_id(Position));
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_set_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Pair);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Pair, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.copy.invoked, 0);
    
    ecs_set_pair(world, e, Pair, ecs_id(Position), {0, 0});
    test_assert(ctx.copy.invoked != 0);
    test_int(ctx.copy.component, ecs_id(Pair));
    test_int(ctx.copy.size, sizeof(Pair));
    test_int(ctx.copy.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_set_pair_tag() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Pair);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.copy.invoked, 0);
    
    ecs_set_pair_object(world, e, Pair, Position, {0, 0});
    test_assert(ctx.copy.invoked != 0);
    test_int(ctx.copy.component, ecs_id(Position));
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_allow_lifecycle_overwrite_equal_callbacks() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position)
    });

    /* Same actions, should be ok */
    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position)
    });

    ecs_new(world, Position);

    test_int(ctor_position, 1);

    ecs_fini(world); 
}

static
void AddPosition(ecs_iter_t *it) { }

void ComponentLifecycle_set_lifecycle_after_trigger() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, AddPosition, EcsOnAdd, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position)
    });

    ecs_new(world, Position);

    test_int(ctor_position, 1);

    ecs_fini(world);  
}

static int dummy_dtor_invoked = 0;

typedef struct Dummy {
    ecs_world_t *world;
    ecs_entity_t e;
    int value;
} Dummy;

static
void dummy_dtor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *info)
{
    Dummy *d = ptr;

    int i;
    for (i = 0; i < count; i ++) {
        test_assert(ecs_is_valid(d[i].world, d[i].e));
    }

    dummy_dtor_invoked ++;
}

void ComponentLifecycle_valid_entity_in_dtor_after_delete() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Dummy);

    ecs_set_hooks(world, Dummy, {
        .dtor = dummy_dtor
    });

    ecs_entity_t e = ecs_new(world, Dummy);
    test_assert(e != 0);
    ecs_set(world, e, Dummy, {world, e, 0});

    ecs_delete(world, e);
    test_assert(!ecs_is_valid(world, e));
    test_assert(!ecs_is_alive(world, e));

    test_int(dummy_dtor_invoked, 1);

    ecs_fini(world);  
}

void ComponentLifecycle_ctor_w_emplace() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
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
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Dummy);

    ecs_set_hooks(world, Dummy, {
        .dtor = dummy_dtor
    });

    ecs_entity_t e = ecs_new(world, Dummy);
    test_assert(e != 0);

    ecs_set(world, e, Dummy, {world, e});

    test_int(dummy_dtor_invoked, 0);

    ecs_fini(world); 

    test_int(dummy_dtor_invoked, 1);
}

static
void type_dtor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *info)
{
    test_int(count, 1);

    Dummy *d = ptr;

    ecs_world_t *world = d->world;
    ecs_entity_t e = d->e;
    test_assert(world != NULL);
    test_assert(e != 0);

    const ecs_type_t *type = ecs_get_type(world, e);
    test_assert(type != NULL);

    test_int(type->count, 3);

    dummy_dtor_invoked = 1;
}

typedef struct {
    ecs_world_t *world;
    ecs_entity_t e;
    ecs_entity_t other;
} Entity;

typedef struct {
    ecs_world_t *world;
    ecs_entity_t e;
    char *str;
} String;

static int other_dtor_invoked;
static int other_dtor_valid_entity;

static
void other_type_dtor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *info)
{
    test_int(count, 1);
    test_assert(ptr != NULL);

    Entity *comp = ptr;
    ecs_world_t *world = comp->world;
    ecs_entity_t e = comp->e;
    test_assert(e != 0);
    test_assert(world != NULL);
    test_assert(ecs_is_valid(world, e));

    test_assert(comp->other != 0);
    
    if (ecs_is_valid(world, comp->other)) {
        const ecs_type_t *type = ecs_get_type(world, comp->other);
        test_assert(type != NULL);
        test_int(type->count, 2);
        other_dtor_valid_entity ++;
    }

    other_dtor_invoked ++;
}

static
void other_comp_dtor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *info)
{
    test_int(count, 1);
    test_assert(ptr != NULL);

    Entity *comp = ptr;

    ecs_world_t *world = comp->world;
    ecs_entity_t e = comp->e;
    test_assert(e != 0);
    test_assert(ecs_is_valid(world, e));

    test_assert(comp->other != 0);

    if (ecs_is_valid(world, comp->other)) {
        if (ecs_has(world, comp->other, String)) {
            const String *str_ptr = ecs_get(world, comp->other, String);
            test_assert(str_ptr != NULL);
            test_assert(str_ptr->str != NULL);
            test_str(str_ptr->str, "Foo");
            other_dtor_valid_entity ++;
        } else {
            test_assert(ecs_get(world, comp->other, String) == NULL);
        }
    }

    other_dtor_invoked ++;
}

static
void other_delete_dtor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *info)
{
    test_int(count, 1);
    test_assert(ptr != NULL);

    Entity *comp = ptr;

    ecs_world_t *world = comp->world;
    ecs_entity_t e = comp->e;
    test_assert(e != 0);
    test_assert(ecs_is_valid(world, e));

    test_assert(comp->other != 0);
    
    if (ecs_is_valid(world, comp->other)) {
        ecs_delete(world, comp->other);
        other_dtor_valid_entity ++;

        test_assert(ecs_is_valid(world, e));
        test_assert(ecs_get(world, e, Entity) == comp);
        test_assert(ecs_is_valid(world, comp->other));
    }

    other_dtor_invoked ++;
}

static
void self_delete_dtor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *info)
{
    test_int(count, 1);
    test_assert(ptr != NULL);

    Dummy *d = ptr;
    ecs_world_t *world = d->world;
    ecs_entity_t e = d->e;
    test_assert(world != 0);
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
    void *ptr,
    int32_t count,
    const ecs_type_info_t *info)
{
    test_int(count, 1);
    test_assert(ptr != NULL);

    String *comp = ptr;
    ecs_world_t *world = comp->world;
    ecs_entity_t e = comp->e;
    test_assert(e != 0);
    test_assert(ecs_is_valid(world, e));

    test_assert(comp->str != NULL);
    ecs_os_free(comp->str);
}

void ComponentLifecycle_valid_type_in_dtor_on_fini() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Dummy);

    ecs_set_hooks(world, Dummy, {
        .dtor = type_dtor
    });

    ecs_entity_t e = ecs_new(world, Dummy);
    test_assert(e != 0);
    ecs_set(world, e, Dummy, {world, e});
    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);

    test_int(dummy_dtor_invoked, 0);

    ecs_fini(world); 

    test_int(dummy_dtor_invoked, 1);
}

void ComponentLifecycle_valid_other_type_of_entity_in_dtor_on_fini() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT_DEFINE(world, Entity);

    ecs_set_hooks(world, Entity, {
        .dtor = other_type_dtor
    });

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_set(world, e2, Entity, {world, e2, e1});

    ecs_add(world, e1, Velocity);
    ecs_set(world, e1, Entity, {world, e1, e2});

    test_int(other_dtor_invoked, 0);

    ecs_fini(world);

    test_int(other_dtor_invoked, 2);
    test_int(other_dtor_valid_entity, 1);
}

void ComponentLifecycle_delete_in_dtor_other_type_on_fini() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, String);
    ECS_COMPONENT_DEFINE(world, Entity);
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_set_hooks(world, String, {
        .dtor = string_dtor
    });

    ecs_set_hooks(world, Entity, {
        .dtor = other_delete_dtor
    });

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Velocity);

    ecs_set(world, e2, Entity, {world, e2, e1});
    ecs_set(world, e1, Entity, {world, e1, e2});

    ecs_set(world, e1, String, {world, e1, ecs_os_strdup("Foo")});
    ecs_set(world, e2, String, {world, e2, ecs_os_strdup("Foo")});

    test_int(other_dtor_invoked, 0);

    ecs_fini(world);

    test_int(other_dtor_invoked, 2);
    test_int(other_dtor_valid_entity, 1);
}

void ComponentLifecycle_delete_in_dtor_other_type_on_delete_parent() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, String);
    ECS_COMPONENT_DEFINE(world, Entity);
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_set_hooks(world, String, {
        .dtor = string_dtor
    });

    ecs_set_hooks(world, Entity, {
        .dtor = other_delete_dtor
    });

    ecs_entity_t parent = ecs_new_id(world);

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Velocity);

    ecs_add_pair(world, e1, EcsChildOf, parent);
    ecs_add_pair(world, e2, EcsChildOf, parent);

    ecs_set(world, e2, Entity, {world, e2, e1});
    ecs_set(world, e1, Entity, {world, e1, e2});

    ecs_set(world, e1, String, {world, e1, ecs_os_strdup("Foo")});
    ecs_set(world, e2, String, {world, e2, ecs_os_strdup("Foo")});

    test_int(other_dtor_invoked, 0);

    ecs_delete(world, parent);

    test_int(other_dtor_invoked, 2);
    test_int(other_dtor_valid_entity, 1);

    test_assert(!ecs_is_alive(world, e1));
    test_assert(!ecs_is_alive(world, e2));    

    ecs_fini(world);
}

void ComponentLifecycle_delete_in_dtor_same_type_on_delete() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, String);
    ECS_COMPONENT_DEFINE(world, Entity);

    ecs_set_hooks(world, String, {
        .dtor = string_dtor
    });

    ecs_set_hooks(world, Entity, {
        .dtor = other_delete_dtor
    });

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);

    ecs_set(world, e1, Entity, {world, e1, e2});
    ecs_set(world, e2, Entity, {world, e2, e3});
    ecs_set(world, e3, Entity, {world, e3, e1});

    ecs_set(world, e1, String, {world, e1, ecs_os_strdup("Foo")});
    ecs_set(world, e2, String, {world, e2, ecs_os_strdup("Foo")});
    ecs_set(world, e3, String, {world, e3, ecs_os_strdup("Foo")});

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
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, String);
    ECS_COMPONENT_DEFINE(world, Entity);
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_set_hooks(world, String, {
        .dtor = string_dtor
    });

    ecs_set_hooks(world, Entity, {
        .dtor = other_delete_dtor
    });

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Velocity);

    ecs_set(world, e2, Entity, {world, e2, e1});
    ecs_set(world, e1, Entity, {world, e1, e2});

    ecs_set(world, e1, String, {world, e1, ecs_os_strdup("Foo")});
    ecs_set(world, e2, String, {world, e2, ecs_os_strdup("Foo")});

    test_int(other_dtor_invoked, 0);

    ecs_delete(world, e1);
    ecs_delete(world, e2);

    test_int(other_dtor_invoked, 2);
    test_int(other_dtor_valid_entity, 1);

    ecs_fini(world);
}

void ComponentLifecycle_delete_self_in_dtor_on_delete() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Dummy);

    ecs_set_hooks(world, Dummy, {
        .dtor = self_delete_dtor
    });

    ecs_entity_t e1 = ecs_new(world, Dummy);
    ecs_entity_t e2 = ecs_new(world, Dummy);
    ecs_entity_t e3 = ecs_new(world, Dummy);

    ecs_set(world, e1, Dummy, {world, e1});
    ecs_set(world, e2, Dummy, {world, e2});
    ecs_set(world, e3, Dummy, {world, e3});

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
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
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

static int on_add_position = 0;

static void ecs_on_add(Position)(ecs_iter_t *it) {
    test_assert(it->count >= 1);
    test_assert(it->event == EcsOnAdd);

    Position *p = ecs_term(it, Position, 1);
    for (int i = 0; i < it->count; i ++) {
        on_add_position ++;
        test_int(p[i].x, 0);
        test_int(p[i].y, 0);
    }
}

void ComponentLifecycle_on_add_after_new() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_default_ctor,
        .on_add = ecs_on_add(Position)
    });

    ecs_new(world, Position);
    test_int(on_add_position, 1);
    ecs_new(world, Position);
    test_int(on_add_position, 2);
    ecs_new(world, Position);
    test_int(on_add_position, 3);

    ecs_fini(world);
}

void ComponentLifecycle_on_add_after_add() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_default_ctor,
        .on_add = ecs_on_add(Position)
    });

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);

    ecs_add(world, e1, Position);
    test_int(on_add_position, 1);
    ecs_add(world, e2, Position);
    test_int(on_add_position, 2);
    ecs_add(world, e3, Position);
    test_int(on_add_position, 3);

    ecs_fini(world);
}

void ComponentLifecycle_on_add_after_set() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_default_ctor,
        .on_add = ecs_on_add(Position)
    });

    ecs_set(world, 0, Position, {10, 20});
    test_int(on_add_position, 1);
    ecs_set(world, 0, Position, {10, 20});
    test_int(on_add_position, 2);
    ecs_set(world, 0, Position, {10, 20});
    test_int(on_add_position, 3);

    ecs_fini(world);
}

static int on_remove_position = 0;

static void ecs_on_remove(Position)(ecs_iter_t *it) {
    test_assert(it->count >= 1);
    test_assert(it->event == EcsOnRemove);

    Position *p = ecs_term(it, Position, 1);
    for (int i = 0; i < it->count; i ++) {
        on_remove_position ++;
        test_int(p[i].x, 10);
        test_int(p[i].y, 20);
    }
}

void ComponentLifecycle_on_remove_after_remove() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_remove = ecs_on_remove(Position)
    });

    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {10, 20});

    ecs_remove(world, e1, Position);
    test_int(on_remove_position, 1);
    ecs_remove(world, e2, Position);
    test_int(on_remove_position, 2);
    ecs_remove(world, e3, Position);
    test_int(on_remove_position, 3);

    ecs_fini(world);
}

void ComponentLifecycle_on_remove_after_clear() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_remove = ecs_on_remove(Position)
    });

    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {10, 20});

    ecs_clear(world, e1);
    test_int(on_remove_position, 1);
    ecs_clear(world, e2);
    test_int(on_remove_position, 2);
    ecs_clear(world, e3);
    test_int(on_remove_position, 3);

    ecs_fini(world);
}

void ComponentLifecycle_on_remove_after_delete() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_remove = ecs_on_remove(Position)
    });

    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {10, 20});

    ecs_delete(world, e1);
    test_int(on_remove_position, 1);
    ecs_delete(world, e2);
    test_int(on_remove_position, 2);
    ecs_delete(world, e3);
    test_int(on_remove_position, 3);

    ecs_fini(world);
}

static int on_remove_tag_set_position_invoked = 0;

static
void on_remove_tag_set_position(ecs_iter_t *it) {
    for (int i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], Position, {10, 20});
        on_remove_tag_set_position_invoked ++;
    }
}

static
void on_remove_tag_set_position_pair(ecs_iter_t *it) {
    for (int i = 0; i < it->count; i ++) {
        ecs_set_pair(it->world, it->entities[i], 
            Position, ecs_new_id(it->world), {10, 20});
        on_remove_tag_set_position_invoked ++;
    }
}

static
void on_remove_tag_set_position_obj_pair(ecs_iter_t *it) {
    for (int i = 0; i < it->count; i ++) {
        ecs_set_pair_object(it->world, it->entities[i], 
            ecs_new_id(it->world), Position, {10, 20});
        on_remove_tag_set_position_invoked ++;
    }
}

void ComponentLifecycle_free_component_new_id_while_fini() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG(world, Tag);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = Tag,
        .events = {EcsOnRemove},
        .callback = on_remove_tag_set_position
    });

    ecs_new(world, Tag);

    ecs_fini(world);

    test_int(on_remove_tag_set_position_invoked, 1);
}

void ComponentLifecycle_dtor_component_new_id_while_fini() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG(world, Tag);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = Tag,
        .events = {EcsOnRemove},
        .callback = on_remove_tag_set_position
    });

    ecs_set_hooks(world, Position, {
        .ctor = ecs_default_ctor,
        .dtor = ecs_dtor(Position)
    });

    ecs_new(world, Tag);

    test_int(dtor_position, 0);

    ecs_fini(world);

    test_int(on_remove_tag_set_position_invoked, 1);
    test_int(dtor_position, 1);
}

void ComponentLifecycle_free_component_new_pair_id_while_fini() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG(world, Tag);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = Tag,
        .events = {EcsOnRemove},
        .callback = on_remove_tag_set_position_pair
    });

    ecs_new(world, Tag);

    ecs_fini(world);

    test_int(on_remove_tag_set_position_invoked, 1);
}

void ComponentLifecycle_dtor_component_new_pair_id_while_fini() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG(world, Tag);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = Tag,
        .events = {EcsOnRemove},
        .callback = on_remove_tag_set_position_pair
    });

    ecs_set_hooks(world, Position, {
        .ctor = ecs_default_ctor,
        .dtor = ecs_dtor(Position)
    });

    ecs_new(world, Tag);

    test_int(dtor_position, 0);

    ecs_fini(world);

    test_int(on_remove_tag_set_position_invoked, 1);
    test_int(dtor_position, 1);
}

void ComponentLifecycle_free_component_new_obj_pair_id_while_fini() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG(world, Tag);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = Tag,
        .events = {EcsOnRemove},
        .callback = on_remove_tag_set_position_obj_pair
    });

    ecs_new(world, Tag);

    ecs_fini(world);

    test_int(on_remove_tag_set_position_invoked, 1);
}

void ComponentLifecycle_dtor_component_new_obj_pair_id_while_fini() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG(world, Tag);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = Tag,
        .events = {EcsOnRemove},
        .callback = on_remove_tag_set_position_obj_pair
    });

    ecs_set_hooks(world, Position, {
        .ctor = ecs_default_ctor,
        .dtor = ecs_dtor(Position)
    });

    ecs_new(world, Tag);

    test_int(dtor_position, 0);

    ecs_fini(world);

    test_int(on_remove_tag_set_position_invoked, 1);
    test_int(dtor_position, 1);
}

void ComponentLifecycle_ctor_move_dtor_after_resize() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .ctor = comp_ctor,
        .copy = comp_copy,
        .move = comp_move,
        .dtor = comp_dtor,
        .ctx = &ctx
    });

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);

    ecs_add(world, e1, Position);
    test_int(ctx.ctor.count, 1);
    test_int(ctx.copy.count, 0);
    test_int(ctx.move.count, 0);
    test_int(ctx.dtor.count, 0);

    ecs_os_zeromem(&ctx);

    ecs_add(world, e2, Position);
    test_int(ctx.ctor.count, 1);
    test_int(ctx.copy.count, 0);
    test_int(ctx.move.count, 0);
    test_int(ctx.dtor.count, 0);

    ecs_os_zeromem(&ctx);

    ecs_add(world, e3, Position);
    test_int(ctx.ctor.count, 3);
    test_int(ctx.copy.count, 0);
    test_int(ctx.move.count, 2);
    test_int(ctx.dtor.count, 2);

    ecs_fini(world);
}

static int component_lifecycle_ctx = 0;
static int component_lifecycle_binding_ctx = 0;

static void component_lifecycle_ctx_free(void *ctx) {
    test_assert(ctx == &component_lifecycle_ctx);
    component_lifecycle_ctx ++;
}

void ComponentLifecycle_ctx_free() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctx = &component_lifecycle_ctx,
        .ctx_free = component_lifecycle_ctx_free
    });

    ecs_fini(world);

    test_int(1, component_lifecycle_ctx);
}

void ComponentLifecycle_binding_ctx_free() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .binding_ctx = &component_lifecycle_ctx,
        .binding_ctx_free = component_lifecycle_ctx_free
    });

    ecs_fini(world);

    test_int(1, component_lifecycle_ctx);
}

void ComponentLifecycle_ctx_free_after_delete_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctx = &component_lifecycle_ctx,
        .ctx_free = component_lifecycle_ctx_free
    });

    ecs_remove_pair(world, ecs_id(Position), EcsOnDelete, EcsPanic);
    ecs_delete(world, ecs_id(Position));

    test_int(1, component_lifecycle_ctx);

    ecs_fini(world);
}

void ComponentLifecycle_binding_ctx_free_after_delete_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .binding_ctx = &component_lifecycle_ctx,
        .binding_ctx_free = component_lifecycle_ctx_free
    });

    ecs_remove_pair(world, ecs_id(Position), EcsOnDelete, EcsPanic);
    ecs_delete(world, ecs_id(Position));

    test_int(1, component_lifecycle_ctx);

    ecs_fini(world);
}

static void test_lifecycle_ctx(ecs_iter_t *it) {
    test_assert(it->ctx == &component_lifecycle_ctx);
    test_assert(it->binding_ctx == &component_lifecycle_binding_ctx);
    component_lifecycle_ctx ++;
    component_lifecycle_binding_ctx ++;
}

void ComponentLifecycle_on_add_ctx() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_add = test_lifecycle_ctx,
        .ctx = &component_lifecycle_ctx,
        .binding_ctx = &component_lifecycle_binding_ctx
    });

    ecs_new(world, Position);
    test_int(1, component_lifecycle_ctx);
    test_int(1, component_lifecycle_binding_ctx);

    ecs_fini(world);

    test_int(1, component_lifecycle_ctx);
    test_int(1, component_lifecycle_binding_ctx);
}

void ComponentLifecycle_on_remove_ctx() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_remove = test_lifecycle_ctx,
        .ctx = &component_lifecycle_ctx,
        .binding_ctx = &component_lifecycle_binding_ctx
    });

    ecs_entity_t e = ecs_new(world, Position);
    test_int(0, component_lifecycle_ctx);
    test_int(0, component_lifecycle_binding_ctx);

    ecs_remove(world, e, Position);
    test_int(1, component_lifecycle_ctx);
    test_int(1, component_lifecycle_binding_ctx);

    ecs_fini(world);

    test_int(1, component_lifecycle_ctx);
    test_int(1, component_lifecycle_binding_ctx);
}

void ComponentLifecycle_on_set_ctx() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_set = test_lifecycle_ctx,
        .ctx = &component_lifecycle_ctx,
        .binding_ctx = &component_lifecycle_binding_ctx
    });

    ecs_entity_t e = ecs_new(world, Position);
    test_int(0, component_lifecycle_ctx);
    test_int(0, component_lifecycle_binding_ctx);

    ecs_set(world, e, Position, {10, 20});
    test_int(1, component_lifecycle_ctx);
    test_int(1, component_lifecycle_binding_ctx);

    ecs_fini(world);

    test_int(1, component_lifecycle_ctx);
    test_int(1, component_lifecycle_binding_ctx);
}

static int test_on_event_invoked = 0;

static void test_on_event(ecs_iter_t *it) {
    test_assert(it->ctx != NULL);
    test_assert(*(ecs_entity_t*)it->ctx == it->event);
    test_on_event_invoked ++;
}

void ComponentLifecycle_on_add_w_existing_component() {
    ecs_world_t* world = ecs_mini();

    ECS_TAG(world, Tag);

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_add = test_on_event,
        .ctx = (void*)&EcsOnAdd
    });

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_add(world, e, Position);

    test_int(1, test_on_event_invoked);

    ecs_fini(world);
}

void ComponentLifecycle_on_remove_w_existing_component() {
    ecs_world_t* world = ecs_mini();

    ECS_TAG(world, Tag);

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_remove = test_on_event,
        .ctx = (void*)&EcsOnRemove
    });

    ecs_entity_t e = ecs_new_entity(world, "Foo");
    ecs_add(world, e, Position);

    test_int(0, test_on_event_invoked);
    
    ecs_remove(world, e, Position);

    test_int(1, test_on_event_invoked);

    ecs_fini(world);
}

static int on_add_count = 0;
static int on_remove_count = 0;

static void test_on_add(ecs_iter_t *it) {
    on_add_count ++;
}

static void test_on_remove(ecs_iter_t *it) {
    on_remove_count ++;
}

void ComponentLifecycle_component_init_set_hooks() {
    ecs_world_t* world = ecs_mini();

    ecs_entity_t c = ecs_component_init(world, &(ecs_component_desc_t){
        .entity.name = "Position",
        .type = {
            .size = ECS_SIZEOF(Position),
            .alignment = ECS_ALIGNOF(Position),
            .hooks = {
                .on_add = test_on_add,
                .on_remove = test_on_remove
            }
        }
    });

    const ecs_type_info_t *ti = ecs_get_type_info(world, c);
    test_assert(ti != NULL);
    test_uint(ti->size, ECS_SIZEOF(Position));
    test_uint(ti->alignment, ECS_ALIGNOF(Position));
    test_assert(ti->hooks.on_add == test_on_add);
    test_assert(ti->hooks.on_remove == test_on_remove);

    test_int(0, on_add_count);
    test_int(0, on_remove_count);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_id(world, e, c);

    test_int(1, on_add_count);
    test_int(0, on_remove_count);

    ecs_remove_id(world, e, c);

    test_int(1, on_add_count);
    test_int(1, on_remove_count);
    
    ecs_fini(world);

    test_int(1, on_add_count);
    test_int(1, on_remove_count);
}

static int ctor_before_on_add_count = 0;
static int on_add_after_ctor_count = 0;

static void ctor_before_on_add(
    void *ptr, int32_t count, const ecs_type_info_t *info)
{
    test_int(0, on_add_after_ctor_count);
    ctor_before_on_add_count ++;
}

static void on_add_after_ctor(ecs_iter_t *it)
{
    test_int(1, ctor_before_on_add_count);
    on_add_after_ctor_count ++;
}

void ComponentLifecycle_on_add_after_ctor_w_add() {
    ecs_world_t* world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ctor_before_on_add,
        .on_add = on_add_after_ctor
    });

    test_int(0, ctor_before_on_add_count);
    test_int(0, on_add_after_ctor_count);

    ecs_new(world, Position);

    test_int(1, ctor_before_on_add_count);
    test_int(1, on_add_after_ctor_count);

    ecs_fini(world);
}

void ComponentLifecycle_on_add_after_ctor_w_add_to() {
    ecs_world_t* world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_set_hooks(world, Position, {
        .ctor = ctor_before_on_add,
        .on_add = on_add_after_ctor
    });

    test_int(0, ctor_before_on_add_count);
    test_int(0, on_add_after_ctor_count);

    ecs_entity_t e = ecs_new(world, Tag);

    test_int(0, ctor_before_on_add_count);
    test_int(0, on_add_after_ctor_count);

    ecs_add(world, e, Position);

    test_int(1, ctor_before_on_add_count);
    test_int(1, on_add_after_ctor_count);

    ecs_fini(world);
}

void ComponentLifecycle_with_before_hooks() {
    ecs_world_t* world = ecs_mini();

    ecs_entity_t pos_id = ecs_new_id(world);
    ecs_entity_t tag = ecs_new_w_pair(world, EcsWith, pos_id);

    ecs_entity_t ecs_id(Position) = 
        ecs_component_init(world, &(ecs_component_desc_t){
            .entity.entity = pos_id,
            .type = {
                .size = ECS_SIZEOF(Position),
                .alignment = ECS_ALIGNOF(Position),
                .hooks = {
                    .ctor = ecs_ctor(Position)
                }
            }
        });

    test_assert(ecs_id(Position) == pos_id);

    ecs_entity_t e = ecs_new_w_id(world, tag);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has_id(world, e, tag));
    test_assert(ecs_get(world, e, Position) != NULL);
    test_int(ctor_position, 1);

    const ecs_type_info_t *ti = ecs_get_type_info(world, ecs_id(Position));
    test_assert(ti != NULL);
    test_assert(ti->hooks.ctor == ecs_ctor(Position));

    ecs_fini(world);
}
