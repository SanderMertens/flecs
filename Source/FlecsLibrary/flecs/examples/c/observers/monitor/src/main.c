#include <monitor.h>
#include <stdio.h>

// A monitor observer triggers when an entity starts/stop matching the observer
// filter. The observer communicates whether an entity is "entering/leaving" the
// monitor by setting ecs_iter_t::event to EcsOnAdd (for entering) or 
// EcsOnRemove (for leaving).
//
// To specify that an observer is a monitor observer, the EcsMonitor tag must be
// provided as event. No additional event kinds should be provided for a monitor
// observer.

typedef struct {
    double x, y;
} Position, Velocity;

void Observer(ecs_iter_t *it) {
    ecs_world_t *ecs = it->world;

    ecs_entity_t event = it->event;
    ecs_entity_t event_id = it->event_id;

    for (int i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        
        if (event == EcsOnAdd) {
            printf(" - Enter: %s: %s\n", 
                ecs_get_name(ecs, event_id),
                ecs_get_name(ecs, e));
        } else if (event == EcsOnRemove) {
            printf(" - Leave: %s: %s\n", 
                ecs_get_name(ecs, event_id),
                ecs_get_name(ecs, e));
        }
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Velocity);

    ecs_observer(ecs, {
        .filter = { .terms = {{ .id = ecs_id(Position) }, { .id = ecs_id(Velocity) }}},
        .events = { EcsMonitor }, // Monitor entities entering/leaving the query
        .callback = Observer,
    });

    // Create entity
    ecs_entity_t e = ecs_new_entity(ecs, "e");

    // This does not yet trigger the monitor, as the entity does not yet match.
    ecs_set(ecs, e, Position, {10, 20});

    // This triggers the monitor with EcsOnAdd, as the entity now matches.
    ecs_set(ecs, e, Velocity, {1, 2});

    // This triggers the monitor with EcsOnRemove, as the entity no longer matches.
    ecs_remove(ecs, e, Position);

    ecs_fini(ecs);

    // Output
    //   - Enter: Velocity: e
    //   - Leave: Position: e
}
