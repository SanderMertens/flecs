#include <traits.h>

/* Ordinary position & velocity components */
typedef struct Position {
    float x;
    float y;
} Position;

typedef struct Velocity {
    float x;
    float y;
} Velocity;

/* This component will be used as a trait. A trait is a component that can be
 * added to an entity / component pair, which lets applications implement
 * generic functionality that works with any component. In this case, the
 * trait implements a timer after which the component it is attached to is
 * removed from the entity. */
typedef struct ExpiryTimer {
    float expiry_time;
    float t;
} ExpiryTimer;

/* The system that executes the trait logic */
void ExpireComponents(ecs_iter_t *it) {
    /* First, get the trait component just like a normal component */
    ExpiryTimer *et = ecs_column(it, ExpiryTimer, 1);

    /* Get the handle to the trait. This will tell us on which component the
     * trait is applied */
    ecs_entity_t trait = ecs_column_entity(it, 1);

    /* Obtain the component handle. The component handle is always in the lower
     * 32 bits of the trait handle, which can be obtained with the 
     * ecs_entity_t_lo macro. */
    ecs_entity_t comp = ecs_entity_t_lo(trait);

    /* Iterate trait component as usual ... */
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        /* Increase timer. When timer hits expiry time, remove component */
        et[i].t += it->delta_time;
        if (et[i].t >= et[i].expiry_time) {
            /* Remove both the component and the trait. If the trait would not
             * be removed, the system would still be invoked after this. */
            printf("Remove component '%s'\n", ecs_get_name(it->world, comp));

            /* Removes component (Position or Velocity) */
            ecs_remove_entity(it->world, it->entities[i], comp);

            /* Removes trait */
            ecs_remove_entity(it->world, it->entities[i], trait);
        }
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init();

    /* Register components and trait as a regular component */
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, ExpiryTimer);

    /* Create a system that matches ExpiryTimer as a trait. Without the TRAIT
     * role the system would look for entities that added ExpiryTimer as usual,
     * but with the role the system will be matched against every component to
     * which the trait has been applied. */
    ECS_SYSTEM(world, ExpireComponents, EcsOnUpdate, TRAIT | ExpiryTimer);

    /* Create an entity with Position and Velocity */
    ecs_entity_t e = ecs_new(world, 0);
    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);

    /* Assign the trait to the Position component. After 3 seconds Position will
     * be removed from the entity */
    ecs_set_trait(world, e, Position, ExpiryTimer, {
        .expiry_time = 3
    });

    /* Also assign the trait to the Velocity comopnent. After 2 seconds the
     * Velocity component will be removed. */
    ecs_set_trait(world, e, Velocity, ExpiryTimer, {
        .expiry_time = 2
    });

    /* Note that the trait has been added to the same entity twice, which is not
     * something that is ordinarily possible with a component. */

    /* Run the main loop until both components have been removed */
    ecs_set_target_fps(world, 10);

    while (ecs_progress(world, 0)) {
        /* As soon as both components are removed, exit main loop */
        if (!ecs_get_type(world, e)) {
            break;
        }
    }

    return ecs_fini(world);
}
