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

struct MyTag { };

void Trigger_on_add_tag_action() {
    flecs::world world;

    int invoked = 0;

    world.system<MyTag>()
        .kind(flecs::OnAdd)
        .action([&](flecs::iter it, flecs::column<MyTag>) {
            invoked ++;
        });

    world.entity()
        .add<MyTag>();

    test_int(invoked, 1);
}

void Trigger_on_add_tag_iter() {
    flecs::world world;

    int invoked = 0;

    world.system<MyTag>()
        .kind(flecs::OnAdd)
        .iter([&](flecs::iter it, MyTag*) {
            invoked ++;
        });

    world.entity()
        .add<MyTag>();

    test_int(invoked, 1);
}

void Trigger_on_add_tag_each() {
    flecs::world world;

    int invoked = 0;

    world.system<MyTag>()
        .kind(flecs::OnAdd)
        .each([&](flecs::entity e, MyTag&) {
            invoked ++;
        });

    world.entity()
        .add<MyTag>();

    test_int(invoked, 1);
}
