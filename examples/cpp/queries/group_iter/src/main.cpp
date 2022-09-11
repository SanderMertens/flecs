#include <group_iter.h>
#include <iostream>

// A group iterator iterates over a single group of a grouped query (see the 
// group_by example for more details). This can be useful when an application
// may need to match different entities based on the context of the game, such 
// as editor mode, day/night, inside/outside or location in the world.
//
// One example is that of an open game which is divided up into world 
// cells. Even though a world may contain many entities, only the entities in 
// cells close to the player need to be processed.
//
// Instead of creating a cached query per world cell, which could be expensive
// as there are more caches to keep in sync, applications can create a single
// query grouped by world cell, and use group iterators to only iterate the
// necessary cells.

// A world cell relationship with four cells
struct WorldCell {};
struct Cell_0_0 {};
struct Cell_0_1 {};
struct Cell_1_0 {};
struct Cell_1_1 {};

// Npc tags
struct Npc {};
struct Merchant {};
struct Soldier {};
struct Beggar {};
struct Mage {};

uint64_t group_by_relation(flecs::world_t *ecs, flecs::table_t *table, 
    flecs::entity_t id, void *) 
{
    // Use ecs_search to find the target for the relationship in the table
    flecs::id_t match;
    if (ecs_search(ecs, table, flecs::id(id, flecs::Wildcard), &match) != -1) {
        return flecs::id(ecs, match).second(); // World cell is 2nd element of pair
    }
    return 0;
}

int main() {
    flecs::world ecs;

    // Create npc's in world cell 0_0
    ecs.entity().add<WorldCell, Cell_0_0>()
        .add<Merchant>()
        .add<Npc>();
    ecs.entity().add<WorldCell, Cell_0_0>()
        .add<Merchant>()
        .add<Npc>();

    // Create npc's in world cell 0_1
    ecs.entity().add<WorldCell, Cell_0_1>()
        .add<Beggar>()
        .add<Npc>();
    ecs.entity().add<WorldCell, Cell_0_1>()
        .add<Soldier>()
        .add<Npc>();

    // Create npc's in world cell 1_0
    ecs.entity().add<WorldCell, Cell_1_0>()
        .add<Mage>()
        .add<Npc>();
    ecs.entity().add<WorldCell, Cell_1_0>()
        .add<Beggar>()
        .add<Npc>();

    // Create npc's in world cell 1_1
    ecs.entity().add<WorldCell, Cell_1_1>()
        .add<Soldier>()
        .add<Npc>();

    auto q = ecs.query_builder<Npc>()
        .group_by<WorldCell>(group_by_relation)
        .build();

    // Iterate all tables
    std::cout << "All tables:\n";
    q.iter([&](flecs::iter& it) {
        auto group = ecs.entity(it.group_id());
        std::cout << " - group " << group.path() << ": table ["
            << it.table().str() << "]\n";
    });
    
    std::cout << "\n";

    // Only iterate entities in cell 1_0
    std::cout << "Tables for cell 1_0:\n";
    q.iter().set_group<Cell_1_0>().iter([&](flecs::iter& it) {
        auto group = ecs.entity(it.group_id());
        std::cout << " - group " << group.path() << ": table ["
            << it.table().str() << "]\n";
    });
    
    // Output:
    //  All tables:
    //   - group ::Cell_0_0: table [Merchant, Npc, (WorldCell,Cell_0_0)]
    //   - group ::Cell_0_1: table [Npc, Beggar, (WorldCell,Cell_0_1)]
    //   - group ::Cell_0_1: table [Npc, Soldier, (WorldCell,Cell_0_1)]
    //   - group ::Cell_1_0: table [Npc, Mage, (WorldCell,Cell_1_0)]
    //   - group ::Cell_1_0: table [Npc, Beggar, (WorldCell,Cell_1_0)]
    //   - group ::Cell_1_1: table [Npc, Soldier, (WorldCell,Cell_1_1)]
    //  
    //  Tables for cell 1_0:
    //   - group ::Cell_1_0: table [Npc, Mage, (WorldCell,Cell_1_0)]
    //   - group ::Cell_1_0: table [Npc, Beggar, (WorldCell,Cell_1_0)]
}
