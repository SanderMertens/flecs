#include <addons.h>

static ecs_entity_t recycled_id(ecs_world_t *world, const char *name) {
    ecs_entity_t result = ecs_new_id(world);
    ecs_delete(world, result);
    ecs_entity_t result_2 = ecs_new_id(world);
    test_assert(result_2 != (uint32_t)result);
    ecs_set_name(world, result_2, name);
    return result_2;
}

void RulesRecycled_recycled_vars(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    
    ecs_entity_t src = recycled_id(world, "src");
    ecs_entity_t rel = recycled_id(world, "rel");
    ecs_add(world, src, Tag);
    ecs_add_id(world, src, rel);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x($y), Tag($y)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);
    int32_t y_var = ecs_rule_find_var(r, "y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(true, ecs_rule_next(&it));
    test_uint(0, it.count);
    test_uint(Tag, ecs_field_id(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 2));
    test_uint(src, ecs_field_src(&it, 1));
    test_uint(src, ecs_field_src(&it, 2));
    test_uint(Tag, ecs_iter_get_var(&it, x_var));
    test_uint(src, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_rule_next(&it));
    test_uint(0, it.count);
    test_uint(rel, ecs_field_id(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 2));
    test_uint(src, ecs_field_src(&it, 1));
    test_uint(src, ecs_field_src(&it, 2));
    test_uint(rel, ecs_iter_get_var(&it, x_var));
    test_uint(src, ecs_iter_get_var(&it, y_var));

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesRecycled_recycled_pair_vars(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    
    ecs_entity_t src = recycled_id(world, "src");
    ecs_entity_t rel = recycled_id(world, "rel");
    ecs_entity_t tgt = recycled_id(world, "tgt");
    ecs_add(world, src, Tag);
    ecs_add_pair(world, src, rel, tgt);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x($y, $z), Tag($y)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);
    int32_t y_var = ecs_rule_find_var(r, "y");
    test_assert(y_var != -1);
    int32_t z_var = ecs_rule_find_var(r, "z");
    test_assert(z_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(true, ecs_rule_next(&it));
    test_uint(0, it.count);
    test_uint(ecs_pair(ecs_id(EcsIdentifier), EcsName), ecs_field_id(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 2));
    test_uint(src, ecs_field_src(&it, 1));
    test_uint(src, ecs_field_src(&it, 2));
    test_uint(ecs_id(EcsIdentifier), ecs_iter_get_var(&it, x_var));
    test_uint(src, ecs_iter_get_var(&it, y_var));
    test_uint(EcsName, ecs_iter_get_var(&it, z_var));

    test_bool(true, ecs_rule_next(&it));
    test_uint(0, it.count);
    test_uint(ecs_pair(rel, tgt), ecs_field_id(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 2));
    test_uint(src, ecs_field_src(&it, 1));
    test_uint(src, ecs_field_src(&it, 2));
    test_uint(rel, ecs_iter_get_var(&it, x_var));
    test_uint(src, ecs_iter_get_var(&it, y_var));
    test_uint(tgt, ecs_iter_get_var(&it, z_var));

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesRecycled_recycled_this_ent_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    
    ecs_entity_t src = recycled_id(world, "src");
    ecs_entity_t rel = recycled_id(world, "rel");
    ecs_entity_t tgt = recycled_id(world, "tgt");
    ecs_add(world, src, Tag);
    ecs_add_pair(world, src, rel, tgt);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x($y, $this), Tag($y)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);
    int32_t y_var = ecs_rule_find_var(r, "y");
    test_assert(y_var != -1);
    int32_t this_var = ecs_rule_find_var(r, "This");
    test_assert(this_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(true, ecs_rule_next(&it));
    test_uint(1, it.count);
    test_uint(ecs_pair(ecs_id(EcsIdentifier), EcsName), ecs_field_id(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 2));
    test_uint(src, ecs_field_src(&it, 1));
    test_uint(src, ecs_field_src(&it, 2));
    test_uint(ecs_id(EcsIdentifier), ecs_iter_get_var(&it, x_var));
    test_uint(src, ecs_iter_get_var(&it, y_var));
    test_uint(EcsName, ecs_iter_get_var(&it, this_var));
    test_uint(EcsName, it.entities[0]);

    test_bool(true, ecs_rule_next(&it));
    test_uint(1, it.count);
    test_uint(ecs_pair(rel, tgt), ecs_field_id(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 2));
    test_uint(src, ecs_field_src(&it, 1));
    test_uint(src, ecs_field_src(&it, 2));
    test_uint(rel, ecs_iter_get_var(&it, x_var));
    test_uint(src, ecs_iter_get_var(&it, y_var));
    test_uint(tgt, ecs_iter_get_var(&it, this_var));
    test_uint(tgt, it.entities[0]);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void RulesRecycled_has_recycled_id_from_pair(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    
    ecs_entity_t src = recycled_id(world, "src");
    ecs_entity_t rel = recycled_id(world, "rel");
    ecs_entity_t tgt = recycled_id(world, "tgt");
    ecs_add_pair(world, src, rel, tgt);
    ecs_add_id(world, src, tgt);
    ecs_add(world, src, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "$x($y, $z), $z($y), Tag($y)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "x");
    test_assert(x_var != -1);
    int32_t y_var = ecs_rule_find_var(r, "y");
    test_assert(y_var != -1);
    int32_t z_var = ecs_rule_find_var(r, "z");
    test_assert(z_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(true, ecs_rule_next(&it));
    test_uint(0, it.count);
    test_uint(ecs_pair(rel, tgt), ecs_field_id(&it, 1));
    test_uint(tgt, ecs_field_id(&it, 2));
    test_uint(Tag, ecs_field_id(&it, 3));
    test_uint(src, ecs_field_src(&it, 1));
    test_uint(src, ecs_field_src(&it, 2));
    test_uint(src, ecs_field_src(&it, 3));
    test_uint(rel, ecs_iter_get_var(&it, x_var));
    test_uint(src, ecs_iter_get_var(&it, y_var));
    test_uint(tgt, ecs_iter_get_var(&it, z_var));

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}
