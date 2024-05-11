#include <query.h>

static ecs_query_cache_kind_t cache_kind = EcsQueryCacheDefault;

void Recycled_setup(void) {
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

static ecs_entity_t recycled_id(ecs_world_t *world, const char *name) {
    ecs_entity_t result = ecs_new(world);
    ecs_delete(world, result);
    ecs_entity_t result_2 = ecs_new(world);
    test_assert(result_2 != (uint32_t)result);
    ecs_set_name(world, result_2, name);
    return result_2;
}

void Recycled_recycled_vars(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    
    ecs_entity_t src = recycled_id(world, "src");
    ecs_entity_t rel = recycled_id(world, "rel");
    ecs_add(world, src, Tag);
    ecs_add_id(world, src, rel);

    ecs_query_t *r = ecs_query(world, {
        .expr = "$x($y), Tag($y)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);
    int32_t y_var = ecs_query_find_var(r, "y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_uint(0, it.count);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(Tag, ecs_field_id(&it, 1));
    test_uint(src, ecs_field_src(&it, 0));
    test_uint(src, ecs_field_src(&it, 1));
    test_uint(Tag, ecs_iter_get_var(&it, x_var));
    test_uint(src, ecs_iter_get_var(&it, y_var));

    test_bool(true, ecs_query_next(&it));
    test_uint(0, it.count);
    test_uint(rel, ecs_field_id(&it, 0));
    test_uint(Tag, ecs_field_id(&it, 1));
    test_uint(src, ecs_field_src(&it, 0));
    test_uint(src, ecs_field_src(&it, 1));
    test_uint(rel, ecs_iter_get_var(&it, x_var));
    test_uint(src, ecs_iter_get_var(&it, y_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Recycled_recycled_pair_vars(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    
    ecs_entity_t src = recycled_id(world, "src");
    ecs_entity_t rel = recycled_id(world, "rel");
    ecs_entity_t tgt = recycled_id(world, "tgt");
    ecs_add(world, src, Tag);
    ecs_add_pair(world, src, rel, tgt);

    ecs_query_t *r = ecs_query(world, {
        .expr = "$x($y, $z), Tag($y)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);
    int32_t y_var = ecs_query_find_var(r, "y");
    test_assert(y_var != -1);
    int32_t z_var = ecs_query_find_var(r, "z");
    test_assert(z_var != -1);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_uint(0, it.count);
    test_uint(ecs_pair(ecs_id(EcsIdentifier), EcsName), ecs_field_id(&it, 0));
    test_uint(Tag, ecs_field_id(&it, 1));
    test_uint(src, ecs_field_src(&it, 0));
    test_uint(src, ecs_field_src(&it, 1));
    test_uint(ecs_id(EcsIdentifier), ecs_iter_get_var(&it, x_var));
    test_uint(src, ecs_iter_get_var(&it, y_var));
    test_uint(EcsName, ecs_iter_get_var(&it, z_var));

    test_bool(true, ecs_query_next(&it));
    test_uint(0, it.count);
    test_uint(ecs_pair(rel, tgt), ecs_field_id(&it, 0));
    test_uint(Tag, ecs_field_id(&it, 1));
    test_uint(src, ecs_field_src(&it, 0));
    test_uint(src, ecs_field_src(&it, 1));
    test_uint(rel, ecs_iter_get_var(&it, x_var));
    test_uint(src, ecs_iter_get_var(&it, y_var));
    test_uint(tgt, ecs_iter_get_var(&it, z_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Recycled_recycled_this_ent_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    
    ecs_entity_t src = recycled_id(world, "src");
    ecs_entity_t rel = recycled_id(world, "rel");
    ecs_entity_t tgt = recycled_id(world, "tgt");
    ecs_add(world, src, Tag);
    ecs_add_pair(world, src, rel, tgt);

    ecs_query_t *r = ecs_query(world, {
        .expr = "$x($y, $this), Tag($y)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);
    int32_t y_var = ecs_query_find_var(r, "y");
    test_assert(y_var != -1);
    int32_t this_var = ecs_query_find_var(r, "this");
    test_assert(this_var != -1);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_uint(1, it.count);
    test_uint(ecs_pair(ecs_id(EcsIdentifier), EcsName), ecs_field_id(&it, 0));
    test_uint(Tag, ecs_field_id(&it, 1));
    test_uint(src, ecs_field_src(&it, 0));
    test_uint(src, ecs_field_src(&it, 1));
    test_uint(ecs_id(EcsIdentifier), ecs_iter_get_var(&it, x_var));
    test_uint(src, ecs_iter_get_var(&it, y_var));
    test_uint(EcsName, ecs_iter_get_var(&it, this_var));
    test_uint(EcsName, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_uint(1, it.count);
    test_uint(ecs_pair(rel, tgt), ecs_field_id(&it, 0));
    test_uint(Tag, ecs_field_id(&it, 1));
    test_uint(src, ecs_field_src(&it, 0));
    test_uint(src, ecs_field_src(&it, 1));
    test_uint(rel, ecs_iter_get_var(&it, x_var));
    test_uint(src, ecs_iter_get_var(&it, y_var));
    test_uint(tgt, ecs_iter_get_var(&it, this_var));
    test_uint(tgt, it.entities[0]);

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Recycled_has_recycled_id_from_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    
    ecs_entity_t src = recycled_id(world, "src");
    ecs_entity_t rel = recycled_id(world, "rel");
    ecs_entity_t tgt = recycled_id(world, "tgt");
    ecs_add_pair(world, src, rel, tgt);
    ecs_add_id(world, src, tgt);
    ecs_add(world, src, Tag);

    ecs_query_t *r = ecs_query(world, {
        .expr = "$x($y, $z), $z($y), Tag($y)",
        .cache_kind = cache_kind
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_query_find_var(r, "x");
    test_assert(x_var != -1);
    int32_t y_var = ecs_query_find_var(r, "y");
    test_assert(y_var != -1);
    int32_t z_var = ecs_query_find_var(r, "z");
    test_assert(z_var != -1);

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_uint(0, it.count);
    test_uint(ecs_pair(rel, tgt), ecs_field_id(&it, 0));
    test_uint(tgt, ecs_field_id(&it, 1));
    test_uint(Tag, ecs_field_id(&it, 2));
    test_uint(src, ecs_field_src(&it, 0));
    test_uint(src, ecs_field_src(&it, 1));
    test_uint(src, ecs_field_src(&it, 2));
    test_uint(rel, ecs_iter_get_var(&it, x_var));
    test_uint(src, ecs_iter_get_var(&it, y_var));
    test_uint(tgt, ecs_iter_get_var(&it, z_var));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}

void Recycled_recycled_pair(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t rel = ecs_entity(world, { .name = "Rel" });
    ecs_entity_t tgt = ecs_entity(world, { .name = "Tgt" });
    ecs_delete(world, rel);
    ecs_delete(world, tgt);
    rel = ecs_entity(world, { .name = "Rel" });
    tgt = ecs_entity(world, { .name = "Tgt" });

    ecs_entity_t e = ecs_new_w_pair(world, rel, tgt);

    ecs_query_t *q = ecs_query(world, {
        .terms[0] = {
            .first.id = rel,
            .second.id = tgt
        },
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e);
    test_uint(ecs_field_id(&it, 0), ecs_pair(rel, tgt));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Recycled_recycled_component_id(void) {
    ecs_world_t* ecs = ecs_mini();

    for (int i = 0; i < FLECS_HI_COMPONENT_ID; i ++) {
        ecs_new_low_id(ecs);
    }

    ecs_entity_t e = ecs_new(ecs);
    ecs_delete(ecs, e);

    ECS_COMPONENT(ecs, Position);

    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { .id = ecs_id(Position) }
        },
        .cache_kind = cache_kind
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_insert(ecs, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(ecs, ecs_value(Position, {20, 30}));

    ecs_iter_t it = ecs_query_iter(ecs, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 2);
    test_uint(it.entities[0], e1);
    test_uint(it.entities[1], e2);
    Position *p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(ecs);
}
