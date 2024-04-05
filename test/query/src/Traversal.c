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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new(world, Foo);
    ecs_entity_t p2 = ecs_new(world, Foo);
    ecs_entity_t p3 = ecs_new(world, Foo);
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);

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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new(world, Foo);
    ecs_entity_t p2 = ecs_new(world, Foo);
    ecs_entity_t p3 = ecs_new(world, Foo);
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);

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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tag);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new(world, Foo);
    ecs_entity_t p2 = ecs_new(world, Foo);
    ecs_entity_t p3 = ecs_new(world, Foo);
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);
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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tag);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new(world, Foo);
    ecs_entity_t p2 = ecs_new(world, Foo);
    ecs_entity_t p3 = ecs_new(world, Foo);
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);
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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new(world, Foo);
    ecs_entity_t p2 = ecs_new(world, Foo);
    ecs_entity_t p3 = ecs_new(world, Foo);
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo($x:self|up)",
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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new(world, Foo);
    ecs_entity_t p2 = ecs_new(world, Foo);
    ecs_entity_t p3 = ecs_new(world, Foo);
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo($x:up)",
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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tag);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new(world, Foo);
    ecs_entity_t p2 = ecs_new(world, Foo);
    ecs_entity_t p3 = ecs_new(world, Foo);
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);
    ecs_set_with(world, 0);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag($x:self), Foo($x:self|up)",
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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tag);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new(world, Foo);
    ecs_entity_t p2 = ecs_new(world, Foo);
    ecs_entity_t p3 = ecs_new(world, Foo);
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);
    ecs_set_with(world, 0);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag($x:self), Foo($x:up)",
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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new(world, Foo);
    ecs_entity_t p2 = ecs_new(world, Foo);
    ecs_entity_t p3 = ecs_new(world, Foo);
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_entity_t e8 = ecs_new_w_pair(world, EcsChildOf, p0);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo($x:self|up)",
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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new(world, Foo);
    ecs_entity_t p2 = ecs_new(world, Foo);
    ecs_entity_t p3 = ecs_new(world, Foo);
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_entity_t e8 = ecs_new_w_pair(world, EcsChildOf, p0);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo($x:up)",
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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tag);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new(world, Foo);
    ecs_entity_t p2 = ecs_new(world, Foo);
    ecs_entity_t p3 = ecs_new(world, Foo);
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_entity_t e8 = ecs_new_w_pair(world, EcsChildOf, p0);
    ecs_set_with(world, 0);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag($x:self), Foo($x:self|up)",
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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag($x:self), Foo($x:up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new(world, Foo);
    ecs_entity_t p2 = ecs_new(world, Foo);
    ecs_entity_t p3 = ecs_new(world, Foo);
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_entity_t e8 = ecs_new_w_pair(world, EcsChildOf, p0);
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

    ECS_TAG(world, Foo);

    ecs_entity_t parent = ecs_new_id(world);
    ecs_entity_t e = ecs_new_entity(world, "ent");

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(ent:self|up)",
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

    ECS_TAG(world, Foo);

    ecs_entity_t parent = ecs_new_id(world);
    ecs_entity_t e = ecs_new_entity(world, "ent");

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(ent:up)",
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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new(world, Foo);
    ecs_entity_t p2 = ecs_new(world, Foo);
    ecs_entity_t p3 = ecs_new(world, Foo);
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsIsA, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsIsA, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsIsA, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsIsA, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsIsA, p3);

    ecs_new_w_pair(world, EcsIsA, p0);

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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new(world, Foo);
    ecs_entity_t p2 = ecs_new(world, Foo);
    ecs_entity_t p3 = ecs_new(world, Foo);
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsIsA, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsIsA, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsIsA, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsIsA, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsIsA, p3);

    ecs_new_w_pair(world, EcsIsA, p0);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(up(IsA))",
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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo($x)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new(world, Foo);
    ecs_entity_t p2 = ecs_new(world, Foo);
    ecs_entity_t p3 = ecs_new(world, Foo);
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsIsA, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsIsA, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsIsA, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsIsA, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsIsA, p3);

    ecs_new_w_pair(world, EcsIsA, p0);

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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo($x:up(IsA))",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new(world, Foo);
    ecs_entity_t p2 = ecs_new(world, Foo);
    ecs_entity_t p3 = ecs_new(world, Foo);
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsIsA, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsIsA, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsIsA, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsIsA, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsIsA, p3);

    ecs_new_w_pair(world, EcsIsA, p0);

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

    ECS_TAG(world, Foo);

    ecs_entity_t parent = ecs_new_id(world);
    ecs_entity_t e = ecs_new_entity(world, "ent");

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(ent:self|up(IsA))",
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

    ECS_TAG(world, Foo);

    ecs_entity_t parent = ecs_new_id(world);
    ecs_entity_t e = ecs_new_entity(world, "ent");

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(ent:up(IsA))",
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

    ECS_TAG(world, Foo);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(self|up(IsA))",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_entity_t parent_a = ecs_new_id(world);
    ecs_entity_t parent_b = ecs_new(world, Foo);
    ecs_entity_t parent_c = ecs_new(world, Foo);
    ecs_entity_t e_1 = ecs_new_w_pair(world, EcsIsA, parent_a);
    ecs_add_pair(world, e_1, EcsIsA, parent_b);
    ecs_entity_t e_2 = ecs_new_w_pair(world, EcsIsA, parent_b);
    ecs_new_w_pair(world, EcsIsA, parent_a);
    ecs_entity_t e_4 = ecs_new_w_pair(world, EcsIsA, parent_b);
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

    ECS_TAG(world, Foo);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(up(IsA))",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_entity_t parent_a = ecs_new_id(world);
    ecs_entity_t parent_b = ecs_new(world, Foo);
    ecs_entity_t parent_c = ecs_new(world, Foo);
    ecs_entity_t e_1 = ecs_new_w_pair(world, EcsIsA, parent_a);
    ecs_add_pair(world, e_1, EcsIsA, parent_b);
    ecs_entity_t e_2 = ecs_new_w_pair(world, EcsIsA, parent_b);
    ecs_new_w_pair(world, EcsIsA, parent_a);
    ecs_entity_t e_4 = ecs_new_w_pair(world, EcsIsA, parent_b);
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

    ECS_TAG(world, Foo);

    ecs_entity_t parent_root = ecs_new(world, Foo);
    ecs_entity_t parent_a = ecs_new_w_pair(world, EcsIsA, parent_root);
    ecs_entity_t parent_b = ecs_new_w_pair(world, EcsIsA, parent_root);
    ecs_entity_t parent_c = ecs_new_w_pair(world, EcsIsA, parent_a);
    ecs_add_pair(world, parent_c, EcsIsA, parent_b);

    ecs_entity_t e_1 = ecs_new_w_pair(world, EcsIsA, parent_a);
    ecs_entity_t e_2 = ecs_new_w_pair(world, EcsIsA, parent_b);
    ecs_entity_t e_3 = ecs_new_w_pair(world, EcsIsA, parent_c);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(self|up(IsA))",
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

    ECS_TAG(world, Foo);

    ecs_entity_t parent_root = ecs_new(world, Foo);
    ecs_entity_t parent_a = ecs_new_w_pair(world, EcsIsA, parent_root);
    ecs_entity_t parent_b = ecs_new_w_pair(world, EcsIsA, parent_root);
    ecs_entity_t parent_c = ecs_new_w_pair(world, EcsIsA, parent_a);
    ecs_add_pair(world, parent_c, EcsIsA, parent_b);

    ecs_entity_t e_1 = ecs_new_w_pair(world, EcsIsA, parent_a);
    ecs_entity_t e_2 = ecs_new_w_pair(world, EcsIsA, parent_b);
    ecs_entity_t e_3 = ecs_new_w_pair(world, EcsIsA, parent_c);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(up(IsA))",
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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Foo(self|up(IsA))",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);
    
    ecs_set_with(world, Tag);
    ecs_entity_t parent_a = ecs_new_id(world);
    ecs_entity_t parent_b = ecs_new(world, Foo);
    ecs_entity_t parent_c = ecs_new(world, Foo);
    ecs_entity_t e_1 = ecs_new_w_pair(world, EcsIsA, parent_a);
    ecs_add_pair(world, e_1, EcsIsA, parent_b);
    ecs_entity_t e_2 = ecs_new_w_pair(world, EcsIsA, parent_b);
    ecs_new_w_pair(world, EcsIsA, parent_a);
    ecs_entity_t e_4 = ecs_new_w_pair(world, EcsIsA, parent_b);
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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Foo(up(IsA))",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);
    
    ecs_set_with(world, Tag);
    ecs_entity_t parent_a = ecs_new_id(world);
    ecs_entity_t parent_b = ecs_new(world, Foo);
    ecs_entity_t parent_c = ecs_new(world, Foo);
    ecs_entity_t e_1 = ecs_new_w_pair(world, EcsIsA, parent_a);
    ecs_add_pair(world, e_1, EcsIsA, parent_b);
    ecs_entity_t e_2 = ecs_new_w_pair(world, EcsIsA, parent_b);
    ecs_new_w_pair(world, EcsIsA, parent_a);
    ecs_entity_t e_4 = ecs_new_w_pair(world, EcsIsA, parent_b);
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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Foo(self|up(IsA))",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t parent_root = ecs_new(world, Foo);
    ecs_entity_t parent_a = ecs_new_w_pair(world, EcsIsA, parent_root);
    ecs_entity_t parent_b = ecs_new_w_pair(world, EcsIsA, parent_root);
    ecs_entity_t parent_c = ecs_new_w_pair(world, EcsIsA, parent_a);
    ecs_add_pair(world, parent_c, EcsIsA, parent_b);

    ecs_entity_t e_1 = ecs_new_w_pair(world, EcsIsA, parent_a);
    ecs_entity_t e_2 = ecs_new_w_pair(world, EcsIsA, parent_b);
    ecs_entity_t e_3 = ecs_new_w_pair(world, EcsIsA, parent_c);
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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Foo(up(IsA))",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t parent_root = ecs_new(world, Foo);
    ecs_entity_t parent_a = ecs_new_w_pair(world, EcsIsA, parent_root);
    ecs_entity_t parent_b = ecs_new_w_pair(world, EcsIsA, parent_root);
    ecs_entity_t parent_c = ecs_new_w_pair(world, EcsIsA, parent_a);
    ecs_add_pair(world, parent_c, EcsIsA, parent_b);

    ecs_entity_t e_1 = ecs_new_w_pair(world, EcsIsA, parent_a);
    ecs_entity_t e_2 = ecs_new_w_pair(world, EcsIsA, parent_b);
    ecs_entity_t e_3 = ecs_new_w_pair(world, EcsIsA, parent_c);
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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Tag);

    ecs_set_with(world, Tag);
    ecs_entity_t parent_a = ecs_new_id(world);
    ecs_entity_t parent_b = ecs_new(world, Foo);
    ecs_new_w_pair(world, EcsIsA, parent_a);
    ecs_entity_t e_2 = ecs_new_w_pair(world, EcsIsA, parent_b);
    ecs_entity_t e_3 = ecs_new_w_pair(world, EcsIsA, e_2);
    ecs_set_with(world, 0);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag, Foo(self|up(IsA))",
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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Tag);

    ecs_set_with(world, Tag);
    ecs_entity_t parent_a = ecs_new_id(world);
    ecs_entity_t parent_b = ecs_new(world, Foo);
    ecs_entity_t parent_c = ecs_new(world, Foo);
    ecs_entity_t e_1 = ecs_new_w_pair(world, EcsIsA, parent_a);
    ecs_add_pair(world, e_1, EcsIsA, parent_b);
    ecs_entity_t e_2 = ecs_new_w_pair(world, EcsIsA, parent_b);
    ecs_new_w_pair(world, EcsIsA, parent_a);
    ecs_entity_t e_4 = ecs_new_w_pair(world, EcsIsA, parent_b);
    ecs_add_pair(world, e_4, EcsIsA, parent_c);
    ecs_set_with(world, 0);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag, Foo(self|up(IsA))",
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

    ECS_TAG(world, Foo);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(self|up(IsA))",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);
    
    ecs_entity_t e1 = ecs_new(world, Foo);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, e1);
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

    ECS_TAG(world, Foo);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(up(IsA))",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);
    
    ecs_entity_t e1 = ecs_new(world, Foo);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, e1);
    ecs_delete(world, e2);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Traversal_self_up_all_owned(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self|up(IsA))",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t parent_root = ecs_new(world, 0);
    ecs_entity_t parent_a = ecs_new_w_pair(world, EcsIsA, parent_root);
    ecs_entity_t parent_b = ecs_new_w_pair(world, EcsIsA, parent_root);
    ecs_entity_t parent_c = ecs_new_w_pair(world, EcsIsA, parent_a);
    ecs_add_pair(world, parent_c, EcsIsA, parent_b);

    ecs_entity_t e_1 = ecs_new_w_pair(world, EcsIsA, parent_a);
    ecs_entity_t e_2 = ecs_new_w_pair(world, EcsIsA, parent_b);
    ecs_entity_t e_3 = ecs_new_w_pair(world, EcsIsA, parent_c);
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

    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(up(IsA))",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t parent_root = ecs_new(world, 0);
    ecs_entity_t parent_a = ecs_new_w_pair(world, EcsIsA, parent_root);
    ecs_entity_t parent_b = ecs_new_w_pair(world, EcsIsA, parent_root);
    ecs_entity_t parent_c = ecs_new_w_pair(world, EcsIsA, parent_a);
    ecs_add_pair(world, parent_c, EcsIsA, parent_b);

    ecs_entity_t e_1 = ecs_new_w_pair(world, EcsIsA, parent_a);
    ecs_entity_t e_2 = ecs_new_w_pair(world, EcsIsA, parent_b);
    ecs_entity_t e_3 = ecs_new_w_pair(world, EcsIsA, parent_c);
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

    ECS_TAG(world, Foo);

    ecs_entity_t base = ecs_new(world, Foo);
    ecs_entity_t parent = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    
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

    ECS_TAG(world, Foo);

    ecs_entity_t base = ecs_new(world, Foo);
    ecs_entity_t parent = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    
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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Foo(self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t base = ecs_new(world, Foo);
    ecs_entity_t parent = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Foo(up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t base = ecs_new(world, Foo);
    ecs_entity_t parent = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
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

    ECS_TAG(world, Foo);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo($x:self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t base = ecs_new(world, Foo);
    ecs_entity_t parent = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    
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

    ECS_TAG(world, Foo);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo($x:up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t base = ecs_new(world, Foo);
    ecs_entity_t parent = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    
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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag($x:self), Foo($x:self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_set_with(world, Tag);
    ecs_entity_t base = ecs_new(world, Foo);
    ecs_entity_t parent = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag($x:self), Foo($x:up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_set_with(world, Tag);
    ecs_entity_t base = ecs_new(world, Foo);
    ecs_entity_t parent = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
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

    ECS_TAG(world, Foo);

    ecs_entity_t base = ecs_new(world, Foo);
    ecs_entity_t parent = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_set_name(world, parent, "parent");
    ecs_set_name(world, child, "child");

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(parent.child:self|up)",
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

    ECS_TAG(world, Foo);

    ecs_entity_t base = ecs_new(world, Foo);
    ecs_entity_t parent = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_set_name(world, parent, "parent");
    ecs_set_name(world, child, "child");

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(parent.child:up)",
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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Tag);

    ecs_set_with(world, Tag);
    ecs_entity_t base = ecs_new(world, Foo);
    ecs_entity_t parent = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_set_name(world, parent, "parent");
    ecs_set_name(world, child, "child");
    ecs_set_with(world, 0);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(parent.child:self|up)",
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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Tag);

    ecs_set_with(world, Tag);
    ecs_entity_t base = ecs_new(world, Foo);
    ecs_entity_t parent = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_set_name(world, parent, "parent");
    ecs_set_name(world, child, "child");
    ecs_set_with(world, 0);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(parent.child:up)",
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
    ECS_TAG(world, Bar);

    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t p2 = ecs_set(world, 0, Position, {20, 30});
    ecs_entity_t p3 = ecs_set(world, 0, Position, {30, 40});
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);

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
    ECS_TAG(world, Bar);

    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t p2 = ecs_set(world, 0, Position, {20, 30});
    ecs_entity_t p3 = ecs_set(world, 0, Position, {30, 40});
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);

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
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Position(self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_set(world, ecs_new(world, Tag), Position, {10, 20});
    ecs_entity_t p2 = ecs_set(world, ecs_new(world, Tag), Position, {20, 30});
    ecs_entity_t p3 = ecs_set(world, ecs_new(world, Tag), Position, {30, 40});
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);
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
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Position(up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_set(world, ecs_new(world, Tag), Position, {10, 20});
    ecs_entity_t p2 = ecs_set(world, ecs_new(world, Tag), Position, {20, 30});
    ecs_entity_t p3 = ecs_set(world, ecs_new(world, Tag), Position, {30, 40});
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);
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
    ECS_TAG(world, Bar);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position($x:self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t p2 = ecs_set(world, 0, Position, {20, 30});
    ecs_entity_t p3 = ecs_set(world, 0, Position, {30, 40});
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);

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
    ECS_TAG(world, Bar);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Position($x:up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t p2 = ecs_set(world, 0, Position, {20, 30});
    ecs_entity_t p3 = ecs_set(world, 0, Position, {30, 40});
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);

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
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag($x:self), Position($x:self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_set(world, ecs_new(world, Tag), Position, {10, 20});
    ecs_entity_t p2 = ecs_set(world, ecs_new(world, Tag), Position, {20, 30});
    ecs_entity_t p3 = ecs_set(world, ecs_new(world, Tag), Position, {30, 40});
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);
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
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag($x:self), Position($x:up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_set(world, ecs_new(world, Tag), Position, {10, 20});
    ecs_entity_t p2 = ecs_set(world, ecs_new(world, Tag), Position, {20, 30});
    ecs_entity_t p3 = ecs_set(world, ecs_new(world, Tag), Position, {30, 40});
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);
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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p0 = ecs_new_id(world);
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p1 = ecs_new(world, Foo);
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p2 = ecs_new(world, Foo);
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p3 = ecs_new(world, Foo);
    ecs_add(world, p3, Bar);

    test_assert((uint32_t)p0 != p0);
    test_assert((uint32_t)p1 != p1);
    test_assert((uint32_t)p2 != p2);
    test_assert((uint32_t)p3 != p3);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);

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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p0 = ecs_new_id(world);
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p1 = ecs_new(world, Foo);
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p2 = ecs_new(world, Foo);
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p3 = ecs_new(world, Foo);
    ecs_add(world, p3, Bar);

    test_assert((uint32_t)p0 != p0);
    test_assert((uint32_t)p1 != p1);
    test_assert((uint32_t)p2 != p2);
    test_assert((uint32_t)p3 != p3);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);

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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Foo(self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p0 = ecs_new_id(world);
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p1 = ecs_new(world, Foo);
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p2 = ecs_new(world, Foo);
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p3 = ecs_new(world, Foo);
    ecs_add(world, p3, Bar);

    test_assert((uint32_t)p0 != p0);
    test_assert((uint32_t)p1 != p1);
    test_assert((uint32_t)p2 != p2);
    test_assert((uint32_t)p3 != p3);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);
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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Foo(up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p0 = ecs_new_id(world);
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p1 = ecs_new(world, Foo);
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p2 = ecs_new(world, Foo);
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p3 = ecs_new(world, Foo);
    ecs_add(world, p3, Bar);

    test_assert((uint32_t)p0 != p0);
    test_assert((uint32_t)p1 != p1);
    test_assert((uint32_t)p2 != p2);
    test_assert((uint32_t)p3 != p3);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);
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
    ECS_TAG(world, Bar);

    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p0 = ecs_new_id(world);
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p1 = ecs_set(world, 0, Position, {10, 20});
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p2 = ecs_set(world, 0, Position, {20, 30});
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p3 = ecs_set(world, 0, Position, {30, 40});
    ecs_add(world, p3, Bar);

    test_assert((uint32_t)p0 != p0);
    test_assert((uint32_t)p1 != p1);
    test_assert((uint32_t)p2 != p2);
    test_assert((uint32_t)p3 != p3);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);

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
    ECS_TAG(world, Bar);

    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p0 = ecs_new_id(world);
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p1 = ecs_set(world, 0, Position, {10, 20});
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p2 = ecs_set(world, 0, Position, {20, 30});
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p3 = ecs_set(world, 0, Position, {30, 40});
    ecs_add(world, p3, Bar);

    test_assert((uint32_t)p0 != p0);
    test_assert((uint32_t)p1 != p1);
    test_assert((uint32_t)p2 != p2);
    test_assert((uint32_t)p3 != p3);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);

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
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Position(self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p0 = ecs_new_id(world);
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p1 = ecs_set(world, ecs_new(world, Tag), Position, {10, 20});
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p2 = ecs_set(world, ecs_new(world, Tag), Position, {20, 30});
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p3 = ecs_set(world, ecs_new(world, Tag), Position, {30, 40});
    ecs_add(world, p3, Bar);

    test_assert((uint32_t)p0 != p0);
    test_assert((uint32_t)p1 != p1);
    test_assert((uint32_t)p2 != p2);
    test_assert((uint32_t)p3 != p3);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);
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
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Position(up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p0 = ecs_new_id(world);
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p1 = ecs_set(world, ecs_new(world, Tag), Position, {10, 20});
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p2 = ecs_set(world, ecs_new(world, Tag), Position, {20, 30});
    ecs_delete(world, ecs_new_id(world));
    ecs_entity_t p3 = ecs_set(world, ecs_new(world, Tag), Position, {30, 40});
    ecs_add(world, p3, Bar);

    test_assert((uint32_t)p0 != p0);
    test_assert((uint32_t)p1 != p1);
    test_assert((uint32_t)p2 != p2);
    test_assert((uint32_t)p3 != p3);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);
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

    ECS_TAG(world, Bar);
    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new_w_pair(world, Rel, Tgt);
    ecs_entity_t p2 = ecs_new_w_pair(world, Rel, Tgt);
    ecs_entity_t p3 = ecs_new_w_pair(world, Rel, Tgt);
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);

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

    ECS_TAG(world, Bar);
    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new_w_pair(world, Rel, Tgt);
    ecs_entity_t p2 = ecs_new_w_pair(world, Rel, Tgt);
    ecs_entity_t p3 = ecs_new_w_pair(world, Rel, Tgt);
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);

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

    ECS_TAG(world, Bar);
    ECS_TAG(world, Tag);
    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Rel(self|up, Tgt)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new_w_pair(world, Rel, Tgt);
    ecs_entity_t p2 = ecs_new_w_pair(world, Rel, Tgt);
    ecs_entity_t p3 = ecs_new_w_pair(world, Rel, Tgt);
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);
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

    ECS_TAG(world, Bar);
    ECS_TAG(world, Tag);
    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Rel(up, Tgt)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new_w_pair(world, Rel, Tgt);
    ecs_entity_t p2 = ecs_new_w_pair(world, Rel, Tgt);
    ecs_entity_t p3 = ecs_new_w_pair(world, Rel, Tgt);
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);
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

    ECS_TAG(world, Bar);
    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t p2 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_entity_t p3 = ecs_new_w_pair(world, Rel, TgtC);
    ecs_add_pair(world, p3, EcsChildOf, p2);
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);

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

    ECS_TAG(world, Bar);
    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t p2 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_entity_t p3 = ecs_new_w_pair(world, Rel, TgtC);
    ecs_add_pair(world, p3, EcsChildOf, p2);
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);

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

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Rel(self|up, *)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t p2 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_entity_t p3 = ecs_new_w_pair(world, Rel, TgtC);
    ecs_add(world, p3, Bar);
    ecs_add_pair(world, p3, EcsChildOf, p2);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);
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

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Rel(up, *)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t p2 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_entity_t p3 = ecs_new_w_pair(world, Rel, TgtC);
    ecs_add(world, p3, Bar);
    ecs_add_pair(world, p3, EcsChildOf, p2);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);
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

    ECS_TAG(world, Bar);
    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Rel(self|up, $x)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t p2 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_entity_t p3 = ecs_new_w_pair(world, Rel, TgtC);
    ecs_add_pair(world, p3, EcsChildOf, p2);
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);

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

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), Rel(self|up, $x)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t p2 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_entity_t p3 = ecs_new_w_pair(world, Rel, TgtC);
    ecs_add(world, p3, Bar);
    ecs_add_pair(world, p3, EcsChildOf, p2);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);
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

    ECS_TAG(world, Bar);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, Tgt);

    ecs_query_t *r = ecs_query(world, {
        .expr = "$x(self|up, Tgt)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new_w_pair(world, RelA, Tgt);
    ecs_entity_t p2 = ecs_new_w_pair(world, RelB, Tgt);
    ecs_entity_t p3 = ecs_new_w_pair(world, RelC, Tgt);
    ecs_add_pair(world, p3, EcsChildOf, p2);
    ecs_add(world, p3, Bar);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);

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

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, RelC);
    ECS_TAG(world, Tgt);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag(self), $x(self|up, Tgt)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_set_with(world, Tag);
    ecs_entity_t p0 = ecs_new_id(world);
    ecs_entity_t p1 = ecs_new_w_pair(world, RelA, Tgt);
    ecs_entity_t p2 = ecs_new_w_pair(world, RelB, Tgt);
    ecs_entity_t p3 = ecs_new_w_pair(world, RelC, Tgt);
    ecs_add(world, p3, Bar);
    ecs_add_pair(world, p3, EcsChildOf, p2);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add(world, e2, Bar);

    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add(world, e4, Bar);

    ecs_entity_t e5 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_add(world, e6, Bar);

    ecs_entity_t e7 = ecs_new_w_pair(world, EcsChildOf, p3);

    ecs_new_w_pair(world, EcsChildOf, p0);
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
        .expr = "Rel(ent, $x), $x(self|up(IsA))",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t p1 = ecs_new(world, TagA);
    ecs_entity_t p2 = ecs_new(world, TagB);
    ecs_entity_t p3 = ecs_new(world, TagC);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, p1);
    ecs_add_pair(world, e1, EcsIsA, p2);
    ecs_add_pair(world, e1, EcsIsA, p3);

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
    test_uint(e1, it.entities[0]);
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
    test_uint(e1, it.entities[0]);
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
        .expr = "Rel(ent, $x), $x(up(IsA))",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t p1 = ecs_new(world, TagA);
    ecs_entity_t p2 = ecs_new(world, TagB);
    ecs_entity_t p3 = ecs_new(world, TagC);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, p1);
    ecs_add_pair(world, e1, EcsIsA, p2);
    ecs_add_pair(world, e1, EcsIsA, p3);

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
    test_uint(e1, it.entities[0]);
    test_uint(ent, ecs_field_src(&it, 0));
    test_uint(p2, ecs_field_src(&it, 1));
    test_uint(ecs_pair(Rel, TagB), ecs_field_id(&it, 0));
    test_uint(TagB, ecs_field_id(&it, 1));
    test_uint(TagB, ecs_iter_get_var(&it, x_var));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
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

    ECS_TAG(world, Rel);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Rel(self, $x), $x(self|up)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t p0 = ecs_new(world, 0);
    ecs_entity_t p1 = ecs_new(world, TagA);
    ecs_entity_t p2 = ecs_new(world, TagB);
    ecs_entity_t p3 = ecs_new(world, TagC);

    ecs_entity_t e0 = ecs_new_w_pair(world, EcsChildOf, p0);
    ecs_add_pair(world, e0, Rel, TagA);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p1);
    ecs_add_pair(world, e1, Rel, TagA);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, p2);
    ecs_add_pair(world, e2, Rel, TagB);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, p3);
    ecs_add_pair(world, e3, Rel, TagC);

    ecs_entity_t e4 = ecs_new_w_pair(world, Rel, TagA);
    ecs_add(world, e4, TagA);
    ecs_new_w_pair(world, Rel, TagB);

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

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);
    ECS_TAG(world, Rel);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ECS_ENTITY(world, ent, (Rel, TagA), (Rel, TagB), (Rel, TagC));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Rel(ent, $x), $x(self|up(IsA))",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_entity_t p0 = ecs_new(world, 0);
    ecs_entity_t p0_a = ecs_new(world, TagA);
    ecs_entity_t p0_b = ecs_new(world, TagB);
    ecs_add(world, p0_b, Foo);
    ecs_entity_t p0_c = ecs_new(world, TagC);
    ecs_add(world, p0_c, Foo);
    ecs_add(world, p0_c, Bar);
    ecs_entity_t p1 = ecs_new_id(world);
    ecs_add_pair(world, p1, EcsIsA, p0_a);
    ecs_add_pair(world, p1, EcsIsA, p0_b);
    ecs_add_pair(world, p1, EcsIsA, p0_c);

    ecs_new_w_pair(world, EcsIsA, p0);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, p1);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TagA);
    ecs_add(world, e2, TagA);
    ecs_new_w_pair(world, Rel, TagB);

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

    ECS_TAG(world, Rel);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Rel(self, $x), $x(self|up(IsA))",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    ecs_entity_t p0 = ecs_new(world, 0);
    ecs_entity_t p1 = ecs_new(world, TagA);
    ecs_entity_t p2 = ecs_new(world, TagB);
    ecs_entity_t p3 = ecs_new(world, TagC);

    ecs_entity_t e0 = ecs_new_w_pair(world, EcsIsA, p0);
    ecs_add_pair(world, e0, Rel, TagA);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, p1);
    ecs_add_pair(world, e1, EcsIsA, p2);
    ecs_add_pair(world, e1, EcsIsA, p3);
    ecs_add_pair(world, e1, Rel, TagA);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, p1);
    ecs_add_pair(world, e2, EcsIsA, p2);
    ecs_add_pair(world, e2, EcsIsA, p3);
    ecs_add_pair(world, e2, Rel, TagB);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsIsA, p1);
    ecs_add_pair(world, e3, EcsIsA, p2);
    ecs_add_pair(world, e3, EcsIsA, p3);
    ecs_add_pair(world, e3, Rel, TagC);

    ecs_entity_t e4 = ecs_new_w_pair(world, Rel, TagA);
    ecs_add(world, e4, TagA);
    ecs_new_w_pair(world, Rel, TagB);

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

void Traversal_self_up_2_levels(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_set(world, base, Position, {10, 20});

    ecs_entity_t base_2 = ecs_new_w_id(world, EcsPrefab);
    ecs_add_pair(world, base_2, EcsIsA, base);

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base_2);

    ecs_query_t *f = ecs_query(world, {
        .terms[0] = { .id = ecs_id(Position) }
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

void Traversal_not_up_disabled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_query_t *f = ecs_query(world, {
        .terms = {
            { TagA },
            { .id = EcsDisabled, .src.id = EcsUp, .oper = EcsNot }
        }
    });
    test_assert(f != NULL);

    ecs_entity_t parent = ecs_new_w_id(world, EcsDisabled);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_add_id(world, e2, EcsDisabled);
    ecs_entity_t e3 = ecs_new(world, TagA);
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

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, Rel);

    ecs_add_id(world, Rel, EcsTraversable);

    ecs_entity_t b1 = ecs_new(world, TagA);
    ecs_entity_t b2 = ecs_new(world, TagA);
    ecs_add(world, b1, TagB);
    ecs_add(world, b2, TagB);
    ecs_add(world, b1, TagC);
    ecs_add(world, b2, TagC);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, Rel, b1);
    ecs_add_pair(world, e, Rel, b2);

    ecs_query_t *f = ecs_query(world, {
        .terms = {
            {TagC, .src.id = EcsUp, .trav = Rel }
        }
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

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, Rel);

    ecs_add_id(world, Rel, EcsTraversable);

    ecs_entity_t b1 = ecs_new(world, TagA);
    ecs_entity_t b2 = ecs_new(world, TagA);
    ecs_add(world, b1, TagB);
    ecs_add(world, b2, TagB);
    ecs_add(world, b2, TagC);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, Rel, b1);
    ecs_add_pair(world, e, Rel, b2);

    ecs_query_t *f = ecs_query(world, {
        .terms = {
            { TagC, .src.id = EcsUp, .trav = Rel }
        }
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

    ecs_entity_t e_0 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e_1 = ecs_set(world, 0, Position, {30, 40});
    ecs_entity_t e_2 = ecs_set(world, 0, Position, {50, 60});
    ecs_entity_t e_3 = ecs_set(world, 0, Position, {70, 80});

    ecs_add_pair(world, e_3, R, e_2);
    ecs_add_pair(world, e_2, R, e_1);
    ecs_add_pair(world, e_1, R, e_0);

    ecs_query_t *f = ecs_query(world, {
        .expr = "Position(up(R))",
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

    ECS_TAG(world, Foo);
    ECS_ENTITY(world, Rel, Traversable);

    ecs_query_t *r = ecs_query(world, {
        .expr = "Foo(up(Rel))",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_new(world, Foo);
    
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
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t base = ecs_new(world, Velocity);
    
    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_add_pair(world, e1, EcsIsA, base);

    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_add(world, e2, Velocity);

    ecs_entity_t e3 = ecs_new(world, Position);

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
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t base = ecs_new(world, Velocity);
    
    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_add_pair(world, e1, EcsIsA, base);

    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_add(world, e2, Velocity);

    ecs_entity_t e3 = ecs_new(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "?Velocity(up(IsA)), Position",
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
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t base = ecs_new(world, Velocity);
    
    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_add_pair(world, e1, EcsIsA, base);

    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_add(world, e2, Velocity);

    ecs_entity_t e3 = ecs_new(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "?Velocity(self|up(IsA)), Position",
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
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t base = ecs_new(world, Velocity);
    
    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_add_pair(world, e1, EcsIsA, base);

    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_add(world, e2, Velocity);

    ecs_entity_t e3 = ecs_new(world, Position);

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
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t base = ecs_new(world, Velocity);
    
    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_add_pair(world, e1, EcsIsA, base);

    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_add(world, e2, Velocity);

    ecs_entity_t e3 = ecs_new(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, ?Velocity(up(IsA))",
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
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t base = ecs_new(world, Velocity);
    
    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_add_pair(world, e1, EcsIsA, base);

    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_add(world, e2, Velocity);

    ecs_entity_t e3 = ecs_new(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, ?Velocity(self|up(IsA))",
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
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t Game = ecs_new_entity(world, "Game");
    ecs_set(world, Game, Position, {10, 20});
    ecs_set(world, Game, Velocity, {1, 2});

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_set(world, p, Mass, {30});
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, p);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Velocity(Game), Mass(up(IsA))",
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
    ECS_TAG(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(up)",
        .cache_kind = cache_kind,
        .flags =  EcsQueryMatchEmptyTables
    });

    test_assert(q != NULL);

    ecs_entity_t p = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p);
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
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagB, Position(up)",
        .cache_kind = cache_kind,
        .flags =  EcsQueryMatchEmptyTables
    });

    test_assert(q != NULL);

    ecs_entity_t p = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p);
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
    ECS_TAG(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(up)",
        .cache_kind = cache_kind,
        .flags =  EcsQueryMatchEmptyTables
    });

    test_assert(q != NULL);

    ecs_entity_t b = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t p = ecs_new_w_pair(world, EcsIsA, b);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p);
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
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagB, Position(up)",
        .cache_kind = cache_kind,
        .flags =  EcsQueryMatchEmptyTables
    });

    test_assert(q != NULL);

    ecs_entity_t b = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t p = ecs_new_w_pair(world, EcsIsA, b);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, p);
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
    ECS_TAG(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(up(IsA))",
        .cache_kind = cache_kind,
        .flags =  EcsQueryMatchEmptyTables
    });

    test_assert(q != NULL);

    ecs_entity_t p = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, p);
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
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagB, Position(up(IsA))",
        .cache_kind = cache_kind,
        .flags =  EcsQueryMatchEmptyTables
    });

    test_assert(q != NULL);

    ecs_entity_t p = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, p);
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
