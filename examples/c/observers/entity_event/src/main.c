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

// The event to emit.
ECS_TAG_DECLARE(OnClick);

void Observer(ecs_iter_t *it) {
    // The event source can be obtained with ecs_field_src(1). This allows the
    // same event function to be used for different entities.
    char *path = ecs_get_fullpath(it->world, ecs_field_src(it, 1));
    printf("clicked on %s!\n", path);
    ecs_os_free(path);
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_TAG_DEFINE(ecs, OnClick);

    // Create a widget entity
    ecs_entity_t widget = ecs_new_entity(ecs, "MyWidget");

    // Create entity observer. Use EcsAny to indicate we're not interested in
    // matching specific components.
    ecs_observer(ecs, {
        .filter.terms = {{ .id = EcsAny, .src.id = widget }},
        .events = { OnClick },
        .callback = Observer
    });

    // Emit event for entity
    ecs_emit(ecs, &(ecs_event_desc_t) {
        .event = OnClick,
        .entity = widget
    });

    ecs_fini(ecs);

    // Output
    //   clicked on ::MyWidget!
}
