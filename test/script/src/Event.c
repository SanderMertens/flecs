#include <script.h>

static bool ir_enabled = false;

void Event_setup(void) {
    const char *ir_param = test_param("ir");
    ir_enabled = ir_param && !strcmp(ir_param, "enabled");
}

/* Emit an event and propagate it to the parents of the target the way a host
 * would. Parents receive the coordinates offset by 100 per level so tests can
 * tell which coordinates a listener got. */
/* Mut variables of the Widget template of an instance */
typedef struct {
    int64_t count;
    float x, y, dx, dy;
    int64_t button;
    ecs_entity_t target;
} ev_Widget;

void Event_import(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    test_assert(ecs_lookup(world, "flecs.script.event") != 0);
    test_assert(ecs_lookup(world, "flecs.script.event.MouseEvent") ==
        ecs_id(EcsScriptMouseEvent));
    test_assert(ecs_lookup(world, "flecs.script.event.KeyboardEvent") ==
        ecs_id(EcsScriptKeyboardEvent));
    test_assert(ecs_lookup(world, "flecs.script.on.press") != 0);
    test_assert(ecs_lookup(world, "flecs.script.on.drag") != 0);
    test_assert(ecs_lookup(world, "flecs.script.on.release") != 0);
    test_assert(ecs_lookup(world, "flecs.script.on.click") != 0);
    test_assert(ecs_lookup(world, "flecs.script.on.move") != 0);
    test_assert(ecs_lookup(world, "flecs.script.on.key_down") != 0);
    test_assert(ecs_lookup(world, "flecs.script.on.key_up") != 0);

    ecs_fini(world);
}

void Event_press(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  mut dy: f32 = 0"
        LINE "  mut button = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.press(this)"
        LINE "      count = count + 1"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "      dx = e.delta_x"
        LINE "      dy = e.delta_y"
        LINE "      button = e.button"
        LINE "      target = e.target"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *widget = ecs_get_id(world, e, mut);
    test_assert(widget != NULL);
    test_int(widget->count, 0);

    EcsScriptMouseEvent evt = {
        .target = e, .screen_x = 10, .screen_y = 20,
        .local_x = 10, .local_y = 20, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);

    test_int(w->count, 1);
    test_flt(w->x, 10);
    test_flt(w->y, 20);
    test_int(w->button, 0);
    test_uint(w->target, e);

    ecs_fini(world);
}

void Event_press_other_target(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  mut dy: f32 = 0"
        LINE "  mut button = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.press(this)"
        LINE "      count = count + 1"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "      dx = e.delta_x"
        LINE "      dy = e.delta_y"
        LINE "      button = e.button"
        LINE "      target = e.target"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e" LINE "other {}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t other = ecs_lookup(world, "other");
    test_assert(e != 0);
    test_assert(other != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    EcsScriptMouseEvent evt = {
        .target = other, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    evt = (EcsScriptMouseEvent){
        .target = other, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);
    test_int(w->count, 0);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);
    test_int(w->count, 1);

    ecs_fini(world);
}

void Event_press_no_target(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  mut dy: f32 = 0"
        LINE "  mut button = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.press(this)"
        LINE "      count = count + 1"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "      dx = e.delta_x"
        LINE "      dy = e.delta_y"
        LINE "      button = e.button"
        LINE "      target = e.target"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    EcsScriptMouseEvent evt = {
        .target = 0, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    evt = (EcsScriptMouseEvent){
        .target = 0, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);
    test_int(w->count, 0);

    ecs_fini(world);
}

void Event_press_bubbles_to_parent(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  mut dy: f32 = 0"
        LINE "  mut button = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.press(this)"
        LINE "      count = count + 1"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "      dx = e.delta_x"
        LINE "      dy = e.delta_y"
        LINE "      button = e.button"
        LINE "      target = e.target"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e { child {} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(e != 0);
    test_assert(child != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    EcsScriptMouseEvent evt = {
        .target = child, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);

    test_int(w->count, 1);
    test_uint(w->target, e);

    ecs_fini(world);
}

void Event_press_two_listeners(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "using flecs.script"
            LINE "template Widget {"
            LINE "  mut a = 0"
            LINE "  mut b = 0"
            LINE "  async {"
            LINE "    await on.press(this)"
            LINE "    a = 1"
            LINE "  }"
            LINE "  async {"
            LINE "    await on.press(this)"
            LINE "    b = 1"
            LINE "  }"
            LINE "}"
            LINE "Widget e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 2);

    EcsScriptMouseEvent evt = {
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    const int64_t *m = ecs_get_id(world, e, ecs_lookup(world, "Widget.mut"));
    test_assert(m != NULL);
    test_int(m[0], 1);
    test_int(m[1], 1);

    ecs_fini(world);
}

void Event_press_two_widgets(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  mut dy: f32 = 0"
        LINE "  mut button = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.press(this)"
        LINE "      count = count + 1"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "      dx = e.delta_x"
        LINE "      dy = e.delta_y"
        LINE "      button = e.button"
        LINE "      target = e.target"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget a" LINE "Widget b",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");
    test_assert(a != 0);
    test_assert(b != 0);

    test_int(ecs_script_tasks_progress(world), 2);

    EcsScriptMouseEvent evt = {
        .target = b, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    evt = (EcsScriptMouseEvent){
        .target = b, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, a, mut);
    test_assert(w != NULL);
    test_int(w->count, 0);

    mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    w = ecs_get_id(world, b, mut);
    test_assert(w != NULL);
    test_int(w->count, 1);

    evt = (EcsScriptMouseEvent){
        .target = a, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    w = ecs_get_id(world, a, mut);
    test_assert(w != NULL);
    test_int(w->count, 1);

    mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    w = ecs_get_id(world, b, mut);
    test_assert(w != NULL);
    test_int(w->count, 1);

    ecs_fini(world);
}

void Event_press_while_other_button_held(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  mut dy: f32 = 0"
        LINE "  mut button = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.press(this)"
        LINE "      count = count + 1"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "      dx = e.delta_x"
        LINE "      dy = e.delta_y"
        LINE "      button = e.button"
        LINE "      target = e.target"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    EcsScriptMouseEvent evt = {
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 3
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);
    test_int(w->count, 2);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 2
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);
    test_int(w->count, 2);

    ecs_fini(world);
}

void Event_drag(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  mut dy: f32 = 0"
        LINE "  mut button = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.drag(this)"
        LINE "      count = count + 1"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "      dx = e.delta_x"
        LINE "      dy = e.delta_y"
        LINE "      button = e.button"
        LINE "      target = e.target"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e" LINE "other {}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t other = ecs_lookup(world, "other");
    test_assert(e != 0);
    test_assert(other != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    EcsScriptMouseEvent evt = {
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *widget = ecs_get_id(world, e, mut);
    test_assert(widget != NULL);
    test_int(widget->count, 1);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 5, .screen_y = 2,
        .local_x = 5, .local_y = 2, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);

    test_int(w->count, 2);
    test_flt(w->x, 5);
    test_flt(w->y, 2);
    test_flt(w->dx, 5);
    test_flt(w->dy, 2);

    /* A host without pointer capture reports the entity under the pointer,
     * which doesn't get the drag. A host with capture keeps reporting the
     * pressed entity. */

    evt = (EcsScriptMouseEvent){
        .target = other, .screen_x = 50, .screen_y = 50,
        .local_x = 50, .local_y = 50, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    widget = ecs_get_id(world, e, mut);
    test_assert(widget != NULL);
    test_int(widget->count, 2);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 50, .screen_y = 50,
        .local_x = 50, .local_y = 50, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    widget = ecs_get_id(world, e, mut);
    test_assert(widget != NULL);
    w = widget;

    test_int(w->count, 3);
    test_flt(w->x, 50);
    test_uint(w->target, e);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 50, .screen_y = 50,
        .local_x = 50, .local_y = 50, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    widget = ecs_get_id(world, e, mut);
    test_assert(widget != NULL);
    test_int(widget->count, 3);

    evt = (EcsScriptMouseEvent){
        .target = other, .screen_x = 60, .screen_y = 60,
        .local_x = 60, .local_y = 60, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    evt = (EcsScriptMouseEvent){
        .target = other, .screen_x = 70, .screen_y = 70,
        .local_x = 70, .local_y = 70, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    widget = ecs_get_id(world, e, mut);
    test_assert(widget != NULL);
    test_int(widget->count, 3);

    ecs_fini(world);
}

void Event_no_drag_without_button(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  mut dy: f32 = 0"
        LINE "  mut button = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.drag(this)"
        LINE "      count = count + 1"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "      dx = e.delta_x"
        LINE "      dy = e.delta_y"
        LINE "      button = e.button"
        LINE "      target = e.target"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    EcsScriptMouseEvent evt = {
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 5, .screen_y = 5,
        .local_x = 5, .local_y = 5, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);
    test_int(w->count, 0);

    ecs_fini(world);
}

void Event_release(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  mut dy: f32 = 0"
        LINE "  mut button = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.release(this)"
        LINE "      count = count + 1"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "      dx = e.delta_x"
        LINE "      dy = e.delta_y"
        LINE "      button = e.button"
        LINE "      target = e.target"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e" LINE "other {}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t other = ecs_lookup(world, "other");
    test_assert(e != 0);
    test_assert(other != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    EcsScriptMouseEvent evt = {
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 40, .screen_y = 40,
        .local_x = 40, .local_y = 40, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *widget = ecs_get_id(world, e, mut);
    test_assert(widget != NULL);
    test_int(widget->count, 0);

    /* Released over another entity: not delivered to the pressed entity */

    evt = (EcsScriptMouseEvent){
        .target = other, .screen_x = 40, .screen_y = 40,
        .local_x = 40, .local_y = 40, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    widget = ecs_get_id(world, e, mut);
    test_assert(widget != NULL);
    test_int(widget->count, 0);

    /* Released on the pressed entity (host with pointer capture) */

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 40, .screen_y = 40,
        .local_x = 40, .local_y = 40, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);

    test_int(w->count, 1);
    test_flt(w->x, 40);
    test_uint(w->target, e);

    ecs_fini(world);
}

void Event_click(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  mut dy: f32 = 0"
        LINE "  mut button = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.click(this)"
        LINE "      count = count + 1"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "      dx = e.delta_x"
        LINE "      dy = e.delta_y"
        LINE "      button = e.button"
        LINE "      target = e.target"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    EcsScriptMouseEvent evt = {
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);
    test_int(w->count, 0);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 2, .screen_y = 2,
        .local_x = 2, .local_y = 2, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);
    test_int(w->count, 1);

    ecs_fini(world);
}

void Event_no_click_when_released_elsewhere(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  mut dy: f32 = 0"
        LINE "  mut button = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.click(this)"
        LINE "      count = count + 1"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "      dx = e.delta_x"
        LINE "      dy = e.delta_y"
        LINE "      button = e.button"
        LINE "      target = e.target"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e" LINE "other {}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t other = ecs_lookup(world, "other");
    test_assert(e != 0);
    test_assert(other != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    EcsScriptMouseEvent evt = {
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    evt = (EcsScriptMouseEvent){
        .target = other, .screen_x = 40, .screen_y = 40,
        .local_x = 40, .local_y = 40, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);
    test_int(w->count, 0);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    evt = (EcsScriptMouseEvent){
        .target = 0, .screen_x = 40, .screen_y = 40,
        .local_x = 40, .local_y = 40, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);
    test_int(w->count, 0);

    ecs_fini(world);
}

void Event_move(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  mut dy: f32 = 0"
        LINE "  mut button = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.move(this)"
        LINE "      count = count + 1"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "      dx = e.delta_x"
        LINE "      dy = e.delta_y"
        LINE "      button = e.button"
        LINE "      target = e.target"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    EcsScriptMouseEvent evt = {
        .target = 0, .screen_x = 40, .screen_y = 40,
        .local_x = 40, .local_y = 40, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *widget = ecs_get_id(world, e, mut);
    test_assert(widget != NULL);
    test_int(widget->count, 0);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 1, .screen_y = 2,
        .local_x = 1, .local_y = 2, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);

    test_int(w->count, 1);
    test_flt(w->x, 1);
    test_flt(w->y, 2);
    test_flt(w->dx, -39);
    test_flt(w->dy, -38);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 1, .screen_y = 2,
        .local_x = 1, .local_y = 2, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 3, .screen_y = 4,
        .local_x = 3, .local_y = 4, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    widget = ecs_get_id(world, e, mut);
    test_assert(widget != NULL);
    test_int(widget->count, 1);

    ecs_fini(world);
}

void Event_delta_from_event(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  mut dy: f32 = 0"
        LINE "  mut button = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.press(this)"
        LINE "      count = count + 1"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "      dx = e.delta_x"
        LINE "      dy = e.delta_y"
        LINE "      button = e.button"
        LINE "      target = e.target"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = e, .local_x = 1, .local_y = 1,
        .delta_x = 3, .delta_y = 4, .buttons = 1
    });

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);

    test_flt(w->dx, 3);
    test_flt(w->dy, 4);

    ecs_fini(world);
}

void Event_button_and_modifiers(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "using flecs.script"
            LINE "template Widget {"
            LINE "  mut button = -1"
            LINE "  mut buttons = 0"
            LINE "  mut sx: f32 = 0"
            LINE "  mut sy: f32 = 0"
            LINE "  mut ctrl = false"
            LINE "  mut shift = false"
            LINE "  mut alt = false"
            LINE "  mut meta = false"
            LINE "  async {"
            LINE "    const e = await on.press(this)"
            LINE "    button = e.button"
            LINE "    buttons = e.buttons"
            LINE "    sx = e.screen_x"
            LINE "    sy = e.screen_y"
            LINE "    ctrl = e.ctrl"
            LINE "    shift = e.shift"
            LINE "    alt = e.alt"
            LINE "    meta = e.meta"
            LINE "  }"
            LINE "}"
            LINE "Widget e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = e, .screen_x = 640, .screen_y = 480,
        .button = 2, .buttons = 4, .ctrl = true, .shift = true, .meta = true
    });

    typedef struct {
        int64_t button, buttons;
        float sx, sy;
        bool ctrl, shift, alt, meta;
    } Mut;
    const Mut *m = ecs_get_id(world, e, ecs_lookup(world, "Widget.mut"));
    test_assert(m != NULL);
    test_int(m->button, 2);
    test_int(m->buttons, 4);
    test_flt(m->sx, 640);
    test_flt(m->sy, 480);
    test_bool(m->ctrl, true);
    test_bool(m->shift, true);
    test_bool(m->alt, false);
    test_bool(m->meta, true);

    ecs_fini(world);
}

void Event_key_down_up(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "using flecs.script"
            LINE "template Widget {"
            LINE "  mut downs = 0"
            LINE "  mut ups = 0"
            LINE "  mut key = \"\""
            LINE "  async {"
            LINE "    while true {"
            LINE "      const e = await on.key_down(this)"
            LINE "      downs = downs + 1"
            LINE "      key = e.key"
            LINE "    }"
            LINE "  }"
            LINE "  async {"
            LINE "    while true {"
            LINE "      await on.key_up(this)"
            LINE "      ups = ups + 1"
            LINE "    }"
            LINE "  }"
            LINE "}"
            LINE "Widget e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    typedef struct { int64_t downs, ups; char *key; } Mut;
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");

    test_int(ecs_script_tasks_progress(world), 2);

    /* No focus yet */

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "a"), .down = true
    });

    test_int(((const Mut*)ecs_get_id(world, e, mut))->downs, 0);

    EcsScriptMouseEvent evt = {
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "Enter"), .down = true
    });

    const Mut *m = ecs_get_id(world, e, mut);
    test_int(m->downs, 1);
    test_int(m->ups, 0);
    test_str(m->key, "Enter");

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "Enter"), .down = false
    });

    m = ecs_get_id(world, e, mut);
    test_int(m->downs, 1);
    test_int(m->ups, 1);

    ecs_fini(world);
}

void Event_key_modifiers(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "using flecs.script"
            LINE "template Widget {"
            LINE "  mut down = false"
            LINE "  mut repeat = false"
            LINE "  mut ctrl = false"
            LINE "  mut shift = false"
            LINE "  mut alt = false"
            LINE "  mut meta = false"
            LINE "  async {"
            LINE "    const e = await on.key_down(this)"
            LINE "    down = e.down"
            LINE "    repeat = e.repeat"
            LINE "    ctrl = e.ctrl"
            LINE "    shift = e.shift"
            LINE "    alt = e.alt"
            LINE "    meta = e.meta"
            LINE "  }"
            LINE "}"
            LINE "Widget e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    EcsScriptMouseEvent evt = {
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "x"), .down = true, .repeat = true,
        .alt = true, .meta = true
    });

    const bool *m = ecs_get_id(world, e, ecs_lookup(world, "Widget.mut"));
    test_assert(m != NULL);
    test_bool(m[0], true);
    test_bool(m[1], true);
    test_bool(m[2], false);
    test_bool(m[3], false);
    test_bool(m[4], true);
    test_bool(m[5], true);

    ecs_fini(world);
}

void Event_key_focus_follows_press(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "using flecs.script"
            LINE "template Widget {"
            LINE "  mut downs = 0"
            LINE "  async {"
            LINE "    while true {"
            LINE "      await on.key_down(this)"
            LINE "      downs = downs + 1"
            LINE "    }"
            LINE "  }"
            LINE "}"
            LINE "Widget a"
            LINE "Widget b",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(a != 0);
    test_assert(b != 0);

    test_int(ecs_script_tasks_progress(world), 2);

    EcsScriptMouseEvent evt = {
        .target = a, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    evt = (EcsScriptMouseEvent){
        .target = a, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "a"), .down = true
    });

    test_int(*(int64_t*)ecs_get_id(world, a, mut), 1);
    test_int(*(int64_t*)ecs_get_id(world, b, mut), 0);

    evt = (EcsScriptMouseEvent){
        .target = b, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    evt = (EcsScriptMouseEvent){
        .target = b, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "b"), .down = true
    });

    test_int(*(int64_t*)ecs_get_id(world, a, mut), 1);
    test_int(*(int64_t*)ecs_get_id(world, b, mut), 1);

    /* Pressing on nothing clears the focus */

    evt = (EcsScriptMouseEvent){
        .target = 0, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    evt = (EcsScriptMouseEvent){
        .target = 0, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "c"), .down = true
    });

    test_int(*(int64_t*)ecs_get_id(world, a, mut), 1);
    test_int(*(int64_t*)ecs_get_id(world, b, mut), 1);

    ecs_fini(world);
}

void Event_key_bubbles_to_parent(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "using flecs.script"
            LINE "template Widget {"
            LINE "  mut downs = 0"
            LINE "  async {"
            LINE "    await on.key_down(this)"
            LINE "    downs = downs + 1"
            LINE "  }"
            LINE "}"
            LINE "Widget e { child {} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(e != 0);
    test_assert(child != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    EcsScriptMouseEvent evt = {
        .target = child, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "a"), .down = true
    });

    test_int(*(int64_t*)ecs_get_id(world, e, ecs_lookup(world, "Widget.mut")), 1);

    ecs_fini(world);
}

void Event_slider(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "using flecs.script"
            LINE "struct Position(x: f32, y: f32)"
            LINE "struct Rect(width: f32, height: f32)"
            LINE "template Slider {"
            LINE "  prop max: f32 = 100"
            LINE "  prop width = 200"
            LINE "  mut value: f32 = 0"
            LINE "  const fillWidth = width * value / max"
            LINE "  bg { Position: {}; Rect: {width, 20} }"
            LINE "  fill { Position: {x: (fillWidth - width) / 2}; Rect: {fillWidth, 4} }"
            LINE "  async {"
            LINE "    while true {"
            LINE "      const e = await on.drag(this)"
            LINE "      value = (e.local_x + width / 2) / width * max"
            LINE "    }"
            LINE "  }"
            LINE "}"
            LINE "Slider slider",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t slider = ecs_lookup(world, "slider");
    ecs_entity_t bg = ecs_lookup(world, "slider.bg");
    ecs_entity_t fill = ecs_lookup(world, "slider.fill");
    ecs_entity_t mut = ecs_lookup(world, "Slider.mut");
    ecs_entity_t rect = ecs_lookup(world, "Rect");
    test_assert(slider != 0);
    test_assert(bg != 0);
    test_assert(fill != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    EcsScriptMouseEvent evt = {
        .target = bg, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    test_flt(*(float*)ecs_get_id(world, slider, mut), 50);
    test_flt(*(float*)ecs_get_id(world, fill, rect), 100);

    evt = (EcsScriptMouseEvent){
        .target = bg, .screen_x = 50, .screen_y = 0,
        .local_x = 50, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    test_flt(*(float*)ecs_get_id(world, slider, mut), 75);
    test_flt(*(float*)ecs_get_id(world, fill, rect), 150);

    ecs_fini(world);
}

void Event_wrong_arg_fails(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "using flecs.script"
        LINE "async {"
        LINE "  await on.press(\"not an entity\")"
        LINE "}", &(ecs_script_eval_desc_t){ .ir = ir_enabled }, NULL) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void Event_delete_listener(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  mut dy: f32 = 0"
        LINE "  mut button = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.press(this)"
        LINE "      count = count + 1"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "      dx = e.delta_x"
        LINE "      dy = e.delta_y"
        LINE "      button = e.button"
        LINE "      target = e.target"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    ecs_delete(world, e);

    EcsScriptMouseEvent evt = {
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "a"), .down = true
    });

    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void Event_delete_listener_while_pressed(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  mut dy: f32 = 0"
        LINE "  mut button = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.drag(this)"
        LINE "      count = count + 1"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "      dx = e.delta_x"
        LINE "      dy = e.delta_y"
        LINE "      button = e.button"
        LINE "      target = e.target"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    EcsScriptMouseEvent evt = {
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ecs_delete(world, e);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 1, .screen_y = 1,
        .local_x = 1, .local_y = 1, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 1, .screen_y = 1,
        .local_x = 1, .local_y = 1, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "a"), .down = true
    });

    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void Event_fini_w_pending_listener(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  mut dy: f32 = 0"
        LINE "  mut button = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.press(this)"
        LINE "      count = count + 1"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "      dx = e.delta_x"
        LINE "      dy = e.delta_y"
        LINE "      button = e.button"
        LINE "      target = e.target"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    test_int(ecs_script_tasks_progress(world), 1);

    ecs_fini(world);
}

void Event_script_update_w_pending_listener(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  mut dy: f32 = 0"
        LINE "  mut button = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.press(this)"
        LINE "      count = count + 1"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "      dx = e.delta_x"
        LINE "      dy = e.delta_y"
        LINE "      button = e.button"
        LINE "      target = e.target"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    test_int(ecs_script_tasks_progress(world), 1);

    test_int(ecs_script_update(world, s, 0,
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  mut dy: f32 = 0"
        LINE "  mut button = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.press(this)"
        LINE "      count = count + 1"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "      dx = e.delta_x"
        LINE "      dy = e.delta_y"
        LINE "      button = e.button"
        LINE "      target = e.target"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget f"), 0);
    ecs_entity_t f = ecs_lookup(world, "f");
    test_assert(f != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    EcsScriptMouseEvent evt = {
        .target = f, .screen_x = 3, .screen_y = 4,
        .local_x = 3, .local_y = 4, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, f, mut);
    test_assert(w != NULL);

    test_int(w->count, 1);
    test_flt(w->x, 3);

    ecs_fini(world);
}

void Event_mouse_event_w_stage(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  mut dy: f32 = 0"
        LINE "  mut button = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.press(this)"
        LINE "      count = count + 1"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "      dx = e.delta_x"
        LINE "      dy = e.delta_y"
        LINE "      button = e.button"
        LINE "      target = e.target"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    EcsScriptMouseEvent evt = {
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(ecs_get_stage(world, 0), &evt) && evt.target &&
        ecs_is_alive(ecs_get_stage(world, 0), evt.target))
    {
        evt.target = ecs_get_target(ecs_get_stage(world, 0), evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);
    test_int(w->count, 1);

    ecs_fini(world);
}

void Event_mouse_event_in_progress(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  mut dy: f32 = 0"
        LINE "  mut button = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.press(this)"
        LINE "      count = count + 1"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "      dx = e.delta_x"
        LINE "      dy = e.delta_y"
        LINE "      button = e.button"
        LINE "      target = e.target"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_progress(world, 0);

    EcsScriptMouseEvent evt = {
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);
    test_int(w->count, 1);

    ecs_progress(world, 0);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);
    test_int(w->count, 2);

    ecs_fini(world);
}

void Event_import_enter_leave(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    test_assert(ecs_lookup(world, "flecs.script.on.enter") != 0);
    test_assert(ecs_lookup(world, "flecs.script.on.leave") != 0);

    ecs_fini(world);
}

void Event_enter_leave(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "using flecs.script"
            LINE "template Widget {"
            LINE "  mut enters = 0"
            LINE "  mut leaves = 0"
            LINE "  async {"
            LINE "    while true {"
            LINE "      await on.enter(this)"
            LINE "      enters = enters + 1"
            LINE "    }"
            LINE "  }"
            LINE "  async {"
            LINE "    while true {"
            LINE "      await on.leave(this)"
            LINE "      leaves = leaves + 1"
            LINE "    }"
            LINE "  }"
            LINE "}"
            LINE "Widget e"
            LINE "other {}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t other = ecs_lookup(world, "other");
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(e != 0);
    test_assert(other != 0);

    test_int(ecs_script_tasks_progress(world), 2);
    const int64_t *m = ecs_get_id(world, e, mut);

    EcsScriptMouseEvent evt = {
        .target = 0, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    test_int(m[0], 0);
    test_int(m[1], 0);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 1, .screen_y = 1,
        .local_x = 1, .local_y = 1, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 0);

    /* Staying on the entity doesn't enter again */

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 2, .screen_y = 2,
        .local_x = 2, .local_y = 2, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 0);

    evt = (EcsScriptMouseEvent){
        .target = other, .screen_x = 3, .screen_y = 3,
        .local_x = 3, .local_y = 3, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 1);

    evt = (EcsScriptMouseEvent){
        .target = 0, .screen_x = 4, .screen_y = 4,
        .local_x = 4, .local_y = 4, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 1);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 5, .screen_y = 5,
        .local_x = 5, .local_y = 5, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    evt = (EcsScriptMouseEvent){
        .target = 0, .screen_x = 6, .screen_y = 6,
        .local_x = 6, .local_y = 6, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    m = ecs_get_id(world, e, mut);
    test_int(m[0], 2);
    test_int(m[1], 2);

    ecs_fini(world);
}

void Event_enter_leave_between_children(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "using flecs.script"
            LINE "template Widget {"
            LINE "  mut enters = 0"
            LINE "  mut leaves = 0"
            LINE "  async {"
            LINE "    while true {"
            LINE "      await on.enter(this)"
            LINE "      enters = enters + 1"
            LINE "    }"
            LINE "  }"
            LINE "  async {"
            LINE "    while true {"
            LINE "      await on.leave(this)"
            LINE "      leaves = leaves + 1"
            LINE "    }"
            LINE "  }"
            LINE "}"
            LINE "Widget e { a {}; b {} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t a = ecs_lookup(world, "e.a");
    ecs_entity_t b = ecs_lookup(world, "e.b");
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(e != 0);
    test_assert(a != 0);
    test_assert(b != 0);

    test_int(ecs_script_tasks_progress(world), 2);

    EcsScriptMouseEvent evt = {
        .target = a, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    const int64_t *m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 0);

    /* Moving from one child to another stays inside the widget */

    evt = (EcsScriptMouseEvent){
        .target = b, .screen_x = 1, .screen_y = 1,
        .local_x = 1, .local_y = 1, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 0);

    /* Moving from a child to the widget itself stays inside too */

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 2, .screen_y = 2,
        .local_x = 2, .local_y = 2, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 0);

    evt = (EcsScriptMouseEvent){
        .target = 0, .screen_x = 3, .screen_y = 3,
        .local_x = 3, .local_y = 3, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 1);

    ecs_fini(world);
}

void Event_enter_leave_children_listen(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "using flecs.script"
            LINE "template Widget {"
            LINE "  mut enters = 0"
            LINE "  mut leaves = 0"
            LINE "  async {"
            LINE "    while true {"
            LINE "      await on.enter(this)"
            LINE "      enters = enters + 1"
            LINE "    }"
            LINE "  }"
            LINE "  async {"
            LINE "    while true {"
            LINE "      await on.leave(this)"
            LINE "      leaves = leaves + 1"
            LINE "    }"
            LINE "  }"
            LINE "}"
            LINE "parent { Widget a; Widget b }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t a = ecs_lookup(world, "parent.a");
    ecs_entity_t b = ecs_lookup(world, "parent.b");
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(a != 0);
    test_assert(b != 0);

    test_int(ecs_script_tasks_progress(world), 4);

    EcsScriptMouseEvent evt = {
        .target = a, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    const int64_t *ma = ecs_get_id(world, a, mut);
    const int64_t *mb = ecs_get_id(world, b, mut);
    test_int(ma[0], 1);
    test_int(ma[1], 0);
    test_int(mb[0], 0);
    test_int(mb[1], 0);

    evt = (EcsScriptMouseEvent){
        .target = b, .screen_x = 1, .screen_y = 1,
        .local_x = 1, .local_y = 1, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ma = ecs_get_id(world, a, mut);
    mb = ecs_get_id(world, b, mut);
    test_int(ma[0], 1);
    test_int(ma[1], 1);
    test_int(mb[0], 1);
    test_int(mb[1], 0);

    ecs_fini(world);
}

void Event_enter_leave_while_dragging(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "using flecs.script"
            LINE "template Widget {"
            LINE "  mut enters = 0"
            LINE "  mut leaves = 0"
            LINE "  mut drags = 0"
            LINE "  async {"
            LINE "    while true {"
            LINE "      await on.enter(this)"
            LINE "      enters = enters + 1"
            LINE "    }"
            LINE "  }"
            LINE "  async {"
            LINE "    while true {"
            LINE "      await on.leave(this)"
            LINE "      leaves = leaves + 1"
            LINE "    }"
            LINE "  }"
            LINE "  async {"
            LINE "    while true {"
            LINE "      await on.drag(this)"
            LINE "      drags = drags + 1"
            LINE "    }"
            LINE "  }"
            LINE "}"
            LINE "Widget e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 3);

    EcsScriptMouseEvent evt = {
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    const int64_t *m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 0);
    test_int(m[2], 1);

    /* Dragging off the entity leaves it; without pointer capture the host
     * reports no target, so no drag is delivered */

    evt = (EcsScriptMouseEvent){
        .target = 0, .screen_x = 10, .screen_y = 10,
        .local_x = 10, .local_y = 10, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 1);
    test_int(m[2], 1);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    m = ecs_get_id(world, e, mut);
    test_int(m[0], 2);
    test_int(m[1], 1);
    test_int(m[2], 2);

    ecs_fini(world);
}

void Event_leave_deleted_hover_target(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "using flecs.script"
            LINE "template Widget {"
            LINE "  mut enters = 0"
            LINE "  async {"
            LINE "    while true {"
            LINE "      await on.enter(this)"
            LINE "      enters = enters + 1"
            LINE "    }"
            LINE "  }"
            LINE "}"
            LINE "Widget a"
            LINE "Widget b",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(a != 0);
    test_assert(b != 0);

    test_int(ecs_script_tasks_progress(world), 2);

    EcsScriptMouseEvent evt = {
        .target = a, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ecs_delete(world, a);

    evt = (EcsScriptMouseEvent){
        .target = b, .screen_x = 1, .screen_y = 1,
        .local_x = 1, .local_y = 1, .buttons = 0
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    test_int(*(int64_t*)ecs_get_id(world, b, mut), 1);

    ecs_fini(world);
}

void Event_returns_true(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  mut dy: f32 = 0"
        LINE "  mut button = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.press(this)"
        LINE "      count = count + 1"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "      dx = e.delta_x"
        LINE "      dy = e.delta_y"
        LINE "      button = e.button"
        LINE "      target = e.target"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_bool(ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = e, .buttons = 1 }), true);
    test_bool(ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = 0, .buttons = 0 }), true);

    ecs_fini(world);
}

void Event_no_propagation_without_host(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  mut dy: f32 = 0"
        LINE "  mut button = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.press(this)"
        LINE "      count = count + 1"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "      dx = e.delta_x"
        LINE "      dy = e.delta_y"
        LINE "      button = e.button"
        LINE "      target = e.target"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e { child {} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(e != 0);
    test_assert(child != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    /* The host only emits for the child: the parent doesn't get the event */
    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = child, .buttons = 1 });

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);
    test_int(w->count, 0);

    ecs_fini(world);
}

void Event_propagation_translates_coordinates(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  mut dy: f32 = 0"
        LINE "  mut button = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.press(this)"
        LINE "      count = count + 1"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "      dx = e.delta_x"
        LINE "      dy = e.delta_y"
        LINE "      button = e.button"
        LINE "      target = e.target"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e { child {} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(e != 0);
    test_assert(child != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    EcsScriptMouseEvent evt = {
        .target = child, .screen_x = 2, .screen_y = 3,
        .local_x = 2, .local_y = 3, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        evt.local_x += 100;
        evt.local_y += 100;
        if (!evt.target) {
            break;
        }
    }

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);

    test_int(w->count, 1);
    test_flt(w->x, 102);
    test_flt(w->y, 103);
    test_uint(w->target, e);

    ecs_fini(world);
}

void Event_propagation_keeps_deltas(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  mut dy: f32 = 0"
        LINE "  mut button = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.drag(this)"
        LINE "      count = count + 1"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "      dx = e.delta_x"
        LINE "      dy = e.delta_y"
        LINE "      button = e.button"
        LINE "      target = e.target"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e { child {} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(e != 0);
    test_assert(child != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    EcsScriptMouseEvent evt = {
        .target = child, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        evt.local_x += 100;
        evt.local_y += 100;
        if (!evt.target) {
            break;
        }
    }

    evt = (EcsScriptMouseEvent){
        .target = child, .screen_x = 5, .screen_y = 2,
        .local_x = 5, .local_y = 2, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        evt.local_x += 100;
        evt.local_y += 100;
        if (!evt.target) {
            break;
        }
    }

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);

    test_int(w->count, 2);
    test_flt(w->x, 105);
    test_flt(w->dx, 5);
    test_flt(w->dy, 2);

    ecs_fini(world);
}

void Event_propagation_press_once_per_level(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  mut dy: f32 = 0"
        LINE "  mut button = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.press(this)"
        LINE "      count = count + 1"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "      dx = e.delta_x"
        LINE "      dy = e.delta_y"
        LINE "      button = e.button"
        LINE "      target = e.target"
        LINE "    }"
        LINE "  }"
        LINE "}"
            LINE "Widget e { Widget child { Widget grandchild {} } }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    ecs_entity_t grandchild = ecs_lookup(world, "e.child.grandchild");
    test_assert(e != 0);
    test_assert(child != 0);
    test_assert(grandchild != 0);

    test_int(ecs_script_tasks_progress(world), 3);

    EcsScriptMouseEvent evt = {
        .target = grandchild, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);
    test_int(w->count, 1);

    mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    w = ecs_get_id(world, child, mut);
    test_assert(w != NULL);
    test_int(w->count, 1);

    mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    w = ecs_get_id(world, grandchild, mut);
    test_assert(w != NULL);
    test_int(w->count, 1);

    /* Same pointer state on the same target again is a new event, not a
     * propagation, so nothing is pressed again */

    evt = (EcsScriptMouseEvent){
        .target = grandchild, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);
    test_int(w->count, 1);

    mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    w = ecs_get_id(world, child, mut);
    test_assert(w != NULL);
    test_int(w->count, 1);

    mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    w = ecs_get_id(world, grandchild, mut);
    test_assert(w != NULL);
    test_int(w->count, 1);

    ecs_fini(world);
}

void Event_drag_not_propagated_to_unrelated_target(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut dx: f32 = 0"
        LINE "  mut dy: f32 = 0"
        LINE "  mut button = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.drag(this)"
        LINE "      count = count + 1"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "      dx = e.delta_x"
        LINE "      dy = e.delta_y"
        LINE "      button = e.button"
        LINE "      target = e.target"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget a" LINE "Widget b",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");
    test_assert(a != 0);
    test_assert(b != 0);

    test_int(ecs_script_tasks_progress(world), 2);

    EcsScriptMouseEvent evt = {
        .target = a, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    /* A host without pointer capture reports the entity under the pointer */

    evt = (EcsScriptMouseEvent){
        .target = b, .screen_x = 5, .screen_y = 5,
        .local_x = 5, .local_y = 5, .buttons = 1
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        if (!evt.target) {
            break;
        }
    }

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, a, mut);
    test_assert(w != NULL);
    test_int(w->count, 1);

    mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    w = ecs_get_id(world, b, mut);
    test_assert(w != NULL);
    test_int(w->count, 0);

    ecs_fini(world);
}
