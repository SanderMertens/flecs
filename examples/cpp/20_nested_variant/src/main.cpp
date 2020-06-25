#include <nested_variant.h>
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

    /* Create a base prefab which will be inherited from by a child prefab */
    auto ChildBase = flecs::prefab(world, "ChildBase")
        .set<Position>({15, 25});

    /* Create the root of the prefab hierarchy  */
    auto Root = flecs::prefab(world, "RootPrefab")
        .set<Position>({10, 20});
        
        /* Create two child prefabs that inherit from ChildBase */
        flecs::prefab(world, "Child1")
            .add_childof(Root)
            .add_instanceof(ChildBase)
            .set<Velocity>({30, 40});

        flecs::prefab(world, "Child2")
            .add_childof(Root)
            .add_instanceof(ChildBase)
            .set<Velocity>({50, 60});            

    /* Create instance of Root */
    auto e = flecs::entity(world)
        .add_instanceof(Root);

    /* Print types of child1 and child2 */
    auto child1 = e.lookup("Child1");
    std::cout << "Child1 type = [" << child1.type().str() << "]" << std::endl;

    auto child2 = e.lookup("Child2");
    std::cout << "Child2 type = [" << child2.type().str() << "]" << std::endl;

    /* e shares Position from Root */
    const Position *p = e.get<Position>();
    std::cout << "Position of e = {" << p->x << ", " << p->y << "}" << std::endl;

    /* Children will share Position from ChildBase and Velocity from the Child1
     * and Child2 prefabs respectively */
    p = child1.get<Position>();
    const Velocity *v = child1.get<Velocity>();
    std::cout << "Child1 Position = {" << p->x << ", " << p->y << "} " 
        << "Velocity = {" << v->x << ", " << v->y << "}" << std::endl;

    p = child2.get<Position>();
    v = child2.get<Velocity>();
    std::cout << "Child2 Position = {" << p->x << ", " << p->y << "} " 
        << "Velocity = {" << v->x << ", " << v->y << "}" << std::endl;        
}
