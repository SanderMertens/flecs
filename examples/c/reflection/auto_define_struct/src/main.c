#include <auto_define_struct.h>
#include <stdio.h>

// This type captured is used later to inject reflection data for Position
ECS_STRUCT(Position, {
    float x;
    float y;
});

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // This registers the component with reflection data
    ECS_META_COMPONENT(ecs, Position);

    // Serialize as usual
    Position value = {10, 20};
    char *json = ecs_ptr_to_json(ecs, ecs_id(Position), &value);
    printf("%s\n", json); // {"x": 10.0, "y": 20.0}
    ecs_os_free(json);

    return ecs_fini(ecs);
}
