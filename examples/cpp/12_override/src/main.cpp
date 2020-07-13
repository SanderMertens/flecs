#include <override.h>
#include <iostream>

struct Mass {
    float value;
};

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world world(argc, argv);

    /* Register component */
    flecs::component<Mass>(world, "Mass");

    /* Create base entity */
    auto base = flecs::entity(world)
        .set<Mass>({10});

    /* Create instances which share the Mass component from a base */
    auto instance = flecs::entity(world)
        .add_instanceof(base);

    /* Print value before overriding Mass. The component is not owned, as it is
     * shared with the base entity. */
    std::cout << "Before overriding:" << std::endl;
    std::cout << "instance: " 
        << instance.get<Mass>()->value << " (owned = " 
        << instance.owns<Mass>() << ")" << std::endl;

    /* Override Mass of instance, which will give instance a private copy of the
     * Mass component. */
    instance.set<Mass>({20});

    /* Print value before overriding Mass. The component is not owned, as it is
     * shared with the base entity. */
    std::cout << "After overriding:" << std::endl;
    std::cout << "instance: " 
        << instance.get<Mass>()->value << " (owned = " 
        << instance.owns<Mass>() << ")" << std::endl;

    /* Remove override of Mass. This will remove the private copy of the Mass
     * component from instance. */
    instance.remove<Mass>();

    /* Print value after removing the override Mass. The component is no longer
     * owned, as the instance is again sharing the component with base. */
    std::cout << "After removing override:" << std::endl;
    std::cout << "instance: " 
        << instance.get<Mass>()->value << " (owned = " 
        << instance.owns<Mass>() << ")" << std::endl;
}
