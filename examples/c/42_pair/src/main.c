#include <pair.h>

/* Ordinary position & velocity components */
typedef struct {
    double x, y;
} Position, Velocity;

/* This component will be used as a relation. A relation can be added to an
 * entity together with a relation object, which is the thing to which relation
 * applies. In this case, the ExpiryTime relation will apply to a component that
 * we want to remove after a timeout. */
typedef struct ExpiryTimer {
    double expiry_time;
    double t;
} ExpiryTimer;

/* Create a system that matches all entities with an ExpiryTimer relation */
void ExpireComponents(ecs_iter_t *it) {
    /* First, get the pair data, just like a regular component */
    ExpiryTimer *et = ecs_term(it, ExpiryTimer, 1);

    /* Get the handle to the pair. This will tell us on which component the
     * pair is applied */
    ecs_entity_t pair = ecs_term_id(it, 1);

    /* Get the id of the component (object) of the pair */
    ecs_entity_t comp = ecs_pair_object(it->world, pair);

    /* Iterate as usual */
    for (int32_t i = 0; i < it->count; i ++) {
        /* Increase timer. When timer hits expiry time, remove component */
        et[i].t += (double)it->delta_time;
        if (et[i].t >= et[i].expiry_time) {
            /* Remove both the component and the pair. If the pair would not
             * be removed, the system would still be invoked after this. */
            printf("Remove component '%s'\n", ecs_get_name(it->world, comp));

            /* Removes component (Position or Velocity) */
            ecs_remove_id(it->world, it->entities[i], comp);

            /* Removes pair */
            ecs_remove_id(it->world, it->entities[i], pair);
        }
    }
}

int main(void) {
    ecs_world_t *world = ecs_init();

    /* Register components and pair as a regular component */
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, ExpiryTimer);

    /* Create a system that matches all entities with an ExpiryTimer relation */
    ECS_SYSTEM(world, ExpireComponents, EcsOnUpdate, (ExpiryTimer, *));

    /* Create an entity with Position and Velocity */
    ecs_entity_t e = ecs_new(world, 0);
    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);

    /* Assign pair with ExpiryTime and Position. After 3 seconds Position will
     * be removed from the entity */
    ecs_set_pair(world, e, ExpiryTimer, ecs_id(Position), {
        .expiry_time = 3
    });

    /* Also assign pair with ExpiryTime and Velocity. After 2 seconds the
     * Velocity component will be removed. */
    ecs_set_pair(world, e, ExpiryTimer, ecs_id(Velocity), {
        .expiry_time = 2
    });

    /* Note that ExpiryTime is added to the same entity twice, which happens
     * because the pair objects are different */

    /* Run the main loop until both components have been removed */
    ecs_set_target_fps(world, 10);

    printf("Running...\n");

    while (ecs_progress(world, 0)) {
        /* As soon as both components are removed, exit main loop */
        if (!ecs_get_type(world, e)) {
            break;
        }
    }

    return ecs_fini(world);
}
