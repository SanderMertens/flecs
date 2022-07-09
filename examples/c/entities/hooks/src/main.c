#include <hooks.h>
#include <stdio.h>

// Component hooks are callbacks that can be registered for a type that are
// invoked during different parts of the component lifecycle.

typedef struct {
    char *value; // Pointer to external memory
} String;

// Resource management hooks. The convenience macros hide details of
// the callback signature, while allowing hooks to be called on multiple 
// entities.

// The constructor should initialize the component value.
ECS_CTOR(String, ptr, {
    ecs_trace("Ctor");
    ptr->value = NULL;
})

// The destructor should free resources.
ECS_DTOR(String, ptr, {
    ecs_trace("Dtor");
    ecs_os_free(ptr->value);
})

// The move hook should move resources from one location to another.
ECS_MOVE(String, dst, src, {
    ecs_trace("Move");
    ecs_os_free(dst->value);
    dst->value = src->value;
    src->value = NULL; // This makes sure the value doesn't get deleted twice,
                       // as the destructor is still invoked after a move.
})

// The copy hook should copy resources from one location to another.
ECS_COPY(String, dst, src, {
    ecs_trace("Copy");
    ecs_os_free(dst->value);
    dst->value = ecs_os_strdup(src->value);
})

// This callback is used for the add, remove and set hooks. Note that the
// signature is the same as systems, triggers, observers.
void hook_callback(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    ecs_entity_t event = it->event;

    for (int i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_trace("%s: %s", 
            ecs_get_name(world, event), ecs_get_name(world, e));
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, String);
    ECS_TAG(world, Tag);

    ecs_set_hooks(world, String, {
        /* Resource management hooks. These hooks should primarily be used for
         * managing memory used by the component. */
        .ctor = ecs_ctor(String),
        .move = ecs_move(String),
        .copy = ecs_copy(String),
        .dtor = ecs_dtor(String),

        /* Lifecycle hooks. These hooks should be used for application logic. */
        .on_add = hook_callback,
        .on_remove = hook_callback,
        .on_set = hook_callback
    });

    ecs_log_set_level(0);

    ecs_entity_t e = ecs_new_entity(world, "Entity");

    ecs_trace("ecs_add(world, e, String)");
    ecs_log_push();
        ecs_add(world, e, String);
    ecs_log_pop();

    ecs_trace("ecs_set(world, e, String, {\"Hello World\"})");
    ecs_log_push();
        ecs_set(world, e, String, {(char*)"Hello World"});
    ecs_log_pop();

    // This operation changes the entity's archetype, which invokes a move
    ecs_trace("ecs_add(world, e, Tag)");
    ecs_log_push();
        ecs_add(world, e, Tag);
    ecs_log_pop();

    ecs_trace("ecs_delete(world, e)");
    ecs_log_push();
        ecs_delete(world, e);
    ecs_log_pop();

    ecs_log_set_level(-1);

    return ecs_fini(world);

    // Output:
    //   info: ecs_add(world, e, String)
    //   info: | Ctor
    //   info: | OnAdd: Entity
    //   info: ecs_set(world, e, String, {"Hello World"})
    //   info: | Copy
    //   info: | OnSet: Entity
    //   info: ecs_add(world, e, Tag)
    //   info: | Ctor
    //   info: | Move
    //   info: | Dtor
    //   info: ecs_delete(world, e)
    //   info: | OnRemove: Entity
    //   info: | Dtor
}
