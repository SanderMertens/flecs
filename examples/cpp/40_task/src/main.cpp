#include <task.h>
#include <iostream>

struct TaskContext {
    int value;
};

int main(int argc, char *argv[]) {
    flecs::world world(argc, argv);

    flecs::component<TaskContext>(world, "TaskContext");

    // Tasks are systems that are not matched with any entities.

    // Basic task
    flecs::system<>(world)
        .action([](flecs::iter& it) {
            std::cout << "Task executed every second" << std::endl;
        });

    // Task that is executed every 2 seconds
    flecs::system<>(world)
        .period(2.0)
        .action([](flecs::iter& it) {
            std::cout << "Task executed every 2 seconds" << std::endl;
        });

    // It is possible to add components to a task, just like regular systems
    auto system = flecs::system<>(world)
        .signature("SYSTEM:TaskContext")
        .action([](flecs::iter& it) {
            flecs::column<const TaskContext> ctx(it, 1);
            std::cout << "Task with context: " << ctx->value << std::endl;
        });

    system.set<TaskContext>({10});

    world.set_target_fps(1);

    std::cout << "Application task is running, press CTRL-C to exit..." << std::endl;

    /* Run systems */
    while (world.progress()) { }
}
