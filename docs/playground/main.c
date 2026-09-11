#include <flecs.h>
#include <emscripten.h>

/* Host application for the Flecs playground wasm image. Mouse and keyboard
 * events from the page are forwarded to the flecs.script.event module, which
 * delivers them to the async blocks of scripts. */

static ecs_world_t *playground_world;

/* Called by the page for pointer events on the 2D/3D views. The target is the
 * path of the entity the event is delivered to (empty when there is none), x
 * and y the pointer position relative to that entity. Returns whether the
 * page should propagate the event to the parent of the target. */
EMSCRIPTEN_KEEPALIVE
int flecs_playground_mouse(
    const char *target,
    float screen_x,
    float screen_y,
    float x,
    float y,
    int button,
    int buttons,
    int ctrl,
    int shift,
    int alt,
    int meta)
{
    ecs_world_t *world = playground_world;
    if (!world) {
        return 0;
    }

    return ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
        .target = target && target[0] ? ecs_lookup(world, target) : 0,
        .screen_x = screen_x,
        .screen_y = screen_y,
        .local_x = x,
        .local_y = y,
        .button = button,
        .buttons = (uint32_t)buttons,
        .ctrl = ctrl != 0,
        .shift = shift != 0,
        .alt = alt != 0,
        .meta = meta != 0
    });
}

/* Called by the page for keyboard events while a view has focus. */
EMSCRIPTEN_KEEPALIVE
void flecs_playground_key(
    const char *key,
    int down,
    int repeat,
    int ctrl,
    int shift,
    int alt,
    int meta)
{
    if (!playground_world || !key) {
        return;
    }

    ecs_script_keyboard_event(playground_world, &(EcsScriptKeyboardEvent){
        .key = ECS_CONST_CAST(char*, key),
        .down = down != 0,
        .repeat = repeat != 0,
        .ctrl = ctrl != 0,
        .shift = shift != 0,
        .alt = alt != 0,
        .meta = meta != 0
    });
}

static void playground_frame(void *world) {
    ecs_progress(world, 0);
}

static int playground_run(ecs_world_t *world, ecs_app_desc_t *desc) {
    (void)desc;
    emscripten_set_main_loop_arg(playground_frame, world, 0, false);
    return 0;
}

int main(void) {
    ecs_world_t *world = ecs_init();
    playground_world = world;

    ECS_IMPORT(world, FlecsUnits);
    ECS_IMPORT(world, FlecsScriptMath);
    ECS_IMPORT(world, FlecsScriptPlatform);
    ECS_IMPORT(world, FlecsScriptEvent);
    ECS_IMPORT(world, FlecsAlerts);
    ECS_IMPORT(world, FlecsMetrics);

    ecs_script(world, {
        .entity = ecs_entity(world, { .name = "playground" }),
        .code = ""
    });

    ecs_app_set_run_action(playground_run);

    /* Expose the input entry points to the page. The explorer's flecs.js
     * client keeps the module instance to itself, so the image publishes the
     * functions on the window object. */
    EM_ASM({
        var bridge = {};
        bridge.mouse = Module.cwrap("flecs_playground_mouse", "number",
            "string number number number number number number number number number number".split(" "));
        bridge.key = Module.cwrap("flecs_playground_key", null,
            "string number number number number number number".split(" "));
        bridge.request = Module.cwrap("flecs_explorer_request", "string",
            "string string string".split(" "));
        window.flecsPlayground = bridge;
    });

    return ecs_app_run(world, &(ecs_app_desc_t){
        .enable_rest = true,
        .enable_stats = true
    });
}
