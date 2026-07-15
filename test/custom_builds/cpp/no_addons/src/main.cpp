#include <no_addons.h>

#ifdef FLECS_MULTI_WORLD
#error "FLECS_MULTI_WORLD should not be enabled in this custom build"
#endif

#ifdef FLECS_CONSTRAINT_TRAITS
#error "FLECS_CONSTRAINT_TRAITS should not be enabled in this custom build"
#endif

struct Position {
    float x;
    float y;
};

enum Color {
    Red,
    Green,
    Blue
};

int main(int, char *[]) {
    flecs::world world;

    flecs::entity_t position = world.id<Position>();
    assert(position == flecs::_::type<Position>::s_id);

    flecs::entity_t red = world.to_entity(Color::Red);
    assert(red == flecs::_::enum_type<Color>::get().constants[0].id);
}
