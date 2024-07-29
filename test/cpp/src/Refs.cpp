#include <cpp.h>

void Refs_get_ref_by_ptr(void) {
    flecs::world world;

    auto e = flecs::entity(world)
        .set<Position>({10, 20});

    auto ref = e.get_ref<Position>();
    test_assert(ref->x == 10);
    test_assert(ref->y == 20);
}

void Refs_get_ref_by_method(void) {
    flecs::world world;

    auto e = flecs::entity(world)
        .set<Position>({10, 20});

    auto ref = e.get_ref<Position>();
    test_assert(ref.get()->x == 10);
    test_assert(ref.get()->y == 20);
}

void Refs_ref_after_add(void) {
    flecs::world world;

    auto e = flecs::entity(world)
        .set<Position>({10, 20});

    auto ref = e.get_ref<Position>();

    e.add<Velocity>();
    test_assert(ref->x == 10);
    test_assert(ref->y == 20);
}

void Refs_ref_after_remove(void) {
    flecs::world world;

    auto e = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({1, 1});

    auto ref = e.get_ref<Position>();

    e.remove<Velocity>();
    test_assert(ref->x == 10);
    test_assert(ref->y == 20);
}

void Refs_ref_after_set(void) {
    flecs::world world;

    auto e = flecs::entity(world)
        .set<Position>({10, 20});

    auto ref = e.get_ref<Position>();
    
    e.set<Velocity>({1, 1});
    test_assert(ref->x == 10);
    test_assert(ref->y == 20);
}

void Refs_ref_before_set(void) {
    flecs::world world;

    auto e = flecs::entity(world);
    auto ref = e.get_ref<Position>();

    e.set<Position>({10, 20});

    test_assert(ref->x == 10);
    test_assert(ref->y == 20);
}

void Refs_non_const_ref(void) {
    flecs::world world;

    auto e = world.entity().set<Position>({10, 20});
    auto ref = e.get_ref<Position>();
    ref->x ++;

    test_int(e.get<Position>()->x, 11);
}

void Refs_pair_ref(void) {
    flecs::world world;

    auto e = world.entity().set<Position, Tag>({10, 20});
    auto ref = e.get_ref<Position, Tag>();
    ref->x ++;

    test_int((e.get<Position, Tag>()->x), 11);
}

using PositionTag = flecs::pair<Position, Tag>;

void Refs_pair_ref_w_pair_type(void) {
    flecs::world world;

    auto e = world.entity().set<PositionTag>({10, 20});
    auto ref = e.get_ref<PositionTag>();
    ref->x++;

    test_int((e.get<PositionTag>()->x), 11);
}

using TagPosition = flecs::pair<Tag, Position>;

void Refs_pair_ref_w_pair_type_second(void) {
    flecs::world world;

    auto e = world.entity().set<TagPosition>({10, 20});
    auto ref = e.get_ref<TagPosition>();
    ref->x++;

    test_int((e.get<TagPosition>()->x), 11);
}

void Refs_pair_ref_w_entity(void) {
    flecs::world world;

    auto tag = world.entity();
    auto e = world.entity().set<Position>(tag, {10, 20});
    auto ref = e.get_ref<Position>(tag);
    ref->x ++;

    test_int(e.get<Position>(tag)->x, 11);
}

void Refs_pair_ref_second(void) {
    flecs::world world;

    auto tag = world.entity();
    auto e = world.entity().set_second<Position>(tag, {10, 20});
    auto ref = e.get_ref_second<Position>(tag);
    ref->x ++;

    test_int(e.get_second<Position>(tag)->x, 11);
}

void Refs_from_stage(void) {
    flecs::world world;
    flecs::world stage = world.get_stage(0); // get default stage
    flecs::entity e = stage.entity().set<Position>({10, 20});
    auto ref = e.get_ref<Position>();
    test_int(ref->x, 10);
    test_int(ref->y, 20);
}

void Refs_default_ctor(void) {
    flecs::world world;

    // Make sure default ctor works
    flecs::ref<Position> p;

    flecs::entity e = world.entity().set<Position>({10, 20});

    p = e.get_ref<Position>();
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Refs_ctor_from_entity(void) {
    flecs::world world;

    flecs::entity e = world.entity().set<Position>({10, 20});

    flecs::ref<Position> p(e);

    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Refs_implicit_operator_bool(void) {
    flecs::world world;

    flecs::entity e = world.entity().set<Position>({10, 20});

    flecs::ref<Position> p(e);

    test_assert(p);
}

void Refs_try_get(void) {
    flecs::world world;

    flecs::ref<Position> p;

    test_assert(p.try_get() == nullptr);
}
