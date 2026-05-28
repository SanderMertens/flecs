#include <query.h>

typedef struct {
    int32_t value;
} Weight;

typedef struct {
    int32_t value;
    int32_t bonus;
} HeavyWeight;

static ecs_query_cache_kind_t cache_kind = EcsQueryCacheDefault;

void ComponentInheritance_setup(void) {
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

ECS_TAG_DECLARE(Unit);
ECS_TAG_DECLARE(MeleeUnit);
ECS_TAG_DECLARE(RangedUnit);
ECS_TAG_DECLARE(Warrior);
ECS_TAG_DECLARE(Archer);
ECS_TAG_DECLARE(Wizard);
ECS_TAG_DECLARE(Warlock);

static
void populate_facts(ecs_world_t *world) {
    ECS_TAG_DEFINE(world, Unit);
    ECS_ENTITY_DEFINE(world, MeleeUnit, (IsA, Unit));
    ECS_ENTITY_DEFINE(world, RangedUnit, (IsA, Unit));
    ECS_ENTITY_DEFINE(world, Warrior, (IsA, MeleeUnit));
    ECS_ENTITY_DEFINE(world, Archer, (IsA, RangedUnit));
    ECS_ENTITY_DEFINE(world, Wizard, (IsA, RangedUnit));
    ECS_ENTITY_DEFINE(world, Warlock, (IsA, Wizard), (IsA, Warrior));
}

void ComponentInheritance_1_fixed_0_lvl(void) {
    ecs_world_t *world = ecs_mini();

    populate_facts(world);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add(world, e1, Warlock);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Warlock(e1)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Warlock, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_1_fixed_1_lvl(void) {
    ecs_world_t *world = ecs_mini();

    populate_facts(world);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add(world, e1, Warlock);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Warrior(e1)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Warlock, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_1_fixed_2_lvl(void) {
    ecs_world_t *world = ecs_mini();

    populate_facts(world);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add(world, e1, Warlock);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "MeleeUnit(e1)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Warlock, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_1_fixed_3_lvl(void) {
    ecs_world_t *world = ecs_mini();

    populate_facts(world);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add(world, e1, Warlock);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Unit(e1)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Warlock, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}


void ComponentInheritance_1_this_0_lvl(void) {
    ecs_world_t *world = ecs_mini();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_w(world, Warlock);
    ecs_entity_t e2 = ecs_new_w(world, Wizard);
    ecs_entity_t e3 = ecs_new_w(world, Warrior);
    ecs_entity_t e4 = ecs_new_w(world, Archer);
    ecs_entity_t e5 = ecs_new_w(world, MeleeUnit);
    ecs_entity_t e6 = ecs_new_w(world, RangedUnit);
    ecs_entity_t e7 = ecs_new_w(world, Unit);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Unit($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Warlock, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Wizard, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Warrior, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Archer, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e4, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(MeleeUnit, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e5, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(RangedUnit, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e6, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Unit, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e7, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_1_this_1_lvl(void) {
    ecs_world_t *world = ecs_mini();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_w(world, Warlock);
    /* ecs_entity_t e2 = */ ecs_new_w(world, Wizard);
    ecs_entity_t e3 = ecs_new_w(world, Warrior);
    /* ecs_entity_t e4 = */ ecs_new_w(world, Archer);
    ecs_entity_t e5 = ecs_new_w(world, MeleeUnit);
    /* ecs_entity_t e6 = */ ecs_new_w(world, RangedUnit);
    /* ecs_entity_t e7 = */ ecs_new_w(world, Unit);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "MeleeUnit($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Warlock, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Warrior, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(MeleeUnit, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e5, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_1_this_2_lvl(void) {
    ecs_world_t *world = ecs_mini();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_w(world, Warlock);
    /* ecs_entity_t e2 = */ ecs_new_w(world, Wizard);
    ecs_entity_t e3 = ecs_new_w(world, Warrior);
    /* ecs_entity_t e4 = */ ecs_new_w(world, Archer);
    /* ecs_entity_t e5 = */ ecs_new_w(world, MeleeUnit);
    /* ecs_entity_t e6 = */ ecs_new_w(world, RangedUnit);
    /* ecs_entity_t e7 = */ ecs_new_w(world, Unit);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Warrior($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Warlock, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Warrior, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_1_this_3_lvl(void) {
    ecs_world_t *world = ecs_mini();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_w(world, Warlock);
    /* ecs_entity_t e2 = */ ecs_new_w(world, Wizard);
    /* ecs_entity_t e3 = */ ecs_new_w(world, Warrior);
    /* ecs_entity_t e4 = */ ecs_new_w(world, Archer);
    /* ecs_entity_t e5 = */ ecs_new_w(world, MeleeUnit);
    /* ecs_entity_t e6 = */ ecs_new_w(world, RangedUnit);
    /* ecs_entity_t e7 = */ ecs_new_w(world, Unit);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Warlock($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Warlock, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_1_this_0_lvl_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_entity_t e6 = ecs_new_w(world, Tag);
    ecs_entity_t e7 = ecs_new_w(world, Tag);

    ecs_add(world, e1, Warlock);
    ecs_add(world, e2, Wizard);
    ecs_add(world, e3, Warrior);
    ecs_add(world, e4, Archer);
    ecs_add(world, e5, MeleeUnit);
    ecs_add(world, e6, RangedUnit);
    ecs_add(world, e7, Unit);

    /* entities that don't match query */
    ecs_new_w(world, Unit);
    ecs_new_w(world, MeleeUnit);
    ecs_new_w(world, Warrior);
    ecs_new_w(world, Warlock);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Tag($this), Unit($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Warlock, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Wizard, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Warrior, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Archer, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e4, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(MeleeUnit, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e5, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(RangedUnit, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e6, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Unit, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e7, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_1_this_1_lvl_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_entity_t e6 = ecs_new_w(world, Tag);
    ecs_entity_t e7 = ecs_new_w(world, Tag);

    ecs_add(world, e1, Warlock);
    ecs_add(world, e2, Wizard);
    ecs_add(world, e3, Warrior);
    ecs_add(world, e4, Archer);
    ecs_add(world, e5, MeleeUnit);
    ecs_add(world, e6, RangedUnit);
    ecs_add(world, e7, Unit);

    /* entities that don't match query */
    ecs_new_w(world, Unit);
    ecs_new_w(world, MeleeUnit);
    ecs_new_w(world, Warrior);
    ecs_new_w(world, Warlock);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Tag($this), MeleeUnit($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Warlock, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Warrior, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(MeleeUnit, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e5, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_1_this_2_lvl_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_entity_t e6 = ecs_new_w(world, Tag);
    ecs_entity_t e7 = ecs_new_w(world, Tag);

    ecs_add(world, e1, Warlock);
    ecs_add(world, e2, Wizard);
    ecs_add(world, e3, Warrior);
    ecs_add(world, e4, Archer);
    ecs_add(world, e5, MeleeUnit);
    ecs_add(world, e6, RangedUnit);
    ecs_add(world, e7, Unit);

    /* entities that don't match query */
    ecs_new_w(world, Unit);
    ecs_new_w(world, MeleeUnit);
    ecs_new_w(world, Warrior);
    ecs_new_w(world, Warlock);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Tag($this), Warrior($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Warlock, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Warrior, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_1_this_3_lvl_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_entity_t e6 = ecs_new_w(world, Tag);
    ecs_entity_t e7 = ecs_new_w(world, Tag);

    ecs_add(world, e1, Warlock);
    ecs_add(world, e2, Wizard);
    ecs_add(world, e3, Warrior);
    ecs_add(world, e4, Archer);
    ecs_add(world, e5, MeleeUnit);
    ecs_add(world, e6, RangedUnit);
    ecs_add(world, e7, Unit);

    /* entities that don't match query */
    ecs_new_w(world, Unit);
    ecs_new_w(world, MeleeUnit);
    ecs_new_w(world, Warrior);
    ecs_new_w(world, Warlock);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Tag($this), Warlock($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Warlock, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_1_var_0_lvl(void) {
    ecs_world_t *world = ecs_mini();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_w(world, Warlock);
    ecs_entity_t e2 = ecs_new_w(world, Wizard);
    ecs_entity_t e3 = ecs_new_w(world, Warrior);
    ecs_entity_t e4 = ecs_new_w(world, Archer);
    ecs_entity_t e5 = ecs_new_w(world, MeleeUnit);
    ecs_entity_t e6 = ecs_new_w(world, RangedUnit);
    ecs_entity_t e7 = ecs_new_w(world, Unit);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Unit($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Warlock, ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Wizard, ecs_field_id(&it, 0));
        test_uint(e2, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Warrior, ecs_field_id(&it, 0));
        test_uint(e3, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e3, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Archer, ecs_field_id(&it, 0));
        test_uint(e4, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e4, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(MeleeUnit, ecs_field_id(&it, 0));
        test_uint(e5, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e5, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(RangedUnit, ecs_field_id(&it, 0));
        test_uint(e6, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e6, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Unit, ecs_field_id(&it, 0));
        test_uint(e7, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e7, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_1_var_1_lvl(void) {
    ecs_world_t *world = ecs_mini();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_w(world, Warlock);
    /* ecs_entity_t e2 = */ ecs_new_w(world, Wizard);
    ecs_entity_t e3 = ecs_new_w(world, Warrior);
    /* ecs_entity_t e4 = */ ecs_new_w(world, Archer);
    ecs_entity_t e5 = ecs_new_w(world, MeleeUnit);
    /* ecs_entity_t e6 = */ ecs_new_w(world, RangedUnit);
    /* ecs_entity_t e7 = */ ecs_new_w(world, Unit);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "MeleeUnit($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Warlock, ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Warrior, ecs_field_id(&it, 0));
        test_uint(e3, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e3, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(MeleeUnit, ecs_field_id(&it, 0));
        test_uint(e5, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e5, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_1_var_2_lvl(void) {
    ecs_world_t *world = ecs_mini();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_w(world, Warlock);
    /* ecs_entity_t e2 = */ ecs_new_w(world, Wizard);
    ecs_entity_t e3 = ecs_new_w(world, Warrior);
    /* ecs_entity_t e4 = */ ecs_new_w(world, Archer);
    /* ecs_entity_t e5 = */ ecs_new_w(world, MeleeUnit);
    /* ecs_entity_t e6 = */ ecs_new_w(world, RangedUnit);
    /* ecs_entity_t e7 = */ ecs_new_w(world, Unit);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Warrior($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Warlock, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Warrior, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e3, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_1_var_3_lvl(void) {
    ecs_world_t *world = ecs_mini();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_w(world, Warlock);
    /* ecs_entity_t e2 = */ ecs_new_w(world, Wizard);
    /* ecs_entity_t e3 = */ ecs_new_w(world, Warrior);
    /* ecs_entity_t e4 = */ ecs_new_w(world, Archer);
    /* ecs_entity_t e5 = */ ecs_new_w(world, MeleeUnit);
    /* ecs_entity_t e6 = */ ecs_new_w(world, RangedUnit);
    /* ecs_entity_t e7 = */ ecs_new_w(world, Unit);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Warlock($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Warlock, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_1_var_0_lvl_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_entity_t e6 = ecs_new_w(world, Tag);
    ecs_entity_t e7 = ecs_new_w(world, Tag);

    ecs_add(world, e1, Warlock);
    ecs_add(world, e2, Wizard);
    ecs_add(world, e3, Warrior);
    ecs_add(world, e4, Archer);
    ecs_add(world, e5, MeleeUnit);
    ecs_add(world, e6, RangedUnit);
    ecs_add(world, e7, Unit);

    /* entities that don't match query */
    ecs_new_w(world, Unit);
    ecs_new_w(world, MeleeUnit);
    ecs_new_w(world, Warrior);
    ecs_new_w(world, Warlock);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Tag($x), Unit($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Warlock, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Wizard, ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_src(&it, 0));
        test_uint(e2, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Warrior, ecs_field_id(&it, 1));
        test_uint(e3, ecs_field_src(&it, 0));
        test_uint(e3, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e3, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Archer, ecs_field_id(&it, 1));
        test_uint(e4, ecs_field_src(&it, 0));
        test_uint(e4, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e4, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(MeleeUnit, ecs_field_id(&it, 1));
        test_uint(e5, ecs_field_src(&it, 0));
        test_uint(e5, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e5, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(RangedUnit, ecs_field_id(&it, 1));
        test_uint(e6, ecs_field_src(&it, 0));
        test_uint(e6, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e6, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Unit, ecs_field_id(&it, 1));
        test_uint(e7, ecs_field_src(&it, 0));
        test_uint(e7, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e7, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_1_var_1_lvl_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_entity_t e6 = ecs_new_w(world, Tag);
    ecs_entity_t e7 = ecs_new_w(world, Tag);

    ecs_add(world, e1, Warlock);
    ecs_add(world, e2, Wizard);
    ecs_add(world, e3, Warrior);
    ecs_add(world, e4, Archer);
    ecs_add(world, e5, MeleeUnit);
    ecs_add(world, e6, RangedUnit);
    ecs_add(world, e7, Unit);

    /* entities that don't match query */
    ecs_new_w(world, Unit);
    ecs_new_w(world, MeleeUnit);
    ecs_new_w(world, Warrior);
    ecs_new_w(world, Warlock);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Tag($x), MeleeUnit($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Warlock, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Warrior, ecs_field_id(&it, 1));
        test_uint(e3, ecs_field_src(&it, 0));
        test_uint(e3, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e3, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(MeleeUnit, ecs_field_id(&it, 1));
        test_uint(e5, ecs_field_src(&it, 0));
        test_uint(e5, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e5, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_1_var_2_lvl_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_entity_t e6 = ecs_new_w(world, Tag);
    ecs_entity_t e7 = ecs_new_w(world, Tag);

    ecs_add(world, e1, Warlock);
    ecs_add(world, e2, Wizard);
    ecs_add(world, e3, Warrior);
    ecs_add(world, e4, Archer);
    ecs_add(world, e5, MeleeUnit);
    ecs_add(world, e6, RangedUnit);
    ecs_add(world, e7, Unit);

    /* entities that don't match query */
    ecs_new_w(world, Unit);
    ecs_new_w(world, MeleeUnit);
    ecs_new_w(world, Warrior);
    ecs_new_w(world, Warlock);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Tag($x), Warrior($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Warlock, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Warrior, ecs_field_id(&it, 1));
        test_uint(e3, ecs_field_src(&it, 0));
        test_uint(e3, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e3, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_1_var_3_lvl_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_entity_t e6 = ecs_new_w(world, Tag);
    ecs_entity_t e7 = ecs_new_w(world, Tag);

    ecs_add(world, e1, Warlock);
    ecs_add(world, e2, Wizard);
    ecs_add(world, e3, Warrior);
    ecs_add(world, e4, Archer);
    ecs_add(world, e5, MeleeUnit);
    ecs_add(world, e6, RangedUnit);
    ecs_add(world, e7, Unit);

    /* entities that don't match query */
    ecs_new_w(world, Unit);
    ecs_new_w(world, MeleeUnit);
    ecs_new_w(world, Warrior);
    ecs_new_w(world, Warlock);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Tag($x), Warlock($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Warlock, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_1_fixed_1_lvl_self(void) {
    ecs_world_t *world = ecs_mini();

    populate_facts(world);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add(world, e1, Warlock);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Warrior(e1|self)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Warlock, ecs_field_id(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_1_this_1_lvl_self(void) {
    ecs_world_t *world = ecs_mini();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_w(world, Warlock);
    /* ecs_entity_t e2 = */ ecs_new_w(world, Wizard);
    ecs_entity_t e3 = ecs_new_w(world, Warrior);
    /* ecs_entity_t e4 = */ ecs_new_w(world, Archer);
    ecs_entity_t e5 = ecs_new_w(world, MeleeUnit);
    /* ecs_entity_t e6 = */ ecs_new_w(world, RangedUnit);
    /* ecs_entity_t e7 = */ ecs_new_w(world, Unit);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "MeleeUnit(self)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Warlock, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Warrior, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(MeleeUnit, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e5, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_1_this_1_lvl_written_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_entity_t e6 = ecs_new_w(world, Tag);
    ecs_entity_t e7 = ecs_new_w(world, Tag);

    ecs_add(world, e1, Warlock);
    ecs_add(world, e2, Wizard);
    ecs_add(world, e3, Warrior);
    ecs_add(world, e4, Archer);
    ecs_add(world, e5, MeleeUnit);
    ecs_add(world, e6, RangedUnit);
    ecs_add(world, e7, Unit);

    /* entities that don't match query */
    ecs_new_w(world, Unit);
    ecs_new_w(world, MeleeUnit);
    ecs_new_w(world, Warrior);
    ecs_new_w(world, Warlock);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Tag(self), MeleeUnit(self)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Warlock, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Warrior, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(MeleeUnit, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 0));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e5, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_1_var_1_lvl_self(void) {
    ecs_world_t *world = ecs_mini();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_w(world, Warlock);
    /* ecs_entity_t e2 = */ ecs_new_w(world, Wizard);
    ecs_entity_t e3 = ecs_new_w(world, Warrior);
    /* ecs_entity_t e4 = */ ecs_new_w(world, Archer);
    ecs_entity_t e5 = ecs_new_w(world, MeleeUnit);
    /* ecs_entity_t e6 = */ ecs_new_w(world, RangedUnit);
    /* ecs_entity_t e7 = */ ecs_new_w(world, Unit);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "MeleeUnit($x|self)"
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Warlock, ecs_field_id(&it, 0));
        test_uint(e1, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Warrior, ecs_field_id(&it, 0));
        test_uint(e3, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e3, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(MeleeUnit, ecs_field_id(&it, 0));
        test_uint(e5, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_uint(e5, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_1_var_1_lvl_written_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_entity_t e6 = ecs_new_w(world, Tag);
    ecs_entity_t e7 = ecs_new_w(world, Tag);

    ecs_add(world, e1, Warlock);
    ecs_add(world, e2, Wizard);
    ecs_add(world, e3, Warrior);
    ecs_add(world, e4, Archer);
    ecs_add(world, e5, MeleeUnit);
    ecs_add(world, e6, RangedUnit);
    ecs_add(world, e7, Unit);

    /* entities that don't match query */
    ecs_new_w(world, Unit);
    ecs_new_w(world, MeleeUnit);
    ecs_new_w(world, Warrior);
    ecs_new_w(world, Warlock);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Tag($x), MeleeUnit($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Warlock, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 0));
        test_uint(e1, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Warrior, ecs_field_id(&it, 1));
        test_uint(e3, ecs_field_src(&it, 0));
        test_uint(e3, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e3, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(MeleeUnit, ecs_field_id(&it, 1));
        test_uint(e5, ecs_field_src(&it, 0));
        test_uint(e5, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e5, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_1_fixed_src_not(void) {
    ecs_world_t *world = ecs_mini();

    populate_facts(world);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add(world, e1, Warrior);

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "!Unit(e1)" });
        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "!MeleeUnit(e1)" });
        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "!Warrior(e1)" });
        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "!Warlock(e1)" });
        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Warlock, ecs_field_id(&it, 0));
            test_uint(e1, ecs_field_src(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "!RangedUnit(e1)" });
        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(RangedUnit, ecs_field_id(&it, 0));
            test_uint(e1, ecs_field_src(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "!Archer(e1)" });
        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Archer, ecs_field_id(&it, 0));
            test_uint(e1, ecs_field_src(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "!Wizard(e1)" });
        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Wizard, ecs_field_id(&it, 0));
            test_uint(e1, ecs_field_src(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    ecs_fini(world);
}

void ComponentInheritance_1_this_src_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_entity_t e6 = ecs_new_w(world, Tag);
    ecs_entity_t e7 = ecs_new_w(world, Tag);

    ecs_add(world, e1, Unit);
    ecs_add(world, e2, MeleeUnit);
    ecs_add(world, e3, RangedUnit);
    ecs_add(world, e4, Warrior);
    ecs_add(world, e5, Archer);
    ecs_add(world, e6, Wizard);
    ecs_add(world, e7, Warlock);

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "!Unit($this), Tag($this)" });
        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "!MeleeUnit($this), Tag($this)" });
        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(MeleeUnit, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e1, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(MeleeUnit, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e3, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(MeleeUnit, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e5, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(MeleeUnit, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e6, it.entities[0]);

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "!Warrior($this), Tag($this)" });
        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Warrior, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e1, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Warrior, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e2, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Warrior, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e3, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Warrior, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e5, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Warrior, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e6, it.entities[0]);

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "!Warlock($this), Tag($this)" });
        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Warlock, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e1, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Warlock, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e2, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Warlock, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e3, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Warlock, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e4, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Warlock, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e5, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Warlock, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e6, it.entities[0]);

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "!RangedUnit($this), Tag($this)" });
        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(RangedUnit, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e1, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(RangedUnit, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e2, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(RangedUnit, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e4, it.entities[0]);

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "!Archer($this), Tag($this)" });
        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Archer, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e1, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Archer, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e2, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Archer, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e3, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Archer, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e4, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Archer, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e6, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Archer, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e7, it.entities[0]);

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "!Wizard($this), Tag($this)" });
        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Wizard, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e1, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Wizard, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e2, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Wizard, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e3, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Wizard, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e4, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Wizard, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e5, it.entities[0]);

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    ecs_fini(world);
}

void ComponentInheritance_1_var_src_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_entity_t e6 = ecs_new_w(world, Tag);
    ecs_entity_t e7 = ecs_new_w(world, Tag);

    ecs_add(world, e1, Unit);
    ecs_add(world, e2, MeleeUnit);
    ecs_add(world, e3, RangedUnit);
    ecs_add(world, e4, Warrior);
    ecs_add(world, e5, Archer);
    ecs_add(world, e6, Wizard);
    ecs_add(world, e7, Warlock);

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "!Unit($x), Tag($x)" });
        test_assert(r != NULL);

        int x_var = ecs_query_find_var(r, "x");
        test_assert(x_var != -1);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "!MeleeUnit($x), Tag($x)" });
        test_assert(r != NULL);

        int x_var = ecs_query_find_var(r, "x");
        test_assert(x_var != -1);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(MeleeUnit, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e1, ecs_field_src(&it, 0));
            test_uint(e1, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e1, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(MeleeUnit, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e3, ecs_field_src(&it, 0));
            test_uint(e3, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e3, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(MeleeUnit, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e5, ecs_field_src(&it, 0));
            test_uint(e5, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e5, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(MeleeUnit, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e6, ecs_field_src(&it, 0));
            test_uint(e6, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e6, ecs_iter_get_var(&it, x_var));

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "!Warrior($x), Tag($x)" });
        test_assert(r != NULL);

        int x_var = ecs_query_find_var(r, "x");
        test_assert(x_var != -1);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Warrior, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e1, ecs_field_src(&it, 0));
            test_uint(e1, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e1, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Warrior, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e2, ecs_field_src(&it, 0));
            test_uint(e2, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e2, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Warrior, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e3, ecs_field_src(&it, 0));
            test_uint(e3, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e3, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Warrior, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e5, ecs_field_src(&it, 0));
            test_uint(e5, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e5, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Warrior, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e6, ecs_field_src(&it, 0));
            test_uint(e6, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e6, ecs_iter_get_var(&it, x_var));

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "!Warlock($x), Tag($x)" });
        test_assert(r != NULL);

        int x_var = ecs_query_find_var(r, "x");
        test_assert(x_var != -1);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Warlock, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e1, ecs_field_src(&it, 0));
            test_uint(e1, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e1, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Warlock, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e2, ecs_field_src(&it, 0));
            test_uint(e2, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e2, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Warlock, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e3, ecs_field_src(&it, 0));
            test_uint(e3, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e3, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Warlock, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e4, ecs_field_src(&it, 0));
            test_uint(e4, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e4, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Warlock, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e5, ecs_field_src(&it, 0));
            test_uint(e5, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e5, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Warlock, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e6, ecs_field_src(&it, 0));
            test_uint(e6, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e6, ecs_iter_get_var(&it, x_var));

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "!RangedUnit($x), Tag($x)" });
        test_assert(r != NULL);

        int x_var = ecs_query_find_var(r, "x");
        test_assert(x_var != -1);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(RangedUnit, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e1, ecs_field_src(&it, 0));
            test_uint(e1, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e1, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(RangedUnit, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e2, ecs_field_src(&it, 0));
            test_uint(e2, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e2, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(RangedUnit, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e4, ecs_field_src(&it, 0));
            test_uint(e4, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e4, ecs_iter_get_var(&it, x_var));

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "!Archer($x), Tag($x)" });
        test_assert(r != NULL);

        int x_var = ecs_query_find_var(r, "x");
        test_assert(x_var != -1);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Archer, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e1, ecs_field_src(&it, 0));
            test_uint(e1, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e1, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Archer, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e2, ecs_field_src(&it, 0));
            test_uint(e2, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e2, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Archer, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e3, ecs_field_src(&it, 0));
            test_uint(e3, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e3, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Archer, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e4, ecs_field_src(&it, 0));
            test_uint(e4, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e4, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Archer, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e6, ecs_field_src(&it, 0));
            test_uint(e6, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e6, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Archer, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e7, ecs_field_src(&it, 0));
            test_uint(e7, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e7, ecs_iter_get_var(&it, x_var));

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "!Wizard($x), Tag($x)" });
        test_assert(r != NULL);

        int x_var = ecs_query_find_var(r, "x");
        test_assert(x_var != -1);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Wizard, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e1, ecs_field_src(&it, 0));
            test_uint(e1, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e1, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Wizard, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e2, ecs_field_src(&it, 0));
            test_uint(e2, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e2, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Wizard, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e3, ecs_field_src(&it, 0));
            test_uint(e3, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e3, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Wizard, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e4, ecs_field_src(&it, 0));
            test_uint(e4, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e4, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Wizard, ecs_field_id(&it, 0));
            test_uint(Tag, ecs_field_id(&it, 1));
            test_uint(e5, ecs_field_src(&it, 0));
            test_uint(e5, ecs_field_src(&it, 1));
            test_bool(false, ecs_field_is_set(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 1));
            test_uint(e5, ecs_iter_get_var(&it, x_var));

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    ecs_fini(world);
}

void ComponentInheritance_1_this_src_not_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_entity_t e6 = ecs_new_w(world, Tag);
    ecs_entity_t e7 = ecs_new_w(world, Tag);

    ecs_add(world, e1, Unit);
    ecs_add(world, e2, MeleeUnit);
    ecs_add(world, e3, RangedUnit);
    ecs_add(world, e4, Warrior);
    ecs_add(world, e5, Archer);
    ecs_add(world, e6, Wizard);
    ecs_add(world, e7, Warlock);

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Tag($this), !Unit($this)" });
        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Tag($this), !MeleeUnit($this)" });
        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(e1, it.entities[0]);
            test_uint(Tag, ecs_field_id(&it, 0));

            test_uint(MeleeUnit, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(e3, it.entities[0]);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(MeleeUnit, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(e5, it.entities[0]);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(MeleeUnit, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(e6, it.entities[0]);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(MeleeUnit, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Tag($this), !Warrior($this)" });
        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Warrior, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e1, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Warrior, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e2, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Warrior, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e3, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Warrior, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e5, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Warrior, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e6, it.entities[0]);

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Tag($this), !Warlock($this)" });
        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Warlock, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e1, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Warlock, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e2, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Warlock, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e3, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Warlock, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e4, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Warlock, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e5, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Warlock, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e6, it.entities[0]);

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Tag($this), !RangedUnit($this)" });
        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(RangedUnit, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e1, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(RangedUnit, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e2, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(RangedUnit, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e4, it.entities[0]);

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Tag($this), !Archer($this)" });
        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Archer, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e1, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Archer, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e2, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Archer, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e3, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Archer, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e4, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Archer, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e6, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Archer, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e7, it.entities[0]);

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Tag($this), !Wizard($this)" });
        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Wizard, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e1, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Wizard, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e2, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Wizard, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e3, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Wizard, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e4, it.entities[0]);

            test_bool(true, ecs_query_next(&it));
            test_uint(1, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Wizard, ecs_field_id(&it, 1));
            test_uint(0, ecs_field_src(&it, 0));
            test_uint(0, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e5, it.entities[0]);

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    ecs_fini(world);
}

void ComponentInheritance_1_var_src_not_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_entity_t e4 = ecs_new_w(world, Tag);
    ecs_entity_t e5 = ecs_new_w(world, Tag);
    ecs_entity_t e6 = ecs_new_w(world, Tag);
    ecs_entity_t e7 = ecs_new_w(world, Tag);

    ecs_add(world, e1, Unit);
    ecs_add(world, e2, MeleeUnit);
    ecs_add(world, e3, RangedUnit);
    ecs_add(world, e4, Warrior);
    ecs_add(world, e5, Archer);
    ecs_add(world, e6, Wizard);
    ecs_add(world, e7, Warlock);

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Tag($x), !Unit($x)" });
        test_assert(r != NULL);

        int x_var = ecs_query_find_var(r, "x");
        test_assert(x_var != -1);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Tag($x), !MeleeUnit($x)" });
        test_assert(r != NULL);

        int x_var = ecs_query_find_var(r, "x");
        test_assert(x_var != -1);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(MeleeUnit, ecs_field_id(&it, 1));
            test_uint(e1, ecs_field_src(&it, 0));
            test_uint(e1, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e1, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(MeleeUnit, ecs_field_id(&it, 1));
            test_uint(e3, ecs_field_src(&it, 0));
            test_uint(e3, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e3, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(MeleeUnit, ecs_field_id(&it, 1));
            test_uint(e5, ecs_field_src(&it, 0));
            test_uint(e5, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e5, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(MeleeUnit, ecs_field_id(&it, 1));
            test_uint(e6, ecs_field_src(&it, 0));
            test_uint(e6, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e6, ecs_iter_get_var(&it, x_var));

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Tag($x), !Warrior($x)" });
        test_assert(r != NULL);

        int x_var = ecs_query_find_var(r, "x");
        test_assert(x_var != -1);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Warrior, ecs_field_id(&it, 1));
            test_uint(e1, ecs_field_src(&it, 0));
            test_uint(e1, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e1, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Warrior, ecs_field_id(&it, 1));
            test_uint(e2, ecs_field_src(&it, 0));
            test_uint(e2, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e2, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Warrior, ecs_field_id(&it, 1));
            test_uint(e3, ecs_field_src(&it, 0));
            test_uint(e3, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e3, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Warrior, ecs_field_id(&it, 1));
            test_uint(e5, ecs_field_src(&it, 0));
            test_uint(e5, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e5, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Warrior, ecs_field_id(&it, 1));
            test_uint(e6, ecs_field_src(&it, 0));
            test_uint(e6, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e6, ecs_iter_get_var(&it, x_var));

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Tag($x), !Warlock($x)" });
        test_assert(r != NULL);

        int x_var = ecs_query_find_var(r, "x");
        test_assert(x_var != -1);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Warlock, ecs_field_id(&it, 1));
            test_uint(e1, ecs_field_src(&it, 0));
            test_uint(e1, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e1, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Warlock, ecs_field_id(&it, 1));
            test_uint(e2, ecs_field_src(&it, 0));
            test_uint(e2, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e2, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Warlock, ecs_field_id(&it, 1));
            test_uint(e3, ecs_field_src(&it, 0));
            test_uint(e3, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e3, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Warlock, ecs_field_id(&it, 1));
            test_uint(e4, ecs_field_src(&it, 0));
            test_uint(e4, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e4, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Warlock, ecs_field_id(&it, 1));
            test_uint(e5, ecs_field_src(&it, 0));
            test_uint(e5, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e5, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Warlock, ecs_field_id(&it, 1));
            test_uint(e6, ecs_field_src(&it, 0));
            test_uint(e6, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e6, ecs_iter_get_var(&it, x_var));

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Tag($x), !RangedUnit($x)" });
        test_assert(r != NULL);

        int x_var = ecs_query_find_var(r, "x");
        test_assert(x_var != -1);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(RangedUnit, ecs_field_id(&it, 1));
            test_uint(e1, ecs_field_src(&it, 0));
            test_uint(e1, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e1, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(RangedUnit, ecs_field_id(&it, 1));
            test_uint(e2, ecs_field_src(&it, 0));
            test_uint(e2, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e2, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(RangedUnit, ecs_field_id(&it, 1));
            test_uint(e4, ecs_field_src(&it, 0));
            test_uint(e4, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e4, ecs_iter_get_var(&it, x_var));

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Tag($x), !Archer($x)" });
        test_assert(r != NULL);

        int x_var = ecs_query_find_var(r, "x");
        test_assert(x_var != -1);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Archer, ecs_field_id(&it, 1));
            test_uint(e1, ecs_field_src(&it, 0));
            test_uint(e1, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e1, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Archer, ecs_field_id(&it, 1));
            test_uint(e2, ecs_field_src(&it, 0));
            test_uint(e2, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e2, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Archer, ecs_field_id(&it, 1));
            test_uint(e3, ecs_field_src(&it, 0));
            test_uint(e3, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e3, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Archer, ecs_field_id(&it, 1));
            test_uint(e4, ecs_field_src(&it, 0));
            test_uint(e4, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e4, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Archer, ecs_field_id(&it, 1));
            test_uint(e6, ecs_field_src(&it, 0));
            test_uint(e6, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e6, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Archer, ecs_field_id(&it, 1));
            test_uint(e7, ecs_field_src(&it, 0));
            test_uint(e7, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e7, ecs_iter_get_var(&it, x_var));

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Tag($x), !Wizard($x)" });
        test_assert(r != NULL);

        int x_var = ecs_query_find_var(r, "x");
        test_assert(x_var != -1);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Wizard, ecs_field_id(&it, 1));
            test_uint(e1, ecs_field_src(&it, 0));
            test_uint(e1, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e1, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Wizard, ecs_field_id(&it, 1));
            test_uint(e2, ecs_field_src(&it, 0));
            test_uint(e2, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e2, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Wizard, ecs_field_id(&it, 1));
            test_uint(e3, ecs_field_src(&it, 0));
            test_uint(e3, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e3, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Wizard, ecs_field_id(&it, 1));
            test_uint(e4, ecs_field_src(&it, 0));
            test_uint(e4, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e4, ecs_iter_get_var(&it, x_var));

            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Tag, ecs_field_id(&it, 0));
            test_uint(Wizard, ecs_field_id(&it, 1));
            test_uint(e5, ecs_field_src(&it, 0));
            test_uint(e5, ecs_field_src(&it, 1));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_bool(false, ecs_field_is_set(&it, 1));
            test_uint(e5, ecs_iter_get_var(&it, x_var));

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    ecs_fini(world);
}

void ComponentInheritance_first_self(void) {
    ecs_world_t *world = ecs_mini();

    populate_facts(world);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add(world, e1, Warlock);
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_add(world, e2, Warrior);

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
            .expr = "Warrior|self(e1)"
        });

        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Warlock, ecs_field_id(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_uint(e1, ecs_field_src(&it, 0));

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
            .expr = "Warlock|self(e1)"
        });

        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Warlock, ecs_field_id(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_uint(e1, ecs_field_src(&it, 0));

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    {
        ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
            .expr = "Warrior|self(e2)"
        });

        test_assert(r != NULL);

        {
            ecs_iter_t it = ecs_query_iter(world, r);
            test_bool(true, ecs_query_next(&it));
            test_uint(0, it.count);
            test_uint(Warrior, ecs_field_id(&it, 0));
            test_bool(true, ecs_field_is_set(&it, 0));
            test_uint(e2, ecs_field_src(&it, 0));

            test_bool(false, ecs_query_next(&it));
        }

        ecs_query_fini(r);
    }

    ecs_fini(world);
}

void ComponentInheritance_inheritable_trait(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Unit, Inheritable);
    test_assert(ecs_has_id(world, Unit, EcsInheritable));

    ecs_query_t *q = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Unit"
    });

    test_assert(q != NULL);

    ECS_ENTITY(world, Warrior, (IsA, Unit));
    ECS_ENTITY(world, Wizard, (IsA, Unit));

    ecs_entity_t e1 = ecs_new_w(world, Unit);
    ecs_entity_t e2 = ecs_new_w(world, Warrior);
    ecs_entity_t e3 = ecs_new_w(world, Wizard);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(Unit, ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_uint(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(Warrior, ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_uint(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(Wizard, ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void ComponentInheritance_query_before_isa_relationship_1st_term(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);

    ecs_query_t *q = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Unit"
    });

    test_assert(q != NULL);

    test_expect_abort();

    ECS_ENTITY(world, Warrior, (IsA, Unit));
}

void ComponentInheritance_query_before_isa_relationship_2nd_term(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Foo, Unit"
    });

    test_assert(q != NULL);

    test_expect_abort();

    ECS_ENTITY(world, Warrior, (IsA, Unit));
}

void ComponentInheritance_query_before_isa_relationship_subtype(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_ENTITY(world, MeleeUnit, (IsA, Unit));

    ecs_query_t *q = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "MeleeUnit"
    });

    test_assert(q != NULL);

    ECS_ENTITY(world, Warrior, (IsA, MeleeUnit));

    ecs_new_w(world, Unit);
    ecs_entity_t e2 = ecs_new_w(world, MeleeUnit);
    ecs_entity_t e3 = ecs_new_w(world, Warrior);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(MeleeUnit, ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_uint(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(Warrior, ecs_field_id(&it, 0));
    test_uint(0, ecs_field_src(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void ComponentInheritance_query_before_isa_relationship_0_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Unit);

    ecs_query_t *q = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Foo, Unit()"
    });

    test_assert(q != NULL);

    ECS_ENTITY(world, Warrior, (IsA, Unit));

    ecs_entity_t e1 = ecs_new_w(world, Foo);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(Foo, ecs_field_id(&it, 0));
    test_uint(Unit, ecs_field_id(&it, 1));
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(0, ecs_field_src(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void ComponentInheritance_populated_remove_isa_2_lvl(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t Unit = ecs_entity(world, { .name = "Unit" });
    ecs_entity_t Warrior = ecs_entity(world, { .name = "Warrior" });
    ecs_add_pair(world, Warrior, EcsIsA, Unit);

    ecs_entity_t e1 = ecs_new_w_id(world, Warrior);
    ecs_entity_t e2 = ecs_new_w_id(world, Unit);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Unit($this)" });
    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Warrior, ecs_field_id(&it, 0));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Unit, ecs_field_id(&it, 0));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    test_expect_abort();
    ecs_remove_pair(world, Warrior, EcsIsA, Unit);
}

void ComponentInheritance_populated_remove_isa_3_lvl(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t Unit = ecs_entity(world, { .name = "Unit" });
    ecs_entity_t Melee = ecs_entity(world, { .name = "Melee" });
    ecs_entity_t Warrior = ecs_entity(world, { .name = "Warrior" });
    ecs_add_pair(world, Melee, EcsIsA, Unit);
    ecs_add_pair(world, Warrior, EcsIsA, Melee);

    ecs_entity_t e1 = ecs_new_w_id(world, Warrior);
    ecs_entity_t e2 = ecs_new_w_id(world, Melee);
    ecs_entity_t e3 = ecs_new_w_id(world, Unit);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Unit($this)" });
    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(Warrior, ecs_field_id(&it, 0));
        test_uint(e1, it.entities[0]);
        test_bool(true, ecs_query_next(&it));
        test_uint(Melee, ecs_field_id(&it, 0));
        test_uint(e2, it.entities[0]);
        test_bool(true, ecs_query_next(&it));
        test_uint(Unit, ecs_field_id(&it, 0));
        test_uint(e3, it.entities[0]);
        test_bool(false, ecs_query_next(&it));
    }

    test_expect_abort();
    ecs_remove_pair(world, Melee, EcsIsA, Unit);
}

void ComponentInheritance_populated_match_entities_added_after(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Unit = ecs_entity(world, { .name = "Unit" });
    ecs_entity_t Warrior = ecs_entity(world, { .name = "Warrior" });
    ecs_add_pair(world, Warrior, EcsIsA, Unit);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Unit($this)" });
    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(false, ecs_query_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_id(world, Warrior);
    ecs_entity_t e2 = ecs_new_w_id(world, Unit);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Warrior, ecs_field_id(&it, 0));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(Unit, ecs_field_id(&it, 0));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_populated_remove_isa_var(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t Unit = ecs_entity(world, { .name = "Unit" });
    ecs_entity_t Warrior = ecs_entity(world, { .name = "Warrior" });
    ecs_add_pair(world, Warrior, EcsIsA, Unit);

    ecs_entity_t e1 = ecs_new_w_id(world, Warrior);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Unit($x)" });
    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(Warrior, ecs_field_id(&it, 0));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_bool(false, ecs_query_next(&it));
    }

    test_expect_abort();
    ecs_remove_pair(world, Warrior, EcsIsA, Unit);
}

void ComponentInheritance_optional_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Unit = ecs_entity(world, { .name = "Unit" });
    ecs_entity_t Warrior = ecs_entity(world, { .name = "Warrior" });
    ecs_entity_t Tag = ecs_entity(world, { .name = "Tag" });
    ecs_add_pair(world, Warrior, EcsIsA, Unit);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add_id(world, e1, Tag);
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_add_id(world, e2, Tag);
    ecs_add_id(world, e2, Warrior);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Tag, ?Unit" });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_query_iter(world, r);

    test_bool(true, ecs_query_next(&it));
    test_uint(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_uint(Unit, ecs_field_id(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_uint(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_uint(Warrior, ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);
    ecs_fini(world);
}

void ComponentInheritance_or_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Unit = ecs_entity(world, { .name = "Unit" });
    ecs_entity_t Warrior = ecs_entity(world, { .name = "Warrior" });
    ecs_entity_t Other = ecs_entity(world, { .name = "Other" });
    ecs_add_pair(world, Warrior, EcsIsA, Unit);

    ecs_entity_t e1 = ecs_new_w_id(world, Warrior);
    ecs_entity_t e2 = ecs_new_w_id(world, Other);
    ecs_entity_t e3 = ecs_new_w_id(world, Unit);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Unit || Other" });
    test_assert(r != NULL);

    int32_t count = 0;
    bool found_e1 = false, found_e2 = false, found_e3 = false;
    ecs_iter_t it = ecs_query_iter(world, r);
    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            ecs_entity_t e = it.entities[i];
            count ++;
            if (e == e1) {
                found_e1 = true;
                test_uint(Warrior, ecs_field_id(&it, 0));
            }
            if (e == e2) {
                found_e2 = true;
                test_uint(Other, ecs_field_id(&it, 0));
            }
            if (e == e3) {
                found_e3 = true;
                test_uint(Unit, ecs_field_id(&it, 0));
            }
        }
    }

    test_int(count, 3);
    test_bool(found_e1, true);
    test_bool(found_e2, true);
    test_bool(found_e3, true);

    ecs_query_fini(r);
    ecs_fini(world);
}

void ComponentInheritance_multiple_inherited_terms(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Unit = ecs_entity(world, { .name = "Unit" });
    ecs_entity_t Caster = ecs_entity(world, { .name = "Caster" });
    ecs_entity_t Warrior = ecs_entity(world, { .name = "Warrior" });
    ecs_entity_t Mage = ecs_entity(world, { .name = "Mage" });
    ecs_add_pair(world, Warrior, EcsIsA, Unit);
    ecs_add_pair(world, Mage, EcsIsA, Caster);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add_id(world, e1, Warrior);
    ecs_add_id(world, e1, Mage);

    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_add_id(world, e2, Warrior);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Unit, Caster" });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_uint(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(Warrior, ecs_field_id(&it, 0));
    test_uint(Mage, ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);
    ecs_fini(world);
}

void ComponentInheritance_up_traversal_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Unit = ecs_entity(world, { .name = "Unit" });
    ecs_add_pair(world, Unit, EcsOnInstantiate, EcsInherit);
    ecs_entity_t Warrior = ecs_entity(world, { .name = "Warrior" });
    ecs_add_pair(world, Warrior, EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, Warrior, EcsIsA, Unit);

    ecs_entity_t prefab = ecs_entity(world, { .name = "prefab" });
    ecs_add_id(world, prefab, EcsPrefab);
    ecs_add_id(world, prefab, Warrior);

    ecs_entity_t inst = ecs_entity(world, { .name = "inst" });
    ecs_add_pair(world, inst, EcsIsA, prefab);

    test_bool(ecs_owns_id(world, inst, Warrior), false);
    test_bool(ecs_has_id(world, inst, Warrior), true);
    test_bool(ecs_has_id(world, inst, Unit), true);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .terms[0] = { .id = Unit, .src.id = EcsSelf|EcsUp, .trav = EcsIsA }
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_uint(1, it.count);
    test_uint(inst, it.entities[0]);
    test_uint(Warrior, ecs_field_id(&it, 0));
    test_uint(prefab, ecs_field_src(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);
    ecs_fini(world);
}

void ComponentInheritance_multi_derived_this(void) {
    ecs_world_t *world = ecs_mini();

    populate_facts(world);

    ecs_entity_t em = ecs_new_w(world, Warrior);
    ecs_add(world, em, Wizard);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Unit($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(em, it.entities[0]);
        test_uint(Warrior, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(em, it.entities[0]);
        test_uint(Wizard, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_multi_derived_this_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t em = ecs_new_w(world, Tag);
    ecs_add(world, em, Warrior);
    ecs_add(world, em, Wizard);

    ecs_new_w(world, Warrior);
    ecs_new_w(world, Wizard);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Tag($this), Unit($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(em, it.entities[0]);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Warrior, ecs_field_id(&it, 1));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(em, it.entities[0]);
        test_uint(Tag, ecs_field_id(&it, 0));
        test_uint(Wizard, ecs_field_id(&it, 1));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_multi_derived_fixed(void) {
    ecs_world_t *world = ecs_mini();

    populate_facts(world);

    ecs_entity_t em = ecs_entity(world, { .name = "em" });
    ecs_add(world, em, Warrior);
    ecs_add(world, em, Wizard);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Unit(em)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Warrior, ecs_field_id(&it, 0));
        test_uint(em, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Wizard, ecs_field_id(&it, 0));
        test_uint(em, ecs_field_src(&it, 0));
        test_bool(true, ecs_field_is_set(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_multi_derived_var(void) {
    ecs_world_t *world = ecs_mini();

    populate_facts(world);

    ecs_entity_t em = ecs_new_w(world, Warrior);
    ecs_add(world, em, Wizard);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Unit($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Warrior, ecs_field_id(&it, 0));
        test_uint(em, ecs_field_src(&it, 0));
        test_uint(em, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_query_next(&it));
        test_uint(0, it.count);
        test_uint(Wizard, ecs_field_id(&it, 0));
        test_uint(em, ecs_field_src(&it, 0));
        test_uint(em, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_multi_derived_mid_level_once(void) {
    ecs_world_t *world = ecs_mini();

    populate_facts(world);

    ecs_entity_t em = ecs_new_w(world, Warrior);
    ecs_add(world, em, Wizard);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "MeleeUnit($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(em, it.entities[0]);
        test_uint(Warrior, ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_multi_derived_count_results(void) {
    ecs_world_t *world = ecs_mini();

    populate_facts(world);

    ecs_entity_t e_single = ecs_new_w(world, Warrior);
    ecs_entity_t em = ecs_new_w(world, Warrior);
    ecs_add(world, em, Wizard);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "Unit($this)"
    });

    test_assert(r != NULL);

    int32_t single_matches = 0, multi_matches = 0;

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        while (ecs_query_next(&it)) {
            int32_t i;
            for (i = 0; i < it.count; i ++) {
                if (it.entities[i] == e_single) single_matches ++;
                if (it.entities[i] == em) multi_matches ++;
            }
        }
    }

    test_int(single_matches, 1);
    test_int(multi_matches, 2);

    ecs_query_fini(r);

    ecs_fini(world);
}


void ComponentInheritance_1_this_pair(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Likes = ecs_entity(world, { .name = "Likes" });
    ecs_entity_t Loves = ecs_entity(world, { .name = "Loves" });
    ecs_entity_t Apples = ecs_entity(world, { .name = "Apples" });
    ecs_add_pair(world, Loves, EcsIsA, Likes);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Loves, Apples);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "(Likes, Apples)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(ecs_pair(Loves, Apples), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_pair_3_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Likes = ecs_entity(world, { .name = "Likes" });
    ecs_entity_t Loves = ecs_entity(world, { .name = "Loves" });
    ecs_entity_t Worships = ecs_entity(world, { .name = "Worships" });
    ecs_entity_t Apples = ecs_entity(world, { .name = "Apples" });
    ecs_add_pair(world, Loves, EcsIsA, Likes);
    ecs_add_pair(world, Worships, EcsIsA, Loves);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Worships, Apples);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "(Likes, Apples)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(ecs_pair(Worships, Apples), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_pair_multi(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Likes = ecs_entity(world, { .name = "Likes" });
    ecs_entity_t Loves = ecs_entity(world, { .name = "Loves" });
    ecs_entity_t Adores = ecs_entity(world, { .name = "Adores" });
    ecs_entity_t Apples = ecs_entity(world, { .name = "Apples" });
    ecs_add_pair(world, Loves, EcsIsA, Likes);
    ecs_add_pair(world, Adores, EcsIsA, Likes);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Loves, Apples);
    ecs_add_pair(world, e, Adores, Apples);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "(Likes, Apples)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(ecs_pair(Loves, Apples), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(ecs_pair(Adores, Apples), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_pair_different_target(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Likes = ecs_entity(world, { .name = "Likes" });
    ecs_entity_t Loves = ecs_entity(world, { .name = "Loves" });
    ecs_entity_t Apples = ecs_entity(world, { .name = "Apples" });
    ecs_entity_t Pears = ecs_entity(world, { .name = "Pears" });
    ecs_add_pair(world, Loves, EcsIsA, Likes);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add_pair(world, e1, Loves, Apples);
    ecs_entity_t e2 = ecs_new(world);
    ecs_add_pair(world, e2, Loves, Pears);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "(Likes, Apples)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(ecs_pair(Loves, Apples), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_pair_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Likes = ecs_entity(world, { .name = "Likes" });
    ecs_entity_t Loves = ecs_entity(world, { .name = "Loves" });
    ecs_entity_t Apples = ecs_entity(world, { .name = "Apples" });
    ecs_entity_t Pears = ecs_entity(world, { .name = "Pears" });
    ecs_add_pair(world, Loves, EcsIsA, Likes);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Loves, Apples);
    ecs_add_pair(world, e, Loves, Pears);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "(Likes, *)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_query_iter(world, r);
        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(ecs_pair(Loves, Apples), ecs_field_id(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_uint(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(ecs_pair(Loves, Pears), ecs_field_id(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(r);

    ecs_fini(world);
}

void ComponentInheritance_pair_wildcard_multi_rel(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Likes = ecs_entity(world, { .name = "Likes" });
    ecs_entity_t Loves = ecs_entity(world, { .name = "Loves" });
    ecs_entity_t Adores = ecs_entity(world, { .name = "Adores" });
    ecs_entity_t Apples = ecs_entity(world, { .name = "Apples" });
    ecs_entity_t Pears = ecs_entity(world, { .name = "Pears" });
    ecs_add_pair(world, Loves, EcsIsA, Likes);
    ecs_add_pair(world, Adores, EcsIsA, Likes);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Loves, Apples);
    ecs_add_pair(world, e, Adores, Pears);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind,
        .expr = "(Likes, *)"
    });

    test_assert(r != NULL);

    int32_t matched = 0;
    bool found_loves = false, found_adores = false;
    {
        ecs_iter_t it = ecs_query_iter(world, r);
        while (ecs_query_next(&it)) {
            int32_t i;
            for (i = 0; i < it.count; i ++) {
                test_uint(e, it.entities[i]);
                matched ++;
                if (ecs_field_id(&it, 0) == ecs_pair(Loves, Apples)) found_loves = true;
                if (ecs_field_id(&it, 0) == ecs_pair(Adores, Pears)) found_adores = true;
            }
        }
    }

    test_int(matched, 2);
    test_bool(found_loves, true);
    test_bool(found_adores, true);

    ecs_query_fini(r);

    ecs_fini(world);
}

/* --- Data component (value) inheritance, non-pair --- */

void ComponentInheritance_1_fixed_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Weight);
    ECS_COMPONENT(world, HeavyWeight);
    ecs_add_pair(world, ecs_id(HeavyWeight), EcsIsA, ecs_id(Weight));

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_set(world, e1, HeavyWeight, {.value = 10, .bonus = 3});

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Weight(e1)" });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_uint(0, it.count);
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(ecs_id(HeavyWeight), ecs_field_id(&it, 0));
    const Weight *w = ecs_base_field(&it, Weight, 0);
    test_assert(w != NULL);
    test_int(w->value, 10);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);
    ecs_fini(world);
}

void ComponentInheritance_1_this_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Weight);
    ECS_COMPONENT(world, HeavyWeight);
    ecs_add_pair(world, ecs_id(HeavyWeight), EcsIsA, ecs_id(Weight));

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, HeavyWeight, {.value = 10, .bonus = 3});

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Weight($this)" });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_uint(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_id(HeavyWeight), ecs_field_id(&it, 0));
    const Weight *w = ecs_base_field(&it, Weight, 0);
    test_assert(w != NULL);
    test_int(w[0].value, 10);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);
    ecs_fini(world);
}

void ComponentInheritance_1_this_component_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Weight);
    ECS_COMPONENT(world, HeavyWeight);
    ecs_add_pair(world, ecs_id(HeavyWeight), EcsIsA, ecs_id(Weight));

    ecs_entity_t e = ecs_new_w(world, Tag);
    ecs_set(world, e, HeavyWeight, {.value = 10, .bonus = 3});

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Tag($this), Weight($this)" });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_uint(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_id(HeavyWeight), ecs_field_id(&it, 1));
    const Weight *w = ecs_base_field(&it, Weight, 1);
    test_assert(w != NULL);
    test_int(w[0].value, 10);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);
    ecs_fini(world);
}

void ComponentInheritance_1_var_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Weight);
    ECS_COMPONENT(world, HeavyWeight);
    ecs_add_pair(world, ecs_id(HeavyWeight), EcsIsA, ecs_id(Weight));

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, HeavyWeight, {.value = 10, .bonus = 3});

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Weight($x)" });
    test_assert(r != NULL);
    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_uint(0, it.count);
    test_uint(e, ecs_field_src(&it, 0));
    test_uint(e, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_id(HeavyWeight), ecs_field_id(&it, 0));
    const Weight *w = ecs_base_field(&it, Weight, 0);
    test_assert(w != NULL);
    test_int(w->value, 10);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);
    ecs_fini(world);
}

void ComponentInheritance_1_var_component_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Weight);
    ECS_COMPONENT(world, HeavyWeight);
    ecs_add_pair(world, ecs_id(HeavyWeight), EcsIsA, ecs_id(Weight));

    ecs_entity_t e = ecs_new_w(world, Tag);
    ecs_set(world, e, HeavyWeight, {.value = 10, .bonus = 3});

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Tag($x), Weight($x)" });
    test_assert(r != NULL);
    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_uint(0, it.count);
    test_uint(e, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_id(HeavyWeight), ecs_field_id(&it, 1));
    const Weight *w = ecs_base_field(&it, Weight, 1);
    test_assert(w != NULL);
    test_int(w->value, 10);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);
    ecs_fini(world);
}

/* --- Data component (value) inheritance, pair (relationship has data) --- */

void ComponentInheritance_1_fixed_component_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Weight);
    ECS_COMPONENT(world, HeavyWeight);
    ecs_add_pair(world, ecs_id(HeavyWeight), EcsIsA, ecs_id(Weight));
    ecs_entity_t Apples = ecs_entity(world, { .name = "Apples" });

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_set_pair(world, e1, HeavyWeight, Apples, {.value = 10, .bonus = 3});

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Weight(e1, Apples)" });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_uint(0, it.count);
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(ecs_id(HeavyWeight), Apples), ecs_field_id(&it, 0));
    const Weight *w = ecs_base_field(&it, Weight, 0);
    test_assert(w != NULL);
    test_int(w->value, 10);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);
    ecs_fini(world);
}

void ComponentInheritance_1_this_component_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Weight);
    ECS_COMPONENT(world, HeavyWeight);
    ecs_add_pair(world, ecs_id(HeavyWeight), EcsIsA, ecs_id(Weight));
    ecs_entity_t Apples = ecs_entity(world, { .name = "Apples" });

    ecs_entity_t e = ecs_new(world);
    ecs_set_pair(world, e, HeavyWeight, Apples, {.value = 10, .bonus = 3});

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "(Weight, Apples)" });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_uint(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_pair(ecs_id(HeavyWeight), Apples), ecs_field_id(&it, 0));
    const Weight *w = ecs_base_field(&it, Weight, 0);
    test_assert(w != NULL);
    test_int(w[0].value, 10);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);
    ecs_fini(world);
}

void ComponentInheritance_1_this_component_pair_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Weight);
    ECS_COMPONENT(world, HeavyWeight);
    ecs_add_pair(world, ecs_id(HeavyWeight), EcsIsA, ecs_id(Weight));
    ecs_entity_t Apples = ecs_entity(world, { .name = "Apples" });

    ecs_entity_t e = ecs_new_w(world, Tag);
    ecs_set_pair(world, e, HeavyWeight, Apples, {.value = 10, .bonus = 3});

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Tag($this), Weight($this, Apples)" });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_uint(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_pair(ecs_id(HeavyWeight), Apples), ecs_field_id(&it, 1));
    const Weight *w = ecs_base_field(&it, Weight, 1);
    test_assert(w != NULL);
    test_int(w[0].value, 10);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);
    ecs_fini(world);
}

void ComponentInheritance_1_var_component_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Weight);
    ECS_COMPONENT(world, HeavyWeight);
    ecs_add_pair(world, ecs_id(HeavyWeight), EcsIsA, ecs_id(Weight));
    ecs_entity_t Apples = ecs_entity(world, { .name = "Apples" });

    ecs_entity_t e = ecs_new(world);
    ecs_set_pair(world, e, HeavyWeight, Apples, {.value = 10, .bonus = 3});

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Weight($x, Apples)" });
    test_assert(r != NULL);
    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_uint(0, it.count);
    test_uint(e, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_pair(ecs_id(HeavyWeight), Apples), ecs_field_id(&it, 0));
    const Weight *w = ecs_base_field(&it, Weight, 0);
    test_assert(w != NULL);
    test_int(w->value, 10);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);
    ecs_fini(world);
}

void ComponentInheritance_1_var_component_pair_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Weight);
    ECS_COMPONENT(world, HeavyWeight);
    ecs_add_pair(world, ecs_id(HeavyWeight), EcsIsA, ecs_id(Weight));
    ecs_entity_t Apples = ecs_entity(world, { .name = "Apples" });

    ecs_entity_t e = ecs_new_w(world, Tag);
    ecs_set_pair(world, e, HeavyWeight, Apples, {.value = 10, .bonus = 3});

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Tag($x), Weight($x, Apples)" });
    test_assert(r != NULL);
    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_uint(0, it.count);
    test_uint(e, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_pair(ecs_id(HeavyWeight), Apples), ecs_field_id(&it, 1));
    const Weight *w = ecs_base_field(&it, Weight, 1);
    test_assert(w != NULL);
    test_int(w->value, 10);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);
    ecs_fini(world);
}

/* --- Pair (tag) relationship inheritance, remaining src/written --- */

void ComponentInheritance_1_fixed_pair(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Likes = ecs_entity(world, { .name = "Likes" });
    ecs_entity_t Loves = ecs_entity(world, { .name = "Loves" });
    ecs_entity_t Apples = ecs_entity(world, { .name = "Apples" });
    ecs_add_pair(world, Loves, EcsIsA, Likes);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add_pair(world, e1, Loves, Apples);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Likes(e1, Apples)" });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_uint(0, it.count);
    test_uint(e1, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Loves, Apples), ecs_field_id(&it, 0));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);
    ecs_fini(world);
}

void ComponentInheritance_1_var_pair(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Likes = ecs_entity(world, { .name = "Likes" });
    ecs_entity_t Loves = ecs_entity(world, { .name = "Loves" });
    ecs_entity_t Apples = ecs_entity(world, { .name = "Apples" });
    ecs_add_pair(world, Loves, EcsIsA, Likes);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Loves, Apples);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Likes($x, Apples)" });
    test_assert(r != NULL);
    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_uint(0, it.count);
    test_uint(e, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_pair(Loves, Apples), ecs_field_id(&it, 0));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);
    ecs_fini(world);
}

void ComponentInheritance_1_this_pair_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ecs_entity_t Likes = ecs_entity(world, { .name = "Likes" });
    ecs_entity_t Loves = ecs_entity(world, { .name = "Loves" });
    ecs_entity_t Apples = ecs_entity(world, { .name = "Apples" });
    ecs_add_pair(world, Loves, EcsIsA, Likes);

    ecs_entity_t e = ecs_new_w(world, Tag);
    ecs_add_pair(world, e, Loves, Apples);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Tag($this), Likes($this, Apples)" });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_uint(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_pair(Loves, Apples), ecs_field_id(&it, 1));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);
    ecs_fini(world);
}

void ComponentInheritance_1_var_pair_written(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ecs_entity_t Likes = ecs_entity(world, { .name = "Likes" });
    ecs_entity_t Loves = ecs_entity(world, { .name = "Loves" });
    ecs_entity_t Apples = ecs_entity(world, { .name = "Apples" });
    ecs_add_pair(world, Loves, EcsIsA, Likes);

    ecs_entity_t e = ecs_new_w(world, Tag);
    ecs_add_pair(world, e, Loves, Apples);

    ecs_query_t *r = ecs_query(world, {
        .cache_kind = cache_kind, .expr = "Tag($x), Likes($x, Apples)" });
    test_assert(r != NULL);
    int x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_uint(0, it.count);
    test_uint(e, ecs_iter_get_var(&it, x_var));
    test_uint(ecs_pair(Loves, Apples), ecs_field_id(&it, 1));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);
    ecs_fini(world);
}
