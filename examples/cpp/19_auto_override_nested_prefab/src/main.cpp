#include <auto_override_nested_prefab.h>
#include <iostream>

struct Position {
    double x, y;
};

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world ecs(argc, argv);

    /* Create root prefab */
    auto RootPrefab = ecs.prefab("Root")
        .set<Position>({10, 20})
        .add_owned<Position>();

        /* Create child prefab. Instead of adding the child directly to
         * Root, create a type that overrides the components from the
         * ChildPrefab. This ensures that when the prefab is instantiated, the
         * components from the child prefab are owned by the instance. */
        auto ChildPrefab = ecs.prefab("ChildPrefab")
            .set<Position>({30, 40});

        /* Instead of the ChildPrefab, add the Child type to RootPrefab. Use a
         * string-based type expression to create the type, as the type needs to
         * be fully constructed before registering it with the prefab parent. */
        ecs.prefab("Child")
            .child_of(RootPrefab)
            .is_a(ChildPrefab);

    /* Create new entity from Root. */
    auto e = ecs.entity().is_a(RootPrefab);

    /* Lookup child in the instance we just created. This child will have e in
     * its type with a ChildOF relation, and the prefab ChildPrefab in its type with
     * an IsA relation. Note how the identifier is Child, not ChildPrefab. */
    auto child = e.lookup("Child");
    std::cout << "Child type = [" << child.type().str() << "]" << std::endl;

    /* Print position of e and of the child. Note that since types were used to
     * automatically override the components, the components are owned by both
     * e and child. */
    const Position *p = e.get<Position>();
    std::cout << "Position of e = {" << p->x << ", " << p->y << "} (owned = "
        << e.owns<Position>() << ")"
        << std::endl;

    p = child.get<Position>();
    std::cout << "Position of child = {" << p->x << ", " << p->y << "} (owned = "
        << child.owns<Position>() << ")" 
        << std::endl;
}
