#include <script_platform.h>

int main(int, char *[]) {
    flecs::world ecs;

    ECS_IMPORT(ecs, FlecsScriptPlatform);

    assert(ecs_lookup(ecs, "flecs.script.platform") != 0);

    return 0;
}
