#include <group_iter_all.h>
#include <stdio.h>

// This example demonstrates how to iterate over all active groups of a
// grouped query using ecs_query_get_groups(), and then iterate the entities
// within each group with ecs_iter_set_group(). This is useful when an
// application needs to do per-group logic before/after processing the entities
// in that group.

typedef struct {
    double x, y;
} Position;

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, Position);

    ecs_entity_t Asset_A = ecs_entity(ecs, { .name = "Asset_A" });
    ecs_entity_t Asset_B = ecs_entity(ecs, { .name = "Asset_B" });
    ecs_entity_t Asset_C = ecs_entity(ecs, { .name = "Asset_C" });

    ecs_set(ecs, ecs_new_w_pair(ecs, EcsIsA, Asset_A), Position, {1, 1});
    ecs_set(ecs, ecs_new_w_pair(ecs, EcsIsA, Asset_A), Position, {2, 2});
    ecs_set(ecs, ecs_new_w_pair(ecs, EcsIsA, Asset_B), Position, {3, 3});
    ecs_set(ecs, ecs_new_w_pair(ecs, EcsIsA, Asset_C), Position, {4, 4});
    ecs_set(ecs, ecs_new_w_pair(ecs, EcsIsA, Asset_C), Position, {5, 5});
    ecs_set(ecs, ecs_new_w_pair(ecs, EcsIsA, Asset_C), Position, {6, 6});

    // Group entities by their IsA target. With no group_by_callback set,
    // the default callback uses the relationship's target as the group ID.
    ecs_query_t *q = ecs_query(ecs, {
        .terms = {{ .id = ecs_id(Position) }},
        .group_by = EcsIsA
    });

    // Iterate all active groups via the query's group map. The map key is
    // the group ID, which here is the asset entity (the target of IsA).
    const ecs_map_t *groups = ecs_query_get_groups(q);
    ecs_map_iter_t mit = ecs_map_iter(groups);
    while (ecs_map_next(&mit)) {
        uint64_t group_id = ecs_map_key(&mit);
        char *group_str = ecs_get_path(ecs, group_id);
        printf("Group %s:\n", group_str);
        ecs_os_free(group_str);

        // Iterate just the entities in this group.
        ecs_iter_t it = ecs_query_iter(ecs, q);
        ecs_iter_set_group(&it, group_id);
        while (ecs_query_next(&it)) {
            Position *p = ecs_field(&it, Position, 0);
            for (int i = 0; i < it.count; i ++) {
                char *e_str = ecs_get_path(ecs, it.entities[i]);
                printf(" - %s: {%.0f, %.0f}\n", e_str, p[i].x, p[i].y);
                ecs_os_free(e_str);
            }
        }
    }

    ecs_query_fini(q);

    // Output (group iteration order is unspecified):
    //  Group Asset_B:
    //   - #475: {3, 3}
    //  Group Asset_A:
    //   - #473: {1, 1}
    //   - #474: {2, 2}
    //  Group Asset_C:
    //   - #476: {4, 4}
    //   - #477: {5, 5}
    //   - #478: {6, 6}

    return ecs_fini(ecs);
}
