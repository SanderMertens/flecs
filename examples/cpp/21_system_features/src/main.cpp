#include <system_features.h>
#include <iostream>

void Action(flecs::iter& it) {
    std::cout << it.system().name() << " called!" << std::endl;
}

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world ecs(argc, argv);

    /* Create dummy systems */
    auto SystemA = ecs.system<>("SystemA").iter(Action);
    auto SystemB = ecs.system<>("SystemB").iter(Action);
    auto SystemC = ecs.system<>("SystemC").iter(Action);
    auto SystemD = ecs.system<>("SystemD").iter(Action);
    auto SystemE = ecs.system<>("SystemE").iter(Action);

    /* Create two features, each with a set of systems. Features are regular
     * types, and the name feature is just a convention to indicate that a type
     * only contains systems. Since systems, just like components, are stored as
     * entities, they can be contained by types. */
    auto Feature1 = ecs.type("Feature1")
        .add(SystemA)
        .add(SystemB);

    auto Feature2 = ecs.type("Feature2")
        .add(SystemC)
        .add(SystemD);

    /* Create a feature that includes Feature2 and SystemE. Types/features can
     * be organized in hierarchies */
    auto Feature3 = ecs.type("Feature3")
        .add(Feature2)
        .add(SystemE);

    /* First, disable Feature1 and Feature3. No systems will be executed. */
    std::cout << "Disable Feature1, Feature3" << std::endl;
    Feature1.disable();
    Feature3.disable();
    ecs.progress();

    /* Enable Feature3. SystemC, SystemD and SystemE will be executed */
    std::cout << std::endl << "Enable Feature3" << std::endl;
    Feature3.enable();
    ecs.progress();

    /* Disable Feature2. SystemE will be executed */
    std::cout << std::endl << "Disable Feature2" << std::endl;
    Feature2.disable();
    ecs.progress();

    /* Enable Feature1. SystemA, SystemB and SystemE will be executed. */
    std::cout << std::endl << "Enable Feature1" << std::endl;
    Feature1.enable();
    ecs.progress();

    /* Disable only SystemE */
    std::cout << std::endl << "Disable SystemE" << std::endl;
    SystemE.disable();
    ecs.progress();
}
