#include <addons.h>

void InitVelocity(ecs_iter_t *it) {
    Velocity *v = ecs_field(it, Velocity, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        v[i].x = 10;
        v[i].y = 20;
    }
}

void InitMass(ecs_iter_t *it) {
    Mass *m = ecs_field(it, Mass, 1);    
    int i;
    for (i = 0; i < it->count; i ++) {
        m[i] = 3;
    }
}

void Iter(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 1);

    Velocity *v = NULL;
    Mass *m = NULL;

    if (it->field_count >= 2) {
        v = ecs_field(it, Velocity, 2);
        test_assert(!ecs_field_is_self(it, 2));
    }

    if (it->field_count >= 3) {
        m = ecs_field(it, Mass, 3);
        test_assert(!m || !ecs_field_is_self(it, 3));
    }

    probe_iter(it);

    int i;
    for (i = 0; i < it->count; i ++) {
        p[i].x += v->x;
        p[i].y += v->y;

        if (m) {
            p[i].x += *m;
            p[i].y += *m;
        }
    }
}

void System_w_FromSystem_2_column_1_from_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_OBSERVER(world, InitVelocity, EcsOnAdd, Velocity);
    ECS_SYSTEM(world, Iter, EcsOnUpdate, Position, Velocity(Iter));

    test_assert( ecs_has(world, Iter, Velocity));
    const Velocity *v = ecs_get(world, Iter, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 10);
    test_int(v->y, 20);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_set(world, 0, Position, {0, 0});

    ecs_progress(world, 1);
    
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], Iter);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_progress(world, 1);

    test_int(p->x, 20);
    test_int(p->y, 40);

    ecs_fini(world);
}

void System_w_FromSystem_3_column_2_from_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_OBSERVER(world, InitVelocity, EcsOnAdd, Velocity);
    ECS_OBSERVER(world, InitMass, EcsOnAdd, Mass);
    ECS_SYSTEM(world, Iter, EcsOnUpdate, Position, Velocity(Iter), Mass(Iter));

    test_assert( ecs_has(world, Iter, Velocity));
    const Velocity *v = ecs_get(world, Iter, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 10);
    test_int(v->y, 20);

    test_assert( ecs_has(world, Iter, Mass));
    const Mass *m = ecs_get(world, Iter, Mass);
    test_assert(m != NULL);
    test_int(*m, 3);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_set(world, 0, Position, {0, 0});

    ecs_progress(world, 1);
    
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 3);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], Iter);
    test_int(ctx.c[0][2], ecs_id(Mass));
    test_int(ctx.s[0][2], Iter);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);

    ecs_progress(world, 1);

    test_int(p->x, 26);
    test_int(p->y, 46);

    ecs_fini(world);
}

void Iter_reactive(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 1);

    Velocity *v = it->param;
    Mass *m = NULL;

    if (it->field_count >= 2) {
        v = ecs_field(it, Velocity, 2);
        test_assert(!ecs_field_is_self(it, 2));
    }

    probe_iter(it);

    int i;
    for (i = 0; i < it->count; i ++) {
        p[i].x = v->x;
        p[i].y = v->y;

        if (m) {
            p[i].x = *m;
            p[i].y = *m;
        }
    }
}

void Dummy_1(ecs_iter_t *it) { }
void Dummy_2(ecs_iter_t *it) { }

void System_w_FromSystem_auto_add_tag() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Dummy_1, EcsOnUpdate, Position, Foo(Dummy_1));
    ECS_SYSTEM(world, Dummy_2, 0, Position, Foo(Dummy_2));

    test_assert( ecs_has_id(world, Dummy_1, Foo));
    test_assert( ecs_has_id(world, Dummy_2, Foo));

    ecs_fini(world);
}
