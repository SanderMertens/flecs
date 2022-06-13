#include <api.h>

void Clone_empty() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_new(world, 0);
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_clone(world, 0, e1, false);
    test_assert(e2 != 0);
    test_assert(e1 != e2);

    test_assert(!ecs_get_type(world, e1));
    test_assert(!ecs_get_type(world, e2));

    ecs_fini(world);
}

void Clone_empty_w_value() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_new(world, 0);
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_clone(world, 0, e1, true);
    test_assert(e2 != 0);
    test_assert(e1 != e2);

    test_assert(!ecs_get_type(world, e1));
    test_assert(!ecs_get_type(world, e2));

    ecs_fini(world);
}

void Clone_null() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    test_expect_abort();
    ecs_clone(world, 0, 0, false);
}

void Clone_null_w_value() {
    install_test_abort();
    ecs_world_t *world = ecs_mini();

    test_expect_abort();
    ecs_clone(world, 0, 0, true);
}

void Clone_1_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new(world, Position);
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_clone(world, 0, e1, false);
    test_assert(e2 != 0);
    test_assert(e1 != e2);

    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));
    test_assert(ecs_get(world, e1, Position) !=  
                ecs_get(world, e2, Position));

    ecs_fini(world);
}

void Clone_2_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e1, Position, Velocity);
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_clone(world, 0, e1, false);
    test_assert(e2 != 0);
    test_assert(e1 != e2);

    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));
    test_assert(ecs_get(world, e1, Position) !=  
                ecs_get(world, e2, Position));

    test_assert(ecs_has(world, e1, Velocity));
    test_assert(ecs_has(world, e2, Velocity));
    test_assert(ecs_get(world, e1, Velocity) !=  
                ecs_get(world, e2, Velocity));

    ecs_fini(world);
}

void Clone_3_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e1, Position, Velocity, Mass);
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_clone(world, 0, e1, false);
    test_assert(e2 != 0);
    test_assert(e1 != e2);

    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));
    test_assert(ecs_get(world, e1, Position) !=  
                ecs_get(world, e2, Position));

    test_assert(ecs_has(world, e1, Velocity));
    test_assert(ecs_has(world, e2, Velocity));
    test_assert(ecs_get(world, e1, Velocity) !=  
                ecs_get(world, e2, Velocity));

    test_assert(ecs_has(world, e1, Mass));
    test_assert(ecs_has(world, e2, Mass));
    test_assert(ecs_get(world, e1, Mass) !=  
                ecs_get(world, e2, Mass));

    ecs_fini(world);
}

void Clone_1_component_w_value() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new(world, 0);
    test_assert(e1 != 0);

    ecs_set(world, e1, Position, {10, 20});

    ecs_entity_t e2 = ecs_clone(world, 0, e1, true);
    test_assert(e2 != 0);
    test_assert(e1 != e2);

    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));

    const Position *p_1 = ecs_get(world, e1, Position);
    test_assert(p_1 != NULL);
    test_int(p_1->x, 10);
    test_int(p_1->y, 20);

    const Position *p_2 = ecs_get(world, e2, Position);
    test_assert(p_2 != NULL);
    test_assert(p_1 != p_2);
    test_int(p_2->x, 10);
    test_int(p_2->y, 20);

    ecs_fini(world);
}

void Clone_2_component_w_value() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new(world, 0);
    test_assert(e1 != 0);

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {30, 40});

    ecs_entity_t e2 = ecs_clone(world, 0, e1, true);
    test_assert(e2 != 0);
    test_assert(e1 != e2);

    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));
    test_assert(ecs_has(world, e1, Velocity));
    test_assert(ecs_has(world, e2, Velocity));

    const Position *p_1 = ecs_get(world, e1, Position);
    test_assert(p_1 != NULL);
    test_int(p_1->x, 10);
    test_int(p_1->y, 20);

    const Position *p_2 = ecs_get(world, e2, Position);
    test_assert(p_2 != NULL);
    test_assert(p_1 != p_2);
    test_int(p_2->x, 10);
    test_int(p_2->y, 20);

    const Velocity *v_1 = ecs_get(world, e1, Velocity);
    test_assert(v_1 != NULL);
    test_int(v_1->x, 30);
    test_int(v_1->y, 40);

    const Velocity *v_2 = ecs_get(world, e2, Velocity);
    test_assert(v_2 != NULL);
    test_assert(v_1 != v_2);
    test_int(v_2->x, 30);
    test_int(v_2->y, 40);

    ecs_fini(world);
}

void Clone_3_component_w_value() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t e1 = ecs_new(world, 0);
    test_assert(e1 != 0);

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e1, Velocity, {30, 40});
    ecs_set(world, e1, Mass, {50});

    ecs_entity_t e2 = ecs_clone(world, 0, e1, true);
    test_assert(e2 != 0);
    test_assert(e1 != e2);

    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));
    test_assert(ecs_has(world, e1, Velocity));
    test_assert(ecs_has(world, e2, Velocity));
    test_assert(ecs_has(world, e1, Mass));
    test_assert(ecs_has(world, e2, Mass));

    const Position *p_1 = ecs_get(world, e1, Position);
    test_assert(p_1 != NULL);
    test_int(p_1->x, 10);
    test_int(p_1->y, 20);

    const Position *p_2 = ecs_get(world, e2, Position);
    test_assert(p_2 != NULL);
    test_assert(p_1 != p_2);
    test_int(p_2->x, 10);
    test_int(p_2->y, 20);

    const Velocity *v_1 = ecs_get(world, e1, Velocity);
    test_assert(v_1 != NULL);
    test_int(v_1->x, 30);
    test_int(v_1->y, 40);

    const Velocity *v_2 = ecs_get(world, e2, Velocity);
    test_assert(v_2 != NULL);
    test_assert(v_1 != v_2);
    test_int(v_2->x, 30);
    test_int(v_2->y, 40);

    const Mass *m_1 = ecs_get(world, e1, Mass);
    test_assert(m_1 != NULL);
    test_int(*m_1, 50);

    const Mass *m_2 = ecs_get(world, e2, Mass);
    test_assert(m_2 != NULL);
    test_assert(m_1 != m_2);
    test_int(*m_2, 50);

    ecs_fini(world);
}

void Clone_tag() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, 0);

    ecs_entity_t e1 = ecs_new_w_id(world, Tag);
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_clone(world, 0, e1, false);
    test_assert(e2 != 0);
    test_assert(e1 != e2);

    test_assert(ecs_has_id(world, e1, Tag));
    test_assert(ecs_has_id(world, e2, Tag));

    ecs_fini(world);
}

void Clone_tag_w_value() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, 0);

    ecs_entity_t e1 = ecs_new_w_id(world, Tag);
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_clone(world, 0, e1, true);
    test_assert(e2 != 0);
    test_assert(e1 != e2);

    test_assert(ecs_has_id(world, e1, Tag));
    test_assert(ecs_has_id(world, e2, Tag));

    ecs_fini(world);
}

void Clone_1_tag_1_component() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, 0);
    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, e1, Position, Tag);

    ecs_set(world, e1, Position, {10, 20});

    ecs_entity_t e2 = ecs_clone(world, 0, e1, false);
    test_assert(e2 != 0);
    test_assert(e1 != e2);

    test_assert(ecs_has_id(world, e1, Tag));
    test_assert(ecs_has_id(world, e2, Tag));

    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));

    ecs_fini(world);
}

void Clone_1_tag_1_component_w_value() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, 0);
    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_add(world, e1, Tag);
    ecs_set(world, e1, Position, {10, 20});

    ecs_entity_t e2 = ecs_clone(world, 0, e1, true);
    test_assert(e2 != 0);
    test_assert(e1 != e2);

    test_assert(ecs_has_id(world, e1, Tag));
    test_assert(ecs_has_id(world, e2, Tag));

    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));

    const Position *p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}
