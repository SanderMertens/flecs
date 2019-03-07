#include <include/api.h>

void New_w_Count_empty() {
    EcsWorld *world = ecs_init();

    EcsEntity e = ecs_new_w_count(world, 0, 1000, NULL);
    test_assert(e != 0);

    ecs_fini(world);
}

void New_w_Count_empty_w_handles() {
    EcsWorld *world = ecs_init();

    EcsEntity handles[1000];
    EcsEntity e = ecs_new_w_count(world, 0, 1000, handles);
    test_assert(e != 0);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(handles[i] == e + i);
    }

    ecs_fini(world);
}

void New_w_Count_nonempty_w_handles() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    EcsEntity handles[1000];
    EcsEntity e = ecs_new_w_count(world, Position, 1000, handles);
    test_assert(e != 0);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(handles[i] == e + i);
        test_assert(ecs_has(world, handles[i], Position));
    }

    ecs_fini(world);
}

void New_w_Count_component() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    EcsEntity e = ecs_new_w_count(world, Position, 1000, NULL);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void New_w_Count_type() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, MyType, Position);

    EcsEntity handles[1000];
    EcsEntity e = ecs_new_w_count(world, MyType, 1000, handles);
    test_assert(e != 0);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(handles[i] == e + i);
        test_assert(ecs_has(world, handles[i], Position));
    }

    ecs_fini(world);
}

void New_w_Count_type_of_2() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, MyType, Position, Velocity);

    EcsEntity handles[1000];
    EcsEntity e = ecs_new_w_count(world, MyType, 1000, handles);
    test_assert(e != 0);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(handles[i] == e + i);
        test_assert(ecs_has(world, handles[i], Position));
        test_assert(ecs_has(world, handles[i], Velocity));
    }

    ecs_fini(world);
}

void New_w_Count_type_w_type() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, Type_1, Position);
    ECS_TYPE(world, Type_2, Type_1);

    EcsEntity handles[1000];
    EcsEntity e = ecs_new_w_count(world, Type_2, 1000, handles);
    test_assert(e != 0);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(handles[i] == e + i);
        test_assert(ecs_has(world, handles[i], Position));
    }

    ecs_fini(world);
}

void New_w_Count_type_w_2_types() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type_1, Position);
    ECS_TYPE(world, Type_2, Velocity);
    ECS_TYPE(world, Type_3, Type_1, Type_2);

    EcsEntity handles[1000];
    EcsEntity e = ecs_new_w_count(world, Type_3, 1000, handles);
    test_assert(e != 0);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(handles[i] == e + i);
        test_assert(ecs_has(world, handles[i], Position));
        test_assert(ecs_has(world, handles[i], Velocity));
    }

    ecs_fini(world);
}

void New_w_Count_type_mixed() {
        EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type_1, Position);
    ECS_TYPE(world, Type_2, Type_1, Velocity);

    EcsEntity handles[1000];
    EcsEntity e = ecs_new_w_count(world, Type_2, 1000, handles);
    test_assert(e != 0);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(handles[i] == e + i);
        test_assert(ecs_has(world, handles[i], Position));
        test_assert(ecs_has(world, handles[i], Velocity));
    }

    ecs_fini(world);
}
