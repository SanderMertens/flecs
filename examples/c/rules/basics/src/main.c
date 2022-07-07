#include <basics.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_TAG(ecs, Eats);
    ECS_TAG(ecs, Healthy);

    ecs_entity_t Apples = ecs_new_entity(ecs, "Apples");
    ecs_entity_t Salad = ecs_new_entity(ecs, "Salad");
    ecs_entity_t Burgers = ecs_new_entity(ecs, "Burgers");
    ecs_entity_t Pizza = ecs_new_entity(ecs, "Pizza");
    ecs_entity_t Chocolate = ecs_new_entity(ecs, "Chocolate");

    ecs_add(ecs, Apples, Healthy);
    ecs_add(ecs, Salad, Healthy);

    ecs_entity_t bob = ecs_new_entity(ecs, "Bob");
    ecs_add_pair(ecs, bob, Eats, Apples);
    ecs_add_pair(ecs, bob, Eats, Burgers);
    ecs_add_pair(ecs, bob, Eats, Pizza);

    ecs_entity_t alice = ecs_new_entity(ecs, "Alice");
    ecs_add_pair(ecs, alice, Eats, Salad);
    ecs_add_pair(ecs, alice, Eats, Chocolate);
    ecs_add_pair(ecs, alice, Eats, Apples);

    // Here we're creating a rule that in the query DSL would look like this:
    //   Eats(This, $Food), Healthy($Food)
    //
    // Rules are similar to queries, but support more advanced features. This
    // example shows how the basics of how to use rules & variables.
    ecs_rule_t *r = ecs_rule_init(ecs, &(ecs_filter_desc_t){
        .terms = {
            // Query variables are like wildcards, but enforce that the entity
            // substituted by the wildcard is the same across terms.
            //
            // For example, in this query it is not guaranteed that the entity
            // substituted by the * for Eats is the same as for Healthy:
            //   (Eats, *), Healthy(*)
            //
            // By replacing * with _Food, both terms are constrained to use the
            // same entity.
            { .pred.entity = Eats, .obj = { 
                .name = (char*)"Food", .var = EcsVarIsVariable },
            },
            { .pred.entity = Healthy, .subj = {
                .name = (char*)"Food", .var = EcsVarIsVariable }
            }
        }
    });

    // Lookup the index of the variable. This will let us quickly lookup its
    // value while we're iterating.
    int food_var = ecs_rule_find_var(r, "Food");

    // Iterate the rule
    ecs_iter_t it = ecs_rule_iter(ecs, r);
    while (ecs_rule_next(&it)) {
        ecs_entity_t food = ecs_iter_get_var(&it, food_var);
        for (int i = 0; i < it.count; i ++) {
            printf("%s eats %s\n", ecs_get_name(ecs, it.entities[i]),
                ecs_get_name(ecs, food));
        }
    }

    // Rules need to be explicitly deleted
    ecs_rule_fini(r);

    // Output
    //  Bob eats Apples
    //  Alice eats Apples
    //  Alice eats Salad

    return ecs_fini(ecs);
}
