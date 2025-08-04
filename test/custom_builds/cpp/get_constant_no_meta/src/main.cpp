#include <get_constant_no_meta.h>
#include <iostream>

enum Color {
    Red, Green, Blue
};

int main(int, char *[]) {
    flecs::world world;

    flecs::entity e = world.entity().add(Color::Green);

    Color c1 = e.target<Color>().to_constant<Color>();
    assert(c1 == Green);

    Color c2 = e.get_constant<Color>();
    assert(c2 == Green);
}
