#include <simple_module.h>

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world ecs(argc, argv);

    /* Import module containing Position, Velocity and Move */
    ecs.import<simple_module>();

    /* Create entity with imported components */
    ecs.entity("MyEntity")
        .set<simple_module::Position>({10, 20})
        .set<simple_module::Velocity>({1, 1});

    ecs.set_target_fps(1);

    /* Run imported move system */
    while (ecs.progress()) { }
}
