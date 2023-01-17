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

void Refs_pair_ref() {
    flecs::world world;

    auto e = world.entity().set<Position, Tag>({10, 20});
    auto ref = e.get_ref<Position, Tag>();
    ref->x ++;

    test_int((e.get<Position, Tag>()->x), 11);
}

void Refs_pair_ref_w_entity() {
    flecs::world world;

    auto tag = world.entity();
    auto e = world.entity().set<Position>(tag, {10, 20});
    auto ref = e.get_ref<Position>(tag);
    ref->x ++;

    test_int(e.get<Position>(tag)->x, 11);
}

void Refs_pair_ref_second() {
    flecs::world world;

    auto tag = world.entity();
    auto e = world.entity().set_second<Position>(tag, {10, 20});
    auto ref = e.get_ref_second<Position>(tag);
    ref->x ++;

    test_int(e.get_second<Position>(tag)->x, 11);
}

void Refs_from_stage() {
    flecs::world world;
    flecs::world stage = world.get_stage(0); // get default stage
    flecs::entity e = stage.entity().set<Position>({10, 20});
    auto ref = e.get_ref<Position>();
    test_int(ref->x, 10);
    test_int(ref->y, 20);
}
