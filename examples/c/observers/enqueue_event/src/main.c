#include <enqueue_event.h>
#include <stdio.h>

// This is the same example as custom_event, but instead of emit() the example
// uses the enqueue() function. Whereas emit() invokes observers synchronously,
// enqueue() adds the event to the command queue, which delays invoking 
// observers until the next time the command queue is flushed.

typedef struct {
    double x, y;
} Position;

void Observer(ecs_iter_t *it) {
    ecs_world_t *ecs = it->stage;
    
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
    ecs_entity_t MyEvent = ecs_entity(ecs, { .name = "MyEvent" });

    // Create observer for custom event
    ECS_OBSERVER(ecs, Observer, MyEvent, Position);

    // Create entity
    ecs_entity_t e = ecs_entity(ecs, { .name = "e" });
    
    // The observer query can be matched against the entity, so make sure it
    // has the Position component before emitting the event. This does not 
    // trigger the observer yet.
    ecs_set(ecs, e, Position, {10, 20});

    ecs_world_t *stage_1 = ecs_is_deferred(ecs) ? ecs : ecs_get_stage(ecs, 0);

    // Emit the custom event
    ecs_enqueue(stage_1, &(ecs_event_desc_t) {
        .event = MyEvent,
        .ids = &(ecs_type_t){ (ecs_id_t[]){ ecs_id(Position) }, 1 }, // 1 id
        .entity = e
    });
    
    printf("Event enqueued!\n");

    // Flushes the queue, and invokes the observer
    ecs_merge(stage_1);

    ecs_fini(ecs);

    // Output
    //   Event enqueued!
    //    - MyEvent: Position: e
}
