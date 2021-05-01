#include <helloworld.h>
#include <iostream>

struct Position {
    double x, y;
    Position& set(double xa, double ya) {
        x = xa;
        y = ya;
        return *this;
    }
};

struct Velocity {
    double x, y;
};

struct Mass {
    double value;
};

int main(int argc, char *argv[]) {
    // Create the world, pass arguments for overriding the number of threads,fps
    // or for starting the admin dashboard (see flecs.h for details).
    flecs::world world(argc, argv);

    // Create a new empty entity. Entity names are optional.
    auto e = world.entity("E1");

    // Set the Position component on the entity.
    e.set<Position>({10, 20});

    // Get the Position component.
    const Position *p = e.get<Position>();

    std::cout << "Position of " << e.name() << " is {" 
              << p->x << ", " << p->y << "}" 
              << std::endl;

    auto Tag = world.entity();
    auto Likes = world.entity();
    auto Bob = world.entity();
    auto Alice = world.entity();
    auto Src = world.entity();

    e.add<Velocity>();

    world.entity("E2")
        .add<Position>()
        .add<Mass>();

    auto q = world.query<Position>();

    q.each([](flecs::entity e, Position& p) {
        std::cout << "Entity " << e.name() << std::endl;
    });

    flecs::query<Position> qb = world.query_builder<Position>();
    qb.each([](flecs::entity e, Position& p) {
        std::cout << "Entity " << e.name() << std::endl;
    });

    auto qb2 = world.query_builder<Position>().build();
    qb2.each([](flecs::entity e, Position& p) {
        std::cout << "Entity " << e.name() << std::endl;
    });

    printf("\n\n");

    auto qb3 = world.query_builder<Position>()
        .add<Mass>()
        .build();
        
    qb3.each([](flecs::entity e, Position& p) {
        std::cout << "Entity " << e.name() << std::endl;
    });    

    return 0;
}
