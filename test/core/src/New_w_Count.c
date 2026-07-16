#include <core.h>

static ECS_COMPONENT_DECLARE(Position);
static ECS_COMPONENT_DECLARE(Velocity);

void New_w_Count_empty(void) {
    ecs_world_t *world = ecs_mini();

    const ecs_entity_t *ids = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 1000
    }, NULL);
    test_assert(ids != NULL);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(ids[i] != 0);
    }

    ecs_fini(world);
}

void New_w_Count_component(void) {
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

void New_w_Count_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, 0);

    const ecs_entity_t *ids = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 1000,
        .ids = {Tag}
    }, NULL);
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

void New_w_Count_bulk_init_empty(void) {
    ecs_world_t *world = ecs_mini();

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 3
    }, NULL);

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

    test_assert(ecs_get_table(world, entities[0]) != NULL);
    test_assert(ecs_get_table(world, entities[1]) != NULL);
    test_assert(ecs_get_table(world, entities[2]) != NULL);

    ecs_fini(world);
}

void New_w_Count_bulk_init_empty_w_entities(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t ents[] = {1000, 1001, 1002};

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .entities = ents,
        .count = 3
    }, NULL);

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

void New_w_Count_bulk_init_1_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 3,
        .ids = {Tag}
    }, NULL);

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

void New_w_Count_bulk_init_1_tag_w_entities(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t ents[] = {1000, 1001, 1002};

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .entities = ents,
        .count = 3,
        .ids = {Tag}
    }, NULL);

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

void New_w_Count_bulk_init_2_tags(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 3,
        .ids = {TagA, TagB}
    }, NULL);

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

void New_w_Count_bulk_init_1_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 3,
        .ids = {ecs_id(Position)}
    }, NULL);

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

void New_w_Count_bulk_init_2_components(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 3,
        .ids = {ecs_id(Position), ecs_id(Velocity)}
    }, NULL);

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

void New_w_Count_bulk_init_1_component_w_value(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    Position p[] = {
        {10, 20},
        {30, 40},
        {50, 60}
    };

    void *data[1];

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 3,
        .ids = {ecs_id(Position)}
    }, data);
    ecs_os_memcpy_n(data[0], p, Position, 3);
    ecs_bulk_modified(world, &(ecs_bulk_desc_t){
        .count = 3,
        .ids = {ecs_id(Position)}
    }, data);

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

void New_w_Count_bulk_init_2_components_w_value(void) {
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

    void *data[2];

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 3,
        .ids = {ecs_id(Position), ecs_id(Velocity)}
    }, data);
    ecs_os_memcpy_n(data[0], p, Position, 3);
    ecs_os_memcpy_n(data[1], v, Velocity, 3);
    ecs_bulk_modified(world, &(ecs_bulk_desc_t){
        .count = 3,
        .ids = {ecs_id(Position), ecs_id(Velocity)}
    }, data);

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

void New_w_Count_bulk_init_2_components_tag_w_value(void) {
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

    void *data[3];

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 3,
        .ids = {ecs_id(Position), Tag, ecs_id(Velocity)}
    }, data);
    ecs_os_memcpy_n(data[0], p, Position, 3);
    test_assert(data[1] == NULL);
    ecs_os_memcpy_n(data[2], v, Velocity, 3);
    ecs_bulk_modified(world, &(ecs_bulk_desc_t){
        .count = 3,
        .ids = {ecs_id(Position), Tag, ecs_id(Velocity)}
    }, data);

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

static int bulk_ctor_count;
static int bulk_on_set_count;

static void bulk_ctor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *ti)
{
    bulk_ctor_count += count;
    ecs_os_memset(ptr, 0, ti->size * count);
}

static void bulk_on_set(ecs_iter_t *it) {
    bulk_on_set_count += it->count;
}

void New_w_Count_bulk_init_emplace(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_set_hooks(world, Position, {
        .ctor = bulk_ctor,
        .on_set = bulk_on_set
    });

    bulk_ctor_count = 0;
    bulk_on_set_count = 0;

    void *data[1];
    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 3,
        .ids = {ecs_id(Position)},
        .emplace = true
    }, data);

    test_assert(entities != NULL);
    test_assert(data[0] != NULL);
    test_int(bulk_ctor_count, 0);
    test_int(bulk_on_set_count, 0);

    Position *p = data[0];
    p[0] = (Position){10, 20};
    p[1] = (Position){30, 40};
    p[2] = (Position){50, 60};

    ecs_bulk_modified(world, &(ecs_bulk_desc_t){
        .count = 3,
        .ids = {ecs_id(Position)}
    }, data);

    test_int(bulk_ctor_count, 0);
    test_int(bulk_on_set_count, 3);
    test_int(ecs_get(world, entities[0], Position)->x, 10);
    test_int(ecs_get(world, entities[1], Position)->x, 30);
    test_int(ecs_get(world, entities[2], Position)->x, 50);

    ecs_fini(world);
}

void New_w_Count_add_after_bulk(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, 0);

    const ecs_entity_t *ids = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 10,
        .ids = {Tag}
    }, NULL);
    test_assert(ids != NULL);
    test_int(ecs_count_id(world, Tag), 10);

    int i;
    for (i = 0; i < 10; i ++) {
        ecs_entity_t e = ids[i];
        test_assert(e != 0);
        test_assert(ecs_has_id(world, e, Tag));
    }

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Tag);
    test_assert(ecs_has(world, e, Tag));

    ecs_fini(world);
}

void New_w_Count_add_after_bulk_w_component(void) {
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

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void New_w_Count_add_after_bulk_w_ctor(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = flecs_default_ctor
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

    ecs_entity_t e = ecs_new(world);

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void New_w_Count_recycle_1_of_2(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t tag = ecs_new(world);

    /* ecs_entity_t e0 = */ ecs_new(world);
    ecs_entity_t e1 = ecs_new(world);
    ecs_delete(world, e1);

    const ecs_entity_t *ids = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 2,
        .ids = {tag}
    }, NULL);
    test_assert(ids[0] != 0);
    test_assert(ids[1] != 0);
    test_assert(ids[0] != e1);
    test_assert((uint32_t)ids[0] == e1);

    ecs_fini(world);
}

void New_w_Count_recycle_1_of_3(void) {
  ecs_world_t *world = ecs_mini();

  ecs_entity_t tag = ecs_new(world);

  /* ecs_entity_t e0 = */ ecs_new(world);
  ecs_entity_t e1 = ecs_new(world);
  ecs_delete(world, e1);

  const ecs_entity_t *ids = ecs_bulk_init(world, &(ecs_bulk_desc_t){
    .count = 3,
    .ids = {tag}
  }, NULL);
  test_assert(ids[0] != 0);
  test_assert(ids[1] != 0);
  test_assert(ids[2] != 0);
  test_assert(ids[0] != e1);
  test_assert((uint32_t)ids[0] == e1);

  ecs_fini(world);
}

void New_w_Count_recycle_2_of_3(void) {
  ecs_world_t *world = ecs_mini();

  ecs_entity_t tag = ecs_new(world);

  ecs_entity_t e0 = ecs_new(world);
  ecs_entity_t e1 = ecs_new(world);
  ecs_delete(world, e1);
  ecs_delete(world, e0);

  const ecs_entity_t *ids = ecs_bulk_init(world, &(ecs_bulk_desc_t){
    .count = 3,
    .ids = {tag}
  }, NULL);
  test_assert(ids[0] != 0);
  test_assert(ids[1] != 0);
  test_assert(ids[2] != 0);
  test_assert(ids[0] != e0);
  test_assert(ids[1] != e1);
  test_assert((uint32_t)ids[0] == e0);
  test_assert((uint32_t)ids[1] == e1);

  ecs_fini(world);
}

void New_w_Count_bulk_init_w_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_table_t *table = ecs_table_add_id(world, NULL, Tag);
    ecs_bulk_desc_t desc = {0};
    desc.count = 3;
    desc.table = table;
    const ecs_entity_t *entities = ecs_bulk_init(world, &desc, NULL);

    test_assert(entities != NULL);
    test_assert(ecs_has(world, entities[0], Tag));
    test_assert(ecs_has(world, entities[1], Tag));
    test_assert(ecs_has(world, entities[2], Tag));
    test_assert(ecs_get_table(world, entities[0]) == table);
    test_assert(ecs_get_table(world, entities[1]) == table);
    test_assert(ecs_get_table(world, entities[2]) == table);
    test_int(3, ecs_count(world, Tag));

    ecs_delete_with(world, Tag);
    test_int(0, ecs_count(world, Tag));

    ecs_fini(world);
}

void New_w_Count_bulk_ids_w_1_exceed_32_bits(void) {
	install_test_abort();
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, 0);

    const ecs_entity_range_t *range = ecs_entity_range_new(world, UINT32_MAX, 0);
    ecs_entity_range_set(world, range);
    ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 1,
        .ids = {Tag}
    }, NULL);
    test_expect_abort();
    ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 1,
        .ids = {Tag}
    }, NULL);

    ecs_fini(world);
}

void New_w_Count_bulk_ids_w_2_exceed_32_bits(void) {
	install_test_abort();
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, 0);

    const ecs_entity_range_t *range = ecs_entity_range_new(world, UINT32_MAX, 0);
    ecs_entity_range_set(world, range);
    test_expect_abort();
    ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 2,
        .ids = {Tag}
    }, NULL);

    ecs_fini(world);
}

void New_w_Count_bulk_init_w_alive_entity(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t ents[] = {ecs_new(world)};

    test_expect_abort();

    ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .entities = ents,
        .count = 1
    }, NULL);
}

static void hook_w_add(ecs_iter_t *it) {
    for (int i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], Velocity, {1, 2});
    }
}

void New_w_Count_bulk_init_w_cmd_in_on_add_hook(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);

    ecs_set_hooks(world, Position, {
        .on_add = hook_w_add
    });

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 3,
        .ids = {ecs_id(Position)}
    }, NULL);

    for (int i = 0; i < 3; i ++) {
        const Velocity *v = ecs_get(world, entities[i], Velocity);
        test_assert(v != NULL);
        test_int(v->x, 1);
        test_int(v->y, 2);
    }

    ecs_fini(world);
}

void New_w_Count_bulk_init_w_cmd_in_on_add_observer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);

    ecs_observer(world, {
        .query.terms = {
            { .id = ecs_id(Position) },
        },
        .callback = hook_w_add,
        .events = { EcsOnAdd }
    });

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 3,
        .ids = {ecs_id(Position)}
    }, NULL);

    for (int i = 0; i < 3; i ++) {
        const Velocity *v = ecs_get(world, entities[i], Velocity);
        test_assert(v != NULL);
        test_int(v->x, 1);
        test_int(v->y, 2);
    }

    ecs_fini(world);
}
