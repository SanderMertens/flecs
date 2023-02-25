#include <addons.h>

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

void RulesComponentInheritance_1_ent_0_lvl() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add(world, e1, Warlock);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Warlock(e1)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Warlock, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesComponentInheritance_1_ent_1_lvl() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add(world, e1, Warlock);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Warrior(e1)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Warlock, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesComponentInheritance_1_ent_2_lvl() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add(world, e1, Warlock);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "MeleeUnit(e1)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Warlock, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesComponentInheritance_1_ent_3_lvl() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add(world, e1, Warlock);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Unit(e1)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Warlock, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));

        test_bool(true, ecs_rule_next(&it)); /* not ideal, diamond problem */
        test_uint(0, it.count);
        test_uint(Warlock, ecs_field_id(&it, 1)); 
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}


void RulesComponentInheritance_1_this_0_lvl() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new(world, Warlock);
    ecs_entity_t e2 = ecs_new(world, Wizard);
    ecs_entity_t e3 = ecs_new(world, Warrior);
    ecs_entity_t e4 = ecs_new(world, Archer);
    ecs_entity_t e5 = ecs_new(world, MeleeUnit);
    ecs_entity_t e6 = ecs_new(world, RangedUnit);
    ecs_entity_t e7 = ecs_new(world, Unit);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Unit($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Unit, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e7, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(MeleeUnit, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e5, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(RangedUnit, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e6, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Warrior, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Warlock, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Archer, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e4, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Wizard, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Warlock, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesComponentInheritance_1_this_1_lvl() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new(world, Warlock);
    /* ecs_entity_t e2 = */ ecs_new(world, Wizard);
    ecs_entity_t e3 = ecs_new(world, Warrior);
    /* ecs_entity_t e4 = */ ecs_new(world, Archer);
    ecs_entity_t e5 = ecs_new(world, MeleeUnit);
    /* ecs_entity_t e6 = */ ecs_new(world, RangedUnit);
    /* ecs_entity_t e7 = */ ecs_new(world, Unit);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "MeleeUnit($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(MeleeUnit, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e5, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Warrior, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Warlock, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesComponentInheritance_1_this_2_lvl() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new(world, Warlock);
    /* ecs_entity_t e2 = */ ecs_new(world, Wizard);
    ecs_entity_t e3 = ecs_new(world, Warrior);
    /* ecs_entity_t e4 = */ ecs_new(world, Archer);
    /* ecs_entity_t e5 = */ ecs_new(world, MeleeUnit);
    /* ecs_entity_t e6 = */ ecs_new(world, RangedUnit);
    /* ecs_entity_t e7 = */ ecs_new(world, Unit);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Warrior($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Warrior, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Warlock, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesComponentInheritance_1_this_3_lvl() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new(world, Warlock);
    /* ecs_entity_t e2 = */ ecs_new(world, Wizard);
    /* ecs_entity_t e3 = */ ecs_new(world, Warrior);
    /* ecs_entity_t e4 = */ ecs_new(world, Archer);
    /* ecs_entity_t e5 = */ ecs_new(world, MeleeUnit);
    /* ecs_entity_t e6 = */ ecs_new(world, RangedUnit);
    /* ecs_entity_t e7 = */ ecs_new(world, Unit);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Warlock($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Warlock, ecs_field_id(&it, 1));
        test_uint(0, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesComponentInheritance_1_this_0_lvl_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_entity_t e4 = ecs_new(world, Tag);
    ecs_entity_t e5 = ecs_new(world, Tag);
    ecs_entity_t e6 = ecs_new(world, Tag);
    ecs_entity_t e7 = ecs_new(world, Tag);

    ecs_add(world, e1, Warlock);
    ecs_add(world, e2, Wizard);
    ecs_add(world, e3, Warrior);
    ecs_add(world, e4, Archer);
    ecs_add(world, e5, MeleeUnit);
    ecs_add(world, e6, RangedUnit);
    ecs_add(world, e7, Unit);

    /* entities that don't match query */
    ecs_new(world, Unit);
    ecs_new(world, MeleeUnit);
    ecs_new(world, Warrior);
    ecs_new(world, Warlock);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this), Unit($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(Warlock, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(Warlock, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(Wizard, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e2, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(Warrior, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(Archer, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e4, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(MeleeUnit, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e5, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(RangedUnit, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e6, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(Unit, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e7, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesComponentInheritance_1_this_1_lvl_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_entity_t e4 = ecs_new(world, Tag);
    ecs_entity_t e5 = ecs_new(world, Tag);
    ecs_entity_t e6 = ecs_new(world, Tag);
    ecs_entity_t e7 = ecs_new(world, Tag);

    ecs_add(world, e1, Warlock);
    ecs_add(world, e2, Wizard);
    ecs_add(world, e3, Warrior);
    ecs_add(world, e4, Archer);
    ecs_add(world, e5, MeleeUnit);
    ecs_add(world, e6, RangedUnit);
    ecs_add(world, e7, Unit);

    /* entities that don't match query */
    ecs_new(world, Unit);
    ecs_new(world, MeleeUnit);
    ecs_new(world, Warrior);
    ecs_new(world, Warlock);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this), MeleeUnit($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(Warlock, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(Warrior, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e3, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(MeleeUnit, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e5, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesComponentInheritance_1_this_2_lvl_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_entity_t e4 = ecs_new(world, Tag);
    ecs_entity_t e5 = ecs_new(world, Tag);
    ecs_entity_t e6 = ecs_new(world, Tag);
    ecs_entity_t e7 = ecs_new(world, Tag);

    ecs_add(world, e1, Warlock);
    ecs_add(world, e2, Wizard);
    ecs_add(world, e3, Warrior);
    ecs_add(world, e4, Archer);
    ecs_add(world, e5, MeleeUnit);
    ecs_add(world, e6, RangedUnit);
    ecs_add(world, e7, Unit);

    /* entities that don't match query */
    ecs_new(world, Unit);
    ecs_new(world, MeleeUnit);
    ecs_new(world, Warrior);
    ecs_new(world, Warlock);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this), Warrior($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(Warlock, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);

        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(Warrior, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e3, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesComponentInheritance_1_this_3_lvl_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_entity_t e4 = ecs_new(world, Tag);
    ecs_entity_t e5 = ecs_new(world, Tag);
    ecs_entity_t e6 = ecs_new(world, Tag);
    ecs_entity_t e7 = ecs_new(world, Tag);

    ecs_add(world, e1, Warlock);
    ecs_add(world, e2, Wizard);
    ecs_add(world, e3, Warrior);
    ecs_add(world, e4, Archer);
    ecs_add(world, e5, MeleeUnit);
    ecs_add(world, e6, RangedUnit);
    ecs_add(world, e7, Unit);

    /* entities that don't match query */
    ecs_new(world, Unit);
    ecs_new(world, MeleeUnit);
    ecs_new(world, Warrior);
    ecs_new(world, Warlock);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($this), Warlock($this)"
    });

    test_assert(r != NULL);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(1, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(Warlock, ecs_field_id(&it, 2));
        test_uint(0, ecs_field_src(&it, 1));
        test_uint(0, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e1, it.entities[0]);

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesComponentInheritance_1_var_0_lvl() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new(world, Warlock);
    ecs_entity_t e2 = ecs_new(world, Wizard);
    ecs_entity_t e3 = ecs_new(world, Warrior);
    ecs_entity_t e4 = ecs_new(world, Archer);
    ecs_entity_t e5 = ecs_new(world, MeleeUnit);
    ecs_entity_t e6 = ecs_new(world, RangedUnit);
    ecs_entity_t e7 = ecs_new(world, Unit);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Unit($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Unit, ecs_field_id(&it, 1));
        test_uint(e7, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e7, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(MeleeUnit, ecs_field_id(&it, 1));
        test_uint(e5, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e5, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(RangedUnit, ecs_field_id(&it, 1));
        test_uint(e6, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e6, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Warrior, ecs_field_id(&it, 1));
        test_uint(e3, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e3, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Warlock, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Archer, ecs_field_id(&it, 1));
        test_uint(e4, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e4, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Wizard, ecs_field_id(&it, 1));
        test_uint(e2, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Warlock, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesComponentInheritance_1_var_1_lvl() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new(world, Warlock);
    /* ecs_entity_t e2 = */ ecs_new(world, Wizard);
    ecs_entity_t e3 = ecs_new(world, Warrior);
    /* ecs_entity_t e4 = */ ecs_new(world, Archer);
    ecs_entity_t e5 = ecs_new(world, MeleeUnit);
    /* ecs_entity_t e6 = */ ecs_new(world, RangedUnit);
    /* ecs_entity_t e7 = */ ecs_new(world, Unit);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "MeleeUnit($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(MeleeUnit, ecs_field_id(&it, 1));
        test_uint(e5, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e5, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Warrior, ecs_field_id(&it, 1));
        test_uint(e3, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e3, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Warlock, ecs_field_id(&it, 1));
        test_uint(e1, ecs_field_src(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesComponentInheritance_1_var_2_lvl() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new(world, Warlock);
    /* ecs_entity_t e2 = */ ecs_new(world, Wizard);
    ecs_entity_t e3 = ecs_new(world, Warrior);
    /* ecs_entity_t e4 = */ ecs_new(world, Archer);
    /* ecs_entity_t e5 = */ ecs_new(world, MeleeUnit);
    /* ecs_entity_t e6 = */ ecs_new(world, RangedUnit);
    /* ecs_entity_t e7 = */ ecs_new(world, Unit);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Warrior($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Warrior, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e3, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Warlock, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesComponentInheritance_1_var_3_lvl() {
    ecs_world_t *world = ecs_init();

    populate_facts(world);

    ecs_entity_t e1 = ecs_new(world, Warlock);
    /* ecs_entity_t e2 = */ ecs_new(world, Wizard);
    /* ecs_entity_t e3 = */ ecs_new(world, Warrior);
    /* ecs_entity_t e4 = */ ecs_new(world, Archer);
    /* ecs_entity_t e5 = */ ecs_new(world, MeleeUnit);
    /* ecs_entity_t e6 = */ ecs_new(world, RangedUnit);
    /* ecs_entity_t e7 = */ ecs_new(world, Unit);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Warlock($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Warlock, ecs_field_id(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesComponentInheritance_1_var_0_lvl_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_entity_t e4 = ecs_new(world, Tag);
    ecs_entity_t e5 = ecs_new(world, Tag);
    ecs_entity_t e6 = ecs_new(world, Tag);
    ecs_entity_t e7 = ecs_new(world, Tag);

    ecs_add(world, e1, Warlock);
    ecs_add(world, e2, Wizard);
    ecs_add(world, e3, Warrior);
    ecs_add(world, e4, Archer);
    ecs_add(world, e5, MeleeUnit);
    ecs_add(world, e6, RangedUnit);
    ecs_add(world, e7, Unit);

    /* entities that don't match query */
    ecs_new(world, Unit);
    ecs_new(world, MeleeUnit);
    ecs_new(world, Warrior);
    ecs_new(world, Warlock);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($x), Unit($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(Warlock, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(Warlock, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(Wizard, ecs_field_id(&it, 2));
        test_uint(e2, ecs_field_src(&it, 1));
        test_uint(e2, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e2, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(Warrior, ecs_field_id(&it, 2));
        test_uint(e3, ecs_field_src(&it, 1));
        test_uint(e3, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e3, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(Archer, ecs_field_id(&it, 2));
        test_uint(e4, ecs_field_src(&it, 1));
        test_uint(e4, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e4, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(MeleeUnit, ecs_field_id(&it, 2));
        test_uint(e5, ecs_field_src(&it, 1));
        test_uint(e5, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e5, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(RangedUnit, ecs_field_id(&it, 2));
        test_uint(e6, ecs_field_src(&it, 1));
        test_uint(e6, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e6, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(Unit, ecs_field_id(&it, 2));
        test_uint(e7, ecs_field_src(&it, 1));
        test_uint(e7, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e7, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesComponentInheritance_1_var_1_lvl_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_entity_t e4 = ecs_new(world, Tag);
    ecs_entity_t e5 = ecs_new(world, Tag);
    ecs_entity_t e6 = ecs_new(world, Tag);
    ecs_entity_t e7 = ecs_new(world, Tag);

    ecs_add(world, e1, Warlock);
    ecs_add(world, e2, Wizard);
    ecs_add(world, e3, Warrior);
    ecs_add(world, e4, Archer);
    ecs_add(world, e5, MeleeUnit);
    ecs_add(world, e6, RangedUnit);
    ecs_add(world, e7, Unit);

    /* entities that don't match query */
    ecs_new(world, Unit);
    ecs_new(world, MeleeUnit);
    ecs_new(world, Warrior);
    ecs_new(world, Warlock);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($x), MeleeUnit($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(Warlock, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(Warrior, ecs_field_id(&it, 2));
        test_uint(e3, ecs_field_src(&it, 1));
        test_uint(e3, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e3, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(MeleeUnit, ecs_field_id(&it, 2));
        test_uint(e5, ecs_field_src(&it, 1));
        test_uint(e5, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e5, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesComponentInheritance_1_var_2_lvl_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_entity_t e4 = ecs_new(world, Tag);
    ecs_entity_t e5 = ecs_new(world, Tag);
    ecs_entity_t e6 = ecs_new(world, Tag);
    ecs_entity_t e7 = ecs_new(world, Tag);

    ecs_add(world, e1, Warlock);
    ecs_add(world, e2, Wizard);
    ecs_add(world, e3, Warrior);
    ecs_add(world, e4, Archer);
    ecs_add(world, e5, MeleeUnit);
    ecs_add(world, e6, RangedUnit);
    ecs_add(world, e7, Unit);

    /* entities that don't match query */
    ecs_new(world, Unit);
    ecs_new(world, MeleeUnit);
    ecs_new(world, Warrior);
    ecs_new(world, Warlock);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($x), Warrior($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(Warlock, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(Warrior, ecs_field_id(&it, 2));
        test_uint(e3, ecs_field_src(&it, 1));
        test_uint(e3, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e3, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesComponentInheritance_1_var_3_lvl_written() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    populate_facts(world);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);
    ecs_entity_t e4 = ecs_new(world, Tag);
    ecs_entity_t e5 = ecs_new(world, Tag);
    ecs_entity_t e6 = ecs_new(world, Tag);
    ecs_entity_t e7 = ecs_new(world, Tag);

    ecs_add(world, e1, Warlock);
    ecs_add(world, e2, Wizard);
    ecs_add(world, e3, Warrior);
    ecs_add(world, e4, Archer);
    ecs_add(world, e5, MeleeUnit);
    ecs_add(world, e6, RangedUnit);
    ecs_add(world, e7, Unit);

    /* entities that don't match query */
    ecs_new(world, Unit);
    ecs_new(world, MeleeUnit);
    ecs_new(world, Warrior);
    ecs_new(world, Warlock);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag($x), Warlock($x)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);

    {
        ecs_iter_t it = ecs_rule_iter(world, r);
        test_bool(true, ecs_rule_next(&it));
        test_uint(0, it.count);
        test_uint(Tag, ecs_field_id(&it, 1));
        test_uint(Warlock, ecs_field_id(&it, 2));
        test_uint(e1, ecs_field_src(&it, 1));
        test_uint(e1, ecs_field_src(&it, 2));
        test_bool(true, ecs_field_is_set(&it, 1));
        test_bool(true, ecs_field_is_set(&it, 2));
        test_uint(e1, ecs_iter_get_var(&it, x_var));

        test_bool(false, ecs_rule_next(&it));
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}
