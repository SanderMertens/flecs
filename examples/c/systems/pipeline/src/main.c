#include <pipeline.h>
#include <stdio.h>

typedef struct {
    double x, y;
} Position, Velocity;

void Move(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    const Velocity *v = ecs_term(it, const Velocity, 2);

    for (int i = 0; i < it->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}

void PrintPosition(ecs_iter_t *it) {
    const Position *p = ecs_term(it, const Position, 1);

    for (int i = 0; i < it->count; i ++) {
        printf("%s: {%f, %f}\n", ecs_get_name(it->world, it->entities[i]), 
            p[i].x, p[i].y);
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Velocity);

    // Create system for moving an entity in the EcsOnUpdate phase
    ECS_SYSTEM(ecs, Move, EcsOnUpdate, Position, [in] Velocity);

    // Create system for printing a Position in the EcsPostUpdate phase
    ECS_SYSTEM(ecs, PrintPosition, EcsPostUpdate, [in] Position);

    // Create a few test entities for a Position, Velocity query
    ecs_entity_t e1 = ecs_new_entity(ecs, "e1");
    ecs_set(ecs, e1, Position, {10, 20});
    ecs_set(ecs, e1, Velocity, {1, 2});

    ecs_entity_t e2 = ecs_new_entity(ecs, "e2");
    ecs_set(ecs, e2, Position, {10, 20});
    ecs_set(ecs, e2, Velocity, {3, 4});

    // Run the default pipeline. This will run all systems ordered by their
    // phase. Systems within the same phase are ran in declaration order. This
    // function is usually called in a loop.
    ecs_progress(ecs, 0.0f);

    return ecs_fini(ecs);
}
