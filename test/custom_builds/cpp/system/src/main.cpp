#include <system.h>

struct Position { 
    int32_t x, y;
};

struct Velocity { 
    int32_t x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    auto s = ecs.system<Position, const Velocity>()
        .each([](Position& p, const Velocity& v) {
            p.x += v.x;
            p.y += v.y;
        });

    auto e = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    s.run();

    const Position& p = e.get<Position>();
    assert(p.x == 11);
    assert(p.y == 22);
}
