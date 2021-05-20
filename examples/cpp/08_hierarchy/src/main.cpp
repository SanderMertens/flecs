#include <hierarchy.h>
#include <iostream>

struct Position {
    double x, y;
};

struct WorldPosition {
    double x, y;
};

struct Velocity {
    double x, y;
};

/* Implement a simple move system */
void Move(flecs::entity e, Position& p, const Velocity& v) {
    p.x += v.x;
    p.y += v.y;

    std::cout << e.name() << " moved to {.x = "
        << p.x << ", .y = "
        << p.y << "}" << std::endl;
}

/* Implement a system that transforms world coordinates hierarchically. If the 
 * CASCADE column is set, it points to the world coordinate of the parent. This
 * will be used to then transform Position to WorldPosition of the child.
 * If the CASCADE column is not set, the system matched a root. In that case,
 * just assign the Position to the WorldPosition. */
void Transform(flecs::iter& it, WorldPosition* wp, Position *p) {
    auto parent_wp = it.term<const WorldPosition>(3);

    if (!parent_wp.is_set()) {
        for (auto row : it) {
            wp[row].x = p[row].x;
            wp[row].y = p[row].y;

            std::cout << it.entity(row).name() << " transformed to {.x = "
                << wp[row].x << ", .y = "
                << wp[row].y << "} <<root>>" << std::endl;
        }
    } else {
        for (auto row : it) {
            /* Note that we're not using row to access parent_wp. This function
             * ('Transform') is invoked for every matching archetype, and the 
             * parent is part of the archetype. That means that all entities 
             * that are being iterated over have the same, single parent. */
            wp[row].x = parent_wp->x + p[row].x;
            wp[row].y = parent_wp->y + p[row].y;

            std::cout << it.entity(row).name() << " transformed to {.x = "
                << wp[row].x << ", .y = "
                << wp[row].y << "} <<child>>" << std::endl;
        }
    }
}

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world ecs(argc, argv);

    /* Move entities with Position and Velocity */
    ecs.system<Position, const Velocity>().each(Move);

    /* Transform local coordinates to world coordinates. A CASCADE column
     * guarantees that entities are evaluated breadth-first, according to the
     * hierarchy. This system will depth-sort based on parents that have the
     * WorldPosition component. 
     *
     * Note that columns that have modifiers (like CASCADE) cannot be provided
     * as a template parameter, and have to be specified using hte 'signature'
     * method. This string will be appended to the signature, so that the full
     * signature for this system will be:
     *     WorldPosition, Position. CASCADE:WorldPosition.
     *
     * Additionally, note that we're using 'action' instead of 'each'. Actions
     * provide more flexibility and performance, but also are slightly more
     * complex to implement.
     *
     * In this case we need to use 'action', since otherwise we cannot access
     * the WorldPosition component from the parent.
     */
    ecs.system<WorldPosition, Position>()
        .term<WorldPosition>()
            .superset(flecs::ChildOf, flecs::Cascade)
            .oper(flecs::Optional)
        .iter(Transform);

    // Create entity hierarchy
    ecs.entity("Root")
        .add<WorldPosition>()
        .set<Position>({0, 0})
        .set<Velocity>({1, 2})
        .scope([&]{ // From here entities are created with (ChildOf, Root)
            ecs.entity("Child1")
                .add<WorldPosition>()
                .set<Position>({100, 100})
                .scope([&]{ // (ChildOf, Root::Child1)
                    ecs.entity("GChild1")
                        .add<WorldPosition>()
                        .set<Position>({1000, 1000});
                });

            ecs.entity("Child2")
                .add<WorldPosition>()
                .set<Position>({200, 200})
                .scope([&]{ // (ChildOf, Root::Child2)
                    ecs.entity("GChild2")
                        .add<WorldPosition>()
                        .set<Position>({2000, 2000});
                });
        });

    ecs.set_target_fps(1);

    std::cout 
        << "Application move_system is running, press CTRL-C to exit..." 
        << std::endl;

    /* Run systems */
    while ( ecs.progress()) {
        std::cout << "----" << std::endl;
    }
}
