#include <script.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_mini();
    (void)argc;
    (void)argv;

    ECS_IMPORT(world, FlecsScript);

    return ecs_fini(world);
}
