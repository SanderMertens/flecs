#include <observer.h>
#include <iostream>

struct Position {
    float x, y;
};

struct Velocity {
    float x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    auto Likes = ecs.entity();
    auto Apples = ecs.entity();

    // An observer triggers when an event matching one of its terms matches, and
    // the entity matches with all other terms.
    ecs.observer<Position, Velocity>()
        .event(flecs::OnAdd)
        .term(Likes, flecs::Wildcard)
        .each([&](Position&, Velocity&) {
            std::cout << "Observer triggered!" << std::endl;
        });

    auto e = ecs.entity();

    e.add<Position>(); // Observer does not trigger

    e.add<Velocity>(); // Observer does not trigger

    // Observer triggers, entity matches all of its terms
    e.add(Likes, Apples);
}
