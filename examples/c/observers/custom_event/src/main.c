#include <custom_event.h>
#include <stdio.h>

// Observers can be used to match custom events. Custom events can be emitted
// using the ecs_emit function. This function is also used by builtin events,
// so builtin and custom events use the same rules for matching with observers.
//
// An event consists out of three pieces of data used to match with observers:
//  - An single event kind (EcsOnAdd, EcsOnRemove, ...)
//  - One or more event ids (Position, Velocity, ...)
//  - A source (either an entity or a table)

typedef struct {
    double x, y;
} Position;

void Observer(ecs_iter_t *it) {
    ecs_world_t *ecs = it->world;
    
    // The event kind
    ecs_entity_t event = it->event;

    // The (component) id for which the event was emitted
    ecs_entity_t event_id = it->event_id;

    for (int i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];

        printf(" - %s: %s: %s\n", 
            ecs_get_name(ecs, event), 
            ecs_get_name(ecs, event_id),
            ecs_get_name(ecs, e));
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, Position);

    // Create custom event
    ecs_entity_t MyEvent = ecs_new_entity(ecs, "MyEvent");

    // Create observer for custom event
    ECS_OBSERVER(ecs, Observer, MyEvent, Position);

    // Create entity
    ecs_entity_t e = ecs_new_entity(ecs, "e");
    
    // The observer filter can be matched against the entity, so make sure it
    // has the Position component before emitting the event. This does not 
    // trigger the observer yet.
    ecs_set(ecs, e, Position, {10, 20});

    // Emit the custom event
    ecs_emit(ecs, &(ecs_event_desc_t) {
        .event = MyEvent,
        .ids = &(ecs_type_t){ (ecs_id_t[]){ ecs_id(Position) }, 1 }, // 1 id
        .entity = e
    });

    ecs_fini(ecs);

    // Output
    //    - MyEvent: Position: e
}
