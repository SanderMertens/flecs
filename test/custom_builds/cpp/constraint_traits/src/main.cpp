#include <constraint_traits.h>

int main(int, char *[]) {
    flecs::world ecs;

    auto likes = ecs.entity().add(flecs::Symmetric);
    auto a = ecs.entity();
    auto b = ecs.entity();

    a.add(likes, b);
    assert(b.has(likes, a));

    auto rel = ecs.entity().add(flecs::Acyclic);
    assert(rel.has(flecs::Acyclic));

    return 0;
}
