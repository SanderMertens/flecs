#include <cpp.h>

void WorldFactory_entity(void) {
    flecs::world ecs;

    auto e = ecs.entity();
    test_assert(e.id() != 0);
}

void WorldFactory_entity_w_name(void) {
    flecs::world ecs;

    auto e = ecs.entity("MyName");
    test_assert(e.id() != 0);
    test_str(e.name().c_str(), "MyName");
}

void WorldFactory_entity_w_id(void) {
    flecs::world ecs;

    auto e = ecs.entity(100);
    test_assert(e.id() == 100);
}

void WorldFactory_prefab(void) {
    flecs::world ecs;

    auto e = ecs.prefab();
    test_assert(e.id() != 0);
    test_assert(e.has(flecs::Prefab));
}

void WorldFactory_prefab_w_name(void) {
    flecs::world ecs;

    auto e = ecs.prefab("MyName");
    test_assert(e.id() != 0);
    test_assert(e.has(flecs::Prefab));
    test_str(e.name().c_str(), "MyName");
}

void WorldFactory_system(void) {
    flecs::world ecs;

    auto s = ecs.system<Position, const Velocity>()
        .each([](flecs::entity e, Position& p, const Velocity& v) {
            p.x += v.x;
            p.y += v.y;
        });

    test_assert(s.id() != 0);

    auto e = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    ecs.progress();

    const Position *p = e.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);
}

void WorldFactory_system_w_name(void) {
    flecs::world ecs;

    auto s = ecs.system<Position, const Velocity>("MySystem")
        .each([](flecs::entity e, Position& p, const Velocity& v) {
            p.x += v.x;
            p.y += v.y;
        });

    test_assert(s.id() != 0);
    test_str(s.name().c_str(), "MySystem");

    auto e = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    ecs.progress();

    const Position *p = e.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);
}

void WorldFactory_system_w_expr(void) {
    flecs::world ecs;

    ecs.component<Position>();
    ecs.component<Velocity>();

    auto s = ecs.system<>("MySystem")
        .expr("Position, [in] Velocity")
        .run([](flecs::iter it) {
            while (it.next()) {
                flecs::field<Position> p(it, 0);
                flecs::field<const Velocity> v(it, 1);

                for (auto i : it) {
                    p[i].x += v[i].x;
                    p[i].y += v[i].y;
                }
            }
        });

    test_assert(s.id() != 0);
    test_str(s.name().c_str(), "MySystem");

    auto e = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    ecs.progress();

    const Position *p = e.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);
}

void WorldFactory_query(void) {
    flecs::world ecs;

    auto q = ecs.query<Position, const Velocity>();

    auto e = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    q.each([](flecs::entity e, Position& p, const Velocity& v) {
        p.x += v.x;
        p.y += v.y;
    });

    const Position *p = e.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);
}

void WorldFactory_query_w_expr(void) {
    flecs::world ecs;

    ecs.component<Position>();
    ecs.component<Velocity>();

    auto q = ecs.query_builder<>().expr("Position, [in] Velocity").build();

    auto e = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    q.run([](flecs::iter it) {
        while (it.next()) {
            flecs::field<Position> p(it, 0);
            flecs::field<const Velocity> v(it, 1);

            for (auto i : it) {
                p[i].x += v[i].x;
                p[i].y += v[i].y;
            }
        }
    });

    const Position *p = e.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);
}

class MyModule {
public:
    MyModule(flecs::world& ecs) {
        ecs.module<MyModule>();
        ecs.component<Position>();
    }
};

void WorldFactory_module(void) {
    flecs::world ecs;

    ecs.import<MyModule>();

    auto p = ecs.lookup("MyModule::Position");
    test_assert(p.id() != 0);
}
