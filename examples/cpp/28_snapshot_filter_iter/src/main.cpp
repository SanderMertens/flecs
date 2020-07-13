#include <snapshot_filter_iter.h>
#include <iostream>

/* Component types */
struct Position {
    float x;
    float y;
};

struct Velocity {
    float x;
    float y;
};

struct Mass {
    float value;
};

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world world(argc, argv);

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");
    flecs::component<Mass>(world, "Mass");

    flecs::entity(world, "E1")
        .set<Position>({10, 20})
        .set<Velocity>({1, 1});

    flecs::entity(world, "E2")
        .set<Position>({30, 40})
        .set<Velocity>({1, 1})
        .set<Mass>({1});

    /* Take a snapshot of the current state */
    flecs::snapshot s(world);
    s.take();

    /* Create filter that matches all entities with [Position, Velocity] */
    auto f = flecs::filter(world)
        .include<Position>()
        .include<Velocity>()
        .include_kind(flecs::MatchAll);    

    /* Delete matching entities from world to prove we're using snapshot data */
    world.delete_entities(f);

    for (auto it : s.filter(f)) {
        /* Get the Position and Velocity columns from the current table */
        auto p = it.table_column<Position>();
        auto v = it.table_column<Velocity>();

        /* We can't use the name() method anymore to fetch the entity name as
         * we have removed the entities from the world. Instead, we must get the
         * name directly from the table in the snapshot */
        auto name = it.table_column<flecs::Name>();

        for (auto row : it) {
            p[row].x += v[row].x;
            p[row].y += v[row].y;

            std::cout << "Moved " << name[row].value << " to {" <<
                p[row].x << ", " << p[row].y << "}" << std::endl;
        }
    }
}
