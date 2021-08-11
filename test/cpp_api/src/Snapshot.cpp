#include <cpp_api.h>

void Snapshot_simple_snapshot() {
    flecs::world world;

    auto e = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({1, 1});

    flecs::snapshot s(world);
    s.take();

    e.set<Position>({30, 40});
    e.set<Velocity>({2, 2});
    
    s.restore();

    const Position *p = e.get<Position>();
    const Velocity *v = e.get<Velocity>();

    test_assert(p != NULL);
    test_assert(v != NULL);

    test_int(p->x, 10);
    test_int(p->y, 20);

    test_int(v->x, 1);
    test_int(v->y, 1);
}

void Snapshot_snapshot_iter() {
    flecs::world world;

    auto e1 = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({1, 1});

    auto e2 = flecs::entity(world)
        .set<Position>({30, 40})
        .set<Velocity>({2, 2});

    flecs::snapshot s(world);
    s.take();

    bool e1_found = false;
    bool e2_found = false;

    for (auto it : s) {
        for (auto i : it) {
            auto e = it.entity(i);
            if (e == e1) {
                e1_found = true;
                const Position *p = e.get<Position>();
                test_assert(p != NULL);
                test_int(p->x, 10);
                test_int(p->y, 20);

                const Velocity *v = e.get<Velocity>();
                test_assert(v != NULL);
                test_int(v->x, 1);
                test_int(v->y, 1);
            } else if (e == e2) {
                e2_found = true;
                const Position *p = e.get<Position>();
                test_assert(p != NULL);
                test_int(p->x, 30);
                test_int(p->y, 40);

                const Velocity *v = e.get<Velocity>();
                test_assert(v != NULL);
                test_int(v->x, 2);
                test_int(v->y, 2);                
            }
        }
    }

    test_bool(e1_found, true);
    test_bool(e2_found, true);
}
