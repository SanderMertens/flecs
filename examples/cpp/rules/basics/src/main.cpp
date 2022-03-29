#include <basics.h>
#include <iostream>

struct Eats { };
struct Healthy { };

int main(int, char *[]) {
    flecs::world ecs;

    auto Apples = ecs.entity("Apples").add<Healthy>();
    auto Salad = ecs.entity("Salad").add<Healthy>();
    auto Burgers = ecs.entity("Burgers");
    auto Pizza = ecs.entity("Pizza");
    auto Chocolate = ecs.entity("Chocolate");

    ecs.entity("Bob")
        .add<Eats>(Apples)
        .add<Eats>(Burgers)
        .add<Eats>(Pizza);

    ecs.entity("Alice")
        .add<Eats>(Salad)
        .add<Eats>(Chocolate)
        .add<Eats>(Apples);

    // Here we're creating a rule that in the query DSL would look like this:
    //   Eats(This, $Food), Healthy($Food)
    //
    // Rules are similar to queries, but support more advanced features. This
    // example shows how the basics of how to use rules & variables.
    auto r = ecs.rule_builder()
        // Identifiers that start with _ are query variables. Query variables
        // are like wildcards, but enforce that the entity substituted by the
        // wildcard is the same across terms.
        //
        // For example, in this query it is not guaranteed that the entity
        // substituted by the * for Eats is the same as for Healthy:
        //   (Eats, *), Healthy(*)
        //
        // By replacing * with _Food, both terms are constrained to use the
        // same entity.
        .term<Eats>().obj().var("Food")
        .term<Healthy>().subj().var("Food")
        .build();
    
    // Lookup the index of the variable. This will let us quickly lookup its
    // value while we're iterating.
    int food_var = r.find_var("Food");

    // Iterate the rule
    r.each([&](flecs::iter& it, size_t index) {
        std::cout 
            << it.entity(index).name()
            << " eats " << it.get_var(food_var).name()
            << "\n";
    });

    // Rules need to be explicitly deleted.
    r.destruct();

    // Output:
    //  Bob eats Apples
    //  Alice eats Apples
    //  Alice eats Salad
}
