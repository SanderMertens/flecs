#include <query_plans.h>

struct Position {
    float x, y;
};

struct Tag {};

int main(int, char *[]) {
    flecs::world ecs;

    auto e1 = ecs.entity().set<Position>({10, 20});
    ecs.entity().set<Position>({30, 40}).add<Tag>();

    auto q = ecs.query_builder<Position>()
        .cache_kind(flecs::QueryCacheNone)
        .with<Tag>().oper(flecs::Not)
        .build();

    int32_t count = 0;
    q.each([&](flecs::entity e, Position&) {
        assert(e == e1);
        count ++;
    });
    assert(count == 1);

    return 0;
}
