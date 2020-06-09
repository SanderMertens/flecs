#include <api.h>

static
void Iter(ecs_iter_t *it) {
    ECS_COLUMN(it, Position, p, 1);
    Velocity *v = NULL;
    Mass *m = NULL;
        
    if (it->column_count >= 2) {
        v = ecs_column(it, Velocity, 2);
    }

    if (it->column_count >= 3) {
        m = ecs_column(it, Mass, 3);
    }

    probe_system(it);

    int i;
    for (i = 0; i < it->count; i ++) {
        p[i].x = 10;
        p[i].y = 20;

        if (v) {
            v[i].x = 30;
            v[i].y = 40;
        }

        if (m) {
            m[i] = 50;
        }
    }
}

void SystemManual_1_type_1_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Iter, 0, Position);

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);
    ECS_ENTITY(world, e_3, Position);
    

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_run(world, Iter, 1, NULL);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_3);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

static int normal_count;

static
void NormalSystem(ecs_iter_t *it) {
    normal_count ++;
}

static bool system_status_action_invoked = false;
static ecs_system_status_t enable_status = EcsSystemStatusNone;
static ecs_system_status_t active_status = EcsSystemStatusNone;

static
void status_action(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_system_status_t status,
    void *ctx)
{
    system_status_action_invoked = true;

    if (status == EcsSystemEnabled || status == EcsSystemDisabled) {
        enable_status = status;
    } else if (status == EcsSystemActivated || status == EcsSystemDeactivated) {
        active_status = status;
    }
}

static
void reset_status() {
    system_status_action_invoked = false;
    enable_status = EcsSystemStatusNone;
    active_status = EcsSystemStatusNone;
}

void SystemManual_activate_status() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, NormalSystem, 0, Position);

    ecs_set_system_status_action(world, NormalSystem, status_action, NULL);

    test_bool(system_status_action_invoked, true);
    test_assert(enable_status == EcsSystemEnabled);
    test_assert(active_status == EcsSystemStatusNone);

    ecs_run(world, NormalSystem, 0, NULL);
    test_int(normal_count, 0);

    reset_status();
    ecs_new(world, Position);    

    test_bool(system_status_action_invoked, true);
    test_assert(enable_status == EcsSystemStatusNone);
    test_assert(active_status == EcsSystemActivated);

    ecs_run(world, NormalSystem, 0, NULL);
    test_int(normal_count, 1);

    reset_status();
    ecs_fini(world);

    test_bool(system_status_action_invoked, true);
    test_assert(enable_status == EcsSystemDisabled);
    test_assert(active_status == EcsSystemDeactivated);
}

static
void AddVelocity(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, Velocity, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add(it->world, it->entities[i], Velocity);
    }
}

void SystemManual_no_automerge() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, AddVelocity, 0, Position, :Velocity);

    ECS_ENTITY(world, e_1, Position);

    ecs_set_automerge(world, false);

    ecs_run(world, AddVelocity, 1, NULL);

    test_assert(!ecs_has(world, e_1, Velocity));

    ecs_merge(world);

    test_assert(ecs_has(world, e_1, Velocity));

    ecs_fini(world);
}
