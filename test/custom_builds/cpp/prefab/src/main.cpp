#include <prefab.h>

struct Position {
    int32_t x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    auto p = ecs.prefab().set<Position>({10, 20});
    auto i = ecs.entity().is_a(p);

    const Position *pos = i.try_get<Position>();
    assert(pos != NULL);
    assert(pos->x == 10);
    assert(pos->y == 20);

    return 0;
}
