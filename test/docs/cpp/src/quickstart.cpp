#include <cpp.h>

void quickstart_Concepts_Entity_01(void) {
    flecs::world world;
    auto e = world.entity();
    e.is_alive(); // true!

    e.destruct();
    e.is_alive(); // false!

        test_assert(true);
}
