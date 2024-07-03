#include <basics.h>
#include <stdio.h>

typedef struct {
    double x;
    double y;
} Position, Velocity;

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Velocity);

    // Create a cached query for Position, Velocity. Cached queries are the 
    // fastest way to iterate entities as they cache results.
    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { .id = ecs_id(Position) }, 
            { .id = ecs_id(Velocity), .inout = EcsIn}
        },

        // QueryCache Auto automatically caches all terms that can be cached.
        .cache_kind = EcsQueryCacheAuto
    });

    // Create a few test entities for a Position, Velocity query
    ecs_entity_t e1 = ecs_entity(ecs, { .name = "e1" });
    ecs_set(ecs, e1, Position, {10, 20});
    ecs_set(ecs, e1, Velocity, {1, 2});

    ecs_entity_t e2 = ecs_entity(ecs, { .name = "e2" });
    ecs_set(ecs, e2, Position, {10, 20});
    ecs_set(ecs, e2, Velocity, {3, 4});

    // This entity will not match as it does not have Position, Velocity
    ecs_entity_t e3 = ecs_entity(ecs, { .name = "e3" });
    ecs_set(ecs, e3, Position, {10, 20});

    // Iterate entities matching the query
    ecs_iter_t it = ecs_query_iter(ecs, q);

    // Outer loop, iterates archetypes
    while (ecs_query_next(&it)) {
        Position *p = ecs_field(&it, Position, 0);
        const Velocity *v = ecs_field(&it, Velocity, 1);

        // Inner loop, iterates entities in archetype
        for (int i = 0; i < it.count; i ++) {
            p[i].x += v[i].x;
            p[i].y += v[i].y;
            printf("%s: {%f, %f}\n", ecs_get_name(ecs, it.entities[i]), 
                p[i].x, p[i].y);
        }
    }

    // Cleanup query resources.
    ecs_query_fini(q);

    return ecs_fini(ecs);
}
