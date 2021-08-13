#include <${id underscore}.h>
#include <iostream>

int main(int argc, char *argv[]) {
    flecs::world world(argc, argv);

    world.set_target_fps(1);

    std::cout << "Application ${id} is running, press CTRL-C to exit..." << std::endl;

    /* Run systems */
    while (world.progress()) { }
}
