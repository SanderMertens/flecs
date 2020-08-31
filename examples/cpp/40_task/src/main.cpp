#include <task.h>
#include <iostream>

struct TaskContext {
    int value;
};

int main(int argc, char *argv[]) {
    flecs::world ecs(argc, argv);

    ecs.component<TaskContext>();

    // Tasks are systems that are not matched with any entities.

    // Basic task
    ecs.system<>()
        .action([](flecs::iter& it) {
            std::cout << "Task executed every second" << std::endl;
        });

    // Task that is executed every 2 seconds
    ecs.system<>()
        .period(2.0)
        .action([](flecs::iter& it) {
            std::cout << "Task executed every 2 seconds" << std::endl;
        });

    // It is possible to add components to a task, just like regular systems
    auto system = ecs.system<>(nullptr, "SYSTEM:TaskContext")
        .action([](flecs::iter& it) {
            flecs::column<const TaskContext> ctx(it, 1);
            std::cout << "Task with context: " << ctx->value << std::endl;
        });

    system.set<TaskContext>({10});

    ecs.set_target_fps(1);

    std::cout << "Application task is running, press CTRL-C to exit..." << std::endl;

    /* Run systems */
    while (ecs.progress()) { }
}
