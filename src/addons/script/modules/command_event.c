/**
 * @file addons/script/modules/command_event.c
 * @brief Commands for emitting mouse, keyboard and focus events.
 */

#include "flecs.h"

#if defined(FLECS_SCRIPT_COMMAND) && defined(FLECS_SCRIPT_EVENT)
#include "command.h"

static const char* flecs_command_parse_modifiers(
    const char *str,
    EcsScriptKeyboardEvent *mods)
{
    const char *ptr = str;
    while (true) {
        const char *plus = strchr(ptr, '+');
        if (!plus || plus == ptr || !plus[1]) {
            break;
        }

        ecs_size_t len = flecs_ito(ecs_size_t, plus - ptr);
        if (len == 4 && !ecs_os_strncmp(ptr, "ctrl", 4)) {
            mods->ctrl = true;
        } else if (len == 5 && !ecs_os_strncmp(ptr, "shift", 5)) {
            mods->shift = true;
        } else if (len == 3 && !ecs_os_strncmp(ptr, "alt", 3)) {
            mods->alt = true;
        } else if (len == 4 && !ecs_os_strncmp(ptr, "meta", 4)) {
            mods->meta = true;
        } else {
            ecs_err("invalid modifier '%.*s' in '%s', expected ctrl, shift, "
                "alt or meta", len, ptr, str);
            return NULL;
        }

        ptr = plus + 1;
    }

    if (!ptr[0]) {
        ecs_err("missing key in '%s'", str);
        return NULL;
    }

    return ptr;
}

static void flecs_command_pointer_event(
    ecs_world_t *world,
    ecs_entity_t entity,
    bool has_position,
    float x,
    float y,
    EcsScriptMouseEvent *evt)
{
    EcsScriptMouseEvent last;
    ecs_script_get_mouse_state(world, &last, NULL);

    ecs_os_zeromem(evt);
    evt->target = entity;
    evt->buttons = last.buttons;

    if (!has_position && entity == last.target) {
        evt->local_x = last.local_x;
        evt->local_y = last.local_y;
        evt->screen_x = last.screen_x;
        evt->screen_y = last.screen_y;
        return;
    }

    evt->local_x = has_position ? x : 0;
    evt->local_y = has_position ? y : 0;
    flecs_script_event_to_screen(world, entity, evt->local_x, evt->local_y,
        &evt->screen_x, &evt->screen_y);
}

static int flecs_command_button_event(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    int32_t button_arg,
    EcsScriptMouseEvent *evt,
    uint32_t *held)
{
    ecs_entity_t e = flecs_command_arg(argv, 0, ecs_entity_t);
    if (!flecs_command_check_alive(ctx, e)) {
        return -1;
    }

    int32_t button = 0;
    EcsScriptKeyboardEvent mods = {0};
    if (argc > button_arg) {
        const char *str = flecs_command_arg(argv, button_arg, char*);
        const char *name = flecs_command_parse_modifiers(str, &mods);
        if (!name) {
            flecs_command_fail(ctx);
            return -1;
        }

        if (!ecs_os_strcmp(name, "middle")) {
            button = 1;
        } else if (!ecs_os_strcmp(name, "right")) {
            button = 2;
        } else if (ecs_os_strcmp(name, "left")) {
            flecs_command_err(ctx,
                "invalid button '%s', expected left, middle or right", name);
            return -1;
        }
    }

    flecs_command_pointer_event(ctx->world, e, false, 0, 0, evt);
    *held = evt->buttons;
    evt->button = button;
    evt->buttons |= (uint32_t)1 << button;
    evt->ctrl = mods.ctrl;
    evt->shift = mods.shift;
    evt->alt = mods.alt;
    evt->meta = mods.meta;
    return 0;
}

static void flecs_command_mouse_emit(
    ecs_world_t *world,
    const EcsScriptMouseEvent *evt,
    ecs_script_event_result_t *result)
{
    ecs_script_event_result_t r = ecs_script_mouse_event(world, evt);
    result->resolved += r.resolved;
    result->stopped |= r.stopped;
}

static void flecs_command_key_emit(
    ecs_world_t *world,
    EcsScriptKeyboardEvent *evt,
    const char *key,
    bool down,
    ecs_script_event_result_t *result)
{
    evt->key = ECS_CONST_CAST(char*, key);
    evt->down = down;
    ecs_script_event_result_t r = ecs_script_keyboard_event(world, evt);
    result->resolved += r.resolved;
    result->stopped |= r.stopped;
}

static void flecs_command_focus(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    ecs_world_t *world = ctx->world;
    if (argc) {
        ecs_entity_t e = flecs_command_arg(argv, 0, ecs_entity_t);
        if (e && !flecs_command_check_alive(ctx, e)) {
            return;
        }

        *(ecs_entity_t*)result->ptr = ecs_script_set_focus(world, e);
    } else {
        *(ecs_entity_t*)result->ptr = ecs_script_get_focus(world);
    }
}

static void flecs_command_blur(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    (void)argv;
    (void)result;
    ecs_script_set_focus(ctx->world, 0);
}

static void flecs_command_pointer(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    (void)argv;
    ecs_command_pointer_state_t *state = result->ptr;
    ecs_script_get_mouse_state(ctx->world, &state->event, &state->pressed);
}

static void flecs_command_move(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    ecs_entity_t e = flecs_command_arg(argv, 0, ecs_entity_t);
    if (!flecs_command_check_alive(ctx, e)) {
        return;
    }

    if (argc == 2) {
        flecs_command_err(ctx, "move expects both x and y");
        return;
    }

    EcsScriptMouseEvent evt;
    flecs_command_pointer_event(ctx->world, e, argc == 3,
        argc == 3 ? flecs_command_arg(argv, 1, float) : 0,
        argc == 3 ? flecs_command_arg(argv, 2, float) : 0, &evt);
    flecs_command_mouse_emit(ctx->world, &evt, result->ptr);
}

static void flecs_command_move_by(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    float dx = flecs_command_arg(argv, 0, float);
    float dy = flecs_command_arg(argv, 1, float);

    EcsScriptMouseEvent evt;
    ecs_script_get_mouse_state(ctx->world, &evt, NULL);
    if (evt.target && !ecs_is_alive(ctx->world, evt.target)) {
        evt.target = 0;
    }

    evt.local_x += dx;
    evt.local_y += dy;
    evt.screen_x += dx;
    evt.screen_y += dy;
    evt.delta_x = dx;
    evt.delta_y = dy;
    evt.id = 0;
    flecs_command_mouse_emit(ctx->world, &evt, result->ptr);
}

static void flecs_command_press(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    EcsScriptMouseEvent evt;
    uint32_t held;
    if (!flecs_command_button_event(ctx, argc, argv, 1, &evt, &held)) {
        flecs_command_mouse_emit(ctx->world, &evt, result->ptr);
    }
}

static int32_t flecs_command_lowest_button(
    uint32_t buttons)
{
    int32_t i;
    for (i = 0; i < 32; i ++) {
        if (buttons & ((uint32_t)1 << i)) {
            return i;
        }
    }

    return 0;
}

static void flecs_command_release(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    ecs_world_t *world = ctx->world;
    EcsScriptMouseEvent last;
    ecs_script_get_mouse_state(world, &last, NULL);

    ecs_entity_t e = argc ? flecs_command_arg(argv, 0, ecs_entity_t) :
        last.target;
    if (argc && !flecs_command_check_alive(ctx, e)) {
        return;
    }

    if (e && !ecs_is_alive(world, e)) {
        e = 0;
    }

    EcsScriptMouseEvent evt;
    flecs_command_pointer_event(world, e, false, 0, 0, &evt);
    evt.button = flecs_command_lowest_button(last.buttons);
    evt.buttons = 0;
    flecs_command_mouse_emit(world, &evt, result->ptr);
}

static void flecs_command_click(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    EcsScriptMouseEvent evt;
    uint32_t held;
    if (flecs_command_button_event(ctx, argc, argv, 1, &evt, &held)) {
        return;
    }

    flecs_command_mouse_emit(ctx->world, &evt, result->ptr);
    if (ecs_is_alive(ctx->world, evt.target)) {
        evt.buttons = held & ~((uint32_t)1 << evt.button);
        flecs_command_mouse_emit(ctx->world, &evt, result->ptr);
    }
}

static void flecs_command_drag(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    ecs_world_t *world = ctx->world;
    float dx = flecs_command_arg(argv, 1, float);
    float dy = flecs_command_arg(argv, 2, float);
    EcsScriptMouseEvent evt;
    uint32_t held;
    if (flecs_command_button_event(ctx, argc, argv, 3, &evt, &held)) {
        return;
    }

    flecs_command_mouse_emit(world, &evt, result->ptr);
    if (!ecs_is_alive(world, evt.target)) {
        return;
    }

    evt.local_x += dx;
    evt.local_y += dy;
    evt.screen_x += dx;
    evt.screen_y += dy;
    evt.delta_x = dx;
    evt.delta_y = dy;
    flecs_command_mouse_emit(world, &evt, result->ptr);
    if (!ecs_is_alive(world, evt.target)) {
        return;
    }

    evt.delta_x = 0;
    evt.delta_y = 0;
    evt.buttons = held & ~((uint32_t)1 << evt.button);
    flecs_command_mouse_emit(world, &evt, result->ptr);
}

static void flecs_command_key(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    uintptr_t mode = (uintptr_t)ctx->ctx;
    const char *str = flecs_command_arg(argv, 0, char*);
    EcsScriptKeyboardEvent mods = {0};
    const char *key = flecs_command_parse_modifiers(str ? str : "", &mods);
    if (!key) {
        flecs_command_fail(ctx);
        return;
    }

    if (mode != 2) {
        flecs_command_key_emit(ctx->world, &mods, key, true, result->ptr);
    }

    if (mode != 1) {
        flecs_command_key_emit(ctx->world, &mods, key, false, result->ptr);
    }
}

static void flecs_command_type(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    const char *ptr = flecs_command_arg(argv, 0, char*);
    EcsScriptKeyboardEvent mods = {0};
    char key[8];

    while (ptr && ptr[0]) {
        int32_t len = 1;
        unsigned char ch = (unsigned char)ptr[0];
        if (ch >= 0xF0) {
            len = 4;
        } else if (ch >= 0xE0) {
            len = 3;
        } else if (ch >= 0xC0) {
            len = 2;
        }

        int32_t i;
        for (i = 1; i < len; i ++) {
            if (!ptr[i]) {
                len = i;
                break;
            }
        }

        if (ch == '\n') {
            ecs_os_strcpy(key, "Enter");
        } else if (ch == '\t') {
            ecs_os_strcpy(key, "Tab");
        } else {
            ecs_os_memcpy(key, ptr, len);
            key[len] = '\0';
        }

        flecs_command_key_emit(ctx->world, &mods, key, true, result->ptr);
        flecs_command_key_emit(ctx->world, &mods, key, false, result->ptr);
        ptr += len;
    }
}

static void flecs_command_mouse_event(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    const EcsScriptMouseEvent *evt = argv[0].ptr;
    if (evt->target && !flecs_command_check_alive(ctx, evt->target)) {
        return;
    }

    flecs_command_mouse_emit(ctx->world, evt, result->ptr);
}

static void flecs_command_keyboard_event(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    EcsScriptKeyboardEvent *evt = argv[0].ptr;
    ecs_script_event_result_t *r = result->ptr;
    *r = ecs_script_keyboard_event(ctx->world, evt);
    ecs_os_free(evt->key);
    evt->key = NULL;
}

static void flecs_command_listeners(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)argc;
    ecs_entity_t e = flecs_command_arg(argv, 0, ecs_entity_t);
    if (!flecs_command_check_alive(ctx, e)) {
        return;
    }

    const char *kinds[32];
    int32_t i, count = ecs_script_event_listeners(ctx->world, e, kinds, 32);
    if (count > 32) {
        count = 32;
    }

    for (i = 0; i < count; i ++) {
        ecs_vec_append_t(NULL, result->ptr, char*)[0] =
            ecs_os_strdup(kinds[i]);
    }
}

void flecs_command_event_import(
    ecs_world_t *world)
{
    ecs_entity_t string_t = ecs_id(ecs_string_t);
    ecs_entity_t entity_t = ecs_id(ecs_entity_t);
    ecs_entity_t f32_t = ecs_id(ecs_f32_t);
    ecs_entity_t event_result = ecs_id(ecs_script_event_result_t);
    const char *button_brief = "Button: [mod+]*(left|middle|right), where mod "
        "is ctrl, shift, alt or meta (default: left)";
    const char *key_brief = "Key: [mod+]*<KeyboardEvent.key>, where mod is "
        "ctrl, shift, alt or meta";

    ecs_entity_t string_vec = flecs_script_vector_type(world, string_t);

    flecs_command_desc_t commands[] = {
        { "focus", flecs_command_focus, entity_t, "Get or set the focus", {
            { "entity", entity_t, true, "Entity to focus, 0 to clear" }}},
        { "blur", flecs_command_blur, 0, "Clear the focus" },
        { "pointer", flecs_command_pointer,
            ecs_id(ecs_command_pointer_state_t),
            "Last mouse event and pressed entity" },
        { "move", flecs_command_move, event_result,
            "Move the pointer to an entity, keeps held buttons held", {
            { "entity", entity_t, false, "Entity under the pointer" },
            { "x", f32_t, true, "X position relative to entity" },
            { "y", f32_t, true, "Y position relative to entity" }}},
        { "move_by", flecs_command_move_by, event_result,
            "Move the pointer, becomes a drag when buttons are held", {
            { "dx", f32_t, false, "Horizontal movement" },
            { "dy", f32_t, false, "Vertical movement" }}},
        { "press", flecs_command_press, event_result, "Press a mouse button", {
            { "entity", entity_t, false, "Entity to press" },
            { "button", string_t, true, button_brief }}},
        { "release", flecs_command_release, event_result,
            "Release the held mouse buttons", {
            { "entity", entity_t, true,
                "Entity under the pointer (default: current target)" }}},
        { "click", flecs_command_click, event_result, "Click an entity", {
            { "entity", entity_t, false, "Entity to click" },
            { "button", string_t, true, button_brief }}},
        { "drag", flecs_command_drag, event_result, "Press, move and release", {
            { "entity", entity_t, false, "Entity to drag" },
            { "dx", f32_t, false, "Horizontal movement" },
            { "dy", f32_t, false, "Vertical movement" },
            { "button", string_t, true, button_brief }}},
        { "key", flecs_command_key, event_result, "Press and release a key", {
            { "key", string_t, false, key_brief }}},
        { "key_down", flecs_command_key, event_result, "Press a key", {
            { "key", string_t, false, key_brief }}, (void*)1 },
        { "key_up", flecs_command_key, event_result, "Release a key", {
            { "key", string_t, false, key_brief }}, (void*)2 },
        { "type", flecs_command_type, event_result,
            "Press and release a key for each character", {
            { "text", string_t, false, "Text to type" }}},
        { "mouse_event", flecs_command_mouse_event, event_result,
            "Emit a mouse event", {
            { "event", ecs_id(EcsScriptMouseEvent), false, "Mouse event" }}},
        { "keyboard_event", flecs_command_keyboard_event, event_result,
            "Emit a keyboard event", {
            { "event", ecs_id(EcsScriptKeyboardEvent), false,
                "Keyboard event" }}},
        { "listeners", flecs_command_listeners, string_vec,
            "Kinds of events that scripts wait for on an entity", {
            { "entity", entity_t, false, "Entity" }}},
    };

    flecs_command_register(world, commands,
        ECS_SIZEOF(commands) / ECS_SIZEOF(flecs_command_desc_t));
}

#endif
