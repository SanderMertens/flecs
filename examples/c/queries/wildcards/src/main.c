#include <wildcards.h>
#include <stdio.h>

// Queries can have wildcard terms that can match multiple instances of a
// relationship or relationship target.

typedef struct {
    int amount;
} Eats;

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, Eats);
    ECS_TAG(ecs, Apples);
    ECS_TAG(ecs, Pears);

    // Create a query that matches edible components
    ecs_query_t *q = ecs_query_init(ecs, &(ecs_query_desc_t){
        .filter.terms = {{ .id = ecs_pair(ecs_id(Eats), EcsWildcard ) }}
    });

    // Create a few entities that match the query
    ecs_entity_t bob = ecs_new_entity(ecs, "Bob");
    ecs_set_pair(ecs, bob, Eats, Apples, {10});
    ecs_set_pair(ecs, bob, Eats, Pears, {5});

    ecs_entity_t alice = ecs_new_entity(ecs, "Alice");
    ecs_set_pair(ecs, alice, Eats, Apples, {4});

    // Iterate the query
    ecs_iter_t it = ecs_query_iter(ecs, q);
    while (ecs_query_next(&it)) {
        Eats *eats = ecs_term(&it, Eats, 1);
        ecs_id_t pair = ecs_term_id(&it, 1);
        ecs_entity_t food = ecs_pair_second(ecs, pair);

        for (int i = 0; i < it.count; i ++) {
            printf("%s eats %d %s\n", ecs_get_name(ecs, it.entities[i]),
                eats[i].amount, ecs_get_name(ecs, food));
        }
    }

    // Output:
    //   Bob eats 10 Apples
    //   Bob eats 5 Pears
    //   Alice eats 4 Apples   

    return ecs_fini(ecs);
}
