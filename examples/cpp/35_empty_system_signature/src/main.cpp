#include <empty_system_signature.h>
#include <iostream>

int main(int argc, char *argv[]) {
    flecs::world ecs(argc, argv);

    /* Define a system with an empty signature. Systems that do not match with
     * any entities are invoked once per frame */
    ecs.system<>()
        .iter([](flecs::iter&) {
            std::cout << "System invoked!" << std::endl;
        });

    ecs.set_target_fps(1);

    std::cout << "Application simple_system is running, press CTRL-C to exit..." << std::endl;

    /* Run systems */
    while (ecs.progress()) { }
}
