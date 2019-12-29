#include <get_children.h>
#include <iostream>
#include <vector>

struct Foo {
    float value;
};

struct Bar {
    float value;
};

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world world(argc, argv);
    
    /* Register components */
    flecs::component<Foo>(world, "Foo");
    flecs::component<Bar>(world, "Bar");

    /* Vector to store children */
    std::vector<flecs::entity> children{};

    /* Match all non-disabled entities, add entities to children vector */
    auto GetChildren = flecs::system<>(world).kind(flecs::Manual).signature("!EcsDisabled").action(
        [&children](flecs::rows& rows) {
            for (auto row : rows) {
                children.push_back(rows.entity(row));
            }
        });

    /* Create two parents */
    auto Parent1 = flecs::entity(world);
    auto Parent2 = flecs::entity(world);

    /* Create two children for each parent, add different components to force
     * the system to iterate over multiple tables while collecting (just for
     * demo purposes). */
    flecs::entity(world, "Child1").add_childof(Parent1).add<Foo>();
    flecs::entity(world, "Child2").add_childof(Parent1).add<Bar>();
    flecs::entity(world, "Child3").add_childof(Parent2).add<Foo>();
    flecs::entity(world, "Child4").add_childof(Parent2).add<Bar>();

    /* Run system to collect children for Parent1 */
    std::cout << "Collect children for Parent1:" << std::endl;
    GetChildren.run().filter( flecs::filter(world).include(Parent1) );
    for (auto child : children) {
        std::cout << "Child found: " << child.name() << std::endl;
    }
    children.clear();

    /* Run system to collect children for Parent2 */
    std::cout << std::endl << "Collect children for Parent2:" << std::endl;
    GetChildren.run().filter( flecs::filter(world).include(Parent2) );
    for (auto child : children) {
        std::cout << "Child found: " << child.name() << std::endl;
    }    
}
