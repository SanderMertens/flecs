#include <basics.h>
#include <stdio.h>

typedef struct {
    double x, y;
} Position, Velocity;

void Move(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 1);
    const Velocity *v = ecs_field(it, Velocity, 2);

    for (int i = 0; i < it->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
        printf("%s: {%f, %f}\n", ecs_get_name(it->world, it->entities[i]), 
            p[i].x, p[i].y);
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Velocity);

    // Create a system for Position, Velocity.
    ecs_entity_t move = ecs_system(ecs, {
        // Systems are entities, and by initializing the .entity field we can
        // set some additional properties for the system like a name. While this
        // is not mandatory, it makes a system easier to find in tools like the
        // explorer (https://www.flecs.dev/explorer/).
        .entity = ecs_entity(ecs, {
            .name = "Move" 
        }),
        .query.filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Velocity), .inout = EcsIn }
        },
        .callback = Move
    });

    // C applications can also use the ECS_SYSTEM convenience macro:
    // ECS_SYSTEM(ecs, Move, 0, Position, [in] Velocity);

    // Create a few test entities for a Position, Velocity query
    ecs_entity_t e1 = ecs_new_entity(ecs, "e1");
    ecs_set(ecs, e1, Position, {10, 20});
    ecs_set(ecs, e1, Velocity, {1, 2});

    ecs_entity_t e2 = ecs_new_entity(ecs, "e2");
    ecs_set(ecs, e2, Position, {10, 20});
    ecs_set(ecs, e2, Velocity, {3, 4});

    // This entity will not match as it does not have Position, Velocity
    ecs_entity_t e3 = ecs_new_entity(ecs, "e3");
    ecs_set(ecs, e3, Position, {10, 20});

    // Run the system
    ecs_run(ecs, move, 0.0f, NULL);

    return ecs_fini(ecs);
}
