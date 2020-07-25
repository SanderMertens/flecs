#include <replace.h>
#include <iostream>

// Implement a simple counter with the patch API. Other than a regular set, 
// patch lets an application to read a value before setting it.

struct Counter {
    int value;
};

void inc(flecs::entity e) {
    e.patch<Counter>([](Counter& counter, bool exists) {
        // If this is the first time that we're setting Counter, initialize.
        if (!exists) {
            counter.value = 0;
        }

        // Add one to the previous value of counter
        counter.value ++;
    });

    // Patch will call the OnSet systems for the specified component.
}

int main(int argc, char *argv[]) {
    // Create the world, pass arguments for overriding the number of threads,fps
    // or for starting the admin dashboard (see flecs.h for details).
    flecs::world world(argc, argv);

    flecs::component<Counter>(world, "Counter");

    // System that is invoked when Counter is added
    flecs::system<Counter>(world).kind(flecs::OnAdd).each(
        [](flecs::entity e, Counter&) {
            std::cout << e.name() << ": Counter added" << std::endl;
        });

    // System that prints the value of Counter when it changes
    flecs::system<Counter>(world).kind(flecs::OnSet).each(
        [](flecs::entity e, Counter& counter) {
            std::cout << e.name() << ": Counter changed to " 
                      << counter.value << std::endl;
        });

    // Create entity, increase counter twice
    auto e = flecs::entity(world, "MyEntity");
    inc(e);
    inc(e);
}
