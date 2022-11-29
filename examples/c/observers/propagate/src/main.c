#include <propagate.h>
#include <stdio.h>

// Events are propagated along relationship edges. This means that observers can
// listen for events from a parent or prefab, like triggering when a component
// inherited from a prefab was set.
//
// Event propagation happens automatically when an observer contains a filter
// with the EcsUp flag set (indicating upwards traversal). Observers use the 
// same matching logic as queries: if a query with upwards traversal matches an
// entity, so will an observer.
//
// Events are only propagated along acyclic relationship edges.

typedef struct {
    double x, y;
} Position;

void Observer(ecs_iter_t *it) {
    ecs_world_t *ecs = it->world;
    
    // The event kind
    ecs_entity_t event = it->event;

    // The (component) id for which the event was emitted
    ecs_entity_t event_id = it->event_id;

    // Grab Position from self and parent
    Position *p_self = ecs_field(it, Position, 1);
    Position *p_parent = ecs_field(it, Position, 2);

    for (int i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];

        printf(" - %s: %s: %s: self: {%f, %f}, parent: {%f, %f}\n", 
            ecs_get_name(ecs, event), 
            ecs_get_name(ecs, event_id),
            ecs_get_name(ecs, e),
            p_self[i].x, p_self[i].y,
            p_parent[i].x, p_parent[i].y);
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, Position);

    // Create observer that listens for events from both self and parent
    ecs_observer(ecs, {
        .filter = { .terms = {
            // Listen for Position events from self
            { .id = ecs_id(Position) },
            // Listen for Position events from parent
            { .id = ecs_id(Position), .src.flags = EcsUp, .src.trav = EcsChildOf }
        }},
        .events = { EcsOnSet },
        .callback = Observer
    });

    // Create entity and parent
    ecs_entity_t p = ecs_new_entity(ecs, "p");
    ecs_entity_t e = ecs_new_entity(ecs, "p.e"); // Create as child of p

    // Set Position on entity. This doesn't trigger the observer yet, since the
    // parent doesn't have Position yet.
    ecs_set(ecs, e, Position, {10, 20});

    // Set Position on parent. This event will be propagated and trigger the 
    // observer, as the observer query now matches.
    ecs_set(ecs, p, Position, {1, 2});

    ecs_fini(ecs);

    // Output
    //   - OnSet: Position: e: self: {10.000000, 20.000000}, parent: {1.000000, 2.000000}
}
