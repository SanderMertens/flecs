#include <auto_override.h>
#include <iostream>

struct Position {
    float x;
    float y;
};

struct Velocity {
    float x;
    float y;
};

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world world(argc, argv);

    /* Register components */
    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    /* Create entity with default values for Position and Velocity. Add the
     * EcsDisabled tag to ensure the entity will not be matched by systems,
     * since it is only used to provide initial component values. */
    auto Base = flecs::entity(world, "Base")
        .set<Position>({10, 20})
        .set<Velocity>({1, 2})
        .add(flecs::Disabled);

    /* This type inherits from Base, as well as adding Position and Velocity as
     * private components. This will automatically override the components as an
     * entity with this type is created, which will initialize the private
     * values with the values of the Base entity. This is a common approach to
     * creating entities with an initialized set of components. */
    auto Movable = flecs::type(world, "Movable")
        .add_instanceof(Base)
        .add<Position>()
        .add<Velocity>();

    auto e = flecs::entity(world)
        .add(Movable);

    const Position *p = e.get<Position>();
    const Velocity *v = e.get<Velocity>();

    std::cout << "Position: {" << p->x << ", " << p->y << "} (owned = " 
        << e.owns<Position>() << "} " << std::endl;
    std::cout << "Velocity: {" << v->x << ", " << v->y << "} (owned = " 
        << e.owns<Velocity>() << "} " << std::endl;
}
