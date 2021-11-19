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
