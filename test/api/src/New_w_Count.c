#include <api.h>

void New_w_Count_empty() {
    ecs_world_t *world = ecs_mini();

    const ecs_entity_t *ids = ecs_bulk_new(world, 0, 1000);
    test_assert(ids != NULL);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(ids[i] != 0);
    }

    ecs_fini(world);
}

void New_w_Count_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    const ecs_entity_t *ids = ecs_bulk_new(world, Position, 1000);
    test_assert(ids != NULL);
    
    int i;
    for (i = 0; i < 1000; i ++) {
        ecs_entity_t e = ids[i];
        test_assert(e != 0);
        test_assert(ecs_has(world, e, Position));
    }

    test_int(ecs_count(world, Position), 1000);

    ecs_fini(world);
}

void New_w_Count_tag() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, 0);

    const ecs_entity_t *ids = ecs_bulk_new_w_id(world, Tag, 1000);
    test_assert(ids != NULL);
    test_int(ecs_count_id(world, Tag), 1000);

    int i;
    for (i = 0; i < 1000; i ++) {
        ecs_entity_t e = ids[i];
        test_assert(e != 0);
        test_assert(ecs_has_id(world, e, Tag));
    }

    ecs_fini(world);
}

void New_w_Count_bulk_init_empty() {
    ecs_world_t *world = ecs_mini();

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 3
    });

    test_assert(entities != NULL);

    test_assert(entities[0] != 0);
    test_assert(entities[1] != 0);
    test_assert(entities[2] != 0);

    test_assert(entities[0] != entities[1]);
    test_assert(entities[0] != entities[2]);
    test_assert(entities[0] != entities[1]);
    test_assert(entities[1] != entities[2]);

    test_assert(ecs_is_alive(world, entities[0]));
    test_assert(ecs_is_alive(world, entities[1]));
    test_assert(ecs_is_alive(world, entities[2]));

    ecs_fini(world);
}

void New_w_Count_bulk_init_empty_w_entities() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t ents[] = {1000, 1001, 1002};

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .entities = ents,
        .count = 3
    });

    test_assert(entities != NULL);

    test_assert(entities[0] != 0);
    test_assert(entities[1] != 0);
    test_assert(entities[2] != 0);

    test_uint(entities[0], 1000);
    test_uint(entities[1], 1001);
    test_uint(entities[2], 1002);

    test_assert(ecs_is_alive(world, entities[0]));
    test_assert(ecs_is_alive(world, entities[1]));
    test_assert(ecs_is_alive(world, entities[2]));

    ecs_fini(world);
}

void New_w_Count_bulk_init_1_tag() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 3,
        .ids = {Tag}
    });

    test_assert(entities != NULL);

    test_assert(entities[0] != 0);
    test_assert(entities[1] != 0);
    test_assert(entities[2] != 0);

    test_assert(entities[0] != entities[1]);
    test_assert(entities[0] != entities[2]);
    test_assert(entities[0] != entities[1]);
    test_assert(entities[1] != entities[2]);

    test_assert(ecs_is_alive(world, entities[0]));
    test_assert(ecs_is_alive(world, entities[1]));
    test_assert(ecs_is_alive(world, entities[2]));

    test_assert(ecs_has(world, entities[0], Tag));
    test_assert(ecs_has(world, entities[1], Tag));
    test_assert(ecs_has(world, entities[2], Tag));

    ecs_fini(world);
}

void New_w_Count_bulk_init_1_tag_w_entities() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    
    ecs_entity_t ents[] = {1000, 1001, 1002};

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .entities = ents,
        .count = 3,
        .ids = {Tag}
    });

    test_assert(entities != NULL);

    test_assert(entities[0] != 0);
    test_assert(entities[1] != 0);
    test_assert(entities[2] != 0);

    test_uint(entities[0], 1000);
    test_uint(entities[1], 1001);
    test_uint(entities[2], 1002);

    test_assert(ecs_is_alive(world, entities[0]));
    test_assert(ecs_is_alive(world, entities[1]));
    test_assert(ecs_is_alive(world, entities[2]));

    test_assert(ecs_has(world, entities[0], Tag));
    test_assert(ecs_has(world, entities[1], Tag));
    test_assert(ecs_has(world, entities[2], Tag));

    ecs_fini(world);
}

void New_w_Count_bulk_init_2_tags() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 3,
        .ids = {TagA, TagB}
    });

    test_assert(entities != NULL);

    test_assert(entities[0] != 0);
    test_assert(entities[1] != 0);
    test_assert(entities[2] != 0);

    test_assert(entities[0] != entities[1]);
    test_assert(entities[0] != entities[2]);
    test_assert(entities[0] != entities[1]);
    test_assert(entities[1] != entities[2]);

    test_assert(ecs_is_alive(world, entities[0]));
    test_assert(ecs_is_alive(world, entities[1]));
    test_assert(ecs_is_alive(world, entities[2]));

    test_assert(ecs_has(world, entities[0], TagA));
    test_assert(ecs_has(world, entities[1], TagA));
    test_assert(ecs_has(world, entities[2], TagA));

    test_assert(ecs_has(world, entities[0], TagB));
    test_assert(ecs_has(world, entities[1], TagB));
    test_assert(ecs_has(world, entities[2], TagB));

    ecs_fini(world);
}

void New_w_Count_bulk_init_1_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 3,
        .ids = {ecs_id(Position)}
    });

    test_assert(entities != NULL);

    test_assert(entities[0] != 0);
    test_assert(entities[1] != 0);
    test_assert(entities[2] != 0);

    test_assert(entities[0] != entities[1]);
    test_assert(entities[0] != entities[2]);
    test_assert(entities[0] != entities[1]);
    test_assert(entities[1] != entities[2]);

    test_assert(ecs_is_alive(world, entities[0]));
    test_assert(ecs_is_alive(world, entities[1]));
    test_assert(ecs_is_alive(world, entities[2]));

    test_assert(ecs_has(world, entities[0], Position));
    test_assert(ecs_has(world, entities[1], Position));
    test_assert(ecs_has(world, entities[2], Position));

    test_assert( ecs_get(world, entities[0], Position) != NULL);
    test_assert( ecs_get(world, entities[1], Position) != NULL);
    test_assert( ecs_get(world, entities[2], Position) != NULL);

    ecs_fini(world);
}

void New_w_Count_bulk_init_2_components() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 3,
        .ids = {ecs_id(Position), ecs_id(Velocity)}
    });

    test_assert(entities != NULL);

    test_assert(entities[0] != 0);
    test_assert(entities[1] != 0);
    test_assert(entities[2] != 0);

    test_assert(entities[0] != entities[1]);
    test_assert(entities[0] != entities[2]);
    test_assert(entities[0] != entities[1]);
    test_assert(entities[1] != entities[2]);

    test_assert(ecs_is_alive(world, entities[0]));
    test_assert(ecs_is_alive(world, entities[1]));
    test_assert(ecs_is_alive(world, entities[2]));

    test_assert(ecs_has(world, entities[0], Position));
    test_assert(ecs_has(world, entities[1], Position));
    test_assert(ecs_has(world, entities[2], Position));

    test_assert(ecs_has(world, entities[0], Velocity));
    test_assert(ecs_has(world, entities[1], Velocity));
    test_assert(ecs_has(world, entities[2], Velocity));

    test_assert( ecs_get(world, entities[0], Position) != NULL);
    test_assert( ecs_get(world, entities[1], Position) != NULL);
    test_assert( ecs_get(world, entities[2], Position) != NULL);

    test_assert( ecs_get(world, entities[0], Velocity) != NULL);
    test_assert( ecs_get(world, entities[1], Velocity) != NULL);
    test_assert( ecs_get(world, entities[2], Velocity) != NULL);

    ecs_fini(world);
}

void New_w_Count_bulk_init_1_component_w_value() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    Position p[] = {
        {10, 20},
        {30, 40},
        {50, 60}
    };

    void *data[] = {p};

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 3,
        .ids = {ecs_id(Position)},
        .data = data
    });

    test_assert(entities != NULL);

    test_assert(entities[0] != 0);
    test_assert(entities[1] != 0);
    test_assert(entities[2] != 0);

    test_assert(entities[0] != entities[1]);
    test_assert(entities[0] != entities[2]);
    test_assert(entities[0] != entities[1]);
    test_assert(entities[1] != entities[2]);

    test_assert(ecs_is_alive(world, entities[0]));
    test_assert(ecs_is_alive(world, entities[1]));
    test_assert(ecs_is_alive(world, entities[2]));

    test_assert(ecs_has(world, entities[0], Position));
    test_assert(ecs_has(world, entities[1], Position));
    test_assert(ecs_has(world, entities[2], Position));
    

    const Position *ptr;
    ptr = ecs_get(world, entities[0], Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ptr = ecs_get(world, entities[1], Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 30);
    test_int(ptr->y, 40);

    ptr = ecs_get(world, entities[2], Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 50);
    test_int(ptr->y, 60);

    ecs_fini(world);
}

void New_w_Count_bulk_init_2_components_w_value() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    Position p[] = {
        {10, 20},
        {30, 40},
        {50, 60}
    };

    Velocity v[] = {
        {1, 2},
        {3, 4},
        {5, 6}
    };

    void *data[] = {p, v};

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 3,
        .ids = {ecs_id(Position), ecs_id(Velocity)},
        .data = data
    });

    test_assert(entities != NULL);

    test_assert(entities[0] != 0);
    test_assert(entities[1] != 0);
    test_assert(entities[2] != 0);

    test_assert(entities[0] != entities[1]);
    test_assert(entities[0] != entities[2]);
    test_assert(entities[0] != entities[1]);
    test_assert(entities[1] != entities[2]);

    test_assert(ecs_is_alive(world, entities[0]));
    test_assert(ecs_is_alive(world, entities[1]));
    test_assert(ecs_is_alive(world, entities[2]));

    test_assert(ecs_has(world, entities[0], Position));
    test_assert(ecs_has(world, entities[1], Position));
    test_assert(ecs_has(world, entities[2], Position));

    test_assert(ecs_has(world, entities[0], Velocity));
    test_assert(ecs_has(world, entities[1], Velocity));
    test_assert(ecs_has(world, entities[2], Velocity));

    {
    const Position *ptr;
    ptr = ecs_get(world, entities[0], Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ptr = ecs_get(world, entities[1], Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 30);
    test_int(ptr->y, 40);

    ptr = ecs_get(world, entities[2], Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 50);
    test_int(ptr->y, 60);
    }

    {
    const Velocity *ptr;
    ptr = ecs_get(world, entities[0], Velocity);
    test_assert(ptr != NULL);
    test_int(ptr->x, 1);
    test_int(ptr->y, 2);

    ptr = ecs_get(world, entities[1], Velocity);
    test_assert(ptr != NULL);
    test_int(ptr->x, 3);
    test_int(ptr->y, 4);

    ptr = ecs_get(world, entities[2], Velocity);
    test_assert(ptr != NULL);
    test_int(ptr->x, 5);
    test_int(ptr->y, 6);
    }

    ecs_fini(world);
}

void New_w_Count_bulk_init_2_components_tag_w_value() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    Position p[] = {
        {10, 20},
        {30, 40},
        {50, 60}
    };

    Velocity v[] = {
        {1, 2},
        {3, 4},
        {5, 6}
    };

    void *data[] = {p, NULL, v};

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 3,
        .ids = {ecs_id(Position), Tag, ecs_id(Velocity)},
        .data = data
    });

    test_assert(entities != NULL);

    test_assert(entities[0] != 0);
    test_assert(entities[1] != 0);
    test_assert(entities[2] != 0);

    test_assert(entities[0] != entities[1]);
    test_assert(entities[0] != entities[2]);
    test_assert(entities[0] != entities[1]);
    test_assert(entities[1] != entities[2]);

    test_assert(ecs_is_alive(world, entities[0]));
    test_assert(ecs_is_alive(world, entities[1]));
    test_assert(ecs_is_alive(world, entities[2]));

    test_assert(ecs_has(world, entities[0], Tag));
    test_assert(ecs_has(world, entities[1], Tag));
    test_assert(ecs_has(world, entities[2], Tag));

    test_assert(ecs_has(world, entities[0], Position));
    test_assert(ecs_has(world, entities[1], Position));
    test_assert(ecs_has(world, entities[2], Position));

    test_assert(ecs_has(world, entities[0], Velocity));
    test_assert(ecs_has(world, entities[1], Velocity));
    test_assert(ecs_has(world, entities[2], Velocity));

    {
    const Position *ptr;
    ptr = ecs_get(world, entities[0], Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ptr = ecs_get(world, entities[1], Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 30);
    test_int(ptr->y, 40);

    ptr = ecs_get(world, entities[2], Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 50);
    test_int(ptr->y, 60);
    }

    {
    const Velocity *ptr;
    ptr = ecs_get(world, entities[0], Velocity);
    test_assert(ptr != NULL);
    test_int(ptr->x, 1);
    test_int(ptr->y, 2);

    ptr = ecs_get(world, entities[1], Velocity);
    test_assert(ptr != NULL);
    test_int(ptr->x, 3);
    test_int(ptr->y, 4);

    ptr = ecs_get(world, entities[2], Velocity);
    test_assert(ptr != NULL);
    test_int(ptr->x, 5);
    test_int(ptr->y, 6);
    }

    ecs_fini(world);
}

void New_w_Count_add_after_bulk() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, 0);

    const ecs_entity_t *ids = ecs_bulk_new_w_id(world, Tag, 10);
    test_assert(ids != NULL);
    test_int(ecs_count_id(world, Tag), 10);

    int i;
    for (i = 0; i < 10; i ++) {
        ecs_entity_t e = ids[i];
        test_assert(e != 0);
        test_assert(ecs_has_id(world, e, Tag));
    }

    ecs_entity_t e = ecs_new_id(world);
    ecs_add(world, e, Tag);
    test_assert(ecs_has(world, e, Tag));

    ecs_fini(world);
}

void New_w_Count_add_after_bulk_w_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    const ecs_entity_t *ids = ecs_bulk_new(world, Position, 10);
    test_assert(ids != NULL);
    test_int(ecs_count(world, Position), 10);

    int i;
    for (i = 0; i < 10; i ++) {
        ecs_entity_t e = ids[i];
        test_assert(e != 0);
        test_assert(ecs_has(world, e, Position));
    }

    ecs_entity_t e = ecs_new_id(world);
    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void New_w_Count_add_after_bulk_w_ctor() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_default_ctor
    });

    const ecs_entity_t *ids = ecs_bulk_new(world, Position, 10);
    test_assert(ids != NULL);
    test_int(ecs_count(world, Position), 10);

    int i;
    for (i = 0; i < 10; i ++) {
        ecs_entity_t e = ids[i];
        test_assert(e != 0);
        test_assert(ecs_has(world, e, Position));
    }

    ecs_entity_t e = ecs_new_id(world);

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}
