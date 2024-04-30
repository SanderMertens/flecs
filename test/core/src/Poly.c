#include <core.h>

static
void FooTrigger(ecs_iter_t *it) {}

static
void PolyTrigger(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);

    EcsPoly *poly = ecs_field(it, EcsPoly, 0);

    test_int(1, it->count);
    test_assert(poly->poly != NULL);
}

void Poly_on_set_poly_observer(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t tag = ecs_new(world);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms[0] = { ecs_pair(ecs_id(EcsPoly), EcsObserver) },
        .events = { EcsOnSet },
        .callback = PolyTrigger,
        .ctx = &ctx
    });

    test_int(1, ctx.invoked);

    ecs_os_zeromem(&ctx);

    ecs_entity_t t = ecs_observer(world, {
        .query.terms = {{ tag }},
        .events = { EcsOnAdd },
        .callback = FooTrigger
    });

    test_int(1, ctx.invoked);
    test_int(1, ctx.count);
    test_uint(t, ctx.e[0]);

    ecs_fini(world);
}

void Poly_on_set_poly_query(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t tag = ecs_new(world);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms[0] = { ecs_pair(ecs_id(EcsPoly), EcsQuery) },
        .events = { EcsOnSet },
        .callback = PolyTrigger,
        .ctx = &ctx
    });

    test_int(0, ctx.invoked);

    ecs_query(world, {
        .terms = {{ tag }},
        .entity = ecs_new(world)
    });

    test_int(1, ctx.invoked);
    test_int(1, ctx.count);

    ecs_fini(world);
}

void Poly_on_set_poly_system(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t tag = ecs_new(world);

    Probe ctx = {0};
    ecs_observer(world, {
        .query.terms[0] = { ecs_pair(ecs_id(EcsPoly), EcsSystem) },
        .events = { EcsOnSet },
        .callback = PolyTrigger,
        .ctx = &ctx
    });

    test_int(0, ctx.invoked);

    ecs_entity_t s = ecs_system(world, {
        .query.terms = {{ tag }},
        .callback = FooTrigger
    });

    test_int(1, ctx.invoked);
    test_int(1, ctx.count);
    test_uint(s, ctx.e[0]);

    ecs_fini(world);
}

void Poly_free_query_entity(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t qe = ecs_new(world);
    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Tag }},
        .entity = qe
    });

    const EcsPoly *poly = ecs_get_pair(world, qe, EcsPoly, EcsQuery);
    test_assert(poly != NULL);
    test_assert(poly->poly == q);

    ecs_delete(world, qe);

    test_assert(!ecs_is_alive(world, qe));

    ecs_fini(world);
}
