#include <api.h>

void Add_remove_bulk_add_remove_add_only() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_bulk_new(world, Position, 10);
    test_assert(e != 0);

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    ecs_bulk_add_remove(world, Velocity, 0, &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( ecs_has(world, e + i, Velocity));

        const Position *p = ecs_get(world, e + i, Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2);

        test_assert(ecs_get(world, e + i, Velocity) != NULL);
    }

    test_int( ecs_count(world, Position), 10);
    test_int( ecs_count(world, Velocity), 10);

    ecs_fini(world);
}

void Add_remove_bulk_add_remove_remove_only() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t e = ecs_bulk_new(world, Type, 10);
    test_assert(e != 0);

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( ecs_has(world, e + i, Velocity));
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    ecs_bulk_add_remove(world, 0, Velocity, &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( !ecs_has(world, e + i, Velocity));

        const Position *p = ecs_get(world, e + i, Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2);
    }

    test_int( ecs_count(world, Position), 10);
    test_int( ecs_count(world, Velocity), 0);

    ecs_fini(world);
}

void Add_remove_bulk_add_remove_both() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t e = ecs_bulk_new(world, Type, 10);
    test_assert(e != 0);

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( ecs_has(world, e + i, Velocity));
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    ecs_bulk_add_remove(world, Mass, Velocity, &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( !ecs_has(world, e + i, Velocity));
        test_assert( ecs_has(world, e + i, Mass));

        const Position *p = ecs_get(world, e + i, Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2);

        test_assert(ecs_get(world, e + i, Mass) != NULL);
    }

    test_int( ecs_count(world, Position), 10);
    test_int( ecs_count(world, Velocity), 0);

    ecs_fini(world);
}

void Add_remove_bulk_add_remove_same() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t e = ecs_bulk_new(world, Type, 10);
    test_assert(e != 0);

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( ecs_has(world, e + i, Velocity));
        ecs_set(world, e + i, Position, {i, i * 2});
        ecs_set(world, e + i, Velocity, {i * 3, i * 4});
    }

    ecs_bulk_add_remove(world, Velocity, Velocity, &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( ecs_has(world, e + i, Velocity));

        const Position *p = ecs_get(world, e + i, Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2);

        const Velocity *v = ecs_get(world, e + i, Velocity);
        test_assert(v != NULL);
        test_int(v->x, i * 3);
        test_int(v->y, i * 4);
    }

    test_int( ecs_count(world, Position), 10);
    test_int( ecs_count(world, Velocity), 10);

    ecs_fini(world);
}
