#include <json.h>
#include <string.h>

struct Position {
    float x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    ecs.component<Position>()
        .member<float>("x")
        .member<float>("y");

    auto e = ecs.entity("e").set<Position>({10, 20});

    auto json = e.to_json();
    assert(json.size() != 0);
    assert(strstr(json.c_str(), "\"x\":10") != NULL);

    return 0;
}
