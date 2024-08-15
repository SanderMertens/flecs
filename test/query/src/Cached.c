#include <query.h>
#include <stdlib.h>

void Cached_simple_query_existing_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));

    ecs_entity_t e = ecs_new_w(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_bool(false, ecs_query_next(&it));


    ecs_fini(world);
}

void Cached_simple_query_2_existing_tables(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_add(world, e2, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));


    ecs_fini(world);
}

void Cached_simple_query_new_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t e = ecs_new_w(world, TagA);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_bool(false, ecs_query_next(&it));


    ecs_fini(world);
}

void Cached_simple_query_2_new_tables(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_add(world, e2, TagB);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_bool(false, ecs_query_next(&it));


    ecs_fini(world);
}

void Cached_simple_query_existing_and_new_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));

    ecs_entity_t e1 = ecs_new_w(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_add(world, e2, TagB);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_bool(false, ecs_query_next(&it));


    ecs_fini(world);
}

void Cached_wildcard_query_existing_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));
    ECS_ENTITY(world, ObjA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, ObjB, (OnInstantiate, Inherit));

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, ObjA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, ObjB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, *)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_pair(Rel, ObjA), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_pair(Rel, ObjB), ecs_field_id(&it, 0));
    test_bool(false, ecs_query_next(&it));


    ecs_fini(world);
}

void Cached_wildcard_query_new_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));
    ECS_ENTITY(world, ObjA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, ObjB, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, *)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, ObjA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, ObjB);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_pair(Rel, ObjA), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_pair(Rel, ObjB), ecs_field_id(&it, 0));
    test_bool(false, ecs_query_next(&it));


    ecs_fini(world);
}

void Cached_wildcard_query_existing_table_2_results_p_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));
    ECS_ENTITY(world, ObjA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, ObjB, (OnInstantiate, Inherit));
    ECS_ENTITY(world, ObjC, (OnInstantiate, Inherit));

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, ObjA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, ObjB);
    ecs_add_pair(world, e1, Rel, ObjC);
    ecs_add_pair(world, e2, Rel, ObjC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, *)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_pair(Rel, ObjA), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_pair(Rel, ObjC), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_pair(Rel, ObjB), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_pair(Rel, ObjC), ecs_field_id(&it, 0));
    test_bool(false, ecs_query_next(&it));


    ecs_fini(world);
}

void Cached_wildcard_query_new_table_2_results_p_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));
    ECS_ENTITY(world, ObjA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, ObjB, (OnInstantiate, Inherit));
    ECS_ENTITY(world, ObjC, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "(Rel, *)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, ObjA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, ObjB);
    ecs_add_pair(world, e1, Rel, ObjC);
    ecs_add_pair(world, e2, Rel, ObjC);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_pair(Rel, ObjA), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_pair(Rel, ObjC), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_pair(Rel, ObjB), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_pair(Rel, ObjC), ecs_field_id(&it, 0));
    test_bool(false, ecs_query_next(&it));


    ecs_fini(world);
}

void Cached_wildcard_query_2nd_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TgtA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TgtB, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TgtC, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_add_pair(world, e1, Rel, TgtA);
    ecs_add_pair(world, e1, Rel, TgtB);

    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_add_pair(world, e2, Rel, TgtA);

    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_add_pair(world, e3, Rel, TgtA);
    ecs_add_pair(world, e3, Rel, TgtC);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Tag }, { ecs_pair(Rel, EcsWildcard) }},
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(Tag, it.ids[0]);
    test_uint(ecs_pair(Rel, TgtA), it.ids[1]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(Tag, it.ids[0]);
    test_uint(ecs_pair(Rel, TgtA), it.ids[1]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(Tag, it.ids[0]);
    test_uint(ecs_pair(Rel, TgtB), it.ids[1]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(Tag, it.ids[0]);
    test_uint(ecs_pair(Rel, TgtA), it.ids[1]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(Tag, it.ids[0]);
    test_uint(ecs_pair(Rel, TgtC), it.ids[1]);
    test_bool(false, ecs_query_next(&it));


    ecs_fini(world);
}

void Cached_wildcard_query_2nd_term_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TgtA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TgtB, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TgtC, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_add_pair(world, e1, Rel, TgtA);
    ecs_add_pair(world, e1, Rel, TgtB);

    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_add_pair(world, e2, Rel, TgtA);

    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_add_pair(world, e3, Rel, TgtA);
    ecs_add_pair(world, e3, Rel, TgtC);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Tag }, { ecs_pair(Rel, EcsWildcard), .src.id = EcsSelf }},
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(Tag, it.ids[0]);
    test_uint(ecs_pair(Rel, TgtA), it.ids[1]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(Tag, it.ids[0]);
    test_uint(ecs_pair(Rel, TgtA), it.ids[1]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(Tag, it.ids[0]);
    test_uint(ecs_pair(Rel, TgtB), it.ids[1]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(Tag, it.ids[0]);
    test_uint(ecs_pair(Rel, TgtA), it.ids[1]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(Tag, it.ids[0]);
    test_uint(ecs_pair(Rel, TgtC), it.ids[1]);
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_simple_query_existing_empty_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_add(world, e1, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_remove(world, e1, TagB);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_bool(false, ecs_query_next(&it));


    ecs_fini(world);
}

void Cached_simple_query_existing_empty_type(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));
    ECS_PREFAB(world, TypeX, TagA, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_add(world, e1, TagB);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_bool(false, ecs_query_next(&it));


    ecs_fini(world);
}

void Cached_simple_query_new_empty_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_add(world, e1, TagB);
    ecs_remove(world, e1, TagB);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_bool(false, ecs_query_next(&it));


    ecs_fini(world);
}

void Cached_component_query_existing_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_assert(ecs_field_size(&it, 0) == sizeof(Position));
    test_assert(ecs_field(&it, Position, 0) != NULL);
    test_bool(false, ecs_query_next(&it));


    ecs_fini(world);
}

void Cached_component_query_new_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_field_size(&it, 0), sizeof(Position));
    test_assert(ecs_field(&it, Position, 0) != NULL);
    test_bool(false, ecs_query_next(&it));


    ecs_fini(world);
}

void Cached_component_query_existing_empty_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));

    ecs_entity_t e = ecs_new_w(world, Position);
    ecs_add(world, e, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_remove(world, e, TagA);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_field_size(&it, 0), sizeof(Position));
    test_assert(ecs_field(&it, Position, 0) != NULL);
    test_bool(false, ecs_query_next(&it));


    ecs_fini(world);
}

void Cached_2_component_query_existing_empty_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));
    ECS_PREFAB(world, MyType, Position, Velocity);

    ecs_entity_t e = ecs_new_w(world, Position);
    ecs_add(world, e, Velocity);
    ecs_add(world, e, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, Velocity",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_remove(world, e, Tag);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_field_size(&it, 0), sizeof(Position));
    test_assert(ecs_field(&it, Position, 0) != NULL);
    test_bool(false, ecs_query_next(&it));


    ecs_fini(world);
}

void Cached_2_component_query_existing_empty_type(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_PREFAB(world, MyType, Position, Velocity);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, Velocity",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t e = ecs_new_w(world, Position);
    ecs_add(world, e, Velocity);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_field_size(&it, 0), sizeof(Position));
    test_assert(ecs_field(&it, Position, 0) != NULL);
    test_bool(false, ecs_query_next(&it));


    ecs_fini(world);
}

void Cached_only_optional(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "?TagA",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t e = ecs_new_w(world, TagA);
    int32_t count = 0;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        if (ecs_field_is_set(&it, 0)) {
            test_assert(count == 0);
            test_int(it.count, 1);
            test_uint(it.entities[0], e);
            count ++;
        }
    }

    test_int(count, 1);

    ecs_fini(world);
}

void Cached_only_optional_new_empty_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));

    ecs_entity_t e = ecs_new_w(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "?TagA",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    int32_t count = 0, total_count = 0;
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        if (ecs_field_is_set(&it, 0)) {
            test_assert(count == 0);
            test_int(it.count, 1);
            test_uint(it.entities[0], e);
            count ++;
        }
        total_count ++;
    }

    test_int(count, 1);
    test_assert(total_count >= count);

    int32_t prev_total_count = total_count;
    total_count = 0;

    ecs_remove(world, e, TagA);

    it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_assert(!ecs_field_is_set(&it, 0));
        test_assert(it.count > 0);
        total_count ++;
    }

    test_assert(total_count == (prev_total_count - 1));


    ecs_fini(world);
}

void Cached_only_optional_new_empty_non_empty_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));

    ecs_entity_t e = ecs_new_w(world, TagA);
    ecs_add(world, e, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "?TagA",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    int32_t count = 0, total_count = 0;
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        if (ecs_field_is_set(&it, 0)) {
            test_assert(count == 0);
            test_int(it.count, 1);
            test_uint(it.entities[0], e);
            count ++;
        }
        total_count ++;
    }

    test_int(count, 1);
    test_assert(total_count >= count);

    int32_t prev_total_count = total_count;
    count = 0; total_count = 0;

    ecs_remove(world, e, TagA);
    ecs_table_t *table = ecs_get_table(world, e);

    it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_assert(!ecs_field_is_set(&it, 0));
        test_assert(it.count > 0);
        total_count ++;
        if (it.entities[0] == e) {
            test_assert(table == it.table);
            count ++;
        }
    }

    test_int(count, 1);
    test_assert(total_count == prev_total_count);


    ecs_fini(world);
}

void Cached_only_optional_new_unset_tables(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagC, (OnInstantiate, Inherit));

    ecs_entity_t e = ecs_new_w(world, TagA);
    ecs_add(world, e, TagB);
    ecs_table_t *table = ecs_get_table(world, e);

    ecs_query_t *q = ecs_query(world, {
        .expr = "?TagC",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    int32_t count = 0, total_count = 0;
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_assert(!ecs_field_is_set(&it, 0));
        test_assert(it.count > 0);
        if (it.entities[0] == e) {
            test_assert(table == it.table);
            count ++;
        }
        total_count ++;
    }

    test_int(count, 1);
    test_assert(total_count >= count);

    int32_t prev_total_count = total_count;
    count = 0; total_count = 0;

    ecs_remove(world, e, TagA);
    table = ecs_get_table(world, e);

    it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_assert(!ecs_field_is_set(&it, 0));
        test_assert(it.count > 0);
        total_count ++;
        if (it.entities[0] == e) {
            test_assert(table == it.table);
            count ++;
        }
    }

    test_int(count, 1);
    test_assert(total_count == prev_total_count);


    ecs_fini(world);
}

void Cached_singleton_w_optional_new_empty_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Singleton, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));

    ecs_singleton_add(world, Singleton);

    ecs_entity_t e = ecs_new_w(world, TagA);
    ecs_set_name(world, e, "e");

    ecs_query_t *q = ecs_query(world, {
        .expr = "Singleton($), ?TagA",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    int32_t count = 0, total_count = 0;
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        if (ecs_field_is_set(&it, 1)) {
            test_assert(count == 0);
            test_int(it.count, 1);
            test_uint(it.entities[0], e);
            count ++;
        }
        total_count ++;
    }

    test_int(count, 1);
    test_assert(total_count >= count);

    int32_t prev_total_count = total_count;
    total_count = 0;

    ecs_remove(world, e, TagA);

    it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_assert(!ecs_field_is_set(&it, 1));
        test_assert(it.count > 0);
        total_count ++;
    }

    test_assert(total_count == prev_total_count);

    ecs_set_name(world, e, NULL);
    total_count = 0;

    it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_assert(!ecs_field_is_set(&it, 1));
        test_assert(it.count > 0);
        total_count ++;
    }

    test_assert(total_count == (prev_total_count - 1));

    ecs_fini(world);
}

void Cached_singleton_w_optional_new_empty_non_empty_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Singleton, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));

    ecs_singleton_add(world, Singleton);

    ecs_entity_t e = ecs_new_w(world, TagA);
    ecs_add(world, e, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Singleton($), ?TagA",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    int32_t count = 0, total_count = 0;
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        if (ecs_field_is_set(&it, 1)) {
            test_assert(count == 0);
            test_int(it.count, 1);
            test_uint(it.entities[0], e);
            count ++;
        }
        total_count ++;
    }

    test_int(count, 1);
    test_assert(total_count >= count);

    int32_t prev_total_count = total_count;
    count = 0; total_count = 0;

    ecs_remove(world, e, TagA);
    ecs_table_t *table = ecs_get_table(world, e);

    it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_assert(!ecs_field_is_set(&it, 1));
        test_assert(it.count > 0);
        total_count ++;
        if (it.entities[0] == e) {
            test_assert(table == it.table);
            count ++;
        }
    }

    test_int(count, 1);
    test_assert(total_count == prev_total_count);


    ecs_fini(world);
}

void Cached_singleton_w_optional_new_unset_tables(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Singleton, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagC, (OnInstantiate, Inherit));

    ecs_singleton_add(world, Singleton);

    ecs_entity_t e = ecs_new_w(world, TagA);
    ecs_add(world, e, TagB);
    ecs_table_t *table = ecs_get_table(world, e);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Singleton($), ?TagC",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    int32_t count = 0, total_count = 0;
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_assert(!ecs_field_is_set(&it, 1));
        test_assert(it.count > 0);
        if (it.entities[0] == e) {
            test_assert(table == it.table);
            count ++;
        }
        total_count ++;
    }

    test_int(count, 1);
    test_assert(total_count >= count);

    int32_t prev_total_count = total_count;
    count = 0; total_count = 0;

    ecs_remove(world, e, TagA);
    table = ecs_get_table(world, e);

    it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        test_assert(!ecs_field_is_set(&it, 1));
        test_assert(it.count > 0);
        total_count ++;
        if (it.entities[0] == e) {
            test_assert(table == it.table);
            count ++;
        }
    }

    test_int(count, 1);
    test_assert(total_count == prev_total_count);


    ecs_fini(world);
}

void Cached_query_w_from_entity_match_after(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));

    ecs_entity_t e1 = ecs_entity(world, { .name = "e" });
    ecs_entity_t e2 = ecs_new_w(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, TagB(e)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_add(world, e1, TagB);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(ecs_field_src(&it, 0), 0);
    test_uint(ecs_field_id(&it, 0), TagA);
    test_uint(ecs_field_src(&it, 1), e1);
    test_uint(ecs_field_id(&it, 1), TagB);
    test_bool(false, ecs_query_next(&it));


    ecs_fini(world);
}

void Cached_query_w_from_singleton_match_after(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));

    ecs_entity_t e2 = ecs_new_w(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, TagB($)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_singleton_add(world, TagB);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(ecs_field_src(&it, 0), 0);
    test_uint(ecs_field_id(&it, 0), TagA);
    test_uint(ecs_field_src(&it, 1), TagB);
    test_uint(ecs_field_id(&it, 1), TagB);
    test_bool(false, ecs_query_next(&it));


    ecs_fini(world);
}

void Cached_rematch_after_add_to_recycled_parent(void) {
    ecs_world_t *world = ecs_mini();
    
    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, Position(up)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(q != NULL);

    ecs_entity_t parent = ecs_new(world);
    test_assert(parent != 0);

    ecs_delete(world, parent);
    test_assert( !ecs_is_alive(world, parent));

    parent = ecs_new(world);
    test_assert(parent != 0);

    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, parent);
    test_assert(e != 0);
    test_assert( ecs_has_pair(world, e, EcsChildOf, parent));
    ecs_add(world, e, Tag);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), false);

    ecs_set(world, parent, Position, {10, 20});

    ecs_run_aperiodic(world, 0);

    it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);

    const Position *p = ecs_field(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    test_assert(ecs_field_src(&it, 1) == parent);
    test_bool(ecs_query_next(&it), false);

    ecs_fini(world);
}

void Cached_query_rematch_optional_after_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new(world);
    
    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_add_pair(world, e1, EcsIsA, base);
    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_add(world, e2, Velocity);
    ecs_entity_t e3 = ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, ?Velocity(up IsA)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    /* First iteration, base doesn't have Velocity but query should match with
     * entity anyway since the component is optional */
    ecs_iter_t it = ecs_query_iter(world, q);
    int32_t count = 0;
    
    while (ecs_query_next(&it)) {
        Position *p = ecs_field(&it, Position, 0);
        Velocity *v = ecs_field(&it, Velocity, 1);
        
        test_int(it.count, 1);
        test_assert(p != NULL);
        
        if (it.entities[0] == e1) {
            test_assert(v == NULL);
        } else if (it.entities[0] == e2) {
            test_assert(v == NULL);
        } else if (it.entities[0] == e3) {
            test_assert(v == NULL);
        }

        count ++;
    }

    test_int(count, 3);

    /* Add Velocity to base, should trigger rematch */
    ecs_add(world, base, Velocity);

    /* Trigger a merge, which triggers the rematch */
    ecs_readonly_begin(world, false);
    ecs_readonly_end(world);

    /* Second iteration, base has Velocity and entity should be able to access
     * the shared component. */
    it = ecs_query_iter(world, q);
    count = 0;
    
    while (ecs_query_next(&it)) {
        Position *p = ecs_field(&it, Position, 0);
        Velocity *v = ecs_field(&it, Velocity, 1);
        
        test_int(it.count, 1);
        test_assert(p != NULL);
        
        if (it.entities[0] == e1) {
            test_assert(ecs_field_is_set(&it, 1));
            test_assert(v != NULL);
        } else if (it.entities[0] == e2) {
            test_assert(!ecs_field_is_set(&it, 1));
        } else if (it.entities[0] == e3) {
            test_assert(!ecs_field_is_set(&it, 1));
        }

        count ++;
    }
    test_int(count, 3);


    ecs_fini(world);    
}

void Cached_get_owned_tag(void) {
    install_test_abort();
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_new_w(world, Tag);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag",
        .cache_kind = EcsQueryCacheAuto
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));

    test_expect_abort();
    test_assert(ecs_field_w_size(&it, 0, 0) == NULL);
}

void Cached_get_shared_tag(void) {
    install_test_abort();
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_entity_t base = ecs_new_w(world, Tag);
    ecs_new_w_pair(world, EcsIsA, base);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag(up IsA)",
        .cache_kind = EcsQueryCacheAuto
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));

    test_expect_abort();
    test_assert(ecs_field_w_size(&it, 0, 0) == NULL);
}

void Cached_explicit_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    /* Ensure query isn't deleted twice when deleting world */
    ecs_query_fini(q);

    ecs_fini(world);
}

void Cached_get_column_size(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    
    ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it));
    test_int(ecs_field_size(&it, 0), sizeof(Position));
    test_assert(!ecs_query_next(&it));


    ecs_fini(world);    
}

void Cached_stresstest_query_free(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Hello, (OnInstantiate, Inherit));

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Foo);
    ecs_add(world, e, Bar);
    ecs_add(world, e, Hello);

    /* Create & delete query to test if query is properly unregistered with
     * the table */

    for (int i = 0; i < 10000; i ++) {
        ecs_query_t *q = ecs_query(world, {
            .expr = "Foo",
            .cache_kind = EcsQueryCacheAuto
        });
        ecs_query_fini(q);
    }

    /* If code did not crash, test passes */
    test_assert(true);

    ecs_fini(world);
}

void Cached_query_optional_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));
    
    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_add_id(world, e2, TagB);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, ?TagB",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    int32_t count = 0;
    
    while (ecs_query_next(&it)) {    
        test_uint(ecs_field_id(&it, 0), TagA);
        test_assert(ecs_field_id(&it, 1) == TagB);
        test_int(it.count, 1);

        if (it.entities[0] == e1) {
            test_bool(ecs_field_is_set(&it, 0), true);
            test_bool(ecs_field_is_set(&it, 1), false); 
        } else if (it.entities[0] == e2) {
            test_bool(ecs_field_is_set(&it, 0), true);
            test_bool(ecs_field_is_set(&it, 1), true); 
        }

        count ++;
    }

    test_int(count, 2);


    ecs_fini(world);
}

void Cached_query_optional_shared_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));
    
    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_add_id(world, e2, TagB);
    
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_add_pair(world, e3, EcsIsA, e2);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, ?TagB(self|up IsA)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    int32_t count = 0;
    
    while (ecs_query_next(&it)) {    
        test_uint(ecs_field_id(&it, 0), TagA);
        test_assert(ecs_field_id(&it, 1) == TagB);
        test_int(it.count, 1);

        if (it.entities[0] == e1) {
            test_bool(ecs_field_is_set(&it, 0), true);
            test_bool(ecs_field_is_set(&it, 1), false); 
        } else if (it.entities[0] == e2) {
            test_bool(ecs_field_is_set(&it, 0), true);
            test_bool(ecs_field_is_set(&it, 1), true); 
        } else if (it.entities[0] == e3) {
            test_bool(ecs_field_is_set(&it, 0), true);
            test_bool(ecs_field_is_set(&it, 1), true);
        }

        count ++;
    }

    test_int(count, 3);


    ecs_fini(world);
}

void Cached_query_iter_10_tags(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagC, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagD, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagE, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagF, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagG, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagH, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagI, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagJ, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagK, (OnInstantiate, Inherit));

    ecs_entity_t e_1 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_1, TagB);
    ecs_add_id(world, e_1, TagC);
    ecs_add_id(world, e_1, TagD);
    ecs_add_id(world, e_1, TagE);
    ecs_add_id(world, e_1, TagF);
    ecs_add_id(world, e_1, TagG);
    ecs_add_id(world, e_1, TagH);
    ecs_add_id(world, e_1, TagI);
    ecs_add_id(world, e_1, TagJ);
    
    ecs_entity_t e_2 = ecs_new_w(world, TagA);
    ecs_add_id(world, e_2, TagB);
    ecs_add_id(world, e_2, TagC);
    ecs_add_id(world, e_2, TagD);
    ecs_add_id(world, e_2, TagE);
    ecs_add_id(world, e_2, TagF);
    ecs_add_id(world, e_2, TagG);
    ecs_add_id(world, e_2, TagH);
    ecs_add_id(world, e_2, TagI);
    ecs_add_id(world, e_2, TagJ);
    ecs_add_id(world, e_2, TagK); /* 2nd match in different table */

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            {TagA}, {TagB}, {TagC}, {TagD}, {TagE}, {TagF}, {TagG}, {TagH}, 
            {TagI}, {TagJ}
        },
        .cache_kind = EcsQueryCacheAuto
    });

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_field_id(&it, 0), TagA);
    test_int(ecs_field_id(&it, 1), TagB);
    test_int(ecs_field_id(&it, 2), TagC);
    test_int(ecs_field_id(&it, 3), TagD);
    test_int(ecs_field_id(&it, 4), TagE);
    test_int(ecs_field_id(&it, 5), TagF);
    test_int(ecs_field_id(&it, 6), TagG);
    test_int(ecs_field_id(&it, 7), TagH);
    test_int(ecs_field_id(&it, 8), TagI);
    test_int(ecs_field_id(&it, 9), TagJ);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_2);
    test_int(ecs_field_id(&it, 0), TagA);
    test_int(ecs_field_id(&it, 1), TagB);
    test_int(ecs_field_id(&it, 2), TagC);
    test_int(ecs_field_id(&it, 3), TagD);
    test_int(ecs_field_id(&it, 4), TagE);
    test_int(ecs_field_id(&it, 5), TagF);
    test_int(ecs_field_id(&it, 6), TagG);
    test_int(ecs_field_id(&it, 7), TagH);
    test_int(ecs_field_id(&it, 8), TagI);
    test_int(ecs_field_id(&it, 9), TagJ);

    test_assert(!ecs_query_next(&it));


    ecs_fini(world);
}

typedef struct {
    float v;
} CompA, CompB, CompC, CompD, CompE, CompF, CompG, CompH, CompI, CompJ, CompK,
  CompL, CompM, CompN, CompO, CompP, CompQ, CompR, CompS, CompT, CompU;

void Cached_query_iter_10_components(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, CompA);
    ecs_add_pair(world, ecs_id(CompA), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, CompB);
    ecs_add_pair(world, ecs_id(CompB), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, CompC);
    ecs_add_pair(world, ecs_id(CompC), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, CompD);
    ecs_add_pair(world, ecs_id(CompD), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, CompE);
    ecs_add_pair(world, ecs_id(CompE), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, CompF);
    ecs_add_pair(world, ecs_id(CompF), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, CompG);
    ecs_add_pair(world, ecs_id(CompG), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, CompH);
    ecs_add_pair(world, ecs_id(CompH), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, CompI);
    ecs_add_pair(world, ecs_id(CompI), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, CompJ);
    ecs_add_pair(world, ecs_id(CompJ), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, CompK);
    ecs_add_pair(world, ecs_id(CompK), EcsOnInstantiate, EcsInherit);

    ecs_entity_t e_1 = ecs_insert(world, ecs_value(CompA, {10}));
    ecs_set(world, e_1, CompB, {10});
    ecs_set(world, e_1, CompC, {10});
    ecs_set(world, e_1, CompD, {10});
    ecs_set(world, e_1, CompE, {10});
    ecs_set(world, e_1, CompF, {10});
    ecs_set(world, e_1, CompG, {10});
    ecs_set(world, e_1, CompH, {10});
    ecs_set(world, e_1, CompI, {10});
    ecs_set(world, e_1, CompJ, {10});
    
    ecs_entity_t e_2 = ecs_insert(world, ecs_value(CompA, {10}));
    ecs_set(world, e_2, CompB, {10});
    ecs_set(world, e_2, CompC, {10});
    ecs_set(world, e_2, CompD, {10});
    ecs_set(world, e_2, CompE, {10});
    ecs_set(world, e_2, CompF, {10});
    ecs_set(world, e_2, CompG, {10});
    ecs_set(world, e_2, CompH, {10});
    ecs_set(world, e_2, CompI, {10});
    ecs_set(world, e_2, CompJ, {10});
    ecs_set(world, e_2, CompK, {10});

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            {ecs_id(CompA)}, {ecs_id(CompB)}, {ecs_id(CompC)}, {ecs_id(CompD)}, 
            {ecs_id(CompE)}, {ecs_id(CompF)}, {ecs_id(CompG)}, {ecs_id(CompH)}, 
            {ecs_id(CompI)}, {ecs_id(CompJ)}
        },
        .cache_kind = EcsQueryCacheAuto
    });

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);
    test_int(ecs_field_id(&it, 0), ecs_id(CompA));
    test_int(ecs_field_id(&it, 1), ecs_id(CompB));
    test_int(ecs_field_id(&it, 2), ecs_id(CompC));
    test_int(ecs_field_id(&it, 3), ecs_id(CompD));
    test_int(ecs_field_id(&it, 4), ecs_id(CompE));
    test_int(ecs_field_id(&it, 5), ecs_id(CompF));
    test_int(ecs_field_id(&it, 6), ecs_id(CompG));
    test_int(ecs_field_id(&it, 7), ecs_id(CompH));
    test_int(ecs_field_id(&it, 8), ecs_id(CompI));
    test_int(ecs_field_id(&it, 9), ecs_id(CompJ));

    int i;
    for (i = 0; i < 10; i ++) {
        CompA *ptr = ecs_field_w_size(&it, 0, i);
        test_assert(ptr != NULL);
        test_int(ptr[0].v, 10);
    }

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e_2);
    test_int(ecs_field_id(&it, 0), ecs_id(CompA));
    test_int(ecs_field_id(&it, 1), ecs_id(CompB));
    test_int(ecs_field_id(&it, 2), ecs_id(CompC));
    test_int(ecs_field_id(&it, 3), ecs_id(CompD));
    test_int(ecs_field_id(&it, 4), ecs_id(CompE));
    test_int(ecs_field_id(&it, 5), ecs_id(CompF));
    test_int(ecs_field_id(&it, 6), ecs_id(CompG));
    test_int(ecs_field_id(&it, 7), ecs_id(CompH));
    test_int(ecs_field_id(&it, 8), ecs_id(CompI));
    test_int(ecs_field_id(&it, 9), ecs_id(CompJ));

    for (i = 0; i < 10; i ++) {
        CompA *ptr = ecs_field_w_size(&it, 0, i);
        test_assert(ptr != NULL);
        test_int(ptr[0].v, 10);
    }

    test_assert(!ecs_query_next(&it));


    ecs_fini(world);
}

void Cached_iter_type_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    
    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_assert(it.table != NULL);
    const ecs_type_t *type = ecs_table_get_type(it.table);
    test_int(type->count, 1);
    test_int(type->array[0], ecs_id(Position));

    test_assert(!ecs_query_next(&it));


    ecs_fini(world);
}

void Cached_filter_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = ecs_id(Position), .inout = EcsInOutNone }},
        .cache_kind = EcsQueryCacheAuto
    });

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(ecs_query_next(&it), true);
    test_assert(it.ids != NULL);
    test_assert(it.ids[0] == ecs_id(Position));

    test_int(it.count, 1);
    test_assert(it.entities != NULL);
    test_assert(it.entities[0] == e);

    test_assert(it.trs != NULL);

    test_bool(ecs_query_next(&it), false);

    ecs_fini(world);
}

void Cached_2_terms_1_filter(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), .inout = EcsInOutNone },
            { .id = ecs_id(Velocity) }
        },
        .cache_kind = EcsQueryCacheAuto
    });

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_set(world, e, Velocity, {1, 1});

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(ecs_query_next(&it), true);
    test_assert(it.ids != NULL);
    test_assert(it.ids[0] == ecs_id(Position));
    test_assert(it.ids[1] == ecs_id(Velocity));

    test_int(it.count, 1);
    test_assert(it.entities != NULL);
    test_assert(it.entities[0] == e);

    test_assert(it.sizes != NULL);
    test_assert(it.trs != NULL);

    test_assert(ecs_field_w_size(&it, 0, 1) != NULL);

    test_bool(ecs_query_next(&it), false);

    ecs_fini(world);
}

void Cached_3_terms_2_filter(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Mass);
    ecs_add_pair(world, ecs_id(Mass), EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), .inout = EcsInOutNone },
            { .id = ecs_id(Velocity), .inout = EcsInOutNone },
            { .id = ecs_id(Mass) }
        },
        .cache_kind = EcsQueryCacheAuto
    });

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_set(world, e, Velocity, {1, 1});
    ecs_set(world, e, Mass, {1});

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(ecs_query_next(&it), true);
    test_assert(it.ids != NULL);
    test_assert(it.ids[0] == ecs_id(Position));
    test_assert(it.ids[1] == ecs_id(Velocity));
    test_assert(it.ids[2] == ecs_id(Mass));

    test_int(it.count, 1);
    test_assert(it.entities != NULL);
    test_assert(it.entities[0] == e);

    test_assert(it.sizes != NULL);
    test_assert(it.trs != NULL);

    test_assert(ecs_field_w_size(&it, 0, 2) != NULL);

    test_bool(ecs_query_next(&it), false);

    ecs_fini(world);
}

void Cached_add_singleton_after_query(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { TagA },
            { TagB, .src.id = TagB }
        },
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, TagA);
    
    ecs_singleton_add(world, TagB);
    test_assert(ecs_has(world, TagB, TagB));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool (ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_bool (ecs_query_next(&it), false);

    ecs_singleton_remove(world, TagB);
    test_assert(!ecs_has(world, TagB, TagB));

    it = ecs_query_iter(world, q);
    test_bool (ecs_query_next(&it), false);

    ecs_fini(world);
}

void Cached_query_w_component_from_parent_from_non_this(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));

    ecs_entity_t parent = ecs_new_w(world, TagB);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { TagA },
            { TagB, .src.id = child|EcsUp, .trav = EcsChildOf }
        },
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, TagA);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool (ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(it.sources[0], 0);
    test_int(it.sources[1], parent);
    test_bool (ecs_query_next(&it), false);

    ecs_remove_pair(world, child, EcsChildOf, parent);
    it = ecs_query_iter(world, q);
    test_bool (ecs_query_next(&it), false);

    ecs_fini(world);
}

void Cached_create_query_while_pending(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));

    ecs_entity_t e = ecs_new_w(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ TagA }},
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_add(world, e, TagB);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert( ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);

    test_assert( !ecs_query_next(&it));
    
    ecs_fini(world);
}

void Cached_empty_query(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ 0 }},
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);


    ecs_fini(world);
}

void Cached_implicit_existing_isa_superset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_entity_t base = ecs_new_w(world, Tag);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], base);
    test_uint(it.sources[0], 0);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], inst);
    test_uint(it.sources[0], base);
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_implicit_new_isa_superset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t base = ecs_new_w(world, Tag);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], base);
    test_uint(it.sources[0], 0);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], inst);
    test_uint(it.sources[0], base);
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_isa_superset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag(up IsA)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t base = ecs_new_w(world, Tag);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], inst);
    test_uint(it.sources[0], base);
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_isa_superset_2_lvls(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag(up IsA)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t base = ecs_new_w(world, Tag);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, e1);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(it.sources[0], base);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(it.sources[0], base);

    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_isa_superset_3_lvls(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag(up IsA)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t base = ecs_new_w(world, Tag);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsIsA, e2);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(it.sources[0], base);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(it.sources[0], base);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e3);
    test_uint(it.sources[0], base);

    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_isa_superset_2_lvls_owned(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag(up IsA)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t base = ecs_new_w(world, Tag);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, e1);

    ecs_add(world, e1, Tag);
    ecs_add(world, e2, Tag);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(it.sources[0], base);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(it.sources[0], e1);

    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_isa_superset_3_lvls_owned(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag(up IsA)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t base = ecs_new_w(world, Tag);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsIsA, e2);

    ecs_add(world, e1, Tag);
    ecs_add(world, e2, Tag);
    ecs_add(world, e3, Tag);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(it.sources[0], base);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(it.sources[0], e1);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e3);
    test_uint(it.sources[0], e2);

    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_isa_superset_owned_empty_table_after_match(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag(up IsA)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t base = ecs_new_w(world, Tag);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsIsA, e2);

    ecs_add(world, e1, Tag);
    ecs_add(world, e2, Tag);
    ecs_add(world, e3, Tag);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(it.sources[0], base);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(it.sources[0], e1);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e3);
    test_uint(it.sources[0], e2);

    test_bool(false, ecs_query_next(&it));

    ecs_remove_pair(world, e3, EcsIsA, e2);

    it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(it.sources[0], base);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(it.sources[0], e1);

    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_isa_self_superset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag(self|up IsA)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t base = ecs_new_w(world, Tag);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], base);
    test_uint(it.sources[0], 0);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], inst);
    test_uint(it.sources[0], base);
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_childof_superset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag(up)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t base = ecs_new_w(world, Tag);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsChildOf, base);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], inst);
    test_uint(it.sources[0], base);
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_superset_2_targets(void) {
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

    ecs_entity_t t = ecs_new(world);
    ecs_add(world, t, Position);
    ecs_add_pair(world, t, R, e_2);
    ecs_add_pair(world, t, R, e_1);
    ecs_delete(world, t);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(up R)",
        .cache_kind = EcsQueryCacheAuto
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    {
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e_1);
        test_uint(it.sources[0], e_0);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p[0].x, 10);
        test_int(p[0].y, 20);
    }

    {
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e_3);
        test_uint(it.sources[0], e_2);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p[0].x, 50);
        test_int(p[0].y, 60);
    }

    {
        test_bool(true, ecs_query_next(&it));
        test_int(it.count, 1);
        test_uint(it.entities[0], e_2);
        test_uint(it.sources[0], e_1);
        Position *p = ecs_field(&it, Position, 0);
        test_int(p[0].x, 30);
        test_int(p[0].y, 40);
    }

    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_superset_2_relations(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = TagA, .trav = EcsChildOf, .src.id = EcsUp },
            { .id = TagA, .trav = EcsIsA, .src.id = EcsUp },
        },
        .cache_kind = EcsQueryCacheAuto
    });

    ecs_entity_t base = ecs_new_w(world, TagA);
    ecs_entity_t parent = ecs_new_w(world, TagA);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add_pair(world, e1, EcsIsA, base);
    ecs_add_pair(world, e1, EcsChildOf, parent);

    ecs_entity_t e2 = ecs_new(world);
    ecs_add_pair(world, e2, EcsIsA, base);
    ecs_add_pair(world, e2, EcsChildOf, parent);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(e2, it.entities[1]);
    test_uint(parent, it.sources[0]);
    test_uint(base, it.sources[1]);
    test_bool(false, ecs_query_next(&it));


    ecs_fini(world);
}

void Cached_superset_2_relations_instanced(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = TagA, .trav = EcsChildOf, .src.id = EcsUp },
            { .id = TagA, .trav = EcsIsA, .src.id = EcsUp },
        },
        .cache_kind = EcsQueryCacheAuto
    });

    ecs_entity_t base = ecs_new_w(world, TagA);
    ecs_entity_t parent = ecs_new_w(world, TagA);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add_pair(world, e1, EcsIsA, base);
    ecs_add_pair(world, e1, EcsChildOf, parent);

    ecs_entity_t e2 = ecs_new(world);
    ecs_add_pair(world, e2, EcsIsA, base);
    ecs_add_pair(world, e2, EcsChildOf, parent);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(e2, it.entities[1]);
    test_uint(parent, it.sources[0]);
    test_uint(base, it.sources[1]);
    test_bool(false, ecs_query_next(&it));


    ecs_fini(world);
}

void Cached_superset_2_relations_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), .trav = EcsChildOf, .src.id = EcsUp },
            { .id = ecs_id(Position), .trav = EcsIsA, .src.id = EcsUp },
        },
        .cache_kind = EcsQueryCacheAuto
    });

    ecs_entity_t base = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t parent = ecs_insert(world, ecs_value(Position, {30, 40}));

    ecs_entity_t e1 = ecs_new(world);
    ecs_add_pair(world, e1, EcsIsA, base);
    ecs_add_pair(world, e1, EcsChildOf, parent);

    ecs_entity_t e2 = ecs_new(world);
    ecs_add_pair(world, e2, EcsIsA, base);
    ecs_add_pair(world, e2, EcsChildOf, parent);

    ecs_iter_t it = ecs_query_iter(world, q);
    {
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(parent, it.sources[0]);
        test_uint(base, it.sources[1]);
        Position *p1 = ecs_field(&it, Position, 0);
        Position *p2 = ecs_field(&it, Position, 1);
        test_assert(p1 != NULL);
        test_assert(p2 != NULL);
        test_int(p1->x, 30); test_int(p1->y, 40);
        test_int(p2->x, 10); test_int(p2->y, 20);
        test_int(p1->x, 30); test_int(p1->y, 40);
        test_int(p2->x, 10); test_int(p2->y, 20);
    }
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Cached_superset_2_relations_instanced_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_id(Position), .trav = EcsChildOf, .src.id = EcsUp },
            { .id = ecs_id(Position), .trav = EcsIsA, .src.id = EcsUp },
        },
        .cache_kind = EcsQueryCacheAuto
    });

    ecs_entity_t base = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t parent = ecs_insert(world, ecs_value(Position, {30, 40}));

    ecs_entity_t e1 = ecs_new(world);
    ecs_add_pair(world, e1, EcsIsA, base);
    ecs_add_pair(world, e1, EcsChildOf, parent);

    ecs_entity_t e2 = ecs_new(world);
    ecs_add_pair(world, e2, EcsIsA, base);
    ecs_add_pair(world, e2, EcsChildOf, parent);

    ecs_iter_t it = ecs_query_iter(world, q);
    {
        test_bool(true, ecs_query_next(&it));
        test_int(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(parent, it.sources[0]);
        test_uint(base, it.sources[1]);
        Position *p1 = ecs_field(&it, Position, 0);
        Position *p2 = ecs_field(&it, Position, 1);
        test_assert(p1 != NULL);
        test_assert(p2 != NULL);
        test_int(p1->x, 30);
        test_int(p1->y, 40);
        test_int(p2->x, 10);
        test_int(p2->y, 20);
    }
    test_bool(false, ecs_query_next(&it));


    ecs_fini(world);
}

void Cached_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag(up)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t base = ecs_new_w(world, Tag);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsChildOf, base);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], inst);
    test_uint(it.sources[0], base);
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_not_pair_relation_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, RelA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, RelB, (OnInstantiate, Inherit));
    ECS_ENTITY(world, ObjA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, ObjB, (OnInstantiate, Inherit));
    
    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, !(*, ObjA)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_entity_t e3 = ecs_new_w(world, Foo);
    ecs_entity_t e4 = ecs_new_w(world, Foo);

    ecs_add_pair(world, e1, RelA, ObjA);
    ecs_add_pair(world, e2, RelA, ObjB);
    ecs_add_pair(world, e3, RelB, ObjA);
    ecs_add_pair(world, e4, RelB, ObjB);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(ecs_field_id(&it, 1), ecs_pair(EcsWildcard, ObjA));

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e4);
    test_uint(ecs_field_id(&it, 1), ecs_pair(EcsWildcard, ObjA));

    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_not_pair_object_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, RelA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, RelB, (OnInstantiate, Inherit));
    ECS_ENTITY(world, ObjA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, ObjB, (OnInstantiate, Inherit));
    
    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, !(RelA, *)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_entity_t e3 = ecs_new_w(world, Foo);
    ecs_entity_t e4 = ecs_new_w(world, Foo);

    ecs_add_pair(world, e1, RelA, ObjA);
    ecs_add_pair(world, e2, RelA, ObjB);
    ecs_add_pair(world, e3, RelB, ObjA);
    ecs_add_pair(world, e4, RelB, ObjB);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e3);
    test_uint(ecs_field_id(&it, 1), ecs_pair(RelA, EcsWildcard));

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e4);
    test_uint(ecs_field_id(&it, 1), ecs_pair(RelA, EcsWildcard));

    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_two_pair_wildcards_one_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, RelA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, RelB, (OnInstantiate, Inherit));
    ECS_ENTITY(world, ObjA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, ObjB, (OnInstantiate, Inherit));
    
    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, (RelA, *), !(RelB, *)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_entity_t e3 = ecs_new_w(world, Foo);
    ecs_entity_t e4 = ecs_new_w(world, Foo);

    ecs_add_pair(world, e1, RelA, ObjA);
    ecs_add_pair(world, e1, RelB, ObjA);

    ecs_add_pair(world, e2, RelA, ObjA);

    ecs_add_pair(world, e3, RelA, ObjB);
    ecs_add_pair(world, e3, RelB, ObjB);

    ecs_add_pair(world, e4, RelA, ObjB);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(ecs_field_id(&it, 0), Foo);
    test_uint(ecs_field_id(&it, 1), ecs_pair(RelA, ObjA));
    test_uint(ecs_field_id(&it, 2), ecs_pair(RelB, EcsWildcard));

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e4);
    test_uint(ecs_field_id(&it, 0), Foo);
    test_uint(ecs_field_id(&it, 1), ecs_pair(RelA, ObjB));
    test_uint(ecs_field_id(&it, 2), ecs_pair(RelB, EcsWildcard));

    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_two_pair_wildcards_one_not_any(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, RelA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, RelB, (OnInstantiate, Inherit));
    ECS_ENTITY(world, ObjA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, ObjB, (OnInstantiate, Inherit));
    
    ecs_query_t *q = ecs_query(world, {
        .expr = "Foo, (RelA, *), !(RelB, _)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w(world, Foo);
    ecs_entity_t e2 = ecs_new_w(world, Foo);
    ecs_entity_t e3 = ecs_new_w(world, Foo);
    ecs_entity_t e4 = ecs_new_w(world, Foo);

    ecs_add_pair(world, e1, RelA, ObjA);
    ecs_add_pair(world, e1, RelB, ObjA);

    ecs_add_pair(world, e2, RelA, ObjA);

    ecs_add_pair(world, e3, RelA, ObjB);
    ecs_add_pair(world, e3, RelB, ObjB);

    ecs_add_pair(world, e4, RelA, ObjB);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(ecs_field_id(&it, 0), Foo);
    test_uint(ecs_field_id(&it, 1), ecs_pair(RelA, ObjA));

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e4);
    test_uint(ecs_field_id(&it, 0), Foo);
    test_uint(ecs_field_id(&it, 1), ecs_pair(RelA, ObjB));

    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_isa_rematch(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base_1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t base_2 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base_1);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 2);
    test_uint(it.entities[0], base_1);
    test_uint(it.entities[1], base_2);
    Position *p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 30);
    test_int(p[1].y, 40);
    test_uint(it.sources[0], 0);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], inst);
    p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_uint(it.sources[0], base_1);
    test_bool(false, ecs_query_next(&it));

    ecs_remove_pair(world, inst, EcsIsA, base_1);
    ecs_add_pair(world, inst, EcsIsA, base_2);

    it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 2);
    test_uint(it.entities[0], base_1);
    test_uint(it.entities[1], base_2);
    p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 30);
    test_int(p[1].y, 40);
    test_uint(it.sources[0], 0);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], inst);
    p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);
    test_uint(it.sources[0], base_2);
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_childof_rematch(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base_1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t base_2 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t inst = ecs_new_w_pair(world, EcsChildOf, base_1);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(up)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], inst);
    Position *p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_uint(it.sources[0], base_1);
    test_bool(false, ecs_query_next(&it));

    ecs_add_pair(world, inst, EcsChildOf, base_2);

    it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], inst);
    p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);
    test_uint(it.sources[0], base_2);
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_isa_unmatch(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base_1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t base_2 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base_1);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 2);
    test_uint(it.entities[0], base_1);
    test_uint(it.entities[1], base_2);
    Position *p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 30);
    test_int(p[1].y, 40);
    test_uint(it.sources[0], 0);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], inst);
    p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_uint(it.sources[0], base_1);
    test_bool(false, ecs_query_next(&it));

    ecs_remove(world, base_1, Position);

    it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], base_2);
    p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);
    test_uint(it.sources[0], 0);

    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_childof_unmatch(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base_1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t inst = ecs_new_w_pair(world, EcsChildOf, base_1);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(up)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], inst);
    Position *p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_uint(it.sources[0], base_1);
    test_bool(false, ecs_query_next(&it));

    ecs_remove(world, base_1, Position);

    it = ecs_query_iter(world, q);
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_isa_rematch_2_lvls(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base_1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t base_2 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t base = ecs_new_w_pair(world, EcsIsA, base_1);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 2);
    test_uint(it.entities[0], base_1);
    test_uint(it.entities[1], base_2);
    Position *p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 30);
    test_int(p[1].y, 40);
    test_uint(it.sources[0], 0);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], base);
    test_uint(it.sources[0], base_1);
    p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], inst);
    test_uint(it.sources[0], base_1);
    p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_bool(false, ecs_query_next(&it));

    ecs_remove_pair(world, base, EcsIsA, base_1);
    ecs_add_pair(world, base, EcsIsA, base_2);

    it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 2);
    test_uint(it.entities[0], base_1);
    test_uint(it.entities[1], base_2);
    p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 30);
    test_int(p[1].y, 40);
    test_uint(it.sources[0], 0);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], inst);
    test_uint(it.sources[0], base_2);
    p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], base);
    test_uint(it.sources[0], base_2);
    p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);

    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_childof_rematch_2_lvls(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base_1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t base_2 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t base = ecs_new_w_pair(world, EcsChildOf, base_1);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsChildOf, base);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(up)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], base);
    test_uint(it.sources[0], base_1);
    Position *p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], inst);
    test_uint(it.sources[0], base_1);
    p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_bool(false, ecs_query_next(&it));

    ecs_add_pair(world, base, EcsChildOf, base_2);

    it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], inst);
    test_uint(it.sources[0], base_2);
    p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], base);
    test_uint(it.sources[0], base_2);
    p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);

    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_childof_rematch_from_isa(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base_1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t base_2 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t base = ecs_new_w_pair(world, EcsIsA, base_1);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsChildOf, base);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position(up)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], inst);
    test_uint(it.sources[0], base_1);
    Position *p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_bool(false, ecs_query_next(&it));

    ecs_remove_pair(world, base, EcsIsA, base_1);
    ecs_add_pair(world, base, EcsIsA, base_2);

    it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], inst);
    test_uint(it.sources[0], base_2);
    p = ecs_field(&it, Position, 0);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_rematch_optional_ref(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));
    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, ?Position(up)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t parent = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t child = ecs_new_w(world, Tag);
    ecs_add_pair(world, child, EcsChildOf, parent);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(child, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_uint(0, it.sources[0]);
    test_uint(parent, it.sources[1]);
    Position *p = ecs_field(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    test_bool(false, ecs_query_next(&it));

    ecs_remove(world, parent, Position);

    it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(child, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_uint(0, it.sources[0]);
    test_uint(0, it.sources[1]);
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_rematch_optional_ref_w_2_refs(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));
    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_COMPONENT(world, Velocity);
    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Tag, Velocity(up), ?Position(up)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t parent = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_set(world, parent, Velocity, {1, 2});
    ecs_entity_t child = ecs_new_w(world, Tag);
    ecs_add_pair(world, child, EcsChildOf, parent);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(child, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));
    test_uint(0, it.sources[0]);
    test_uint(parent, it.sources[1]);
    Velocity *v = ecs_field(&it, Velocity, 1);
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 2);
    Position *p = ecs_field(&it, Position, 2);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    test_bool(false, ecs_query_next(&it));

    ecs_remove(world, parent, Position);

    it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(child, it.entities[0]);
    test_uint(Tag, ecs_field_id(&it, 0));
    test_uint(ecs_id(Velocity), ecs_field_id(&it, 1));
    test_uint(ecs_id(Position), ecs_field_id(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));
    test_uint(0, it.sources[0]);
    test_uint(parent, it.sources[1]);
    test_uint(0, it.sources[2]);
    v = ecs_field(&it, Velocity, 1);
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 2);
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_rematch_optional_ref_tag_w_ref_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, ?Position(up), TagB(up)",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_entity_t parent = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_add(world, parent, TagB);

    ecs_entity_t child = ecs_new_w(world, TagA);
    ecs_add_pair(world, child, EcsChildOf, parent);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(child, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));
    test_uint(0, it.sources[0]);
    test_uint(parent, it.sources[1]);
    test_uint(parent, it.sources[2]);
    Position *p = ecs_field(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    test_bool(false, ecs_query_next(&it));

    ecs_remove(world, parent, Position);

    it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(child, it.entities[0]);
    test_uint(TagA, ecs_field_id(&it, 0));
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 0));
    test_bool(false, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));
    test_uint(0, it.sources[0]);
    test_uint(0, it.sources[1]);
    test_uint(parent, it.sources[2]);
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_match_query_expr_from_scope(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t foo = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Foo"
    });

    ecs_entity_t bar = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Foo.Bar"
    });

    ecs_set_scope(world, foo);

    ecs_query_t *q = ecs_query(world, {
        .expr = "Bar",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_set_scope(world, 0);

    ecs_entity_t e = ecs_new_w_id(world, bar);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(bar, ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_query_long_or_w_ref(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ECS_ENTITY(world, A, (OnInstantiate, Inherit));
    ECS_ENTITY(world, B, (OnInstantiate, Inherit));
    ECS_ENTITY(world, C, (OnInstantiate, Inherit));
    ECS_ENTITY(world, D, (OnInstantiate, Inherit));
    ECS_ENTITY(world, E, (OnInstantiate, Inherit));
    ECS_ENTITY(world, F, (OnInstantiate, Inherit));
    ECS_ENTITY(world, G, (OnInstantiate, Inherit));
    ECS_ENTITY(world, H, (OnInstantiate, Inherit));
    ECS_ENTITY(world, I, (OnInstantiate, Inherit));
    ECS_ENTITY(world, J, (OnInstantiate, Inherit));
    ECS_ENTITY(world, K, (OnInstantiate, Inherit));
    ECS_ENTITY(world, L, (OnInstantiate, Inherit));
    ECS_ENTITY(world, M, (OnInstantiate, Inherit));
    ECS_ENTITY(world, N, (OnInstantiate, Inherit));

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Position, {10, 20});

    ecs_entity_t e2 = ecs_new_w(world, A);

    ecs_query_t *q = ecs_query(world, {
       .expr = 
        "Position(e), A || B || C || D || E || F || G || H || I || J || K || L ||"
        "M || N",
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_uint(ecs_id(Position), ecs_field_id(&it, 0));
    test_uint(A, ecs_field_id(&it, 1));
    test_uint(e2, it.entities[0]);
    test_uint(e, it.sources[0]);
    test_uint(0, it.sources[1]);

    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_query_w_pair_id_and_subj(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Obj, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Subj, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{
            .id = ecs_pair(Rel, Obj), .src.id = Subj
        }},
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(false, ecs_query_next(&it));

    ecs_add_pair(world, Subj, Rel, Obj);

    it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 0);
    test_uint(it.sources[0], Subj);
    test_uint(it.ids[0], ecs_pair(Rel, Obj));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Cached_rematch_after_delete_inherited_tag(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t tag = ecs_new(world);
    ecs_entity_t base = ecs_new_w_id(world, tag);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = tag }
        },
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(base, it.entities[0]);
    test_uint(tag, ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(inst, it.entities[0]);
    test_uint(tag, ecs_field_id(&it, 0));
    test_bool(false, ecs_query_next(&it));

    ecs_delete_with(world, tag);

    it = ecs_query_iter(world, q);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Cached_rematch_after_delete_rel_of_inherited_pair(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t rel = ecs_new(world);
    ecs_entity_t obj = ecs_new(world);
    ecs_entity_t base = ecs_new_w_pair(world, rel, obj);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_pair(rel, obj) }
        },
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(base, it.entities[0]);
    test_uint(ecs_pair(rel, obj), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(inst, it.entities[0]);
    test_uint(ecs_pair(rel, obj), ecs_field_id(&it, 0));
    test_bool(false, ecs_query_next(&it));

    ecs_delete_with(world, ecs_pair(rel, EcsWildcard));

    it = ecs_query_iter(world, q);
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_rematch_after_delete_obj_of_inherited_pair(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t rel = ecs_new(world);
    ecs_entity_t obj = ecs_new(world);
    ecs_entity_t base = ecs_new_w_pair(world, rel, obj);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_pair(rel, obj) }
        },
        .cache_kind = EcsQueryCacheAuto
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(base, it.entities[0]);
    test_uint(ecs_pair(rel, obj), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(inst, it.entities[0]);
    test_uint(ecs_pair(rel, obj), ecs_field_id(&it, 0));
    test_bool(false, ecs_query_next(&it));

    ecs_delete_with(world, ecs_pair(EcsWildcard, obj));

    it = ecs_query_iter(world, q);
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_rematch_empty(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }},
        .cache_kind = EcsQueryCacheAuto
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
        test_bool(false, ecs_query_next(&it));
    }
    
    ecs_entity_t e = ecs_new_w_table(world, table);

    {
        ecs_iter_t it = ecs_query_iter(world, q);

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(base, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(0, ecs_field_src(&it, 0));

        test_bool(true, ecs_query_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        test_uint(Foo, ecs_field_id(&it, 0));
        test_uint(base, ecs_field_src(&it, 0));

        test_bool(false, ecs_query_next(&it));
    }

    ecs_query_fini(q);

    ecs_fini(world);
}

void Cached_rematch_empty_table_w_superset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { Bar, .oper = EcsNot }},
        .cache_kind = EcsQueryCacheAuto
    });

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_add(world, base, Foo);
    test_assert( ecs_is_alive(world, base));

    ecs_delete_with(world, Foo);
    test_assert( !ecs_is_alive(world, base));

    base = ecs_new_w_id(world, EcsPrefab);
    ecs_add(world, base, Foo);

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    ecs_add(world, inst, Bar);

    ecs_run_aperiodic(world, 0); // force table (IsA, base) to empty list

    ecs_remove(world, inst, Bar);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], inst);
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Cached_2_self_up_terms_new_tables(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tag, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Tag, Foo(self|up IsA)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(r != NULL);

    ecs_set_with(world, Tag);
    ecs_entity_t parent_a = ecs_entity(world, {0});
    ecs_entity_t parent_b = ecs_entity(world, { .add = ecs_ids(Foo) });
    ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_a)) });
    ecs_entity_t e_2 = ecs_entity(world, { .add = ecs_ids(ecs_isa(parent_b)) });
    ecs_entity_t e_3 = ecs_entity(world, { .add = ecs_ids(ecs_isa(e_2)) });
    ecs_set_with(world, 0);

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

void Cached_this_self_up_childof_pair_new_tables(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Bar, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Rel, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Tgt, (OnInstantiate, Inherit));

    ecs_query_t *r = ecs_query(world, {
        .expr = "Rel(self|up, Tgt)",
        .cache_kind = EcsQueryCacheAuto
    });

    test_assert(r != NULL);

    ecs_entity_t p0 = ecs_new(world);
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

    ecs_iter_t it = ecs_query_iter(world, r);
    test_bool(true, ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(p1, it.entities[0]);
    test_uint(p2, it.entities[1]);
    test_uint(0, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 0));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(p3, it.entities[0]);
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
    test_uint(e7, it.entities[0]);
    test_uint(p3, ecs_field_src(&it, 0));
    test_uint(ecs_pair(Rel, Tgt), ecs_field_id(&it, 0));

    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(r);

    ecs_fini(world);
}
