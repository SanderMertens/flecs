#include <script.h>

static bool ir_enabled = false;

void Event_setup(void) {
    const char *ir_param = test_param("ir");
    ir_enabled = ir_param && !strcmp(ir_param, "enabled");
}

static ecs_world_t* ev_world(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);
    return world;
}

static ecs_entity_t ev_script(ecs_world_t *world, const char *code) {
    ecs_entity_t s = ecs_script(world, {
        .code = code,
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);
    return s;
}

/* Emit an event and propagate it to the parents of the target the way a host
 * would. Parents receive the coordinates offset by 100 per level so tests can
 * tell which coordinates a listener got. */
static void ev_mouse_w_offset(
    ecs_world_t *world,
    ecs_entity_t target,
    float x,
    float y,
    uint32_t buttons,
    float offset)
{
    EcsScriptMouseEvent evt = {
        .target = target, .screen_x = x, .screen_y = y,
        .local_x = x, .local_y = y, .buttons = buttons
    };
    while (ecs_script_mouse_event(world, &evt) && evt.target &&
        ecs_is_alive(world, evt.target))
    {
        evt.target = ecs_get_target(world, evt.target, EcsChildOf, 0);
        evt.local_x += offset;
        evt.local_y += offset;
        if (!evt.target) {
            break;
        }
    }
}

static void ev_mouse(
    ecs_world_t *world,
    ecs_entity_t target,
    float x,
    float y,
    uint32_t buttons)
{
    ev_mouse_w_offset(world, target, x, y, buttons, 0);
}

static void ev_key(
    ecs_world_t *world,
    const char *key,
    bool down)
{
    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, key), .down = down
    });
}

/* Mut variables of the Widget template of an instance */
typedef struct {
    int64_t count;
    float x, y, dx, dy;
    int64_t button;
    ecs_entity_t target;
} ev_Widget;

static const ev_Widget* ev_widget(
    ecs_world_t *world,
    ecs_entity_t instance)
{
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(mut != 0);
    const ev_Widget *w = ecs_get_id(world, instance, mut);
    test_assert(w != NULL);
    return w;
}

#define EV_WIDGET(event)\
    HEAD "using flecs.script"\
    LINE "template Widget {"\
    LINE "  mut count = 0"\
    LINE "  mut x: f32 = 0"\
    LINE "  mut y: f32 = 0"\
    LINE "  mut dx: f32 = 0"\
    LINE "  mut dy: f32 = 0"\
    LINE "  mut button = -1"\
    LINE "  mut target: entity = 0"\
    LINE "  async {"\
    LINE "    while true {"\
    LINE "      const e = await on." event "(this)"\
    LINE "      count = count + 1"\
    LINE "      x = e.local_x"\
    LINE "      y = e.local_y"\
    LINE "      dx = e.delta_x"\
    LINE "      dy = e.delta_y"\
    LINE "      button = e.button"\
    LINE "      target = e.target"\
    LINE "    }"\
    LINE "  }"\
    LINE "}"

void Event_import(void) {
    ecs_world_t *world = ev_world();

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
    ecs_world_t *world = ev_world();

    ev_script(world, EV_WIDGET("press") LINE "Widget e");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ev_widget(world, e)->count, 0);

    ev_mouse(world, e, 10, 20, 1);
    const ev_Widget *w = ev_widget(world, e);
    test_int(w->count, 1);
    test_flt(w->x, 10);
    test_flt(w->y, 20);
    test_int(w->button, 0);
    test_uint(w->target, e);

    ecs_fini(world);
}

void Event_press_other_target(void) {
    ecs_world_t *world = ev_world();

    ev_script(world, EV_WIDGET("press") LINE "Widget e" LINE "other {}");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t other = ecs_lookup(world, "other");
    test_assert(e != 0);
    test_assert(other != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    ev_mouse(world, other, 0, 0, 1);
    ev_mouse(world, other, 0, 0, 0);
    test_int(ev_widget(world, e)->count, 0);

    ev_mouse(world, e, 0, 0, 1);
    test_int(ev_widget(world, e)->count, 1);

    ecs_fini(world);
}

void Event_press_no_target(void) {
    ecs_world_t *world = ev_world();

    ev_script(world, EV_WIDGET("press") LINE "Widget e");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    ev_mouse(world, 0, 0, 0, 1);
    ev_mouse(world, 0, 0, 0, 0);
    test_int(ev_widget(world, e)->count, 0);

    ecs_fini(world);
}

void Event_press_bubbles_to_parent(void) {
    ecs_world_t *world = ev_world();

    ev_script(world, EV_WIDGET("press") LINE "Widget e { child {} }");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(e != 0);
    test_assert(child != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    ev_mouse(world, child, 0, 0, 1);
    const ev_Widget *w = ev_widget(world, e);
    test_int(w->count, 1);
    test_uint(w->target, e);

    ecs_fini(world);
}

void Event_press_two_listeners(void) {
    ecs_world_t *world = ev_world();

    ev_script(world,
        HEAD "using flecs.script"
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
        LINE "Widget e");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 2);
    ev_mouse(world, e, 0, 0, 1);
    const int64_t *m = ecs_get_id(world, e, ecs_lookup(world, "Widget.mut"));
    test_assert(m != NULL);
    test_int(m[0], 1);
    test_int(m[1], 1);

    ecs_fini(world);
}

void Event_press_two_widgets(void) {
    ecs_world_t *world = ev_world();

    ev_script(world, EV_WIDGET("press") LINE "Widget a" LINE "Widget b");
    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");
    test_assert(a != 0);
    test_assert(b != 0);

    test_int(ecs_script_tasks_progress(world), 2);
    ev_mouse(world, b, 0, 0, 1);
    ev_mouse(world, b, 0, 0, 0);
    test_int(ev_widget(world, a)->count, 0);
    test_int(ev_widget(world, b)->count, 1);

    ev_mouse(world, a, 0, 0, 1);
    test_int(ev_widget(world, a)->count, 1);
    test_int(ev_widget(world, b)->count, 1);

    ecs_fini(world);
}

void Event_press_while_other_button_held(void) {
    ecs_world_t *world = ev_world();

    ev_script(world, EV_WIDGET("press") LINE "Widget e");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    ev_mouse(world, e, 0, 0, 1);
    ev_mouse(world, e, 0, 0, 3);
    test_int(ev_widget(world, e)->count, 2);
    ev_mouse(world, e, 0, 0, 2);
    ev_mouse(world, e, 0, 0, 0);
    test_int(ev_widget(world, e)->count, 2);

    ecs_fini(world);
}

void Event_drag(void) {
    ecs_world_t *world = ev_world();

    ev_script(world, EV_WIDGET("drag") LINE "Widget e" LINE "other {}");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t other = ecs_lookup(world, "other");
    test_assert(e != 0);
    test_assert(other != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    ev_mouse(world, e, 0, 0, 1);
    test_int(ev_widget(world, e)->count, 1);

    ev_mouse(world, e, 5, 2, 1);
    const ev_Widget *w = ev_widget(world, e);
    test_int(w->count, 2);
    test_flt(w->x, 5);
    test_flt(w->y, 2);
    test_flt(w->dx, 5);
    test_flt(w->dy, 2);

    /* A host without pointer capture reports the entity under the pointer,
     * which doesn't get the drag. A host with capture keeps reporting the
     * pressed entity. */
    ev_mouse(world, other, 50, 50, 1);
    test_int(ev_widget(world, e)->count, 2);

    ev_mouse(world, e, 50, 50, 1);
    w = ev_widget(world, e);
    test_int(w->count, 3);
    test_flt(w->x, 50);
    test_uint(w->target, e);

    ev_mouse(world, e, 50, 50, 0);
    test_int(ev_widget(world, e)->count, 3);

    ev_mouse(world, other, 60, 60, 1);
    ev_mouse(world, other, 70, 70, 1);
    test_int(ev_widget(world, e)->count, 3);

    ecs_fini(world);
}

void Event_no_drag_without_button(void) {
    ecs_world_t *world = ev_world();

    ev_script(world, EV_WIDGET("drag") LINE "Widget e");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    ev_mouse(world, e, 0, 0, 0);
    ev_mouse(world, e, 5, 5, 0);
    test_int(ev_widget(world, e)->count, 0);

    ecs_fini(world);
}

void Event_release(void) {
    ecs_world_t *world = ev_world();

    ev_script(world, EV_WIDGET("release") LINE "Widget e" LINE "other {}");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t other = ecs_lookup(world, "other");
    test_assert(e != 0);
    test_assert(other != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    ev_mouse(world, e, 0, 0, 1);
    ev_mouse(world, e, 40, 40, 1);
    test_int(ev_widget(world, e)->count, 0);

    /* Released over another entity: not delivered to the pressed entity */
    ev_mouse(world, other, 40, 40, 0);
    test_int(ev_widget(world, e)->count, 0);

    /* Released on the pressed entity (host with pointer capture) */
    ev_mouse(world, e, 0, 0, 1);
    ev_mouse(world, e, 40, 40, 0);
    const ev_Widget *w = ev_widget(world, e);
    test_int(w->count, 1);
    test_flt(w->x, 40);
    test_uint(w->target, e);

    ecs_fini(world);
}

void Event_click(void) {
    ecs_world_t *world = ev_world();

    ev_script(world, EV_WIDGET("click") LINE "Widget e");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    ev_mouse(world, e, 0, 0, 1);
    test_int(ev_widget(world, e)->count, 0);
    ev_mouse(world, e, 2, 2, 0);
    test_int(ev_widget(world, e)->count, 1);

    ecs_fini(world);
}

void Event_no_click_when_released_elsewhere(void) {
    ecs_world_t *world = ev_world();

    ev_script(world, EV_WIDGET("click") LINE "Widget e" LINE "other {}");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t other = ecs_lookup(world, "other");
    test_assert(e != 0);
    test_assert(other != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    ev_mouse(world, e, 0, 0, 1);
    ev_mouse(world, other, 40, 40, 0);
    test_int(ev_widget(world, e)->count, 0);

    ev_mouse(world, e, 0, 0, 1);
    ev_mouse(world, 0, 40, 40, 0);
    test_int(ev_widget(world, e)->count, 0);

    ecs_fini(world);
}

void Event_move(void) {
    ecs_world_t *world = ev_world();

    ev_script(world, EV_WIDGET("move") LINE "Widget e");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    ev_mouse(world, 0, 40, 40, 0);
    test_int(ev_widget(world, e)->count, 0);

    ev_mouse(world, e, 1, 2, 0);
    const ev_Widget *w = ev_widget(world, e);
    test_int(w->count, 1);
    test_flt(w->x, 1);
    test_flt(w->y, 2);
    test_flt(w->dx, -39);
    test_flt(w->dy, -38);

    ev_mouse(world, e, 1, 2, 1);
    ev_mouse(world, e, 3, 4, 1);
    test_int(ev_widget(world, e)->count, 1);

    ecs_fini(world);
}

void Event_delta_from_event(void) {
    ecs_world_t *world = ev_world();

    ev_script(world, EV_WIDGET("press") LINE "Widget e");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = e, .local_x = 1, .local_y = 1,
        .delta_x = 3, .delta_y = 4, .buttons = 1
    });
    const ev_Widget *w = ev_widget(world, e);
    test_flt(w->dx, 3);
    test_flt(w->dy, 4);

    ecs_fini(world);
}

void Event_button_and_modifiers(void) {
    ecs_world_t *world = ev_world();

    ev_script(world,
        HEAD "using flecs.script"
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
        LINE "Widget e");
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
    ecs_world_t *world = ev_world();

    ev_script(world,
        HEAD "using flecs.script"
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
        LINE "Widget e");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    typedef struct { int64_t downs, ups; char *key; } Mut;
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");

    test_int(ecs_script_tasks_progress(world), 2);

    /* No focus yet */
    ev_key(world, "a", true);
    test_int(((const Mut*)ecs_get_id(world, e, mut))->downs, 0);

    ev_mouse(world, e, 0, 0, 1);
    ev_mouse(world, e, 0, 0, 0);

    ev_key(world, "Enter", true);
    const Mut *m = ecs_get_id(world, e, mut);
    test_int(m->downs, 1);
    test_int(m->ups, 0);
    test_str(m->key, "Enter");

    ev_key(world, "Enter", false);
    m = ecs_get_id(world, e, mut);
    test_int(m->downs, 1);
    test_int(m->ups, 1);

    ecs_fini(world);
}

void Event_key_modifiers(void) {
    ecs_world_t *world = ev_world();

    ev_script(world,
        HEAD "using flecs.script"
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
        LINE "Widget e");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    ev_mouse(world, e, 0, 0, 1);
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
    ecs_world_t *world = ev_world();

    ev_script(world,
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
        LINE "Widget b");
    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(a != 0);
    test_assert(b != 0);

    test_int(ecs_script_tasks_progress(world), 2);

    ev_mouse(world, a, 0, 0, 1);
    ev_mouse(world, a, 0, 0, 0);
    ev_key(world, "a", true);
    test_int(*(int64_t*)ecs_get_id(world, a, mut), 1);
    test_int(*(int64_t*)ecs_get_id(world, b, mut), 0);

    ev_mouse(world, b, 0, 0, 1);
    ev_mouse(world, b, 0, 0, 0);
    ev_key(world, "b", true);
    test_int(*(int64_t*)ecs_get_id(world, a, mut), 1);
    test_int(*(int64_t*)ecs_get_id(world, b, mut), 1);

    /* Pressing on nothing clears the focus */
    ev_mouse(world, 0, 0, 0, 1);
    ev_mouse(world, 0, 0, 0, 0);
    ev_key(world, "c", true);
    test_int(*(int64_t*)ecs_get_id(world, a, mut), 1);
    test_int(*(int64_t*)ecs_get_id(world, b, mut), 1);

    ecs_fini(world);
}

void Event_key_bubbles_to_parent(void) {
    ecs_world_t *world = ev_world();

    ev_script(world,
        HEAD "using flecs.script"
        LINE "template Widget {"
        LINE "  mut downs = 0"
        LINE "  async {"
        LINE "    await on.key_down(this)"
        LINE "    downs = downs + 1"
        LINE "  }"
        LINE "}"
        LINE "Widget e { child {} }");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(e != 0);
    test_assert(child != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    ev_mouse(world, child, 0, 0, 1);
    ev_key(world, "a", true);
    test_int(*(int64_t*)ecs_get_id(world, e, ecs_lookup(world, "Widget.mut")), 1);

    ecs_fini(world);
}

void Event_slider(void) {
    ecs_world_t *world = ev_world();

    ev_script(world,
        HEAD "using flecs.script"
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
        LINE "Slider slider");
    ecs_entity_t slider = ecs_lookup(world, "slider");
    ecs_entity_t bg = ecs_lookup(world, "slider.bg");
    ecs_entity_t fill = ecs_lookup(world, "slider.fill");
    ecs_entity_t mut = ecs_lookup(world, "Slider.mut");
    ecs_entity_t rect = ecs_lookup(world, "Rect");
    test_assert(slider != 0);
    test_assert(bg != 0);
    test_assert(fill != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    ev_mouse(world, bg, 0, 0, 1);
    test_flt(*(float*)ecs_get_id(world, slider, mut), 50);
    test_flt(*(float*)ecs_get_id(world, fill, rect), 100);

    ev_mouse(world, bg, 50, 0, 1);
    test_flt(*(float*)ecs_get_id(world, slider, mut), 75);
    test_flt(*(float*)ecs_get_id(world, fill, rect), 150);

    ecs_fini(world);
}

void Event_wrong_arg_fails(void) {
    ecs_world_t *world = ev_world();

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
    ecs_world_t *world = ev_world();

    ev_script(world, EV_WIDGET("press") LINE "Widget e");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    ecs_delete(world, e);

    ev_mouse(world, e, 0, 0, 1);
    ev_mouse(world, e, 0, 0, 0);
    ev_key(world, "a", true);
    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void Event_delete_listener_while_pressed(void) {
    ecs_world_t *world = ev_world();

    ev_script(world, EV_WIDGET("drag") LINE "Widget e");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    ev_mouse(world, e, 0, 0, 1);
    ecs_delete(world, e);

    ev_mouse(world, e, 1, 1, 1);
    ev_mouse(world, e, 1, 1, 0);
    ev_key(world, "a", true);
    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void Event_fini_w_pending_listener(void) {
    ecs_world_t *world = ev_world();

    ev_script(world, EV_WIDGET("press") LINE "Widget e");
    test_int(ecs_script_tasks_progress(world), 1);

    ecs_fini(world);
}

void Event_script_update_w_pending_listener(void) {
    ecs_world_t *world = ev_world();

    ecs_entity_t s = ev_script(world, EV_WIDGET("press") LINE "Widget e");
    test_int(ecs_script_tasks_progress(world), 1);

    test_int(ecs_script_update(world, s, 0,
        EV_WIDGET("press") LINE "Widget f"), 0);
    ecs_entity_t f = ecs_lookup(world, "f");
    test_assert(f != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    ev_mouse(world, f, 3, 4, 1);
    const ev_Widget *w = ev_widget(world, f);
    test_int(w->count, 1);
    test_flt(w->x, 3);

    ecs_fini(world);
}

void Event_mouse_event_w_stage(void) {
    ecs_world_t *world = ev_world();

    ev_script(world, EV_WIDGET("press") LINE "Widget e");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    ev_mouse(ecs_get_stage(world, 0), e, 0, 0, 1);
    test_int(ev_widget(world, e)->count, 1);

    ecs_fini(world);
}

void Event_mouse_event_in_progress(void) {
    ecs_world_t *world = ev_world();

    ev_script(world, EV_WIDGET("press") LINE "Widget e");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_progress(world, 0);
    ev_mouse(world, e, 0, 0, 1);
    test_int(ev_widget(world, e)->count, 1);
    ecs_progress(world, 0);
    ev_mouse(world, e, 0, 0, 0);
    ev_mouse(world, e, 0, 0, 1);
    test_int(ev_widget(world, e)->count, 2);

    ecs_fini(world);
}

void Event_import_enter_leave(void) {
    ecs_world_t *world = ev_world();

    test_assert(ecs_lookup(world, "flecs.script.on.enter") != 0);
    test_assert(ecs_lookup(world, "flecs.script.on.leave") != 0);

    ecs_fini(world);
}

void Event_enter_leave(void) {
    ecs_world_t *world = ev_world();

    ev_script(world,
        HEAD "using flecs.script"
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
        LINE "other {}");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t other = ecs_lookup(world, "other");
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(e != 0);
    test_assert(other != 0);

    test_int(ecs_script_tasks_progress(world), 2);
    const int64_t *m = ecs_get_id(world, e, mut);

    ev_mouse(world, 0, 0, 0, 0);
    test_int(m[0], 0);
    test_int(m[1], 0);

    ev_mouse(world, e, 1, 1, 0);
    m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 0);

    /* Staying on the entity doesn't enter again */
    ev_mouse(world, e, 2, 2, 0);
    m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 0);

    ev_mouse(world, other, 3, 3, 0);
    m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 1);

    ev_mouse(world, 0, 4, 4, 0);
    m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 1);

    ev_mouse(world, e, 5, 5, 0);
    ev_mouse(world, 0, 6, 6, 0);
    m = ecs_get_id(world, e, mut);
    test_int(m[0], 2);
    test_int(m[1], 2);

    ecs_fini(world);
}

void Event_enter_leave_between_children(void) {
    ecs_world_t *world = ev_world();

    ev_script(world,
        HEAD "using flecs.script"
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
        LINE "Widget e { a {}; b {} }");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t a = ecs_lookup(world, "e.a");
    ecs_entity_t b = ecs_lookup(world, "e.b");
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(e != 0);
    test_assert(a != 0);
    test_assert(b != 0);

    test_int(ecs_script_tasks_progress(world), 2);

    ev_mouse(world, a, 0, 0, 0);
    const int64_t *m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 0);

    /* Moving from one child to another stays inside the widget */
    ev_mouse(world, b, 1, 1, 0);
    m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 0);

    /* Moving from a child to the widget itself stays inside too */
    ev_mouse(world, e, 2, 2, 0);
    m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 0);

    ev_mouse(world, 0, 3, 3, 0);
    m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 1);

    ecs_fini(world);
}

void Event_enter_leave_children_listen(void) {
    ecs_world_t *world = ev_world();

    ev_script(world,
        HEAD "using flecs.script"
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
        LINE "parent { Widget a; Widget b }");
    ecs_entity_t a = ecs_lookup(world, "parent.a");
    ecs_entity_t b = ecs_lookup(world, "parent.b");
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(a != 0);
    test_assert(b != 0);

    test_int(ecs_script_tasks_progress(world), 4);

    ev_mouse(world, a, 0, 0, 0);
    const int64_t *ma = ecs_get_id(world, a, mut);
    const int64_t *mb = ecs_get_id(world, b, mut);
    test_int(ma[0], 1);
    test_int(ma[1], 0);
    test_int(mb[0], 0);
    test_int(mb[1], 0);

    ev_mouse(world, b, 1, 1, 0);
    ma = ecs_get_id(world, a, mut);
    mb = ecs_get_id(world, b, mut);
    test_int(ma[0], 1);
    test_int(ma[1], 1);
    test_int(mb[0], 1);
    test_int(mb[1], 0);

    ecs_fini(world);
}

void Event_enter_leave_while_dragging(void) {
    ecs_world_t *world = ev_world();

    ev_script(world,
        HEAD "using flecs.script"
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
        LINE "Widget e");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 3);

    ev_mouse(world, e, 0, 0, 1);
    const int64_t *m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 0);
    test_int(m[2], 1);

    /* Dragging off the entity leaves it; without pointer capture the host
     * reports no target, so no drag is delivered */
    ev_mouse(world, 0, 10, 10, 1);
    m = ecs_get_id(world, e, mut);
    test_int(m[0], 1);
    test_int(m[1], 1);
    test_int(m[2], 1);

    ev_mouse(world, e, 0, 0, 1);
    m = ecs_get_id(world, e, mut);
    test_int(m[0], 2);
    test_int(m[1], 1);
    test_int(m[2], 2);

    ecs_fini(world);
}

void Event_leave_deleted_hover_target(void) {
    ecs_world_t *world = ev_world();

    ev_script(world,
        HEAD "using flecs.script"
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
        LINE "Widget b");
    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    test_assert(a != 0);
    test_assert(b != 0);

    test_int(ecs_script_tasks_progress(world), 2);
    ev_mouse(world, a, 0, 0, 0);
    ecs_delete(world, a);
    ev_mouse(world, b, 1, 1, 0);
    test_int(*(int64_t*)ecs_get_id(world, b, mut), 1);

    ecs_fini(world);
}

void Event_returns_true(void) {
    ecs_world_t *world = ev_world();

    ev_script(world, EV_WIDGET("press") LINE "Widget e");
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_bool(ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = e, .buttons = 1 }), true);
    test_bool(ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = 0, .buttons = 0 }), true);

    ecs_fini(world);
}

void Event_no_propagation_without_host(void) {
    ecs_world_t *world = ev_world();

    ev_script(world, EV_WIDGET("press") LINE "Widget e { child {} }");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(e != 0);
    test_assert(child != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    /* The host only emits for the child: the parent doesn't get the event */
    ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = child, .buttons = 1 });
    test_int(ev_widget(world, e)->count, 0);

    ecs_fini(world);
}

void Event_propagation_translates_coordinates(void) {
    ecs_world_t *world = ev_world();

    ev_script(world, EV_WIDGET("press") LINE "Widget e { child {} }");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(e != 0);
    test_assert(child != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    ev_mouse_w_offset(world, child, 2, 3, 1, 100);
    const ev_Widget *w = ev_widget(world, e);
    test_int(w->count, 1);
    test_flt(w->x, 102);
    test_flt(w->y, 103);
    test_uint(w->target, e);

    ecs_fini(world);
}

void Event_propagation_keeps_deltas(void) {
    ecs_world_t *world = ev_world();

    ev_script(world, EV_WIDGET("drag") LINE "Widget e { child {} }");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(e != 0);
    test_assert(child != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    ev_mouse_w_offset(world, child, 0, 0, 1, 100);
    ev_mouse_w_offset(world, child, 5, 2, 1, 100);
    const ev_Widget *w = ev_widget(world, e);
    test_int(w->count, 2);
    test_flt(w->x, 105);
    test_flt(w->dx, 5);
    test_flt(w->dy, 2);

    ecs_fini(world);
}

void Event_propagation_press_once_per_level(void) {
    ecs_world_t *world = ev_world();

    ev_script(world,
        EV_WIDGET("press")
        LINE "Widget e { Widget child { Widget grandchild {} } }");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    ecs_entity_t grandchild = ecs_lookup(world, "e.child.grandchild");
    test_assert(e != 0);
    test_assert(child != 0);
    test_assert(grandchild != 0);

    test_int(ecs_script_tasks_progress(world), 3);

    ev_mouse(world, grandchild, 0, 0, 1);
    test_int(ev_widget(world, e)->count, 1);
    test_int(ev_widget(world, child)->count, 1);
    test_int(ev_widget(world, grandchild)->count, 1);

    /* Same pointer state on the same target again is a new event, not a
     * propagation, so nothing is pressed again */
    ev_mouse(world, grandchild, 0, 0, 1);
    test_int(ev_widget(world, e)->count, 1);
    test_int(ev_widget(world, child)->count, 1);
    test_int(ev_widget(world, grandchild)->count, 1);

    ecs_fini(world);
}

void Event_drag_not_propagated_to_unrelated_target(void) {
    ecs_world_t *world = ev_world();

    ev_script(world, EV_WIDGET("drag") LINE "Widget a" LINE "Widget b");
    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");
    test_assert(a != 0);
    test_assert(b != 0);

    test_int(ecs_script_tasks_progress(world), 2);

    ev_mouse(world, a, 0, 0, 1);
    /* A host without pointer capture reports the entity under the pointer */
    ev_mouse(world, b, 5, 5, 1);
    test_int(ev_widget(world, a)->count, 1);
    test_int(ev_widget(world, b)->count, 0);

    ecs_fini(world);
}
