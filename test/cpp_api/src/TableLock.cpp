#include <cpp_api.h>

void TableLock_each() {
    flecs::world ecs;

    ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Velocity>();

    ecs.filter<Position>()
        .each([&](flecs::entity e, Position& p) {
            e2.add<Mass>();
        });

    test_assert(e2.has<Mass>());
}

void TableLock_each_locked() {
    install_test_abort();

    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();

    ecs.filter<Position>()
        .each([&](flecs::entity e, Position& p) {
            test_expect_abort();
            e1.add<Mass>();
        });

    test_assert(false);
}

void TableLock_each_without_entity() {
    flecs::world ecs;

    ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Velocity>();

    ecs.filter<Position>()
        .each([&](Position& p) {
            e2.add<Mass>();
        });

    test_assert(e2.has<Mass>());
}

void TableLock_each_without_entity_locked() {
    install_test_abort();

    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();

    ecs.filter<Position>()
        .each([&](Position& p) {
            test_expect_abort();
            e1.add<Mass>();
        });

    test_assert(false);
}

void TableLock_iter() {
    flecs::world ecs;

    ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Velocity>();

    ecs.filter<Position>()
        .iter([&](flecs::iter& it, Position* p) {
            e2.add<Mass>();
        });

    test_assert(e2.has<Mass>());
}

void TableLock_iter_locked() {
    install_test_abort();

    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();

    ecs.filter<Position>()
        .iter([&](flecs::iter& it, Position* p) {
            test_expect_abort();
            e1.add<Mass>();
        });

    test_assert(false);
}

void TableLock_iter_without_components() {
    flecs::world ecs;

    ecs.entity().add<Position>();
    auto e2 = ecs.entity().add<Velocity>();

    ecs.filter<Position>()
        .iter([&](flecs::iter& it) {
            e2.add<Mass>();
        });

    test_assert(e2.has<Mass>());
}

void TableLock_iter_without_components_locked() {
    install_test_abort();

    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>();

    ecs.filter<Position>()
        .iter([&](flecs::iter& it) {
            test_expect_abort();
            e1.add<Mass>();
        });

    test_assert(false);
}

void TableLock_multi_get() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    auto e2 = ecs.entity().add<Position>();

    test_bool(true, e1.get([&](const Position& p, const Velocity& v) {
        e2.add<Mass>();
    }));

    test_assert(e2.has<Mass>());
}

void TableLock_multi_get_locked() {
    install_test_abort();

    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    auto e2 = ecs.entity().add<Position>();

    test_bool(true, e1.get([&](const Position& p, const Velocity& v) {
        test_expect_abort();
        e2.add<Velocity>();
    }));

    test_assert(false);
}

void TableLock_multi_set() {
    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    auto e2 = ecs.entity().add<Position>();

    e1.set([&](Position& p, Velocity& v) {
        e2.add<Mass>();
    });

    test_assert(e2.has<Mass>());
}

void TableLock_multi_set_locked() {
    install_test_abort();

    flecs::world ecs;

    auto e1 = ecs.entity().add<Position>().add<Velocity>();
    auto e2 = ecs.entity().add<Position>();

    e1.set([&](Position& p, Velocity& v) {
        test_expect_abort();
        e2.add<Velocity>();
    });

    test_assert(false);
}
