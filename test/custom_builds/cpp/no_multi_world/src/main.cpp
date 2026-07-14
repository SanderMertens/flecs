#include <no_multi_world.h>

#ifdef FLECS_MULTI_WORLD
#error "FLECS_MULTI_WORLD should be disabled"
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
