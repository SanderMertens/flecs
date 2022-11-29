#include <two_components.h>
#include <stdio.h>

// An observer can match multiple components/tags. Only entities that match the
// entire observer filter will be forwarded to the callback. For example, an
// observer for Position,Velocity won't match an entity that only has Position.

typedef struct {
    double x, y;
} Position, Velocity;

void Observer(ecs_iter_t *it) {
    ecs_world_t *ecs = it->world;

    ecs_entity_t event = it->event;
    ecs_entity_t event_id = it->event_id;

    Position *p = ecs_field(it, Position, 1);
    Velocity *v = ecs_field(it, Velocity, 2);

    for (int i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        printf("%s: %s: %s: p: {%f, %f} v: {%f, %f}\n",
            ecs_get_name(ecs, event),
            ecs_get_name(ecs, event_id),
            ecs_get_name(ecs, e),
            p[i].x, p[i].y,
            v[i].x, v[i].y);
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Velocity);

    ECS_OBSERVER(ecs, Observer, EcsOnSet, Position, Velocity);

    // Create entity
    ecs_entity_t e = ecs_new_entity(ecs, "e");
    
    // Set Position (emits EcsOnSet, does not yet match observer)
    ecs_set(ecs, e, Position, {10, 20});

    // Set Velocity (emits EcsOnSet, matches observer)
    ecs_set(ecs, e, Velocity, {1, 2});

    return ecs_fini(ecs);

    // Output:
    //   OnSet: Velocity: e: p: {10.000000, 20.000000} v: {1.000000, 2.000000}
}
