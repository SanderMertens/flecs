#include <change_tracking.h>
#include <iostream>

// Queries have a builtin mechanism for tracking changes per matched table. This
// is a cheap way of eliminating redundant work, as many entities can be skipped
// with a single check. 
// 
// This example shows how to use change tracking in combination with a few other
// techniques, like using prefabs to store a single dirty state for multiple
// entities and instanced queries.

struct Dirty {
    bool value;
};

struct Position {
    double x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    // Create a query that just reads a component. We'll use this query for
    // change tracking. Change tracking for a query is automatically enabled
    // when query::changed() is called.
    // Each query has its own private dirty state which is reset only when the
    // query is iterated.
    auto q_read = ecs.query<const Position>();

    // Create a query that writes the component based on a Dirty state.
    auto q_write = ecs.query_builder<const Dirty, Position>()
        .term_at(1).up()     // Only match Dirty from prefab
        .instanced()         // Instanced iteration is faster (see example)
        .build();

    // Create two prefabs with a Dirty component. We can use this to share a
    // single Dirty value for all entities in a table.
    auto p1 = ecs.prefab("p1").set<Dirty>({false});
    auto p2 = ecs.prefab("p2").set<Dirty>({true});

    // Create instances of p1 and p2. Because the entities have different
    // prefabs, they end up in different tables.
    ecs.entity("e1").is_a(p1)
        .set<Position>({10, 20});

    ecs.entity("e2").is_a(p1)
        .set<Position>({30, 40});

    ecs.entity("e3").is_a(p2)
        .set<Position>({40, 50});

    ecs.entity("e4").is_a(p2)
        .set<Position>({60, 70});

    // We can use the changed() function on the query to check if any of the
    // tables it is matched with has changed. Since this is the first time that
    // we check this and the query is matched with the tables we just created,
    // the function will return true.
    std::cout << "q_read.changed(): " << q_read.changed() << "\n";

    // The changed state will remain true until we have iterated each table.
    q_read.iter([](flecs::iter& it) {
        // With the it.changed() function we can check if the table we're
        // currently iterating has changed since last iteration.
        // Because this is the first time the query is iterated, all tables
        // will show up as changed.
        std::cout << "it.changed() for table [" << it.type().str() << "]: "
            << it.changed() << "\n";
    });

    // Now that we have iterated all tables, the dirty state is reset.
    std::cout << "q_read.changed(): " << q_read.changed() << "\n\n";

    // Iterate the write query. Because the Position term is InOut (default)
    // iterating the query will write to the dirty state of iterated tables.
    q_write.iter([](flecs::iter& it, const Dirty *dirty, Position *p) {
        std::cout << "iterate table [" << it.type().str() << "]\n";

        // Because we enforced that Dirty is a shared component, we can check
        // a single value for the entire table.
        if (!dirty->value) {
            // If the dirty flag is false, skip the table. This way the table's
            // dirty state is not updated by the query.
            it.skip();
            std::cout << "it.skip() for table [" << it.type().str() << "]\n";
            return;
        }

        // For all other tables the dirty state will be set.
        for (auto i : it) {
            p[i].x ++;
            p[i].y ++;
        }
    });

    // One of the tables has changed, so q_read.changed() will return true
    std::cout << "\nq_read.changed(): " << q_read.changed() << "\n";

    // When we iterate the read query, we'll see that one table has changed.
    q_read.iter([](flecs::iter& it) {
        std::cout << "it.changed() for table [" << it.type().str() << "]: "
            << it.changed() << "\n";
    }); 

    // Output:
    //  q_read.changed(): 1
    //  it.changed() for table [Position, (Identifier,Name), (IsA,p1)]: 1
    //  it.changed() for table [Position, (Identifier,Name), (IsA,p2)]: 1
    //  q_read.changed(): 0
    //  
    //  iterate table [Position, (Identifier,Name), (IsA,p1)]
    //  it.skip() for table [Position, (Identifier,Name), (IsA,p1)]
    //  iterate table [Position, (Identifier,Name), (IsA,p2)]
    //  
    //  q_read.changed(): 1
    //  it.changed() for table [Position, (Identifier,Name), (IsA,p1)]: 0
    //  it.changed() for table [Position, (Identifier,Name), (IsA,p2)]: 1
}
