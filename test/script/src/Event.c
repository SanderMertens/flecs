#include <script.h>

static bool ir_enabled = false;

void Event_setup(void) {
    const char *ir_param = test_param("ir");
    ir_enabled = ir_param && !strcmp(ir_param, "enabled");
}

/* Mut variables of the Widget template of an instance */
typedef struct {
    int64_t count;
    float x, y, dx, dy;
    int64_t button;
    ecs_entity_t target;
} ev_Widget;

static int32_t to_local_invoked = 0;
static ecs_entity_t to_local_entity = 0;

static void Event_to_local(
    ecs_world_t *world,
    ecs_entity_t entity,
    float screen_x,
    float screen_y,
    float *local_x,
    float *local_y,
    void *ctx)
{
    (void)world;
    (void)ctx;
    to_local_invoked ++;
    to_local_entity = entity;
    *local_x = screen_x + 100;
    *local_y = screen_y + 100;
}

typedef struct {
    int64_t focus_count;
    int64_t blur_count;
    ecs_entity_t focus_target;
    ecs_entity_t focus_related;
    ecs_entity_t blur_target;
    ecs_entity_t blur_related;
} ev_Focus;

static int32_t to_focus_invoked = 0;
static ecs_entity_t to_focus_entity = 0;
static ecs_entity_t to_focus_current = 0;
static ecs_script_focus_source_t to_focus_source = EcsScriptFocusPress;

static ecs_entity_t Event_to_focus(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t current,
    ecs_script_focus_source_t source,
    void *ctx)
{
    (void)world;
    to_focus_invoked ++;
    to_focus_entity = entity;
    to_focus_current = current;
    to_focus_source = source;
    if (ctx) {
        return *(ecs_entity_t*)ctx;
    }
    return entity;
}

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
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    evt = (EcsScriptMouseEvent){
        .target = other, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 0
    };
    ecs_script_mouse_event(world, &evt);

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);
    test_int(w->count, 0);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    evt = (EcsScriptMouseEvent){
        .target = 0, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 0
    };
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);

    test_int(w->count, 1);
    test_uint(w->target, child);

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
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    evt = (EcsScriptMouseEvent){
        .target = b, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 0
    };
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 3
    };
    ecs_script_mouse_event(world, &evt);

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);
    test_int(w->count, 2);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 2
    };
    ecs_script_mouse_event(world, &evt);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 0
    };
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *widget = ecs_get_id(world, e, mut);
    test_assert(widget != NULL);
    test_int(widget->count, 1);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 5, .screen_y = 2,
        .local_x = 5, .local_y = 2, .buttons = 1
    };
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    widget = ecs_get_id(world, e, mut);
    test_assert(widget != NULL);
    test_int(widget->count, 2);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 50, .screen_y = 50,
        .local_x = 50, .local_y = 50, .buttons = 1
    };
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    widget = ecs_get_id(world, e, mut);
    test_assert(widget != NULL);
    test_int(widget->count, 3);

    evt = (EcsScriptMouseEvent){
        .target = other, .screen_x = 60, .screen_y = 60,
        .local_x = 60, .local_y = 60, .buttons = 1
    };
    ecs_script_mouse_event(world, &evt);

    evt = (EcsScriptMouseEvent){
        .target = other, .screen_x = 70, .screen_y = 70,
        .local_x = 70, .local_y = 70, .buttons = 1
    };
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 5, .screen_y = 5,
        .local_x = 5, .local_y = 5, .buttons = 0
    };
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 40, .screen_y = 40,
        .local_x = 40, .local_y = 40, .buttons = 1
    };
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 40, .screen_y = 40,
        .local_x = 40, .local_y = 40, .buttons = 0
    };
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);
    test_int(w->count, 0);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 2, .screen_y = 2,
        .local_x = 2, .local_y = 2, .buttons = 0
    };
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    evt = (EcsScriptMouseEvent){
        .target = other, .screen_x = 40, .screen_y = 40,
        .local_x = 40, .local_y = 40, .buttons = 0
    };
    ecs_script_mouse_event(world, &evt);

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);
    test_int(w->count, 0);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    ecs_script_mouse_event(world, &evt);

    evt = (EcsScriptMouseEvent){
        .target = 0, .screen_x = 40, .screen_y = 40,
        .local_x = 40, .local_y = 40, .buttons = 0
    };
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *widget = ecs_get_id(world, e, mut);
    test_assert(widget != NULL);
    test_int(widget->count, 0);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 1, .screen_y = 2,
        .local_x = 1, .local_y = 2, .buttons = 0
    };
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 3, .screen_y = 4,
        .local_x = 3, .local_y = 4, .buttons = 1
    };
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 0
    };
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    evt = (EcsScriptMouseEvent){
        .target = a, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 0
    };
    ecs_script_mouse_event(world, &evt);

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "a"), .down = true
    });

    test_int(*(int64_t*)ecs_get_id(world, a, mut), 1);
    test_int(*(int64_t*)ecs_get_id(world, b, mut), 0);

    evt = (EcsScriptMouseEvent){
        .target = b, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    ecs_script_mouse_event(world, &evt);

    evt = (EcsScriptMouseEvent){
        .target = b, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 0
    };
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    evt = (EcsScriptMouseEvent){
        .target = 0, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 0
    };
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    test_flt(*(float*)ecs_get_id(world, slider, mut), 50);
    test_flt(*(float*)ecs_get_id(world, fill, rect), 100);

    evt = (EcsScriptMouseEvent){
        .target = bg, .screen_x = 50, .screen_y = 0,
        .local_x = 50, .local_y = 0, .buttons = 1
    };
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 0
    };
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    ecs_delete(world, e);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 1, .screen_y = 1,
        .local_x = 1, .local_y = 1, .buttons = 1
    };
    ecs_script_mouse_event(world, &evt);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 1, .screen_y = 1,
        .local_x = 1, .local_y = 1, .buttons = 0
    };
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(ecs_get_stage(world, 0), &evt);

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
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    test_int(m[0], 0);
    test_int(m[1], 0);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 1, .screen_y = 1,
        .local_x = 1, .local_y = 1, .buttons = 0
    };
    ecs_script_mouse_event(world, &evt);

    m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 0);

    /* Staying on the entity doesn't enter again */

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 2, .screen_y = 2,
        .local_x = 2, .local_y = 2, .buttons = 0
    };
    ecs_script_mouse_event(world, &evt);

    m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 0);

    evt = (EcsScriptMouseEvent){
        .target = other, .screen_x = 3, .screen_y = 3,
        .local_x = 3, .local_y = 3, .buttons = 0
    };
    ecs_script_mouse_event(world, &evt);

    m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 1);

    evt = (EcsScriptMouseEvent){
        .target = 0, .screen_x = 4, .screen_y = 4,
        .local_x = 4, .local_y = 4, .buttons = 0
    };
    ecs_script_mouse_event(world, &evt);

    m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 1);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 5, .screen_y = 5,
        .local_x = 5, .local_y = 5, .buttons = 0
    };
    ecs_script_mouse_event(world, &evt);

    evt = (EcsScriptMouseEvent){
        .target = 0, .screen_x = 6, .screen_y = 6,
        .local_x = 6, .local_y = 6, .buttons = 0
    };
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    const int64_t *m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 0);

    /* Moving from one child to another stays inside the widget */

    evt = (EcsScriptMouseEvent){
        .target = b, .screen_x = 1, .screen_y = 1,
        .local_x = 1, .local_y = 1, .buttons = 0
    };
    ecs_script_mouse_event(world, &evt);

    m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 0);

    /* Moving from a child to the widget itself stays inside too */

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 2, .screen_y = 2,
        .local_x = 2, .local_y = 2, .buttons = 0
    };
    ecs_script_mouse_event(world, &evt);

    m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 0);

    evt = (EcsScriptMouseEvent){
        .target = 0, .screen_x = 3, .screen_y = 3,
        .local_x = 3, .local_y = 3, .buttons = 0
    };
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 1);
    test_int(m[2], 1);

    evt = (EcsScriptMouseEvent){
        .target = e, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    ecs_delete(world, a);

    evt = (EcsScriptMouseEvent){
        .target = b, .screen_x = 1, .screen_y = 1,
        .local_x = 1, .local_y = 1, .buttons = 0
    };
    ecs_script_mouse_event(world, &evt);

    test_int(*(int64_t*)ecs_get_id(world, b, mut), 1);

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

    to_local_invoked = 0;
    to_local_entity = 0;
    ecs_script_set_event_hooks(world, &(ecs_script_event_hooks_t){
        .to_local = Event_to_local
    });

    EcsScriptMouseEvent evt = {
        .target = child, .screen_x = 2, .screen_y = 3,
        .local_x = 1, .local_y = 1, .buttons = 1
    };
    ecs_script_mouse_event(world, &evt);

    test_int(to_local_invoked, 1);
    test_uint(to_local_entity, e);

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);

    test_int(w->count, 1);
    test_flt(w->x, 102);
    test_flt(w->y, 103);
    test_uint(w->target, child);

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

    ecs_script_set_event_hooks(world, &(ecs_script_event_hooks_t){
        .to_local = Event_to_local
    });

    EcsScriptMouseEvent evt = {
        .target = child, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    ecs_script_mouse_event(world, &evt);

    evt = (EcsScriptMouseEvent){
        .target = child, .screen_x = 5, .screen_y = 2,
        .local_x = 5, .local_y = 2, .buttons = 1
    };
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

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

    evt = (EcsScriptMouseEvent){
        .target = grandchild, .screen_x = 0, .screen_y = 0,
        .local_x = 0, .local_y = 0, .buttons = 1
    };
    ecs_script_mouse_event(world, &evt);

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
    ecs_script_mouse_event(world, &evt);

    /* A host without pointer capture reports the entity under the pointer */

    evt = (EcsScriptMouseEvent){
        .target = b, .screen_x = 5, .screen_y = 5,
        .local_x = 5, .local_y = 5, .buttons = 1
    };
    ecs_script_mouse_event(world, &evt);

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

void Event_propagation_screen_coords_without_to_local(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.press(this)"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e { Widget child {} }",
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

    test_int(ecs_script_tasks_progress(world), 2);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = child, .screen_x = 10, .screen_y = 20,
        .local_x = 1, .local_y = 2, .buttons = 1
    });

    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);

    const float *child_xy = ecs_get_id(world, child, mut);
    test_assert(child_xy != NULL);
    test_flt(child_xy[0], 1);
    test_flt(child_xy[1], 2);

    const float *e_xy = ecs_get_id(world, e, mut);
    test_assert(e_xy != NULL);
    test_flt(e_xy[0], 10);
    test_flt(e_xy[1], 20);

    ecs_fini(world);
}

void Event_stop_propagation_press(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      await on.press(this)"
        LINE "      count = count + 1"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "template Stopper {"
        LINE "  mut count = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.press(this)"
        LINE "      count = count + 1"
        LINE "      await e.stop_propagation()"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e { Stopper child { Widget grandchild {} } }",
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

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = grandchild, .buttons = 1 });

    ecs_entity_t widget_mut = ecs_lookup(world, "Widget.mut");
    ecs_entity_t stopper_mut = ecs_lookup(world, "Stopper.mut");
    test_assert(widget_mut != 0);
    test_assert(stopper_mut != 0);

    test_int(*(const int64_t*)ecs_get_id(world, grandchild, widget_mut), 1);
    test_int(*(const int64_t*)ecs_get_id(world, child, stopper_mut), 1);
    test_int(*(const int64_t*)ecs_get_id(world, e, widget_mut), 0);

    ecs_fini(world);
}

void Event_stop_propagation_same_level(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      await on.press(this)"
        LINE "      count = count + 1"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "template Stopper {"
        LINE "  mut count = 0"
        LINE "  mut other = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.press(this)"
        LINE "      count = count + 1"
        LINE "      await e.stop_propagation()"
        LINE "    }"
        LINE "  }"
        LINE "  async {"
        LINE "    while true {"
        LINE "      await on.press(this)"
        LINE "      other = other + 1"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e { Stopper child {} }",
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

    test_int(ecs_script_tasks_progress(world), 3);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = child, .buttons = 1 });

    ecs_entity_t widget_mut = ecs_lookup(world, "Widget.mut");
    ecs_entity_t stopper_mut = ecs_lookup(world, "Stopper.mut");
    test_assert(widget_mut != 0);
    test_assert(stopper_mut != 0);

    const int64_t *stopper = ecs_get_id(world, child, stopper_mut);
    test_assert(stopper != NULL);
    test_int(stopper[0], 1);
    test_int(stopper[1], 1);
    test_int(*(const int64_t*)ecs_get_id(world, e, widget_mut), 0);

    ecs_fini(world);
}

void Event_stop_propagation_once(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      await on.press(this)"
        LINE "      count = count + 1"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "template Stopper {"
        LINE "  mut count = 0"
        LINE "  async {"
        LINE "    const e = await on.press(this)"
        LINE "    count = count + 1"
        LINE "    await e.stop_propagation()"
        LINE "    while true {"
        LINE "      await on.press(this)"
        LINE "      count = count + 1"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e { Stopper child {} }",
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

    test_int(ecs_script_tasks_progress(world), 2);

    ecs_entity_t widget_mut = ecs_lookup(world, "Widget.mut");
    ecs_entity_t stopper_mut = ecs_lookup(world, "Stopper.mut");
    test_assert(widget_mut != 0);
    test_assert(stopper_mut != 0);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = child, .buttons = 1 });
    test_int(*(const int64_t*)ecs_get_id(world, child, stopper_mut), 1);
    test_int(*(const int64_t*)ecs_get_id(world, e, widget_mut), 0);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = child, .buttons = 0 });
    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = child, .buttons = 1 });
    test_int(*(const int64_t*)ecs_get_id(world, child, stopper_mut), 2);
    test_int(*(const int64_t*)ecs_get_id(world, e, widget_mut), 1);

    ecs_fini(world);
}

void Event_stop_propagation_result(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut inside = false"
        LINE "  mut stale = true"
        LINE "  async {"
        LINE "    const e = await on.press(this)"
        LINE "    const a = await e.stop_propagation()"
        LINE "    inside = a"
        LINE "    await on.release(this)"
        LINE "    const b = await e.stop_propagation()"
        LINE "    stale = b"
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

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = e, .buttons = 1 });

    const bool *w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);
    test_bool(w[0], true);
    test_bool(w[1], true);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = e, .buttons = 0 });

    w = ecs_get_id(world, e, mut);
    test_assert(w != NULL);
    test_bool(w[0], true);
    test_bool(w[1], false);

    ecs_fini(world);
}

void Event_stop_propagation_key(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut downs = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      await on.key_down(this)"
        LINE "      downs = downs + 1"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "template Stopper {"
        LINE "  mut downs = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.key_down(this)"
        LINE "      downs = downs + 1"
        LINE "      await e.stop_propagation()"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e { Stopper child { Widget grandchild {} } }",
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

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = grandchild, .buttons = 1 });

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "a"), .down = true
    });

    ecs_entity_t widget_mut = ecs_lookup(world, "Widget.mut");
    ecs_entity_t stopper_mut = ecs_lookup(world, "Stopper.mut");
    test_assert(widget_mut != 0);
    test_assert(stopper_mut != 0);

    test_int(*(const int64_t*)ecs_get_id(world, grandchild, widget_mut), 1);
    test_int(*(const int64_t*)ecs_get_id(world, child, stopper_mut), 1);
    test_int(*(const int64_t*)ecs_get_id(world, e, widget_mut), 0);

    ecs_fini(world);
}

void Event_stop_propagation_keeps_enter(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut enters = 0"
        LINE "  mut moves = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      await on.enter(this)"
        LINE "      enters = enters + 1"
        LINE "    }"
        LINE "  }"
        LINE "  async {"
        LINE "    while true {"
        LINE "      await on.move(this)"
        LINE "      moves = moves + 1"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "template Stopper {"
        LINE "  mut moves = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.move(this)"
        LINE "      moves = moves + 1"
        LINE "      await e.stop_propagation()"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e { Stopper child {} }",
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

    test_int(ecs_script_tasks_progress(world), 3);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = child, .screen_x = 1, .screen_y = 1 });

    ecs_entity_t widget_mut = ecs_lookup(world, "Widget.mut");
    ecs_entity_t stopper_mut = ecs_lookup(world, "Stopper.mut");
    test_assert(widget_mut != 0);
    test_assert(stopper_mut != 0);

    test_int(*(const int64_t*)ecs_get_id(world, child, stopper_mut), 1);

    const int64_t *w = ecs_get_id(world, e, widget_mut);
    test_assert(w != NULL);
    test_int(w[0], 1);
    test_int(w[1], 0);

    ecs_fini(world);
}

void Event_stop_propagation_stale_event(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      await on.press(this)"
        LINE "      count = count + 1"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "template Stopper {"
        LINE "  mut result = true"
        LINE "  async {"
        LINE "    const first = await on.press(this)"
        LINE "    await on.release(this)"
        LINE "    await on.press(this)"
        LINE "    const r = await first.stop_propagation()"
        LINE "    result = r"
        LINE "  }"
        LINE "}"
        LINE "Widget e { Stopper child {} }",
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

    test_int(ecs_script_tasks_progress(world), 2);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = child, .buttons = 1 });
    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = child, .buttons = 0 });
    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = child, .buttons = 1 });

    ecs_entity_t widget_mut = ecs_lookup(world, "Widget.mut");
    ecs_entity_t stopper_mut = ecs_lookup(world, "Stopper.mut");
    test_assert(widget_mut != 0);
    test_assert(stopper_mut != 0);

    test_bool(*(const bool*)ecs_get_id(world, child, stopper_mut), false);
    test_int(*(const int64_t*)ecs_get_id(world, e, widget_mut), 2);

    ecs_fini(world);
}

void Event_stop_propagation_enter_event(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut moves = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      await on.move(this)"
        LINE "      moves = moves + 1"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "template Stopper {"
        LINE "  mut result = true"
        LINE "  async {"
        LINE "    const e = await on.enter(this)"
        LINE "    const r = await e.stop_propagation()"
        LINE "    result = r"
        LINE "  }"
        LINE "}"
        LINE "Widget e { Stopper child {} }",
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

    test_int(ecs_script_tasks_progress(world), 2);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = child, .screen_x = 1, .screen_y = 1 });

    ecs_entity_t widget_mut = ecs_lookup(world, "Widget.mut");
    ecs_entity_t stopper_mut = ecs_lookup(world, "Stopper.mut");
    test_assert(widget_mut != 0);
    test_assert(stopper_mut != 0);

    test_bool(*(const bool*)ecs_get_id(world, child, stopper_mut), false);
    test_int(*(const int64_t*)ecs_get_id(world, e, widget_mut), 1);

    ecs_fini(world);
}

void Event_import_focus(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    test_assert(ecs_lookup(world, "flecs.script.event.FocusEvent") ==
        ecs_id(EcsScriptFocusEvent));
    test_assert(ecs_lookup(world, "flecs.script.on.focus") != 0);
    test_assert(ecs_lookup(world, "flecs.script.on.blur") != 0);
    test_assert(ecs_lookup(world, "flecs.script.focus") != 0);
    test_uint(ecs_script_get_focus(world), 0);

    ecs_fini(world);
}

void Event_set_get_focus(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t a = ecs_new(world);
    ecs_entity_t b = ecs_new(world);

    test_uint(ecs_script_get_focus(world), 0);
    test_uint(ecs_script_set_focus(world, a), a);
    test_uint(ecs_script_get_focus(world), a);
    test_uint(ecs_script_set_focus(world, b), b);
    test_uint(ecs_script_get_focus(world), b);
    test_uint(ecs_script_set_focus(world, 0), 0);
    test_uint(ecs_script_get_focus(world), 0);

    ecs_fini(world);
}

void Event_get_focus_after_press(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t a = ecs_new(world);
    ecs_entity_t b = ecs_new(world);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = a, .buttons = 1 });
    test_uint(ecs_script_get_focus(world), a);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){ .target = a });
    test_uint(ecs_script_get_focus(world), a);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = b, .buttons = 1 });
    test_uint(ecs_script_get_focus(world), b);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){ .target = b });
    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = 0, .buttons = 1 });
    test_uint(ecs_script_get_focus(world), 0);

    ecs_fini(world);
}

void Event_key_to_set_focus(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
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
    test_assert(mut != 0);

    test_int(ecs_script_tasks_progress(world), 2);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = a, .buttons = 1 });
    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){ .target = a });

    ecs_script_set_focus(world, b);

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "a"), .down = true
    });

    test_int(*(const int64_t*)ecs_get_id(world, a, mut), 0);
    test_int(*(const int64_t*)ecs_get_id(world, b, mut), 1);

    ecs_fini(world);
}

void Event_focus_blur_on_press(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut focus_count = 0"
        LINE "  mut blur_count = 0"
        LINE "  mut focus_target: entity = 0"
        LINE "  mut focus_related: entity = 0"
        LINE "  mut blur_target: entity = 0"
        LINE "  mut blur_related: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.focus(this)"
        LINE "      focus_count = focus_count + 1"
        LINE "      focus_target = e.target"
        LINE "      focus_related = e.related"
        LINE "    }"
        LINE "  }"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.blur(this)"
        LINE "      blur_count = blur_count + 1"
        LINE "      blur_target = e.target"
        LINE "      blur_related = e.related"
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
    test_assert(mut != 0);

    test_int(ecs_script_tasks_progress(world), 4);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = a, .buttons = 1 });
    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){ .target = a });

    const ev_Focus *fa = ecs_get_id(world, a, mut);
    const ev_Focus *fb = ecs_get_id(world, b, mut);
    test_int(fa->focus_count, 1);
    test_uint(fa->focus_target, a);
    test_uint(fa->focus_related, 0);
    test_int(fa->blur_count, 0);
    test_int(fb->focus_count, 0);
    test_int(fb->blur_count, 0);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = b, .buttons = 1 });
    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){ .target = b });

    fa = ecs_get_id(world, a, mut);
    fb = ecs_get_id(world, b, mut);
    test_int(fa->focus_count, 1);
    test_int(fa->blur_count, 1);
    test_uint(fa->blur_target, a);
    test_uint(fa->blur_related, b);
    test_int(fb->focus_count, 1);
    test_uint(fb->focus_target, b);
    test_uint(fb->focus_related, a);
    test_int(fb->blur_count, 0);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = 0, .buttons = 1 });

    fa = ecs_get_id(world, a, mut);
    fb = ecs_get_id(world, b, mut);
    test_int(fa->focus_count, 1);
    test_int(fa->blur_count, 1);
    test_int(fb->focus_count, 1);
    test_int(fb->blur_count, 1);
    test_uint(fb->blur_target, b);
    test_uint(fb->blur_related, 0);

    ecs_fini(world);
}

void Event_focus_blur_on_set_focus(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut focus_count = 0"
        LINE "  mut blur_count = 0"
        LINE "  mut focus_target: entity = 0"
        LINE "  mut focus_related: entity = 0"
        LINE "  mut blur_target: entity = 0"
        LINE "  mut blur_related: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.focus(this)"
        LINE "      focus_count = focus_count + 1"
        LINE "      focus_target = e.target"
        LINE "      focus_related = e.related"
        LINE "    }"
        LINE "  }"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.blur(this)"
        LINE "      blur_count = blur_count + 1"
        LINE "      blur_target = e.target"
        LINE "      blur_related = e.related"
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
    test_assert(mut != 0);

    test_int(ecs_script_tasks_progress(world), 4);

    ecs_script_set_focus(world, a);

    const ev_Focus *fa = ecs_get_id(world, a, mut);
    test_int(fa->focus_count, 1);
    test_uint(fa->focus_target, a);
    test_uint(fa->focus_related, 0);

    ecs_script_set_focus(world, b);

    fa = ecs_get_id(world, a, mut);
    const ev_Focus *fb = ecs_get_id(world, b, mut);
    test_int(fa->blur_count, 1);
    test_uint(fa->blur_target, a);
    test_uint(fa->blur_related, b);
    test_int(fb->focus_count, 1);
    test_uint(fb->focus_target, b);
    test_uint(fb->focus_related, a);

    ecs_script_set_focus(world, 0);

    fb = ecs_get_id(world, b, mut);
    test_int(fb->blur_count, 1);
    test_uint(fb->blur_target, b);
    test_uint(fb->blur_related, 0);

    ecs_fini(world);
}

void Event_focus_unchanged_no_events(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut focus_count = 0"
        LINE "  mut blur_count = 0"
        LINE "  mut focus_target: entity = 0"
        LINE "  mut focus_related: entity = 0"
        LINE "  mut blur_target: entity = 0"
        LINE "  mut blur_related: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.focus(this)"
        LINE "      focus_count = focus_count + 1"
        LINE "      focus_target = e.target"
        LINE "      focus_related = e.related"
        LINE "    }"
        LINE "  }"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.blur(this)"
        LINE "      blur_count = blur_count + 1"
        LINE "      blur_target = e.target"
        LINE "      blur_related = e.related"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget a",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(a != 0);
    test_assert(mut != 0);

    test_int(ecs_script_tasks_progress(world), 2);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = a, .buttons = 1 });
    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){ .target = a });
    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = a, .buttons = 1 });
    ecs_script_set_focus(world, a);

    const ev_Focus *fa = ecs_get_id(world, a, mut);
    test_int(fa->focus_count, 1);
    test_int(fa->blur_count, 0);

    ecs_fini(world);
}

void Event_focus_between_children_reaches_parent(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut focus_count = 0"
        LINE "  mut blur_count = 0"
        LINE "  mut focus_target: entity = 0"
        LINE "  mut focus_related: entity = 0"
        LINE "  mut blur_target: entity = 0"
        LINE "  mut blur_related: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.focus(this)"
        LINE "      focus_count = focus_count + 1"
        LINE "      focus_target = e.target"
        LINE "      focus_related = e.related"
        LINE "    }"
        LINE "  }"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.blur(this)"
        LINE "      blur_count = blur_count + 1"
        LINE "      blur_target = e.target"
        LINE "      blur_related = e.related"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget p {"
        LINE "  c1 {}"
        LINE "  c2 {}"
        LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t p = ecs_lookup(world, "p");
    ecs_entity_t c1 = ecs_lookup(world, "p.c1");
    ecs_entity_t c2 = ecs_lookup(world, "p.c2");
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(p != 0);
    test_assert(c1 != 0);
    test_assert(c2 != 0);
    test_assert(mut != 0);

    test_int(ecs_script_tasks_progress(world), 2);

    ecs_script_set_focus(world, c1);

    const ev_Focus *fp = ecs_get_id(world, p, mut);
    test_int(fp->focus_count, 1);
    test_uint(fp->focus_target, c1);
    test_int(fp->blur_count, 0);

    ecs_script_set_focus(world, c2);

    fp = ecs_get_id(world, p, mut);
    test_int(fp->blur_count, 1);
    test_uint(fp->blur_target, c1);
    test_uint(fp->blur_related, c2);
    test_int(fp->focus_count, 2);
    test_uint(fp->focus_target, c2);
    test_uint(fp->focus_related, c1);

    ecs_fini(world);
}

void Event_stop_propagation_focus(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut focus_count = 0"
        LINE "  mut blur_count = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      await on.focus(this)"
        LINE "      focus_count = focus_count + 1"
        LINE "    }"
        LINE "  }"
        LINE "  async {"
        LINE "    while true {"
        LINE "      await on.blur(this)"
        LINE "      blur_count = blur_count + 1"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "template Stopper {"
        LINE "  mut stops = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.focus(this)"
        LINE "      stops = stops + 1"
        LINE "      await e.stop_propagation()"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e { Stopper child { grandchild {} } }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    ecs_entity_t grandchild = ecs_lookup(world, "e.child.grandchild");
    ecs_entity_t widget_mut = ecs_lookup(world, "Widget.mut");
    ecs_entity_t stopper_mut = ecs_lookup(world, "Stopper.mut");
    test_assert(e != 0);
    test_assert(child != 0);
    test_assert(grandchild != 0);
    test_assert(widget_mut != 0);
    test_assert(stopper_mut != 0);

    test_int(ecs_script_tasks_progress(world), 3);

    ecs_script_set_focus(world, grandchild);

    test_int(*(const int64_t*)ecs_get_id(world, child, stopper_mut), 1);
    const int64_t *w = ecs_get_id(world, e, widget_mut);
    test_int(w[0], 0);
    test_int(w[1], 0);

    ecs_script_set_focus(world, 0);

    w = ecs_get_id(world, e, widget_mut);
    test_int(w[0], 0);
    test_int(w[1], 1);

    ecs_fini(world);
}

void Event_to_focus_reject_press(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut focus_count = 0"
        LINE "  mut blur_count = 0"
        LINE "  mut focus_target: entity = 0"
        LINE "  mut focus_related: entity = 0"
        LINE "  mut blur_target: entity = 0"
        LINE "  mut blur_related: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.focus(this)"
        LINE "      focus_count = focus_count + 1"
        LINE "      focus_target = e.target"
        LINE "      focus_related = e.related"
        LINE "    }"
        LINE "  }"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.blur(this)"
        LINE "      blur_count = blur_count + 1"
        LINE "      blur_target = e.target"
        LINE "      blur_related = e.related"
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
    test_assert(mut != 0);

    test_int(ecs_script_tasks_progress(world), 4);

    ecs_script_set_focus(world, a);

    to_focus_invoked = 0;
    to_focus_entity = 0;
    ecs_entity_t keep = a;
    ecs_script_set_event_hooks(world, &(ecs_script_event_hooks_t){
        .to_focus = Event_to_focus,
        .ctx = &keep
    });

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = b, .buttons = 1 });

    test_int(to_focus_invoked, 1);
    test_uint(to_focus_entity, b);
    test_uint(to_focus_current, a);
    test_int(to_focus_source, EcsScriptFocusPress);
    test_uint(ecs_script_get_focus(world), a);

    const ev_Focus *fa = ecs_get_id(world, a, mut);
    const ev_Focus *fb = ecs_get_id(world, b, mut);
    test_int(fa->focus_count, 1);
    test_int(fa->blur_count, 0);
    test_int(fb->focus_count, 0);
    test_int(fb->blur_count, 0);

    ecs_fini(world);
}

void Event_to_focus_redirect(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut focus_count = 0"
        LINE "  mut blur_count = 0"
        LINE "  mut focus_target: entity = 0"
        LINE "  mut focus_related: entity = 0"
        LINE "  mut blur_target: entity = 0"
        LINE "  mut blur_related: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.focus(this)"
        LINE "      focus_count = focus_count + 1"
        LINE "      focus_target = e.target"
        LINE "      focus_related = e.related"
        LINE "    }"
        LINE "  }"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.blur(this)"
        LINE "      blur_count = blur_count + 1"
        LINE "      blur_target = e.target"
        LINE "      blur_related = e.related"
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
    test_assert(mut != 0);

    test_int(ecs_script_tasks_progress(world), 4);

    to_focus_invoked = 0;
    to_focus_entity = 0;
    ecs_entity_t redirect = b;
    ecs_script_set_event_hooks(world, &(ecs_script_event_hooks_t){
        .to_focus = Event_to_focus,
        .ctx = &redirect
    });

    test_uint(ecs_script_set_focus(world, a), b);
    test_int(to_focus_invoked, 1);
    test_uint(to_focus_entity, a);
    test_uint(to_focus_current, 0);
    test_int(to_focus_source, EcsScriptFocusSet);
    test_uint(ecs_script_get_focus(world), b);

    const ev_Focus *fa = ecs_get_id(world, a, mut);
    const ev_Focus *fb = ecs_get_id(world, b, mut);
    test_int(fa->focus_count, 0);
    test_int(fb->focus_count, 1);
    test_uint(fb->focus_target, b);

    ecs_fini(world);
}

void Event_to_focus_not_invoked_when_unchanged(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t a = ecs_new(world);

    to_focus_invoked = 0;
    ecs_script_set_event_hooks(world, &(ecs_script_event_hooks_t){
        .to_focus = Event_to_focus
    });

    ecs_script_set_focus(world, a);
    test_int(to_focus_invoked, 1);

    ecs_script_set_focus(world, a);
    test_int(to_focus_invoked, 1);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = a, .buttons = 1 });
    test_int(to_focus_invoked, 1);

    ecs_fini(world);
}

void Event_remove_event_hooks(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t a = ecs_new(world);
    ecs_entity_t b = ecs_new(world);

    ecs_entity_t keep = 0;
    to_focus_invoked = 0;
    ecs_script_set_event_hooks(world, &(ecs_script_event_hooks_t){
        .to_focus = Event_to_focus,
        .ctx = &keep
    });

    test_uint(ecs_script_set_focus(world, a), 0);
    test_int(to_focus_invoked, 1);

    ecs_script_set_event_hooks(world, NULL);

    test_uint(ecs_script_set_focus(world, b), b);
    test_int(to_focus_invoked, 1);
    test_uint(ecs_script_get_focus(world), b);

    ecs_fini(world);
}

void Event_script_focus(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut downs = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      await on.key_down(this)"
        LINE "      downs = downs + 1"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget b"
        LINE "template Caller {"
        LINE "  prop other: entity = b"
        LINE "  mut result: entity = 0"
        LINE "  async {"
        LINE "    await on.press(this)"
        LINE "    const r = await focus(other)"
        LINE "    result = r"
        LINE "  }"
        LINE "}"
        LINE "Caller a",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");
    ecs_entity_t widget_mut = ecs_lookup(world, "Widget.mut");
    ecs_entity_t caller_mut = ecs_lookup(world, "Caller.mut");
    test_assert(a != 0);
    test_assert(b != 0);
    test_assert(widget_mut != 0);
    test_assert(caller_mut != 0);

    test_int(ecs_script_tasks_progress(world), 2);

    to_focus_invoked = 0;
    ecs_script_set_event_hooks(world, &(ecs_script_event_hooks_t){
        .to_focus = Event_to_focus
    });

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = a, .buttons = 1 });

    test_int(to_focus_invoked, 2);
    test_uint(to_focus_entity, b);
    test_int(to_focus_source, EcsScriptFocusScript);
    test_uint(ecs_script_get_focus(world), b);
    test_uint(*(const ecs_entity_t*)ecs_get_id(world, a, caller_mut), b);

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "a"), .down = true
    });

    test_int(*(const int64_t*)ecs_get_id(world, b, widget_mut), 1);

    ecs_fini(world);
}

void Event_script_focus_returns_to_focus_result(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "b {}"
        LINE "template Caller {"
        LINE "  prop other: entity = b"
        LINE "  mut result: entity = 0"
        LINE "  async {"
        LINE "    await on.press(this)"
        LINE "    const r = await focus(other)"
        LINE "    result = r"
        LINE "  }"
        LINE "}"
        LINE "Caller a",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");
    ecs_entity_t mut = ecs_lookup(world, "Caller.mut");
    test_assert(a != 0);
    test_assert(b != 0);
    test_assert(mut != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    ecs_script_set_focus(world, a);

    ecs_entity_t keep = a;
    ecs_script_set_event_hooks(world, &(ecs_script_event_hooks_t){
        .to_focus = Event_to_focus,
        .ctx = &keep
    });

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = a, .buttons = 1 });

    test_uint(ecs_script_get_focus(world), a);
    test_uint(*(const ecs_entity_t*)ecs_get_id(world, a, mut), a);

    ecs_fini(world);
}

void Event_script_focus_clear(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Caller {"
        LINE "  mut result: entity = 0"
        LINE "  mut done = false"
        LINE "  async {"
        LINE "    await on.release(this)"
        LINE "    const r = await focus(0)"
        LINE "    result = r"
        LINE "    done = true"
        LINE "  }"
        LINE "}"
        LINE "Caller a",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t mut = ecs_lookup(world, "Caller.mut");
    test_assert(a != 0);
    test_assert(mut != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = a, .buttons = 1 });
    test_uint(ecs_script_get_focus(world), a);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){ .target = a });
    test_uint(ecs_script_get_focus(world), 0);

    const ecs_entity_t *result = ecs_get_id(world, a, mut);
    test_uint(result[0], 0);
    test_bool(*(const bool*)&result[1], true);

    ecs_fini(world);
}

void Event_script_focus_resumes_listeners(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Listener {"
        LINE "  mut focus_count = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      await on.focus(this)"
        LINE "      focus_count = focus_count + 1"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Listener b"
        LINE "template Caller {"
        LINE "  prop other: entity = b"
        LINE "  async {"
        LINE "    await on.press(this)"
        LINE "    await focus(other)"
        LINE "  }"
        LINE "}"
        LINE "Caller a",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");
    ecs_entity_t mut = ecs_lookup(world, "Listener.mut");
    test_assert(a != 0);
    test_assert(b != 0);
    test_assert(mut != 0);

    test_int(ecs_script_tasks_progress(world), 2);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = a, .buttons = 1 });

    test_uint(ecs_script_get_focus(world), b);
    test_int(*(const int64_t*)ecs_get_id(world, b, mut), 1);

    ecs_fini(world);
}

void Event_delete_focus(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut downs = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      await on.key_down(this)"
        LINE "      downs = downs + 1"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget p { child {} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t p = ecs_lookup(world, "p");
    ecs_entity_t child = ecs_lookup(world, "p.child");
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(p != 0);
    test_assert(child != 0);
    test_assert(mut != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    ecs_script_set_focus(world, child);
    test_uint(ecs_script_get_focus(world), child);

    ecs_delete(world, child);
    test_uint(ecs_script_get_focus(world), 0);

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "a"), .down = true
    });

    test_int(*(const int64_t*)ecs_get_id(world, p, mut), 0);

    ecs_script_set_focus(world, p);

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "a"), .down = true
    });

    test_int(*(const int64_t*)ecs_get_id(world, p, mut), 1);

    ecs_fini(world);
}

void Event_import_activate(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    test_assert(ecs_lookup(world, "flecs.script.event.ActivateEvent") ==
        ecs_id(EcsScriptActivateEvent));
    test_assert(ecs_lookup(world, "flecs.script.on.activate") != 0);

    ecs_fini(world);
}

void Event_activate_click(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut keyboard = true"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.activate(this)"
        LINE "      count = count + 1"
        LINE "      keyboard = e.keyboard"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
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

    typedef struct {
        int64_t count;
        bool keyboard;
        float x, y;
        ecs_entity_t target;
    } Mut;
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = e, .screen_x = 3, .screen_y = 4,
        .local_x = 3, .local_y = 4, .buttons = 1
    });

    const Mut *m = ecs_get_id(world, e, mut);
    test_int(m->count, 0);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = e, .screen_x = 5, .screen_y = 6,
        .local_x = 5, .local_y = 6, .buttons = 0
    });

    m = ecs_get_id(world, e, mut);
    test_int(m->count, 1);
    test_bool(m->keyboard, false);
    test_flt(m->x, 5);
    test_flt(m->y, 6);
    test_uint(m->target, e);

    ecs_fini(world);
}

void Event_no_activate_when_released_elsewhere(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      await on.activate(this)"
        LINE "      count = count + 1"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e"
        LINE "Widget other",
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
    test_assert(mut != 0);

    test_int(ecs_script_tasks_progress(world), 2);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = e, .buttons = 1 });
    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = other, .buttons = 0 });

    test_int(*(const int64_t*)ecs_get_id(world, e, mut), 0);
    test_int(*(const int64_t*)ecs_get_id(world, other, mut), 0);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = e, .buttons = 1 });
    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = 0, .buttons = 0 });

    test_int(*(const int64_t*)ecs_get_id(world, e, mut), 0);
    test_int(*(const int64_t*)ecs_get_id(world, other, mut), 0);

    ecs_fini(world);
}

void Event_activate_enter(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut keyboard = false"
        LINE "  mut x: f32 = -1"
        LINE "  mut y: f32 = -1"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.activate(this)"
        LINE "      count = count + 1"
        LINE "      keyboard = e.keyboard"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
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

    typedef struct {
        int64_t count;
        bool keyboard;
        float x, y;
        ecs_entity_t target;
    } Mut;
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    ecs_script_set_focus(world, e);

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "Enter"), .down = true
    });

    const Mut *m = ecs_get_id(world, e, mut);
    test_int(m->count, 1);
    test_bool(m->keyboard, true);
    test_flt(m->x, 0);
    test_flt(m->y, 0);
    test_uint(m->target, e);

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "Enter"), .down = false
    });

    m = ecs_get_id(world, e, mut);
    test_int(m->count, 1);

    ecs_fini(world);
}

void Event_activate_space_on_release(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut keyboard = false"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.activate(this)"
        LINE "      count = count + 1"
        LINE "      keyboard = e.keyboard"
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

    typedef struct {
        int64_t count;
        bool keyboard;
    } Mut;
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    ecs_script_set_focus(world, e);

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, " "), .down = true
    });

    test_int(((const Mut*)ecs_get_id(world, e, mut))->count, 0);

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, " "), .down = true, .repeat = true
    });

    test_int(((const Mut*)ecs_get_id(world, e, mut))->count, 0);

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, " "), .down = false
    });

    const Mut *m = ecs_get_id(world, e, mut);
    test_int(m->count, 1);
    test_bool(m->keyboard, true);

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "Space"), .down = true
    });

    test_int(((const Mut*)ecs_get_id(world, e, mut))->count, 1);

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "Space"), .down = false
    });

    test_int(((const Mut*)ecs_get_id(world, e, mut))->count, 2);

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, " "), .down = false
    });

    test_int(((const Mut*)ecs_get_id(world, e, mut))->count, 2);

    ecs_fini(world);
}

void Event_no_activate_without_focus(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      await on.activate(this)"
        LINE "      count = count + 1"
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
    test_assert(mut != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "Enter"), .down = true });
    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "Enter"), .down = false });
    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, " "), .down = true });
    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, " "), .down = false });

    test_int(*(const int64_t*)ecs_get_id(world, e, mut), 0);

    ecs_script_set_focus(world, other);

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "Enter"), .down = true });
    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "Enter"), .down = false });
    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, " "), .down = true });
    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, " "), .down = false });

    test_int(*(const int64_t*)ecs_get_id(world, e, mut), 0);

    ecs_fini(world);
}

void Event_no_activate_space_focus_changed(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      await on.activate(this)"
        LINE "      count = count + 1"
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
    test_assert(mut != 0);

    test_int(ecs_script_tasks_progress(world), 2);

    ecs_script_set_focus(world, a);
    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, " "), .down = true });
    ecs_script_set_focus(world, b);
    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, " "), .down = true, .repeat = true });
    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, " "), .down = false });

    test_int(*(const int64_t*)ecs_get_id(world, a, mut), 0);
    test_int(*(const int64_t*)ecs_get_id(world, b, mut), 0);

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, " "), .down = true });
    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, " "), .down = false });

    test_int(*(const int64_t*)ecs_get_id(world, a, mut), 0);
    test_int(*(const int64_t*)ecs_get_id(world, b, mut), 1);

    ecs_fini(world);
}

void Event_no_activate_enter_repeat(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      await on.activate(this)"
        LINE "      count = count + 1"
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
    test_assert(mut != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    ecs_script_set_focus(world, e);

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "Enter"), .down = true });
    test_int(*(const int64_t*)ecs_get_id(world, e, mut), 1);

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "Enter"), .down = true, .repeat = true });
    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "Enter"), .down = true, .repeat = true });
    test_int(*(const int64_t*)ecs_get_id(world, e, mut), 1);

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "Enter"), .down = false });
    test_int(*(const int64_t*)ecs_get_id(world, e, mut), 1);

    ecs_fini(world);
}

void Event_activate_bubbles_to_parent(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut keyboard = true"
        LINE "  mut x: f32 = 0"
        LINE "  mut y: f32 = 0"
        LINE "  mut target: entity = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.activate(this)"
        LINE "      count = count + 1"
        LINE "      keyboard = e.keyboard"
        LINE "      x = e.local_x"
        LINE "      y = e.local_y"
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

    typedef struct {
        int64_t count;
        bool keyboard;
        float x, y;
        ecs_entity_t target;
    } Mut;
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);

    ecs_script_set_event_hooks(world, &(ecs_script_event_hooks_t){
        .to_local = Event_to_local
    });

    test_int(ecs_script_tasks_progress(world), 1);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = child, .screen_x = 5, .screen_y = 6,
        .local_x = 1, .local_y = 2, .buttons = 1
    });
    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = child, .screen_x = 5, .screen_y = 6,
        .local_x = 1, .local_y = 2, .buttons = 0
    });

    const Mut *m = ecs_get_id(world, e, mut);
    test_int(m->count, 1);
    test_bool(m->keyboard, false);
    test_flt(m->x, 105);
    test_flt(m->y, 106);
    test_uint(m->target, child);

    ecs_script_set_focus(world, child);
    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "Enter"), .down = true });

    m = ecs_get_id(world, e, mut);
    test_int(m->count, 2);
    test_bool(m->keyboard, true);
    test_flt(m->x, 0);
    test_flt(m->y, 0);
    test_uint(m->target, child);

    ecs_fini(world);
}

void Event_activate_once_per_activation(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      await on.activate(this)"
        LINE "      count = count + 1"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Widget e { Widget child }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(e != 0);
    test_assert(child != 0);
    test_assert(mut != 0);

    test_int(ecs_script_tasks_progress(world), 2);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = child, .buttons = 1 });
    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = child, .buttons = 0 });

    test_int(*(const int64_t*)ecs_get_id(world, e, mut), 1);
    test_int(*(const int64_t*)ecs_get_id(world, child, mut), 1);

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "Enter"), .down = true });
    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "Enter"), .down = false });

    test_int(*(const int64_t*)ecs_get_id(world, e, mut), 2);
    test_int(*(const int64_t*)ecs_get_id(world, child, mut), 2);

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, " "), .down = true });
    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, " "), .down = false });

    test_int(*(const int64_t*)ecs_get_id(world, e, mut), 3);
    test_int(*(const int64_t*)ecs_get_id(world, child, mut), 3);

    ecs_fini(world);
}

void Event_activate_modifiers(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut count = 0"
        LINE "  mut ctrl = false"
        LINE "  mut shift = false"
        LINE "  mut alt = false"
        LINE "  mut meta = false"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.activate(this)"
        LINE "      count = count + 1"
        LINE "      ctrl = e.ctrl"
        LINE "      shift = e.shift"
        LINE "      alt = e.alt"
        LINE "      meta = e.meta"
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

    typedef struct {
        int64_t count;
        bool ctrl, shift, alt, meta;
    } Mut;
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = e, .buttons = 1, .ctrl = true, .alt = true });
    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = e, .buttons = 0, .ctrl = true, .alt = true });

    const Mut *m = ecs_get_id(world, e, mut);
    test_int(m->count, 1);
    test_bool(m->ctrl, true);
    test_bool(m->shift, false);
    test_bool(m->alt, true);
    test_bool(m->meta, false);

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "Enter"), .down = true,
        .shift = true, .meta = true });

    m = ecs_get_id(world, e, mut);
    test_int(m->count, 2);
    test_bool(m->ctrl, false);
    test_bool(m->shift, true);
    test_bool(m->alt, false);
    test_bool(m->meta, true);

    ecs_fini(world);
}

void Event_activate_and_click(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut activates = 0"
        LINE "  mut clicks = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      await on.activate(this)"
        LINE "      activates = activates + 1"
        LINE "    }"
        LINE "  }"
        LINE "  async {"
        LINE "    while true {"
        LINE "      await on.click(this)"
        LINE "      clicks = clicks + 1"
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

    typedef struct {
        int64_t activates, clicks;
    } Mut;
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);

    test_int(ecs_script_tasks_progress(world), 2);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = e, .buttons = 1 });
    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = e, .buttons = 0 });

    const Mut *m = ecs_get_id(world, e, mut);
    test_int(m->activates, 1);
    test_int(m->clicks, 1);

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "Enter"), .down = true });

    m = ecs_get_id(world, e, mut);
    test_int(m->activates, 2);
    test_int(m->clicks, 1);

    ecs_fini(world);
}

void Event_stop_propagation_activate(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Parent {"
        LINE "  mut activates = 0"
        LINE "  mut keys = 0"
        LINE "  async {"
        LINE "    while true {"
        LINE "      await on.activate(this)"
        LINE "      activates = activates + 1"
        LINE "    }"
        LINE "  }"
        LINE "  async {"
        LINE "    while true {"
        LINE "      await on.key_down(this)"
        LINE "      keys = keys + 1"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "template Child {"
        LINE "  mut stopped = false"
        LINE "  async {"
        LINE "    while true {"
        LINE "      const e = await on.activate(this)"
        LINE "      const r = await e.stop_propagation()"
        LINE "      stopped = r"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "Parent p { Child c }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t p = ecs_lookup(world, "p");
    ecs_entity_t c = ecs_lookup(world, "p.c");
    test_assert(p != 0);
    test_assert(c != 0);

    typedef struct {
        int64_t activates, keys;
    } ParentMut;
    ecs_entity_t parent_mut = ecs_lookup(world, "Parent.mut");
    ecs_entity_t child_mut = ecs_lookup(world, "Child.mut");
    test_assert(parent_mut != 0);
    test_assert(child_mut != 0);

    test_int(ecs_script_tasks_progress(world), 3);

    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = c, .buttons = 1 });
    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = c, .buttons = 0 });

    test_bool(*(const bool*)ecs_get_id(world, c, child_mut), true);
    const ParentMut *m = ecs_get_id(world, p, parent_mut);
    test_int(m->activates, 0);

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "Enter"), .down = true });

    m = ecs_get_id(world, p, parent_mut);
    test_int(m->activates, 0);
    test_int(m->keys, 0);

    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, "a"), .down = true });

    m = ecs_get_id(world, p, parent_mut);
    test_int(m->activates, 0);
    test_int(m->keys, 1);

    ecs_fini(world);
}

void Event_get_mouse_state(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t e = ecs_new(world);

    EcsScriptMouseEvent last;
    ecs_entity_t pressed = 1;
    test_bool(ecs_script_get_mouse_state(world, &last, &pressed), false);
    test_uint(pressed, 0);

    EcsScriptMouseEvent evt = {
        .target = e, .screen_x = 1, .screen_y = 2, .buttons = 1
    };
    ecs_script_mouse_event(world, &evt);

    test_bool(ecs_script_get_mouse_state(world, &last, &pressed), true);
    test_uint(last.target, e);
    test_flt(last.screen_x, 1);
    test_flt(last.screen_y, 2);
    test_uint(last.buttons, 1);
    test_assert(last.id != 0);
    test_uint(pressed, e);

    evt.buttons = 0;
    ecs_script_mouse_event(world, &evt);
    test_bool(ecs_script_get_mouse_state(world, &last, &pressed), true);
    test_uint(last.buttons, 0);
    test_uint(pressed, 0);

    ecs_fini(world);
}

void Event_listeners(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  async { await on.click(this) }"
        LINE "  async { await on.key_up(this) }"
        LINE "  async { await on.click(this) }"
        LINE "}"
        LINE "Widget e"
        LINE "f {}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    ecs_script_tasks_progress(world);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t f = ecs_lookup(world, "f");

    const char *kinds[4] = {0};
    test_int(ecs_script_event_listeners(world, e, kinds, 4), 2);
    test_str(kinds[0], "click");
    test_str(kinds[1], "key_up");

    test_int(ecs_script_event_listeners(world, e, kinds, 1), 2);
    test_int(ecs_script_event_listeners(world, e, NULL, 0), 2);
    test_int(ecs_script_event_listeners(world, f, kinds, 4), 0);

    EcsScriptMouseEvent evt = { .target = e, .buttons = 1 };
    ecs_script_mouse_event(world, &evt);
    evt.buttons = 0;
    ecs_script_mouse_event(world, &evt);

    test_int(ecs_script_event_listeners(world, e, kinds, 4), 1);
    test_str(kinds[0], "key_up");

    ecs_fini(world);
}

void Event_emit_result(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  async { while true { await on.press(this) } }"
        LINE "  async { while true { await on.key_down(this) } }"
        LINE "}"
        LINE "Widget parent {"
        LINE "  Widget child {}"
        LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    ecs_script_tasks_progress(world);

    ecs_entity_t child = ecs_lookup(world, "parent.child");
    test_assert(child != 0);

    EcsScriptMouseEvent evt = { .target = child, .buttons = 1 };
    ecs_script_event_result_t r = ecs_script_mouse_event(world, &evt);
    test_int(r.resolved, 2);
    test_bool(r.stopped, false);

    EcsScriptKeyboardEvent key = { .key = "a", .down = true };
    r = ecs_script_keyboard_event(world, &key);
    test_int(r.resolved, 2);
    test_bool(r.stopped, false);

    evt.buttons = 0;
    r = ecs_script_mouse_event(world, &evt);
    test_int(r.resolved, 0);

    ecs_fini(world);
}

void Event_emit_result_stopped(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);

    ecs_entity_t s = ecs_script(world, {
        .code =
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  async { while true {"
        LINE "    const e = await on.press(this)"
        LINE "    await e.stop_propagation()"
        LINE "  } }"
        LINE "}"
        LINE "Widget parent {"
        LINE "  Widget child {}"
        LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    ecs_script_tasks_progress(world);

    ecs_entity_t child = ecs_lookup(world, "parent.child");
    test_assert(child != 0);

    EcsScriptMouseEvent evt = { .target = child, .buttons = 1 };
    ecs_script_event_result_t r = ecs_script_mouse_event(world, &evt);
    test_int(r.resolved, 1);
    test_bool(r.stopped, true);

    ecs_fini(world);
}
