#include <singleton.h>
#include <iostream>

// This example shows how to use singleton components in queries.

// Singleton component
struct Gravity {
    double value; 
};

// Entity component
struct Velocity {
    double x;
    double y;
};

int main(int, char *[]) {
    flecs::world world;

    // Set singleton
    world.set<Gravity>({ 9.81 });

    // Set Velocity
    world.entity("e1").set<Velocity>({0, 0});
    world.entity("e2").set<Velocity>({0, 1});
    world.entity("e3").set<Velocity>({0, 2});

    // Create query that matches Gravity as singleton
    flecs::query<Velocity, const Gravity> q = 
        world.query_builder<Velocity, const Gravity>()
            .term_at(1).singleton()
            .build();

    // In a query string expression you can use the $ shortcut for singletons:
    //   Velocity, Gravity($)

    q.each([](flecs::entity e, Velocity& v, const Gravity& g) {
        v.y += g.value;
        std::cout << e.path() << " velocity is {" 
            << v.x << ", " << v.y << "}" << std::endl;
    });

    // Output
    //   ::e1 velocity is {0, 9.81}
    //   ::e2 velocity is {0, 10.81}
    //   ::e3 velocity is {0, 11.81}
}
