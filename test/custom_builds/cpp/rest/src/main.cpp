#include <rest.h>
#include <iostream>

int main(int, char *[]) {
    flecs::world world;

    world.set<flecs::Rest>({});
}
