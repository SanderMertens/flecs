#include <cpp_api.h>

void Singleton_set_get_singleton() {
    flecs::world world;

    world.set<Position>({10, 20});

    const Position *p = world.get<Position>();
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Singleton_get_mut_singleton() {
    flecs::world world;

    Position *p_mut = world.get_mut<Position>();
    p_mut->x = 10;
    p_mut->y = 20;

    const Position *p = world.get<Position>();
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Singleton_emplace_singleton() {
    flecs::world world;

    world.emplace<Position>(10.0f, 20.0f);

    const Position *p = world.get<Position>();
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Singleton_modified_singleton() {
    flecs::world world;

    int invoked = 0;

    world.observer<Position>()
        .event(flecs::OnSet)
        .iter([&](flecs::iter it, Position *p) {
            invoked ++;
        });

    auto e = world.entity();
    Position *p = e.get_mut<Position>();
    test_assert(p != NULL);
    test_int(invoked, 0);

    e.modified<Position>();
    test_int(invoked, 1);
}

void Singleton_add_singleton() {
    flecs::world world;

    int invoked = 0;

    world.observer<Position>()
        .event(flecs::OnAdd)
        .iter([&](flecs::iter it, Position *p) {
            invoked ++;
        });

    world.add<Position>();

    test_int(invoked, 1);
}


void Singleton_remove_singleton() {
    flecs::world world;

    int invoked = 0;

    world.observer<Position>()
        .event(flecs::OnRemove)
        .iter([&](flecs::iter it, Position *p) {
            invoked ++;
        });

    Position *p_mut = world.get_mut<Position>();
    test_assert(p_mut != NULL);

    world.remove<Position>();

    test_int(invoked, 1);
}

void Singleton_has_singleton() {
    flecs::world world;

    test_assert(!world.has<Position>());

    world.set<Position>({10, 20});

    test_assert(world.has<Position>());
}

void Singleton_singleton_system() {
    flecs::world world;

    world.set<Position>({10, 20});

    world.system<>()
        .expr("[inout] Position($)")
        .iter([](flecs::iter it) {
            auto p = it.field<Position>(1);
            test_int(p->x, 10);
            test_int(p->y, 20);

            p->x ++;
            p->y ++;
        });

    world.progress();

    const Position *p = world.get<Position>();
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 21);
}

void Singleton_get_singleton() {
    flecs::world world;

    world.set<Position>({10, 20});

    auto s = world.singleton<Position>();
    test_assert(s.has<Position>());
    test_assert(s.id() == flecs::type_id<Position>());

    const Position* p = s.get<Position>();
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Singleton_type_id_from_world() {
    flecs::world world;

    world.set<Position>({10, 20});

    flecs::entity_t id = world.id<Position>();
    test_assert(id == flecs::type_id<Position>());

    auto s = world.singleton<Position>();
    test_assert(s.id() == flecs::type_id<Position>());
    test_assert(s.id() == flecs::type_id<Position>());
}

void Singleton_set_lambda() {
    flecs::world world;

    world.set([](Position& p) {
        p.x = 10;
        p.y = 20;
    });

    const Position* p = world.get<Position>();
    test_int(p->x, 10);
    test_int(p->y, 20);

    world.set([](Position& p) {
        p.x ++;
        p.y ++;
    });

    p = world.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 21);
}

void Singleton_get_lambda() {
    flecs::world world;

    world.set<Position>({10, 20});

    int32_t count = 0;
    world.get([&](const Position& p) {
        test_int(p.x, 10);
        test_int(p.y, 20);
        count ++;
    });

    test_int(count, 1);
}

void Singleton_get_write_lambda() {
    flecs::world world;

    world.set<Position>({10, 20});

    int32_t count = 0;
    world.get([&](Position& p) {
        test_int(p.x, 10);
        test_int(p.y, 20);
        p.x ++;
        p.y ++;
        count ++;
    });

    test_int(count, 1);

    const Position *p = world.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 21);
}
