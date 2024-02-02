#include <yield_existing.h>
#include <stdio.h>

// Observers can enable a "yield_existing" feature that upon creation of the
// observer produces events for all entities that match the observer query. The
// feature is only implemented for the builtin EcsOnAdd and EcsOnSet events.
//
// Custom events can also implement behavior for yield_existing by adding the
// Iterable component to the event (see EcsIterable for more details).

typedef struct {
    double x, y;
} Position;

void Observer(ecs_iter_t *it) {
    ecs_world_t *ecs = it->world;

    ecs_entity_t event = it->event;
    ecs_entity_t event_id = it->event_id;
    Position *p = ecs_field(it, Position, 1);

    for (int i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        printf(" - %s: %s: %s: {%f, %f}\n", 
            ecs_get_name(ecs, event), 
            ecs_get_name(ecs, event_id),
            ecs_get_name(ecs, e),
            p[i].x, p[i].y);
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, Position);

    // Create existing entities with Position component
    ecs_entity_t e1 = ecs_new_entity(ecs, "e1");
    ecs_set(ecs, e1, Position, {10, 20});
    ecs_entity_t e2 = ecs_new_entity(ecs, "e2");
    ecs_set(ecs, e2, Position, {20, 30});

    ecs_observer(ecs, {
        .filter = { .terms = {{ .id = ecs_id(Position) }}},
        .events = { EcsOnSet },
        .callback = Observer,
        .yield_existing = true // Trigger for existing matching entities
    });

    ecs_fini(ecs);

    // Output
    //   - OnSet: Position: e1: {10.000000, 20.000000}
    //   - OnSet: Position: e2: {20.000000, 30.000000}
}
