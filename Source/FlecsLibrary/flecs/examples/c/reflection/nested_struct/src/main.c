#include <nested_struct.h>
#include <stdio.h>

typedef struct {
    float x, y;
} Point;

typedef struct {
    Point start, stop;
} Line;

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, Point);
    ECS_COMPONENT(ecs, Line);

    ecs_struct(ecs, {
        .entity = ecs_id(Point),
        .members = {
            { .name = "x", .type = ecs_id(ecs_f32_t) },
            { .name = "y", .type = ecs_id(ecs_f32_t) }
        }
    });

    ecs_struct(ecs, {
        .entity = ecs_id(Line),
        .members = {
            { .name = "start", .type = ecs_id(Point) },
            { .name = "stop", .type = ecs_id(Point) }
        }
    });

    ecs_entity_t ent = ecs_new_id(ecs);
    ecs_set(ecs, ent, Line, {{10, 20}, {30, 40}});

    const Line *ptr = ecs_get(ecs, ent, Line);
    char *str = ecs_ptr_to_expr(ecs, ecs_id(Line), ptr);
    printf("%s\n", str); // {start: {x: 10, y: 20}, stop: {x: 30, y: 40}}
    ecs_os_free(str);

    return ecs_fini(ecs);
}
