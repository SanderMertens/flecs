#include <journal.h>

struct Position {
    float x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    auto e = ecs.entity().set<Position>({10, 20});
    e.remove<Position>();
    e.destruct();

    return 0;
}
