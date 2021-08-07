#include <nested_prefab.h>
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

    /* Create a prefab with a child entity. When this prefab is instantiated, 
     * the child will be instantiated too as a child of the instance.  */
    auto Root = ecs.prefab("Root")
        .set<Position>({10, 20});

        /* Specify the RootPrefab as the parent for the nested prefab. This will
         * cause the child prefab to be instantiated whenever an instanceof
         * RootPrefab is created. */
        ecs.prefab("Child")
            .child_of(Root)
            .set<Position>({30, 40});

    auto e = ecs.entity()
        .is_a(Root);

    /* Lookup child in the instance we just created. This child will have e in
     * its type with a CHILDOF mask, and the prefab Child in its type with an
     * IsA pair. */
    auto child = e.lookup("Child");
    std::cout << "Child type = [" << child.type().str() << "]" << std::endl;

    /* Print position of e and of the child. Note that since we did not override
     * any components, both position components are owned by the prefabs, not by
     * the instances. */
    const Position *p = e.get<Position>();
    std::cout << "Position of e = {" << p->x << ", " << p->y << "}" << std::endl;

    p = child.get<Position>();
    std::cout << "Position of child = {" << p->x << ", " << p->y << "}" << std::endl;
}
