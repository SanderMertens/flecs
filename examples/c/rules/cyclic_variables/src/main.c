#include <cyclic_variables.h>
#include <stdio.h>

// This example shows how a rule may have terms with cyclic dependencies on
// variables.

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_TAG(ecs, Likes); // Likes relation

    ecs_entity_t bob = ecs_new_entity(ecs, "Bob");
    ecs_entity_t alice = ecs_new_entity(ecs, "Alice");
    ecs_entity_t john = ecs_new_entity(ecs, "John");
    ecs_entity_t jane = ecs_new_entity(ecs, "Jane");

    ecs_add_pair(ecs, bob, Likes, alice);
    ecs_add_pair(ecs, alice, Likes, bob);
    ecs_add_pair(ecs, john, Likes, jane);
    ecs_add_pair(ecs, jane, Likes, john);
    ecs_add_pair(ecs, bob, Likes, jane); // inserting a bit of drama

    // The following rule will only return entities that have a cyclic Likes
    // relationship- that is they must both like each other.
    //
    // The equivalent query in the DSL is:
    //   Likes($X, $Y), Likes($Y, $X)
    //
    // This is also an example of a query where all subjects are variables. By
    // default queries use the builtin "This" variable as subject, which is what
    // populates the entities array in the query result (accessed by the
    // iter::entity function).
    //
    // Because this query does not use This at all, the entities array will not
    // be populated, and it.count() will always be 0.
    // Create a rule to find all ranged units
    ecs_rule_t *r = ecs_rule_init(ecs, &(ecs_filter_desc_t){
        .terms = {
            { 
                .pred.entity = Likes, 
                .subj = { .name = (char*)"X", .var = EcsVarIsVariable },
                .obj = { .name = (char*)"Y", .var = EcsVarIsVariable }
            },
            { 
                .pred.entity = Likes, 
                .subj = { .name = (char*)"Y", .var = EcsVarIsVariable },
                .obj = { .name = (char*)"X", .var = EcsVarIsVariable }
            }
        }
    });

    // Lookup the index of the variables. This will let us quickly lookup their
    // values while we're iterating.
    int x_var = ecs_rule_find_var(r, "X");
    int y_var = ecs_rule_find_var(r, "Y");

    // Iterate the rule
    ecs_iter_t it = ecs_rule_iter(ecs, r);
    while (ecs_rule_next(&it)) {
        ecs_entity_t x = ecs_iter_get_var(&it, x_var);
        ecs_entity_t y = ecs_iter_get_var(&it, y_var);
        printf("%s likes %s\n", ecs_get_name(ecs, x), ecs_get_name(ecs, y));
    }

    ecs_rule_fini(r);

    // Output
    //  Bob likes Alice
    //  Jane likes John
    //  John likes Jane
    //  Alice likes Bob

    // Note that the rule returns each pair twice. The reason for this is that
    // the goal of the rule engine is to return all "facts" that are true
    // within the given constraints. Since we did not give it any constraints
    // that would favor a person being matched by X or Y, the rule engine
    // returns both.

    return ecs_fini(ecs);
}
