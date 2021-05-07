#include <override.h>
#include <iostream>

struct Mass {
    double value;
};

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world ecs(argc, argv);

    /* Create base entity */
    auto base = ecs.entity().set<Mass>({10});

    /* Create instances which share the Mass component from a base */
    auto instance = ecs.entity().is_a(base);

    /* Print value before overriding Mass. The component is not owned, as it is
     * shared with the base entity. */
    std::cout << "Before overriding:" << std::endl;
    std::cout << "instance: " 
        << instance.get<Mass>()->value << " (owned = " 
        << instance.owns<Mass>() << ")" << std::endl;

    /* Override Mass of instance, which will give instance a private copy of the
     * Mass component. */
    instance.set<Mass>({20});

    /* Print values after overriding Mass. The value of Mass for instance_1 will
     * be the value assigned in the override (20). Instance now owns Mass,
     * confirming it has a private copy of the component. */
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
