#include <core.h>

static
void OnPosition(ecs_iter_t *it) {
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

    ECS_PREFAB(world, Prefab, Position);

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
