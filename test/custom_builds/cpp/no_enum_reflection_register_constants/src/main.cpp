#include <no_enum_reflection_register_constants.h>
#include <stdio.h>

enum Color {
    Red, Green, Blue
};

int main(int argc, char *argv[]) {    
    flecs::world world(argc, argv);

    auto c = world.component<Color>()
        .constant("Red", Red)
        .constant("Green", Green)
        .constant("Blue", Blue);

    if (c.lookup("Red") == 0) {
        printf("enum constant is not defined\n");
        return -1;
    }

    if (c.lookup("Green") == 0) {
        printf("enum constant is not defined\n");
        return -1;
    }

    if (c.lookup("Blue") == 0) {
        printf("enum constant is not defined\n");
        return -1;
    }

    flecs::entity e = world.entity().add(Color::Green);
    assert(!e.has(Color::Red));
    assert(e.has(Color::Green));
    assert(!e.has(Color::Blue));

    int32_t count = 0;
    world.query_builder()
        .with(Color::Green)
        .each([&](flecs::entity) {
            count ++;
        });

    assert(count == 1);

    world.query_builder()
        .with(Color::Red)
        .each([&](flecs::entity) {
            count ++;
        });

    world.query_builder()
        .with(Color::Blue)
        .each([&](flecs::entity) {
            count ++;
        });

    assert(count == 1);

    return 0;
}
