#include <core.h>

static void OnPosition(ecs_iter_t *it) {
    probe_iter(it);
}

void Monitor_1_comp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, OnPosition, EcsOnAdd, Position);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPosition);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    ecs_os_zeromem(&ctx);

    ecs_run_aperiodic(world, 0);
    test_int(ctx.invoked, 0);

    ecs_add(world, e, Velocity);
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Monitor_2_comps(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, OnPosition, EcsOnAdd, Position, Velocity);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_int(ctx.invoked, 0);

    ecs_add(world, e, Velocity);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPosition);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0);    

    ecs_os_zeromem(&ctx);

    ecs_remove(world, e, Position);
    test_int(ctx.invoked, 0);

    ecs_add(world, e, Position);
    test_int(ctx.invoked, 1);    
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPosition);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0); 

    ecs_fini(world);
}

void Monitor_1_comp_1_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, OnPosition, EcsOnAdd, Position, !Velocity);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPosition);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0);    

    ecs_os_zeromem(&ctx);

    ecs_add(world, e, Velocity);
    test_int(ctx.invoked, 0);

    ecs_remove(world, e, Velocity);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPosition);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0); 

    ecs_fini(world);
}

void Monitor_1_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, OnPosition, EcsOnAdd, Position(up));

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t parent = ecs_new_w(world, Position);
    test_int(ctx.invoked, 0);

    ecs_entity_t e = ecs_new(world);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, EcsChildOf, parent);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.system, OnPosition);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], parent); 

    ecs_fini(world);
}

void Monitor_1_comp_1_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, OnPosition, EcsMonitor, 
        Position, Position(up ChildOf));

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t parent = ecs_new_w(world, Position);
    test_int(ctx.invoked, 0);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, EcsChildOf, parent);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.system, OnPosition);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Position));
    test_int(ctx.s[0][1], parent);    

    ecs_os_zeromem(&ctx);

    ecs_add(world, e, Velocity);
    test_int(ctx.invoked, 0);

    ecs_remove(world, e, Velocity);
    test_int(ctx.invoked, 0);

    ecs_remove(world, e, Position);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.event, EcsOnRemove);

    ecs_os_zeromem(&ctx);

    ecs_add(world, e, Position);
    test_int(ctx.invoked, 1);
    test_int(ctx.count, 1);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.system, OnPosition);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Position));
    test_int(ctx.s[0][1], parent);

    ecs_fini(world);
}

void Monitor_1_comp_prefab_new(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, OnPosition, EcsOnAdd, Position);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t Prefab = ecs_entity(world, { .name = "Prefab" });
    ecs_add_id(world, Prefab, EcsPrefab);
    ecs_add(world, Prefab, Position);

    test_int(ctx.invoked, 0);

    ecs_run_aperiodic(world, 0);
    test_int(ctx.invoked, 0);

    ecs_add(world, Prefab, Velocity);
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Monitor_1_comp_prefab_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, OnPosition, EcsOnAdd, Position);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w_id(world, EcsPrefab);
    ecs_add(world, e, Position);
    test_int(ctx.invoked, 0);

    ecs_run_aperiodic(world, 0);
    test_int(ctx.invoked, 0);

    ecs_add(world, e, Velocity);
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Monitor_monitor_w_and(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_OBSERVER(world, OnPosition, EcsMonitor, Position, Velocity);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_add(world, e, Velocity);
    test_int(ctx.invoked, 1); // First match, trigger OnAdd
    test_int(ctx.event, EcsOnAdd);

    ecs_os_zeromem(&ctx);

    ecs_remove(world, e, Velocity);
    test_int(ctx.invoked, 1); // First unmatch, trigger OnRemove
    test_int(ctx.event, EcsOnRemove);

    ecs_os_zeromem(&ctx);

    ecs_remove(world, e, Position);
    test_int(ctx.invoked, 0); // Second unmatch, no trigger

    ecs_add(world, e, Velocity);
    test_int(ctx.invoked, 0);

    ecs_add(world, e, Position);
    test_int(ctx.invoked, 1); // First match, trigger OnAdd
    test_int(ctx.event, EcsOnAdd);

    ecs_fini(world);
}

void Monitor_monitor_w_or(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_OBSERVER(world, OnPosition, EcsMonitor, Position, Velocity || Mass);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);
    test_int(ctx.invoked, 0); // No match, filter doesn't match yet

    ecs_add(world, e, Velocity);
    test_int(ctx.invoked, 1); // First match, triggers OnAdd
    test_int(ctx.event, EcsOnAdd);

    ecs_os_zeromem(&ctx);

    ecs_add(world, e, Mass);
    test_int(ctx.invoked, 0); // Second match, no trigger

    ecs_remove(world, e, Position);
    test_int(ctx.invoked, 1); // First non-match, triggers OnRemove
    test_int(ctx.event, EcsOnRemove);

    ecs_os_zeromem(&ctx);

    ecs_add(world, e, Position); // First match, trigger
    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnAdd);

    ecs_fini(world);
}

void Monitor_monitor_w_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_OBSERVER(world, OnPosition, EcsMonitor, Position, Mass, !Velocity);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_add(world, e, Mass); 
    test_int(ctx.invoked, 1); // First match, trigger OnAdd
    test_int(ctx.event, EcsOnAdd);

    ecs_os_zeromem(&ctx);

    ecs_add(world, e, Velocity);
    test_int(ctx.invoked, 1); // First unmatch, trigger OnRemove
    test_int(ctx.event, EcsOnRemove);

    ecs_os_zeromem(&ctx);

    ecs_remove(world, e, Velocity);
    test_int(ctx.invoked, 1); // First match, trigger OnAdd
    test_int(ctx.event, EcsOnAdd);

    ecs_fini(world);
}

void Monitor_monitor_w_optional(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_OBSERVER(world, OnPosition, EcsMonitor, Position, ?Velocity);

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);

    test_int(ctx.invoked, 1); // First match, trigger OnAdd
    test_int(ctx.event, EcsOnAdd);

    ecs_os_zeromem(&ctx);

    ecs_add(world, e, Velocity);
    test_int(ctx.invoked, 0); // Second match, no trigger

    ecs_remove(world, e, Position);
    test_int(ctx.invoked, 1); // First unmatch, trigger OnRemove
    test_int(ctx.event, EcsOnRemove);

    ecs_os_zeromem(&ctx);

    ecs_add(world, e, Position);
    test_int(ctx.invoked, 1); // First match, trigger OnAdd
    test_int(ctx.event, EcsOnAdd);

    ecs_os_zeromem(&ctx);

    ecs_remove(world, e, Velocity);
    test_int(ctx.invoked, 0); // Second match, no trigger

    ecs_fini(world);
}

void Monitor_monitor_w_superset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ECS_OBSERVER(world, OnPosition, EcsMonitor, Position, Velocity(up IsA));

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t base_1 = ecs_new_w(world, Velocity);
    test_assert(base_1 != 0);
    test_int(ctx.invoked, 0);

    ecs_entity_t base_2 = ecs_new_w(world, Velocity);
    test_assert(base_2 != 0);
    test_int(ctx.invoked, 0);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, EcsIsA, base_1);
    test_int(ctx.invoked, 1); // First match, trigger
    test_int(ctx.event, EcsOnAdd);

    ecs_os_zeromem(&ctx);

    ecs_add_pair(world, e, EcsIsA, base_2);
    test_int(ctx.invoked, 0); // Second match, no trigger

    ecs_remove_pair(world, e, EcsIsA, base_2);
    test_int(ctx.invoked, 0);

    ecs_remove_pair(world, e, EcsIsA, base_1);
    test_int(ctx.invoked, 1); // First unmatch, trigger OnRemove
    test_int(ctx.event, EcsOnRemove);

    ecs_os_zeromem(&ctx);

    ecs_remove(world, e, Position);
    test_int(ctx.invoked, 0); // Second unmatch, no trigger

    ecs_fini(world);
}

void Monitor_monitor_w_self_superset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ECS_OBSERVER(world, OnPosition, EcsMonitor, Position, Velocity(self|up IsA));

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t base_1 = ecs_new_w(world, Velocity);
    test_assert(base_1 != 0);
    test_int(ctx.invoked, 0);

    ecs_entity_t base_2 = ecs_new_w(world, Velocity);
    test_assert(base_2 != 0);
    test_int(ctx.invoked, 0);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, EcsIsA, base_1);
    test_int(ctx.invoked, 1); // First match, trigger
    test_int(ctx.event, EcsOnAdd);

    ecs_os_zeromem(&ctx);

    ecs_add_pair(world, e, EcsIsA, base_2);
    test_int(ctx.invoked, 0); // Second match, no trigger

    ecs_add(world, e, Velocity);
    test_int(ctx.invoked, 0); // Third match, no trigger

    ecs_remove_pair(world, e, EcsIsA, base_2);
    test_int(ctx.invoked, 0);

    ecs_remove_pair(world, e, EcsIsA, base_1);
    test_int(ctx.invoked, 0);

    ecs_remove(world, e, Velocity);
    test_int(ctx.invoked, 1); // First unmatch, trigger OnRemove
    test_int(ctx.event, EcsOnRemove);

    ecs_os_zeromem(&ctx);

    ecs_add(world, e, Velocity);
    test_int(ctx.invoked, 1); // First match, trigger
    test_int(ctx.event, EcsOnAdd);

    ecs_os_zeromem(&ctx);

    ecs_remove(world, e, Position);
    test_int(ctx.invoked, 1); // First unmatch, trigger OnRemove
    test_int(ctx.event, EcsOnRemove);

    ecs_fini(world);
}

void Monitor_monitor_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Eats);
    ECS_TAG(world, Apples);
    ECS_TAG(world, Pears);
    ECS_OBSERVER(world, OnPosition, EcsMonitor, Position, (Eats, *));

    Probe ctx = { 0 };
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);
    test_int(ctx.invoked, 0);

    ecs_add_pair(world, e, Eats, Apples);

    test_int(ctx.invoked, 1); // First match, trigger OnAdd
    test_int(ctx.event, EcsOnAdd);

    ecs_os_zeromem(&ctx);

    ecs_add_pair(world, e, Eats, Pears);
    test_int(ctx.invoked, 0); // Second match, no trigger

    ecs_remove(world, e, Position);
    test_int(ctx.invoked, 1); // First unmatch, trigger OnRemove
    test_int(ctx.event, EcsOnRemove);

    ecs_os_zeromem(&ctx);

    ecs_add(world, e, Position);
    test_int(ctx.invoked, 1); // First match, trigger OnAdd
    test_int(ctx.event, EcsOnAdd);

    ecs_os_zeromem(&ctx);

    ecs_remove_pair(world, e, Eats, Apples);
    test_int(ctx.invoked, 0);

    ecs_remove_pair(world, e, Eats, Pears);
    test_int(ctx.invoked, 1); // First unmatch, trigger OnRemove
    test_int(ctx.event, EcsOnRemove);

    ecs_fini(world);
}

static void Monitor(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);
}

void Monitor_monitor_at_fini(void) {
	ecs_world_t *world = ecs_mini();

	ECS_TAG(world, TagA);

    Probe ctx = {0};

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ TagA }},
        .events = {EcsMonitor},
        .callback = Monitor,
        .ctx = &ctx
    });

    test_int(ctx.invoked, 0);

	ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, TagA);

    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnAdd);

    ecs_os_zeromem(&ctx);
	
	ecs_fini(world);

    test_int(ctx.invoked, 1);
    test_int(ctx.event, EcsOnRemove);
}

typedef struct {
    ecs_entity_t event;
    ecs_table_t *table;
    ecs_table_t *other_table;
    int32_t invoked;
} check_table_t;

static void CheckTable(ecs_iter_t *it) {
    check_table_t *ctx = it->ctx;

    test_assert(it->event == ctx->event);
    test_assert(it->table == ctx->table);
    test_assert(it->other_table == ctx->other_table);

    ctx->invoked ++;
}

void Monitor_monitor_other_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, X);
    ECS_TAG(world, Y);

    ecs_entity_t x = ecs_new_w(world, X);
    ecs_entity_t xy = ecs_new_w(world, X);
    ecs_add(world, xy, Y);

    check_table_t ctx = {0};
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {
            { X },
            { Y, .oper = EcsNot }
        },
        .events = {EcsMonitor},
        .callback = CheckTable,
        .ctx = &ctx
    });

    ecs_table_t *root = ecs_table_find(world, NULL, 0);
    test_assert(root != NULL);
    test_int(ecs_table_get_type(root)->count, 0);

    ctx.table = ecs_get_table(world, x);
    ctx.other_table = root;
    ctx.event = EcsOnAdd;
    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, X);
    test_int(ctx.invoked, 1);

    ctx.table = ecs_get_table(world, xy);
    ctx.other_table = ecs_get_table(world, x);
    ctx.event = EcsOnRemove;
    ecs_add(world, e, Y);
    test_int(ctx.invoked, 2);

    ctx.table = ecs_get_table(world, xy);
    ctx.other_table = ecs_get_table(world, x);
    ctx.event = EcsOnAdd;
    ecs_remove(world, e, Y);
    test_int(ctx.invoked, 3);

    ctx.table = ecs_get_table(world, x);
    ctx.other_table = root;
    ctx.event = EcsOnRemove;

    ecs_fini(world);
}

typedef struct {
    ecs_entity_t event;
    Position *result;
    int32_t invoked;
} check_component_t;

static void CheckComponent(ecs_iter_t *it) {
    check_component_t *ctx = it->ctx;

    test_assert(it->event == ctx->event);

    ctx->result = ecs_field(it, Position, 0);
    ctx->invoked ++;
}

void Monitor_monitor_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    check_component_t ctx = {0};
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {
            { ecs_id(Position) },
            { Tag, .oper = EcsNot }
        },
        .events = {EcsMonitor},
        .callback = CheckComponent,
        .ctx = &ctx
    });

    ctx.event = EcsOnAdd;
    ecs_entity_t e = ecs_new_w(world, Position);
    const Position *expect = ecs_get(world, e, Position);
    test_int(ctx.invoked, 1);
    test_assert(ctx.result == expect);

    ctx.event = EcsOnRemove;
    ecs_add(world, e, Tag);
    expect = ecs_get(world, e, Position);
    test_int(ctx.invoked, 2);
    test_assert(ctx.result == expect);

    ctx.event = EcsOnAdd;
    ecs_remove(world, e, Tag);
    test_int(ctx.invoked, 3);
    test_assert(ctx.result == expect);

    expect = ecs_get(world, e, Position);
    ctx.event = EcsOnRemove;

    ecs_fini(world);

    test_int(ctx.invoked, 4);
    test_assert(ctx.result == expect);
}

void Monitor_yield_existing(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    /* Create entities before trigger */
    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    Probe ctx = {0};
    ecs_entity_t t = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ Tag }},
        .events = {EcsMonitor},
        .callback = Monitor,
        .ctx = &ctx,
        .yield_existing = true
    });

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 3);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnAdd);
    test_int(ctx.event_id, Tag);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);
    test_int(ctx.c[0][0], Tag);

    ecs_os_zeromem(&ctx);

    ecs_delete(world, t);

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 3);
    test_int(ctx.system, t);
    test_int(ctx.event, EcsOnRemove);
    test_int(ctx.event_id, Tag);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);
    test_int(ctx.c[0][0], Tag);

    ecs_fini(world);
}

typedef struct {
    int32_t count;
    ecs_entity_t events[16];
    ecs_entity_t entities[16];
} MonitorEvents;

static void Monitor_record_events(ecs_iter_t *it) {
    MonitorEvents *ctx = it->ctx;
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        test_assert(ctx->count < 16);
        ctx->events[ctx->count] = it->event;
        ctx->entities[ctx->count] = it->entities[i];
        ctx->count ++;
    }
}

void Monitor_monitor_w_not_childof_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t parent = ecs_new(world);

    MonitorEvents ctx = {0};
    ecs_entity_t o = ecs_observer(world, {
        .query.terms = {
            { .id = Tag },
            { .id = ecs_pair(EcsChildOf, EcsWildcard), .oper = EcsNot }
        },
        .events = { EcsMonitor },
        .callback = Monitor_record_events,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new_w(world, Tag);
    test_int(ctx.count, 1);
    test_uint(ctx.events[0], EcsOnAdd);
    test_uint(ctx.entities[0], e);

    ecs_add_pair(world, e, EcsChildOf, parent);
    test_int(ctx.count, 2);
    test_uint(ctx.events[1], EcsOnRemove);
    test_uint(ctx.entities[1], e);

    ecs_remove_pair(world, e, EcsChildOf, parent);
    test_int(ctx.count, 3);
    test_uint(ctx.events[2], EcsOnAdd);
    test_uint(ctx.entities[2], e);

    ecs_fini(world);
}

static int32_t monitor_error_count = 0;
static ecs_os_api_log_t monitor_prev_log;

static void Monitor_count_errors(
    int32_t level,
    const char *file,
    int32_t line,
    const char *msg)
{
    if (level <= -3) {
        monitor_error_count ++;
    }

    monitor_prev_log(level, file, line, msg);
}

void Monitor_monitor_w_neq_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t excluded = ecs_entity(world, { .name = "excluded" });
    ecs_add(world, excluded, Tag);

    ecs_os_api_t api = ecs_os_get_api();
    monitor_prev_log = api.log_;
    monitor_error_count = 0;
    api.log_ = Monitor_count_errors;
    ecs_os_set_api(&api);

    MonitorEvents ctx = {0};
    ecs_entity_t o = ecs_observer(world, {
        .query.expr = "Tag, $this != excluded",
        .events = { EcsMonitor },
        .callback = Monitor_record_events,
        .ctx = &ctx
    });

    api.log_ = monitor_prev_log;
    ecs_os_set_api(&api);

    test_assert(o != 0);
    test_int(monitor_error_count, 0);

    ecs_entity_t e = ecs_new_w(world, Tag);
    test_int(ctx.count, 1);
    test_uint(ctx.events[0], EcsOnAdd);
    test_uint(ctx.entities[0], e);

    ecs_remove(world, excluded, Tag);
    ecs_add(world, excluded, Tag);
    test_int(ctx.count, 1);

    ecs_fini(world);
}

void Monitor_monitor_w_parent_nonfragmenting_reparent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t parent_b = ecs_new(world);

    MonitorEvents ctx = {0};
    ecs_observer(world, {
        .query.terms = {{ .id = ecs_pair(EcsChildOf, parent_a) }},
        .events = { EcsMonitor },
        .callback = Monitor_record_events,
        .ctx = &ctx
    });

    ecs_entity_t child = ecs_new_w_parent(world, parent_a, NULL);
    test_int(ctx.count, 1);
    test_uint(ctx.events[0], EcsOnAdd);
    test_uint(ctx.entities[0], child);

    ecs_set(world, child, EcsParent, {parent_b});
    test_int(ctx.count, 2);
    test_uint(ctx.events[1], EcsOnRemove);
    test_uint(ctx.entities[1], child);

    ecs_set(world, child, EcsParent, {parent_a});
    test_int(ctx.count, 3);
    test_uint(ctx.events[2], EcsOnAdd);
    test_uint(ctx.entities[2], child);

    ecs_fini(world);
}

void Monitor_not_only_new_empty(void) {
    ecs_world_t *world = ecs_mini();

    MonitorEvents ctx = {0};
    ecs_entity_t o = ecs_observer(world, {
        .query.terms = {
            { .id = ecs_pair(EcsChildOf, EcsWildcard), .oper = EcsNot }
        },
        .events = { EcsMonitor },
        .callback = Monitor_record_events,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);
    test_int(ctx.count, 1);
    test_uint(ctx.events[0], EcsOnAdd);
    test_uint(ctx.entities[0], e);

    ecs_delete(world, o);

    ecs_fini(world);
}

void Monitor_not_only_delete_empty(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);

    MonitorEvents ctx = {0};
    ecs_entity_t o = ecs_observer(world, {
        .query.terms = {
            { .id = ecs_pair(EcsChildOf, EcsWildcard), .oper = EcsNot }
        },
        .events = { EcsMonitor },
        .callback = Monitor_record_events,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_delete(world, e);
    test_int(ctx.count, 1);
    test_uint(ctx.events[0], EcsOnRemove);
    test_uint(ctx.entities[0], e);

    ecs_delete(world, o);

    ecs_fini(world);
}

void Monitor_not_only_delete_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_w(world, Tag);

    MonitorEvents ctx = {0};
    ecs_entity_t o = ecs_observer(world, {
        .query.terms = {
            { .id = ecs_pair(EcsChildOf, EcsWildcard), .oper = EcsNot }
        },
        .events = { EcsMonitor },
        .callback = Monitor_record_events,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_delete(world, e);
    test_int(ctx.count, 1);
    test_uint(ctx.events[0], EcsOnRemove);
    test_uint(ctx.entities[0], e);

    ecs_delete(world, o);

    ecs_fini(world);
}

void Monitor_not_only_add_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world);

    MonitorEvents ctx = {0};
    ecs_entity_t o = ecs_observer(world, {
        .query.terms = {
            { .id = ecs_pair(EcsChildOf, EcsWildcard), .oper = EcsNot }
        },
        .events = { EcsMonitor },
        .callback = Monitor_record_events,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_add(world, e, Tag);
    test_int(ctx.count, 0);

    ecs_remove(world, e, Tag);
    test_int(ctx.count, 0);

    ecs_delete(world, o);

    ecs_fini(world);
}

void Monitor_not_only_new_child(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);

    MonitorEvents ctx = {0};
    ecs_entity_t o = ecs_observer(world, {
        .query.terms = {
            { .id = ecs_pair(EcsChildOf, EcsWildcard), .oper = EcsNot }
        },
        .events = { EcsMonitor },
        .callback = Monitor_record_events,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_new_w_pair(world, EcsChildOf, parent);
    test_int(ctx.count, 0);

    ecs_delete(world, o);

    ecs_fini(world);
}

void Monitor_not_only_delete_child(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);

    MonitorEvents ctx = {0};
    ecs_entity_t o = ecs_observer(world, {
        .query.terms = {
            { .id = ecs_pair(EcsChildOf, EcsWildcard), .oper = EcsNot }
        },
        .events = { EcsMonitor },
        .callback = Monitor_record_events,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_delete(world, child);
    test_int(ctx.count, 0);

    ecs_delete(world, parent);
    test_int(ctx.count, 1);
    test_uint(ctx.events[0], EcsOnRemove);
    test_uint(ctx.entities[0], parent);

    ecs_delete(world, o);

    ecs_fini(world);
}

void Monitor_optional_only_new_empty(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    MonitorEvents ctx = {0};
    ecs_entity_t o = ecs_observer(world, {
        .query.terms = {
            { .id = Tag, .oper = EcsOptional }
        },
        .events = { EcsMonitor },
        .callback = Monitor_record_events,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t e = ecs_new(world);
    test_int(ctx.count, 1);
    test_uint(ctx.events[0], EcsOnAdd);
    test_uint(ctx.entities[0], e);

    ecs_add(world, e, Tag);
    test_int(ctx.count, 1);

    ecs_delete(world, o);

    ecs_fini(world);
}

void Monitor_optional_only_delete_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_w(world, Tag);

    MonitorEvents ctx = {0};
    ecs_entity_t o = ecs_observer(world, {
        .query.terms = {
            { .id = Tag, .oper = EcsOptional }
        },
        .events = { EcsMonitor },
        .callback = Monitor_record_events,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_delete(world, e);
    test_int(ctx.count, 1);
    test_uint(ctx.events[0], EcsOnRemove);
    test_uint(ctx.entities[0], e);

    ecs_delete(world, o);

    ecs_fini(world);
}

void Monitor_on_add_any_not_invoked_for_new_empty(void) {
    ecs_world_t *world = ecs_mini();

    MonitorEvents ctx = {0};
    ecs_entity_t o = ecs_observer(world, {
        .query.terms = {{ .id = EcsAny }},
        .events = { EcsOnAdd },
        .callback = Monitor_record_events,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_new(world);
    test_int(ctx.count, 0);

    ecs_delete(world, o);

    ecs_fini(world);
}

void Monitor_on_remove_any_not_invoked_for_delete_empty(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);

    MonitorEvents ctx = {0};
    ecs_entity_t o = ecs_observer(world, {
        .query.terms = {{ .id = EcsAny }},
        .events = { EcsOnRemove },
        .callback = Monitor_record_events,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_delete(world, e);
    test_int(ctx.count, 0);

    ecs_delete(world, o);

    ecs_fini(world);
}

void Monitor_dont_keep_alive_delete_pair_target(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t parent = ecs_new(world);

    MonitorEvents ctx = {0};
    ecs_entity_t o = ecs_observer(world, {
        .query.terms = {
            { .id = ecs_pair(EcsChildOf, parent) },
            { .id = Tag, .oper = EcsOptional }
        },
        .query.flags = EcsQueryDontKeepAlive,
        .events = { EcsMonitor },
        .callback = Monitor_record_events,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    test_int(ctx.count, 1);
    test_uint(ctx.events[0], EcsOnAdd);
    test_uint(ctx.entities[0], child);

    ecs_delete(world, parent);
    test_assert(!ecs_is_alive(world, parent));
    test_assert(!ecs_is_alive(world, child));
    test_int(ctx.count, 2);
    test_uint(ctx.events[1], EcsOnRemove);
    test_uint(ctx.entities[1], child);

    ecs_delete(world, o);

    ecs_fini(world);
}

void Monitor_dont_keep_alive_single_term_delete_target(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);

    MonitorEvents ctx = {0};
    ecs_entity_t o = ecs_observer(world, {
        .query.terms = {{ .id = ecs_pair(EcsChildOf, parent) }},
        .query.flags = EcsQueryDontKeepAlive,
        .events = { EcsOnAdd },
        .callback = Monitor_record_events,
        .ctx = &ctx
    });
    test_assert(o != 0);

    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    test_int(ctx.count, 1);
    test_uint(ctx.entities[0], child);

    ecs_delete(world, parent);
    test_assert(!ecs_is_alive(world, parent));

    ecs_delete(world, o);

    ecs_fini(world);
}

void Monitor_delete_pair_target_in_use(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);

    MonitorEvents ctx = {0};
    ecs_entity_t o = ecs_observer(world, {
        .query.terms = {{ .id = ecs_pair(EcsChildOf, parent) }},
        .events = { EcsMonitor },
        .callback = Monitor_record_events,
        .ctx = &ctx
    });
    test_assert(o != 0);

    test_expect_abort();
    ecs_delete(world, parent);
}
