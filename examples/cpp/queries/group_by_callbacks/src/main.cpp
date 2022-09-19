#include <group_by_callbacks.h>
#include <iostream>

// This example shows how the on_group_create and on_group_delete callbacks can
// be used to get notified when a new group is registered for a query. These
// callbacks make it possible to associate and manage user data attached to
// groups.

struct Position {
    double x, y;
};

// Custom type to associate with group
struct group_ctx {
    int32_t counter;
};

int group_counter = 0;

// Dummy tag to put entities in different tables
struct Tag { };

// Create a relationship to use for for the group_by function. Tables will
// be assigned the relationship target as group id
struct Group { };

// Targets for the relationship, which will be used as group ids.
struct First { };
struct Second { };
struct Third { };

int main() {
    flecs::world ecs;

    // Register components in order so that id for First is lower than Third
    ecs.component<First>();
    ecs.component<Second>();
    ecs.component<Third>();

    // Grouped query
    auto q = ecs.query_builder<Position>()
        .group_by<Group>()

        // Callback invoked when a new group is created
        .on_group_create([](flecs::world_t *world, 
            uint64_t id,         // id of the group that was created
            void *group_by_arg)  // group_by_ctx parameter in ecs_query_desc_t struct
        {
            (void)group_by_arg; // silence unused warning
            flecs::world w(world);
            std::cout << "Group " << w.entity(id).name() << " created\n";

            // Return data that will be associated with the group
            group_ctx *ctx = new group_ctx;
            ctx->counter = ++ group_counter;
            return static_cast<void*>(ctx); // Cast to make sure function type matches
        })

        // Callback invoked when a group is deleted
        .on_group_delete([](flecs::world_t *world, 
            uint64_t id,         // id of the group that was deleted
            void *ctx,           // group context
            void *group_by_arg)  // group_by_ctx parameter in ecs_query_desc_t struct
        {
            (void)group_by_arg; // silence unused warning
            flecs::world w(world);
            std::cout << "Group " << w.entity(id).name() << " deleted\n";

            // Free data associated with group
            delete static_cast<group_ctx*>(ctx);
        })
        .build();

    // Create entities in 6 different tables with 3 group ids
    ecs.entity().add<Group, Third>()
        .set<Position>({1, 1});
    ecs.entity().add<Group, Second>()
        .set<Position>({2, 2});
    ecs.entity().add<Group, First>()
        .set<Position>({3, 3});

    ecs.entity().add<Group, Third>()
        .set<Position>({4, 4})
        .add<Tag>();
    ecs.entity().add<Group, Second>()
        .set<Position>({5, 5})
        .add<Tag>();
    ecs.entity().add<Group, First>()
        .set<Position>({6, 6})
        .add<Tag>();

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

    std::cout << "\n";

    q.iter([&](flecs::iter& it, Position *p) {
        auto group = ecs.entity(it.group_id());
        group_ctx *ctx = static_cast<group_ctx*>(q.group_ctx(group));
        std::cout << " - group " << group.path() << ": table ["
            << it.table().str() << "]\n";
        std::cout << "   counter: " << ctx->counter << "\n";

        for (auto i : it) {
            std::cout << "     {" << p[i].x << ", " << p[i].y << "}\n";
        }

        std::cout << "\n";
    });

    // Deleting the query will call the on_group_deleted callback
    q.destruct();

    // Output:
    //  Group Third created
    //  Group Second created
    //  Group First created
    //  
    //   - group ::First: table [Position, (Group,First)]
    //     counter: 3
    //       {3, 3}
    //  
    //   - group ::First: table [Position, Tag, (Group,First)]
    //     counter: 3
    //       {6, 6}
    //  
    //   - group ::Second: table [Position, (Group,Second)]
    //     counter: 2
    //       {2, 2}
    //  
    //   - group ::Second: table [Position, Tag, (Group,Second)]
    //     counter: 2
    //       {5, 5}
    //  
    //   - group ::Third: table [Position, (Group,Third)]
    //     counter: 1
    //       {1, 1}
    //  
    //   - group ::Third: table [Position, Tag, (Group,Third)]
    //     counter: 1
    //       {4, 4}
    //  
    //  Group Third deleted
    //  Group Second deleted
    //  Group First deleted
}
