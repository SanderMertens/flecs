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
    auto RootPrefab = ecs.prefab("RootPrefab")
        .set<Position>({10, 20});
        
        /* Create two child prefabs that inherit from ChildBase */
        ecs.prefab("Child1")
            .child_of(RootPrefab)
            .is_a(ChildBase)
            .set<Velocity>({30, 40});

        ecs.prefab("Child2")
            .child_of(RootPrefab)
            .is_a(ChildBase)
            .set<Velocity>({50, 60});            

    /* Create instance of Root */
    auto e = ecs.entity()
        .is_a(RootPrefab);

    /* Print types of child1 and child2 */
    auto child1 = e.lookup("Child1");
    std::cout << "Child1 type = [" << child1.type().str() << "]" << std::endl;

    auto child2 = e.lookup("Child2");
    std::cout << "Child2 type = [" << child2.type().str() << "]" << std::endl;

    /* e shares Position from RootPrefab */
    e.get([](const Position& p) {
        std::cout << "Position of e = {" << p.x << ", " << p.y << "}" 
                  << std::endl;
    });    

    /* Children will share Position from ChildBase and Velocity from the Child1
     * and Child2 prefabs respectively */
    child1.get([](const Position& p, const Velocity& v) {
        std::cout << "Child1 Position = {" << p.x << ", " << p.y << "} " 
                  <<        "Velocity = {" << v.x << ", " << v.y << "}" 
                  << std::endl;
    });

    child2.get([](const Position& p, const Velocity& v) {
        std::cout << "Child2 Position = {" << p.x << ", " << p.y << "} " 
                  <<        "Velocity = {" << v.x << ", " << v.y << "}" 
                  << std::endl;
    });      
}
