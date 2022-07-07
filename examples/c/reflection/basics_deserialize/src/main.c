#include <basics_deserialize.h>
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

    // Create entity, set value of position using reflection API
    ecs_entity_t ent = ecs_new_entity(ecs, "ent");
    Position *ptr = ecs_get_mut(ecs, ent, Position);

    ecs_meta_cursor_t cur = ecs_meta_cursor(ecs, ecs_id(Position), ptr);
    ecs_meta_push(&cur);          // {
    ecs_meta_set_float(&cur, 10); //   10
    ecs_meta_next(&cur);          //   ,
    ecs_meta_set_float(&cur, 20); //   20
    ecs_meta_pop(&cur);           // }

    char *str = ecs_ptr_to_expr(ecs, ecs_id(Position), ptr);
    printf("%s\n", str); // {x: 10.00, y: 20.00}
    ecs_os_free(str);

    // Use member names before assigning values
    cur = ecs_meta_cursor(ecs, ecs_id(Position), ptr);
    ecs_meta_push(&cur);          // {
    ecs_meta_member(&cur, "y");   //   y: 
    ecs_meta_set_float(&cur, 10); //   10
    ecs_meta_member(&cur, "x");   //   x: 
    ecs_meta_set_float(&cur, 20); //   20
    ecs_meta_pop(&cur);           // }

    str = ecs_ptr_to_expr(ecs, ecs_id(Position), ptr);
    printf("%s\n", str); // {x: 20.00, y: 10.00}
    ecs_os_free(str);

    ecs_fini(ecs);
}
