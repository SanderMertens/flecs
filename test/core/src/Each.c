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

void Each_each_root_entities(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t e1 = ecs_new_w(world, Foo);

    bool e1_found = false, Foo_found = false, Flecs_found = false;
    int32_t count = 0;

    ecs_iter_t it = ecs_each_pair(world, EcsChildOf, 0);
    while (ecs_iter_next(&it)) {
        for (int i = 0; i < it.count; i ++) {
            e1_found |= it.entities[i] == e1;
            Foo_found |= it.entities[i] == Foo;
            Flecs_found |= it.entities[i] == EcsFlecs;
            count ++;
        }
    }

    test_assert(e1_found);
    test_assert(Foo_found);
    test_assert(Flecs_found);
    test_int(count, 3);

    ecs_fini(world);
}

void Each_each_empty_root_entities(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t e1 = ecs_new(world);

    bool e1_found = false, Foo_found = false, Flecs_found = false;
    int32_t count = 0;

    ecs_iter_t it = ecs_each_pair(world, EcsChildOf, 0);
    while (ecs_iter_next(&it)) {
        for (int i = 0; i < it.count; i ++) {
            e1_found |= it.entities[i] == e1;
            Foo_found |= it.entities[i] == Foo;
            Flecs_found |= it.entities[i] == EcsFlecs;
            count ++;
        }
    }

    test_assert(e1_found);
    test_assert(Foo_found);
    test_assert(Flecs_found);
    test_int(count, 3);

    ecs_fini(world);
}

void Each_each_sparse_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Bar);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

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
    test_uint(1, it.row_fields);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
        p = ecs_field_at(&it, Position, 0, 1);
        test_assert(p != NULL);
        test_int(p->x, 20); test_int(p->y, 30);
    }

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_uint(1, it.row_fields);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 30); test_int(p->y, 40);
    }

    test_bool(false, ecs_iter_next(&it));

    ecs_fini(world);
}

void Each_each_sparse_pair_rel_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_entity_t e1 = ecs_new(world);
    ecs_set_pair(world, e1, Position, TgtA, {10, 20});
    ecs_entity_t e2 = ecs_new(world);
    ecs_set_pair(world, e2, Position, TgtB, {20, 30});

    ecs_iter_t it = ecs_each_pair_t(world, Position, EcsWildcard);
    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_pair_t(Position, TgtA), ecs_field_id(&it, 0));
    test_uint(1, it.row_fields);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
    }

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_pair_t(Position, TgtB), ecs_field_id(&it, 0));
    test_uint(1, it.row_fields);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 20); test_int(p->y, 30);
    }

    test_bool(false, ecs_iter_next(&it));

    ecs_fini(world);
}

void Each_each_dont_fragment_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_add_id(world, Foo, EcsDontFragment);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_entity_t e3 = ecs_new_w(world, Foo);
    ecs_add(world, e3, Bar);

    ecs_iter_t it = ecs_each(world, Foo);
    test_bool(true, ecs_iter_next(&it));
    test_int(3, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(e2, it.entities[1]);
    test_uint(e3, it.entities[2]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_bool(false, ecs_iter_next(&it));

    ecs_fini(world);
}

void Each_each_dont_fragment_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Bar);

    ecs_add_id(world, ecs_id(Position), EcsDontFragment);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_add(world, e3, Bar);

    ecs_iter_t it = ecs_each(world, Position);
    test_bool(true, ecs_iter_next(&it));
    test_int(3, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(e2, it.entities[1]);
    test_uint(e3, it.entities[2]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_uint(ECS_SIZEOF(Position), ecs_field_size(&it, 0));
    test_uint(1, it.row_fields);
    {
        Position *p = ecs_field_at(&it, Position, 0, 0);
        test_assert(p != NULL);
        test_int(p->x, 10); test_int(p->y, 20);
        p = ecs_field_at(&it, Position, 0, 1);
        test_assert(p != NULL);
        test_int(p->x, 20); test_int(p->y, 30);
        p = ecs_field_at(&it, Position, 0, 2);
        test_assert(p != NULL);
        test_int(p->x, 30); test_int(p->y, 40);
    }

    test_bool(false, ecs_iter_next(&it));

    ecs_fini(world);
}

void Each_each_dont_fragment_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_add_id(world, Rel, EcsDontFragment);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_new_w_pair(world, Rel, TgtB);

    ecs_iter_t it = ecs_each_pair(world, Rel, TgtA);
    test_bool(true, ecs_iter_next(&it));
    test_int(2, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(e2, it.entities[1]);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_iter_next(&it));

    ecs_fini(world);
}

void Each_each_dont_fragment_pair_rel_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_add_id(world, Rel, EcsDontFragment);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_add_pair(world, e2, Rel, TgtB);
    ecs_entity_t e3 = ecs_new_w_pair(world, Rel, TgtB);

    ecs_iter_t it = ecs_each_pair(world, Rel, EcsWildcard);
    test_bool(true, ecs_iter_next(&it));
    test_int(2, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(e3, it.entities[1]);
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(true, ecs_iter_next(&it));
    test_int(2, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(e2, it.entities[1]);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(false, ecs_iter_next(&it));

    ecs_fini(world);
}

void Each_each_dont_fragment_pair_rel_wildcard_w_empty_tgt(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_add_id(world, Rel, EcsDontFragment);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_remove_pair(world, e2, Rel, TgtB);
    ecs_entity_t e3 = ecs_new_w_pair(world, Rel, TgtC);

    ecs_iter_t it = ecs_each_pair(world, Rel, EcsWildcard);
    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 0));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));

    test_bool(false, ecs_iter_next(&it));

    ecs_fini(world);
}

void Each_each_dont_fragment_exclusive_pair_rel_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_add_id(world, Rel, EcsDontFragment);
    ecs_add_id(world, Rel, EcsExclusive);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_entity_t e3 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_add_pair(world, e3, Rel, TgtA);

    ecs_iter_t it = ecs_each_pair(world, Rel, EcsWildcard);
    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));

    test_bool(true, ecs_iter_next(&it));
    test_int(2, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(e3, it.entities[1]);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));

    test_bool(false, ecs_iter_next(&it));

    ecs_fini(world);
}

void Each_each_dont_fragment_pair_tgt_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, Tgt);

    ecs_add_id(world, RelB, EcsDontFragment);
    ecs_add_id(world, RelC, EcsDontFragment);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelA, Tgt);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelB, Tgt);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelC, Tgt);
    ecs_add_pair(world, e3, RelB, Tgt);

    ecs_iter_t it = ecs_each_pair(world, EcsWildcard, Tgt);
    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 0));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(ecs_pair(RelC, Tgt), ecs_field_id(&it, 0));

    test_bool(true, ecs_iter_next(&it));
    test_int(2, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(e3, it.entities[1]);
    test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 0));

    test_bool(false, ecs_iter_next(&it));

    ecs_fini(world);
}

void Each_each_dont_fragment_wildcard_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelX);
    ECS_TAG(world, RelY);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_add_id(world, RelX, EcsDontFragment);
    ecs_add_id(world, RelY, EcsDontFragment);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelX, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelY, TgtB);
    ecs_remove_pair(world, e2, RelY, TgtB);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelY, TgtA);

    bool e1_found = false, e3_found = false;

    ecs_iter_t it = ecs_each_pair(world, EcsWildcard, EcsWildcard);
    while (ecs_iter_next(&it)) {
        for (int i = 0; i < it.count; i ++) {
            if (it.entities[i] == e1) {
                test_assert(!e1_found);
                test_uint(ecs_pair(RelX, TgtA), ecs_field_id(&it, 0));
                e1_found = true;
            }

            if (it.entities[i] == e3) {
                test_assert(!e3_found);
                test_uint(ecs_pair(RelY, TgtA), ecs_field_id(&it, 0));
                e3_found = true;
            }

            test_assert(it.entities[i] != e2);
        }
    }

    test_bool(true, e1_found);
    test_bool(true, e3_found);

    ecs_fini(world);
}

void Each_each_dont_fragment_empty(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_add_id(world, Foo, EcsDontFragment);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_remove(world, e1, Foo);

    ecs_iter_t it = ecs_each(world, Foo);
    test_bool(false, ecs_iter_next(&it));

    ecs_fini(world);
}
