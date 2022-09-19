#include <group_by_callbacks.h>
#include <stdio.h>
#include <stdlib.h>

// This example shows how the on_group_create and on_group_delete callbacks can
// be used to get notified when a new group is registered for a query. These
// callbacks make it possible to associate and manage user data attached to
// groups.

typedef struct {
    double x, y;
} Position;

// Custom type to associate with group
typedef struct {
    int32_t counter;
} group_ctx;

int group_counter = 0;

// Callback invoked when a new group is created
static void* on_group_create(ecs_world_t *world, 
    uint64_t group_id,  // id of the group that was created
    void *group_by_arg) // group_by_ctx parameter in ecs_query_desc_t struct
{
    (void)group_by_arg; // silence unused warning
    printf("Group %s created\n", ecs_get_name(world, group_id));

    // Return data that will be associated with the group
    group_ctx *ctx = malloc(sizeof(group_ctx));
    ctx->counter = ++ group_counter;
    return ctx;
}

// Callback invoked when a group is deleted
static void on_group_delete(ecs_world_t *world, 
    uint64_t group_id,  // id of the group that was deleted
    void *ctx,          // group context
    void *group_by_arg) // group_by_ctx parameter in ecs_query_desc_t struct
{
    (void)group_by_arg; // silence unused warning
    printf("Group %s deleted\n", ecs_get_name(world, group_id));

    // Free data associated with group
    free(ctx);
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
        .group_by_id = Group,
        .on_group_create = on_group_create,
        .on_group_delete = on_group_delete
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

    printf("\n");

    // Iterate query, print position & table components
    ecs_iter_t it = ecs_query_iter(ecs, q);
    while (ecs_query_next(&it)) {
        Position *p = ecs_field(&it, Position, 1);
        char *table_str = ecs_table_str(ecs, it.table);
        char *group_str = ecs_get_fullpath(ecs, it.group_id);
        group_ctx *ctx = ecs_query_get_group_ctx(q, it.group_id);

        printf(" - group %s: table [%s]\n", group_str, table_str);
        printf("   counter: %d\n", ctx->counter);

        for (int i = 0; i < it.count; i ++) {
            printf("     {%f, %f}\n", p[i].x, p[i].y);
        }

        ecs_os_free(group_str);
        ecs_os_free(table_str);
        printf("\n");
    }

    // Deleting the query will call the on_group_deleted callback
    ecs_query_fini(q);

    // Output:
    //   Group Third created
    //   Group Second created
    //   Group First created
    //   
    //    - group First: table [Position, (Group,First)]
    //      counter: 3
    //        {3.000000, 3.000000}
    //   
    //    - group First: table [Position, Tag, (Group,First)]
    //      counter: 3
    //        {6.000000, 6.000000}
    //   
    //    - group Second: table [Position, (Group,Second)]
    //      counter: 2
    //        {2.000000, 2.000000}
    //   
    //    - group Second: table [Position, Tag, (Group,Second)]
    //      counter: 2
    //        {5.000000, 5.000000}
    //   
    //    - group Third: table [Position, (Group,Third)]
    //      counter: 1
    //        {1.000000, 1.000000}
    //   
    //    - group Third: table [Position, Tag, (Group,Third)]
    //      counter: 1
    //        {4.000000, 4.000000}
    //   
    //   Group First deleted
    //   Group Third deleted
    //   Group Second deleted

    return ecs_fini(ecs);
}
