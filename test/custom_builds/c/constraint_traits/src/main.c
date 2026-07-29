#include <constraint_traits.h>

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ecs_entity_t likes = ecs_new(world);
    ecs_add_id(world, likes, EcsSymmetric);

    ecs_entity_t a = ecs_new(world);
    ecs_entity_t b = ecs_new(world);
    ecs_add_pair(world, a, likes, b);
    assert(ecs_has_pair(world, b, likes, a));

    ecs_entity_t rel = ecs_new(world);
    ecs_add_id(world, rel, EcsAcyclic);
    assert(ecs_has_id(world, rel, EcsAcyclic));

    assert(ecs_has_id(world, EcsChildOf, EcsRelationship));
    assert(ecs_has_id(world, EcsDontInherit, EcsTarget));

    return ecs_fini(world);
}
