#include <system_features.h>
#include <iostream>

void Action(flecs::iter& it) {
    std::cout << it.system().name() << " called!" << std::endl;
}

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world world(argc, argv);

    /* Create dummy systems */
    auto SystemA = flecs::system<>(world, "SystemA").action(Action);
    auto SystemB = flecs::system<>(world, "SystemB").action(Action);
    auto SystemC = flecs::system<>(world, "SystemC").action(Action);
    auto SystemD = flecs::system<>(world, "SystemD").action(Action);
    auto SystemE = flecs::system<>(world, "SystemE").action(Action);

    /* Create two features, each with a set of systems. Features are regular
     * types, and the name feature is just a convention to indicate that a type
     * only contains systems. Since systems, just like components, are stored as
     * entities, they can be contained by types. */
    auto Feature1 = flecs::type(world, "Feature1")
        .add(SystemA)
        .add(SystemB);

    auto Feature2 = flecs::type(world, "Feature2")
        .add(SystemC)
        .add(SystemD);

    /* Create a feature that includes Feature2 and SystemE. Types/features can
     * be organized in hierarchies */
    auto Feature3 = flecs::type(world, "Feature3")
        .add(Feature2)
        .add(SystemE);

    /* First, disable Feature1 and Feature3. No systems will be executed. */
    std::cout << "Disable Feature1, Feature3" << std::endl;
    Feature1.disable();
    Feature3.disable();
    world.progress();

    /* Enable Feature3. SystemC, SystemD and SystemE will be executed */
    std::cout << std::endl << "Enable Feature3" << std::endl;
    Feature3.enable();
    world.progress();

    /* Disable Feature2. SystemE will be executed */
    std::cout << std::endl << "Disable Feature2" << std::endl;
    Feature2.disable();
    world.progress();

    /* Enable Feature1. SystemA, SystemB and SystemE will be executed. */
    std::cout << std::endl << "Enable Feature1" << std::endl;
    Feature1.enable();
    world.progress();

    /* Disable only SystemE */
    std::cout << std::endl << "Disable SystemE" << std::endl;
    SystemE.disable();
    world.progress();
}
