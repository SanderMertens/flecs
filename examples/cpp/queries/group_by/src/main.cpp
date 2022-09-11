#include <group_by.h>
#include <iostream>

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
// set_group example for more details.

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
