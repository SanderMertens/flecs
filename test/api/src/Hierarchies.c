#include <api.h>

void Hierarchies_delete_children() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_entity_t child = ecs_new_w_entity(world, ECS_CHILDOF | e);
    test_assert(child != 0);

    ecs_delete(world, e);

    test_assert(ecs_get_type(world, child) == NULL);

    ecs_fini(world);
}
