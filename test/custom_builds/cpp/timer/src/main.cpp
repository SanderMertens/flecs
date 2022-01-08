#include <timer.h>
#include <iostream>

struct Position { 
    double x, y;
};

struct Velocity { 
    double x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    ecs.system<Position, const Velocity>()
        .interval(2.9f)
        .each([](Position& p, const Velocity& v) {
            p.x += v.x;
            p.y += v.y;
        });

    auto e = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    const Position *p = e.get<Position>();

    ecs.progress(1.0);
    ecs.progress(1.0);

    assert(p->x == 10);
    assert(p->y == 20);

    ecs.progress(1.0);

    assert(p->x == 11);
    assert(p->y == 22);
}
