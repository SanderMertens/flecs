#include <pair_types.h>
#include <iostream>

// The relation type
struct Eats { int amount; };

// The object types
struct Apples { };
struct Pears { };

int main(int, char*[]) {
    flecs::world ecs;

    // Pair types are a feature that allow an application to define a relation-
    // object pair at compile time. Pair types can be used in any location where
    // the API expects a component type.
    //
    // When setting or getting a value with a pair type, the value assumes the
    // type in the pair that is not empty. If both of the types in the pair are
    // not empty, the pair assumes the type of the relation.
    using EatsApples = flecs::pair<Eats, Apples>;
    using EatsPears = flecs::pair<Eats, Pears>;

    // Query for entities with both (Eats, Apples) and (Eats, Pears)
    auto q = ecs.query<EatsApples, EatsPears>();

    // Pair types can be passed to function-style get/set's. Note that the pair
    // type must be passed by value, and that we need to use the -> operator
    // to access the value of the pair.
    auto e = ecs.entity()
        .set([](EatsApples a, EatsPears p) { 
            a->amount = 10; 
            p->amount = 20;
        });

    // Using a pair type is equivalent to specifying the individual types
    e.set<Eats, Apples>({10});
    e.set<EatsApples>({10});

    // The each function accepts pair types. Note that just as with the function
    // style get/set, the pair is passed by value and the -> operator is used.
    q.each([](EatsApples a, EatsPears p) {
        std::cout <<  "Apples: " << a->amount 
                  << ", Pears: " << p->amount << std::endl;
    });

    // When using the iter function, the actual type of the pair must be used.
    // This type can be either provided directly or indirectly, by providing the
    // pair_type_t trait.
    q.iter([](flecs::iter it, Eats *a, flecs::pair_type_t<EatsPears> *p) {
        for (auto i : it) {
            std::cout <<  "Apples: " << a[i].amount 
                      << ", Pears: " << p[i].amount << std::endl;
        }
    });
}
