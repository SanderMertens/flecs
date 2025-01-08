#include <core.h>

void Search_search(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_table_t *table = ecs_table_add_id(world, 0, TagA);
    table = ecs_table_add_id(world, table, TagB);
    table = ecs_table_add_id(world, table, TagC);

    ecs_id_t id;
    int32_t c = ecs_search(world, table, TagA, &id);
    test_int(c, 0);
    test_uint(id, TagA);

    c = ecs_search(world, table, TagB, &id);
    test_int(c, 1);
    test_uint(id, TagB);

    c = ecs_search(world, table, TagC, &id);
    test_int(c, 2);
    test_uint(id, TagC);

    ecs_fini(world);
}

void Search_search_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_table_t *table = ecs_table_add_id(world, 0, Tag);
    table = ecs_table_add_id(world, table, ecs_pair(Rel, ObjA));
    table = ecs_table_add_id(world, table, ecs_pair(Rel, ObjB));

    ecs_id_t id;
    int32_t c = ecs_search(world, table, ecs_pair(Rel, EcsWildcard), &id);
    test_int(c, 1);
    test_uint(id, ecs_pair(Rel, ObjA));

    ecs_fini(world);
}

void Search_search_wildcard_w_offset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);
    ECS_TAG(world, ObjC);

    ecs_table_t *table = ecs_table_add_id(world, 0, Tag);
    table = ecs_table_add_id(world, table, ecs_pair(Rel, ObjA));
    table = ecs_table_add_id(world, table, ecs_pair(Rel, ObjB));
    table = ecs_table_add_id(world, table, ecs_pair(Rel, ObjC));

    ecs_id_t id;
    int32_t c = ecs_search(world, table, ecs_pair(Rel, EcsWildcard), &id);
    test_int(c, 1);
    test_uint(id, ecs_pair(Rel, ObjA));

    c = ecs_search_offset(world, table, 2, ecs_pair(Rel, EcsWildcard), &id);
    test_int(c, 2);
    test_uint(id, ecs_pair(Rel, ObjB));

    c = ecs_search_offset(world, table, 3, ecs_pair(Rel, EcsWildcard), &id);
    test_int(c, 3);
    test_uint(id, ecs_pair(Rel, ObjC));

    c = ecs_search_offset(world, table, 4, ecs_pair(Rel, EcsWildcard), &id);
    test_int(c, -1);

    ecs_fini(world);
}

void Search_search_relation_wildcard_w_offset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, RelX);
    ECS_TAG(world, RelY);
    ECS_TAG(world, RelZ);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_table_t *table = ecs_table_add_id(world, 0, Tag);
    table = ecs_table_add_id(world, table, ecs_pair(RelX, ObjA));
    table = ecs_table_add_id(world, table, ecs_pair(RelX, ObjB));
    table = ecs_table_add_id(world, table, ecs_pair(RelY, ObjA));
    table = ecs_table_add_id(world, table, ecs_pair(RelY, ObjB));
    table = ecs_table_add_id(world, table, ecs_pair(RelZ, ObjA));
    table = ecs_table_add_id(world, table, ecs_pair(RelZ, ObjB));

    ecs_id_t id;
    int32_t c = ecs_search(world, table, ecs_pair(EcsWildcard, ObjB), &id);
    test_int(c, 2);
    test_uint(id, ecs_pair(RelX, ObjB));

    c = ecs_search_offset(world, table, 3, ecs_pair(EcsWildcard, ObjB), &id);
    test_int(c, 4);
    test_uint(id, ecs_pair(RelY, ObjB));

    c = ecs_search_offset(world, table, 5, ecs_pair(EcsWildcard, ObjB), &id);
    test_int(c, 6);
    test_uint(id, ecs_pair(RelZ, ObjB));

    c = ecs_search_offset(world, table, 7, ecs_pair(EcsWildcard, ObjB), &id);
    test_int(c, -1);

    ecs_fini(world);
}

void Search_search_pair_w_any_rel(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_table_t *table = ecs_table_add_id(world, 0, Tag);
    table = ecs_table_add_id(world, table, ecs_pair(Rel, ObjA));
    table = ecs_table_add_id(world, table, ecs_pair(Rel, ObjB));

    ecs_id_t id;
    int32_t c = ecs_search(world, table, ecs_pair(EcsAny, ObjB), &id);
    test_int(c, 2);
    test_uint(id, ecs_pair(Rel, ObjB));

    ecs_fini(world);
}

void Search_search_pair_w_any_obj(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_table_t *table = ecs_table_add_id(world, 0, Tag);
    table = ecs_table_add_id(world, table, ecs_pair(Rel, ObjA));
    table = ecs_table_add_id(world, table, ecs_pair(Rel, ObjB));

    ecs_id_t id;
    int32_t c = ecs_search(world, table, ecs_pair(Rel, EcsAny), &id);
    test_int(c, 1);
    test_uint(id, ecs_pair(Rel, ObjA));

    ecs_fini(world);
}

void Search_search_follow_relation_lvl_0(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Rel);

    ecs_entity_t e_0 = ecs_new_w(world, TagA);
    ecs_add(world, e_0, TagB);

    ecs_table_t *t = ecs_get_table(world, e_0);
    
    ecs_entity_t subj = 0;
    ecs_id_t id = 0;
    int32_t c = ecs_search_relation(world, t, 0, TagB, Rel, 0, &subj, &id, 0);
    test_int(c, 1);
    test_uint(id, TagB);
    test_uint(subj, 0);

    ecs_fini(world);
}

void Search_search_follow_relation_lvl_1(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Rel);

    ecs_entity_t e_0 = ecs_new_w(world, TagA);
    ecs_add(world, e_0, TagB);
    ecs_entity_t e_1 = ecs_new_w_pair(world, Rel, e_0);

    ecs_table_t *t = ecs_get_table(world, e_1);
    
    ecs_entity_t subj = 0;
    ecs_id_t id = 0;
    int32_t c = ecs_search_relation(world, t, 0, TagB, Rel, 0, &subj, &id, 0);
    test_int(c, 0);
    test_uint(id, TagB);
    test_uint(subj, e_0);

    ecs_fini(world);
}

void Search_search_follow_relation_lvl_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Rel);

    ecs_entity_t e_0 = ecs_new_w(world, TagA);
    ecs_add(world, e_0, TagB);
    ecs_entity_t e_1 = ecs_new_w_pair(world, Rel, e_0);
    ecs_entity_t e_2 = ecs_new_w_pair(world, Rel, e_1);

    ecs_table_t *t = ecs_get_table(world, e_2);
    
    ecs_entity_t subj = 0;
    ecs_id_t id = 0;
    int32_t c = ecs_search_relation(world, t, 0, TagB, Rel, 0, &subj, &id, 0);
    test_int(c, 0);
    test_uint(id, TagB);
    test_uint(subj, e_0);

    ecs_fini(world);
}

void Search_search_follow_relation_lvl_3(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Rel);

    ecs_entity_t e_0 = ecs_new_w(world, TagA);
    ecs_add(world, e_0, TagB);
    ecs_entity_t e_1 = ecs_new_w_pair(world, Rel, e_0);
    ecs_entity_t e_2 = ecs_new_w_pair(world, Rel, e_1);
    ecs_entity_t e_3 = ecs_new_w_pair(world, Rel, e_2);

    ecs_table_t *t = ecs_get_table(world, e_3);
    
    ecs_entity_t subj = 0;
    ecs_id_t id = 0;
    int32_t c = ecs_search_relation(world, t, 0, TagB, Rel, 0, &subj, &id, 0);
    test_int(c, 0);
    test_uint(id, TagB);
    test_uint(subj, e_0);

    ecs_fini(world);
}

void Search_search_first_lvl_0(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Rel);

    ecs_entity_t e_0 = ecs_new_w(world, TagA);
    ecs_add(world, e_0, TagB);

    ecs_table_t *t = ecs_get_table(world, e_0);
    
    ecs_entity_t subj = 0;
    ecs_id_t id = 0;
    int32_t c = ecs_search_relation(world, t, 0, TagB, Rel, 0, &subj, &id, 0);
    test_int(c, 1);
    test_uint(id, TagB);
    test_uint(subj, 0);

    ecs_fini(world);
}

void Search_search_first_lvl_1(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Rel);

    ecs_entity_t e_0 = ecs_new_w(world, TagA);
    ecs_entity_t e_1 = ecs_new_w_pair(world, Rel, e_0);
    ecs_add(world, e_0, TagB);

    ecs_table_t *t = ecs_get_table(world, e_1);
    
    ecs_entity_t subj = 0;
    ecs_id_t id = 0;
    int32_t c = ecs_search_relation(world, t, 0, TagB, Rel, 0, &subj, &id, 0);
    test_int(c, 0);
    test_uint(id, TagB);
    test_uint(subj, e_0);

    ecs_fini(world);
}

void Search_search_first_lvl_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Rel);

    ecs_entity_t e_0 = ecs_new_w(world, TagA);
    ecs_entity_t e_1 = ecs_new_w_pair(world, Rel, e_0);
    ecs_entity_t e_2 = ecs_new_w_pair(world, Rel, e_1);
    ecs_add(world, e_0, TagB);
    ecs_add(world, e_1, TagB);

    ecs_table_t *t = ecs_get_table(world, e_2);
    
    ecs_entity_t subj = 0;
    ecs_id_t id = 0;
    int32_t c = ecs_search_relation(world, t, 0, TagB, Rel, 0, &subj, &id, 0);
    test_int(c, 0);
    test_uint(id, TagB);
    test_uint(subj, e_1);

    ecs_fini(world);
}

void Search_search_first_lvl_3(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Rel);

    ecs_entity_t e_0 = ecs_new_w(world, TagA);
    ecs_entity_t e_1 = ecs_new_w_pair(world, Rel, e_0);
    ecs_entity_t e_2 = ecs_new_w_pair(world, Rel, e_1);
    ecs_entity_t e_3 = ecs_new_w_pair(world, Rel, e_2);
    ecs_add(world, e_0, TagB);
    ecs_add(world, e_1, TagB);
    ecs_add(world, e_2, TagB);

    ecs_table_t *t = ecs_get_table(world, e_3);
    
    ecs_entity_t subj = 0;
    ecs_id_t id = 0;
    int32_t c = ecs_search_relation(world, t, 0, TagB, Rel, 0, &subj, &id, 0);
    test_int(c, 0);
    test_uint(id, TagB);
    test_uint(subj, e_2);

    ecs_fini(world);
}

void Search_search_relation_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, Rel);

    ecs_entity_t b = ecs_new_w(world, Tag);
    ecs_add(world, b, Tag);
    ecs_add_pair(world, b, Rel, TgtA);
    ecs_add_pair(world, b, Rel, TgtB);
    
    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsChildOf, b);

    ecs_table_t *table = ecs_get_table(world, e);
    ecs_entity_t subj = 0;
    ecs_id_t id = 0;
    int32_t column = 0;

    column = ecs_search_relation(world, table, 0, ecs_pair(Rel, EcsWildcard), 
        EcsChildOf, EcsUp, &subj, &id, 0);
    test_int(column, 0);
    test_uint(subj, b);
    test_uint(id, ecs_pair(Rel, TgtA));

    ecs_fini(world);
}

void Search_search_relation_at_offset(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Rel);

    ecs_entity_t b1 = ecs_new_w(world, Tag);
    ecs_entity_t b2 = ecs_new_w(world, Tag);
    
    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Rel, b1);
    ecs_add_pair(world, e, Rel, b2);

    ecs_table_t *table = ecs_get_table(world, e);
    ecs_entity_t subj = 0;
    ecs_id_t id = 0;
    int32_t column = 0;

    column = ecs_search_relation(world, table, 0, Tag, Rel, EcsUp, &subj, &id, 0);
    test_int(column, 0);
    test_uint(subj, b1);
    test_uint(id, Tag);
    
    subj = 0;
    column = ecs_search_relation(
        world, table, 1, Tag, Rel, EcsUp, &subj, &id, 0);
    test_int(column, 1);
    test_uint(subj, b2);
    test_uint(id, Tag);

    ecs_fini(world);
}

void Search_search_relation_inherit_from_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagC, (OnInstantiate, Inherit));

    ecs_entity_t b = ecs_new(world);
    ecs_add(world, b, TagA);
    ecs_add(world, b, TagB);

    ecs_entity_t p = ecs_new(world);
    ecs_add_pair(world, p, EcsIsA, b);
    ecs_add(world, p, TagC);
    
    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, TagA);
    ecs_add(world, e, TagB);
    ecs_add(world, e, TagC);
    ecs_add_pair(world, e, EcsChildOf, p);

    ecs_table_t *table = ecs_get_table(world, e);
    ecs_entity_t subj = 0;
    ecs_id_t id = 0;
    int32_t column = 0;

    column = ecs_search_relation(world, table, 0, TagA, EcsChildOf, EcsUp, &subj, &id, 0);
    test_int(column, 3);
    test_int(subj, b);
    test_int(id, TagA);
    column = ecs_search_relation(world, table, 0, TagB, EcsChildOf, EcsUp, &subj, &id, 0);
    test_int(column, 3);
    test_int(subj, b);
    test_int(id, TagB);
    column = ecs_search_relation(world, table, 0, TagC, EcsChildOf, EcsUp, &subj, &id, 0);
    test_int(column, 3);
    test_int(subj, p);
    test_int(id, TagC);

    ecs_fini(world);
}

void Search_search_relation_dont_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, DontInherit));
    ECS_ENTITY(world, TagC, (OnInstantiate, Inherit));

    ecs_entity_t b = ecs_new(world);
    ecs_add(world, b, TagA);
    ecs_add(world, b, TagB);
    ecs_add(world, b, TagC);
    
    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, TagA);
    ecs_add(world, e, TagB);
    ecs_add(world, e, TagC);
    ecs_add_pair(world, e, EcsIsA, b);

    ecs_table_t *table = ecs_get_table(world, e);
    ecs_entity_t subj = 0;
    ecs_id_t id = 0;
    int32_t column = 0;

    column = ecs_search_relation(world, table, 0, TagA, EcsIsA, EcsUp, &subj, &id, 0);
    test_int(column, 3);
    test_int(subj, b);
    test_int(id, TagA);

    column = ecs_search_relation(world, table, 0, TagB, EcsIsA, EcsUp, &subj, &id, 0);
    test_int(column, -1);
    column = ecs_search_relation(world, table, 0, TagC, EcsIsA, EcsUp, &subj, &id, 0);
    test_int(column, 3);
    test_int(subj, b);
    test_int(id, TagC);

    column = ecs_search_relation(world, table, 0, TagB, EcsIsA, EcsSelf|EcsUp, &subj, &id, 0);
    test_int(column, 1);
    test_int(subj, 0);
    test_int(id, TagB);

    column = ecs_search_relation(world, table, 0, TagB, EcsIsA, EcsSelf, &subj, &id, 0);
    test_int(column, 1);
    test_int(subj, 0);
    test_int(id, TagB);

    ecs_fini(world);
}

void Search_search_relation_dont_inherit_from_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, DontInherit));
    ECS_ENTITY(world, TagC, (OnInstantiate, Inherit));

    ecs_entity_t b = ecs_new(world);
    ecs_add(world, b, TagA);
    ecs_add(world, b, TagB);

    ecs_entity_t p = ecs_new(world);
    ecs_add_pair(world, p, EcsIsA, b);
    ecs_add(world, p, TagC);
    
    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, TagA);
    ecs_add(world, e, TagB);
    ecs_add(world, e, TagC);
    ecs_add_pair(world, e, EcsChildOf, p);

    ecs_table_t *table = ecs_get_table(world, e);
    ecs_entity_t subj = 0;
    ecs_id_t id = 0;
    int32_t column = 0;

    column = ecs_search_relation(world, table, 0, TagA, EcsChildOf, EcsUp, &subj, &id, 0);
    test_int(column, 3);
    test_int(subj, b);
    test_int(id, TagA);
    column = ecs_search_relation(world, table, 0, TagB, EcsChildOf, EcsUp, &subj, &id, 0);
    test_int(column, -1);
    column = ecs_search_relation(world, table, 0, TagC, EcsChildOf, EcsUp, &subj, &id, 0);
    test_int(column, 3);
    test_int(subj, p);
    test_int(id, TagC);

    column = ecs_search_relation(world, table, 0, TagB, EcsChildOf, EcsSelf|EcsUp, &subj, &id, 0);
    test_int(column, 1);
    test_int(subj, 0);
    test_int(id, TagB);

    column = ecs_search_relation(world, table, 0, TagB, EcsChildOf, EcsSelf, &subj, &id, 0);
    test_int(column, 1);
    test_int(subj, 0);
    test_int(id, TagB);

    ecs_fini(world);
}

void Search_search_relation_exclusive(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, (OnInstantiate, Inherit));
    ECS_ENTITY(world, TagB, (OnInstantiate, Inherit));
    ECS_ENTITY(world, Rel, Exclusive, (OnInstantiate, Inherit));

    ecs_entity_t b = ecs_new(world);
    ecs_add_pair(world, b, Rel, TagA);
    
    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Rel, TagB);
    ecs_add_pair(world, e, EcsIsA, b);

    ecs_table_t *table = ecs_get_table(world, e);
    ecs_entity_t subj = 0;
    ecs_id_t id = 0;
    int32_t column = 0;

    column = ecs_search_relation(world, table, 0, ecs_pair(Rel, TagA), EcsIsA, EcsSelf|EcsUp, &subj, &id, 0);
    test_int(column, -1);

    column = ecs_search_relation(world, table, 0, ecs_pair(Rel, TagB), EcsIsA, EcsSelf|EcsUp, &subj, &id, 0);
    test_int(column, 1);
    test_int(subj, 0);
    test_int(id, ecs_pair(Rel, TagB));

    column = ecs_search_relation(world, table, 0, ecs_pair(Rel, TagA), EcsIsA, EcsUp, &subj, &id, 0);
    test_int(column, -1);

    column = ecs_search_relation(world, table, 0, ecs_pair(Rel, TagB), EcsIsA, EcsUp, &subj, &id, 0);
    test_int(column, -1);

    ecs_fini(world);
}
