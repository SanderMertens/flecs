#include <basics.h>
#include <stdio.h>

// Observers provide a mechanism for responding to builtin and user defined
// events. They are similar to systems, in that they have the same callback
// signature and use the same query interface to match with entities, but 
// instead of a phase have an event kind.
//
// The most commonly used builtin events are:
//  - EcsOnAdd: a component was added
//  - EcsOnRemove: a component was removed
//  - EcsOnSet: a component's value was changed
//
// The EcsOnAdd and EcsOnRemove events are only thrown when a component is 
// actually added or removed. If an application invokes ecs_add and the entity
// already has the component, no event is emitted. Similarly, if an application
// invokes ecs_remove for a component the entity doesn't have, no event is 
// emitted. That is in contrast to EcsOnSet, which is invoked each time ecs_set 
// or ecs_modified is invoked.
//
// Observers are different from component hooks in a number of ways:
//  - A component can only have one hook, whereas it can match many observers
//  - A hook matches one component, whereas observers can match complex queries
//  - Hooks are for add/set/remove events, observers can match custom events.

typedef struct {
    double x, y;
} Position;

void Observer(ecs_iter_t *it) {
    ecs_world_t *ecs = it->world;
    
    // The event kind
    ecs_entity_t event = it->event;

    // The (component) id for which the event was emitted
    ecs_entity_t event_id = it->event_id;

    // Get component values as usual
    Position *p = ecs_field(it, Position, 1);

    for (int i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];

        if (event == EcsOnAdd) {
            // No assumptions about the component value should be made here. If 
            // a ctor for the component was registered it will be called before
            // the EcsOnAdd event, but a value assigned by set won't be visible.
            printf(" - OnAdd: %s: %s\n", 
                ecs_get_name(ecs, event_id),
                ecs_get_name(ecs, e));
        } else {
            // EcsOnSet or EcsOnRemove event
            printf(" - %s: %s: %s: {%f, %f}\n", 
                ecs_get_name(ecs, event), 
                ecs_get_name(ecs, event_id),
                ecs_get_name(ecs, e),
                p[i].x, p[i].y);
        }
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, Position);

    // The ecs_observer macro (which calls the ecs_observer_init function) can
    // be used to create an observer.
    ecs_observer(ecs, {
        // Observer filter. Uses same ecs_filter_desc_t as systems/queries
        .filter = { .terms = {{ .id = ecs_id(Position) }}},
        // Events the observer will listen for. Can contain multiple events
        .events = { EcsOnAdd, EcsOnRemove },
        // Observer callback
        .callback = Observer
    });

    // Alternatively an observer can be created with the ECS_OBSERVER macro:
    ECS_OBSERVER(ecs, Observer, EcsOnSet, Position);

    // Create entity
    ecs_entity_t e = ecs_new_entity(ecs, "e");
    
    // Set Position (emits EcsOnAdd and EcsOnSet)
    ecs_set(ecs, e, Position, {10, 20});

    // Remove component (emits EcsOnRemove)
    ecs_remove(ecs, e, Position);

    // Remove component again (no event is emitted)
    ecs_remove(ecs, e, Position);

    ecs_fini(ecs);

    // Output
    //   - OnAdd: Position: e
    //   - OnSet: Position: e: {10.000000, 20.000000}
    //   - OnRemove: Position: e: {10.000000, 20.000000}
}
