#include <enqueue_entity_event.h>
#include <stdio.h>

// This is the same example as entity_event, but instead of emit() the example
// uses the enqueue() method. Whereas emit() invokes observers synchronously,
// enqueue() adds the event to the command queue, which delays invoking 
// observers until the next time the command queue is flushed.

// Event without payload
ECS_TAG_DECLARE(Click);

// Event with payload
typedef struct {
    double width, height;
} Resize;

ECS_COMPONENT_DECLARE(Resize);

void OnClick(ecs_iter_t *it) {
    // The event source can be obtained with ecs_field_src(1). This allows the
    // same event function to be used for different entities.
    char *path = ecs_get_path(it->world, ecs_field_src(it, 0));
    printf("clicked on %s!\n", path);
    ecs_os_free(path);
}

void OnResize(ecs_iter_t *it) {
    // Event payload can be obtained from the it->param member
    Resize *p = it->param;
    char *path = ecs_get_path(it->world, ecs_field_src(it, 0));
    printf("resized %s to {%.0f, %.0f}!\n", path, p->width, p->height);
    ecs_os_free(path);
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_TAG_DEFINE(ecs, Click);
    ECS_COMPONENT_DEFINE(ecs, Resize);

    // Create a widget entity
    ecs_entity_t widget = ecs_entity(ecs, { .name = "MyWidget" });

    // Create entity observer. Use EcsAny to indicate we're not interested in
    // matching specific components.
    ecs_observer(ecs, {
        .query.terms = {{ .id = EcsAny, .src.id = widget }},
        .events = { Click },
        .callback = OnClick
    });

    // Create another one for the Resize event
    ecs_observer(ecs, {
        .query.terms = {{ .id = EcsAny, .src.id = widget }},
        .events = { ecs_id(Resize) },
        .callback = OnResize
    });

    // We can only call enqueue events while the world is deferred mode.
    ecs_defer_begin(ecs);

    // Emit the Click event
    ecs_enqueue(ecs, &(ecs_event_desc_t) {
        .event = Click,
        .entity = widget
    });

    // Emit the Resize event
    ecs_enqueue(ecs, &(ecs_event_desc_t) {
        .event = ecs_id(Resize),
        .entity = widget,
        .param = &(Resize){100, 200} // pass payload
    });

    printf("Events enqueued\n");

    // Flushes the queue, and invokes the observer
    ecs_defer_end(ecs);

    ecs_fini(ecs);

    // Output
    //  Events enqueued
    //  clicked on MyWidget!
    //  resized MyWidget to {100, 200}!
}
