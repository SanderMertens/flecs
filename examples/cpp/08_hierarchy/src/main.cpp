#include <hierarchy.h>
#include <iostream>

struct Position {
    float x;
    float y;
};

struct WorldPosition {
    float x;
    float y;
};

struct Velocity {
    float x;
    float y;
};

/* Implement a simple move system */
void Move(flecs::entity e, Position& p, Velocity& v) {
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
void Transform(flecs::iter& it, flecs::column<WorldPosition> wp, flecs::column<Position> p) {
    flecs::column<const WorldPosition> parent_wp(it, 3);

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
    flecs::world world(argc, argv);

    /* Register components */
    flecs::component<Position>(world, "Position");
    flecs::component<WorldPosition>(world, "WorldPosition");
    flecs::component<Velocity>(world, "Velocity");

    /* Move entities with Position and Velocity */
    flecs::system<Position, Velocity>(world).each(Move);

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
    flecs::system<WorldPosition, Position>(world)
        .signature("CASCADE:WorldPosition")
        .action(Transform);

    /* Create root of the hierachy which moves around */
    auto Root = flecs::entity(world, "Root")
        .add<WorldPosition>()
        .set<Position>({0, 0})
        .set<Velocity>({1, 2});

        auto Child1 = flecs::entity(world, "Child1")
            .add_childof(Root)
            .add<WorldPosition>()
            .set<Position>({100, 100});

            flecs::entity(world, "GChild1")
                .add_childof(Child1)
                .add<WorldPosition>()
                .set<Position>({1000, 1000});

        auto Child2 = flecs::entity(world, "Child2")
            .add_childof(Root)
            .add<WorldPosition>()
            .set<Position>({200, 200});

            flecs::entity(world, "GChild2")
                .add_childof(Child2)
                .add<WorldPosition>()
                .set<Position>({2000, 2000});

    world.set_target_fps(1);

    std::cout 
        << "Application move_system is running, press CTRL-C to exit..." 
        << std::endl;

    /* Run systems */
    while ( world.progress()) {
        std::cout << "----" << std::endl;
    }
}
