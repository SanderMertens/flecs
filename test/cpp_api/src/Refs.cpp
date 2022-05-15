#include <cpp_api.h>

void Refs_get_ref_by_ptr() {
    flecs::world world;

    auto e = flecs::entity(world)
        .set<Position>({10, 20});

    auto ref = e.get_ref<Position>();
    test_assert(ref->x == 10);
    test_assert(ref->y == 20);
}

void Refs_get_ref_by_method() {
    flecs::world world;

    auto e = flecs::entity(world)
        .set<Position>({10, 20});

    auto ref = e.get_ref<Position>();
    test_assert(ref.get()->x == 10);
    test_assert(ref.get()->y == 20);
}

void Refs_ref_after_add() {
    flecs::world world;

    auto e = flecs::entity(world)
        .set<Position>({10, 20});

    auto ref = e.get_ref<Position>();

    e.add<Velocity>();
    test_assert(ref->x == 10);
    test_assert(ref->y == 20);
}

void Refs_ref_after_remove() {
    flecs::world world;

    auto e = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({1, 1});

    auto ref = e.get_ref<Position>();

    e.remove<Velocity>();
    test_assert(ref->x == 10);
    test_assert(ref->y == 20);
}

void Refs_ref_after_set() {
    flecs::world world;

    auto e = flecs::entity(world)
        .set<Position>({10, 20});

    auto ref = e.get_ref<Position>();
    
    e.set<Velocity>({1, 1});
    test_assert(ref->x == 10);
    test_assert(ref->y == 20);
}

void Refs_ref_before_set() {
    flecs::world world;

    auto e = flecs::entity(world);
    auto ref = e.get_ref<Position>();

    e.set<Position>({10, 20});

    test_assert(ref->x == 10);
    test_assert(ref->y == 20);
}

void Refs_non_const_ref() {
    flecs::world world;

    auto e = world.entity().set<Position>({10, 20});
    auto ref = e.get_ref<Position>();
    ref->x ++;

    test_int(e.get<Position>()->x, 11);
}
