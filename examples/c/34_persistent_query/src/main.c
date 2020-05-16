#include <persistent_query.h>

/* Component types */
typedef struct Vector2D {
    float x;
    float y;
} Vector2D;

/* Typedefs can be used as component types */
typedef Vector2D Position;
typedef Vector2D Velocity;

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    /* Create a query. Queries are 'persistent' meaning they are registered with
     * the world and continuously matched with new entities (tables). Queries
     * are the fastest way to iterate over entities, as a lot of processing is
     * done when entities are matched, outside of the main loop.
     *
     * Queries are the mechanism used by systems, and as such both accept the
     * same signature expressions, and have similar performance.  */
    ecs_query_t *q = ecs_query_new(world, "Position, Velocity");

    /* Create a few entities that match the query */
    ecs_entity_t E1 = ecs_set(world, 0, EcsName, {"E1"});
    ecs_set(world, E1, Position, {1, 2});
    ecs_set(world, E1, Velocity, {1, 1});

    ecs_entity_t E2 = ecs_set(world, 0, EcsName, {"E2"});
    ecs_set(world, E2, Position, {1, 2});
    ecs_set(world, E2, Velocity, {1, 1});

    /* Don't add Velocity here, E3 will not match query */
    ecs_entity_t E3 = ecs_set(world, 0, EcsName, {"E3"});
    ecs_set(world, E3, Position, {1, 2});

    /* Iterate over entities matching the query */
    ecs_query_iter_t it = ecs_query_iter(q, 0, 0);

    while (ecs_query_next(&it)) {
        ecs_rows_t *rows = &it.rows;
        
        ECS_COLUMN(rows, Position, p, 1);
        ECS_COLUMN(rows, Velocity, v, 2);

        for (int i = 0; i < rows->count; i ++) {
            p[i].x += v[i].x;
            p[i].y += v[i].y;

            printf("%s moved to {.x = %f, .y = %f}\n",
                ecs_get_name(world, rows->entities[i]), p[i].x, p[i].y);
        }
    }

    /* Cleanup */
    return ecs_fini(world);
}
