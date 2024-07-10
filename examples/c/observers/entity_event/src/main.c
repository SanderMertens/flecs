#include <entity_event.h>
#include <stdio.h>

// Entity events are events that are emitted and observed for a specific entity.
// They are a thin wrapper around regular observers, which match against queries
// instead of single entities. While they work similarly under the hood, entity
// events provide a much simpler API.
//
// An entity event only needs two pieces of data:
// - The entity on which to emit the event
// - The event to emit
//
// Any (_) is provided as component, indicating that we're not emitting an event 
// for a specific component but rather for the entity itself.

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

    // Emit the Click event
    ecs_emit(ecs, &(ecs_event_desc_t) {
        .event = Click,
        .entity = widget
    });

    // Emit the Resize event
    ecs_emit(ecs, &(ecs_event_desc_t) {
        .event = ecs_id(Resize),
        .entity = widget,
        .param = &(Resize){100, 200} // pass payload
    });

    ecs_fini(ecs);

    // Output
    //  clicked on MyWidget!
    //  resized MyWidget to {100, 200}!
}
