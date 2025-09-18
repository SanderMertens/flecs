#include <iter_targets.h>
#include <stdio.h>

// This example shows how to iterate matching targets of a relationship without
// iterating the same entity multiple times.
//
// When creating a (Relationship, *) query, the query will return one result per
// matching relationship pair, which returns the same entity multiple times. 
// This example uses a (Relationship, _) query which at most returns a single
// result per matching pair, and the returned table record to iterate the
// targets for the current result.

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_TAG(ecs, Eats);
    ECS_TAG(ecs, Pizza);
    ECS_TAG(ecs, Salad);

    ecs_entity_t bob = ecs_entity(ecs, { .name = "Bob" });
    ecs_add_pair(ecs, bob, Eats, Pizza);
    ecs_add_pair(ecs, bob, Eats, Salad);

    ecs_query_t *q = ecs_query(ecs, {
        // EcsAny ensures that only a single result is returned per entity, as 
        // opposed to EcsWildcard which returns a result per matched pair.
        .terms = {{ .id = ecs_pair(Eats, EcsAny) }}
    });
    
    ecs_iter_t it = ecs_query_iter(ecs, q);
    while (ecs_query_next(&it)) {
        const ecs_type_t *type = ecs_table_get_type(it.table);
        const ecs_table_record_t *tr = it.trs[0];

        for (int i = 0; i < it.count; i ++) {
            printf("%s eats:\n", ecs_get_name(ecs, it.entities[i]));
            for (int t = 0; t < tr->count; t ++) {
                ecs_entity_t tgt = ecs_pair_second(
                    ecs, type->array[t + tr->index]);
                printf(" - %s\n", ecs_get_name(ecs, tgt));
            }
        }
    }

    return ecs_fini(ecs);

    // Output:
    //  Bob eats: 
    //   - Pizza
    //   - Salad
}
