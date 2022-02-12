#include <wildcards.h>
#include <iostream>

// Queries can have wildcard terms that can match multiple instances of a
// relationship or relationship target.

struct Eats {
    int amount;
};

struct Apples { };
struct Pears { };

int main(int, char *[]) {
    flecs::world ecs;

    // Create a query that matches edible components
    auto q = ecs.query_builder<Eats>()
        .arg(1).obj(flecs::Wildcard)   // Change first argument to (Eats, *)
        .build();

    // Create a few entities that match the query
    ecs.entity("Bob")
        .set<Eats, Apples>({10})
        .set<Eats, Pears>({5});

    ecs.entity("Alice")
        .set<Eats, Apples>({4});

    // Iterate the query with a flecs::iter. This makes it possible to inspect
    // the pair that we are currently matched with.
    q.each([](flecs::iter& it, size_t index, Eats& eats) {
        auto e = it.entity(index);
        auto food = it.pair(1).second();

        std::cout << e.name() << " eats " 
            << eats.amount << " " << food.name() << std::endl;
    });

    // Output:
    //   Bob eats 10 Apples
    //   Bob eats 5 Pears
    //   Alice eats 4 Apples    
}
