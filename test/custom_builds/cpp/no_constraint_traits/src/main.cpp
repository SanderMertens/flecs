#include <no_constraint_traits.h>

#ifdef FLECS_CONSTRAINT_TRAITS
#error "FLECS_CONSTRAINT_TRAITS should be disabled"
#endif

int main(int, char *[]) {
    flecs::world world;
    world.entity();
}
