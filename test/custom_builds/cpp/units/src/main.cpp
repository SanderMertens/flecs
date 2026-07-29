#include <units.h>

int main(int, char *[]) {
    flecs::world ecs;

    ecs.import<flecs::units>();

    assert(ecs.lookup("::flecs::units") != 0);

    return 0;
}
