#include <api.h>

void Table_get_index() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new_id(world);
    ecs_set(world, e, Position, {10, 20});
    ecs_set(world, e, Velocity, {10, 20});

    ecs_table_t *table = ecs_get_table(world, e);
    test_assert(table != NULL);
    test_int(ecs_table_get_index(world, table, ecs_id(Position)), 0);
    test_int(ecs_table_get_index(world, table, ecs_id(Velocity)), 1);

    ecs_fini(world);
}

void Table_get_index_not_in_table() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t e = ecs_new_id(world);
    ecs_set(world, e, Position, {10, 20});
    ecs_set(world, e, Velocity, {10, 20});

    ecs_table_t *table = ecs_get_table(world, e);
    test_assert(table != NULL);
    test_int(ecs_table_get_index(world, table, ecs_id(Position)), 0);
    test_int(ecs_table_get_index(world, table, ecs_id(Velocity)), 1);
    test_int(ecs_table_get_index(world, table, ecs_id(Mass)), -1);

    ecs_fini(world);
}

void Table_get_column() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e2, Velocity, {2, 3});

    ecs_table_t *table = ecs_get_table(world, e1);
    test_assert(table != NULL);
    int32_t pos_id = ecs_table_get_index(world, table, ecs_id(Position));
    int32_t vel_id = ecs_table_get_index(world, table, ecs_id(Velocity));

    Position *p = ecs_table_get_column(table, pos_id, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);

    Velocity *v = ecs_table_get_column(table, vel_id, 0);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);
    test_int(v[1].x, 2);
    test_int(v[1].y, 3);

    ecs_fini(world);
}

void Table_get_column_for_tag() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_set(world, e1, Position, {10, 20});
    ecs_add(world, e1, Tag);

    ecs_table_t *table = ecs_get_table(world, e1);
    test_assert(table != NULL);
    int32_t tag_id = ecs_table_get_index(world, table, Tag);

    test_assert(ecs_table_get_column(table, tag_id, 0) == NULL);

    ecs_fini(world);
}

void Table_get_column_for_component_after_tag() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t tag_w_low_id = ecs_entity(world, {
        .use_low_id = true
    });

    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});
    ecs_add_id(world, e1, tag_w_low_id);

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e2, Velocity, {2, 3});
    ecs_add_id(world, e2, tag_w_low_id);

    ecs_table_t *table = ecs_get_table(world, e1);
    test_assert(table != NULL);
    int32_t pos_id = ecs_table_get_index(world, table, ecs_id(Position));
    int32_t vel_id = ecs_table_get_index(world, table, ecs_id(Velocity));

    Position *p = ecs_table_get_column(table, pos_id, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);

    Velocity *v = ecs_table_get_column(table, vel_id, 0);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);
    test_int(v[1].x, 2);
    test_int(v[1].y, 3);

    ecs_fini(world);
}

void Table_get_column_w_offset() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e2, Velocity, {2, 3});

    ecs_table_t *table = ecs_get_table(world, e1);
    test_assert(table != NULL);
    int32_t pos_id = ecs_table_get_index(world, table, ecs_id(Position));
    int32_t vel_id = ecs_table_get_index(world, table, ecs_id(Velocity));

    Position *p = ecs_table_get_column(table, pos_id, 1);
    test_int(p[0].x, 20);
    test_int(p[0].y, 30);

    Velocity *v = ecs_table_get_column(table, vel_id, 1);
    test_int(v[0].x, 2);
    test_int(v[0].y, 3);

    ecs_fini(world);
}

void Table_get_id() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e2, Velocity, {2, 3});

    ecs_table_t *table = ecs_get_table(world, e1);
    test_assert(table != NULL);

    Position *p = ecs_table_get_id(world, table, ecs_id(Position), 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);

    Velocity *v = ecs_table_get_id(world, table, ecs_id(Velocity), 0);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);
    test_int(v[1].x, 2);
    test_int(v[1].y, 3);

    ecs_fini(world);
}

void Table_get_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e2, Velocity, {2, 3});

    ecs_table_t *table = ecs_get_table(world, e1);
    test_assert(table != NULL);

    Position *p = ecs_table_get(world, table, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);

    Velocity *v = ecs_table_get(world, table, Velocity, 0);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);
    test_int(v[1].x, 2);
    test_int(v[1].y, 3);

    ecs_fini(world);
}

void Table_get_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Tgt);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_set_pair(world, e1, Position, Tgt, {10, 20});
    ecs_set_pair(world, e1, Velocity, Tgt, {1, 2});

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_set_pair(world, e2, Position, Tgt, {20, 30});
    ecs_set_pair(world, e2, Velocity, Tgt, {2, 3});

    ecs_table_t *table = ecs_get_table(world, e1);
    test_assert(table != NULL);

    Position *p = ecs_table_get_pair(world, table, Position, Tgt, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);

    Velocity *v = ecs_table_get_pair(world, table, Velocity, Tgt, 0);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);
    test_int(v[1].x, 2);
    test_int(v[1].y, 3);

    ecs_fini(world);
}
