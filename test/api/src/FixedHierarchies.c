#include <api.h>

void FixedHierarchies_make_fixed_1_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    
    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) != ecs_get_table(world, p_2_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);

    ecs_fini(world);
}

void FixedHierarchies_make_fixed_1_lvl_w_init(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    
    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) != ecs_get_table(world, p_2_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);

    ecs_fini(world);
}

void FixedHierarchies_make_fixed_1_lvl_w_init_comp(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_set_hooks(world, Position, {
        .ctor = ecs_default_ctor
    });

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_set(world, p_1_1, Position, {10, 20});

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_set(world, p_2_1, Position, {30, 40});
    
    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) != ecs_get_table(world, p_2_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);

    {
        const Position *p = ecs_get(world, p_1_1, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Position *p = ecs_get(world, p_2_1, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }

    ecs_fini(world);
}

void FixedHierarchies_make_fixed_1_lvl_w_init_comp_after_tree_fixed(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ecs_set_hooks(world, Position, {
        .ctor = ecs_default_ctor
    });

    ecs_entity_t rel = ecs_new_id(world);
    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_set(world, p_1_1, Position, {10, 20});
    ecs_set_pair_second(world, p_1_1, rel, Position, {100, 200});

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_set(world, p_2_1, Position, {30, 40});
    ecs_set_pair_second(world, p_2_1, rel, Position, {300, 400});
    
    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) != ecs_get_table(world, p_2_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);

    {
        const Position *p = ecs_get(world, p_1_1, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }
    {
        const Position *p = ecs_get(world, p_2_1, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }

    ecs_fini(world);
}

void FixedHierarchies_make_fixed_1_lvl_2_entities(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t p_1_2 = ecs_new_w_pair(world, EcsChildOf, p_1);

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t p_2_2 = ecs_new_w_pair(world, EcsChildOf, p_2);
    
    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) != ecs_get_table(world, p_2_1));
    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_1_2));
    test_assert(ecs_get_table(world, p_2_1) == ecs_get_table(world, p_2_2));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_2, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);
    test_assert(ecs_get_target(world, p_2_2, EcsChildOf, 0) == p_2);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_1));
    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_1_2));
    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_2));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_2, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);
    test_assert(ecs_get_target(world, p_2_2, EcsChildOf, 0) == p_2);

    ecs_fini(world);
}

void FixedHierarchies_make_fixed_1_lvl_2_tables(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t p_1_2 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_add(world, p_1_2, Tag);

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t p_2_2 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_add(world, p_2_2, Tag);
    
    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) != ecs_get_table(world, p_2_1));
    test_assert(ecs_get_table(world, p_1_2) != ecs_get_table(world, p_2_2));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_2, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);
    test_assert(ecs_get_target(world, p_2_2, EcsChildOf, 0) == p_2);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_1));
    test_assert(ecs_get_table(world, p_1_2) == ecs_get_table(world, p_2_2));
    test_assert(ecs_get_table(world, p_1_1) != ecs_get_table(world, p_1_2));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_2, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);
    test_assert(ecs_get_target(world, p_2_2, EcsChildOf, 0) == p_2);

    test_assert(ecs_has(world, p_1_2, Tag));
    test_assert(ecs_has(world, p_2_2, Tag));

    ecs_fini(world);
}

void FixedHierarchies_make_fixed_2_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t p_1_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1_1);

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t p_2_1_1 = ecs_new_w_pair(world, EcsChildOf, p_2_1);
    
    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) != ecs_get_table(world, p_2_1));
    test_assert(ecs_get_table(world, p_1_1_1) != ecs_get_table(world, p_2_1_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_1_1, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);
    test_assert(ecs_get_target(world, p_2_1_1, EcsChildOf, 0) == p_2_1);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_1));
    test_assert(ecs_get_table(world, p_1_1_1) == ecs_get_table(world, p_2_1_1));
    test_assert(ecs_get_table(world, p_1_1_1) != ecs_get_table(world, p_1_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_1_1, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);
    test_assert(ecs_get_target(world, p_2_1_1, EcsChildOf, 0) == p_2_1);

    ecs_fini(world);
}

void FixedHierarchies_make_fixed_2_lvl_2_tables(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t p_1_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1_1);
    ecs_entity_t p_1_1_2 = ecs_new_w_pair(world, EcsChildOf, p_1_1);
    ecs_add(world, p_1_1_2, Tag);

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t p_2_1_1 = ecs_new_w_pair(world, EcsChildOf, p_2_1);
    ecs_entity_t p_2_1_2 = ecs_new_w_pair(world, EcsChildOf, p_2_1);
    ecs_add(world, p_2_1_2, Tag);
    
    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) != ecs_get_table(world, p_2_1));
    test_assert(ecs_get_table(world, p_1_1_1) != ecs_get_table(world, p_1_1_2));
    test_assert(ecs_get_table(world, p_2_1_1) != ecs_get_table(world, p_2_1_2));
    test_assert(ecs_get_table(world, p_1_1_1) != ecs_get_table(world, p_2_1_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_1_1, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);
    test_assert(ecs_get_target(world, p_2_1_1, EcsChildOf, 0) == p_2_1);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_1));
    test_assert(ecs_get_table(world, p_1_1_1) == ecs_get_table(world, p_2_1_1));
    test_assert(ecs_get_table(world, p_1_1_2) == ecs_get_table(world, p_2_1_2));
    test_assert(ecs_get_table(world, p_1_1_1) != ecs_get_table(world, p_1_1_2));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_1_1, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);
    test_assert(ecs_get_target(world, p_2_1_1, EcsChildOf, 0) == p_2_1);

    test_assert(ecs_has(world, p_1_1_2, Tag));
    test_assert(ecs_has(world, p_2_1_2, Tag));

    ecs_fini(world);
}

void FixedHierarchies_make_fixed_3_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t p_1_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1_1);
    ecs_entity_t p_1_1_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1_1_1);

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t p_2_1_1 = ecs_new_w_pair(world, EcsChildOf, p_2_1);
    ecs_entity_t p_2_1_1_1 = ecs_new_w_pair(world, EcsChildOf, p_2_1_1);
    
    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) != ecs_get_table(world, p_2_1));
    test_assert(ecs_get_table(world, p_1_1_1) != ecs_get_table(world, p_2_1_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_1_1, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_1_1_1_1, EcsChildOf, 0) == p_1_1_1);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);
    test_assert(ecs_get_target(world, p_2_1_1, EcsChildOf, 0) == p_2_1);
    test_assert(ecs_get_target(world, p_2_1_1_1, EcsChildOf, 0) == p_2_1_1);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_1));
    test_assert(ecs_get_table(world, p_1_1_1) == ecs_get_table(world, p_2_1_1));
    test_assert(ecs_get_table(world, p_1_1_1) != ecs_get_table(world, p_1_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_1_1, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_1_1_1_1, EcsChildOf, 0) == p_1_1_1);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);
    test_assert(ecs_get_target(world, p_2_1_1, EcsChildOf, 0) == p_2_1);
    test_assert(ecs_get_target(world, p_2_1_1_1, EcsChildOf, 0) == p_2_1_1);

    ecs_fini(world);
}

void FixedHierarchies_make_fixed_3_lvl_w_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_entity(world, "p");
    
    ecs_entity_t p_1 = ecs_new_entity(world, "p._1");
    ecs_entity_t p_1_1 = ecs_new_entity(world, "p._1._1");
    ecs_entity_t p_1_1_1 = ecs_new_entity(world, "p._1._1._1");
    ecs_entity_t p_1_1_1_1 = ecs_new_entity(world, "p._1._1._1._1");

    ecs_entity_t p_2 = ecs_new_entity(world, "p._2");
    ecs_entity_t p_2_1 = ecs_new_entity(world, "p._2._1");
    ecs_entity_t p_2_1_1 = ecs_new_entity(world, "p._2._1._1");
    ecs_entity_t p_2_1_1_1 = ecs_new_entity(world, "p._2._1._1._1");
    
    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) != ecs_get_table(world, p_2_1));
    test_assert(ecs_get_table(world, p_1_1_1) != ecs_get_table(world, p_2_1_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_1_1, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_1_1_1_1, EcsChildOf, 0) == p_1_1_1);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);
    test_assert(ecs_get_target(world, p_2_1_1, EcsChildOf, 0) == p_2_1);
    test_assert(ecs_get_target(world, p_2_1_1_1, EcsChildOf, 0) == p_2_1_1);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_1));
    test_assert(ecs_get_table(world, p_1_1_1) == ecs_get_table(world, p_2_1_1));
    test_assert(ecs_get_table(world, p_1_1_1) != ecs_get_table(world, p_1_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_1_1, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_1_1_1_1, EcsChildOf, 0) == p_1_1_1);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);
    test_assert(ecs_get_target(world, p_2_1_1, EcsChildOf, 0) == p_2_1);
    test_assert(ecs_get_target(world, p_2_1_1_1, EcsChildOf, 0) == p_2_1_1);

    ecs_fini(world);
}

void FixedHierarchies_make_fixed_3_2_lvl_w_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_entity(world, "p");
    
    ecs_entity_t p_1 = ecs_new_entity(world, "p._1");
    ecs_entity_t p_1_1 = ecs_new_entity(world, "p._1._1");
    ecs_entity_t p_1_1_1 = ecs_new_entity(world, "p._1._1._1");
    ecs_entity_t p_1_1_1_1 = ecs_new_entity(world, "p._1._1._1._1");
    ecs_entity_t p_1_1_2 = ecs_new_entity(world, "p._1._1._2");
    ecs_entity_t p_1_1_2_1 = ecs_new_entity(world, "p._1._1._2._1");

    ecs_entity_t p_2 = ecs_new_entity(world, "p._2");
    ecs_entity_t p_2_1 = ecs_new_entity(world, "p._2._1");
    ecs_entity_t p_2_1_1 = ecs_new_entity(world, "p._2._1._1");
    ecs_entity_t p_2_1_1_1 = ecs_new_entity(world, "p._2._1._1._1");
    ecs_entity_t p_2_1_2 = ecs_new_entity(world, "p._2._1._2");
    ecs_entity_t p_2_1_2_1 = ecs_new_entity(world, "p._2._1._2._1");

    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) != ecs_get_table(world, p_2_1));
    test_assert(ecs_get_table(world, p_1_1_1) != ecs_get_table(world, p_2_1_1));
    test_assert(ecs_get_table(world, p_1_1_1_1) != ecs_get_table(world, p_1_1_2_1));
    test_assert(ecs_get_table(world, p_2_1_1_1) != ecs_get_table(world, p_2_1_2_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_1_1, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_1_1_2, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_1_1_1_1, EcsChildOf, 0) == p_1_1_1);
    test_assert(ecs_get_target(world, p_1_1_2_1, EcsChildOf, 0) == p_1_1_2);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);
    test_assert(ecs_get_target(world, p_2_1_1, EcsChildOf, 0) == p_2_1);
    test_assert(ecs_get_target(world, p_2_1_2, EcsChildOf, 0) == p_2_1);
    test_assert(ecs_get_target(world, p_2_1_1_1, EcsChildOf, 0) == p_2_1_1);
    test_assert(ecs_get_target(world, p_2_1_2_1, EcsChildOf, 0) == p_2_1_2);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_1));
    test_assert(ecs_get_table(world, p_1_1_1) == ecs_get_table(world, p_2_1_1));
    test_assert(ecs_get_table(world, p_1_1_1_1) == ecs_get_table(world, p_1_1_2_1));
    test_assert(ecs_get_table(world, p_2_1_1_1) == ecs_get_table(world, p_2_1_2_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_1_1, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_1_1_2, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_1_1_1_1, EcsChildOf, 0) == p_1_1_1);
    test_assert(ecs_get_target(world, p_1_1_2_1, EcsChildOf, 0) == p_1_1_2);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);
    test_assert(ecs_get_target(world, p_2_1_1, EcsChildOf, 0) == p_2_1);
    test_assert(ecs_get_target(world, p_2_1_2, EcsChildOf, 0) == p_2_1);
    test_assert(ecs_get_target(world, p_2_1_1_1, EcsChildOf, 0) == p_2_1_1);
    test_assert(ecs_get_target(world, p_2_1_2_1, EcsChildOf, 0) == p_2_1_2);

    ecs_fini(world);
}

void FixedHierarchies_make_fixed_2_lvl_nested(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_entity(world, "p");
    
    ecs_entity_t p_1 = ecs_new_entity(world, "p._1");
    ecs_entity_t p_1_1 = ecs_new_entity(world, "p._1._1");
    ecs_entity_t p_1_1_1 = ecs_new_entity(world, "p._1._1._1");
    ecs_entity_t p_1_1_2 = ecs_new_entity(world, "p._1._1._2");
    ecs_entity_t p_1_2 = ecs_new_entity(world, "p._1._2");
    ecs_entity_t p_1_2_1 = ecs_new_entity(world, "p._1._2._1");
    ecs_entity_t p_1_2_2 = ecs_new_entity(world, "p._1._2._2");

    ecs_entity_t p_2 = ecs_new_entity(world, "p._2");
    ecs_entity_t p_2_1 = ecs_new_entity(world, "p._2._1");
    ecs_entity_t p_2_1_1 = ecs_new_entity(world, "p._2._1._1");
    ecs_entity_t p_2_1_2 = ecs_new_entity(world, "p._2._1._2");

    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) != ecs_get_table(world, p_2_1));
    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_1_2));
    test_assert(ecs_get_table(world, p_1_1_1) != ecs_get_table(world, p_2_1_1));
    test_assert(ecs_get_table(world, p_1_1_1) != ecs_get_table(world, p_1_2_1));
    test_assert(ecs_get_table(world, p_1_2_2) == ecs_get_table(world, p_1_2_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_2, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_1_1, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_1_1_2, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_1_2_1, EcsChildOf, 0) == p_1_2);
    test_assert(ecs_get_target(world, p_1_2_2, EcsChildOf, 0) == p_1_2);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);
    test_assert(ecs_get_target(world, p_2_1_1, EcsChildOf, 0) == p_2_1);
    test_assert(ecs_get_target(world, p_2_1_2, EcsChildOf, 0) == p_2_1);

    ecs_flatten(world, ecs_pair(EcsChildOf, p_1), NULL);

    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) != ecs_get_table(world, p_2_1));
    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_1_2));
    test_assert(ecs_get_table(world, p_1_1_1) != ecs_get_table(world, p_2_1_1));
    test_assert(ecs_get_table(world, p_1_1_1) == ecs_get_table(world, p_1_2_1));
    test_assert(ecs_get_table(world, p_1_2_2) == ecs_get_table(world, p_1_2_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_2, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_1_1, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_1_1_2, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_1_2_1, EcsChildOf, 0) == p_1_2);
    test_assert(ecs_get_target(world, p_1_2_2, EcsChildOf, 0) == p_1_2);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);
    test_assert(ecs_get_target(world, p_2_1_1, EcsChildOf, 0) == p_2_1);
    test_assert(ecs_get_target(world, p_2_1_2, EcsChildOf, 0) == p_2_1);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_1));
    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_1_2));
    test_assert(ecs_get_table(world, p_1_1_1) == ecs_get_table(world, p_2_1_1));
    test_assert(ecs_get_table(world, p_1_1_1) == ecs_get_table(world, p_1_2_1));
    test_assert(ecs_get_table(world, p_1_2_2) == ecs_get_table(world, p_1_2_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_2, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_1_1, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_1_1_2, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_1_2_1, EcsChildOf, 0) == p_1_2);
    test_assert(ecs_get_target(world, p_1_2_2, EcsChildOf, 0) == p_1_2);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);
    test_assert(ecs_get_target(world, p_2_1_1, EcsChildOf, 0) == p_2_1);
    test_assert(ecs_get_target(world, p_2_1_2, EcsChildOf, 0) == p_2_1);

    ecs_fini(world);
}

void FixedHierarchies_make_fixed_3_lvl_nested(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_entity(world, "p");
    
    ecs_entity_t p_1 = ecs_new_entity(world, "p._1");
    ecs_entity_t p_1_1 = ecs_new_entity(world, "p._1._1");
    ecs_entity_t p_1_1_1 = ecs_new_entity(world, "p._1._1._1");
    ecs_entity_t p_1_1_1_1 = ecs_new_entity(world, "p._1._1._1._1");
    ecs_entity_t p_1_1_2 = ecs_new_entity(world, "p._1._1._2");
    ecs_entity_t p_1_1_2_1 = ecs_new_entity(world, "p._1._1._2._1");

    ecs_entity_t p_2 = ecs_new_entity(world, "p._2");
    ecs_entity_t p_2_1 = ecs_new_entity(world, "p._2._1");
    ecs_entity_t p_2_1_1 = ecs_new_entity(world, "p._2._1._1");
    ecs_entity_t p_2_1_1_1 = ecs_new_entity(world, "p._2._1._1._1");
    ecs_entity_t p_2_1_2 = ecs_new_entity(world, "p._2._1._2");
    ecs_entity_t p_2_1_2_1 = ecs_new_entity(world, "p._2._1._2._1");

    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) != ecs_get_table(world, p_2_1));
    test_assert(ecs_get_table(world, p_1_1_1) != ecs_get_table(world, p_2_1_1));
    test_assert(ecs_get_table(world, p_1_1_1_1) != ecs_get_table(world, p_1_1_2_1));
    test_assert(ecs_get_table(world, p_2_1_1_1) != ecs_get_table(world, p_2_1_2_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_1_1, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_1_1_2, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_1_1_1_1, EcsChildOf, 0) == p_1_1_1);
    test_assert(ecs_get_target(world, p_1_1_2_1, EcsChildOf, 0) == p_1_1_2);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);
    test_assert(ecs_get_target(world, p_2_1_1, EcsChildOf, 0) == p_2_1);
    test_assert(ecs_get_target(world, p_2_1_2, EcsChildOf, 0) == p_2_1);
    test_assert(ecs_get_target(world, p_2_1_1_1, EcsChildOf, 0) == p_2_1_1);
    test_assert(ecs_get_target(world, p_2_1_2_1, EcsChildOf, 0) == p_2_1_2);

    ecs_flatten(world, ecs_pair(EcsChildOf, p_1_1), NULL);

    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) != ecs_get_table(world, p_2_1));
    test_assert(ecs_get_table(world, p_1_1_1) != ecs_get_table(world, p_2_1_1));
    test_assert(ecs_get_table(world, p_1_1_1_1) == ecs_get_table(world, p_1_1_2_1));
    test_assert(ecs_get_table(world, p_2_1_1_1) != ecs_get_table(world, p_2_1_2_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_1_1, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_1_1_2, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_1_1_1_1, EcsChildOf, 0) == p_1_1_1);
    test_assert(ecs_get_target(world, p_1_1_2_1, EcsChildOf, 0) == p_1_1_2);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);
    test_assert(ecs_get_target(world, p_2_1_1, EcsChildOf, 0) == p_2_1);
    test_assert(ecs_get_target(world, p_2_1_2, EcsChildOf, 0) == p_2_1);
    test_assert(ecs_get_target(world, p_2_1_1_1, EcsChildOf, 0) == p_2_1_1);
    test_assert(ecs_get_target(world, p_2_1_2_1, EcsChildOf, 0) == p_2_1_2);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_1));
    test_assert(ecs_get_table(world, p_1_1_1) == ecs_get_table(world, p_2_1_1));
    test_assert(ecs_get_table(world, p_1_1_1_1) == ecs_get_table(world, p_1_1_2_1));
    test_assert(ecs_get_table(world, p_2_1_1_1) == ecs_get_table(world, p_2_1_2_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_1_1, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_1_1_2, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_1_1_1_1, EcsChildOf, 0) == p_1_1_1);
    test_assert(ecs_get_target(world, p_1_1_2_1, EcsChildOf, 0) == p_1_1_2);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);
    test_assert(ecs_get_target(world, p_2_1_1, EcsChildOf, 0) == p_2_1);
    test_assert(ecs_get_target(world, p_2_1_2, EcsChildOf, 0) == p_2_1);
    test_assert(ecs_get_target(world, p_2_1_1_1, EcsChildOf, 0) == p_2_1_1);
    test_assert(ecs_get_target(world, p_2_1_2_1, EcsChildOf, 0) == p_2_1_2);

    ecs_fini(world);
}

void FixedHierarchies_make_fixed_1_lvl_after_delete(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t p_1_2 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t p_1_3 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t p_1_4 = ecs_new_w_pair(world, EcsChildOf, p_1);

    ecs_delete(world, p_1_3);
    ecs_delete(world, p_1_4);

    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_2, EcsChildOf, 0) == p_1);
    test_assert(!ecs_is_alive(world, p_1_3));
    test_assert(!ecs_is_alive(world, p_1_4));

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_2, EcsChildOf, 0) == p_1);

    ecs_fini(world);
}

void FixedHierarchies_get_target_1_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_uint(ecs_get_target(world, p, EcsChildOf, 0), 0);
    test_uint(ecs_get_target(world, p_1, EcsChildOf, 0), p);
    test_uint(ecs_get_target(world, p_2, EcsChildOf, 0), p);

    test_uint(ecs_get_target(world, p_1_1, EcsChildOf, 0), p_1);
    test_uint(ecs_get_target(world, p_2_1, EcsChildOf, 0), p_2);

    ecs_fini(world);
}

void FixedHierarchies_get_target_2_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t p_1_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1_1);

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t p_2_1_1 = ecs_new_w_pair(world, EcsChildOf, p_2_1);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_uint(ecs_get_target(world, p, EcsChildOf, 0), 0);
    test_uint(ecs_get_target(world, p_1, EcsChildOf, 0), p);
    test_uint(ecs_get_target(world, p_2, EcsChildOf, 0), p);

    test_uint(ecs_get_target(world, p_1_1, EcsChildOf, 0), p_1);
    test_uint(ecs_get_target(world, p_2_1, EcsChildOf, 0), p_2);

    test_uint(ecs_get_target(world, p_1_1_1, EcsChildOf, 0), p_1_1);
    test_uint(ecs_get_target(world, p_2_1_1, EcsChildOf, 0), p_2_1);

    ecs_fini(world);
}

void FixedHierarchies_get_depth_1_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_int(ecs_get_depth(world, p, EcsChildOf), 0);

    test_int(ecs_get_depth(world, p_1, EcsChildOf), 1);
    test_int(ecs_get_depth(world, p_2, EcsChildOf), 1);

    test_int(ecs_get_depth(world, p_1_1, EcsChildOf), 2);
    test_int(ecs_get_depth(world, p_2_1, EcsChildOf), 2);

    ecs_fini(world);
}

void FixedHierarchies_get_depth_2_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t p_1_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1_1);

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t p_2_1_1 = ecs_new_w_pair(world, EcsChildOf, p_2_1);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_int(ecs_get_depth(world, p, EcsChildOf), 0);

    test_int(ecs_get_depth(world, p_1, EcsChildOf), 1);
    test_int(ecs_get_depth(world, p_2, EcsChildOf), 1);

    test_int(ecs_get_depth(world, p_1_1, EcsChildOf), 2);
    test_int(ecs_get_depth(world, p_2_1, EcsChildOf), 2);

    test_int(ecs_get_depth(world, p_1_1_1, EcsChildOf), 3);
    test_int(ecs_get_depth(world, p_2_1_1, EcsChildOf), 3);

    ecs_fini(world);
}

void FixedHierarchies_get_depth_after_reparent_root(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_int(ecs_get_depth(world, p, EcsChildOf), 0);

    test_int(ecs_get_depth(world, p_1, EcsChildOf), 1);
    test_int(ecs_get_depth(world, p_2, EcsChildOf), 1);

    test_int(ecs_get_depth(world, p_1_1, EcsChildOf), 2);
    test_int(ecs_get_depth(world, p_2_1, EcsChildOf), 2);

    ecs_entity_t gp = ecs_new_id(world);
    ecs_add_pair(world, p, EcsChildOf, gp);

    test_int(ecs_get_depth(world, p, EcsChildOf), 1);

    test_int(ecs_get_depth(world, p_1, EcsChildOf), 2);
    test_int(ecs_get_depth(world, p_2, EcsChildOf), 2);

    test_int(ecs_get_depth(world, p_1_1, EcsChildOf), 3);
    test_int(ecs_get_depth(world, p_2_1, EcsChildOf), 3);

    ecs_fini(world);
}

void FixedHierarchies_delete_fixed_1_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    ecs_delete(world, p);

    test_assert(!ecs_is_alive(world, p_1));
    test_assert(!ecs_is_alive(world, p_1_1));
    test_assert(!ecs_is_alive(world, p_2));
    test_assert(!ecs_is_alive(world, p_2_1));

    ecs_fini(world);
}

void FixedHierarchies_delete_fixed_2_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t p_1_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1_1);

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t p_2_1_1 = ecs_new_w_pair(world, EcsChildOf, p_2_1);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    ecs_delete(world, p);

    test_assert(!ecs_is_alive(world, p_1));
    test_assert(!ecs_is_alive(world, p_1_1));
    test_assert(!ecs_is_alive(world, p_1_1_1));
    test_assert(!ecs_is_alive(world, p_2));
    test_assert(!ecs_is_alive(world, p_2_1));
    test_assert(!ecs_is_alive(world, p_2_1_1));

    ecs_fini(world);
}

void FixedHierarchies_delete_with_fixed_1_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    ecs_delete_with(world, ecs_pair(EcsChildOf, p));

    test_assert(!ecs_is_alive(world, p_1));
    test_assert(!ecs_is_alive(world, p_1_1));
    test_assert(!ecs_is_alive(world, p_2));
    test_assert(!ecs_is_alive(world, p_2_1));

    ecs_fini(world);
}

void FixedHierarchies_delete_with_fixed_2_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t p_1_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1_1);

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t p_2_1_1 = ecs_new_w_pair(world, EcsChildOf, p_2_1);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    ecs_delete_with(world, ecs_pair(EcsChildOf, p));

    test_assert(!ecs_is_alive(world, p_1));
    test_assert(!ecs_is_alive(world, p_1_1));
    test_assert(!ecs_is_alive(world, p_1_1_1));
    test_assert(!ecs_is_alive(world, p_2));
    test_assert(!ecs_is_alive(world, p_2_1));
    test_assert(!ecs_is_alive(world, p_2_1_1));

    ecs_fini(world);
}

void FixedHierarchies_query_w_parent_field_1_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_set(world, 0, Position, {1, 2});
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1_1, Position, {111, 211});

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_set(world, p_2, Position, {12, 22});
    ecs_set(world, p_2_1, Position, {121, 221});

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .src.flags = EcsParent },
        },
        .filter.instanced = true
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(2, it.count);
        test_uint(p_1, it.entities[0]);
        test_uint(p_2, it.entities[1]);
        test_uint(0, it.sources[0]);
        test_uint(p, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 11); test_int(p[0].y, 21);
        test_int(p[1].x, 12); test_int(p[1].y, 22);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 1); test_int(pp[0].y, 2);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_1_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_1, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 111); test_int(p[0].y, 211);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 11); test_int(pp[0].y, 21);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_2_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_2, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 121); test_int(p[0].y, 221);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 12); test_int(pp[0].y, 22);
    }
    test_bool(ecs_query_next(&it), false);

    ecs_fini(world);
}

void FixedHierarchies_query_w_parent_field_1_lvl_w_init(void) {
    ecs_world_t *world = ecs_init(); /* Registers metadata/ctor for TreeFixed */

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_set(world, 0, Position, {1, 2});
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1_1, Position, {111, 211});

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_set(world, p_2, Position, {12, 22});
    ecs_set(world, p_2_1, Position, {121, 221});

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .src.flags = EcsParent },
        },
        .filter.instanced = true
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(2, it.count);
        test_uint(p_1, it.entities[0]);
        test_uint(p_2, it.entities[1]);
        test_uint(0, it.sources[0]);
        test_uint(p, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 11); test_int(p[0].y, 21);
        test_int(p[1].x, 12); test_int(p[1].y, 22);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 1); test_int(pp[0].y, 2);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_1_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_1, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 111); test_int(p[0].y, 211);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 11); test_int(pp[0].y, 21);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_2_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_2, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 121); test_int(p[0].y, 221);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 12); test_int(pp[0].y, 22);
    }
    test_bool(ecs_query_next(&it), false);

    ecs_fini(world);
}

void FixedHierarchies_query_w_parent_field_1_lvl_w_init_comp_after_tree_fixed(void) {
    ecs_world_t *world = ecs_init(); /* Registers metadata/ctor for TreeFixed */

    ECS_COMPONENT(world, Position);
    ecs_set_hooks(world, Position, {
        .ctor = ecs_default_ctor
    });

    ecs_entity_t rel = ecs_new_id(world);

    ecs_entity_t p = ecs_set(world, 0, Position, {1, 2});
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1_1, Position, {111, 211});
    ecs_set_pair_second(world, p_1_1, rel, Position, {1115, 2115});

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_set(world, p_2, Position, {12, 22});
    ecs_set(world, p_2_1, Position, {121, 221});
    ecs_set_pair_second(world, p_2_1, rel, Position, {1215, 2215});

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_pair(rel, ecs_id(Position)) },
            { .id = ecs_id(Position), .src.flags = EcsParent },
        },
        .filter.instanced = true
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_1_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(0, it.sources[1]);
        test_uint(p_1, it.sources[2]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 111); test_int(p[0].y, 211);

        Position *rp = ecs_field(&it, Position, 2);
        test_int(rp[0].x, 1115); test_int(rp[0].y, 2115);

        Position *pp = ecs_field(&it, Position, 3);
        test_int(pp[0].x, 11); test_int(pp[0].y, 21);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_2_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(0, it.sources[1]);
        test_uint(p_2, it.sources[2]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 121); test_int(p[0].y, 221);

        Position *rp = ecs_field(&it, Position, 2);
        test_int(rp[0].x, 1215); test_int(rp[0].y, 2215);

        Position *pp = ecs_field(&it, Position, 3);
        test_int(pp[0].x, 12); test_int(pp[0].y, 22);
    }
    test_bool(ecs_query_next(&it), false);

    ecs_fini(world);
}

void FixedHierarchies_query_w_parent_field_2_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_set(world, 0, Position, {1, 2});

    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t p_1_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1_1);
    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1_1, Position, {111, 211});
    ecs_set(world, p_1_1_1, Position, {1111, 2111});

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t p_2_1_1 = ecs_new_w_pair(world, EcsChildOf, p_2_1);
    ecs_set(world, p_2, Position, {12, 22});
    ecs_set(world, p_2_1, Position, {121, 221});
    ecs_set(world, p_2_1_1, Position, {1211, 2211});

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .src.flags = EcsParent },
        },
        .filter.instanced = true
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(2, it.count);
        test_uint(p_1, it.entities[0]);
        test_uint(p_2, it.entities[1]);
        test_uint(0, it.sources[0]);
        test_uint(p, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 11); test_int(p[0].y, 21);
        test_int(p[1].x, 12); test_int(p[1].y, 22);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 1); test_int(pp[0].y, 2);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_1_1_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_1_1, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 1111); test_int(p[0].y, 2111);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 111); test_int(pp[0].y, 211);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_2_1_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_2_1, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 1211); test_int(p[0].y, 2211);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 121); test_int(pp[0].y, 221);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_1_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_1, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 111); test_int(p[0].y, 211);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 11); test_int(pp[0].y, 21);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_2_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_2, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 121); test_int(p[0].y, 221);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 12); test_int(pp[0].y, 22);
    }
    test_bool(ecs_query_next(&it), false);

    ecs_fini(world);
}

void FixedHierarchies_query_w_cascade_field_2_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_set(world, 0, Position, {1, 2});

    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t p_1_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1_1);
    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1_1, Position, {111, 211});
    ecs_set(world, p_1_1_1, Position, {1111, 2111});

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t p_2_1_1 = ecs_new_w_pair(world, EcsChildOf, p_2_1);
    ecs_set(world, p_2, Position, {12, 22});
    ecs_set(world, p_2_1, Position, {121, 221});
    ecs_set(world, p_2_1_1, Position, {1211, 2211});

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .src.flags = EcsParent|EcsCascade },
        },
        .filter.instanced = true
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(2, it.count);
        test_uint(p_1, it.entities[0]);
        test_uint(p_2, it.entities[1]);
        test_uint(0, it.sources[0]);
        test_uint(p, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 11); test_int(p[0].y, 21);
        test_int(p[1].x, 12); test_int(p[1].y, 22);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 1); test_int(pp[0].y, 2);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_1_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_1, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 111); test_int(p[0].y, 211);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 11); test_int(pp[0].y, 21);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_2_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_2, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 121); test_int(p[0].y, 221);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 12); test_int(pp[0].y, 22);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_1_1_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_1_1, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 1111); test_int(p[0].y, 2111);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 111); test_int(pp[0].y, 211);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_2_1_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_2_1, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 1211); test_int(p[0].y, 2211);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 121); test_int(pp[0].y, 221);
    }
    test_bool(ecs_query_next(&it), false);

    ecs_fini(world);
}

void FixedHierarchies_query_w_parent_field_1_fixed_1_regular(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t p = ecs_set(world, 0, Position, {1, 2});
    ecs_set(world, p, Velocity, {3, 4});
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1_1, Position, {111, 211});

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_set(world, p_2, Position, {12, 22});
    ecs_set(world, p_2_1, Position, {121, 221});

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .src.flags = EcsParent },
            { .id = ecs_id(Velocity), .src.flags = EcsParent }
        },
        .filter.instanced = true
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(2, it.count);
        test_uint(p_1, it.entities[0]);
        test_uint(p_2, it.entities[1]);
        test_uint(0, it.sources[0]);
        test_uint(p, it.sources[1]);
        test_uint(p, it.sources[2]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 11); test_int(p[0].y, 21);
        test_int(p[1].x, 12); test_int(p[1].y, 22);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 1); test_int(pp[0].y, 2);
        Velocity *vp = ecs_field(&it, Velocity, 3);
        test_int(vp[0].x, 3); test_int(vp[0].y, 4);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_1_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_1, it.sources[1]);
        test_uint(p, it.sources[2]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 111); test_int(p[0].y, 211);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 11); test_int(pp[0].y, 21);
        Velocity *vp = ecs_field(&it, Velocity, 3);
        test_int(vp[0].x, 3); test_int(vp[0].y, 4);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_2_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_2, it.sources[1]);
        test_uint(p, it.sources[2]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 121); test_int(p[0].y, 221);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 12); test_int(pp[0].y, 22);
        Velocity *vp = ecs_field(&it, Velocity, 3);
        test_int(vp[0].x, 3); test_int(vp[0].y, 4);
    }
    test_bool(ecs_query_next(&it), false);

    ecs_fini(world);
}

void FixedHierarchies_query_w_parent_field_only_fixed_1_lvls(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1_1, Position, {111, 211});

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_set(world, p_2, Position, {12, 22});
    ecs_set(world, p_2_1, Position, {121, 221});

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .src.flags = EcsParent },
        },
        .filter.instanced = true
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_1_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_1, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 111); test_int(p[0].y, 211);

        Position *pp = ecs_field(&it, Position, 2);
        test_assert(pp != NULL);
        test_int(pp[0].x, 11); test_int(pp[0].y, 21);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_2_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_2, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 121); test_int(p[0].y, 221);

        Position *pp = ecs_field(&it, Position, 2);
        test_assert(pp != NULL);
        test_int(pp[0].x, 12); test_int(pp[0].y, 22);
    }
    test_bool(ecs_query_next(&it), false);

    ecs_fini(world);
}

void FixedHierarchies_query_w_parent_field_fixed_1_lvls_no_match(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_set(world, p_1_1, Position, {111, 211});

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .src.flags = EcsParent },
        },
        .filter.instanced = true
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), false);

    it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), false);

    ecs_fini(world);
}

void FixedHierarchies_query_w_parent_field_fixed_1_lvls_2_no_match(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_set(world, p_1_1, Position, {111, 211});

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_set(world, p_2_1, Position, {121, 221});

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_1));

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .src.flags = EcsParent },
        },
        .filter.instanced = true
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), false);

    ecs_fini(world);
}

void FixedHierarchies_query_w_parent_field_fixed_1_lvls_no_match_match(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_set(world, p_1_1, Position, {111, 211});

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_set(world, p_2, Position, {12, 22});
    ecs_set(world, p_2_1, Position, {121, 221});

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_1));

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .src.flags = EcsParent },
        },
        .filter.instanced = true
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_2_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_2, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 121); test_int(p[0].y, 221);

        Position *pp = ecs_field(&it, Position, 2);
        test_assert(pp != NULL);
        test_int(pp[0].x, 12); test_int(pp[0].y, 22);
    }

    test_bool(ecs_query_next(&it), false);

    ecs_fini(world);
}

void FixedHierarchies_query_w_parent_field_fixed_1_lvls_match_no_match(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1_1, Position, {111, 211});

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_set(world, p_2_1, Position, {121, 221});

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_1));

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .src.flags = EcsParent },
        },
        .filter.instanced = true
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_1_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_1, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_assert(p != NULL);
        test_int(p[0].x, 111); test_int(p[0].y, 211);

        Position *pp = ecs_field(&it, Position, 2);
        test_assert(pp != NULL);
        test_int(pp[0].x, 11); test_int(pp[0].y, 21);
    }
    test_bool(ecs_query_next(&it), false);

    ecs_fini(world);
}

void FixedHierarchies_query_w_parent_field_2_fixed_2_lvls(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t p = ecs_set(world, 0, Position, {1, 2});
    ecs_set(world, p, Velocity, {3, 4});
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t p_1_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1_1);
    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1, Mass, {5});
    ecs_set(world, p_1_1, Position, {111, 211});
    ecs_set(world, p_1_1_1, Position, {1111, 2111});

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t p_2_1_1 = ecs_new_w_pair(world, EcsChildOf, p_2_1);
    ecs_set(world, p_2, Position, {12, 22});
    ecs_set(world, p_2, Mass, {6});
    ecs_set(world, p_2_1, Position, {121, 221});
    ecs_set(world, p_2_1_1, Position, {1211, 2211});

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .src.flags = EcsParent },
            { .id = ecs_id(Velocity), .src.flags = EcsParent },
            { .id = ecs_id(Mass), .src.flags = EcsSelf|EcsParent }
        },
        .filter.instanced = true
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(2, it.count);
        test_uint(p_1, it.entities[0]);
        test_uint(p_2, it.entities[1]);
        test_uint(0, it.sources[0]);
        test_uint(p, it.sources[1]);
        test_uint(p, it.sources[2]);
        test_uint(0, it.sources[3]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 11); test_int(p[0].y, 21);
        test_int(p[1].x, 12); test_int(p[1].y, 22);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 1); test_int(pp[0].y, 2);
        Velocity *vp = ecs_field(&it, Velocity, 3);
        test_int(vp[0].x, 3); test_int(vp[0].y, 4);
        Mass *mp = ecs_field(&it, Mass, 4);
        test_int(mp[0], 5);
        test_int(mp[1], 6);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_1_1_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_1_1, it.sources[1]);
        test_uint(p, it.sources[2]);
        test_uint(p_1, it.sources[3]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 1111); test_int(p[0].y, 2111);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 111); test_int(pp[0].y, 211);
        Velocity *vp = ecs_field(&it, Velocity, 3);
        test_int(vp[0].x, 3); test_int(vp[0].y, 4);
        Mass *mp = ecs_field(&it, Mass, 4);
        test_int(mp[0], 5);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_2_1_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_2_1, it.sources[1]);
        test_uint(p, it.sources[2]);
        test_uint(p_2, it.sources[3]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 1211); test_int(p[0].y, 2211);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 121); test_int(pp[0].y, 221);
        Velocity *vp = ecs_field(&it, Velocity, 3);
        test_int(vp[0].x, 3); test_int(vp[0].y, 4);
        Mass *mp = ecs_field(&it, Mass, 4);
        test_int(mp[0], 6);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_1_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_1, it.sources[1]);
        test_uint(p, it.sources[2]);
        test_uint(p_1, it.sources[3]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 111); test_int(p[0].y, 211);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 11); test_int(pp[0].y, 21);
        Velocity *vp = ecs_field(&it, Velocity, 3);
        test_int(vp[0].x, 3); test_int(vp[0].y, 4);
        Mass *mp = ecs_field(&it, Mass, 4);
        test_int(mp[0], 5);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_2_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_2, it.sources[1]);
        test_uint(p, it.sources[2]);
        test_uint(p_2, it.sources[3]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 121); test_int(p[0].y, 221);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 12); test_int(pp[0].y, 22);
        Velocity *vp = ecs_field(&it, Velocity, 3);
        test_int(vp[0].x, 3); test_int(vp[0].y, 4);
        Mass *mp = ecs_field(&it, Mass, 4);
        test_int(mp[0], 6);
    }
    test_bool(ecs_query_next(&it), false);

    ecs_fini(world);
}

void FixedHierarchies_query_next_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_set(world, 0, Position, {1, 2});
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1_1, Position, {111, 211});

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_set(world, p_2, Position, {12, 22});
    ecs_set(world, p_2_1, Position, {121, 221});

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .src.flags = EcsParent },
        },
        .filter.instanced = true
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    {
        test_bool(ecs_query_next_table(&it), true);
        test_assert(ecs_query_populate(&it, false) == EcsIterNextYield);
        test_uint(2, it.count);
        test_uint(p_1, it.entities[0]);
        test_uint(p_2, it.entities[1]);
        test_uint(0, it.sources[0]);
        test_uint(p, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 11); test_int(p[0].y, 21);
        test_int(p[1].x, 12); test_int(p[1].y, 22);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 1); test_int(pp[0].y, 2);
    }
    {
        test_bool(ecs_query_next_table(&it), true);
        test_assert(ecs_query_populate(&it, false) == EcsIterYield);
        test_uint(1, it.count);
        test_uint(p_1_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_1, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 111); test_int(p[0].y, 211);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 11); test_int(pp[0].y, 21);
    }
    {
        test_assert(ecs_query_populate(&it, false) == EcsIterNextYield);
        test_uint(1, it.count);
        test_uint(p_2_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_2, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 121); test_int(p[0].y, 221);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 12); test_int(pp[0].y, 22);
    }
    test_bool(ecs_query_next_table(&it), false);

    ecs_fini(world);
}

void FixedHierarchies_query_next_table_1_elem(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_set(world, 0, Position, {1, 2});
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1_1, Position, {111, 211});

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .src.flags = EcsParent },
        },
        .filter.instanced = true
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    {
        test_bool(ecs_query_next_table(&it), true);
        test_assert(ecs_query_populate(&it, false) == EcsIterNextYield);
        test_uint(1, it.count);
        test_uint(p_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 11); test_int(p[0].y, 21);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 1); test_int(pp[0].y, 2);
    }
    {
        test_bool(ecs_query_next_table(&it), true);
        test_assert(ecs_query_populate(&it, false) == EcsIterNextYield);
        test_uint(1, it.count);
        test_uint(p_1_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_1, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 111); test_int(p[0].y, 211);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 11); test_int(pp[0].y, 21);
    }
    test_bool(ecs_query_next_table(&it), false);

    ecs_fini(world);
}

void FixedHierarchies_query_next_table_1_elem_no_match(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_set(world, p_1_1, Position, {111, 211});

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .src.flags = EcsParent },
        },
        .filter.instanced = true
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    {
        test_bool(ecs_query_next_table(&it), true);
        test_assert(ecs_query_populate(&it, false) == EcsIterNext);
        test_uint(0, it.count);
    }
    test_bool(ecs_query_next_table(&it), false);

    ecs_fini(world);
}

void FixedHierarchies_query_nested_make_fixed(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .src.flags = EcsParent|EcsCascade },
        },
        .filter.instanced = true
    });

    ecs_entity_t p = ecs_new_entity(world, "p");
    ecs_entity_t p_1 = ecs_new_entity(world, "p._1");
    ecs_entity_t p_1_1 = ecs_new_entity(world, "p._1._1");
    ecs_entity_t p_1_2 = ecs_new_entity(world, "p._1._2");

    ecs_set(world, p, Position, {1, 2});
    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1_1, Position, {111, 211});
    ecs_set(world, p_1_2, Position, {112, 212});

    ecs_flatten(world, ecs_pair(EcsChildOf, p_1), NULL);
    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    {
        test_bool(ecs_query_next_table(&it), true);
        test_assert(ecs_query_populate(&it, false) == EcsIterNextYield);
        test_uint(1, it.count);
        test_uint(p_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 11); test_int(p[0].y, 21);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 1); test_int(pp[0].y, 2);
    }
    {
        test_bool(ecs_query_next_table(&it), true);
        test_assert(ecs_query_populate(&it, false) == EcsIterNextYield);
        test_uint(2, it.count);
        test_uint(p_1_1, it.entities[0]);
        test_uint(p_1_2, it.entities[1]);
        test_uint(0, it.sources[0]);
        test_uint(p_1, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 111); test_int(p[0].y, 211);
        test_int(p[1].x, 112); test_int(p[1].y, 212);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 11); test_int(pp[0].y, 21);
    }
    test_bool(ecs_query_next_table(&it), false);

    ecs_fini(world);
}

void FixedHierarchies_query_nested_make_fixed_w_optional(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .oper = EcsOptional, .src.flags = EcsParent|EcsCascade },
        },
        .filter.instanced = true
    });

    ecs_entity_t p = ecs_new_entity(world, "p");
    ecs_entity_t p_1 = ecs_new_entity(world, "p._1");
    ecs_entity_t p_1_1 = ecs_new_entity(world, "p._1._1");
    ecs_entity_t p_1_2 = ecs_new_entity(world, "p._1._2");

    ecs_set(world, p, Position, {1, 2});
    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1_1, Position, {111, 211});
    ecs_set(world, p_1_2, Position, {112, 212});

    ecs_flatten(world, ecs_pair(EcsChildOf, p_1), NULL);
    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(0, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 1); test_int(p[0].y, 2);

        Position *pp = ecs_field(&it, Position, 2);
        test_assert(pp == NULL);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 11); test_int(p[0].y, 21);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 1); test_int(pp[0].y, 2);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(2, it.count);
        test_uint(p_1_1, it.entities[0]);
        test_uint(p_1_2, it.entities[1]);
        test_uint(0, it.sources[0]);
        test_uint(p_1, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 111); test_int(p[0].y, 211);
        test_int(p[1].x, 112); test_int(p[1].y, 212);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 11); test_int(pp[0].y, 21);
    }
    test_bool(ecs_query_next(&it), false);

    ecs_fini(world);
}

void FixedHierarchies_query_nested_make_fixed_w_optional_match_children_only(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .oper = EcsOptional, .src.flags = EcsParent|EcsCascade },
        },
        .filter.instanced = true
    });

    ecs_entity_t p = ecs_new_entity(world, "p");
    ecs_entity_t p_1 = ecs_new_entity(world, "p._1");
    ecs_entity_t p_1_1 = ecs_new_entity(world, "p._1._1");
    ecs_entity_t p_1_2 = ecs_new_entity(world, "p._1._2");

    ecs_set(world, p_1_1, Position, {111, 211});
    ecs_set(world, p_1_2, Position, {112, 212});

    ecs_flatten(world, ecs_pair(EcsChildOf, p_1), NULL);
    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(2, it.count);
        test_uint(p_1_1, it.entities[0]);
        test_uint(p_1_2, it.entities[1]);
        test_uint(0, it.sources[0]);
        test_uint(0, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 111); test_int(p[0].y, 211);
        test_int(p[1].x, 112); test_int(p[1].y, 212);

        Position *pp = ecs_field(&it, Position, 2);
        test_assert(pp == NULL);
    }
    test_bool(ecs_query_next(&it), false);

    ecs_fini(world);
}

void FixedHierarchies_query_nested_w_2_parents_make_fixed(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .oper = EcsOptional, .src.flags = EcsParent|EcsCascade },
        },
        .filter.instanced = true
    });

    ecs_entity_t p = ecs_new_entity(world, "p");
    ecs_entity_t p_1 = ecs_new_entity(world, "p._1");
    ecs_entity_t p_1_1 = ecs_new_entity(world, "p._1._1");
    ecs_entity_t p_1_1_1 = ecs_new_entity(world, "p._1._1._1");
    ecs_entity_t p_1_2 = ecs_new_entity(world, "p._1._2");
    ecs_entity_t p_1_2_1 = ecs_new_entity(world, "p._1._2._1");
    ecs_entity_t p_1_2_2 = ecs_new_entity(world, "p._1._2._2");

    ecs_set(world, p, Position, {1, 2});
    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1_1, Position, {111, 211});
    ecs_set(world, p_1_1_1, Position, {1111, 2111});
    ecs_set(world, p_1_2, Position, {112, 212});
    ecs_set(world, p_1_2_1, Position, {1121, 2121});
    ecs_set(world, p_1_2_2, Position, {1122, 2122});

    ecs_flatten(world, ecs_pair(EcsChildOf, p_1), NULL);

    test_assert(ecs_get_table(world, p_1_1_1) == ecs_get_table(world, p_1_2_1));
    test_assert(ecs_get_table(world, p_1_1_1) == ecs_get_table(world, p_1_2_2));

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1_1_1) == ecs_get_table(world, p_1_2_1));
    test_assert(ecs_get_table(world, p_1_1_1) == ecs_get_table(world, p_1_2_2));

    ecs_iter_t it = ecs_query_iter(world, q);
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(0, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 1); test_int(p[0].y, 2);

        Position *pp = ecs_field(&it, Position, 2);
        test_assert(pp == NULL);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 11); test_int(p[0].y, 21);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 1); test_int(pp[0].y, 2);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(2, it.count);
        test_uint(p_1_1, it.entities[0]);
        test_uint(p_1_2, it.entities[1]);
        test_uint(0, it.sources[0]);
        test_uint(p_1, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 111); test_int(p[0].y, 211);
        test_int(p[1].x, 112); test_int(p[1].y, 212);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 11); test_int(pp[0].y, 21);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_1_1_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_1_1, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 1111); test_int(p[0].y, 2111);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 111); test_int(pp[0].y, 211);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(2, it.count);
        test_uint(p_1_2_1, it.entities[0]);
        test_uint(p_1_2_2, it.entities[1]);
        test_uint(0, it.sources[0]);
        test_uint(p_1_2, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 1121); test_int(p[0].y, 2121);
        test_int(p[1].x, 1122); test_int(p[1].y, 2122);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 112); test_int(pp[0].y, 212);
    }

    test_bool(ecs_query_next(&it), false);

    ecs_fini(world);
}

void FixedHierarchies_query_table_w_3_parents(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .oper = EcsOptional, .src.flags = EcsParent|EcsCascade },
        },
        .filter.instanced = true
    });

    ecs_entity_t p = ecs_new_entity(world, "p");
    ecs_entity_t p_1 = ecs_new_entity(world, "p._1");
    ecs_entity_t p_1_1 = ecs_new_entity(world, "p._1._1");
    ecs_entity_t p_1_1_1 = ecs_new_entity(world, "p._1._1._1");
    ecs_entity_t p_1_1_2 = ecs_new_entity(world, "p._1._1._2");
    ecs_entity_t p_1_1_3 = ecs_new_entity(world, "p._1._1._3");
    ecs_entity_t p_1_2 = ecs_new_entity(world, "p._1._2");
    ecs_entity_t p_1_2_1 = ecs_new_entity(world, "p._1._2._1");
    ecs_entity_t p_1_2_2 = ecs_new_entity(world, "p._1._2._2");
    ecs_entity_t p_1_3 = ecs_new_entity(world, "p._1._3");
    ecs_entity_t p_1_3_1 = ecs_new_entity(world, "p._1._3._1");

    ecs_set(world, p, Position, {1, 2});
    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1_1, Position, {111, 211});
    ecs_set(world, p_1_1_1, Position, {1111, 2111});
    ecs_set(world, p_1_1_2, Position, {1112, 2112});
    ecs_set(world, p_1_1_3, Position, {1113, 2113});
    ecs_set(world, p_1_2, Position, {112, 212});
    ecs_set(world, p_1_2_1, Position, {1121, 2121});
    ecs_set(world, p_1_2_2, Position, {1122, 2122});
    ecs_set(world, p_1_3, Position, {113, 213});
    ecs_set(world, p_1_3_1, Position, {1131, 2131});

    ecs_flatten(world, ecs_pair(EcsChildOf, p_1), NULL);

    test_assert(ecs_get_table(world, p_1_1_1) == ecs_get_table(world, p_1_2_1));
    test_assert(ecs_get_table(world, p_1_1_1) == ecs_get_table(world, p_1_2_2));
    test_assert(ecs_get_table(world, p_1_1_1) == ecs_get_table(world, p_1_3_1));

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1_1_1) == ecs_get_table(world, p_1_2_1));
    test_assert(ecs_get_table(world, p_1_1_1) == ecs_get_table(world, p_1_2_2));
    test_assert(ecs_get_table(world, p_1_1_1) == ecs_get_table(world, p_1_3_1));

    ecs_iter_t it = ecs_query_iter(world, q);
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(0, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 1); test_int(p[0].y, 2);

        Position *pp = ecs_field(&it, Position, 2);
        test_assert(pp == NULL);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 11); test_int(p[0].y, 21);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 1); test_int(pp[0].y, 2);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(3, it.count);
        test_uint(p_1_1, it.entities[0]);
        test_uint(p_1_2, it.entities[1]);
        test_uint(p_1_3, it.entities[2]);
        test_uint(0, it.sources[0]);
        test_uint(p_1, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 111); test_int(p[0].y, 211);
        test_int(p[1].x, 112); test_int(p[1].y, 212);
        test_int(p[2].x, 113); test_int(p[2].y, 213);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 11); test_int(pp[0].y, 21);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(3, it.count);
        test_uint(p_1_1_1, it.entities[0]);
        test_uint(p_1_1_2, it.entities[1]);
        test_uint(p_1_1_3, it.entities[2]);
        test_uint(0, it.sources[0]);
        test_uint(p_1_1, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 1111); test_int(p[0].y, 2111);
        test_int(p[1].x, 1112); test_int(p[1].y, 2112);
        test_int(p[2].x, 1113); test_int(p[2].y, 2113);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 111); test_int(pp[0].y, 211);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(2, it.count);
        test_uint(p_1_2_1, it.entities[0]);
        test_uint(p_1_2_2, it.entities[1]);
        test_uint(0, it.sources[0]);
        test_uint(p_1_2, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 1121); test_int(p[0].y, 2121);
        test_int(p[1].x, 1122); test_int(p[1].y, 2122);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 112); test_int(pp[0].y, 212);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_1_3_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_1_3, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 1131); test_int(p[0].y, 2131);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 113); test_int(pp[0].y, 213);
    }

    test_bool(ecs_query_next(&it), false);

    ecs_fini(world);
}

void FixedHierarchies_query_w_parent_change_detection_1st(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {{ 
                .id = ecs_id(Position), 
                .inout = EcsIn
            },
            { 
                .id = ecs_id(Position), 
                .src.flags = EcsParent|EcsCascade,
                .inout = EcsIn
            },
        },
        .filter.instanced = true
    });

    ecs_entity_t p = ecs_new_entity(world, "p");
    ecs_entity_t p_1 = ecs_new_entity(world, "p._1");
    ecs_entity_t p_1_1 = ecs_new_entity(world, "p._1._1");
    ecs_entity_t p_2 = ecs_new_entity(world, "p._2");
    ecs_entity_t p_2_1 = ecs_new_entity(world, "p._2._1");

    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1_1, Position, {111, 211});
    ecs_set(world, p_2, Position, {12, 22});
    ecs_set(world, p_2_1, Position, {121, 221});
    
    // offset the column of Position to verify change detection is testing
    // againt the right one.
    ecs_add(world, p_2, Velocity); 

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_1));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        {
            test_bool(ecs_query_next_table(&it), true);
            test_assert(ecs_query_populate(&it, false) == EcsIterYield);
            test_bool(ecs_query_changed(NULL, &it), true);
            test_uint(1, it.count);
            test_uint(p_1_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_1, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 111); test_int(p[0].y, 211);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 11); test_int(pp->y, 21);
        }
        {
            test_assert(ecs_query_populate(&it, false) == EcsIterNextYield);
            test_bool(ecs_query_changed(NULL, &it), true);
            test_uint(1, it.count);
            test_uint(p_2_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_2, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 121); test_int(p[0].y, 221);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 12); test_int(pp->y, 22);
        }
        test_bool(ecs_query_next_table(&it), false);
    }

    ecs_set(world, p_1, Position, {22, 42});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        {
            test_bool(ecs_query_next_table(&it), true);
            test_assert(ecs_query_populate(&it, false) == EcsIterYield);
            test_bool(ecs_query_changed(NULL, &it), true);
            test_uint(1, it.count);
            test_uint(p_1_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_1, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 111); test_int(p[0].y, 211);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 22); test_int(pp->y, 42);
        }
        {
            test_assert(ecs_query_populate(&it, false) == EcsIterNextYield);
            test_bool(ecs_query_changed(NULL, &it), false);
            test_uint(1, it.count);
            test_uint(p_2_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_2, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 121); test_int(p[0].y, 221);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 12); test_int(pp->y, 22);
        }
        test_bool(ecs_query_next_table(&it), false);
    }

    ecs_fini(world);
}

void FixedHierarchies_query_w_parent_change_detection_2nd(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {{ 
                .id = ecs_id(Position), 
                .inout = EcsIn
            },
            { 
                .id = ecs_id(Position), 
                .src.flags = EcsParent|EcsCascade,
                .inout = EcsIn
            },
        },
        .filter.instanced = true
    });

    ecs_entity_t p = ecs_new_entity(world, "p");
    ecs_entity_t p_1 = ecs_new_entity(world, "p._1");
    ecs_entity_t p_1_1 = ecs_new_entity(world, "p._1._1");
    ecs_entity_t p_2 = ecs_new_entity(world, "p._2");
    ecs_entity_t p_2_1 = ecs_new_entity(world, "p._2._1");

    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1_1, Position, {111, 211});
    ecs_set(world, p_2, Position, {12, 22});
    ecs_set(world, p_2_1, Position, {121, 221});
    
    // offset the column of Position to verify change detection is testing
    // againt the right one.
    ecs_add(world, p_2, Velocity); 

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_1));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        {
            test_bool(ecs_query_next_table(&it), true);
            test_assert(ecs_query_populate(&it, false) == EcsIterYield);
            test_bool(ecs_query_changed(NULL, &it), true);
            test_uint(1, it.count);
            test_uint(p_1_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_1, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 111); test_int(p[0].y, 211);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 11); test_int(pp->y, 21);
        }
        {
            test_assert(ecs_query_populate(&it, false) == EcsIterNextYield);
            test_bool(ecs_query_changed(NULL, &it), true);
            test_uint(1, it.count);
            test_uint(p_2_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_2, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 121); test_int(p[0].y, 221);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 12); test_int(pp->y, 22);
        }
        test_bool(ecs_query_next_table(&it), false);
    }

    ecs_set(world, p_2, Position, {24, 44});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        {
            test_bool(ecs_query_next_table(&it), true);
            test_assert(ecs_query_populate(&it, false) == EcsIterYield);
            test_bool(ecs_query_changed(NULL, &it), false);
            test_uint(1, it.count);
            test_uint(p_1_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_1, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 111); test_int(p[0].y, 211);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 11); test_int(pp->y, 21);
        }
        {
            test_assert(ecs_query_populate(&it, false) == EcsIterNextYield);
            test_bool(ecs_query_changed(NULL, &it), true);
            test_uint(1, it.count);
            test_uint(p_2_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_2, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 121); test_int(p[0].y, 221);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 24); test_int(pp->y, 44);
        }
        test_bool(ecs_query_next_table(&it), false);
    }

    ecs_fini(world);
}

void FixedHierarchies_query_w_parent_change_detection_iter_twice(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {{ 
                .id = ecs_id(Position), 
                .inout = EcsIn
            },
            { 
                .id = ecs_id(Position), 
                .src.flags = EcsParent|EcsCascade,
                .inout = EcsIn
            },
        },
        .filter.instanced = true
    });

    ecs_entity_t p = ecs_new_entity(world, "p");
    ecs_entity_t p_1 = ecs_new_entity(world, "p._1");
    ecs_entity_t p_1_1 = ecs_new_entity(world, "p._1._1");
    ecs_entity_t p_2 = ecs_new_entity(world, "p._2");
    ecs_entity_t p_2_1 = ecs_new_entity(world, "p._2._1");

    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1_1, Position, {111, 211});
    ecs_set(world, p_2, Position, {12, 22});
    ecs_set(world, p_2_1, Position, {121, 221});
    
    // offset the column of Position to verify change detection is testing
    // againt the right one.
    ecs_add(world, p_2, Velocity); 

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_1));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        {
            test_bool(ecs_query_next_table(&it), true);
            test_assert(ecs_query_populate(&it, false) == EcsIterYield);
            test_bool(ecs_query_changed(NULL, &it), true);
            test_uint(1, it.count);
            test_uint(p_1_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_1, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 111); test_int(p[0].y, 211);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 11); test_int(pp->y, 21);
        }
        {
            test_assert(ecs_query_populate(&it, false) == EcsIterNextYield);
            test_bool(ecs_query_changed(NULL, &it), true);
            test_uint(1, it.count);
            test_uint(p_2_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_2, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 121); test_int(p[0].y, 221);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 12); test_int(pp->y, 22);
        }
        test_bool(ecs_query_next_table(&it), false);
    }

    ecs_set(world, p_1, Position, {22, 42});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        {
            test_bool(ecs_query_next_table(&it), true);
            test_assert(ecs_query_populate(&it, false) == EcsIterYield);
            test_bool(ecs_query_changed(NULL, &it), true);
            test_uint(1, it.count);
            test_uint(p_1_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_1, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 111); test_int(p[0].y, 211);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 22); test_int(pp->y, 42);
        }
        {
            test_assert(ecs_query_populate(&it, false) == EcsIterNextYield);
            test_bool(ecs_query_changed(NULL, &it), false);
            test_uint(1, it.count);
            test_uint(p_2_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_2, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 121); test_int(p[0].y, 221);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 12); test_int(pp->y, 22);
        }
        test_bool(ecs_query_next_table(&it), false);
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        {
            test_bool(ecs_query_next_table(&it), true);
            test_assert(ecs_query_populate(&it, false) == EcsIterYield);
            test_bool(ecs_query_changed(NULL, &it), false);
            test_uint(1, it.count);
            test_uint(p_1_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_1, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 111); test_int(p[0].y, 211);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 22); test_int(pp->y, 42);
        }
        {
            test_assert(ecs_query_populate(&it, false) == EcsIterNextYield);
            test_bool(ecs_query_changed(NULL, &it), false);
            test_uint(1, it.count);
            test_uint(p_2_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_2, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 121); test_int(p[0].y, 221);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 12); test_int(pp->y, 22);
        }
        test_bool(ecs_query_next_table(&it), false);
    }

    ecs_fini(world);
}

void FixedHierarchies_query_w_parent_change_detection_iter_twice_each_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {{ 
                .id = ecs_id(Position), 
                .inout = EcsIn
            },
            { 
                .id = ecs_id(Position), 
                .src.flags = EcsParent|EcsCascade,
                .inout = EcsIn
            },
        },
        .filter.instanced = true
    });

    ecs_entity_t p = ecs_new_entity(world, "p");
    ecs_entity_t p_1 = ecs_new_entity(world, "p._1");
    ecs_entity_t p_1_1 = ecs_new_entity(world, "p._1._1");
    ecs_entity_t p_2 = ecs_new_entity(world, "p._2");
    ecs_entity_t p_2_1 = ecs_new_entity(world, "p._2._1");

    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1_1, Position, {111, 211});
    ecs_set(world, p_2, Position, {12, 22});
    ecs_set(world, p_2_1, Position, {121, 221});
    
    // offset the column of Position to verify change detection is testing
    // againt the right one.
    ecs_add(world, p_2, Velocity); 

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_1));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        {
            test_bool(ecs_query_next_table(&it), true);
            test_assert(ecs_query_populate(&it, false) == EcsIterYield);
            test_bool(ecs_query_changed(NULL, &it), true);
            test_uint(1, it.count);
            test_uint(p_1_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_1, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 111); test_int(p[0].y, 211);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 11); test_int(pp->y, 21);
        }
        {
            test_assert(ecs_query_populate(&it, false) == EcsIterNextYield);
            test_bool(ecs_query_changed(NULL, &it), true);
            test_uint(1, it.count);
            test_uint(p_2_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_2, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 121); test_int(p[0].y, 221);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 12); test_int(pp->y, 22);
        }
        test_bool(ecs_query_next_table(&it), false);
    }

    ecs_set(world, p_1, Position, {22, 42});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        {
            test_bool(ecs_query_next_table(&it), true);
            test_assert(ecs_query_populate(&it, false) == EcsIterYield);
            test_bool(ecs_query_changed(NULL, &it), true);
            test_uint(1, it.count);
            test_uint(p_1_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_1, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 111); test_int(p[0].y, 211);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 22); test_int(pp->y, 42);
        }
        {
            test_assert(ecs_query_populate(&it, false) == EcsIterNextYield);
            test_bool(ecs_query_changed(NULL, &it), false);
            test_uint(1, it.count);
            test_uint(p_2_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_2, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 121); test_int(p[0].y, 221);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 12); test_int(pp->y, 22);
        }
        test_bool(ecs_query_next_table(&it), false);
    }

    ecs_set(world, p_2, Position, {24, 44});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        {
            test_bool(ecs_query_next_table(&it), true);
            test_assert(ecs_query_populate(&it, false) == EcsIterYield);
            test_bool(ecs_query_changed(NULL, &it), false);
            test_uint(1, it.count);
            test_uint(p_1_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_1, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 111); test_int(p[0].y, 211);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 22); test_int(pp->y, 42);
        }
        {
            test_assert(ecs_query_populate(&it, false) == EcsIterNextYield);
            test_bool(ecs_query_changed(NULL, &it), true);
            test_uint(1, it.count);
            test_uint(p_2_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_2, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 121); test_int(p[0].y, 221);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 24); test_int(pp->y, 44);
        }
        test_bool(ecs_query_next_table(&it), false);
    }

    ecs_fini(world);
}

void FixedHierarchies_query_w_parent_change_detection_1st_populate_when_changed(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {{ 
                .id = ecs_id(Position), 
                .inout = EcsIn
            },
            { 
                .id = ecs_id(Position), 
                .src.flags = EcsParent|EcsCascade,
                .inout = EcsIn
            },
        },
        .filter.instanced = true
    });

    ecs_entity_t p = ecs_new_entity(world, "p");
    ecs_entity_t p_1 = ecs_new_entity(world, "p._1");
    ecs_entity_t p_1_1 = ecs_new_entity(world, "p._1._1");
    ecs_entity_t p_2 = ecs_new_entity(world, "p._2");
    ecs_entity_t p_2_1 = ecs_new_entity(world, "p._2._1");

    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1_1, Position, {111, 211});
    ecs_set(world, p_2, Position, {12, 22});
    ecs_set(world, p_2_1, Position, {121, 221});
    
    // offset the column of Position to verify change detection is testing
    // againt the right one.
    ecs_add(world, p_2, Velocity); 

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_1));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        {
            test_bool(ecs_query_next_table(&it), true);
            test_assert(ecs_query_populate(&it, true) == EcsIterYield);
            test_uint(1, it.count);
            test_uint(p_1_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_1, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 111); test_int(p[0].y, 211);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 11); test_int(pp->y, 21);
        }
        {
            test_assert(ecs_query_populate(&it, true) == EcsIterNextYield);
            test_uint(1, it.count);
            test_uint(p_2_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_2, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 121); test_int(p[0].y, 221);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 12); test_int(pp->y, 22);
        }
        test_bool(ecs_query_next_table(&it), false);
    }

    ecs_set(world, p_1, Position, {22, 42});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        {
            test_bool(ecs_query_next_table(&it), true);
            test_assert(ecs_query_populate(&it, true) == EcsIterYield);
            test_uint(1, it.count);
            test_uint(p_1_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_1, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 111); test_int(p[0].y, 211);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 22); test_int(pp->y, 42);
        }
        {
            test_assert(ecs_query_populate(&it, true) == EcsIterNext);
        }
        test_bool(ecs_query_next_table(&it), false);
    }

    ecs_fini(world);
}

void FixedHierarchies_query_w_parent_change_detection_2nd_populate_when_changed(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {{ 
                .id = ecs_id(Position), 
                .inout = EcsIn
            },
            { 
                .id = ecs_id(Position), 
                .src.flags = EcsParent|EcsCascade,
                .inout = EcsIn
            },
        },
        .filter.instanced = true
    });

    ecs_entity_t p = ecs_new_entity(world, "p");
    ecs_entity_t p_1 = ecs_new_entity(world, "p._1");
    ecs_entity_t p_1_1 = ecs_new_entity(world, "p._1._1");
    ecs_entity_t p_2 = ecs_new_entity(world, "p._2");
    ecs_entity_t p_2_1 = ecs_new_entity(world, "p._2._1");

    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1_1, Position, {111, 211});
    ecs_set(world, p_2, Position, {12, 22});
    ecs_set(world, p_2_1, Position, {121, 221});
    
    // offset the column of Position to verify change detection is testing
    // againt the right one.
    ecs_add(world, p_2, Velocity); 

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_1));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        {
            test_bool(ecs_query_next_table(&it), true);
            test_assert(ecs_query_populate(&it, true) == EcsIterYield);
            test_uint(1, it.count);
            test_uint(p_1_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_1, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 111); test_int(p[0].y, 211);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 11); test_int(pp->y, 21);
        }
        {
            test_assert(ecs_query_populate(&it, true) == EcsIterNextYield);
            test_uint(1, it.count);
            test_uint(p_2_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_2, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 121); test_int(p[0].y, 221);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 12); test_int(pp->y, 22);
        }
        test_bool(ecs_query_next_table(&it), false);
    }

    ecs_set(world, p_2, Position, {24, 44});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        {
            test_bool(ecs_query_next_table(&it), true);
            test_assert(ecs_query_populate(&it, true) == EcsIterNextYield);
            test_bool(ecs_query_changed(NULL, &it), true);
            test_uint(1, it.count);
            test_uint(p_2_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_2, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 121); test_int(p[0].y, 221);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 24); test_int(pp->y, 44);
        }
        test_bool(ecs_query_next_table(&it), false);
    }

    ecs_fini(world);
}

void FixedHierarchies_query_w_parent_change_detection_iter_twice_populate_when_changed(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {{ 
                .id = ecs_id(Position), 
                .inout = EcsIn
            },
            { 
                .id = ecs_id(Position), 
                .src.flags = EcsParent|EcsCascade,
                .inout = EcsIn
            },
        },
        .filter.instanced = true
    });

    ecs_entity_t p = ecs_new_entity(world, "p");
    ecs_entity_t p_1 = ecs_new_entity(world, "p._1");
    ecs_entity_t p_1_1 = ecs_new_entity(world, "p._1._1");
    ecs_entity_t p_2 = ecs_new_entity(world, "p._2");
    ecs_entity_t p_2_1 = ecs_new_entity(world, "p._2._1");

    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1_1, Position, {111, 211});
    ecs_set(world, p_2, Position, {12, 22});
    ecs_set(world, p_2_1, Position, {121, 221});
    
    // offset the column of Position to verify change detection is testing
    // againt the right one.
    ecs_add(world, p_2, Velocity); 

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_1));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        {
            test_bool(ecs_query_next_table(&it), true);
            test_assert(ecs_query_populate(&it, true) == EcsIterYield);
            test_uint(1, it.count);
            test_uint(p_1_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_1, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 111); test_int(p[0].y, 211);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 11); test_int(pp->y, 21);
        }
        {
            test_assert(ecs_query_populate(&it, true) == EcsIterNextYield);
            test_uint(1, it.count);
            test_uint(p_2_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_2, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 121); test_int(p[0].y, 221);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 12); test_int(pp->y, 22);
        }
        test_bool(ecs_query_next_table(&it), false);
    }

    ecs_set(world, p_1, Position, {22, 42});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        {
            test_bool(ecs_query_next_table(&it), true);
            test_assert(ecs_query_populate(&it, true) == EcsIterYield);
            test_uint(1, it.count);
            test_uint(p_1_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_1, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 111); test_int(p[0].y, 211);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 22); test_int(pp->y, 42);
        }
        {
            test_assert(ecs_query_populate(&it, true) == EcsIterNext);
        }
        test_bool(ecs_query_next_table(&it), false);
    }

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        {
            test_bool(ecs_query_next_table(&it), true);
            test_assert(ecs_query_populate(&it, true) == EcsIterNext);
        }
        test_bool(ecs_query_next_table(&it), false);
    }

    ecs_fini(world);
}

void FixedHierarchies_query_w_parent_change_detection_iter_twice_each_parent_populate_when_changed(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {{ 
                .id = ecs_id(Position), 
                .inout = EcsIn
            },
            { 
                .id = ecs_id(Position), 
                .src.flags = EcsParent|EcsCascade,
                .inout = EcsIn
            },
        },
        .filter.instanced = true
    });

    ecs_entity_t p = ecs_new_entity(world, "p");
    ecs_entity_t p_1 = ecs_new_entity(world, "p._1");
    ecs_entity_t p_1_1 = ecs_new_entity(world, "p._1._1");
    ecs_entity_t p_2 = ecs_new_entity(world, "p._2");
    ecs_entity_t p_2_1 = ecs_new_entity(world, "p._2._1");

    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1_1, Position, {111, 211});
    ecs_set(world, p_2, Position, {12, 22});
    ecs_set(world, p_2_1, Position, {121, 221});
    
    // offset the column of Position to verify change detection is testing
    // againt the right one.
    ecs_add(world, p_2, Velocity); 

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_1));

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        {
            test_bool(ecs_query_next_table(&it), true);
            test_assert(ecs_query_populate(&it, true) == EcsIterYield);
            test_uint(1, it.count);
            test_uint(p_1_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_1, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 111); test_int(p[0].y, 211);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 11); test_int(pp->y, 21);
        }
        {
            test_assert(ecs_query_populate(&it, true) == EcsIterNextYield);
            test_uint(1, it.count);
            test_uint(p_2_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_2, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 121); test_int(p[0].y, 221);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 12); test_int(pp->y, 22);
        }
        test_bool(ecs_query_next_table(&it), false);
    }

    ecs_set(world, p_1, Position, {22, 42});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        {
            test_bool(ecs_query_next_table(&it), true);
            test_assert(ecs_query_populate(&it, true) == EcsIterYield);
            test_uint(1, it.count);
            test_uint(p_1_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_1, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 111); test_int(p[0].y, 211);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 22); test_int(pp->y, 42);
        }
        {
            test_assert(ecs_query_populate(&it, true) == EcsIterNext);
        }
        test_bool(ecs_query_next_table(&it), false);
    }

    ecs_set(world, p_2, Position, {24, 44});

    {
        ecs_iter_t it = ecs_query_iter(world, q);
        {
            test_bool(ecs_query_next_table(&it), true);
            test_assert(ecs_query_populate(&it, true) == EcsIterNextYield);
            test_uint(1, it.count);
            test_uint(p_2_1, it.entities[0]);
            test_uint(0, it.sources[0]);
            test_uint(p_2, it.sources[1]);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 121); test_int(p[0].y, 221);

            Position *pp = ecs_field(&it, Position, 2);
            test_assert(pp != NULL);
            test_int(pp->x, 24); test_int(pp->y, 44);
        }
        test_bool(ecs_query_next_table(&it), false);
    }

    ecs_fini(world);
}

void FixedHierarchies_staged_query_w_parent_field_1_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_set(world, 0, Position, {1, 2});
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1_1, Position, {111, 211});

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_set(world, p_2, Position, {12, 22});
    ecs_set(world, p_2_1, Position, {121, 221});

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .src.flags = EcsParent },
        },
        .filter.instanced = true
    });

    ecs_world_t *stage = ecs_get_stage(world, 0);
    ecs_readonly_begin(world, false);

    ecs_iter_t it = ecs_query_iter(stage, q);
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(2, it.count);
        test_uint(p_1, it.entities[0]);
        test_uint(p_2, it.entities[1]);
        test_uint(0, it.sources[0]);
        test_uint(p, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 11); test_int(p[0].y, 21);
        test_int(p[1].x, 12); test_int(p[1].y, 22);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 1); test_int(pp[0].y, 2);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_1_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_1, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 111); test_int(p[0].y, 211);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 11); test_int(pp[0].y, 21);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_2_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_2, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 121); test_int(p[0].y, 221);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 12); test_int(pp[0].y, 22);
    }
    test_bool(ecs_query_next(&it), false);

    ecs_readonly_end(world);

    ecs_fini(world);
}

void FixedHierarchies_staged_query_w_parent_field_2_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_set(world, 0, Position, {1, 2});

    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t p_1_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1_1);
    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1_1, Position, {111, 211});
    ecs_set(world, p_1_1_1, Position, {1111, 2111});

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t p_2_1_1 = ecs_new_w_pair(world, EcsChildOf, p_2_1);
    ecs_set(world, p_2, Position, {12, 22});
    ecs_set(world, p_2_1, Position, {121, 221});
    ecs_set(world, p_2_1_1, Position, {1211, 2211});

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .src.flags = EcsParent },
        },
        .filter.instanced = true
    });

    ecs_world_t *stage = ecs_get_stage(world, 0);
    ecs_readonly_begin(world, false);

    ecs_iter_t it = ecs_query_iter(stage, q);
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(2, it.count);
        test_uint(p_1, it.entities[0]);
        test_uint(p_2, it.entities[1]);
        test_uint(0, it.sources[0]);
        test_uint(p, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 11); test_int(p[0].y, 21);
        test_int(p[1].x, 12); test_int(p[1].y, 22);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 1); test_int(pp[0].y, 2);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_1_1_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_1_1, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 1111); test_int(p[0].y, 2111);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 111); test_int(pp[0].y, 211);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_2_1_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_2_1, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 1211); test_int(p[0].y, 2211);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 121); test_int(pp[0].y, 221);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_1_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_1, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 111); test_int(p[0].y, 211);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 11); test_int(pp[0].y, 21);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_2_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_2, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 121); test_int(p[0].y, 221);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 12); test_int(pp[0].y, 22);
    }
    test_bool(ecs_query_next(&it), false);

    ecs_readonly_end(world);

    ecs_fini(world);
}

void FixedHierarchies_staged_query_w_parent_field_1_fixed_1_regular(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t p = ecs_set(world, 0, Position, {1, 2});
    ecs_set(world, p, Velocity, {3, 4});
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1_1, Position, {111, 211});

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_set(world, p_2, Position, {12, 22});
    ecs_set(world, p_2_1, Position, {121, 221});

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .src.flags = EcsParent },
            { .id = ecs_id(Velocity), .src.flags = EcsParent }
        },
        .filter.instanced = true
    });

    ecs_world_t *stage = ecs_get_stage(world, 0);
    ecs_readonly_begin(world, false);

    ecs_iter_t it = ecs_query_iter(stage, q);
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(2, it.count);
        test_uint(p_1, it.entities[0]);
        test_uint(p_2, it.entities[1]);
        test_uint(0, it.sources[0]);
        test_uint(p, it.sources[1]);
        test_uint(p, it.sources[2]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 11); test_int(p[0].y, 21);
        test_int(p[1].x, 12); test_int(p[1].y, 22);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 1); test_int(pp[0].y, 2);
        Velocity *vp = ecs_field(&it, Velocity, 3);
        test_int(vp[0].x, 3); test_int(vp[0].y, 4);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_1_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_1, it.sources[1]);
        test_uint(p, it.sources[2]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 111); test_int(p[0].y, 211);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 11); test_int(pp[0].y, 21);
        Velocity *vp = ecs_field(&it, Velocity, 3);
        test_int(vp[0].x, 3); test_int(vp[0].y, 4);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_2_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_2, it.sources[1]);
        test_uint(p, it.sources[2]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 121); test_int(p[0].y, 221);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 12); test_int(pp[0].y, 22);
        Velocity *vp = ecs_field(&it, Velocity, 3);
        test_int(vp[0].x, 3); test_int(vp[0].y, 4);
    }
    test_bool(ecs_query_next(&it), false);

    ecs_readonly_end(world);

    ecs_fini(world);
}

void FixedHierarchies_staged_query_w_cascade_field_2_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_set(world, 0, Position, {1, 2});

    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t p_1_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1_1);
    ecs_set(world, p_1, Position, {11, 21});
    ecs_set(world, p_1_1, Position, {111, 211});
    ecs_set(world, p_1_1_1, Position, {1111, 2111});

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t p_2_1_1 = ecs_new_w_pair(world, EcsChildOf, p_2_1);
    ecs_set(world, p_2, Position, {12, 22});
    ecs_set(world, p_2_1, Position, {121, 221});
    ecs_set(world, p_2_1_1, Position, {1211, 2211});

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    ecs_query_t *q = ecs_query(world, {
        .filter.terms = {
            { .id = ecs_id(Position) },
            { .id = ecs_id(Position), .src.flags = EcsParent|EcsCascade },
        },
        .filter.instanced = true
    });

    ecs_world_t *stage = ecs_get_stage(world, 0);
    ecs_readonly_begin(world, false);

    ecs_iter_t it = ecs_query_iter(stage, q);
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(2, it.count);
        test_uint(p_1, it.entities[0]);
        test_uint(p_2, it.entities[1]);
        test_uint(0, it.sources[0]);
        test_uint(p, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 11); test_int(p[0].y, 21);
        test_int(p[1].x, 12); test_int(p[1].y, 22);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 1); test_int(pp[0].y, 2);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_1_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_1, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 111); test_int(p[0].y, 211);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 11); test_int(pp[0].y, 21);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_2_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_2, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 121); test_int(p[0].y, 221);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 12); test_int(pp[0].y, 22);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_1_1_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_1_1, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 1111); test_int(p[0].y, 2111);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 111); test_int(pp[0].y, 211);
    }
    {
        test_bool(ecs_query_next(&it), true);
        test_uint(1, it.count);
        test_uint(p_2_1_1, it.entities[0]);
        test_uint(0, it.sources[0]);
        test_uint(p_2_1, it.sources[1]);
        Position *p = ecs_field(&it, Position, 1);
        test_int(p[0].x, 1211); test_int(p[0].y, 2211);

        Position *pp = ecs_field(&it, Position, 2);
        test_int(pp[0].x, 121); test_int(pp[0].y, 221);
    }
    test_bool(ecs_query_next(&it), false);

    ecs_readonly_end(world);

    ecs_fini(world);
}

void FixedHierarchies_add_to_fixed(void) {
    install_test_abort();
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_expect_abort();
    ecs_add(world, p_1_1, Tag);
}

void FixedHierarchies_remove_from_fixed(void) {
    install_test_abort();
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_add(world, p_1_1, Tag);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_expect_abort();
    ecs_remove(world, p_1_1, Tag);
}

void FixedHierarchies_delete_fixed(void) {
    install_test_abort();
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_expect_abort();
    ecs_delete(world, p_1_1);
}

void FixedHierarchies_clear_fixed(void) {
    install_test_abort();
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_expect_abort();
    ecs_clear(world, p_1_1);
}

void FixedHierarchies_make_fixed_1_lvl_w_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_id(world);
    ecs_set_name(world, p, "p");
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, p_1, "name_1");
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_set_name(world, p_1_1, "child_1");

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, p_2, "name_2");
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_set_name(world, p_2_1, "child_1");

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_lookup(world, "p.name_1") == p_1);
    test_assert(ecs_lookup(world, "p.name_2") == p_2);
    test_assert(ecs_lookup(world, "p.name_1.child_1") == 0);
    test_assert(ecs_lookup(world, "p.name_2.child_1") == 0);

    test_str(ecs_get_name(world, p_1), "name_1");
    test_str(ecs_get_name(world, p_2), "name_2");
    test_str(ecs_get_name(world, p_1_1), NULL);
    test_str(ecs_get_name(world, p_2_1), NULL);

    ecs_fini(world);
}

void FixedHierarchies_make_fixed_2_lvl_w_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_id(world);
    ecs_set_name(world, p, "p");
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, p_1, "name_1");
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_set_name(world, p_1_1, "child_1");
    ecs_entity_t p_1_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1_1);
    ecs_set_name(world, p_1_1_1, "child_1_1");

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, p_2, "name_2");
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_set_name(world, p_2_1, "child_1");
    ecs_entity_t p_2_1_1 = ecs_new_w_pair(world, EcsChildOf, p_2_1);
    ecs_set_name(world, p_2_1_1, "child_1_1");

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_lookup(world, "p.name_1") == p_1);
    test_assert(ecs_lookup(world, "p.name_2") == p_2);
    test_assert(ecs_lookup(world, "p.name_1.child_1") == 0);
    test_assert(ecs_lookup(world, "p.name_2.child_1") == 0);
    test_assert(ecs_lookup(world, "p.name_1.child_1.child_1_1") == 0);
    test_assert(ecs_lookup(world, "p.name_2.child_1.child_1_1") == 0);

    test_str(ecs_get_name(world, p_1), "name_1");
    test_str(ecs_get_name(world, p_2), "name_2");
    test_str(ecs_get_name(world, p_1_1), NULL);
    test_str(ecs_get_name(world, p_2_1), NULL);
    test_str(ecs_get_name(world, p_1_1_1), NULL);
    test_str(ecs_get_name(world, p_2_1_1), NULL);

    ecs_fini(world);
}

void FixedHierarchies_make_fixed_1_lvl_w_name_keep_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_id(world);
    ecs_set_name(world, p, "p");
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, p_1, "name_1");
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_set_name(world, p_1_1, "child_1_1");

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, p_2, "name_2");
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_set_name(world, p_2_1, "child_2_1");

    ecs_flatten(world, ecs_pair(EcsChildOf, p), &(ecs_flatten_desc_t){
        .keep_names = true
    });

    test_assert(ecs_lookup(world, "p.name_1") == p_1);
    test_assert(ecs_lookup(world, "p.name_2") == p_2);
    test_assert(ecs_lookup(world, "p.name_1.child_1") == 0);
    test_assert(ecs_lookup(world, "p.name_2.child_1") == 0);

    test_str(ecs_get_name(world, p_1), "name_1");
    test_str(ecs_get_name(world, p_2), "name_2");
    test_str(ecs_get_name(world, p_1_1), "child_1_1");
    test_str(ecs_get_name(world, p_2_1), "child_2_1");

    ecs_fini(world);
}

void FixedHierarchies_make_fixed_2_lvl_w_name_keep_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_id(world);
    ecs_set_name(world, p, "p");
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, p_1, "name_1");
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_set_name(world, p_1_1, "child_1_1");
    ecs_entity_t p_1_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1_1);
    ecs_set_name(world, p_1_1_1, "child_1_1_1");

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_set_name(world, p_2, "name_2");
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_set_name(world, p_2_1, "child_2_1");
    ecs_entity_t p_2_1_1 = ecs_new_w_pair(world, EcsChildOf, p_2_1);
    ecs_set_name(world, p_2_1_1, "child_2_1_1");

    ecs_flatten(world, ecs_pair(EcsChildOf, p), &(ecs_flatten_desc_t){
        .keep_names = true
    });

    test_assert(ecs_lookup(world, "p.name_1") == p_1);
    test_assert(ecs_lookup(world, "p.name_2") == p_2);
    test_assert(ecs_lookup(world, "p.name_1.child_1") == 0);
    test_assert(ecs_lookup(world, "p.name_2.child_1") == 0);
    test_assert(ecs_lookup(world, "p.name_1.child_1.child_1_1") == 0);
    test_assert(ecs_lookup(world, "p.name_2.child_1.child_1_1") == 0);

    test_str(ecs_get_name(world, p_1), "name_1");
    test_str(ecs_get_name(world, p_2), "name_2");
    test_str(ecs_get_name(world, p_1_1), "child_1_1");
    test_str(ecs_get_name(world, p_2_1), "child_2_1");
    test_str(ecs_get_name(world, p_1_1_1), "child_1_1_1");
    test_str(ecs_get_name(world, p_2_1_1), "child_2_1_1");

    ecs_fini(world);
}

void FixedHierarchies_make_fixed_2_lvl_lose_depth(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t p_1_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1_1);

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t p_2_1_1 = ecs_new_w_pair(world, EcsChildOf, p_2_1);
    
    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) != ecs_get_table(world, p_2_1));
    test_assert(ecs_get_table(world, p_1_1_1) != ecs_get_table(world, p_2_1_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_1_1, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);
    test_assert(ecs_get_target(world, p_2_1_1, EcsChildOf, 0) == p_2_1);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), &(ecs_flatten_desc_t){
        .lose_depth = true
    });

    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_1));
    test_assert(ecs_get_table(world, p_1_1_1) == ecs_get_table(world, p_1_1));
    test_assert(ecs_get_table(world, p_1_1_1) == ecs_get_table(world, p_2_1_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_1_1, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);
    test_assert(ecs_get_target(world, p_2_1_1, EcsChildOf, 0) == p_2_1);

    ecs_fini(world);
}

void FixedHierarchies_make_fixed_3_lvl_lose_depth(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_id(world);
    
    ecs_entity_t p_1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t p_1_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1_1);
    ecs_entity_t p_1_1_1_1 = ecs_new_w_pair(world, EcsChildOf, p_1_1_1);

    ecs_entity_t p_2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t p_2_1 = ecs_new_w_pair(world, EcsChildOf, p_2);
    ecs_entity_t p_2_1_1 = ecs_new_w_pair(world, EcsChildOf, p_2_1);
    ecs_entity_t p_2_1_1_1 = ecs_new_w_pair(world, EcsChildOf, p_2_1_1);
    
    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) != ecs_get_table(world, p_2_1));
    test_assert(ecs_get_table(world, p_1_1_1) != ecs_get_table(world, p_2_1_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_1_1, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_1_1_1_1, EcsChildOf, 0) == p_1_1_1);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);
    test_assert(ecs_get_target(world, p_2_1_1, EcsChildOf, 0) == p_2_1);
    test_assert(ecs_get_target(world, p_2_1_1_1, EcsChildOf, 0) == p_2_1_1);

    ecs_flatten(world, ecs_pair(EcsChildOf, p), NULL);

    test_assert(ecs_get_table(world, p_1) == ecs_get_table(world, p_2));
    test_assert(ecs_get_table(world, p_1_1) == ecs_get_table(world, p_2_1));
    test_assert(ecs_get_table(world, p_1_1_1) == ecs_get_table(world, p_2_1_1));
    test_assert(ecs_get_table(world, p_1_1_1) != ecs_get_table(world, p_1_1));
    test_assert(ecs_get_target(world, p_1, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_1_1, EcsChildOf, 0) == p_1);
    test_assert(ecs_get_target(world, p_1_1_1, EcsChildOf, 0) == p_1_1);
    test_assert(ecs_get_target(world, p_1_1_1_1, EcsChildOf, 0) == p_1_1_1);
    test_assert(ecs_get_target(world, p_2, EcsChildOf, 0) == p);
    test_assert(ecs_get_target(world, p_2_1, EcsChildOf, 0) == p_2);
    test_assert(ecs_get_target(world, p_2_1_1, EcsChildOf, 0) == p_2_1);
    test_assert(ecs_get_target(world, p_2_1_1_1, EcsChildOf, 0) == p_2_1_1);

    ecs_fini(world);
}
