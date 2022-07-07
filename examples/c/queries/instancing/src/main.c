#include <instancing.h>
#include <stdio.h>

/* Instancing is the ability of queries to iterate results with fields that have
 * different numbers of elements. The term "instancing" is borrowed from 
 * graphics APIs, where it means reusing the same data for multiple "instances".
 * 
 * Query instancing works in much the same way. By default queries match all
 * components on the same entity. It is however possible to request data from
 * other entities, like getting the Position from the entity's parent.
 * 
 * Instancing refers to the ability of queries to iterate components for 
 * multiple entities while at the same time providing "instanced" components,
 * which are always provided one element at a time.
 * 
 * Instancing is often used in combination with parent-child relationships and
 * prefabs, but is applicable to any kind of query where some of the terms are
 * matched on N entities, and some on a single entity.
 * 
 * By default queries are not instanced, which means that if a result contains
 * mixed fields, entities will be iterated one by one instead of in batches.
 * This is safer, as code doesn't have to do anything different for owned and
 * shared fields, but does come at a performance penalty.
 * 
 * The each() iterator function always uses an instanced iterator under the
 * hood. This is transparent to the application, but improves performance. For
 * this reason using each() can be faster than using uninstanced iter().
 */

typedef struct { 
    double x, y;
} Position, Velocity;

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    // Create a query for Position, Velocity. We'll create a few entities that
    // have Velocity as owned and shared component.
    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter = {
            .terms = {
                // Position must always be owned by the entity
                { .id = ecs_id(Position), .subj.set.mask = EcsSelf }, 
                { .id = ecs_id(Velocity) } // Velocity may be shared (default)
            },
            .instanced = true
        }
    });

    // Create a prefab with Velocity. Prefabs are not matched with queries.
    ecs_entity_t prefab = ecs_new_prefab(world, "p");
    ecs_set(world, prefab, Velocity, {1, 2});

    // Create a few entities that own Position & share Velocity from the prefab.
    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add_pair(world, e1, EcsIsA, prefab);
    ecs_set(world, e1, Position, {10, 20});

    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_add_pair(world, e2, EcsIsA, prefab);
    ecs_set(world, e2, Position, {10, 20});

    // Create a few entities that own all components
    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    ecs_set(world, e3, Position, {10, 20});
    ecs_set(world, e3, Velocity, {3, 4});

    ecs_entity_t e4 = ecs_new_entity(world, "e4");
    ecs_set(world, e4, Position, {10, 20});
    ecs_set(world, e4, Velocity, {4, 5});

    // Iterate the instanced query. Note how when a query is instanced, it needs
    // to check whether a field is owned or not in order to know how to access
    // it. In the case of an owned field it is iterated as an array, whereas
    // in the case of a shared field, it is accessed as a pointer.
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        Position *p = ecs_term(&it, Position, 1);
        Position *v = ecs_term(&it, Velocity, 2);

        // Check if Velocity is owned, in which case it's accessed as array.
        // Position will always be owned, since we set the term to Self.
        if (ecs_term_is_owned(&it, 2)) { // Velocity is term 2
            printf("Velocity is owned\n");
            for (int i = 0; i < it.count; i ++) {
                // If Velocity is shared, access the field as an array.
                p[i].x += v[i].x;
                p[i].y += v[i].y;
                printf("%s: {%f, %f}\n", ecs_get_name(it.world, it.entities[i]),
                    p[i].x, p[i].y);
            }
        } else {
            printf("Velocity is shared\n");
            for (int i = 0; i < it.count; i ++) {
                // If Velocity is shared, access the field as a pointer.
                p[i].x += v->x;
                p[i].y += v->y;
                printf("%s: {%f, %f}\n", ecs_get_name(it.world, it.entities[i]),
                    p[i].x, p[i].y);
            }
        }
    }

    return ecs_fini(world);
}
