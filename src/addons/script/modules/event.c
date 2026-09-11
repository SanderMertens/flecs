/**
 * @file addons/script/modules/event.c
 * @brief Mouse and keyboard events for flecs script.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT_EVENT
#include "../script.h"

ECS_COMPONENT_DECLARE(EcsScriptMouseEvent);
ECS_COMPONENT_DECLARE(EcsScriptKeyboardEvent);

typedef enum {
    FlecsScriptEventPress,
    FlecsScriptEventDrag,
    FlecsScriptEventRelease,
    FlecsScriptEventClick,
    FlecsScriptEventMove,
    FlecsScriptEventEnter,
    FlecsScriptEventLeave,
    FlecsScriptEventKeyDown,
    FlecsScriptEventKeyUp,
    FlecsScriptEventKindCount
} flecs_script_event_kind_t;

static const char *flecs_script_event_names[] = {
    "press", "drag", "release", "click", "move", "enter", "leave",
    "key_down", "key_up"
};

typedef struct {
    ecs_script_future_t *future;
    ecs_entity_t target;
    flecs_script_event_kind_t kind;
} flecs_script_event_waiter_t;

/* Singleton with the pending listeners and the state of the last event */
typedef struct EcsScriptEventState {
    ecs_vec_t waiters;
    EcsScriptMouseEvent last;   /* Last event, with the deltas it was given */
    uint32_t pressed_mask;      /* Buttons pressed by the last event */
    uint32_t released_mask;     /* Buttons released by the last event */
    ecs_entity_t pressed;       /* Entity that received the last press */
    ecs_entity_t focus;         /* Receives keyboard events */
    ecs_entity_t hover;         /* Target of the last event */
    ecs_entity_t prev_hover;    /* Target of the event before that */
    ecs_entity_t last_target;   /* Target of the last call */
    bool has_last;
} EcsScriptEventState;

static ECS_COMPONENT_DECLARE(EcsScriptEventState);

static ECS_CTOR(EcsScriptEventState, state, {
    ecs_os_zeromem(state);
    ecs_vec_init_t(NULL, &state->waiters, flecs_script_event_waiter_t, 0);
})

static ECS_DTOR(EcsScriptEventState, state, {
    flecs_script_event_waiter_t *waiters = ecs_vec_first(&state->waiters);
    for (int32_t w = 0; w < ecs_vec_count(&state->waiters); w ++) {
        ecs_script_future_release(waiters[w].future);
    }
    ecs_vec_fini_t(NULL, &state->waiters, flecs_script_event_waiter_t);
})

static EcsScriptEventState* flecs_script_event_state(
    ecs_world_t *world)
{
    if (!ecs_id(EcsScriptEventState) || ecs_is_fini(world)) {
        return NULL;
    }
    return ecs_singleton_get_mut(world, EcsScriptEventState);
}

static void flecs_script_event_wait(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_script_future_t *future)
{
    flecs_script_event_kind_t kind =
        (flecs_script_event_kind_t)(uintptr_t)ctx->ctx;
    ecs_entity_t target = argc == 1 ? *(ecs_entity_t*)argv[0].ptr : 0;
    EcsScriptEventState *state = flecs_script_event_state(ctx->world);
    if (!target || !state) {
        char msg[64];
        ecs_os_snprintf(msg, sizeof(msg), "on.%s expects an entity",
            flecs_script_event_names[kind]);
        ecs_script_future_reject(future, msg);
        ecs_script_future_release(future);
        return;
    }

    ecs_vec_append_t(NULL, &state->waiters, flecs_script_event_waiter_t)[0] =
        (flecs_script_event_waiter_t){ future, target, kind };
}

static void flecs_script_event_cancel(
    const ecs_function_ctx_t *ctx,
    ecs_script_future_t *future)
{
    EcsScriptEventState *state = flecs_script_event_state(ctx->world);
    if (!state) {
        return;
    }
    flecs_script_event_waiter_t *waiters = ecs_vec_first(&state->waiters);
    for (int32_t i = 0; i < ecs_vec_count(&state->waiters); i ++) {
        if (waiters[i].future == future) {
            ecs_script_future_release(future);
            ecs_vec_remove_ordered_t(
                &state->waiters, flecs_script_event_waiter_t, i);
            return;
        }
    }
}

/* Resolve the listeners for an event kind on a single entity */
static int32_t flecs_script_event_deliver_one(
    EcsScriptEventState *state,
    flecs_script_event_kind_t kind,
    ecs_entity_t target,
    ecs_entity_t type,
    void *value)
{
    int32_t resolved = 0;
    for (int32_t i = 0; i < ecs_vec_count(&state->waiters); ) {
        flecs_script_event_waiter_t *w = ecs_vec_get_t(
            &state->waiters, flecs_script_event_waiter_t, i);
        if (w->kind != kind || w->target != target) {
            i ++;
            continue;
        }
        ecs_script_future_t *future = w->future;
        ecs_vec_remove_ordered_t(
            &state->waiters, flecs_script_event_waiter_t, i);
        ecs_script_future_resolve(future,
            &(ecs_value_t){ .type = type, .ptr = value });
        ecs_script_future_release(future);
        resolved ++;
    }
    return resolved;
}

/* Resolve the listeners for an event kind on an entity and its parents */
static int32_t flecs_script_event_deliver(
    ecs_world_t *world,
    EcsScriptEventState *state,
    flecs_script_event_kind_t kind,
    ecs_entity_t target,
    ecs_entity_t type,
    void *value)
{
    int32_t resolved = 0;
    for (; target && ecs_is_alive(world, target);
        target = ecs_get_target(world, target, EcsChildOf, 0))
    {
        resolved += flecs_script_event_deliver_one(
            state, kind, target, type, value);
    }
    return resolved;
}

static bool flecs_script_event_is_ancestor_or_self(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_entity_t target)
{
    for (; target && ecs_is_alive(world, target);
        target = ecs_get_target(world, target, EcsChildOf, 0))
    {
        if (target == e) {
            return true;
        }
    }
    return false;
}

/* A call with the same pointer state as the previous call, for the parent of
 * the previous target, propagates the previous event. */
static bool flecs_script_event_is_propagation(
    ecs_world_t *world,
    const EcsScriptEventState *state,
    const EcsScriptMouseEvent *e)
{
    const EcsScriptMouseEvent *last = &state->last;
    return state->has_last && e->target && state->last_target &&
        ecs_is_alive(world, state->last_target) &&
        ecs_get_target(world, state->last_target, EcsChildOf, 0) == e->target &&
        ECS_EQ(e->screen_x, last->screen_x) &&
        ECS_EQ(e->screen_y, last->screen_y) &&
        e->button == last->button && e->buttons == last->buttons &&
        e->ctrl == last->ctrl && e->shift == last->shift &&
        e->alt == last->alt && e->meta == last->meta;
}

bool ecs_script_mouse_event(
    ecs_world_t *world,
    const EcsScriptMouseEvent *evt)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(evt != NULL, ECS_INVALID_PARAMETER, NULL);
    world = ECS_CONST_CAST(ecs_world_t*, ecs_get_world(world));
    EcsScriptEventState *state = flecs_script_event_state(world);
    ecs_check(state != NULL, ECS_INVALID_OPERATION,
        "script event module is not imported");

    EcsScriptMouseEvent e = *evt;
    ecs_entity_t type = ecs_id(EcsScriptMouseEvent);
    int32_t resolved = 0;

    if (!flecs_script_event_is_propagation(world, state, &e)) {
        /* New pointer event: derive what happened from the previous one */
        if (ECS_EQZERO(e.delta_x) && ECS_EQZERO(e.delta_y)) {
            e.delta_x = e.local_x - state->last.local_x;
            e.delta_y = e.local_y - state->last.local_y;
        }
        state->pressed_mask = e.buttons & ~state->last.buttons;
        state->released_mask = state->last.buttons & ~e.buttons;
        state->prev_hover = state->hover;
        state->hover = e.target;
        if (!state->last.buttons) {
            state->pressed = 0;
        }
        if (state->pressed_mask) {
            state->pressed = e.target;
            state->focus = e.target;
        }
        state->last = e;
        state->has_last = true;

        /* Leave goes to the entities that are no longer under the pointer.
         * The host doesn't propagate over the previous target, so this walks
         * the previous parent chain here. */
        ecs_entity_t prev = state->prev_hover;
        for (; prev && ecs_is_alive(world, prev);
            prev = ecs_get_target(world, prev, EcsChildOf, 0))
        {
            if (!flecs_script_event_is_ancestor_or_self(world, prev, e.target)) {
                resolved += flecs_script_event_deliver_one(
                    state, FlecsScriptEventLeave, prev, type, &e);
            }
        }
    } else {
        e.delta_x = state->last.delta_x;
        e.delta_y = state->last.delta_y;
    }

    ecs_entity_t target = e.target;
    state->last_target = target;
    if (!target) {
        goto done;
    }

    bool captured = flecs_script_event_is_ancestor_or_self(
        world, target, state->pressed);

    if (state->pressed_mask) {
        resolved += flecs_script_event_deliver_one(
            state, FlecsScriptEventPress, target, type, &e);
        resolved += flecs_script_event_deliver_one(
            state, FlecsScriptEventDrag, target, type, &e);
    } else if (e.buttons && !state->released_mask && captured) {
        resolved += flecs_script_event_deliver_one(
            state, FlecsScriptEventDrag, target, type, &e);
    }

    if (state->released_mask && captured) {
        resolved += flecs_script_event_deliver_one(
            state, FlecsScriptEventRelease, target, type, &e);
        if (state->hover == state->pressed) {
            resolved += flecs_script_event_deliver_one(
                state, FlecsScriptEventClick, target, type, &e);
        }
    }

    if (!e.buttons && !state->released_mask) {
        resolved += flecs_script_event_deliver_one(
            state, FlecsScriptEventMove, target, type, &e);
    }

    if (!flecs_script_event_is_ancestor_or_self(
        world, target, state->prev_hover))
    {
        resolved += flecs_script_event_deliver_one(
            state, FlecsScriptEventEnter, target, type, &e);
    }

done:
    if (resolved) {
        ecs_script_tasks_progress(world);
    }
    return true;
error:
    return false;
}

void ecs_script_keyboard_event(
    ecs_world_t *world,
    const EcsScriptKeyboardEvent *evt)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(evt != NULL, ECS_INVALID_PARAMETER, NULL);
    world = ECS_CONST_CAST(ecs_world_t*, ecs_get_world(world));
    EcsScriptEventState *state = flecs_script_event_state(world);
    ecs_check(state != NULL, ECS_INVALID_OPERATION,
        "script event module is not imported");

    EcsScriptKeyboardEvent e = *evt;
    if (flecs_script_event_deliver(world, state,
        e.down ? FlecsScriptEventKeyDown : FlecsScriptEventKeyUp,
        state->focus, ecs_id(EcsScriptKeyboardEvent), &e))
    {
        ecs_script_tasks_progress(world);
    }
error:
    return;
}

void FlecsScriptEventImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsScriptEvent);
    ECS_IMPORT(world, FlecsScript);
    ecs_set_name_prefix(world, "EcsScript");

    ECS_COMPONENT_DEFINE(world, EcsScriptMouseEvent);
    ECS_COMPONENT_DEFINE(world, EcsScriptKeyboardEvent);
    ECS_COMPONENT_DEFINE(world, EcsScriptEventState);

    ecs_set_hooks(world, EcsScriptEventState, {
        .ctor = ecs_ctor(EcsScriptEventState),
        .dtor = ecs_dtor(EcsScriptEventState),
        .flags = ECS_TYPE_HOOK_COPY_ILLEGAL | ECS_TYPE_HOOK_MOVE_ILLEGAL
    });

    ecs_struct(world, {
        .entity = ecs_id(EcsScriptMouseEvent),
        .members = {
            { .name = "target", .type = ecs_id(ecs_entity_t) },
            { .name = "screen_x", .type = ecs_id(ecs_f32_t) },
            { .name = "screen_y", .type = ecs_id(ecs_f32_t) },
            { .name = "local_x", .type = ecs_id(ecs_f32_t) },
            { .name = "local_y", .type = ecs_id(ecs_f32_t) },
            { .name = "delta_x", .type = ecs_id(ecs_f32_t) },
            { .name = "delta_y", .type = ecs_id(ecs_f32_t) },
            { .name = "button", .type = ecs_id(ecs_i32_t) },
            { .name = "buttons", .type = ecs_id(ecs_u32_t) },
            { .name = "ctrl", .type = ecs_id(ecs_bool_t) },
            { .name = "shift", .type = ecs_id(ecs_bool_t) },
            { .name = "alt", .type = ecs_id(ecs_bool_t) },
            { .name = "meta", .type = ecs_id(ecs_bool_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(EcsScriptKeyboardEvent),
        .members = {
            { .name = "key", .type = ecs_id(ecs_string_t) },
            { .name = "down", .type = ecs_id(ecs_bool_t) },
            { .name = "repeat", .type = ecs_id(ecs_bool_t) },
            { .name = "ctrl", .type = ecs_id(ecs_bool_t) },
            { .name = "shift", .type = ecs_id(ecs_bool_t) },
            { .name = "alt", .type = ecs_id(ecs_bool_t) },
            { .name = "meta", .type = ecs_id(ecs_bool_t) }
        }
    });

    ecs_entity_t on = ecs_entity(world, {
        .name = "on", .parent = ecs_lookup(world, "flecs.script") });

    for (int32_t i = 0; i < FlecsScriptEventKindCount; i ++) {
        ecs_async_function(world, {
            .name = flecs_script_event_names[i],
            .parent = on,
            .return_type = i < FlecsScriptEventKeyDown
                ? ecs_id(EcsScriptMouseEvent)
                : ecs_id(EcsScriptKeyboardEvent),
            .params = {{ "target", ecs_id(ecs_entity_t) }},
            .callback = flecs_script_event_wait,
            .cancel = flecs_script_event_cancel,
            .ctx = (void*)(uintptr_t)i
        });
    }

    ecs_singleton_add(world, EcsScriptEventState);
}

#endif
