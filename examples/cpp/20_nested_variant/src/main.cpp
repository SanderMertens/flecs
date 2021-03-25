#include <nested_variant.h>
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

    /* Create a base prefab which will be inherited from by a child prefab */
    auto ChildBase = ecs.prefab("ChildBase")
        .set<Position>({15, 25});

    /* Create the root of the prefab hierarchy  */
    auto Root = ecs.prefab("RootPrefab")
        .set<Position>({10, 20});
        
        /* Create two child prefabs that inherit from ChildBase */
        ecs.prefab("Child1")
            .add(flecs::ChildOf, Root)
            .add(flecs::IsA, ChildBase)
            .set<Velocity>({30, 40});

        ecs.prefab("Child2")
            .add(flecs::ChildOf, Root)
            .add(flecs::IsA, ChildBase)
            .set<Velocity>({50, 60});            

    /* Create instance of Root */
    auto e = ecs.entity()
        .add(flecs::IsA, Root);

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
