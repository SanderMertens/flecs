#include <relation_component.h>
#include <iostream>

// This example shows how relations can be combined with components to attach
// data to a relationship.

// Some demo components:

struct Requires {
  double amount;
};

struct Gigawatts { };

struct Expires {
    double timeout;
};

struct Position {
    double x;
    double y;
};

struct MustHave { };

int main(int, char*[]) {
    flecs::world ecs;

    // When one element of a pair is a component and the other element is a tag, 
    // the pair assumes the type of the component. 
    auto e1 = ecs.entity().set<Requires, Gigawatts>({1.21});
    const Requires *r = e1.get<Requires, Gigawatts>();
    std::cout << "requires: " << r->amount << std::endl;

    // The component can be either the first or second part of a pair:
    auto e2 = ecs.entity().set<Gigawatts, Requires>({1.21});
    r = e2.get<Gigawatts, Requires>();
    std::cout << "requires: " << r->amount << std::endl;

    // Note that <Requires, Gigawatts> and <Gigawatts, Requires> are two 
    // different pairs, and can be added to an entity at the same time.

    // If both parts of a pair are components, the pair assumes the type of
    // the first element:
    auto e3 = ecs.entity().set<Expires, Position>({0.5});
    const Expires *e = e3.get<Expires, Position>();
    std::cout << "expires: " << e->timeout << std::endl;

    // You can prevent a pair from assuming the type of a component by adding
    // the Tag property to a relationship:
    ecs.component<MustHave>().add(flecs::Tag);

    // Even though Position is a component, <MustHave, Position> contains no
    // data because MustHave has the Tag property.
    ecs.entity().add<MustHave, Position>();

    // The id::type_id method can be used to find the component type for a pair:
    std::cout << ecs.pair<Requires, Gigawatts>().type_id().path() << "\n";
    std::cout << ecs.pair<Gigawatts, Requires>().type_id().path() << "\n";
    std::cout << ecs.pair<Expires, Position>().type_id().path() << "\n";
    std::cout << ecs.pair<MustHave, Position>().type_id().path() << "\n";

    // When querying for a relationship component, add the pair type as template
    // argument to the builder:
    auto q = ecs.query_builder<Requires>()
        .arg(1).obj<Gigawatts>() // set second part of pair for first term
        .build();
    
    // When iterating, always use the pair type:
    q.each([](Requires& rq) {
        std::cout << "requires " << rq.amount << " gigawatts\n";
    });

    // Output:
    //  requires: 1.21
    //  requires: 1.21
    //  expires: 0.5
    //  ::Requires
    //  ::Requires
    //  ::Expires
    //  0
    //  requires 1.21 gigawatts
}
