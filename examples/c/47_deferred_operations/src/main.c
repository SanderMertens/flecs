#include <deferred_operations.h>

typedef struct {
    double x, y;
} Position, Velocity;

void SetVelocity(ecs_iter_t *it) {
    Velocity *v = ecs_term(it, Velocity, 1);

    for (int i = 0; i < it->count; i ++) {
        printf("Velocity set to {%f, %f}\n", v[i].x, v[i].y);
    }
}

int main(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    /* Create OnSet system so we can see when Velocity is actually set */
    ECS_SYSTEM(world, SetVelocity, EcsOnSet, Velocity);

    /* Create 3 entities that have Position */
    ecs_bulk_new(world, Position, 3);
    
    /* Create a query for Position to set Velocity for each entity with Position.
     * Because adding a component changes the underlying data structures, we
     * need to defer the operations until we have finished iterating. */
    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(q);

    printf("Defer begin\n");
    ecs_defer_begin(world);

    while (ecs_query_next(&it)) {
        for (int i = 0; i < it.count; i ++) {
            ecs_set(world, it.entities[i], Velocity, {1, 2});
        }
    }

    printf("Defer end\n");
    ecs_defer_end(world);

    ecs_fini(world);
}
