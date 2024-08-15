#include <query.h>

static ecs_query_cache_kind_t cache_kind = EcsQueryCacheDefault;

void Traversal_setup(void) {
    const char *cache_param = test_param("cache_kind");
    if (cache_param) {
        if (!strcmp(cache_param, "default")) {
            // already set to default
        } else if (!strcmp(cache_param, "auto")) {
            cache_kind = EcsQueryCacheAuto;
        } else {
            printf("unexpected value for cache_param '%s'\n", cache_param);
        }
    }
}

void Traversal_this_self_up_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

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

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(self|up)",
        .cache_kind = cache_kind
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

void Traversal_this_up_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

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

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

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

void Traversal_this_written_self_up_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

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

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Foo(self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

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

void Traversal_this_written_up_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

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

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Foo(up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

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

void Traversal_var_self_up_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

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

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo($x|self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(p1, ecs_iter_get_var(&it, x_var));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(p2, ecs_iter_get_var(&it, x_var));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(e1, ecs_iter_get_var(&it, x_var));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(e2, ecs_iter_get_var(&it, x_var));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(e5, ecs_iter_get_var(&it, x_var));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(e6, ecs_iter_get_var(&it, x_var));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(e3, ecs_iter_get_var(&it, x_var));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(e4, ecs_iter_get_var(&it, x_var));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(p3, ecs_iter_get_var(&it, x_var));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(e7, ecs_iter_get_var(&it, x_var));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_var_up_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

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

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo($x|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(e1, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(e2, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(e5, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(e6, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(e3, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(e4, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(e7, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_var_written_self_up_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

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

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag($x|self), Foo($x|self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(p1, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(p2, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(p3, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(p3, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e1, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e2, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e2, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e3, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e3, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e4, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e4, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e5, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e5, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e6, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e6, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e7, ecs_field_src(&it, 0));
    test_uint(p3, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e7, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_var_written_up_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

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

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag($x|self), Foo($x|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e1, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e2, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e2, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e3, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e3, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e4, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e4, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e5, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e5, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e6, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e6, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e7, ecs_field_src(&it, 0));
    test_uint(p3, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(e7, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_set_var_self_up_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

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

    ecs_entity(world, { .parent = p3 });

    ecs_entity_t e8 = ecs_entity(world, { .parent = p0 });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo($x|self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        ecs_iter_set_var(&it, x_var, p2);

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(p2, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(p2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        ecs_iter_set_var(&it, x_var, e1);

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(p1, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        ecs_iter_set_var(&it, x_var, e3);

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(p2, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(e3, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        ecs_iter_set_var(&it, x_var, e5);

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(p1, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(e5, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        ecs_iter_set_var(&it, x_var, e8);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_set_var_up_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

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

    ecs_entity(world, { .parent = p3 });

    ecs_entity_t e8 = ecs_entity(world, { .parent = p0 });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo($x|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        ecs_iter_set_var(&it, x_var, p2);

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        ecs_iter_set_var(&it, x_var, e1);

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(p1, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        ecs_iter_set_var(&it, x_var, e3);

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(p2, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(e3, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        ecs_iter_set_var(&it, x_var, e5);

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(p1, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(e5, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        ecs_iter_set_var(&it, x_var, e8);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_set_var_written_self_up_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

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

    ecs_entity(world, { .parent = p3 });

    ecs_entity_t e8 = ecs_entity(world, { .parent = p0 });
    ecs_set_with(world, 0);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag($x|self), Foo($x|self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        ecs_iter_set_var(&it, x_var, p2);

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(p2, ecs_field_src(&it, 0));
        test_uint(p2, ecs_field_src(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(p2, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        ecs_iter_set_var(&it, x_var, e1);

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(p1, ecs_field_src(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        ecs_iter_set_var(&it, x_var, e3);

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e3, ecs_field_src(&it, 0));
        test_uint(p2, ecs_field_src(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e3, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        ecs_iter_set_var(&it, x_var, e5);

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e5, ecs_field_src(&it, 0));
        test_uint(p1, ecs_field_src(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e5, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        ecs_iter_set_var(&it, x_var, e8);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_set_var_written_up_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag($x|self), Foo($x|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

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

    ecs_entity(world, { .parent = p3 });

    ecs_entity_t e8 = ecs_entity(world, { .parent = p0 });
    ecs_set_with(world, 0);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        ecs_iter_set_var(&it, x_var, p2);

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        ecs_iter_set_var(&it, x_var, e1);

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(p1, ecs_field_src(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        ecs_iter_set_var(&it, x_var, e3);

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e3, ecs_field_src(&it, 0));
        test_uint(p2, ecs_field_src(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e3, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        ecs_iter_set_var(&it, x_var, e5);

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e5, ecs_field_src(&it, 0));
        test_uint(p1, ecs_field_src(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(e5, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        ecs_iter_set_var(&it, x_var, e8);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_ent_self_up_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t e = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(ent|self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove(world, e, Foo);
    ecs_add_pair(world, e, EcsChildOf, parent);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, parent, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(parent, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_ent_up_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t e = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(ent|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e, EcsChildOf, parent);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, parent, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(parent, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_implicit_this_self_up_isa(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_entity(world, { .add = ecs_ids(ecs_isa(p1)) });
    ecs_entity_t e2 = ecs_entity(world, { .add = ecs_ids(ecs_isa(p1)) });
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_entity(world, { .add = ecs_ids(ecs_isa(p2)) });
    ecs_entity_t e4 = ecs_entity(world, { .add = ecs_ids(ecs_isa(p2)) });
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_entity(world, { .add = ecs_ids(ecs_isa(e2)) });
    ecs_entity_t e6 = ecs_entity(world, { .add = ecs_ids(ecs_isa(e2)) });
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_entity(world, { .add = ecs_ids(ecs_isa(p3)) });

    ecs_entity(world, { .add = ecs_ids(ecs_isa(p0)) });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo",
        .cache_kind = cache_kind
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

void Traversal_implicit_this_up_isa(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_entity(world, { .add = ecs_ids(ecs_isa(p1)) });
    ecs_entity_t e2 = ecs_entity(world, { .add = ecs_ids(ecs_isa(p1)) });
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_entity(world, { .add = ecs_ids(ecs_isa(p2)) });
    ecs_entity_t e4 = ecs_entity(world, { .add = ecs_ids(ecs_isa(p2)) });
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_entity(world, { .add = ecs_ids(ecs_isa(e2)) });
    ecs_entity_t e6 = ecs_entity(world, { .add = ecs_ids(ecs_isa(e2)) });
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_entity(world, { .add = ecs_ids(ecs_isa(p3)) });

    ecs_entity(world, { .add = ecs_ids(ecs_isa(p0)) });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(up IsA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

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

void Traversal_implicit_var_self_up_isa(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo($x)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_entity(world, { .add = ecs_ids(ecs_isa(p1)) });
    ecs_entity_t e2 = ecs_entity(world, { .add = ecs_ids(ecs_isa(p1)) });
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_entity(world, { .add = ecs_ids(ecs_isa(p2)) });
    ecs_entity_t e4 = ecs_entity(world, { .add = ecs_ids(ecs_isa(p2)) });
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_entity(world, { .add = ecs_ids(ecs_isa(e2)) });
    ecs_entity_t e6 = ecs_entity(world, { .add = ecs_ids(ecs_isa(e2)) });
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_entity(world, { .add = ecs_ids(ecs_isa(p3)) });

    ecs_entity(world, { .add = ecs_ids(ecs_isa(p0)) });

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p1, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p2, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(e1, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(e2, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(e5, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(e6, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(e3, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(e4, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(p3, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(e7, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_implicit_var_up_isa(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo($x|up IsA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_entity(world, { .add = ecs_ids(ecs_isa(p1)) });
    ecs_entity_t e2 = ecs_entity(world, { .add = ecs_ids(ecs_isa(p1)) });
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_entity(world, { .add = ecs_ids(ecs_isa(p2)) });
    ecs_entity_t e4 = ecs_entity(world, { .add = ecs_ids(ecs_isa(p2)) });
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_entity(world, { .add = ecs_ids(ecs_isa(e2)) });
    ecs_entity_t e6 = ecs_entity(world, { .add = ecs_ids(ecs_isa(e2)) });
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_entity(world, { .add = ecs_ids(ecs_isa(p3)) });

    ecs_entity(world, { .add = ecs_ids(ecs_isa(p0)) });

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(e1, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(e2, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(e5, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(e6, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(e3, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(e4, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(e7, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_implicit_ent_self_up_isa(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t e = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(ent|self|up IsA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_remove(world, e, Foo);
    ecs_add_pair(world, e, EcsIsA, parent);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, parent, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(parent, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_implicit_ent_up_isa(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t e = ecs_entity(world, { .name = "ent" });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(ent|up IsA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add_pair(world, e, EcsIsA, parent);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, parent, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(parent, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_self_up_2_targets(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(self|up IsA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t parent_b = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t parent_c = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t e_1 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_a)) });
    ecs_add_pair(world, e_1, EcsIsA, parent_b);
    ecs_entity_t e_2 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_b)) });
    ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_a)) });
    ecs_entity_t e_4 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_b)) });
    ecs_add_pair(world, e_4, EcsIsA, parent_c);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(parent_b, it.entities[0]);
    test_uint(parent_c, it.entities[1]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_1, it.entities[0]);
    test_uint(parent_b, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_2, it.entities[0]);
    test_uint(parent_b, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_4, it.entities[0]);
    test_uint(parent_b, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_up_2_targets(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(up IsA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t parent_b = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t parent_c = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t e_1 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_a)) });
    ecs_add_pair(world, e_1, EcsIsA, parent_b);
    ecs_entity_t e_2 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_b)) });
    ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_a)) });
    ecs_entity_t e_4 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_b)) });
    ecs_add_pair(world, e_4, EcsIsA, parent_c);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_1, it.entities[0]);
    test_uint(parent_b, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_2, it.entities[0]);
    test_uint(parent_b, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_4, it.entities[0]);
    test_uint(parent_b, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_self_up_2_targets_diamond(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_entity_t parent_root = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t parent_a = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_root)) });
    ecs_entity_t parent_b = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_root)) });
    ecs_entity_t parent_c = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_a)) });
    ecs_add_pair(world, parent_c, EcsIsA, parent_b);

    ecs_entity_t e_1 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_a)) });
    ecs_entity_t e_2 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_b)) });
    ecs_entity_t e_3 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_c)) });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(self|up IsA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent_root, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(parent_a, it.entities[0]);
    test_uint(parent_b, it.entities[1]);
    test_uint(parent_root, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_1, it.entities[0]);
    test_uint(parent_root, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent_c, it.entities[0]);
    test_uint(parent_root, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_3, it.entities[0]);
    test_uint(parent_root, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_2, it.entities[0]);
    test_uint(parent_root, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_up_2_targets_diamond(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_entity_t parent_root = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t parent_a = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_root)) });
    ecs_entity_t parent_b = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_root)) });
    ecs_entity_t parent_c = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_a)) });
    ecs_add_pair(world, parent_c, EcsIsA, parent_b);

    ecs_entity_t e_1 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_a)) });
    ecs_entity_t e_2 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_b)) });
    ecs_entity_t e_3 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_c)) });

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(up IsA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(parent_a, it.entities[0]);
    test_uint(parent_b, it.entities[1]);
    test_uint(parent_root, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_1, it.entities[0]);
    test_uint(parent_root, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent_c, it.entities[0]);
    test_uint(parent_root, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_3, it.entities[0]);
    test_uint(parent_root, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_2, it.entities[0]);
    test_uint(parent_root, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_written_self_up_2_targets(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Foo(self|up IsA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);
    
    ecs_set_with(world, Tag);
    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t parent_b = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t parent_c = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t e_1 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_a)) });
    ecs_add_pair(world, e_1, EcsIsA, parent_b);
    ecs_entity_t e_2 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_b)) });
    ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_a)) });
    ecs_entity_t e_4 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_b)) });
    ecs_add_pair(world, e_4, EcsIsA, parent_c);
    ecs_set_with(world, 0);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(parent_b, it.entities[0]);
    test_uint(parent_c, it.entities[1]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent_b, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent_b, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_4, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent_b, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_written_up_2_targets(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Foo(up IsA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);
    
    ecs_set_with(world, Tag);
    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t parent_b = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t parent_c = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t e_1 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_a)) });
    ecs_add_pair(world, e_1, EcsIsA, parent_b);
    ecs_entity_t e_2 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_b)) });
    ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_a)) });
    ecs_entity_t e_4 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_b)) });
    ecs_add_pair(world, e_4, EcsIsA, parent_c);
    ecs_set_with(world, 0);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent_b, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent_b, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_4, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent_b, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_written_self_up_2_targets_diamond(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Foo(self|up IsA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t parent_root = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t parent_a = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_root)) });
    ecs_entity_t parent_b = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_root)) });
    ecs_entity_t parent_c = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_a)) });
    ecs_add_pair(world, parent_c, EcsIsA, parent_b);

    ecs_entity_t e_1 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_a)) });
    ecs_entity_t e_2 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_b)) });
    ecs_entity_t e_3 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_c)) });
    ecs_set_with(world, 0);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent_root, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(parent_a, it.entities[0]);
    test_uint(parent_b, it.entities[1]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent_root, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent_root, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent_c, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent_root, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent_root, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent_root, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_written_up_2_targets_diamond(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Foo(up IsA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t parent_root = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t parent_a = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_root)) });
    ecs_entity_t parent_b = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_root)) });
    ecs_entity_t parent_c = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_a)) });
    ecs_add_pair(world, parent_c, EcsIsA, parent_b);

    ecs_entity_t e_1 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_a)) });
    ecs_entity_t e_2 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_b)) });
    ecs_entity_t e_3 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_c)) });
    ecs_set_with(world, 0);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(parent_a, it.entities[0]);
    test_uint(parent_b, it.entities[1]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent_root, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent_root, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent_c, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent_root, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent_root, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent_root, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_2_self_up_terms(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_set_with(world, Tag);
    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t parent_b = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_a)) });
    ecs_entity_t e_2 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_b)) });
    ecs_entity_t e_3 = ecs_entity(world, { .add = ecs_ids(ecs_isa(e_2)) });
    ecs_set_with(world, 0);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag, Foo(self|up IsA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent_b, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent_b, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent_b, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_2_self_up_terms_2_targets(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_set_with(world, Tag);
    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t parent_b = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t parent_c = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t e_1 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_a)) });
    ecs_add_pair(world, e_1, EcsIsA, parent_b);
    ecs_entity_t e_2 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_b)) });
    ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_a)) });
    ecs_entity_t e_4 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_b)) });
    ecs_add_pair(world, e_4, EcsIsA, parent_c);
    ecs_set_with(world, 0);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag, Foo(self|up IsA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(parent_b, it.entities[0]);
    test_uint(parent_c, it.entities[1]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent_b, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent_b, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_4, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent_b, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_self_up_empty_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(self|up IsA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);
    
    ecs_entity_t e1 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t e2 = ecs_entity(world, { .add = ecs_ids(ecs_isa(e1)) });
    ecs_delete(world, e2);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_up_empty_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(up IsA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);
    
    ecs_entity_t e1 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t e2 = ecs_entity(world, { .add = ecs_ids(ecs_isa(e1)) });
    ecs_delete(world, e2);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_self_up_match_empty_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }},
        .cache_kind = cache_kind,
        .flags = EcsQueryMatchEmptyTables
    });

    ecs_entity_t base = ecs_new(world);
    ecs_table_t *table = ecs_table_add_id(world, NULL, ecs_pair(EcsIsA, base));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, base, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(base, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_assert(table == it.table);
        test_int(0, it.count);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(base, ecs_field_src(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_up_match_empty_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo, .src.id = EcsUp, .trav = EcsIsA }},
        .cache_kind = cache_kind,
        .flags = EcsQueryMatchEmptyTables
    });

    ecs_entity_t base = ecs_new_w(world, Foo);
    ecs_table_t *table = ecs_table_add_id(world, NULL, ecs_pair(EcsIsA, base));

    {
        ecs_iter_t it = ecs_query_iter(world, q);

        test_bool(true, ecs_query_next(&it));
        test_assert(table == it.table);
        test_int(0, it.count);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(base, ecs_field_src(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_self_up_all_owned(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self|up IsA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t parent_root = ecs_entity(world, {0});
    ecs_entity_t parent_a = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_root)) });
    ecs_entity_t parent_b = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_root)) });
    ecs_entity_t parent_c = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_a)) });
    ecs_add_pair(world, parent_c, EcsIsA, parent_b);

    ecs_entity_t e_1 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_a)) });
    ecs_entity_t e_2 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_b)) });
    ecs_entity_t e_3 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_c)) });
    ecs_set_with(world, 0);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent_root, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Tag, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(parent_a, it.entities[0]);
    test_uint(parent_b, it.entities[1]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Tag, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Tag, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent_c, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Tag, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Tag, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Tag, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_up_all_owned(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(up IsA)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t parent_root = ecs_entity(world, {0});
    ecs_entity_t parent_a = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_root)) });
    ecs_entity_t parent_b = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_root)) });
    ecs_entity_t parent_c = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_a)) });
    ecs_add_pair(world, parent_c, EcsIsA, parent_b);

    ecs_entity_t e_1 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_a)) });
    ecs_entity_t e_2 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_b)) });
    ecs_entity_t e_3 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_c)) });
    ecs_set_with(world, 0);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(parent_a, it.entities[0]);
    test_uint(parent_b, it.entities[1]);
    test_uint(parent_root, ecs_field_src(&it, 0));
    test_uint(Tag, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_1, it.entities[0]);
    test_uint(parent_a, ecs_field_src(&it, 0));
    test_uint(Tag, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(parent_c, it.entities[0]);
    test_uint(parent_a, ecs_field_src(&it, 0));
    test_uint(Tag, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_2, it.entities[0]);
    test_uint(parent_b, ecs_field_src(&it, 0));
    test_uint(Tag, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e_3, it.entities[0]);
    test_uint(parent_c, ecs_field_src(&it, 0));
    test_uint(Tag, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_self_up_childof_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_entity_t base = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t parent = ecs_entity(world, { .add = ecs_ids(ecs_isa(base)) });
    ecs_entity_t child = ecs_entity(world, { .parent = parent });
    
    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(base, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(child, it.entities[0]);
    test_uint(base, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_up_childof_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_entity_t base = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t parent = ecs_entity(world, { .add = ecs_ids(ecs_isa(base)) });
    ecs_entity_t child = ecs_entity(world, { .parent = parent });
    
    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(child, it.entities[0]);
    test_uint(base, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_written_self_up_childof_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Foo(self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t base = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t parent = ecs_entity(world, { .add = ecs_ids(ecs_isa(base)) });
    ecs_entity_t child = ecs_entity(world, { .parent = parent });
    ecs_set_with(world, 0);
    
    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(base, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(child, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(base, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_written_up_childof_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Foo(up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t base = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t parent = ecs_entity(world, { .add = ecs_ids(ecs_isa(base)) });
    ecs_entity_t child = ecs_entity(world, { .parent = parent });
    ecs_set_with(world, 0);
    
    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(child, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(base, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_var_self_up_childof_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo($x|self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t base = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t parent = ecs_entity(world, { .add = ecs_ids(ecs_isa(base)) });
    ecs_entity_t child = ecs_entity(world, { .parent = parent });
    
    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(base, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(base, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(base, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(child, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_var_up_childof_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo($x|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t base = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t parent = ecs_entity(world, { .add = ecs_ids(ecs_isa(base)) });
    ecs_entity_t child = ecs_entity(world, { .parent = parent });
    
    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(base, ecs_field_src(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(child, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_var_written_self_up_childof_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag($x|self), Foo($x|self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_set_with(world, Tag);
    ecs_entity_t base = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t parent = ecs_entity(world, { .add = ecs_ids(ecs_isa(base)) });
    ecs_entity_t child = ecs_entity(world, { .parent = parent });
    ecs_set_with(world, 0);
    
    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(base, ecs_field_src(&it, 0));
    test_uint(base, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(base, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(child, ecs_field_src(&it, 0));
    test_uint(base, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(child, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_var_written_up_childof_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag($x|self), Foo($x|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_set_with(world, Tag);
    ecs_entity_t base = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t parent = ecs_entity(world, { .add = ecs_ids(ecs_isa(base)) });
    ecs_entity_t child = ecs_entity(world, { .parent = parent });
    ecs_set_with(world, 0);
    
    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(child, ecs_field_src(&it, 0));
    test_uint(base, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_uint(child, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_ent_self_up_childof_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_entity_t base = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t parent = ecs_entity(world, { .add = ecs_ids(ecs_isa(base)) });
    ecs_entity_t child = ecs_entity(world, { .parent = parent });
    ecs_set_name(world, parent, "parent");
    ecs_set_name(world, child, "child");

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(parent.child|self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);
    
    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(base, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, child, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(child, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_ent_up_childof_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_entity_t base = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t parent = ecs_entity(world, { .add = ecs_ids(ecs_isa(base)) });
    ecs_entity_t child = ecs_entity(world, { .parent = parent });
    ecs_set_name(world, parent, "parent");
    ecs_set_name(world, child, "child");

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(parent.child|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);
    
    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(base, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, child, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(base, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_ent_written_self_up_childof_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_set_with(world, Tag);
    ecs_entity_t base = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t parent = ecs_entity(world, { .add = ecs_ids(ecs_isa(base)) });
    ecs_entity_t child = ecs_entity(world, { .parent = parent });
    ecs_set_name(world, parent, "parent");
    ecs_set_name(world, child, "child");
    ecs_set_with(world, 0);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(parent.child|self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);
    
    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(base, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, child, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(child, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_ent_written_up_childof_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_set_with(world, Tag);
    ecs_entity_t base = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity_t parent = ecs_entity(world, { .add = ecs_ids(ecs_isa(base)) });
    ecs_entity_t child = ecs_entity(world, { .parent = parent });
    ecs_set_name(world, parent, "parent");
    ecs_set_name(world, child, "child");
    ecs_set_with(world, 0);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(parent.child|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);
    
    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(base, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, child, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(base, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_self_up_childof_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t p2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_entity_t p3 = ecs_insert(world, ecs_value(Position, {30, 40}));
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
        .expr = "Position(self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(p1, it.entities[0]);
    test_uint(p2, it.entities[1]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
        test_int(p[1].x, 20);
        test_int(p[1].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 30);
        test_int(p[0].y, 40);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 30);
        test_int(p[0].y, 40);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_up_childof_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t p2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_entity_t p3 = ecs_insert(world, ecs_value(Position, {30, 40}));
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
        .expr = "Position(up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 30);
        test_int(p[0].y, 40);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_written_self_up_childof_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Position(self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_insert(world, {Tag}, ecs_value(Position, {10, 20}));
    ecs_entity_t p2 = ecs_insert(world, {Tag}, ecs_value(Position, {20, 30}));
    ecs_entity_t p3 = ecs_insert(world, {Tag}, ecs_value(Position, {30, 40}));
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
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
        test_int(p[1].x, 20);
        test_int(p[1].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 30);
        test_int(p[0].y, 40);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p3, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 30);
        test_int(p[0].y, 40);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_written_up_childof_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Position(up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_insert(world, {Tag}, ecs_value(Position, {10, 20}));
    ecs_entity_t p2 = ecs_insert(world, {Tag}, ecs_value(Position, {20, 30}));
    ecs_entity_t p3 = ecs_insert(world, {Tag}, ecs_value(Position, {30, 40}));
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
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p3, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 30);
        test_int(p[0].y, 40);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_var_self_up_childof_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position($x|self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t p2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_entity_t p3 = ecs_insert(world, ecs_value(Position, {30, 40}));
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
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e1, ecs_iter_get_var(&it, x_var));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e2, ecs_iter_get_var(&it, x_var));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e5, ecs_iter_get_var(&it, x_var));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e6, ecs_iter_get_var(&it, x_var));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e3, ecs_iter_get_var(&it, x_var));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e4, ecs_iter_get_var(&it, x_var));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(p3, ecs_iter_get_var(&it, x_var));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 30);
        test_int(p[0].y, 40);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e7, ecs_iter_get_var(&it, x_var));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 30);
        test_int(p[0].y, 40);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_var_up_childof_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position($x|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t p2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_entity_t p3 = ecs_insert(world, ecs_value(Position, {30, 40}));
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
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e1, ecs_iter_get_var(&it, x_var));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e2, ecs_iter_get_var(&it, x_var));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e5, ecs_iter_get_var(&it, x_var));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e6, ecs_iter_get_var(&it, x_var));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e3, ecs_iter_get_var(&it, x_var));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e4, ecs_iter_get_var(&it, x_var));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(e7, ecs_iter_get_var(&it, x_var));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 30);
        test_int(p[0].y, 40);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_var_written_self_up_childof_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag($x|self), Position($x|self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_insert(world, {Tag}, ecs_value(Position, {10, 20}));
    ecs_entity_t p2 = ecs_insert(world, {Tag}, ecs_value(Position, {20, 30}));
    ecs_entity_t p3 = ecs_insert(world, {Tag}, ecs_value(Position, {30, 40}));
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
    test_int(0, it.count);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(p1, ecs_iter_get_var(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(p2, ecs_iter_get_var(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(p3, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 30);
        test_int(p[0].y, 40);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(e1, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e2, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(e2, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e3, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(e3, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e4, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(e4, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e5, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(e5, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e6, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(e6, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e7, ecs_field_src(&it, 0));
    test_uint(p3, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(e7, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 30);
        test_int(p[0].y, 40);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_var_written_up_childof_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag($x|self), Position($x|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_insert(world, {Tag}, ecs_value(Position, {10, 20}));
    ecs_entity_t p2 = ecs_insert(world, {Tag}, ecs_value(Position, {20, 30}));
    ecs_entity_t p3 = ecs_insert(world, {Tag}, ecs_value(Position, {30, 40}));
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
    test_int(0, it.count);
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(e1, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e2, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(e2, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e3, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(e3, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e4, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(e4, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e5, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(e5, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e6, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(e6, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(e7, ecs_field_src(&it, 0));
    test_uint(p3, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(e7, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 30);
        test_int(p[0].y, 40);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_self_up_childof_recycled_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_delete(world, ecs_new(world));
    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_add(world, p3, Bar);

    test_assert((uint32_t)p0 != p0);
    test_assert((uint32_t)p1 != p1);
    test_assert((uint32_t)p2 != p2);
    test_assert((uint32_t)p3 != p3);

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
        .expr = "Foo(self|up)",
        .cache_kind = cache_kind
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

void Traversal_this_up_childof_recycled_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_delete(world, ecs_new(world));
    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_add(world, p3, Bar);

    test_assert((uint32_t)p0 != p0);
    test_assert((uint32_t)p1 != p1);
    test_assert((uint32_t)p2 != p2);
    test_assert((uint32_t)p3 != p3);

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
        .expr = "Foo(up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

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

void Traversal_this_written_self_up_childof_recycled_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Foo(self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_add(world, p3, Bar);

    test_assert((uint32_t)p0 != p0);
    test_assert((uint32_t)p1 != p1);
    test_assert((uint32_t)p2 != p2);
    test_assert((uint32_t)p3 != p3);

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

void Traversal_this_written_up_childof_recycled_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Foo(up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_add(world, p3, Bar);

    test_assert((uint32_t)p0 != p0);
    test_assert((uint32_t)p1 != p1);
    test_assert((uint32_t)p2 != p2);
    test_assert((uint32_t)p3 != p3);

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

void Traversal_this_self_up_childof_recycled_parent_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_delete(world, ecs_new(world));
    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p3 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_add(world, p3, Bar);

    test_assert((uint32_t)p0 != p0);
    test_assert((uint32_t)p1 != p1);
    test_assert((uint32_t)p2 != p2);
    test_assert((uint32_t)p3 != p3);

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
        .expr = "Position(self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(p1, it.entities[0]);
    test_uint(p2, it.entities[1]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
        test_int(p[1].x, 20);
        test_int(p[1].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 30);
        test_int(p[0].y, 40);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 30);
        test_int(p[0].y, 40);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_up_childof_recycled_parent_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_delete(world, ecs_new(world));
    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p3 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_add(world, p3, Bar);

    test_assert((uint32_t)p0 != p0);
    test_assert((uint32_t)p1 != p1);
    test_assert((uint32_t)p2 != p2);
    test_assert((uint32_t)p3 != p3);

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
        .expr = "Position(up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p[0].x, 30);
        test_int(p[0].y, 40);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_written_self_up_childof_recycled_parent_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Position(self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p1 = ecs_insert(world, {Tag}, ecs_value(Position, {10, 20}));
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p2 = ecs_insert(world, {Tag}, ecs_value(Position, {20, 30}));
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p3 = ecs_insert(world, {Tag}, ecs_value(Position, {30, 40}));
    ecs_add(world, p3, Bar);

    test_assert((uint32_t)p0 != p0);
    test_assert((uint32_t)p1 != p1);
    test_assert((uint32_t)p2 != p2);
    test_assert((uint32_t)p3 != p3);

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
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
        test_int(p[1].x, 20);
        test_int(p[1].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 30);
        test_int(p[0].y, 40);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p3, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 30);
        test_int(p[0].y, 40);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_written_up_childof_recycled_parent_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Position(up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p1 = ecs_insert(world, {Tag}, ecs_value(Position, {10, 20}));
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p2 = ecs_insert(world, {Tag}, ecs_value(Position, {20, 30}));
    ecs_delete(world, ecs_new(world));
    ecs_entity_t p3 = ecs_insert(world, {Tag}, ecs_value(Position, {30, 40}));
    ecs_add(world, p3, Bar);

    test_assert((uint32_t)p0 != p0);
    test_assert((uint32_t)p1 != p1);
    test_assert((uint32_t)p2 != p2);
    test_assert((uint32_t)p3 != p3);

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
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 20);
        test_int(p[0].y, 30);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p3, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 30);
        test_int(p[0].y, 40);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_self_up_childof_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tgt, (OnInstantiate, Inherit));

    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, Tgt) ) });
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, Tgt) ) });
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, Tgt) ) });
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
        .expr = "Rel(self|up, Tgt)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(p1, it.entities[0]);
    test_uint(p2, it.entities[1]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_up_childof_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tgt, (OnInstantiate, Inherit));

    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, Tgt) ) });
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, Tgt) ) });
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, Tgt) ) });
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
        .expr = "Rel(up, Tgt)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_written_self_up_childof_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tgt, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Rel(self|up, Tgt)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, Tgt) ) });
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, Tgt) ) });
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, Tgt) ) });
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
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p3, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_written_up_childof_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tgt, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Rel(up, Tgt)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, Tgt) ) });
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, Tgt) ) });
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, Tgt) ) });
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
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p3, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_self_up_childof_pair_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TgtA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TgtB, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TgtC, (OnInstantiate, Inherit));

    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, TgtA) ) });
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, TgtB) ) });
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, TgtC) ) });
    ecs_add_pair(world, p3, EcsChildOf, p2);
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
        .expr = "Rel(self|up, *)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_up_childof_pair_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TgtA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TgtB, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TgtC, (OnInstantiate, Inherit));

    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, TgtA) ) });
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, TgtB) ) });
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, TgtC) ) });
    ecs_add_pair(world, p3, EcsChildOf, p2);
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
        .expr = "Rel(up, *)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p3, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_written_self_up_childof_pair_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TgtA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TgtB, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TgtC, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Rel(self|up, *)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, TgtA) ) });
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, TgtB) ) });
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, TgtC) ) });
    ecs_add(world, p3, Bar);
    ecs_add_pair(world, p3, EcsChildOf, p2);

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
    test_uint(p1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p3, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_written_up_childof_pair_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TgtA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TgtB, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TgtC, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Rel(up, *)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, TgtA) ) });
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, TgtB) ) });
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, TgtC) ) });
    ecs_add(world, p3, Bar);
    ecs_add_pair(world, p3, EcsChildOf, p2);

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
    test_uint(p3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p3, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_self_up_childof_pair_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TgtA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TgtB, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TgtC, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Rel(self|up, $x)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, TgtA) ) });
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, TgtB) ) });
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, TgtC) ) });
    ecs_add_pair(world, p3, EcsChildOf, p2);
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
    test_uint(p1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_uint(TgtA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_uint(TgtA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_uint(TgtA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_uint(TgtA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_uint(TgtA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));
    test_uint(TgtB, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));
    test_uint(TgtB, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));
    test_uint(TgtB, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 0));
    test_uint(TgtC, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 0));
    test_uint(TgtC, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_written_self_up_childof_pair_tgt_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TgtA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TgtB, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TgtC, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Rel(self|up, $x)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, TgtA) ) });
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, TgtB) ) });
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, TgtC) ) });
    ecs_add(world, p3, Bar);
    ecs_add_pair(world, p3, EcsChildOf, p2);

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
    test_uint(p1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));
    test_uint(TgtA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 1));
    test_uint(TgtB, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 1));
    test_uint(TgtC, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));
    test_uint(TgtA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));
    test_uint(TgtA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 1));
    test_uint(TgtB, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 1));
    test_uint(TgtB, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));
    test_uint(TgtA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 1));
    test_uint(TgtA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p3, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 1));
    test_uint(TgtC, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_self_up_childof_pair_rel_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, RelA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, RelB, (OnInstantiate, Inherit));
    ECS_ENTITY(world, RelC, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tgt, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "$x(self|up, Tgt)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids( ecs_pair(RelA, Tgt) ) });
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids( ecs_pair(RelB, Tgt) ) });
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids( ecs_pair(RelC, Tgt) ) });
    ecs_add_pair(world, p3, EcsChildOf, p2);
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
    test_uint(p1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 0));
    test_uint(RelA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 0));
    test_uint(RelA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 0));
    test_uint(RelA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 0));
    test_uint(RelA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(p1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 0));
    test_uint(RelA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 0));
    test_uint(RelB, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 0));
    test_uint(RelB, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 0));
    test_uint(RelB, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(ecs_pair(RelC, Tgt), ecs_field_id(&it, 0));
    test_uint(RelC, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(ecs_pair(RelC, Tgt), ecs_field_id(&it, 0));
    test_uint(RelC, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_written_self_up_childof_pair_rel_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, RelA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, RelB, (OnInstantiate, Inherit));
    ECS_ENTITY(world, RelC, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tgt, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), $x(self|up, Tgt)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_entity(world, { .add = ecs_ids( ecs_pair(RelA, Tgt) ) });
    ecs_entity_t p2 = ecs_entity(world, { .add = ecs_ids( ecs_pair(RelB, Tgt) ) });
    ecs_entity_t p3 = ecs_entity(world, { .add = ecs_ids( ecs_pair(RelC, Tgt) ) });
    ecs_add(world, p3, Bar);
    ecs_add_pair(world, p3, EcsChildOf, p2);

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
    test_uint(p1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 1));
    test_uint(RelA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 1));
    test_uint(RelB, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelC, Tgt), ecs_field_id(&it, 1));
    test_uint(RelC, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 1));
    test_uint(RelA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 1));
    test_uint(RelA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 1));
    test_uint(RelB, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelB, Tgt), ecs_field_id(&it, 1));
    test_uint(RelB, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 1));
    test_uint(RelA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelA, Tgt), ecs_field_id(&it, 1));
    test_uint(RelA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p3, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_pair(RelC, Tgt), ecs_field_id(&it, 1));
    test_uint(RelC, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_self_up_childof_pair_for_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ECS_ENTITY(world, ent, (Rel, TagA), (Rel, TagB), (Rel, TagC));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Rel(ent, $x), $x(self|up ChildOf)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t p1 = ecs_new_w(world, TagA);
    ecs_entity_t p2 = ecs_new_w(world, TagB);
    ecs_entity_t p3 = ecs_new_w(world, TagC);

    ecs_entity_t e1 = ecs_entity(world, { .parent = p1 });
    ecs_entity_t e2 = ecs_entity(world, { .parent = p2 });
    ecs_entity_t e3 = ecs_entity(world, { .parent = p3 });

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p1, it.entities[0]);
    test_uint(ent, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagA), ecs_field_id(&it, 0));
    test_uint(TagA, ecs_field_id(&it, 1));
    test_uint(TagA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ent, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagA), ecs_field_id(&it, 0));
    test_uint(TagA, ecs_field_id(&it, 1));
    test_uint(TagA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p2, it.entities[0]);
    test_uint(ent, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagB), ecs_field_id(&it, 0));
    test_uint(TagB, ecs_field_id(&it, 1));
    test_uint(TagB, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ent, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagB), ecs_field_id(&it, 0));
    test_uint(TagB, ecs_field_id(&it, 1));
    test_uint(TagB, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p3, it.entities[0]);
    test_uint(ent, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagC), ecs_field_id(&it, 0));
    test_uint(TagC, ecs_field_id(&it, 1));
    test_uint(TagC, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(ent, ecs_field_src(&it, 0));
    test_uint(p3, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagC), ecs_field_id(&it, 0));
    test_uint(TagC, ecs_field_id(&it, 1));
    test_uint(TagC, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_up_childof_pair_for_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ECS_ENTITY(world, ent, (Rel, TagA), (Rel, TagB), (Rel, TagC));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Rel(ent, $x), $x(up ChildOf)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t p1 = ecs_new_w(world, TagA);
    ecs_entity_t p2 = ecs_new_w(world, TagB);
    ecs_entity_t p3 = ecs_new_w(world, TagC);

    ecs_entity_t e1 = ecs_entity(world, { .parent = p1 });
    ecs_entity_t e2 = ecs_entity(world, { .parent = p2 });
    ecs_entity_t e3 = ecs_entity(world, { .parent = p3 });

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ent, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagA), ecs_field_id(&it, 0));
    test_uint(TagA, ecs_field_id(&it, 1));
    test_uint(TagA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ent, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagB), ecs_field_id(&it, 0));
    test_uint(TagB, ecs_field_id(&it, 1));
    test_uint(TagB, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(ent, ecs_field_src(&it, 0));
    test_uint(p3, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagC), ecs_field_id(&it, 0));
    test_uint(TagC, ecs_field_id(&it, 1));
    test_uint(TagC, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_written_self_up_childof_pair_for_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagC, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Rel(self, $x), $x(self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_new_w(world, TagA);
    ecs_entity_t p2 = ecs_new_w(world, TagB);
    ecs_entity_t p3 = ecs_new_w(world, TagC);

    ecs_entity_t e0 = ecs_entity(world, { .parent = p0 });
    ecs_add_pair(world, e0, Rel, TagA);
    ecs_entity_t e1 = ecs_entity(world, { .parent = p1 });
    ecs_add_pair(world, e1, Rel, TagA);
    ecs_entity_t e2 = ecs_entity(world, { .parent = p2 });
    ecs_add_pair(world, e2, Rel, TagB);
    ecs_entity_t e3 = ecs_entity(world, { .parent = p3 });
    ecs_add_pair(world, e3, Rel, TagC);

    ecs_entity_t e4 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, TagA) ) });
    ecs_add(world, e4, TagA);
    ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, TagB) ) });

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagA), ecs_field_id(&it, 0));
    test_uint(TagA, ecs_field_id(&it, 1));
    test_uint(TagA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagB), ecs_field_id(&it, 0));
    test_uint(TagB, ecs_field_id(&it, 1));
    test_uint(TagB, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p3, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagC), ecs_field_id(&it, 0));
    test_uint(TagC, ecs_field_id(&it, 1));
    test_uint(TagC, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagA), ecs_field_id(&it, 0));
    test_uint(TagA, ecs_field_id(&it, 1));
    test_uint(TagA, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_self_up_childof_pair_for_var_written_n_targets(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagC, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Contains, Traversable); // Non-exclusive relationship

    ECS_ENTITY(world, ent, (Rel, TagA), (Rel, TagB), (Rel, TagC));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Rel(ent, $x), $x(self|up Contains)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p0_a = ecs_new_w(world, TagA);
    ecs_entity_t p0_b = ecs_new_w(world, TagB);
    ecs_add(world, p0_b, Foo);
    ecs_entity_t p0_c = ecs_new_w(world, TagC);
    ecs_add(world, p0_c, Foo);
    ecs_add(world, p0_c, Bar);
    ecs_entity_t p1 = ecs_new(world);
    ecs_add_pair(world, p1, Contains, p0_a);
    ecs_add_pair(world, p1, Contains, p0_b);
    ecs_add_pair(world, p1, Contains, p0_c);

    ecs_entity(world, { .add = ecs_ids( ecs_pair(Contains, p0) ) });
    ecs_entity_t e1 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Contains, p1)) });
    ecs_entity_t e2 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, TagA) ) });
    ecs_add(world, e2, TagA);
    ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, TagB) ) });

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p0_a, it.entities[0]);
    test_uint(ent, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagA), ecs_field_id(&it, 0));
    test_uint(TagA, ecs_field_id(&it, 1));
    test_uint(TagA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p1, it.entities[0]);
    test_uint(ent, ecs_field_src(&it, 0));
    test_uint(p0_a, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagA), ecs_field_id(&it, 0));
    test_uint(TagA, ecs_field_id(&it, 1));
    test_uint(TagA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ent, ecs_field_src(&it, 0));
    test_uint(p0_a, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagA), ecs_field_id(&it, 0));
    test_uint(TagA, ecs_field_id(&it, 1));
    test_uint(TagA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ent, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagA), ecs_field_id(&it, 0));
    test_uint(TagA, ecs_field_id(&it, 1));
    test_uint(TagA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p0_b, it.entities[0]);
    test_uint(ent, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagB), ecs_field_id(&it, 0));
    test_uint(TagB, ecs_field_id(&it, 1));
    test_uint(TagB, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p1, it.entities[0]);
    test_uint(ent, ecs_field_src(&it, 0));
    test_uint(p0_b, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagB), ecs_field_id(&it, 0));
    test_uint(TagB, ecs_field_id(&it, 1));
    test_uint(TagB, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ent, ecs_field_src(&it, 0));
    test_uint(p0_b, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagB), ecs_field_id(&it, 0));
    test_uint(TagB, ecs_field_id(&it, 1));
    test_uint(TagB, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p0_c, it.entities[0]);
    test_uint(ent, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagC), ecs_field_id(&it, 0));
    test_uint(TagC, ecs_field_id(&it, 1));
    test_uint(TagC, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p1, it.entities[0]);
    test_uint(ent, ecs_field_src(&it, 0));
    test_uint(p0_c, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagC), ecs_field_id(&it, 0));
    test_uint(TagC, ecs_field_id(&it, 1));
    test_uint(TagC, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ent, ecs_field_src(&it, 0));
    test_uint(p0_c, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagC), ecs_field_id(&it, 0));
    test_uint(TagC, ecs_field_id(&it, 1));
    test_uint(TagC, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_written_self_up_childof_pair_for_var_written_n_targets(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagC, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Contains, Traversable); // Non-exclusive relationship

    ecs_query_t *r = ecs_query(world, {
        .expr = "Rel(self, $x), $x(self|up Contains)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_entity_t p0 = ecs_entity(world, {0});
    ecs_entity_t p1 = ecs_new_w(world, TagA);
    ecs_entity_t p2 = ecs_new_w(world, TagB);
    ecs_entity_t p3 = ecs_new_w(world, TagC);

    ecs_entity_t e0 = ecs_entity(world, { .add = ecs_ids(ecs_pair(Contains, p0)) });
    ecs_add_pair(world, e0, Rel, TagA);
    ecs_entity_t e1 = ecs_entity(world, { .add = ecs_ids(ecs_pair(Contains, p1)) });
    ecs_add_pair(world, e1, Contains, p2);
    ecs_add_pair(world, e1, Contains, p3);
    ecs_add_pair(world, e1, Rel, TagA);
    ecs_entity_t e2 = ecs_entity(world, { .add = ecs_ids(ecs_pair(Contains, p1)) });
    ecs_add_pair(world, e2, Contains, p2);
    ecs_add_pair(world, e2, Contains, p3);
    ecs_add_pair(world, e2, Rel, TagB);
    ecs_entity_t e3 = ecs_entity(world, { .add = ecs_ids(ecs_pair(Contains, p1)) });
    ecs_add_pair(world, e3, Contains, p2);
    ecs_add_pair(world, e3, Contains, p3);
    ecs_add_pair(world, e3, Rel, TagC);

    ecs_entity_t e4 = ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, TagA) ) });
    ecs_add(world, e4, TagA);
    ecs_entity(world, { .add = ecs_ids( ecs_pair(Rel, TagB) ) });

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p1, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagA), ecs_field_id(&it, 0));
    test_uint(TagA, ecs_field_id(&it, 1));
    test_uint(TagA, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagB), ecs_field_id(&it, 0));
    test_uint(TagB, ecs_field_id(&it, 1));
    test_uint(TagB, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p3, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagC), ecs_field_id(&it, 0));
    test_uint(TagC, ecs_field_id(&it, 1));
    test_uint(TagC, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagA), ecs_field_id(&it, 0));
    test_uint(TagA, ecs_field_id(&it, 1));
    test_uint(TagA, ecs_iter_get_var(&it, x_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_self_up_2_levels_w_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t base_2 = ecs_new_w_id(world, EcsPrefab);
    ecs_add_pair(world, base_2, EcsIsA, base);

    ecs_entity_t inst = ecs_entity(world, { .add = ecs_ids(ecs_isa(base_2)) });

    ecs_query_t *f = ecs_query(world, {
        .terms[0] = { .id = ecs_id(Position) },
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, f);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(inst, it.entities[0]);
    test_uint(base, it.sources[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    Position *p = ecs_field(&it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Traversal_self_up_2_levels_other_trav_rel_w_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t base_2 = ecs_new_w_id(world, EcsPrefab);
    ecs_add_pair(world, base_2, EcsIsA, base);

    ecs_entity(world, { .add = ecs_ids(ecs_isa(base_2)) });

    ecs_query_t *f = ecs_query(world, {
        .terms[0] = { .id = ecs_id(Position), .src.id = EcsSelf|EcsUp },
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, f);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Traversal_up_2_levels_w_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t base_2 = ecs_new_w_id(world, EcsPrefab);
    ecs_add_pair(world, base_2, EcsIsA, base);

    ecs_entity_t inst = ecs_entity(world, { .add = ecs_ids(ecs_isa(base_2)) });

    ecs_query_t *f = ecs_query(world, {
        .terms[0] = { .id = ecs_id(Position), .src.id = EcsUp, .trav = EcsIsA },
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, f);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(inst, it.entities[0]);
    test_uint(base, it.sources[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    Position *p = ecs_field(&it, Position, 0);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Traversal_up_2_levels_other_trav_rel_w_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t base_2 = ecs_new_w_id(world, EcsPrefab);
    ecs_add_pair(world, base_2, EcsIsA, base);

    ecs_entity(world, { .add = ecs_ids(ecs_isa(base_2)) });

    ecs_query_t *f = ecs_query(world, {
        .terms[0] = { .id = ecs_id(Position), .src.id = EcsUp },
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, f);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Traversal_self_up_2_levels(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new(world);
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t base_2 = ecs_new(world);
    ecs_add_pair(world, base_2, EcsIsA, base);

    ecs_entity_t inst = ecs_entity(world, { .add = ecs_ids(ecs_isa(base_2)) });

    ecs_query_t *f = ecs_query(world, {
        .terms[0] = { .id = ecs_id(Position) },
        .cache_kind = cache_kind
    });
    
    test_assert(f->terms[0].trav == EcsIsA);

    ecs_iter_t it = ecs_query_iter(world, f);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(base, it.entities[0]);
    test_uint(0, it.sources[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(base_2, it.entities[0]);
    test_uint(base, it.sources[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(inst, it.entities[0]);
    test_uint(base, it.sources[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Traversal_self_up_2_levels_other_trav_rel(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new(world);
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t base_2 = ecs_new(world);
    ecs_add_pair(world, base_2, EcsIsA, base);

    ecs_entity(world, { .add = ecs_ids(ecs_isa(base_2)) });

    ecs_query_t *f = ecs_query(world, {
        .terms[0] = { .id = ecs_id(Position), .src.id = EcsSelf|EcsUp },
        .cache_kind = cache_kind
    });

    test_assert(f->terms[0].trav == EcsChildOf);

    ecs_iter_t it = ecs_query_iter(world, f);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(base, it.entities[0]);
    test_uint(0, it.sources[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it)); 

    ecs_query_fini(f);

    ecs_fini(world);
}

void Traversal_up_2_levels(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new(world);
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t base_2 = ecs_new(world);
    ecs_add_pair(world, base_2, EcsIsA, base);

    ecs_entity_t inst = ecs_entity(world, { .add = ecs_ids(ecs_isa(base_2)) });

    ecs_query_t *f = ecs_query(world, {
        .terms[0] = { .id = ecs_id(Position), .src.id = EcsUp, .trav = EcsIsA },
        .cache_kind = cache_kind
    });

    test_assert(f->terms[0].trav == EcsIsA);

    ecs_iter_t it = ecs_query_iter(world, f);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(base_2, it.entities[0]);
    test_uint(base, it.sources[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(inst, it.entities[0]);
    test_uint(base, it.sources[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    test_bool(false, ecs_query_next(&it)); 

    ecs_query_fini(f);

    ecs_fini(world);
}

void Traversal_up_2_levels_other_trav_rel(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new(world);
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t base_2 = ecs_new(world);
    ecs_add_pair(world, base_2, EcsIsA, base);

    ecs_entity(world, { .add = ecs_ids(ecs_isa(base_2)) });

    ecs_query_t *f = ecs_query(world, {
        .terms[0] = { .id = ecs_id(Position), .src.id = EcsUp },
        .cache_kind = cache_kind
    });

    test_assert(f->terms[0].trav == EcsChildOf);

    ecs_iter_t it = ecs_query_iter(world, f);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Traversal_self_up_mixed_traversable(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));

    ecs_entity_t p1 = ecs_new(world);
    ecs_entity_t p2 = ecs_new(world);
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, p2);

    ecs_add(world, p1, TagA);
    ecs_add(world, p2, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA(self|up ChildOf)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(p1, it.entities[0]);
    test_uint(p2, it.entities[1]);
    test_uint(0, ecs_field_src(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(c, it.entities[0]);
    test_uint(p2, ecs_field_src(&it, 0));
    
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_not_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));

    ecs_entity_t p1 = ecs_new_w(world, TagB);
    ecs_entity_t p2 = ecs_new(world);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_entity_t e4 = ecs_new_w(world, TagA);
    
    ecs_add_pair(world, e1, EcsChildOf, p1);
    ecs_add_pair(world, e2, EcsChildOf, p2);
    ecs_add(world, e4, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, !TagB(up)",
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_not_self_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));

    ecs_entity_t p1 = ecs_new_w(world, TagB);
    ecs_entity_t p2 = ecs_new(world);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_entity_t e4 = ecs_new_w(world, TagA);
    
    ecs_add_pair(world, e1, EcsChildOf, p1);
    ecs_add_pair(world, e2, EcsChildOf, p2);
    ecs_add(world, e4, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, !TagB(self|up)",
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_not_up_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));
    ECS_TAG(world, TgtA);

    ecs_entity_t p1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t p2 = ecs_new(world);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_entity_t e4 = ecs_new_w(world, TagA);
    
    ecs_add_pair(world, e1, EcsChildOf, p1);
    ecs_add_pair(world, e2, EcsChildOf, p2);
    ecs_add_pair(world, e4, Rel, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, !Rel(up, *)",
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_not_self_up_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));
    ECS_TAG(world, TgtA);

    ecs_entity_t p1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t p2 = ecs_new(world);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_entity_t e4 = ecs_new_w(world, TagA);
    
    ecs_add_pair(world, e1, EcsChildOf, p1);
    ecs_add_pair(world, e2, EcsChildOf, p2);
    ecs_add_pair(world, e4, Rel, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, !Rel(self|up, *)",
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_not_up_disabled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));

    ecs_query_t *f = ecs_query(world, {
        .terms = {
            { TagA },
            { .id = EcsDisabled, .src.id = EcsUp, .oper = EcsNot }
        },
        .cache_kind = cache_kind
    });
    test_assert(f != NULL);

    ecs_entity_t parent = ecs_new_w_id(world, EcsDisabled);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_add_id(world, e2, EcsDisabled);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e3, EcsChildOf, parent);
    
    ecs_iter_t it = ecs_query_iter(world, f);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(TagA, it.ids[0]);
    test_uint(0, it.sources[0]);

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(f);
    
    ecs_fini(world);
}

void Traversal_up_2_rel_instances(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagC, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));

    ecs_add_id(world, Rel, EcsTraversable);

    ecs_entity_t b1 = ecs_new_w(world, TagA);
    ecs_entity_t b2 = ecs_new_w(world, TagA);
    ecs_add(world, b1, TagB);
    ecs_add(world, b2, TagB);
    ecs_add(world, b1, TagC);
    ecs_add(world, b2, TagC);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Rel, b1);
    ecs_add_pair(world, e, Rel, b2);

    ecs_query_t *f = ecs_query(world, {
        .terms = {
            {TagC, .src.id = EcsUp, .trav = Rel }
        },
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, f);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e);
    test_uint(it.sources[0], b1);
    test_uint(it.ids[0], TagC);

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Traversal_up_2_rel_instances_match_2nd(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagC, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));

    ecs_add_id(world, Rel, EcsTraversable);

    ecs_entity_t b1 = ecs_new_w(world, TagA);
    ecs_entity_t b2 = ecs_new_w(world, TagA);
    ecs_add(world, b1, TagB);
    ecs_add(world, b2, TagB);
    ecs_add(world, b2, TagC);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Rel, b1);
    ecs_add_pair(world, e, Rel, b2);

    ecs_query_t *f = ecs_query(world, {
        .terms = {
            { TagC, .src.id = EcsUp, .trav = Rel }
        },
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, f);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e);
    test_uint(it.sources[0], b2);
    test_uint(it.ids[0], TagC);

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Traversal_up_only_w_owned(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, R, Traversable);
    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t e_0 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e_1 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t e_2 = ecs_insert(world, ecs_value(Position, {50, 60}));
    ecs_entity_t e_3 = ecs_insert(world, ecs_value(Position, {70, 80}));

    ecs_add_pair(world, e_3, R, e_2);
    ecs_add_pair(world, e_2, R, e_1);
    ecs_add_pair(world, e_1, R, e_0);

    ecs_query_t *f = ecs_query(world, {
        .expr = "Position(up R)",
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, f);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e_1);
    test_uint(it.sources[0], e_0);
    Position *p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e_3);
    test_uint(it.sources[0], e_2);
    p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 50);
    test_int(p[0].y, 60);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e_2);
    test_uint(it.sources[0], e_1);
    p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Traversal_this_up_trav_unused_rel(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Rel, Traversable);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(up Rel)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity(world, { .add = ecs_ids(Foo) });
    
    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_this_optional_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new_w(world, Velocity);
    
    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_add_pair(world, e1, EcsIsA, base);

    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_add(world, e2, Velocity);

    ecs_entity_t e3 = ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "?Velocity(self), Position",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    int32_t count = 0;

    while (ecs_query_next(&it)) {
        Velocity *v = ecs_field(&it, Velocity, 0);
        Position *p = ecs_field(&it, Position, 1);
        
        test_int(it.count, 1);
        test_assert(p != NULL);

        if (it.entities[0] == e1) {
            test_bool(ecs_field_is_set(&it, 0), false); 
            test_bool(ecs_field_is_set(&it, 1), true);
        } else if (it.entities[0] == e2) {
            test_bool(ecs_field_is_set(&it, 0), true); 
            test_bool(ecs_field_is_set(&it, 1), true);
            test_assert(v != NULL);
        } else if (it.entities[0] == e3) {
            test_bool(ecs_field_is_set(&it, 0), false);
            test_bool(ecs_field_is_set(&it, 1), true);
        }

        count ++;
    }

    test_int(count, 3);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_this_optional_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new_w(world, Velocity);
    
    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_add_pair(world, e1, EcsIsA, base);

    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_add(world, e2, Velocity);

    ecs_entity_t e3 = ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "?Velocity(up IsA), Position",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    int32_t count = 0;
    
    while (ecs_query_next(&it)) {
        Velocity *v = ecs_field(&it, Velocity, 0);
        Position *p = ecs_field(&it, Position, 1);
        
        test_int(it.count, 1);
        test_assert(p != NULL);
        
        if (it.entities[0] == e1) {
            test_bool(ecs_field_is_set(&it, 0), true);
            test_bool(ecs_field_is_set(&it, 1), true);
            test_assert(v != NULL);
        } else if (it.entities[0] == e2) {
            test_bool(ecs_field_is_set(&it, 0), false);            
            test_bool(ecs_field_is_set(&it, 1), true);
        } else if (it.entities[0] == e3) {
            test_bool(ecs_field_is_set(&it, 0), false); 
            test_bool(ecs_field_is_set(&it, 1), true);
        }

        count ++;
    }

    test_int(count, 3);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_this_optional_self_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new_w(world, Velocity);
    
    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_add_pair(world, e1, EcsIsA, base);

    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_add(world, e2, Velocity);

    ecs_entity_t e3 = ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "?Velocity(self|up IsA), Position",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    int32_t count = 0;
    
    while (ecs_query_next(&it)) {
        Velocity *v = ecs_field(&it, Velocity, 0);
        Position *p = ecs_field(&it, Position, 1);
        
        test_int(it.count, 1);
        test_assert(p != NULL);
        
        if (it.entities[0] == e1) {
            test_bool(ecs_field_is_set(&it, 0), true);
            test_bool(ecs_field_is_set(&it, 1), true);
            test_assert(v != NULL);
        } else if (it.entities[0] == e2) {
            test_bool(ecs_field_is_set(&it, 0), true);
            test_bool(ecs_field_is_set(&it, 1), true);            
            test_assert(v != NULL);
        } else if (it.entities[0] == e3) {
            test_bool(ecs_field_is_set(&it, 0), false); 
            test_bool(ecs_field_is_set(&it, 1), true);
        }

        count ++;
    }

    test_int(count, 3);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_this_written_optional_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new_w(world, Velocity);
    
    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_add_pair(world, e1, EcsIsA, base);

    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_add(world, e2, Velocity);

    ecs_entity_t e3 = ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, ?Velocity(self)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    int32_t count = 0;

    while (ecs_query_next(&it)) {
        Position *p = ecs_field(&it, Position, 0);
        Velocity *v = ecs_field(&it, Velocity, 1);
        
        test_int(it.count, 1);
        test_assert(p != NULL);

        if (it.entities[0] == e1) {
            test_bool(ecs_field_is_set(&it, 0), true);
            test_bool(ecs_field_is_set(&it, 1), false); 
            test_assert(v == NULL);
        } else if (it.entities[0] == e2) {
            test_bool(ecs_field_is_set(&it, 0), true);
            test_bool(ecs_field_is_set(&it, 1), true); 
            test_assert(v != NULL);
        } else if (it.entities[0] == e3) {
            test_bool(ecs_field_is_set(&it, 0), true);
            test_bool(ecs_field_is_set(&it, 1), false);
            test_assert(v == NULL);
        }

        count ++;
    }

    test_int(count, 3);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_this_written_optional_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new_w(world, Velocity);
    
    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_add_pair(world, e1, EcsIsA, base);

    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_add(world, e2, Velocity);

    ecs_entity_t e3 = ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, ?Velocity(up IsA)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    int32_t count = 0;
    
    while (ecs_query_next(&it)) {
        Position *p = ecs_field(&it, Position, 0);
        Velocity *v = ecs_field(&it, Velocity, 1);
        
        test_int(it.count, 1);
        test_assert(p != NULL);
        
        if (it.entities[0] == e1) {
            test_bool(ecs_field_is_set(&it, 0), true);
            test_bool(ecs_field_is_set(&it, 1), true);
            test_assert(v != NULL);
        } else if (it.entities[0] == e2) {
            test_bool(ecs_field_is_set(&it, 0), true);
            test_bool(ecs_field_is_set(&it, 1), false);            
        } else if (it.entities[0] == e3) {
            test_bool(ecs_field_is_set(&it, 0), true);
            test_bool(ecs_field_is_set(&it, 1), false); 
        }

        count ++;
    }

    test_int(count, 3);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_this_written_optional_self_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new_w(world, Velocity);
    
    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_add_pair(world, e1, EcsIsA, base);

    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_add(world, e2, Velocity);

    ecs_entity_t e3 = ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, ?Velocity(self|up IsA)",
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    int32_t count = 0;
    
    while (ecs_query_next(&it)) {
        Position *p = ecs_field(&it, Position, 0);
        Velocity *v = ecs_field(&it, Velocity, 1);
        
        test_int(it.count, 1);
        test_assert(p != NULL);
        
        if (it.entities[0] == e1) {
            test_bool(ecs_field_is_set(&it, 0), true);
            test_bool(ecs_field_is_set(&it, 1), true);
            test_assert(v != NULL);
        } else if (it.entities[0] == e2) {
            test_bool(ecs_field_is_set(&it, 0), true);
            test_bool(ecs_field_is_set(&it, 1), true);            
            test_assert(v != NULL);
        } else if (it.entities[0] == e3) {
            test_bool(ecs_field_is_set(&it, 0), true);
            test_bool(ecs_field_is_set(&it, 1), false); 
        }

        count ++;
    }

    test_int(count, 3);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_fixed_src_w_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);

    ecs_entity_t Game = ecs_entity(world, { .name = "Game" });
    ecs_set(world, Game, Position, {10, 20});
    ecs_set(world, Game, Velocity, {1, 2});

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_set(world, p, Mass, {30});
    ecs_entity_t e = ecs_entity(world, { .add = ecs_ids(ecs_isa(p)) });

    ecs_query_t *q = ecs_query(world, {
        .expr = "Velocity(Game), Mass(up IsA)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 0));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 1));
    test_uint(Game, ecs_field_src(&it, 0));
    test_uint(p, ecs_field_src(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_match_empty_table_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(up)",
        .cache_kind = cache_kind,
        .flags =  EcsQueryMatchEmptyTables
    });

    test_assert(q != NULL);

    ecs_entity_t p = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_entity(world, { .parent = p });
    ecs_table_t *t1 = ecs_get_table(world, e1);

    ecs_add(world, e1, TagA); // (ChildOf, p) is empty
    ecs_table_t *t2 = ecs_get_table(world, e1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_assert(it.table == t1);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(p, ecs_field_src(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_assert(it.table == t2);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(p, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_match_empty_table_up_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagB, Position(up)",
        .cache_kind = cache_kind,
        .flags =  EcsQueryMatchEmptyTables
    });

    test_assert(q != NULL);

    ecs_entity_t p = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_entity(world, { .parent = p });
    ecs_add(world, e1, TagB);
    ecs_table_t *t1 = ecs_get_table(world, e1);

    ecs_add(world, e1, TagA); // TagB, (ChildOf, p) is empty
    ecs_table_t *t2 = ecs_get_table(world, e1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_assert(it.table == t1);
    test_uint(TagB, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p, ecs_field_src(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_assert(it.table == t2);
    test_uint(e1, it.entities[0]);
    test_uint(TagB, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_match_empty_table_up_implicit_isa(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(up)",
        .cache_kind = cache_kind,
        .flags =  EcsQueryMatchEmptyTables
    });

    test_assert(q != NULL);

    ecs_entity_t b = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t p = ecs_entity(world, { .add = ecs_ids(ecs_isa(b)) });
    ecs_entity_t e1 = ecs_entity(world, { .parent = p });
    ecs_table_t *t1 = ecs_get_table(world, e1);

    ecs_add(world, e1, TagA); // (ChildOf, p) is empty
    ecs_table_t *t2 = ecs_get_table(world, e1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_assert(it.table == t1);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(b, ecs_field_src(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_assert(it.table == t2);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(b, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_match_empty_table_up_written_implicit_isa(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagB, Position(up)",
        .cache_kind = cache_kind,
        .flags =  EcsQueryMatchEmptyTables
    });

    test_assert(q != NULL);

    ecs_entity_t b = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t p = ecs_entity(world, { .add = ecs_ids(ecs_isa(b)) });
    ecs_entity_t e1 = ecs_entity(world, { .parent = p });
    ecs_add(world, e1, TagB);
    ecs_table_t *t1 = ecs_get_table(world, e1);

    ecs_add(world, e1, TagA); // TagB, (ChildOf, p) is empty
    ecs_table_t *t2 = ecs_get_table(world, e1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_assert(it.table == t1);
    test_uint(TagB, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(b, ecs_field_src(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_assert(it.table == t2);
    test_uint(e1, it.entities[0]);
    test_uint(TagB, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(b, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_match_empty_table_up_isa(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(up IsA)",
        .cache_kind = cache_kind,
        .flags =  EcsQueryMatchEmptyTables
    });

    test_assert(q != NULL);

    ecs_entity_t p = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_entity(world, { .add = ecs_ids(ecs_isa(p)) });
    ecs_table_t *t1 = ecs_get_table(world, e1);

    ecs_add(world, e1, TagA); // (ChildOf, p) is empty
    ecs_table_t *t2 = ecs_get_table(world, e1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_assert(it.table == t1);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(p, ecs_field_src(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_assert(it.table == t2);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(p, ecs_field_src(&it, 0));
    {
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_match_empty_table_up_written_isa(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagB, Position(up IsA)",
        .cache_kind = cache_kind,
        .flags =  EcsQueryMatchEmptyTables
    });

    test_assert(q != NULL);

    ecs_entity_t p = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e1 = ecs_entity(world, { .add = ecs_ids(ecs_isa(p)) });
    ecs_add(world, e1, TagB);
    ecs_table_t *t1 = ecs_get_table(world, e1);

    ecs_add(world, e1, TagA); // TagB, (ChildOf, p) is empty
    ecs_table_t *t2 = ecs_get_table(world, e1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_assert(it.table == t1);
    test_uint(TagB, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p, ecs_field_src(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_assert(it.table == t2);
    test_uint(e1, it.entities[0]);
    test_uint(TagB, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(p, ecs_field_src(&it, 1));
    {
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_up_after_add_batched_to_parent(void) {
	ecs_world_t *world = ecs_init();

	ECS_TAG(world, Foo);
	ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = Foo, .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

	ecs_entity_t e1 = ecs_new(world);
	ecs_entity_t e2 = ecs_new(world);
	ecs_add_pair(world, e2, EcsChildOf, e1);

    ecs_defer_begin(world);
    ecs_add(world, e1, Foo);
    ecs_add(world, e1, Bar);
    ecs_defer_end(world);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(e1, ecs_field_src(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_up_component_after_parent_table_change(void) {
	ecs_world_t *world = ecs_init();

	ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

	ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});
	ecs_entity_t e2 = ecs_new(world);
	ecs_add_pair(world, e2, EcsChildOf, e1);
    ecs_set(world, e2, Position, {30, 40});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p1 = ecs_field(&it, Position, 0);
            Position *p2 = ecs_field(&it, Position, 1);
            test_assert(p1 != NULL);
            test_assert(p2 != NULL);
            test_int(p1->x, 30); test_int(p1->y, 40);
            test_int(p2->x, 10); test_int(p2->y, 20);
        }
        test_assert(it.trs[0]->hdr.table == ecs_get_table(world, e2));
        test_assert(it.trs[1]->hdr.table == ecs_get_table(world, e1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p1 = ecs_field(&it, Position, 0);
            Position *p2 = ecs_field(&it, Position, 1);
            test_assert(p1 != NULL);
            test_assert(p2 != NULL);
            test_int(p1->x, 30); test_int(p1->y, 40);
            test_int(p2->x, 10); test_int(p2->y, 20);
        }
        test_assert(it.trs[0]->hdr.table == ecs_get_table(world, e2));
        test_assert(it.trs[1]->hdr.table == ecs_get_table(world, e1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_up_component_w_singleton_after_parent_table_change(void) {
	ecs_world_t *world = ecs_init();

	ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .src.id = EcsUp },
            { .id = ecs_id(Velocity), .src.id = EcsSingleton }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_singleton_set(world, Velocity, {1, 2});

	ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});
	ecs_entity_t e2 = ecs_new(world);
	ecs_add_pair(world, e2, EcsChildOf, e1);
    ecs_set(world, e2, Position, {30, 40});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        {
            Position *p1 = ecs_field(&it, Position, 0);
            Position *p2 = ecs_field(&it, Position, 1);
            Velocity *v = ecs_field(&it, Velocity, 2);
            test_assert(p1 != NULL);
            test_assert(p2 != NULL);
            test_assert(v != NULL);
            test_int(p1->x, 30); test_int(p1->y, 40);
            test_int(p2->x, 10); test_int(p2->y, 20);
            test_int(v->x, 1); test_int(v->y, 2);
        }
        test_assert(it.trs[0]->hdr.table == ecs_get_table(world, e2));
        test_assert(it.trs[1]->hdr.table == ecs_get_table(world, e1));
        test_assert(it.trs[2]->hdr.table == ecs_get_table(world, ecs_id(Velocity)));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        {
            Position *p1 = ecs_field(&it, Position, 0);
            Position *p2 = ecs_field(&it, Position, 1);
            Velocity *v = ecs_field(&it, Velocity, 2);
            test_assert(p1 != NULL);
            test_assert(p2 != NULL);
            test_assert(v != NULL);
            test_int(p1->x, 30); test_int(p1->y, 40);
            test_int(p2->x, 10); test_int(p2->y, 20);
            test_int(v->x, 1); test_int(v->y, 2);
        }
        test_assert(it.trs[0]->hdr.table == ecs_get_table(world, e2));
        test_assert(it.trs[1]->hdr.table == ecs_get_table(world, e1));
        test_assert(it.trs[2]->hdr.table == ecs_get_table(world, ecs_id(Velocity)));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_up_component_w_var_after_parent_table_change(void) {
	ecs_world_t *world = ecs_init();

	ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, Position(up), (Velocity, $this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

	ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});

	ecs_entity_t e2 = ecs_new(world);
	ecs_add_pair(world, e2, EcsChildOf, e1);
    ecs_set(world, e2, Position, {30, 40});
    ecs_set_pair(world, e2, Velocity, e2, {1, 2});

	ecs_entity_t e3 = ecs_new(world);
	ecs_add_pair(world, e3, EcsChildOf, e1);
    ecs_set(world, e3, Position, {50, 60});
    ecs_set_pair(world, e3, Velocity, e3, {3, 4});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(ecs_pair_t(Velocity, e2), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        {
            Position *p1 = ecs_field(&it, Position, 0);
            Position *p2 = ecs_field(&it, Position, 1);
            Velocity *v = ecs_field(&it, Velocity, 2);
            test_assert(p1 != NULL);
            test_assert(p2 != NULL);
            test_assert(v != NULL);
            test_int(p1->x, 30); test_int(p1->y, 40);
            test_int(p2->x, 10); test_int(p2->y, 20);
            test_int(v->x, 1); test_int(v->y, 2);
        }
        test_assert(it.trs[0]->hdr.table == ecs_get_table(world, e2));
        test_assert(it.trs[1]->hdr.table == ecs_get_table(world, e1));
        test_assert(it.trs[2]->hdr.table == ecs_get_table(world, e2));

        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(ecs_pair_t(Velocity, e3), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        {
            Position *p1 = ecs_field(&it, Position, 0);
            Position *p2 = ecs_field(&it, Position, 1);
            Velocity *v = ecs_field(&it, Velocity, 2);
            test_assert(p1 != NULL);
            test_assert(p2 != NULL);
            test_assert(v != NULL);
            test_int(p1->x, 50); test_int(p1->y, 60);
            test_int(p2->x, 10); test_int(p2->y, 20);
            test_int(v->x, 3); test_int(v->y, 4);
        }

        test_assert(it.trs[0]->hdr.table == ecs_get_table(world, e3));
        test_assert(it.trs[1]->hdr.table == ecs_get_table(world, e1));
        test_assert(it.trs[2]->hdr.table == ecs_get_table(world, e3));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(ecs_pair_t(Velocity, e2), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        {
            Position *p1 = ecs_field(&it, Position, 0);
            Position *p2 = ecs_field(&it, Position, 1);
            Velocity *v = ecs_field(&it, Velocity, 2);
            test_assert(p1 != NULL);
            test_assert(p2 != NULL);
            test_assert(v != NULL);
            test_int(p1->x, 30); test_int(p1->y, 40);
            test_int(p2->x, 10); test_int(p2->y, 20);
            test_int(v->x, 1); test_int(v->y, 2);
        }
        test_assert(it.trs[0]->hdr.table == ecs_get_table(world, e2));
        test_assert(it.trs[1]->hdr.table == ecs_get_table(world, e1));
        test_assert(it.trs[2]->hdr.table == ecs_get_table(world, e2));

        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(ecs_pair_t(Velocity, e3), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        {
            Position *p1 = ecs_field(&it, Position, 0);
            Position *p2 = ecs_field(&it, Position, 1);
            Velocity *v = ecs_field(&it, Velocity, 2);
            test_assert(p1 != NULL);
            test_assert(p2 != NULL);
            test_assert(v != NULL);
            test_int(p1->x, 50); test_int(p1->y, 60);
            test_int(p2->x, 10); test_int(p2->y, 20);
            test_int(v->x, 3); test_int(v->y, 4);
        }

        test_assert(it.trs[0]->hdr.table == ecs_get_table(world, e3));
        test_assert(it.trs[1]->hdr.table == ecs_get_table(world, e1));
        test_assert(it.trs[2]->hdr.table == ecs_get_table(world, e3));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_test_up_component_after_parent_table_change(void) {
	ecs_world_t *world = ecs_init();

	ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .src.id = EcsUp }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

	ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});
	ecs_entity_t e2 = ecs_new(world);
	ecs_add_pair(world, e2, EcsChildOf, e1);
    ecs_set(world, e2, Position, {30, 40});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p1 = ecs_field(&it, Position, 0);
            Position *p2 = ecs_field(&it, Position, 1);
            test_assert(p1 != NULL);
            test_assert(p2 != NULL);
            test_int(p1->x, 30); test_int(p1->y, 40);
            test_int(p2->x, 10); test_int(p2->y, 20);
        }

        test_assert(it.trs[0]->hdr.table == ecs_get_table(world, e2));
        test_assert(it.trs[1]->hdr.table == ecs_get_table(world, e1));
        test_bool(false, ecs_query_next(&it));
    }
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        {
            Position *p1 = ecs_field(&it, Position, 0);
            Position *p2 = ecs_field(&it, Position, 1);
            test_assert(p1 != NULL);
            test_assert(p2 != NULL);
            test_int(p1->x, 30); test_int(p1->y, 40);
            test_int(p2->x, 10); test_int(p2->y, 20);
        }

        test_assert(it.trs[0]->hdr.table == ecs_get_table(world, e2));
        test_assert(it.trs[1]->hdr.table == ecs_get_table(world, e1));
        test_bool(false, ecs_query_next(&it));
    }
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_test_up_component_w_singleton_after_parent_table_change(void) {
	ecs_world_t *world = ecs_init();

	ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .src.id = EcsUp },
            { .id = ecs_id(Velocity), .src.id = EcsSingleton }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_singleton_set(world, Velocity, {1, 2});

	ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});
	ecs_entity_t e2 = ecs_new(world);
	ecs_add_pair(world, e2, EcsChildOf, e1);
    ecs_set(world, e2, Position, {30, 40});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        {
            Position *p1 = ecs_field(&it, Position, 0);
            Position *p2 = ecs_field(&it, Position, 1);
            Velocity *v = ecs_field(&it, Velocity, 2);
            test_assert(p1 != NULL);
            test_assert(p2 != NULL);
            test_assert(v != NULL);
            test_int(p1->x, 30); test_int(p1->y, 40);
            test_int(p2->x, 10); test_int(p2->y, 20);
            test_int(v->x, 1); test_int(v->y, 2);
        }

        test_assert(it.trs[0]->hdr.table == ecs_get_table(world, e2));
        test_assert(it.trs[1]->hdr.table == ecs_get_table(world, e1));
        test_assert(it.trs[2]->hdr.table == ecs_get_table(world, ecs_id(Velocity)));
        test_bool(false, ecs_query_next(&it));
    }
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        {
            Position *p1 = ecs_field(&it, Position, 0);
            Position *p2 = ecs_field(&it, Position, 1);
            Velocity *v = ecs_field(&it, Velocity, 2);
            test_assert(p1 != NULL);
            test_assert(p2 != NULL);
            test_assert(v != NULL);
            test_int(p1->x, 30); test_int(p1->y, 40);
            test_int(p2->x, 10); test_int(p2->y, 20);
            test_int(v->x, 1); test_int(v->y, 2);
        }

        test_assert(it.trs[0]->hdr.table == ecs_get_table(world, e2));
        test_assert(it.trs[1]->hdr.table == ecs_get_table(world, e1));
        test_assert(it.trs[2]->hdr.table == ecs_get_table(world, ecs_id(Velocity)));
        test_bool(false, ecs_query_next(&it));
    }
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_up_component_after_parent_table_change_no_data(void) {
	ecs_world_t *world = ecs_init();

	ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), .inout = EcsInOutNone },
            { .id = ecs_id(Position), .src.id = EcsUp, .inout = EcsInOutNone }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

	ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});
	ecs_entity_t e2 = ecs_new(world);
	ecs_add_pair(world, e2, EcsChildOf, e1);
    ecs_set(world, e2, Position, {30, 40});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_assert(it.trs[0]->hdr.table == ecs_get_table(world, e2));
        test_assert(it.trs[1]->hdr.table == ecs_get_table(world, e1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_assert(it.trs[0]->hdr.table == ecs_get_table(world, e2));
        test_assert(it.trs[1]->hdr.table == ecs_get_table(world, e1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_up_component_w_singleton_after_parent_table_change_no_data(void) {
	ecs_world_t *world = ecs_init();

	ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), .inout = EcsInOutNone },
            { .id = ecs_id(Position), .src.id = EcsUp, .inout = EcsInOutNone },
            { .id = ecs_id(Velocity), .src.id = EcsSingleton, .inout = EcsInOutNone }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_singleton_set(world, Velocity, {1, 2});

	ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});
	ecs_entity_t e2 = ecs_new(world);
	ecs_add_pair(world, e2, EcsChildOf, e1);
    ecs_set(world, e2, Position, {30, 40});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_assert(it.trs[0]->hdr.table == ecs_get_table(world, e2));
        test_assert(it.trs[1]->hdr.table == ecs_get_table(world, e1));
        test_assert(it.trs[2]->hdr.table == ecs_get_table(world, ecs_id(Velocity)));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_assert(it.trs[0]->hdr.table == ecs_get_table(world, e2));
        test_assert(it.trs[1]->hdr.table == ecs_get_table(world, e1));
        test_assert(it.trs[2]->hdr.table == ecs_get_table(world, ecs_id(Velocity)));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_up_component_w_var_after_parent_table_change_no_data(void) {
	ecs_world_t *world = ecs_init();

	ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "[none] Position, [none] Position(up), [none] (Velocity, $this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

	ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});

	ecs_entity_t e2 = ecs_new(world);
	ecs_add_pair(world, e2, EcsChildOf, e1);
    ecs_set(world, e2, Position, {30, 40});
    ecs_set_pair(world, e2, Velocity, e2, {1, 2});

	ecs_entity_t e3 = ecs_new(world);
	ecs_add_pair(world, e3, EcsChildOf, e1);
    ecs_set(world, e3, Position, {50, 60});
    ecs_set_pair(world, e3, Velocity, e3, {3, 4});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(ecs_pair_t(Velocity, e2), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_assert(it.trs[0]->hdr.table == ecs_get_table(world, e2));
        test_assert(it.trs[1]->hdr.table == ecs_get_table(world, e1));
        test_assert(it.trs[2]->hdr.table == ecs_get_table(world, e2));

        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(ecs_pair_t(Velocity, e3), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));

        test_assert(it.trs[0]->hdr.table == ecs_get_table(world, e3));
        test_assert(it.trs[1]->hdr.table == ecs_get_table(world, e1));
        test_assert(it.trs[2]->hdr.table == ecs_get_table(world, e3));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(ecs_pair_t(Velocity, e2), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_assert(it.trs[0]->hdr.table == ecs_get_table(world, e2));
        test_assert(it.trs[1]->hdr.table == ecs_get_table(world, e1));
        test_assert(it.trs[2]->hdr.table == ecs_get_table(world, e2));

        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e3);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(ecs_pair_t(Velocity, e3), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));

        test_assert(it.trs[0]->hdr.table == ecs_get_table(world, e3));
        test_assert(it.trs[1]->hdr.table == ecs_get_table(world, e1));
        test_assert(it.trs[2]->hdr.table == ecs_get_table(world, e3));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_test_up_component_after_parent_table_change_no_data(void) {
	ecs_world_t *world = ecs_init();

	ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), .inout = EcsInOutNone },
            { .id = ecs_id(Position), .src.id = EcsUp, .inout = EcsInOutNone }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

	ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});
	ecs_entity_t e2 = ecs_new(world);
	ecs_add_pair(world, e2, EcsChildOf, e1);
    ecs_set(world, e2, Position, {30, 40});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_assert(it.trs[0]->hdr.table == ecs_get_table(world, e2));
        test_assert(it.trs[1]->hdr.table == ecs_get_table(world, e1));
        test_bool(false, ecs_query_next(&it));
    }
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_assert(it.trs[0]->hdr.table == ecs_get_table(world, e2));
        test_assert(it.trs[1]->hdr.table == ecs_get_table(world, e1));
        test_bool(false, ecs_query_next(&it));
    }
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Traversal_test_up_component_w_singleton_after_parent_table_change_no_data(void) {
	ecs_world_t *world = ecs_init();

	ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), .inout = EcsInOutNone },
            { .id = ecs_id(Position), .src.id = EcsUp, .inout = EcsInOutNone },
            { .id = ecs_id(Velocity), .src.id = EcsSingleton, .inout = EcsInOutNone }
        },
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_singleton_set(world, Velocity, {1, 2});

	ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {10, 20});
	ecs_entity_t e2 = ecs_new(world);
	ecs_add_pair(world, e2, EcsChildOf, e1);
    ecs_set(world, e2, Position, {30, 40});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_assert(it.trs[0]->hdr.table == ecs_get_table(world, e2));
        test_assert(it.trs[1]->hdr.table == ecs_get_table(world, e1));
        test_assert(it.trs[2]->hdr.table == ecs_get_table(world, ecs_id(Velocity)));
        test_bool(false, ecs_query_next(&it));
    }
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e2);
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e2);
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_assert(it.trs[0]->hdr.table == ecs_get_table(world, e2));
        test_assert(it.trs[1]->hdr.table == ecs_get_table(world, e1));
        test_assert(it.trs[2]->hdr.table == ecs_get_table(world, ecs_id(Velocity)));
        test_bool(false, ecs_query_next(&it));
    }
    {
        ecs_iter_t it = ecs_query_iter(world, q);
        ecs_iter_set_var(&it, 0, e1);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}
