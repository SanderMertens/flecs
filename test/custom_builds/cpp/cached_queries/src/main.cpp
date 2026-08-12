#include <cached_queries.h>

struct Position {
    int32_t x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    auto q = ecs.query_builder<Position>()
        .cache_kind(flecs::QueryCacheAll)
        .build();

    ecs.entity().set<Position>({10, 20});

    int32_t count = 0;
    q.each([&](Position& p) {
        assert(p.x == 10);
        assert(p.y == 20);
        count ++;
    });
    assert(count == 1);

    return 0;
}
