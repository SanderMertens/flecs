#include <variables.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_TAG(ecs, Eats);
    ECS_TAG(ecs, Healthy);

    ecs_entity_t Apples = ecs_entity(ecs, { .name = "Apples" });
    ecs_entity_t Salad = ecs_entity(ecs, { .name = "Salad" });
    ecs_entity_t Burgers = ecs_entity(ecs, { .name = "Burgers" });
    ecs_entity_t Pizza = ecs_entity(ecs, { .name = "Pizza" });
    ecs_entity_t Chocolate = ecs_entity(ecs, { .name = "Chocolate" });

    ecs_add(ecs, Apples, Healthy);
    ecs_add(ecs, Salad, Healthy);

    ecs_entity_t bob = ecs_entity(ecs, { .name = "Bob" });
    ecs_add_pair(ecs, bob, Eats, Apples);
    ecs_add_pair(ecs, bob, Eats, Burgers);
    ecs_add_pair(ecs, bob, Eats, Pizza);

    ecs_entity_t alice = ecs_entity(ecs, { .name = "Alice" });
    ecs_add_pair(ecs, alice, Eats, Salad);
    ecs_add_pair(ecs, alice, Eats, Chocolate);
    ecs_add_pair(ecs, alice, Eats, Apples);

    // Here we're creating a query that in the query DSL would look like this:
    //   Eats($this, $Food), Healthy($Food)
    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            // Query variables are like wildcards, but enforce that the entity
            // substituted by the wildcard is the same across terms.
            //
            // For example, in this query it is not guaranteed that the entity
            // substituted by the * for Eats is the same as for Healthy:
            //   (Eats, *), Healthy(*)
            //
            // By replacing * with $food, both terms will be constrained to
            // match the same entity for the wildcard.
            { .first.id = Eats,    .second.name = "$food" },
            { .first.id = Healthy, .src.name    = "$food" }
        }
    });

    // Lookup the index of the variable. This will let us quickly lookup its
    // value while we're iterating.
    int food_var = ecs_query_find_var(q, "food");

    // Iterate the query
    ecs_iter_t it = ecs_query_iter(ecs, q);
    while (ecs_query_next(&it)) {
        ecs_entity_t food = ecs_iter_get_var(&it, food_var);
        for (int i = 0; i < it.count; i ++) {
            printf("%s eats %s\n", ecs_get_name(ecs, it.entities[i]),
                ecs_get_name(ecs, food));
        }
    }

    ecs_query_fini(q);

    // Output
    //  Bob eats Apples
    //  Alice eats Apples
    //  Alice eats Salad

    return ecs_fini(ecs);
}
