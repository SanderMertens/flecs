#include <nested_set_member.h>
#include <stdio.h>

ECS_STRUCT(Point, {
    float x;
    float y;
});

ECS_STRUCT(Line, {
    Point start;
    Point stop;
});

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_META_COMPONENT(ecs, Point);
    ECS_META_COMPONENT(ecs, Line);

    ecs_entity_t ent = ecs_new_id(ecs);
    Line *ptr = ecs_get_mut(ecs, ent, Line);

    ecs_meta_cursor_t cur = ecs_meta_cursor(ecs, ecs_id(Line), ptr);
    ecs_meta_push(&cur);            // {
    ecs_meta_member(&cur, "start"); //   start:
    ecs_meta_push(&cur);            //   {
    ecs_meta_member(&cur, "x");     //     x:
    ecs_meta_set_float(&cur, 10);   //     10
    ecs_meta_member(&cur, "y");     //     y:
    ecs_meta_set_float(&cur, 20);   //     20
    ecs_meta_pop(&cur);             //   }
    ecs_meta_member(&cur, "stop");  //   stop:
    ecs_meta_push(&cur);            //   {
    ecs_meta_member(&cur, "x");     //     x:
    ecs_meta_set_float(&cur, 30);   //     30
    ecs_meta_member(&cur, "y");     //     y:
    ecs_meta_set_float(&cur, 40);   //     40
    ecs_meta_pop(&cur);             //   }
    ecs_meta_pop(&cur);             // }

    char *str = ecs_ptr_to_expr(ecs, ecs_id(Line), ptr);
    printf("%s\n", str);
    ecs_os_free(str);
    
    // {start: {x: 10.00, y: 20.00}, stop: {x: 30.00, y: 40.00}}

    return ecs_fini(ecs);
}
