#include <facts.h>
#include <iostream>

// This example shows how to use rules for testing facts. A fact is a query that
// has no variable elements. Consider a regular ECS query like this:
//   Position, Velocity
//
// When written out in full, this query looks like:
//   Position(This), Velocity(This)
//
// "This" is a (builtin) query variable that is unknown before we evaluate the 
// query. Therefore this query does not test a fact, we can't know which values
// This will assume.
//
// An example of a fact-checking query is:
//   IsA(Cat, Animal)
//
// This is a fact: the query has no elements that are unknown before evaluating
// the query. A rule that checks a fact does not return entities, but will
// instead return the reasons why a fact is true (if it is true).

struct Likes { };

int main(int, char *[]) {
    flecs::world ecs;

    auto bob = ecs.entity("Bob");
    auto alice = ecs.entity("Alice");
    auto jane = ecs.entity("Jane");
    auto john = ecs.entity("John");

    bob.add<Likes>(alice);
    alice.add<Likes>(bob);
    jane.add<Likes>(john);
    john.add<Likes>(jane);

    bob.add<Likes>(john); // bit of drama

    // Create a rule that checks if two entities like each other. By itself this
    // rule is not a fact, but we can use it to check facts by populating both
    // of its variables.
    //
    // The equivalent query in the DSL is:
    //  Likes($X, $Y), Likes($Y, $X)
    //
    // Instead of using variables we could have created a rule that referred the
    // entities directly, but then we would have to create a rule for each
    // fact, vs reusing a single rule for multiple facts.
    auto friends = ecs.rule_builder()
        .term<Likes>().subj().var("X").obj().var("Y")
        .term<Likes>().subj().var("Y").obj().var("X")
        .build();

    int x_var = friends.find_var("X");
    int y_var = friends.find_var("Y");

    // Check a few facts

    std::cout << "Are Bob and Alice friends? " << 
        (friends.iter()
            .set_var(x_var, bob)
            .set_var(y_var, alice)
            .is_true() ? "Yes\n" : "No\n");

    std::cout << "Are Bob and John friends? " << 
        (friends.iter()
            .set_var(x_var, bob)
            .set_var(y_var, john)
            .is_true() ? "Yes\n" : "No\n");

    std::cout << "Are Jane and John friends? " << 
        (friends.iter()
            .set_var(x_var, jane)
            .set_var(y_var, john)
            .is_true() ? "Yes\n" : "No\n");

    // It doesn't matter who we assign to X or Y. After the variables are 
    // substituted, either yields a fact that is true.
    std::cout << "Are John and Jane friends? " << 
        (friends.iter()
            .set_var(x_var, john)
            .set_var(y_var, jane)
            .is_true() ? "Yes\n" : "No\n");

    // Output
    //  Are Bob and Alice friends? Yes
    //  Are Bob and John friends? No
    //  Are Jane and John friends? Yes
    //  Are John and Jane friends? Yes
}
