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
    ecs_script_to_local_t to_local;
    void *to_local_ctx;
    uint64_t last_id;
    uint64_t current;
    bool stopped;
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
    (void)argc;
    ecs_entity_t target = *(ecs_entity_t*)argv[0].ptr;
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

static void flecs_script_event_to_local(
    ecs_world_t *world,
    const EcsScriptEventState *state,
    ecs_entity_t entity,
    EcsScriptMouseEvent *e)
{
    e->local_x = e->screen_x;
    e->local_y = e->screen_y;
    if (state->to_local) {
        state->to_local(world, entity, e->screen_x, e->screen_y,
            &e->local_x, &e->local_y, state->to_local_ctx);
    }
}

void ecs_script_mouse_event(
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
    e.id = ++ state->last_id;
    ecs_entity_t type = ecs_id(EcsScriptMouseEvent);
    int32_t resolved = 0;

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

    EcsScriptMouseEvent leave = e;
    leave.id = 0;
    ecs_entity_t prev = state->prev_hover;
    for (; prev && ecs_is_alive(world, prev);
        prev = ecs_get_target(world, prev, EcsChildOf, 0))
    {
        if (!flecs_script_event_is_ancestor_or_self(world, prev, e.target)) {
            resolved += flecs_script_event_deliver_one(
                state, FlecsScriptEventLeave, prev, type, &leave);
        }
    }

    uint32_t pressed_mask = state->pressed_mask;
    uint32_t released_mask = state->released_mask;
    ecs_entity_t pressed = state->pressed;
    ecs_entity_t prev_hover = state->prev_hover;
    bool click = released_mask && e.target == pressed;
    uint64_t current = state->current;
    bool stopped = state->stopped;
    state->current = e.id;
    state->stopped = false;

    ecs_entity_t cur = e.target;
    while (cur && ecs_is_alive(world, cur)) {
        bool entered = !flecs_script_event_is_ancestor_or_self(
            world, cur, prev_hover);
        if (state->stopped && !entered) {
            break;
        }

        ecs_entity_t parent = ecs_get_target(world, cur, EcsChildOf, 0);
        EcsScriptMouseEvent level = e;
        if (cur != e.target) {
            flecs_script_event_to_local(world, state, cur, &level);
        }

        EcsScriptMouseEvent enter = level;
        enter.id = 0;

        bool bubble = !state->stopped;
        bool captured = flecs_script_event_is_ancestor_or_self(
            world, cur, pressed);
        bool released = released_mask && captured;
        bool deliver[] = {
            [FlecsScriptEventPress] = bubble && pressed_mask != 0,
            [FlecsScriptEventDrag] = bubble && (pressed_mask ||
                (e.buttons && !released_mask && captured)),
            [FlecsScriptEventRelease] = bubble && released,
            [FlecsScriptEventClick] = bubble && released && click,
            [FlecsScriptEventMove] = bubble && !e.buttons && !released_mask,
            [FlecsScriptEventEnter] = entered
        };

        for (int32_t k = FlecsScriptEventPress; k <= FlecsScriptEventEnter; k ++) {
            if (deliver[k]) {
                resolved += flecs_script_event_deliver_one(
                    state, (flecs_script_event_kind_t)k, cur, type,
                    k == FlecsScriptEventEnter ? &enter : &level);
            }
        }

        if (resolved) {
            ecs_script_tasks_progress(world);
            state = flecs_script_event_state(world);
            resolved = 0;
        }

        cur = parent;
    }

    if (resolved) {
        ecs_script_tasks_progress(world);
        state = flecs_script_event_state(world);
    }

    state->current = current;
    state->stopped = stopped;
error:
    return;
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
    e.id = ++ state->last_id;
    flecs_script_event_kind_t kind = e.down
        ? FlecsScriptEventKeyDown : FlecsScriptEventKeyUp;
    uint64_t current = state->current;
    bool stopped = state->stopped;
    state->current = e.id;
    state->stopped = false;

    ecs_entity_t cur = state->focus;
    while (cur && ecs_is_alive(world, cur) && !state->stopped) {
        ecs_entity_t parent = ecs_get_target(world, cur, EcsChildOf, 0);
        if (flecs_script_event_deliver_one(state, kind, cur,
            ecs_id(EcsScriptKeyboardEvent), &e))
        {
            ecs_script_tasks_progress(world);
            state = flecs_script_event_state(world);
        }

        cur = parent;
    }

    state->current = current;
    state->stopped = stopped;
error:
    return;
}

void ecs_script_set_to_local(
    ecs_world_t *world,
    ecs_script_to_local_t callback,
    void *ctx)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    world = ECS_CONST_CAST(ecs_world_t*, ecs_get_world(world));
    EcsScriptEventState *state = flecs_script_event_state(world);
    ecs_check(state != NULL, ECS_INVALID_OPERATION,
        "script event module is not imported");
    state->to_local = callback;
    state->to_local_ctx = ctx;
error:
    return;
}

static void flecs_script_event_stop_propagation(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_script_future_t *future)
{
    (void)argc;
    uint64_t id = *(const uint64_t*)ECS_OFFSET(
        argv[0].ptr, (uintptr_t)ctx->ctx);
    EcsScriptEventState *state = flecs_script_event_state(ctx->world);
    bool stopped = state && id && state->current == id;
    if (stopped) {
        state->stopped = true;
    }

    ecs_script_future_resolve(future,
        &(ecs_value_t){ .type = ecs_id(ecs_bool_t), .ptr = &stopped });
    ecs_script_future_release(future);
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
            { .name = "meta", .type = ecs_id(ecs_bool_t) },
            { .name = "id", .type = ecs_id(ecs_u64_t) }
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
            { .name = "meta", .type = ecs_id(ecs_bool_t) },
            { .name = "id", .type = ecs_id(ecs_u64_t) }
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

    ecs_async_method(world, {
        .name = "stop_propagation",
        .parent = ecs_id(EcsScriptMouseEvent),
        .return_type = ecs_id(ecs_bool_t),
        .callback = flecs_script_event_stop_propagation,
        .ctx = (void*)offsetof(EcsScriptMouseEvent, id)
    });

    ecs_async_method(world, {
        .name = "stop_propagation",
        .parent = ecs_id(EcsScriptKeyboardEvent),
        .return_type = ecs_id(ecs_bool_t),
        .callback = flecs_script_event_stop_propagation,
        .ctx = (void*)offsetof(EcsScriptKeyboardEvent, id)
    });

    ecs_singleton_add(world, EcsScriptEventState);
}

#endif
