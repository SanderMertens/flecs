#include <cyclic_variables.h>
#include <stdio.h>

// This example shows how a query may have terms with cyclic dependencies on
// variables.

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

    // The following query will only return entities that have a cyclic Likes
    // relationship- that is they must both like each other.
    //
    // The equivalent query in the DSL is:
    //   Likes($X, $Y), Likes($Y, $X)
    //
    // This is also an example of a query where all sources are variables. By
    // default queries use the builtin "this" variable as subject, which is what
    // populates the entities array in the query result (accessed by the
    // iter::entity function).
    //
    // Because this query does not use This at all, the entities array will not
    // be populated, and it.count() will always be 0.
    // Create a query to find all ranged units
    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { .first.id = Likes, .src.name = "$x", .second.name = "$y" },
            { .first.id = Likes, .src.name = "$y", .second.name = "$x" }
        }
    });

    // Lookup the index of the variables. This will let us quickly lookup their
    // values while we're iterating.
    int x_var = ecs_query_find_var(q, "x");
    int y_var = ecs_query_find_var(q, "y");

    // Iterate the query
    ecs_iter_t it = ecs_query_iter(ecs, q);
    while (ecs_query_next(&it)) {
        ecs_entity_t x = ecs_iter_get_var(&it, x_var);
        ecs_entity_t y = ecs_iter_get_var(&it, y_var);
        printf("%s likes %s\n", ecs_get_name(ecs, x), ecs_get_name(ecs, y));
    }

    ecs_query_fini(q);

    // Output
    //  Bob likes Alice
    //  Jane likes John
    //  John likes Jane
    //  Alice likes Bob

    // Note that the query returns each pair twice. The reason for this is that
    // the goal of the query engine is to return all "facts" that are true
    // within the given constraints. Since we did not give it any constraints
    // that would favor a person being matched by X or Y, the query
    // returns both.

    return ecs_fini(ecs);
}
