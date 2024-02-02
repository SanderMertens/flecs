#include <module.h>
#include <iostream>

struct my_module {
    my_module(flecs::world& world) {
        world.module<my_module>();
    }
};

int main(int argc, char *argv[]) {
    flecs::world world(argc, argv);
    
    world.import<my_module>();
}
