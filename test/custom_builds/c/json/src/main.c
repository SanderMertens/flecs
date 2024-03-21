#include <json.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    char *json = ecs_world_to_json(world, NULL);
    ecs_os_free(json);

    return ecs_fini(world);
}
