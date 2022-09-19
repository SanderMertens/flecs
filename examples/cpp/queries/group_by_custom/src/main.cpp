#include <group_by_custom.h>
#include <iostream>

// This example does the same as the group_by example, but with a custom
// group_by function. A custom function makes it possible to customize how a
// group id is calculated for a table.

struct Position {
    double x, y;
};

// Dummy tag to put entities in different tables
struct Tag { };

// Create a relationship to use for for the group_by function. Tables will
// be assigned the relationship target as group id
struct Group { };

// Targets for the relationship, which will be used as group ids.
struct First { };
struct Second { };
struct Third { };

uint64_t group_by_relation(flecs::world_t *ecs, flecs::table_t *table, 
    flecs::entity_t id, void *) 
{
    // Use ecs_search to find the target for the relationship in the table
    flecs::id_t match;
    if (ecs_search(ecs, table, flecs::id(id, flecs::Wildcard), &match) != -1) {
        return flecs::id(ecs, match).second(); // First, Second or Third
    }
    return 0;
}

int main() {
    flecs::world ecs;

    // Register components in order so that id for First is lower than Third
    ecs.component<First>();
    ecs.component<Second>();
    ecs.component<Third>();

    // Grouped query
    auto q = ecs.query_builder<Position>()
        .group_by<Group>(group_by_relation)
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

    q.iter([&](flecs::iter& it, Position *p) {
        auto group = ecs.entity(it.group_id());
        std::cout << " - group " << group.path() << ": table ["
            << it.table().str() << "]\n";

        for (auto i : it) {
            std::cout << "     {" << p[i].x << ", " << p[i].y << "}\n";
        }

        std::cout << "\n";
    });

    // Output:
    //  - group ::First: table [Position, (Group,First)]
    //      {3, 3}
    // 
    //  - group ::First: table [Position, Tag, (Group,First)]
    //      {6, 6}
    // 
    //  - group ::Second: table [Position, (Group,Second)]
    //      {2, 2}
    // 
    //  - group ::Second: table [Position, Tag, (Group,Second)]
    //      {5, 5}
    // 
    //  - group ::Third: table [Position, (Group,Third)]
    //      {1, 1}
    // 
    //  - group ::Third: table [Position, Tag, (Group,Third)]
    //      {4, 4}
}
