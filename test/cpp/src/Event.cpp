#include <cpp.h>

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
        .with(id)
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
        .with(id_a)
        .each([&](flecs::entity e) {
            test_assert(e == e1);
            count ++;
        });

    ecs.observer()
        .event(evt)
        .with(id_b)
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
        .with(id)
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
        .with(id_a)
        .each([&](flecs::entity e) {
            test_assert(e == e1);
            count ++;
        });

    ecs.observer()
        .event(evt)
        .with(id_b)
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
        .with(id)
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
        .with<IdA>()
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
        .with<IdA>()
        .each([&](flecs::entity e) {
            test_assert(e == e1);
            count ++;
        });

    ecs.observer()
        .event<Evt>()
        .with<IdB>()
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
        .with(id)
        .run([&](flecs::iter& it) {
            while (it.next()) {
                test_assert(it.entity(0) == e1);
                test_int(it.param<EvtData>()->value, 10);
                count ++;
            }
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
        .with(id)
        .run([&](flecs::iter& it) {
            while (it.next()) {
                test_assert(it.entity(0) == e1);
                test_int(it.param<EvtData>()->value, 10);
                count ++;
            }
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
        .with(id)
        .run([&](flecs::iter& it) {
            while (it.next()) {
                test_assert(it.entity(0) == e1);
                test_int(it.param<EvtData>()->value, 10);
                count ++;
            }
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
        .with(rel, obj)
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
        .with<IdA>(obj)
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
        .with<IdA, IdB>()
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
        .with<Tag>()
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
        .with<Tag>()
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
    e.observe(evt, [&](flecs::entity src) {
        count ++;
        test_assert(src == e);
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
    e.observe<Evt>([&](flecs::entity src) {
        count ++;
        test_assert(src == e);
    });

    test_int(count, 0);

    e.emit<Evt>();

    test_int(count, 1);
}

void Event_entity_emit_event_w_payload(void) {
    flecs::world ecs;

    flecs::entity e = ecs.entity()
        .add<Tag>();

    int32_t count = 0;
    e.observe<Position>([&](flecs::entity src, Position& p) {
        count ++;
        test_assert(src == e);
        test_int(p.x, 10);
        test_int(p.y, 20);
    });

    test_int(count, 0);

    e.emit<Position>({10, 20});

    test_int(count, 1);
}

void Event_entity_emit_event_id_no_src(void) {
    flecs::world ecs;

    flecs::entity evt = ecs.entity();

    flecs::entity e = ecs.entity()
        .add<Tag>();

    int32_t count = 0;
    e.observe(evt, [&]() {
        count ++;
    });

    test_int(count, 0);

    e.emit(evt);

    test_int(count, 1);
}

void Event_entity_emit_event_type_no_src(void) {
    flecs::world ecs;

    flecs::entity e = ecs.entity()
        .add<Tag>();

    int32_t count = 0;
    e.observe<Evt>([&]() {
        count ++;
    });

    test_int(count, 0);

    e.emit<Evt>();

    test_int(count, 1);
}

void Event_entity_emit_event_w_payload_no_src(void) {
    flecs::world ecs;

    flecs::entity e = ecs.entity()
        .add<Tag>();

    int32_t count = 0;
    e.observe<Position>([&](Position& p) {
        count ++;
        test_int(p.x, 10);
        test_int(p.y, 20);
    });

    test_int(count, 0);

    e.emit<Position>({10, 20});

    test_int(count, 1);
}

void Event_entity_emit_event_w_payload_derived_event_type(void) {
    flecs::world ecs;

    flecs::entity e = ecs.entity()
        .add<Tag>();

    int32_t count = 0;
    e.observe([&](flecs::entity src, Position& p) {
        count ++;
        test_assert(src == e);
        test_int(p.x, 10);
        test_int(p.y, 20);
    });

    test_int(count, 0);

    e.emit<Position>({10, 20});

    test_int(count, 1);
}

void Event_entity_emit_event_w_payload_derived_event_type_no_src(void) {
    flecs::world ecs;

    flecs::entity e = ecs.entity()
        .add<Tag>();

    int32_t count = 0;
    e.observe([&](Position& p) {
        count ++;
        test_int(p.x, 10);
        test_int(p.y, 20);
    });

    test_int(count, 0);

    e.emit<Position>({10, 20});

    test_int(count, 1);
}

// Generic lambdas are a C++14 feature.

struct GenericLambdaObserveEntityPayload {
    template <typename E, typename P>
    void operator()(E&&, P&&) const {
        static_assert(flecs::is_same<E, flecs::entity>(), "");
        static_assert(flecs::is_same<P, Position&>(), "");
    }
};

struct GenericLambdaObservePayload {
    template <typename P>
    void operator()(P&&) const {
        static_assert(flecs::is_same<P, Position&>(), "");
    }
};

struct GenericLambdaObserveEntity {
    template <typename E>
    void operator()(E&&) const {
        static_assert(flecs::is_same<E, flecs::entity>(), "");
    }
};

void Event_entity_observe_generic(void) {
    flecs::world ecs;

    flecs::entity e = ecs.entity();

    e.observe<Position>(GenericLambdaObserveEntityPayload{});
    e.observe<Position>(GenericLambdaObservePayload{});
    e.observe<Evt>(GenericLambdaObserveEntity{});
}

void Event_enqueue_event(void) {
    flecs::world ecs;
    
    int32_t count = 0;

    flecs::entity evt = ecs.entity();
    flecs::entity id_a = ecs.entity();
    flecs::entity e1 = ecs.entity().add(id_a);

    ecs.observer()
        .event(evt)
        .with(id_a)
        .each([&](flecs::entity e) {
            test_assert(e == e1);
            count ++;
        });

    ecs.defer_begin();

    ecs.event(evt)
        .id(id_a)
        .entity(e1)
        .enqueue();

    test_int(count, 0);

    ecs.defer_end();

    test_int(count, 1);
}

void Event_enqueue_entity_event(void) {
    flecs::world ecs;
    
    int32_t count = 0;

    flecs::entity evt = ecs.entity();
    flecs::entity id_a = ecs.entity();
    flecs::entity e1 = ecs.entity().add(id_a);

    e1.observe(evt, [&]() {
        count ++;
    });

    ecs.defer_begin();

    e1.enqueue(evt);

    test_int(count, 0);

    ecs.defer_end();

    test_int(count, 1);
}

void Event_enqueue_event_w_payload(void) {
    flecs::world ecs;
    
    int32_t count = 0;

    flecs::entity id_a = ecs.entity();
    flecs::entity e1 = ecs.entity().add(id_a);

    ecs.observer()
        .event<Position>()
        .with(id_a)
        .each([&](flecs::iter& it, size_t i) {
            test_assert(it.entity(i) == e1);
            test_int(it.param<Position>()->x, 10);
            test_int(it.param<Position>()->y, 20);
            count ++;
        });

    ecs.defer_begin();

    ecs.event<Position>()
        .id(id_a)
        .entity(e1)
        .ctx(Position{10, 20})
        .enqueue();

    test_int(count, 0);

    ecs.defer_end();

    test_int(count, 1);
}

void Event_enqueue_entity_event_w_payload(void) {
    flecs::world ecs;
    
    int32_t count = 0;

    flecs::entity id_a = ecs.entity();
    flecs::entity e1 = ecs.entity().add(id_a);

    e1.observe<Position>([&](Position& p) {
        test_int(p.x, 10);
        test_int(p.y, 20);
        count ++;
    });

    ecs.defer_begin();

    e1.enqueue<Position>({10, 20});

    test_int(count, 0);

    ecs.defer_end();

    test_int(count, 1);
}

void Event_enqueue_entity_from_readonly_world(void) {
    flecs::world ecs;
    
    int32_t count = 0;

    flecs::entity evt = ecs.entity();
    flecs::entity id_a = ecs.entity();
    flecs::entity e1 = ecs.entity().add(id_a);

    e1.observe(evt, [&]() {
        count ++;
    });

    ecs.readonly_begin();

    e1.enqueue(evt);

    test_int(count, 0);

    ecs.readonly_end();

    test_int(count, 1);
}

void Event_enqueue_entity_w_payload_from_readonly_world(void) {
    flecs::world ecs;
    
    int32_t count = 0;

    flecs::entity id_a = ecs.entity();
    flecs::entity e1 = ecs.entity().add(id_a);

    e1.observe<Position>([&](Position& p) {
        test_int(p.x, 10);
        test_int(p.y, 20);
        count ++;
    });

    ecs.readonly_begin();

    e1.enqueue<Position>({10, 20});

    test_int(count, 0);

    ecs.readonly_end();

    test_int(count, 1);
}

enum Type { A, B = 42, C };
void Event_enum_event(void) {
	struct Event {};
 
 
    struct Data {
        Type type;
        int value;
    };


    struct {
        size_t _any = 0u;
        size_t type_a = 0u;
        size_t type_b = 0u;
        size_t data = 0u;
        size_t data_type_a = 0u;
        size_t data_type_b = 0u;
    } observed;

    flecs::world ecs;

    ecs.component<Event>();
    ecs.component<Type>();
    ecs.component<Data>();

    ecs.observer()
        .with(flecs::Wildcard)
        .event<Event>()
        .each([&](flecs::entity) {
            ++observed._any;
        });

    ecs.observer()
        .with(Type::A)
        .event<Event>()
        .each([&](flecs::entity) {
            ++observed.type_a;
        });

    ecs.observer()
        .with(Type::B)
        .event<Event>()
        .each([&](flecs::entity) {
            ++observed.type_b;
        });

    ecs.observer<Data>()
        .event<Event>()
        .each([&](flecs::entity, Data data) {
            ++observed.data;
            test_assert(data.value >= 1 && data.value <= 2);
        });
 
 
    ecs.observer<Data>()
        .with(Type::A)
        .event<Event>()
        .each([&](flecs::entity, Data data) {
            ++observed.data_type_a;
            test_int(data.value, 1);
        });

    ecs.observer<Data>()
        .with(Type::B)
        .event<Event>()
        .each([&](flecs::entity, Data data) {
            ++observed.data_type_b;
            test_int(data.value, 2);
        });

    {
        auto event1 = ecs.entity()
            .add(Type::A)
            .emplace<Data>(Type::A, 1);

        ecs.event<Event>()
            .id(Type::A)
            .id<Data>()
            .entity(event1).emit();
    }

    {
        auto event2 = ecs.entity()
            .add(Type::B)
            .emplace<Data>(Type::B, 2);

        ecs.event<Event>()
            .id(Type::B)
            .id<Data>()
            .entity(event2).emit();
    }

    test_int(observed._any, 2u);
    test_int(observed.type_a, 1u);
    test_int(observed.type_b, 1u);
    test_int(observed.data_type_a, 1u);
    test_int(observed.data_type_b, 1u);
}
