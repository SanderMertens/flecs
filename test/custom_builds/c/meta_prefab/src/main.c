#include <meta_prefab.h>

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ecs_entity_t base = ecs_struct_init(world, &(ecs_struct_desc_t){
        .members = {
            { .name = "x", .type = ecs_id(ecs_i32_t) },
            { .name = "y", .type = ecs_id(ecs_i32_t) }
        }
    });
    assert(base != 0);

    /* A struct that inherits its members from a base struct */
    ecs_entity_t s = ecs_struct_init(world, &(ecs_struct_desc_t){0});
    assert(s != 0);
    ecs_add_pair(world, s, EcsIsA, base);

    const EcsStruct *st = ecs_get(world, s, EcsStruct);
    assert(st != NULL);
    assert(ecs_vec_count(&st->members) == 2);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    assert(ecs_has_pair(world, i, EcsIsA, p));

    return ecs_fini(world);
}
