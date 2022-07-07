#include <basics_json.h>
#include <stdio.h>

typedef struct {
    float x, y;
} Position;

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // Register component as usual
    ECS_COMPONENT(ecs, Position);

    // Add reflection data to component
    ecs_struct_init(ecs, &(ecs_struct_desc_t){
        .entity.entity = ecs_id(Position), // Make sure to use existing id
        .members = {
            { .name = "x", .type = ecs_id(ecs_f32_t) }, // builtin float type
            { .name = "y", .type = ecs_id(ecs_f32_t) }
        }
    });

    // Create entity with Position as usual
    ecs_entity_t ent = ecs_new_entity(ecs, "ent");
    ecs_set(ecs, ent, Position, {10, 20});

    // Convert position component to JSON
    const Position *ptr = ecs_get(ecs, ent, Position);
    char *str = ecs_ptr_to_json(ecs, ecs_id(Position), ptr);
    printf("Position = %s\n", str);
    ecs_os_free(str);

    // Convert entity & all its components to json
    str = ecs_entity_to_json(ecs, ent, NULL);
    printf("ent = %s\n", str);
    ecs_os_free(str);

    ecs_fini(ecs);
}
