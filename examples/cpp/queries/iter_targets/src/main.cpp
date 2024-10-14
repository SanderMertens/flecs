#include <iter_targets.h>
#include <iostream>

// This example shows how to iterate matching targets of a relationship without
// iterating the same entity multiple times.
//
// When creating a (Relationship, *) query, the query will return one result per
// matching relationship pair, which returns the same entity multiple times. 
// This example uses a (Relationship, _) query which at most returns a single
// result per matching pair, and a targets() function that iterates the targets
// for the current entity.

struct Eats { };
struct Pizza { };
struct Salad { };

int main(int, char *[]) {
    flecs::world ecs;

    ecs.entity("Bob")
        .add<Eats, Pizza>()
        .add<Eats, Salad>();

    ecs.query_builder()
        .with<Eats>(flecs::Any) // flecs::Any ensures that only a single result
                                // is returned per entity, as opposed to 
                                // flecs::Wildcard which returns a result per
                                // matched pair.

        .each([](flecs::iter& it, size_t row) {
            flecs::entity e = it.entity(row);
            std::cout << e.name() << " eats: \n";

            it.targets(0, [](flecs::entity tgt) {
                std::cout << " - " << tgt.name() << "\n";
            });
        });

    // Output:
    //  Bob eats: 
    //   - Pizza
    //   - Salad
}
