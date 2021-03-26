#include <api.h>

void Deprecated_trait_type_str() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Rel, 0);
    ECS_ENTITY(world, Obj, 0);

    ECS_TYPE(world, MyType, TRAIT | Rel > Obj);

    test_assert(MyType != 0);
    test_assert( ecs_type_has_id(world, ecs_type(MyType), ecs_trait(Obj, Rel)));

    ecs_fini(world);
}
