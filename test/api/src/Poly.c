#include <api.h>

static ECS_COMPONENT_DECLARE(Position);
static ECS_DECLARE(Tag);

static
void test_no_chain(
    ecs_world_t *world,
    ecs_poly_t *poly,
    ecs_id_t filter)
{
    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {30, 40});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {50, 60});

    ecs_add(world, e3, Tag);

    ecs_iter_t it;
    if (filter) {
        ecs_term_t term = { .id = filter };
        ecs_iter_poly(world, poly, &it, &term);
    } else {
        ecs_iter_poly(world, poly, &it, NULL);
    }

    test_assert(ecs_iter_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], e1);
    test_int(it.entities[1], e2);
    test_int(ecs_term_id(&it, 1), ecs_id(Position));
    test_int(ecs_term_source(&it, 1), 0);
    test_assert(it.table != NULL);

    Position *ptr = ecs_term(&it, Position, 1);
    test_assert(ptr != NULL);
    test_int(ptr[0].x, 10);
    test_int(ptr[0].y, 20);
    test_int(ptr[1].x, 30);
    test_int(ptr[1].y, 40);

    test_assert(ecs_iter_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    test_int(ecs_term_id(&it, 1), ecs_id(Position));
    test_int(ecs_term_source(&it, 1), 0);
    test_assert(it.table != NULL);

    ptr = ecs_term(&it, Position, 1);
    test_assert(ptr != NULL);
    test_int(ptr[0].x, 50);
    test_int(ptr[0].y, 60);

    test_assert(!ecs_iter_next(&it));
}

static
void test_w_chain(
    ecs_world_t *world,
    ecs_poly_t *poly)
{
    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {30, 40});
    ecs_set(world, 0, Position, {50, 60}); // Not matched

    ecs_add(world, e1, Tag);
    ecs_add(world, e2, Tag);

    ecs_iter_t it_arr[2];
    ecs_iter_t *it = it_arr;
    ecs_iter_poly(world, poly, it_arr, &(ecs_term_t){ .id = Tag});

    test_assert(ecs_iter_next(it));
    test_int(it->count, 2);
    test_int(it->entities[0], e1);
    test_int(it->entities[1], e2);
    test_int(ecs_term_id(it, 1), Tag);
    test_int(ecs_term_source(it, 1), 0);
    test_assert(it->table != NULL);

    ecs_iter_t *chain_it = it->chain_it;
    test_assert(chain_it != NULL);
    
    test_int(chain_it->count, 2);
    test_int(chain_it->entities[0], e1);
    test_int(chain_it->entities[1], e2);
    test_int(ecs_term_id(chain_it, 1), ecs_id(Position));
    test_int(ecs_term_source(chain_it, 1), 0);

    Position *ptr = ecs_term(chain_it, Position, 1);
    test_assert(ptr != NULL);
    test_int(ptr[0].x, 10);
    test_int(ptr[0].y, 20);
    test_int(ptr[1].x, 30);
    test_int(ptr[1].y, 40);

    test_assert(!ecs_iter_next(it));
}

void Poly_iter_query() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG_DEFINE(world, Tag);
    
    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);

    test_no_chain(world, q, 0);

    ecs_fini(world);
}

void Poly_iter_query_w_filter() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG_DEFINE(world, Tag);
    
    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);

    test_w_chain(world, q);

    ecs_fini(world);
}

void Poly_iter_world() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG_DEFINE(world, Tag);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {30, 40});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {50, 60});
    ecs_add(world, e3, Tag);

    bool e1_found = false;
    bool e2_found = false;
    bool e3_found = false;
    bool position_found = false;
    bool tag_found = false;

    ecs_iter_t it;
    ecs_iter_poly(world, world, &it, 0);

    test_assert(ecs_iter_next(&it));
    test_assert(it.count != 0); 

    int i;
    for (i = 0; i < it.count; i ++) {
        test_assert( ecs_is_alive(world, it.entities[i]));
        if (it.entities[i] == e1) e1_found = true;
        if (it.entities[i] == e2) e2_found = true;
        if (it.entities[i] == e3) e3_found = true;
        if (it.entities[i] == ecs_id(Position)) position_found = true;
        if (it.entities[i] == Tag) tag_found = true;
    }

    test_assert(!ecs_iter_next(&it));

    test_assert(e1_found);
    test_assert(e2_found);
    test_assert(e3_found);
    test_assert(position_found);
    test_assert(tag_found);

    ecs_fini(world);
}

void Poly_iter_world_w_filter() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG_DEFINE(world, Tag);

    test_no_chain(world, world, ecs_id(Position));

    ecs_fini(world);
}

void Poly_iter_rule() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG_DEFINE(world, Tag);
    
    ecs_rule_t *q = ecs_rule_new(world, "Position");
    test_assert(q != NULL);

    test_no_chain(world, q, 0);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void Poly_iter_rule_w_filter() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG_DEFINE(world, Tag);
    
    ecs_rule_t *q = ecs_rule_new(world, "Position");
    test_assert(q != NULL);

    test_w_chain(world, q);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void Poly_iter_filter() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG_DEFINE(world, Tag);

    ecs_filter_t *f = ecs_filter_init(world, &(ecs_filter_desc_t){
        .expr = "Position"});
    test_assert(f != NULL);

    test_no_chain(world, f, 0);

    ecs_filter_fini(f);

    ecs_fini(world);
}

void Poly_iter_filter_w_filter() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG_DEFINE(world, Tag);
    
    ecs_filter_t *f = ecs_filter_init(world, &(ecs_filter_desc_t){
        .expr = "Position"});
    test_assert(f != NULL);

    test_w_chain(world, f);

    ecs_filter_fini(f);

    ecs_fini(world);
}

static
void FooTrigger(ecs_iter_t *it) {}

static
void PolyTrigger(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);

    EcsPoly *poly = ecs_term(it, EcsPoly, 1);

    test_int(1, it->count);
    test_assert(poly->poly != NULL);
}

void Poly_on_set_poly_observer() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t tag = ecs_new_id(world);

    Probe ctx = {0};
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = { ecs_pair(ecs_id(EcsPoly), EcsObserver) },
        .events = { EcsOnSet },
        .callback = PolyTrigger,
        .ctx = &ctx
    });

    test_int(1, ctx.invoked);

    ecs_os_zeromem(&ctx);

    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ tag }},
        .events = { EcsOnAdd },
        .callback = FooTrigger
    });

    test_int(1, ctx.invoked);
    test_int(1, ctx.count);
    test_uint(t, ctx.e[0]);

    ecs_fini(world);
}

void Poly_on_set_poly_query() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t tag = ecs_new_id(world);

    Probe ctx = {0};
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = { ecs_pair(ecs_id(EcsPoly), EcsQuery) },
        .events = { EcsOnSet },
        .callback = PolyTrigger,
        .ctx = &ctx
    });

    test_int(0, ctx.invoked);

    ecs_query_init(world, &(ecs_query_desc_t){
        .filter.terms = {{ tag }},
    });

    test_int(1, ctx.invoked);
    test_int(1, ctx.count);

    ecs_fini(world);
}

void Poly_on_set_poly_system() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t tag = ecs_new_id(world);

    Probe ctx = {0};
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = { ecs_pair(ecs_id(EcsPoly), EcsSystem) },
        .events = { EcsOnSet },
        .callback = PolyTrigger,
        .ctx = &ctx
    });

    test_int(0, ctx.invoked);

    ecs_entity_t s = ecs_system_init(world, &(ecs_system_desc_t){
        .query.filter.terms = {{ tag }},
        .callback = FooTrigger
    });

    test_int(1, ctx.invoked);
    test_int(1, ctx.count);
    test_uint(s, ctx.e[0]);

    ecs_fini(world);
}
