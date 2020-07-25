#include <cpp_api.h>

void Type_add_2() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto type = flecs::type(world)
        .add<Position>()
        .add<Velocity>();

    auto entity = flecs::entity(world);
    test_assert(entity.id() != 0);

    entity.add(type);
    test_assert(entity.has<Position>());
    test_assert(entity.has<Velocity>());
}

void Type_add_instanceof() {
    flecs::world world;

    auto base = flecs::entity(world);
    auto type = flecs::type(world)
        .add_instanceof(base);

    auto entity = flecs::entity(world);
    test_assert(entity.id() != 0);

    entity.add(type);
    test_assert(entity.has(flecs::Instanceof | base.id()));
}

void Type_add_childof() {
    flecs::world world;

    auto parent = flecs::entity(world);
    auto type = flecs::type(world)
        .add_childof(parent);

    auto entity = flecs::entity(world);
    test_assert(entity.id() != 0);

    entity.add(type);
    test_assert(entity.has(flecs::Childof | parent.id()));
}

void Type_1_component() {
    flecs::world world;

    flecs::component<Position>(world, "Position");

    auto type = flecs::type(world).add<Position>();

    auto entity = flecs::entity(world);
    test_assert(entity.id() != 0);

    entity.add(type);
    test_assert(entity.has<Position>());
}

void Type_2_component() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto type = flecs::type(world).add<Position, Velocity>();

    auto entity = flecs::entity(world);
    test_assert(entity.id() != 0);

    entity.add(type);
    test_assert(entity.has<Position>());
    test_assert(entity.has<Velocity>());
}

void Type_1_component_signature() {
    flecs::world world;

    flecs::component<Position>(world, "Position");

    auto type = flecs::type(world, nullptr, "Position");

    auto entity = flecs::entity(world);
    test_assert(entity.id() != 0);

    entity.add(type);
    test_assert(entity.has<Position>());
}

void Type_2_component_signature() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto type = flecs::type(world, nullptr, "Position, Velocity");

    auto entity = flecs::entity(world);
    test_assert(entity.id() != 0);

    entity.add(type);
    test_assert(entity.has<Position>());
    test_assert(entity.has<Velocity>());
}
