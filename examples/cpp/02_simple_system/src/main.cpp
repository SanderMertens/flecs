#include <simple_system.h>
#include <iostream>

/* Component type */
struct Message {
    const char *text;
};

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world ecs(argc, argv);

    ecs.system<Message>()
        .each([](Message& messages) {
            std::cout << messages.text << std::endl;
        });

    ecs.entity().set<Message>({"Hello Flecs!"});

    ecs.set_target_fps(1);

    std::cout << "Application simple_system is running, press CTRL-C to exit..." << std::endl;

    /* Run systems */
    while (ecs.progress()) { }
}
