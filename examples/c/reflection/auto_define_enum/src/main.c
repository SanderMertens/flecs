#include <auto_define_enum.h>
#include <stdio.h>

// Color is used by Car, so it must also be captured
ECS_ENUM(Color, {
    Red, Black, White, StainlessSteel
});

ECS_STRUCT(Car, {
    const char *brand;
    Color color;
    float speed;
});

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // Register both components. Note that Color is registered first, so that
    // when Line is registered, the type information of Color can be looked up
    ECS_META_COMPONENT(ecs, Color);
    ECS_META_COMPONENT(ecs, Car);

    // Serialize as usual
    Car value = {"Delorean", StainlessSteel, 1.21f};
    char *json = ecs_ptr_to_json(ecs, ecs_id(Car), &value);
    printf("%s\n", json); // {"brand": "Delorean", "color": "StainlessSteel", "speed": 1.21}
    ecs_os_free(json);

    return ecs_fini(ecs);
}
