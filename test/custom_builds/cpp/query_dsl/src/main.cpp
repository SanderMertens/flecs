#include <query_dsl.h>

struct Position {
    float x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    ecs.component<Position>();

    auto q = ecs.query_builder()
        .expr("Position")
        .build();

    ecs.entity().set<Position>({10, 20});

    int32_t count = 0;
    q.each([&](flecs::entity) {
        count ++;
    });
    assert(count == 1);

    return 0;
}
