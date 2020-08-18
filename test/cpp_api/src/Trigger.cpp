#include <cpp_api.h>

void Trigger_on_add() {
    flecs::world world;

    int invoked = 0;

    flecs::system<Position>(world)
        .kind(flecs::OnAdd)
        .each([&](flecs::entity e, Position& p) {
            invoked ++;
        });

    flecs::entity(world)
        .add<Position>();

    test_int(invoked, 1);
}

void Trigger_on_remove() {
    flecs::world world;

    int invoked = 0;

    flecs::system<Position>(world)
        .kind(flecs::OnRemove)
        .each([&](flecs::entity e, Position& p) {
            invoked ++;
        });

    auto e = flecs::entity(world)
        .add<Position>();

    test_int(invoked, 0);
    
    e.remove<Position>();

    test_int(invoked, 1);
}
