#include <traits.h>
#include <iostream>

/* Ordinary position & velocity components */
struct Position {
    float x;
    float y;
};

struct Velocity {
    float x;
    float y;
};

/* This component will be used as a trait. A trait is a component that can be
 * added to an entity / component pair, which lets applications implement
 * generic functionality that works with any component. In this case, the
 * trait implements a timer after which the component it is attached to is
 * removed from the entity. */
struct ExpiryTimer {
    float expiry_time;
    float t;
};

int main(int argc, char *argv[]) {
    flecs::world ecs(argc, argv);

    /* Register trait component so that the system can resolve it by name */
    ecs.component<ExpiryTimer>();

    /* Create a system that matches ExpiryTimer as a trait. Without the TRAIT
     * role the system would look for entities that added ExpiryTimer as usual,
     * but with the role the system will be matched against every component to
     * which the trait has been applied. */
    ecs.system<>(nullptr, "TRAIT | ExpiryTimer")
        .action([](flecs::iter it) {
            /* First, get the trait component */
            flecs::column<ExpiryTimer> et(it, 1);

            /* Get the handle to the trait. This will tell us on which component
             * the trait is applied */
            flecs::entity trait = it.column_entity(1);

            /* Obtain the component handle. The component handle is always in 
             * the lower 32 bits of the trait handle. */
            flecs::entity comp = trait.lo();

            for (auto i : it) {
                /* Increase timer. When it equals expiry time, remove component */
                et[i].t += it.delta_time();
                if (et[i].t >= et[i].expiry_time) {
                    /* Remove both the component and the trait. If the trait 
                     * would not be removed, the system would still be invoked 
                     * after this. */
                    std::cout << "Remove component " << comp.name() << std::endl;

                    /* Removes component (Position or Velocity) */
                    it.entity(i).remove(comp);

                    /* Removes trait */
                    it.entity(i).remove(trait);                    
                }
            }
        });

    /* Create an entity with Position and Velocity */
    auto e = ecs.entity()
        .add<Position>()
        .add<Velocity>();

    /* Assign the trait to the Position component. After 3 seconds Position will
     * be removed from the entity */
    e.set_trait<ExpiryTimer, Position>({ 3 });

    /* Also assign the trait to the Velocity comopnent. After 2 seconds the
     * Velocity component will be removed. */
    e.set_trait<ExpiryTimer, Velocity>({ 2 });

    /* Note that the trait has been added to the same entity twice, which is not
     * something that is ordinarily possible with a component. */

    /* Run the main loop until both components have been removed */
    ecs.set_target_fps(1);

    /* Run systems */
    while (ecs.progress()) { 
        /* As soon as both components are removed, exit main loop */
        if (e.type().vector().count() == 0) {
            break;
        }
    }
}
