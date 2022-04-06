#include <cpp_api.h>

void Meta_struct() {
    flecs::world ecs;

    struct Test {
        int32_t a;
        float b;
    };

    auto c = ecs.component<Test>()
        .member<int32_t>("a")
        .member<float>("b");    
    test_assert(c != 0);
    
    auto a = c.lookup("a");
    test_assert(a != 0);
    test_assert( a.has<flecs::Member>() );
    const flecs::Member *m = a.get<flecs::Member>();
    test_uint(m->type, flecs::I32);

    auto b = c.lookup("b");
    test_assert(b != 0);
    test_assert( b.has<flecs::Member>() );
    m = b.get<flecs::Member>();
    test_uint(m->type, flecs::F32);
}

void Meta_nested_struct() {
    flecs::world ecs;

    struct Test {
        int32_t x;
    };

    struct Nested {
        Test a;
    };

    auto t = ecs.component<Test>()
        .member<int32_t>("x");

    auto n = ecs.component<Nested>()
        .member(t, "a");
    test_assert(n != 0);
    
    auto a = n.lookup("a");
    test_assert(a != 0);
    test_assert( a.has<flecs::Member>() );
    const flecs::Member *m = a.get<flecs::Member>();
    test_uint(m->type, t);
}

void Meta_struct_w_portable_type() {
    flecs::world ecs;

    struct Test {
        uintptr_t a;
        uintptr_t b;
        flecs::entity_t c;
        flecs::entity_t d;
    };

    auto t = ecs.component<Test>()
        .member<uintptr_t>("a")
        .member(flecs::Uptr, "b")
        .member<flecs::entity_t>("c")
        .member(flecs::Entity, "d");
    test_assert(t != 0);

    auto a = t.lookup("a");
    test_assert(a != 0);
    test_assert( a.has<flecs::Member>() );
    const flecs::Member *m = a.get<flecs::Member>();
    test_uint(m->type, ecs.component<uintptr_t>());

    auto b = t.lookup("b");
    test_assert(b != 0);
    test_assert( b.has<flecs::Member>() );
    m = b.get<flecs::Member>();
    test_uint(m->type, flecs::Uptr);

    auto c = t.lookup("c");
    test_assert(c != 0);
    test_assert( c.has<flecs::Member>() );
    m = c.get<flecs::Member>();
    test_uint(m->type, flecs::U64);

    auto d = t.lookup("d");
    test_assert(d != 0);
    test_assert( d.has<flecs::Member>() );
    m = d.get<flecs::Member>();
    test_uint(m->type, flecs::Entity);
}

void Meta_units() {
    struct Test {
        int32_t meters;
        int32_t custom_unit;
    };

    flecs::world ecs;

    ecs.import<flecs::units>();

    auto custom_unit = ecs.entity("some_unit").unit("u");
    test_assert(custom_unit != 0);
    test_str(custom_unit.name(), "some_unit");

    const flecs::Unit *unit = custom_unit.get<flecs::Unit>();
    test_assert(unit != nullptr);
    test_str(unit->symbol, "u");

    auto t = ecs.component<Test>()
        .member<int32_t, flecs::units::length::Meters>("meters")
        .member<int32_t>(custom_unit, "custom_unit");
    test_assert(t != 0);

    flecs::entity m_meters = t.lookup("meters");
    test_assert(m_meters != 0);
    const flecs::Member *m = m_meters.get<flecs::Member>();
    test_assert(m != nullptr);
    test_uint(m->type, flecs::I32);
    test_uint(m->unit, ecs.id<flecs::units::length::Meters>());

    flecs::entity m_custom_unit = t.lookup("custom_unit");
    test_assert(m_custom_unit != 0);
    m = m_custom_unit.get<flecs::Member>();
    test_assert(m != nullptr);
    test_uint(m->type, flecs::I32);
    test_uint(m->unit, custom_unit);
}

void Meta_unit_w_quantity() {
    struct Test {
        int32_t m_1;
        int32_t m_2;
    };

    flecs::world ecs;

    ecs.import<flecs::units>();

    auto custom_quantity = ecs.entity().quantity();
    auto unit_1 = ecs.entity().unit("u1").quantity(custom_quantity);
    auto unit_2 = ecs.entity().unit("u2").quantity<flecs::units::Length>();

    test_assert(unit_1.has(flecs::Quantity, custom_quantity));
    test_assert(unit_2.has(flecs::Quantity, ecs.id<flecs::units::Length>()));

    auto t = ecs.component<Test>()
        .member<int32_t>(unit_1, "m_1")
        .member<int32_t>(unit_2, "m_2");
    test_assert(t != 0);

    flecs::entity m_1 = t.lookup("m_1");
    test_assert(m_1 != 0);
    const flecs::Member *m = m_1.get<flecs::Member>();
    test_assert(m != nullptr);
    test_uint(m->type, flecs::I32);
    test_uint(m->unit, unit_1);

    flecs::entity m_2 = t.lookup("m_2");
    test_assert(m_2 != 0);
    m = m_2.get<flecs::Member>();
    test_assert(m != nullptr);
    test_uint(m->type, flecs::I32);
    test_uint(m->unit, unit_2);
}

void Meta_unit_w_prefix() {
    struct Test {
        int32_t m_1;
        int32_t m_2;
    };

    flecs::world ecs;

    ecs.import<flecs::units>();

    auto prefix = ecs.entity().unit_prefix("p", 100, 1);
    
    auto unit_1 = ecs.entity().unit("U1");
    const flecs::Unit *unit = unit_1.get<flecs::Unit>();
    test_assert(unit != nullptr);
    test_str(unit->symbol, "U1");

    auto unit_2 = ecs.entity().unit(prefix, unit_1);
    unit = unit_2.get<flecs::Unit>();
    test_assert(unit != nullptr);
    test_str(unit->symbol, "pU1");
}

void Meta_unit_w_over() {
    struct Test {
        int32_t m_1;
        int32_t m_2;
    };

    flecs::world ecs;

    ecs.import<flecs::units>();

    auto prefix = ecs.entity().unit_prefix("p", 100, 1);
    
    auto unit_0 = ecs.entity().unit("U0");
    const flecs::Unit *unit = unit_0.get<flecs::Unit>();
    test_assert(unit != nullptr);
    test_str(unit->symbol, "U0");

    auto unit_1 = ecs.entity().unit("U1");
    unit = unit_1.get<flecs::Unit>();
    test_assert(unit != nullptr);
    test_str(unit->symbol, "U1");

    auto unit_2 = ecs.entity().unit(prefix, unit_1, unit_0);
    unit = unit_2.get<flecs::Unit>();
    test_assert(unit != nullptr);
    test_str(unit->symbol, "pU1/U0");
}

void Meta_partial_struct() {
    flecs::world ecs;

    auto c = ecs.component<Position>()
        .member<float>("x");
    test_assert(c != 0);

    const flecs::Component *ptr = c.get<flecs::Component>();
    test_int(ptr->size, 8);
    test_int(ptr->alignment, 4);
    
    auto xe = c.lookup("x");
    test_assert(xe != 0);
    test_assert( xe.has<flecs::Member>() );
    const flecs::Member *x = xe.get<flecs::Member>();
    test_uint(x->type, flecs::F32);
    test_uint(x->offset, 0);
}

void Meta_partial_struct_custom_offset() {
    flecs::world ecs;

    auto c = ecs.component<Position>()
        .member<float>("y", 1, offsetof(Position, y));
    test_assert(c != 0);

    const flecs::Component *ptr = c.get<flecs::Component>();
    test_int(ptr->size, 8);
    test_int(ptr->alignment, 4);
    
    auto xe = c.lookup("y");
    test_assert(xe != 0);
    test_assert( xe.has<flecs::Member>() );
    const flecs::Member *x = xe.get<flecs::Member>();
    test_uint(x->type, flecs::F32);
    test_uint(x->offset, 4);
}
