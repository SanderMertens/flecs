#include <api.h>

static
void OnPosition(ecs_iter_t *it) {
    probe_iter(it);
}

void Monitor_1_comp() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, OnPosition, EcsOnAdd, Position);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);
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

void Monitor_2_comps() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, OnPosition, EcsOnAdd, Position, Velocity);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);
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

void Monitor_1_comp_1_not() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, OnPosition, EcsOnAdd, Position, !Velocity);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);
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

void Monitor_1_parent() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, OnPosition, EcsOnAdd, Position(parent));

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t parent = ecs_new(world, Position);
    test_int(ctx.invoked, 0);

    ecs_entity_t e = ecs_new(world, 0);
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

void Monitor_1_comp_1_parent() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, OnPosition, EcsMonitor, 
        Position, Position(super(ChildOf)));

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t parent = ecs_new(world, Position);
    test_int(ctx.invoked, 0);

    ecs_entity_t e = ecs_new(world, Position);
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

void Monitor_1_comp_prefab_new() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, OnPosition, EcsOnAdd, Position);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ECS_PREFAB(world, Prefab, Position);

    test_int(ctx.invoked, 0);

    ecs_run_aperiodic(world, 0);
    test_int(ctx.invoked, 0);

    ecs_add(world, Prefab, Velocity);
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Monitor_1_comp_prefab_add() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_OBSERVER(world, OnPosition, EcsOnAdd, Position);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new_w_id(world, EcsPrefab);
    ecs_add(world, e, Position);
    test_int(ctx.invoked, 0);

    ecs_run_aperiodic(world, 0);
    test_int(ctx.invoked, 0);

    ecs_add(world, e, Velocity);
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Monitor_monitor_w_and() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_OBSERVER(world, OnPosition, EcsMonitor, Position, Velocity);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);
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

void Monitor_monitor_w_or() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_OBSERVER(world, OnPosition, EcsMonitor, Position, Velocity || Mass);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);
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

void Monitor_monitor_w_not() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_OBSERVER(world, OnPosition, EcsMonitor, Position, Mass, !Velocity);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);
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

void Monitor_monitor_w_optional() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_OBSERVER(world, OnPosition, EcsMonitor, Position, ?Velocity);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);
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

void Monitor_monitor_w_superset() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_OBSERVER(world, OnPosition, EcsMonitor, Position, Velocity(super));

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t base_1 = ecs_new(world, Velocity);
    test_assert(base_1 != 0);
    test_int(ctx.invoked, 0);

    ecs_entity_t base_2 = ecs_new(world, Velocity);
    test_assert(base_2 != 0);
    test_int(ctx.invoked, 0);

    ecs_entity_t e = ecs_new(world, Position);
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

void Monitor_monitor_w_self_superset() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_OBSERVER(world, OnPosition, EcsMonitor, Position, Velocity(self|super));

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t base_1 = ecs_new(world, Velocity);
    test_assert(base_1 != 0);
    test_int(ctx.invoked, 0);

    ecs_entity_t base_2 = ecs_new(world, Velocity);
    test_assert(base_2 != 0);
    test_int(ctx.invoked, 0);

    ecs_entity_t e = ecs_new(world, Position);
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

void Monitor_monitor_w_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Eats);
    ECS_TAG(world, Apples);
    ECS_TAG(world, Pears);
    ECS_OBSERVER(world, OnPosition, EcsMonitor, Position, (Eats, *));

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);
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

void Monitor_monitor_at_fini() {
	ecs_world_t *world = ecs_mini();

	ECS_TAG(world, TagA);

    Probe ctx = {0};

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ TagA }},
        .events = {EcsMonitor},
        .callback = Monitor,
        .ctx = &ctx
    });

    test_int(ctx.invoked, 0);

	ecs_entity_t e = ecs_new_id(world);
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

void Monitor_monitor_other_table() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, X);
    ECS_TAG(world, Y);

    ecs_entity_t x = ecs_new(world, X);
    ecs_entity_t xy = ecs_new(world, X);
    ecs_add(world, xy, Y);

    check_table_t ctx = {0};
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {
            { X },
            { Y, .oper = EcsNot }
        },
        .events = {EcsMonitor},
        .callback = CheckTable,
        .ctx = &ctx
    });

    ctx.table = ecs_get_table(world, x);
    ctx.other_table = NULL;
    ctx.event = EcsOnAdd;
    ecs_entity_t e = ecs_new(world, X);
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
    ctx.other_table = NULL;
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

    ctx->result = ecs_term(it, Position, 1);
    ctx->invoked ++;
}

void Monitor_monitor_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    check_component_t ctx = {0};
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {
            { ecs_id(Position) },
            { Tag, .oper = EcsNot }
        },
        .events = {EcsMonitor},
        .callback = CheckComponent,
        .ctx = &ctx
    });

    ctx.event = EcsOnAdd;
    ecs_entity_t e = ecs_new(world, Position);
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
