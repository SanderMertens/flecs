#include <core.h>

static ECS_COMPONENT_DECLARE(Position);
static ECS_COMPONENT_DECLARE(String);
static ECS_COMPONENT_DECLARE(Entity);

void ComponentLifecycle_setup(void) {
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
    copy_ctx ctor_move_dtor;
    copy_ctx move_dtor;

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

static
void comp_move_dtor(
    void *dst_ptr,
    void *src_ptr,
    int32_t count,
    const ecs_type_info_t *info)
{
    cl_ctx *data = info->hooks.ctx;
    data->move_dtor.component = info->component;
    data->move_dtor.size = info->size;
    data->move_dtor.count += count;
    data->move_dtor.invoked ++;

    memcpy(dst_ptr, src_ptr, info->size * count);
}

static
void comp_pos_ctor_move_dtor(void *dst_ptr, void *src_ptr,
    int32_t count, const ecs_type_info_t *info)
{
    cl_ctx *data = info->hooks.ctx;
    data->ctor_move_dtor.component = info->component;
    data->ctor_move_dtor.size = info->size;
    data->ctor_move_dtor.count += count;
    data->ctor_move_dtor.invoked ++;

    Position *p = src_ptr;
    int i;
    for (i = 0; i < count; i ++) {
        p[i].x = 10;
        p[i].y = 20;
    }

    memcpy(dst_ptr, src_ptr, info->size * count);
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

static int on_add_position = 0;

static void ecs_on_add(Position)(ecs_iter_t *it) {
    test_assert(it->count >= 1);
    test_assert(it->event == EcsOnAdd);

    Position *p = ecs_field(it, Position, 0);
    for (int i = 0; i < it->count; i ++) {
        on_add_position ++;
        test_int(p[i].x, 0);
        test_int(p[i].y, 0);
    }
}

static void on_add_position_emplace(ecs_iter_t *it) {
    on_add_position += it->count;
}

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

void ComponentLifecycle_ctor_on_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world);
    test_int(ctx.ctor.invoked, 0);

    ecs_add(world, e, Position);
    test_assert(ctx.ctor.invoked != 0);
    test_int(ctx.ctor.component, ecs_id(Position));
    test_int(ctx.ctor.size, sizeof(Position));
    test_int(ctx.ctor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_ctor_on_new(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    ecs_new_w(world, Position);
    test_assert(ctx.ctor.invoked != 0);
    test_int(ctx.ctor.component, ecs_id(Position));
    test_int(ctx.ctor.size, sizeof(Position));
    test_int(ctx.ctor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_dtor_on_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .dtor = comp_dtor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new_w(world, Position);
    test_int(ctx.dtor.invoked, 0);

    ecs_remove(world, e, Position);
    test_assert(ctx.dtor.invoked != 0);
    test_int(ctx.dtor.component, ecs_id(Position));
    test_int(ctx.dtor.size, sizeof(Position));
    test_int(ctx.dtor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_dtor_on_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .dtor = comp_dtor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new_w(world, Position);
    test_int(ctx.dtor.invoked, 0);

    ecs_delete(world, e);
    test_assert(ctx.dtor.invoked != 0);
    test_int(ctx.dtor.component, ecs_id(Position));
    test_int(ctx.dtor.size, sizeof(Position));
    test_int(ctx.dtor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world);
    test_int(ctx.copy.invoked, 0);

    ecs_set(world, e, Position, {0, 0});
    test_assert(ctx.copy.invoked != 0);
    test_int(ctx.copy.component, ecs_id(Position));
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_override(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new_w(world, Position);
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

void ComponentLifecycle_copy_on_clone(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    test_assert(ctx.copy.invoked != 0);
    memset(&ctx, 0, sizeof(ctx));

    ecs_clone(world, 0, e, true);
    test_assert(ctx.copy.invoked != 0);
    test_int(ctx.copy.component, ecs_id(Position));
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_no_copy_on_move(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    test_assert(ctx.copy.invoked != 0);
    memset(&ctx, 0, sizeof(ctx));

    ecs_add(world, e, Velocity);
    test_int(ctx.copy.invoked, 0);

    ecs_fini(world);
}

void ComponentLifecycle_ctor_on_tag(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    test_expect_abort();

    ecs_set_hooks_id(world, Tag, &(ecs_type_hooks_t){
        .ctor = comp_ctor
    });

    ecs_fini(world);
}

void ComponentLifecycle_dtor_on_tag(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    test_expect_abort();

    ecs_set_hooks_id(world, Tag, &(ecs_type_hooks_t){
        .dtor = comp_dtor
    });

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_tag(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    test_expect_abort();

    ecs_set_hooks_id(world, Tag, &(ecs_type_hooks_t){
        .copy = comp_copy
    });

    ecs_fini(world);
}

void ComponentLifecycle_move_on_tag(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    test_expect_abort();

    ecs_set_hooks_id(world, Tag, &(ecs_type_hooks_t){
        .move = comp_move
    });

    ecs_fini(world);
}

void ComponentLifecycle_merge_to_different_table(void) {
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

    test_assert(ctor_velocity !=  0);
    test_assert(dtor_velocity != 0);
    test_int(copy_velocity, 0);
    test_assert(move_velocity != 0);

    test_assert(ctor_rotation == 0);
    test_assert(dtor_rotation != 0);
    test_int(copy_rotation, 0);
    test_assert(move_rotation == 0);

    test_assert(ctor_mass != 0); // got added, moved once
    test_assert(dtor_mass == 0);
    test_int(copy_mass, 0);
    test_assert(move_mass == 0);

    ecs_fini(world);
}

void ComponentLifecycle_merge_to_new_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world);

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

void ComponentLifecycle_delete_in_stage(void) {
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

void ComponentLifecycle_ctor_on_add_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Pair);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Pair, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world);
    test_int(ctx.ctor.invoked, 0);

    ecs_add_pair(world, e, ecs_id(Pair), ecs_id(Position));
    test_assert(ctx.ctor.invoked != 0);
    test_int(ctx.ctor.component, ecs_id(Pair));
    test_int(ctx.ctor.size, sizeof(Pair));
    test_int(ctx.ctor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_ctor_on_add_pair_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Pair);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world);
    test_int(ctx.ctor.invoked, 0);

    ecs_add_pair(world, e, Pair, ecs_id(Position));

    test_assert(ctx.ctor.invoked != 0);
    test_int(ctx.ctor.component, ecs_id(Position));
    test_int(ctx.ctor.size, sizeof(Position));
    test_int(ctx.ctor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_ctor_on_move_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Pair);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Pair, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    /* Create entity in existing table */
    ecs_entity_t e = ecs_new_w(world, Position);
    test_int(ctx.ctor.invoked, 0);

    /* Add pair to existing table */
    ecs_add_pair(world, e, ecs_id(Pair), ecs_id(Position));

    test_assert(ctx.ctor.invoked != 0);
    test_int(ctx.ctor.component, ecs_id(Pair));
    test_int(ctx.ctor.size, sizeof(Pair));
    test_int(ctx.ctor.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_move_on_realloc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .ctor = comp_ctor,
        .move = comp_move,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world);
    test_int(ctx.ctor.invoked, 0);

    ecs_add(world, e, Position);
    test_assert(ctx.ctor.invoked != 0);
    test_int(ctx.move.invoked, 0);
    test_int(ctx.ctor.component, ecs_id(Position));
    test_int(ctx.ctor.size, sizeof(Position));
    test_int(ctx.ctor.count, 1);

    ctx = (cl_ctx){ { 0 } };

    /* Trigger realloc & move */
    ecs_new_w(world, Position);
    ecs_new_w(world, Position);
    test_assert(ctx.ctor.invoked != 0);
    test_assert(ctx.move.invoked != 0);

    ecs_fini(world);
}

void ComponentLifecycle_move_on_bulk_new(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .ctor = comp_ctor,
        .move = comp_move,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world);
    test_int(ctx.ctor.invoked, 0);

    ecs_add(world, e, Position);
    test_assert(ctx.ctor.invoked != 0);
    test_int(ctx.move.invoked, 0);
    test_int(ctx.ctor.component, ecs_id(Position));
    test_int(ctx.ctor.size, sizeof(Position));
    test_int(ctx.ctor.count, 1);

    ctx = (cl_ctx){ { 0 } };

    ecs_bulk_new(world, Position, 1000);
    test_assert(ctx.ctor.invoked != 0);
    test_assert(ctx.move.invoked != 0);

    ecs_fini(world);
}

void ComponentLifecycle_on_add_on_bulk_new(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .on_add = ecs_on_add(Position)
    });

    ecs_bulk_new(world, Position, 1000);
    test_int(ctor_position, 1000);
    test_int(on_add_position, 1000);

    ecs_fini(world);
}

void ComponentLifecycle_move_on_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .ctor = comp_ctor,
        .move = comp_move,
        .ctx = &ctx
    });

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_new_w(world, Position);
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

void ComponentLifecycle_move_dtor_on_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .dtor = comp_dtor,
        .move = comp_move,
        .ctx = &ctx
    });

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_new_w(world, Position);

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

void ComponentLifecycle_copy_on_override_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Pair);

    ecs_add_pair(world, ecs_id(Pair), EcsOnInstantiate, EcsInherit);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Pair, {
        .ctor = comp_ctor,
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new(world);
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

void ComponentLifecycle_copy_on_override_pair_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Pair);

    ecs_add_pair(world, Pair, EcsOnInstantiate, EcsInherit);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .ctor = comp_ctor,
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t base = ecs_new(world);
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

void ComponentLifecycle_copy_on_set_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Pair);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Pair, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world);
    test_int(ctx.copy.invoked, 0);

    ecs_set_pair(world, e, Pair, ecs_id(Position), {0, 0});
    test_assert(ctx.copy.invoked != 0);
    test_int(ctx.copy.component, ecs_id(Pair));
    test_int(ctx.copy.size, sizeof(Pair));
    test_int(ctx.copy.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_copy_on_set_pair_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Pair);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world);
    test_int(ctx.copy.invoked, 0);
    
    ecs_set_pair_second(world, e, Pair, Position, {0, 0});
    test_assert(ctx.copy.invoked != 0);
    test_int(ctx.copy.component, ecs_id(Position));
    test_int(ctx.copy.size, sizeof(Position));
    test_int(ctx.copy.count, 1);

    ecs_fini(world);
}

void ComponentLifecycle_allow_lifecycle_overwrite_equal_callbacks(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position)
    });

    /* Same actions, should be ok */
    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position)
    });

    ecs_new_w(world, Position);

    test_int(ctor_position, 1);

    ecs_fini(world);
}

static
void AddPosition(ecs_iter_t *it) { }

void ComponentLifecycle_set_lifecycle_after_trigger(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, AddPosition, EcsOnAdd, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position)
    });

    ecs_new_w(world, Position);

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

void ComponentLifecycle_valid_entity_in_dtor_after_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Dummy);

    ecs_set_hooks(world, Dummy, {
        .dtor = dummy_dtor
    });

    ecs_entity_t e = ecs_new_w(world, Dummy);
    test_assert(e != 0);
    ecs_set(world, e, Dummy, {world, e, 0});

    ecs_delete(world, e);
    test_assert(!ecs_is_valid(world, e));
    test_assert(!ecs_is_alive(world, e));

    test_int(dummy_dtor_invoked, 1);

    ecs_fini(world);
}

void ComponentLifecycle_ctor_w_emplace(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .ctor = comp_ctor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    const Position *ptr = ecs_emplace(world, e, Position, NULL);
    test_assert(ptr != NULL);
    test_int(ctx.ctor.invoked, 0);

    ecs_fini(world);
}

void ComponentLifecycle_ctor_w_emplace_defer(void) {
    ecs_world_t* world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position)
    });

    test_int(ctor_position, 0);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    ecs_defer_begin(world);
    Position *ptr = ecs_emplace(world, e, Position, NULL);
    test_assert(ptr != NULL);
    test_int(ctor_position, 0);
    ptr->x = 10;
    ptr->y = 20;
    test_assert(!ecs_has(world, e, Position));
    test_int(ctor_position, 0);
    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Position));
    test_int(ctor_position, 0);
    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void ComponentLifecycle_on_add_w_emplace(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_add = on_add_position_emplace
    });

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    test_int(on_add_position, 0);
    const Position *ptr = ecs_emplace(world, e, Position, NULL);
    test_assert(ptr != NULL);
    test_int(on_add_position, 1);

    ecs_fini(world);
}

void ComponentLifecycle_on_add_w_emplace_existing(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .on_add = on_add_position_emplace
    });

    ecs_entity_t e = ecs_new_w(world, Velocity);
    test_assert(e != 0);

    test_int(ctor_position, 0);
    test_int(on_add_position, 0);
    const Position *ptr = ecs_emplace(world, e, Position, NULL);
    test_assert(ptr != NULL);
    test_int(ctor_position, 0);
    test_int(on_add_position, 1);

    ecs_fini(world);
}

void ComponentLifecycle_on_add_w_emplace_defer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_add = on_add_position_emplace
    });

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    ecs_defer_begin(world);
    test_int(on_add_position, 0);
    const Position *ptr = ecs_emplace(world, e, Position, NULL);
    test_assert(ptr != NULL);
    test_int(on_add_position, 0);
    ecs_defer_end(world);

    test_int(on_add_position, 1);

    ecs_fini(world);
}

static int move_ctor_position = 0;
static
void position_move_ctor(
    void *dst,
    void *src,
    int32_t count,
    const ecs_type_info_t *info)
{
    *((Position*)dst) = *((Position*)src);
    move_ctor_position ++;
}

static int move_dtor_position = 0;
static
void position_move_dtor(
    void *dst,
    void *src,
    int32_t count,
    const ecs_type_info_t *info)
{
    *((Position*)dst) = *((Position*)src);
    move_dtor_position ++;
}

void ComponentLifecycle_ctor_w_emplace_defer_use_move_ctor(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .move = ecs_move(Position),
        .move_ctor = position_move_ctor
    });

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    ecs_defer_begin(world);
    test_int(on_add_position, 0);
    Position *ptr = ecs_emplace(world, e, Position, NULL);
    ptr->x = 10;
    ptr->y = 20;
    test_assert(ptr != NULL);
    test_int(ctor_position, 0);
    test_int(move_position, 0);
    test_int(move_ctor_position, 0);
    ecs_defer_end(world);

    test_int(ctor_position, 0);
    test_int(move_position, 0);
    test_int(move_ctor_position, 1);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void ComponentLifecycle_ctor_w_emplace_defer_twice(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .move = ecs_move(Position),
        .move_ctor = position_move_ctor,
        .move_dtor = position_move_dtor
    });

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    ecs_defer_begin(world);

    {
        bool is_new = false;
        Position *ptr = ecs_emplace(world, e, Position, &is_new);
        test_bool(is_new, true);
        test_assert(ptr != NULL);
        ptr->x = 10;
        ptr->y = 20;
    }
    {
        bool is_new = false;
        Position *ptr = ecs_emplace(world, e, Position, &is_new);
        test_bool(is_new, true);
        test_assert(ptr != NULL);
        ptr->x = 20;
        ptr->y = 30;
    }

    test_int(ctor_position, 0);
    test_int(move_position, 0);
    test_int(move_ctor_position, 0);
    test_int(move_dtor_position, 0);
    ecs_defer_end(world);

    test_int(ctor_position, 0);
    test_int(move_position, 0);
    test_int(move_ctor_position, 1);
    test_int(move_dtor_position, 1);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 20);
    test_int(p->y, 30);

    ecs_fini(world); 
}

void ComponentLifecycle_ctor_w_emplace_defer_existing(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .move = ecs_move(Position),
        .move_ctor = position_move_ctor,
        .move_dtor = position_move_dtor
    });

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);

    test_int(ctor_position, 1);
    ctor_position = 0;

    ecs_defer_begin(world);

    {
        bool is_new = false;
        Position *ptr = ecs_emplace(world, e, Position, &is_new);
        test_bool(is_new, false);
        test_assert(ptr != NULL);
        ptr->x = 10;
        ptr->y = 20;
    }
    {
        bool is_new = false;
        Position *ptr = ecs_emplace(world, e, Position, &is_new);
        test_bool(is_new, false);
        test_assert(ptr != NULL);
        ptr->x = 20;
        ptr->y = 30;
    }

    test_int(ctor_position, 0);
    test_int(move_position, 0);
    test_int(move_ctor_position, 0);
    test_int(move_dtor_position, 0);
    ecs_defer_end(world);

    test_int(ctor_position, 0);
    test_int(move_position, 0);
    test_int(move_ctor_position, 0);
    test_int(move_dtor_position, 0);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 20);
    test_int(p->y, 30);

    ecs_fini(world);
}

void ComponentLifecycle_merge_async_stage_w_emplace(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .copy = ecs_copy(Position),
        .move_ctor = position_move_ctor,
        .dtor = ecs_dtor(Position)
    });

    ecs_entity_t e = ecs_new(world);

    ecs_world_t *async = ecs_stage_new(world);

    Position *p = ecs_emplace(async, e, Position, NULL);
    test_assert(!ecs_has(world, e, Position));
    test_int(ctor_position, 0);
    test_int(copy_position, 0);
    test_int(move_position, 0);
    test_int(move_ctor_position, 0);
    test_int(dtor_position, 0);
    p->x = 10;
    p->y = 20;

    ecs_merge(async);
    test_assert(ecs_has(world, e, Position));
    test_int(ctor_position, 0);
    test_int(copy_position, 0);
    test_int(move_position, 0);
    test_int(move_ctor_position, 1);
    test_int(dtor_position, 1);

    const Position *ptr = ecs_get(world, e, Position);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_stage_free(async);

    ecs_fini(world);
}

void ComponentLifecycle_merge_async_stage_w_emplace_to_deferred_world(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .copy = ecs_copy(Position)
    });

    ecs_entity_t e = ecs_new(world);

    ecs_world_t *async = ecs_stage_new(world);

    Position *p = ecs_emplace(async, e, Position, NULL);
    test_assert(!ecs_has(world, e, Position));
    test_int(ctor_position, 0);
    p->x = 10;
    p->y = 20;

    ecs_defer_begin(world);
    ecs_merge(async);
    test_assert(!ecs_has(world, e, Position));
    test_int(ctor_position, 0);

    ecs_defer_end(world);
    test_assert(ecs_has(world, e, Position));
    test_int(ctor_position, 0);

    const Position *ptr = ecs_get(world, e, Position);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_stage_free(async);

    ecs_fini(world);
}

static void invalid_ctor(void *ptr, int count, const ecs_type_info_t *ti) {
    test_assert(false);
}

void ComponentLifecycle_emplace_grow_w_existing_component(void) {
    test_quarantine("5 Feb 2023");

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_set_hooks(world, Position, {
        .ctor = invalid_ctor,
        .copy = ecs_copy(Position),
        .move = ecs_move(Position),
        .dtor = ecs_dtor(Position)
    });

    ecs_entity_t e1 = ecs_new_w(world, Velocity);
    ecs_entity_t e2 = ecs_new_w(world, Velocity);
    ecs_entity_t e3 = ecs_new_w(world, Velocity);

    {
        Position *p = ecs_emplace(world, e1, Position, NULL);
        p->x = 10;
        p->y = 20;
    }
    {
        Position *p = ecs_emplace(world, e2, Position, NULL);
        p->x = 30;
        p->y = 40;
    }
    {
        Position *p = ecs_emplace(world, e3, Position, NULL);
        p->x = 50;
        p->y = 60;
    }

    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));
    test_assert(ecs_has(world, e3, Position));

    {
        const Position *p = ecs_get(world, e1, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Position *p = ecs_get(world, e2, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }
    {
        const Position *p = ecs_get(world, e3, Position);
        test_assert(p != NULL);
        test_int(p->x, 50);
        test_int(p->y, 60);
    }

    ecs_fini(world);
}

void ComponentLifecycle_dtor_on_fini(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Dummy);

    ecs_set_hooks(world, Dummy, {
        .dtor = dummy_dtor
    });

    ecs_entity_t e = ecs_new_w(world, Dummy);
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

ECS_MOVE(Entity, dst, src, {
    dst->world = src->world;
    dst->e = src->e;
    dst->other = src->other;

    src->world = NULL;
    src->e = 0;
    src->other = 0;
})

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

void ComponentLifecycle_valid_type_in_dtor_on_fini(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Dummy);

    ecs_set_hooks(world, Dummy, {
        .dtor = type_dtor
    });

    ecs_entity_t e = ecs_new_w(world, Dummy);
    test_assert(e != 0);
    ecs_set(world, e, Dummy, {world, e});
    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);

    test_int(dummy_dtor_invoked, 0);

    ecs_fini(world);

    test_int(dummy_dtor_invoked, 1);
}

void ComponentLifecycle_valid_other_type_of_entity_in_dtor_on_fini(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT_DEFINE(world, Entity);

    ecs_set_hooks(world, Entity, {
        .dtor = other_type_dtor
    });

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_set(world, e2, Entity, {world, e2, e1});

    ecs_add(world, e1, Velocity);
    ecs_set(world, e1, Entity, {world, e1, e2});

    test_int(other_dtor_invoked, 0);

    ecs_fini(world);

    test_int(other_dtor_invoked, 2);
    test_int(other_dtor_valid_entity, 1);
}

void ComponentLifecycle_delete_in_dtor_other_type_on_fini(void) {
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

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_entity_t e2 = ecs_new_w(world, Velocity);

    ecs_set(world, e2, Entity, {world, e2, e1});
    ecs_set(world, e1, Entity, {world, e1, e2});

    ecs_set(world, e1, String, {world, e1, ecs_os_strdup("Foo")});
    ecs_set(world, e2, String, {world, e2, ecs_os_strdup("Foo")});

    test_int(other_dtor_invoked, 0);

    ecs_fini(world);

    test_int(other_dtor_invoked, 2);
    test_int(other_dtor_valid_entity, 1);
}

void ComponentLifecycle_delete_in_dtor_other_type_on_delete_parent(void) {
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

    ecs_entity_t parent = ecs_new(world);

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_entity_t e2 = ecs_new_w(world, Velocity);

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

void ComponentLifecycle_delete_in_dtor_other_type_on_delete(void) {
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

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_entity_t e2 = ecs_new_w(world, Velocity);

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

void ComponentLifecycle_delete_self_in_dtor_on_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Dummy);

    ecs_set_hooks(world, Dummy, {
        .dtor = self_delete_dtor
    });

    ecs_entity_t e1 = ecs_new_w(world, Dummy);
    ecs_entity_t e2 = ecs_new_w(world, Dummy);
    ecs_entity_t e3 = ecs_new_w(world, Dummy);

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

void ComponentLifecycle_on_set_after_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .on_set = ecs_on_set(Position)
    });

    ecs_entity_t e = ecs_new_w(world, Position);
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

void ComponentLifecycle_on_add_after_new(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = flecs_default_ctor,
        .on_add = ecs_on_add(Position)
    });

    ecs_new_w(world, Position);
    test_int(on_add_position, 1);
    ecs_new_w(world, Position);
    test_int(on_add_position, 2);
    ecs_new_w(world, Position);
    test_int(on_add_position, 3);

    ecs_fini(world);
}

void ComponentLifecycle_on_add_after_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = flecs_default_ctor,
        .on_add = ecs_on_add(Position)
    });

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_add(world, e1, Position);
    test_int(on_add_position, 1);
    ecs_add(world, e2, Position);
    test_int(on_add_position, 2);
    ecs_add(world, e3, Position);
    test_int(on_add_position, 3);

    ecs_fini(world);
}

void ComponentLifecycle_on_add_after_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = flecs_default_ctor,
        .on_add = ecs_on_add(Position)
    });

    ecs_insert(world, ecs_value(Position, {10, 20}));
    test_int(on_add_position, 1);
    ecs_insert(world, ecs_value(Position, {10, 20}));
    test_int(on_add_position, 2);
    ecs_insert(world, ecs_value(Position, {10, 20}));
    test_int(on_add_position, 3);

    ecs_fini(world);
}

static int on_remove_position = 0;

static void ecs_on_remove(Position)(ecs_iter_t *it) {
    test_assert(it->count >= 1);
    test_assert(it->event == EcsOnRemove);

    Position *p = ecs_field(it, Position, 0);
    for (int i = 0; i < it->count; i ++) {
        on_remove_position ++;
        test_int(p[i].x, 10);
        test_int(p[i].y, 20);
    }
}

void ComponentLifecycle_on_remove_after_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_remove = ecs_on_remove(Position)
    });

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {10, 20}));

    ecs_remove(world, e1, Position);
    test_int(on_remove_position, 1);
    ecs_remove(world, e2, Position);
    test_int(on_remove_position, 2);
    ecs_remove(world, e3, Position);
    test_int(on_remove_position, 3);

    ecs_fini(world);
}

void ComponentLifecycle_on_remove_after_clear(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_remove = ecs_on_remove(Position)
    });

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {10, 20}));

    ecs_clear(world, e1);
    test_int(on_remove_position, 1);
    ecs_clear(world, e2);
    test_int(on_remove_position, 2);
    ecs_clear(world, e3);
    test_int(on_remove_position, 3);

    ecs_fini(world);
}

void ComponentLifecycle_on_remove_after_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_remove = ecs_on_remove(Position)
    });

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {10, 20}));

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
            Position, ecs_new(it->world), {10, 20});
        on_remove_tag_set_position_invoked ++;
    }
}

static
void on_remove_tag_set_position_obj_pair(ecs_iter_t *it) {
    for (int i = 0; i < it->count; i ++) {
        ecs_set_pair_second(it->world, it->entities[i], 
            ecs_new(it->world), Position, {10, 20});
        on_remove_tag_set_position_invoked ++;
    }
}

void ComponentLifecycle_free_component_new_id_while_fini(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG(world, Tag);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = Tag,
        .events = {EcsOnRemove},
        .callback = on_remove_tag_set_position
    });

    ecs_new_w(world, Tag);

    ecs_fini(world);

    test_int(on_remove_tag_set_position_invoked, 1);
}

void ComponentLifecycle_dtor_component_new_id_while_fini(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG(world, Tag);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = Tag,
        .events = {EcsOnRemove},
        .callback = on_remove_tag_set_position
    });

    ecs_set_hooks(world, Position, {
        .ctor = flecs_default_ctor,
        .dtor = ecs_dtor(Position)
    });

    ecs_new_w(world, Tag);

    test_int(dtor_position, 0);

    ecs_fini(world);

    test_int(on_remove_tag_set_position_invoked, 1);
    test_int(dtor_position, 1);
}

void ComponentLifecycle_free_component_new_pair_id_while_fini(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG(world, Tag);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = Tag,
        .events = {EcsOnRemove},
        .callback = on_remove_tag_set_position_pair
    });

    ecs_new_w(world, Tag);

    ecs_fini(world);

    test_int(on_remove_tag_set_position_invoked, 1);
}

void ComponentLifecycle_dtor_component_new_pair_id_while_fini(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG(world, Tag);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = Tag,
        .events = {EcsOnRemove},
        .callback = on_remove_tag_set_position_pair
    });

    ecs_set_hooks(world, Position, {
        .ctor = flecs_default_ctor,
        .dtor = ecs_dtor(Position)
    });

    ecs_new_w(world, Tag);

    test_int(dtor_position, 0);

    ecs_fini(world);

    test_int(on_remove_tag_set_position_invoked, 1);
    test_int(dtor_position, 1);
}

void ComponentLifecycle_free_component_new_obj_pair_id_while_fini(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG(world, Tag);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = Tag,
        .events = {EcsOnRemove},
        .callback = on_remove_tag_set_position_obj_pair
    });

    ecs_new_w(world, Tag);

    ecs_fini(world);

    test_int(on_remove_tag_set_position_invoked, 1);
}

void ComponentLifecycle_dtor_component_new_obj_pair_id_while_fini(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG(world, Tag);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = Tag,
        .events = {EcsOnRemove},
        .callback = on_remove_tag_set_position_obj_pair
    });

    ecs_set_hooks(world, Position, {
        .ctor = flecs_default_ctor,
        .dtor = ecs_dtor(Position)
    });

    ecs_new_w(world, Tag);

    test_int(dtor_position, 0);

    ecs_fini(world);

    test_int(on_remove_tag_set_position_invoked, 1);
    test_int(dtor_position, 1);
}

void ComponentLifecycle_ctor_move_dtor_after_resize(void) {
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

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

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

void ComponentLifecycle_ctx_free(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctx = &component_lifecycle_ctx,
        .ctx_free = component_lifecycle_ctx_free
    });

    ecs_fini(world);

    test_int(1, component_lifecycle_ctx);
}

void ComponentLifecycle_binding_ctx_free(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .binding_ctx = &component_lifecycle_ctx,
        .binding_ctx_free = component_lifecycle_ctx_free
    });

    ecs_fini(world);

    test_int(1, component_lifecycle_ctx);
}

void ComponentLifecycle_ctx_free_after_delete_component(void) {
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

void ComponentLifecycle_binding_ctx_free_after_delete_component(void) {
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
    test_assert(it->callback_ctx == &component_lifecycle_binding_ctx);
    component_lifecycle_ctx ++;
    component_lifecycle_binding_ctx ++;
}

void ComponentLifecycle_on_add_ctx(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_add = test_lifecycle_ctx,
        .ctx = &component_lifecycle_ctx,
        .binding_ctx = &component_lifecycle_binding_ctx
    });

    ecs_new_w(world, Position);
    test_int(1, component_lifecycle_ctx);
    test_int(1, component_lifecycle_binding_ctx);

    ecs_fini(world);

    test_int(1, component_lifecycle_ctx);
    test_int(1, component_lifecycle_binding_ctx);
}

void ComponentLifecycle_on_remove_ctx(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_remove = test_lifecycle_ctx,
        .ctx = &component_lifecycle_ctx,
        .binding_ctx = &component_lifecycle_binding_ctx
    });

    ecs_entity_t e = ecs_new_w(world, Position);
    test_int(0, component_lifecycle_ctx);
    test_int(0, component_lifecycle_binding_ctx);

    ecs_remove(world, e, Position);
    test_int(1, component_lifecycle_ctx);
    test_int(1, component_lifecycle_binding_ctx);

    ecs_fini(world);

    test_int(1, component_lifecycle_ctx);
    test_int(1, component_lifecycle_binding_ctx);
}

void ComponentLifecycle_on_set_ctx(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_set = test_lifecycle_ctx,
        .ctx = &component_lifecycle_ctx,
        .binding_ctx = &component_lifecycle_binding_ctx
    });

    ecs_entity_t e = ecs_new_w(world, Position);
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

void ComponentLifecycle_on_add_w_existing_component(void) {
    ecs_world_t* world = ecs_mini();

    ECS_TAG(world, Tag);

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_add = test_on_event,
        .ctx = (void*)&EcsOnAdd
    });

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_add(world, e, Position);

    test_int(1, test_on_event_invoked);

    ecs_fini(world);
}

void ComponentLifecycle_on_remove_w_existing_component(void) {
    ecs_world_t* world = ecs_mini();

    ECS_TAG(world, Tag);

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_remove = test_on_event,
        .ctx = (void*)&EcsOnRemove
    });

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
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

void ComponentLifecycle_component_init_set_hooks(void) {
    ecs_world_t* world = ecs_mini();

    ecs_entity_t c = ecs_component_init(world, &(ecs_component_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
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

    ecs_entity_t e = ecs_new(world);
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

void ComponentLifecycle_component_init_name_from_type_info(void) {
    ecs_world_t* world = ecs_mini();

    ecs_entity_t c = ecs_component_init(world, &(ecs_component_desc_t){
        .type = {
            .name = "Position",
            .size = ECS_SIZEOF(Position),
            .alignment = ECS_ALIGNOF(Position),
        }
    });

    test_assert(c != 0);
    test_assert(c == ecs_lookup(world, "Position"));

    const EcsComponent *ptr = ecs_get(world, c, EcsComponent);
    test_assert(ptr != NULL);
    test_int(ptr->size, ECS_SIZEOF(Position));
    test_int(ptr->size, ECS_SIZEOF(Velocity));

    ecs_fini(world);
}

void ComponentLifecycle_component_init_scoped_name_from_type_info(void) {
    ecs_world_t* world = ecs_mini();

    ecs_entity_t c = ecs_component_init(world, &(ecs_component_desc_t){
        .type = {
            .name = "ns.Position",
            .size = ECS_SIZEOF(Position),
            .alignment = ECS_ALIGNOF(Position),
        }
    });

    test_assert(c != 0);
    test_assert(c == ecs_lookup(world, "ns.Position"));

    const EcsComponent *ptr = ecs_get(world, c, EcsComponent);
    test_assert(ptr != NULL);
    test_int(ptr->size, ECS_SIZEOF(Position));
    test_int(ptr->size, ECS_SIZEOF(Velocity));

    ecs_fini(world);
}

void ComponentLifecycle_component_init_w_recycled_id(void) {
    ecs_world_t *world = ecs_init();

    {
        ecs_entity_t c = ecs_component(world, {
            .entity = ecs_new(world),
            .type.size = 4,
            .type.alignment = 4
        });

        test_assert(c != 0);
        test_assert(ecs_has(world, c, EcsComponent));

        ecs_delete(world, c);
    }

    {
        ecs_entity_t c = ecs_component(world, {
            .entity = ecs_new(world),
            .type.size = 4,
            .type.alignment = 4
        });

        test_assert(c != 0);
        test_assert(ecs_has(world, c, EcsComponent));

        ecs_delete(world, c);
    }

    ecs_fini(world);
}

void ComponentLifecycle_component_init_w_recycled_non_component_id(void) {
    ecs_world_t *world = ecs_init();

    {
        ecs_entity_t c = ecs_component(world, {
            .entity = ecs_new(world),
            .type.size = 4,
            .type.alignment = 4
        });

        test_assert(c != 0);
        test_assert(ecs_has(world, c, EcsComponent));

        ecs_delete(world, c);
    }

    {
        ecs_entity_t c = ecs_new(world);
        test_assert(c != 0);
        test_assert(!ecs_has(world, c, EcsComponent));
        ecs_delete(world, c);
    }

    {
        ecs_entity_t c = ecs_component(world, {
            .entity = ecs_new(world),
            .type.size = 4,
            .type.alignment = 4
        });

        test_assert(c != 0);
        test_assert(ecs_has(world, c, EcsComponent));

        ecs_delete(world, c);
    }

    ecs_fini(world);
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

void ComponentLifecycle_on_add_after_ctor_w_add(void) {
    ecs_world_t* world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ctor_before_on_add,
        .on_add = on_add_after_ctor
    });

    test_int(0, ctor_before_on_add_count);
    test_int(0, on_add_after_ctor_count);

    ecs_new_w(world, Position);

    test_int(1, ctor_before_on_add_count);
    test_int(1, on_add_after_ctor_count);

    ecs_fini(world);
}

void ComponentLifecycle_on_add_after_ctor_w_add_to(void) {
    ecs_world_t* world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_set_hooks(world, Position, {
        .ctor = ctor_before_on_add,
        .on_add = on_add_after_ctor
    });

    test_int(0, ctor_before_on_add_count);
    test_int(0, on_add_after_ctor_count);

    ecs_entity_t e = ecs_new_w(world, Tag);

    test_int(0, ctor_before_on_add_count);
    test_int(0, on_add_after_ctor_count);

    ecs_add(world, e, Position);

    test_int(1, ctor_before_on_add_count);
    test_int(1, on_add_after_ctor_count);

    ecs_fini(world);
}

void ComponentLifecycle_with_before_hooks(void) {
    ecs_world_t* world = ecs_mini();

    ecs_entity_t pos_id = ecs_new(world);
    ecs_entity_t tag = ecs_new_w_pair(world, EcsWith, pos_id);

    ecs_entity_t ecs_id(Position) =
        ecs_component_init(world, &(ecs_component_desc_t){
            .entity = pos_id,
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

void ComponentLifecycle_with_component_on_add(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t ecs_id(Position) =
        ecs_component(world, {
            .type = {
                .size = ECS_SIZEOF(Position),
                .alignment = ECS_ALIGNOF(Position),
                .hooks = {
                    .ctor = flecs_default_ctor,
                    .on_add = ecs_on_add(Position)
                }
            }
        });

    ECS_TAG(world, Foo);

    ecs_add_pair(world, Foo, EcsWith, ecs_id(Position));

    test_int(on_add_position, 0);

    ecs_entity_t e = ecs_new_w(world, Foo);
    test_assert(ecs_has(world, e, Foo));
    test_assert(ecs_has(world, e, Position));
    test_int(on_add_position, 1);

    ecs_fini(world);
}

void ComponentLifecycle_move_ctor_on_move(void) {
    ecs_world_t* world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .dtor = ecs_dtor(Position),
        .move = ecs_move(Position),
        .move_ctor = position_move_ctor,
    });

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t p = ecs_new(world);

    Position *p1 = ecs_emplace(world, e1, Position, NULL);
    test_assert(p1 != NULL);
    Position *p2 = ecs_emplace(world, e2, Position, NULL);
    test_assert(p2 != NULL);

    test_int(ctor_position, 0);

    ecs_add_pair(world, e1, EcsChildOf, p);
    test_int(ctor_position, 0);
    test_int(move_ctor_position, 1); // move e1 to other table
    test_int(move_position, 1); // move e2 to old position of e1
    test_int(dtor_position, 1); // dtor old position for e2

    ecs_add_pair(world, e2, EcsChildOf, p);

    ecs_fini(world);
}

typedef struct {
    void *ptr;
} TestSelf;

ECS_COPY(TestSelf, dst, src, {
    test_assert(dst->ptr == dst);
})

ECS_MOVE(TestSelf, dst, src, {
    test_assert(dst->ptr == dst);
    test_assert(src->ptr == src);
})

ECS_CTOR(TestSelf, ptr, {
    ptr->ptr = ptr;
})

ECS_DTOR(TestSelf, ptr, {
    test_assert(ptr->ptr == ptr);
})

void ComponentLifecycle_ptr_to_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, TestSelf);

    ecs_set_hooks(world, TestSelf, {
        .ctor = ecs_ctor(TestSelf),
        .copy = ecs_copy(TestSelf),
        .move = ecs_move(TestSelf),
        .dtor = ecs_dtor(TestSelf)
    });

    ecs_entity_t role = ecs_entity(world, { .name = "MyRole" });

    ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, TestSelf, {"a"});

    ecs_entity_t e2 = ecs_new(world);
    ecs_set(world, e2, TestSelf, {"a"});

    ecs_entity_t e3 = ecs_new(world);
    ecs_add_pair(world, e2, e3, role);

    ecs_entity_t e4 = ecs_new(world);
    ecs_set(world, e4, TestSelf, {"a"});

    ecs_delete(world, role);

    ecs_delete_with(world, ecs_id(TestSelf));

    ecs_fini(world);
}

void ComponentLifecycle_ctor_move_dtor_from_move_ctor(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .copy = ecs_copy(Position),
        .move_ctor = position_move_ctor
    });

    ecs_entity_t e = ecs_new(world);

    ecs_world_t *async = ecs_stage_new(world);

    Position *p = ecs_emplace(async, e, Position, NULL);
    test_assert(!ecs_has(world, e, Position));
    test_int(ctor_position, 0);
    test_int(copy_position, 0);
    test_int(move_ctor_position, 0);
    p->x = 10;
    p->y = 20;

    ecs_merge(async);
    test_assert(ecs_has(world, e, Position));
    test_int(ctor_position, 0);
    test_int(copy_position, 0);
    test_int(move_ctor_position, 1);

    const Position *ptr = ecs_get(world, e, Position);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_stage_free(async);

    ecs_fini(world);
}

static int hook_w_offset_invoked = 0;
static int hook_w_offset_offset = 0;
static Position hook_w_offset_position;

static
void hook_w_offset(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 0);
    test_int(it->count, 1);
    hook_w_offset_offset = it->offset;
    hook_w_offset_invoked ++;
    hook_w_offset_position = *p;
}

void ComponentLifecycle_on_add_hook_check_offset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_add = hook_w_offset
    });

    ecs_insert(world, ecs_value(Position, {10, 20}));
    test_int(hook_w_offset_invoked, 1);
    test_int(hook_w_offset_offset, 0);

    ecs_insert(world, ecs_value(Position, {30, 40}));
    test_int(hook_w_offset_invoked, 2);
    test_int(hook_w_offset_offset, 1);

    ecs_insert(world, ecs_value(Position, {50, 60}));
    test_int(hook_w_offset_invoked, 3);
    test_int(hook_w_offset_offset, 2);

    ecs_fini(world);
}

void ComponentLifecycle_on_remove_hook_check_offset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_remove = hook_w_offset
    });

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {50, 60}));
    test_int(hook_w_offset_invoked, 0);

    ecs_remove(world, e3, Position);
    test_int(hook_w_offset_invoked, 1);
    test_int(hook_w_offset_offset, 2);
    test_int(hook_w_offset_position.x, 50);
    test_int(hook_w_offset_position.y, 60);

    ecs_remove(world, e2, Position);
    test_int(hook_w_offset_invoked, 2);
    test_int(hook_w_offset_offset, 1);
    test_int(hook_w_offset_position.x, 30);
    test_int(hook_w_offset_position.y, 40);

    ecs_remove(world, e1, Position);
    test_int(hook_w_offset_invoked, 3);
    test_int(hook_w_offset_offset, 0);
    test_int(hook_w_offset_position.x, 10);
    test_int(hook_w_offset_position.y, 20);

    ecs_fini(world);
}

void ComponentLifecycle_on_set_hook_check_offset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_set = hook_w_offset
    });

    ecs_insert(world, ecs_value(Position, {10, 20}));
    test_int(hook_w_offset_invoked, 1);
    test_int(hook_w_offset_offset, 0);
    test_int(hook_w_offset_position.x, 10);
    test_int(hook_w_offset_position.y, 20);

    ecs_insert(world, ecs_value(Position, {30, 40}));
    test_int(hook_w_offset_invoked, 2);
    test_int(hook_w_offset_offset, 1);
    test_int(hook_w_offset_position.x, 30);
    test_int(hook_w_offset_position.y, 40);

    ecs_insert(world, ecs_value(Position, {50, 60}));
    test_int(hook_w_offset_invoked, 3);
    test_int(hook_w_offset_offset, 2);
    test_int(hook_w_offset_position.x, 50);
    test_int(hook_w_offset_position.y, 60);

    ecs_fini(world);
}

static int on_set_position_invoked = 0;

static
void on_set_position(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 0);
    test_int(it->count, 1);

    test_int(p->x, 10);
    test_int(p->y, 20);

    on_set_position_invoked ++;
}

void ComponentLifecycle_on_set_hook_on_override(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_set = on_set_position
    });

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t p = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_add_id(world, p, EcsPrefab);
    test_int(on_set_position_invoked, 1);

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    test_int(on_set_position_invoked, 1);

    ecs_add(world, i, Position);
    test_int(on_set_position_invoked, 2);

    {
        const Position *p = ecs_get(world, i, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

void ComponentLifecycle_on_set_hook_on_auto_override(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_set_hooks(world, Position, {
        .on_set = on_set_position
    });

    ecs_entity_t p = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_add_id(world, p, ECS_AUTO_OVERRIDE | ecs_id(Position));
    ecs_add_id(world, p, EcsPrefab);
    test_int(on_set_position_invoked, 1);

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    test_int(on_set_position_invoked, 2);

    {
        const Position *p = ecs_get(world, i, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

void ComponentLifecycle_batched_set_new_component_w_lifecycle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .move = ecs_move(Position),
        .copy = ecs_copy(Position),
        .dtor = ecs_dtor(Position)
    });

    ecs_defer_begin(world);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});
    ecs_add(world, e, Tag); // to hit command batching

    test_int(ctor_position, 1);
    test_int(move_position, 0);
    test_int(copy_position, 1);
    test_int(dtor_position, 0);

    ecs_defer_end(world);

    test_int(ctor_position, 2);
    test_int(move_position, 1);
    test_int(copy_position, 1);
    test_int(dtor_position, 1);

    ecs_delete(world, e);

    test_int(ctor_position, 2);
    test_int(move_position, 1);
    test_int(copy_position, 1);
    test_int(dtor_position, 2);

    ecs_fini(world);
}

void ComponentLifecycle_batched_ensure_new_component_w_lifecycle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .move = ecs_move(Position),
        .copy = ecs_copy(Position),
        .dtor = ecs_dtor(Position)
    });

    ecs_defer_begin(world);

    ecs_entity_t e = ecs_new(world);
    ecs_ensure(world, e, Position);
    ecs_add(world, e, Tag); // to hit command batching

    test_int(ctor_position, 1);
    test_int(move_position, 0);
    test_int(copy_position, 0);
    test_int(dtor_position, 0);

    ecs_defer_end(world);

    test_int(ctor_position, 2);
    test_int(move_position, 1);
    test_int(copy_position, 0);
    test_int(dtor_position, 1);

    ecs_delete(world, e);

    test_int(ctor_position, 2);
    test_int(move_position, 1);
    test_int(copy_position, 0);
    test_int(dtor_position, 2);

    ecs_fini(world);
}

void ComponentLifecycle_on_nested_prefab_copy_test_invokes_copy_count(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .copy = comp_copy,
        .ctx = &ctx
    });

    ecs_entity_t child = ecs_new_w(world, Position);
    ecs_add_id(world, child, ECS_AUTO_OVERRIDE | ecs_id(Position));
    ecs_add_id(world, child, EcsPrefab);
    test_int(ctx.copy.invoked, 0);

    ecs_entity_t parent = ecs_entity(world, { .name = "parent"});
    ecs_add_id(world, parent, EcsPrefab);

    ecs_entity_t child_e1 = ecs_entity(world, { .name = "e1"});
    ecs_add_id(world, child_e1, EcsPrefab);
    ecs_add_pair(world, child_e1, EcsIsA, child);
    ecs_add_pair(world, child_e1, EcsChildOf, parent);

    test_int(ctx.copy.invoked, 1);

    ecs_entity_t child_e2 = ecs_entity(world, { .name = "e2"});
    ecs_add_id(world, child_e2, EcsPrefab);
    ecs_add_pair(world, child_e2, EcsIsA, child);
    ecs_add_pair(world, child_e2, EcsChildOf, parent);

    test_int(ctx.copy.invoked, 2);

    ecs_entity_t child_e3 = ecs_entity(world, { .name = "e3"});
    ecs_add_id(world, child_e3, EcsPrefab);
    ecs_add_pair(world, child_e3, EcsIsA, child);
    ecs_add_pair(world, child_e3, EcsChildOf, parent);

    test_int(ctx.copy.invoked, 3);

    ecs_entity_t a_parent = ecs_new_w_pair(world, EcsIsA, parent);
    (void)a_parent;

    test_int(ctx.copy.invoked, 7);
    test_int(ctx.copy.count, 9);

    ecs_fini(world);
}

// Tests if neither move nor move_ctor are set but move_dtor and ctor_move_dtor are set, 
// the move_dtor does not get invoked when a component is moved within the table.
// Instead ctor_move_dtor should be invoked for a destructive move operation. 
// Hence in total ctor_move_dtor should be invoked 2 times. 
// Once for the initial move, and once for the move within the table.
void ComponentLifecycle_no_move_no_move_ctor_with_move_dtor_with_ctor_move_dtor(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    cl_ctx ctx = { { 0 } };

    ecs_set_hooks(world, Position, {
        .ctor = NULL,
        .move = NULL,
        .move_ctor = NULL,
        .dtor = comp_dtor,
        .ctor_move_dtor = comp_pos_ctor_move_dtor,
        .move_dtor = comp_move_dtor,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position); 

    ecs_entity_t e2 = ecs_new(world);
    ecs_add(world, e2, Position);

    ecs_entity_t e3 = ecs_new(world);
    ecs_add(world, e3, Position);

    ctx.ctor_move_dtor.invoked = 0;
    ctx.move_dtor.invoked = 0;

    ecs_add(world, e2, Velocity);

    test_int(ctx.move_dtor.invoked, 0);
    test_int(ctx.ctor_move_dtor.invoked, 2);

    ecs_fini(world);
}
