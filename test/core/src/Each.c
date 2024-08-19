#include <core.h>

void Each_each_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_entity_t e3 = ecs_new_w(world, Foo);
    ecs_add(world, e3, Bar);

    ecs_iter_t it = ecs_each(world, Foo);
    test_bool(true, ecs_iter_next(&it));
    test_int(2, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(e2, it.entities[1]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_iter_next(&it));

    ecs_fini(world);
}

void Each_each_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Bar);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_add(world, e3, Bar);

    ecs_iter_t it = ecs_each(world, Position);
    test_bool(true, ecs_iter_next(&it));
    test_int(2, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(e2, it.entities[1]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10); test_int(p[0].y, 20);
        test_int(p[1].x, 20); test_int(p[1].y, 30);
    }

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    {    
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 30); test_int(p[0].y, 40);
    }

    test_bool(false, ecs_iter_next(&it));

    ecs_fini(world);
}

void Each_each_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, Foo);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e3 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_add(world, e3, Foo);
    ecs_new_w_pair(world, Rel, TgtB);

    ecs_iter_t it = ecs_each_pair(world, Rel, TgtA);
    test_bool(true, ecs_iter_next(&it));
    test_int(2, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(e2, it.entities[1]);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_iter_next(&it));

    ecs_fini(world);
}

void Each_each_pair_rel_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, Foo);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e3 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_add(world, e3, Foo);
    ecs_entity_t e4 = ecs_new_w_pair(world, Rel, TgtB);

    ecs_iter_t it = ecs_each_pair(world, Rel, EcsWildcard);
    test_bool(true, ecs_iter_next(&it));
    test_int(2, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(e2, it.entities[1]);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_iter_next(&it));

    ecs_fini(world);
}

void Each_each_pair_tgt_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, Tgt);
    ECS_TAG(world, Foo);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelA, Tgt);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelA, Tgt);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelA, Tgt);
    ecs_add(world, e3, Foo);
    ecs_entity_t e4 = ecs_new_w_pair(world, RelB, Tgt);

    ecs_iter_t it = ecs_each_pair(world, EcsWildcard, Tgt);
    test_bool(true, ecs_iter_next(&it));
    test_int(2, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(e2, it.entities[1]);
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_iter_next(&it));

    ecs_fini(world);
}
