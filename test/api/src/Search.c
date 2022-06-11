#include <api.h>

void Search_search() {
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

void Search_search_wildcard() {
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

void Search_search_wildcard_w_offset() {
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

void Search_search_relation_wildcard_w_offset() {
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

void Search_search_pair_w_any_rel() {
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

void Search_search_pair_w_any_obj() {
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

void Search_search_follow_relation_lvl_0() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Rel);

    ecs_entity_t e_0 = ecs_new(world, TagA);
    ecs_add(world, e_0, TagB);

    ecs_table_t *t = ecs_get_table(world, e_0);
    
    ecs_entity_t subj = 0;
    ecs_id_t id = 0;
    int32_t depth = 0;
    int32_t c = ecs_search_relation(
        world, t, 0, TagB, Rel, 0, 0, &subj, &id, &depth, 0);
    test_int(c, 1);
    test_int(depth, 0);
    test_uint(id, TagB);
    test_uint(subj, 0);

    ecs_fini(world);
}

void Search_search_follow_relation_lvl_1() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Rel);

    ecs_entity_t e_0 = ecs_new(world, TagA);
    ecs_add(world, e_0, TagB);
    ecs_entity_t e_1 = ecs_new_w_pair(world, Rel, e_0);

    ecs_table_t *t = ecs_get_table(world, e_1);
    
    ecs_entity_t subj = 0;
    ecs_id_t id = 0;
    int32_t depth = 0;
    int32_t c = ecs_search_relation(
        world, t, 0, TagB, Rel, 0, 0, &subj, &id, &depth, 0);
    test_int(c, 0);
    test_int(depth, 1);
    test_uint(id, TagB);
    test_uint(subj, e_0);

    ecs_fini(world);
}

void Search_search_follow_relation_lvl_2() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Rel);

    ecs_entity_t e_0 = ecs_new(world, TagA);
    ecs_add(world, e_0, TagB);
    ecs_entity_t e_1 = ecs_new_w_pair(world, Rel, e_0);
    ecs_entity_t e_2 = ecs_new_w_pair(world, Rel, e_1);

    ecs_table_t *t = ecs_get_table(world, e_2);
    
    ecs_entity_t subj = 0;
    ecs_id_t id = 0;
    int32_t depth = 0;
    int32_t c = ecs_search_relation(
        world, t, 0, TagB, Rel, 0, 0, &subj, &id, &depth, 0);
    test_int(c, 0);
    test_int(depth, 2);
    test_uint(id, TagB);
    test_uint(subj, e_0);

    ecs_fini(world);
}

void Search_search_follow_relation_lvl_3() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Rel);

    ecs_entity_t e_0 = ecs_new(world, TagA);
    ecs_add(world, e_0, TagB);
    ecs_entity_t e_1 = ecs_new_w_pair(world, Rel, e_0);
    ecs_entity_t e_2 = ecs_new_w_pair(world, Rel, e_1);
    ecs_entity_t e_3 = ecs_new_w_pair(world, Rel, e_2);

    ecs_table_t *t = ecs_get_table(world, e_3);
    
    ecs_entity_t subj = 0;
    ecs_id_t id = 0;
    int32_t depth = 0;
    int32_t c = ecs_search_relation(
        world, t, 0, TagB, Rel, 0, 0, &subj, &id, &depth, 0);
    test_int(c, 0);
    test_int(depth, 3);
    test_uint(id, TagB);
    test_uint(subj, e_0);

    ecs_fini(world);
}

void Search_search_first_lvl_0() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Rel);

    ecs_entity_t e_0 = ecs_new(world, TagA);
    ecs_add(world, e_0, TagB);

    ecs_table_t *t = ecs_get_table(world, e_0);
    
    ecs_entity_t subj = 0;
    ecs_id_t id = 0;
    int32_t depth = 0;
    int32_t c = ecs_search_relation(
        world, t, 0, TagB, Rel, 0, 0, &subj, &id, &depth, 0);
    test_int(c, 1);
    test_int(depth, 0);
    test_uint(id, TagB);
    test_uint(subj, 0);

    ecs_fini(world);
}

void Search_search_first_lvl_1() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Rel);

    ecs_entity_t e_0 = ecs_new(world, TagA);
    ecs_entity_t e_1 = ecs_new_w_pair(world, Rel, e_0);
    ecs_add(world, e_0, TagB);

    ecs_table_t *t = ecs_get_table(world, e_1);
    
    ecs_entity_t subj = 0;
    ecs_id_t id = 0;
    int32_t depth = 0;
    int32_t c = ecs_search_relation(
        world, t, 0, TagB, Rel, 0, 0, &subj, &id, &depth, 0);
    test_int(c, 0);
    test_int(depth, 1);
    test_uint(id, TagB);
    test_uint(subj, e_0);

    ecs_fini(world);
}

void Search_search_first_lvl_2() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Rel);

    ecs_entity_t e_0 = ecs_new(world, TagA);
    ecs_entity_t e_1 = ecs_new_w_pair(world, Rel, e_0);
    ecs_entity_t e_2 = ecs_new_w_pair(world, Rel, e_1);
    ecs_add(world, e_0, TagB);
    ecs_add(world, e_1, TagB);

    ecs_table_t *t = ecs_get_table(world, e_2);
    
    ecs_entity_t subj = 0;
    ecs_id_t id = 0;
    int32_t depth = 0;
    int32_t c = ecs_search_relation(
        world, t, 0, TagB, Rel, 0, 0, &subj, &id, &depth, 0);
    test_int(c, 0);
    test_int(depth, 1);
    test_uint(id, TagB);
    test_uint(subj, e_1);

    ecs_fini(world);
}

void Search_search_first_lvl_3() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Rel);

    ecs_entity_t e_0 = ecs_new(world, TagA);
    ecs_entity_t e_1 = ecs_new_w_pair(world, Rel, e_0);
    ecs_entity_t e_2 = ecs_new_w_pair(world, Rel, e_1);
    ecs_entity_t e_3 = ecs_new_w_pair(world, Rel, e_2);
    ecs_add(world, e_0, TagB);
    ecs_add(world, e_1, TagB);
    ecs_add(world, e_2, TagB);

    ecs_table_t *t = ecs_get_table(world, e_3);
    
    ecs_entity_t subj = 0;
    ecs_id_t id = 0;
    int32_t depth = 0;
    int32_t c = ecs_search_relation(
        world, t, 0, TagB, Rel, 0, 0, &subj, &id, &depth, 0);
    test_int(c, 0);
    test_int(depth, 1);
    test_uint(id, TagB);
    test_uint(subj, e_2);

    ecs_fini(world);
}

void Search_search_relation_at_offset() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Rel);

    ecs_entity_t b1 = ecs_new(world, Tag);
    ecs_entity_t b2 = ecs_new(world, Tag);
    
    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, Rel, b1);
    ecs_add_pair(world, e, Rel, b2);

    ecs_table_t *table = ecs_get_table(world, e);

    ecs_entity_t subj = 0;
    ecs_id_t id = 0;
    int32_t depth = 0;
    int32_t column = 0;

    column = ecs_search_relation(
        world, table, 0, Tag, Rel, 1, 0, &subj, &id, &depth, 0);
    test_int(column, 0);
    test_int(depth, 1);
    test_uint(subj, b1);
    test_uint(id, Tag);
    
    subj = 0;
    depth = 0;
    column = ecs_search_relation(
        world, table, 1, Tag, Rel, 1, 0, &subj, &id, &depth, 0);
    test_int(column, 1);
    test_int(depth, 1);
    test_uint(subj, b2);
    test_uint(id, Tag);

    ecs_fini(world);
}
