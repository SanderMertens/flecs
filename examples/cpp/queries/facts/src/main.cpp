#include <facts.h>
#include <iostream>

// This example shows how to use queries for testing facts. A fact is a query
// that has no variable elements. Consider a regular ECS query like this:
//   Position, Velocity
//
// When written out in full, this query looks like:
//   Position($this), Velocity($this)
//
// "this" is a (builtin) query variable that is unknown before we evaluate the 
// query. Therefore this query does not test a fact, we can't know which values
// This will assume.
//
// An example of a fact-checking query is:
//   IsA(Cat, Animal)
//
// This is a fact: the query has no elements that are unknown before evaluating
// the query. A query that checks a fact does not return entities, but will
// instead return the reasons why a fact is true (if it is true).

struct Likes { };

int main(int, char *[]) {
    flecs::world ecs;

    flecs::entity bob = ecs.entity("Bob");
    flecs::entity alice = ecs.entity("Alice");
    flecs::entity jane = ecs.entity("Jane");
    flecs::entity john = ecs.entity("John");

    bob.add<Likes>(alice);
    alice.add<Likes>(bob);
    jane.add<Likes>(john);
    john.add<Likes>(jane);

    bob.add<Likes>(john); // bit of drama

    // Create a query that checks if two entities like each other. By itself this
    // query is not a fact, but we can use it to check facts by populating both
    // of its variables.
    //
    // The equivalent query in the DSL is:
    //  Likes($x, $y), Likes($y, $x)
    //
    // Instead of using variables we could have created a query that referred the
    // entities directly, but then we would have to create a query for each
    // fact, vs reusing a single query for multiple facts.
    flecs::query<> friends = ecs.query_builder()
        .with<Likes>("$y").src("$x")
        .with<Likes>("$x").src("$y")
        .build();

    int x_var = friends.find_var("x");
    int y_var = friends.find_var("y");

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
