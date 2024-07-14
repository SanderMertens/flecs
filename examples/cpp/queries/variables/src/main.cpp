#include <variables.h>
#include <iostream>

struct Eats { };
struct Healthy { };

int main(int, char *[]) {
    flecs::world ecs;

    flecs::entity Apples = ecs.entity("Apples").add<Healthy>();
    flecs::entity Salad = ecs.entity("Salad").add<Healthy>();
    flecs::entity Burgers = ecs.entity("Burgers");
    flecs::entity Pizza = ecs.entity("Pizza");
    flecs::entity Chocolate = ecs.entity("Chocolate");

    ecs.entity("Bob")
        .add<Eats>(Apples)
        .add<Eats>(Burgers)
        .add<Eats>(Pizza);

    ecs.entity("Alice")
        .add<Eats>(Salad)
        .add<Eats>(Chocolate)
        .add<Eats>(Apples);

    // Here we're creating a query that in the query DSL would look like this:
    //   Eats($this, $Food), Healthy($Food)
    flecs::query<> q = ecs.query_builder()
        // Identifiers that start with $ are query variables. Query variables
        // are like wildcards, but enforce that the entity substituted by the
        // wildcard is the same across terms.
        //
        // For example, in this query it is not guaranteed that the entity
        // substituted by the * for Eats is the same as for Healthy:
        //   (Eats, *), Healthy(*)
        //
        // By replacing * with $food, both terms will be constrained to
        // match the same entity for the wildcard.
        .with<Eats>("$food")
        .with<Healthy>().src("$food")
        .build();

    // Lookup the index of the variable. This will let us quickly lookup its
    // value while we're iterating.
    int food_var = q.find_var("food");

    // Iterate the query
    q.each([&](flecs::iter& it, size_t index) {
        std::cout 
            << it.entity(index).name()
            << " eats " << it.get_var(food_var).name()
            << "\n";
    });

    // Output:
    //  Bob eats Apples
    //  Alice eats Apples
    //  Alice eats Salad
}
