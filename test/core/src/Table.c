#include <core.h>

void Table_get_index(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});
    ecs_set(world, e, Velocity, {10, 20});

    ecs_table_t *table = ecs_get_table(world, e);
    test_assert(table != NULL);
    test_int(ecs_table_get_type_index(world, table, ecs_id(Position)), 0);
    test_int(ecs_table_get_type_index(world, table, ecs_id(Velocity)), 1);

    ecs_fini(world);
}

void Table_get_index_not_in_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Position, {10, 20});
    ecs_set(world, e, Velocity, {10, 20});

    ecs_table_t *table = ecs_get_table(world, e);
    test_assert(table != NULL);
    test_int(ecs_table_get_type_index(world, table, ecs_id(Position)), 0);
    test_int(ecs_table_get_type_index(world, table, ecs_id(Velocity)), 1);
    test_int(ecs_table_get_type_index(world, table, ecs_id(Mass)), -1);

    ecs_fini(world);
}

void Table_get_column(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_entity_t e2 = ecs_new(world);
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e2, Velocity, {2, 3});

    ecs_table_t *table = ecs_get_table(world, e1);
    test_assert(table != NULL);
    int32_t pos_id = ecs_table_get_column_index(world, table, ecs_id(Position));
    int32_t vel_id = ecs_table_get_column_index(world, table, ecs_id(Velocity));

    Position *p = ecs_table_get_column(table, pos_id, 0);
    test_assert(p != NULL);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);

    Velocity *v = ecs_table_get_column(table, vel_id, 0);
    test_assert(v != NULL);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);
    test_int(v[1].x, 2);
    test_int(v[1].y, 3);

    ecs_fini(world);
}

void Table_get_column_for_tag(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});
    ecs_add(world, e1, Tag);

    ecs_table_t *table = ecs_get_table(world, e1);
    test_assert(table != NULL);

    test_expect_abort();
    int32_t tag_id = ecs_table_get_type_index(world, table, Tag);

    test_expect_abort();
    ecs_table_get_column(table, tag_id, 0);
}

void Table_get_column_for_component_after_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t tag_w_low_id = ecs_entity(world, {
        .use_low_id = true
    });

    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});
    ecs_add_id(world, e1, tag_w_low_id);

    ecs_entity_t e2 = ecs_new(world);
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e2, Velocity, {2, 3});
    ecs_add_id(world, e2, tag_w_low_id);

    ecs_table_t *table = ecs_get_table(world, e1);
    test_assert(table != NULL);
    int32_t pos_id = ecs_table_get_column_index(world, table, ecs_id(Position));
    int32_t vel_id = ecs_table_get_column_index(world, table, ecs_id(Velocity));

    test_assert(pos_id != -1);
    test_assert(vel_id != -1);

    Position *p = ecs_table_get_column(table, pos_id, 0);
    test_assert(p != NULL);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);

    Velocity *v = ecs_table_get_column(table, vel_id, 0);
    test_assert(v != NULL);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);
    test_int(v[1].x, 2);
    test_int(v[1].y, 3);

    ecs_fini(world);
}

void Table_get_column_w_offset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_entity_t e2 = ecs_new(world);
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e2, Velocity, {2, 3});

    ecs_table_t *table = ecs_get_table(world, e1);
    test_assert(table != NULL);
    int32_t pos_id = ecs_table_get_column_index(world, table, ecs_id(Position));
    int32_t vel_id = ecs_table_get_column_index(world, table, ecs_id(Velocity));

    Position *p = ecs_table_get_column(table, pos_id, 1);
    test_assert(p != NULL);
    test_int(p[0].x, 20);
    test_int(p[0].y, 30);

    Velocity *v = ecs_table_get_column(table, vel_id, 1);
    test_assert(v != NULL);
    test_int(v[0].x, 2);
    test_int(v[0].y, 3);

    ecs_fini(world);
}

void Table_get_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_entity_t e2 = ecs_new(world);
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

void Table_get_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_entity_t e2 = ecs_new(world);
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

void Table_get_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Tgt);

    ecs_entity_t e1 = ecs_new(world);
    ecs_set_pair(world, e1, Position, Tgt, {10, 20});
    ecs_set_pair(world, e1, Velocity, Tgt, {1, 2});

    ecs_entity_t e2 = ecs_new(world);
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

void Table_get_from_stage(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_entity_t e2 = ecs_new(world);
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e2, Velocity, {2, 3});

    ecs_table_t *table = ecs_get_table(world, e1);
    test_assert(table != NULL);

    ecs_world_t *stage = ecs_get_stage(world, 0);
    test_assert(stage != NULL);
    test_assert(stage != world);

    Position *p = ecs_table_get(stage, table, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);

    Velocity *v = ecs_table_get(stage, table, Velocity, 0);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);
    test_int(v[1].x, 2);
    test_int(v[1].y, 3);

    ecs_fini(world);
}

void Table_get_depth(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, e3);

    test_int(1, ecs_table_get_depth(world, ecs_get_table(world, e2), EcsChildOf));
    test_int(2, ecs_table_get_depth(world, ecs_get_table(world, e3), EcsChildOf));
    test_int(3, ecs_table_get_depth(world, ecs_get_table(world, e4), EcsChildOf));

    ecs_fini(world);
}

void Table_get_depth_non_acyclic(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, Tgt);
    
    test_expect_abort();
    ecs_table_get_depth(world, ecs_get_table(world, e1), Rel);
}

void Table_get_depth_2_paths(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsIsA, e2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsIsA, e2);
    ecs_add_pair(world, e4, EcsIsA, e3);

    test_int(1, ecs_table_get_depth(world, ecs_get_table(world, e2), EcsIsA));
    test_int(2, ecs_table_get_depth(world, ecs_get_table(world, e3), EcsIsA));
    test_int(3, ecs_table_get_depth(world, ecs_get_table(world, e4), EcsIsA));

    ecs_fini(world);
}

void Table_get_column_size(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_entity_t ecs_id(Mass) = ecs_component(world, {
        .entity = ecs_new(world),
        .type.size = 4,
        .type.alignment = 4
    });

    ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});
    ecs_add(world, e1, Tag);
    ecs_set(world, e1, Mass, {1});

    ecs_table_t *table = ecs_get_table(world, e1);
    test_assert(table != NULL);

    test_uint(8, ecs_table_get_column_size(table, 0));
    test_uint(4, ecs_table_get_column_size(table, 1));

    ecs_fini(world);
}
