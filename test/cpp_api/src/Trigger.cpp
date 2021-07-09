#include <cpp_api.h>

void Trigger_on_add() {
    flecs::world world;

    int invoked = 0;

    world.system<Position>()
        .kind(flecs::OnAdd)
        .each([&](flecs::entity e, Position& p) {
            invoked ++;
        });

    world.entity()
        .add<Position>();

    test_int(invoked, 1);
}

void Trigger_on_remove() {
    flecs::world world;

    int invoked = 0;

    world.system<Position>()
        .kind(flecs::OnRemove)
        .each([&](flecs::entity e, Position& p) {
            invoked ++;
        });

    auto e = world.entity()
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
        .iter([&](flecs::iter it, MyTag*) {
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

void Trigger_trigger_w_self() {
    flecs::world world;

    auto self = world.entity();

    bool invoked = false;
    world.system<Position>()
        .kind(flecs::OnAdd)
        .self(self)
        .iter([&](flecs::iter& it) {
            test_assert(it.self() == self);
            invoked = true;
        });

    world.entity().set<Position>({10, 20});

    test_bool(invoked, true);
}
