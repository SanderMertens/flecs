#include <no_auto_registration.h>
#include <iostream>

int main(int, char *[]) {
    flecs::world world;

    world.import<flecs::units>();
    world.import<flecs::monitor>();

    return 0;
}
