#include <api.h>

void ReadWrite_read() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});

    const ecs_record_t *r = ecs_read_begin(world, e);
    test_assert(r != NULL);
    const Position *p = ecs_record_get(world, r, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    ecs_read_end(r);

    ecs_add(world, e, Tag);
    test_assert(ecs_has(world, e, Tag));

    ecs_fini(world);
}

void ReadWrite_nested_read() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});

    const ecs_record_t *r = ecs_read_begin(world, e);
    const ecs_record_t *r_2 = ecs_read_begin(world, e);
    test_assert(r != NULL);
    test_assert(r_2 != NULL);
    test_assert(r == r_2);
    const Position *p = ecs_record_get(world, r, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    ecs_read_end(r_2);
    ecs_read_end(r);

    ecs_add(world, e, Tag);
    test_assert(ecs_has(world, e, Tag));

    ecs_fini(world);
}

void ReadWrite_write() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});

    ecs_record_t *r = ecs_write_begin(world, e);
    test_assert(r != NULL);
    Position *p = ecs_record_get_mut(world, r, Position);
    test_assert(p != NULL);
    test_assert(p == ecs_get(world, e, Position));
    test_int(p->x, 10);
    test_int(p->y, 20);
    ecs_write_end(r);

    ecs_add(world, e, Tag);
    test_assert(ecs_has(world, e, Tag));

    ecs_fini(world);
}

void ReadWrite_nested_write() {
    install_test_abort();
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});

    ecs_record_t *r = ecs_write_begin(world, e);
    test_assert(r != NULL);

    test_expect_abort();
    ecs_write_begin(world, e);
}

void ReadWrite_add_while_read() {
    install_test_abort();
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});

    const ecs_record_t *r = ecs_read_begin(world, e);
    test_assert(r != NULL);

    test_expect_abort();
    ecs_add(world, e, Tag);
}

void ReadWrite_add_while_write() {
    install_test_abort();
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});

    ecs_record_t *r = ecs_write_begin(world, e);
    test_assert(r != NULL);

    test_expect_abort();
    ecs_add(world, e, Tag);
}

void ReadWrite_read_from_stage() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});

    ecs_world_t *s = ecs_get_stage(world, 0);
    ecs_readonly_begin(world);

    const ecs_record_t *r = ecs_read_begin(s, e);
    test_assert(r != NULL);
    const Position *p = ecs_record_get(s, r, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    ecs_read_end(r);

    ecs_readonly_end(world);

    ecs_add(world, e, Tag);
    test_assert(ecs_has(world, e, Tag));

    ecs_fini(world);
}

void ReadWrite_write_from_stage() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});

    ecs_world_t *s = ecs_get_stage(world, 0);
    ecs_readonly_begin(world);

    ecs_record_t *r = ecs_write_begin(s, e);
    test_assert(r != NULL);
    Position *p = ecs_record_get_mut(s, r, Position);
    test_assert(p != NULL);
    test_assert(p == ecs_get(world, e, Position));
    test_int(p->x, 10);
    test_int(p->y, 20);
    ecs_write_end(r);

    ecs_readonly_end(world);

    ecs_add(world, e, Tag);
    test_assert(ecs_has(world, e, Tag));

    ecs_fini(world);
}
