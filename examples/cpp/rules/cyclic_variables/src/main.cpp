#include <cyclic_variables.h>
#include <iostream>

// This example shows how a rule may have terms with cyclic dependencies on
// variables.

struct Likes { };

int main(int, char *[]) {
    flecs::world ecs;

    auto bob = ecs.entity("Bob");
    auto alice = ecs.entity("Alice");
    auto john = ecs.entity("John");
    auto jane = ecs.entity("Jane");

    bob.add<Likes>(alice);
    alice.add<Likes>(bob);
    john.add<Likes>(jane);
    jane.add<Likes>(john);
    bob.add<Likes>(jane); // inserting a bit of drama

    // The following rule will only return entities that have a cyclic Likes
    // relationship- that is they must both like each other.
    //
    // The equivalent query in the DSL is:
    //   Likes($X, $Y), Likes($Y, $X)
    //
    // This is also an example of a query where all sources are variables. By
    // default queries use the builtin "This" variable as subject, which is what
    // populates the entities array in the query result (accessed by the
    // iter::entity function). 
    //
    // Because this query does not use This at all, the entities array will not
    // be populated, and it.count() will always be 0.
    auto r = ecs.rule_builder()
        .term<Likes>("$Y").src("$X")
        .term<Likes>("$X").src("$Y")
        .build();

    // Lookup the index of the variables. This will let us quickly lookup their
    // values while we're iterating.
    int x_var = r.find_var("X");
    int y_var = r.find_var("Y");

    // Because the query doesn't use the This variable we cannot use "each"
    // which iterates the entities array. Instead we can use iter like this:
    r.iter([&](flecs::iter& it) {
        auto x = it.get_var(x_var);
        auto y = it.get_var(y_var);
        std::cout << x.name() << " likes " << y.name() << "\n";
    });

    // Output
    //  Alice likes Bob
    //  Bob likes Alice
    //  Jane likes John
    //  John likes Jane

    // Note that the rule returns each pair twice. The reason for this is that
    // the goal of the rule engine is to return all "facts" that are true
    // within the given constraints. Since we did not give it any constraints
    // that would favor a person being matched by X or Y, the rule engine
    // returns both.

    r.destruct();
}
