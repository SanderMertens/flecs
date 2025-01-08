#include <query.h>
#include <stdlib.h>

static ecs_query_cache_kind_t cache_kind = EcsQueryCacheDefault;

void Toggle_setup(void) {
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

void Toggle_fixed_src_1_tag_toggle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_add_id(world, Foo, EcsCanToggle);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo(e)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_id(world, e, Foo, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_id(world, e, Foo, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_fixed_src_1_component_toggle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(e)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, e, Position, {10, 20});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e, ecs_field_src(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, e, Position, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, e, Position, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e, ecs_field_src(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_fixed_src_2_tag_toggle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_add_id(world, Foo, EcsCanToggle);
    ecs_add_id(world, Bar, EcsCanToggle);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo(e), Bar(e)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e, Foo);
    ecs_add(world, e, Bar);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e, ecs_field_src(&it, 0));
        test_uint(e, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_id(world, e, Foo, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_id(world, e, Foo, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e, ecs_field_src(&it, 0));
        test_uint(e, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_id(world, e, Bar, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_id(world, e, Bar, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e, ecs_field_src(&it, 0));
        test_uint(e, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_id(world, e, Foo, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_fixed_src_2_component_toggle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(e), Velocity(e)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, e, Position, {10, 20});
    ecs_set(world, e, Velocity, {1, 2});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e, ecs_field_src(&it, 0));
        test_uint(e, ecs_field_src(&it, 1));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        {
            const Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(v->x, 1);
            test_int(v->y, 2);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, e, Position, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, e, Position, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e, ecs_field_src(&it, 0));
        test_uint(e, ecs_field_src(&it, 1));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        {
            const Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(v->x, 1);
            test_int(v->y, 2);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, e, Velocity, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, e, Velocity, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e, ecs_field_src(&it, 0));
        test_uint(e, ecs_field_src(&it, 1));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        {
            const Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(v->x, 1);
            test_int(v->y, 2);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, e, Position, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_fixed_2_src_w_toggle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_add_id(world, Foo, EcsCanToggle);
    ecs_add_id(world, Bar, EcsCanToggle);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo(e1), Bar(e2)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Bar);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_id(world, e1, Foo, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_id(world, e1, Foo, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_id(world, e2, Bar, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_id(world, e2, Bar, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_id(world, e1, Foo, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_w_fixed_src_w_toggle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_add_id(world, Foo, EcsCanToggle);
    ecs_add_id(world, Bar, EcsCanToggle);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($this), Bar(e2)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Bar);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_id(world, e1, Foo, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_id(world, e1, Foo, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_id(world, e2, Bar, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_id(world, e2, Bar, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_id(world, e1, Foo, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_fixed_src_w_this_w_toggle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_add_id(world, Foo, EcsCanToggle);
    ecs_add_id(world, Bar, EcsCanToggle);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo(e1), Bar($this)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Bar);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_id(world, e1, Foo, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_id(world, e1, Foo, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_id(world, e2, Bar, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_id(world, e2, Bar, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Bar, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_id(world, e1, Foo, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_from_nothing(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            {TagA},
            {TagB, .src.id = EcsIsEntity}
        },
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_id(world, TagA);
    ecs_add(world, e1, TagB);
    ecs_add_id(world, e1, ECS_TOGGLE | TagB);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(ecs_field_id(&it, 0), TagA);
    test_uint(ecs_field_id(&it, 1), TagB);
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_entity_t e3 = ecs_new_w(world, Position);

    ecs_enable_component(world, e1, Position, true);
    ecs_enable_component(world, e2, Position, false);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = cache_kind
    });

    int32_t table_count = 0, count = 0;
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != e2);
            test_assert(it.entities[i] == e1 || it.entities[i] == e3);
            test_assert(ecs_is_enabled(world, it.entities[i], Position));
        }
        count += it.count;
        table_count ++;
    }

    test_int(count, 2);
    test_int(table_count, 2);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tgt);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ecs_entity_t e1 = ecs_new_w_pair(world, ecs_id(Position), Tgt);
    ecs_entity_t e2 = ecs_new_w_pair(world, ecs_id(Position), Tgt);
    ecs_entity_t e3 = ecs_new_w_pair(world, ecs_id(Position), Tgt);

    ecs_enable_pair(world, e1, Position, Tgt, true);
    ecs_enable_pair(world, e2, Position, Tgt, false);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Position, Tgt)",
        .cache_kind = cache_kind
    });
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t table_count = 0, count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != e2);
            test_assert(it.entities[i] == e1 || it.entities[i] == e3);
            test_assert(ecs_is_enabled_pair(world, it.entities[i], Position, Tgt));
        }
        count += it.count;
        table_count ++;
    }

    test_int(count, 2);
    test_int(table_count, 2);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_skip_initial(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    
    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_entity_t e3 = ecs_new_w(world, Position);

    ecs_enable_component(world, e1, Position, false);
    ecs_enable_component(world, e2, Position, true);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = cache_kind
    });
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != e1);
            test_assert(it.entities[i] == e2 || it.entities[i] == e3);
            test_assert(ecs_is_enabled(world, it.entities[i], Position));
        }
        count += it.count;
    }

    test_int(count, 2);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_pair_skip_initial(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tgt);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ecs_entity_t e1 = ecs_new_w_pair(world, ecs_id(Position), Tgt);
    ecs_entity_t e2 = ecs_new_w_pair(world, ecs_id(Position), Tgt);
    ecs_entity_t e3 = ecs_new_w_pair(world, ecs_id(Position), Tgt);

    ecs_enable_pair(world, e1, Position, Tgt, false);
    ecs_enable_pair(world, e2, Position, Tgt, true);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Position, Tgt)",
        .cache_kind = cache_kind
    });
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != e1);
            test_assert(it.entities[i] == e2 || it.entities[i] == e3);
            test_assert(ecs_is_enabled_pair(world, it.entities[i], Position, Tgt));
        }
        count += it.count;
    }

    test_int(count, 2);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_add_id(world, Foo, EcsCanToggle);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_entity_t e3 = ecs_new_w(world, Foo);

    ecs_enable_id(world, e1, Foo, true);
    ecs_enable_id(world, e2, Foo, false);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo",
        .cache_kind = cache_kind
    });

    int32_t table_count = 0, count = 0;
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != e2);
            test_assert(it.entities[i] == e1 || it.entities[i] == e3);
            test_assert(ecs_is_enabled_id(world, it.entities[i], Foo));
        }
        count += it.count;
        table_count ++;
    }

    test_int(count, 2);
    test_int(table_count, 2);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_tag_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_add_id(world, Rel, EcsCanToggle);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, Tgt);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, Tgt);
    ecs_entity_t e3 = ecs_new_w_pair(world, Rel, Tgt);

    ecs_enable_pair(world, e1, Rel, Tgt, true);
    ecs_enable_pair(world, e2, Rel, Tgt, false);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, Tgt)",
        .cache_kind = cache_kind
    });

    int32_t table_count = 0, count = 0;
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != e2);
            test_assert(it.entities[i] == e1 || it.entities[i] == e3);
            test_assert(ecs_is_enabled_pair(world, it.entities[i], Rel, Tgt));
        }
        count += it.count;
        table_count ++;
    }

    test_int(count, 2);
    test_int(table_count, 2);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_tag_pair_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);

    ecs_add_id(world, Rel, EcsCanToggle);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e3 = ecs_new_w_pair(world, Rel, TgtA);

    ecs_entity_t e4 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_entity_t e5 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_entity_t e6 = ecs_new_w_pair(world, Rel, TgtB);

    ecs_enable_pair(world, e1, Rel, TgtA, true);
    ecs_enable_pair(world, e2, Rel, TgtA, false);
    ecs_enable_pair(world, e3, Rel, TgtA, true);

    ecs_enable_pair(world, e4, Rel, TgtB, false);
    ecs_enable_pair(world, e5, Rel, TgtB, true);
    ecs_enable_pair(world, e6, Rel, TgtB, false);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, *)",
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(e1, it.entities[0]);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_uint(e3, it.entities[0]);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_uint(e5, it.entities[0]);
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_toggle_shared_self_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(self|up ChildOf)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, p, Position, {10, 20});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(p, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(p, ecs_field_src(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(cache_kind == EcsQueryCacheDefault ? p : e, it.entities[0]);
        test_uint(cache_kind == EcsQueryCacheDefault ? 0 : p, ecs_field_src(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(cache_kind == EcsQueryCacheDefault ? e : p, it.entities[0]);
        test_uint(cache_kind == EcsQueryCacheDefault ? p : 0, ecs_field_src(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_toggle_shared_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(up ChildOf)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, p, Position, {10, 20});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(p, ecs_field_src(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(p, ecs_field_src(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_toggle_shared_self_up_w_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(self|up ChildOf), Velocity",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, p, Position, {10, 20});
    ecs_set(world, p, Velocity, {1, 2});
    ecs_set(world, e, Velocity, {2, 4});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(p, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        {
            const Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(v->x, 1);
            test_int(v->y, 2);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(p, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        {
            const Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(v->x, 2);
            test_int(v->y, 4);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(cache_kind == EcsQueryCacheDefault ? p : e, it.entities[0]);
        test_uint(cache_kind == EcsQueryCacheDefault ? 0 : p, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        {
            const Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(cache_kind == EcsQueryCacheDefault ? 1 : 2, v->x);
            test_int(cache_kind == EcsQueryCacheDefault ? 2 : 4, v->y);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(cache_kind == EcsQueryCacheDefault ? e : p, it.entities[0]);
        test_uint(cache_kind == EcsQueryCacheDefault ? p : 0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        {
            const Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(cache_kind == EcsQueryCacheDefault ? 2 : 1, v->x);
            test_int(cache_kind == EcsQueryCacheDefault ? 4 : 2, v->y);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_toggle_shared_up_w_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(up ChildOf), Velocity",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, p, Position, {10, 20});
    ecs_set(world, p, Velocity, {1, 2});
    ecs_set(world, e, Velocity, {2, 4});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(p, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        {
            const Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(v->x, 2);
            test_int(v->y, 4);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(p, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        {
            const Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(v->x, 2);
            test_int(v->y, 4);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_toggle_shared_self_up_w_self_reverse(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Velocity, Position(self|up ChildOf)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, p, Position, {10, 20});
    ecs_set(world, p, Velocity, {1, 2});
    ecs_set(world, e, Velocity, {2, 4});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(p, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        {
            const Velocity *v = ecs_field(&it, Velocity, 0);
            test_assert(v != NULL);
            test_int(v->x, 1);
            test_int(v->y, 2);
        }
        {
            const Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(p, ecs_field_src(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        {
            const Velocity *v = ecs_field(&it, Velocity, 0);
            test_assert(v != NULL);
            test_int(v->x, 2);
            test_int(v->y, 4);
        }
        {
            const Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(p, ecs_field_src(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        {
            const Velocity *v = ecs_field(&it, Velocity, 0);
            test_assert(v != NULL);
            test_int(v->x, 2);
            test_int(v->y, 4);
        }
        {
            const Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(p, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        {
            const Velocity *v = ecs_field(&it, Velocity, 0);
            test_assert(v != NULL);
            test_int(v->x, 1);
            test_int(v->y, 2);
        }
        {
            const Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_toggle_shared_up_w_self_reverse(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Velocity, Position(up ChildOf)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, p, Position, {10, 20});
    ecs_set(world, p, Velocity, {1, 2});
    ecs_set(world, e, Velocity, {2, 4});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(p, ecs_field_src(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        {
            const Velocity *v = ecs_field(&it, Velocity, 0);
            test_assert(v != NULL);
            test_int(v->x, 2);
            test_int(v->y, 4);
        }
        {
            const Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(p, ecs_field_src(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        {
            const Velocity *v = ecs_field(&it, Velocity, 0);
            test_assert(v != NULL);
            test_int(v->x, 2);
            test_int(v->y, 4);
        }
        {
            const Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_toggle_shared_self_up_w_self_toggle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(self|up ChildOf), Velocity",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, p, Position, {10, 20});
    ecs_set(world, p, Velocity, {1, 2});
    ecs_set(world, e, Velocity, {2, 4});

    ecs_enable_component(world, p, Velocity, true);
    ecs_enable_component(world, e, Velocity, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(p, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        {
            const Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(v->x, 1);
            test_int(v->y, 2);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(p, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        {
            const Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(v->x, 2);
            test_int(v->y, 4);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(cache_kind == EcsQueryCacheDefault ? p : e, it.entities[0]);
        test_uint(cache_kind == EcsQueryCacheDefault ? 0 : p, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        {
            const Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(cache_kind == EcsQueryCacheDefault ? 1 : 2, v->x);
            test_int(cache_kind == EcsQueryCacheDefault ? 2 : 4, v->y);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(cache_kind == EcsQueryCacheDefault ? e : p, it.entities[0]);
        test_uint(cache_kind == EcsQueryCacheDefault ? p : 0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        {
            const Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(cache_kind == EcsQueryCacheDefault ? 2 : 1, v->x);
            test_int(cache_kind == EcsQueryCacheDefault ? 4 : 2, v->y);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_toggle_shared_up_w_self_toggle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(up ChildOf), Velocity",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, p, Position, {10, 20});
    ecs_set(world, p, Velocity, {1, 2});
    ecs_set(world, e, Velocity, {2, 4});

    ecs_enable_component(world, p, Velocity, true);
    ecs_enable_component(world, e, Velocity, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(p, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        {
            const Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(v->x, 2);
            test_int(v->y, 4);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(p, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        {
            const Position *p = ecs_field(&it, Position, 0);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        {
            const Velocity *v = ecs_field(&it, Velocity, 1);
            test_assert(v != NULL);
            test_int(v->x, 2);
            test_int(v->y, 4);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_toggle_shared_self_up_w_self_toggle_reverse(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Velocity, Position(self|up ChildOf)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, p, Position, {10, 20});
    ecs_set(world, p, Velocity, {1, 2});
    ecs_set(world, e, Velocity, {2, 4});

    ecs_enable_component(world, p, Velocity, true);
    ecs_enable_component(world, e, Velocity, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(p, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        {
            const Velocity *v = ecs_field(&it, Velocity, 0);
            test_assert(v != NULL);
            test_int(v->x, 1);
            test_int(v->y, 2);
        }
        {
            const Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(p, ecs_field_src(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        {
            const Velocity *v = ecs_field(&it, Velocity, 0);
            test_assert(v != NULL);
            test_int(v->x, 2);
            test_int(v->y, 4);
        }
        {
            const Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(p, ecs_field_src(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        {
            const Velocity *v = ecs_field(&it, Velocity, 0);
            test_assert(v != NULL);
            test_int(v->x, 2);
            test_int(v->y, 4);
        }
        {
            const Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(p, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        {
            const Velocity *v = ecs_field(&it, Velocity, 0);
            test_assert(v != NULL);
            test_int(v->x, 1);
            test_int(v->y, 2);
        }
        {
            const Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_toggle_shared_up_w_self_toggle_reverse(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Velocity, Position(up ChildOf)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, p, Position, {10, 20});
    ecs_set(world, p, Velocity, {1, 2});
    ecs_set(world, e, Velocity, {2, 4});

    ecs_enable_component(world, p, Velocity, true);
    ecs_enable_component(world, e, Velocity, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(p, ecs_field_src(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        {
            const Velocity *v = ecs_field(&it, Velocity, 0);
            test_assert(v != NULL);
            test_int(v->x, 2);
            test_int(v->y, 4);
        }
        {
            const Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(p, ecs_field_src(&it, 1));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        {
            const Velocity *v = ecs_field(&it, Velocity, 0);
            test_assert(v != NULL);
            test_int(v->x, 2);
            test_int(v->y, 4);
        }
        {
            const Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_toggle_not_shared_self_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_add(world, p, Foo);
    ecs_add(world, e, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, !Position(self|up ChildOf)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(p, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, p, Position, {10, 20});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(p, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_toggle_not_shared_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_add(world, p, Foo);
    ecs_add(world, e, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, !Position(up ChildOf)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(p, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, p, Position, {10, 20});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(p, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(p, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(p, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_toggle_optional_shared_self_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_add(world, p, Foo);
    ecs_add(world, e, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, ?Position(self|up ChildOf)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(p, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, p, Position, {10, 20});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(p, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(p, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(p, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(p, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(p, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_toggle_optional_shared_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_add(world, p, Foo);
    ecs_add(world, e, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, ?Position(up ChildOf)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(p, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_set(world, p, Position, {10, 20});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(p, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(p, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, false);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(p, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_enable_component(world, p, Position, true);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(p, ecs_field_src(&it, 1));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(p, it.entities[0]);
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

static
void test_disable_toggle_mod(int32_t total, int32_t mod) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    int32_t i, total_count = 0, total_toggle_count = 0;
    for (i = 0; i < total; i ++) {
        ecs_entity_t e = ecs_new_w(world, Position);
        ecs_set(world, e, Position, {e, e});

        if (!(e % mod)) {
            ecs_enable_component(world, e, Position, false);
        } else {
            ecs_enable_component(world, e, Position, true);
            total_count ++;
            total_toggle_count ++;
        }
    }

    for (i = 0; i < total; i ++) {
        ecs_entity_t e = ecs_new_w(world, Position);
        ecs_set(world, e, Position, {e, e});
        total_count ++;
    }

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    int32_t count = 0, toggle_count = 0;

    while (ecs_query_next(&it)) {
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            if (ecs_table_has_id(world, it.table, ECS_TOGGLE | ecs_id(Position))) {
                test_assert(it.entities[i] % mod);
                toggle_count ++;
            }
            test_assert(ecs_is_enabled(world, it.entities[i], Position));
            test_int(p[i].x, it.entities[i]);
            test_int(p[i].y, it.entities[i]);
        }
        count += it.count;
    }

    test_int(toggle_count, total_toggle_count);
    test_int(count, total_count);

    ecs_query_fini(q);

    ecs_fini(world);
}

static
void test_enable_toggle_mod(int32_t total, int32_t mod) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    int32_t i, total_count = 0, total_toggle_count = 0;
    for (i = 0; i < total; i ++) {
        ecs_entity_t e = ecs_new_w(world, Position);
        ecs_set(world, e, Position, {e, e});
        if (e % mod) {
            ecs_enable_component(world, e, Position, false);
        } else {
            ecs_enable_component(world, e, Position, true);
            total_count ++;
            total_toggle_count ++;
        }
    }

    for (i = 0; i < total; i ++) {
        ecs_entity_t e = ecs_new_w(world, Position);
        ecs_set(world, e, Position, {e, e});
        total_count ++;
    }

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = cache_kind
    });
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0, toggle_count = 0;

    while (ecs_query_next(&it)) {
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            if (ecs_table_has_id(world, it.table, ECS_TOGGLE | ecs_id(Position))) {
                test_assert(!(it.entities[i] % mod));
                toggle_count ++;
            }

            test_assert(ecs_is_enabled(world, it.entities[i], Position));
            test_int(p[i].x, it.entities[i]);
            test_int(p[i].y, it.entities[i]);
        }
        count += it.count;
    }

    test_int(toggle_count, total_toggle_count);
    test_int(count, total_count);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_64_mod_1(void) {
    test_disable_toggle_mod(64, 1);
}

void Toggle_this_64_mod_2(void) {
    test_disable_toggle_mod(64, 2);
}

void Toggle_this_64_mod_3(void) {
    test_disable_toggle_mod(64, 3);
}

void Toggle_this_64_mod_7(void) {
    test_disable_toggle_mod(64, 7);
}

void Toggle_this_64_mod_8(void) {
    test_disable_toggle_mod(64, 8);
}

void Toggle_this_64_mod_10(void) {
    test_disable_toggle_mod(64, 10);
}

void Toggle_this_64_mod_64(void) {
    test_disable_toggle_mod(64, 64);
}

void Toggle_this_64_mod_256(void) {
    test_disable_toggle_mod(64, 256);
}

void Toggle_this_64_mod_1024(void) {
    test_disable_toggle_mod(64, 1024);
}

void Toggle_this_100_mod_1(void) {
    test_disable_toggle_mod(100, 1);
}

void Toggle_this_100_mod_2(void) {
    test_disable_toggle_mod(100, 2);
}

void Toggle_this_100_mod_3(void) {
    test_disable_toggle_mod(100, 3);
}

void Toggle_this_100_mod_7(void) {
    test_disable_toggle_mod(100, 7);
}

void Toggle_this_100_mod_8(void) {
    test_disable_toggle_mod(100, 8);
}

void Toggle_this_100_mod_10(void) {
    test_disable_toggle_mod(100, 10);
}

void Toggle_this_100_mod_64(void) {
    test_disable_toggle_mod(100, 64);
}

void Toggle_this_100_mod_256(void) {
    test_disable_toggle_mod(100, 256);
}

void Toggle_this_100_mod_1024(void) {
    test_disable_toggle_mod(100, 1024);
}

void Toggle_this_128_mod_1(void) {
    test_disable_toggle_mod(128, 1);
}

void Toggle_this_128_mod_2(void) {
    test_disable_toggle_mod(128, 2);
}

void Toggle_this_128_mod_3(void) {
    test_disable_toggle_mod(128, 3);
}

void Toggle_this_128_mod_7(void) {
    test_disable_toggle_mod(128, 7);
}

void Toggle_this_128_mod_8(void) {
    test_disable_toggle_mod(128, 8);
}

void Toggle_this_128_mod_10(void) {
    test_disable_toggle_mod(128, 10);
}

void Toggle_this_128_mod_64(void) {
    test_disable_toggle_mod(128, 64);
}

void Toggle_this_128_mod_256(void) {
    test_disable_toggle_mod(128, 256);
}

void Toggle_this_128_mod_1024(void) {
    test_disable_toggle_mod(128, 1024);
}

void Toggle_this_200_mod_1(void) {
    test_disable_toggle_mod(200, 1);
}

void Toggle_this_200_mod_2(void) {
    test_disable_toggle_mod(200, 2);
}

void Toggle_this_200_mod_3(void) {
    test_disable_toggle_mod(200, 3);
}

void Toggle_this_200_mod_7(void) {
    test_disable_toggle_mod(200, 7);
}

void Toggle_this_200_mod_8(void) {
    test_disable_toggle_mod(200, 8);
}

void Toggle_this_200_mod_10(void) {
    test_disable_toggle_mod(200, 10);
}

void Toggle_this_200_mod_64(void) {
    test_disable_toggle_mod(200, 64);
}

void Toggle_this_200_mod_256(void) {
    test_disable_toggle_mod(200, 256);
}

void Toggle_this_200_mod_1024(void) {
    test_disable_toggle_mod(200, 1024);
}

void Toggle_this_1024_mod_1(void) {
    test_disable_toggle_mod(1024, 1);
}

void Toggle_this_1024_mod_2(void) {
    test_disable_toggle_mod(1024, 2);
}

void Toggle_this_1024_mod_3(void) {
    test_disable_toggle_mod(1024, 3);
}

void Toggle_this_1024_mod_7(void) {
    test_disable_toggle_mod(1024, 7);
}

void Toggle_this_1024_mod_8(void) {
    test_disable_toggle_mod(1024, 8);
}

void Toggle_this_1024_mod_10(void) {
    test_disable_toggle_mod(1024, 10);
}

void Toggle_this_1024_mod_64(void) {
    test_disable_toggle_mod(1024, 64);
}

void Toggle_this_1024_mod_256(void) {
    test_disable_toggle_mod(1024, 256);
}

void Toggle_this_1024_mod_1024(void) {
    test_disable_toggle_mod(1024, 1024);
}

void Toggle_this_enabled_64_mod_1(void) {
    test_enable_toggle_mod(64, 1);
}

void Toggle_this_enabled_64_mod_2(void) {
    test_enable_toggle_mod(64, 2);
}

void Toggle_this_enabled_64_mod_3(void) {
    test_enable_toggle_mod(64, 3);
}

void Toggle_this_enabled_64_mod_7(void) {
    test_enable_toggle_mod(64, 7);
}

void Toggle_this_enabled_64_mod_8(void) {
    test_enable_toggle_mod(64, 8);
}

void Toggle_this_enabled_64_mod_10(void) {
    test_enable_toggle_mod(64, 10);
}

void Toggle_this_enabled_64_mod_64(void) {
    test_enable_toggle_mod(64, 64);
}

void Toggle_this_enabled_64_mod_256(void) {
    test_enable_toggle_mod(64, 256);
}

void Toggle_this_enabled_64_mod_1024(void) {
    test_enable_toggle_mod(64, 1024);
}

void Toggle_this_enabled_100_mod_1(void) {
    test_enable_toggle_mod(100, 1);
}

void Toggle_this_enabled_100_mod_2(void) {
    test_enable_toggle_mod(100, 2);
}

void Toggle_this_enabled_100_mod_3(void) {
    test_enable_toggle_mod(100, 3);
}

void Toggle_this_enabled_100_mod_7(void) {
    test_enable_toggle_mod(100, 7);
}

void Toggle_this_enabled_100_mod_8(void) {
    test_enable_toggle_mod(100, 8);
}

void Toggle_this_enabled_100_mod_10(void) {
    test_enable_toggle_mod(100, 10);
}

void Toggle_this_enabled_100_mod_64(void) {
    test_enable_toggle_mod(100, 64);
}

void Toggle_this_enabled_100_mod_256(void) {
    test_enable_toggle_mod(100, 256);
}

void Toggle_this_enabled_100_mod_1024(void) {
    test_enable_toggle_mod(100, 1024);
}

void Toggle_this_enabled_128_mod_1(void) {
    test_enable_toggle_mod(128, 1);
}

void Toggle_this_enabled_128_mod_2(void) {
    test_enable_toggle_mod(128, 2);
}

void Toggle_this_enabled_128_mod_3(void) {
    test_enable_toggle_mod(128, 3);
}

void Toggle_this_enabled_128_mod_7(void) {
    test_enable_toggle_mod(128, 7);
}

void Toggle_this_enabled_128_mod_8(void) {
    test_enable_toggle_mod(128, 8);
}

void Toggle_this_enabled_128_mod_10(void) {
    test_enable_toggle_mod(128, 10);
}

void Toggle_this_enabled_128_mod_64(void) {
    test_enable_toggle_mod(128, 64);
}

void Toggle_this_enabled_128_mod_256(void) {
    test_enable_toggle_mod(128, 256);
}

void Toggle_this_enabled_128_mod_1024(void) {
    test_enable_toggle_mod(128, 1024);
}

void Toggle_this_enabled_200_mod_1(void) {
    test_enable_toggle_mod(200, 1);
}

void Toggle_this_enabled_200_mod_2(void) {
    test_enable_toggle_mod(200, 2);
}

void Toggle_this_enabled_200_mod_3(void) {
    test_enable_toggle_mod(200, 3);
}

void Toggle_this_enabled_200_mod_7(void) {
    test_enable_toggle_mod(200, 7);
}

void Toggle_this_enabled_200_mod_8(void) {
    test_enable_toggle_mod(200, 8);
}

void Toggle_this_enabled_200_mod_10(void) {
    test_enable_toggle_mod(200, 10);
}

void Toggle_this_enabled_200_mod_64(void) {
    test_enable_toggle_mod(200, 64);
}

void Toggle_this_enabled_200_mod_256(void) {
    test_enable_toggle_mod(200, 256);
}

void Toggle_this_enabled_200_mod_1024(void) {
    test_enable_toggle_mod(200, 1024);
}

void Toggle_this_enabled_1024_mod_1(void) {
    test_enable_toggle_mod(1024, 1);
}

void Toggle_this_enabled_1024_mod_2(void) {
    test_enable_toggle_mod(1024, 2);
}

void Toggle_this_enabled_1024_mod_3(void) {
    test_enable_toggle_mod(1024, 3);
}

void Toggle_this_enabled_1024_mod_7(void) {
    test_enable_toggle_mod(1024, 7);
}

void Toggle_this_enabled_1024_mod_8(void) {
    test_enable_toggle_mod(1024, 8);
}

void Toggle_this_enabled_1024_mod_10(void) {
    test_enable_toggle_mod(1024, 10);
}

void Toggle_this_enabled_1024_mod_64(void) {
    test_enable_toggle_mod(1024, 64);
}

void Toggle_this_enabled_1024_mod_256(void) {
    test_enable_toggle_mod(1024, 256);
}

void Toggle_this_enabled_1024_mod_1024(void) {
    test_enable_toggle_mod(1024, 1024);
}

void Toggle_this_mod_2_2_bitsets(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);

    int32_t i, total_count = 0;
    for (i = 0; i < 65536; i ++) {
        ecs_entity_t e = ecs_new_w(world, Position);
        ecs_add(world, e, Velocity);
        if (!(e % 2)) {
            ecs_enable_component(world, e, Position, true);
        } else {
            ecs_enable_component(world, e, Position, false);
        }
        
        if (!(e % 3)) {
            ecs_enable_component(world, e, Velocity, true);
        } else {
            ecs_enable_component(world, e, Velocity, false);
        }        

        if (!(e % 2) && !(e % 3)) {
            total_count ++;
        }
    }

    test_assert(total_count != 0);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, Velocity",
        .cache_kind = cache_kind
    });
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_is_enabled(world, it.entities[i], Position));
            test_assert(ecs_is_enabled(world, it.entities[i], Velocity));
            test_assert(!(it.entities[i] % 2) && !(it.entities[i] % 3));
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_mod_8_2_bitsets(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);

    int32_t i, total_count = 0;
    for (i = 0; i < 65536; i ++) {
        ecs_entity_t e = ecs_new_w(world, Position);
        ecs_add(world, e, Velocity);
        if (!(e % 8)) {
            ecs_enable_component(world, e, Position, true);
        } else {
            ecs_enable_component(world, e, Position, false);
        }
        
        if (!(e % 4)) {
            ecs_enable_component(world, e, Velocity, true);
        } else {
            ecs_enable_component(world, e, Velocity, false);
        }        

        if (!(e % 8) && !(e % 4)) {
            total_count ++;
        }
    }

    test_assert(total_count != 0);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, Velocity",
        .cache_kind = cache_kind
    });
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_is_enabled(world, it.entities[i], Position));
            test_assert(ecs_is_enabled(world, it.entities[i], Velocity));
            test_assert(!(it.entities[i] % 8) && !(it.entities[i] % 4));
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_mod_64_2_bitsets(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);

    int32_t i, total_count = 0;
    for (i = 0; i < 65536; i ++) {
        ecs_entity_t e = ecs_new_w(world, Position);
        ecs_add(world, e, Velocity);
        if (!(e % 64)) {
            ecs_enable_component(world, e, Position, true);
        } else {
            ecs_enable_component(world, e, Position, false);
        }
        
        if (!(e % 16)) {
            ecs_enable_component(world, e, Velocity, true);
        } else {
            ecs_enable_component(world, e, Velocity, false);
        }        

        if (!(e % 64) && !(e % 16)) {
            total_count ++;
        }
    }

    test_assert(total_count != 0);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, Velocity",
        .cache_kind = cache_kind
    });
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_is_enabled(world, it.entities[i], Position));
            test_assert(ecs_is_enabled(world, it.entities[i], Velocity));
            test_assert(!(it.entities[i] % 64) && !(it.entities[i] % 16));
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_mod_256_2_bitsets(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);

    int32_t i, total_count = 0;
    for (i = 0; i < 65536; i ++) {
        ecs_entity_t e = ecs_new_w(world, Position);
        ecs_add(world, e, Velocity);
        if (!(e % 256)) {
            ecs_enable_component(world, e, Position, true);
        } else {
            ecs_enable_component(world, e, Position, false);
        }
        
        if (!(e % 64)) {
            ecs_enable_component(world, e, Velocity, true);
        } else {
            ecs_enable_component(world, e, Velocity, false);
        }        

        if (!(e % 256) && !(e % 64)) {
            total_count ++;
        }
    }

    test_assert(total_count != 0);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, Velocity",
        .cache_kind = cache_kind
    });
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_is_enabled(world, it.entities[i], Position));
            test_assert(ecs_is_enabled(world, it.entities[i], Velocity));
            test_assert(!(it.entities[i] % 256) && !(it.entities[i] % 64));
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_mod_1024_2_bitsets(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);

    int32_t i, total_count = 0;
    for (i = 0; i < 65536; i ++) {
        ecs_entity_t e = ecs_new_w(world, Position);
        ecs_add(world, e, Velocity);
        if (!(e % 1024)) {
            ecs_enable_component(world, e, Position, true);
        } else {
            ecs_enable_component(world, e, Position, false);
        }
        
        if (!(e % 128)) {
            ecs_enable_component(world, e, Velocity, true);
        } else {
            ecs_enable_component(world, e, Velocity, false);
        }        

        if (!(e % 1024) && !(e % 128)) {
            total_count ++;
        }
    }

    test_assert(total_count != 0);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, Velocity",
        .cache_kind = cache_kind
    });
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_is_enabled(world, it.entities[i], Position));
            test_assert(ecs_is_enabled(world, it.entities[i], Velocity));
            test_assert(!(it.entities[i] % 1024) && !(it.entities[i] % 128));
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_randomized_2_bitsets(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);

    int32_t i, total_count = 0;
    for (i = 0; i < 65536; i ++) {
        ecs_entity_t e = ecs_new_w(world, Position);
        ecs_add(world, e, Velocity);

        bool enable_1 = rand() % 2;
        ecs_enable_component(world, e, Position, enable_1);
        bool enable_2 = rand() % 2;
        ecs_enable_component(world, e, Velocity, enable_2);
        
        if (enable_1 && enable_2) {
            total_count ++;
        }
    }

    test_assert(total_count != 0);

    for (i = 0; i < 65536; i ++) {
        ecs_new_w(world, Position); // no match
        ecs_new_w(world, Velocity); // no match

        { // match
            ecs_entity_t e = ecs_new_w(world, Position);
            ecs_add(world, e, Velocity);
            total_count ++;
        }
    }

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, Velocity",
        .cache_kind = cache_kind
    });
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_is_enabled(world, it.entities[i], Position));
            test_assert(ecs_is_enabled(world, it.entities[i], Velocity));
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_randomized_3_bitsets(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);
    ecs_add_id(world, ecs_id(Mass), EcsCanToggle);

    int32_t i, total_count = 0;
    for (i = 0; i < 65536; i ++) {
        ecs_entity_t e = ecs_new_w(world, Position);
        ecs_add(world, e, Velocity);
        ecs_add(world, e, Mass);

        bool enable_1 = rand() % 2;
        ecs_enable_component(world, e, Position, enable_1);
        bool enable_2 = rand() % 2;
        ecs_enable_component(world, e, Velocity, enable_2);
        bool enable_3 = rand() % 2;
        ecs_enable_component(world, e, Mass, enable_3);        

        if (enable_1 && enable_2 && enable_3) {
            total_count ++;
        }
    }

    test_assert(total_count != 0);

    for (i = 0; i < 65536; i ++) {
        ecs_new_w(world, Position); // no match
        ecs_new_w(world, Velocity); // no match
        ecs_new_w(world, Mass); // no match

        { // match
            ecs_entity_t e = ecs_new_w(world, Position);
            ecs_add(world, e, Velocity);
            ecs_add(world, e, Mass);
            total_count ++;
        }
    }

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, Velocity, Mass",
        .cache_kind = cache_kind
    });
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_is_enabled(world, it.entities[i], Position));
            test_assert(ecs_is_enabled(world, it.entities[i], Velocity));
            test_assert(ecs_is_enabled(world, it.entities[i], Mass));
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_randomized_4_bitsets(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);
    ecs_add_id(world, ecs_id(Mass), EcsCanToggle);
    ecs_add_id(world, ecs_id(Rotation), EcsCanToggle);

    int32_t i, total_count = 0;
    for (i = 0; i < 65536; i ++) {
        ecs_entity_t e = ecs_new_w(world, Position);
        ecs_add(world, e, Velocity);
        ecs_add(world, e, Mass);
        ecs_add(world, e, Rotation);

        bool enable_1 = rand() % 2;
        ecs_enable_component(world, e, Position, enable_1);
        bool enable_2 = rand() % 2;
        ecs_enable_component(world, e, Velocity, enable_2);
        bool enable_3 = rand() % 2;
        ecs_enable_component(world, e, Mass, enable_3);        
        bool enable_4 = rand() % 2;
        ecs_enable_component(world, e, Rotation, enable_4); 

        if (enable_1 && enable_2 && enable_3 && enable_4) {
            total_count ++;
        }
    }

    test_assert(total_count != 0);

    for (i = 0; i < 65536; i ++) {
        ecs_new_w(world, Position); // no match
        ecs_new_w(world, Velocity); // no match
        ecs_new_w(world, Mass); // no match
        ecs_new_w(world, Rotation); // no match

        { // match
            ecs_entity_t e = ecs_new_w(world, Position);
            ecs_add(world, e, Velocity);
            ecs_add(world, e, Mass);
            ecs_add(world, e, Rotation);
            total_count ++;
        }
    }


    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, Velocity, Mass, Rotation",
        .cache_kind = cache_kind
    });
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_is_enabled(world, it.entities[i], Position));
            test_assert(ecs_is_enabled(world, it.entities[i], Velocity));
            test_assert(ecs_is_enabled(world, it.entities[i], Mass));
            test_assert(ecs_is_enabled(world, it.entities[i], Rotation));
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_w_other_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    int32_t i, total_count = 0;
    for (i = 0; i < 1024; i ++) {
        ecs_entity_t e = ecs_new_w(world, Position);
        ecs_set(world, e, Position, {e, e});

        if (!(e % 3)) {
            ecs_enable_component(world, e, Position, false);
            if (!(e % 5)) {
                ecs_add(world, e, Tag);
            }
        } else {
            ecs_enable_component(world, e, Position, true);
            if (!(e % 5)) {
                ecs_add(world, e, Tag);
                total_count ++;
            }
        }
    }

    test_assert(total_count != 0);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, Tag",
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(Tag, ecs_field_id(&it, 1));
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] % 3);
            test_assert(!(it.entities[i] % 5));
            test_assert(ecs_is_enabled(world, it.entities[i], Position));
            test_int(p[i].x, it.entities[i]);
            test_int(p[i].y, it.entities[i]);
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_w_other_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    int32_t i, total_count = 0;
    for (i = 0; i < 1024; i ++) {
        ecs_entity_t e = ecs_new_w(world, Position);
        ecs_set(world, e, Position, {e, e});

        if (!(e % 3)) {
            ecs_enable_component(world, e, Position, false);
            if (!(e % 5)) {
                ecs_set(world, e, Velocity, {e * 2, e * 2});
            }
        } else {
            ecs_enable_component(world, e, Position, true);
            if (!(e % 5)) {
                ecs_set(world, e, Velocity, {e * 2, e * 2});
                total_count ++;
            }
        }
    }

    test_assert(total_count != 0);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, Velocity",
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        Velocity *v = ecs_field(&it, Velocity, 1);
        test_assert(v != NULL);

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] % 3);
            test_assert(!(it.entities[i] % 5));
            test_assert(ecs_is_enabled(world, it.entities[i], Position));
            test_int(p[i].x, it.entities[i]);
            test_int(p[i].y, it.entities[i]);
            test_int(v[i].x, it.entities[i] * 2);
            test_int(v[i].y, it.entities[i] * 2);
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    for (int i = 0; i < 1024; i ++) {
        ecs_entity_t e = ecs_new_w(world, Position);
        ecs_set(world, e, Position, {e, e});

        if (!(e % 3)) {
            ecs_enable_component(world, e, Position, false);
        } else {
            ecs_enable_component(world, e, Position, true);
        }
    }

    ecs_query_t *q = ecs_query(world, {
        .expr = "!Position",
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    int32_t count = 0, pos_count = 0;

    while (ecs_query_next(&it)) {
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        test_bool(false, ecs_field_is_set(&it, 0));

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            if (ecs_has(world, it.entities[i], Position)) {
                pos_count ++;
            }

            test_assert(!ecs_is_enabled(
                world, it.entities[i], Position));
        }

        count += it.count;
    }

    test_assert(count != 0);
    test_assert(pos_count != 0);

    ecs_query_fini(q);

    ecs_fini(world);
}

static
void this_written_not(int total, int mod) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    int i, total_without_pos = 0;
    for (i = 0; i < total; i ++) {
        ecs_entity_t e = ecs_new_w(world, Position);
        ecs_set(world, e, Position, {e, e});

        if (!(e % mod)) {
            ecs_enable_component(world, e, Position, false);
            total_without_pos ++;
        } else {
            ecs_enable_component(world, e, Position, true);
        }

        ecs_add(world, e, Tag);
    }

    for (int i = 0; i < total; i ++) {
        ecs_new_w(world, Tag);
        total_without_pos ++;
    }

    for (int i = 0; i < total; i ++) {
        ecs_entity_t e = ecs_new_w(world, Position);
        ecs_set(world, e, Position, {e, e});
        ecs_add(world, e, Tag);
    }

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, !Position",
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    int32_t count = 0, pos_count = 0;

    while (ecs_query_next(&it)) {
        test_uint(Tag, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            if (ecs_has(world, it.entities[i], Position)) {
                pos_count ++;
                test_assert(!(it.entities[i] % mod));
            }

            test_assert(!ecs_is_enabled(
                world, it.entities[i], Position));
        }

        count += it.count;
    }

    test_assert(count != 0);
    test_assert(pos_count != 0);
    test_int(count, total_without_pos);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_written_not_1024_mod_2(void) {
    this_written_not(1024, 2);
}

void Toggle_this_written_not_1024_mod_3(void) {
    this_written_not(1024, 3);
}

void Toggle_this_written_not_1024_mod_7(void) {
    this_written_not(1024, 7);
}

void Toggle_this_written_not_1024_mod_8(void) {
    this_written_not(1024, 8);
}

void Toggle_this_written_not_1024_mod_10(void) {
    this_written_not(1024, 10);
}

void Toggle_this_written_not_1024_mod_64(void) {
    this_written_not(1024, 64);
}

void Toggle_this_written_not_1024_mod_256(void) {
    this_written_not(1024, 256);
}

void Toggle_this_written_not_1024_mod_1024(void) {
    this_written_not(1024, 1024);
}

static
void this_optional(int total, int mod) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    int i, total_with_pos = 0;
    for (i = 0; i < total; i ++) {
        ecs_entity_t e = ecs_new_w(world, Position);
        ecs_set(world, e, Position, {e, e});

        if (!(e % mod)) {
            ecs_enable_component(world, e, Position, true);
            total_with_pos ++;
        } else {
            ecs_enable_component(world, e, Position, false);
        }
    }

    for (int i = 0; i < total; i ++) {
        ecs_entity_t e = ecs_new_w(world, Position);
        ecs_set(world, e, Position, {e, e});
        total_with_pos ++;
    }

    ecs_query_t *q = ecs_query(world, {
        .expr = "?Position",
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    int32_t count = 0, with_pos_count = 0;

    while (ecs_query_next(&it)) {
        test_uint(ecs_id(Position), ecs_field_id(&it, 0));
        Position *ptr = ecs_field(&it, Position, 0);

        if (ecs_table_has_id(world, it.table, ECS_TOGGLE | ecs_id(Position))) {
            for (int32_t i = 0; i < it.count; i ++) {
                if (ecs_is_enabled(world, it.entities[i], Position)) {
                    test_bool(true, ecs_field_is_set(&it, 0));
                    test_assert(ptr != NULL);
                    test_assert(ptr[i].x == it.entities[i]);
                    test_assert(ptr[i].y == it.entities[i]);
                    with_pos_count ++;
                } else {
                    test_bool(false, ecs_field_is_set(&it, 0));
                }
            }
        } else {
            if (ecs_table_has_id(world, it.table, ecs_id(Position))) {
                test_bool(true, ecs_field_is_set(&it, 0));
                test_assert(ptr != NULL);
                for (int32_t i = 0; i < it.count; i ++) {
                    test_assert(ecs_is_enabled(world, it.entities[i], Position));
                    test_assert(ptr[i].x == it.entities[i]);
                    test_assert(ptr[i].y == it.entities[i]);
                    with_pos_count ++;
                }
            } else {
                test_bool(false, ecs_field_is_set(&it, 0));
                for (int32_t i = 0; i < it.count; i ++) {
                    test_assert(!ecs_is_enabled(world, it.entities[i], Position));
                }
            }
        }

        count += it.count;
    }

    test_assert(count != 0);
    test_int(with_pos_count, total_with_pos);

    ecs_query_fini(q);

    ecs_fini(world);
}

static
void this_written_optional(int total, int mod) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    int i, total_with_pos = 0, total_without_pos = 0;
    for (i = 0; i < total; i ++) {
        ecs_entity_t e = ecs_new_w(world, Position);
        ecs_set(world, e, Position, {e, e});

        if (!(e % mod)) {
            ecs_enable_component(world, e, Position, true);
            total_with_pos ++;
        } else {
            ecs_enable_component(world, e, Position, false);
            total_without_pos ++;
        }

        ecs_add(world, e, Tag);
    }

    for (int i = 0; i < total; i ++) {
        ecs_new_w(world, Tag);
        total_without_pos ++;
    }

    for (int i = 0; i < total; i ++) {
        ecs_entity_t e = ecs_new_w(world, Position);
        ecs_set(world, e, Position, {e, e});
        ecs_add(world, e, Tag);
        total_with_pos ++;
    }

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, ?Position",
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    int32_t count = 0, with_pos_count = 0, without_pos_count = 0;

    while (ecs_query_next(&it)) {
        test_uint(Tag, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        Position *ptr = ecs_field(&it, Position, 1);

        if (ecs_table_has_id(world, it.table, ECS_TOGGLE | ecs_id(Position))) {
            for (int32_t i = 0; i < it.count; i ++) {
                if (ecs_is_enabled(world, it.entities[i], Position)) {
                    test_bool(true, ecs_field_is_set(&it, 1));
                    test_assert(ptr != NULL);
                    test_assert(ptr[i].x == it.entities[i]);
                    test_assert(ptr[i].y == it.entities[i]);
                    with_pos_count ++;
                } else {
                    test_bool(false, ecs_field_is_set(&it, 1));
                    without_pos_count ++;
                }
            }
        } else {
            if (ecs_table_has_id(world, it.table, ecs_id(Position))) {
                test_bool(true, ecs_field_is_set(&it, 1));
                test_assert(ptr != NULL);
                for (int32_t i = 0; i < it.count; i ++) {
                    test_assert(ecs_is_enabled(world, it.entities[i], Position));
                    test_assert(ptr[i].x == it.entities[i]);
                    test_assert(ptr[i].y == it.entities[i]);
                    with_pos_count ++;
                }
            } else {
                test_bool(false, ecs_field_is_set(&it, 1));
                for (int32_t i = 0; i < it.count; i ++) {
                    test_assert(!ecs_is_enabled(world, it.entities[i], Position));
                    without_pos_count ++;
                }
            }
        }

        count += it.count;
    }

    test_assert(count != 0);
    test_int(count, (with_pos_count + without_pos_count));
    test_int(with_pos_count, total_with_pos);
    test_int(without_pos_count, total_without_pos);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_optional(void) {
    this_optional(1024, 2);
}

void Toggle_this_written_optional_1024_mod_2(void) {
    this_written_optional(1024, 2);
}

void Toggle_this_written_optional_1024_mod_3(void) {
    this_written_optional(1024, 3);
}

void Toggle_this_written_optional_1024_mod_7(void) {
    this_written_optional(1024, 7);
}

void Toggle_this_written_optional_1024_mod_8(void) {
    this_written_optional(1024, 8);
}

void Toggle_this_written_optional_1024_mod_10(void) {
    this_written_optional(1024, 10);
}

void Toggle_this_written_optional_1024_mod_64(void) {
    this_written_optional(1024, 64);
}

void Toggle_this_written_optional_1024_mod_256(void) {
    this_written_optional(1024, 256);
}

void Toggle_this_written_optional_1024_mod_1024(void) {
    this_written_optional(1024, 1024);
}

static
int compare_position(ecs_entity_t e1, const void *ptr1, ecs_entity_t e2, const void *ptr2) {
    const Position *p1 = ptr1;
    const Position *p2 = ptr2;
    return (p1->x > p2->x) - (p1->x < p2->x);
}

void Toggle_this_sort(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {3, 2}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {2, 2}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {1, 2}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Position, {0, 2}));

    ecs_enable_component(world, e1, Position, true);
    ecs_enable_component(world, e2, Position, true);
    ecs_enable_component(world, e3, Position, false);
    ecs_enable_component(world, e4, Position, false);

    test_bool(ecs_is_enabled(world, e1, Position), true);
    test_bool(ecs_is_enabled(world, e2, Position), true);
    test_bool(ecs_is_enabled(world, e3, Position), false);
    test_bool(ecs_is_enabled(world, e4, Position), false);
    
    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .order_by = ecs_id(Position),
        .order_by_callback = compare_position,
        .cache_kind = cache_kind
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 2);
    test_assert(it.entities[0] == e2);
    test_assert(it.entities[1] == e1);
    test_assert(!ecs_query_next(&it));

    /* Entities will have shuffled around, ensure bits got shuffled too */
    test_bool(ecs_is_enabled(world, e1, Position), true);
    test_bool(ecs_is_enabled(world, e2, Position), true);
    test_bool(ecs_is_enabled(world, e3, Position), false);
    test_bool(ecs_is_enabled(world, e4, Position), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_table_move_2_from_3(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ecs_query_t *q = ecs_query(world, { 
        .expr = "Position",
        .cache_kind = cache_kind
    });

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {30, 40}));

    ecs_enable_component(world, e1, Position, true);
    ecs_enable_component(world, e2, Position, false);
    ecs_enable_component(world, e3, Position, true);

    test_bool(ecs_is_enabled(world, e1, Position), true);
    test_bool(ecs_is_enabled(world, e2, Position), false);
    test_bool(ecs_is_enabled(world, e3, Position), true);

    ecs_add(world, e3, Tag);
    ecs_add(world, e2, Tag);
    
    test_bool(ecs_is_enabled(world, e1, Position), true);
    test_bool(ecs_is_enabled(world, e2, Position), false);
    test_bool(ecs_is_enabled(world, e3, Position), true);

    ecs_iter_t it = ecs_query_iter(world, q);
    {
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 10); test_int(p->y, 20);
    }
    {
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p->x, 30); test_int(p->y, 40);
    }
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_written_toggle_w_not_toggle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);

    /* No toggles */
    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_add(world, e1, Position);

    /* No components */
    /* ecs_entity_t e2 = */ ecs_new_w(world, Tag);

    /* Components, No toggles */
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Velocity);

    /* Toggle Position == true */
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_add(world, e4, Position);
    ecs_enable_component(world, e4, Position, true);

    /* Toggle Position == false */
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_add(world, e5, Position);
    ecs_enable_component(world, e5, Position, false);

    /* Toggle Velocity == true */
    ecs_entity_t e6 = ecs_new_w(world, Tag);
    ecs_add(world, e6, Position);
    ecs_add(world, e6, Velocity);
    ecs_enable_component(world, e6, Velocity, true);

    /* Toggle Velocity == false */
    ecs_entity_t e7 = ecs_new_w(world, Tag);
    ecs_add(world, e7, Position);
    ecs_add(world, e7, Velocity);
    ecs_enable_component(world, e7, Velocity, false);

    /* Toggle Position == true, Velocity == true */
    ecs_entity_t e8 = ecs_new_w(world, Tag);
    ecs_add(world, e8, Position);
    ecs_add(world, e8, Velocity);
    ecs_enable_component(world, e8, Position, true);
    ecs_enable_component(world, e8, Velocity, true);

    /* Toggle Position == true, Velocity == false */
    ecs_entity_t e9 = ecs_new_w(world, Tag);
    ecs_add(world, e9, Position);
    ecs_add(world, e9, Velocity);
    ecs_enable_component(world, e9, Position, true);
    ecs_enable_component(world, e9, Velocity, false);

    /* Toggle Position == false, Velocity == true */
    ecs_entity_t e10 = ecs_new_w(world, Tag);
    ecs_add(world, e10, Position);
    ecs_add(world, e10, Velocity);
    ecs_enable_component(world, e10, Position, false);
    ecs_enable_component(world, e10, Velocity, true);

    /* Toggle Position == false, Velocity == false */
    ecs_entity_t e11 = ecs_new_w(world, Tag);
    ecs_add(world, e11, Position);
    ecs_add(world, e11, Velocity);
    ecs_enable_component(world, e11, Position, false);
    ecs_enable_component(world, e11, Velocity, false);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, Position, !Velocity",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e9, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(false, ecs_iter_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_written_not_toggle_w_toggle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);

    /* No toggles */
    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_add(world, e1, Position);

    /* No components */
    /* ecs_entity_t e2 = */ ecs_new_w(world, Tag);

    /* Components, No toggles */
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Velocity);

    /* Toggle Position == true */
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_add(world, e4, Position);
    ecs_enable_component(world, e4, Position, true);

    /* Toggle Position == false */
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_add(world, e5, Position);
    ecs_enable_component(world, e5, Position, false);

    /* Toggle Velocity == true */
    ecs_entity_t e6 = ecs_new_w(world, Tag);
    ecs_add(world, e6, Position);
    ecs_add(world, e6, Velocity);
    ecs_enable_component(world, e6, Velocity, true);

    /* Toggle Velocity == false */
    ecs_entity_t e7 = ecs_new_w(world, Tag);
    ecs_add(world, e7, Position);
    ecs_add(world, e7, Velocity);
    ecs_enable_component(world, e7, Velocity, false);

    /* Toggle Position == true, Velocity == true */
    ecs_entity_t e8 = ecs_new_w(world, Tag);
    ecs_add(world, e8, Position);
    ecs_add(world, e8, Velocity);
    ecs_enable_component(world, e8, Position, true);
    ecs_enable_component(world, e8, Velocity, true);

    /* Toggle Position == true, Velocity == false */
    ecs_entity_t e9 = ecs_new_w(world, Tag);
    ecs_add(world, e9, Position);
    ecs_add(world, e9, Velocity);
    ecs_enable_component(world, e9, Position, true);
    ecs_enable_component(world, e9, Velocity, false);

    /* Toggle Position == false, Velocity == true */
    ecs_entity_t e10 = ecs_new_w(world, Tag);
    ecs_add(world, e10, Position);
    ecs_add(world, e10, Velocity);
    ecs_enable_component(world, e10, Position, false);
    ecs_enable_component(world, e10, Velocity, true);

    /* Toggle Position == false, Velocity == false */
    ecs_entity_t e11 = ecs_new_w(world, Tag);
    ecs_add(world, e11, Position);
    ecs_add(world, e11, Velocity);
    ecs_enable_component(world, e11, Position, false);
    ecs_enable_component(world, e11, Velocity, false);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, !Velocity, Position",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e9, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(false, ecs_iter_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_written_toggle_w_optional_toggle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);

    /* No toggles */
    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_add(world, e1, Position);

    /* No components */
    /* ecs_entity_t e2 = */ ecs_new_w(world, Tag);

    /* Components, No toggles */
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Velocity);

    /* Toggle Position == true */
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_add(world, e4, Position);
    ecs_enable_component(world, e4, Position, true);

    /* Toggle Position == false */
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_add(world, e5, Position);
    ecs_enable_component(world, e5, Position, false);

    /* Toggle Velocity == true */
    ecs_entity_t e6 = ecs_new_w(world, Tag);
    ecs_add(world, e6, Position);
    ecs_add(world, e6, Velocity);
    ecs_enable_component(world, e6, Velocity, true);

    /* Toggle Velocity == false */
    ecs_entity_t e7 = ecs_new_w(world, Tag);
    ecs_add(world, e7, Position);
    ecs_add(world, e7, Velocity);
    ecs_enable_component(world, e7, Velocity, false);

    /* Toggle Position == true, Velocity == true */
    ecs_entity_t e8 = ecs_new_w(world, Tag);
    ecs_add(world, e8, Position);
    ecs_add(world, e8, Velocity);
    ecs_enable_component(world, e8, Position, true);
    ecs_enable_component(world, e8, Velocity, true);

    /* Toggle Position == true, Velocity == false */
    ecs_entity_t e9 = ecs_new_w(world, Tag);
    ecs_add(world, e9, Position);
    ecs_add(world, e9, Velocity);
    ecs_enable_component(world, e9, Position, true);
    ecs_enable_component(world, e9, Velocity, false);

    /* Toggle Position == false, Velocity == true */
    ecs_entity_t e10 = ecs_new_w(world, Tag);
    ecs_add(world, e10, Position);
    ecs_add(world, e10, Velocity);
    ecs_enable_component(world, e10, Position, false);
    ecs_enable_component(world, e10, Velocity, true);

    /* Toggle Position == false, Velocity == false */
    ecs_entity_t e11 = ecs_new_w(world, Tag);
    ecs_add(world, e11, Position);
    ecs_add(world, e11, Velocity);
    ecs_enable_component(world, e11, Position, false);
    ecs_enable_component(world, e11, Velocity, false);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, Position, ?Velocity",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e8, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e9, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(false, ecs_iter_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_written_optional_toggle_w_toggle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);

    /* No toggles */
    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_add(world, e1, Position);

    /* No components */
    /* ecs_entity_t e2 = */ ecs_new_w(world, Tag);

    /* Components, No toggles */
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Velocity);

    /* Toggle Position == true */
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_add(world, e4, Position);
    ecs_enable_component(world, e4, Position, true);

    /* Toggle Position == false */
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_add(world, e5, Position);
    ecs_enable_component(world, e5, Position, false);

    /* Toggle Velocity == true */
    ecs_entity_t e6 = ecs_new_w(world, Tag);
    ecs_add(world, e6, Position);
    ecs_add(world, e6, Velocity);
    ecs_enable_component(world, e6, Velocity, true);

    /* Toggle Velocity == false */
    ecs_entity_t e7 = ecs_new_w(world, Tag);
    ecs_add(world, e7, Position);
    ecs_add(world, e7, Velocity);
    ecs_enable_component(world, e7, Velocity, false);

    /* Toggle Position == true, Velocity == true */
    ecs_entity_t e8 = ecs_new_w(world, Tag);
    ecs_add(world, e8, Position);
    ecs_add(world, e8, Velocity);
    ecs_enable_component(world, e8, Position, true);
    ecs_enable_component(world, e8, Velocity, true);

    /* Toggle Position == true, Velocity == false */
    ecs_entity_t e9 = ecs_new_w(world, Tag);
    ecs_add(world, e9, Position);
    ecs_add(world, e9, Velocity);
    ecs_enable_component(world, e9, Position, true);
    ecs_enable_component(world, e9, Velocity, false);

    /* Toggle Position == false, Velocity == true */
    ecs_entity_t e10 = ecs_new_w(world, Tag);
    ecs_add(world, e10, Position);
    ecs_add(world, e10, Velocity);
    ecs_enable_component(world, e10, Position, false);
    ecs_enable_component(world, e10, Velocity, true);

    /* Toggle Position == false, Velocity == false */
    ecs_entity_t e11 = ecs_new_w(world, Tag);
    ecs_add(world, e11, Position);
    ecs_add(world, e11, Velocity);
    ecs_enable_component(world, e11, Position, false);
    ecs_enable_component(world, e11, Velocity, false);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, ?Velocity, Position",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e8, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e9, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(false, ecs_iter_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_written_not_w_optional_toggle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);

    /* No toggles */
    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_add(world, e1, Position);

    /* No components */
    ecs_entity_t e2 = ecs_new_w(world, Tag);

    /* Components, No toggles */
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Velocity);

    /* Toggle Position == true */
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_add(world, e4, Position);
    ecs_enable_component(world, e4, Position, true);

    /* Toggle Position == false */
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_add(world, e5, Position);
    ecs_enable_component(world, e5, Position, false);

    /* Toggle Velocity == true */
    ecs_entity_t e6 = ecs_new_w(world, Tag);
    ecs_add(world, e6, Position);
    ecs_add(world, e6, Velocity);
    ecs_enable_component(world, e6, Velocity, true);

    /* Toggle Velocity == false */
    ecs_entity_t e7 = ecs_new_w(world, Tag);
    ecs_add(world, e7, Position);
    ecs_add(world, e7, Velocity);
    ecs_enable_component(world, e7, Velocity, false);

    /* Toggle Position == true, Velocity == true */
    ecs_entity_t e8 = ecs_new_w(world, Tag);
    ecs_add(world, e8, Position);
    ecs_add(world, e8, Velocity);
    ecs_enable_component(world, e8, Position, true);
    ecs_enable_component(world, e8, Velocity, true);

    /* Toggle Position == true, Velocity == false */
    ecs_entity_t e9 = ecs_new_w(world, Tag);
    ecs_add(world, e9, Position);
    ecs_add(world, e9, Velocity);
    ecs_enable_component(world, e9, Position, true);
    ecs_enable_component(world, e9, Velocity, false);

    /* Toggle Position == false, Velocity == true */
    ecs_entity_t e10 = ecs_new_w(world, Tag);
    ecs_add(world, e10, Position);
    ecs_add(world, e10, Velocity);
    ecs_enable_component(world, e10, Position, false);
    ecs_enable_component(world, e10, Velocity, true);

    /* Toggle Position == false, Velocity == false */
    ecs_entity_t e11 = ecs_new_w(world, Tag);
    ecs_add(world, e11, Position);
    ecs_add(world, e11, Velocity);
    ecs_enable_component(world, e11, Position, false);
    ecs_enable_component(world, e11, Velocity, false);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, !Velocity, ?Position",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e9, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e11, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(false, ecs_iter_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_written_optional_w_not_toggle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);

    /* No toggles */
    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_add(world, e1, Position);

    /* No components */
    ecs_entity_t e2 = ecs_new_w(world, Tag);

    /* Components, No toggles */
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Velocity);

    /* Toggle Position == true */
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_add(world, e4, Position);
    ecs_enable_component(world, e4, Position, true);

    /* Toggle Position == false */
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_add(world, e5, Position);
    ecs_enable_component(world, e5, Position, false);

    /* Toggle Velocity == true */
    ecs_entity_t e6 = ecs_new_w(world, Tag);
    ecs_add(world, e6, Position);
    ecs_add(world, e6, Velocity);
    ecs_enable_component(world, e6, Velocity, true);

    /* Toggle Velocity == false */
    ecs_entity_t e7 = ecs_new_w(world, Tag);
    ecs_add(world, e7, Position);
    ecs_add(world, e7, Velocity);
    ecs_enable_component(world, e7, Velocity, false);

    /* Toggle Position == true, Velocity == true */
    ecs_entity_t e8 = ecs_new_w(world, Tag);
    ecs_add(world, e8, Position);
    ecs_add(world, e8, Velocity);
    ecs_enable_component(world, e8, Position, true);
    ecs_enable_component(world, e8, Velocity, true);

    /* Toggle Position == true, Velocity == false */
    ecs_entity_t e9 = ecs_new_w(world, Tag);
    ecs_add(world, e9, Position);
    ecs_add(world, e9, Velocity);
    ecs_enable_component(world, e9, Position, true);
    ecs_enable_component(world, e9, Velocity, false);

    /* Toggle Position == false, Velocity == true */
    ecs_entity_t e10 = ecs_new_w(world, Tag);
    ecs_add(world, e10, Position);
    ecs_add(world, e10, Velocity);
    ecs_enable_component(world, e10, Position, false);
    ecs_enable_component(world, e10, Velocity, true);

    /* Toggle Position == false, Velocity == false */
    ecs_entity_t e11 = ecs_new_w(world, Tag);
    ecs_add(world, e11, Position);
    ecs_add(world, e11, Velocity);
    ecs_enable_component(world, e11, Position, false);
    ecs_enable_component(world, e11, Velocity, false);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, ?Position, !Velocity",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e9, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e11, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(false, ecs_iter_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_written_2_not_toggle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);

    /* No toggles */
    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_add(world, e1, Position);

    /* No components */
    ecs_entity_t e2 = ecs_new_w(world, Tag);

    /* Components, No toggles */
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Velocity);

    /* Toggle Position == true */
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_add(world, e4, Position);
    ecs_enable_component(world, e4, Position, true);

    /* Toggle Position == false */
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_add(world, e5, Position);
    ecs_enable_component(world, e5, Position, false);

    /* Toggle Velocity == true */
    ecs_entity_t e6 = ecs_new_w(world, Tag);
    ecs_add(world, e6, Position);
    ecs_add(world, e6, Velocity);
    ecs_enable_component(world, e6, Velocity, true);

    /* Toggle Velocity == false */
    ecs_entity_t e7 = ecs_new_w(world, Tag);
    ecs_add(world, e7, Position);
    ecs_add(world, e7, Velocity);
    ecs_enable_component(world, e7, Velocity, false);

    /* Toggle Position == true, Velocity == true */
    ecs_entity_t e8 = ecs_new_w(world, Tag);
    ecs_add(world, e8, Position);
    ecs_add(world, e8, Velocity);
    ecs_enable_component(world, e8, Position, true);
    ecs_enable_component(world, e8, Velocity, true);

    /* Toggle Position == true, Velocity == false */
    ecs_entity_t e9 = ecs_new_w(world, Tag);
    ecs_add(world, e9, Position);
    ecs_add(world, e9, Velocity);
    ecs_enable_component(world, e9, Position, true);
    ecs_enable_component(world, e9, Velocity, false);

    /* Toggle Position == false, Velocity == true */
    ecs_entity_t e10 = ecs_new_w(world, Tag);
    ecs_add(world, e10, Position);
    ecs_add(world, e10, Velocity);
    ecs_enable_component(world, e10, Position, false);
    ecs_enable_component(world, e10, Velocity, true);

    /* Toggle Position == false, Velocity == false */
    ecs_entity_t e11 = ecs_new_w(world, Tag);
    ecs_add(world, e11, Position);
    ecs_add(world, e11, Velocity);
    ecs_enable_component(world, e11, Position, false);
    ecs_enable_component(world, e11, Velocity, false);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, !Position, !Velocity",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e11, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(false, ecs_iter_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_written_2_optional_toggle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);

    /* No toggles */
    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_add(world, e1, Position);

    /* No components */
    ecs_entity_t e2 = ecs_new_w(world, Tag);

    /* Components, No toggles */
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Velocity);

    /* Toggle Position == true */
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_add(world, e4, Position);
    ecs_enable_component(world, e4, Position, true);

    /* Toggle Position == false */
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_add(world, e5, Position);
    ecs_enable_component(world, e5, Position, false);

    /* Toggle Velocity == true */
    ecs_entity_t e6 = ecs_new_w(world, Tag);
    ecs_add(world, e6, Position);
    ecs_add(world, e6, Velocity);
    ecs_enable_component(world, e6, Velocity, true);

    /* Toggle Velocity == false */
    ecs_entity_t e7 = ecs_new_w(world, Tag);
    ecs_add(world, e7, Position);
    ecs_add(world, e7, Velocity);
    ecs_enable_component(world, e7, Velocity, false);

    /* Toggle Position == true, Velocity == true */
    ecs_entity_t e8 = ecs_new_w(world, Tag);
    ecs_add(world, e8, Position);
    ecs_add(world, e8, Velocity);
    ecs_enable_component(world, e8, Position, true);
    ecs_enable_component(world, e8, Velocity, true);

    /* Toggle Position == true, Velocity == false */
    ecs_entity_t e9 = ecs_new_w(world, Tag);
    ecs_add(world, e9, Position);
    ecs_add(world, e9, Velocity);
    ecs_enable_component(world, e9, Position, true);
    ecs_enable_component(world, e9, Velocity, false);

    /* Toggle Position == false, Velocity == true */
    ecs_entity_t e10 = ecs_new_w(world, Tag);
    ecs_add(world, e10, Position);
    ecs_add(world, e10, Velocity);
    ecs_enable_component(world, e10, Position, false);
    ecs_enable_component(world, e10, Velocity, true);

    /* Toggle Position == false, Velocity == false */
    ecs_entity_t e11 = ecs_new_w(world, Tag);
    ecs_add(world, e11, Position);
    ecs_add(world, e11, Velocity);
    ecs_enable_component(world, e11, Position, false);
    ecs_enable_component(world, e11, Velocity, false);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, ?Position, ?Velocity",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e8, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e9, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e10, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e11, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(false, ecs_iter_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_written_toggle_w_2_not_toggle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);
    ecs_add_id(world, ecs_id(Mass), EcsCanToggle);

    /* No toggles */
    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_add(world, e1, Position);

    /* No components */
    /* ecs_entity_t e2 = */ ecs_new_w(world, Tag);

    /* Components, No toggles */
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Velocity);
    ecs_add(world, e3, Mass);

    /* Toggle Position == true */
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_add(world, e4, Position);
    ecs_enable_component(world, e4, Position, true);

    /* Toggle Position == false */
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_add(world, e5, Position);
    ecs_enable_component(world, e5, Position, false);

    /* Toggle Velocity == true, Mass = true */
    ecs_entity_t e6 = ecs_new_w(world, Tag);
    ecs_add(world, e6, Position);
    ecs_add(world, e6, Velocity);
    ecs_enable_component(world, e6, Velocity, true);
    ecs_add(world, e6, Mass);
    ecs_enable_component(world, e6, Mass, true);

    /* Toggle Velocity == false, Mass == false */
    ecs_entity_t e7 = ecs_new_w(world, Tag);
    ecs_add(world, e7, Position);
    ecs_add(world, e7, Velocity);
    ecs_enable_component(world, e7, Velocity, false);
    ecs_add(world, e7, Mass);
    ecs_enable_component(world, e7, Mass, false);

    /* Toggle Position == true, Velocity == true, Mass == true */
    ecs_entity_t e8 = ecs_new_w(world, Tag);
    ecs_add(world, e8, Position);
    ecs_add(world, e8, Velocity);
    ecs_add(world, e8, Mass);
    ecs_enable_component(world, e8, Position, true);
    ecs_enable_component(world, e8, Velocity, true);
    ecs_enable_component(world, e8, Mass, true);

    /* Toggle Position == true, Velocity == false, Mass == false */
    ecs_entity_t e9 = ecs_new_w(world, Tag);
    ecs_add(world, e9, Position);
    ecs_add(world, e9, Velocity);
    ecs_add(world, e9, Mass);
    ecs_enable_component(world, e9, Position, true);
    ecs_enable_component(world, e9, Velocity, false);
    ecs_enable_component(world, e9, Mass, false);

    /* Toggle Position == false, Velocity == true, Mass == true */
    ecs_entity_t e10 = ecs_new_w(world, Tag);
    ecs_add(world, e10, Position);
    ecs_add(world, e10, Velocity);
    ecs_add(world, e10, Mass);
    ecs_enable_component(world, e10, Position, false);
    ecs_enable_component(world, e10, Velocity, true);
    ecs_enable_component(world, e10, Mass, true);

    /* Toggle Position == false, Velocity == false, Mass == false */
    ecs_entity_t e11 = ecs_new_w(world, Tag);
    ecs_add(world, e11, Position);
    ecs_add(world, e11, Velocity);
    ecs_add(world, e11, Mass);
    ecs_enable_component(world, e11, Position, false);
    ecs_enable_component(world, e11, Velocity, false);
    ecs_enable_component(world, e11, Mass, false);

    /* Toggle Position == true, Velocity == true, Mass == true */
    ecs_entity_t e12 = ecs_new_w(world, Tag);
    ecs_add(world, e12, Position);
    ecs_add(world, e12, Velocity);
    ecs_add(world, e12, Mass);
    ecs_enable_component(world, e12, Position, true);
    ecs_enable_component(world, e12, Velocity, true);
    ecs_enable_component(world, e12, Mass, false);

    /* Toggle Position == true, Velocity == false, Mass == true */
    ecs_entity_t e13 = ecs_new_w(world, Tag);
    ecs_add(world, e13, Position);
    ecs_add(world, e13, Velocity);
    ecs_add(world, e13, Mass);
    ecs_enable_component(world, e13, Position, true);
    ecs_enable_component(world, e13, Velocity, false);
    ecs_enable_component(world, e13, Mass, true);

    /* Toggle Position == false, Velocity == true, Mass == false */
    ecs_entity_t e14 = ecs_new_w(world, Tag);
    ecs_add(world, e14, Position);
    ecs_add(world, e14, Velocity);
    ecs_add(world, e14, Mass);
    ecs_enable_component(world, e14, Position, false);
    ecs_enable_component(world, e14, Velocity, true);
    ecs_enable_component(world, e14, Mass, false);

    /* Toggle Position == false, Velocity == false, Mass == true */
    ecs_entity_t e15 = ecs_new_w(world, Tag);
    ecs_add(world, e15, Position);
    ecs_add(world, e15, Velocity);
    ecs_add(world, e15, Mass);
    ecs_enable_component(world, e15, Position, false);
    ecs_enable_component(world, e15, Velocity, false);
    ecs_enable_component(world, e15, Mass, true);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, Position, !Velocity, !Mass",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 3));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));
    test_bool(false, ecs_field_is_set(&it, 3));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 3));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));
    test_bool(false, ecs_field_is_set(&it, 3));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 3));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));
    test_bool(false, ecs_field_is_set(&it, 3));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e9, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 3));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));
    test_bool(false, ecs_field_is_set(&it, 3));

    test_bool(false, ecs_iter_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_written_toggle_w_2_optional_toggle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);
    ecs_add_id(world, ecs_id(Mass), EcsCanToggle);

    /* No toggles */
    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_add(world, e1, Position);

    /* No components */
    /* ecs_entity_t e2 = */ ecs_new_w(world, Tag);

    /* Components, No toggles */
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Velocity);
    ecs_add(world, e3, Mass);

    /* Toggle Position == true */
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_add(world, e4, Position);
    ecs_enable_component(world, e4, Position, true);

    /* Toggle Position == false */
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_add(world, e5, Position);
    ecs_enable_component(world, e5, Position, false);

    /* Toggle Velocity == true, Mass = true */
    ecs_entity_t e6 = ecs_new_w(world, Tag);
    ecs_add(world, e6, Position);
    ecs_add(world, e6, Velocity);
    ecs_enable_component(world, e6, Velocity, true);
    ecs_add(world, e6, Mass);
    ecs_enable_component(world, e6, Mass, true);

    /* Toggle Velocity == false, Mass == false */
    ecs_entity_t e7 = ecs_new_w(world, Tag);
    ecs_add(world, e7, Position);
    ecs_add(world, e7, Velocity);
    ecs_enable_component(world, e7, Velocity, false);
    ecs_add(world, e7, Mass);
    ecs_enable_component(world, e7, Mass, false);

    /* Toggle Position == true, Velocity == true, Mass == true */
    ecs_entity_t e8 = ecs_new_w(world, Tag);
    ecs_add(world, e8, Position);
    ecs_add(world, e8, Velocity);
    ecs_add(world, e8, Mass);
    ecs_enable_component(world, e8, Position, true);
    ecs_enable_component(world, e8, Velocity, true);
    ecs_enable_component(world, e8, Mass, true);

    /* Toggle Position == true, Velocity == false, Mass == false */
    ecs_entity_t e9 = ecs_new_w(world, Tag);
    ecs_add(world, e9, Position);
    ecs_add(world, e9, Velocity);
    ecs_add(world, e9, Mass);
    ecs_enable_component(world, e9, Position, true);
    ecs_enable_component(world, e9, Velocity, false);
    ecs_enable_component(world, e9, Mass, false);

    /* Toggle Position == false, Velocity == true, Mass == true */
    ecs_entity_t e10 = ecs_new_w(world, Tag);
    ecs_add(world, e10, Position);
    ecs_add(world, e10, Velocity);
    ecs_add(world, e10, Mass);
    ecs_enable_component(world, e10, Position, false);
    ecs_enable_component(world, e10, Velocity, true);
    ecs_enable_component(world, e10, Mass, true);

    /* Toggle Position == false, Velocity == false, Mass == false */
    ecs_entity_t e11 = ecs_new_w(world, Tag);
    ecs_add(world, e11, Position);
    ecs_add(world, e11, Velocity);
    ecs_add(world, e11, Mass);
    ecs_enable_component(world, e11, Position, false);
    ecs_enable_component(world, e11, Velocity, false);
    ecs_enable_component(world, e11, Mass, false);

    /* Toggle Position == true, Velocity == true, Mass == true */
    ecs_entity_t e12 = ecs_new_w(world, Tag);
    ecs_add(world, e12, Position);
    ecs_add(world, e12, Velocity);
    ecs_add(world, e12, Mass);
    ecs_enable_component(world, e12, Position, true);
    ecs_enable_component(world, e12, Velocity, true);
    ecs_enable_component(world, e12, Mass, false);

    /* Toggle Position == true, Velocity == false, Mass == true */
    ecs_entity_t e13 = ecs_new_w(world, Tag);
    ecs_add(world, e13, Position);
    ecs_add(world, e13, Velocity);
    ecs_add(world, e13, Mass);
    ecs_enable_component(world, e13, Position, true);
    ecs_enable_component(world, e13, Velocity, false);
    ecs_enable_component(world, e13, Mass, true);

    /* Toggle Position == false, Velocity == true, Mass == false */
    ecs_entity_t e14 = ecs_new_w(world, Tag);
    ecs_add(world, e14, Position);
    ecs_add(world, e14, Velocity);
    ecs_add(world, e14, Mass);
    ecs_enable_component(world, e14, Position, false);
    ecs_enable_component(world, e14, Velocity, true);
    ecs_enable_component(world, e14, Mass, false);

    /* Toggle Position == false, Velocity == false, Mass == true */
    ecs_entity_t e15 = ecs_new_w(world, Tag);
    ecs_add(world, e15, Position);
    ecs_add(world, e15, Velocity);
    ecs_add(world, e15, Mass);
    ecs_enable_component(world, e15, Position, false);
    ecs_enable_component(world, e15, Velocity, false);
    ecs_enable_component(world, e15, Mass, true);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, Position, ?Velocity, ?Mass",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 3));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));
    test_bool(false, ecs_field_is_set(&it, 3));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 3));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 3));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 3));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));
    test_bool(false, ecs_field_is_set(&it, 3));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 3));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 3));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e7, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 3));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));
    test_bool(false, ecs_field_is_set(&it, 3));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e8, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 3));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 3));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e9, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 3));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));
    test_bool(false, ecs_field_is_set(&it, 3));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e12, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 3));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));
    test_bool(false, ecs_field_is_set(&it, 3));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e13, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 3));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 3));

    test_bool(false, ecs_iter_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_written_2_toggle_w_not_toggle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);
    ecs_add_id(world, ecs_id(Mass), EcsCanToggle);

    /* No toggles */
    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_add(world, e1, Position);

    /* No components */
    /* ecs_entity_t e2 = */ ecs_new_w(world, Tag);

    /* Components, No toggles */
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Velocity);
    ecs_add(world, e3, Mass);

    /* Toggle Position == true */
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_add(world, e4, Position);
    ecs_enable_component(world, e4, Position, true);

    /* Toggle Position == false */
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_add(world, e5, Position);
    ecs_enable_component(world, e5, Position, false);

    /* Toggle Velocity == true, Mass = true */
    ecs_entity_t e6 = ecs_new_w(world, Tag);
    ecs_add(world, e6, Position);
    ecs_add(world, e6, Velocity);
    ecs_enable_component(world, e6, Velocity, true);
    ecs_add(world, e6, Mass);
    ecs_enable_component(world, e6, Mass, true);

    /* Toggle Velocity == false, Mass == false */
    ecs_entity_t e7 = ecs_new_w(world, Tag);
    ecs_add(world, e7, Position);
    ecs_add(world, e7, Velocity);
    ecs_enable_component(world, e7, Velocity, false);
    ecs_add(world, e7, Mass);
    ecs_enable_component(world, e7, Mass, false);

    /* Toggle Position == true, Velocity == true, Mass == true */
    ecs_entity_t e8 = ecs_new_w(world, Tag);
    ecs_add(world, e8, Position);
    ecs_add(world, e8, Velocity);
    ecs_add(world, e8, Mass);
    ecs_enable_component(world, e8, Position, true);
    ecs_enable_component(world, e8, Velocity, true);
    ecs_enable_component(world, e8, Mass, true);

    /* Toggle Position == true, Velocity == false, Mass == false */
    ecs_entity_t e9 = ecs_new_w(world, Tag);
    ecs_add(world, e9, Position);
    ecs_add(world, e9, Velocity);
    ecs_add(world, e9, Mass);
    ecs_enable_component(world, e9, Position, true);
    ecs_enable_component(world, e9, Velocity, false);
    ecs_enable_component(world, e9, Mass, false);

    /* Toggle Position == false, Velocity == true, Mass == true */
    ecs_entity_t e10 = ecs_new_w(world, Tag);
    ecs_add(world, e10, Position);
    ecs_add(world, e10, Velocity);
    ecs_add(world, e10, Mass);
    ecs_enable_component(world, e10, Position, false);
    ecs_enable_component(world, e10, Velocity, true);
    ecs_enable_component(world, e10, Mass, true);

    /* Toggle Position == false, Velocity == false, Mass == false */
    ecs_entity_t e11 = ecs_new_w(world, Tag);
    ecs_add(world, e11, Position);
    ecs_add(world, e11, Velocity);
    ecs_add(world, e11, Mass);
    ecs_enable_component(world, e11, Position, false);
    ecs_enable_component(world, e11, Velocity, false);
    ecs_enable_component(world, e11, Mass, false);

    /* Toggle Position == true, Velocity == true, Mass == true */
    ecs_entity_t e12 = ecs_new_w(world, Tag);
    ecs_add(world, e12, Position);
    ecs_add(world, e12, Velocity);
    ecs_add(world, e12, Mass);
    ecs_enable_component(world, e12, Position, true);
    ecs_enable_component(world, e12, Velocity, true);
    ecs_enable_component(world, e12, Mass, false);

    /* Toggle Position == true, Velocity == false, Mass == true */
    ecs_entity_t e13 = ecs_new_w(world, Tag);
    ecs_add(world, e13, Position);
    ecs_add(world, e13, Velocity);
    ecs_add(world, e13, Mass);
    ecs_enable_component(world, e13, Position, true);
    ecs_enable_component(world, e13, Velocity, false);
    ecs_enable_component(world, e13, Mass, true);

    /* Toggle Position == false, Velocity == true, Mass == false */
    ecs_entity_t e14 = ecs_new_w(world, Tag);
    ecs_add(world, e14, Position);
    ecs_add(world, e14, Velocity);
    ecs_add(world, e14, Mass);
    ecs_enable_component(world, e14, Position, false);
    ecs_enable_component(world, e14, Velocity, true);
    ecs_enable_component(world, e14, Mass, false);

    /* Toggle Position == false, Velocity == false, Mass == true */
    ecs_entity_t e15 = ecs_new_w(world, Tag);
    ecs_add(world, e15, Position);
    ecs_add(world, e15, Velocity);
    ecs_add(world, e15, Mass);
    ecs_enable_component(world, e15, Position, false);
    ecs_enable_component(world, e15, Velocity, false);
    ecs_enable_component(world, e15, Mass, true);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, Position, Velocity, !Mass",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e12, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 3));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));
    test_bool(false, ecs_field_is_set(&it, 3));

    test_bool(false, ecs_iter_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_this_written_2_toggle_w_optional_toggle(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TAG(world, Tag);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);
    ecs_add_id(world, ecs_id(Velocity), EcsCanToggle);
    ecs_add_id(world, ecs_id(Mass), EcsCanToggle);

    /* No toggles */
    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_add(world, e1, Position);

    /* No components */
    /* ecs_entity_t e2 = */ ecs_new_w(world, Tag);

    /* Components, No toggles */
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_add(world, e3, Position);
    ecs_add(world, e3, Velocity);
    ecs_add(world, e3, Mass);

    /* Toggle Position == true */
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_add(world, e4, Position);
    ecs_enable_component(world, e4, Position, true);

    /* Toggle Position == false */
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_add(world, e5, Position);
    ecs_enable_component(world, e5, Position, false);

    /* Toggle Velocity == true, Mass = true */
    ecs_entity_t e6 = ecs_new_w(world, Tag);
    ecs_add(world, e6, Position);
    ecs_add(world, e6, Velocity);
    ecs_enable_component(world, e6, Velocity, true);
    ecs_add(world, e6, Mass);
    ecs_enable_component(world, e6, Mass, true);

    /* Toggle Velocity == false, Mass == false */
    ecs_entity_t e7 = ecs_new_w(world, Tag);
    ecs_add(world, e7, Position);
    ecs_add(world, e7, Velocity);
    ecs_enable_component(world, e7, Velocity, false);
    ecs_add(world, e7, Mass);
    ecs_enable_component(world, e7, Mass, false);

    /* Toggle Position == true, Velocity == true, Mass == true */
    ecs_entity_t e8 = ecs_new_w(world, Tag);
    ecs_add(world, e8, Position);
    ecs_add(world, e8, Velocity);
    ecs_add(world, e8, Mass);
    ecs_enable_component(world, e8, Position, true);
    ecs_enable_component(world, e8, Velocity, true);
    ecs_enable_component(world, e8, Mass, true);

    /* Toggle Position == true, Velocity == false, Mass == false */
    ecs_entity_t e9 = ecs_new_w(world, Tag);
    ecs_add(world, e9, Position);
    ecs_add(world, e9, Velocity);
    ecs_add(world, e9, Mass);
    ecs_enable_component(world, e9, Position, true);
    ecs_enable_component(world, e9, Velocity, false);
    ecs_enable_component(world, e9, Mass, false);

    /* Toggle Position == false, Velocity == true, Mass == true */
    ecs_entity_t e10 = ecs_new_w(world, Tag);
    ecs_add(world, e10, Position);
    ecs_add(world, e10, Velocity);
    ecs_add(world, e10, Mass);
    ecs_enable_component(world, e10, Position, false);
    ecs_enable_component(world, e10, Velocity, true);
    ecs_enable_component(world, e10, Mass, true);

    /* Toggle Position == false, Velocity == false, Mass == false */
    ecs_entity_t e11 = ecs_new_w(world, Tag);
    ecs_add(world, e11, Position);
    ecs_add(world, e11, Velocity);
    ecs_add(world, e11, Mass);
    ecs_enable_component(world, e11, Position, false);
    ecs_enable_component(world, e11, Velocity, false);
    ecs_enable_component(world, e11, Mass, false);

    /* Toggle Position == true, Velocity == true, Mass == true */
    ecs_entity_t e12 = ecs_new_w(world, Tag);
    ecs_add(world, e12, Position);
    ecs_add(world, e12, Velocity);
    ecs_add(world, e12, Mass);
    ecs_enable_component(world, e12, Position, true);
    ecs_enable_component(world, e12, Velocity, true);
    ecs_enable_component(world, e12, Mass, false);

    /* Toggle Position == true, Velocity == false, Mass == true */
    ecs_entity_t e13 = ecs_new_w(world, Tag);
    ecs_add(world, e13, Position);
    ecs_add(world, e13, Velocity);
    ecs_add(world, e13, Mass);
    ecs_enable_component(world, e13, Position, true);
    ecs_enable_component(world, e13, Velocity, false);
    ecs_enable_component(world, e13, Mass, true);

    /* Toggle Position == false, Velocity == true, Mass == false */
    ecs_entity_t e14 = ecs_new_w(world, Tag);
    ecs_add(world, e14, Position);
    ecs_add(world, e14, Velocity);
    ecs_add(world, e14, Mass);
    ecs_enable_component(world, e14, Position, false);
    ecs_enable_component(world, e14, Velocity, true);
    ecs_enable_component(world, e14, Mass, false);

    /* Toggle Position == false, Velocity == false, Mass == true */
    ecs_entity_t e15 = ecs_new_w(world, Tag);
    ecs_add(world, e15, Position);
    ecs_add(world, e15, Velocity);
    ecs_add(world, e15, Mass);
    ecs_enable_component(world, e15, Position, false);
    ecs_enable_component(world, e15, Velocity, false);
    ecs_enable_component(world, e15, Mass, true);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, Position, Velocity, ?Mass",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 3));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 3));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 3));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 3));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e8, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 3));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 3));

    test_bool(true, ecs_iter_next(&it));
    test_int(1, it.count);
    test_uint(e12, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 2));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 3));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));
    test_bool(false, ecs_field_is_set(&it, 3));

    test_bool(false, ecs_iter_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_toggle_0_src_only_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(#0)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(0, it.count);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Toggle_toggle_0_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_add_id(world, ecs_id(Position), EcsCanToggle);

    ecs_entity_t e = ecs_new_w(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(#0), Foo",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(Foo, ecs_field_id(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}
