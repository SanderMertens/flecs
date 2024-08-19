#include <query.h>

void Cascade_this_self_cascade_childof_uncached(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p0 = ecs_new(world);
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_entity(world, { .parent = p1 });
    ecs_entity_t e2 = ecs_entity(world, { .parent = p1 });
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_entity(world, { .parent = p2 });
    ecs_entity_t e4 = ecs_entity(world, { .parent = p2 });
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_entity(world, { .parent = e2 });
    ecs_entity_t e6 = ecs_entity(world, { .parent = e2 });
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_entity(world, { .parent = p3 });

    ecs_entity(world, { .parent = p0 });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(self|cascade)",
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(p1, it.entities[0]);
    test_uint(p2, it.entities[1]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Cascade_this_cascade_childof_uncached(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(cascade)",
    });

    test_assert(r != NULL);

    ecs_entity_t p0 = ecs_new(world);
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_entity(world, { .parent = p1 });
    ecs_entity_t e2 = ecs_entity(world, { .parent = p1 });
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_entity(world, { .parent = p2 });
    ecs_entity_t e4 = ecs_entity(world, { .parent = p2 });
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_entity(world, { .parent = e2 });
    ecs_entity_t e6 = ecs_entity(world, { .parent = e2 });
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_entity(world, { .parent = p3 });

    ecs_entity(world, { .parent = p0 });

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Cascade_this_written_self_cascade_childof_uncached(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Foo(self|cascade)",
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_new(world);
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_entity(world, { .parent = p1 });
    ecs_entity_t e2 = ecs_entity(world, { .parent = p1 });
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_entity(world, { .parent = p2 });
    ecs_entity_t e4 = ecs_entity(world, { .parent = p2 });
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_entity(world, { .parent = e2 });
    ecs_entity_t e6 = ecs_entity(world, { .parent = e2 });
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_entity(world, { .parent = p3 });

    ecs_entity(world, { .parent = p0 });
    ecs_set_with(world, 0);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(p1, it.entities[0]);
    test_uint(p2, it.entities[1]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p3, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Cascade_this_written_cascade_childof_uncached(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Foo(cascade)",
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_new(world);
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_entity(world, { .parent = p1 });
    ecs_entity_t e2 = ecs_entity(world, { .parent = p1 });
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_entity(world, { .parent = p2 });
    ecs_entity_t e4 = ecs_entity(world, { .parent = p2 });
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_entity(world, { .parent = e2 });
    ecs_entity_t e6 = ecs_entity(world, { .parent = e2 });
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_entity(world, { .parent = p3 });

    ecs_entity(world, { .parent = p0 });
    ecs_set_with(world, 0);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p3, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Cascade_this_self_cascade_childof_w_parent_flag_uncached(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(self|cascade)",
    });

    test_assert(r != NULL);

    ecs_entity_t p0 = ecs_new(world);
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_entity(world, { .parent = p1 });
    ecs_entity_t e2 = ecs_entity(world, { .parent = p1 });
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_entity(world, { .parent = p2 });
    ecs_entity_t e4 = ecs_entity(world, { .parent = p2 });
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_entity(world, { .parent = e2 });
    ecs_entity_t e6 = ecs_entity(world, { .parent = e2 });
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_entity(world, { .parent = p3 });

    ecs_entity(world, { .parent = p0 });

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(p1, it.entities[0]);
    test_uint(p2, it.entities[1]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Cascade_this_cascade_childof_w_parent_flag_uncached(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(cascade)",
    });

    test_assert(r != NULL);

    ecs_entity_t p0 = ecs_new(world);
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_entity(world, { .parent = p1 });
    ecs_entity_t e2 = ecs_entity(world, { .parent = p1 });
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_entity(world, { .parent = p2 });
    ecs_entity_t e4 = ecs_entity(world, { .parent = p2 });
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_entity(world, { .parent = e2 });
    ecs_entity_t e6 = ecs_entity(world, { .parent = e2 });
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_entity(world, { .parent = p3 });

    ecs_entity(world, { .parent = p0 });

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Cascade_this_written_self_cascade_childof_w_parent_flag_uncached(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Foo(self|cascade)",
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_entity(world, { .parent = p1 });
    ecs_entity_t e2 = ecs_entity(world, { .parent = p1 });
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_entity(world, { .parent = p2 });
    ecs_entity_t e4 = ecs_entity(world, { .parent = p2 });
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_entity(world, { .parent = e2 });
    ecs_entity_t e6 = ecs_entity(world, { .parent = e2 });
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_entity(world, { .parent = p3 });

    ecs_entity(world, { .parent = p0 });
    ecs_set_with(world, 0);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(p1, it.entities[0]);
    test_uint(p2, it.entities[1]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p3, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Cascade_this_written_cascade_childof_w_parent_flag_uncached(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Foo(cascade)",
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_new(world);
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_entity(world, { .parent = p1 });
    ecs_entity_t e2 = ecs_entity(world, { .parent = p1 });
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_entity(world, { .parent = p2 });
    ecs_entity_t e4 = ecs_entity(world, { .parent = p2 });
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_entity(world, { .parent = e2 });
    ecs_entity_t e6 = ecs_entity(world, { .parent = e2 });
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_entity(world, { .parent = p3 });

    ecs_entity(world, { .parent = p0 });
    ecs_set_with(world, 0);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p3, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Cascade_existing_isa_cascade(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e0 = ecs_new_w(world, Tag);
    ecs_entity_t e1 = ecs_entity(world, { .add = ecs_ids( ecs_pair(EcsIsA, e0)) });
    ecs_entity_t e2 = ecs_entity(world, { .add = ecs_ids( ecs_pair(EcsIsA, e1)) });
    ecs_entity_t e3 = ecs_entity(world, { .add = ecs_ids( ecs_pair(EcsIsA, e2)) });

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag(cascade IsA)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_add_id(world, e1, Bar); /* mix up order */
    ecs_add_id(world, e2, Foo);

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(it.sources[0], e0);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(it.sources[0], e0);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e3);
    test_uint(it.sources[0], e0);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Cascade_new_isa_cascade(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag(cascade IsA)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t e0 = ecs_new_w(world, Tag);
    ecs_entity_t e1 = ecs_entity(world, { .add = ecs_ids( ecs_pair(EcsIsA, e0)) });
    ecs_entity_t e2 = ecs_entity(world, { .add = ecs_ids( ecs_pair(EcsIsA, e1)) });
    ecs_entity_t e3 = ecs_entity(world, { .add = ecs_ids( ecs_pair(EcsIsA, e2)) });

    ecs_add_id(world, e2, Foo); /* mix up order */
    ecs_add_id(world, e1, Bar);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(it.sources[0], e0);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(it.sources[0], e0);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e3);
    test_uint(it.sources[0], e0);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Cascade_childof_cascade(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag(cascade)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t e0 = ecs_new_w(world, Tag);
    ecs_entity_t e1 = ecs_entity(world, { .parent = e0 });
    ecs_entity_t e2 = ecs_entity(world, { .parent = e1 });
    ecs_entity_t e3 = ecs_entity(world, { .parent = e2 });

    ecs_add_id(world, e2, Foo); /* mix up order */
    ecs_add_id(world, e1, Bar);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(it.sources[0], e0);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(it.sources[0], e0);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e3);
    test_uint(it.sources[0], e0);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Cascade_parent_cascade(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag(cascade)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t e0 = ecs_new_w(world, Tag);
    ecs_entity_t e1 = ecs_entity(world, { .parent = e0 });
    ecs_entity_t e2 = ecs_entity(world, { .parent = e1 });
    ecs_entity_t e3 = ecs_entity(world, { .parent = e2 });

    ecs_add_id(world, e2, Foo); /* mix up order */
    ecs_add_id(world, e1, Bar);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(it.sources[0], e0);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(it.sources[0], e0);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e3);
    test_uint(it.sources[0], e0);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Cascade_existing_custom_rel_cascade(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_ENTITY(world, Rel, Traversable);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e0 = ecs_new_w(world, Tag);
    ecs_entity_t e1 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, e0)) });
    ecs_entity_t e2 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, e1)) });
    ecs_entity_t e3 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, e2)) });

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag(cascade Rel)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_add_id(world, e2, Foo); /* mix up order */
    ecs_add_id(world, e1, Bar);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(it.sources[0], e0);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(it.sources[0], e0);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e3);
    test_uint(it.sources[0], e0);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Cascade_new_custom_rel_cascade(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_ENTITY(world, Rel, Traversable);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag(cascade Rel)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t e0 = ecs_new_w(world, Tag);
    ecs_entity_t e1 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, e0)) });
    ecs_entity_t e2 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, e1)) });
    ecs_entity_t e3 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, e2)) });

    ecs_add_id(world, e2, Foo); /* mix up order */
    ecs_add_id(world, e1, Bar);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(it.sources[0], e0);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(it.sources[0], e0);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e3);
    test_uint(it.sources[0], e0);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Cascade_cascade_w_2_depths(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_ENTITY(world, Rel, Traversable);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag(cascade Rel)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t e0 = ecs_new_w(world, Tag);
    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_add_pair(world, e1, Rel, e0);
    ecs_add_pair(world, e2, Rel, e1);
    ecs_add_pair(world, e3, Rel, e2);

    ecs_add_id(world, e2, Foo); /* mix up order */
    ecs_add_id(world, e1, Bar);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(it.sources[0], e0);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(it.sources[0], e1);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e3);
    test_uint(it.sources[0], e1);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Cascade_cascade_w_3_depths(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_ENTITY(world, Rel, Traversable);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag(cascade Rel)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t e0 = ecs_new_w(world, Tag);
    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new(world);

    ecs_add_pair(world, e1, Rel, e0);
    ecs_add_pair(world, e2, Rel, e1);
    ecs_add_pair(world, e3, Rel, e2);

    ecs_add_id(world, e2, Foo); /* mix up order */
    ecs_add_id(world, e1, Bar);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(it.sources[0], e0);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(it.sources[0], e1);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e3);
    test_uint(it.sources[0], e2);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Cascade_cascade_w_2_depths_desc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_ENTITY(world, Rel, Traversable);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag(cascade|desc Rel)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t e0 = ecs_new_w(world, Tag);
    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new(world);
    ecs_entity_t e3 = ecs_new(world);

    ecs_add_pair(world, e1, Rel, e0);
    ecs_add_pair(world, e2, Rel, e1);
    ecs_add_pair(world, e3, Rel, e2);

    ecs_add_id(world, e2, Foo); /* mix up order */
    ecs_add_id(world, e1, Bar);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e3);
    test_uint(it.sources[0], e1);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(it.sources[0], e1);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(it.sources[0], e0);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Cascade_cascade_w_3_depths_desc(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_ENTITY(world, Rel, Traversable);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag(cascade|desc Rel)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t e0 = ecs_new_w(world, Tag);
    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new(world);

    ecs_add_pair(world, e1, Rel, e0);
    ecs_add_pair(world, e2, Rel, e1);
    ecs_add_pair(world, e3, Rel, e2);

    ecs_add_id(world, e2, Foo); /* mix up order */
    ecs_add_id(world, e1, Bar);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e3);
    test_uint(it.sources[0], e2);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(it.sources[0], e1);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(it.sources[0], e0);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Cascade_cascade_rematch_2_lvls(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e_0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e_1 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t e_2 = ecs_insert(world, ecs_value(Position, {50, 60}));
    ecs_entity_t e_3 = ecs_insert(world, ecs_value(Position, {70, 80}));
    ecs_add_pair(world, e_3, EcsChildOf, e_2);

    ecs_add_pair(world, e_2, EcsChildOf, e_1);
    ecs_add_pair(world, e_1, EcsChildOf, e_0);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(cascade)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e_1);
        test_uint(it.sources[0], e_0);
        {
            Position *p = ecs_field(&it, Position, 0);
            test_int(p[0].x, 10);
            test_int(p[0].y, 20);
        }

        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e_2);
        test_uint(it.sources[0], e_1);
        {
            Position *p = ecs_field(&it, Position, 0);
            test_int(p[0].x, 30);
            test_int(p[0].y, 40);
        }

        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e_3);
        test_uint(it.sources[0], e_2);
        {
            Position *p = ecs_field(&it, Position, 0);
            test_int(p[0].x, 50);
            test_int(p[0].y, 60);
            test_bool(false, ecs_query_next(&it));
        }
    }

    ecs_remove_pair(world, e_1, EcsChildOf, EcsWildcard);
    ecs_remove_pair(world, e_2, EcsChildOf, EcsWildcard);
    ecs_remove_pair(world, e_3, EcsChildOf, EcsWildcard);

    ecs_add_pair(world, e_0, EcsChildOf, e_1);
    ecs_add_pair(world, e_1, EcsChildOf, e_2);
    ecs_add_pair(world, e_2, EcsChildOf, e_3);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e_2);
        test_uint(it.sources[0], e_3);
        {
            Position *p = ecs_field(&it, Position, 0);
            test_int(p[0].x, 70);
            test_int(p[0].y, 80);
        }

        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e_1);
        test_uint(it.sources[0], e_2);
        {
            Position *p = ecs_field(&it, Position, 0);
            test_int(p[0].x, 50);
            test_int(p[0].y, 60);
        }

        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e_0);
        test_uint(it.sources[0], e_1);
        {
            Position *p = ecs_field(&it, Position, 0);
            test_int(p[0].x, 30);
            test_int(p[0].y, 40);
        }

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Cascade_cascade_rematch_2_lvls_2_relations(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, R, Traversable);
    ECS_COMPONENT(world, Position);

    ecs_entity_t e_0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e_1 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t e_2 = ecs_insert(world, ecs_value(Position, {50, 60}));
    ecs_entity_t e_3 = ecs_insert(world, ecs_value(Position, {70, 80}));

    ecs_add_pair(world, e_3, R, e_2);
    ecs_add_pair(world, e_2, R, e_1);
    ecs_add_pair(world, e_1, R, e_0);

    ecs_entity_t t = ecs_new(world);
    ecs_add(world, t, Position);
    ecs_add_pair(world, t, R, e_2);
    ecs_add_pair(world, t, R, e_1);
    ecs_delete(world, t);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(cascade R)",
        .cache_kind = EcsQueryCacheAuto
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e_1);
    test_uint(it.sources[0], e_0);
    Position *p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e_2);
    test_uint(it.sources[0], e_1);
    p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e_3);
    test_uint(it.sources[0], e_2);
    p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 50);
    test_int(p[0].y, 60);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Cascade_cascade_topological(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, R, Traversable);
    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_entity_t e6 = ecs_new_w(world, Tag);

    ecs_add_pair(world, e3, R, e1);
    ecs_add_pair(world, e3, R, e2);
    ecs_add_pair(world, e3, R, e4);
    ecs_add_pair(world, e1, R, e5);
    ecs_add_pair(world, e2, R, e6);
    ecs_add_pair(world, e4, R, e1);
    ecs_add_pair(world, e4, R, e2);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, ?Tag(cascade R)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(it.entities[0], e5);
    test_uint(it.entities[1], e6);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], e1);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], e2);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], e4);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], e3);

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Cascade_cascade_desc_rematch_2_lvls(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e_0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e_1 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t e_2 = ecs_insert(world, ecs_value(Position, {50, 60}));
    ecs_entity_t e_3 = ecs_insert(world, ecs_value(Position, {70, 80}));
    
    ecs_add_pair(world, e_3, EcsChildOf, e_2);
    ecs_add_pair(world, e_2, EcsChildOf, e_1);
    ecs_add_pair(world, e_1, EcsChildOf, e_0);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(cascade|desc)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e_3);
    test_uint(it.sources[0], e_2);
    Position *p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 50);
    test_int(p[0].y, 60);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e_2);
    test_uint(it.sources[0], e_1);
    p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e_1);
    test_uint(it.sources[0], e_0);
    p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);

    test_bool(false, ecs_query_next(&it));

    ecs_remove_pair(world, e_1, EcsChildOf, EcsWildcard);
    ecs_remove_pair(world, e_2, EcsChildOf, EcsWildcard);
    ecs_remove_pair(world, e_3, EcsChildOf, EcsWildcard);

    ecs_add_pair(world, e_0, EcsChildOf, e_1);
    ecs_add_pair(world, e_1, EcsChildOf, e_2);
    ecs_add_pair(world, e_2, EcsChildOf, e_3);

    it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e_0);
    test_uint(it.sources[0], e_1);
    p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e_1);
    test_uint(it.sources[0], e_2);
    p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 50);
    test_int(p[0].y, 60);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e_2);
    test_uint(it.sources[0], e_3);
    p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 70);
    test_int(p[0].y, 80);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Cascade_cascade_desc_rematch_2_lvls_2_relations(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, R, Traversable);
    ECS_COMPONENT(world, Position);

    ecs_entity_t e_0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e_1 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t e_2 = ecs_insert(world, ecs_value(Position, {50, 60}));
    ecs_entity_t e_3 = ecs_insert(world, ecs_value(Position, {70, 80}));

    ecs_add_pair(world, e_3, R, e_2);
    ecs_add_pair(world, e_2, R, e_1);
    ecs_add_pair(world, e_1, R, e_0);

    ecs_entity_t t = ecs_new(world);
    ecs_add(world, t, Position);
    ecs_add_pair(world, t, R, e_2);
    ecs_add_pair(world, t, R, e_1);
    ecs_delete(world, t);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(cascade|desc R)",
        .cache_kind = EcsQueryCacheAuto
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e_3);
    test_uint(it.sources[0], e_2);
    Position *p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 50);
    test_int(p[0].y, 60);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e_2);
    test_uint(it.sources[0], e_1);
    p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e_1);
    test_uint(it.sources[0], e_0);
    p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Cascade_cascade_desc_topological(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, R, Traversable);
    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_entity_t e6 = ecs_new_w(world, Tag);

    ecs_add_pair(world, e3, R, e1);
    ecs_add_pair(world, e3, R, e2);
    ecs_add_pair(world, e3, R, e4);
    ecs_add_pair(world, e1, R, e5);
    ecs_add_pair(world, e2, R, e6);
    ecs_add_pair(world, e4, R, e1);
    ecs_add_pair(world, e4, R, e2);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, ?Tag(cascade|desc R)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], e3);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], e4);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], e1);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(it.entities[0], e2);

    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(it.entities[0], e5);
    test_uint(it.entities[1], e6);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Cascade_cascade_after_recycled_parent_change(void) {
    ecs_world_t *world = ecs_mini();
    
    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, ?Position(cascade)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(q != NULL);

    ecs_entity_t parent = ecs_new(world);
    test_assert(parent != 0);
    ecs_entity_t child = ecs_new(world);
    test_assert(child != 0);

    ecs_delete(world, parent);
    test_assert( !ecs_is_alive(world, parent));
    ecs_delete(world, child);
    test_assert( !ecs_is_alive(world, child));

    parent = ecs_new_w_id(world, Tag);
    test_assert(parent != 0);
    child = ecs_new_w_id(world, Tag);
    test_assert(child != 0);
    ecs_add_pair(world, child, EcsChildOf, parent);

    ecs_entity_t e = ecs_new_w_id(world, Tag);
    test_assert(e != 0);

    ecs_add_pair(world, e, EcsChildOf, child);
    test_assert( ecs_has_pair(world, e, EcsChildOf, child));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_uint(it.entities[0], parent);
    test_assert(ecs_field_src(&it, 1) == 0);
    test_assert(!ecs_field_is_set(&it, 1));
    const Position *p = ecs_field(&it, Position, 1);
    test_assert(p == NULL);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_uint(it.entities[0], child);
    test_assert(ecs_field_src(&it, 1) == 0);
    test_assert(!ecs_field_is_set(&it, 1));
    p = ecs_field(&it, Position, 1);
    test_assert(p == NULL);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_uint(it.entities[0], e);
    test_assert(ecs_field_src(&it, 1) == 0);
    test_assert(!ecs_field_is_set(&it, 1));
    p = ecs_field(&it, Position, 1);
    test_assert(p == NULL);

    test_bool(ecs_query_next(&it), false);

    ecs_set(world, parent, Position, {10, 20});
    ecs_set(world, child, Position, {20, 30});

    ecs_run_aperiodic(world, 0);

    it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_uint(it.entities[0], parent);
    test_assert(!ecs_field_is_set(&it, 1));
    p = ecs_field(&it, Position, 1);
    test_assert(p == NULL);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_uint(it.entities[0], child);
    test_assert(ecs_field_src(&it, 1) == parent);
    test_assert(ecs_field_is_set(&it, 1));
    p = ecs_field(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_uint(it.entities[0], e);
    test_assert(ecs_field_src(&it, 1) == child);
    test_assert(ecs_field_is_set(&it, 1));
    p = ecs_field(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 30);

    test_bool(ecs_query_next(&it), false);

    ecs_fini(world);
}
