#include <api.h>

void New_w_Count_empty() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_w_count(world, 0, 1000);
    test_assert(e != 0);

    ecs_fini(world);
}

void New_w_Count_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w_count(world, Position, 1000);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_int(ecs_count(world, Position), 1000);

    ecs_fini(world);
}

void New_w_Count_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, MyType, Position);

    ecs_entity_t e = ecs_new_w_count(world, MyType, 1000);
    test_assert(e != 0);
    test_int(ecs_count(world, MyType), 1000);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(ecs_has(world, e + i, Position));
    }

    ecs_fini(world);
}

void New_w_Count_type_of_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, MyType, Position, Velocity);

    ecs_entity_t e = ecs_new_w_count(world, MyType, 1000);
    test_assert(e != 0);
    test_int(ecs_count(world, MyType), 1000);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(ecs_has(world, e + i, Position));
        test_assert(ecs_has(world, e + i, Velocity));
    }

    ecs_fini(world);
}

void New_w_Count_type_w_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, Type_1, Position);
    ECS_TYPE(world, Type_2, Type_1);

    ecs_entity_t e = ecs_new_w_count(world, Type_2, 1000);
    test_assert(e != 0);
    test_int(ecs_count(world, Type_2), 1000);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(ecs_has(world, e + i, Position));
    }

    ecs_fini(world);
}

void New_w_Count_type_w_2_types() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type_1, Position);
    ECS_TYPE(world, Type_2, Velocity);
    ECS_TYPE(world, Type_3, Type_1, Type_2);

    ecs_entity_t e = ecs_new_w_count(world, Type_3, 1000);
    test_assert(e != 0);
    test_int(ecs_count(world, Type_3), 1000);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(ecs_has(world, e + i, Position));
        test_assert(ecs_has(world, e + i, Velocity));
    }

    ecs_fini(world);
}

void New_w_Count_type_mixed() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type_1, Position);
    ECS_TYPE(world, Type_2, Type_1, Velocity);

    ecs_entity_t e = ecs_new_w_count(world, Type_2, 1000);
    test_assert(e != 0);
    test_int(ecs_count(world, Type_2), 1000);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(ecs_has(world, e + i, Position));
        test_assert(ecs_has(world, e + i, Velocity));
    }

    ecs_fini(world);
}

void New_w_Count_tag() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_w_count(world, Tag, 1000);
    test_assert(e != 0);
    test_int(ecs_count(world, Tag), 1000);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(ecs_has(world, e + i, Tag));
    }

    ecs_fini(world);
}

void New_w_Count_type_w_tag() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TYPE(world, Type, Tag);

    ecs_entity_t e = ecs_new_w_count(world, Type, 1000);
    test_assert(e != 0);
    test_int(ecs_count(world, Type), 1000);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(ecs_has(world, e + i, Tag));
    }

    ecs_fini(world);
}

void New_w_Count_type_w_2_tags() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag_1);
    ECS_TAG(world, Tag_2);
    ECS_TYPE(world, Type, Tag_1, Tag_2);

    ecs_entity_t e = ecs_new_w_count(world, Type, 1000);
    test_assert(e != 0);
    test_int(ecs_count(world, Type), 1000);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(ecs_has(world, e + i, Tag_1));
        test_assert(ecs_has(world, e + i, Tag_2));
    }

    ecs_fini(world);
}

void New_w_Count_type_w_tag_mixed() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag_1);
    ECS_TAG(world, Tag_2);
    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, Type, Tag_1, Tag_2, Position);

    ecs_entity_t e = ecs_new_w_count(world, Type, 1000);
    test_assert(e != 0);
    test_int(ecs_count(world, Type), 1000);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(ecs_has(world, e + i, Tag_1));
        test_assert(ecs_has(world, e + i, Tag_2));
        test_assert(ecs_has(world, e + i, Position));
    }

    ecs_fini(world);
}
