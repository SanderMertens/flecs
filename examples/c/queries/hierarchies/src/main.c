#include <hierarchies.h>

typedef struct {
    double x;
    double y;
} Position;

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);

    // Tags for local/world position
    ECS_TAG(world, Local);
    ECS_TAG(world, World);

    // Create a hierarchy. For an explanation see the entities/hierarchy example
    ecs_entity_t sun = ecs_new_entity(world, "Sun");
    ecs_add_pair(world, sun, ecs_id(Position), World);
    ecs_set_pair(world, sun, Position, Local, {1, 1});

        ecs_entity_t mercury = ecs_new_entity(world, "Mercury");
        ecs_add_pair(world, mercury, EcsChildOf, sun);
        ecs_add_pair(world, mercury, ecs_id(Position), World);
        ecs_set_pair(world, mercury, Position, Local, {1, 1});

        ecs_entity_t venus = ecs_new_entity(world, "Venus");
        ecs_add_pair(world, venus, EcsChildOf, sun);
        ecs_add_pair(world, venus, ecs_id(Position), World);
        ecs_set_pair(world, venus, Position, Local, {2, 2});

        ecs_entity_t earth = ecs_new_entity(world, "Earth");
        ecs_add_pair(world, earth, EcsChildOf, sun);
        ecs_add_pair(world, earth, ecs_id(Position), World);
        ecs_set_pair(world, earth, Position, Local, {3, 3});

            ecs_entity_t moon = ecs_new_entity(world, "Moon");
            ecs_add_pair(world, moon, EcsChildOf, earth);
            ecs_add_pair(world, moon, ecs_id(Position), World);
            ecs_set_pair(world, moon, Position, Local, {0.1, 0.1});

    // Create a hierarchical query to compute the global position from the
    // local position and the parent position.
    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t) {
        .filter.terms = {
            // Read from entity's Local position
            { .id = ecs_pair(ecs_id(Position), Local), .inout = EcsIn }, 
            // Write to entity's World position
            { .id = ecs_pair(ecs_id(Position), World), .inout = EcsOut },

            // Read from parent's World position
            {
                .id = ecs_pair(ecs_id(Position), World), 
                .inout = EcsIn,
                // Get from the parent, in breadth-first order (cascade)
                .args[0].set.mask = EcsParent | EcsCascade,
                // Make parent term optional so we also match the root (sun)
                .oper = EcsOptional
            }
        }
    });

    // Do the transform
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        const Position *p = ecs_term(&it, Position, 1);
        Position *p_out = ecs_term(&it, Position, 2);
        const Position *p_parent = ecs_term(&it, Position, 3);
        
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

    // Print world positions
    it = ecs_term_iter(world, &(ecs_term_t) {
        .id = ecs_pair(ecs_id(Position), World)
    });

    while (ecs_term_next(&it)) {
        Position *p = ecs_term(&it, Position, 1);
        for (int i = 0; i < it.count; i ++) {
            printf("%s: {%f, %f}\n", ecs_get_name(world, it.entities[i]),
                p[i].x, p[i].y);
        }
    }

    return ecs_fini(world);
}
