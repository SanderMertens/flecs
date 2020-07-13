#include <simple_system.h>
#include <iostream>

/* Component type */
struct Message {
    const char *text;
};

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world world(argc, argv);

    flecs::component<Message>(world, "Message");

    flecs::system<Message>(world)
        .each([](flecs::entity e, Message& messages) {
            std::cout << messages.text << std::endl;
        });

    flecs::entity(world)
        .set<Message>({"Hello Flecs!"});

    world.set_target_fps(1);

    std::cout << "Application simple_system is running, press CTRL-C to exit..." << std::endl;

    /* Run systems */
    while (world.progress()) { }
}
