#include <group_by.h>
#include <stdio.h>

// Group by is a feature of cached queries that allows applications to assign a
// group id to each matched table. Tables that are assigned the same group id
// are stored together in "groups". This ensures that when a query is iterated,
// tables that share a group are iterated together.
//
// Groups in the cache are ordered by group id, which ensures that tables with
// lower ids are iterated before table with higher ids. This is the same 
// mechanism that is used by the cascade feature, which groups tables by depth
// in a relationship hierarchy.
//
// This makes groups a more efficient, though less granular mechanism for 
// ordering entities. Order is maintained at the group level, which means that
// once a group is created, tables can get added and removed to the group
// with is an O(1) operation.
//
// Groups can also be used as an efficient filtering mechanism. See the 
// group_iter example for more details.

typedef struct {
    double x, y;
} Position;

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, Position);
    ECS_TAG(ecs, Tag); // Dummy tag to put entities in different tables

    // Create a relationship to use for the group_by function. Tables will
    // be assigned the relationship target as group id
    ECS_TAG(ecs, Group);

    // Targets for the relationship, which will be used as group ids.
    ECS_TAG(ecs, First);
    ECS_TAG(ecs, Second);
    ECS_TAG(ecs, Third);

    // Grouped query
    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { .id = ecs_id(Position) }
        },
        // Group tables by the relationship target of the "Group" relationship.
        // A custom group_by function can be provided to derive a group id from
        // a table, see the group_by_custom example for more details.
        .group_by = Group
    });

    // Create entities in 6 different tables with 3 group ids
    ecs_entity_t e1 = ecs_new_w_pair(ecs, Group, Third);
    ecs_set(ecs, e1, Position, {1, 1});

    ecs_entity_t e2 = ecs_new_w_pair(ecs, Group, Second);
    ecs_set(ecs, e2, Position, {2, 2});

    ecs_entity_t e3 = ecs_new_w_pair(ecs, Group, First);
    ecs_set(ecs, e3, Position, {3, 3});

    ecs_entity_t e4 = ecs_new_w_pair(ecs, Group, Third);
    ecs_set(ecs, e4, Position, {4, 4});
    ecs_add(ecs, e4, Tag);

    ecs_entity_t e5 = ecs_new_w_pair(ecs, Group, Second);
    ecs_set(ecs, e5, Position, {5, 5});
    ecs_add(ecs, e5, Tag);

    ecs_entity_t e6 = ecs_new_w_pair(ecs, Group, First);
    ecs_set(ecs, e6, Position, {6, 6});
    ecs_add(ecs, e6, Tag);

    // The query cache now looks like this:
    //  - group First:
    //     - table [Position, (Group, First)]
    //     - table [Position, Tag, (Group, First)]
    //
    //  - group Second:
    //     - table [Position, (Group, Second)]
    //     - table [Position, Tag, (Group, Second)]
    //  
    //  - group Third:
    //     - table [Position, (Group, Third)]
    //     - table [Position, Tag, (Group, Third)]
    //

    // Iterate query, print position & table components
    ecs_iter_t it = ecs_query_iter(ecs, q);
    while (ecs_query_next(&it)) {
        Position *p = ecs_field(&it, Position, 0);
        char *table_str = ecs_table_str(ecs, it.table);
        char *group_str = ecs_get_path(ecs, it.group_id);

        printf(" - group %s: table [%s]\n", group_str, table_str);

        for (int i = 0; i < it.count; i ++) {
            printf("     {%f, %f}\n", p[i].x, p[i].y);
        }

        ecs_os_free(group_str);
        ecs_os_free(table_str);
        printf("\n");
    }

    // Output:
    //  - group First: table [Position, (Group,First)]
    //      {3.000000, 3.000000}
    // 
    //  - group First: table [Position, Tag, (Group,First)]
    //      {6.000000, 6.000000}
    // 
    //  - group Second: table [Position, (Group,Second)]
    //      {2.000000, 2.000000}
    // 
    //  - group Second: table [Position, Tag, (Group,Second)]
    //      {5.000000, 5.000000}
    // 
    //  - group Third: table [Position, (Group,Third)]
    //      {1.000000, 1.000000}
    // 
    //  - group Third: table [Position, Tag, (Group,Third)]
    //      {4.000000, 4.000000}

    return ecs_fini(ecs);
}
