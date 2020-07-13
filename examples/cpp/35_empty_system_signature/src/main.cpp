#include <empty_system_signature.h>
#include <iostream>

int main(int argc, char *argv[]) {
    flecs::world world(argc, argv);

    /* Define a system with an empty signature. Systems that do not match with
     * any entites are invoked once per frame */
    flecs::system<>(world)
        .action([](const flecs::iter& it) {
            std::cout << "System invoked!" << std::endl;
        });

    world.set_target_fps(1);

    std::cout << "Application simple_system is running, press CTRL-C to exit..." << std::endl;

    /* Run systems */
    while (world.progress()) { }
}
