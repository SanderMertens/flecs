#include <hierarchies.h>
#include <stdio.h>

// Simple transform location with 2D location
typedef struct {
    double x;
    double y;
} Transform;

// Create two typedefs types for world and local transform
typedef Transform WorldTransform;
typedef Transform LocalTransform;

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, LocalTransform);
    ECS_COMPONENT(ecs, WorldTransform);

    // Whenever we add LocalTransform, also add WorldTransform
    ecs_add_pair(ecs, ecs_id(LocalTransform), EcsWith, ecs_id(WorldTransform));

    // Create a hierarchy. For an explanation see the entities/hierarchy example
    ecs_entity_t sun = ecs_entity(ecs, { .name = "Sun" });
    ecs_set(ecs, sun, LocalTransform, {1, 1});

        ecs_entity_t mercury = ecs_entity(ecs, { .name = "Mercury", .parent = sun });
        ecs_set(ecs, mercury, LocalTransform, {1, 1});

        ecs_entity_t venus = ecs_entity(ecs, { .name = "Venus", .parent = sun });
        ecs_set(ecs, venus, LocalTransform, {2, 2});

        ecs_entity_t earth = ecs_entity(ecs, { .name = "Earth", .parent = sun });
        ecs_set(ecs, earth, LocalTransform, {3, 3});

            ecs_entity_t moon = ecs_entity(ecs, { .name = "Moon", .parent = earth });
            ecs_set(ecs, moon, LocalTransform, {0.1, 0.1});

    // Create a hierarchical query to compute the global transform from the
    // local transform and the parent transform.
    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            // Read from entity's Local transform
            { .id = ecs_id(LocalTransform), .inout = EcsIn }, 
            // Write to entity's World transform
            { .id = ecs_id(WorldTransform), .inout = EcsOut },

            // Read from parent's World transform
            {
                .id = ecs_id(WorldTransform), 
                .inout = EcsIn,
                // Get from the parent in breadth-first order (cascade)
                .src.id = EcsCascade,
                // Make parent term optional so we also match the root (sun)
                .oper = EcsOptional
            }
        }
    });

    // Do the transform
    ecs_iter_t it = ecs_query_iter(ecs, q);
    while (ecs_query_next(&it)) {
        const LocalTransform *p = ecs_field(&it, LocalTransform, 0);
        WorldTransform *p_out = ecs_field(&it, WorldTransform, 1);
        const WorldTransform *p_parent = ecs_field(&it, WorldTransform, 2);
        
        // Inner loop, iterates entities in archetype
        for (int i = 0; i < it.count; i ++) {
            p_out[i].x = p[i].x;
            p_out[i].y = p[i].y;
            if (p_parent) {
                p_out[i].x += p_parent->x;
                p_out[i].y += p_parent->y;
            }
        }
    }

    // Print ecs transforms
    it = ecs_each(ecs, WorldTransform);
    while (ecs_each_next(&it)) {
        WorldTransform *p = ecs_field(&it, WorldTransform, 0);
        for (int i = 0; i < it.count; i ++) {
            printf("%s: {%f, %f}\n", ecs_get_name(ecs, it.entities[i]),
                p[i].x, p[i].y);
        }
    }

    return ecs_fini(ecs);
}
