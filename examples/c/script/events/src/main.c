#include <events.h>
#include <stdio.h>

// This example shows how to use mouse, keyboard and focus events in scripts.
// The script describes a panel with a text field, a slider and a button. Each
// widget uses async blocks to wait for the events it is interested in.
//
// The application is responsible for figuring out which entity is under the
// mouse, and sends events with ecs_script_mouse_event() and
// ecs_script_keyboard_event(). Events are first delivered to their target, and
// then propagate to the parents of the target. Keyboard events are delivered to
// the entity that has the focus.

// Tag for entities that can get the focus
ECS_TAG_DECLARE(Focusable);

// Position of an entity relative to its parent
typedef struct {
    float x, y;
} Position;

ECS_COMPONENT_DECLARE(Position);

const char *code =
    "using flecs.script\n"
    "\n"

    // A button counts how often it is activated. The activate event is sent
    // when the button is clicked, and when Enter or Space is pressed while the
    // button has the focus. A click on a child of the button (like its label)
    // propagates to the button.
    "template Button {\n"
    "  Focusable\n"
    "  mut count = 0\n"
    "  async {\n"
    "    while true {\n"
    "      const e = await on.activate(this)\n"
    "      count = count + 1\n"
    "      if e.keyboard {\n"
    "        await print(\"{this.name()}: activated by key ({count})\")\n"
    "      } else {\n"
    "        await print(\"{this.name()}: activated by click ({count})\")\n"
    "      }\n"
    "    }\n"
    "  }\n"
    "}\n"
    "\n"

    // A slider changes its value when it is dragged. A drag event is delivered
    // for each mouse movement while a button is held down, with the distance
    // moved since the previous event in delta_x and delta_y.
    "template Slider {\n"
    "  Focusable\n"
    "  mut value = 50\n"
    "  async {\n"
    "    while true {\n"
    "      const e = await on.drag(this)\n"
    "      if e.delta_x != 0 {\n"
    "        value = value + e.delta_x\n"
    "        await print(\"{this.name()}: dragged to {value}\")\n"
    "      }\n"
    "    }\n"
    "  }\n"
    "  async {\n"
    "    while true {\n"
    "      const e = await on.key_down(this)\n"
    "      if e.key == \"ArrowRight\" {\n"
    "        value = value + 1\n"
    "        await print(\"{this.name()}: stepped to {value}\")\n"
    "      }\n"
    "    }\n"
    "  }\n"
    "}\n"
    "\n"

    // A text field reacts to getting and losing the focus, and adds the keys
    // that are typed to its text. Typed keys stop propagating, so that they are
    // not handled by the panel as well. Escape does propagate.
    "template TextField {\n"
    "  Focusable\n"
    "  mut text = \"\"\n"
    "  async {\n"
    "    while true {\n"
    "      await on.focus(this)\n"
    "      await print(\"{this.name()}: focused\")\n"
    "    }\n"
    "  }\n"
    "  async {\n"
    "    while true {\n"
    "      await on.blur(this)\n"
    "      await print(\"{this.name()}: lost focus\")\n"
    "    }\n"
    "  }\n"
    "  async {\n"
    "    while true {\n"
    "      const e = await on.key_down(this)\n"
    "      if e.key != \"Escape\" {\n"
    "        text = \"{text}{e.key}\"\n"
    "        await print(\"{this.name()}: text is '{text}'\")\n"
    "        await e.stop_propagation()\n"
    "      }\n"
    "    }\n"
    "  }\n"
    "}\n"
    "\n"

    // The panel receives the events of its children. It gets a focus event
    // whenever one of its children gets the focus, and handles keys that its
    // children let through. Escape clears the focus with the focus() function.
    // When a press propagates to the panel, local_x and local_y are relative to
    // the panel instead of to the entity that was pressed.
    "template Panel {\n"
    "  async {\n"
    "    while true {\n"
    "      const e = await on.press(this)\n"
    "      const x: i32 = e.local_x\n"
    "      const y: i32 = e.local_y\n"
    "      await print(\"{this.name()}: press at {x}, {y}\")\n"
    "    }\n"
    "  }\n"
    "  async {\n"
    "    while true {\n"
    "      const e = await on.focus(this)\n"
    "      await print(\"{this.name()}: focus moved to {e.target.name()}\")\n"
    "    }\n"
    "  }\n"
    "  async {\n"
    "    while true {\n"
    "      const e = await on.key_down(this)\n"
    "      if e.key == \"Escape\" {\n"
    "        await focus(0)\n"
    "        await print(\"{this.name()}: Escape cleared the focus\")\n"
    "      } else {\n"
    "        await print(\"{this.name()}: received key {e.key}\")\n"
    "      }\n"
    "    }\n"
    "  }\n"
    "}\n"
    "\n"
    "Panel panel {\n"
    "  Position: {100, 100}\n"
    "  TextField name { Position: {10, 10} }\n"
    "  Slider volume { Position: {10, 40} }\n"
    "  Button ok {\n"
    "    Position: {10, 70}\n"
    "    label { Position: {5, 5} }\n"
    "  }\n"
    "}\n";

// Print function that can be called from async blocks. Async functions return
// a future, which this function resolves immediately.
static void print(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_script_future_t *future)
{
    (void)ctx;
    (void)argc;
    bool ok = true;
    printf("  %s\n", *(char**)argv[0].ptr);
    ecs_script_future_resolve(future,
        &(ecs_value_t){ .type = ecs_id(ecs_bool_t), .ptr = &ok });
    ecs_script_future_release(future);
}

// Hook that decides which entity gets the focus. When a mouse button is
// pressed, the focus goes to the first entity with the Focusable tag, starting
// from the entity under the mouse. A press on an entity without a Focusable
// parent (like the panel background) keeps the focus where it is.
static ecs_entity_t to_focus(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t current,
    ecs_script_focus_source_t source,
    void *ctx)
{
    (void)ctx;

    // Accept focus changes from the application and from scripts as they are
    if (source != EcsScriptFocusPress) {
        return entity;
    }

    for (; entity; entity = ecs_get_parent(world, entity)) {
        if (ecs_has(world, entity, Focusable)) {
            return entity;
        }
    }

    return current;
}

// Hook that converts a screen position to a position relative to an entity.
// When a mouse event propagates to a parent, the event module calls this hook
// to compute local_x and local_y for the parent.
static void to_local(
    ecs_world_t *world,
    ecs_entity_t entity,
    float screen_x,
    float screen_y,
    float *local_x,
    float *local_y,
    void *ctx)
{
    (void)ctx;

    *local_x = screen_x;
    *local_y = screen_y;

    // Subtract the positions of the entity and its parents
    for (; entity; entity = ecs_get_parent(world, entity)) {
        const Position *p = ecs_get(world, entity, Position);
        if (p) {
            *local_x -= p->x;
            *local_y -= p->y;
        }
    }
}

// Send a mouse event. The target is the entity under the mouse, and buttons is
// a bitmask of the buttons that are held down. The event module derives press,
// drag, release and click events from changes in the buttons. The application
// provides the position relative to the target, which here uses the same
// function as the to_local hook.
static void mouse(
    ecs_world_t *world,
    ecs_entity_t target,
    float x,
    float y,
    uint32_t buttons)
{
    EcsScriptMouseEvent evt = {
        .target = target,
        .screen_x = x, .screen_y = y,
        .buttons = buttons
    };

    to_local(world, target, x, y, &evt.local_x, &evt.local_y, NULL);
    ecs_script_mouse_event(world, &evt);
}

// Send a key down and key up event to the entity with the focus
static void key(
    ecs_world_t *world,
    const char *name)
{
    char *k = ECS_CONST_CAST(char*, name);
    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = k, .down = true });
    ecs_script_keyboard_event(world, &(EcsScriptKeyboardEvent){
        .key = k, .down = false });
}

static void print_focus(
    ecs_world_t *world)
{
    ecs_entity_t focus = ecs_script_get_focus(world);
    printf("  [app] focus is %s\n", focus ? ecs_get_name(world, focus) : "none");
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    // Import the module that adds the on.* functions to scripts
    ECS_IMPORT(world, FlecsScriptInput);

    ECS_TAG_DEFINE(world, Focusable);
    ECS_COMPONENT_DEFINE(world, Position);

    // Add reflection data to Position so it can be assigned in the script
    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            { .name = "x", .type = ecs_id(ecs_f32_t) },
            { .name = "y", .type = ecs_id(ecs_f32_t) }
        }
    });

    ecs_async_function(world, {
        .name = "print",
        .return_type = ecs_id(ecs_bool_t),
        .params = {{ .name = "msg", .type = ecs_id(ecs_string_t) }},
        .callback = print
    });

    ecs_script_set_event_hooks(world, &(ecs_script_event_hooks_t){
        .to_local = to_local,
        .to_focus = to_focus
    });

    if (!ecs_script(world, { .code = code })) {
        printf("script failed to run\n");
        return -1;
    }

    // Run the async blocks until they wait for their first event
    ecs_script_tasks_progress(world);

    ecs_entity_t panel = ecs_lookup(world, "panel");
    ecs_entity_t name = ecs_lookup(world, "panel.name");
    ecs_entity_t volume = ecs_lookup(world, "panel.volume");
    ecs_entity_t ok = ecs_lookup(world, "panel.ok");
    ecs_entity_t label = ecs_lookup(world, "panel.ok.label");

    // The click on the label propagates to the button. The to_focus hook moves
    // the focus from the label to the button.
    printf("Click the label of the ok button:\n");
    mouse(world, label, 117, 177, 1);
    mouse(world, label, 117, 177, 0);
    print_focus(world);

    // Keys typed into the text field don't reach the panel
    printf("\nClick the name field and type:\n");
    mouse(world, name, 120, 115, 1);
    mouse(world, name, 120, 115, 0);
    key(world, "h");
    key(world, "i");

    // The panel is not Focusable, so the focus stays on the text field
    printf("\nClick the panel background:\n");
    mouse(world, panel, 180, 190, 1);
    mouse(world, panel, 180, 190, 0);
    print_focus(world);

    // Move the mouse to the slider, press, move while pressed and release
    printf("\nDrag the volume slider:\n");
    mouse(world, volume, 120, 145, 0);
    mouse(world, volume, 120, 145, 1);
    mouse(world, volume, 140, 145, 1);
    mouse(world, volume, 155, 145, 1);
    mouse(world, volume, 155, 145, 0);

    // The slider doesn't stop the key event, so it propagates to the panel
    printf("\nPress a key on the slider:\n");
    key(world, "ArrowRight");

    // Applications can set the focus directly, for example to implement Tab
    printf("\nMove the focus from the application:\n");
    ecs_script_set_focus(world, ok);
    key(world, "Enter");

    printf("\nPress Escape:\n");
    key(world, "Escape");
    print_focus(world);

    return ecs_fini(world);

    // Output
    //  Click the label of the ok button:
    //    panel: focus moved to ok
    //    panel: press at 17, 77
    //    ok: activated by click (1)
    //    [app] focus is ok
    //
    //  Click the name field and type:
    //    name: focused
    //    panel: focus moved to name
    //    panel: press at 20, 15
    //    name: text is 'h'
    //    name: text is 'hi'
    //
    //  Click the panel background:
    //    panel: press at 80, 90
    //    [app] focus is name
    //
    //  Drag the volume slider:
    //    name: lost focus
    //    panel: focus moved to volume
    //    panel: press at 20, 45
    //    volume: dragged to 70
    //    volume: dragged to 85
    //
    //  Press a key on the slider:
    //    volume: stepped to 86
    //    panel: received key ArrowRight
    //
    //  Move the focus from the application:
    //    panel: focus moved to ok
    //    ok: activated by key (2)
    //    panel: received key Enter
    //
    //  Press Escape:
    //    panel: Escape cleared the focus
    //    [app] focus is none
}
