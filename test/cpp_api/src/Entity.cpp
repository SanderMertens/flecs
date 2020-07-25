#include <cpp_api.h>

void Entity_new() {
    flecs::world world;

    auto entity = flecs::entity(world);
    test_assert(entity.id() != 0);
}

void Entity_new_named() {
    flecs::world world;

    auto entity = flecs::entity(world, "Foo");
    test_assert(entity.id() != 0);
    test_str(entity.name().c_str(), "Foo");
}

void Entity_new_add() {
    flecs::world world;

    flecs::component<Position>(world, "Position");

    auto entity = flecs::entity(world)
        .add<Position>();

    test_assert(entity.id() != 0);
    test_assert(entity.has<Position>());
}

void Entity_new_add_2() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = flecs::entity(world)
        .add<Position>()
        .add<Velocity>();

    test_assert(entity.id() != 0);
    test_assert(entity.has<Position>());
    test_assert(entity.has<Velocity>());
}

void Entity_new_set() {
    flecs::world world;

    flecs::component<Position>(world, "Position");

    auto entity = flecs::entity(world)
        .set<Position>({10, 20});

    test_assert(entity.id() != 0);
    test_assert(entity.has<Position>());

    const Position *p = entity.get<Position>();
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Entity_new_set_2() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    test_assert(entity.id() != 0);
    test_assert(entity.has<Position>());
    test_assert(entity.has<Velocity>());

    const Position *p = entity.get<Position>();
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Velocity *v = entity.get<Velocity>();
    test_int(v->x, 1);
    test_int(v->y, 2);
}

void Entity_add() {
    flecs::world world;

    flecs::component<Position>(world, "Position");

    auto entity = flecs::entity(world);
    test_assert(entity.id() != 0);

    entity.add<Position>();
    test_assert(entity.has<Position>());
}

void Entity_remove() {
    flecs::world world;

    flecs::component<Position>(world, "Position");

    auto entity = flecs::entity(world);
    test_assert(entity.id() != 0);

    entity.add<Position>();
    test_assert(entity.has<Position>());

    entity.remove<Position>();
    test_assert(!entity.has<Position>());
}

void Entity_set() {
    flecs::world world;

    flecs::component<Position>(world, "Position");

    auto entity = flecs::entity(world);
    test_assert(entity.id() != 0);

    entity.set<Position>({10, 20});
    test_assert(entity.has<Position>());

    const Position *p = entity.get<Position>();
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Entity_replace() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = flecs::entity(world);
    test_assert(entity.id() != 0);

    entity.replace<Position>([](Position& p, bool exists) {
        test_assert(!exists);
        p.x = 10;
        p.y = 20;
    });

    const Position *p = entity.get<Position>();
    test_int(p->x, 10);
    test_int(p->y, 20);

    entity.replace<Position>([](Position& p, bool exists) {
        test_assert(exists);
        p.x = 30;
    });

    test_int(p->x, 30); 
}

void Entity_add_2() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = flecs::entity(world);
    test_assert(entity.id() != 0);

    entity.add<Position>()
          .add<Velocity>();

    test_assert(entity.has<Position>());
    test_assert(entity.has<Velocity>());
}

void Entity_remove_2() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = flecs::entity(world);
    test_assert(entity.id() != 0);

    entity.add<Position>()
          .add<Velocity>();

    test_assert(entity.has<Position>());
    test_assert(entity.has<Velocity>());

    entity.remove<Position>()
          .remove<Velocity>();

    test_assert(!entity.has<Position>());
    test_assert(!entity.has<Velocity>());          
}

void Entity_set_2() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = flecs::entity(world);
    test_assert(entity.id() != 0);

    entity.set<Position>({10, 20})
        .set<Velocity>({1, 2});    
    test_assert(entity.has<Position>());
    test_assert(entity.has<Velocity>());

    const Position *p = entity.get<Position>();
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Velocity *v = entity.get<Velocity>();
    test_int(v->x, 1);
    test_int(v->y, 2);    
}

