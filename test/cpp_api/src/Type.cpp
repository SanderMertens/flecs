#include <cpp_api.h>

void Type_type_no_name() {
    flecs::world world;

    flecs::component<Position>(world, "Position");

    auto type = flecs::type(world, nullptr, "Position");
    auto id = type.id();
    auto e = world.entity(id);
    test_assert(!e.has<flecs::Identifier>(flecs::Name));
}

void Type_null_args() {
    flecs::world world;

    flecs::type t();

    // Make sure code didn't crash
    test_assert(true);
}

void Type_has_type() {
    flecs::world world;

    auto type = world.type()
        .add<Position>()
        .add<Velocity>();

    test_assert(type.has<Position>());
    test_assert(type.has<Velocity>());
    test_assert(!type.has<Mass>());
}

void Type_has_entity() {
    flecs::world world;

    auto e1 = world.entity();
    auto e2 = world.entity();
    auto e3 = world.entity();

    auto type = world.type()
        .add(e1)
        .add(e2);

    test_assert(type.has(e1));
    test_assert(type.has(e2));
    test_assert(!type.has(e3));
}

void Type_has_pair_type() {
    flecs::world world;

    struct Eats {};
    struct Apples {};
    struct Pears {};
    struct Bananas {};

    auto type = world.type()
        .add<Eats, Apples>()
        .add<Eats, Pears>();

    test_assert((type.has<Eats, Apples>()));
    test_assert((type.has<Eats, Pears>()));
    test_assert((!type.has<Eats, Bananas>()));
}

void Type_has_pair_entity() {
    flecs::world world;

    auto eats = world.entity();
    auto apples = world.entity();
    auto pears = world.entity();
    auto bananas = world.entity();

    auto type = world.type()
        .add(eats, apples)
        .add(eats, pears);

    test_assert(type.has(eats, apples));
    test_assert(type.has(eats, pears));
    test_assert(!type.has(eats, bananas));
}

void Type_get() {
    flecs::world world;

    auto apples = world.entity();
    auto pears = world.entity();
    auto bananas = world.entity();

    auto type = world.type()
        .add(apples)
        .add(pears)
        .add(bananas);

    test_assert(type.has(apples));
    test_assert(type.has(pears));
    test_assert(type.has(bananas));

    test_assert(type.get(0) == apples);
    test_assert(type.get(1) == pears);
    test_assert(type.get(2) == bananas);
}

void Type_get_out_of_range() {
    install_test_abort();

    flecs::world world;

    auto apples = world.entity();
    auto pears = world.entity();

    auto type = world.type()
        .add(apples)
        .add(pears);

    test_assert(type.has(apples));
    test_assert(type.has(pears));

    test_assert(type.get(0) == apples);
    test_assert(type.get(1) == pears);

    test_expect_abort();
    type.get(2);
}
