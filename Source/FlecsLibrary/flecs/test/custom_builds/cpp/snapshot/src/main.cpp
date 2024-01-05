#include <snapshot.h>
#include <iostream>

int main(int, char *[]) {
    flecs::world world;

    auto s = world.snapshot();

    s.restore();
}
