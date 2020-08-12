#include <subquery.h>
#include <iostream>

/* Subqueries allow applications to narrow down the potential set of tables to
 * match with to a small size, which makes creation and deletion of queries 
 * efficient enough to do in the main loop. Additionally, subqueries have a
 * lower adminstrative footprint than regular queries, which reduces the amount
 * of memory that is required to store them. */

/* Component types */
struct Position {
    float x;
    float y;
};

struct Velocity {
    float x;
    float y;
};

int main(int argc, char *argv[]) {
    flecs::world world(argc, argv);

    flecs::entity(world, "e1")
        .add<Position>();

    flecs::entity(world, "e2")
        .add<Position>()
        .add<Velocity>();
        
    flecs::entity(world, "e3")
        .add<Velocity>();

    /* Create a parent query that subscribes for all entities with Position */
    auto q_parent = flecs::query<Position>(world);

    /* Create a subquery that selects the subset of entities matched by q_parent
     * that have the Velocity component. */
    auto q_sub = flecs::query<Velocity>(world, q_parent);

    /* Iterate the subquery. Note that only e2 is matched, since it has both
     * Position and Velocity */
    q_sub.each([](flecs::entity e, Velocity &v) {
        std::cout << e.name() << " matched" << std::endl;
    });

    /* The subquery could also have subscribed itself for both Position and
     * Velocity, in which case it would still only have matched against the
     * entities of the parent query, but the result would have been harder to
     * see as on the surface it would be the same  . */
}
