#include <traits.h>
#include <iostream>

/* Ordinary position & velocity components */
struct Position {
    double x, y;
};

struct Velocity {
    double x, y;
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

    /* Register pair component so that the system can resolve it by name */
    ecs.component<ExpiryTimer>();

    /* Create a system that matches ExpiryTimer as a pair. Without the PAIR
     * role the system would look for entities that added ExpiryTimer as usual,
     * but with the role the system will be matched against every component to
     * which the pair has been applied. */
    ecs.system<>(nullptr, "PAIR | ExpiryTimer")
        .iter([](flecs::iter it) {
            /* First, get the pair component */
            auto et = it.term<ExpiryTimer>(1);

            /* Get the id of the term, which contains the pair */
            flecs::entity pair = it.term_id(1);

            std::cout << "WORLD: " << pair.world().c_ptr() << std::endl;

            /* Obtain component id, which is the 'object' part of the pair */
            flecs::entity obj = pair.object();

            std::cout << "WORLD: " << obj.world().c_ptr() << std::endl;

            for (auto i : it) {
                /* Increase timer. When it equals expiry time, remove component */
                et[i].t += it.delta_time();
                if (et[i].t >= et[i].expiry_time) {
                    /* Remove both the component and the pair. If the pair 
                     * would not be removed, the system would still be invoked 
                     * after this. */
                    std::cout << "Remove component " << obj.name() << std::endl;

                    /* Removes component (Position or Velocity) */
                    it.entity(i).remove(obj);

                    /* Removes pair */
                    it.entity(i).remove(pair);                    
                }
            }
        });

    /* Create an entity with Position and Velocity */
    auto e = ecs.entity()
        .add<Position>()
        .add<Velocity>();

    /* Assign the pair to the Position component. After 3 seconds Position will
     * be removed from the entity */
    e.set<ExpiryTimer, Position>({ 3, 0 });

    /* Also assign the pair to the Velocity comopnent. After 2 seconds the
     * Velocity component will be removed. */
    e.set<ExpiryTimer, Velocity>({ 2, 0 });

    /* Note that the pair has been added to the same entity twice, which is not
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
