#include <multi_world.h>

struct Position {
    int32_t x, y;
};

int main(int, char *[]) {
    flecs::world world_a;
    flecs::world world_b;

    auto ea = world_a.entity().set<Position>({1, 2});
    auto eb = world_b.entity().set<Position>({3, 4});

    assert(ea.try_get<Position>()->x == 1);
    assert(eb.try_get<Position>()->x == 3);

    return 0;
}
