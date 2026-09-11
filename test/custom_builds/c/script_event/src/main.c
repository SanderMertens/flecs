#include <script_event.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_IMPORT(world, FlecsScriptEvent);

    assert(ecs_lookup(world, "flecs.script.on.press") != 0);
    assert(ecs_lookup(world, "flecs.script.on.key_down") != 0);

    ecs_entity_t s = ecs_script(world, {
        .code =
            "using flecs.script\n"
            "struct Position(x: f32, y: f32)\n"
            "struct Rect(width: f32, height: f32)\n"
            "template Widget {\n"
            "  mut count = 0\n"
            "  bg { Position: {}; Rect: {10, 10} }\n"
            "  async {\n"
            "    await on.press(this)\n"
            "    count = count + 1\n"
            "  }\n"
            "}\n"
            "Widget e\n"
    });
    assert(s != 0);
    assert(ecs_get(world, s, EcsScript)->error == NULL);
    (void)s;

    ecs_script_tasks_progress(world);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t mut = ecs_lookup(world, "Widget.mut");
    assert(e != 0 && mut != 0);

    EcsScriptMouseEvent evt = { .target = e, .buttons = 1 };
    ecs_script_mouse_event(world, &evt);
    assert(*(const int64_t*)ecs_get_id(world, e, mut) == 1);
    (void)mut;

    return ecs_fini(world);
}
