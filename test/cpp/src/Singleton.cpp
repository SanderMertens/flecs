#include <cpp.h>

void Singleton_set_get_singleton(void) {
    flecs::world world;

    world.set<Position>({10, 20});

    const Position *p = world.get<Position>();
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Singleton_ensure_singleton(void) {
    flecs::world world;

    Position& p_mut = world.ensure<Position>();
    p_mut.x = 10;
    p_mut.y = 20;

    const Position *p = world.get<Position>();
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Singleton_get_mut_singleton(void) {
    flecs::world world;

    Position *p = world.get_mut<Position>();
    test_assert(p == nullptr);

    world.set<Position>({10, 20});
    p = world.get_mut<Position>();
    test_assert(p != nullptr);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Singleton_emplace_singleton(void) {
    flecs::world world;

    world.emplace<Position>(10.0f, 20.0f);

    const Position *p = world.get<Position>();
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Singleton_modified_singleton(void) {
    flecs::world world;

    int invoked = 0;

    world.observer<Position>()
        .event(flecs::OnSet)
        .each([&](Position&) {
            invoked ++;
        });

    auto e = world.entity();
    e.ensure<Position>();
    test_int(invoked, 0);

    e.modified<Position>();
    test_int(invoked, 1);
}

void Singleton_add_singleton(void) {
    flecs::world world;

    int invoked = 0;

    world.observer<Position>()
        .event(flecs::OnAdd)
        .each([&](Position& p) {
            invoked ++;
        });

    world.add<Position>();

    test_int(invoked, 1);
}


void Singleton_remove_singleton(void) {
    flecs::world world;

    int invoked = 0;

    world.observer<Position>()
        .event(flecs::OnRemove)
        .each([&](Position& p) {
            invoked ++;
        });

    world.ensure<Position>();
    test_int(invoked, 0);

    world.remove<Position>();
    test_int(invoked, 1);
}

void Singleton_has_singleton(void) {
    flecs::world world;

    test_assert(!world.has<Position>());

    world.set<Position>({10, 20});

    test_assert(world.has<Position>());
}

void Singleton_singleton_system(void) {
    flecs::world world;

    world.set<Position>({10, 20});

    world.system<>()
        .expr("[inout] Position($)")
        .run([](flecs::iter& it) {
            while (it.next()) {
                auto p = it.field<Position>(0);
                test_int(p->x, 10);
                test_int(p->y, 20);

                p->x ++;
                p->y ++;
            }
        });

    world.progress();

    const Position *p = world.get<Position>();
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 21);
}

void Singleton_get_singleton(void) {
    flecs::world world;

    world.set<Position>({10, 20});

    auto s = world.singleton<Position>();
    test_assert(s.has<Position>());
    test_assert(s.id() == flecs::type_id<Position>());

    const Position* p = s.get<Position>();
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Singleton_type_id_from_world(void) {
    flecs::world world;

    world.set<Position>({10, 20});

    flecs::entity_t id = world.id<Position>();
    test_assert(id == flecs::type_id<Position>());

    auto s = world.singleton<Position>();
    test_assert(s.id() == flecs::type_id<Position>());
    test_assert(s.id() == flecs::type_id<Position>());
}

void Singleton_set_lambda(void) {
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

void Singleton_get_lambda(void) {
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

void Singleton_get_write_lambda(void) {
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

void Singleton_get_set_singleton_pair_R_T(void) {
    flecs::world world;

    world.set<Position, Tag>({10, 20});

    const Position *p = world.get<Position, Tag>();
    test_assert(p != nullptr);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Singleton_get_set_singleton_pair_R_t(void) {
    flecs::world world;

    flecs::entity tgt = world.entity();

    world.set<Position>(tgt, {10, 20});

    const Position *p = world.get<Position>(tgt);
    test_assert(p != nullptr);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Singleton_add_remove_singleton_pair_R_T(void) {
    flecs::world world;

    world.add<Position, Tag>();
    test_assert((world.has<Position, Tag>()));
    world.remove<Position, Tag>();
    test_assert(!(world.has<Position, Tag>()));
}

void Singleton_add_remove_singleton_pair_R_t(void) {
    flecs::world world;

    flecs::entity tgt = world.entity();

    world.add<Position>(tgt);
    test_assert((world.has<Position>(tgt)));
    world.remove<Position>(tgt);
    test_assert(!(world.has<Position>(tgt)));
}

void Singleton_add_remove_singleton_pair_r_t(void) {
    flecs::world world;

    flecs::entity rel = world.entity();
    flecs::entity tgt = world.entity();

    world.add(rel, tgt);
    test_assert((world.has(rel, tgt)));
    world.remove(rel, tgt);
    test_assert(!(world.has(rel, tgt)));
}

void Singleton_get_target(void) {
    flecs::world world;

    auto Rel = world.singleton<Tag>();

    auto obj1 = world.entity()
        .add<Position>();

    auto obj2 = world.entity()
        .add<Velocity>();

    auto obj3 = world.entity()
        .add<Mass>();

    flecs::entity entities[3] = {obj1, obj2, obj3};

    world.add<Tag>(obj1);
    world.add<Tag>(obj2);
    world.add(Rel, obj3);

    auto p = world.target<Tag>();
    test_assert(p != 0);
    test_assert(p == obj1);

    p = world.target<Tag>(Rel);
    test_assert(p != 0);
    test_assert(p == obj1);

    p = world.target(Rel);
    test_assert(p != 0);
    test_assert(p == obj1);

    for (int i = 0; i < 3; i++) {
        p = world.target<Tag>(i);
        test_assert(p != 0);
        test_assert(p == entities[i]);
    }

    for (int i = 0; i < 3; i++) {
        p = world.target<Tag>(Rel, i);
        test_assert(p != 0);
        test_assert(p == entities[i]);
    }

    for (int i = 0; i < 3; i++) {
        p = world.target(Rel, i);
        test_assert(p != 0);
        test_assert(p == entities[i]);
    }
}
