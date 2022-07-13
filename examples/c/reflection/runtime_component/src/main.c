#include <runtime_component.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // Create component for a type that isn't known at compile time
    ecs_entity_t Position = ecs_struct(ecs, {
        .members = {
            { .name = "x", .type = ecs_id(ecs_f32_t) }, // builtin float type
            { .name = "y", .type = ecs_id(ecs_f32_t) }
        }
    });

    // Create entity, set value of position using reflection API
    ecs_entity_t ent = ecs_new_entity(ecs, "ent");
    void *ptr = ecs_get_mut_id(ecs, ent, Position);

    ecs_meta_cursor_t cur = ecs_meta_cursor(ecs, Position, ptr);
    ecs_meta_push(&cur);          // {
    ecs_meta_set_float(&cur, 10); //   10
    ecs_meta_next(&cur);          //   ,
    ecs_meta_set_float(&cur, 20); //   20
    ecs_meta_pop(&cur);           // }

    // Convert component to string
    char *str = ecs_ptr_to_expr(ecs, Position, ptr);
    printf("%s\n", str); // {x: 10, y: 20}
    ecs_os_free(str);

    ecs_fini(ecs);
}
