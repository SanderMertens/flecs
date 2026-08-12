#include <meta.h>

struct Position {
    int32_t x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    auto c = ecs.component<Position>()
        .member<int32_t>("x")
        .member<int32_t>("y");

    const EcsStruct *s = ecs_get(ecs, c, EcsStruct);
    assert(s != NULL);
    assert(ecs_vec_count(&s->members) == 2);

    Position p = {0, 0};
    flecs::cursor cur = ecs.cursor<Position>(&p);
    cur.push();
    cur.set_int(10);
    cur.next();
    cur.set_int(20);
    cur.pop();

    assert(p.x == 10);
    assert(p.y == 20);

    return 0;
}
