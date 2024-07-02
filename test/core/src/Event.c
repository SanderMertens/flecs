#include <core.h>

static
void system_callback(ecs_iter_t *it) {
    probe_iter(it);
}

static Position *system_param = NULL;

static
void system_w_param_callback(ecs_iter_t *it) {
    Position *p = it->param;
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    system_param = p;
    probe_iter(it);
}

void Event_table_1_id_w_trigger(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t evt = ecs_new(world);
    ecs_entity_t id = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id);
    ecs_table_t *table = ecs_get_table(world, e);

    Probe ctx = {0};

    ecs_entity_t s = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id,
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx
    });

    ecs_emit(world, &(ecs_event_desc_t){
        .event = evt,
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .table = table,
        .observable = world
    });

    test_int(ctx.invoked, 1);
    test_assert(ctx.system == s);
    test_assert(ctx.event == evt);
    test_assert(ctx.event_id == id);
    test_int(ctx.count, 1);
    test_assert(ctx.e[0] == e);

    ecs_fini(world);
}

void Event_table_2_ids_w_trigger(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t evt = ecs_new(world);
    ecs_entity_t id_a = ecs_new(world);
    ecs_entity_t id_b = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id_a);
    ecs_add_id(world, e, id_b);
    ecs_table_t *table = ecs_get_table(world, e);

    Probe ctx = {0};

    ecs_entity_t s_a = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id_a,
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx,
    });

    ecs_emit(world, &(ecs_event_desc_t){
        .event = evt,
        .ids = &(ecs_type_t){.count = 2, .array = (ecs_id_t[]){ id_a, id_b }},
        .table = table,
        .observable = world
    });

    test_int(ctx.invoked, 1);
    test_assert(ctx.system == s_a);
    test_assert(ctx.event == evt);
    test_assert(ctx.event_id == id_a);
    test_int(ctx.count, 1);
    test_assert(ctx.e[0] == e);

    ecs_delete(world, s_a);

    ecs_entity_t s_b = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id_b,
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx
    });

    ecs_os_zeromem(&ctx);

    ecs_emit(world, &(ecs_event_desc_t){
        .event = evt,
        .ids = &(ecs_type_t){.count = 2, .array = (ecs_id_t[]){ id_a, id_b }},
        .table = table,
        .observable = world
    });

    test_int(ctx.invoked, 1);
    test_assert(ctx.system == s_b);
    test_assert(ctx.event == evt);
    test_assert(ctx.event_id == id_b);
    test_int(ctx.count, 1);
    test_assert(ctx.e[0] == e);

    ecs_fini(world);
}

void Event_table_1_id_w_observer(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t evt = ecs_new(world);
    ecs_entity_t id = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id);
    ecs_table_t *table = ecs_get_table(world, e);

    Probe ctx = {0};

    ecs_entity_t s = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ id }},
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx
    });

    ecs_emit(world, &(ecs_event_desc_t){
        .event = evt,
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .table = table,
        .observable = world
    });

    test_int(ctx.invoked, 1);
    test_assert(ctx.system == s);
    test_assert(ctx.event == evt);
    test_assert(ctx.event_id == id);
    test_int(ctx.count, 1);
    test_assert(ctx.e[0] == e);

    ecs_fini(world);
}

void Event_table_2_ids_w_observer(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t evt = ecs_new(world);
    ecs_entity_t id_a = ecs_new(world);
    ecs_entity_t id_b = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id_a);
    ecs_add_id(world, e, id_b);
    ecs_table_t *table = ecs_get_table(world, e);

    Probe ctx = {0};

    ecs_entity_t s_a = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ id_a }},
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx
    });

    ecs_emit(world, &(ecs_event_desc_t){
        .event = evt,
        .ids = &(ecs_type_t){.count = 2, .array = (ecs_id_t[]){ id_a, id_b }},
        .table = table,
        .observable = world
    });

    test_int(ctx.invoked, 1);
    test_assert(ctx.system == s_a);
    test_assert(ctx.event == evt);
    test_assert(ctx.event_id == id_a || ctx.event_id == id_b);
    test_int(ctx.count, 1);
    test_assert(ctx.e[0] == e);

    ecs_fini(world);
}

static int empty_table_callback_invoked = 0;

static
void empty_table_callback(ecs_iter_t *it) {
    test_assert(it->table == it->ctx);
    test_int(it->count, 0);
    empty_table_callback_invoked++;
}

void Event_emit_event_for_empty_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t evt = ecs_new(world);

    ecs_entity_t e = ecs_new_w(world, TagA);
    ecs_table_t *table = ecs_get_table(world, e);
    ecs_delete(world, e);

    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ TagA }},
        .events = {evt},
        .callback = empty_table_callback,
        .ctx = table
    });

    ecs_emit(world, &(ecs_event_desc_t){
        .event = evt,
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ TagA }},
        .table = table,
        .observable = world
    });

    test_int(empty_table_callback_invoked, 1);

    ecs_delete(world, o);

    ecs_fini(world);
}

void Event_emit_table_event(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_add_pair(world, TagA, EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new_w_id(world, TagA);
    ecs_table_t *base_table = ecs_get_table(world, base);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsIsA, base);

    Probe ctx_a = {0};
    Probe ctx_b = {0};

    ecs_entity_t evt = ecs_new(world);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = TagA,
        .query.terms[0].src.id = EcsSelf,
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx_a
    });

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = TagA,
        .query.terms[0].src.id = EcsUp,
        .query.terms[0].trav = EcsIsA,
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx_b
    });

    ecs_emit(world, &(ecs_event_desc_t){
        .event = evt,
        .ids = &(ecs_type_t){.array = (ecs_id_t[]){ TagA }, .count = 1},
        .table = base_table,
        .observable = world
    });

    test_int(ctx_a.invoked, 1);
    test_int(ctx_b.invoked, 1);

    ecs_emit(world, &(ecs_event_desc_t){
        .event = evt,
        .ids = &(ecs_type_t){.array = (ecs_id_t[]){ TagA }, .count = 1},
        .table = base_table,
        .observable = world,
        .flags = EcsEventTableOnly
    });

    test_int(ctx_a.invoked, 2);
    test_int(ctx_b.invoked, 1);

    ecs_fini(world);
}

void Event_emit_staged_from_world(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t evt = ecs_new(world);
    ecs_entity_t id = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id);
    ecs_table_t *table = ecs_get_table(world, e);

    Probe ctx = {0};

    ecs_entity_t s = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id,
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx
    });
    ecs_readonly_begin(world, false);
    ecs_emit(world, &(ecs_event_desc_t){
        .event = evt,
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .table = table,
        .observable = world
    });
    ecs_readonly_end(world);

    test_int(ctx.invoked, 1);
    test_assert(ctx.system == s);
    test_assert(ctx.event == evt);
    test_assert(ctx.event_id == id);
    test_int(ctx.count, 1);
    test_assert(ctx.e[0] == e);

    ecs_fini(world);
}

void Event_emit_staged_from_stage(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t evt = ecs_new(world);
    ecs_entity_t id = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id);
    ecs_table_t *table = ecs_get_table(world, e);

    Probe ctx = {0};

    ecs_entity_t s = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id,
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx
    });
    ecs_readonly_begin(world, false);
    ecs_world_t *stage = ecs_get_stage(world, 0);
    ecs_emit(stage, &(ecs_event_desc_t){
        .event = evt,
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .table = table,
        .observable = world
    });
    ecs_readonly_end(world);

    test_int(ctx.invoked, 1);
    test_assert(ctx.system == s);
    test_assert(ctx.event == evt);
    test_assert(ctx.event_id == id);
    test_int(ctx.count, 1);
    test_assert(ctx.e[0] == e);

    ecs_fini(world);
}

void Event_emit_staged_from_world_observer(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t evt = ecs_new(world);
    ecs_entity_t id = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id);
    ecs_table_t *table = ecs_get_table(world, e);

    Probe ctx = {0};

    ecs_entity_t s = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ id }},
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx
    });
    ecs_readonly_begin(world, false);
    ecs_emit(world, &(ecs_event_desc_t){
        .event = evt,
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .table = table,
        .observable = world
    });
    ecs_readonly_end(world);

    test_int(ctx.invoked, 1);
    test_assert(ctx.system == s);
    test_assert(ctx.event == evt);
    test_assert(ctx.event_id == id);
    test_int(ctx.count, 1);
    test_assert(ctx.e[0] == e);

    ecs_fini(world);
}

void Event_emit_staged_from_stage_observer(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t evt = ecs_new(world);
    ecs_entity_t id = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id);
    ecs_table_t *table = ecs_get_table(world, e);

    Probe ctx = {0};

    ecs_entity_t s = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ id }},
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx
    });

    ecs_readonly_begin(world, false);
    ecs_world_t *stage = ecs_get_stage(world, 0);

    ecs_emit(stage, &(ecs_event_desc_t){
        .event = evt,
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .table = table,
        .observable = world
    });
    ecs_readonly_end(world);

    test_int(ctx.invoked, 1);
    test_assert(ctx.system == s);
    test_assert(ctx.event == evt);
    test_assert(ctx.event_id == id);
    test_int(ctx.count, 1);
    test_assert(ctx.e[0] == e);

    ecs_fini(world);
}

void Event_emit_for_entity(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t evt = ecs_new(world);
    ecs_entity_t id = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id);

    Probe ctx = {0};

    ecs_entity_t s = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id,
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx
    });

    ecs_emit(world, &(ecs_event_desc_t){
        .event = evt,
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .entity = e
    });

    test_int(ctx.invoked, 1);
    test_assert(ctx.system == s);
    test_assert(ctx.event == evt);
    test_assert(ctx.event_id == id);
    test_int(ctx.count, 1);
    test_assert(ctx.e[0] == e);

    ecs_fini(world);
}

static int ObserverA_invoked = false;
static void ObserverA(ecs_iter_t *it) {
    ObserverA_invoked ++;
}

static int ObserverB_invoked = false;
static void ObserverB(ecs_iter_t *it) {
    ObserverB_invoked ++;
}

void Event_emit_custom_for_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t MyEvent = ecs_new(world);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);

    ecs_observer(world, {
        .query.terms = {{ .id = EcsAny, .src.id = e1 }},
        .events = {MyEvent},
        .callback = ObserverA
    });

    ecs_observer(world, {
        .query.terms = {{ .id = EcsAny, .src.id = e2 }},
        .events = {MyEvent},
        .callback = ObserverB
    });

    ecs_emit(world, &(ecs_event_desc_t) {
        .ids = &(ecs_type_t){ .count = 1, .array = (ecs_id_t[]){ EcsAny } },
        .entity = e1,
        .event = MyEvent
    });

    test_int(ObserverA_invoked, 1);
    test_int(ObserverB_invoked, 0);
    
    ecs_emit(world, &(ecs_event_desc_t) {
        .ids = &(ecs_type_t){ .count = 1, .array = (ecs_id_t[]){ EcsAny } },
        .entity = e2,
        .event = MyEvent
    });

    test_int(ObserverA_invoked, 1);
    test_int(ObserverB_invoked, 1);

    ecs_fini(world);
}

void Event_emit_custom_implicit_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t MyEvent = ecs_new(world);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);

    ecs_observer(world, {
        .query.terms = {{ .id = EcsAny, .src.id = e1 }},
        .events = {MyEvent},
        .callback = ObserverA
    });

    ecs_observer(world, {
        .query.terms = {{ .id = EcsAny, .src.id = e2 }},
        .events = {MyEvent},
        .callback = ObserverB
    });

    ecs_emit(world, &(ecs_event_desc_t) {
        .entity = e1,
        .event = MyEvent
    });

    test_int(ObserverA_invoked, 1);
    test_int(ObserverB_invoked, 0);
    
    ecs_emit(world, &(ecs_event_desc_t) {
        .entity = e2,
        .event = MyEvent
    });

    test_int(ObserverA_invoked, 1);
    test_int(ObserverB_invoked, 1);

    ecs_fini(world);
}

void Event_emit_custom_empty_type(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t MyEvent = ecs_new(world);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);

    ecs_observer(world, {
        .query.terms = {{ .id = EcsAny, .src.id = e1 }},
        .events = {MyEvent},
        .callback = ObserverA
    });

    ecs_observer(world, {
        .query.terms = {{ .id = EcsAny, .src.id = e2 }},
        .events = {MyEvent},
        .callback = ObserverB
    });

    ecs_emit(world, &(ecs_event_desc_t) {
        .ids = &(ecs_type_t){ .count = 0 },
        .entity = e1,
        .event = MyEvent
    });

    test_int(ObserverA_invoked, 1);
    test_int(ObserverB_invoked, 0);
    
    ecs_emit(world, &(ecs_event_desc_t) {
        .ids = &(ecs_type_t){ .count = 0 },
        .entity = e2,
        .event = MyEvent
    });

    test_int(ObserverA_invoked, 1);
    test_int(ObserverB_invoked, 1);

    ecs_fini(world);
}

void Event_emit_w_param(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position); /* Used as event type */
    
    ecs_entity_t id = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id);

    Probe ctx = {0};
    Position p = {10, 20};

    ecs_entity_t s = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id,
        .events = {ecs_id(Position)},
        .callback = system_w_param_callback,
        .ctx = &ctx
    });

    ecs_emit(world, &(ecs_event_desc_t){
        .event = ecs_id(Position),
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .entity = e,
        .param = &p
    });

    test_int(ctx.invoked, 1);
    test_assert(ctx.system == s);
    test_assert(ctx.event == ecs_id(Position));
    test_assert(ctx.event_id == id);
    test_int(ctx.count, 1);
    test_assert(ctx.e[0] == e);
    test_assert(system_param == &p);

    ecs_fini(world);
}

void Event_emit_w_param_multi_observer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position); /* Used as event type */
    ECS_TAG(world, Foo);

    ecs_entity_t id = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id);
    ecs_add(world, e, Foo);

    Probe ctx = {0};
    Position p = {10, 20};

    ecs_entity_t s = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ id }, { Foo }},
        .events = {ecs_id(Position)},
        .callback = system_w_param_callback,
        .ctx = &ctx
    });

    ecs_emit(world, &(ecs_event_desc_t){
        .event = ecs_id(Position),
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .entity = e,
        .param = &p
    });

    test_int(ctx.invoked, 1);
    test_assert(ctx.system == s);
    test_assert(ctx.event == ecs_id(Position));
    test_assert(ctx.event_id == id);
    test_int(ctx.count, 1);
    test_assert(ctx.e[0] == e);
    test_assert(system_param == &p);

    ecs_fini(world);
}

void Event_emit_w_const_param(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t id = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id);

    Probe ctx = {0};
    Position p = {10, 20};

    ecs_entity_t s = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id,
        .events = {ecs_id(Position)},
        .callback = system_w_param_callback,
        .ctx = &ctx
    });

    ecs_emit(world, &(ecs_event_desc_t){
        .event = ecs_id(Position),
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .entity = e,
        .const_param = &p
    });

    test_int(ctx.invoked, 1);
    test_assert(ctx.system == s);
    test_assert(ctx.event == ecs_id(Position));
    test_assert(ctx.event_id == id);
    test_int(ctx.count, 1);
    test_assert(ctx.e[0] == e);
    test_assert(system_param == &p);

    ecs_fini(world);
}

static ecs_entity_t other = 0;

static void Nested1(ecs_iter_t *it) {
    int *ctx = it->ctx;
    (*ctx) ++;

    test_int(it->count, 1);

    if (it->entities[0] != other) {
        ecs_emit(it->world, &(ecs_event_desc_t) {
            .event = it->event,
            .ids = &(ecs_type_t){ .count = 1, .array = &it->event_id },
            .entity = other,
            .observable = it->real_world
        });
    }
}

static void Nested2(ecs_iter_t *it) {
    int *ctx = it->ctx;
    (*ctx) ++;
}

void Event_emit_nested(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Event);

    int ctx_1 = 0;
    int ctx_2 = 0;

    ecs_entity_t e1 = ecs_new(world);
    other = ecs_new(world);

    ecs_add(world, e1, Foo);
    ecs_add(world, e1, Bar);
    ecs_add(world, other, Foo);
    ecs_add(world, other, Bar);

    ecs_observer(world, {
        .entity = ecs_entity(world, { .name = "o1" }),
        .query.terms = {{ Foo }, { Bar }},
        .events = { Event },
        .ctx = &ctx_1,
        .callback = Nested1
    });

    ecs_observer(world, {
        .entity = ecs_entity(world, { .name = "o2" }),
        .query.terms = {{ Foo }, { Bar }},
        .events = { Event },
        .ctx = &ctx_2,
        .callback = Nested2
    });
    
    ecs_emit(world, &(ecs_event_desc_t) {
        .event = Event,
        .ids =  &(ecs_type_t){ .count = 1, .array = &Foo },
        .entity = e1,
        .observable = world
    });

    test_int(ctx_1, 2);
    test_int(ctx_2, 2);

    ecs_fini(world);
}

void Event_enqueue_event_1_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t evt = ecs_new(world);
    ecs_entity_t id = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id);

    Probe ctx = {0};

    ecs_entity_t s = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id,
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx
    });

    ecs_defer_begin(world);

    ecs_enqueue(world, &(ecs_event_desc_t){
        .event = evt,
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .entity = e
    });

    test_int(ctx.invoked, 0);

    ecs_defer_end(world);

    test_int(ctx.invoked, 1);
    test_assert(ctx.system == s);
    test_assert(ctx.event == evt);
    test_assert(ctx.event_id == id);
    test_int(ctx.count, 1);
    test_assert(ctx.e[0] == e);

    ecs_fini(world);
}

void Event_enqueue_event_2_ids(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t evt = ecs_new(world);
    ecs_entity_t id_a = ecs_new(world);
    ecs_entity_t id_b = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id_a);
    ecs_add_id(world, e, id_b);

    Probe *ctx_a = ecs_os_calloc_t(Probe);
    Probe *ctx_b = ecs_os_calloc_t(Probe);

    ecs_entity_t s_a = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id_a,
        .events = {evt},
        .callback = system_callback,
        .ctx = ctx_a
    });

    ecs_entity_t s_b = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id_b,
        .events = {evt},
        .callback = system_callback,
        .ctx = ctx_b
    });

    ecs_defer_begin(world);

    ecs_enqueue(world, &(ecs_event_desc_t){
        .event = evt,
        .ids = &(ecs_type_t){.count = 2, .array = (ecs_id_t[]){ id_a, id_b }},
        .entity = e
    });

    test_int(ctx_a->invoked, 0);
    test_int(ctx_b->invoked, 0);

    ecs_defer_end(world);

    test_int(ctx_a->invoked, 1);
    test_assert(ctx_a->system == s_a);
    test_assert(ctx_a->event == evt);
    test_assert(ctx_a->event_id == id_a);
    test_int(ctx_a->count, 1);
    test_assert(ctx_a->e[0] == e);

    test_int(ctx_b->invoked, 1);
    test_assert(ctx_b->system == s_b);
    test_assert(ctx_b->event == evt);
    test_assert(ctx_b->event_id == id_b);
    test_int(ctx_b->count, 1);
    test_assert(ctx_b->e[0] == e);

    ecs_os_free(ctx_a);
    ecs_os_free(ctx_b);

    ecs_fini(world);
}

void Event_enqueue_event_w_data(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t id = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id);

    Probe ctx = {0};
    Position p = {10, 20};

    ecs_entity_t s = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id,
        .events = {ecs_id(Position)},
        .callback = system_w_param_callback,
        .ctx = &ctx
    });

    ecs_defer_begin(world);

    ecs_enqueue(world, &(ecs_event_desc_t){
        .event = ecs_id(Position),
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .entity = e,
        .param = &p
    });

    test_int(ctx.invoked, 0);

    ecs_defer_end(world);

    test_int(ctx.invoked, 1);
    test_assert(ctx.system == s);
    test_assert(ctx.event == ecs_id(Position));
    test_assert(ctx.event_id == id);
    test_int(ctx.count, 1);
    test_assert(ctx.e[0] == e);
    test_assert(system_param != NULL);
    test_assert(system_param != &p);

    ecs_fini(world);
}

ECS_MOVE(Position, dst, src, {
    dst->x = src->x;
    dst->y = src->y;
    src->x = 0;
    src->y = 0;
})

static int copy_position = 0;

ECS_COPY(Position, dst, src, {
    dst->x = src->x;
    dst->y = src->y;
    copy_position ++;
})

static int dtor_position = 0;

ECS_DTOR(Position, ptr, {
    dtor_position ++;
})

void Event_enqueue_event_w_data_move(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .move = ecs_move(Position),
        .dtor = ecs_dtor(Position)
    });

    ecs_entity_t id = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id);

    Probe ctx = {0};
    Position p = {10, 20};

    ecs_entity_t s = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id,
        .events = {ecs_id(Position)},
        .callback = system_w_param_callback,
        .ctx = &ctx
    });

    ecs_defer_begin(world);

    ecs_enqueue(world, &(ecs_event_desc_t){
        .event = ecs_id(Position),
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .entity = e,
        .param = &p
    });

    test_int(ctx.invoked, 0);
    test_int(dtor_position, 0);
    test_int(p.x, 0);
    test_int(p.y, 0);

    ecs_defer_end(world);

    test_int(ctx.invoked, 1);
    test_assert(ctx.system == s);
    test_assert(ctx.event == ecs_id(Position));
    test_assert(ctx.event_id == id);
    test_int(ctx.count, 1);
    test_assert(ctx.e[0] == e);
    test_assert(system_param != NULL);
    test_assert(system_param != &p);
    test_int(dtor_position, 1);

    ecs_fini(world);
}

void Event_enqueue_event_w_data_copy(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .move = ecs_move(Position),
        .copy = ecs_copy(Position),
        .dtor = ecs_dtor(Position)
    });

    ecs_entity_t id = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id);

    Probe ctx = {0};
    Position p = {10, 20};

    ecs_entity_t s = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id,
        .events = {ecs_id(Position)},
        .callback = system_w_param_callback,
        .ctx = &ctx
    });

    ecs_defer_begin(world);

    ecs_enqueue(world, &(ecs_event_desc_t){
        .event = ecs_id(Position),
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .entity = e,
        .const_param = &p
    });

    test_int(ctx.invoked, 0);
    test_int(dtor_position, 0);
    test_int(p.x, 10);
    test_int(p.y, 20);
    test_int(copy_position, 1);

    ecs_defer_end(world);

    test_int(ctx.invoked, 1);
    test_assert(ctx.system == s);
    test_assert(ctx.event == ecs_id(Position));
    test_assert(ctx.event_id == id);
    test_int(ctx.count, 1);
    test_assert(ctx.e[0] == e);
    test_assert(system_param != NULL);
    test_assert(system_param != &p);
    test_int(dtor_position, 1);

    ecs_fini(world);
}

void Event_enqueue_event_w_const_data_no_copy(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t id = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id);

    Probe ctx = {0};
    Position p = {10, 20};

    ecs_entity_t s = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id,
        .events = {ecs_id(Position)},
        .callback = system_w_param_callback,
        .ctx = &ctx
    });

    ecs_defer_begin(world);

    ecs_enqueue(world, &(ecs_event_desc_t){
        .event = ecs_id(Position),
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .entity = e,
        .const_param = &p
    });

    test_int(ctx.invoked, 0);
    test_int(dtor_position, 0);
    test_int(p.x, 10);
    test_int(p.y, 20);

    ecs_defer_end(world);

    test_int(ctx.invoked, 1);
    test_assert(ctx.system == s);
    test_assert(ctx.event == ecs_id(Position));
    test_assert(ctx.event_id == id);
    test_int(ctx.count, 1);
    test_assert(ctx.e[0] == e);
    test_assert(system_param != NULL);
    test_assert(system_param != &p);

    ecs_fini(world);
}

void Event_enqueue_event_not_alive(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t evt = ecs_new(world);
    ecs_entity_t id = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id);

    Probe ctx = {0};

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id,
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx
    });

    ecs_defer_begin(world);

    ecs_delete(world, e);

    ecs_enqueue(world, &(ecs_event_desc_t){
        .event = evt,
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .entity = e
    });

    test_int(ctx.invoked, 0);

    ecs_defer_end(world);

    test_int(ctx.invoked, 0);
    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void Event_enqueue_event_not_alive_w_data_move(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .move = ecs_move(Position),
        .dtor = ecs_dtor(Position)
    });

    ecs_entity_t id = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id);

    Probe ctx = {0};
    Position p = {10, 20};

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id,
        .events = {ecs_id(Position)},
        .callback = system_w_param_callback,
        .ctx = &ctx
    });

    ecs_defer_begin(world);

    ecs_delete(world, e);

    ecs_enqueue(world, &(ecs_event_desc_t){
        .event = ecs_id(Position),
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .entity = e,
        .param = &p
    });

    test_int(ctx.invoked, 0);

    ecs_defer_end(world);

    test_int(ctx.invoked, 0);
    test_int(dtor_position, 1);

    ecs_fini(world);
}

void Event_enqueue_event_not_alive_w_data_copy(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .copy = ecs_copy(Position),
        .dtor = ecs_dtor(Position)
    });

    ecs_entity_t id = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id);

    Probe ctx = {0};
    Position p = {10, 20};

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id,
        .events = {ecs_id(Position)},
        .callback = system_w_param_callback,
        .ctx = &ctx
    });

    ecs_defer_begin(world);

    ecs_delete(world, e);

    ecs_enqueue(world, &(ecs_event_desc_t){
        .event = ecs_id(Position),
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .entity = e,
        .const_param = &p
    });

    test_int(ctx.invoked, 0);

    ecs_defer_end(world);

    test_int(ctx.invoked, 0);
    test_int(copy_position, 1);
    test_int(dtor_position, 1);

    ecs_fini(world);
}

static
void system_delete_callback(ecs_iter_t *it) {
    for (int i = 0; i < it->count; i ++) {
        ecs_delete(it->world, it->entities[i]);
    }
}

void Event_enqueue_event_not_alive_after_delete_during_merge(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t evt = ecs_new(world);
    ecs_entity_t delete_evt = ecs_new(world);
    ecs_entity_t id = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id);

    Probe ctx = {0};

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id,
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx
    });

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id,
        .events = {delete_evt},
        .callback = system_delete_callback,
    });

    ecs_defer_begin(world);

    ecs_enqueue(world, &(ecs_event_desc_t){
        .event = delete_evt,
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .entity = e
    });

    ecs_enqueue(world, &(ecs_event_desc_t){
        .event = evt,
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .entity = e
    });

    test_int(ctx.invoked, 0);

    ecs_defer_end(world);

    test_int(ctx.invoked, 0);
    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void Event_enqueue_event_not_alive_w_data_move_after_delete_during_merge(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .move = ecs_move(Position),
        .dtor = ecs_dtor(Position)
    });

    ecs_entity_t delete_evt = ecs_new(world);
    ecs_entity_t id = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id);

    Probe ctx = {0};
    Position p = {10, 20};

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id,
        .events = {ecs_id(Position)},
        .callback = system_w_param_callback,
        .ctx = &ctx
    });

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id,
        .events = {delete_evt},
        .callback = system_delete_callback,
    });

    ecs_defer_begin(world);

    ecs_enqueue(world, &(ecs_event_desc_t){
        .event = delete_evt,
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .entity = e
    });

    ecs_enqueue(world, &(ecs_event_desc_t){
        .event = ecs_id(Position),
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .entity = e,
        .param = &p
    });

    test_int(ctx.invoked, 0);
    test_int(dtor_position, 0);
    test_int(p.x, 0);
    test_int(p.y, 0);

    ecs_defer_end(world);

    test_int(ctx.invoked, 0);
    test_int(dtor_position, 1);

    ecs_fini(world);
}

void Event_enqueue_event_not_alive_w_data_copy_after_delete_during_merge(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .copy = ecs_copy(Position),
        .dtor = ecs_dtor(Position)
    });

    ecs_entity_t delete_evt = ecs_new(world);
    ecs_entity_t id = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id);

    Probe ctx = {0};
    Position p = {10, 20};

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id,
        .events = {ecs_id(Position)},
        .callback = system_w_param_callback,
        .ctx = &ctx
    });

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id,
        .events = {delete_evt},
        .callback = system_delete_callback,
    });

    ecs_defer_begin(world);

    ecs_enqueue(world, &(ecs_event_desc_t){
        .event = delete_evt,
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .entity = e
    });

    ecs_enqueue(world, &(ecs_event_desc_t){
        .event = ecs_id(Position),
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .entity = e,
        .param = &p
    });

    test_int(ctx.invoked, 0);
    test_int(dtor_position, 0);
    test_int(copy_position, 0);
    test_int(p.x, 10);
    test_int(p.y, 20);

    ecs_defer_end(world);

    test_int(ctx.invoked, 0);
    test_int(dtor_position, 1);

    ecs_fini(world);
}

void Event_enqueue_event_not_deferred(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t evt = ecs_new(world);
    ecs_entity_t id = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id);
    ecs_table_t *table = ecs_get_table(world, e);

    Probe ctx = {0};

    ecs_entity_t s = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id,
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx
    });

    ecs_enqueue(world, &(ecs_event_desc_t){
        .event = evt,
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .table = table,
        .observable = world
    });

    test_int(ctx.invoked, 1);
    test_assert(ctx.system == s);
    test_assert(ctx.event == evt);
    test_assert(ctx.event_id == id);
    test_int(ctx.count, 1);
    test_assert(ctx.e[0] == e);

    ecs_fini(world);
}

void Event_enqueue_event_not_deferred_to_async(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t evt = ecs_new(world);
    ecs_entity_t id = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id);

    Probe ctx = {0};

    ecs_entity_t s = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id,
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx
    });

    ecs_world_t *my_stage = ecs_stage_new(world);

    ecs_enqueue(my_stage, &(ecs_event_desc_t){
        .event = evt,
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .entity = e
    });

    test_int(ctx.invoked, 0);

    ecs_merge(my_stage);

    test_int(ctx.invoked, 1);
    test_assert(ctx.system == s);
    test_assert(ctx.event == evt);
    test_assert(ctx.event_id == id);
    test_int(ctx.count, 1);
    test_assert(ctx.e[0] == e);

    ecs_stage_free(my_stage);

    ecs_fini(world);
}

void Event_enqueue_custom_implicit_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t MyEvent = ecs_new(world);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);

    ecs_observer(world, {
        .query.terms = {{ .id = EcsAny, .src.id = e1 }},
        .events = {MyEvent},
        .callback = ObserverA
    });

    ecs_observer(world, {
        .query.terms = {{ .id = EcsAny, .src.id = e2 }},
        .events = {MyEvent},
        .callback = ObserverB
    });

    ecs_defer_begin(world);

    ecs_enqueue(world, &(ecs_event_desc_t) {
        .entity = e1,
        .event = MyEvent
    });

    test_int(ObserverA_invoked, 0);
    test_int(ObserverB_invoked, 0);

    ecs_defer_end(world);
    
    test_int(ObserverA_invoked, 1);
    test_int(ObserverB_invoked, 0);

    ecs_defer_begin(world);

    ecs_enqueue(world, &(ecs_event_desc_t) {
        .entity = e2,
        .event = MyEvent
    });

    test_int(ObserverA_invoked, 1);
    test_int(ObserverB_invoked, 0);

    ecs_defer_end(world);

    test_int(ObserverA_invoked, 1);
    test_int(ObserverB_invoked, 1);

    ecs_fini(world);
}

typedef struct LargeType {
    char value[8888];
} LargeType;

static int on_position_invoked = 0;

static
void OnPosition(ecs_iter_t *it) {
    Position *p = it->param;
    test_int(p->x, 10);
    test_int(p->y, 20);
    on_position_invoked ++;
}

void Event_enqueue_custom_after_large_cmd(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, LargeType);
    ECS_COMPONENT(world, Position);

    ECS_OBSERVER(world, OnPosition, ecs_id(Position), LargeType);

    ecs_defer_begin(world);
    ecs_entity_t e = ecs_insert(world, ecs_value(LargeType, {{0}}));
    ecs_defer_end(world);
    test_assert(ecs_has(world, e, LargeType));

    ecs_defer_begin(world);
    Position p = {10, 20};
    ecs_enqueue(world, &(ecs_event_desc_t) {
        .entity = e,
        .ids = &(ecs_type_t){ .count = 1, .array = (ecs_id_t[]){ ecs_id(LargeType) }},
        .event = ecs_id(Position),
        .param = &p,
    });

    test_int(on_position_invoked, 0);
    ecs_defer_end(world);
    test_int(on_position_invoked, 1);

    ecs_fini(world);
}

void Event_enqueue_on_readonly_world(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t evt = ecs_new(world);
    ecs_entity_t id = ecs_new(world);
    ecs_entity_t e = ecs_new_w_id(world, id);

    Probe ctx = {0};

    ecs_entity_t s = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = id,
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx
    });

    ecs_readonly_begin(world, false);

    ecs_enqueue(world, &(ecs_event_desc_t){
        .event = evt,
        .ids = &(ecs_type_t){.count = 1, .array = (ecs_id_t[]){ id }},
        .entity = e
    });

    test_int(ctx.invoked, 0);

    ecs_readonly_end(world);

    test_int(ctx.invoked, 1);
    test_assert(ctx.system == s);
    test_assert(ctx.event == evt);
    test_assert(ctx.event_id == id);
    test_int(ctx.count, 1);
    test_assert(ctx.e[0] == e);

    ecs_fini(world);
}
