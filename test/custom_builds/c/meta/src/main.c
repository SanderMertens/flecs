#include <meta.h>

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ecs_entity_t s = ecs_struct_init(world, &(ecs_struct_desc_t){
        .members = {
            { .name = "x", .type = ecs_id(ecs_i32_t) },
            { .name = "y", .type = ecs_id(ecs_i32_t) }
        }
    });
    assert(s != 0);

    return ecs_fini(world);
}
