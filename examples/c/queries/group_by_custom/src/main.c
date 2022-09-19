#include <group_by_custom.h>
#include <stdio.h>

// This example does the same as the group_by example, but with a custom
// group_by function. A custom function makes it possible to customize how a
// group id is calculated for a table.

typedef struct {
    double x, y;
} Position;

uint64_t group_by_relation(ecs_world_t *ecs, ecs_table_t *table, 
    ecs_entity_t id, void *ctx) 
{
    (void)ctx; // marks ctx as unused, avoids compiler warnings

    // Use ecs_search to find the target for the relationship in the table
    ecs_id_t match;
    if (ecs_search(ecs, table, ecs_pair(id, EcsWildcard), &match) != -1) {
        return ecs_pair_second(ecs, match); // First, Second or Third
    }

    return 0;
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, Position);
    ECS_TAG(ecs, Tag); // Dummy tag to put entities in different tables

    // Create a relationship to use for for the group_by function. Tables will
    // be assigned the relationship target as group id
    ECS_TAG(ecs, Group);

    // Targets for the relationship, which will be used as group ids.
    ECS_TAG(ecs, First);
    ECS_TAG(ecs, Second);
    ECS_TAG(ecs, Third);

    // Grouped query
    ecs_query_t *q = ecs_query(ecs, {
        .filter.terms = {
            { .id = ecs_id(Position) }
        },
        .group_by = group_by_relation,
        .group_by_id = Group // Passed to id argument of group_by function
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
    //     - table [Postion, Tag, (Group, First)]
    //
    //  - group Second:
    //     - table [Position, (Group, Second)]
    //     - table [Postion, Tag, (Group, Second)]
    //  
    //  - group Third:
    //     - table [Position, (Group, Third)]
    //     - table [Postion, Tag, (Group, Third)]
    //

    // Iterate query, print position & table components
    ecs_iter_t it = ecs_query_iter(ecs, q);
    while (ecs_query_next(&it)) {
        Position *p = ecs_field(&it, Position, 1);
        char *table_str = ecs_table_str(ecs, it.table);
        char *group_str = ecs_get_fullpath(ecs, it.group_id);

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
