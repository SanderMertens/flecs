#include <group_iter.h>
#include <stdio.h>

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

uint64_t group_by_relation(ecs_world_t *ecs, ecs_table_t *table, 
    ecs_entity_t id, void *ctx) 
{
    (void)ctx; // marks ctx as unused, avoids compiler warnings

    // Use ecs_search to find the target for the relationship in the table
    ecs_id_t match;
    if (ecs_search(ecs, table, ecs_pair(id, EcsWildcard), &match) != -1) {
        return ecs_pair_second(ecs, match); // World cell is 2nd element of pair
    }

    return 0;
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // A world cell relationship with four cells
    ECS_TAG(ecs, WorldCell);
    ECS_TAG(ecs, Cell_0_0);
    ECS_TAG(ecs, Cell_0_1);
    ECS_TAG(ecs, Cell_1_0);
    ECS_TAG(ecs, Cell_1_1);

    // Npc tags
    ECS_TAG(ecs, Npc);
    ECS_TAG(ecs, Merchant);
    ECS_TAG(ecs, Soldier);
    ECS_TAG(ecs, Beggar);
    ECS_TAG(ecs, Mage);

    // Create Npcs in world cell 0_0
    ecs_entity_t npc_01 = ecs_new_w_pair(ecs, WorldCell, Cell_0_0);
    ecs_add(ecs, npc_01, Merchant);
    ecs_add(ecs, npc_01, Npc);
    ecs_entity_t npc_02 = ecs_new_w_pair(ecs, WorldCell, Cell_0_0);
    ecs_add(ecs, npc_02, Merchant);
    ecs_add(ecs, npc_02, Npc);

    // Create Npcs in world cell 0_1
    ecs_entity_t npc_03 = ecs_new_w_pair(ecs, WorldCell, Cell_0_1);
    ecs_add(ecs, npc_03, Beggar);
    ecs_add(ecs, npc_03, Npc);
    ecs_entity_t npc_04 = ecs_new_w_pair(ecs, WorldCell, Cell_0_1);
    ecs_add(ecs, npc_04, Soldier);
    ecs_add(ecs, npc_04, Npc);

    // Create Npcs in world cell 1_0
    ecs_entity_t npc_05 = ecs_new_w_pair(ecs, WorldCell, Cell_1_0);
    ecs_add(ecs, npc_05, Mage);
    ecs_add(ecs, npc_05, Npc);
    ecs_entity_t npc_06 = ecs_new_w_pair(ecs, WorldCell, Cell_1_0);
    ecs_add(ecs, npc_06, Beggar);
    ecs_add(ecs, npc_06, Npc);

    // Create Npcs in world cell 1_1
    ecs_entity_t npc_07 = ecs_new_w_pair(ecs, WorldCell, Cell_1_1);
    ecs_add(ecs, npc_07, Soldier);
    ecs_add(ecs, npc_07, Npc);

    // Create a query that matches all Npc's, grouped by WorldCell
    ecs_query_t *q = ecs_query(ecs, {
        .filter.terms = {
            { .id = Npc }
        },
        .group_by = group_by_relation,
        .group_by_id = WorldCell
    });

    // Iterate all tables
    printf("All tables:\n");
    ecs_iter_t it = ecs_query_iter(ecs, q);
    while (ecs_query_next(&it)) {
        char *table_str = ecs_table_str(ecs, it.table);
        char *group_str = ecs_get_fullpath(ecs, it.group_id);

        printf(" - group %s: table [%s]\n", group_str, table_str);

        // Uncomment to iterate entities in table
        // for (int i = 0; i < it.count; i ++) { }

        ecs_os_free(group_str);
        ecs_os_free(table_str);
    }
    printf("\n");

    // Only iterate entities in cell 1_0
    printf("Tables for cell 1_0:\n");
    it = ecs_query_iter(ecs, q);
    ecs_query_set_group(&it, Cell_1_0);
    while (ecs_query_next(&it)) {
        char *table_str = ecs_table_str(ecs, it.table);
        char *group_str = ecs_get_fullpath(ecs, it.group_id);

        printf(" - group %s: table [%s]\n", group_str, table_str);

        ecs_os_free(group_str);
        ecs_os_free(table_str);
    }

    // Output:
    //  All tables:
    //   - group Cell_0_0: table [Npc, Merchant, (WorldCell,Cell_0_0)]
    //   - group Cell_0_1: table [Npc, Beggar, (WorldCell,Cell_0_1)]
    //   - group Cell_0_1: table [Npc, Soldier, (WorldCell,Cell_0_1)]
    //   - group Cell_1_0: table [Npc, Mage, (WorldCell,Cell_1_0)]
    //   - group Cell_1_0: table [Npc, Beggar, (WorldCell,Cell_1_0)]
    //   - group Cell_1_1: table [Npc, Soldier, (WorldCell,Cell_1_1)]
    //  
    //  Tables for cell 1_0:
    //   - group Cell_1_0: table [Npc, Mage, (WorldCell,Cell_1_0)]
    //   - group Cell_1_0: table [Npc, Beggar, (WorldCell,Cell_1_0)]

    return ecs_fini(ecs);
}
