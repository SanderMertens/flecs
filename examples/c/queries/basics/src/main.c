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

    // Create a query for Position, Velocity. Queries are the fastest way to
    // iterate entities as they cache results.
    ecs_query_t *q = ecs_query_init(ecs, &(ecs_query_desc_t){
        .filter.terms = {
            { .id = ecs_id(Position) }, 
            { .id = ecs_id(Velocity), .inout = EcsIn}
        }
    });

    // Create a few test entities for a Position, Velocity query
    ecs_entity_t e1 = ecs_new_entity(ecs, "e1");
    ecs_set(ecs, e1, Position, {10, 20});
    ecs_set(ecs, e1, Velocity, {1, 2});

    ecs_entity_t e2 = ecs_new_entity(ecs, "e2");
    ecs_set(ecs, e2, Position, {10, 20});
    ecs_set(ecs, e2, Velocity, {3, 4});

    // This entity will not match as it does not have Position, Velocity
    ecs_entity_t e3 = ecs_new_entity(ecs, "e3");
    ecs_set(ecs, e3, Position, {10, 20});

    // Iterate entities matching the query
    ecs_iter_t it = ecs_query_iter(ecs, q);

    // Outer loop, iterates archetypes
    while (ecs_query_next(&it)) {
        Position *p = ecs_term(&it, Position, 1);
        const Velocity *v = ecs_term(&it, Velocity, 2);

        // Inner loop, iterates entities in archetype
        for (int i = 0; i < it.count; i ++) {
            p[i].x += v[i].x;
            p[i].y += v[i].y;
            printf("%s: {%f, %f}\n", ecs_get_name(ecs, it.entities[i]), 
                p[i].x, p[i].y);
        }
    }

    // Filters are uncached queries. They are a bit slower to iterate but faster
    // to create & have lower overhead as they don't have to maintain a cache.
    ecs_filter_t *f = ecs_filter_init(ecs, &(ecs_filter_desc_t){
        .terms = {
            { .id = ecs_id(Position) }, 
            { .id = ecs_id(Velocity), .inout = EcsIn}
        }
    });

    // Filter iteration looks the same as query iteration
    it = ecs_filter_iter(ecs, f);

    while (ecs_filter_next(&it)) {
        Position *p = ecs_term(&it, Position, 1);
        const Velocity *v = ecs_term(&it, Velocity, 2);

        for (int i = 0; i < it.count; i ++) {
            p[i].x += v[i].x;
            p[i].y += v[i].y;
            printf("%s: {%f, %f}\n", ecs_get_name(ecs, it.entities[i]), 
                p[i].x, p[i].y);
        }
    }

    // Cleanup filter. Filters can allocate memory if the number of terms 
    // exceeds their internal buffer, or when terms have names. In this case the
    // filter didn't allocate, so while fini isn't strictly necessary here, it's
    // still good practice to add it.
    ecs_filter_fini(f);

    return ecs_fini(ecs);
}
