#include <entity_view.h>
#include <iostream>

struct Position {
    double x, y;
};

// Component with entity reference. Use entity_view to provide compiler-checked
// protection against calling functions that accidentally mutate an entity.
struct EntityView {
    flecs::entity_view ev;
};

int main(int argc, char *argv[]) {
    flecs::world ecs(argc, argv);

    ecs.system<EntityView>()
        .each([](flecs::entity e, EntityView& v) {
            // Operations that don't mutate the entity are allowed
            std::cout << "Readonly handle: " << v.ev.name() << std::endl;

            // Uncommenting this line will fail, as it is not possible to mutate
            // entity_view instances
            // v.ev.set<Position>({10, 20});

            // To mutate the entity, use the mut function. This will create a
            // new handle for the current stage (obtained from the entity) which
            // lets us do deferred operations
            v.ev.mut(e).set<Position>({10, 20});

            // The mut function also accepts an iterator (for iter functions)
            // and a world/stage.
        });

    // Create regular (mutable) entity to store in EntityView component
    auto Foo = ecs.entity("Foo");

    // Create entity with EntityView component to match with system. Store the
    // Foo entity handle in the component as a readonly handle.
    ecs.entity("Bar").set<EntityView>({ Foo });

    // Run the system
    ecs.progress();

    // Verify that Position has been added to Foo
    const Position *p = Foo.get<Position>();
    std::cout << "Foo position: " << p->x << ", " << p->y << std::endl;
}
