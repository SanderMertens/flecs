#include <query.h>

static ecs_query_cache_kind_t cache_kind = EcsQueryCacheDefault;

void MemberTarget_setup(void) {
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

typedef struct {
    ecs_entity_t value;
} Movement;

typedef struct {
    ecs_entity_t a;
    ecs_entity_t b;
} TwoMembers;

static ecs_entity_t ecs_id(Movement) = 0;
static ecs_entity_t ecs_id(TwoMembers) = 0;
static ecs_entity_t Running = 0;
static ecs_entity_t Walking = 0;
static ecs_entity_t Sitting = 0;

static void register_types(
    ecs_world_t *world)
{
    ecs_id(Movement) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Movement" }),
        .members = {
            { "value", ecs_id(ecs_entity_t) }
        }
    });

    ecs_id(TwoMembers) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "TwoMembers" }),
        .members = {
            { "a", ecs_id(ecs_entity_t) },
            { "b", ecs_id(ecs_entity_t) }
        }
    });

    Running = ecs_entity(world, { .name = "Running" });
    Walking = ecs_entity(world, { .name = "Walking" });
    Sitting = ecs_entity(world, { .name = "Sitting" });
}

void MemberTarget_this_member_eq_1(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement.value, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    /* ecs_entity_t e3 = */ ecs_insert(world, ecs_value(Movement, { Walking }));
    /* ecs_entity_t e4 = */ ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e5, it.entities[0]);
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_member_eq_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement.value, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    /* ecs_entity_t e1 = */ ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    /* ecs_entity_t e3 = */ ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Running }));
    /* ecs_entity_t e5 = */ ecs_insert(world, ecs_value(Movement, { Sitting }));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_member_eq_no_matches(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement.value, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    /* ecs_entity_t e1 = */ ecs_insert(world, ecs_value(Movement, { Walking }));
    /* ecs_entity_t e2 = */ ecs_insert(world, ecs_value(Movement, { Walking }));
    /* ecs_entity_t e3 = */ ecs_insert(world, ecs_value(Movement, { Sitting }));
    /* ecs_entity_t e4 = */ ecs_insert(world, ecs_value(Movement, { Walking }));
    /* ecs_entity_t e5 = */ ecs_insert(world, ecs_value(Movement, { Sitting }));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_member_eq_all_matches(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement.value, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e5, it.entities[0]);
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_member_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement.value, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(ecs_pair(member, Sitting), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e5, it.entities[0]);
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_member_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement.value, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(Running, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(Running, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(Walking, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(Sitting, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Sitting), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e5, it.entities[0]);
        test_uint(Running, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_member_var_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($x), (Movement.value, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_add(world, Running, Foo);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    /* ecs_entity_t e3 = */ ecs_insert(world, ecs_value(Movement, { Walking }));
    /* ecs_entity_t e4 = */ ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Running, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Running, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e5, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Running, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_member_var_read(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement.value, $x), Foo($x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_add(world, Running, Foo);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    /* ecs_entity_t e3 = */ ecs_insert(world, ecs_value(Movement, { Walking }));
    /* ecs_entity_t e4 = */ ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(Running, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(Running, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e5, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 1));
        test_uint(Running, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_member_eq_1_2nd_member(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(TwoMembers.b, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "TwoMembers.b");
    test_assert(member != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(TwoMembers, { Walking, Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(TwoMembers, { Walking, Running }));
    /* ecs_entity_t e3 = */ ecs_insert(world, ecs_value(TwoMembers, { Walking, Walking }));
    /* ecs_entity_t e4 = */ ecs_insert(world, ecs_value(TwoMembers, { Walking, Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(TwoMembers, { Walking, Running }));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e5, it.entities[0]);
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_member_eq_2_2nd_member(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(TwoMembers.b, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "TwoMembers.b");
    test_assert(member != 0);

    /* ecs_entity_t e1 = */ ecs_insert(world, ecs_value(TwoMembers, { Sitting, Walking }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(TwoMembers, { Sitting, Running }));
    /* ecs_entity_t e3 = */ ecs_insert(world, ecs_value(TwoMembers, { Sitting, Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(TwoMembers, { Sitting, Running }));
    /* ecs_entity_t e5 = */ ecs_insert(world, ecs_value(TwoMembers, { Sitting, Sitting }));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_member_var_same_1(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(TwoMembers.a, $x), (TwoMembers.b, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member_a = ecs_lookup(world, "TwoMembers.a");
    test_assert(member_a != 0);
    ecs_entity_t member_b = ecs_lookup(world, "TwoMembers.b");
    test_assert(member_b != 0);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(TwoMembers, { Running, Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(TwoMembers, { Walking, Walking }));
    /* ecs_entity_t e3 = */ ecs_insert(world, ecs_value(TwoMembers, { Walking, Sitting }));
    /* ecs_entity_t e4 = */ ecs_insert(world, ecs_value(TwoMembers, { Sitting, Walking }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(TwoMembers, { Sitting, Sitting }));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(Running, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member_a, Running), ecs_field_id(&it, 0));
        test_uint(ecs_pair(member_b, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(Walking, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member_a, Walking), ecs_field_id(&it, 0));
        test_uint(ecs_pair(member_b, Walking), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e5, it.entities[0]);
        test_uint(Sitting, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member_a, Sitting), ecs_field_id(&it, 0));
        test_uint(ecs_pair(member_b, Sitting), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_member_var_same_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(TwoMembers.a, $x), (TwoMembers.b, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member_a = ecs_lookup(world, "TwoMembers.a");
    test_assert(member_a != 0);
    ecs_entity_t member_b = ecs_lookup(world, "TwoMembers.b");
    test_assert(member_b != 0);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    /* ecs_entity_t e1 = */ ecs_insert(world, ecs_value(TwoMembers, { Walking, Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(TwoMembers, { Running, Running }));
    /* ecs_entity_t e3 = */ ecs_insert(world, ecs_value(TwoMembers, { Walking, Sitting }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(TwoMembers, { Walking, Walking }));
    /* ecs_entity_t e5 = */ ecs_insert(world, ecs_value(TwoMembers, { Running, Sitting }));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(Running, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member_a, Running), ecs_field_id(&it, 0));
        test_uint(ecs_pair(member_b, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(Walking, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member_a, Walking), ecs_field_id(&it, 0));
        test_uint(ecs_pair(member_b, Walking), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_written_member_eq_1(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, (Movement.value, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e5, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_written_member_eq_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, (Movement.value, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Sitting }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_written_member_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, (Movement.value, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member, Sitting), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e5, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_written_member_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, (Movement.value, $x)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Running, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Running, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Walking, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Sitting, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Sitting), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e5, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Running, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_member_neq_1(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!(Movement.value, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t e0 = ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    bool e0_found = false;
    bool e1_found = false;
    bool e2_found = false;
    bool e3_found = false;
    bool e4_found = false;
    bool e5_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_assert(it.count != 0);
        for (int i = 0; i < it.count; i ++) {
            e0_found |= it.entities[i] == e0;
            e1_found |= it.entities[i] == e1;
            e2_found |= it.entities[i] == e2;
            e3_found |= it.entities[i] == e3;
            e4_found |= it.entities[i] == e4;
            e5_found |= it.entities[i] == e5;

            if (it.entities[i] == e0) test_assert(it.table == ecs_get_table(world, e0));
            if (it.entities[i] == e1) test_assert(it.table == ecs_get_table(world, e1));
            if (it.entities[i] == e2) test_assert(it.table == ecs_get_table(world, e2));
            if (it.entities[i] == e3) test_assert(it.table == ecs_get_table(world, e3));
            if (it.entities[i] == e4) test_assert(it.table == ecs_get_table(world, e4));
            if (it.entities[i] == e5) test_assert(it.table == ecs_get_table(world, e5));
        }
    }

    test_bool(true, e0_found);
    test_bool(false, e1_found);
    test_bool(false, e2_found);
    test_bool(true, e3_found);
    test_bool(true, e4_found);
    test_bool(false, e5_found);

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_member_neq_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!(Movement.value, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t e0 = ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Sitting }));

    bool e0_found = false;
    bool e1_found = false;
    bool e2_found = false;
    bool e3_found = false;
    bool e4_found = false;
    bool e5_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        for (int i = 0; i < it.count; i ++) {
            e0_found |= it.entities[i] == e0;
            e1_found |= it.entities[i] == e1;
            e2_found |= it.entities[i] == e2;
            e3_found |= it.entities[i] == e3;
            e4_found |= it.entities[i] == e4;
            e5_found |= it.entities[i] == e5;

            if (it.entities[i] == e0) test_assert(it.table == ecs_get_table(world, e0));
            if (it.entities[i] == e1) test_assert(it.table == ecs_get_table(world, e1));
            if (it.entities[i] == e2) test_assert(it.table == ecs_get_table(world, e2));
            if (it.entities[i] == e3) test_assert(it.table == ecs_get_table(world, e3));
            if (it.entities[i] == e4) test_assert(it.table == ecs_get_table(world, e4));
            if (it.entities[i] == e5) test_assert(it.table == ecs_get_table(world, e5));
        }
    }

    test_bool(true, e0_found);
    test_bool(true, e1_found);
    test_bool(false, e2_found);
    test_bool(true, e3_found);
    test_bool(false, e4_found);
    test_bool(true, e5_found);

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_member_neq_no_matches(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!(Movement.value, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t e0 = ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    bool e0_found = false;
    bool e1_found = false;
    bool e2_found = false;
    bool e3_found = false;
    bool e4_found = false;
    bool e5_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_assert(it.count != 0);
        for (int i = 0; i < it.count; i ++) {
            e0_found |= it.entities[i] == e0;
            e1_found |= it.entities[i] == e1;
            e2_found |= it.entities[i] == e2;
            e3_found |= it.entities[i] == e3;
            e4_found |= it.entities[i] == e4;
            e5_found |= it.entities[i] == e5;

            if (it.entities[i] == e0) test_assert(it.table == ecs_get_table(world, e0));
        }
    }

    test_bool(true, e0_found);
    test_bool(false, e1_found);
    test_bool(false, e2_found);
    test_bool(false, e3_found);
    test_bool(false, e4_found);
    test_bool(false, e5_found);

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_member_neq_all_matches(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!(Movement.value, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t e0 = ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Walking }));

    bool e0_found = false;
    bool e1_found = false;
    bool e2_found = false;
    bool e3_found = false;
    bool e4_found = false;
    bool e5_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_assert(it.count != 0);
        for (int i = 0; i < it.count; i ++) {
            e0_found |= it.entities[i] == e0;
            e1_found |= it.entities[i] == e1;
            e2_found |= it.entities[i] == e2;
            e3_found |= it.entities[i] == e3;
            e4_found |= it.entities[i] == e4;
            e5_found |= it.entities[i] == e5;

            if (it.entities[i] == e0) test_assert(it.table == ecs_get_table(world, e0));
        }
    }

    test_bool(true, e0_found);
    test_bool(true, e1_found);
    test_bool(true, e2_found);
    test_bool(true, e3_found);
    test_bool(true, e4_found);
    test_bool(true, e5_found);

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_member_neq_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!(Movement.value, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t e0 = ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    bool e0_found = false;
    bool e1_found = false;
    bool e2_found = false;
    bool e3_found = false;
    bool e4_found = false;
    bool e5_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_assert(it.count != 0);
        for (int i = 0; i < it.count; i ++) {
            e0_found |= it.entities[i] == e0;
            e1_found |= it.entities[i] == e1;
            e2_found |= it.entities[i] == e2;
            e3_found |= it.entities[i] == e3;
            e4_found |= it.entities[i] == e4;
            e5_found |= it.entities[i] == e5;

            if (it.entities[i] == e0) test_assert(it.table == ecs_get_table(world, e0));
            if (it.entities[i] == e1) test_assert(it.table == ecs_get_table(world, e1));
            if (it.entities[i] == e2) test_assert(it.table == ecs_get_table(world, e2));
            if (it.entities[i] == e3) test_assert(it.table == ecs_get_table(world, e3));
            if (it.entities[i] == e4) test_assert(it.table == ecs_get_table(world, e4));
            if (it.entities[i] == e5) test_assert(it.table == ecs_get_table(world, e5));
        }
    }

    test_bool(true, e0_found);
    test_bool(false, e1_found);
    test_bool(false, e2_found);
    test_bool(false, e3_found);
    test_bool(false, e4_found);
    test_bool(false, e5_found);

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_written_member_neq_1(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, !(Movement.value, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t e0 = ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e0, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Movement), ecs_field_id(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Sitting), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_written_member_neq_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, !(Movement.value, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t e0 = ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Sitting }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e0, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Movement), ecs_field_id(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e5, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Sitting), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_written_member_neq_no_matches(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, !(Movement.value, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t e0 = ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e0, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Movement), ecs_field_id(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_written_member_neq_all_matches(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, !(Movement.value, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t e0 = ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Walking }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e0, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Movement), ecs_field_id(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Sitting), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Sitting), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e5, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_written_member_neq_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, !(Movement.value, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t e0 = ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Walking }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e0, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Movement), ecs_field_id(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_member_eq_optional(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "?(Movement.value, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t e0 = ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    bool e0_found = false;
    bool e1_found = false;
    bool e2_found = false;
    bool e3_found = false;
    bool e4_found = false;
    bool e5_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_assert(it.count != 0);
        for (int i = 0; i < it.count; i ++) {
            e0_found |= it.entities[i] == e0;
            e1_found |= it.entities[i] == e1;
            e2_found |= it.entities[i] == e2;
            e3_found |= it.entities[i] == e3;
            e4_found |= it.entities[i] == e4;
            e5_found |= it.entities[i] == e5;

            if (it.entities[i] == e0) test_assert(it.table == ecs_get_table(world, e0));
            if (it.entities[i] == e1) test_assert(it.table == ecs_get_table(world, e1));
            if (it.entities[i] == e2) test_assert(it.table == ecs_get_table(world, e2));
            if (it.entities[i] == e3) test_assert(it.table == ecs_get_table(world, e3));
            if (it.entities[i] == e4) test_assert(it.table == ecs_get_table(world, e4));
            if (it.entities[i] == e5) test_assert(it.table == ecs_get_table(world, e5));
        }
    }

    test_bool(true, e0_found);
    test_bool(true, e1_found);
    test_bool(true, e2_found);
    test_bool(true, e3_found);
    test_bool(true, e4_found);
    test_bool(true, e5_found);

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_member_eq_optional_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "?(Movement.value, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t e0 = ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    bool e0_found = false;
    bool e1_found = false;
    bool e2_found = false;
    bool e3_found = false;
    bool e4_found = false;
    bool e5_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_assert(it.count != 0);
        for (int i = 0; i < it.count; i ++) {
            e0_found |= it.entities[i] == e0;
            e1_found |= it.entities[i] == e1;
            e2_found |= it.entities[i] == e2;
            e3_found |= it.entities[i] == e3;
            e4_found |= it.entities[i] == e4;
            e5_found |= it.entities[i] == e5;

            if (it.entities[i] == e0) test_assert(it.table == ecs_get_table(world, e0));
            if (it.entities[i] == e1) test_assert(it.table == ecs_get_table(world, e1));
            if (it.entities[i] == e2) test_assert(it.table == ecs_get_table(world, e2));
            if (it.entities[i] == e3) test_assert(it.table == ecs_get_table(world, e3));
            if (it.entities[i] == e4) test_assert(it.table == ecs_get_table(world, e4));
            if (it.entities[i] == e5) test_assert(it.table == ecs_get_table(world, e5));
        }
    }

    test_bool(true, e0_found);
    test_bool(true, e1_found);
    test_bool(true, e2_found);
    test_bool(true, e3_found);
    test_bool(true, e4_found);
    test_bool(true, e5_found);

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_written_member_eq_optional(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, ?(Movement.value, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t e0 = ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e0, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Movement), ecs_field_id(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Sitting), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e5, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_written_member_eq_optional_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, ?(Movement.value, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t e0 = ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e0, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Movement), ecs_field_id(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Sitting), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e5, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_member_eq_w_other_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Tag);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement.value, Running), Tag",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Running }));
    /* ecs_entity_t e4 = */ ecs_insert(world, ecs_value(Movement, { Sitting }));
    /* ecs_entity_t e5 = */ ecs_insert(world, ecs_value(Movement, { Running }));

    ecs_add(world, e1, Tag);
    ecs_add(world, e2, Tag);
    ecs_add(world, e3, Tag);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_member_eq_w_other_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_COMPONENT(world, Position);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement.value, Running), Position",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Running }));
    /* ecs_entity_t e4 = */ ecs_insert(world, ecs_value(Movement, { Sitting }));
    /* ecs_entity_t e5 = */ ecs_insert(world, ecs_value(Movement, { Running }));

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {40, 50});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p[0].x, 10);
            test_int(p[0].y, 20);
        }

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(ecs_id(Position), ecs_field_id(&it, 1));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));
        {
            Position *p = ecs_field(&it, Position, 1);
            test_assert(p != NULL);
            test_int(p[0].x, 40);
            test_int(p[0].y, 50);
        }

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_written_member_eq_w_other_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Tag);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, (Movement.value, Running), Tag",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    ecs_add(world, e1, Tag);
    ecs_add(world, e2, Tag);
    ecs_add(world, e3, Tag);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_written_member_eq_w_other_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, (Movement.value, Running), Position",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {40, 50});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 2));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 2);
            test_assert(p != NULL);
            test_int(p[0].x, 10);
            test_int(p[0].y, 20);
        }

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 2));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 2);
            test_assert(p != NULL);
            test_int(p[0].x, 40);
            test_int(p[0].y, 50);
        }

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_written_member_eq_w_other_inherit_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, (Movement.value, Running), Position",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 30});
    ecs_set(world, e3, Position, {40, 50});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 2));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 2);
            test_assert(p != NULL);
            test_int(p[0].x, 10);
            test_int(p[0].y, 20);
        }

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Position), ecs_field_id(&it, 2));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));
        {
            Position *p = ecs_field(&it, Position, 2);
            test_assert(p != NULL);
            test_int(p[0].x, 40);
            test_int(p[0].y, 50);
        }

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_2_or(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement.value, Running) || (Movement.value, Walking)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    /* ecs_entity_t e0 = */ ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e5, it.entities[0]);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_3_or(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement.value, Running) || (Movement.value, Walking) || (Movement.value, Sitting)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t Idle = ecs_new(world);

    /* ecs_entity_t e0 = */ ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Idle }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Idle }));
    ecs_entity_t e6 = ecs_insert(world, ecs_value(Movement, { Running }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);
    ecs_add(world, e6, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e6, it.entities[0]);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_pair(member, Sitting), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_written_2_or(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, (Movement.value, Running) || (Movement.value, Walking)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    /* ecs_entity_t e0 = */ ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e5, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_written_3_or(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, (Movement.value, Running) || (Movement.value, Walking) || (Movement.value, Sitting)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    ecs_entity_t Idle = ecs_new(world);

    /* ecs_entity_t e0 = */ ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Idle }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Idle }));
    ecs_entity_t e6 = ecs_insert(world, ecs_value(Movement, { Running }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);
    ecs_add(world, e6, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Sitting), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e6, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_var_2_or(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Movement.value($x, Running) || Movement.value($x, Walking)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    /* ecs_entity_t e0 = */ ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e2, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e5, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e3, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_var_3_or(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Movement.value($x, Running) || Movement.value($x, Walking) || Movement.value($x, Sitting)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t Idle = ecs_new(world);

    /* ecs_entity_t e0 = */ ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Idle }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Idle }));
    ecs_entity_t e6 = ecs_insert(world, ecs_value(Movement, { Running }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);
    ecs_add(world, e6, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e6, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e3, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e4, ecs_iter_get_var(&it, x_var));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(ecs_pair(member, Sitting), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_var_written_2_or(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($x), Movement.value($x, Running) || Movement.value($x, Walking)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    /* ecs_entity_t e0 = */ ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e2, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e3, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e5, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_var_written_3_or(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($x), Movement.value($x, Running) || Movement.value($x, Walking) || Movement.value($x, Sitting)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t Idle = ecs_new(world);

    /* ecs_entity_t e0 = */ ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Idle }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Idle }));
    ecs_entity_t e6 = ecs_insert(world, ecs_value(Movement, { Running }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);
    ecs_add(world, e6, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e3, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e4, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Sitting), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e6, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_2_or_w_2_members(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(TwoMembers.a, Running) || (TwoMembers.b, Walking)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member_a = ecs_lookup(world, "TwoMembers.a");
    test_assert(member_a != 0);
    ecs_entity_t member_b = ecs_lookup(world, "TwoMembers.b");
    test_assert(member_b != 0);

    /* ecs_entity_t e1 = */ ecs_insert(world, ecs_value(TwoMembers, { Walking, Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(TwoMembers, { Running, Sitting }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(TwoMembers, { Sitting, Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(TwoMembers, { Running, Walking }));
    /* ecs_entity_t e5 = */ ecs_insert(world, ecs_value(TwoMembers, { Walking, Running }));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(ecs_pair(member_a, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(ecs_pair(member_a, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(ecs_pair(member_b, Walking), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_2_or_w_2_types(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Movement.value, Running) || (TwoMembers.b, Walking)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member_a = ecs_lookup(world, "Movement.value");
    test_assert(member_a != 0);
    ecs_entity_t member_b = ecs_lookup(world, "TwoMembers.b");
    test_assert(member_b != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(TwoMembers, { Walking, Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(TwoMembers, { Sitting, Sitting }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(TwoMembers, { Sitting, Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(TwoMembers, { Sitting, Walking }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(TwoMembers, { Walking, Running }));
    
    ecs_set(world, e1, Movement, { Walking });
    ecs_set(world, e2, Movement, { Running });
    ecs_set(world, e3, Movement, { Sitting });
    ecs_set(world, e4, Movement, { Running });
    ecs_set(world, e5, Movement, { Sitting });

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(ecs_pair(member_a, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(ecs_pair(member_a, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(ecs_pair(member_b, Walking), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_written_2_or_w_2_members(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, (TwoMembers.a, Running) || (TwoMembers.b, Walking)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member_a = ecs_lookup(world, "TwoMembers.a");
    test_assert(member_a != 0);
    ecs_entity_t member_b = ecs_lookup(world, "TwoMembers.b");
    test_assert(member_b != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(TwoMembers, { Walking, Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(TwoMembers, { Running, Sitting }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(TwoMembers, { Sitting, Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(TwoMembers, { Running, Walking }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(TwoMembers, { Walking, Running }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member_a, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member_b, Walking), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member_a, Running), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_written_2_or_w_2_types(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, (Movement.value, Running) || (TwoMembers.b, Walking)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member_a = ecs_lookup(world, "Movement.value");
    test_assert(member_a != 0);
    ecs_entity_t member_b = ecs_lookup(world, "TwoMembers.b");
    test_assert(member_b != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(TwoMembers, { Walking, Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(TwoMembers, { Sitting, Sitting }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(TwoMembers, { Sitting, Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(TwoMembers, { Sitting, Walking }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(TwoMembers, { Walking, Running }));
    
    ecs_set(world, e1, Movement, { Walking });
    ecs_set(world, e2, Movement, { Running });
    ecs_set(world, e3, Movement, { Sitting });
    ecs_set(world, e4, Movement, { Running });
    ecs_set(world, e5, Movement, { Sitting });

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member_a, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member_b, Walking), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member_a, Running), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_2_or_2_types_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(TwoMembers.b, *) || (Movement.value, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member_a = ecs_lookup(world, "TwoMembers.b");
    test_assert(member_a != 0);
    ecs_entity_t member_b = ecs_lookup(world, "Movement.value");
    test_assert(member_b != 0);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(TwoMembers, { Walking, Running }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(TwoMembers, { Walking, Sitting }));
    
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_set(world, e4, TwoMembers, { Running, Walking });

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(ecs_pair(member_a, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e5, it.entities[0]);
        test_uint(ecs_pair(member_a, Sitting), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(ecs_pair(member_a, Walking), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(ecs_pair(member_b, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(ecs_pair(member_b, Sitting), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_this_2_or_2_types_dep_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(TwoMembers.b, $x) || (Movement.value, $y)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member_a = ecs_lookup(world, "TwoMembers.b");
    test_assert(member_a != 0);
    ecs_entity_t member_b = ecs_lookup(world, "Movement.value");
    test_assert(member_b != 0);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    int y_var = ecs_query_find_var(q, "y");
    test_assert(y_var != -1);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(TwoMembers, { Walking, Running }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(TwoMembers, { Walking, Sitting }));
    
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_set(world, e4, TwoMembers, { Running, Walking });

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(Running, ecs_iter_get_var(&it, x_var));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, y_var));
        test_uint(ecs_pair(member_a, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e5, it.entities[0]);
        test_uint(Sitting, ecs_iter_get_var(&it, x_var));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, y_var));
        test_uint(ecs_pair(member_a, Sitting), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e4, it.entities[0]);
        test_uint(Walking, ecs_iter_get_var(&it, x_var));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, y_var));
        test_uint(ecs_pair(member_a, Walking), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(Running, ecs_iter_get_var(&it, y_var));
        test_uint(ecs_pair(member_b, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(Sitting, ecs_iter_get_var(&it, y_var));
        test_uint(ecs_pair(member_b, Sitting), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_var_member_eq(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Movement.value($x, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    /* ecs_entity_t e3 = */ ecs_insert(world, ecs_value(Movement, { Walking }));
    /* ecs_entity_t e4 = */ ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e2, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e5, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_var_member_eq_no_matches(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Movement.value($x, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    /* ecs_entity_t e1 = */ ecs_insert(world, ecs_value(Movement, { Walking }));
    /* ecs_entity_t e2 = */ ecs_insert(world, ecs_value(Movement, { Sitting }));
    /* ecs_entity_t e3 = */ ecs_insert(world, ecs_value(Movement, { Walking }));
    /* ecs_entity_t e4 = */ ecs_insert(world, ecs_value(Movement, { Sitting }));
    /* ecs_entity_t e5 = */ ecs_insert(world, ecs_value(Movement, { Walking }));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_var_member_eq_all_matches(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Movement.value($x, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e2, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e3, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e4, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e5, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_var_member_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Movement.value($x, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e2, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e3, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e4, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Sitting), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e5, ecs_iter_get_var(&it, x_var));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_var_member_neq(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!Movement.value($x, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e0 = ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    bool e0_found = false;
    bool e1_found = false;
    bool e2_found = false;
    bool e3_found = false;
    bool e4_found = false;
    bool e5_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_assert(it.count == 0);
        ecs_entity_t x = ecs_iter_get_var(&it, x_var);
        e0_found |= x == e0;
        e1_found |= x == e1;
        e2_found |= x == e2;
        e3_found |= x == e3;
        e4_found |= x == e4;
        e5_found |= x == e5;
    }

    test_bool(true, e0_found);
    test_bool(false, e1_found);
    test_bool(false, e2_found);
    test_bool(true, e3_found);
    test_bool(true, e4_found);
    test_bool(false, e5_found);

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_var_member_neq_no_matches(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!Movement.value($x, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e0 = ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    bool e0_found = false;
    bool e1_found = false;
    bool e2_found = false;
    bool e3_found = false;
    bool e4_found = false;
    bool e5_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_assert(it.count == 0);
        ecs_entity_t x = ecs_iter_get_var(&it, x_var);
        e0_found |= x == e0;
        e1_found |= x == e1;
        e2_found |= x == e2;
        e3_found |= x == e3;
        e4_found |= x == e4;
        e5_found |= x == e5;
    }

    test_bool(true, e0_found);
    test_bool(false, e1_found);
    test_bool(false, e2_found);
    test_bool(false, e3_found);
    test_bool(false, e4_found);
    test_bool(false, e5_found);

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_var_member_neq_all_matches(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "!Movement.value($x, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);
    
    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e0 = ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Walking }));

    bool e0_found = false;
    bool e1_found = false;
    bool e2_found = false;
    bool e3_found = false;
    bool e4_found = false;
    bool e5_found = false;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_assert(it.count == 0);
        ecs_entity_t x = ecs_iter_get_var(&it, x_var);
        e0_found |= x == e0;
        e1_found |= x == e1;
        e2_found |= x == e2;
        e3_found |= x == e3;
        e4_found |= x == e4;
        e5_found |= x == e5;
    }

    test_bool(true, e0_found);
    test_bool(true, e1_found);
    test_bool(true, e2_found);
    test_bool(true, e3_found);
    test_bool(true, e4_found);
    test_bool(true, e5_found);

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_var_written_member_eq(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($x), Movement.value($x, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e2, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e5, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_var_written_member_eq_no_matches(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($x), Movement.value($x, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Walking }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_var_written_member_eq_all_matches(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($x), Movement.value($x, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e2, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e3, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e4, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e5, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_var_written_member_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($x), Movement.value($x, *)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Walking }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e2, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e3, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member, Sitting), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e4, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member, Running), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e5, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_var_written_member_neq(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($x), !Movement.value($x, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e0 = ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e0, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Movement), ecs_field_id(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e3, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e4, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Sitting), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_var_written_member_neq_no_matches(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($x), !Movement.value($x, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e0 = ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Running }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Running }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e0, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Movement), ecs_field_id(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void MemberTarget_var_written_member_neq_all_matches(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsMeta);
    ECS_TAG(world, Foo);

    register_types(world);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo($x), !Movement.value($x, Running)",
        .cache_kind = cache_kind
    });

    test_assert(q != NULL);

    ecs_entity_t member = ecs_lookup(world, "Movement.value");
    test_assert(member != 0);

    int x_var = ecs_query_find_var(q, "x");
    test_assert(x_var != -1);

    ecs_entity_t e0 = ecs_new_w(world, Foo);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Movement, { Walking }));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Movement, { Sitting }));
    ecs_entity_t e5 = ecs_insert(world, ecs_value(Movement, { Walking }));

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e3, Foo);
    ecs_add(world, e4, Foo);
    ecs_add(world, e5, Foo);

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e0, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(ecs_id(Movement), ecs_field_id(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e2, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Sitting), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e3, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e4, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Sitting), ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_int(0, it.count);
        test_uint(e5, ecs_iter_get_var(&it, x_var));
        test_uint(Foo, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(ecs_pair(member, Walking), ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}
