#include <api.h>

static
void system_callback(ecs_iter_t *it) {
    probe_iter(it);
}

void Event_table_1_id_w_trigger() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t evt = ecs_new_id(world);
    ecs_entity_t id = ecs_new_id(world);
    ecs_entity_t e = ecs_new_w_id(world, id);
    ecs_table_t *table = ecs_get_table(world, e);

    Probe ctx = {0};

    ecs_entity_t s = ecs_trigger_init(world, &(ecs_trigger_desc_t) {
        .term.id = id,
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx
    });

    ecs_emit(world, &(ecs_event_desc_t) {
        .event = evt,
        .ids = &(ecs_ids_t){.count = 1, .array = (ecs_id_t[]){ id }},
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

void Event_table_2_ids_w_trigger() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t evt = ecs_new_id(world);
    ecs_entity_t id_a = ecs_new_id(world);
    ecs_entity_t id_b = ecs_new_id(world);
    ecs_entity_t e = ecs_new_w_id(world, id_a);
    ecs_add_id(world, e, id_b);
    ecs_table_t *table = ecs_get_table(world, e);

    Probe ctx = {0};

    ecs_entity_t s_a = ecs_trigger_init(world, &(ecs_trigger_desc_t) {
        .term.id = id_a,
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx,
    });

    ecs_emit(world, &(ecs_event_desc_t) {
        .event = evt,
        .ids = &(ecs_ids_t){.count = 2, .array = (ecs_id_t[]){ id_a, id_b }},
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

    ecs_entity_t s_b = ecs_trigger_init(world, &(ecs_trigger_desc_t) {
        .term.id = id_b,
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx
    });

    ecs_os_zeromem(&ctx);

    ecs_emit(world, &(ecs_event_desc_t) {
        .event = evt,
        .ids = &(ecs_ids_t){.count = 2, .array = (ecs_id_t[]){ id_a, id_b }},
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

void Event_table_1_id_w_observer() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t evt = ecs_new_id(world);
    ecs_entity_t id = ecs_new_id(world);
    ecs_entity_t e = ecs_new_w_id(world, id);
    ecs_table_t *table = ecs_get_table(world, e);

    Probe ctx = {0};

    ecs_entity_t s = ecs_observer_init(world, &(ecs_observer_desc_t) {
        .filter.terms = {{ id }},
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx
    });

    ecs_emit(world, &(ecs_event_desc_t) {
        .event = evt,
        .ids = &(ecs_ids_t){.count = 1, .array = (ecs_id_t[]){ id }},
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

void Event_table_2_ids_w_observer() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t evt = ecs_new_id(world);
    ecs_entity_t id_a = ecs_new_id(world);
    ecs_entity_t id_b = ecs_new_id(world);
    ecs_entity_t e = ecs_new_w_id(world, id_a);
    ecs_add_id(world, e, id_b);
    ecs_table_t *table = ecs_get_table(world, e);

    Probe ctx = {0};

    ecs_entity_t s_a = ecs_observer_init(world, &(ecs_observer_desc_t) {
        .filter.terms = {{ id_a }},
        .events = {evt},
        .callback = system_callback,
        .ctx = &ctx
    });

    ecs_emit(world, &(ecs_event_desc_t) {
        .event = evt,
        .ids = &(ecs_ids_t){.count = 2, .array = (ecs_id_t[]){ id_a, id_b }},
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
