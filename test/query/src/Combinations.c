#include <query.h>

static ecs_query_cache_kind_t cache_kind = EcsQueryCacheDefault;
static ecs_entity_t on_instantiate = 0;

void Combinations_setup(void) {
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

    const char *on_instantiate_param = test_param("on_instantiate");
    if (on_instantiate_param) {
        if (!strcmp(on_instantiate_param, "override")) {
            on_instantiate = EcsOverride;
        } else if (!strcmp(on_instantiate_param, "inherit")) {
            on_instantiate = EcsInherit;
        } else if (!strcmp(on_instantiate_param, "dont_inherit")) {
            on_instantiate = EcsDontInherit;
        } else {
            printf("unexpected value for on_instantiate '%s'\n", cache_param);
        }
    }
}

void Combinations_trav_and_singleton_and_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, on_instantiate);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, on_instantiate);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, on_instantiate);

    ecs_singleton_set(world, Velocity, {1, 2});

    ecs_entity_t parent = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t child = ecs_insert(world,
        ecs_value(Mass, {100}));
    ecs_add_pair(world, child, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(up), Velocity($), Mass",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(child, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 2));
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_src(&it, 1));
    test_uint(0, ecs_field_src(&it, 2));
    {
        Position *ptr = ecs_field(&it, Position, 0);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10); test_int(ptr->y, 20);
    }
    {
        Velocity *ptr = ecs_field(&it, Velocity, 1);
        test_assert(ptr != NULL);
        test_int(ptr->x, 1); test_int(ptr->y, 2);
    }
    {
        Mass *ptr = ecs_field(&it, Mass, 2);
        test_assert(ptr != NULL);
        test_int(ptr[0], 100);
    }
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Combinations_self_and_singleton_and_trav(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, on_instantiate);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, on_instantiate);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, on_instantiate);

    ecs_singleton_set(world, Velocity, {1, 2});

    ecs_entity_t parent = ecs_insert(world, ecs_value(Mass, {100}));
    ecs_entity_t child = ecs_insert(world,
        ecs_value(Position, {10, 20}));
    ecs_add_pair(world, child, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, Velocity($), Mass(up)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(child, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 2));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_src(&it, 1));
    test_uint(parent, ecs_field_src(&it, 2));
    {
        Position *ptr = ecs_field(&it, Position, 0);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10); test_int(ptr->y, 20);
    }
    {
        Velocity *ptr = ecs_field(&it, Velocity, 1);
        test_assert(ptr != NULL);
        test_int(ptr->x, 1); test_int(ptr->y, 2);
    }
    {
        Mass *ptr = ecs_field(&it, Mass, 2);
        test_assert(ptr != NULL);
        test_int(ptr[0], 100);
    }
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Combinations_self_and_trav_and_singleton(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, on_instantiate);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, on_instantiate);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, on_instantiate);

    ecs_singleton_set(world, Mass, {100});

    ecs_entity_t parent = ecs_insert(world, ecs_value(Velocity, {1, 2}));
    ecs_entity_t child = ecs_insert(world,
        ecs_value(Position, {10, 20}));
    ecs_add_pair(world, child, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, Velocity(up), Mass($)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(child, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 2));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(ecs_id(Mass), ecs_field_src(&it, 2));
    {
        Position *ptr = ecs_field(&it, Position, 0);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10); test_int(ptr->y, 20);
    }
    {
        Velocity *ptr = ecs_field(&it, Velocity, 1);
        test_assert(ptr != NULL);
        test_int(ptr->x, 1); test_int(ptr->y, 2);
    }
    {
        Mass *ptr = ecs_field(&it, Mass, 2);
        test_assert(ptr != NULL);
        test_int(ptr[0], 100);
    }
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Combinations_singleton_and_self_and_trav(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, on_instantiate);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, on_instantiate);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, on_instantiate);

    ecs_singleton_set(world, Position, {10, 20});

    ecs_entity_t parent = ecs_insert(world, ecs_value(Mass, {100}));
    ecs_entity_t child = ecs_insert(world,
        ecs_value(Velocity, {1, 2}));
    ecs_add_pair(world, child, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position($), Velocity, Mass(up)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(child, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 2));
    test_uint(ecs_id(Position), ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(parent, ecs_field_src(&it, 2));
    {
        Position *ptr = ecs_field(&it, Position, 0);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10); test_int(ptr->y, 20);
    }
    {
        Velocity *ptr = ecs_field(&it, Velocity, 1);
        test_assert(ptr != NULL);
        test_int(ptr->x, 1); test_int(ptr->y, 2);
    }
    {
        Mass *ptr = ecs_field(&it, Mass, 2);
        test_assert(ptr != NULL);
        test_int(ptr[0], 100);
    }
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Combinations_trav_and_self_and_singleton(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, on_instantiate);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, on_instantiate);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, on_instantiate);

    ecs_singleton_set(world, Mass, {100});

    ecs_entity_t parent = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t child = ecs_insert(world,
        ecs_value(Velocity, {1, 2}));
    ecs_add_pair(world, child, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(up), Velocity, Mass($)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(child, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 2));
    test_uint(parent, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(ecs_id(Mass), ecs_field_src(&it, 2));
    {
        Position *ptr = ecs_field(&it, Position, 0);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10); test_int(ptr->y, 20);
    }
    {
        Velocity *ptr = ecs_field(&it, Velocity, 1);
        test_assert(ptr != NULL);
        test_int(ptr->x, 1); test_int(ptr->y, 2);
    }
    {
        Mass *ptr = ecs_field(&it, Mass, 2);
        test_assert(ptr != NULL);
        test_int(ptr[0], 100);
    }
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Combinations_singleton_and_trav_and_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, on_instantiate);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, on_instantiate);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, on_instantiate);

    ecs_singleton_set(world, Position, {10, 20});

    ecs_entity_t parent = ecs_insert(world, ecs_value(Velocity, {1, 2}));
    ecs_entity_t child = ecs_insert(world,
        ecs_value(Mass, {100}));
    ecs_add_pair(world, child, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position($), Velocity(up), Mass",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(child, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 2));
    test_uint(ecs_id(Position), ecs_field_src(&it, 0));
    test_uint(parent, ecs_field_src(&it, 1));
    test_uint(0, ecs_field_src(&it, 2));
    {
        Position *ptr = ecs_field(&it, Position, 0);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10); test_int(ptr->y, 20);
    }
    {
        Velocity *ptr = ecs_field(&it, Velocity, 1);
        test_assert(ptr != NULL);
        test_int(ptr->x, 1); test_int(ptr->y, 2);
    }
    {
        Mass *ptr = ecs_field(&it, Mass, 2);
        test_assert(ptr != NULL);
        test_int(ptr[0], 100);
    }
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Combinations_singleton_and_self_and_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, on_instantiate);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, on_instantiate);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, on_instantiate);

    ecs_singleton_set(world, Position, {10, 20});

    ecs_entity_t child = ecs_insert(world,
        ecs_value(Velocity, {1, 2}),
        ecs_value(Mass, {100}));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position($), Velocity, Mass",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(child, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 2));
    test_uint(ecs_id(Position), ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(0, ecs_field_src(&it, 2));
    {
        Position *ptr = ecs_field(&it, Position, 0);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10); test_int(ptr->y, 20);
    }
    {
        Velocity *ptr = ecs_field(&it, Velocity, 1);
        test_assert(ptr != NULL);
        test_int(ptr->x, 1); test_int(ptr->y, 2);
    }
    {
        Mass *ptr = ecs_field(&it, Mass, 2);
        test_assert(ptr != NULL);
        test_int(ptr[0], 100);
    }
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Combinations_self_and_singleton_and_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, on_instantiate);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, on_instantiate);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, on_instantiate);

    ecs_singleton_set(world, Velocity, {1, 2});

    ecs_entity_t child = ecs_insert(world,
        ecs_value(Position, {10, 20}),
        ecs_value(Mass, {100}));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, Velocity($), Mass",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(child, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 2));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_src(&it, 1));
    test_uint(0, ecs_field_src(&it, 2));
    {
        Position *ptr = ecs_field(&it, Position, 0);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10); test_int(ptr->y, 20);
    }
    {
        Velocity *ptr = ecs_field(&it, Velocity, 1);
        test_assert(ptr != NULL);
        test_int(ptr->x, 1); test_int(ptr->y, 2);
    }
    {
        Mass *ptr = ecs_field(&it, Mass, 2);
        test_assert(ptr != NULL);
        test_int(ptr[0], 100);
    }
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Combinations_self_and_self_and_singleton(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, on_instantiate);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, on_instantiate);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, on_instantiate);

    ecs_singleton_set(world, Mass, {100});

    ecs_entity_t child = ecs_insert(world,
        ecs_value(Position, {10, 20}),
        ecs_value(Velocity, {1, 2}));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, Velocity, Mass($)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(child, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Mass), ecs_field_id(&it, 2));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_uint(ecs_id(Mass), ecs_field_src(&it, 2));
    {
        Position *ptr = ecs_field(&it, Position, 0);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10); test_int(ptr->y, 20);
    }
    {
        Velocity *ptr = ecs_field(&it, Velocity, 1);
        test_assert(ptr != NULL);
        test_int(ptr->x, 1); test_int(ptr->y, 2);
    }
    {
        Mass *ptr = ecs_field(&it, Mass, 2);
        test_assert(ptr != NULL);
        test_int(ptr[0], 100);
    }
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}
