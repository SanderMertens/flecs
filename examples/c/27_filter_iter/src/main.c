#include <filter_iter.h>

typedef struct {
    double x, y;
} Position, Velocity;

typedef double Mass;

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t e1 = ecs_set_name(world, 0, "e1");
    ecs_set(world, e1, Position, {0, 0});
    ecs_set(world, e1, Velocity, {1, 1});

    ecs_entity_t e2 = ecs_set_name(world, 0, "e2");
    ecs_set(world, e2, Position, {10, 20});
    ecs_set(world, e2, Velocity, {1, 1});

    /* Create a filter for the Movable type, which includes Position and 
    * Velocity. Make sure to only match tables that include both Position and
    * Velocity. */
    ecs_filter_t f;
    ecs_filter_init(world, &f, &(ecs_filter_desc_t) {
        .terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Velocity) }
        }
    });

    /* Iteration of filter is the same as a query */
    ecs_iter_t it = ecs_filter_iter(world, &f);
    while (ecs_filter_next(&it)) {
        Position *p = ecs_term(&it, Position, 1);
        Velocity *v = ecs_term(&it, Velocity, 2);

        for (int i = 0; i < it.count; i ++) {
            p[i].x += v[i].x;
            p[i].y += v[i].y;

            printf("moved %s to {%f, %f}\n", 
                ecs_get_name(world, it.entities[i]), p[i].x, p[i].y);
        }
    }

    /* Cleanup */
    return ecs_fini(world);
}
