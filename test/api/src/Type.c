#include <api.h>

void Type_type_of_1_tostr() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_TYPE(world, Type, Position);

    char *str = ecs_type_str(world, Type);
    
    test_str(str, "Position");

    free(str);

    ecs_fini(world);
}

void Type_type_of_2_tostr() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type, Position, Velocity);

    char *str = ecs_type_str(world, Type);
    
    test_str(str, "Position,Velocity");

    free(str);

    ecs_fini(world);
}

void Type_type_of_2_tostr_no_id() {
    ecs_world_t *world = ecs_init();

    ecs_type_t t_1 = ecs_type_from_entity(world, 100);
    ecs_type_t t_2 = ecs_type_from_entity(world, 200);
    ecs_type_t t = 0;
    t = _ecs_merge_type(world, t, t_1, 0);
    t = _ecs_merge_type(world, t, t_2, 0);
    
    char *str = _ecs_type_str(world, t);
    
    test_str(str, "100,200");

    free(str);

    ecs_fini(world);
}

void Type_type_redefine() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t type = ecs_new_type(world, "Type", "Position");

    test_assert(type == ecs_new_type(world, "Type", "Position"));

    ecs_fini(world);
}
