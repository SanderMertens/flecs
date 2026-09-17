#include <core.h>

typedef struct {
    int32_t hp;
} Unit;

typedef struct {
    int32_t hp;
    int32_t dmg;
} Warrior;

typedef struct {
    int32_t hp;
    int32_t armor;
} MeleeUnit;

typedef struct {
    int32_t hp;
    int32_t mana;
} Mage;

void ComponentInheritance_has_2_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);

    test_bool(ecs_has_id(world, e, Warrior), true);
    test_bool(ecs_has_id(world, e, Unit), true);

    ecs_fini(world);
}

void ComponentInheritance_has_3_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Melee);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Melee, EcsIsA, Unit);
    ecs_add_pair(world, Warrior, EcsIsA, Melee);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);

    test_bool(ecs_has_id(world, e, Warrior), true);
    test_bool(ecs_has_id(world, e, Melee), true);
    test_bool(ecs_has_id(world, e, Unit), true);

    ecs_fini(world);
}

void ComponentInheritance_has_not_derived(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);
    ECS_TAG(world, Wizard);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);
    ecs_add_pair(world, Wizard, EcsIsA, Unit);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);

    test_bool(ecs_has_id(world, e, Unit), true);
    test_bool(ecs_has_id(world, e, Warrior), true);
    test_bool(ecs_has_id(world, e, Wizard), false);

    ecs_fini(world);
}

void ComponentInheritance_owns_2_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);

    test_bool(ecs_owns_id(world, e, Warrior), true);
    test_bool(ecs_owns_id(world, e, Unit), true);

    ecs_fini(world);
}

void ComponentInheritance_owns_3_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Melee);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Melee, EcsIsA, Unit);
    ecs_add_pair(world, Warrior, EcsIsA, Melee);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);

    test_bool(ecs_owns_id(world, e, Warrior), true);
    test_bool(ecs_owns_id(world, e, Melee), true);
    test_bool(ecs_owns_id(world, e, Unit), true);

    ecs_fini(world);
}

void ComponentInheritance_get_2_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Unit);
    ECS_COMPONENT(world, Warrior);

    ecs_add_pair(world, ecs_id(Warrior), EcsIsA, ecs_id(Unit));

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Warrior, {.hp = 10, .dmg = 5});

    const Warrior *w = ecs_get(world, e, Warrior);
    const Unit *u = ecs_get(world, e, Unit);

    test_assert(w != NULL);
    test_assert(u != NULL);
    test_assert((const void*)w == (const void*)u);
    test_int(u->hp, 10);

    ecs_fini(world);
}

void ComponentInheritance_get_3_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Unit);
    ECS_COMPONENT(world, MeleeUnit);
    ECS_COMPONENT(world, Warrior);

    ecs_add_pair(world, ecs_id(MeleeUnit), EcsIsA, ecs_id(Unit));
    ecs_add_pair(world, ecs_id(Warrior), EcsIsA, ecs_id(MeleeUnit));

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Warrior, {.hp = 10, .dmg = 5});

    const Warrior *w = ecs_get(world, e, Warrior);
    const MeleeUnit *m = ecs_get(world, e, MeleeUnit);
    const Unit *u = ecs_get(world, e, Unit);

    test_assert(w != NULL);
    test_assert(m != NULL);
    test_assert(u != NULL);
    test_assert((const void*)w == (const void*)m);
    test_assert((const void*)w == (const void*)u);
    test_int(u->hp, 10);

    ecs_fini(world);
}

void ComponentInheritance_search_2_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);
    ecs_table_t *table = ecs_get_table(world, e);
    test_assert(table != NULL);

    ecs_id_t id_out = 0;
    int32_t col = ecs_search(world, table, Unit, &id_out);
    test_assert(col != -1);
    test_uint(id_out, Warrior);

    ecs_fini(world);
}

void ComponentInheritance_search_3_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Melee);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Melee, EcsIsA, Unit);
    ecs_add_pair(world, Warrior, EcsIsA, Melee);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);
    ecs_table_t *table = ecs_get_table(world, e);
    test_assert(table != NULL);

    ecs_id_t id_out = 0;
    test_assert(ecs_search(world, table, Unit, &id_out) != -1);
    test_uint(id_out, Warrior);

    id_out = 0;
    test_assert(ecs_search(world, table, Melee, &id_out) != -1);
    test_uint(id_out, Warrior);

    ecs_fini(world);
}

void ComponentInheritance_search_base_no_match(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);
    ECS_TAG(world, Foo);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);

    ecs_entity_t e = ecs_new_w_id(world, Foo);
    ecs_table_t *table = ecs_get_table(world, e);
    test_assert(table != NULL);

    ecs_id_t id_out = 0;
    int32_t col = ecs_search(world, table, Unit, &id_out);
    test_int(col, -1);
    test_uint(id_out, 0);

    col = ecs_search_offset(world, table, 0, Unit, &id_out);
    test_int(col, -1);

    ecs_fini(world);
}

void ComponentInheritance_search_base_w_base_only(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);

    ecs_entity_t e = ecs_new_w_id(world, Unit);
    ecs_table_t *table = ecs_get_table(world, e);
    test_assert(table != NULL);

    ecs_id_t id_out = 0;
    int32_t col = ecs_search(world, table, Unit, &id_out);
    test_int(col, 0);
    test_uint(id_out, Unit);

    id_out = 0;
    col = ecs_search_offset(world, table, 0, Unit, &id_out);
    test_int(col, 0);
    test_uint(id_out, Unit);

    col = ecs_search_offset(world, table, 1, Unit, &id_out);
    test_int(col, -1);

    ecs_fini(world);
}

void ComponentInheritance_search_base_w_single_derived(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);
    ecs_table_t *table = ecs_get_table(world, e);
    test_assert(table != NULL);

    ecs_id_t id_out = 0;
    int32_t col = ecs_search(world, table, Unit, &id_out);
    test_int(col, 0);
    test_uint(id_out, Warrior);

    id_out = 0;
    col = ecs_search_offset(world, table, 0, Unit, &id_out);
    test_int(col, 0);
    test_uint(id_out, Warrior);

    col = ecs_search_offset(world, table, 1, Unit, &id_out);
    test_int(col, -1);

    ecs_fini(world);
}

void ComponentInheritance_search_base_w_two_derived(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);
    ECS_TAG(world, Wizard);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);
    ecs_add_pair(world, Wizard, EcsIsA, Unit);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);
    ecs_add_id(world, e, Wizard);
    ecs_table_t *table = ecs_get_table(world, e);
    test_assert(table != NULL);

    ecs_id_t id_out = 0;
    int32_t col = ecs_search(world, table, Unit, &id_out);
    test_int(col, 0);
    test_uint(id_out, Warrior);

    ecs_fini(world);
}

void ComponentInheritance_search_two_derived_for_base(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);
    ECS_TAG(world, Wizard);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);
    ecs_add_pair(world, Wizard, EcsIsA, Unit);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);
    ecs_add_id(world, e, Wizard);
    ecs_table_t *table = ecs_get_table(world, e);
    test_assert(table != NULL);

    ecs_id_t id_out = 0;
    int32_t col = ecs_search_offset(world, table, 0, Unit, &id_out);
    test_int(col, 0);
    test_uint(id_out, Warrior);

    id_out = 0;
    col = ecs_search_offset(world, table, col + 1, Unit, &id_out);
    test_int(col, 1);
    test_uint(id_out, Wizard);

    col = ecs_search_offset(world, table, col + 1, Unit, &id_out);
    test_int(col, -1);

    ecs_fini(world);
}

void ComponentInheritance_search_base_and_derived_for_base(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);

    ecs_entity_t e = ecs_new_w_id(world, Unit);
    ecs_add_id(world, e, Warrior);
    ecs_table_t *table = ecs_get_table(world, e);
    test_assert(table != NULL);

    ecs_id_t id_out = 0;
    int32_t col = ecs_search(world, table, Unit, &id_out);
    test_int(col, 0);
    test_uint(id_out, Unit);

    id_out = 0;
    col = ecs_search_offset(world, table, col + 1, Unit, &id_out);
    test_int(col, 1);
    test_uint(id_out, Warrior);

    col = ecs_search_offset(world, table, col + 1, Unit, &id_out);
    test_int(col, -1);

    ecs_fini(world);
}

void ComponentInheritance_get_after_add_isa(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Unit);
    ECS_COMPONENT(world, Warrior);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Warrior, {.hp = 10, .dmg = 5});

    test_assert(ecs_get(world, e, Unit) == NULL);
    test_bool(ecs_has_id(world, e, ecs_id(Unit)), false);

    test_expect_abort();
    ecs_add_pair(world, ecs_id(Warrior), EcsIsA, ecs_id(Unit));
}

void ComponentInheritance_has_after_add_isa(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);

    test_bool(ecs_has_id(world, e, Unit), false);
    test_bool(ecs_owns_id(world, e, Unit), false);

    test_expect_abort();
    ecs_add_pair(world, Warrior, EcsIsA, Unit);
}

void ComponentInheritance_has_after_remove_isa(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);
    test_bool(ecs_has_id(world, e, Unit), true);

    test_expect_abort();
    ecs_remove_pair(world, Warrior, EcsIsA, Unit);
}

void ComponentInheritance_add_isa_3_lvl_after_populate(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Melee);
    ECS_TAG(world, Warrior);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);

    test_bool(ecs_has_id(world, e, Melee), false);
    test_bool(ecs_has_id(world, e, Unit), false);

    test_expect_abort();
    ecs_add_pair(world, Warrior, EcsIsA, Melee);
}

void ComponentInheritance_remove_isa_middle_3_lvl(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Melee);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Melee, EcsIsA, Unit);
    ecs_add_pair(world, Warrior, EcsIsA, Melee);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);
    test_bool(ecs_has_id(world, e, Melee), true);
    test_bool(ecs_has_id(world, e, Unit), true);

    test_expect_abort();
    ecs_remove_pair(world, Melee, EcsIsA, Unit);
}

void ComponentInheritance_add_isa_w_multiple_tables(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_w_id(world, Warrior);
    ecs_add_id(world, e1, TagA);
    ecs_entity_t e2 = ecs_new_w_id(world, Warrior);
    ecs_add_id(world, e2, TagB);

    test_bool(ecs_has_id(world, e1, Unit), false);
    test_bool(ecs_has_id(world, e2, Unit), false);

    test_expect_abort();
    ecs_add_pair(world, Warrior, EcsIsA, Unit);
}

void ComponentInheritance_base_and_derived_same_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);

    ecs_entity_t e = ecs_new_w_id(world, Unit);
    ecs_add_id(world, e, Warrior);

    test_bool(ecs_has_id(world, e, Unit), true);
    test_bool(ecs_has_id(world, e, Warrior), true);
    test_bool(ecs_owns_id(world, e, Unit), true);

    ecs_fini(world);
}

void ComponentInheritance_diamond_allowed(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);
    ECS_TAG(world, Wizard);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);
    ecs_add_pair(world, Wizard, EcsIsA, Unit);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);
    ecs_add_id(world, e, Wizard);

    test_bool(ecs_has_id(world, e, Warrior), true);
    test_bool(ecs_has_id(world, e, Wizard), true);
    test_bool(ecs_has_id(world, e, Unit), true);
    test_bool(ecs_owns_id(world, e, Unit), true);

    ecs_fini(world);
}

void ComponentInheritance_derived_w_multiple_isa_bases(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);

    ecs_entity_t base_a = ecs_new(world);
    ecs_entity_t base_b = ecs_new(world);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsIsA, base_a);
    ecs_add_pair(world, e, EcsIsA, base_b);
    ecs_add_id(world, e, Warrior);

    test_bool(ecs_has_id(world, e, Warrior), true);
    test_bool(ecs_has_id(world, e, Unit), true);
    test_bool(ecs_has_pair(world, e, EcsIsA, base_a), true);
    test_bool(ecs_has_pair(world, e, EcsIsA, base_b), true);

    ecs_id_t id_out = 0;
    test_assert(ecs_search(world, ecs_get_table(world, e), Unit, &id_out) != -1);
    test_uint(id_out, Warrior);

    ecs_fini(world);
}

void ComponentInheritance_delete_with_base_deletes_derived(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);

    ecs_entity_t w1 = ecs_new_w_id(world, Warrior);
    ecs_entity_t w2 = ecs_new_w_id(world, Warrior);
    ecs_entity_t u1 = ecs_new_w_id(world, Unit);

    ecs_delete_with(world, Unit);

    test_bool(ecs_is_alive(world, w1), false);
    test_bool(ecs_is_alive(world, w2), false);
    test_bool(ecs_is_alive(world, u1), false);

    ecs_fini(world);
}

void ComponentInheritance_delete_with_base_deletes_derived_3_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Melee);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Melee, EcsIsA, Unit);
    ecs_add_pair(world, Warrior, EcsIsA, Melee);

    ecs_entity_t warrior = ecs_new_w_id(world, Warrior);
    ecs_entity_t melee = ecs_new_w_id(world, Melee);
    ecs_entity_t unit = ecs_new_w_id(world, Unit);

    ecs_delete_with(world, Unit);

    test_bool(ecs_is_alive(world, warrior), false);
    test_bool(ecs_is_alive(world, melee), false);
    test_bool(ecs_is_alive(world, unit), false);

    ecs_fini(world);
}

void ComponentInheritance_delete_with_middle_keeps_base(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Melee);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Melee, EcsIsA, Unit);
    ecs_add_pair(world, Warrior, EcsIsA, Melee);

    ecs_entity_t warrior = ecs_new_w_id(world, Warrior);
    ecs_entity_t melee = ecs_new_w_id(world, Melee);
    ecs_entity_t unit = ecs_new_w_id(world, Unit);

    ecs_delete_with(world, Melee);

    test_bool(ecs_is_alive(world, warrior), false);
    test_bool(ecs_is_alive(world, melee), false);
    test_bool(ecs_is_alive(world, unit), true);

    ecs_fini(world);
}

void ComponentInheritance_remove_all_base_removes_derived(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);

    ecs_entity_t w1 = ecs_new_w_id(world, Warrior);
    ecs_entity_t u1 = ecs_new_w_id(world, Unit);

    ecs_remove_all(world, Unit);

    test_bool(ecs_is_alive(world, w1), true);
    test_bool(ecs_is_alive(world, u1), true);
    test_bool(ecs_has_id(world, w1, Warrior), false);
    test_bool(ecs_has_id(world, u1, Unit), false);
    test_bool(ecs_has_id(world, w1, Unit), false);

    ecs_fini(world);
}

void ComponentInheritance_delete_with_after_remove_isa(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);

    ecs_new_w_id(world, Warrior);
    ecs_new_w_id(world, Unit);

    test_expect_abort();
    ecs_remove_pair(world, Warrior, EcsIsA, Unit);
}

void ComponentInheritance_get_mut_via_base(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Unit);
    ECS_COMPONENT(world, Warrior);

    ecs_add_pair(world, ecs_id(Warrior), EcsIsA, ecs_id(Unit));

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Warrior, {.hp = 10, .dmg = 5});

    Unit *u = ecs_get_mut(world, e, Unit);
    test_assert(u != NULL);
    u->hp = 20;

    const Warrior *w = ecs_get(world, e, Warrior);
    test_assert(w != NULL);
    test_int(w->hp, 20);

    ecs_fini(world);
}

void ComponentInheritance_each_base_matches_derived(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);

    ecs_entity_t w1 = ecs_new_w_id(world, Warrior);
    ecs_entity_t w2 = ecs_new_w_id(world, Warrior);
    ecs_entity_t u1 = ecs_new_w_id(world, Unit);

    bool found_w1 = false, found_w2 = false, found_u1 = false;
    int32_t total = 0;

    ecs_iter_t it = ecs_each_id(world, Unit);
    while (ecs_each_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            ecs_entity_t e = it.entities[i];
            total ++;
            if (e == w1) found_w1 = true;
            if (e == w2) found_w2 = true;
            if (e == u1) found_u1 = true;
        }
    }

    test_int(total, 3);
    test_bool(found_w1, true);
    test_bool(found_w2, true);
    test_bool(found_u1, true);

    ecs_fini(world);
}

void ComponentInheritance_count_base_counts_derived(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Melee);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Melee, EcsIsA, Unit);
    ecs_add_pair(world, Warrior, EcsIsA, Melee);

    ecs_new_w_id(world, Warrior);
    ecs_new_w_id(world, Melee);
    ecs_new_w_id(world, Unit);

    test_int(ecs_count_id(world, Unit), 3);
    test_int(ecs_count_id(world, Melee), 2);
    test_int(ecs_count_id(world, Warrior), 1);

    ecs_fini(world);
}

void ComponentInheritance_ensure_via_base(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Unit);
    ECS_COMPONENT(world, Warrior);

    ecs_add_pair(world, ecs_id(Warrior), EcsIsA, ecs_id(Unit));

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Warrior, {.hp = 10, .dmg = 5});

    Unit *u = ecs_ensure_id(world, e, ecs_id(Unit), sizeof(Unit));
    test_assert(u != NULL);
    test_bool(ecs_owns_id(world, e, ecs_id(Unit)), true);
    test_bool(ecs_owns_id(world, e, ecs_id(Warrior)), true);
    test_assert((void*)u != (void*)ecs_get(world, e, Warrior));

    const Warrior *war = ecs_get(world, e, Warrior);
    test_assert(war != NULL);
    test_int(war->hp, 10);

    ecs_fini(world);
}

void ComponentInheritance_id_in_use_base(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);

    ecs_new_w_id(world, Warrior);

    test_bool(ecs_id_in_use(world, Unit), true);
    test_bool(ecs_id_in_use(world, Warrior), true);

    ecs_fini(world);
}

void ComponentInheritance_clone_derived(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Unit);
    ECS_COMPONENT(world, Warrior);

    ecs_add_pair(world, ecs_id(Warrior), EcsIsA, ecs_id(Unit));

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Warrior, {.hp = 10, .dmg = 5});

    ecs_entity_t c = ecs_clone(world, 0, e, true);

    test_bool(ecs_has_id(world, c, ecs_id(Warrior)), true);
    test_bool(ecs_has_id(world, c, ecs_id(Unit)), true);

    const Unit *u = ecs_get(world, c, Unit);
    test_assert(u != NULL);
    test_int(u->hp, 10);

    ecs_fini(world);
}

void ComponentInheritance_low_id_base(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Unit);
    ECS_COMPONENT(world, Warrior);

    test_assert(ecs_id(Unit) < FLECS_HI_COMPONENT_ID);
    test_assert(ecs_id(Warrior) < FLECS_HI_COMPONENT_ID);

    ecs_add_pair(world, ecs_id(Warrior), EcsIsA, ecs_id(Unit));

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Warrior, {.hp = 10, .dmg = 5});

    test_bool(ecs_has_id(world, e, ecs_id(Unit)), true);
    test_bool(ecs_owns_id(world, e, ecs_id(Unit)), true);

    const Unit *u = ecs_get(world, e, Unit);
    test_assert(u != NULL);
    test_int(u->hp, 10);

    ecs_id_t id_out = 0;
    ecs_table_t *table = ecs_get_table(world, e);
    test_assert(ecs_search(world, table, ecs_id(Unit), &id_out) != -1);
    test_uint(id_out, ecs_id(Warrior));

    ecs_fini(world);
}

void ComponentInheritance_recycled_base(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t u = ecs_new(world);
    ecs_delete(world, u);
    test_bool(ecs_is_alive(world, u), false);

    ecs_entity_t Unit = ecs_new(world);
    test_assert((uint32_t)Unit == (uint32_t)u);
    test_assert(Unit != u);

    ecs_entity_t Warrior = ecs_new(world);
    ecs_add_pair(world, Warrior, EcsIsA, Unit);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);

    test_bool(ecs_has_id(world, e, Unit), true);
    test_bool(ecs_owns_id(world, e, Unit), true);

    ecs_id_t id_out = 0;
    test_assert(ecs_search(world, ecs_get_table(world, e), Unit, &id_out) != -1);
    test_uint(id_out, Warrior);

    ecs_fini(world);
}

void ComponentInheritance_readd_isa_after_remove(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);
    ecs_entity_t e = ecs_new_w_id(world, Warrior);
    test_bool(ecs_has_id(world, e, Unit), true);

    test_expect_abort();
    ecs_remove_pair(world, Warrior, EcsIsA, Unit);
}

void ComponentInheritance_change_grandparent_isa(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Super);
    ECS_TAG(world, Melee);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Melee, EcsIsA, Unit);
    ecs_add_pair(world, Warrior, EcsIsA, Melee);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);
    test_bool(ecs_has_id(world, e, Unit), true);
    test_bool(ecs_has_id(world, e, Melee), true);

    test_expect_abort();
    ecs_remove_pair(world, Melee, EcsIsA, Unit);
}

void ComponentInheritance_reparent_isa(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Melee);
    ECS_TAG(world, Ranged);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Melee, EcsIsA, Unit);
    ecs_add_pair(world, Ranged, EcsIsA, Unit);
    ecs_add_pair(world, Warrior, EcsIsA, Melee);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);
    test_bool(ecs_has_id(world, e, Melee), true);
    test_bool(ecs_has_id(world, e, Ranged), false);
    test_bool(ecs_has_id(world, e, Unit), true);

    test_expect_abort();
    ecs_remove_pair(world, Warrior, EcsIsA, Melee);
}

void ComponentInheritance_add_second_base_after_populate(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);
    ECS_TAG(world, Wizard);
    ECS_TAG(world, Warlock);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);
    ecs_add_pair(world, Wizard, EcsIsA, Unit);
    ecs_add_pair(world, Warlock, EcsIsA, Warrior);

    ecs_entity_t e = ecs_new_w_id(world, Warlock);
    test_bool(ecs_has_id(world, e, Warrior), true);
    test_bool(ecs_has_id(world, e, Unit), true);
    test_bool(ecs_has_id(world, e, Wizard), false);

    test_expect_abort();
    ecs_add_pair(world, Warlock, EcsIsA, Wizard);
}

void ComponentInheritance_remove_second_base_after_populate(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);
    ECS_TAG(world, Wizard);
    ECS_TAG(world, Warlock);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);
    ecs_add_pair(world, Wizard, EcsIsA, Unit);
    ecs_add_pair(world, Warlock, EcsIsA, Warrior);
    ecs_add_pair(world, Warlock, EcsIsA, Wizard);

    ecs_entity_t e = ecs_new_w_id(world, Warlock);
    test_bool(ecs_has_id(world, e, Unit), true);
    test_bool(ecs_has_id(world, e, Warrior), true);
    test_bool(ecs_has_id(world, e, Wizard), true);

    test_expect_abort();
    ecs_remove_pair(world, Warlock, EcsIsA, Warrior);
}

void ComponentInheritance_add_isa_4_lvl_after_populate(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, A);
    ECS_TAG(world, B);
    ECS_TAG(world, C);
    ECS_TAG(world, D);

    ecs_new_w_id(world, D);

    ecs_add_pair(world, B, EcsIsA, A);
    ecs_add_pair(world, C, EcsIsA, B);

    test_expect_abort();
    ecs_add_pair(world, D, EcsIsA, C);
}

void ComponentInheritance_wide_hierarchy(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, S1);
    ECS_TAG(world, S2);
    ECS_TAG(world, S3);
    ECS_TAG(world, S4);

    ecs_add_pair(world, S1, EcsIsA, Unit);
    ecs_add_pair(world, S2, EcsIsA, Unit);
    ecs_add_pair(world, S3, EcsIsA, Unit);
    ecs_add_pair(world, S4, EcsIsA, Unit);

    ecs_entity_t e1 = ecs_new_w_id(world, S1);
    ecs_entity_t e2 = ecs_new_w_id(world, S2);
    ecs_entity_t e3 = ecs_new_w_id(world, S3);
    ecs_entity_t e4 = ecs_new_w_id(world, S4);

    test_bool(ecs_has_id(world, e1, Unit), true);
    test_bool(ecs_has_id(world, e2, Unit), true);
    test_bool(ecs_has_id(world, e3, Unit), true);
    test_bool(ecs_has_id(world, e4, Unit), true);
    test_int(ecs_count_id(world, Unit), 4);

    ecs_fini(world);
}

void ComponentInheritance_change_isa_deferred(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);
    test_bool(ecs_has_id(world, e, Unit), false);

    ecs_defer_begin(world);
    ecs_add_pair(world, Warrior, EcsIsA, Unit);
    test_bool(ecs_has_id(world, e, Unit), false);

    test_expect_abort();
    ecs_defer_end(world);
}

void ComponentInheritance_cyclic_isa_not_allowed(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, A);
    ECS_TAG(world, B);

    ecs_add_pair(world, A, EcsIsA, B);

    test_expect_abort();
    ecs_add_pair(world, B, EcsIsA, A);
}

void ComponentInheritance_has_tag_base(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);
    ecs_entity_t e = ecs_new_w_id(world, Warrior);

    test_bool(ecs_has_id(world, e, Unit), true);
    test_bool(ecs_owns_id(world, e, Unit), true);

    ecs_id_t id_out = 0;
    test_assert(ecs_search(world, ecs_get_table(world, e), Unit, &id_out) != -1);
    test_uint(id_out, Warrior);

    ecs_fini(world);
}

void ComponentInheritance_get_tag_base_asserts(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);
    ecs_entity_t e = ecs_new_w_id(world, Warrior);

    test_expect_abort();
    ecs_get_id(world, e, Unit);
}

void ComponentInheritance_get_tag_base_w_data_derived(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Unit = ecs_entity(world, { .name = "Unit" });

    ECS_COMPONENT(world, Warrior);
    ecs_add_pair(world, ecs_id(Warrior), EcsIsA, Unit);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Warrior, {.hp = 77, .dmg = 1});

    const Warrior *w = ecs_get_id(world, e, Unit);
    test_assert(w != NULL);
    test_int(w->hp, 77);
    test_assert((const void*)w == ecs_get(world, e, Warrior));

    ecs_fini(world);
}

void ComponentInheritance_delete_base_component_entity(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);
    ecs_new_w_id(world, Warrior);

    test_expect_abort();
    ecs_delete(world, Unit);
}

void ComponentInheritance_delete_derived_component_entity(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);
    ecs_entity_t e = ecs_new_w_id(world, Warrior);

    ecs_delete(world, Warrior);

    test_bool(ecs_is_alive(world, Warrior), false);
    test_bool(ecs_is_alive(world, Unit), true);
    test_bool(ecs_is_alive(world, e), true);
    test_bool(ecs_has_id(world, e, Unit), false);

    ecs_fini(world);
}

void ComponentInheritance_clear_with_derived(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);
    ecs_entity_t e = ecs_new_w_id(world, Warrior);
    test_bool(ecs_has_id(world, e, Unit), true);

    ecs_clear(world, e);

    test_bool(ecs_is_alive(world, e), true);
    test_bool(ecs_has_id(world, e, Warrior), false);
    test_bool(ecs_has_id(world, e, Unit), false);

    ecs_fini(world);
}

void ComponentInheritance_add_base_to_derived(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);
    ecs_add_id(world, e, Unit);

    test_bool(ecs_has_id(world, e, Unit), true);
    test_bool(ecs_has_id(world, e, Warrior), true);
    test_bool(ecs_owns_id(world, e, Unit), true);
    test_bool(ecs_owns_id(world, e, Warrior), true);

    ecs_fini(world);
}

void ComponentInheritance_multi_derived_has(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);
    ECS_TAG(world, Wizard);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);
    ecs_add_pair(world, Wizard, EcsIsA, Unit);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);
    ecs_add_id(world, e, Wizard);

    test_bool(ecs_has_id(world, e, Warrior), true);
    test_bool(ecs_has_id(world, e, Wizard), true);
    test_bool(ecs_has_id(world, e, Unit), true);
    test_bool(ecs_owns_id(world, e, Warrior), true);
    test_bool(ecs_owns_id(world, e, Wizard), true);
    test_bool(ecs_owns_id(world, e, Unit), true);

    ecs_fini(world);
}

void ComponentInheritance_multi_derived_search_first(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);
    ECS_TAG(world, Wizard);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);
    ecs_add_pair(world, Wizard, EcsIsA, Unit);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);
    ecs_add_id(world, e, Wizard);

    ecs_table_t *table = ecs_get_table(world, e);
    test_assert(table != NULL);

    ecs_id_t id_out = 0;
    int32_t col = ecs_search(world, table, Unit, &id_out);
    test_assert(col != -1);
    test_uint(id_out, Warrior);

    ecs_fini(world);
}

void ComponentInheritance_multi_derived_count(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);
    ECS_TAG(world, Wizard);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);
    ecs_add_pair(world, Wizard, EcsIsA, Unit);

    ecs_entity_t e1 = ecs_new_w_id(world, Warrior);
    ecs_add_id(world, e1, Wizard);

    ecs_new_w_id(world, Warrior);

    test_int(ecs_count_id(world, Unit), 3);
    test_int(ecs_count_id(world, Warrior), 2);
    test_int(ecs_count_id(world, Wizard), 1);

    ecs_fini(world);
}

void ComponentInheritance_multi_derived_each_once(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);
    ECS_TAG(world, Wizard);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);
    ecs_add_pair(world, Wizard, EcsIsA, Unit);

    ecs_entity_t e1 = ecs_new_w_id(world, Warrior);
    ecs_add_id(world, e1, Wizard);
    ecs_entity_t e2 = ecs_new_w_id(world, Warrior);

    bool found_e1 = false, found_e2 = false;
    int32_t total = 0;

    ecs_iter_t it = ecs_each_id(world, Unit);
    while (ecs_each_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            ecs_entity_t e = it.entities[i];
            total ++;
            if (e == e1) found_e1 = true;
            if (e == e2) found_e2 = true;
        }
    }

    test_int(total, 2);
    test_bool(found_e1, true);
    test_bool(found_e2, true);

    ecs_fini(world);
}

void ComponentInheritance_multi_derived_get_returns_first(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Unit);
    ECS_COMPONENT(world, Warrior);
    ECS_COMPONENT(world, Mage);

    ecs_add_pair(world, ecs_id(Warrior), EcsIsA, ecs_id(Unit));
    ecs_add_pair(world, ecs_id(Mage), EcsIsA, ecs_id(Unit));

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Warrior, {.hp = 10, .dmg = 5});
    ecs_set(world, e, Mage, {.hp = 20, .mana = 7});

    const Unit *u = ecs_get(world, e, Unit);
    test_assert(u != NULL);
    test_int(u->hp, 10);
    test_assert((const void*)u == (const void*)ecs_get(world, e, Warrior));

    ecs_fini(world);
}

void ComponentInheritance_multi_derived_remove_one_keeps_base(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);
    ECS_TAG(world, Wizard);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);
    ecs_add_pair(world, Wizard, EcsIsA, Unit);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);
    ecs_add_id(world, e, Wizard);
    test_bool(ecs_has_id(world, e, Unit), true);

    ecs_remove_id(world, e, Wizard);

    test_bool(ecs_has_id(world, e, Wizard), false);
    test_bool(ecs_has_id(world, e, Warrior), true);
    test_bool(ecs_has_id(world, e, Unit), true);
    test_int(ecs_count_id(world, Unit), 1);

    ecs_fini(world);
}

void ComponentInheritance_multi_derived_remove_both_loses_base(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);
    ECS_TAG(world, Wizard);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);
    ecs_add_pair(world, Wizard, EcsIsA, Unit);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);
    ecs_add_id(world, e, Wizard);
    test_bool(ecs_has_id(world, e, Unit), true);

    ecs_remove_id(world, e, Warrior);
    test_bool(ecs_has_id(world, e, Unit), true);

    ecs_remove_id(world, e, Wizard);
    test_bool(ecs_has_id(world, e, Unit), false);
    test_bool(ecs_owns_id(world, e, Unit), false);

    ecs_fini(world);
}

void ComponentInheritance_multi_derived_3_lvl_mixed(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Melee);
    ECS_TAG(world, Warrior);
    ECS_TAG(world, Wizard);

    ecs_add_pair(world, Melee, EcsIsA, Unit);
    ecs_add_pair(world, Warrior, EcsIsA, Melee);
    ecs_add_pair(world, Wizard, EcsIsA, Unit);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);
    ecs_add_id(world, e, Wizard);

    test_bool(ecs_has_id(world, e, Unit), true);
    test_bool(ecs_has_id(world, e, Melee), true);
    test_int(ecs_count_id(world, Unit), 2);
    test_int(ecs_count_id(world, Melee), 1);

    ecs_fini(world);
}

void ComponentInheritance_multi_derived_add_isa_after_populate(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);
    ECS_TAG(world, Wizard);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);
    ecs_add_id(world, e, Wizard);

    test_bool(ecs_has_id(world, e, Unit), false);

    test_expect_abort();
    ecs_add_pair(world, Warrior, EcsIsA, Unit);
}

void ComponentInheritance_multi_derived_delete_with(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);
    ECS_TAG(world, Wizard);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);
    ecs_add_pair(world, Wizard, EcsIsA, Unit);

    ecs_entity_t e1 = ecs_new_w_id(world, Warrior);
    ecs_add_id(world, e1, Wizard);
    ecs_entity_t e2 = ecs_new_w_id(world, Wizard);

    ecs_delete_with(world, Unit);

    test_bool(ecs_is_alive(world, e1), false);
    test_bool(ecs_is_alive(world, e2), false);

    ecs_fini(world);
}

void ComponentInheritance_multi_derived_remove_all(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);
    ECS_TAG(world, Wizard);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);
    ecs_add_pair(world, Wizard, EcsIsA, Unit);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);
    ecs_add_id(world, e, Wizard);

    ecs_remove_all(world, Unit);

    test_bool(ecs_is_alive(world, e), true);
    test_bool(ecs_has_id(world, e, Warrior), false);
    test_bool(ecs_has_id(world, e, Wizard), false);
    test_bool(ecs_has_id(world, e, Unit), false);

    ecs_fini(world);
}

void ComponentInheritance_multi_derived_clone(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Unit);
    ECS_COMPONENT(world, Warrior);
    ECS_COMPONENT(world, Mage);

    ecs_add_pair(world, ecs_id(Warrior), EcsIsA, ecs_id(Unit));
    ecs_add_pair(world, ecs_id(Mage), EcsIsA, ecs_id(Unit));

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Warrior, {.hp = 10, .dmg = 5});
    ecs_set(world, e, Mage, {.hp = 20, .mana = 7});

    ecs_entity_t c = ecs_clone(world, 0, e, true);

    test_bool(ecs_has_id(world, c, ecs_id(Warrior)), true);
    test_bool(ecs_has_id(world, c, ecs_id(Mage)), true);
    test_bool(ecs_has_id(world, c, ecs_id(Unit)), true);

    const Warrior *w = ecs_get(world, c, Warrior);
    test_assert(w != NULL);
    test_int(w->hp, 10);
    test_int(w->dmg, 5);

    const Mage *m = ecs_get(world, c, Mage);
    test_assert(m != NULL);
    test_int(m->hp, 20);
    test_int(m->mana, 7);

    ecs_fini(world);
}

void ComponentInheritance_multi_derived_three_bases(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, S1);
    ECS_TAG(world, S2);
    ECS_TAG(world, S3);

    ecs_add_pair(world, S1, EcsIsA, Unit);
    ecs_add_pair(world, S2, EcsIsA, Unit);
    ecs_add_pair(world, S3, EcsIsA, Unit);

    ecs_entity_t e = ecs_new_w_id(world, S1);
    ecs_add_id(world, e, S2);
    ecs_add_id(world, e, S3);

    test_bool(ecs_has_id(world, e, Unit), true);
    test_int(ecs_count_id(world, Unit), 3);

    ecs_id_t id_out = 0;
    test_assert(ecs_search(world, ecs_get_table(world, e), Unit, &id_out) != -1);
    test_uint(id_out, S1);

    ecs_fini(world);
}

void ComponentInheritance_multi_derived_component_diamond_plus_extra(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);
    ECS_TAG(world, Wizard);
    ECS_TAG(world, Warlock);
    ECS_TAG(world, Mage);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);
    ecs_add_pair(world, Wizard, EcsIsA, Unit);
    ecs_add_pair(world, Warlock, EcsIsA, Warrior);
    ecs_add_pair(world, Warlock, EcsIsA, Wizard);
    ecs_add_pair(world, Mage, EcsIsA, Unit);

    ecs_entity_t e = ecs_new_w_id(world, Warlock);
    ecs_add_id(world, e, Mage);

    test_bool(ecs_has_id(world, e, Unit), true);
    test_int(ecs_count_id(world, Unit), 2);

    ecs_fini(world);
}

void ComponentInheritance_multi_derived_id_in_use(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);
    ECS_TAG(world, Wizard);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);
    ecs_add_pair(world, Wizard, EcsIsA, Unit);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);
    ecs_add_id(world, e, Wizard);

    test_bool(ecs_id_in_use(world, Unit), true);

    ecs_fini(world);
}

void ComponentInheritance_multi_derived_deferred(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Unit);
    ECS_TAG(world, Warrior);
    ECS_TAG(world, Wizard);

    ecs_add_pair(world, Warrior, EcsIsA, Unit);
    ecs_add_pair(world, Wizard, EcsIsA, Unit);

    ecs_entity_t e = ecs_new_w_id(world, Warrior);

    ecs_defer_begin(world);
    ecs_add_id(world, e, Wizard);
    ecs_defer_end(world);

    test_bool(ecs_has_id(world, e, Warrior), true);
    test_bool(ecs_has_id(world, e, Wizard), true);
    test_bool(ecs_has_id(world, e, Unit), true);
    test_int(ecs_count_id(world, Unit), 2);

    ecs_fini(world);
}

void ComponentInheritance_pair_rel_has(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Loves);
    ECS_TAG(world, Apples);

    ecs_add_pair(world, Loves, EcsIsA, Likes);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Loves, Apples);

    test_bool(ecs_has_pair(world, e, Loves, Apples), true);
    test_bool(ecs_has_pair(world, e, Likes, Apples), true);

    ecs_fini(world);
}

void ComponentInheritance_pair_rel_3_lvl(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Loves);
    ECS_TAG(world, Worships);
    ECS_TAG(world, Apples);

    ecs_add_pair(world, Loves, EcsIsA, Likes);
    ecs_add_pair(world, Worships, EcsIsA, Loves);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Worships, Apples);

    test_bool(ecs_has_pair(world, e, Worships, Apples), true);
    test_bool(ecs_has_pair(world, e, Loves, Apples), true);
    test_bool(ecs_has_pair(world, e, Likes, Apples), true);

    ecs_fini(world);
}

void ComponentInheritance_pair_rel_search(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Loves);
    ECS_TAG(world, Apples);

    ecs_add_pair(world, Loves, EcsIsA, Likes);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Loves, Apples);

    ecs_id_t id_out = 0;
    ecs_table_t *table = ecs_get_table(world, e);
    test_assert(ecs_search(world, table, ecs_pair(Likes, Apples), &id_out) != -1);
    test_uint(id_out, ecs_pair(Loves, Apples));

    ecs_fini(world);
}

void ComponentInheritance_pair_rel_unrelated(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Loves);
    ECS_TAG(world, Eats);
    ECS_TAG(world, Apples);

    ecs_add_pair(world, Loves, EcsIsA, Likes);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Loves, Apples);

    test_bool(ecs_has_pair(world, e, Likes, Apples), true);
    test_bool(ecs_has_pair(world, e, Eats, Apples), false);

    ecs_fini(world);
}

void ComponentInheritance_pair_rel_target_not_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Fruit);
    ECS_TAG(world, Apples);

    ecs_add_pair(world, Apples, EcsIsA, Fruit);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Likes, Apples);

    test_bool(ecs_has_pair(world, e, Likes, Apples), true);
    test_bool(ecs_has_pair(world, e, Likes, Fruit), false);

    ecs_fini(world);
}

void ComponentInheritance_pair_rel_different_target(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Loves);
    ECS_TAG(world, Apples);
    ECS_TAG(world, Pears);

    ecs_add_pair(world, Loves, EcsIsA, Likes);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Loves, Apples);

    test_bool(ecs_has_pair(world, e, Likes, Apples), true);
    test_bool(ecs_has_pair(world, e, Likes, Pears), false);

    ecs_fini(world);
}

void ComponentInheritance_pair_rel_add_isa_after(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Loves);
    ECS_TAG(world, Apples);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Loves, Apples);
    test_bool(ecs_has_pair(world, e, Likes, Apples), false);

    test_expect_abort();
    ecs_add_pair(world, Loves, EcsIsA, Likes);
}

void ComponentInheritance_pair_rel_remove_isa(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Loves);
    ECS_TAG(world, Apples);

    ecs_add_pair(world, Loves, EcsIsA, Likes);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Loves, Apples);
    test_bool(ecs_has_pair(world, e, Likes, Apples), true);

    test_expect_abort();
    ecs_remove_pair(world, Loves, EcsIsA, Likes);
}

void ComponentInheritance_pair_rel_multi(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Loves);
    ECS_TAG(world, Adores);
    ECS_TAG(world, Apples);

    ecs_add_pair(world, Loves, EcsIsA, Likes);
    ecs_add_pair(world, Adores, EcsIsA, Likes);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Loves, Apples);
    ecs_add_pair(world, e, Adores, Apples);

    test_bool(ecs_has_pair(world, e, Likes, Apples), true);
    test_int(ecs_count_id(world, ecs_pair(Likes, Apples)), 2);

    ecs_fini(world);
}

void ComponentInheritance_pair_rel_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Loves);
    ECS_TAG(world, Apples);
    ECS_TAG(world, Pears);

    ecs_add_pair(world, Loves, EcsIsA, Likes);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Loves, Apples);
    ecs_add_pair(world, e, Loves, Pears);

    test_bool(ecs_has_id(world, e, ecs_pair(Likes, EcsWildcard)), true);
    test_int(ecs_count_id(world, ecs_pair(Likes, EcsWildcard)), 2);

    ecs_fini(world);
}

void ComponentInheritance_pair_rel_wildcard_unrelated(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Loves);
    ECS_TAG(world, Eats);
    ECS_TAG(world, Apples);

    ecs_add_pair(world, Loves, EcsIsA, Likes);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Loves, Apples);

    test_bool(ecs_has_id(world, e, ecs_pair(Likes, EcsWildcard)), true);
    test_bool(ecs_has_id(world, e, ecs_pair(Eats, EcsWildcard)), false);

    ecs_fini(world);
}

void ComponentInheritance_depth_w_derived_pairs_non_contiguous(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, DerivedA);
    ECS_TAG(world, Other);
    ECS_TAG(world, DerivedB);

    ecs_add_id(world, Rel, EcsAcyclic);
    ecs_add_id(world, Rel, EcsTraversable);
    ecs_add_pair(world, DerivedA, EcsIsA, Rel);
    ecs_add_pair(world, DerivedB, EcsIsA, Rel);

    ecs_entity_t pa = ecs_new(world);
    ecs_entity_t po = ecs_new(world);
    ecs_entity_t pb = ecs_new(world);
    ecs_entity_t pb_parent = ecs_new(world);
    ecs_add_pair(world, pb, Rel, pb_parent);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, DerivedA, pa);
    ecs_add_pair(world, e, Other, po);
    ecs_add_pair(world, e, DerivedB, pb);

    test_int(2, ecs_get_depth(world, e, Rel));

    ecs_fini(world);
}
