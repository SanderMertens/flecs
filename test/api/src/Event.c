#include <api.h>

static
void system_callback(ecs_iter_t *it) {
    probe_iter(it);
}

void Event_table_1_id_w_trigger(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t evt = ecs_new_id(world);
    ecs_entity_t id = ecs_new_id(world);
    ecs_entity_t e = ecs_new_w_id(world, id);
    ecs_table_t *table = ecs_get_table(world, e);

    Probe ctx = {0};

    ecs_entity_t s = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = id,
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

    ecs_entity_t evt = ecs_new_id(world);
    ecs_entity_t id_a = ecs_new_id(world);
    ecs_entity_t id_b = ecs_new_id(world);
    ecs_entity_t e = ecs_new_w_id(world, id_a);
    ecs_add_id(world, e, id_b);
    ecs_table_t *table = ecs_get_table(world, e);

    Probe ctx = {0};

    ecs_entity_t s_a = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = id_a,
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
        .filter.terms[0].id = id_b,
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

    ecs_entity_t evt = ecs_new_id(world);
    ecs_entity_t id = ecs_new_id(world);
    ecs_entity_t e = ecs_new_w_id(world, id);
    ecs_table_t *table = ecs_get_table(world, e);

    Probe ctx = {0};

    ecs_entity_t s = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ id }},
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

    ecs_entity_t evt = ecs_new_id(world);
    ecs_entity_t id_a = ecs_new_id(world);
    ecs_entity_t id_b = ecs_new_id(world);
    ecs_entity_t e = ecs_new_w_id(world, id_a);
    ecs_add_id(world, e, id_b);
    ecs_table_t *table = ecs_get_table(world, e);

    Probe ctx = {0};

    ecs_entity_t s_a = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ id_a }},
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

    ecs_entity_t evt = ecs_new_id(world);

    ecs_entity_t e = ecs_new(world, TagA);
    ecs_table_t *table = ecs_get_table(world, e);
    ecs_delete(world, e);

    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ TagA }},
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

    ecs_entity_t base = ecs_new_w_id(world, TagA);
    ecs_table_t *base_table = ecs_get_table(world, base);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, EcsIsA, base);

    Probe ctx_a = {0};
    Probe ctx_b = {0};

    ecs_entity_t evt = ecs_new_id(world);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .filter.terms[0].src.flags = EcsSelf,
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx_a
    });

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = TagA,
        .filter.terms[0].src.flags = EcsUp,
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

    ecs_entity_t evt = ecs_new_id(world);
    ecs_entity_t id = ecs_new_id(world);
    ecs_entity_t e = ecs_new_w_id(world, id);
    ecs_table_t *table = ecs_get_table(world, e);

    Probe ctx = {0};

    ecs_entity_t s = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = id,
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx
    });
    ecs_readonly_begin(world);
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

    ecs_entity_t evt = ecs_new_id(world);
    ecs_entity_t id = ecs_new_id(world);
    ecs_entity_t e = ecs_new_w_id(world, id);
    ecs_table_t *table = ecs_get_table(world, e);

    Probe ctx = {0};

    ecs_entity_t s = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = id,
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx
    });
    ecs_readonly_begin(world);
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

    ecs_entity_t evt = ecs_new_id(world);
    ecs_entity_t id = ecs_new_id(world);
    ecs_entity_t e = ecs_new_w_id(world, id);
    ecs_table_t *table = ecs_get_table(world, e);

    Probe ctx = {0};

    ecs_entity_t s = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ id }},
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx
    });
    ecs_readonly_begin(world);
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

    ecs_entity_t evt = ecs_new_id(world);
    ecs_entity_t id = ecs_new_id(world);
    ecs_entity_t e = ecs_new_w_id(world, id);
    ecs_table_t *table = ecs_get_table(world, e);

    Probe ctx = {0};

    ecs_entity_t s = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms = {{ id }},
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx
    });

    ecs_readonly_begin(world);
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

    ecs_entity_t evt = ecs_new_id(world);
    ecs_entity_t id = ecs_new_id(world);
    ecs_entity_t e = ecs_new_w_id(world, id);

    Probe ctx = {0};

    ecs_entity_t s = ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0].id = id,
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

    ecs_entity_t MyEvent = ecs_new_id(world);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);

    ecs_observer(world, {
        .filter.terms = {{ .id = EcsAny, .src.id = e1 }},
        .events = {MyEvent},
        .callback = ObserverA
    });

    ecs_observer(world, {
        .filter.terms = {{ .id = EcsAny, .src.id = e2 }},
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

    ecs_entity_t MyEvent = ecs_new_id(world);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);

    ecs_observer(world, {
        .filter.terms = {{ .id = EcsAny, .src.id = e1 }},
        .events = {MyEvent},
        .callback = ObserverA
    });

    ecs_observer(world, {
        .filter.terms = {{ .id = EcsAny, .src.id = e2 }},
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

    ecs_entity_t MyEvent = ecs_new_id(world);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);

    ecs_observer(world, {
        .filter.terms = {{ .id = EcsAny, .src.id = e1 }},
        .events = {MyEvent},
        .callback = ObserverA
    });

    ecs_observer(world, {
        .filter.terms = {{ .id = EcsAny, .src.id = e2 }},
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
