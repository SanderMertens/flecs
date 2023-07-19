#include <addons.h>

ECS_ENTITY_DECLARE(LocatedIn);
ECS_TAG_DECLARE(Earth);
ECS_TAG_DECLARE(Location);
ECS_TAG_DECLARE(UnitedStates);
ECS_TAG_DECLARE(California);
ECS_TAG_DECLARE(Washington);
ECS_TAG_DECLARE(SanFrancisco);
ECS_TAG_DECLARE(LosAngeles);
ECS_TAG_DECLARE(Seattle);
ECS_TAG_DECLARE(NoordHolland);
ECS_TAG_DECLARE(Netherlands);
ECS_TAG_DECLARE(Amsterdam);

static
void populate_facts(ecs_world_t *world) {
    ECS_ENTITY_DEFINE(world, LocatedIn, Final, Transitive);
    ECS_TAG_DEFINE(world, Location);
    ECS_ENTITY_DEFINE(world, Earth, Location);
    ECS_ENTITY_DEFINE(world, UnitedStates, Location, (LocatedIn, Earth));
    ECS_ENTITY_DEFINE(world, California, Location, (LocatedIn, UnitedStates));
    ECS_ENTITY_DEFINE(world, Washington, Location, (LocatedIn, UnitedStates));
    ECS_ENTITY_DEFINE(world, SanFrancisco, Location, (LocatedIn, California));
    ECS_ENTITY_DEFINE(world, LosAngeles, Location, (LocatedIn, California));
    ECS_ENTITY_DEFINE(world, Seattle, Location, (LocatedIn, Washington));
    ECS_ENTITY_DEFINE(world, Netherlands, Location, (LocatedIn, Earth));
    ECS_ENTITY_DEFINE(world, NoordHolland, Location, (LocatedIn, Netherlands));
    ECS_ENTITY_DEFINE(world, Amsterdam, Location, (LocatedIn, NoordHolland));
}

void RulesTransitive_1_fact_0_lvl_true() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn(e1, SanFrancisco)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_fact_1_lvl_true() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn(e1, California)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_fact_2_lvl_true() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn(e1, UnitedStates)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_fact_0_lvl_false() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn(e1, Seattle)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_fact_1_lvl_false() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn(e1, Washington)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_fact_2_lvl_false() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn(e1, Netherlands)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_fact_reflexive() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_add_id(world, LocatedIn, EcsReflexive);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn(e1, e1)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_this_src_written_0_lvl() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_entity_t e4 = ecs_new(world, Tag);
    ecs_entity_t e5 = ecs_new(world, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this), LocatedIn($this, SanFrancisco)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, LosAngeles);
    ecs_add_pair(world, e3, LocatedIn, Seattle);
    ecs_add_pair(world, e4, LocatedIn, Amsterdam);
    ecs_add_pair(world, e5, LocatedIn, SanFrancisco);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, it.entities[0]);
        test_uint(e5, it.entities[1]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_this_src_written_1_lvl() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_entity_t e4 = ecs_new(world, Tag);
    ecs_entity_t e5 = ecs_new(world, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this), LocatedIn($this, California)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, LosAngeles);
    ecs_add_pair(world, e3, LocatedIn, Seattle);
    ecs_add_pair(world, e4, LocatedIn, Amsterdam);
    ecs_add_pair(world, e5, LocatedIn, SanFrancisco);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, it.entities[0]);
        test_uint(e5, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_this_src_written_2_lvl() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_entity_t e4 = ecs_new(world, Tag);
    ecs_entity_t e5 = ecs_new(world, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this), LocatedIn($this, UnitedStates)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, LosAngeles);
    ecs_add_pair(world, e3, LocatedIn, Seattle);
    ecs_add_pair(world, e4, LocatedIn, Amsterdam);
    ecs_add_pair(world, e5, LocatedIn, SanFrancisco);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e1, it.entities[0]);
        test_uint(e5, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_this_src_written_reflexive() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_add_id(world, LocatedIn, EcsReflexive);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_entity_t e4 = ecs_new(world, Tag);
    ecs_entity_t e5 = ecs_new(world, Tag);
    ecs_entity_t e6 = ecs_new(world, Tag);
    ecs_entity_t e7 = ecs_new(world, Tag);
    ecs_entity_t e8 = ecs_new(world, Tag);

    ecs_set_name(world, e1, "e1");
    ecs_set_name(world, e2, "e2");
    ecs_set_name(world, e3, "e3");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this), LocatedIn($this, e2)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, e2), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e3, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e4, LocatedIn, LosAngeles);
    ecs_add_pair(world, e5, LocatedIn, Seattle);
    ecs_add_pair(world, e6, LocatedIn, Amsterdam);
    ecs_add_pair(world, e7, LocatedIn, e2);
    ecs_add_pair(world, e8, LocatedIn, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, e2), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, e2), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(e7, it.entities[0]);
        test_uint(e8, it.entities[1]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_this_src_0_lvl() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);
    ecs_entity_t e4 = ecs_new_id(world);
    ecs_entity_t e5 = ecs_new_id(world);
    ecs_entity_t e6 = ecs_new_id(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn($this, SanFrancisco)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e3, LocatedIn, LosAngeles);
    ecs_add_pair(world, e4, LocatedIn, Seattle);
    ecs_add_pair(world, e5, LocatedIn, Amsterdam);
    ecs_add_pair(world, e6, LocatedIn, SanFrancisco);

    ecs_add(world, e2, Tag);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, it.entities[0]);
        test_uint(e6, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e2, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_this_src_1_lvl() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);
    ecs_entity_t e4 = ecs_new_id(world);
    ecs_entity_t e5 = ecs_new_id(world);
    ecs_entity_t e6 = ecs_new_id(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn($this, California)"
    });

    test_assert(r != NULL);

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e3, LocatedIn, LosAngeles);
    ecs_add_pair(world, e4, LocatedIn, Seattle);
    ecs_add_pair(world, e5, LocatedIn, Amsterdam);
    ecs_add_pair(world, e6, LocatedIn, SanFrancisco);

    ecs_add(world, e2, Tag);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(SanFrancisco, it.entities[0]);
        test_uint(LosAngeles, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, it.entities[0]);
        test_uint(e6, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, LosAngeles), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_this_src_2_lvl() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);
    ecs_entity_t e4 = ecs_new_id(world);
    ecs_entity_t e5 = ecs_new_id(world);
    ecs_entity_t e6 = ecs_new_id(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn($this, UnitedStates)"
    });

    test_assert(r != NULL);

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e3, LocatedIn, LosAngeles);
    ecs_add_pair(world, e4, LocatedIn, Seattle);
    ecs_add_pair(world, e5, LocatedIn, Amsterdam);
    ecs_add_pair(world, e6, LocatedIn, SanFrancisco);

    ecs_add(world, e2, Tag);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(California, it.entities[0]);
        test_uint(Washington, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(SanFrancisco, it.entities[0]);
        test_uint(LosAngeles, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, it.entities[0]);
        test_uint(e6, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, LosAngeles), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Washington), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Seattle, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Seattle), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e4, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_this_src_reflexive() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_add_id(world, LocatedIn, EcsReflexive);

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);
    ecs_entity_t e4 = ecs_new_id(world);
    ecs_entity_t e5 = ecs_new_id(world);
    ecs_entity_t e6 = ecs_new_id(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn($this, California)"
    });

    test_assert(r != NULL);

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e3, LocatedIn, LosAngeles);
    ecs_add_pair(world, e4, LocatedIn, Seattle);
    ecs_add_pair(world, e5, LocatedIn, Amsterdam);
    ecs_add_pair(world, e6, LocatedIn, SanFrancisco);

    ecs_add(world, e2, Tag);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(California, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(SanFrancisco, it.entities[0]);
        test_uint(LosAngeles, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e1, it.entities[0]);
        test_uint(e6, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, LosAngeles), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_ent_src_tgt_var_0_lvl() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn(e1, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, LocatedIn, UnitedStates);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_ent_src_tgt_var_1_lvl() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn(e1, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, LocatedIn, California);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(California, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_ent_src_tgt_var_2_lvl() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn(e1, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(California, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_ent_src_tgt_var_reflexive() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_add_id(world, LocatedIn, EcsReflexive);

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn(e1, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(California, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_this_src_tgt_var() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ECS_TAG(world, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn($this, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, NoordHolland), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(NoordHolland, ecs_iter_get_var(&it, x_var));
        test_uint(Amsterdam, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Netherlands), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Netherlands, ecs_iter_get_var(&it, x_var));
        test_uint(Amsterdam, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(Amsterdam, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Netherlands), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Netherlands, ecs_iter_get_var(&it, x_var));
        test_uint(NoordHolland, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(NoordHolland, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Washington), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Washington, ecs_iter_get_var(&it, x_var));
        test_uint(Seattle, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(Seattle, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(Seattle, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(California, ecs_iter_get_var(&it, x_var));
        test_uint(SanFrancisco, it.entities[0]);
        test_uint(LosAngeles, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(SanFrancisco, it.entities[0]);
        test_uint(LosAngeles, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(SanFrancisco, it.entities[0]);
        test_uint(LosAngeles, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(California, it.entities[0]);
        test_uint(Washington, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(California, it.entities[0]);
        test_uint(Washington, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(UnitedStates, it.entities[0]);
        test_uint(Netherlands, it.entities[1]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_this_src_tgt_var_reflexive() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_add_id(world, LocatedIn, EcsReflexive);

    ECS_TAG(world, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn($this, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Amsterdam), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Amsterdam, ecs_iter_get_var(&it, x_var));
        test_uint(Amsterdam, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, NoordHolland), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(NoordHolland, ecs_iter_get_var(&it, x_var));
        test_uint(Amsterdam, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Netherlands), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Netherlands, ecs_iter_get_var(&it, x_var));
        test_uint(Amsterdam, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(Amsterdam, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, NoordHolland), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(NoordHolland, ecs_iter_get_var(&it, x_var));
        test_uint(NoordHolland, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Netherlands), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Netherlands, ecs_iter_get_var(&it, x_var));
        test_uint(NoordHolland, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(NoordHolland, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Seattle), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Seattle, ecs_iter_get_var(&it, x_var));
        test_uint(Seattle, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Washington), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Washington, ecs_iter_get_var(&it, x_var));
        test_uint(Seattle, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(Seattle, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(Seattle, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, x_var));
        test_uint(SanFrancisco, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, LosAngeles), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(LosAngeles, ecs_iter_get_var(&it, x_var));
        test_uint(LosAngeles, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(California, ecs_iter_get_var(&it, x_var));
        test_uint(SanFrancisco, it.entities[0]);
        test_uint(LosAngeles, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(SanFrancisco, it.entities[0]);
        test_uint(LosAngeles, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(SanFrancisco, it.entities[0]);
        test_uint(LosAngeles, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(California, ecs_iter_get_var(&it, x_var));
        test_uint(California, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Washington), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Washington, ecs_iter_get_var(&it, x_var));
        test_uint(Washington, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(California, it.entities[0]);
        test_uint(Washington, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(California, it.entities[0]);
        test_uint(Washington, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(UnitedStates, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Netherlands), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Netherlands, ecs_iter_get_var(&it, x_var));
        test_uint(Netherlands, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(UnitedStates, it.entities[0]);
        test_uint(Netherlands, it.entities[1]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_var_src_written_0_lvl() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_entity_t e4 = ecs_new(world, Tag);
    ecs_entity_t e5 = ecs_new(world, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($r), LocatedIn($r, SanFrancisco)"
    });

    test_assert(r != NULL);

    int r_var = ecs_rule_find_var(r, "r");
    test_assert(r_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, LosAngeles);
    ecs_add_pair(world, e3, LocatedIn, Seattle);
    ecs_add_pair(world, e4, LocatedIn, Amsterdam);
    ecs_add_pair(world, e5, LocatedIn, SanFrancisco);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 2));
        test_uint(e5, ecs_field_src(&it, 1));
        test_uint(e5, ecs_field_src(&it, 2));
        test_uint(e5, ecs_iter_get_var(&it, r_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_var_src_written_1_lvl() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_entity_t e4 = ecs_new(world, Tag);
    ecs_entity_t e5 = ecs_new(world, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($r), LocatedIn($r, California)"
    });

    test_assert(r != NULL);

    int r_var = ecs_rule_find_var(r, "r");
    test_assert(r_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, LosAngeles);
    ecs_add_pair(world, e3, LocatedIn, Seattle);
    ecs_add_pair(world, e4, LocatedIn, Amsterdam);
    ecs_add_pair(world, e5, LocatedIn, SanFrancisco);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 2));
        test_uint(e5, ecs_field_src(&it, 1));
        test_uint(e5, ecs_field_src(&it, 2));
        test_uint(e5, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 2));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, r_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_var_src_written_2_lvl() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_entity_t e4 = ecs_new(world, Tag);
    ecs_entity_t e5 = ecs_new(world, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($r), LocatedIn($r, UnitedStates)"
    });

    test_assert(r != NULL);

    int r_var = ecs_rule_find_var(r, "r");
    test_assert(r_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, LosAngeles);
    ecs_add_pair(world, e3, LocatedIn, Seattle);
    ecs_add_pair(world, e4, LocatedIn, Amsterdam);
    ecs_add_pair(world, e5, LocatedIn, SanFrancisco);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 2));
        test_uint(e5, ecs_field_src(&it, 1));
        test_uint(e5, ecs_field_src(&it, 2));
        test_uint(e5, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 2));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 2));
        test_uint(e3, ecs_field_src(&it, 1));
        test_uint(e3, ecs_field_src(&it, 2));
        test_uint(e3, ecs_iter_get_var(&it, r_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_var_src_written_reflexive() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_add_id(world, LocatedIn, EcsReflexive);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_entity_t e4 = ecs_new(world, Tag);
    ecs_entity_t e5 = ecs_new(world, Tag);
    ecs_entity_t e6 = ecs_new(world, Tag);
    ecs_entity_t e7 = ecs_new(world, Tag);
    ecs_entity_t e8 = ecs_new(world, Tag);

    ecs_set_name(world, e1, "e1");
    ecs_set_name(world, e2, "e2");
    ecs_set_name(world, e3, "e3");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($r), LocatedIn($r, e2)"
    });

    test_assert(r != NULL);

    int r_var = ecs_rule_find_var(r, "r");
    test_assert(r_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, e2), ecs_field_id(&it, 2));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, r_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e3, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e4, LocatedIn, LosAngeles);
    ecs_add_pair(world, e5, LocatedIn, Seattle);
    ecs_add_pair(world, e6, LocatedIn, Amsterdam);
    ecs_add_pair(world, e7, LocatedIn, e2);
    ecs_add_pair(world, e8, LocatedIn, e2);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, e2), ecs_field_id(&it, 2));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, e2), ecs_field_id(&it, 2));
        test_uint(e7, ecs_field_src(&it, 1));
        test_uint(e7, ecs_field_src(&it, 2));
        test_uint(e7, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, e2), ecs_field_id(&it, 2));
        test_uint(e8, ecs_field_src(&it, 1));
        test_uint(e8, ecs_field_src(&it, 2));
        test_uint(e8, ecs_iter_get_var(&it, r_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_var_src_0_lvl() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);
    ecs_entity_t e4 = ecs_new_id(world);
    ecs_entity_t e5 = ecs_new_id(world);
    ecs_entity_t e6 = ecs_new_id(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn($r, SanFrancisco)"
    });

    test_assert(r != NULL);

    int r_var = ecs_rule_find_var(r, "r");
    test_assert(r_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e3, LocatedIn, LosAngeles);
    ecs_add_pair(world, e4, LocatedIn, Seattle);
    ecs_add_pair(world, e5, LocatedIn, Amsterdam);
    ecs_add_pair(world, e6, LocatedIn, SanFrancisco);

    ecs_add(world, e2, Tag);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(e6, ecs_field_src(&it, 1));
        test_uint(e6, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, r_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_var_src_1_lvl() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);
    ecs_entity_t e4 = ecs_new_id(world);
    ecs_entity_t e5 = ecs_new_id(world);
    ecs_entity_t e6 = ecs_new_id(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn($r, California)"
    });

    test_assert(r != NULL);

    int r_var = ecs_rule_find_var(r, "r");
    test_assert(r_var != -1);

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e3, LocatedIn, LosAngeles);
    ecs_add_pair(world, e4, LocatedIn, Seattle);
    ecs_add_pair(world, e5, LocatedIn, Amsterdam);
    ecs_add_pair(world, e6, LocatedIn, SanFrancisco);

    ecs_add(world, e2, Tag);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(SanFrancisco, ecs_field_src(&it, 1));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(LosAngeles, ecs_field_src(&it, 1));
        test_uint(LosAngeles, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(e6, ecs_field_src(&it, 1));
        test_uint(e6, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, LosAngeles), ecs_field_id(&it, 1));
        test_uint(e3, ecs_field_src(&it, 1));
        test_uint(e3, ecs_iter_get_var(&it, r_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_var_src_2_lvl() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);
    ecs_entity_t e4 = ecs_new_id(world);
    ecs_entity_t e5 = ecs_new_id(world);
    ecs_entity_t e6 = ecs_new_id(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn($r, UnitedStates)"
    });

    test_assert(r != NULL);

    int r_var = ecs_rule_find_var(r, "r");
    test_assert(r_var != -1);

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e3, LocatedIn, LosAngeles);
    ecs_add_pair(world, e4, LocatedIn, Seattle);
    ecs_add_pair(world, e5, LocatedIn, Amsterdam);
    ecs_add_pair(world, e6, LocatedIn, SanFrancisco);

    ecs_add(world, e2, Tag);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(California, ecs_field_src(&it, 1));
        test_uint(California, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Washington, ecs_field_src(&it, 1));
        test_uint(Washington, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(SanFrancisco, ecs_field_src(&it, 1));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(LosAngeles, ecs_field_src(&it, 1));
        test_uint(LosAngeles, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(e6, ecs_field_src(&it, 1));
        test_uint(e6, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, LosAngeles), ecs_field_id(&it, 1));
        test_uint(e3, ecs_field_src(&it, 1));
        test_uint(e3, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Washington), ecs_field_id(&it, 1));
        test_uint(Seattle, ecs_field_src(&it, 1));
        test_uint(Seattle, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Seattle), ecs_field_id(&it, 1));
        test_uint(e4, ecs_field_src(&it, 1));
        test_uint(e4, ecs_iter_get_var(&it, r_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_var_src_reflexive() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_add_id(world, LocatedIn, EcsReflexive);

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);
    ecs_entity_t e4 = ecs_new_id(world);
    ecs_entity_t e5 = ecs_new_id(world);
    ecs_entity_t e6 = ecs_new_id(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn($r, California)"
    });

    test_assert(r != NULL);

    int r_var = ecs_rule_find_var(r, "r");
    test_assert(r_var != -1);

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e3, LocatedIn, LosAngeles);
    ecs_add_pair(world, e4, LocatedIn, Seattle);
    ecs_add_pair(world, e5, LocatedIn, Amsterdam);
    ecs_add_pair(world, e6, LocatedIn, SanFrancisco);

    ecs_add(world, e2, Tag);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(California, ecs_field_src(&it, 1));
        test_uint(California, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(SanFrancisco, ecs_field_src(&it, 1));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(LosAngeles, ecs_field_src(&it, 1));
        test_uint(LosAngeles, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(e6, ecs_field_src(&it, 1));
        test_uint(e6, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, LosAngeles), ecs_field_id(&it, 1));
        test_uint(e3, ecs_field_src(&it, 1));
        test_uint(e3, ecs_iter_get_var(&it, r_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_var_src_tgt_var() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ECS_TAG(world, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn($r, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    int r_var = ecs_rule_find_var(r, "r");
    test_assert(r_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, NoordHolland), ecs_field_id(&it, 1));
        test_uint(Amsterdam, ecs_field_src(&it, 1));
        test_uint(NoordHolland, ecs_iter_get_var(&it, x_var));
        test_uint(Amsterdam, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Netherlands), ecs_field_id(&it, 1));
        test_uint(Amsterdam, ecs_field_src(&it, 1));
        test_uint(Netherlands, ecs_iter_get_var(&it, x_var));
        test_uint(Amsterdam, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(Amsterdam, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(Amsterdam, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Netherlands), ecs_field_id(&it, 1));
        test_uint(NoordHolland, ecs_field_src(&it, 1));
        test_uint(Netherlands, ecs_iter_get_var(&it, x_var));
        test_uint(NoordHolland, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(NoordHolland, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(NoordHolland, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Washington), ecs_field_id(&it, 1));
        test_uint(Seattle, ecs_field_src(&it, 1));
        test_uint(Washington, ecs_iter_get_var(&it, x_var));
        test_uint(Seattle, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Seattle, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(Seattle, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(Seattle, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(Seattle, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(SanFrancisco, ecs_field_src(&it, 1));
        test_uint(California, ecs_iter_get_var(&it, x_var));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(LosAngeles, ecs_field_src(&it, 1));
        test_uint(California, ecs_iter_get_var(&it, x_var));
        test_uint(LosAngeles, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(SanFrancisco, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(LosAngeles, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(LosAngeles, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(SanFrancisco, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(LosAngeles, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(LosAngeles, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(California, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(California, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Washington, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(Washington, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(California, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(California, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(Washington, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(Washington, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(UnitedStates, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(UnitedStates, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(Netherlands, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(Netherlands, ecs_iter_get_var(&it, r_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_var_src_tgt_var_reflexive() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_add_id(world, LocatedIn, EcsReflexive);

    ECS_TAG(world, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn($r, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    int r_var = ecs_rule_find_var(r, "r");
    test_assert(r_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Amsterdam), ecs_field_id(&it, 1));
        test_uint(Amsterdam, ecs_field_src(&it, 1));
        test_uint(Amsterdam, ecs_iter_get_var(&it, x_var));
        test_uint(Amsterdam, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, NoordHolland), ecs_field_id(&it, 1));
        test_uint(Amsterdam, ecs_field_src(&it, 1));
        test_uint(NoordHolland, ecs_iter_get_var(&it, x_var));
        test_uint(Amsterdam, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Netherlands), ecs_field_id(&it, 1));
        test_uint(Amsterdam, ecs_field_src(&it, 1));
        test_uint(Netherlands, ecs_iter_get_var(&it, x_var));
        test_uint(Amsterdam, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(Amsterdam, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(Amsterdam, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, NoordHolland), ecs_field_id(&it, 1));
        test_uint(NoordHolland, ecs_field_src(&it, 1));
        test_uint(NoordHolland, ecs_iter_get_var(&it, x_var));
        test_uint(NoordHolland, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Netherlands), ecs_field_id(&it, 1));
        test_uint(NoordHolland, ecs_field_src(&it, 1));
        test_uint(Netherlands, ecs_iter_get_var(&it, x_var));
        test_uint(NoordHolland, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(NoordHolland, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(NoordHolland, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Seattle), ecs_field_id(&it, 1));
        test_uint(Seattle, ecs_field_src(&it, 1));
        test_uint(Seattle, ecs_iter_get_var(&it, x_var));
        test_uint(Seattle, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Washington), ecs_field_id(&it, 1));
        test_uint(Seattle, ecs_field_src(&it, 1));
        test_uint(Washington, ecs_iter_get_var(&it, x_var));
        test_uint(Seattle, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Seattle, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(Seattle, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(Seattle, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(Seattle, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(SanFrancisco, ecs_field_src(&it, 1));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, x_var));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, LosAngeles), ecs_field_id(&it, 1));
        test_uint(LosAngeles, ecs_field_src(&it, 1));
        test_uint(LosAngeles, ecs_iter_get_var(&it, x_var));
        test_uint(LosAngeles, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(SanFrancisco, ecs_field_src(&it, 1));
        test_uint(California, ecs_iter_get_var(&it, x_var));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(LosAngeles, ecs_field_src(&it, 1));
        test_uint(California, ecs_iter_get_var(&it, x_var));
        test_uint(LosAngeles, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(SanFrancisco, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(LosAngeles, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(LosAngeles, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(SanFrancisco, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(LosAngeles, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(LosAngeles, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(California, ecs_field_src(&it, 1));
        test_uint(California, ecs_iter_get_var(&it, x_var));
        test_uint(California, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Washington), ecs_field_id(&it, 1));
        test_uint(Washington, ecs_field_src(&it, 1));
        test_uint(Washington, ecs_iter_get_var(&it, x_var));
        test_uint(Washington, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(California, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(California, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Washington, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(Washington, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(California, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(California, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(Washington, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(Washington, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(UnitedStates, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(UnitedStates, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Netherlands), ecs_field_id(&it, 1));
        test_uint(Netherlands, ecs_field_src(&it, 1));
        test_uint(Netherlands, ecs_iter_get_var(&it, x_var));
        test_uint(Netherlands, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(UnitedStates, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(UnitedStates, ecs_iter_get_var(&it, r_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(Netherlands, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(Netherlands, ecs_iter_get_var(&it, r_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_ent_src_tgt_this_0_lvl() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn(e1, $this)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, LocatedIn, UnitedStates);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, this_var));
        test_uint(UnitedStates, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, this_var));
        test_uint(Earth, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_ent_src_tgt_this_1_lvl() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn(e1, $this)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, LocatedIn, California);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(California, ecs_iter_get_var(&it, this_var));
        test_uint(California, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, this_var));
        test_uint(UnitedStates, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, this_var));
        test_uint(Earth, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_ent_src_tgt_this_2_lvl() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn(e1, $this)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, this_var));
        test_uint(SanFrancisco, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(California, ecs_iter_get_var(&it, this_var));
        test_uint(California, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, this_var));
        test_uint(UnitedStates, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, this_var));
        test_uint(Earth, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_ent_src_tgt_this_reflexive() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_add_id(world, LocatedIn, EcsReflexive);

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn(e1, $this)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, e1), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, this_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, this_var));
        test_uint(SanFrancisco, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(California, ecs_iter_get_var(&it, this_var));
        test_uint(California, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, this_var));
        test_uint(UnitedStates, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, this_var));
        test_uint(Earth, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_var_src_tgt_this() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ECS_TAG(world, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn($r, $this)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    int r_var = ecs_rule_find_var(r, "r");
    test_assert(r_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, NoordHolland), ecs_field_id(&it, 1));
        test_uint(Amsterdam, ecs_field_src(&it, 1));
        test_uint(NoordHolland, ecs_iter_get_var(&it, this_var));
        test_uint(Amsterdam, ecs_iter_get_var(&it, r_var));
        test_uint(NoordHolland, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Netherlands), ecs_field_id(&it, 1));
        test_uint(Amsterdam, ecs_field_src(&it, 1));
        test_uint(Netherlands, ecs_iter_get_var(&it, this_var));
        test_uint(Amsterdam, ecs_iter_get_var(&it, r_var));
        test_uint(Netherlands, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(Amsterdam, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, this_var));
        test_uint(Amsterdam, ecs_iter_get_var(&it, r_var));
        test_uint(Earth, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Netherlands), ecs_field_id(&it, 1));
        test_uint(NoordHolland, ecs_field_src(&it, 1));
        test_uint(Netherlands, ecs_iter_get_var(&it, this_var));
        test_uint(NoordHolland, ecs_iter_get_var(&it, r_var));
        test_uint(Netherlands, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(NoordHolland, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, this_var));
        test_uint(NoordHolland, ecs_iter_get_var(&it, r_var));
        test_uint(Earth, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Washington), ecs_field_id(&it, 1));
        test_uint(Seattle, ecs_field_src(&it, 1));
        test_uint(Washington, ecs_iter_get_var(&it, this_var));
        test_uint(Seattle, ecs_iter_get_var(&it, r_var));
        test_uint(Washington, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Seattle, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, this_var));
        test_uint(Seattle, ecs_iter_get_var(&it, r_var));
        test_uint(UnitedStates, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(Seattle, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, this_var));
        test_uint(Seattle, ecs_iter_get_var(&it, r_var));
        test_uint(Earth, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(SanFrancisco, ecs_field_src(&it, 1));
        test_uint(California, ecs_iter_get_var(&it, this_var));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, r_var));
        test_uint(California, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(LosAngeles, ecs_field_src(&it, 1));
        test_uint(California, ecs_iter_get_var(&it, this_var));
        test_uint(LosAngeles, ecs_iter_get_var(&it, r_var));
        test_uint(California, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(SanFrancisco, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, this_var));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, r_var));
        test_uint(UnitedStates, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(LosAngeles, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, this_var));
        test_uint(LosAngeles, ecs_iter_get_var(&it, r_var));
        test_uint(UnitedStates, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(SanFrancisco, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, this_var));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, r_var));
        test_uint(Earth, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(LosAngeles, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, this_var));
        test_uint(LosAngeles, ecs_iter_get_var(&it, r_var));
        test_uint(Earth, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(California, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, this_var));
        test_uint(California, ecs_iter_get_var(&it, r_var));
        test_uint(UnitedStates, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Washington, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, this_var));
        test_uint(Washington, ecs_iter_get_var(&it, r_var));
        test_uint(UnitedStates, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(California, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, this_var));
        test_uint(California, ecs_iter_get_var(&it, r_var));
        test_uint(Earth, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(Washington, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, this_var));
        test_uint(Washington, ecs_iter_get_var(&it, r_var));
        test_uint(Earth, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(UnitedStates, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, this_var));
        test_uint(UnitedStates, ecs_iter_get_var(&it, r_var));
        test_uint(Earth, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(Netherlands, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, this_var));
        test_uint(Netherlands, ecs_iter_get_var(&it, r_var));
        test_uint(Earth, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_var_src_tgt_this_reflexive() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_add_id(world, LocatedIn, EcsReflexive);

    ECS_TAG(world, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn($r, $this)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);

    int r_var = ecs_rule_find_var(r, "r");
    test_assert(r_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Amsterdam), ecs_field_id(&it, 1));
        test_uint(Amsterdam, ecs_field_src(&it, 1));
        test_uint(Amsterdam, ecs_iter_get_var(&it, this_var));
        test_uint(Amsterdam, ecs_iter_get_var(&it, r_var));
        test_uint(Amsterdam, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, NoordHolland), ecs_field_id(&it, 1));
        test_uint(Amsterdam, ecs_field_src(&it, 1));
        test_uint(NoordHolland, ecs_iter_get_var(&it, this_var));
        test_uint(Amsterdam, ecs_iter_get_var(&it, r_var));
        test_uint(NoordHolland, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Netherlands), ecs_field_id(&it, 1));
        test_uint(Amsterdam, ecs_field_src(&it, 1));
        test_uint(Netherlands, ecs_iter_get_var(&it, this_var));
        test_uint(Amsterdam, ecs_iter_get_var(&it, r_var));
        test_uint(Netherlands, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(Amsterdam, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, this_var));
        test_uint(Amsterdam, ecs_iter_get_var(&it, r_var));
        test_uint(Earth, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, NoordHolland), ecs_field_id(&it, 1));
        test_uint(NoordHolland, ecs_field_src(&it, 1));
        test_uint(NoordHolland, ecs_iter_get_var(&it, this_var));
        test_uint(NoordHolland, ecs_iter_get_var(&it, r_var));
        test_uint(NoordHolland, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Netherlands), ecs_field_id(&it, 1));
        test_uint(NoordHolland, ecs_field_src(&it, 1));
        test_uint(Netherlands, ecs_iter_get_var(&it, this_var));
        test_uint(NoordHolland, ecs_iter_get_var(&it, r_var));
        test_uint(Netherlands, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(NoordHolland, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, this_var));
        test_uint(NoordHolland, ecs_iter_get_var(&it, r_var));
        test_uint(Earth, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Seattle), ecs_field_id(&it, 1));
        test_uint(Seattle, ecs_field_src(&it, 1));
        test_uint(Seattle, ecs_iter_get_var(&it, this_var));
        test_uint(Seattle, ecs_iter_get_var(&it, r_var));
        test_uint(Seattle, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Washington), ecs_field_id(&it, 1));
        test_uint(Seattle, ecs_field_src(&it, 1));
        test_uint(Washington, ecs_iter_get_var(&it, this_var));
        test_uint(Seattle, ecs_iter_get_var(&it, r_var));
        test_uint(Washington, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Seattle, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, this_var));
        test_uint(Seattle, ecs_iter_get_var(&it, r_var));
        test_uint(UnitedStates, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(Seattle, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, this_var));
        test_uint(Seattle, ecs_iter_get_var(&it, r_var));
        test_uint(Earth, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(SanFrancisco, ecs_field_src(&it, 1));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, this_var));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, r_var));
        test_uint(SanFrancisco, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, LosAngeles), ecs_field_id(&it, 1));
        test_uint(LosAngeles, ecs_field_src(&it, 1));
        test_uint(LosAngeles, ecs_iter_get_var(&it, this_var));
        test_uint(LosAngeles, ecs_iter_get_var(&it, r_var));
        test_uint(LosAngeles, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(SanFrancisco, ecs_field_src(&it, 1));
        test_uint(California, ecs_iter_get_var(&it, this_var));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, r_var));
        test_uint(California, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(LosAngeles, ecs_field_src(&it, 1));
        test_uint(California, ecs_iter_get_var(&it, this_var));
        test_uint(LosAngeles, ecs_iter_get_var(&it, r_var));
        test_uint(California, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(SanFrancisco, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, this_var));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, r_var));
        test_uint(UnitedStates, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(LosAngeles, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, this_var));
        test_uint(LosAngeles, ecs_iter_get_var(&it, r_var));
        test_uint(UnitedStates, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(SanFrancisco, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, this_var));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, r_var));
        test_uint(Earth, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(LosAngeles, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, this_var));
        test_uint(LosAngeles, ecs_iter_get_var(&it, r_var));
        test_uint(Earth, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(California, ecs_field_src(&it, 1));
        test_uint(California, ecs_iter_get_var(&it, this_var));
        test_uint(California, ecs_iter_get_var(&it, r_var));
        test_uint(California, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Washington), ecs_field_id(&it, 1));
        test_uint(Washington, ecs_field_src(&it, 1));
        test_uint(Washington, ecs_iter_get_var(&it, this_var));
        test_uint(Washington, ecs_iter_get_var(&it, r_var));
        test_uint(Washington, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(California, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, this_var));
        test_uint(California, ecs_iter_get_var(&it, r_var));
        test_uint(UnitedStates, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Washington, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, this_var));
        test_uint(Washington, ecs_iter_get_var(&it, r_var));
        test_uint(UnitedStates, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(California, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, this_var));
        test_uint(California, ecs_iter_get_var(&it, r_var));
        test_uint(Earth, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(Washington, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, this_var));
        test_uint(Washington, ecs_iter_get_var(&it, r_var));
        test_uint(Earth, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(UnitedStates, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, this_var));
        test_uint(UnitedStates, ecs_iter_get_var(&it, r_var));
        test_uint(UnitedStates, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Netherlands), ecs_field_id(&it, 1));
        test_uint(Netherlands, ecs_field_src(&it, 1));
        test_uint(Netherlands, ecs_iter_get_var(&it, this_var));
        test_uint(Netherlands, ecs_iter_get_var(&it, r_var));
        test_uint(Netherlands, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(UnitedStates, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, this_var));
        test_uint(UnitedStates, ecs_iter_get_var(&it, r_var));
        test_uint(Earth, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(Netherlands, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, this_var));
        test_uint(Netherlands, ecs_iter_get_var(&it, r_var));
        test_uint(Earth, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_2_ent_src_constrain_tgt_var_before_0_lvl() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, City);

    populate_facts(world);

    ecs_add(world, SanFrancisco, City);
    ecs_add(world, LosAngeles, City);
    ecs_add(world, Seattle, City);
    ecs_add(world, Amsterdam, City);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e3, LocatedIn, LosAngeles);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "City($x), LocatedIn(e1, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(City, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 2));
        test_uint(SanFrancisco, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_2_ent_src_constrain_tgt_var_before_1_lvl() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, State);

    populate_facts(world);

    ecs_add(world, California, State);
    ecs_add(world, Washington, State);
    ecs_add(world, NoordHolland, State);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e3, LocatedIn, LosAngeles);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "State($x), LocatedIn(e1, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(State, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 2));
        test_uint(California, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(California, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_2_ent_src_constrain_tgt_var_before_2_lvl() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Country);

    populate_facts(world);

    ecs_add(world, UnitedStates, Country);
    ecs_add(world, Netherlands, Country);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e3, LocatedIn, Amsterdam);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Country($x), LocatedIn(e1, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Country, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 2));
        test_uint(UnitedStates, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_2_ent_src_constrain_tgt_var_after_0_lvl() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, City);

    populate_facts(world);

    ecs_add(world, SanFrancisco, City);
    ecs_add(world, LosAngeles, City);
    ecs_add(world, Seattle, City);
    ecs_add(world, Amsterdam, City);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e3, LocatedIn, LosAngeles);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn(e1, $x), City($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(City, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(SanFrancisco, ecs_field_src(&it, 2));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_2_ent_src_constrain_tgt_var_after_1_lvl() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, State);

    populate_facts(world);

    ecs_add(world, California, State);
    ecs_add(world, Washington, State);
    ecs_add(world, NoordHolland, State);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e3, LocatedIn, LosAngeles);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn(e1, $x), State($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(State, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(California, ecs_field_src(&it, 2));
        test_uint(California, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_2_ent_src_constrain_tgt_var_after_2_lvl() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Country);

    populate_facts(world);

    ecs_add(world, UnitedStates, Country);
    ecs_add(world, Netherlands, Country);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e3, LocatedIn, Amsterdam);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn(e1, $x), Country($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Country, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_field_src(&it, 2));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_2_this_src_constrain_tgt_var_before_0_lvl() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, City);

    populate_facts(world);

    ecs_add(world, SanFrancisco, City);
    ecs_add(world, LosAngeles, City);
    ecs_add(world, Seattle, City);
    ecs_add(world, Amsterdam, City);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    ecs_entity_t e4 = ecs_new_entity(world, "e4");
    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e3, LocatedIn, LosAngeles);
    ecs_add_pair(world, e4, LocatedIn, Amsterdam);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "City($x), LocatedIn($this, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(City, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 2));
        test_uint(SanFrancisco, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(City, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, LosAngeles), ecs_field_id(&it, 2));
        test_uint(LosAngeles, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(LosAngeles, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(City, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, Amsterdam), ecs_field_id(&it, 2));
        test_uint(Amsterdam, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(Amsterdam, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_2_this_src_constrain_tgt_var_before_1_lvl() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, State);

    populate_facts(world);

    ecs_add(world, California, State);
    ecs_add(world, Washington, State);
    ecs_add(world, NoordHolland, State);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    ecs_entity_t e4 = ecs_new_entity(world, "e4");
    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e3, LocatedIn, LosAngeles);
    ecs_add_pair(world, e4, LocatedIn, Amsterdam);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "State($x), LocatedIn($this, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(State, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 2));
        test_uint(California, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(California, ecs_iter_get_var(&it, x_var));
        test_uint(SanFrancisco, it.entities[0]);
        test_uint(LosAngeles, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(State, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 2));
        test_uint(California, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(California, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(State, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, LosAngeles), ecs_field_id(&it, 2));
        test_uint(California, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(California, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(State, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, Washington), ecs_field_id(&it, 2));
        test_uint(Washington, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(Washington, ecs_iter_get_var(&it, x_var));
        test_uint(Seattle, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(State, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, NoordHolland), ecs_field_id(&it, 2));
        test_uint(NoordHolland, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(NoordHolland, ecs_iter_get_var(&it, x_var));
        test_uint(Amsterdam, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(State, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, Amsterdam), ecs_field_id(&it, 2));
        test_uint(NoordHolland, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(NoordHolland, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_2_this_src_constrain_tgt_var_before_2_lvl() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Country);

    populate_facts(world);

    ecs_add(world, UnitedStates, Country);
    ecs_add(world, Netherlands, Country);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    ecs_entity_t e4 = ecs_new_entity(world, "e4");
    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e3, LocatedIn, LosAngeles);
    ecs_add_pair(world, e4, LocatedIn, Amsterdam);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Country($x), LocatedIn($this, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(Country, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 2));
        test_uint(UnitedStates, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(California, it.entities[0]);
        test_uint(Washington, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(Country, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 2));
        test_uint(UnitedStates, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(SanFrancisco, it.entities[0]);
        test_uint(LosAngeles, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(Country, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 2));
        test_uint(UnitedStates, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Country, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, LosAngeles), ecs_field_id(&it, 2));
        test_uint(UnitedStates, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Country, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, Washington), ecs_field_id(&it, 2));
        test_uint(UnitedStates, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(Seattle, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Country, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, Netherlands), ecs_field_id(&it, 2));
        test_uint(Netherlands, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(Netherlands, ecs_iter_get_var(&it, x_var));
        test_uint(NoordHolland, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Country, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, NoordHolland), ecs_field_id(&it, 2));
        test_uint(Netherlands, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(Netherlands, ecs_iter_get_var(&it, x_var));
        test_uint(Amsterdam, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Country, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, Amsterdam), ecs_field_id(&it, 2));
        test_uint(Netherlands, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_uint(Netherlands, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_2_this_src_constrain_tgt_var_after_0_lvl() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, City);

    populate_facts(world);

    ecs_add(world, SanFrancisco, City);
    ecs_add(world, LosAngeles, City);
    ecs_add(world, Seattle, City);
    ecs_add(world, Amsterdam, City);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    ecs_entity_t e4 = ecs_new_entity(world, "e4");
    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e3, LocatedIn, LosAngeles);
    ecs_add_pair(world, e4, LocatedIn, Amsterdam);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn($this, $x), City($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Amsterdam), ecs_field_id(&it, 1));
        test_uint(City, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Amsterdam, ecs_field_src(&it, 2));
        test_uint(Amsterdam, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, LosAngeles), ecs_field_id(&it, 1));
        test_uint(City, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(LosAngeles, ecs_field_src(&it, 2));
        test_uint(LosAngeles, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(City, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(SanFrancisco, ecs_field_src(&it, 2));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_2_this_src_constrain_tgt_var_after_1_lvl() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, State);

    populate_facts(world);

    ecs_add(world, California, State);
    ecs_add(world, Washington, State);
    ecs_add(world, NoordHolland, State);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    ecs_entity_t e4 = ecs_new_entity(world, "e4");
    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e3, LocatedIn, LosAngeles);
    ecs_add_pair(world, e4, LocatedIn, Amsterdam);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn($this, $x), State($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, NoordHolland), ecs_field_id(&it, 1));
        test_uint(State, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(NoordHolland, ecs_field_src(&it, 2));
        test_uint(NoordHolland, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(State, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(California, ecs_field_src(&it, 2));
        test_uint(California, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(State, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(California, ecs_field_src(&it, 2));
        test_uint(California, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, NoordHolland), ecs_field_id(&it, 1));
        test_uint(State, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(NoordHolland, ecs_field_src(&it, 2));
        test_uint(NoordHolland, ecs_iter_get_var(&it, x_var));
        test_uint(Amsterdam, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Washington), ecs_field_id(&it, 1));
        test_uint(State, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Washington, ecs_field_src(&it, 2));
        test_uint(Washington, ecs_iter_get_var(&it, x_var));
        test_uint(Seattle, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(State, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(California, ecs_field_src(&it, 2));
        test_uint(California, ecs_iter_get_var(&it, x_var));
        test_uint(SanFrancisco, it.entities[0]);
        test_uint(LosAngeles, it.entities[1]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_2_this_src_constrain_tgt_var_after_2_lvl() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Country);

    populate_facts(world);

    ecs_add(world, UnitedStates, Country);
    ecs_add(world, Netherlands, Country);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    ecs_entity_t e4 = ecs_new_entity(world, "e4");
    ecs_add_pair(world, e1, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e2, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e3, LocatedIn, LosAngeles);
    ecs_add_pair(world, e4, LocatedIn, Amsterdam);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn($this, $x), Country($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Netherlands), ecs_field_id(&it, 1));
        test_uint(Country, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Netherlands, ecs_field_src(&it, 2));
        test_uint(Netherlands, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Country, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_field_src(&it, 2));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Country, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_field_src(&it, 2));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Netherlands), ecs_field_id(&it, 1));
        test_uint(Country, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Netherlands, ecs_field_src(&it, 2));
        test_uint(Netherlands, ecs_iter_get_var(&it, x_var));
        test_uint(Amsterdam, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Netherlands), ecs_field_id(&it, 1));
        test_uint(Country, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Netherlands, ecs_field_src(&it, 2));
        test_uint(Netherlands, ecs_iter_get_var(&it, x_var));
        test_uint(NoordHolland, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Country, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_field_src(&it, 2));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(Seattle, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Country, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_field_src(&it, 2));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(SanFrancisco, it.entities[0]);
        test_uint(LosAngeles, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Country, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_field_src(&it, 2));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(California, it.entities[0]);
        test_uint(Washington, it.entities[1]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_src_tgt_same_var() {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn($x, $x)"
    });

    test_assert(r == NULL);

    ecs_fini(world);
}

void RulesTransitive_1_src_tgt_same_var_reflexive() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_add_id(world, LocatedIn, EcsReflexive);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn($x, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Amsterdam), ecs_field_id(&it, 1));
        test_uint(Amsterdam, ecs_field_src(&it, 1));
        test_uint(Amsterdam, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, NoordHolland), ecs_field_id(&it, 1));
        test_uint(NoordHolland, ecs_field_src(&it, 1));
        test_uint(NoordHolland, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Seattle), ecs_field_id(&it, 1));
        test_uint(Seattle, ecs_field_src(&it, 1));
        test_uint(Seattle, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(SanFrancisco, ecs_field_src(&it, 1));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, LosAngeles), ecs_field_id(&it, 1));
        test_uint(LosAngeles, ecs_field_src(&it, 1));
        test_uint(LosAngeles, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(California, ecs_field_src(&it, 1));
        test_uint(California, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Washington), ecs_field_id(&it, 1));
        test_uint(Washington, ecs_field_src(&it, 1));
        test_uint(Washington, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(UnitedStates, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Netherlands), ecs_field_id(&it, 1));
        test_uint(Netherlands, ecs_field_src(&it, 1));
        test_uint(Netherlands, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_src_tgt_same_this_var_reflexive() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_add_id(world, LocatedIn, EcsReflexive);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn($this, $this)"
    });

    test_assert(r != NULL);

    int this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);
    test_assert(this_var != 0);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Amsterdam), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Amsterdam, ecs_iter_get_var(&it, this_var));
        test_uint(Amsterdam, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, NoordHolland), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(NoordHolland, ecs_iter_get_var(&it, this_var));
        test_uint(NoordHolland, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Seattle), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Seattle, ecs_iter_get_var(&it, this_var));
        test_uint(Seattle, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, this_var));
        test_uint(SanFrancisco, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, LosAngeles), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(LosAngeles, ecs_iter_get_var(&it, this_var));
        test_uint(LosAngeles, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(California, ecs_iter_get_var(&it, this_var));
        test_uint(California, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Washington), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Washington, ecs_iter_get_var(&it, this_var));
        test_uint(Washington, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, this_var));
        test_uint(UnitedStates, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Netherlands), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Netherlands, ecs_iter_get_var(&it, this_var));
        test_uint(Netherlands, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_1_any_src_tgt_var() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn(_, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, NoordHolland), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(NoordHolland, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Netherlands), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Netherlands, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Washington), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Washington, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(California, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_not_transitive_ent_tgt() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "!LocatedIn($this, UnitedStates), Location($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Location, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(Earth, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Location, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(UnitedStates, it.entities[0]);
        test_uint(Netherlands, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Location, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(NoordHolland, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Location, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(Amsterdam, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_not_transitive_var_tgt() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "!LocatedIn($this, $x), Location($this)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(Location, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(Earth, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_not_transitive_ent_tgt_written() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Location($this), !LocatedIn($this, UnitedStates)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Location, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(Earth, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(Location, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(UnitedStates, it.entities[0]);
        test_uint(Netherlands, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Location, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(NoordHolland, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Location, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(Amsterdam, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_not_transitive_var_tgt_written() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Location($this), !LocatedIn($this, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Location, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(Earth, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_optional_transitive_ent_tgt() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "?LocatedIn($this, UnitedStates), Location($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Location, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(Earth, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Location, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(UnitedStates, it.entities[0]);
        test_uint(Netherlands, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Location, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(California, it.entities[0]);
        test_uint(Washington, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Location, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(SanFrancisco, it.entities[0]);
        test_uint(LosAngeles, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Location, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(Seattle, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Location, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(NoordHolland, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Location, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(Amsterdam, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_optional_transitive_var_tgt() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "?LocatedIn($this, $x), Tag($this)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_entity_t e4 = ecs_new(world, Tag);

    ecs_add_pair(world, e2, LocatedIn, Earth);
    ecs_add_pair(world, e3, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e4, LocatedIn, Washington);
    ecs_add_pair(world, e4, LocatedIn, NoordHolland);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));

        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(false, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(California, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Washington), ecs_field_id(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(Washington, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, NoordHolland), ecs_field_id(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(NoordHolland, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Netherlands), ecs_field_id(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(Netherlands, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(Tag, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_optional_transitive_ent_tgt_written() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Location($this), ?LocatedIn($this, UnitedStates)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Location, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(Earth, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(Location, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(UnitedStates, it.entities[0]);
        test_uint(Netherlands, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(Location, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(California, it.entities[0]);
        test_uint(Washington, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(Location, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(SanFrancisco, it.entities[0]);
        test_uint(LosAngeles, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Location, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(Seattle, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Location, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(NoordHolland, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Location, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(Amsterdam, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_optional_transitive_var_tgt_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this), ?LocatedIn($this, $x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_entity_t e4 = ecs_new(world, Tag);

    ecs_add_pair(world, e2, LocatedIn, Earth);
    ecs_add_pair(world, e3, LocatedIn, SanFrancisco);
    ecs_add_pair(world, e4, LocatedIn, Washington);
    ecs_add_pair(world, e4, LocatedIn, NoordHolland);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, EcsWildcard), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(false, ecs_field_is_set(&it, 2));
        test_uint(EcsWildcard, ecs_iter_get_var(&it, x_var));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(SanFrancisco, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(California, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, Washington), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(Washington, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, NoordHolland), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(NoordHolland, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, Netherlands), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(Netherlands, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(e4, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_2_var_src_w_same_tgt_ent() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn($x, California), LocatedIn($y, California), Tag($x), Tag($y)"
    });

    test_assert(r != NULL);
    
    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);
    int y_var = ecs_rule_find_var(r, "y");
    test_assert(y_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(false, ecs_rule_next(&it));
    }

    ecs_entity_t e1 = ecs_new_w_pair(world, LocatedIn, SanFrancisco);
    ecs_entity_t e2 = ecs_new_w_pair(world, LocatedIn, California);
    ecs_entity_t e3 = ecs_new_w_pair(world, LocatedIn, Seattle);
    ecs_entity_t e4 = ecs_new_w_pair(world, LocatedIn, Amsterdam);

    ecs_add(world, e1, Tag);
    ecs_add(world, e2, Tag);
    ecs_add(world, e3, Tag);
    ecs_add(world, e4, Tag);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 2));
        test_uint(Tag, ecs_field_id(&it, 3));
        test_uint(Tag, ecs_field_id(&it, 4));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e2, ecs_field_src(&it, 3));
        test_uint(e2, ecs_field_src(&it, 4));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 2));
        test_uint(Tag, ecs_field_id(&it, 3));
        test_uint(Tag, ecs_field_id(&it, 4));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e2, ecs_field_src(&it, 3));
        test_uint(e1, ecs_field_src(&it, 4));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, x_var));
        test_uint(e1, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 2));
        test_uint(Tag, ecs_field_id(&it, 3));
        test_uint(Tag, ecs_field_id(&it, 4));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_uint(e1, ecs_field_src(&it, 3));
        test_uint(e2, ecs_field_src(&it, 4));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e2, ecs_iter_get_var(&it, y_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 1));
        test_uint(ecs_pair(LocatedIn, SanFrancisco), ecs_field_id(&it, 2));
        test_uint(Tag, ecs_field_id(&it, 3));
        test_uint(Tag, ecs_field_id(&it, 4));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_uint(e1, ecs_field_src(&it, 3));
        test_uint(e1, ecs_field_src(&it, 4));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));
        test_uint(e1, ecs_iter_get_var(&it, y_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_self_target() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn($this, $x:self)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, Earth), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(Earth, ecs_iter_get_var(&it, x_var));
        test_uint(UnitedStates, it.entities[0]);
        test_uint(Netherlands, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, UnitedStates), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(UnitedStates, ecs_iter_get_var(&it, x_var));
        test_uint(California, it.entities[0]);
        test_uint(Washington, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, California), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(California, ecs_iter_get_var(&it, x_var));
        test_uint(SanFrancisco, it.entities[0]);
        test_uint(LosAngeles, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Washington), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(Washington, ecs_iter_get_var(&it, x_var));
        test_uint(Seattle, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, Netherlands), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(Netherlands, ecs_iter_get_var(&it, x_var));
        test_uint(NoordHolland, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, NoordHolland), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(NoordHolland, ecs_iter_get_var(&it, x_var));
        test_uint(Amsterdam, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesTransitive_any_target() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "LocatedIn($this, _)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(UnitedStates, it.entities[0]);
        test_uint(Netherlands, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(California, it.entities[0]);
        test_uint(Washington, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(2, it.count);
        test_uint(ecs_pair(LocatedIn, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(SanFrancisco, it.entities[0]);
        test_uint(LosAngeles, it.entities[1]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(Seattle, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(NoordHolland, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(ecs_pair(LocatedIn, EcsWildcard), ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(Amsterdam, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}
