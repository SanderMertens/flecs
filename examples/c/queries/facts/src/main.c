#include <facts.h>
#include <stdio.h>

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

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_TAG(ecs, Likes); // Likes relationship

    ecs_entity_t bob = ecs_entity(ecs, { .name = "Bob" });
    ecs_entity_t alice = ecs_entity(ecs, { .name = "Alice" });
    ecs_entity_t john = ecs_entity(ecs, { .name = "John" });
    ecs_entity_t jane = ecs_entity(ecs, { .name = "Jane" });

    ecs_add_pair(ecs, bob, Likes, alice);
    ecs_add_pair(ecs, alice, Likes, bob);
    ecs_add_pair(ecs, john, Likes, jane);
    ecs_add_pair(ecs, jane, Likes, john);
    ecs_add_pair(ecs, bob, Likes, jane); // inserting a bit of drama

    // Create a query that checks if two entities like each other. By itself this
    // query is not a fact, but we can use it to check facts by populating both
    // of its variables.
    //
    // The equivalent of this query in the DSL is:
    //   Likes($X, $Y), Likes($Y, $X)
    //
    // Instead of using variables we could have created a query that referred the
    // entities directly, but then we would have to create a query for each
    // fact, vs reusing a single query for multiple facts.
    //
    // See the setting_variables example for more details
    ecs_query_t *friends = ecs_query(ecs, {
        .terms = {
            { .first.id = Likes, .src.name = "$x", .second.name = "$y" },
            { .first.id = Likes, .src.name = "$x", .second.name = "$x" }
        }
    });

    // Lookup the index of the variables.
    int x_var = ecs_query_find_var(friends, "x");
    int y_var = ecs_query_find_var(friends, "y");

    ecs_iter_t it = ecs_query_iter(ecs, friends);
    ecs_iter_set_var(&it, x_var, bob);
    ecs_iter_set_var(&it, y_var, alice);
    printf("Are Bob and Alice friends? %s\n",
        ecs_iter_is_true(&it) ? "Yes" : "No");

    it = ecs_query_iter(ecs, friends);
    ecs_iter_set_var(&it, x_var, bob);
    ecs_iter_set_var(&it, y_var, john);
    printf("Are Bob and John friends? %s\n",
        ecs_iter_is_true(&it) ? "Yes" : "No");

    it = ecs_query_iter(ecs, friends);
    ecs_iter_set_var(&it, x_var, jane);
    ecs_iter_set_var(&it, y_var, john);
    printf("Are Jane and John friends? %s\n",
        ecs_iter_is_true(&it) ? "Yes" : "No");        

    // It doesn't matter who we assign to X or Y. After the variables are 
    // substituted, either yields a fact that is true.
    it = ecs_query_iter(ecs, friends);
    ecs_iter_set_var(&it, x_var, john);
    ecs_iter_set_var(&it, y_var, jane);
    printf("Are John and Jane friends? %s\n",
        ecs_iter_is_true(&it) ? "Yes" : "No"); 

    ecs_query_fini(friends);

    // Output
    //  Are Bob and Alice friends? Yes
    //  Are Bob and John friends? No
    //  Are Jane and John friends? Yes
    //  Are John and Jane friends? Yes

    return ecs_fini(ecs);
}
