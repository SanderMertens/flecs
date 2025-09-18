#include <no_enum_reflection.h>
#include <iostream>

enum Color {
    Red, Green, Blue
};

int main(int argc, char *argv[]) {    
    flecs::world world(argc, argv);

    auto c = world.component<Color>();

    if (c.lookup("Red") != 0) {
        printf("enum constant should not be defined\n");
        return -1;
    }

    if (c.lookup("Green") != 0) {
        printf("enum constant should not be defined\n");
        return -1;
    }

    if (c.lookup("Blue") != 0) {
        printf("enum constant should not be defined\n");
        return -1;
    }

    return 0;
}
