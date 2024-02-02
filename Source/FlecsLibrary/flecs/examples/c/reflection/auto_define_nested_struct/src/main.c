#include <auto_define_nested_struct.h>
#include <stdio.h>

// Point is used by Line, so it must also be captured
ECS_STRUCT(Point, {
    int32_t x;
    int32_t y;
});

ECS_STRUCT(Line, {
    Point start;
    Point stop;
});

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // Register both components. Note that Point is registered first, so that
    // when Line is registered, the type information of Point can be looked up
    ECS_META_COMPONENT(ecs, Point);
    ECS_META_COMPONENT(ecs, Line);

    // Serialize as usual
    Line value = {{1, 2}, {3, 4}};
    char *json = ecs_ptr_to_json(ecs, ecs_id(Line), &value);
    printf("%s\n", json); // {"start": {"x": 1, "y": 2}, "stop": {"x": 3, "y": 4}}
    ecs_os_free(json);

    return ecs_fini(ecs);
}
