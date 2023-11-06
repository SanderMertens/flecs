#include <cpp_api.h>

struct Evt { };

struct IdA { };
struct IdB { };

void Event_evt_1_id_entity(void) {
    flecs::world ecs;

    auto evt = ecs.entity();
    auto id = ecs.entity();
    auto e1 = ecs.entity().add(id);

    int32_t count = 0;

    ecs.observer()
        .event(evt)
        .term(id)
        .each([&](flecs::entity e) {
            test_assert(e == e1);
            count ++;
        });

    ecs.event(evt)
        .id(id)
        .entity(e1)
        .emit();

    test_int(count, 1);
}

void Event_evt_2_ids_entity(void) {
    flecs::world ecs;

    auto evt = ecs.entity();
    auto id_a = ecs.entity();
    auto id_b = ecs.entity();
    auto e1 = ecs.entity().add(id_a).add(id_b);

    int32_t count = 0;

    ecs.observer()
        .event(evt)
        .term(id_a)
        .each([&](flecs::entity e) {
            test_assert(e == e1);
            count ++;
        });

    ecs.observer()
        .event(evt)
        .term(id_b)
        .each([&](flecs::entity e) {
            test_assert(e == e1);
            count ++;
        });

    ecs.event(evt)
        .id(id_a)
        .id(id_b)
        .entity(e1)
        .emit();

    test_int(count, 2);
}

void Event_evt_1_id_table(void) {
    flecs::world ecs;

    auto evt = ecs.entity();
    auto id = ecs.entity();
    auto e1 = ecs.entity().add(id);

    auto table = e1.table();

    int32_t count = 0;

    ecs.observer()
        .event(evt)
        .term(id)
        .each([&](flecs::entity e) {
            test_assert(e == e1);
            count ++;
        });

    ecs.event(evt)
        .id(id)
        .table(table)
        .emit();

    test_int(count, 1);
}

void Event_evt_2_ids_table(void) {
    flecs::world ecs;

    auto evt = ecs.entity();
    auto id_a = ecs.entity();
    auto id_b = ecs.entity();
    auto e1 = ecs.entity().add(id_a).add(id_b);
    auto table = e1.table();

    int32_t count = 0;

    ecs.observer()
        .event(evt)
        .term(id_a)
        .each([&](flecs::entity e) {
            test_assert(e == e1);
            count ++;
        });

    ecs.observer()
        .event(evt)
        .term(id_b)
        .each([&](flecs::entity e) {
            test_assert(e == e1);
            count ++;
        });

    ecs.event(evt)
        .id(id_a)
        .id(id_b)
        .table(table)
        .emit();

    test_int(count, 2);
}

void Event_evt_type(void) {
    flecs::world ecs;

    auto id = ecs.entity();
    auto e1 = ecs.entity().add(id);

    int32_t count = 0;

    ecs.observer()
        .event<Evt>()
        .term(id)
        .each([&](flecs::entity e) {
            test_assert(e == e1);
            count ++;
        });

    ecs.event<Evt>()
        .id(id)
        .entity(e1)
        .emit();

    test_int(count, 1);
}

void Event_evt_1_component(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().add<IdA>();

    int32_t count = 0;

    ecs.observer()
        .event<Evt>()
        .term<IdA>()
        .each([&](flecs::entity e) {
            test_assert(e == e1);
            count ++;
        });

    ecs.event<Evt>()
        .id<IdA>()
        .entity(e1)
        .emit();

    test_int(count, 1);
}

void Event_evt_2_components(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().add<IdA>().add<IdB>();

    int32_t count = 0;

    ecs.observer()
        .event<Evt>()
        .term<IdA>()
        .each([&](flecs::entity e) {
            test_assert(e == e1);
            count ++;
        });

    ecs.observer()
        .event<Evt>()
        .term<IdB>()
        .each([&](flecs::entity e) {
            test_assert(e == e1);
            count ++;
        });

    ecs.event<Evt>()
        .id<IdA>()
        .id<IdB>()
        .entity(e1)
        .emit();

    test_int(count, 2);
}

struct EvtData {
    int value;
};

void Event_evt_void_ctx(void) {
    flecs::world ecs;

    auto evt = ecs.entity();
    auto id = ecs.entity();
    auto e1 = ecs.entity().add(id);

    int32_t count = 0;

    ecs.observer()
        .event(evt)
        .term(id)
        .iter([&](flecs::iter& it) {
            test_assert(it.entity(0) == e1);
            test_int(it.param<EvtData>()->value, 10);
            count ++;
        });

    EvtData data = {10};

    ecs.event(evt)
        .id(id)
        .entity(e1)
        .ctx(&data)
        .emit();

    test_int(count, 1);
}

void Event_evt_typed_ctx(void) {
    flecs::world ecs;

    auto id = ecs.entity();
    auto e1 = ecs.entity().add(id);

    int32_t count = 0;

    ecs.observer()
        .event<EvtData>()
        .term(id)
        .iter([&](flecs::iter& it) {
            test_assert(it.entity(0) == e1);
            test_int(it.param<EvtData>()->value, 10);
            count ++;
        });

    ecs.event<EvtData>()
        .id(id)
        .entity(e1)
        .ctx(EvtData{10})
        .emit();

    test_int(count, 1);
}

void Event_evt_implicit_typed_ctx(void) {
    flecs::world ecs;

    auto id = ecs.entity();
    auto e1 = ecs.entity().add(id);

    int32_t count = 0;

    ecs.observer()
        .event<EvtData>()
        .term(id)
        .iter([&](flecs::iter& it) {
            test_assert(it.entity(0) == e1);
            test_int(it.param<EvtData>()->value, 10);
            count ++;
        });

    ecs.event<EvtData>()
        .id(id)
        .entity(e1)
        .ctx({10})
        .emit();

    test_int(count, 1);
}

void Event_evt_1_id_pair_rel_id_obj_id_entity(void) {
    flecs::world ecs;

    auto evt = ecs.entity();
    auto rel = ecs.entity();
    auto obj = ecs.entity();
    auto e1 = ecs.entity().add(rel, obj);

    int32_t count = 0;

    ecs.observer()
        .event(evt)
        .term(rel, obj)
        .each([&](flecs::entity e) {
            test_assert(e == e1);
            count ++;
        });

    ecs.event(evt)
        .id(rel, obj)
        .entity(e1)
        .emit();

    test_int(count, 1);
}

void Event_evt_1_id_pair_rel_obj_id_entity(void) {
    flecs::world ecs;

    auto evt = ecs.entity();
    auto obj = ecs.entity();
    auto e1 = ecs.entity().add<IdA>(obj);

    int32_t count = 0;

    ecs.observer()
        .event(evt)
        .term<IdA>(obj)
        .each([&](flecs::entity e) {
            test_assert(e == e1);
            count ++;
        });

    ecs.event(evt)
        .id<IdA>(obj)
        .entity(e1)
        .emit();

    test_int(count, 1);
}

void Event_evt_1_id_pair_rel_obj_entity(void) {
    flecs::world ecs;

    auto evt = ecs.entity();
    auto e1 = ecs.entity().add<IdA, IdB>();

    int32_t count = 0;

    ecs.observer()
        .event(evt)
        .term<IdA, IdB>()
        .each([&](flecs::entity e) {
            test_assert(e == e1);
            count ++;
        });

    ecs.event(evt)
        .id<IdA, IdB>()
        .entity(e1)
        .emit();

    test_int(count, 1);
}

void Event_emit_staged_from_world(void) {
    flecs::world ecs;

    auto evt = ecs.entity();
    auto e1 = ecs.entity().add<Tag>();

    int32_t count = 0;

    ecs.observer()
        .event(evt)
        .term<Tag>()
        .each([&](flecs::entity e) {
            test_assert(e == e1);
            count ++;
        });

    ecs.readonly_begin();

    ecs.event(evt)
        .id<Tag>()
        .entity(e1)
        .emit();

    ecs.readonly_end();

    test_int(count, 1);
}

void Event_emit_staged_from_stage(void) {
    flecs::world ecs;

    auto evt = ecs.entity();
    auto e1 = ecs.entity().add<Tag>();

    int32_t count = 0;

    ecs.observer()
        .event(evt)
        .term<Tag>()
        .each([&](flecs::entity e) {
            test_assert(e == e1);
            count ++;
        });

    ecs.readonly_begin();

    ecs.get_stage(0).event(evt)
        .id<Tag>()
        .entity(e1)
        .emit();

    ecs.readonly_end();

    test_int(count, 1);
}

void Event_emit_custom_for_any(void) {
    flecs::world ecs;

    int count_a = 0;
    int count_b = 0;

    flecs::entity e1 = ecs.entity().add<Tag>();
    flecs::entity e2 = ecs.entity().add<Tag>();

    ecs.observer()
        .event<Evt>()
        .with(flecs::Any).src(e1)
        .each([&](flecs::entity e) {
            test_assert(e == 0);
            count_a ++;
        });

    ecs.observer()
        .event<Evt>()
        .with(flecs::Any).src(e2)
        .each([&](flecs::entity e) {
            test_assert(e == 0);
            count_b ++;
        });

    ecs.event<Evt>()
        .id(flecs::Any)
        .entity(e1)
        .emit();

    test_int(count_a, 1);
    test_int(count_b, 0);

    ecs.event<Evt>()
        .id(flecs::Any)
        .entity(e2)
        .emit();

    test_int(count_a, 1);
    test_int(count_b, 1);
}

void Event_entity_emit_event_id(void) {
    flecs::world ecs;

    flecs::entity evt = ecs.entity();

    flecs::entity e = ecs.entity()
        .add<Tag>();

    int32_t count = 0;
    e.observe(evt, [&](flecs::iter& it) {
        count ++;
        test_assert(it.event() == evt);
        test_assert(it.src(1) == e);
    });

    test_int(count, 0);

    e.emit(evt);

    test_int(count, 1);
}

void Event_entity_emit_event_type(void) {
    flecs::world ecs;

    flecs::entity e = ecs.entity()
        .add<Tag>();

    int32_t count = 0;
    e.observe<Evt>([&](flecs::iter& it) {
        count ++;
        test_assert(it.event() == ecs.id<Evt>());
        test_assert(it.src(1) == e);
    });

    test_int(count, 0);

    e.emit<Evt>();

    test_int(count, 1);
}
