#include <component_inheritance.h>
#include <iostream>

// This example shows how queries can match components that derive from a base
// component through the IsA relationship. A query for the base component
// matches all entities that have a derived component, and can read the members
// that are inherited from the base. See the tag_inheritance example for
// inheritance trees built from tags instead of components.

// Buf is the base component. HealthBuf and ManaBuf derive from it, so they
// inherit the base members (the base subobject is stored at the start of the
// derived component).
struct Buf {
    float value;
};

struct HealthBuf : Buf { };
struct ManaBuf : Buf { };

int main(int, char *[]) {
    flecs::world ecs;

    // Make the ECS aware of the inheritance relationships.
    ecs.component<HealthBuf>().is_a<Buf>();
    ecs.component<ManaBuf>().is_a<Buf>();

    // Create a few entities with derived buf components
    ecs.entity("warrior").set<HealthBuf>({{ 10 }});
    ecs.entity("wizard").set<ManaBuf>({{ 25 }});
    ecs.entity("paladin").set<HealthBuf>({{ 5 }});

    // Create a query for the base component. This matches all entities with a
    // component that derives from Buf.
    flecs::query<Buf> q = ecs.query<Buf>();

    // Iterate the query. The Buf members are accessible for derived components.
    q.each([](flecs::entity e, Buf& b) {
        std::cout << e.name() << " has buf value " << b.value << "\n";
    });

    // Output:
    //   warrior has buf value 10
    //   paladin has buf value 5
    //   wizard has buf value 25
}
