#include <basics_enum.h>
#include <stdio.h>

typedef enum {
    Red,
    Green,
    Blue
} Color;

typedef struct {
    Color color;
} TypeWithEnum;

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // Register component as usual
    ECS_COMPONENT(ecs, Color);
    ECS_COMPONENT(ecs, TypeWithEnum);

    // Add reflection data to components
    ecs_enum(ecs, {
        .entity = ecs_id(Color), // Make sure to use existing id
        .constants = {
            { .name = "Red", .value = Red },
            { .name = "Green", .value = Green },
            { .name = "Blue", .value = Blue }
        }
    });

    ecs_struct(ecs, {
        .entity = ecs_id(TypeWithEnum), // Make sure to use existing id
        .members = {
            { .name = "color", .type = ecs_id(Color) }
        }
    });

    // Create entity with Position as usual
    ecs_entity_t ent = ecs_new_id(ecs);
    ecs_set(ecs, ent, TypeWithEnum, {Green});

    // Convert position component to flecs expression string
    const TypeWithEnum *ptr = ecs_get(ecs, ent, TypeWithEnum);
    char *str = ecs_ptr_to_expr(ecs, ecs_id(TypeWithEnum), ptr);
    printf("%s\n", str); // {color: Green}
    ecs_os_free(str);

    ecs_fini(ecs);
}
