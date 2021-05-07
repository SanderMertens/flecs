#include <override_init.h>
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

    /* Add component without setting it. This will initialize the new component
     * with the value from the base, which is a common approach to initializing
     * components with a value when they are added. */
    instance.add<Mass>();

    /* Print value of mass. The value will be equal to base, and the instance
     * will own the component. */
    std::cout << "instance: " 
        << instance.get<Mass>()->value << " (owned = " 
        << instance.owns<Mass>() << ")" << std::endl;
}
