#include <group_iter_all.h>
#include <iostream>

// This example demonstrates how to iterate over all active groups of a
// grouped query using query::groups(), and then iterate the entities
// within each group with set_group(). This is useful when an
// application needs to do per-group logic before/after processing the entities
// in that group.

struct Position { double x, y; };

int main() {
    flecs::world ecs;

    auto Asset_A = ecs.entity("Asset_A");
    auto Asset_B = ecs.entity("Asset_B");
    auto Asset_C = ecs.entity("Asset_C");

    ecs.entity().is_a(Asset_A).set<Position>({1, 1});
    ecs.entity().is_a(Asset_A).set<Position>({2, 2});
    ecs.entity().is_a(Asset_B).set<Position>({3, 3});
    ecs.entity().is_a(Asset_C).set<Position>({4, 4});
    ecs.entity().is_a(Asset_C).set<Position>({5, 5});
    ecs.entity().is_a(Asset_C).set<Position>({6, 6});

    // Group entities by their IsA target. The default group_by callback uses
    // the relationship's target as the group ID.
    flecs::query<Position> q = ecs.query_builder<Position>()
        .group_by(flecs::IsA)
        .build();

    // Iterate all active groups, then iterate entities for each group with
    // set_group(). The map key (g.first) is the group ID, which here is the
    // asset entity (the target of IsA).
    for (auto g : q.groups()) {
        flecs::entity asset = ecs.entity(g.first);
        std::cout << "Group " << asset.path() << ":\n";

        q.set_group(g.first).each([&](flecs::entity e, Position& p) {
            std::cout << " - " << e.path() << ": {" << p.x << ", " << p.y << "}\n";
        });
    }

    // Output (group iteration order is unspecified):
    //  Group ::Asset_B:
    //   - #501: {3, 3}
    //  Group ::Asset_A:
    //   - #499: {1, 1}
    //   - #500: {2, 2}
    //  Group ::Asset_C:
    //   - #502: {4, 4}
    //   - #503: {5, 5}
    //   - #504: {6, 6}
}
