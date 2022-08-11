#include <addons.h>

#define test_term_id(it, column_id, str) {\
    ecs_id_t _term_id = ecs_field_id(it, column_id);\
    test_assert(_term_id != 0);\
    char *_term_id_str = ecs_id_str((it)->world, _term_id);\
    test_str(_term_id_str, str);\
    ecs_os_free(_term_id_str);\
}

#define test_term_source(it, column_id, str) {\
    ecs_entity_t _term_source = ecs_field_src(it, column_id);\
    test_assert(_term_source != 0);\
    char *_term_source_str = ecs_id_str((it)->world, _term_source);\
    test_str(_term_source_str, str);\
    ecs_os_free(_term_source_str);\
}

#define test_var(it, var_id, str) {\
    ecs_entity_t _var_e = ecs_iter_get_var(it, var_id);\
    test_assert(_var_e != 0);\
    char *_var_str = ecs_id_str((it)->world, _var_e);\
    test_str(_var_str, str);\
    ecs_os_free(_var_str);\
}

void Rules_empty_rule() {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_mini();

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){ 0 });

    test_assert(r == NULL);

    ecs_fini(world);
}

void Rules_invalid_rule() {
    ecs_log_set_level(-4);
    
    ecs_world_t *world = ecs_mini();

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Foo"
    });

    test_assert(r == NULL);

    ecs_fini(world);
}

void Rules_comp_recycled_id() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t dummy = ecs_new_id(world);
    test_assert(dummy != 0);

    ecs_delete(world, dummy);
    test_assert(!ecs_is_alive(world, dummy));

    ECS_ENTITY(world, Tag, 0);

    /* Ensure recycled id */
    test_assert(Tag != (uint32_t)Tag);

    ecs_entity_t e = ecs_new_w_id(world, Tag);
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, Tag));

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Tag"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);

    test_term_id(&it, 1, "Tag");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_comp_recycled_final_id() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t dummy = ecs_new_id(world);
    test_assert(dummy != 0);

    ecs_delete(world, dummy);
    test_assert(!ecs_is_alive(world, dummy));

    ECS_ENTITY(world, Tag, Final);

    /* Ensure recycled id */
    test_assert(Tag != (uint32_t)Tag);

    ecs_entity_t e = ecs_new_w_id(world, Tag);
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, Tag));

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Tag"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_term_id(&it, 1, "Tag");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_pair_recycled_final_pred() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Obj, 0);

    ecs_entity_t dummy = ecs_new_id(world);
    test_assert(dummy != 0);
    ecs_delete(world, dummy);
    test_assert(!ecs_is_alive(world, dummy));

    ECS_ENTITY(world, Pred, Final);

    /* Ensure recycled id */
    test_assert(Pred != (uint32_t)Pred);

    ecs_entity_t e = ecs_new_w_id(world, ecs_pair(Pred, Obj));
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, ecs_pair(Pred, Obj)));

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Pred($This, $X)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_term_id(&it, 1, "(Pred,Obj)");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_pair_recycled_pred() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Obj, 0);

    ecs_entity_t dummy = ecs_new_id(world);
    test_assert(dummy != 0);
    ecs_delete(world, dummy);
    test_assert(!ecs_is_alive(world, dummy));

    ECS_ENTITY(world, Pred, 0);

    /* Ensure recycled id */
    test_assert(Pred != (uint32_t)Pred);

    ecs_entity_t e = ecs_new_w_id(world, ecs_pair(Pred, Obj));
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, ecs_pair(Pred, Obj)));

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Pred($This, $X)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_term_id(&it, 1, "(Pred,Obj)");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_pair_recycled_obj() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Pred, 0);

    ecs_entity_t dummy = ecs_new_id(world);
    test_assert(dummy != 0);
    ecs_delete(world, dummy);
    test_assert(!ecs_is_alive(world, dummy));

    ECS_ENTITY(world, Obj, 0);

    /* Ensure recycled id */
    test_assert(Obj != (uint32_t)Obj);

    ecs_entity_t e = ecs_new_w_id(world, ecs_pair(Pred, Obj));
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, ecs_pair(Pred, Obj)));

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Pred($This, Obj)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_term_id(&it, 1, "(Pred,Obj)");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_pair_recycled_pred_obj() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t dummy = ecs_new_id(world);
    test_assert(dummy != 0);
    ecs_delete(world, dummy);
    test_assert(!ecs_is_alive(world, dummy));
    ECS_ENTITY(world, Pred, 0);

    dummy = ecs_new_id(world);
    test_assert(dummy != 0);
    ecs_delete(world, dummy);
    test_assert(!ecs_is_alive(world, dummy));
    ECS_ENTITY(world, Obj, 0);

    /* Ensure recycled id */
    test_assert(Pred != (uint32_t)Pred);
    test_assert(Obj != (uint32_t)Obj);

    ecs_entity_t e = ecs_new_w_id(world, ecs_pair(Pred, Obj));
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, ecs_pair(Pred, Obj)));

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Pred($This, Obj)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_term_id(&it, 1, "(Pred,Obj)");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_pair_recycled_matched_obj() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Pred, 0);

    ecs_entity_t dummy = ecs_new_id(world);
    test_assert(dummy != 0);
    ecs_delete(world, dummy);
    test_assert(!ecs_is_alive(world, dummy));

    ECS_ENTITY(world, Obj, 0);

    /* Ensure recycled id */
    test_assert(Obj != (uint32_t)Obj);

    ecs_entity_t e = ecs_new_w_id(world, ecs_pair(Pred, Obj));
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, ecs_pair(Pred, Obj)));

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Pred($This, $X)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_term_id(&it, 1, "(Pred,Obj)");
    test_var(&it, x_var, "Obj");

    test_assert(!ecs_rule_next(&it));
    
    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_pair_recycled_matched_obj_2_terms() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Pred, 0);
    ECS_ENTITY(world, Pred_2, 0);

    ecs_entity_t dummy = ecs_new_id(world);
    test_assert(dummy != 0);
    ecs_delete(world, dummy);
    test_assert(!ecs_is_alive(world, dummy));

    ECS_ENTITY(world, Obj, 0);

    /* Ensure recycled id */
    test_assert(Obj != (uint32_t)Obj);

    ecs_entity_t e = ecs_new_w_id(world, ecs_pair(Pred, Obj));
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, ecs_pair(Pred, Obj)));

    ecs_add_id(world, e, ecs_pair(Pred_2, Obj));
    test_assert(ecs_has_id(world, e, ecs_pair(Pred_2, Obj)));

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Pred($This, $X), Pred_2($This, $X)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_term_id(&it, 1, "(Pred,Obj)");
    test_term_id(&it, 2, "(Pred_2,Obj)");
    test_var(&it, x_var, "Obj");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_pair_recycled_matched_pred_2_terms() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Obj, 0);
    ECS_ENTITY(world, Obj_2, 0);

    ecs_entity_t dummy = ecs_new_id(world);
    test_assert(dummy != 0);
    ecs_delete(world, dummy);
    test_assert(!ecs_is_alive(world, dummy));

    ECS_ENTITY(world, Pred, 0);

    /* Ensure recycled id */
    test_assert(Pred != (uint32_t)Pred);

    ecs_entity_t e = ecs_new_w_id(world, ecs_pair(Pred, Obj));
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, ecs_pair(Pred, Obj)));

    ecs_add_id(world, e, ecs_pair(Pred, Obj_2));
    test_assert(ecs_has_id(world, e, ecs_pair(Pred, Obj_2)));

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "$X($This, Obj), $X($This, Obj_2)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_term_id(&it, 1, "(Pred,Obj)");
    test_term_id(&it, 2, "(Pred,Obj_2)");
    test_var(&it, x_var, "Pred");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_recycled_var() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, Final);
    ECS_TAG(world, Tag);

    ecs_entity_t obj_a = ecs_new_id(world);
    ecs_entity_t obj_b = ecs_new_id(world);
    ecs_delete(world, obj_b);
    ecs_entity_t obj_b_2 = ecs_new_id(world);
    test_assert(obj_b != obj_b_2);
    test_assert(ecs_strip_generation(obj_b) == ecs_strip_generation(obj_b_2));

    ecs_add(world, obj_b_2, Tag); /* make sure it's not an empty table */

    ecs_add_pair(world, obj_a, Rel, obj_b_2);

    ecs_rule_t *r = ecs_rule_new(world, "(Rel, $X)");
    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(true, ecs_rule_next(&it));
    test_int(1, it.count);
    test_uint(obj_a, it.entities[0]);
    test_uint(ecs_pair(Rel, obj_b_2), ecs_field_id(&it, 1));
    
    ecs_entity_t x = ecs_iter_get_var(&it, x_var);
    test_assert(x != 0);
    test_assert(ecs_is_alive(world, x));
    test_assert(x == obj_b_2);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}


void Rules_superset_from_recycled() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, Transitive, Final);
    ECS_TAG(world, Tag);

    ecs_entity_t obj_a = ecs_new_id(world);
    ecs_entity_t obj_b = ecs_new_id(world);
    ecs_delete(world, obj_b);
    ecs_entity_t obj_b_2 = ecs_new_id(world);
    test_assert(obj_b != obj_b_2);
    test_assert(ecs_strip_generation(obj_b) == ecs_strip_generation(obj_b_2));

    ecs_add(world, obj_b_2, Tag); /* make sure it's not an empty table */

    ecs_add_pair(world, obj_a, Rel, obj_b_2);

    ecs_rule_t *r = ecs_rule_new(world, "(Rel, *)");
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(true, ecs_rule_next(&it));
    test_int(1, it.count);
    test_uint(obj_a, it.entities[0]);
    test_uint(ecs_pair(Rel, obj_b_2), ecs_field_id(&it, 1));

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_superset_from_recycled_w_var() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, Transitive, Final);
    ECS_TAG(world, Tag);

    ecs_entity_t obj_a = ecs_new_id(world);
    ecs_entity_t obj_b = ecs_new_id(world);
    ecs_delete(world, obj_b);
    ecs_entity_t obj_b_2 = ecs_new_id(world);
    test_assert(obj_b != obj_b_2);
    test_assert(ecs_strip_generation(obj_b) == ecs_strip_generation(obj_b_2));

    ecs_add(world, obj_b_2, Tag); /* make sure it's not an empty table */

    ecs_add_pair(world, obj_a, Rel, obj_b_2);

    ecs_rule_t *r = ecs_rule_new(world, "(Rel, $X)");
    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(true, ecs_rule_next(&it));
    test_int(1, it.count);
    test_uint(obj_a, it.entities[0]);
    test_uint(ecs_pair(Rel, obj_b_2), ecs_field_id(&it, 1));

    ecs_entity_t x = ecs_iter_get_var(&it, x_var);
    test_assert(x != 0);
    test_assert(ecs_is_alive(world, x));
    test_assert(x == obj_b_2);
    
    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_superset_from_recycled_2_lvls_w_var() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, Transitive, Final);
    ECS_TAG(world, Tag);

    ecs_entity_t obj_a = ecs_new_id(world);
    ecs_entity_t obj_b = ecs_new_id(world);
    ecs_entity_t obj_c = ecs_new_id(world);
    ecs_delete(world, obj_b);
    ecs_entity_t obj_b_2 = ecs_new_id(world);
    ecs_delete(world, obj_c);
    ecs_entity_t obj_c_2 = ecs_new_id(world);
    test_assert(obj_b != obj_b_2);
    test_assert(obj_c != obj_c_2);
    test_assert(ecs_strip_generation(obj_b) == ecs_strip_generation(obj_b_2));
    test_assert(ecs_strip_generation(obj_c) == ecs_strip_generation(obj_c_2));

    ecs_add(world, obj_b_2, Tag); /* make sure it's not an empty table */
    ecs_add(world, obj_c_2, Tag);

    ecs_add_pair(world, obj_a, Rel, obj_b_2);
    ecs_add_pair(world, obj_b_2, Rel, obj_c_2);

    ecs_rule_t *r = ecs_rule_new(world, "(Rel, $X)");
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    int x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    test_bool(true, ecs_rule_next(&it));
    test_int(1, it.count);
    test_uint(obj_a, it.entities[0]);
    test_uint(ecs_pair(Rel, obj_b_2), ecs_field_id(&it, 1));

    ecs_entity_t x = ecs_iter_get_var(&it, x_var);
    test_assert(x != 0);
    test_assert(ecs_is_alive(world, x));
    test_assert(x == obj_b_2);
    
    test_bool(true, ecs_rule_next(&it));
    test_int(1, it.count);
    test_uint(obj_a, it.entities[0]);
    test_uint(ecs_pair(Rel, obj_c_2), ecs_field_id(&it, 1));

    x = ecs_iter_get_var(&it, x_var);
    test_assert(x != 0);
    test_assert(ecs_is_alive(world, x));
    test_assert(x == obj_c_2);

    test_bool(true, ecs_rule_next(&it));
    test_int(1, it.count);
    test_uint(obj_b_2, it.entities[0]);
    test_uint(ecs_pair(Rel, obj_c_2), ecs_field_id(&it, 1));

    x = ecs_iter_get_var(&it, x_var);
    test_assert(x != 0);
    test_assert(ecs_is_alive(world, x));
    test_assert(x == obj_c_2);
    
    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_superset_from_recycled_2_lvls_2_tbls_w_var() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, Transitive, Final);
    ECS_TAG(world, Tag);
    ECS_TAG(world, TagB);

    ecs_entity_t obj_a = ecs_new_id(world);
    ecs_entity_t obj_b = ecs_new_id(world);
    ecs_entity_t obj_c = ecs_new_id(world);
    ecs_entity_t obj_d = ecs_new_id(world);
    ecs_delete(world, obj_b);
    ecs_entity_t obj_b_2 = ecs_new_id(world);
    ecs_delete(world, obj_c);
    ecs_entity_t obj_c_2 = ecs_new_id(world);
    ecs_delete(world, obj_d);
    ecs_entity_t obj_d_2 = ecs_new_id(world);
    test_assert(obj_b != obj_b_2);
    test_assert(obj_c != obj_c_2);
    test_assert(obj_d != obj_d_2);
    test_assert(ecs_strip_generation(obj_b) == ecs_strip_generation(obj_b_2));
    test_assert(ecs_strip_generation(obj_c) == ecs_strip_generation(obj_c_2));
    test_assert(ecs_strip_generation(obj_d) == ecs_strip_generation(obj_d_2));

    ecs_add(world, obj_b_2, Tag); /* make sure it's not an empty table */
    ecs_add(world, obj_c_2, Tag);
    ecs_add(world, obj_d_2, Tag);
    ecs_add(world, obj_c_2, TagB);

    ecs_add_pair(world, obj_a, Rel, obj_b_2);
    ecs_add_pair(world, obj_b_2, Rel, obj_d_2);
    ecs_add_pair(world, obj_c_2, Rel, obj_d_2);

    ecs_rule_t *r = ecs_rule_new(world, "(Rel, $X)");
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    int x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    test_bool(true, ecs_rule_next(&it));
    test_int(1, it.count);
    test_uint(obj_a, it.entities[0]);
    test_uint(ecs_pair(Rel, obj_b_2), ecs_field_id(&it, 1));

    ecs_entity_t x = ecs_iter_get_var(&it, x_var);
    test_assert(x != 0);
    test_assert(ecs_is_alive(world, x));
    test_assert(x == obj_b_2);
    
    test_bool(true, ecs_rule_next(&it));
    test_int(1, it.count);
    test_uint(obj_a, it.entities[0]);
    test_uint(ecs_pair(Rel, obj_d_2), ecs_field_id(&it, 1));

    x = ecs_iter_get_var(&it, x_var);
    test_assert(x != 0);
    test_assert(ecs_is_alive(world, x));
    test_assert(x == obj_d_2);

    test_bool(true, ecs_rule_next(&it));
    test_int(1, it.count);
    test_uint(obj_b_2, it.entities[0]);
    test_uint(ecs_pair(Rel, obj_d_2), ecs_field_id(&it, 1));

    x = ecs_iter_get_var(&it, x_var);
    test_assert(x != 0);
    test_assert(ecs_is_alive(world, x));
    test_assert(x == obj_d_2);
    
    test_bool(true, ecs_rule_next(&it));
    test_int(1, it.count);
    test_uint(obj_c_2, it.entities[0]);
    test_uint(ecs_pair(Rel, obj_d_2), ecs_field_id(&it, 1));

    x = ecs_iter_get_var(&it, x_var);
    test_assert(x != 0);
    test_assert(ecs_is_alive(world, x));
    test_assert(x == obj_d_2);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_reflexive_pair_recycled_subj() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, Transitive, Reflexive, Final);
    ECS_ENTITY(world, Tag, Final);

    ecs_entity_t e = ecs_new_id(world);
    ecs_delete(world, e);
    ecs_entity_t e_2 = ecs_new(world, Tag);
    test_assert(e != e_2);
    test_assert(ecs_strip_generation(e) == ecs_strip_generation(e_2));

    ecs_entity_t f = ecs_new_id(world);
    ecs_delete(world, f);
    ecs_entity_t f_2 = ecs_new(world, Tag);
    test_assert(f != f_2);
    test_assert(ecs_strip_generation(f) == ecs_strip_generation(f_2));

    ecs_set_name(world, e_2, "e");
    ecs_add_pair(world, e_2, Rel, f_2);

    ecs_rule_t *r = ecs_rule_new(world, "Tag(e), Rel(e, e)");
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 0);
    test_uint(it.sources[0], e_2);
    test_uint(it.sources[1], e_2);
    test_uint(ecs_field_id(&it, 2), ecs_pair(Rel, e_2));
    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

static
void test_1_comp(const char *expr) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = expr
    });

    test_assert(r != NULL);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {30, 40});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {50, 60});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {70, 80});
    ecs_set(world, e4, Velocity, {1, 2});

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_true(test_iter(&it, ecs_rule_next, &(test_iter_result_t){
        {e1, e2, e3, e4},
        {{ecs_id(Position)}},
        {
            (Position[]){{10, 20}, {30, 40}, {50, 60}, {70, 80}}
        },
        2
    }));

    ecs_rule_fini(r);

    ecs_fini(world);
}

static
void test_2_comp(const char *expr) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Position, Velocity"
    });

    test_assert(r != NULL);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {30, 40});
    ecs_set(world, e2, Velocity, {3, 4});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {50, 60});
    ecs_set(world, e3, Velocity, {5, 6});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {70, 80});
    ecs_set(world, e4, Velocity, {7, 8});
    ecs_set(world, e4, Mass, {5});

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_true(test_iter(&it, ecs_rule_next, &(test_iter_result_t){
        {e1, e2, e3, e4},
        {{ecs_id(Position), ecs_id(Velocity)}},
        {
            (Position[]){{10, 20}, {30, 40}, {50, 60}, {70, 80}},
            (Velocity[]){{1, 2}, {3, 4}, {5, 6}, {7, 8}}
        },
        2
    }));

    ecs_rule_fini(r);

    ecs_fini(world);    
}

void Rules_1_comp() {
    test_1_comp("Position");
}

void Rules_2_comp() {
    test_2_comp("Position, Velocity");
}

void Rules_1_comp_explicit_subject() {
    test_1_comp("Position($This)");
}

void Rules_2_comp_explicit_subject() {
    test_2_comp("Position($This), Velocity($This)");
}

void Rules_1_empty_comp() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Tag = ecs_new_id(world);
    ecs_entity_t e1 = ecs_new_w_id(world, Tag);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .terms = {{ Tag }}
    });

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), Tag);

    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

const char *rules =
"Final(Parent)\n"
"IsA(CelestialBody, Thing)\n"
"IsA(Planet, CelestialBody)\n"
"IsA(Moon, CelestialBody)\n"
"IsA(Person, Thing)\n"
"IsA(Machine, Thing)\n"
"IsA(SentientMachine, Machine)\n"
"IsA(Character, Person)\n"
"IsA(Human, Character)\n"
"IsA(Droid, Character)\n"
"IsA(Droid, SentientMachine)\n"
"IsA(Creature, Character)\n"
"IsA(Wookie, Creature)\n"
"IsA(Vehicle, Machine)\n"
"IsA(Transport, Vehicle)\n"
"IsA(Transport, Container)\n"
"IsA(SpaceShip, Transport)\n"
"IsA(Speeder, Transport)\n"
"IsA(CorellianLightFreighter, SpaceShip)\n"
"IsA(MilleniumFalcon, CorellianLightFreighter)\n"
"IsA(XWing, SpaceShip)\n"
"IsA(YWing, SpaceShip)\n"
"IsA(Cyborg, SentientMachine)\n"
"IsA(Cyborg, Human)\n"
"Sentient(Droid)\n"
"Sentient(Human)\n"
"Faction(XWing, Rebellion)\n"
"IsA(Rebellion, Faction)\n"
"IsA(FirstOrder, Faction)\n"
"AtWar(FirstOrder, Rebellion)\n"
"AtWar(Rebellion, FirstOrder)\n"
"Human(Luke)\n"
"Human(Leia)\n"
"Human(Rey)\n"
"Human(HanSolo)\n"
"Human(BenSolo)\n"
"Cyborg(Grievous)\n"
"Creature(Yoda)\n"
"Jedi(Yoda)\n"
"Jedi(Luke)\n"
"Jedi(Leia)\n"
"Jedi(Rey)\n"
"Sith(DarthVader)\n"
"Sith(Palpatine)\n"
"Droid(R2D2)\n"
"Droid(C3PO)\n"
"Droid(BB8)\n"
"Wookie(Chewbacca)\n"
"HomePlanet(Yoda, Dagobah)\n"
"HomePlanet(Luke, Tatooine)\n"
"HomePlanet(Rey, Tatooine)\n"
"HomePlanet(BB8, Tatooine)\n"
"HomePlanet(DarthVader, Mustafar)\n"
"Parent(Luke, DarthVader)\n"
"Parent(Leia, DarthVader)\n"
"Parent(BenSolo, HanSolo)\n"
"Parent(BenSolo, Leia)\n"
"Enemy(Luke, Palpatine)\n"
"Enemy(Luke, DarthVader)\n"
"Enemy(Yoda, Palpatine)\n"
"Enemy(Yoda, DarthVader)\n"
"Enemy(Rey, Palpatine)\n"
"Likes(Leia, HanSolo)\n"
"Likes(HanSolo, Leia)\n"
"Likes(Fin, Rey)\n"
"Likes(Rey, Ben)\n"
;

void Rules_1_fact_true() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Jedi(Yoda)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 0);
    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_1_fact_false() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Sith(Yoda)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_2_facts_true() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Jedi(Yoda), Sith(DarthVader)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 0);
    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_2_facts_1_false() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Sith(Yoda), Sith(DarthVader)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_2_facts_false() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Sith(Yoda), Jedi(DarthVader)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_1_fact_pair_true() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "HomePlanet(Yoda, Dagobah)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 0);
    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_1_fact_pair_false() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "HomePlanet(Yoda, Tatooine)"
    });

    test_assert(r != NULL);
    
    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_2_fact_pairs_true() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "HomePlanet(Yoda, Dagobah), HomePlanet(Luke, Tatooine)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 0);
    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_2_fact_pairs_1_false() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "HomePlanet(Yoda, Dagobah), HomePlanet(Luke, Mustafar)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_2_fact_pairs_false() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "HomePlanet(Yoda, Tatooine), HomePlanet(Luke, Mustafar)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_wildcard_pred() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, MatchWith, Final); /* Tag so we don't match everything */
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new(world, MatchWith);
    ecs_add(world, e1, TagA);
    ecs_add(world, e1, TagB);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "*, MatchWith"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), MatchWith);
    test_int(ecs_field_id(&it, 2), MatchWith);
    test_int(ecs_field_src(&it, 1), 0);
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), TagA);
    test_int(ecs_field_id(&it, 2), MatchWith);
    test_int(ecs_field_src(&it, 1), 0);
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), TagB);
    test_int(ecs_field_id(&it, 2), MatchWith);
    test_int(ecs_field_src(&it, 1), 0);
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_wildcard_subj() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, MatchWith); /* Tag so we don't match everything */

    ecs_entity_t e1 = ecs_new(world, MatchWith);
    ecs_entity_t e2 = ecs_new(world, MatchWith);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "MatchWith(*)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), MatchWith);
    test_int(ecs_field_src(&it, 1), e1);
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), MatchWith);
    test_int(ecs_field_src(&it, 1), e2);
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_wildcard_obj() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, ObjA);
    ecs_add_pair(world, e1, Rel, ObjB);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "(Rel, *)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, ObjA));
    test_int(ecs_field_src(&it, 1), 0);
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, ObjB));
    test_int(ecs_field_src(&it, 1), 0);
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_wildcard_pred_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelA, ObjA);
    ecs_add_pair(world, e1, RelA, ObjB);
    ecs_add_pair(world, e1, RelB, ObjA);
    ecs_add_pair(world, e1, RelB, ObjB);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "(*, ObjA)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), ecs_pair(RelA, ObjA));
    test_int(ecs_field_src(&it, 1), 0);
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), ecs_pair(RelB, ObjA));
    test_int(ecs_field_src(&it, 1), 0);
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_any_pred() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, MatchWith); /* Tag so we don't match everything */
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new(world, MatchWith);
    ecs_add(world, e1, TagA);
    ecs_add(world, e1, TagB);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "_, MatchWith"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), EcsAny);
    test_int(ecs_field_id(&it, 2), MatchWith);
    test_int(ecs_field_src(&it, 1), 0);
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_any_subj() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, MatchWith); /* Tag so we don't match everything */

    ecs_new(world, MatchWith);
    ecs_new(world, MatchWith);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "MatchWith(_)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), MatchWith);
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_any_obj() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, ObjA);
    ecs_add_pair(world, e1, Rel, ObjB);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "(Rel, _)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, EcsAny));
    test_int(ecs_field_src(&it, 1), 0);
    test_bool(true, ecs_field_is_set(&it, 1));

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_2_wildcard_as_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, Final);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_new(world, Tag);

    ecs_entity_t c1 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, e2);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Tag, ChildOf(*, $This)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), Tag);
    test_int(ecs_field_id(&it, 2), ecs_childof(e1));
    test_int(ecs_field_src(&it, 1), 0);
    test_int(ecs_field_src(&it, 2), c1);
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e2);
    test_int(ecs_field_id(&it, 1), Tag);
    test_int(ecs_field_id(&it, 2), ecs_childof(e2));
    test_int(ecs_field_src(&it, 1), 0);
    test_int(ecs_field_src(&it, 2), c2);
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_find_1_pair() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "HomePlanet($This, Tatooine)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_true(test_iter(&it, ecs_rule_next, &(test_iter_result_t){
        .entity_names = {"BB8", "Luke", "Rey"},
        .term_ids_expr = {{"(HomePlanet,Tatooine)"}}
    }));

    ecs_rule_fini(r);
    
    ecs_fini(world);
}

void Rules_find_2_pairs() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "HomePlanet($This, Tatooine), Enemy($This, Palpatine)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_true(test_iter(&it, ecs_rule_next, &(test_iter_result_t){
        .entity_names = {"Luke", "Rey"},
        .term_ids_expr = {{"(HomePlanet,Tatooine)", "(Enemy,Palpatine)"}}
    }));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_find_w_pred_var() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "$X($This), Jedi($This)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_true(test_iter(&it, ecs_rule_next, &(test_iter_result_t){
        .entity_names = {
            "Luke", "Luke",
            "Yoda", "Yoda",
            "Leia", "Leia",
            "Rey", "Rey",
        },
        .term_ids_expr = {
            {"Human", "Jedi"},    {"Jedi", "Jedi"},
            {"Creature", "Jedi"}, {"Jedi", "Jedi"},
            {"Human", "Jedi"},    {"Jedi", "Jedi"},
            {"Human", "Jedi"},    {"Jedi", "Jedi"},
        },
        .variables = {
            {x_var, .entity_names = {
                "Human", "Jedi",
                "Creature", "Jedi",
                "Human", "Jedi",
                "Human", "Jedi",
            }}
        }
    }));
    
    ecs_rule_fini(r);
    
    ecs_fini(world);
}

void Rules_find_w_pred_var_explicit_subject() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "$X(Luke)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_true(test_iter(&it, ecs_rule_next, &(test_iter_result_t){
        .term_ids_expr = {
            {"Human"}, {"Jedi"}
        },
        .variables = {
            {x_var, .entity_names = {
                "Human", "Jedi"
            }}
        }
    }));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_find_1_pair_w_object_var() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "HomePlanet($This, $X)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_true(test_iter(&it, ecs_rule_next, &(test_iter_result_t){
        .entity_names = {"BB8", "DarthVader", "Luke", "Yoda", "Rey"},
        .term_ids_expr = {
            {"(HomePlanet,Tatooine)"}, {"(HomePlanet,Mustafar)"}, 
            {"(HomePlanet,Tatooine)"}, {"(HomePlanet,Dagobah)"}, 
            {"(HomePlanet,Tatooine)"}
        },
        .variables = {
            {x_var, .entity_names = {
                "Tatooine", "Mustafar", "Tatooine", "Dagobah", "Tatooine"
            }}
        }
    }));

    ecs_rule_fini(r);
    
    ecs_fini(world);
}

void Rules_find_2_pairs_w_object_var() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "HomePlanet($This, $X), Enemy($This, $Y)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);
    int32_t y_var = ecs_rule_find_var(r, "Y");
    test_assert(y_var != -1);  

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_true(test_iter(&it, ecs_rule_next, &(test_iter_result_t){
        .entity_names = {"Luke", "Luke", "Yoda", "Yoda", "Rey"},
        .term_ids_expr = {
            {"(HomePlanet,Tatooine)", "(Enemy,DarthVader)"}, 
            {"(HomePlanet,Tatooine)", "(Enemy,Palpatine)"}, 
            {"(HomePlanet,Dagobah)", "(Enemy,DarthVader)"},
            {"(HomePlanet,Dagobah)", "(Enemy,Palpatine)"}, 
            {"(HomePlanet,Tatooine)", "(Enemy,Palpatine)"}
        },
        .variables = {
            {x_var, .entity_names = {
                "Tatooine", "Tatooine", "Dagobah", "Dagobah", "Tatooine"
            }},
            {y_var, .entity_names = {
                "DarthVader", "Palpatine", "DarthVader", "Palpatine", "Palpatine"
            }}            
        }
    }));

    ecs_rule_fini(r);
    
    ecs_fini(world);
}

void Rules_find_1_pair_w_pred_var() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "$X($This, Tatooine)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_true(test_iter(&it, ecs_rule_next, &(test_iter_result_t){
        .entity_names = {"BB8", "Luke", "Rey"},
        .term_ids_expr = {
            {"(HomePlanet,Tatooine)"},
        },
        .variables = {
            {x_var, .entity_names = {
                "HomePlanet"
            }}          
        }
    }));

    ecs_rule_fini(r);
    
    ecs_fini(world);    
}

void Rules_find_2_pairs_w_pred_var() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "$X($This, Tatooine), $Y($This, Palpatine)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);
    int32_t y_var = ecs_rule_find_var(r, "Y");
    test_assert(y_var != -1);    

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_true(test_iter(&it, ecs_rule_next, &(test_iter_result_t){
        .entity_names = {"Luke", "Rey"},
        .term_ids_expr = {
            {"(HomePlanet,Tatooine)", "(Enemy,Palpatine)"}
        },
        .variables = {
            {x_var, .entity_names = {
                "HomePlanet"
            }},
            {y_var, .entity_names = {
                "Enemy"
            }}    
        }
    }));

    ecs_rule_fini(r);
    
    ecs_fini(world);
}

void Rules_find_cyclic_pairs() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Likes($This, $X), Likes($X, $This)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_true(test_iter(&it, ecs_rule_next, &(test_iter_result_t){
        .entity_names = {"HanSolo", "Leia"},
        .term_ids_expr = {
            {"(Likes,Leia)", "(Likes,HanSolo)"},
            {"(Likes,HanSolo)", "(Likes,Leia)"}
        },
        .variables = {
            {x_var, .entity_names = {
                "Leia", "HanSolo"
            }}   
        }
    }));

    ecs_rule_fini(r);
    
    ecs_fini(world);
}

void Rules_join_by_object() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Parent($This, $X), Parent($Y, $X)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);
    int32_t y_var = ecs_rule_find_var(r, "Y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_true(test_iter(&it, ecs_rule_next, &(test_iter_result_t){
        .entity_names = {
            "BenSolo", "BenSolo",
            "Luke",    "Luke",
            "Leia",    "Leia"
        },
        .term_ids_expr = {
            {"(Parent,Leia)", "(Parent,Leia)"},             {"(Parent,HanSolo)", "(Parent,HanSolo)"},
            {"(Parent,DarthVader)", "(Parent,DarthVader)"}, {"(Parent,DarthVader)", "(Parent,DarthVader)"},
            {"(Parent,DarthVader)", "(Parent,DarthVader)"}, {"(Parent,DarthVader)", "(Parent,DarthVader)"}
        },
        .variables = {
            {x_var, .entity_names = {
                "Leia", "HanSolo",
                "DarthVader", "DarthVader",
                "DarthVader", "DarthVader",
            }},
            {y_var, .entity_names = {
                "BenSolo", "BenSolo",
                "Luke", "Leia",
                "Luke", "Leia",
            }}
        }
    }));

    ecs_rule_fini(r);
    
    ecs_fini(world);
}

void Rules_join_by_predicate() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "$X($This, DarthVader), $X($Y, DarthVader)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);
    int32_t y_var = ecs_rule_find_var(r, "Y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);
    char *result, *expect;

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (Parent,DarthVader),(Parent,DarthVader)"
    LINE "subj: 0,Luke"
    LINE "vars: X=Parent,Y=Luke"
    LINE "this:"
    LINE "    - Luke"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (Parent,DarthVader),(Parent,DarthVader)"
    LINE "subj: 0,Luke"
    LINE "vars: X=Parent,Y=Luke"
    LINE "this:"
    LINE "    - Leia"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (Enemy,DarthVader),(Enemy,DarthVader)"
    LINE "subj: 0,Luke"
    LINE "vars: X=Enemy,Y=Luke"
    LINE "this:"
    LINE "    - Luke"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (Enemy,DarthVader),(Enemy,DarthVader)"
    LINE "subj: 0,Luke"
    LINE "vars: X=Enemy,Y=Luke"
    LINE "this:"
    LINE "    - Yoda"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (Enemy,DarthVader),(Enemy,DarthVader)"
    LINE "subj: 0,Yoda"
    LINE "vars: X=Enemy,Y=Yoda"
    LINE "this:"
    LINE "    - Luke"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (Enemy,DarthVader),(Enemy,DarthVader)"
    LINE "subj: 0,Yoda"
    LINE "vars: X=Enemy,Y=Yoda"
    LINE "this:"
    LINE "    - Yoda"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (Parent,DarthVader),(Parent,DarthVader)"
    LINE "subj: 0,Leia"
    LINE "vars: X=Parent,Y=Leia"
    LINE "this:"
    LINE "    - Luke"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (Parent,DarthVader),(Parent,DarthVader)"
    LINE "subj: 0,Leia"
    LINE "vars: X=Parent,Y=Leia"
    LINE "this:"
    LINE "    - Leia"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);
    
    ecs_fini(world);
}

void Rules_join_by_predicate_from_subject() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Transitive($This), $This($X, Character)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "IsA");
    test_term_id(&it, 1, "Transitive");
    test_term_id(&it, 2, "(IsA,Character)");
    test_var(&it, x_var, "Creature");

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "IsA");
    test_term_id(&it, 1, "Transitive");
    test_term_id(&it, 2, "(IsA,Character)");
    test_var(&it, x_var, "Droid");

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "IsA");
    test_term_id(&it, 1, "Transitive");
    test_term_id(&it, 2, "(IsA,Character)");
    test_var(&it, x_var, "Human");    

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_find_transitive() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA($This, Character)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Character)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Character");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Character)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Creature");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Creature)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Wookie");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Character)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Droid");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Character)");
    test_int(it.count, 1);    
    test_str(ecs_get_name(world, it.entities[0]), "Human");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Human)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Cyborg");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_find_transitive_2_branches() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA($This, Thing)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Thing)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Thing");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Thing)");
    test_int(it.count, 3);
    test_str(ecs_get_name(world, it.entities[0]), "CelestialBody");
    test_str(ecs_get_name(world, it.entities[1]), "Person");
    test_str(ecs_get_name(world, it.entities[2]), "Machine");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,CelestialBody)");
    test_int(it.count, 2);
    test_str(ecs_get_name(world, it.entities[0]), "Planet");
    test_str(ecs_get_name(world, it.entities[1]), "Moon");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Person)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Character");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Character)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Creature");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Creature)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Wookie");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Character)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Droid");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Character)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Human");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Human)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Cyborg");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Machine)");
    test_int(it.count, 2);
    test_str(ecs_get_name(world, it.entities[0]), "SentientMachine");
    test_str(ecs_get_name(world, it.entities[1]), "Vehicle");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SentientMachine)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Cyborg");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SentientMachine)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Droid");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Vehicle)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Transport");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Transport)");
    test_int(it.count, 2);
    test_str(ecs_get_name(world, it.entities[0]), "SpaceShip");
    test_str(ecs_get_name(world, it.entities[1]), "Speeder");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_int(it.count, 2);
    test_str(ecs_get_name(world, it.entities[0]), "CorellianLightFreighter");
    test_str(ecs_get_name(world, it.entities[1]), "YWing");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,CorellianLightFreighter)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "MilleniumFalcon");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "XWing");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_subsets() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA($This, SpaceShip)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "SpaceShip");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_int(it.count, 2);
    test_str(ecs_get_name(world, it.entities[0]), "CorellianLightFreighter");
    test_str(ecs_get_name(world, it.entities[2]), "YWing");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,CorellianLightFreighter)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "MilleniumFalcon");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "XWing");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_subsets_2_terms() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA($This, Character), IsA($This, Machine)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Character)");
    test_term_id(&it, 2, "(IsA,SentientMachine)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Droid");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Human)");
    test_term_id(&it, 2, "(IsA,SentientMachine)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Cyborg");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);    
}

void Rules_transitive_w_table_object() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Sentient($X), IsA($Y, $X)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    int32_t y_var = ecs_rule_find_var(r, "Y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Sentient");
    test_term_id(&it, 2, "(IsA,Droid)");
    test_var(&it, x_var, "Droid");
    test_var(&it, y_var, "Droid");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Sentient");
    test_term_id(&it, 2, "(IsA,Human)");
    test_var(&it, x_var, "Human");
    test_var(&it, y_var, "Human");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Sentient");
    test_term_id(&it, 2, "(IsA,Human)");
    test_var(&it, x_var, "Human");
    test_var(&it, y_var, "Cyborg");
    test_int(it.count, 0);

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);   
}

void Rules_transitive_supersets() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(SpaceShip, $This)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "SpaceShip");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Transport)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Transport");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Vehicle)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Vehicle");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Machine)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Machine");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Thing)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Thing");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Container)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Container");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_superset_w_subj_var() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "$This(R2D2), IsA($This, $X)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_assert(ecs_rule_next(&it));

    test_term_id(&it, 1, "Droid");
    test_term_id(&it, 2, "(IsA,Droid)");
    test_var(&it, x_var, "Droid");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Droid");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Droid");
    test_term_id(&it, 2, "(IsA,SentientMachine)");
    test_var(&it, x_var, "SentientMachine");   
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Droid");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Droid");
    test_term_id(&it, 2, "(IsA,Machine)");
    test_var(&it, x_var, "Machine");   
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Droid");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Droid");
    test_term_id(&it, 2, "(IsA,Thing)");
    test_var(&it, x_var, "Thing");   
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Droid");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Droid");
    test_term_id(&it, 2, "(IsA,Character)");
    test_var(&it, x_var, "Character");   
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Droid");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Droid");
    test_term_id(&it, 2, "(IsA,Person)");
    test_var(&it, x_var, "Person");   
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Droid");
    
    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Droid");
    test_term_id(&it, 2, "(IsA,Thing)");
    test_var(&it, x_var, "Thing");   
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Droid");    

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_superset_w_subj_var_2_term() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(SentientMachine, $This), IsA($This, $X)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SentientMachine)");
    test_term_id(&it, 2, "(IsA,SentientMachine)");
    test_var(&it, x_var, "SentientMachine");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "SentientMachine");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SentientMachine)");
    test_term_id(&it, 2, "(IsA,Machine)");
    test_var(&it, x_var, "Machine");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "SentientMachine");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SentientMachine)");
    test_term_id(&it, 2, "(IsA,Thing)");
    test_var(&it, x_var, "Thing");   
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "SentientMachine");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Machine)");
    test_term_id(&it, 2, "(IsA,Machine)");
    test_var(&it, x_var, "Machine");   
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Machine");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Machine)");
    test_term_id(&it, 2, "(IsA,Thing)");
    test_var(&it, x_var, "Thing");   
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Machine");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Thing)");
    test_term_id(&it, 2, "(IsA,Thing)");
    test_var(&it, x_var, "Thing");   
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Thing");    

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_supersets_2_terms() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(SpaceShip, $This), IsA(Container, $This)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Container)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Container");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_constraint_on_superset_var() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "$X(C3PO), IsA($X, $Y), Sentient($Y)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    int32_t y_var = ecs_rule_find_var(r, "Y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Droid");
    test_term_id(&it, 2, "(IsA,Droid)");
    test_term_id(&it, 3, "Sentient");
    test_var(&it, x_var, "Droid");
    test_var(&it, x_var, "Droid");
    test_int(it.count, 0);

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_instances() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "$X, IsA($X, Character)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Yoda");
    test_term_id(&it, 1, "Creature");
    test_term_id(&it, 2, "(IsA,Character)");
    test_var(&it, x_var, "Creature");

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);    
    test_str(ecs_get_name(world, it.entities[0]), "Chewbacca");
    test_term_id(&it, 1, "Wookie");
    test_term_id(&it, 2, "(IsA,Creature)");
    test_var(&it, x_var, "Wookie");

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 2);
    test_str(ecs_get_name(world, it.entities[0]), "R2D2");
    test_str(ecs_get_name(world, it.entities[1]), "C3PO");
    test_term_id(&it, 1, "Droid");
    test_term_id(&it, 2, "(IsA,Character)");
    test_var(&it, x_var, "Droid");

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);    
    test_str(ecs_get_name(world, it.entities[0]), "BB8");
    test_term_id(&it, 1, "Droid");
    test_term_id(&it, 2, "(IsA,Character)");
    test_var(&it, x_var, "Droid");

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "BenSolo");
    test_term_id(&it, 1, "Human");
    test_term_id(&it, 2, "(IsA,Character)");
    test_var(&it, x_var, "Human"); 

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Luke");
    test_term_id(&it, 1, "Human");
    test_term_id(&it, 2, "(IsA,Character)");
    test_var(&it, x_var, "Human");  

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Leia");
    test_term_id(&it, 1, "Human");
    test_term_id(&it, 2, "(IsA,Character)");
    test_var(&it, x_var, "Human");

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "HanSolo");
    test_term_id(&it, 1, "Human");
    test_term_id(&it, 2, "(IsA,Character)");
    test_var(&it, x_var, "Human");

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);    
    test_str(ecs_get_name(world, it.entities[0]), "Rey");
    test_term_id(&it, 1, "Human");
    test_term_id(&it, 2, "(IsA,Character)");
    test_var(&it, x_var, "Human"); 

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);    
    test_str(ecs_get_name(world, it.entities[0]), "Grievous");
    test_term_id(&it, 1, "Cyborg");
    test_term_id(&it, 2, "(IsA,Human)");
    test_var(&it, x_var, "Cyborg");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_instances_2_terms() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "$X, IsA($X, Character), IsA($X, Machine)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 2);    
    test_str(ecs_get_name(world, it.entities[0]), "R2D2");
    test_str(ecs_get_name(world, it.entities[1]), "C3PO");
    test_term_id(&it, 1, "Droid");
    test_term_id(&it, 2, "(IsA,Character)");
    test_term_id(&it, 3, "(IsA,SentientMachine)");
    test_var(&it, x_var, "Droid");

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);    
    test_str(ecs_get_name(world, it.entities[0]), "BB8");
    test_term_id(&it, 1, "Droid");
    test_term_id(&it, 2, "(IsA,Character)");
    test_term_id(&it, 3, "(IsA,SentientMachine)");
    test_var(&it, x_var, "Droid");

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Grievous");
    test_term_id(&it, 1, "Cyborg");
    test_term_id(&it, 2, "(IsA,Human)");
    test_term_id(&it, 3, "(IsA,SentientMachine)");
    test_var(&it, x_var, "Cyborg");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_implicit_isa_instances() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Machine"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Cyborg");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Grievous");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Droid");
    test_int(it.count, 2);
    test_str(ecs_get_name(world, it.entities[0]), "R2D2");
    test_str(ecs_get_name(world, it.entities[1]), "C3PO");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Droid");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "BB8");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_implicit_isa_instances_2_terms() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Human, Machine"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Cyborg");
    test_term_id(&it, 2, "Cyborg");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Grievous");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_same_pred_obj() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);

    ecs_entity_t t1 = ecs_pair(Foo, Bar);
    ecs_entity_t t2 = ecs_pair(Foo, Foo);

    ecs_add_id(world, e1, t1);
    ecs_add_id(world, e2, t2);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "$X($This, $X)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e2);

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_same_pred_obj_explicit_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ECS_ENTITY(world, Ent, 0);

    ecs_entity_t t1 = ecs_pair(Foo, Bar);

    ecs_add_id(world, Ent, t1);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "$X(Ent, $X)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_fact_true_depth_1() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(XWing, SpaceShip)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,XWing)");
    test_int(it.count, 0);

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_fact_false() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(XWing, Creature)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_fact_true_depth_2() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(XWing, Transport)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,XWing)");
    test_int(it.count, 0);    

    test_assert(!ecs_rule_next(&it));
    
    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_fact_true_depth_3() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(XWing, Vehicle)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,XWing)");
    test_int(it.count, 0);        

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_fact_true_depth_4() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(XWing, Machine)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,XWing)");
    test_int(it.count, 0);

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_fact_true_depth_5() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(XWing, Thing)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,XWing)");
    test_int(it.count, 0);    

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_fact_true_2_relationships() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(Cyborg, Thing)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Human)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Cyborg)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SentientMachine)");
    test_int(it.count, 0); 

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Cyborg)");
    test_int(it.count, 0);        

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_all() {
    ecs_world_t *world = ecs_mini();

    /* Use smaller rule set so testcase doens't blow up */
    const char *small_ruleset = 
        "IsA(CelestialBody, Thing)\n"
        "IsA(Planet, CelestialBody)\n"
        "IsA(Moon, CelestialBody)\n"
        "IsA(Character, Thing)\n"
        "IsA(Machine, Thing)\n"
        "IsA(Human, Character)\n"
        "IsA(Droid, Character)\n"
        "IsA(Droid, Machine)\n"
        "IsA(Creature, Character)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_ruleset) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA($X, $Y)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    int32_t y_var = ecs_rule_find_var(r, "Y");
    test_assert(y_var != -1);

    /* All permutations of all valid IsA relationships */

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "CelestialBody");
    test_var(&it, y_var, "CelestialBody");
    test_term_id(&it, 1, "(IsA,CelestialBody)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "CelestialBody");
    test_var(&it, y_var, "Thing");
    test_term_id(&it, 1, "(IsA,Thing)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Character");
    test_var(&it, y_var, "Character");
    test_term_id(&it, 1, "(IsA,Character)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Character");
    test_var(&it, y_var, "Thing");
    test_term_id(&it, 1, "(IsA,Thing)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Machine");
    test_var(&it, y_var, "Machine");
    test_term_id(&it, 1, "(IsA,Machine)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Machine");
    test_var(&it, y_var, "Thing");
    test_term_id(&it, 1, "(IsA,Thing)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Planet");
    test_var(&it, y_var, "Planet");
    test_term_id(&it, 1, "(IsA,Planet)");
    test_int(it.count, 0); 

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Planet");
    test_var(&it, y_var, "CelestialBody");
    test_term_id(&it, 1, "(IsA,CelestialBody)");
    test_int(it.count, 0); 

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Planet");
    test_var(&it, y_var, "Thing");
    test_term_id(&it, 1, "(IsA,Thing)");
    test_int(it.count, 0); 

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Moon");
    test_var(&it, y_var, "Moon");
    test_term_id(&it, 1, "(IsA,Moon)");
    test_int(it.count, 0); 

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Moon");
    test_var(&it, y_var, "CelestialBody");
    test_term_id(&it, 1, "(IsA,CelestialBody)");
    test_int(it.count, 0); 

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Moon");
    test_var(&it, y_var, "Thing");
    test_term_id(&it, 1, "(IsA,Thing)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Human");
    test_var(&it, y_var, "Human");
    test_term_id(&it, 1, "(IsA,Human)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Human");
    test_var(&it, y_var, "Character");
    test_term_id(&it, 1, "(IsA,Character)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Human");
    test_var(&it, y_var, "Thing");
    test_term_id(&it, 1, "(IsA,Thing)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Creature");
    test_var(&it, y_var, "Creature");
    test_term_id(&it, 1, "(IsA,Creature)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Creature");
    test_var(&it, y_var, "Character");
    test_term_id(&it, 1, "(IsA,Character)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Creature");
    test_var(&it, y_var, "Thing");
    test_term_id(&it, 1, "(IsA,Thing)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Droid");
    test_var(&it, y_var, "Droid");
    test_term_id(&it, 1, "(IsA,Droid)");
    test_int(it.count, 0);    

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Droid");
    test_var(&it, y_var, "Character");
    test_term_id(&it, 1, "(IsA,Character)");
    test_int(it.count, 0);    

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Droid");
    test_var(&it, y_var, "Thing");
    test_term_id(&it, 1, "(IsA,Thing)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Droid");
    test_var(&it, y_var, "Machine");
    test_term_id(&it, 1, "(IsA,Machine)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Droid");
    test_var(&it, y_var, "Thing");
    test_term_id(&it, 1, "(IsA,Thing)");
    test_int(it.count, 0);          

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_fact_same_subj_obj() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(SpaceShip, SpaceShip)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_int(it.count, 0); 

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_fact_subset_superset() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(SpaceShip, $X), IsA($X, Machine)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    /* Results appear twice because transitive terms are inclusive by default */

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "SpaceShip");
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_term_id(&it, 2, "(IsA,Transport)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "SpaceShip");
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_term_id(&it, 2, "(IsA,SpaceShip)");
    test_int(it.count, 0);     

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Transport");
    test_term_id(&it, 1, "(IsA,Transport)");
    test_term_id(&it, 2, "(IsA,Vehicle)");
    test_int(it.count, 0);  

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Transport");
    test_term_id(&it, 1, "(IsA,Transport)");
    test_term_id(&it, 2, "(IsA,Transport)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Vehicle");
    test_term_id(&it, 1, "(IsA,Vehicle)");
    test_term_id(&it, 2, "(IsA,Machine)");
    test_int(it.count, 0); 

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Vehicle");
    test_term_id(&it, 1, "(IsA,Vehicle)");
    test_term_id(&it, 2, "(IsA,Vehicle)");
    test_int(it.count, 0);   

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Machine");
    test_term_id(&it, 1, "(IsA,Machine)");
    test_term_id(&it, 2, "(IsA,Machine)");
    test_int(it.count, 0);                  

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_nonfinal_fact() {
    ecs_world_t *world = ecs_mini();

    const char *small_rules = 
        "Transitive(PartOf)\n"
        "Reflexive(PartOf)\n"
        "PartOf(ArtCollection, Museum)\n"
        "PartOf(Painting, ArtCollection)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "PartOf(Painting, Museum)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(PartOf,ArtCollection)");
    test_term_source(&it, 1, "Painting");

    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(PartOf,Painting)");
    test_term_source(&it, 1, "Painting");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_nonfinal_fact_w_implicit_pred_subset() {
    ecs_world_t *world = ecs_mini();

    const char *small_rules = 
        "Transitive(ConnectedWith)\n"
        "Reflexive(ConnectedWith)\n"
        "IsA(PartOf, ConnectedWith)\n"
        "PartOf(ArtCollection, Museum)\n"
        "PartOf(Painting, ArtCollection)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "ConnectedWith(Painting, Museum)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(PartOf,ArtCollection)");
    test_term_source(&it, 1, "Painting");

    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(PartOf,Painting)");
    test_term_source(&it, 1, "Painting");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_tag_fact() {
    ecs_world_t *world = ecs_mini();

    const char *small_rules = 
        "Painting(PaintingTmpl)\n"
        "IsA(PaintingTmpl, Painting)\n"
        "IsA(MonaLisa, PaintingTmpl)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Painting(MonaLisa)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "Painting");
    test_term_source(&it, 1, "PaintingTmpl");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_tag_fact_2_lvls() {
    ecs_world_t *world = ecs_mini();

    const char *small_rules = 
        "Painting(PaintingTmpl)\n"
        "IsA(PaintingTmpl, Painting)\n"
        "IsA(PaintingTmpl_2, PaintingTmpl)\n"
        "IsA(MonaLisa, PaintingTmpl_2)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Painting(MonaLisa)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "Painting");
    test_term_source(&it, 1, "PaintingTmpl");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_tag_fact_3_lvls() {
    ecs_world_t *world = ecs_mini();

    const char *small_rules = 
        "Painting(PaintingTmpl)\n"
        "IsA(PaintingTmpl, Painting)\n"
        "IsA(PaintingTmpl_2, PaintingTmpl)\n"
        "IsA(PaintingTmpl_3, PaintingTmpl_2)\n"
        "IsA(MonaLisa, PaintingTmpl_3)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Painting(MonaLisa)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "Painting");
    test_term_source(&it, 1, "PaintingTmpl");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);    
}

void Rules_implicit_is_a_tag_fact_2_lvls_both_matching() {
    ecs_world_t *world = ecs_mini();

    const char *small_rules = 
        "Painting(PaintingTmpl)\n"
        "Painting(PaintingTmpl_2)\n"
        "IsA(PaintingTmpl, Painting)\n"
        "IsA(PaintingTmpl_2, PaintingTmpl)\n"
        "IsA(MonaLisa, PaintingTmpl_2)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Painting(MonaLisa)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "Painting");
    test_term_source(&it, 1, "PaintingTmpl_2");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_tag_fact_2_lvls_entity_matching() {
    ecs_world_t *world = ecs_mini();

    const char *small_rules = 
        "Painting(PaintingTmpl)\n"
        "Painting(MonaLisa)\n"
        "IsA(MonaLisa, PaintingTmpl)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Painting(MonaLisa)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "Painting");
    test_term_source(&it, 1, "MonaLisa");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_pair_fact() {
    ecs_world_t *world = ecs_mini();

    const char *small_rules = 
        "CreatedBy(PaintingTmpl, Painter)\n"
        "IsA(MonaLisa, PaintingTmpl)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "CreatedBy(MonaLisa, Painter)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(CreatedBy,Painter)");
    test_term_source(&it, 1, "PaintingTmpl");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_pair_2_lvls() {
    ecs_world_t *world = ecs_mini();

    const char *small_rules = 
        "CreatedBy(PaintingTmpl, Painter)\n"
        "IsA(PaintingTmpl_2, PaintingTmpl)\n"
        "IsA(MonaLisa, PaintingTmpl_2)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "CreatedBy(MonaLisa, Painter)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(CreatedBy,Painter)");
    test_term_source(&it, 1, "PaintingTmpl");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_pair_3_lvls() {
    ecs_world_t *world = ecs_mini();

    const char *small_rules = 
        "CreatedBy(PaintingTmpl, Painter)\n"
        "IsA(PaintingTmpl_2, PaintingTmpl)\n"
        "IsA(PaintingTmpl_3, PaintingTmpl_2)\n"
        "IsA(MonaLisa, PaintingTmpl_3)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "CreatedBy(MonaLisa, Painter)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(CreatedBy,Painter)");
    test_term_source(&it, 1, "PaintingTmpl");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_pair_2_lvls_both_matching() {
    ecs_world_t *world = ecs_mini();

    const char *small_rules = 
        "CreatedBy(PaintingTmpl, Painter)\n"
        "CreatedBy(PaintingTmpl_2, Painter)\n"
        "IsA(PaintingTmpl_2, PaintingTmpl)\n"
        "IsA(MonaLisa, PaintingTmpl_2)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "CreatedBy(MonaLisa, Painter)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(CreatedBy,Painter)");
    test_term_source(&it, 1, "PaintingTmpl_2");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_pair_2_lvls_entity_matching() {
    ecs_world_t *world = ecs_mini();

    const char *small_rules = 
        "CreatedBy(PaintingTmpl, Painter)\n"
        "CreatedBy(MonaLisa, Painter)\n"
        "IsA(PaintingTmpl_2, PaintingTmpl)\n"
        "IsA(MonaLisa, PaintingTmpl_2)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "CreatedBy(MonaLisa, Painter)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(CreatedBy,Painter)");
    test_term_source(&it, 1, "MonaLisa");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_pred_var() {
    ecs_world_t *world = ecs_mini();

    const char *small_rules = 
        "Canvas(PaintingTmpl)\n"
        "Painting(MonaLisa)\n"
        "IsA(MonaLisa, PaintingTmpl)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "$X(MonaLisa)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "Painting");
    test_term_source(&it, 1, "MonaLisa");
    test_var(&it, x_var, "Painting");

    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "Canvas");
    test_term_source(&it, 1, "PaintingTmpl");
    test_var(&it, x_var, "Canvas");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_pair_var() {
    ecs_world_t *world = ecs_mini();

    const char *small_rules = 
        "CreatedBy(PaintingTmpl,Painter)\n"
        "PaintedBy(MonaLisa,LeonardoDaVinci)\n"
        "IsA(MonaLisa, PaintingTmpl)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "$X(MonaLisa, $Y)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    int32_t y_var = ecs_rule_find_var(r, "Y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(Identifier,Name)");
    test_term_source(&it, 1, "MonaLisa");
    test_var(&it, x_var, "Identifier");
    test_var(&it, y_var, "Name");

    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(IsA,PaintingTmpl)");
    test_term_source(&it, 1, "MonaLisa");
    test_var(&it, x_var, "IsA");
    test_var(&it, y_var, "PaintingTmpl");

    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(PaintedBy,LeonardoDaVinci)");
    test_term_source(&it, 1, "MonaLisa");
    test_var(&it, x_var, "PaintedBy");
    test_var(&it, y_var, "LeonardoDaVinci");

    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(Identifier,Name)");
    test_term_source(&it, 1, "PaintingTmpl");
    test_var(&it, x_var, "Identifier");
    test_var(&it, y_var, "Name");

    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(CreatedBy,Painter)");
    test_term_source(&it, 1, "PaintingTmpl");
    test_var(&it, x_var, "CreatedBy");
    test_var(&it, y_var, "Painter");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_transitive_pair_fact() {
    ecs_world_t *world = ecs_mini();

    const char *small_rules = 
        "Transitive(PartOf)\n"
        "Reflexive(PartOf)\n"
        "PartOf(ArtCollection, Museum)\n"
        "PartOf(Painting, ArtCollection)\n"
        "IsA(MonaLisa, Painting)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "PartOf(MonaLisa, Museum)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(PartOf,ArtCollection)");
    test_term_source(&it, 1, "Painting");

    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(PartOf,Painting)");
    test_term_source(&it, 1, "Painting");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_transitive_pair_fact_w_implicit_pred_subset() {
    ecs_world_t *world = ecs_mini();

    const char *small_rules = 
        "Transitive(ConnectedWith)\n"
        "Reflexive(ConnectedWith)\n"
        "IsA(PartOf, ConnectedWith)\n"
        "PartOf(ArtCollection, Museum)\n"
        "PartOf(Painting, ArtCollection)\n"
        "IsA(MonaLisa, Painting)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "ConnectedWith(MonaLisa, Museum)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(PartOf,ArtCollection)");
    test_term_source(&it, 1, "Painting");

    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(PartOf,Painting)");
    test_term_source(&it, 1, "Painting");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_transitive_pair_fact_w_implicit_pred_obj() {
    test_quarantine("Aug 31 2021");

    ecs_world_t *world = ecs_mini();

    const char *small_rules = 
        "Transitive(PartOf)\n"
        "PartOf(LouvreExhibit, Louvre)\n"
        "PartOf(LouvrePainting, LouvreExhibit)\n"
        "IsA(Louvre, Museum)\n"
        "IsA(MonaLisa, LouvrePainting)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "PartOf(MonaLisa, Museum)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(PartOf,ArtCollection)");
    test_term_source(&it, 1, "Painting");

    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(PartOf,Painting)");
    test_term_source(&it, 1, "Painting");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_2_constrained_vars_by_subject_literal() {
    ecs_world_t *world = ecs_mini();

    const char *small_rules = 
        "Eats(Cat, Fish)\n"
        "Eats(Dog, Chicken)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Eats(Cat, $X), Eats(Dog, $Y)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(Eats,Fish)");
    test_term_source(&it, 1, "Cat");

    test_term_id(&it, 2, "(Eats,Chicken)");
    test_term_source(&it, 2, "Dog");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_2_constrained_vars_by_subject_literal_2_var_terms() {
    ecs_world_t *world = ecs_mini();

    const char *small_rules = 
        "Food(Fish)\n"
        "Food(Chicken)\n"
        "Eats(Cat, Fish)\n"
        "Eats(Dog, Chicken)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Eats(Cat, $X), Eats(Dog, $Y), Food($X), Food($Y)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(Eats,Fish)");
    test_term_source(&it, 1, "Cat");

    test_term_id(&it, 2, "(Eats,Chicken)");
    test_term_source(&it, 2, "Dog");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_term_w_nothing_set() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Pred()"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    test_int(ecs_field_id(&it, 1), Pred);
    test_int(it.count, 0);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_term_w_nothing_set_w_this_term() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Pred);
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world, Tag);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Pred(), Tag"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    test_int(ecs_field_id(&it, 1), Pred);
    test_int(ecs_field_id(&it, 2), Tag);
    test_int(it.count, 1);
    test_int(it.entities[0], e);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_comp_w_not_term() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, 0);
    ECS_ENTITY(world, TagB, 0);
    ECS_ENTITY(world, TagC, 0);

    ecs_entity_t e_1 = ecs_new(world, TagA);
    ecs_entity_t e_2 = ecs_new(world, TagA);
    ecs_add_id(world, e_2, TagB);
    ecs_entity_t e_3 = ecs_new(world, TagA);
    ecs_add_id(world, e_3, TagB);
    ecs_add_id(world, e_3, TagC);
    ecs_entity_t e_4 = ecs_new(world, TagA);
    ecs_add_id(world, e_4, TagC);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "TagA, !TagB"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    test_int(ecs_field_id(&it, 1), TagA);
    test_int(ecs_field_id(&it, 2), TagB);
    test_int(it.count, 1);
    test_int(it.entities[0], e_1);

    test_bool(true, ecs_rule_next(&it));
    test_int(ecs_field_id(&it, 1), TagA);
    test_int(ecs_field_id(&it, 2), TagB);
    test_int(it.count, 1);
    test_int(it.entities[0], e_4);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_not_term_w_subj_var() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, 0);
    ECS_ENTITY(world, TagB, 0);
    ECS_ENTITY(world, TagC, 0);

    ecs_entity_t e_1 = ecs_new(world, TagA);
    ecs_entity_t e_2 = ecs_new(world, TagA);
    ecs_add_id(world, e_2, TagB);
    ecs_entity_t e_3 = ecs_new(world, TagA);
    ecs_add_id(world, e_3, TagB);
    ecs_add_id(world, e_3, TagC);
    ecs_entity_t e_4 = ecs_new(world, TagA);
    ecs_add_id(world, e_4, TagC);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "TagA($X), !TagB($X)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    test_int(ecs_field_id(&it, 1), TagA);
    test_int(ecs_field_id(&it, 2), TagB);
    test_int(it.count, 0);
    test_assert(ecs_iter_get_var(&it, x_var) == e_1);

    test_bool(true, ecs_rule_next(&it));
    test_int(ecs_field_id(&it, 1), TagA);
    test_int(ecs_field_id(&it, 2), TagB);
    test_int(it.count, 0);
    test_assert(ecs_iter_get_var(&it, x_var) == e_4);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_not_term_w_subj_var_match_n_per_type() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, 0);
    ECS_ENTITY(world, TagB, 0);
    ECS_ENTITY(world, TagC, 0);

    ecs_entity_t e_1 = ecs_new(world, TagA);
    ecs_entity_t e_2 = ecs_new(world, TagA);
    ecs_entity_t e_3 = ecs_new(world, TagA);
    ecs_entity_t e_4 = ecs_new(world, TagA);
    ecs_add(world, e_3, TagC);
    ecs_add(world, e_4, TagC);

    ecs_entity_t e_5 = ecs_new(world, TagA);
    ecs_entity_t e_6 = ecs_new(world, TagA);
    ecs_add(world, e_5, TagB);
    ecs_add(world, e_6, TagB);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "TagA($X), !TagB($X)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    test_int(ecs_field_id(&it, 1), TagA);
    test_int(ecs_field_id(&it, 2), TagB);
    test_int(it.count, 0);
    test_assert(ecs_iter_get_var(&it, x_var) == e_1);

    test_bool(true, ecs_rule_next(&it));
    test_int(ecs_field_id(&it, 1), TagA);
    test_int(ecs_field_id(&it, 2), TagB);
    test_int(it.count, 0);
    test_assert(ecs_iter_get_var(&it, x_var) == e_2);

    test_bool(true, ecs_rule_next(&it));
    test_int(ecs_field_id(&it, 1), TagA);
    test_int(ecs_field_id(&it, 2), TagB);
    test_int(it.count, 0);
    test_assert(ecs_iter_get_var(&it, x_var) == e_3);

    test_bool(true, ecs_rule_next(&it));
    test_int(ecs_field_id(&it, 1), TagA);
    test_int(ecs_field_id(&it, 2), TagB);
    test_int(it.count, 0);
    test_assert(ecs_iter_get_var(&it, x_var) == e_4);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_invalid_rule_w_only_not_term() {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "!TagA"
    });

    test_assert(r == NULL);

    ecs_fini(world);
}

void Rules_invalid_rule_w_not_term_unknown_var() {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, 0);
    ECS_ENTITY(world, TagB, 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "TagA, !TagB($X)"
    });

    test_assert(r == NULL);

    ecs_fini(world);
}

void Rules_invalid_rule_w_not_term_unknown_pair_var() {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, 0);
    ECS_ENTITY(world, TagB, 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "TagA, !TagB($This, $X)"
    });

    test_assert(r == NULL);

    ecs_fini(world);
}

void Rules_invalid_rule_w_not_term_unknown_pair_var_subj_var() {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, 0);
    ECS_ENTITY(world, TagB, 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "TagA($Y), !TagB($Y, $X)"
    });

    test_assert(r == NULL);

    ecs_fini(world);
}

void Rules_not_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Rel)
    ECS_TAG(world, Obj);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .terms = {
            { Tag },
            { ecs_pair(Rel, EcsWildcard), .oper = EcsNot }
        }
    });
    test_assert(r != NULL);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_add_pair(world, e1, Rel, Obj);

    ecs_entity_t e2 = ecs_new(world, Tag);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(true, ecs_rule_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_pair(Rel, EcsWildcard), ecs_field_id(&it, 2));
    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_not_any() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, Final);
    ECS_ENTITY(world, Rel, Final);
    ECS_ENTITY(world, Obj, Final);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .terms = {
            { Tag },
            { ecs_pair(Rel, EcsAny), .oper = EcsNot }
        }
    });
    test_assert(r != NULL);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_add_pair(world, e1, Rel, Obj);

    ecs_entity_t e2 = ecs_new(world, Tag);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(true, ecs_rule_next(&it));

    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_pair(Rel, EcsAny), ecs_field_id(&it, 2));
    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_not_wildcard_w_expr() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Rel)
    ECS_TAG(world, Obj);

    ecs_rule_t *r = ecs_rule_new(world, "Tag, !(Rel, *)");

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_add_pair(world, e1, Rel, Obj);

    ecs_entity_t e2 = ecs_new(world, Tag);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(true, ecs_rule_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_pair(Rel, EcsWildcard), ecs_field_id(&it, 2));
    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_not_any_w_expr() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Rel)
    ECS_TAG(world, Obj);

    ecs_rule_t *r = ecs_rule_new(world, "Tag, !(Rel, _)");
    test_assert(r != NULL);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_add_pair(world, e1, Rel, Obj);

    ecs_entity_t e2 = ecs_new(world, Tag);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(true, ecs_rule_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_pair(Rel, EcsAny), ecs_field_id(&it, 2));
    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_rules_w_desc_id() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .terms = {{ Tag }}
    });

    test_assert(r != NULL);

    ecs_entity_t e = ecs_new(world, Tag);
    test_assert(e != 0);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(ecs_field_id(&it, 1), Tag);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_rules_w_desc_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .terms = {{ ecs_pair(Rel, Obj) }}
    });

    test_assert(r != NULL);

    ecs_entity_t e = ecs_new_w_pair(world, Rel, Obj);
    test_assert(e != 0);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, Obj));

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_rules_w_desc_pair_empty_rel_obj() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Rel = ecs_new_id(world);
    ecs_entity_t Obj = ecs_new_id(world);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .terms = {{ ecs_pair(Rel, Obj) }}
    });

    test_assert(r != NULL);

    ecs_entity_t e = ecs_new_w_pair(world, Rel, Obj);
    test_assert(e != 0);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, Obj));

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_rules_w_desc_pair_pred_obj() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .terms = {{ .first.id = Rel, .second.id = Obj }}
    });

    test_assert(r != NULL);

    ecs_entity_t e = ecs_new_w_pair(world, Rel, Obj);
    test_assert(e != 0);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, Obj));

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_rules_w_desc_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .terms = {{ ecs_pair(Rel, EcsWildcard) }}
    });

    test_assert(r != NULL);

    ecs_entity_t e = ecs_new_w_pair(world, Rel, Obj);
    test_assert(e != 0);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, Obj));

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_childof_0() {    
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_new_w_pair(world, EcsChildOf, e1);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .terms = {{ ecs_pair(EcsChildOf, 0) }}
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], EcsFlecs);
    test_int(ecs_field_id(&it, 1), ecs_pair(EcsChildOf, 0));

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], Tag);
    test_int(ecs_field_id(&it, 1), ecs_pair(EcsChildOf, 0));

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), ecs_pair(EcsChildOf, 0));

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_childof_this() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, Final);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_new(world, Tag);

    ecs_entity_t child1 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t child2 = ecs_new_w_pair(world, EcsChildOf, e1);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Tag, ChildOf($X, $This)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != 0);

    ecs_iter_t it = ecs_rule_iter(world, r);
    ecs_entity_t var;

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), Tag);
    test_int(ecs_field_id(&it, 2), ecs_pair(EcsChildOf, e1));
    var = ecs_iter_get_var(&it, x_var);
    test_assert(var != 0);
    test_assert(var == child1);

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), Tag);
    test_int(ecs_field_id(&it, 2), ecs_pair(EcsChildOf, e1));
    var = ecs_iter_get_var(&it, x_var);
    test_assert(var != 0);
    test_assert(var == child2);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_childof_this_as_identifier() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, Final);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_new(world, Tag);

    ecs_entity_t child1 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t child2 = ecs_new_w_pair(world, EcsChildOf, e1);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Tag, ChildOf($X, $This)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != 0);

    ecs_iter_t it = ecs_rule_iter(world, r);
    ecs_entity_t var;

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), Tag);
    test_int(ecs_field_id(&it, 2), ecs_pair(EcsChildOf, e1));
    var = ecs_iter_get_var(&it, x_var);
    test_assert(var != 0);
    test_assert(var == child1);

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), Tag);
    test_int(ecs_field_id(&it, 2), ecs_pair(EcsChildOf, e1));
    var = ecs_iter_get_var(&it, x_var);
    test_assert(var != 0);
    test_assert(var == child2);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_optional_term() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, Final);
    ECS_ENTITY(world, TagB, Final);
    ECS_ENTITY(world, TagC, Final);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);
    
    ecs_add(world, e2, TagB);
    ecs_add(world, e3, TagC);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "TagA, ?TagB, ?TagC"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));

    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), TagA);
    test_int(ecs_field_id(&it, 2), TagB);
    test_int(ecs_field_id(&it, 3), TagC);
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));
    test_bool(false, ecs_field_is_set(&it, 3));

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e2);
    test_int(ecs_field_id(&it, 1), TagA);
    test_int(ecs_field_id(&it, 2), TagB);
    test_int(ecs_field_id(&it, 3), TagC);
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));
    test_bool(false, ecs_field_is_set(&it, 3));

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    test_int(ecs_field_id(&it, 1), TagA);
    test_int(ecs_field_id(&it, 2), TagB);
    test_int(ecs_field_id(&it, 3), TagC);
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 3));

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_optional_term_w_component() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, Final);
    ECS_ENTITY(world, TagB, Final);
    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    
    ecs_add(world, e2, Position);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "TagA, ?Position"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));

    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), TagA);
    test_int(ecs_field_id(&it, 2), ecs_id(Position));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));
    test_assert(ecs_field(&it, Position, 2) == NULL);

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e2);
    test_int(ecs_field_id(&it, 1), TagA);
    test_int(ecs_field_id(&it, 2), ecs_id(Position));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));
    test_assert(ecs_field(&it, Position, 2) != NULL);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_optional_term_on_entity() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, Final);
    ECS_ENTITY(world, TagB, Final);

    ECS_ENTITY(world, E, TagA);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "TagA(E), ?TagB(E)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 0);
    test_int(it.sources[0], E);
    test_int(ecs_field_id(&it, 1), TagA);
    test_int(ecs_field_id(&it, 2), TagB);
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_optional_term_on_variable() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Likes, Final);
    ECS_ENTITY(world, TagA, Final);
    ECS_ENTITY(world, TagB, Final);
    ECS_ENTITY(world, TagC, Final);

    ecs_entity_t e = ecs_new_id(world);
    ecs_entity_t obj_1 = ecs_new(world, TagA);
    ecs_entity_t obj_2 = ecs_new(world, TagA);
    ecs_entity_t obj_3 = ecs_new(world, TagA);

    ecs_add(world, obj_2, TagB);
    ecs_add(world, obj_3, TagC);

    ecs_add_pair(world, e, Likes, obj_1);
    ecs_add_pair(world, e, Likes, obj_2);
    ecs_add_pair(world, e, Likes, obj_3);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "(Likes, $X), TagA($X), ?TagB($X), ?TagC($X)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);
    ecs_entity_t var;

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    var = ecs_iter_get_var(&it, x_var);
    test_assert(var != 0);
    test_assert(var == obj_1);
    test_int(ecs_field_id(&it, 1), ecs_pair(Likes, obj_1));
    test_int(ecs_field_id(&it, 2), TagA);
    test_int(ecs_field_id(&it, 3), TagB);
    test_int(ecs_field_id(&it, 4), TagC);
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));
    test_bool(false, ecs_field_is_set(&it, 3));
    test_bool(false, ecs_field_is_set(&it, 4));

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    var = ecs_iter_get_var(&it, x_var);
    test_assert(var != 0);
    test_assert(var == obj_2);
    test_int(ecs_field_id(&it, 1), ecs_pair(Likes, obj_2));
    test_int(ecs_field_id(&it, 2), TagA);
    test_int(ecs_field_id(&it, 3), TagB);
    test_int(ecs_field_id(&it, 4), TagC);
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));
    test_bool(true, ecs_field_is_set(&it, 3));
    test_bool(false, ecs_field_is_set(&it, 4));

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    var = ecs_iter_get_var(&it, x_var);
    test_assert(var != 0);
    test_assert(var == obj_3);
    test_int(ecs_field_id(&it, 1), ecs_pair(Likes, obj_3));
    test_int(ecs_field_id(&it, 2), TagA);
    test_int(ecs_field_id(&it, 3), TagB);
    test_int(ecs_field_id(&it, 4), TagC);
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));
    test_bool(false, ecs_field_is_set(&it, 3));
    test_bool(true, ecs_field_is_set(&it, 4));

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_optional_term_on_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, Final);

    ecs_entity_t parent = ecs_new_id(world);
    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);

    ecs_add_pair(world, e2, EcsChildOf, parent);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Tag, ?(ChildOf, *)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), Tag);
    test_int(ecs_field_id(&it, 2), ecs_childof(EcsWildcard));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e2);
    test_int(ecs_field_id(&it, 1), Tag);
    test_int(ecs_field_id(&it, 2), ecs_childof(parent));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_optional_term_on_relation_this_obj() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, Final);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);

    ecs_new_w_pair(world, EcsChildOf, e1);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Tag, ?ChildOf(*, $This)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), Tag);
    test_int(ecs_field_id(&it, 2), ecs_childof(e1));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e2);
    test_int(ecs_field_id(&it, 1), Tag);
    test_int(ecs_field_id(&it, 2), ecs_childof(EcsWildcard));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_optional_w_subj_var() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Likes, 0);

    const char *expr = 
    HEAD "Final(Likes)"
    LINE "Likes(Alice, Bob)"
    LINE "Likes(Jane, Bob)"
    LINE "Likes(John, Jane)";
    test_int(ecs_plecs_from_str(world, NULL, expr), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Likes($X, $Y), ?Likes($Z, $X)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    char *result, *expect;

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (Likes,Bob),(Likes,*)"
    LINE "subj: Alice,John"
    LINE "vars: X=Alice,Y=Bob,Z=John"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);
    test_bool(ecs_field_is_set(&it, 1), true);
    test_bool(ecs_field_is_set(&it, 2), false);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (Likes,Bob),(Likes,Jane)"
    LINE "subj: Jane,John"
    LINE "vars: X=Jane,Y=Bob,Z=John"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);
    test_bool(ecs_field_is_set(&it, 1), true);
    test_bool(ecs_field_is_set(&it, 2), true);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (Likes,Jane),(Likes,*)"
    LINE "subj: John,John"
    LINE "vars: X=John,Y=Jane,Z=John"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);
    test_bool(ecs_field_is_set(&it, 1), true);
    test_bool(ecs_field_is_set(&it, 2), false);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_optional_w_obj_var() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_ENTITY(world, Tag, Final);
    ECS_ENTITY(world, Likes, Final);

    ecs_entity_t parent = ecs_new_id(world);
    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);

    ecs_add_pair(world, e2, Likes, parent);

    ecs_add(world, e3, Foo);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Tag, ?(Likes, $X)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(EcsWildcard, ecs_iter_get_var(&it, x_var));
    test_int(ecs_field_id(&it, 1), Tag);
    test_int(ecs_field_id(&it, 2), ecs_pair(Likes, EcsWildcard));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e2);
    test_int(parent, ecs_iter_get_var(&it, x_var));
    test_int(ecs_field_id(&it, 1), Tag);
    test_int(ecs_field_id(&it, 2), ecs_pair(Likes, parent));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(true, ecs_field_is_set(&it, 2));

    test_bool(true, ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    test_int(EcsWildcard, ecs_iter_get_var(&it, x_var));
    test_int(ecs_field_id(&it, 1), Tag);
    test_int(ecs_field_id(&it, 2), ecs_pair(Likes, EcsWildcard));
    test_bool(true, ecs_field_is_set(&it, 1));
    test_bool(false, ecs_field_is_set(&it, 2));

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_terms_set() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "TagA, TagB"
    });

    test_assert(r != NULL);
    ecs_iter_t it = ecs_rule_iter(world, r);
    
    test_int(it.field_count, 2);
    test_assert(it.terms != NULL);

    test_int(it.terms[0].id, TagA);
    test_int(it.terms[1].id, TagB);

    ecs_entity_t e = ecs_new(world, TagA);
    ecs_add(world, e, TagA);

    while (ecs_rule_next(&it)) {
        test_int(it.field_count, 2);
        test_assert(it.terms != NULL);
        test_int(it.terms[0].id, TagA);
        test_int(it.terms[1].id, TagB);
        test_int(it.count, 1);
    }
    
    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_value_set() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Position, Velocity"
    });

    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});

    ecs_entity_t e2 = ecs_set(world, 0, Position, {10, 20});
    ecs_set(world, e2, Velocity, {1, 2});

    test_assert(r != NULL);
    ecs_iter_t it = ecs_rule_iter(world, r);
    
    test_int(it.field_count, 2);
    test_assert(it.terms != NULL);

    test_int(it.terms[0].id, ecs_id(Position));
    test_int(it.terms[1].id, ecs_id(Velocity));

    while (ecs_rule_next(&it)) {
        test_int(it.field_count, 2);
        test_assert(it.terms != NULL);
        test_int(it.terms[0].id, ecs_id(Position));
        test_int(it.terms[1].id, ecs_id(Velocity));

        Position *p = ecs_field(&it, Position, 1);
        Velocity *v = ecs_field(&it, Velocity, 2);
        test_assert(p != NULL);
        test_assert(v != NULL);

        test_int(it.count, 2);
        test_int(it.entities[0], e1);
        test_int(it.entities[1], e2);
    }

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_term_w_this_this_this() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, e, e);

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_add_pair(world, e2, e2, e);

    ecs_entity_t e3 = ecs_new_id(world);
    ecs_add_pair(world, e3, e, e3);

    ecs_entity_t e4 = ecs_new_id(world);
    ecs_add_pair(world, e4, e, e);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "$This($This, $This)"
    });

    test_assert(r != NULL);
    ecs_iter_t it = ecs_rule_iter(world, r);
    
    test_int(it.field_count, 1);
    test_assert(it.terms != NULL);
    test_int(it.terms[0].id, ecs_pair(EcsWildcard, EcsWildcard));

    test_assert(ecs_rule_next(&it));
    test_int(it.field_count, 1);
    test_assert(it.terms != NULL);
    test_int(it.count, 1);
    test_int(it.entities[0], e);

    test_assert(!ecs_rule_next(&it));
    
    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_term_w_x_x_x() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, e, e);

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_add_pair(world, e2, e2, e);

    ecs_entity_t e3 = ecs_new_id(world);
    ecs_add_pair(world, e3, e, e3);

    ecs_entity_t e4 = ecs_new_id(world);
    ecs_add_pair(world, e4, e, e);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "$X($X, $X)"
    });

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    test_assert(r != NULL);
    ecs_iter_t it = ecs_rule_iter(world, r);
    
    test_int(it.field_count, 1);
    test_assert(it.terms != NULL);
    test_int(it.terms[0].id, ecs_pair(EcsWildcard, EcsWildcard));

    test_assert(ecs_rule_next(&it));
    test_int(it.field_count, 1);
    test_assert(it.terms != NULL);
    test_int(it.count, 0);
    test_assert(ecs_iter_get_var(&it, x_var) == e);

    test_assert(!ecs_rule_next(&it));
    
    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_filter_term() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .terms = {{ .id = ecs_id(Position), .inout = EcsInOutNone }}
    });

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(ecs_rule_next(&it), true);
    test_assert(it.ids != NULL);
    test_assert(it.ids[0] == ecs_id(Position));

    test_int(it.count, 1);
    test_assert(it.entities != NULL);
    test_assert(it.entities[0] == e);

    test_assert(it.sizes != NULL);
    test_int(it.sizes[0], ECS_SIZEOF(Position));

    test_assert(it.ptrs == NULL);
    test_assert(it.columns != NULL);

    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_2_terms_1_filter() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .terms = {
            { .id = ecs_id(Position), .inout = EcsInOutNone },
            { .id = ecs_id(Velocity) }
        }
    });

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    ecs_set(world, e, Velocity, {1, 1});

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(ecs_rule_next(&it), true);
    test_assert(it.ids != NULL);
    test_assert(it.ids[0] == ecs_id(Position));
    test_assert(it.ids[1] == ecs_id(Velocity));

    test_int(it.count, 1);
    test_assert(it.entities != NULL);
    test_assert(it.entities[0] == e);

    test_assert(it.ptrs != NULL);
    test_assert(it.sizes != NULL);
    test_assert(it.columns != NULL);

    test_assert(it.ptrs[0] == NULL);
    test_assert(it.ptrs[1] != NULL);

    test_bool(ecs_rule_next(&it), false);
    
    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_3_terms_2_filter() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .terms = {
            { .id = ecs_id(Position), .inout = EcsInOutNone },
            { .id = ecs_id(Velocity), .inout = EcsInOutNone },
            { .id = ecs_id(Mass) }
        }
    });

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    ecs_set(world, e, Velocity, {1, 1});
    ecs_set(world, e, Mass, {1});

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(ecs_rule_next(&it), true);
    test_assert(it.ids != NULL);
    test_assert(it.ids[0] == ecs_id(Position));
    test_assert(it.ids[1] == ecs_id(Velocity));
    test_assert(it.ids[2] == ecs_id(Mass));

    test_int(it.count, 1);
    test_assert(it.entities != NULL);
    test_assert(it.entities[0] == e);

    test_assert(it.ptrs != NULL);
    test_assert(it.sizes != NULL);
    test_assert(it.columns != NULL);

    test_assert(it.ptrs[0] == NULL);
    test_assert(it.ptrs[1] == NULL);
    test_assert(it.ptrs[2] != NULL);

    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_term_obj_w_this() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, 0);
    ecs_add_id(world, TagA, ecs_pair(TagA, TagA));

    ecs_filter_desc_t desc = { 0 };
    desc.terms[0].first.id = TagA;
    desc.terms[0].second.id = EcsThis;
    desc.terms[0].second.flags = EcsIsVariable;
    desc.terms[0].src.id = TagA;

    ecs_rule_t *r = ecs_rule_init(world, &desc);
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it) == true);
    test_int(it.count, 1);
    test_int(it.entities[0], TagA);

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_term_subj_w_this() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, TagA, 0);
    ecs_add_id(world, TagA, ecs_pair(TagA, TagA));

    ecs_filter_desc_t desc = {0};
    desc.terms[0].first.id = TagA;
    desc.terms[0].second.id = TagA;
    desc.terms[0].src.id = EcsThis;
    desc.terms[0].src.flags = EcsIsVariable;

    ecs_rule_t *r = ecs_rule_init(world, &desc);
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it) == true);
    test_int(it.count, 1);
    test_int(it.entities[0], TagA);

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_rule_iter_frame_offset() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_rule_t *q = ecs_rule_init(world, &(ecs_filter_desc_t){
        .terms = {
            { .id = TagA, }
        },
    });

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagA);
    ecs_entity_t e3 = ecs_new(world, TagA);
    ecs_entity_t e4 = ecs_new(world, TagA);
    ecs_entity_t e5 = ecs_new(world, TagA);

    ecs_add(world, e3, TagB);
    ecs_add(world, e4, TagB);
    ecs_add(world, e5, TagC);

    ecs_iter_t it = ecs_rule_iter(world, q);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 2);
    test_int(it.frame_offset, 0);
    test_assert(it.entities != NULL);
    test_assert(it.entities[0] == e1);
    test_assert(it.entities[1] == e2);
    test_assert(it.ids[0] == TagA);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 2);
    test_int(it.frame_offset, 2);
    test_assert(it.entities != NULL);
    test_assert(it.entities[0] == e3);
    test_assert(it.entities[1] == e4);
    test_assert(it.ids[0] == TagA);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.frame_offset, 4);
    test_assert(it.entities != NULL);
    test_assert(it.entities[0] == e5);
    test_assert(it.ids[0] == TagA);

    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(q);

    ecs_fini(world);
}

void Rules_rule_iter_set_var() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);
    ECS_TAG(world, ObjC);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, ObjA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, ObjB);
    ecs_entity_t e3 = ecs_new_w_pair(world, Rel, ObjC);

    ecs_rule_t *r = ecs_rule_new(world, "(Rel, $X)");
    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, ObjA);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(it.ids[0], ecs_pair(Rel, ObjA));
    test_int(ObjA, ecs_iter_get_var(&it, x_var));
    test_bool(ecs_rule_next(&it), false);

    it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, ObjB);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e2);
    test_int(it.ids[0], ecs_pair(Rel, ObjB));
    test_int(ObjB, ecs_iter_get_var(&it, x_var));
    test_bool(ecs_rule_next(&it), false);

    it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, ObjC);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    test_int(it.ids[0], ecs_pair(Rel, ObjC));
    test_int(ObjC, ecs_iter_get_var(&it, x_var));
    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_rule_iter_set_2_vars() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelX);
    ECS_TAG(world, RelY);

    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);
    ECS_TAG(world, ObjC);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelX, ObjA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelX, ObjB);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelX, ObjC);

    ecs_add_pair(world, e1, RelY, ObjB);
    ecs_add_pair(world, e2, RelY, ObjB);
    ecs_add_pair(world, e3, RelY, ObjA);

    ecs_rule_t *r = ecs_rule_new(world, "(RelX, $X), (RelY, $Y)");
    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    int32_t y_var = ecs_rule_find_var(r, "Y");
    test_assert(y_var != -1);

    /* X = *, Y = ObjB */
    ecs_iter_t it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, EcsWildcard);
    ecs_iter_set_var(&it, y_var, ObjB);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(it.ids[0], ecs_pair(RelX, ObjA));
    test_int(it.ids[1], ecs_pair(RelY, ObjB));
    test_int(ObjA, ecs_iter_get_var(&it, x_var));
    test_int(ObjB, ecs_iter_get_var(&it, y_var));

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e2);
    test_int(it.ids[0], ecs_pair(RelX, ObjB));
    test_int(it.ids[1], ecs_pair(RelY, ObjB));
    test_int(ObjB, ecs_iter_get_var(&it, x_var));
    test_int(ObjB, ecs_iter_get_var(&it, y_var));
    test_bool(ecs_rule_next(&it), false);

    /* X = ObjA, Y = ObjB */
    it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, ObjA);
    ecs_iter_set_var(&it, y_var, ObjB);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(it.ids[0], ecs_pair(RelX, ObjA));
    test_int(it.ids[1], ecs_pair(RelY, ObjB));
    test_int(ObjA, ecs_iter_get_var(&it, x_var));
    test_int(ObjB, ecs_iter_get_var(&it, y_var));
    test_bool(ecs_rule_next(&it), false);

    /* X = ObjC, Y = * */
    it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, ObjC);
    ecs_iter_set_var(&it, y_var, EcsWildcard);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    test_int(it.ids[0], ecs_pair(RelX, ObjC));
    test_int(it.ids[1], ecs_pair(RelY, ObjA));
    test_int(ObjC, ecs_iter_get_var(&it, x_var));
    test_int(ObjA, ecs_iter_get_var(&it, y_var));
    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_rule_iter_set_pred_var() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_entity_t e1 = ecs_new(world, TagA);
    ecs_entity_t e2 = ecs_new(world, TagB);
    ecs_entity_t e3 = ecs_new(world, TagC);

    ecs_rule_t *r = ecs_rule_new(world, "$X");
    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, TagA);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(it.ids[0], TagA);
    test_int(TagA, ecs_iter_get_var(&it, x_var));
    test_bool(ecs_rule_next(&it), false);

    it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, TagB);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e2);
    test_int(it.ids[0], TagB);
    test_int(TagB, ecs_iter_get_var(&it, x_var));
    test_bool(ecs_rule_next(&it), false);

    it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, TagC);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    test_int(it.ids[0], TagC);
    test_int(TagC, ecs_iter_get_var(&it, x_var));
    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_rule_iter_set_var_for_2_terms() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelX);
    ECS_TAG(world, RelY);

    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);
    ECS_TAG(world, ObjC);

    ecs_entity_t e1 = ecs_new_w_pair(world, RelX, ObjA);
    ecs_entity_t e2 = ecs_new_w_pair(world, RelX, ObjB);
    ecs_entity_t e3 = ecs_new_w_pair(world, RelX, ObjC);
    ecs_entity_t e4 = ecs_new_w_pair(world, RelX, ObjC);

    ecs_add_pair(world, e1, RelY, ObjA);
    ecs_add_pair(world, e2, RelY, ObjB);
    ecs_add_pair(world, e3, RelY, ObjC);
    ecs_add_pair(world, e4, RelY, ObjB);

    ecs_rule_t *r = ecs_rule_new(world, "(RelX, $X), (RelY, $X)");
    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, ObjA);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(it.ids[0], ecs_pair(RelX, ObjA));
    test_int(it.ids[1], ecs_pair(RelY, ObjA));
    test_int(ObjA, ecs_iter_get_var(&it, x_var));
    test_bool(ecs_rule_next(&it), false);

    it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, ObjB);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e2);
    test_int(it.ids[0], ecs_pair(RelX, ObjB));
    test_int(it.ids[1], ecs_pair(RelY, ObjB));
    test_int(ObjB, ecs_iter_get_var(&it, x_var));
    test_bool(ecs_rule_next(&it), false);
    
    it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, ObjC);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    test_int(it.ids[0], ecs_pair(RelX, ObjC));
    test_int(it.ids[1], ecs_pair(RelY, ObjC));
    test_int(ObjC, ecs_iter_get_var(&it, x_var));
    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_rule_iter_set_cyclic_variable() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);

    ecs_entity_t bob = ecs_new_id(world);
    ecs_entity_t alice = ecs_new_id(world);
    ecs_entity_t jane = ecs_new_id(world);
    ecs_entity_t john = ecs_new_id(world);

    ecs_add_pair(world, bob, Likes, alice);
    ecs_add_pair(world, bob, Likes, john);
    ecs_add_pair(world, alice, Likes, bob);
    ecs_add_pair(world, jane, Likes, john);
    ecs_add_pair(world, john, Likes, jane);

    ecs_rule_t *r = ecs_rule_new(world, "Likes($X, $Y), Likes($Y, $X)");
    int x_var = ecs_rule_find_var(r, "X");
    int y_var = ecs_rule_find_var(r, "Y");

    ecs_iter_t it = ecs_rule_iter(world, r);
    
    // First verify that all entities match when no variables are set
    test_bool( ecs_rule_next(&it), true );
    test_int(ecs_iter_get_var(&it, x_var), alice);
    test_int(ecs_iter_get_var(&it, y_var), bob);

    test_bool( ecs_rule_next(&it), true );
    test_int(ecs_iter_get_var(&it, x_var), bob);
    test_int(ecs_iter_get_var(&it, y_var), alice);

    test_bool( ecs_rule_next(&it), true );
    test_int(ecs_iter_get_var(&it, x_var), john);
    test_int(ecs_iter_get_var(&it, y_var), jane);

    test_bool( ecs_rule_next(&it), true );
    test_int(ecs_iter_get_var(&it, x_var), jane);
    test_int(ecs_iter_get_var(&it, y_var), john);
    test_bool( ecs_rule_next(&it), false );

    // Iterate again with X = bob
    it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, bob);
    test_bool( ecs_rule_next(&it), true );
    test_int(ecs_iter_get_var(&it, x_var), bob);
    test_int(ecs_iter_get_var(&it, y_var), alice);
    test_bool( ecs_rule_next(&it), false );

    // Iterate again with Y = alice
    it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, alice);
    test_bool( ecs_rule_next(&it), true );
    test_int(ecs_iter_get_var(&it, x_var), alice);
    test_int(ecs_iter_get_var(&it, y_var), bob);
    test_bool( ecs_rule_next(&it), false );

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_rule_iter_set_cyclic_variable_w_this() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);

    ecs_entity_t bob = ecs_new_id(world);
    ecs_entity_t alice = ecs_new_id(world);
    ecs_entity_t jane = ecs_new_id(world);
    ecs_entity_t john = ecs_new_id(world);

    ecs_set_name(world, bob, "bob");
    ecs_set_name(world, alice, "alice");

    ecs_add_pair(world, bob, Likes, alice);
    ecs_add_pair(world, bob, Likes, john);
    ecs_add_pair(world, alice, Likes, bob);
    ecs_add_pair(world, jane, Likes, john);
    ecs_add_pair(world, john, Likes, jane);

    ecs_rule_t *r = ecs_rule_new(world, "(Likes, $X), Likes($X, $This)");
    int x_var = ecs_rule_find_var(r, "X");

    ecs_iter_t it = ecs_rule_iter(world, r);
    
    // First verify that all entities match when no variables are set
    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 1);
    test_int(it.entities[0], alice);
    test_int(ecs_iter_get_var(&it, x_var), bob);

    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 1);
    test_int(it.entities[0], bob);
    test_int(ecs_iter_get_var(&it, x_var), alice);

    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 1);
    test_int(it.entities[0], john);
    test_int(ecs_iter_get_var(&it, x_var), jane);

    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 1);
    test_int(it.entities[0], jane);
    test_int(ecs_iter_get_var(&it, x_var), john);
    test_bool( ecs_rule_next(&it), false );

    // Iterate again with X = bob
    it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, bob);

    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 1);
    test_int(it.entities[0], alice);
    test_int(ecs_iter_get_var(&it, x_var), bob);
    test_bool( ecs_rule_next(&it), false );

    // Iterate again with Y = alice
    it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, alice);

    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 1);
    test_int(it.entities[0], bob);
    test_int(ecs_iter_get_var(&it, x_var), alice);
    test_bool( ecs_rule_next(&it), false );

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_term_w_same_subj_obj_var() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, Final);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Rel($X, $X)"
    });

    int x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_add_pair(world, e1, Rel, e1);

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_add_pair(world, e2, Rel, e1);

    ecs_entity_t e3 = ecs_new_id(world);
    ecs_add_pair(world, e3, Rel, e1);

    ecs_entity_t e4 = ecs_new_id(world);
    ecs_add_pair(world, e4, Rel, e3);

    ecs_entity_t e5 = ecs_new_id(world);
    ecs_add_pair(world, e5, Rel, e5);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool( ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int( ecs_field_id(&it, 1), ecs_pair(Rel, e1));
    test_int( ecs_iter_get_var(&it, x_var), e1);

    test_bool( ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int( ecs_field_id(&it, 1), ecs_pair(Rel, e5));
    test_int( ecs_iter_get_var(&it, x_var), e5);

    test_bool( ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_optional_any_object() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, Final);

    ECS_ENTITY(world, Rel, Final);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);
    ECS_TAG(world, ObjC);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);

    ecs_add_pair(world, e2, Rel, ObjA);
    ecs_add_pair(world, e2, Rel, ObjB);
    ecs_add_pair(world, e3, Rel, ObjC);

    ecs_rule_t *r = ecs_rule_new(world, "Tag, ?(Rel, _)");
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(it.ids[0], Tag);
    test_bool(ecs_field_is_set(&it, 1), true);
    test_bool(ecs_field_is_set(&it, 2), false);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e2);
    test_int(it.ids[0], Tag);
    test_bool(ecs_field_is_set(&it, 1), true);
    test_bool(ecs_field_is_set(&it, 2), true);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    test_int(it.ids[0], Tag);
    test_bool(ecs_field_is_set(&it, 1), true);
    test_bool(ecs_field_is_set(&it, 2), true);
    
    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_optional_any_subject() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, Final);

    ECS_ENTITY(world, Rel, Final);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);
    ECS_TAG(world, ObjC);

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);

    ecs_add_pair(world, ObjA, Rel, e2);
    ecs_add_pair(world, ObjB, Rel, e2);
    ecs_add_pair(world, ObjC, Rel, e3);

    ecs_rule_t *r = ecs_rule_new(world, "Tag, ?Rel(_, $This)");
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(it.ids[0], Tag);
    test_bool(ecs_field_is_set(&it, 1), true);
    test_bool(ecs_field_is_set(&it, 2), false);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e2);
    test_int(it.ids[0], Tag);
    test_bool(ecs_field_is_set(&it, 1), true);
    test_bool(ecs_field_is_set(&it, 2), true);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    test_int(it.ids[0], Tag);
    test_bool(ecs_field_is_set(&it, 1), true);
    test_bool(ecs_field_is_set(&it, 2), true);
    
    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_test_subj_w_wildcard_w_pairs() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Rel);

    ecs_entity_t e = ecs_new_entity(world, "e");
    ecs_add(world, e, TagA);
    ecs_add(world, e, TagB);
    ecs_add_pair(world, e, Rel, TagA);
    ecs_add_pair(world, e, Rel, TagB);

    ecs_rule_t *r = ecs_rule_new(world, "*(e)");
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], e);
    test_int(ecs_field_id(&it, 1), TagA);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], e);
    test_int(ecs_field_id(&it, 1), TagB);

    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_test_subj_w_wildcard_wildcard_w_pairs() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Rel);

    ecs_entity_t e = ecs_new_entity(world, "e");
    ecs_add(world, e, TagA);
    ecs_add(world, e, TagB);
    ecs_add_pair(world, e, Rel, TagA);
    ecs_add_pair(world, e, Rel, TagB);

    ecs_rule_t *r = ecs_rule_new(world, "*(e, *)");
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], e);
    test_int(ecs_field_id(&it, 1), ecs_pair(ecs_id(EcsIdentifier), EcsName));

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], e);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, TagA));

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], e);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, TagB));

    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_test_this_w_wildcard_w_pairs() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, MyTag); // So we don't match everything
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Rel);

    ecs_entity_t e = ecs_new(world, MyTag);
    ecs_add(world, e, TagA);
    ecs_add(world, e, TagB);
    ecs_add_pair(world, e, Rel, TagA);
    ecs_add_pair(world, e, Rel, TagB);

    ecs_rule_t *r = ecs_rule_new(world, "*, MyTag");
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(ecs_field_id(&it, 1), MyTag);
    test_int(ecs_field_id(&it, 2), MyTag);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(ecs_field_id(&it, 1), TagA);
    test_int(ecs_field_id(&it, 2), MyTag);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(ecs_field_id(&it, 1), TagB);
    test_int(ecs_field_id(&it, 2), MyTag);

    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_test_this_w_wildcard_wildcard_w_pairs() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, MyTag); // So we don't match everything
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Rel);

    ecs_entity_t e = ecs_new(world, MyTag);
    ecs_add(world, e, TagA);
    ecs_add(world, e, TagB);
    ecs_add_pair(world, e, Rel, TagA);
    ecs_add_pair(world, e, Rel, TagB);

    ecs_rule_t *r = ecs_rule_new(world, "(*, *), MyTag");
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, TagA));
    test_int(ecs_field_id(&it, 2), MyTag);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, TagB));
    test_int(ecs_field_id(&it, 2), MyTag);

    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_test_subj_w_wildcard_w_pairs_var() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t e = ecs_new_entity(world, "e");
    ecs_add(world, e, TagA);
    ecs_add(world, e, TagB);
    ecs_add_pair(world, e, RelA, TagA);
    ecs_add_pair(world, e, RelA, TagB);
    ecs_add_pair(world, e, RelB, TagA);
    ecs_add_pair(world, e, RelB, TagB);

    ecs_rule_t *r = ecs_rule_new(world, "$C(e)");
    test_assert(r != NULL);

    int c_var = ecs_rule_find_var(r, "C");
    test_assert(c_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], e);
    test_int(ecs_field_id(&it, 1), TagA);
    test_int(ecs_iter_get_var(&it, c_var), TagA);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], e);
    test_int(ecs_field_id(&it, 1), TagB);
    test_int(ecs_iter_get_var(&it, c_var), TagB);

    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_test_subj_w_wildcard_wildcard_w_pairs_var() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Rel);

    ecs_entity_t e = ecs_new_entity(world, "e");
    ecs_add(world, e, TagA);
    ecs_add(world, e, TagB);
    ecs_add_pair(world, e, Rel, TagA);
    ecs_add_pair(world, e, Rel, TagB);

    ecs_rule_t *r = ecs_rule_new(world, "$R(e, $O)");
    test_assert(r != NULL);

    int r_var = ecs_rule_find_var(r, "R");
    test_assert(r_var != -1);
    int o_var = ecs_rule_find_var(r, "O");
    test_assert(o_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], e);
    test_int(ecs_field_id(&it, 1), ecs_pair(ecs_id(EcsIdentifier), EcsName));
    test_int(ecs_iter_get_var(&it, r_var), ecs_id(EcsIdentifier));
    test_int(ecs_iter_get_var(&it, o_var), EcsName);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], e);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, TagA));
    test_int(ecs_iter_get_var(&it, r_var), Rel);
    test_int(ecs_iter_get_var(&it, o_var), TagA);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], e);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, TagB));
    test_int(ecs_iter_get_var(&it, r_var), Rel);
    test_int(ecs_iter_get_var(&it, o_var), TagB);

    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_test_this_w_wildcard_w_pairs_var() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, MyTag); // So we don't match everything
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Rel);

    ecs_entity_t e = ecs_new(world, MyTag);
    ecs_add(world, e, TagA);
    ecs_add(world, e, TagB);
    ecs_add_pair(world, e, Rel, TagA);
    ecs_add_pair(world, e, Rel, TagB);

    ecs_rule_t *r = ecs_rule_new(world, "$C, MyTag");
    test_assert(r != NULL);

    int c_var = ecs_rule_find_var(r, "C");
    test_assert(c_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(ecs_field_id(&it, 1), MyTag);
    test_int(ecs_field_id(&it, 2), MyTag);
    test_int(ecs_iter_get_var(&it, c_var), MyTag);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(ecs_field_id(&it, 1), TagA);
    test_int(ecs_field_id(&it, 2), MyTag);
    test_int(ecs_iter_get_var(&it, c_var), TagA);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(ecs_field_id(&it, 1), TagB);
    test_int(ecs_field_id(&it, 2), MyTag);
    test_int(ecs_iter_get_var(&it, c_var), TagB);

    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_test_this_w_wildcard_wildcard_w_pairs_var() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, MyTag); // So we don't match everything
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Rel);

    ecs_entity_t e = ecs_new(world, MyTag);
    ecs_add(world, e, TagA);
    ecs_add(world, e, TagB);
    ecs_add_pair(world, e, Rel, TagA);
    ecs_add_pair(world, e, Rel, TagB);

    ecs_rule_t *r = ecs_rule_new(world, "($R, $O), MyTag");
    test_assert(r != NULL);

    int r_var = ecs_rule_find_var(r, "R");
    test_assert(r_var != -1);
    int o_var = ecs_rule_find_var(r, "O");
    test_assert(o_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, TagA));
    test_int(ecs_field_id(&it, 2), MyTag);
    test_int(ecs_iter_get_var(&it, r_var), Rel);
    test_int(ecs_iter_get_var(&it, o_var), TagA);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, TagB));
    test_int(ecs_field_id(&it, 2), MyTag);
    test_int(ecs_iter_get_var(&it, r_var), Rel);
    test_int(ecs_iter_get_var(&it, o_var), TagB);

    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_test_this_w_wildcard_no_match() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, MyTag); // So we don't match everything
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Rel);

    ecs_entity_t e = ecs_new_w_pair(world, Rel, MyTag);
    ecs_add_pair(world, e, Rel, TagA);
    ecs_add_pair(world, e, Rel, TagB);

    ecs_rule_t *r = ecs_rule_new(world, "*, (Rel, MyTag)");
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_test_this_w_pair_wildcard_no_match() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, MyTag); // So we don't match everything
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e = ecs_new(world, MyTag);
    ecs_add(world, e, TagA);
    ecs_add(world, e, TagB);

    ecs_rule_t *r = ecs_rule_new(world, "(*, *), MyTag");
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_test_subj_w_wildcard_w_isa() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);

    ecs_entity_t base = ecs_new_id(world);
    ecs_add(world, base, TagA);
    ecs_add(world, base, TagB);

    ecs_entity_t e = ecs_new_entity(world, "e");
    ecs_add_pair(world, e, EcsIsA, base);
    ecs_add(world, e, TagC);
    ecs_add(world, e, TagD);

    ecs_rule_t *r = ecs_rule_new(world, "*(e)");
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], e);
    test_int(ecs_field_id(&it, 1), TagC);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], e);
    test_int(ecs_field_id(&it, 1), TagD);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], base);
    test_int(ecs_field_id(&it, 1), TagA);
    
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], base);
    test_int(ecs_field_id(&it, 1), TagB);

    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_test_subj_w_wildcard_w_isa_2_lvls() {
    ecs_world_t *world = ecs_mini();
    
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);
    ECS_TAG(world, TagE);
    ECS_TAG(world, TagF);

    ecs_entity_t base_of_base = ecs_new_id(world);
    ecs_add(world, base_of_base, TagA);
    ecs_add(world, base_of_base, TagB);

    ecs_entity_t base = ecs_new_id(world);
    ecs_add_pair(world, base, EcsIsA, base_of_base);
    ecs_add(world, base, TagC);
    ecs_add(world, base, TagD);

    ecs_entity_t e = ecs_new_entity(world, "e");
    ecs_add_pair(world, e, EcsIsA, base);
    ecs_add(world, e, TagE);
    ecs_add(world, e, TagF);

    ecs_rule_t *r = ecs_rule_new(world, "*(e)");
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], e);
    test_int(ecs_field_id(&it, 1), TagE);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], e);
    test_int(ecs_field_id(&it, 1), TagF);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], base);
    test_int(ecs_field_id(&it, 1), TagC);
    
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], base);
    test_int(ecs_field_id(&it, 1), TagD);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], base_of_base);
    test_int(ecs_field_id(&it, 1), TagA);
    
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], base_of_base);
    test_int(ecs_field_id(&it, 1), TagB);

    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_test_subj_w_pair_wildcard_w_isa() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);

    ecs_entity_t base = ecs_new_id(world);
    ecs_add_pair(world, base, Rel, TagA);
    ecs_add_pair(world, base, Rel, TagB);

    ecs_entity_t e = ecs_new_entity(world, "e");
    ecs_add_pair(world, e, EcsIsA, base);
    ecs_add_pair(world, e, Rel, TagC);
    ecs_add_pair(world, e, Rel, TagD);

    ecs_rule_t *r = ecs_rule_new(world, "*(e, *)");
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], e);
    test_int(ecs_field_id(&it, 1), ecs_pair(ecs_id(EcsIdentifier), EcsName));

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], e);
    test_int(ecs_field_id(&it, 1), ecs_pair(EcsIsA, base));

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], e);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, TagC));

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], e);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, TagD));

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], base);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, TagA));
    
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], base);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, TagB));

    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_test_subj_w_pair_wildcard_w_isa_2_lvls() {
    ecs_world_t *world = ecs_mini();
    
    ECS_TAG(world, Rel);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);
    ECS_TAG(world, TagE);
    ECS_TAG(world, TagF);

    ecs_entity_t base_of_base = ecs_new_id(world);
    ecs_add_pair(world, base_of_base, Rel, TagA);
    ecs_add_pair(world, base_of_base, Rel, TagB);

    ecs_entity_t base = ecs_new_id(world);
    ecs_add_pair(world, base, EcsIsA, base_of_base);
    ecs_add_pair(world, base, Rel, TagC);
    ecs_add_pair(world, base, Rel, TagD);

    ecs_entity_t e = ecs_new_entity(world, "e");
    ecs_add_pair(world, e, EcsIsA, base);
    ecs_add_pair(world, e, Rel, TagE);
    ecs_add_pair(world, e, Rel, TagF);

    ecs_rule_t *r = ecs_rule_new(world, "*(e, *)");
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], e);
    test_int(ecs_field_id(&it, 1), ecs_pair(ecs_id(EcsIdentifier), EcsName));

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], e);
    test_int(ecs_field_id(&it, 1), ecs_pair(EcsIsA, base));

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], e);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, TagE));

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], e);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, TagF));

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], base);
    test_int(ecs_field_id(&it, 1), ecs_pair(EcsIsA, base_of_base));

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], base);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, TagC));
    
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], base);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, TagD));

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], base_of_base);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, TagA));
    
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 0);
    test_int(it.sources[0], base_of_base);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, TagB));

    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_test_this_w_wildcard_w_isa() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, MyTag, Final);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);

    ecs_entity_t base = ecs_new_id(world);
    ecs_add(world, base, TagA);
    ecs_add(world, base, TagB);

    ecs_entity_t e = ecs_new(world, MyTag);
    ecs_add_pair(world, e, EcsIsA, base);
    ecs_add(world, e, TagC);
    ecs_add(world, e, TagD);

    ecs_rule_t *r = ecs_rule_new(world, "*, MyTag");
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(it.sources[0], 0);
    test_int(ecs_field_id(&it, 1), MyTag);
    test_int(ecs_field_id(&it, 2), MyTag);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(it.sources[0], 0);
    test_int(ecs_field_id(&it, 1), TagC);
    test_int(ecs_field_id(&it, 2), MyTag);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(it.sources[0], 0);
    test_int(ecs_field_id(&it, 1), TagD);
    test_int(ecs_field_id(&it, 2), MyTag);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(it.sources[0], 0);
    test_int(ecs_field_id(&it, 1), TagA);
    test_int(ecs_field_id(&it, 2), MyTag);
    
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(it.sources[0], 0);
    test_int(ecs_field_id(&it, 1), TagB);
    test_int(ecs_field_id(&it, 2), MyTag);

    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_test_this_w_wildcard_w_isa_2_lvls() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, MyTag, Final);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);

    ecs_entity_t base_of_base = ecs_new_id(world);
    ecs_add(world, base_of_base, TagB);

    ecs_entity_t base = ecs_new_w_pair(world, EcsIsA, base_of_base);
    ecs_add(world, base, TagA);

    ecs_entity_t e = ecs_new(world, MyTag);
    ecs_add_pair(world, e, EcsIsA, base);
    ecs_add(world, e, TagC);
    ecs_add(world, e, TagD);

    ecs_rule_t *r = ecs_rule_new(world, "*, MyTag");
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(it.sources[0], 0);
    test_int(ecs_field_id(&it, 1), MyTag);
    test_int(ecs_field_id(&it, 2), MyTag);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(it.sources[0], 0);
    test_int(ecs_field_id(&it, 1), TagC);
    test_int(ecs_field_id(&it, 2), MyTag);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(it.sources[0], 0);
    test_int(ecs_field_id(&it, 1), TagD);
    test_int(ecs_field_id(&it, 2), MyTag);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(it.sources[0], 0);
    test_int(ecs_field_id(&it, 1), TagA);
    test_int(ecs_field_id(&it, 2), MyTag);
    
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(it.sources[0], 0);
    test_int(ecs_field_id(&it, 1), TagB);
    test_int(ecs_field_id(&it, 2), MyTag);

    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_test_this_w_wildcard_w_2_isa() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, MyTag, Final);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);

    ecs_entity_t base_1 = ecs_new_id(world);
    ecs_add(world, base_1, TagA);
    ecs_entity_t base_2 = ecs_new_id(world);
    ecs_add(world, base_2, TagB);

    ecs_entity_t e = ecs_new(world, MyTag);
    ecs_add_pair(world, e, EcsIsA, base_1);
    ecs_add_pair(world, e, EcsIsA, base_2);
    ecs_add(world, e, TagC);
    ecs_add(world, e, TagD);

    ecs_rule_t *r = ecs_rule_new(world, "*, MyTag");
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(it.sources[0], 0);
    test_int(ecs_field_id(&it, 1), MyTag);
    test_int(ecs_field_id(&it, 2), MyTag);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(it.sources[0], 0);
    test_int(ecs_field_id(&it, 1), TagC);
    test_int(ecs_field_id(&it, 2), MyTag);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(it.sources[0], 0);
    test_int(ecs_field_id(&it, 1), TagD);
    test_int(ecs_field_id(&it, 2), MyTag);

    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(it.sources[0], 0);
    test_int(ecs_field_id(&it, 1), TagA);
    test_int(ecs_field_id(&it, 2), MyTag);
    
    test_bool(ecs_rule_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_int(it.sources[0], 0);
    test_int(ecs_field_id(&it, 1), TagB);
    test_int(ecs_field_id(&it, 2), MyTag);

    test_bool(ecs_rule_next(&it), false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_rule_w_inout_filter() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {30, 40});

    ecs_rule_t *r = ecs_rule_new(world, "[none] Position");
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 2);
    test_uint(ecs_id(Position), ecs_field_id(&it, 1));
    test_int(it.entities[0], e1);
    test_int(it.entities[1], e2);

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_variable_order() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelX);
    ECS_TAG(world, RelY);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_rule_t *r = ecs_rule_new(world, "RelX($X, $Y), RelX($Y, $Z)");
    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "X");
    int y_var = ecs_rule_find_var(r, "Y");
    int z_var = ecs_rule_find_var(r, "Z");

    test_assert(x_var != -1);
    test_assert(y_var != -1);
    test_assert(z_var != -1);

    test_assert(x_var < y_var);
    test_assert(y_var < z_var);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_table_subj_as_obj_in_not() {
  ecs_world_t *world = ecs_mini();

  ECS_ENTITY(world, Trait, Final);
  ECS_ENTITY(world, Subj, Final);

  ecs_rule_t *r = ecs_rule_new(world, "Trait, !Trait(Subj, $This)");
  test_assert(r != NULL);

  ecs_entity_t e1 = ecs_new(world, Trait);
  ecs_entity_t e2 = ecs_new(world, Trait);
  ecs_entity_t e3 = ecs_new(world, Trait);

  ecs_add_pair(world, Subj, Trait, e2);

  ecs_iter_t it = ecs_rule_iter(world, r);
  test_bool(true, ecs_rule_next(&it));
  test_int(it.count, 1);
  test_uint(it.entities[0], e1);

  test_bool(true, ecs_rule_next(&it));
  test_int(it.count, 1);
  test_uint(it.entities[0], e3);

  test_bool(false, ecs_rule_next(&it));

  ecs_rule_fini(r);

  ecs_fini(world);
}

void Rules_invalid_variable_only() {
    ecs_log_set_level(-4);
    
    ecs_world_t *world = ecs_mini();

    ecs_rule_t *r = ecs_rule_new(world, "$");
    test_assert(r == NULL);

    ecs_fini(world);
}

void Rules_page_iter() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagX);
    ECS_TAG(world, TagY);
    ECS_TAG(world, TagZ);

    /* ecs_entity_t e1 = */ ecs_new(world, TagX);
    ecs_entity_t e2 = ecs_new(world, TagX);
    ecs_entity_t e3 = ecs_new(world, TagX);
    ecs_entity_t e4 = ecs_new(world, TagX);
    ecs_entity_t e5 = ecs_new(world, TagX);
    
    ecs_add(world, e3, TagY);
    ecs_add(world, e4, TagY);
    ecs_add(world, e5, TagZ);

    ecs_rule_t *r = ecs_rule_new(world, "TagX");
    test_assert(r != NULL);

    ecs_iter_t rit = ecs_rule_iter(world, r);
    ecs_iter_t it = ecs_page_iter(&rit, 1, 2);

    test_bool(true, ecs_page_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);

    test_bool(true, ecs_page_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_bool(false, ecs_page_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_rule_w_short_notation() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_rule_t *r = ecs_rule(world, {
        .expr = "Tag"
    });

    test_assert(r != NULL);

    ecs_entity_t e = ecs_new(world, Tag);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(true, ecs_rule_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_uint(Tag, it.ids[0]);
    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}
