#include <prefab.h>
#include <iostream>

struct Position {
    double x, y;
};

struct Velocity {
    double x, y;
};

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world ecs(argc, argv);

    /* Create a prefab. Prefabs are entities that are solely intended as
     * templates for other entities. Prefabs are by default not matched with
     * systems. In that way they are similar to regular entities with the
     * EcsDisbled tag, except that they have more features which are 
     * demonstrated in the nested_prefab example. */
    auto BasePrefab = ecs.prefab("BasePrefab")
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    /* This type inherits from Base, as well as adding Position and Velocity as
     * private components. This will automatically override the components as an
     * entity with this type is created, which will initialize the private
     * values with the values of the Base entity. This is a common approach to
     * creating entities with an initialized set of components. */
    auto Base = ecs.type("Base")
        .is_a(BasePrefab)
        .add<Position>()
        .add<Velocity>();

    auto e = ecs.entity()
        .add(Base);

    const Position *p = e.get<Position>();
    const Velocity *v = e.get<Velocity>();

    std::cout << "Position: {" << p->x << ", " << p->y << "} (owned = " 
        << e.owns<Position>() << "} " << std::endl;
    std::cout << "Velocity: {" << v->x << ", " << v->y << "} (owned = " 
        << e.owns<Velocity>() << "} " << std::endl;
}
