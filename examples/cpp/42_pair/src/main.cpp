#include <pair.h>
#include <iostream>

/* Ordinary position & velocity components */
struct Position {
    double x, y;
};

struct Velocity {
    double x, y;
};

/* This component will be used as a relation. A relation can be added to an
 * entity together with a relation object, which is the thing to which relation
 * applies. In this case, the ExpiryTime relation will apply to a component that
 * we want to remove after a timeout. */
struct ExpiryTimer {
    float expiry_time;
    float t;
};

int main(int argc, char *argv[]) {
    flecs::world ecs(argc, argv);

    /* Create a system that matches all entities with an ExpiryTimer relation */
    ecs.system<>()
        .term<ExpiryTimer>(flecs::Wildcard)
        .iter([](flecs::iter it) {
            /* First, get the pair component */
            auto et = it.term<ExpiryTimer>(1);

            /* Get the id of the term, which contains the pair */
            flecs::entity pair = it.term_id(1);

            /* Obtain component id, which is the 'object' part of the pair */
            flecs::entity obj = pair.object();

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

    std::cout << "Running..." << std::endl;

    /* Run systems */
    while (ecs.progress()) { 
        /* As soon as both components are removed, exit main loop */
        if (e.type().vector().count() == 0) {
            break;
        }
    }
}
